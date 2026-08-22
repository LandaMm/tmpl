#pragma once

#include <cpl-basics/file.hpp>
#include <cpl-ir/ir.h>

#include <cpl-ir/type.hpp>

#include "cpl-codegen/register_allocator.h"

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

	void GenerateData();
	void GenerateBasicBlock(const IRGenerate::BasicBlock* block);
	void GenerateInstr(const IRGenerate::Instr* instr);
private:
	const IRGenerate::IR* m_ir;
	FileStreamWriter* m_output;

	BasicSlotAllocator* m_allocator;
};

} // namespace Codegen

