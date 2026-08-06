
#include "cpl-parser/parser.h"
#include "cpl-parser/error.h"
#include "cpl-parser/token.h"
#include <memory>

namespace AST
{
	Prelude::ErrorManager &Parser::GetErrorManager()
	{
		Prelude::ErrorManager &errorManager = Prelude::ErrorManager::getInstance();
		return errorManager;
	}

	Parser::Parser(Lexer* lexer)
		: m_root(nullptr)
	{
		m_root = m_arena.Alloc<Nodes::ProgramNode>();
		PushLexer(lexer);
	}

	Parser::~Parser()
	{
		for (auto lexer : m_deadLexers)
		{
			delete lexer;
		}
	}

	void Parser::PushLexer(Lexer* lexer)
	{
		if (m_lexers.Empty()) m_lexers.Push(lexer);
		else m_lexers.InsertAt(0, { lexer });
	}

	[[nodiscard]] Lexer* Parser::CurrentLexer()
	{
		assert(!m_lexers.Empty());
		return m_lexers[0];
	}

	Token* Parser::Current()
	{
		auto lexer = CurrentLexer();
		if (lexer->GetToken()->Is(TokenType::_EOF))
		{
			m_lexers.Shift();
			m_deadLexers.Push(lexer);
		}
		return lexer->GetToken();
	}

	Token* Parser::Peek()
	{
		return CurrentLexer()->SeekToken();
	}

	void Parser::Advance()
	{
		Eat(Current()->GetType());
	}

	void Parser::Eat(TokenType type)
	{
		if (Current()->Is(type))
		{
			CurrentLexer()->NextToken();
		}
		else
		{
			if (Current()->Is(TokenType::_EOF))
			{
				auto token = Current();
				GetErrorManager().UnexpectedEofWhileToken(GetFilename(), type, token->GetLine(), token->GetColumn());
			}
			else
			{
				GetErrorManager().UnexpectedToken(GetFilename(), Current(), Current(), type);
			}
		}
	}

	void Parser::Parse()
	{
		while (true)
		{
			if (Current()->Not(TokenType::_EOF))
				m_root->AddStatement(Statement());
			else if (!m_lexers.Empty())
				continue;
			else break;
		}
	}
	
	Node* Parser::CompileTimeStatement()
	{
		CurrentLexer()->SaveState();
		
		Eat(TokenType::Id);

		auto next = Peek();

		CurrentLexer()->RestoreState();

		Node* stmt = nullptr;

		switch (next->GetType())
		{
		case TokenType::OpenBracket:
		{
			auto fn = FunctionDeclaration();
			stmt = fn;
			if (fn->GetSymbolFlag() == Nodes::SymbolFlag::FOREIGN) Eat(TokenType::Semicolon);
			break;
		}
		default: // Type Declaration
		{
			auto typ = TypeDeclaration();
			stmt = typ;
			if ((typ->GetTypeValue()->GetKind() != Nodes::TypeKind::ENUM
				&& typ->GetTypeValue()->GetKind() != Nodes::TypeKind::STRUCT)
				|| Current()->GetType() == TokenType::Semicolon) Eat(TokenType::Semicolon);
			break;
		}
		}

		assert(stmt != nullptr && "SHOULD BE UNREACHABLE");
		return stmt;
	}

	Node* Parser::Statement()
	{
		auto token = Current();
		Node* stmt = nullptr;
		switch (token->GetType())
		{
		case TokenType::If:
			stmt = IfElseStatement();
			break;
        case TokenType::Return:
            stmt = ReturnStatement();
            Eat(TokenType::Semicolon);
            break;
        case TokenType::While:
            stmt = WhileLoop();
            break;
        case TokenType::For:
            stmt = ForLoop();
            break;
        case TokenType::Break:
        {
            stmt = BreakStmt();
            Eat(TokenType::Semicolon);
            if (m_breaks.empty())
            {
                // FIXME:
                // Prelude::ErrorManager& manager = GetErrorManager();
                // manager.BreakNotAllowed(GetFilename(), std::dynamic_pointer_cast<Nodes::BreakNode>(stmt), "ParseError");
                return nullptr;
            }
            break;
        }
        case TokenType::Hash:
        {
            Eat(TokenType::Hash);
            // Directive
            switch (Current()->GetType())
            {
                case TokenType::Import:
                    stmt = ImportStatement();
                    break;
                case TokenType::Extern:
                    stmt = ExternStatement();
                    break;
                default:
					// TODO: better error
                    Prelude::ErrorManager& manager = GetErrorManager();
                    manager.UnexpectedToken(GetFilename(), Peek());
                    return nullptr;
            }
            break;
        }
		case TokenType::Id:
		{
			auto next = Peek();
			
			if (next->GetType() == TokenType::DoubleColon)
			{
				stmt = CompileTimeStatement();
			}
			else if (next->GetType() == TokenType::Colon) // Variable Declaration
			{
				stmt = VariableDeclaration();
				Eat(TokenType::Semicolon);
			}
			else {
				stmt = Assignment();
				Eat(TokenType::Semicolon);
			}

			break;
		}
		default:
			auto& err = GetErrorManager();
			err.UnexpectedToken(GetFilename(), token);
			break;
		}
		return stmt;
	}
}
