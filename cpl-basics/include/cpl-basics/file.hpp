#pragma once

#include <fstream>
#include <sstream>

#include "string.hpp"
#include "array.hpp"

class FileReader
{
public:
	FileReader(const char* path) : m_file(path, std::ios::binary)
	{
		if (!m_file.is_open())
		{
			throw new std::runtime_error((String("failed to open file at ") + path).c_str());
		}
	}

	~FileReader()
	{
		m_file.close();
	}

	FileReader(const FileReader&) = delete;
	FileReader& operator=(const FileReader&) = delete;
	FileReader(FileReader&&) = delete;
	FileReader& operator=(FileReader&&) = delete;
public:
	inline bool Ok() const { return m_file.good(); }

	[[nodiscard]] String ReadEntireText() const
	{
		std::stringstream ss;
		ss << m_file.rdbuf();

		return ss.str();
	}

	[[nodiscard]] Array<unsigned char> ReadAllBytes()
	{
		m_file.seekg(0, std::ios::end);
		std::streamsize size = m_file.tellg();

		Array<unsigned char> buffer(size);
		buffer.ResizeUninitialized(size);

		m_file.seekg(0);
		m_file.read(reinterpret_cast<char*>(buffer.Data()), size);

		return buffer;
	}
private:
	std::ifstream m_file;
};

class FileStreamWriter
{
public:
	FileStreamWriter(const char* path) : m_file(path, std::ios::binary)
	{
		if (!m_file.is_open())
		{
			throw new std::runtime_error((String("failed to open file at ") + path).c_str());
		}
	}

	~FileStreamWriter()
	{
		m_file.close();
	}

	FileStreamWriter(const FileStreamWriter&) = delete;
	FileStreamWriter& operator=(const FileStreamWriter&) = delete;
	FileStreamWriter(FileStreamWriter&&) = delete;
	FileStreamWriter& operator=(FileStreamWriter&&) = delete;
public:
	inline bool Ok() const { return m_file.good(); }

	void Write(const String& what)
	{
		m_file << what;
	}
private:
	std::ofstream m_file;
};

