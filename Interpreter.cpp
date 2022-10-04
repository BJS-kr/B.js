#define GLOBAL "global"

#include "Node.h"
#include "Datatype.h"
#include "Log.h"
#include <map>
#include <vector>

using std::map;
using std::vector;
using std::make_pair;

static auto CONSOLE = new Console();
static auto UNDEFINED = new Undefined();

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
  if (functionTable[GLOBAL] == nullptr) return;
  info("function global found, starting interpret...");
  functionTable[GLOBAL]->interpret();
}
/**
 * @brief Statement Interpreters
 */
void Function::interpret() {
  // 모든 node는 new로 할당되었으므로 메모리 해제도 interpret이 후 이뤄져야한다.
  for (auto& node: block) {
    if (dynamic_cast<ExpressionStatement*>(node)) info("Expression statement found");
    node->interpret();
  }
};
void Return::interpret() {};
void Declare::interpret() {
  auto initial_value = make_pair(name, VariableState{false, nullptr});

  switch(decl_type) {
    case Kind::Constant: {
      auto constants = lexical_environment->variables.at(Kind::Constant);
      if (constants.find(name) == constants.end()) 
        constants.insert(initial_value);
      else {
        error("Cannot declare constant variable twice");
      }
      break;
    }
    case Kind::Let: {
      auto lets = lexical_environment->variables.at(Kind::Let);
      if (lets.find(name) == lets.end()) 
        lets.insert(initial_value);
      else {
        error("Cannot declare let variable twice");
      }
      break;
    }
    case Kind::Variable: {
      auto variables = lexical_environment->variables.at(Kind::Variable);
      variables.insert(initial_value);
      break;
    }
    default: 
      error("Unknown declaration type detected. Declaration must be one of const, let, var");
  }
  // if와 for의 구현에 대한 생각:
  // 어차피 스코프 체인을 염두에 둔다면 무엇이던 간에 블락 스코프 체인임
  // 즉, 항상 상위 스코프만 기억하면 되므로 기존의 구현을 수정할 필요가 있음
};
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
  // divide
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
    return CONSOLE;
  }
  while (lexical_environment != nullptr) {
    auto variables = lexical_environment->variables;
    
    auto consts = variables.at(Kind::Constant);
    auto lets = variables.at(Kind::Let);
    auto vars = variables.at(Kind::Variable);

    auto const_iter = consts.find(name);
    auto let_iter = lets.find(name);
    auto var_iter = vars.find(name);
    
    if (const_iter != consts.end()) {
      info("Constant variable " + name + " found");
      return consts.at(name).value;
    } else if (let_iter != lets.end()) {
      info("Let variable " + name + " found");
      return lets.at(name).value;
    } else if (var_iter != lets.end()) {
      info("Var variable " + name + " found");
      return vars.at(name).value;
    } else {
      info("Moving up to upper scope");
      lexical_environment = lexical_environment->upper_scope;
    }
  }
  cout << "Cannot find variable " << name << " in scope chains. returning UNDEFINED" << endl;
  return UNDEFINED;
};
any SetVariable::interpret() {
  auto allocating_value = VariableState{true, value};
  // lexical_environment가 nullptr이라는 것은 마지막 루프가 global이었다는 의미
  while (lexical_environment != nullptr) {
    auto variables = lexical_environment->variables;
    
    auto consts = variables.at(Kind::Constant);
    auto lets = variables.at(Kind::Let);
    auto vars = variables.at(Kind::Variable);

    auto const_iter = consts.find(name);
    auto let_iter = lets.find(name);
    auto var_iter = vars.find(name);
    
    if (const_iter != consts.end()) {
      if (!consts.at(name).initialized) {
        const_iter->second = allocating_value;
      } else {
        error("Constant variable can be allocated only once");
      }
    } else if (let_iter != lets.end()) {
      let_iter->second = allocating_value;
    } else if (var_iter != lets.end()) {
      var_iter->second = allocating_value;
    } else {
      lexical_environment = lexical_environment->upper_scope;
    }
  }
        
  if (lexical_environment == nullptr) {
    error("Cannot find predefined variable. to allocate value to a variable, a variable must have been declared");
  }
};
any NullLiteral::interpret() {
  info("null literal interpreting..."); 
  return NULL; 
};
any BooleanLiteral::interpret() { 
  info("boolean literal interpreting...");
  return value; 
};
any NumberLiteral::interpret() {
  info("number literal interpreting...");
  return value;
}
any StringLiteral::interpret() {
  info("string literal interpreting...");
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

