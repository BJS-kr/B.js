#pragma once
#include "Node.h"
#include "Datatype.h"
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
  // 주의할 것은, 사실 range-based for-loop에서는
  // rvalue reference(ex: auto&&)가 더 일반적이라는 것이다
  // https://stackoverflow.com/questions/25158976/forcing-auto-to-be-a-reference-type-in-a-range-for-loop
  // 물론 &&는 일반적으로 r l value를 모두 참조할 수 있어서 좋다는 거고 아래처럼 lvalue가 확실할 때는 그냥 &로 적어도된다. 
  for (const auto& node:arguments) {
    auto value = node->interpret();
    cout << value << " ";
    if (lineFeed) cout << endl;
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
any StringLiteral::interpret() {
  return value;
};
any ArrayLiteral::interpret() {};
any MapLiteral::interpret() {};

