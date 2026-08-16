#pragma once

#include <cpl-basics/file.hpp>
#include <cpl-ir/ir.h>

namespace Codegen
{

class Generator
{
public:
	Generator(FileStreamWriter* output, const IRGenerate::IR* ir);
public:
	void Generate();
private:
	void Write(const String& what);
	void WriteLn(const String& what);
private:
	const IRGenerate::IR* m_ir;
	FileStreamWriter* m_output;
};

} // namespace Codegen

