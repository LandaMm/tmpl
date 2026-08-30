#pragma once

#include <cpl-ir/instr/call.hpp>
#include <cpl-ir/instr/alloca.hpp>
#include <cpl-ir/instr/store.hpp>
#include <cpl-ir/instr/load.hpp>
#include <cpl-ir/instr/ptr.hpp>
#include <cpl-ir/ir.h>

void dump_type_name(const IRGenerate::Type* typ, FileStreamWriter* stream);

void dump_type(const IRGenerate::Type* typ, FileStreamWriter* stream);

void dump_symbol_name(const IRGenerate::Symbol* symbol, FileStreamWriter* stream);

void dump_value(const IRGenerate::Value* value, FileStreamWriter* stream);

void dump_symbol(const IRGenerate::Symbol* symbol, FileStreamWriter* stream);

void dump_instr(const IRGenerate::Instr* instr, FileStreamWriter* stream);

void dump_block(const IRGenerate::BasicBlock* block, FileStreamWriter* stream);

void dump_ir(const IRGenerate::IR* ir, FileStreamWriter* stream);

