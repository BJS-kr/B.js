#pragma once
#include "Node.h"
#include <any>
#include <map>
#include <vector>
#include <string>

using std::any;

/**
 * @brief Statement Interpreters
 */
void Function::interpret() {};
void Return::interpret() {};
void Variable::interpret() {};
void For::interpret() {};
void Break::interpret() {};
void Continue::interpret() {};
void If::interpret() {};
void Print::interpret() {};
void ExpressionStatement::interpret() {};

/**
 * @brief Expression Interpreters 
 */
any Or::interpret() {};
any And::interpret() {};
any Relational::interpret() {};
any Arithmetic::interpret() {};
any Unary::interpret() {};
any Call::interpret() {};
any GetElement::interpret() {};
any SetElement::interpret() {};
any GetVariable::interpret() {};
any SetVariable::interpret() {};
any NullLiteral::interpret() {};
any BooleanLiteral::interpret() {};
any NumberLiteral::interpret() {};
any StringLiteral::interpret() {};
any ArrayLiteral::interpret() {};
any MapLiteral::interpret() {};

