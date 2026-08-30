#pragma once

#include <cassert>
#include <cstring>
#include <initializer_list>
#include <limits>
#include <memory>
#include <stdexcept>
#include <type_traits>
#include <utility>

#include "def.hpp"

#ifndef ARRAY_MINIMAL_CAPACITY
#define ARRAY_MINIMAL_CAPACITY 2
#endif

#ifndef ARRAY_DEFAULT_INITIAL_CAPACITY
#define ARRAY_DEFAULT_INITIAL_CAPACITY 256
#endif

template<typename Item, typename SizeT = Uint32>
class Array
{
	static_assert(
		std::is_integral_v<SizeT> && std::is_unsigned_v<SizeT>,
		"Array SizeT must be an unsigned integral type"
	);

	using Allocator = std::allocator<Item>;
	using AllocatorTraits = std::allocator_traits<Allocator>;

public:
	explicit Array(SizeT initialCapacity = ARRAY_DEFAULT_INITIAL_CAPACITY)
		: m_capacity(initialCapacity),
		  m_size(0),
		  m_data(Allocate(initialCapacity))
	{
	}

	Array(std::initializer_list<Item> items)
		: m_capacity(CheckedSize(items.size())),
		  m_size(0),
		  m_data(Allocate(m_capacity))
	{
		try
		{
			for (const Item& item : items)
			{
				std::construct_at(m_data + m_size, item);
				++m_size;
			}
		}
		catch (...)
		{
			DestroyItems(m_data, m_size);
			Deallocate(m_data, m_capacity);
			throw;
		}
	}

	~Array()
	{
		DestroyAndDeallocate();
	}

	Array(const Array& other)
		: m_capacity(other.m_capacity),
		  m_size(0),
		  m_data(Allocate(m_capacity))
	{
		try
		{
			CopyItems(m_data, other.m_data, other.m_size);
			m_size = other.m_size;
		}
		catch (...)
		{
			Deallocate(m_data, m_capacity);
			throw;
		}
	}

	Array& operator=(const Array& other)
	{
		if (this == &other)
			return *this;

		// Copy first. If copying throws, this array remains unchanged.
		Array copy(other);
		Swap(copy);

		return *this;
	}

	Array(Array&& other) noexcept
		: m_capacity(other.m_capacity),
		  m_size(other.m_size),
		  m_data(other.m_data)
	{
		other.m_capacity = 0;
		other.m_size = 0;
		other.m_data = nullptr;
	}

	Array& operator=(Array&& other) noexcept
	{
		if (this == &other)
			return *this;

		// Release anything currently owned by this array first.
		DestroyAndDeallocate();

		m_capacity = other.m_capacity;
		m_size = other.m_size;
		m_data = other.m_data;

		other.m_capacity = 0;
		other.m_size = 0;
		other.m_data = nullptr;

		return *this;
	}

public:
	void Push(const Item& item)
	{
		Emplace(item);
	}

	void Push(Item&& item)
	{
		Emplace(std::move(item));
	}

	template<typename... Args>
	Item& Emplace(Args&&... args)
	{
		CheckCanGrow();

		/*
		 * Construct a temporary before reallocation when the arguments
		 * could refer to an existing array item.
		 *
		 * This makes something such as Push(array[0]) safe.
		 */
		if (m_size == m_capacity && m_size != 0)
		{
			Item temporary(std::forward<Args>(args)...);

			EnsureCapacity(static_cast<SizeT>(m_size + 1));

			std::construct_at(
				m_data + m_size,
				std::move_if_noexcept(temporary)
			);
		}
		else
		{
			EnsureCapacity(static_cast<SizeT>(m_size + 1));

			std::construct_at(
				m_data + m_size,
				std::forward<Args>(args)...
			);
		}

		++m_size;
		return m_data[m_size - 1];
	}

	void InsertAt(SizeT index, const Array& other)
	{
		assert(index <= m_size);

		if (other.m_size == 0)
			return;

		constexpr SizeT maxSize =
			std::numeric_limits<SizeT>::max();

		if (other.m_size > maxSize - m_size)
		{
			throw std::length_error(
				"Array insertion exceeds maximum size"
			);
		}

		const SizeT oldSize = m_size;
		const SizeT insertedSize = other.m_size;
		const SizeT newSize =
			static_cast<SizeT>(oldSize + insertedSize);

		const SizeT newCapacity =
			CalculateCapacity(newSize);

		Item* newData = Allocate(newCapacity);
		SizeT constructed = 0;

		try
		{
			// Copy everything before the insertion point.
			if (index > 0)
			{
				CopyItems(
					newData,
					m_data,
					index
				);

				constructed =
					static_cast<SizeT>(constructed + index);
			}

			// Copy the inserted array.
			CopyItems(
				newData + constructed,
				other.m_data,
				insertedSize
			);

			constructed =
				static_cast<SizeT>(constructed + insertedSize);

			// Copy everything after the insertion point.
			const SizeT tailSize =
				static_cast<SizeT>(oldSize - index);

			if (tailSize > 0)
			{
				CopyItems(
					newData + constructed,
					m_data + index,
					tailSize
				);

				constructed =
					static_cast<SizeT>(constructed + tailSize);
			}
		}
		catch (...)
		{
			DestroyItems(newData, constructed);
			Deallocate(newData, newCapacity);
			throw;
		}

		DestroyItems(m_data, m_size);
		Deallocate(m_data, m_capacity);

		m_data = newData;
		m_size = newSize;
		m_capacity = newCapacity;
	}

	void Clear() noexcept
	{
		DestroyItems(m_data, m_size);
		m_size = 0;
	}

	void Shift()
	{
		assert(m_size > 0);

		static_assert(
			std::is_move_assignable_v<Item> ||
			std::is_copy_assignable_v<Item>,
			"Array::Shift requires an assignable Item type"
		);

		for (SizeT i = 1; i < m_size; ++i)
		{
			if constexpr (
				std::is_nothrow_move_assignable_v<Item> ||
				!std::is_copy_assignable_v<Item>
			)
			{
				m_data[i - 1] = std::move(m_data[i]);
			}
			else
			{
				m_data[i - 1] = m_data[i];
			}
		}

		std::destroy_at(m_data + (m_size - 1));
		--m_size;
	}

	void Reserve(SizeT requestedCapacity)
	{
		EnsureCapacity(requestedCapacity);
	}

	void ResizeUninitialized(SizeT newSize)
		requires (
			std::is_trivially_default_constructible_v<Item>&&
			std::is_trivially_destructible_v<Item>
		)
	{
		if (newSize < m_size)
		{
			m_size = newSize;
			return;
		}

		if (newSize == m_size)
			return;

		EnsureCapacity(newSize);

		/*
		 * Starts the lifetime of the new objects but does not initialize
		 * their values for trivial scalar types such as unsigned char.
		 */
		std::uninitialized_default_construct_n(
			m_data + m_size,
			static_cast<std::size_t>(newSize - m_size)
		);

		m_size = newSize;
	}

public:
	Item* Data() noexcept
	{
		return m_data;
	}

	const Item* Data() const noexcept
	{
		return m_data;
	}

	Item& At(SizeT index) noexcept
	{
		assert(index < m_size);
		return m_data[index];
	}

	const Item& At(SizeT index) const noexcept
	{
		assert(index < m_size);
		return m_data[index];
	}

	Item* begin() noexcept
	{
		return m_data;
	}

	const Item* begin() const noexcept
	{
		return m_data;
	}

	const Item* cbegin() const noexcept
	{
		return m_data;
	}

	Item* end() noexcept
	{
		return m_data == nullptr
			? nullptr
			: m_data + m_size;
	}

	const Item* end() const noexcept
	{
		return m_data == nullptr
			? nullptr
			: m_data + m_size;
	}

	const Item* cend() const noexcept
	{
		return m_data == nullptr
			? nullptr
			: m_data + m_size;
	}

	bool Empty() const noexcept
	{
		return m_size == 0;
	}

	Item& operator[](SizeT index) noexcept
	{
		return At(index);
	}

	const Item& operator[](SizeT index) const noexcept
	{
		return At(index);
	}

	SizeT Size() const noexcept
	{
		return m_size;
	}

	SizeT Capacity() const noexcept
	{
		return m_capacity;
	}

private:
	SizeT CalculateCapacity(SizeT expectedCapacity) const
	{
		if (expectedCapacity <= m_capacity)
			return m_capacity;

		constexpr SizeT maxCapacity =
			std::numeric_limits<SizeT>::max();

		SizeT newCapacity = m_capacity;

		if (newCapacity < static_cast<SizeT>(ARRAY_MINIMAL_CAPACITY))
		{
			newCapacity =
				static_cast<SizeT>(ARRAY_MINIMAL_CAPACITY);
		}
		else
		{
			SizeT growth =
				static_cast<SizeT>(newCapacity / 2);

			if (growth == 0)
				growth = 1;

			if (newCapacity > maxCapacity - growth)
			{
				newCapacity = expectedCapacity;
			}
			else
			{
				newCapacity =
					static_cast<SizeT>(
						newCapacity + growth
						);
			}
		}

		if (newCapacity < expectedCapacity)
			newCapacity = expectedCapacity;

		return newCapacity;
	}

	void EnsureCapacity(SizeT expectedCapacity)
	{
		const SizeT newCapacity =
			CalculateCapacity(expectedCapacity);

		if (newCapacity == m_capacity)
			return;

		Extend(newCapacity);
	}

	void Extend(SizeT newCapacity)
	{
		assert(newCapacity > m_capacity);

		/*
		 * Do not modify m_capacity or m_data until allocation and
		 * relocation have completed successfully.
		 */
		Item* newData = Allocate(newCapacity);

		try
		{
			RelocateItems(newData, m_data, m_size);
		}
		catch (...)
		{
			Deallocate(newData, newCapacity);
			throw;
		}

		Deallocate(m_data, m_capacity);

		m_data = newData;
		m_capacity = newCapacity;
	}

private:
	static SizeT CheckedSize(std::size_t size)
	{
		if (
			size >
			static_cast<std::size_t>(
				std::numeric_limits<SizeT>::max()
			)
		)
		{
			throw std::length_error(
				"Initializer list is too large for Array SizeT"
			);
		}

		return static_cast<SizeT>(size);
	}

	static Item* Allocate(SizeT capacity)
	{
		if (capacity == 0)
			return nullptr;

		Allocator allocator;

		return AllocatorTraits::allocate(
			allocator,
			static_cast<std::size_t>(capacity)
		);
	}

	static void Deallocate(
		Item* data,
		SizeT capacity
	) noexcept
	{
		if (data == nullptr)
			return;

		Allocator allocator;

		AllocatorTraits::deallocate(
			allocator,
			data,
			static_cast<std::size_t>(capacity)
		);
	}

	static void DestroyItems(
		Item* data,
		SizeT size
	) noexcept
	{
		for (SizeT i = 0; i < size; ++i)
		{
			std::destroy_at(data + i);
		}
	}

	static void RelocateItems(
		Item* destination,
		Item* source,
		SizeT size
	)
	{
		if (size == 0)
			return;

		static_assert(
			std::is_trivially_copyable_v<Item> ||
			std::is_nothrow_move_constructible_v<Item> ||
			std::is_copy_constructible_v<Item>,
			"Item must be trivially copyable, nothrow "
			"move-constructible, or copy-constructible"
		);

		if constexpr (std::is_trivially_copyable_v<Item>)
		{
			std::memcpy(
				destination,
				source,
				sizeof(Item) * static_cast<std::size_t>(size)
			);
		}
		else
		{
			SizeT constructed = 0;

			try
			{
				for (; constructed < size; ++constructed)
				{
					std::construct_at(
						destination + constructed,
						std::move_if_noexcept(
							source[constructed]
						)
					);
				}
			}
			catch (...)
			{
				DestroyItems(destination, constructed);
				throw;
			}

			/*
			 * Every source object must be destroyed, including
			 * objects that were moved from.
			 */
			DestroyItems(source, size);
		}
	}

	static void CopyItems(
		Item* destination,
		const Item* source,
		SizeT size
	)
	{
		if (size == 0)
			return;

		static_assert(
			std::is_trivially_copyable_v<Item> ||
			std::is_copy_constructible_v<Item>,
			"Item must be copy-constructible"
		);

		if constexpr (std::is_trivially_copyable_v<Item>)
		{
			std::memcpy(
				destination,
				source,
				sizeof(Item) * static_cast<std::size_t>(size)
			);
		}
		else
		{
			SizeT constructed = 0;

			try
			{
				for (; constructed < size; ++constructed)
				{
					std::construct_at(
						destination + constructed,
						source[constructed]
					);
				}
			}
			catch (...)
			{
				DestroyItems(destination, constructed);
				throw;
			}
		}
	}

	void DestroyAndDeallocate() noexcept
	{
		DestroyItems(m_data, m_size);
		Deallocate(m_data, m_capacity);

		m_capacity = 0;
		m_size = 0;
		m_data = nullptr;
	}

	void Swap(Array& other) noexcept
	{
		using std::swap;

		swap(m_capacity, other.m_capacity);
		swap(m_size, other.m_size);
		swap(m_data, other.m_data);
	}

	void CheckCanGrow() const
	{
		if (m_size == std::numeric_limits<SizeT>::max())
		{
			throw std::length_error(
				"Array cannot grow any further"
			);
		}
	}

private:
	/*
	 * Keep this declaration order consistent with constructor
	 * initializer lists. Members initialize in declaration order,
	 * not the order written in the constructor.
	 */
	SizeT m_capacity = 0;
	SizeT m_size = 0;
	Item* m_data = nullptr;
};
