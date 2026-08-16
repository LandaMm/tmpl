#include <format>

#include "cpl-codegen/codegen.h"
#include "cpl-codegen/dumper.h"

namespace Codegen
{

Generator::Generator(FileStreamWriter* output, const IRGenerate::IR* ir)
		: m_ir(ir), m_output(output) { }

void Generator::Generate()
{
    using namespace IRGenerate;

	WriteLn("format ELF64");
    
	WriteLn("; Symbols:\n");
	for (auto& [_, symbol] : m_ir->Symbols())
	{
		if (symbol->Origin() != SymbolOrigin::FOREIGN)
		{
			WriteLn(std::format("public {}", symbol->Name().c_str()));
		}
		else
		{
			WriteLn(std::format("extrn {}", symbol->Name().c_str()));
		}
	}

	WriteLn("\nsection '.data' data readable writeable\n");
	for (auto& [_, symbol] : m_ir->Symbols())
	{
		if (symbol->Origin() == SymbolOrigin::FOREIGN) continue;
		if (symbol->Kind() != SymbolKind::VARIABLE) continue;
		WriteLn(std::format("{}: db ...", symbol->Name().c_str()));
	}
}

void Generator::Write(const String& what)
{
	m_output->Write(what);
}

void Generator::WriteLn(const String& what)
{
	m_output->Write(what + "\n");
}

} // namespace Codegen

