#define GLOBAL "global"

#include "Node.h"
#include "Datatype.h"
#include "Log.h"
#include <map>
#include <vector>

using std::map;
using std::vector;
using std::make_pair;
using std::stod;

static auto CONSOLE = new Console();

static map<string, FunctionExpression*> functionTable;
struct ReturnException { Expression* result; };
struct BreakException {};
struct ContinueException {};

void interpret(Program* program) {
  // 아래의 for문은 단지 program에 등록된 함수들을 모두 functionTable에 등록시키는 것이다.
  // 이렇게 처리하면 스크립트에서 어떤 순서로 함수를 작성했던지간에 상관없이 함수를 호출할 수 있게 된다.
  for (auto& node: program->functions) {
    functionTable[node->function->name] = node->function;
  }
  // nullptr이 NULL 보다 선호되는 이유: https://stackoverflow.com/questions/20509734/null-vs-nullptr-why-was-it-replaced
  // 간단히 말하자면 0으로 평가될 위험이 없음. 포인터 타입으로만 평가됨
  // 모든 함수를 functionTable에 등록한 후, main이라는 함수가 없으면 즉시 종료
  // main이 있다면 main부터 실행하면서 main내부의 함수를 차례차례 실행하는 방식
  if (functionTable[GLOBAL] == nullptr) return;
  info("function global found, starting interpret...");
  functionTable[GLOBAL]->interpret();
}

any Undefined::interpret() { return this; };
/**
 * @brief Statement Interpreters
 */
void Function::interpret() {
  // 모든 node는 new로 할당되었으므로 메모리 해제도 interpret이 후 이뤄져야한다.
  try {
    for (auto& node: block) {
      if (dynamic_cast<Declare*>(node)) info("Declare found");
      if (dynamic_cast<ExpressionStatement*>(node)) info("ExpressionStatement found");
      if (dynamic_cast<Return*>(node)) info("return statement found");
      
      node->interpret();
    }
  } catch(ReturnException return_exception) {
      throw return_exception;
  }  

};
void Return::interpret() {
  throw ReturnException{ expr };
}
void DeclareFunction::interpret() {
  auto declare = new Declare(Kind::Variable);
  declare->lexical_environment = function->upper_scope;
  declare->name = function->name;
  declare->interpret();

  delete declare;

  auto setter =  new SetVariable();
  setter->lexical_environment = function->upper_scope;
  setter->name = function->name;
  setter->value = function;
  setter->interpret();

  delete setter;
}
void Declare::interpret() {
  auto initial_value = make_pair(name, VariableState{false, nullptr});

  switch(decl_type) {
    case Kind::Constant: {
      info("interpreting const variable declaration: " + name);
      
      if (lexical_environment->variables.at(Kind::Constant).find(name) == 
          lexical_environment->variables.at(Kind::Constant).end()) {
        info("declaring const variable: " + name + ", in lexical environment: " + lexical_environment_to_string(lexical_environment));
        lexical_environment->variables.at(Kind::Constant).insert(initial_value);
      } 
      else 
        error("Cannot declare constant variable twice");
      
      break;
    }
    case Kind::Let: {
      info("interpreting let variable declaration: " + name);
      
      if (lexical_environment->variables.at(Kind::Let).find(name) == 
          lexical_environment->variables.at(Kind::Let).end()) {
        info("declaring let variable: " + name + ", in lexical environment: " + lexical_environment_to_string(lexical_environment));
        lexical_environment->variables.at(Kind::Let).insert(initial_value);
      }
      else 
        error("Cannot declare let variable twice");
      
      break;
    }
    case Kind::Variable: {
      info("interpreting var variable declaration: " + name);
      info("declaring var variable: " + name + ", in lexical environment: " + lexical_environment_to_string(lexical_environment));
      lexical_environment->variables.at(Kind::Variable).insert(initial_value);

      break;
    }
    default: 
      error("Unknown declaration type detected. Declaration must be one of const, let, var");
  }
  // if와 for의 구현에 대한 생각:
  // 어차피 스코프 체인을 염두에 둔다면 무엇이던 간에 블락 스코프 체인임
  // 즉, 항상 상위 스코프만 기억하면 되므로 기존의 구현을 수정할 필요가 있음
};
void For::interpret() {
  
  info("entering for loop...");
  for (auto& node:starting_point) {
    node->interpret();
  }
  info("for loop starting value interpreted");
  while (toBool(condition->interpret())) {
    info("looping...");
    try {
      for (auto& node:block) {
        node->interpret();
      }
    } catch(BreakException) {
      break;
    } catch(ContinueException) {
      expression->interpret();
      continue;
    }
    expression->interpret();
    // for loop은 매 순회마다 선언된 값들을 모두 비워줘야함
    // 그러나 기준 변수는 기억해야함
    // 아래는 그 과정
    auto getter = new GetVariable();
    getter->lexical_environment = this;
    getter->name = starting_point_name;
    auto value = toNumber(getter->interpret());

    delete getter;

    variables = {
      {Kind::Constant, map<string, VariableState>()},
      {Kind::Let, map<string, VariableState>()},
      {Kind::Variable, map<string, VariableState>()}
    };

    auto declare = new Declare(Kind::Let);
    declare->lexical_environment = this;
    declare->name = starting_point_name;
    declare->interpret();

    delete declare;

    auto number = new NumberLiteral();
    number->value = value;

    auto setter = new SetVariable();
    setter->lexical_environment = this;
    setter->name = starting_point_name;
    setter->value = number;
    setter->interpret();

    delete setter;
  }
};
void Break::interpret() {
  throw BreakException{};
};
void Continue::interpret() {
  throw ContinueException{};
};
void If::interpret() {
  info("If interpreting...");
  if (dynamic_cast<And*>(condition)) info("If condition: And");
  if (dynamic_cast<Or*>(condition)) info("If condition: Or");
  if (dynamic_cast<Relational*>(condition) == nullptr) {
    if (is_truthy(condition))
      for (auto& node:block) node->interpret();
    return;
  }

  auto boolean = toBool(condition->interpret());
  if (boolean) {
    for (auto& node:block) node->interpret();
  }
};
void Console::sequencePrint() {
    info("sequence printing started");
    for (const auto& node:arguments) {
      auto value = node->interpret();
      cout << value << " ";
   }
    cout << endl;
    info("sequence printing ended");
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
  if (consoleMethod == "error") {
    cout << red;
    sequencePrint();
  }
};
void ExpressionStatement::interpret() {
  if (auto method = dynamic_cast<Method*>(expression)) {
    info("Method found in ExpressionStatement");
    method->interpret();
  }
  if (auto set_variable = dynamic_cast<SetVariable*>(expression)) {
    info("SetVariable found in ExpressionStatement");
    set_variable->interpret();
  }
  if (auto get_variable = dynamic_cast<GetVariable*>(expression)) {
    info("GetVariable found in ExpressionStatement");
    get_variable->interpret();
  }
  if (auto call = dynamic_cast<Call*>(expression)) {
    info("Call found in ExpressionStatement");
    call->interpret();
  }
  if (auto set_element = dynamic_cast<SetElement*>(expression)) {
    info("SetElement found in ExpressionStatement");
    set_element->interpret();
  }
  if (auto unary = dynamic_cast<Unary*>(expression)){
    info("Unary found in ExpressionStatement");
    unary->interpret();
  }
};

/**
 * @brief Expression Interpreters 
 */
bool Judge::is_truthy(Expression* expr) {
    if (dynamic_cast<Or*>(expr)) info("jundging: Or");
    if (dynamic_cast<And*>(expr)) info("judging: And");
    if (expr == nullptr) return false;
    if (auto boolean = dynamic_cast<BooleanLiteral*>(expr)) {
      info("judging boolean");
      return boolean->boolean;
    };
    if (dynamic_cast<Undefined*>(expr)) {
      info("judging undefined");
      return false;
    }
    if (dynamic_cast<NullLiteral*>(expr)) {
      info("judging null");
      return false;
    }
    if (auto str = dynamic_cast<StringLiteral*>(expr)) {
      info("judging string literal");
      auto empty_str = toString(str->interpret());
      if (empty_str == string("")) return false; 
    }
    if (auto number = dynamic_cast<NumberLiteral*>(expr)) {
      info("judging number literal");
      if (number->value == double(0)) return false;
    }
    if (auto minus_zero = dynamic_cast<Arithmetic*>(expr)) {
      info("judging minus zero(Arithmetic)");
      if (minus_zero->lhs == nullptr && minus_zero->kind == Kind::Subtract) return false; 
    }
    if (auto and_ = dynamic_cast<And*>(expr)) {
      info("nested relational: And detected");
      if (is_truthy(and_->lhs)) return is_truthy(and_->rhs);
      return false;
    }
    if (auto or_ = dynamic_cast<Or*>(expr)) {
      info("nested relational: Or detected");
      if (!is_truthy(or_->lhs)) return is_truthy(or_->rhs);
      return true;
    }
    return true;
}

any Not::interpret() {
  return !is_truthy(expr);
}

any Or::interpret() {
  if (!is_truthy(lhs)) return is_truthy(rhs);
  return true;
};
any And::interpret() {
  if (is_truthy(lhs)) return is_truthy(rhs);
  return false;
};
any Relational::interpret() {
  auto l = lhs->interpret();
  auto r = rhs->interpret();

  if (kind == Kind::GreaterThan) {
    if (isArray(l) || isArray(r) || isObject(l) || isObject(r)) return false;
    if (isNumber(l)) {
      if (isNumber(r)) return toNumber(l) > toNumber(r);
      if (isString(r)) return toNumber(l) > stod(toString(r));
    }
    if (isString(l)) {
      if (isNumber(r)) return stod(toString(l)) > toNumber(r);
      if (isString(r)) return toString(l) > toString(r); 
    }
  }
  if (kind == Kind::LesserThan) {
    if (isArray(l) || isArray(r) || isObject(l) || isObject(r)) return false;
    if (isNumber(l)) {
      if (isNumber(r)) return toNumber(l) < toNumber(r);
      if (isString(r)) return toNumber(l) < stod(toString(r));
    }
    if (isString(l)) {
      if (isNumber(r)) return stod(toString(l)) < toNumber(r);
      if (isString(r)) return toString(l) < toString(r); 
    }
  }
  if (kind == Kind::Equal) {
    if (isNumber(l)) {
      if (isNumber(r)) return toNumber(l) == toNumber(r);
      if (isString(r)) return toNumber(l) == stod(toString(r));
    }
    if (isString(l)) {
      if (isNumber(r)) return stod(toString(l)) == toNumber(r);
      if (isString(r)) return toString(l) == toString(r); 
    }
    if (isArray(l)) {
      if (isArray(r)) return toArray(l) == toArray(r);
      return false;
    }
    if (isObject(l)) {
      if (isObject(r)) return toObject(l) == toObject(r);
      return false;
    } 
  }  
  if (kind == Kind::StrictEqual) {
    if (isNumber(l)) {
      if (isNumber(r)) return toNumber(l) == toNumber(r);
      if (isString(r)) return false;
    }
    if (isString(l)) {
      if (isNumber(r)) return false;
      if (isString(r)) return toString(l) == toString(r); 
    }
    if (isArray(l)) {
      if (isArray(r)) return toArray(l) == toArray(r);
      return false;
    }
    if (isObject(l)) {
      if (isObject(r)) return toObject(l) == toObject(r);
      return false;
    }
  }
  if (kind == Kind::NotEqual) {
    if (isNumber(l)) {
      if (isNumber(r)) return toNumber(l) != toNumber(r);
      if (isString(r)) return toNumber(l) != stod(toString(r));
    }
    if (isString(l)) {
      if (isNumber(r)) return stod(toString(l)) != toNumber(r);
      if (isString(r)) return toString(l) != toString(r); 
    }
    if (isArray(l)) {
      if (isArray(r)) return toArray(l) != toArray(r);
      return false;
    }  
    if (isObject(l)) {
      if (isObject(r)) return toObject(l) != toObject(r);
      return false;
    } 
  }
  if (kind == Kind::StrictNotEqual) {
    if (isNumber(l)) {
      if (isNumber(r)) return toNumber(l) == toNumber(r);
      if (isString(r)) return false;
    }
    if (isString(l)) {
      if (isNumber(r)) return false;
      if (isString(r)) return toString(l) == toString(r); 
    }
    if (isArray(l)) {
      if (isArray(r)) return toArray(l) == toArray(r);
      return false;
    }
    if (isObject(l)) {
      if (isObject(r)) return toObject(l) == toObject(r);
      return false;
    }
  }
  if (kind == Kind::GreaterOrEqual) {
    if (isArray(l) || isArray(r) || isObject(l) || isObject(r)) return false;
    if (isNumber(l)) {
      if (isNumber(r)) return toNumber(l) >= toNumber(r);
      if (isString(r)) return toNumber(l) >= stod(toString(r));
    }
    if (isString(l)) {
      if (isNumber(r)) return stod(toString(l)) >= toNumber(r);
      if (isString(r)) return toString(l) >= toString(r);
    }
  }
  if (kind == Kind::LesserOrEqual) {
    if (isArray(l) || isArray(r) || isObject(l) || isObject(r)) return false;
    if (isNumber(l)) {
      if (isNumber(r)) return toNumber(l) <= toNumber(r);
      if (isString(r)) return toNumber(l) <= toString(r).at(0);
    }
    if (isString(l)) {
      if (isNumber(r)) return toString(l).at(0) <= toNumber(r);
      if (isString(r)) return toString(l) <= toString(r);
    }
  }

  return false;
};
any Arithmetic::interpret() {
  auto left_value = lhs->interpret();
  auto right_value = rhs->interpret();

  if (kind == Kind::Add) {
    info("Arithmetic: number add interpreting...");
    if (isNumber(left_value) && isNumber(right_value)) {
      info("Number + Number");
      return toNumber(left_value) + toNumber(right_value);
    }
    if (isString(left_value) && isString(right_value)) {
      info("String + String");
      return toString(left_value) + toString(right_value);
    }
    if (isString(left_value) && isNumber(right_value)) {
      info("String + Number");
      return toString(left_value) + doubleToString(toNumber(right_value));
    }
    if (isNumber(left_value) && isString(right_value)) {
      info("Number + String");
      return doubleToString(toNumber(left_value)) + toString(right_value);
    }
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
    info("Arithmetic: divide interpreting...");
    return toNumber(left_value) / toNumber(right_value);
  } 
};  
any Unary::interpret() {
  info("interpreting unary...");
  if (kind == Kind::Increment) { 
    info("unary Increment found...");
    if (auto getter = dynamic_cast<GetVariable*>(sub)) {
      info("attempting increment to: " + getter->name);
      auto number_1 = new NumberLiteral();
      number_1->value = 1;

      auto add_1 = new Arithmetic(Kind::Add);
      add_1->lhs = getter;
      add_1->rhs = number_1;
      
      auto added = new NumberLiteral();
      added->value = toNumber(add_1->interpret());

      auto setter = new SetVariable();
      setter->lexical_environment = lexical_environment;
      setter->name = getter->name;
      setter->value = added;
      setter->interpret();

      delete number_1;
      delete add_1;
      delete setter;

      return getter->interpret();
    } 
  }
 if (kind == Kind::Decrement) { 
    info("unary Decrement found...");
    if (auto getter = dynamic_cast<GetVariable*>(sub)) {
      info("attempting Decrement to: " + getter->name);
      auto number_1 = new NumberLiteral();
      number_1->value = 1;

      auto subtract_1 = new Arithmetic(Kind::Subtract);
      subtract_1->lhs = getter;
      subtract_1->rhs = number_1;
      
      auto subtracted = new NumberLiteral();
      subtracted->value = toNumber(subtract_1->interpret());

      auto setter = new SetVariable();
      setter->lexical_environment = lexical_environment;
      setter->name = getter->name;
      setter->value = subtracted;
      setter->interpret();

      delete number_1;
      delete subtract_1;
      delete setter;

      return getter->interpret();
    } 
  }
};
any Call::interpret() {
  info("interpreting call...");
  try {
    if (auto method = dynamic_cast<Method*>(sub)) {
      info("call sub is Method");
      if (auto getter = dynamic_cast<GetVariable*>(method->this_ptr)) {
        if (isConsole(getter->interpret())) {
          info("console method found");
          auto console = toConsole(getter->interpret());
          console->arguments = arguments;
          console->consoleMethod = method->method;
          console->interpret();
        }
        if (isArray(getter->interpret())) {
          info("array method found");
          auto array = toArray(getter->interpret());
          array->arguments = arguments;
          array->array_method = method->method;
          return array->interpret();
        }
      }
      if (auto array = dynamic_cast<ArrayLiteral*>(method->this_ptr)) {
        info("array literal method found");
        array->arguments = arguments;
        array->array_method = method->method;
        return array->interpret();
      }
      // 사실 call이 아닐 가능성도 있지만 사양이 작은 컴파일러이므로 항상 call임을 가정한다.
      if (auto pre_call = dynamic_cast<Call*>(method->this_ptr)) {
        info("method chaining detected");
        method->this_ptr = any_cast<ArrayLiteral*>(pre_call->interpret());
        return this->interpret();
      }
    }
    if (auto func_expr = dynamic_cast<FunctionExpression*>(sub)) {
      info("call sub is FunctionExpression");
    }
    if (auto var_getter = dynamic_cast<GetVariable*>(sub)) {
      info("call sub is GetVariable");
      if (isFunctionExpression(var_getter->interpret())) {
        auto func_expr = toFunctionExpression(var_getter->interpret());
        for (int i = 0; i < func_expr->parameters.size(); i++) {
          auto declare = new Declare(Kind::Variable);
          declare->lexical_environment = func_expr;
          declare->name = func_expr->parameters[i];
          declare->interpret();
          
          delete declare;

          auto setter = new SetVariable();
          setter->lexical_environment = func_expr;
          setter->name = func_expr->parameters[i];
          setter->value = arguments[i];
          setter->interpret();

          delete setter;
        }
        func_expr->interpret();
      }
      
    }
    if (auto elem_getter = dynamic_cast<GetElement*>(sub)) {
      info("call sub is GetElement");
      if (isFunctionExpression(elem_getter->interpret())) {
        auto func_expr = toFunctionExpression(elem_getter->interpret());
        for (int i = 0; i < func_expr->parameters.size(); i++) {
          auto declare = new Declare(Kind::Variable);
          declare->lexical_environment = func_expr;
          declare->name = func_expr->parameters[i];
          declare->interpret();
          
          delete declare;

          auto setter = new SetVariable();
          setter->lexical_environment = func_expr;
          setter->name = func_expr->parameters[i];
          setter->value = arguments[i];
          setter->interpret();

          delete setter;
        }
        func_expr->interpret();
      }
    }
  } catch(ReturnException return_exception) {
    info("return value received after call");
    return return_exception.result->interpret();
  }

  return Undefined{}.interpret();
};
any GetElement::interpret() {
  info("GetElement interpreting...");
  auto idx = index->interpret();
  if (auto getter = dynamic_cast<GetVariable*>(sub)) {
    if (isObject(getter->interpret())) {
      auto obj = toObject(getter->interpret());
      if (isNumber(idx)) {
        auto idx_str = doubleToString(toNumber(idx));
        if (obj->values.find(idx_str) == obj->values.end()) return Undefined{}.interpret();
          return obj->values[idx_str]->interpret();
      }
      if (isString(idx)) {
        auto idx_str = toString(idx);
        if (obj->values.find(idx_str) == obj->values.end()) return Undefined{}.interpret();
          return obj->values[idx_str]->interpret();
      }
    }
    if (isArray(getter->interpret())) {
      auto arr = toArray(getter->interpret());
      if (isNumber(idx)) {
        auto idx_num = toNumber(idx);
        if (idx_num > arr->values.size() - 1 || idx_num < 0) return Undefined{}.interpret();
          return arr->values[idx_num]->interpret();
      }
    }
  }
  return Undefined{}.interpret();
};
any SetElement::interpret() {
  
  if (auto getter = dynamic_cast<GetVariable*>(sub)) {
    if (isArray(getter->interpret())) {
      auto array = toArray(getter->interpret());
      auto idx = toNumber(index->interpret());
      array->values[idx] = value;
    }
  }
  return Undefined{}.interpret();
};
any GetVariable::interpret() {
  if (name == "console") {
    return CONSOLE;
  }

  while (lexical_environment != nullptr) {
    if (lexical_environment->variables.at(Kind::Constant).find(name) != 
        lexical_environment->variables.at(Kind::Constant).end()) {
      info("Constant variable " + name + " found");
      if (auto func_expr = dynamic_cast<FunctionExpression*>(lexical_environment->variables.at(Kind::Constant).at(name).value)) {
        info("const variable function expression found");
        return func_expr;
      }
      return lexical_environment->variables.at(Kind::Constant).at(name).value->interpret();
    }

    if (lexical_environment->variables.at(Kind::Let).find(name) != 
        lexical_environment->variables.at(Kind::Let).end()) {
      info("Let variable " + name + " found");
      if (auto func_expr = dynamic_cast<FunctionExpression*>(lexical_environment->variables.at(Kind::Let).at(name).value)) {
        info("let variable function expression found");
        return func_expr;
      }
      return lexical_environment->variables.at(Kind::Let).at(name).value->interpret();
    }

    if (lexical_environment->variables.at(Kind::Variable).find(name) != 
        lexical_environment->variables.at(Kind::Variable).end()) {
      info("Var variable " + name + " found");
      if (auto func_expr = dynamic_cast<FunctionExpression*>(lexical_environment->variables.at(Kind::Variable).at(name).value)) {
        info("var variable function expression found");
        return func_expr; 
      }
      return lexical_environment->variables.at(Kind::Variable).at(name).value->interpret();
    }

    info("Moving up to upper scope...");
    lexical_environment = lexical_environment->upper_scope;
  }

  info("Cannot find variable " + name + " in scope chains. returning UNDEFINED");
  return Undefined{}.interpret();
};

VariableState SetVariable::get_allocating_value() {
  if (auto arithmetic = dynamic_cast<Arithmetic*>(value)) {
    auto result = arithmetic->interpret();
    if (isNumber(result)) {
      auto number = new NumberLiteral();
      number->value = toNumber(result);

      return VariableState{true, number};
    }
    if (isString(result)) {
      auto string = new StringLiteral();
      string->value = toString(result);

      return VariableState{true, string};
    }
  }

  return VariableState{true, value};
}
any SetVariable::interpret() {
  // lexical_environment가 nullptr이라는 것은 마지막 루프가 global이었다는 의미
  if (dynamic_cast<Call*>(value)) info("Call* found");
  while (lexical_environment != nullptr) {
   
    info("attempting allocation. to: "  + name + ", in lexical environment: " + lexical_environment_to_string(lexical_environment));
    
    if (lexical_environment->variables.at(Kind::Constant).find(name) != 
        lexical_environment->variables.at(Kind::Constant).end()) {
      if (!lexical_environment->variables.at(Kind::Constant).at(name).initialized) {
        lexical_environment->variables.at(Kind::Constant).find(name)->second = get_allocating_value();
        info("Constant variable: " + name + " allocated");
        break;
      } else {
        error("Constant variable can be allocated only once");
      }
    }

    if (lexical_environment->variables.at(Kind::Let).find(name) != 
        lexical_environment->variables.at(Kind::Let).end()) {
      lexical_environment->variables.at(Kind::Let).find(name)->second = get_allocating_value();
      info("Let variable: " + name + " allocated");
      break;
    }
    if (lexical_environment->variables.at(Kind::Variable).find(name) != 
        lexical_environment->variables.at(Kind::Variable).end()) {
      lexical_environment->variables.at(Kind::Variable).find(name)->second = get_allocating_value();
      info("Var variable: " + name + " allocated");
      break;
    } 
      
    info("moving up to upper scope...");
    lexical_environment = lexical_environment->upper_scope;
    if (lexical_environment == nullptr) {
      error("Cannot find predefined variable. to allocate value to a variable, a variable must have been declared");
    }
  }
        
  return Undefined{}.interpret();
};
any NullLiteral::interpret() {
  info("null literal interpreting..."); 
  return nullptr; 
};
any BooleanLiteral::interpret() { 
  info("boolean literal interpreting...");
  return boolean; 
};
any NumberLiteral::interpret() {
  info("number literal interpreting...");
  return value;
}
any StringLiteral::interpret() {
  info("string literal interpreting...");
  return value;
};
any ArrayLiteral::interpret() {
  info("array literal interpreting...");
  if (array_method == "map") {
    // 특정 array객체가 마지막으로 실행되었던 메서드를 기억해서는 안된다.
    array_method = "";
    info("Array.map called");
    if (!arguments.size()) error("Array.map method needs one argument at least");
    if (auto callbackFn = dynamic_cast<FunctionExpression*>(arguments[0])) {

      for (auto& param:callbackFn->parameters) {
        auto declare = new Declare(Kind::Variable);
        declare->lexical_environment = callbackFn;
        declare->name = param;
        
        declare->interpret();

        delete declare;
      }

      auto result = new ArrayLiteral();
      for (int i = 0; i < values.size(); i++) {
        auto setter = new SetVariable();
        setter->lexical_environment = callbackFn;
        setter->name = callbackFn->parameters[0];
        setter->value = values[i];
        setter->interpret();
        delete setter;
        try {
          for (auto& node:callbackFn->block) {
            node->interpret();
          }
        } catch(ReturnException return_exception) {
          info("map callbackFn returned a value");
          auto value = return_exception.result->interpret();
          if (isString(value)) {
            auto str = new StringLiteral();
            str->value = toString(value);
            result->values.push_back(str);
          }
          if (isNumber(value)) {
            auto num = new NumberLiteral();
            num->value = toNumber(value);
            result->values.push_back(num);
          } 
        }
      }
      info("Array.map finished. returning new Array");
      // FunctionExpression객체(callbackFn) 메모리 해제
      delete arguments[0];
      return result;
    } else {
      error("Array.map method's first arg must be a function");
    }
  } 
  if (array_method == "reduce") {
    array_method = "";
    info("Array.reduce called");
    if (!arguments.size()) error("Array.reduce method needs one argument at least");
    if (auto callbackFn = dynamic_cast<FunctionExpression*>(arguments[0])) {
      for (auto& param:callbackFn->parameters) {
        auto declare = new Declare(Kind::Variable);
        declare->lexical_environment = callbackFn;
        declare->name = param;
        declare->interpret();
        delete declare;
      }

      auto initial_value = arguments[1];
      for (int i = 0; i < values.size(); i++) {
        auto acc = new SetVariable();
        acc->lexical_environment = callbackFn;
        acc->name = callbackFn->parameters[0];
        acc->value = initial_value;
        acc->interpret();
        delete acc;

        auto curr = new SetVariable();
        curr->lexical_environment = callbackFn;
        curr->name = callbackFn->parameters[1];
        curr->value = values[i];
        curr->interpret();
        delete curr;

        try{
          for (auto& node:callbackFn->block) {
            node->interpret();
          }
        } catch(ReturnException return_exception) {
          info("reduce callbackFn returned a value");
          auto value = return_exception.result->interpret();
          if (isString(value)) {
            auto str = new StringLiteral();
            str->value = toString(value);
            initial_value = str;
          }
          if (isNumber(value)) {
            auto num = new NumberLiteral();
            num->value = toNumber(value);
            initial_value = num;
          } 
        }
      }
      return initial_value->interpret();
    }
  }
  if (array_method == "push") {
    array_method = "";
    for (auto& arg:arguments) {
      values.push_back(arg);
    }
    return arguments.size();
  }
  info("returning array itself...");
    
  return this;
};
any ObjectLiteral::interpret() { return this; };
any FunctionExpression::interpret() {
    // 모든 node는 new로 할당되었으므로 메모리 해제도 interpret이 후 이뤄져야한다.
  try {
    for (auto& node: block) {
      if (dynamic_cast<Declare*>(node)) info("Declare found");
      if (dynamic_cast<ExpressionStatement*>(node)) info("ExpressionStatement found");
      if (dynamic_cast<Return*>(node)) info("return statement found");
  
      node->interpret();
    }
  } catch(ReturnException return_exception) {
    throw return_exception;
  }
  return 1;
};
any Method::interpret() {
  // 메서드의 호출은 Call이 담당한다
  // 메서드의 역할은 this_ptr등 정보를 기억하는 것 뿐이다
  return 1;
}