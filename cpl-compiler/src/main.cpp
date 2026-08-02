#include "compiler.h"

int evaluate_program(ProgramNode* node)
{
	return node->GetItemsPtr()->size();
}
