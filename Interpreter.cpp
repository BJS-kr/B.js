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
  // 아래의 for문은 단지 program에 등록된 함수들을 모두 functionTable에 등록시키는 것이다.
  // 이렇게 처리하면 스크립트에서 어떤 순서로 함수를 작성했던지간에 상관없이 함수를 호출할 수 있게 된다.
  for (auto& node: program->functions) {
    functionTable[node->name] = node;
  }
  // nullptr이 NULL 보다 선호되는 이유: https://stackoverflow.com/questions/20509734/null-vs-nullptr-why-was-it-replaced
  // 간단히 말하자면 0으로 평가될 위험이 없음. 포인터 타입으로만 평가됨
  // 모든 함수를 functionTable에 등록한 후, main이라는 함수가 없으면 즉시 종료
  // main이 있다면 main부터 실행하면서 main내부의 함수를 차례차례 실행하는 방식
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
any Or::interpret() {return 1;};
any And::interpret() {return 1;};
any Relational::interpret() {return 1;};
any Arithmetic::interpret() {return 1;};
any Unary::interpret() {return 1;};
any Call::interpret() {return 1;};
any GetElement::interpret() {return 1;};
any SetElement::interpret() {return 1;};
any GetVariable::interpret() {return 1;};
any SetVariable::interpret() {return 1;};
any NullLiteral::interpret() {return 1;};
any BooleanLiteral::interpret() {return 1;};
any NumberLiteral::interpret() {return 1;}
any StringLiteral::interpret() {
  return value;
};
any ArrayLiteral::interpret() {return 1;};
any MapLiteral::interpret() {return 1;};

