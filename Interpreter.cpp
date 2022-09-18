#pragma once
#include "Node.h"
#include <any>
#include <map>
#include <vector>
#include <string>
#include <iostream>

using std::any;
using std::map;
using std::string;
using std::vector;
using std::cout;
using std::endl;

static map<string, Function*> functionTable;

void interpret(Program* program) {
  for (auto& node: program->functions) {
    functionTable[node->name] = node;
  }
  // nullptr이 NULL 보다 선호되는 이유: https://stackoverflow.com/questions/20509734/null-vs-nullptr-why-was-it-replaced
  // 간단히 말하자면 0으로 평가될 위험이 없음. 포인터 타입으로만 평가됨
  if (functionTable["main"] == nullptr) return;
  functionTable["main"]->interpret();
}
/**
 * @brief Statement Interpreters
 */
void Function::interpret() {
  for (auto& node: block)
    node->interpret();
};
void Return::interpret() {};
void Variable::interpret() {};
void For::interpret() {};
void Break::interpret() {};
void Continue::interpret() {};
void If::interpret() {};
void Print::interpret() {
  for (auto& node:arguments) {
    auto value = node->interpret();
    cout << value << " ";
  }
  cout << endl;
};
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

