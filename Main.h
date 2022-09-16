#pragma once
#include "Node.h"
#include "Token.h"

vector<Token> scan(string);
Program* parse(vector<Token>);
void printSyntaxTree(Program*);
