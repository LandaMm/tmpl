#include "cpl-parser/node/directive.hpp"

#include <filesystem>
#include <memory>
#include "cpl-parser/parser.h"

namespace AST
{
    using namespace AST::Nodes;

    Node* Parser::ImportStatement()
    {
		auto loc = Current()->GetLocation();
		Eat(TokenType::Import);

		auto token = Current();
		Eat(TokenType::String);
		auto module = token->GetValue<String>();
		assert(module);

		std::filesystem::path modulePath = GetFilename().c_str();
		modulePath = modulePath.parent_path() / module->c_str();

		auto importDirective = m_arena.Alloc<ImportDirective>(*module, loc);

		try {
			FileReader reader(modulePath.string().c_str());

			if (!reader.Ok())
			{
				throw std::runtime_error("file failed to open");
			}

			Lexer lexer(reader, module->c_str(), m_lexer->Allocator());
			lexer.Tokenize();

			m_lexer->MergeLexer(lexer);
		} catch (...) {
			GetErrorManager().FailedOpeningFile(module->c_str());
			return importDirective;
		}

		return importDirective;
    }

    Node* Parser::ExternStatement()
    {
        auto loc = Current()->GetLocation();
        Eat(TokenType::Extern);

        // TODO: maybe add support for static variables from c
        auto fnSign = FunctionSignature();
        Eat(TokenType::Semicolon);

        return m_arena.Alloc<ExternDirective>(fnSign, loc);
    }
}

