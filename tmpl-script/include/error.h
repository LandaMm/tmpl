#pragma once
#ifndef ERROR_H
#define ERROR_H
#include<iostream>
#include<string>

namespace Prelude
{
	class ErrorManager final
	{
	private:
		ErrorManager(const ErrorManager&) = delete;
		ErrorManager& operator=(const ErrorManager&) = delete;
	private:
		ErrorManager();
	private:
		~ErrorManager();
	public:
		static ErrorManager& getInstance()
		{
			static ErrorManager instance;
			return instance;
		}
	public:
		void RaiseError(std::string errorMessage);
	};
}

#endif