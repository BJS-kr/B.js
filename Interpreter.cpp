#include "Node.h"
#include "Datatype.h"
#include "Log.h"
#include <map>
#include <vector>

using std::map;
using std::vector;

static map<string, Statement*> global_variables = {
  {"console", new Console()}
};
static map<string, Function*> functionTable;
static Color::Modifier red(Color::FG_RED);
static Color::Modifier def(Color::FG_DEFAULT);

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
  if (functionTable["global"] == nullptr) return;
  info("function global found, starting interpret...");
  functionTable["global"]->interpret();
  print_log();
}
/**
 * @brief Statement Interpreters
 */
void Function::interpret() {
  for (auto& node: block) {
    if (dynamic_cast<ExpressionStatement*>(node)) info("Expression statement found");
    node->interpret();
  }
};
void Return::interpret() {};
void Variable::interpret() {};
void For::interpret() {};
void Break::interpret() {};
void Continue::interpret() {};
void If::interpret() {};
void Console::sequencePrint() {
    info("sequence printing started");
    for (const auto& node:arguments) {
      auto value = node->interpret();
      cout << value << " ";
   }
    cout << endl;
    info("sequenced printing ended");
}
void Console::interpret() {
  // 주의할 것은, 사실 range-based for-loop에서는
  // rvalue reference(ex: auto&&)가 더 일반적이라는 것이다
  // https://stackoverflow.com/questions/25158976/forcing-auto-to-be-a-reference-type-in-a-range-for-loop
  // 물론 &&는 일반적으로 r l value를 모두 참조할 수 있어서 좋다는 거고 아래처럼 lvalue가 확실할 때는 그냥 &로 적어도된다. 
  if (consoleMethod == "log") {
    info("console method: log");
    cout << def;
    sequencePrint();
  }
  else if (consoleMethod == "error") {
    cout << red;
    sequencePrint();
  }

};
void ExpressionStatement::interpret() {
  if (auto method = dynamic_cast<Method*>(expression)) {
    info("Method found");
    method->interpret();
  }
};

/**
 * @brief Expression Interpreters 
 */
any Or::interpret() {return 1;};
any And::interpret() {return 1;};
any Relational::interpret() {return 1;};
any Arithmetic::interpret() {
  auto left_value = lhs->interpret();
  auto right_value = rhs->interpret();

  // add number
  if (kind == Kind::Add && isNumber(left_value) && isNumber(right_value)) {
    info("Arithmetic: number add interpreting...");
    return toNumber(left_value) + toNumber(right_value);
  } 
  
  // add string
  if (kind == Kind::Add && isString(left_value) && isString(right_value)) {
    return toString(left_value) + toString(right_value);
  }
  // subtract
  if (kind == Kind::Subtract && isNumber(left_value) && isNumber(right_value))
    return toNumber(left_value) - toNumber(right_value);
  // multiply  
  if (kind == Kind::Multiply && isNumber(left_value) && isNumber(right_value)) {
    info("Arithmetic: multiply interpreting...");
    return toNumber(left_value) * toNumber(right_value);
  }
  if (kind == Kind::Divide && isNumber(left_value) && isNumber(right_value)) {
    info("Arithmetic: multiply interpreting...");
    return toNumber(left_value) / toNumber(right_value);
  } 
};  
any Unary::interpret() {return 1;};
any Call::interpret() {return 1;};
any GetElement::interpret() {return 1;};
any SetElement::interpret() {return 1;};
any GetVariable::interpret() {
  if (name == "console") {
    info("getting global variable: console");
    return dynamic_cast<Console*>(global_variables.at("console"));
  }
};
any SetVariable::interpret() {return 1;};
any NullLiteral::interpret() {return 1;};
any BooleanLiteral::interpret() {return 1;};
any NumberLiteral::interpret() {
  info("number literal interpreting...");
  return value;
}
any StringLiteral::interpret() {
  return value;
};
any ArrayLiteral::interpret() {return 1;};
any ObjectLiteral::interpret() {return 1;};
any Method::interpret() {
  // method객체가 아닌 getvariable객체라는 것은 메서드 체인이라고 하더라도 시작점에 도달했다는 의미
  // 즉, 그게 무엇이 되었던 정의된 객체일 것임
  // 메서드의 this_ptr은 언제나 literal, variable, method(메서드 체인일 경우) 밖에 없음
  if (auto get_variable = dynamic_cast<GetVariable*>(this_ptr)) {
    info("method start point(GetVariable) found");
    auto this_object = get_variable->interpret();
    if (auto console = any_cast<Console*>(this_object)){
      info("start point: console initialized");
      console->consoleMethod = method;
      console->arguments = arguments;
      console->interpret();
    }
  }
};

