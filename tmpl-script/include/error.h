#ifndef ERROR_H
#define ERROR_H
#include <string>
#include <memory>
#include "location.h"
#include "token.h"
#include "interpreter/value.h"
#include "node/identifier.h"

using namespace AST;

namespace Prelude
{
	class ErrorManager final
	{
	private:
		ErrorManager(const ErrorManager &) = delete;
		ErrorManager &operator=(const ErrorManager &) = delete;

	private:
		ErrorManager();

	private:
		~ErrorManager();

	public:
		static ErrorManager &getInstance()
		{
			static ErrorManager instance;
			return instance;
		}

	public:
		void RaiseError(std::string errorMessage, std::string prefix);

	public:
		void NoInputFile();
    
    private:
        void LogFileLocation(std::string filename, Location loc, std::string prefix);
        void LogPrefix(std::string prefix);

	public: // Lexer (Tokenizer)
		void UnexpectedCharacter(std::string filename, char ch, size_t line, size_t col);
		void UnexpectedEOF(std::string filename, size_t line, size_t col);

	public: // Parser
		void UnexpectedEofWhileToken(std::string filename, TokenType tokenType, size_t line, size_t col);
		void UnexpectedToken(std::string filename, std::shared_ptr<Token> locToken);
		void UnexpectedToken(std::string filename, std::shared_ptr<Token> locToken, std::shared_ptr<Token> gotToken, TokenType expectedTokenType);
		void MissingConstantDefinition(std::string filename, std::shared_ptr<Token> token);

	public: // Interpreter
		void VarMismatchType(std::string filename, std::string name, Runtime::ValueType type, Runtime::ValueType expectedType, Location loc, std::string prefix);
        void VarAlreadyExists(std::string filename, std::string name, Location loc, std::string prefix);
		void UndefinedType(std::string filename, std::string name, Location loc, std::string prefix);
		void UndeclaredVariable(std::string filename, std::shared_ptr<Nodes::IdentifierNode> id, std::string prefix);
		void OperandMismatchType(std::string filename, Runtime::ValueType leftType, Runtime::ValueType rightType, Location loc, std::string prefix);
		void UndeclaredFunction(std::string filename, std::shared_ptr<Nodes::IdentifierNode> id, std::string prefix);
        // TODO:
		// void UndeclaredFunction(std::shared_ptr<Nodes::ObjectMember> member);
		void ArgMismatchType(std::string filename, std::string name, Runtime::ValueType type, Runtime::ValueType expectedType, Location loc, std::string prefix);
		void ReturnMismatchType(std::string filename, std::string name, Runtime::ValueType type, Runtime::ValueType expectedType, Location loc);
        void ArgsParamsExhausted(std::string filename, std::string name, size_t argsSize, size_t paramsSize, Location loc, std::string prefix);
        void UnaryOperatorNotSupported(std::string filename, std::string op, Runtime::ValueType metType, Location loc);
    public: // TypeChecker
        void UnexpectedReturnType(std::string filename, Runtime::ValueType expected, Runtime::ValueType gotType, Location loc);
        void TypeMismatch(std::string filename, Runtime::ValueType left, Runtime::ValueType right, Location loc);
    public: // CliRunner
        void NotEnoughArgs(int expected, int got, bool atLeast);
        void InvalidArgument(std::string arg, std::string message);
        void FailedOpeningFile(std::string path);
        void ProcedureNotFound(std::string name);
	};
}

#endif
