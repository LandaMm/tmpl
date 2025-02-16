#ifndef ERROR_H
#define ERROR_H
#include <string>
#include <memory>
#include "include/node/object_member.h"
#include "location.h"
#include "token.h"
#include "interpreter/value.h"
#include "node/identifier.h"

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
        void LogFileLocation(std::string filename, AST::Location loc, std::string prefix);
        void LogFileLocation(std::string filename, AST::Location loc);
        void LogPrefix(std::string prefix);

	public: // Lexer (Tokenizer)
		void UnexpectedCharacter(std::string filename, char ch, size_t line, size_t col);
		void UnexpectedEOF(std::string filename, size_t line, size_t col);

	public: // Parser
		void UnexpectedEofWhileToken(std::string filename, AST::TokenType tokenType, size_t line, size_t col);
		void UnexpectedToken(std::string filename, std::shared_ptr<AST::Token> locToken);
		void UnexpectedToken(std::string filename, std::shared_ptr<AST::Token> locToken, std::shared_ptr<AST::Token> gotToken, AST::TokenType expectedTokenType);
		void MissingConstantDefinition(std::string filename, std::shared_ptr<AST::Token> token);

	public: // Interpreter
        // TODO:
		// void UndeclaredFunction(std::shared_ptr<Nodes::ObjectMember> member);
		void ReturnMismatchType(std::string filename, std::string name, Runtime::ValueType type, Runtime::ValueType expectedType, AST::Location loc);
        void UnaryOperatorNotSupported(std::string filename, std::string op, Runtime::ValueType metType, AST::Location loc);
    public: // TypeChecker
        void UnexpectedReturnType(std::string filename, Runtime::ValueType expected, Runtime::ValueType gotType, AST::Location loc);
        void TypeMismatch(std::string filename, Runtime::ValueType left, Runtime::ValueType right, AST::Location loc);
    public: // TypeChecker + Interpreter
        void PrivateFunctionError(std::string filename, std::string fnName, std::string fnModule, AST::Location loc, AST::Location mLoc, std::string prefix);
        void ArgsParamsExhausted(std::string filename, std::string name, size_t argsSize, size_t paramsSize, AST::Location loc, std::string prefix);
		void ArgMismatchType(std::string filename, std::string name, Runtime::ValueType type, Runtime::ValueType expectedType, AST::Location loc, std::string prefix);
		void UndeclaredFunction(std::string filename, std::shared_ptr<AST::Nodes::IdentifierNode> id, std::string prefix);
		void UndeclaredFunction(std::string filename, std::shared_ptr<AST::Nodes::ObjectMember> obj, Runtime::ValueType valType, std::string prefix);
		void OperandMismatchType(std::string filename, Runtime::ValueType leftType, Runtime::ValueType rightType, AST::Location loc, std::string prefix);
		void UndeclaredVariable(std::string filename, std::shared_ptr<AST::Nodes::IdentifierNode> id, std::string prefix);
		void UndefinedType(std::string filename, std::string name, AST::Location loc, std::string prefix);
		void VarMismatchType(std::string filename, std::string name, Runtime::PValType type, Runtime::PValType expectedType, AST::Location loc, std::string prefix);
        void VarAlreadyExists(std::string filename, std::string name, AST::Location loc, std::string prefix);
        void FunctionRedeclaration(std::string filename, std::string name, AST::Location loc, std::string declFilename, AST::Location declLoc, std::string prefix);
    public: // CliRunner
        void NotEnoughArgs(int expected, int got, bool atLeast);
        void InvalidArgument(std::string arg, std::string message);
        void FailedOpeningFile(std::string path);
        void ProcedureNotFound(std::string name);
	};
}

#endif
