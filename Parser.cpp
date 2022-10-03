#include <vector>
#include <iostream>
#include <string>
#include "Token.h"
#include "Node.h"
#include "Datatype.h"

using std::vector;
using std::cout;
using std::endl;
using std::stod;
// Scanner.cpp의 scan함수는 Token list를 반환한다.
// parse함수는 그렇게 생성된 Token list를 EndOfToken을 마주칠 때 까지 순회한다.
// parse함수의 반환 값은 구문 트리의 루트 노드이다. 구문 트리는 프로그램을 실행할 순서가 표현되어 있다.

// static으로 선언해두고, parse함수에서 초기화한다.
static vector<Token>::iterator current;

// auto는 자명한 타입추론 뿐 아니라 함수 줄 맞춤에도 큰 효과가 있는 것 같다. 엄청 깔끔해보인다 :)
static auto skipCurrent(Kind)->void;
static auto skipCurrentIf(Kind)->bool;
static auto parseAssignment()->Expression*;
static auto parseOr()->Expression*;
static auto parseAnd()->Expression*;
static auto parseRelational()->Expression*;
static auto parseAddOrSubtract()->Expression*;
static auto parseMultiplicationOrDivide()->Expression*;
static auto parseElementAccessOrFunctionCall()->Expression*;
static auto parseVariableOrLiterals()->Expression*;
static auto parseArrayLiteralOrObjectLiteral()->Expression*;
static auto parseExpression()->Expression*;
static auto parseExpressionStatement()->ExpressionStatement*;
static auto parseVar()->Variable*;
static auto parseConst()->Variable*;
static auto parseLet()->Variable*;
static auto parseParameters()->vector<string>;
static auto parseBlock()->vector<Statement*>;
static auto parseFunction()->Function*;
static auto getVariableNameAndParseExpression(Variable*)->void;

// 이 함수는 current iterator를 전진시키기 위한 것으로
// 인자가 필요한 이유는 현재 논리상 마땅한 Token의 Kind가 일치하는지를 검증해야하기 때문이다.
// 만약 Token의 Kind가 예상했던 것과 다르다면 bjs스크립트가 잘못 작성되었거나 컴파일러 구현 자체에 결함이 있는 것이므로 구문트리는 올바르게 생성될 수 없다.
// 결론적으로 iterator를 전진시킴과 동시에 올바른 구현인지를 검증하는 함수다.
void skipCurrent(Kind kind) {
  if (current->kind != kind) {
    cout << "current: " << current->code << " " << "kind: " << toString(kind) << " Token kind not matches"; 
    exit(1);
  }
  ++current;
}

bool skipCurrentIf(Kind kind) {
  if (current->kind != kind) {
    return false;
  }
  ++current;
  return true;
}

Program* parse(vector<Token> tokens) {
  auto program = new Program();
  current = tokens.begin();

  while (current->kind != Kind::EndOfToken) {
    switch (current->kind) {
      case Kind::Function:
        // static으로 선언된 current를 참조하므로 parseFunction에는 인자가 필요하지 않다.
        // 이런 접근 방식을 맘에 들어하지 않을 수도 있겠지만 편리한건 사실이다. 다만 함수형적인 측면으로 봤을 땐 참 그렇다.
        program->functions.push_back(parseFunction());
        break;
      default:
        cout << current->code << " " << toString(current->kind) << endl;
        cout << "Cannot parse syntax tree: invalid kind of Token detected" << endl;
        exit(1);
    }    
  }
  return program;
}

// function이라는 키워드 자체는 아무 효용이 없다. 단지 함수를 선언하겠다는 것을 알리기 위해 사용된다.
// 그러므로 function 토큰은 버린다. 여기서 버린다는 것은 current를 한칸 전진시키는 것을 의미한다.
// 다만 function키워드를 통해 함수 노드를 생성해야 함을 알게 되었다(구문 트리에 넣어야하니까). 그러므로 new Function한다.
Function* parseFunction() {
    auto function = new Function();
    skipCurrent(Kind::Function);
    // 생성한 함수 노드에 이름을 붙여준다. function 키워드 이후에 온 함수 이름이다.
    function->name = current->code;
    // 함수건 변수건 이름은 식별자이다. 식별자로 판단되지 않았으면 에러이므로 skipCurrent함수로 검사해준다.
    skipCurrent(Kind::Identifier);
    // 함수의 식별자 다음엔 매개 변수들이 위치할 것이다. 매개 변수는 좌괄호로 시작하므로
    // 좌괄호를 생략한 후, 우괄호를 만나기 전까지 쉼표를 생략하며 수집하면 될 것이다.
    // 리마인드하자면, 빈 칸을 검사하지 않은 이유는 scan할때 이미 공백을 모두 생략한 토큰 리스트를 만들기 때문이다.
    skipCurrent(Kind::LeftParen);
    function->parameters = parseParameters();
    skipCurrent(Kind::RightParen);

    // 다음은 함수의 본문이다
    // 함수의 본문은 중괄호로 시작한다. 본문 내용에 도달하기 위해 생략한다.
    skipCurrent(Kind::LeftBrace);
    function->block = parseBlock();
    skipCurrent(Kind::RightBrace);

    return function;
  }

vector<string> parseParameters() {
  vector<string> parameters;

  if (current->kind != Kind::RightParen) {
      // do-while을 사용하는 이유는 do를 통해 무조건 1번 이상의 실행을 보장받아야하기 때문이다. while을 썼다면 내용은 전혀 실행되지 못하고 넘어갔을 것이다.
      do {
        parameters.push_back(current->code);
        skipCurrent(Kind::Identifier);
        // 1. 식별자 하나를 parameters에 넣는다
        // 2. 다음 토큰이 Comma라면 그 다음 param이 존재한다고 확신할 수 있다
        // 3. 그러므로 skipCurrentIf는 true를 반환하고 루프를 끝내지 않는다
        // 4. 다음 토큰이 Comma가 아니라면 params를 모두 수집했다고 볼 수 있다.
        // 5. 그러므로 skipCurrentIf는 false를 반환하고 루프를 끝낸다.
      } while(skipCurrentIf(Kind::Comma));
  }
  
  return parameters;
}

vector<Statement*> parseBlock() {
  vector<Statement*> block;
  while (current->kind != Kind::RightBrace) {
    // 함수 블록안에 존재해서는 안되는 토큰들 검증
    switch (current->kind) {
      case Kind::Function:
        block.push_back(parseFunction());
        break;
      // 일단은 var 키워드로 변수를 선언하는 처리만 되어있다.
      // 향후 let과 const도 처리하려면 이곳에서 처리를 추가하면 된다.
      // 그때 Node.h에서 Variable에 kind를 추가할지 아니면 const와 let에 해당하는 node를 따로 만들지는 고민해보자
      case Kind::Variable:
        block.push_back(parseVar());
        break;
      case Kind::Constant:
        block.push_back(parseConst());
        break;
      case Kind::Let:
        block.push_back(parseLet());
        break;
      case Kind::EndOfToken: {
        cout << "EndOfToken kind is not allowed to use in function block. there must be some bad implementation in compiler";
        exit(1);
      }
      default:
        block.push_back(parseExpressionStatement());
    }
  }
  return block;
}


auto getVariableNameAndParseExpression(Variable* variable)->void {
  variable->name = current->code;
  skipCurrent(Kind::Identifier);
  skipCurrent(Kind::Assignment);// 상술한대로 토큰 자체는 생략된다.
  // 사실 이는 var과 let에 한해서 js스펙에 걸맞지 않은데, const는 항상 즉시 초기화가 필요한 반면 var과 let은 초기화를 미룰 수 있기 때문이다.
  // 그러므로 Assignment를 즉시 검증하는 것은 올바르지 않고, const의 경우에만 위의 구현이 올바르다.
  // var이나 let이라면 즉시 초기화하는 경우와 미루는 경우를 둘다 판단해야하고, 미뤘다고 판단하면 expression은 undefined를 할당해야하는데 그냥 null로 퉁치겠다.
  variable->expression = parseExpression();
  skipCurrent(Kind::Semicolon);// 이것도 js스펙과는 맞지 않는다. js는 semicolon없이도 구문 트리를 생성할 수 있기 때문이다. 
  // 다만 세미콜론 없이도 줄바꿈으로 초기화 종료를 판단하는 것은 간단한데, 둘다 검증하면 되기 때문이다. 
  // 그렇게 하려면 skipCurrent 함수의 인자는 vector<Kind>가 되어 여러 요소를 순회하며 검증하는 식이 되어야 할것이다. 템플릿 특수화를 하던가..
  // 일단은 간단한 구현으로 완성하는게 먼저이니 굳이 구현하지 않겠다. 
}

Variable* parseVar() {
  auto var = new Variable("var");
  skipCurrent(Kind::Variable);
  getVariableNameAndParseExpression(var);

  return var;
}

Variable* parseConst() {
  auto constant = new Variable("const");
  skipCurrent(Kind::Constant);
  getVariableNameAndParseExpression(constant);

  return constant;
}

Variable* parseLet() {
  auto let = new Variable("let");
  skipCurrent(Kind::Let);
  getVariableNameAndParseExpression(let);

  return let;
}

// 구문 분석의 다른 부분들에서 토큰의 종류에 따라 분석하던 것과는 달리,
// 식은 아무런 토큰이 없다면 식이다. if, for, function등 문의 시작을 알리는 요소가 존재하지 않기 때문이다.
// 혼동을 방지하자면, "BJS"나 숫자 1 등 연산자 없는 rvalue들도 모두 식의 범주에 포함된다.
// 아무튼 아무 요소가 없으면 식임을 표현하기 위해 parseBlock함수 switch문의 default는 expression이다.
/**
 * @brief 식의 연산 순서에 대하여
 * 구문 분석의 중요한 규칙이 있다. 연산자의 우선 순위에 대한 것이다.
 * 1*2 + 3*4 라는 식을 생각해보자. 당연히 곱하기 연산이 우선순위이므로 2 + 12가 되는 것을 알 수 있다.
 * 그러나 구문분석을 할 때는 우선 순위가 낮은 연산부터 트리를 구성해나가야 올바른 연산 결과를 얻을 수 있다.
 * 위의 예를 그대로 생각해보면, + 연산은 루트노드가 되고 * 연산이 브랜치, 그리고 1,2와 3,4가 리프노드가 되어야 하는것이다.
 * 단순 값도 식의 범주임을 떠올려보면 된다. 아무튼 루트노드부터 순서를 따져보면 일반적으로 알고 있는 연산순서의 역순이 되는것이다.
 * 그리고 결과물은 리프노드로부터 루트노드로 향하여 완성되게 된다. 
 */
/**
 * @brief 연산자의 우선 순위
 * 1. 산술연산자 곱하기와 나누기 - 좌결합
 * 2. 산술연산자 덧셈과 뺄셈 - 우결합
 * 3. 관계 연산자 - 좌결합
 * 4. 논리 and 연산자 - 좌결합
 * 5. 논리 or 연산자 - 좌결합
 * 6. 대입연산자 - 우결합
 * 
 * 하지만 현재 구문 분석을 위해선 낮은 우선순위의 연산자부터 분석해야하므로 우선순위는 역순이 된다.
 */
Expression* parseExpression() {
  return parseAssignment();
}

// 아래 함수는 단순히 parseExpression의 결과를 Statement로 감싸 반환하는 역할을 한다.
// 이런 함수가 왜 필요할까? Node.h의 ExpressionStatement에도 설명해 두었지만,
// 모든 식이 소비되는 것은 아니다.
// 함수 본문에서 소비되지 않는 식을 발견하면 그 식은 소비되기 위하여 임의로 문에 감싸져야하므로
// 소비되지 않는, 그러니까 문에 포함되지 않는 식을 발견했을때 문으로 감싸기 위한 함수라는 것이다.
// 문으로 감싸야 하는 이유는 프로그램이 '구문'을 소비하기 때문이다. Statement타입이 아니면 소비할 수 없다.
ExpressionStatement* parseExpressionStatement() {
  cout << "parse expression statement: " << current->code << endl;;
  auto expressionStatement = new ExpressionStatement();
  expressionStatement->expression = parseExpression();
  cout << "parse complete: " << current->code << endl;
  skipCurrent(Kind::Semicolon);

  return expressionStatement;
}

Expression* parseAssignment() {
  // 변수이름을 단순히 parsed로 정한 이유는 반환받은 객체가 정확히 어떤 객체인지 알 수 없기 때문이다.
  // 동작이 혼동될 수 있는데, 할당이 우결합 연산이라고 해서 아래의 parseOr가 우항부터 트리를 만든다는 것이 아니다.
  // 좌항으로부터 전진한 것이며, 만약 정말로 할당 연산이었다면 parseOr가 반환된 시점에 parsed는 무조건 변수 참조이거나 원소참조이다(아니라면 오류이다).
  // 참고로 좌항이 var a, const b, let c 따위일 수는 없는데, parseBlock에서 이미 iterator를 전진시켰을 것이기 때문이다.
  // 만약 이해가 가지 않는다면 parseAssignment는 단지 parseExpression을 위해 존재하며, 연산자 우선순위가 최상단이기 때문에 parseExpression == parseAssignment인 것이고
  // 의미 전달을 위해 parseExpression으로 이름만 가지고 있다는 것을 기억하면 된다.
  auto parsed = parseOr(); 
  // Or과 다르게 if로 검증하는 이유는 Assignment가 우결합연산이기 때문이다. 
  // a = b = 3 이라는 식을 떠올려보자. a와 b는 모두 3이 되어야 하고, 그러려면 b = 3이 먼저 평가된 후 a = b가 평가되어야 한다.
  // 그렇다면 조금 더 길게 a = b = ... z = 3이라고 생각해보자. 결국 평가되어야 하는 것은 3이다.
  // a = b를 평가하던 도중 assignment연산이 더 존재한다면 b = c를 평가하고 그 와중에 c = d를 평가되어야하고.. 결국 바닥인 3을 찍고 돌아와야한다는 것이다.
  // 이런 식으로 조건에 따라 바닥부터 평가가 되돌아오게 하기 위하여 재귀호출하여 처리한다.
  if (current->kind != Kind::Assignment) 
    return parsed; // 할당연산이 아니었으므로 parsed가 무엇이던 간에 그냥 반환하면 된다.
  skipCurrent(Kind::Assignment);

  if (auto get_variable = dynamic_cast<GetVariable*>(parsed)) {
    auto variable_setter = new SetVariable();
    variable_setter->name = get_variable->name;
    variable_setter->value = parseAssignment();
    return variable_setter;
  }

  if (auto get_element = dynamic_cast<GetElement*>(parsed)) {
    auto element_setter = new SetElement();
    element_setter->sub = get_element->sub;
    element_setter->index = get_element->index;
    element_setter->value = parseAssignment();
    return element_setter;
  }
  // 원소참조도 아니고, 변수도 아니라면 불가능한 연산임
  // 그러므로, a = (3 > 4) = 6; 과 같이 적절하지 않은 연산은 이곳에서 걸러짐
  cout << "invalid assignment attempt detected";
  exit(1);
}

Expression* parseOr() {
  auto parsed = parseAnd();
  // LogicalOr(||) 조건은 연속으로 이어 쓰는게 가능하므로 while을 통해 LogicalOr가 존재하지 않을 때 까지 연속으로 실행한다.
  while (skipCurrentIf(Kind::LogicalOr)) {
    // Or가 연속된다면, Or의 우변에 n번째 Or들이 포함된다는 것이다(좌결합 연산이므로)
    // 그러므로 Or가 nest될 수 있도록 while안에서 처리해준다
    auto temp_or = new Or();
    // 첫 루프에서 result가 lhs가 될 수 있는 이유는 애초에 current가 LogicalOr가 아니었다면 while문에 진입조차 하지 못했을 것이기 때문이다.
    temp_or->lhs = parsed;
    // parseAnd인 이유는 parseOr함수 첫 줄과 마찬가지로 토큰이 LogicalOr전까지만 진행되고 멈춰야하기 때문이다
    temp_or->rhs = parseAnd();
    parsed = temp_or;
  }
  // 즉, 함수 첫줄의 parseAnd의 result가 실행된 후 current의 위치가 ||가 아니었다면 Or객체가 아닌 다른 상위의 Expression객체였을 것이다.
  return parsed;
}

auto parseAnd()->Expression* {
  auto parsed = parseRelational();
  return parsed;
}

auto parseRelational()->Expression* {
  auto parsed = parseAddOrSubtract();
  return parsed;
}

auto parseAddOrSubtract()->Expression* {
  auto parsed = parseMultiplicationOrDivide();
  return parsed;
}

auto parseMultiplicationOrDivide()->Expression* {
  auto parsed = parseElementAccessOrFunctionCall();
  
  auto kind = current->kind;
  if (kind != Kind::Multiply && kind != Kind::Divide)
    return parsed;
  
  if (kind == Kind::Multiply) {
    cout << "multiply: " << current->code << endl;
    auto multiply = new Arithmetic(Kind::Multiply);
    multiply->lhs = parsed;
    skipCurrent(Kind::Multiply);
    multiply->rhs = parseExpression();

    kind = current->kind;
    while (skipCurrentIf(Kind::Multiply) || skipCurrentIf(Kind::Divide)) {
      auto temp_multiply = multiply;
      if (kind == Kind::Multiply) {
        multiply = new Arithmetic(Kind::Multiply);
        multiply->lhs = temp_multiply;
        multiply->rhs = parseExpression();
      }
      if (kind == Kind::Divide) {
        multiply = new Arithmetic(Kind::Divide);
        multiply->lhs = temp_multiply;
        multiply->rhs = parseExpression();
      }
      kind = current->kind;
    }

    return multiply;
  }

  if (kind == Kind::Divide) {
    auto divide = new Arithmetic(Kind::Divide);
    divide->lhs = parsed;
    skipCurrent(Kind::Divide);
    divide->rhs = parseExpression();

    kind = current->kind;
    while (skipCurrentIf(Kind::Multiply) || skipCurrentIf(Kind::Divide)) {
      auto temp_divide = divide;
      if (kind == Kind::Multiply) {
        divide = new Arithmetic(Kind::Multiply);
        divide->lhs = temp_divide;
        divide->rhs = parseExpression();
      }
      if (kind == Kind::Divide) {
        divide = new Arithmetic(Kind::Divide);
        divide->lhs = temp_divide;
        divide->rhs = parseExpression();
      }
      kind = current->kind;
    }

    return divide;
  }
}


auto parseElementAccessOrFunctionCall()->Expression* {
  auto parsed = parseVariableOrLiterals();
  // 원소 접근이거나 함수호출이거나 메서드 호출이 아니라면 역할 종료
  auto kind = current->kind;
  if (kind != Kind::LeftBracket && kind != Kind::LeftParen && kind != Kind::Dot) 
    return parsed;
  
  // 원소 접근
  if (skipCurrentIf(Kind::LeftBracket)) {
    
  }
  // 함수 호출
  if (skipCurrentIf(Kind::LeftParen)) {

  }
  // 메서드 호출
  cout << "method: " << current->code << endl;
  skipCurrent(Kind::Dot);

  auto method = new Method();

  method->this_ptr = parsed;
  method->method = current->code;
  skipCurrent(Kind::Identifier);
  skipCurrent(Kind::LeftParen);

  while (current->kind != Kind::RightParen) {
    cout << "parsing method arguments: " << current->code << endl;
    method->arguments.push_back(parseExpression());
    skipCurrentIf(Kind::Comma);
  }
  skipCurrent(Kind::RightParen);
  
  // chained method를 검증
  while (skipCurrentIf(Kind::Dot)) {
    auto temp_method = method;
    method = new Method();
    method->this_ptr = method;
    method->method = current->code;
    skipCurrent(Kind::Identifier);
    skipCurrent(Kind::LeftParen);

    while (current->kind != Kind::RightParen) {
      method->arguments.push_back(parseExpression());
      skipCurrentIf(Kind::Comma);
    }
    skipCurrent(Kind::RightParen);  
  }
  cout << "method parsing complete: " << current->code << endl;
  return method;
}

// 주의: 이곳은 array literal과 object literal은 제외입니다.
// variable과 literals가 같은 우선순위로 평가되어야 하는 이유
// 예를 들어, [](원소접근) 혹은 ()(함수호출) 연산은 인자가 필요한데, 이 인자로 변수를 받는 것이 가능하다.
// const a = 1; arr[a] = 3; 따위 이다. arr또한 어가가에 선언된 변수이므로 먼저 평가되어야 할 것은 자명하다.
// 그러므로 변수, 문자, 숫자는 동일한 우선순위 평가대상이다
auto parseVariableOrLiterals()->Expression* {
  auto parsed = parseArrayLiteralOrObjectLiteral();
  if (parsed != nullptr) return parsed; 
  

  switch(current->kind) {
    case Kind::StringLiteral:{
      auto string_literal = new StringLiteral();
      string_literal->value = current->code;
      skipCurrent(Kind::StringLiteral);
      
      return string_literal;
    }

    case Kind::NumberLiteral:{
      cout << "number literal: " << current->code << endl;
      auto number_literal = new NumberLiteral();
      number_literal->value = stod(current->code);
      skipCurrent(Kind::NumberLiteral);
      
      return number_literal;
    }

    case Kind::Identifier:{
      // 이름은 variable이지만 정확히 말하면 identifier다
      // 미리 정의된 함수의 identifier일 수도 있기 때문에 변수라는 말이 어울리지 않을 수도 있지만
      // 함수도 무조건 값으로 취급하는 js의 특성과 잘 맞는 부분이기도 하다
      auto getter = new GetVariable();
      getter->name = current->code;
      skipCurrent(Kind::Identifier);
      
      return getter;
    }

    case Kind::TrueLiteral:{
      break;
    }
    case Kind::FalseLiteral:{
      break;
    }
    case Kind::NullLiteral:{
      break;
    }
    default:
      return parsed;
  }
}

// 최후순위 평가 대상: array literal과 object literal
// array나 object내의 값은 할당이 없으므로 parseAnd부터 재귀를 돈다
auto parseArrayLiteralOrObjectLiteral()->Expression* {
  if (skipCurrentIf(Kind::LeftBracket)) {
    auto array = new ArrayLiteral();
    
    while (current->kind != Kind::RightBracket) {
      auto parsed = parseAnd();
      array->values.push_back(parsed);
      skipCurrentIf(Kind::Comma);
    }

    skipCurrent(Kind::RightBracket);

    return array;
  }

  if (skipCurrentIf(Kind::LeftBrace)) {
    auto object = new ObjectLiteral();
    
    while (current->kind != Kind::RightBrace) {
      auto kind = current->kind;
      if (kind == Kind::NumberLiteral || kind == Kind::StringLiteral || kind == Kind::Identifier) {
        string key = current->code;
        skipCurrent(current->kind);
        skipCurrent(Kind::Colon);
        Expression* value = parseAnd();
        object->values.insert({ key, value });
      } else {
        cout << "Object key must be number or string" << endl;
        exit(1);
      }

      skipCurrentIf(Kind::Comma);
    }
    skipCurrent(Kind::RightBrace);

    return object;
  }

  return nullptr;
}