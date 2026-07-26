#pragma once

#include <string>
#include <cassert>
#include <cstring>
#include <functional>
#include <limits>
#include <ostream>
#include <string_view>

#include "def.hpp"
#include "array.hpp"

template<typename SizeT = Uint32>
class BasicString
{
	static_assert(
		std::is_integral_v<SizeT> && std::is_unsigned_v<SizeT>,
		"BasicString SizeT must be an unsigned integral type"
	);

public:
	BasicString() = default;

	BasicString(const char* str)
		: m_data(std::strlen(str))
	{
		m_data.ResizeUninitialized(std::strlen(str));
		std::memcpy(m_data.Data(), str, m_data.Size());
	}

	BasicString(const std::string& str)
		: BasicString(str.c_str()) { }

	BasicString(const BasicString&) = default;
	BasicString& operator=(const BasicString&) = default;

	BasicString(BasicString&&) noexcept = default;
	BasicString& operator=(BasicString&&) noexcept = default;
public:
	SizeT Size() const { return m_data.Size(); }
	char* begin() noexcept { return m_data.begin(); }
	const char* begin() const noexcept { return m_data.begin(); }
	const char* cbegin() const noexcept { return m_data.cbegin(); }

	char* end() noexcept { return m_data.end(); }
	const char* end() const noexcept { return m_data.end(); }
	const char* cend() const noexcept { return m_data.cend(); }

	char* c_str() const
	{
		char* buffer = new char[Size() + 1];
		std::memcpy(buffer, m_data.Data(), Size());
		buffer[Size()] = '\0';
		return buffer;
	}
public:
	void Extend(const BasicString& other)
	{
		m_data.ResizeUninitialized(Size() + other.Size());
		std::memcpy(m_data.Data() + Size() - other.Size(), other.m_data.Data(), other.Size());
	}

	void Extend(const char* other)
	{
		auto len = std::strlen(other);
		m_data.ResizeUninitialized(Size() + len);
		std::memcpy(m_data.Data() + Size() - len, other, len);
	}

	void Extend(char other)
	{
		m_data.Push(other);
	}
	
	inline bool Empty() const noexcept
	{
		return m_data.Empty();
	}

	BasicString Substr(SizeT pos, SizeT count = npos) const
	{
		assert(pos < Size());
		SizeT length = std::min(Size() - pos, count);
		BasicString result;
		result.m_data.ResizeUninitialized(length);
		std::memcpy(result.m_data.Data(), m_data.Data() + pos, length);
		return result;
	}

	SizeT Find(char what) const noexcept
	{
		for (SizeT i = 0; i < Size(); ++i)
		{
			if (m_data.At(i) == what)
			{
				return i;
			}
		}
		return npos;
	}

	SizeT FindLastOf(char what) const noexcept
	{
		SizeT pos = npos;
		for (SizeT i = 0; i < Size(); ++i)
		{
			if (m_data.At(i) == what) pos = i;
		}
		return pos;
	}

	const char* Data() const noexcept
	{
		return m_data.Data();
	}

	std::string_view View() const noexcept
	{
		// Avoid constructing a string_view from a potentially null pointer.
		if (Size() == 0)
			return {};

		return std::string_view(
			Data(),
			static_cast<std::size_t>(Size())
		);
	}

	bool operator==(const BasicString& other) const noexcept
	{
		return View() == other.View();
	}

#if __cplusplus < 202002L
	bool operator!=(const BasicString& other) const noexcept
	{
		return !(*this == other);
	}
#endif

	BasicString operator+(const BasicString& other) const
	{
		BasicString str(*this);
		str.Extend(other);
		return str;
	}

	BasicString operator+(const char* other) const
	{
		BasicString str(*this);
		str.Extend(other);
		return str;
	}

	char operator[](SizeT index) noexcept
	{
		return char(m_data.At(index));
	}

	friend BasicString operator+(
		const char* lhs,
		const BasicString& rhs
		)
	{
		BasicString result(lhs);
		result.Extend(rhs);
		return result;
	}

	friend std::ostream& operator<<(
		std::ostream& stream,
		const BasicString& str
		)
	{
		if (str.Size() > 0)
		{
			stream.write(
				str.m_data.Data(),
				static_cast<std::streamsize>(str.Size())
			);
		}

		return stream;
	}
private:
	Array<char, SizeT> m_data;
	static const SizeT npos = -1;
};

namespace std
{
	template<typename SizeT>
	struct hash<::BasicString<SizeT>>
	{
		std::size_t operator()(
			const ::BasicString<SizeT>& value
			) const noexcept
		{
			return std::hash<std::string_view>{}(value.View());
		}
	};
}

using String = BasicString<>;
