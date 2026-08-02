#pragma once

#include <memory>

#include "cpl-basics/def.hpp"
#include "cpl-basics/array.hpp"

#define ARENA_DEFAULT_BLOCK_SIZE (64*1024)

template<typename SizeT = Uint32>
class ArenaAllocator
{
public:
	explicit ArenaAllocator(SizeT blockSize = ARENA_DEFAULT_BLOCK_SIZE)
	{
		AddBlock(blockSize);
	}

	ArenaAllocator(const ArenaAllocator&) = delete;
	ArenaAllocator& operator=(const ArenaAllocator&) = delete;

	~ArenaAllocator()
	{
		for (DestructorEntry* it = m_destructors.end() - 1; it != m_destructors.begin() - 1; it--)
		{
			it->destroy(it->object);
		}
	}

	template<typename T, typename... Args>
	requires std::constructible_from<T, Args...>
	T* Alloc(Args&&... args)
	{
		void* ptr = AllocateRaw(sizeof(T), alignof(T));
		T* object = std::construct_at<T>(static_cast<T*>(ptr), std::forward<Args>(args)...);

		if constexpr (!std::is_trivially_destructible_v<T>)
		{
			m_destructors.Push({
				object,
				[](void* ptr)
				{
					std::destroy_at<T>(static_cast<T*>(ptr));
				}
			});
		}

		return object;
	}

private:
	struct Block
	{
		std::unique_ptr<Byte[]> memory;
		SizeT capacity = 0;
		SizeT offset = 0;
	};

	struct DestructorEntry
	{
		void* object;
		void (*destroy)(void*);
	};

	void AddBlock(SizeT capacity)
	{
		m_blocks.Push({
			std::make_unique<Byte[]>(capacity),
			capacity,
			0
		});
	}

	void* AllocateRaw(SizeT size, SizeT alignment)
	{
		Block* block = &m_blocks[m_blocks.Size() - 1];

		void* current = block->memory.get() + block->offset;
		std::size_t space = block->capacity - block->offset;

		void* aligned = std::align(alignment, size, current, space);

		if (!aligned)
		{
			AddBlock(block->capacity);
			block = &m_blocks[m_blocks.Size() - 1];

			current = block->memory.get();
			space = block->capacity;

			aligned = std::align(alignment, size, current, space);
			assert(aligned != nullptr);
		}

		block->offset = (reinterpret_cast<Byte*>(aligned) - block->memory.get()) + size;
		return aligned;
	}

private:
	Array<Block> m_blocks;
	Array<DestructorEntry> m_destructors;
	SizeT m_capacity;
	SizeT m_offset = 0;
};
