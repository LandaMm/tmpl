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
		if (auto localVariable = dynamic_cast<const Symbols::LocalVariable*>(symbol))
		{
			Write(symbol->Name() + ": ");
			
			auto value = localVariable->InitialValue();
			auto sizedTyp = dynamic_cast<const SizedType*>(localVariable->Typ());
			
			assert(sizedTyp && "only sized types can be locally defined");

			switch (value->Kind())
			{
			case ValueKind::IMMEDIATE:
			{
				auto imm = value->As<ImmediateValue>();
				switch (sizedTyp->Layout().size)
				{
				case TypeSize::BITS8:
					Write(std::format("db {}", std::to_string(*reinterpret_cast<const char*>(imm->ImmValue()))));
					break;
				case TypeSize::BITS16:
					Write(std::format("dw {}", std::to_string(*reinterpret_cast<const short*>(imm->ImmValue()))));
					break;
				case TypeSize::BITS32:
					Write(std::format("dd {}", std::to_string(*reinterpret_cast<const int*>(imm->ImmValue()))));
					break;
				case TypeSize::BITS64:
					Write(std::format("dq {}", std::to_string(*reinterpret_cast<const long*>(imm->ImmValue()))));
					break;
				case TypeSize::UNKNOWN:
				default:
					// TODO: better error
					assert(false && "unsupported data type size");
					break;
				}
			}
			break;
			case ValueKind::UNKNOWN:
			default:
				// TODO: better error
				assert(false && "unsupported value kind for data definition");
			}

			WriteLn("");
		}
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

