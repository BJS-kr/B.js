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
static LexicalEnvironment* current_block_scope = nullptr;
// auto는 자명한 타입추론 뿐 아니라 함수 줄 맞춤에도 큰 효과가 있는 것 같다. 엄청 깔끔해보인다 :)
static auto parseAssignment()                    ->Expression*;
static auto parseNot()                           ->Expression*;
static auto parseOr()                            ->Expression*;
static auto parseAnd()                           ->Expression*;
static auto parseRelational()                    ->Expression*;
static auto parseAddOrSubtract()                 ->Expression*;
static auto parseMultiplicationOrDivideOrModulo()->Expression*;
static auto parseElementAccessOrFunctionCall()   ->Expression*;
static auto parseVariableOrLiterals()            ->Expression*;
static auto parseArrayLiteralOrObjectLiteral()   ->Expression*; 
static auto parseExpression()                    ->Expression*;
static auto parseInnerExpression()               ->Expression*;
static auto parseEqual(Expression*)              ->Expression*;
static auto parseStrictEqual(Expression*)        ->Expression*;
static auto parseNotEqual(Expression*)           ->Expression*;
static auto parseStrictNotEqual(Expression*)     ->Expression*;
static auto parseGreaterOrEqual(Expression*)     ->Expression*;
static auto parseLessOrEqual(Expression*)        ->Expression*;
static auto parseGreaterThan(Expression* expr)   ->Expression*;
static auto parseLesserThan(Expression* expre)   ->Expression*;
static auto parseIncrementOrDecrement()          ->Expression*;
static auto parseIncrement(Expression* expr)     ->Expression*;
static auto parseDecrement(Expression* expr)     ->Expression*;
static auto parseGetElement(Expression* expr)    ->Expression*;
static auto parseCall(Expression* expr)          ->Expression*;
static auto parseMethod(Expression* expr)        ->Expression*;
static auto parseMultiply(Expression* expr)      ->Expression*;
static auto parseDivide(Expression* expr)        ->Expression*;
static auto parseModulo(Expression* expr)        ->Expression*;
static auto parseExpressionStatement()           ->ExpressionStatement*;
static auto parseDeclare(Declare*)               ->Declare*;
static auto parseFunction()                      ->DeclareFunction*;
static auto parseReturn()                        ->Return*;
static auto parseIf()                            ->Statement*;
static auto parseFor()                           ->Statement*;
static auto parseBlock()                         ->vector<Statement*>;
static auto parseParameters()                    ->vector<string>;
static auto skipCurrent(Kind)                    ->void;
static auto skipCurrentIf(Kind)                  ->bool;
// 이 함수는 current iterator를 전진시키기 위한 것으로
// 인자가 필요한 이유는 현재 논리상 마땅한 Token의 Kind가 일치하는지를 검증해야하기 때문이다.
// 만약 Token의 Kind가 예상했던 것과 다르다면 bjs스크립트가 잘못 작성되었거나 컴파일러 구현 자체에 결함이 있는 것이므로 구문트리는 올바르게 생성될 수 없다.
// 결론적으로 iterator를 전진시킴과 동시에 올바른 구현인지를 검증하는 함수다.
void skipCurrent(Kind kind) {
  if (current->kind != kind) {
    cout << "current: " << current->code << " " << "expected kind: " << toString(current->kind) << " " << "actual kind: " << toString(kind) << " Token kind not matches"; 
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
DeclareFunction* parseFunction() {
    auto function = new FunctionExpression();
    // 선언
    skipCurrent(Kind::Function);
    // 이름
    function->name = current->code;
    // 스코프 확정
    function->upper_scope = current_block_scope;
    current_block_scope = function;
    skipCurrent(Kind::Identifier);
    skipCurrent(Kind::LeftParen);
    // 인자
    function->parameters = parseParameters();
    skipCurrent(Kind::RightParen);
    skipCurrent(Kind::LeftBrace);
    // 본문
    function->block = parseBlock();
    skipCurrent(Kind::RightBrace);

    current_block_scope = function->upper_scope;

    return new DeclareFunction(function);
  }

vector<string> parseParameters() {
  vector<string> parameters;
  if (current->kind != Kind::RightParen) {
      // do-while을 사용하는 이유는 do를 통해 무조건 1번 이상의 실행을 보장받아야하기 때문이다. 
      // while을 썼다면 내용은 전혀 실행되지 못하고 넘어갔을 것이다.
      do {
        parameters.push_back(current->code);
        skipCurrent(Kind::Identifier);
      } while(skipCurrentIf(Kind::Comma));
  }
  
  return parameters;
}

vector<Expression*> parseArguments() {
  vector<Expression*> arguments;

  if (current->kind != Kind::RightParen) {
    do {
      arguments.push_back(parseNot());
    } while(skipCurrentIf(Kind::Comma));
  }

  return arguments;
}

Statement* parseIf() {
  auto if_ = new If();
  if_->upper_scope = current_block_scope;
  current_block_scope = if_;
  skipCurrent(Kind::If);
  if_->condition = parseInnerExpression();
  skipCurrent(Kind::LeftBrace);
  if_->block = parseBlock();
  skipCurrent(Kind::RightBrace);
  current_block_scope = if_->upper_scope;

  return if_;
}

Statement* parseFor() {
  cout << "parsing for statement..." << endl;
  auto for_ = new For();
  for_->upper_scope = current_block_scope;
  current_block_scope = for_;
  skipCurrent(Kind::For);
  skipCurrent(Kind::LeftParen);
  // for문에서 조건문 변수를 선언하는 부분이다. let만 가능하도록 고정
  // starting_point가 vector인 이유는 declare와 set variable을 두번 interpret해야하기 때문이다
  // 즉, 할당이 for의 본문인 block에 포함되지 않아야 한다.
  // 다만 for의 scope내 variables에는 할당되므로 연산이 가능하다
  skipCurrent(Kind::Let);
  for_->starting_point_name = current->code;
  for_->starting_point.push_back(parseDeclare(new Declare(Kind::Let)));
  // 이곳에서 맡은 역할은 for의 start_point변수의 할당이다.
  for_->starting_point.push_back(parseExpressionStatement());
  skipCurrent(Kind::Semicolon);
  for_->condition = parseRelational();
  skipCurrent(Kind::Semicolon);
  for_->expression = parseExpression();
  skipCurrent(Kind::RightParen);
  skipCurrent(Kind::LeftBrace);
  for_->block = parseBlock();
  skipCurrent(Kind::RightBrace);
  current_block_scope = for_->upper_scope;

  return for_;
}

vector<Statement*> parseBlock() {
  vector<Statement*> block;
  while (current->kind != Kind::RightBrace) {
    // 함수 블록안에 존재해서는 안되는 토큰들 검증
    switch (current->kind) {
      case Kind::Function:
        block.push_back(parseFunction());
        break;
      case Kind::If:
        block.push_back(parseIf());
        break;
      case Kind::For:
        block.push_back(parseFor());
        break;
      case Kind::Variable:
        skipCurrent(Kind::Variable);
        cout << "parsing var variable" << endl;
        block.push_back(parseDeclare(new Declare(Kind::Variable)));
        break;
      case Kind::Constant:
        skipCurrent(Kind::Constant);
        cout << "parsing const variable" << endl;
        block.push_back(parseDeclare(new Declare(Kind::Constant)));
        break;
      case Kind::Let:
        skipCurrent(Kind::Let);
        cout << "parsing let variable" << endl;
        block.push_back(parseDeclare(new Declare(Kind::Let)));
        break;
      case Kind::Return:
        block.push_back(parseReturn());
        break;
      case Kind::Break:
        skipCurrent(Kind::Break);
        block.push_back(new Break());
        skipCurrent(Kind::Semicolon);
        break;
      case Kind::Continue:
        skipCurrent(Kind::Continue);
        block.push_back(new Continue());
        skipCurrent(Kind::Semicolon);
        break;
      case Kind::EndOfToken: {
        cout << "EndOfToken kind is not allowed to use in function block. there must be some bad implementation in compiler";
        exit(1);
      }
      default:
        block.push_back(parseExpressionStatement());
        skipCurrent(Kind::Semicolon);
    }
  }

  return block;
}

auto parseReturn()->Return* {
  skipCurrent(Kind::Return);
  return new Return(parseNot());
}


Declare* parseDeclare(Declare* declare) {
  cout << "parsing declared variable: " << current->code << endl;
  declare->lexical_environment = current_block_scope;
  cout << "current lexical environment: " << current_block_scope << endl;
  if (current->kind != Kind::Identifier) {
    cout << "must provide identifier when declare";
    exit(1);
  };
  declare->name = current->code;
  // !!매우 중요!! code를 할당하고 iterator를 전진시키지 않는 이유
  // 이렇게 하면 iterator가 선언된 변수의 식별자에 머물게 된다
  // 그렇다면 다음 루프의 파싱에 identifier경로로 파싱이 진입하게 되고
  // 자동으로 변수가 할당된다
  return declare;
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

Expression* parseInnerExpression() {
  skipCurrent(Kind::LeftParen);
  auto expression = parseExpression();
  skipCurrent(Kind::RightParen);

  return expression;
}

// 아래 함수는 단순히 parseExpression의 결과를 Statement로 감싸 반환하는 역할을 한다.
// 이런 함수가 왜 필요할까? Node.h의 ExpressionStatement에도 설명해 두었지만,
// 모든 식이 소비되는 것은 아니다.
// 함수 본문에서 소비되지 않는 식을 발견하면 그 식은 소비되기 위하여 임의로 문에 감싸져야하므로
// 소비되지 않는, 그러니까 문에 포함되지 않는 식을 발견했을때 문으로 감싸기 위한 함수라는 것이다.
// 문으로 감싸야 하는 이유는 프로그램이 '구문'을 소비하기 때문이다. Statement타입이 아니면 소비할 수 없다.
ExpressionStatement* parseExpressionStatement() {
  cout << "parse expression statement: " << current->code << endl;
  auto expressionStatement = new ExpressionStatement();
  expressionStatement->expression = parseExpression();
  cout << "parse complete: " << current->code << endl;

  return expressionStatement;
}

Expression* parseAssignment() {
  // 변수이름을 단순히 parsed로 정한 이유는 반환받은 객체가 정확히 어떤 객체인지 알 수 없기 때문이다.
  // 동작이 혼동될 수 있는데, 할당이 우결합 연산이라고 해서 아래의 parseOr가 우항부터 트리를 만든다는 것이 아니다.
  // 좌항으로부터 전진한 것이며, 만약 정말로 할당 연산이었다면 parseOr가 반환된 시점에 parsed는 무조건 변수 참조이거나 원소참조이다(아니라면 오류이다).
  // 참고로 좌항이 var a, const b, let c 따위일 수는 없는데, parseBlock에서 이미 iterator를 전진시켰을 것이기 때문이다.
  // 만약 이해가 가지 않는다면 parseAssignment는 단지 parseExpression을 위해 존재하며, 연산자 우선순위가 최상단이기 때문에 parseExpression == parseAssignment인 것이고
  // 의미 전달을 위해 parseExpression으로 이름만 가지고 있다는 것을 기억하면 된다.
  auto parsed = parseNot(); 
  // Or과 다르게 if로 검증하는 이유는 Assignment가 우결합연산이기 때문이다. 
  // a = b = 3 이라는 식을 떠올려보자. a와 b는 모두 3이 되어야 하고, 그러려면 b = 3이 먼저 평가된 후 a = b가 평가되어야 한다.
  // 그렇다면 조금 더 길게 a = b = ... z = 3이라고 생각해보자. 결국 평가되어야 하는 것은 3이다.
  // a = b를 평가하던 도중 assignment연산이 더 존재한다면 b = c를 평가하고 그 와중에 c = d를 평가되어야하고.. 결국 바닥인 3을 찍고 돌아와야한다는 것이다.
  // 이런 식으로 조건에 따라 바닥부터 평가가 되돌아오게 하기 위하여 재귀호출하여 처리한다
  // 할당연산이 아니었으므로 parsed가 무엇이던 간에 그냥 반환하면 된다.
  if (skipCurrentIf(Kind::Assignment)) {
    if (auto get_variable = dynamic_cast<GetVariable*>(parsed)) {
      cout << "allocating value to: " << get_variable->name << endl;
      auto variable_setter = new SetVariable();

      variable_setter->name = get_variable->name;
      variable_setter->lexical_environment = get_variable->lexical_environment;
      variable_setter->value = parseAssignment();

      return variable_setter; 
    }

    if (auto get_element = dynamic_cast<GetElement*>(parsed)) {
      cout << "set element found" << endl;
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
  if (skipCurrentIf(Kind::AddAssignment)) {
    cout << "parsing add assignment..." << endl;
    if (auto get_variable = dynamic_cast<GetVariable*>(parsed)) {
      cout << "add assign to: " << get_variable->name << endl;
      auto variable_setter = new SetVariable();
      variable_setter->name = get_variable->name;
      variable_setter->lexical_environment = get_variable->lexical_environment;
      
      auto add = new Arithmetic(Kind::Add);
      add->lhs = get_variable;
      add->rhs = parseAssignment();
      
      variable_setter->value = add;

      return variable_setter;
    }
  }
  return parsed;
}

Expression* parseNot() {
  auto parsed = parseOr();
  if (skipCurrentIf(Kind::Not)) {
    cout << "parsing Not" << endl;
    return new Not(parseExpression());
  }

  return parsed;
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

// &&
auto parseAnd()->Expression* {
  auto parsed = parseRelational();

  while (skipCurrentIf(Kind::LogicalAnd)) {
    auto temp_and = new And();
    temp_and->lhs = parsed;
    temp_and->rhs = parseRelational();
    parsed = temp_and;
  }

  return parsed;
}

// == === != !== > < >= <=
auto parseRelational()->Expression* {
  auto parsed = parseAddOrSubtract();
  auto kind = current->kind;

  if (kind == Kind::Equal)          return parseEqual(parsed);
  if (kind == Kind::StrictEqual)    return parseStrictEqual(parsed);
  if (kind == Kind::NotEqual)       return parseNotEqual(parsed);
  if (kind == Kind::StrictNotEqual) return parseStrictNotEqual(parsed);
  if (kind == Kind::GreaterThan)    return parseGreaterThan(parsed);
  if (kind == Kind::LesserThan)     return parseLesserThan(parsed);
  if (kind == Kind::GreaterOrEqual) return parseGreaterOrEqual(parsed);
  if (kind == Kind::LesserOrEqual)  return parseLessOrEqual(parsed);
    
  return parsed;
}

auto parseEqual(Expression* expr)->Expression* {
  cout << "parsing equal..." << endl;
  skipCurrent(Kind::Equal);
  auto equal = new Relational(Kind::Equal);
  equal->lhs = expr;
  equal->rhs = parseExpression();
  return equal;
} 

auto parseStrictEqual(Expression* expr)->Expression* {
  cout << "parsing strict equal..." << endl;
  skipCurrent(Kind::StrictEqual);
  auto strict_eqaul = new Relational(Kind::StrictEqual);
  strict_eqaul->lhs = expr;
  strict_eqaul->rhs = parseExpression();
  return strict_eqaul;
}

auto parseNotEqual(Expression* expr)->Expression* {
  cout << "parsing not equal..." << endl;
  skipCurrent(Kind::NotEqual);
  auto not_equal = new Relational(Kind::NotEqual);
  not_equal->lhs = expr;
  not_equal->rhs = parseExpression();
  return not_equal;
}

auto parseStrictNotEqual(Expression* expr)->Expression* {
  cout << "parsing strict not equal..." << endl;
  skipCurrent(Kind::StrictNotEqual);
  auto strict_not_equal = new Relational(Kind::StrictNotEqual);
  strict_not_equal->lhs = expr;
  strict_not_equal->rhs = parseExpression();
  return strict_not_equal;
}

auto parseGreaterThan(Expression* expr)->Expression* {
  cout << "parsing greater than..." << endl;
  skipCurrent(Kind::GreaterThan);
  auto greater_than = new Relational(Kind::GreaterThan);
  greater_than->lhs = expr;
  greater_than->rhs = parseExpression();
  return greater_than;
}

auto parseLesserThan(Expression* expr)->Expression* {
  cout << "parsing lesser than..." << endl;
  skipCurrent(Kind::LesserThan);
  auto lesser_than = new Relational(Kind::LesserThan);
  lesser_than->lhs = expr;
  lesser_than->rhs = parseExpression();
  return lesser_than;
}

auto parseGreaterOrEqual(Expression* expr)->Expression* {
  cout << "parsing greater or equal" << endl;
  skipCurrent(Kind::GreaterOrEqual);
  auto greater_equal = new Relational(Kind::GreaterOrEqual);
  greater_equal->lhs = expr;
  greater_equal->rhs = parseExpression();
  return greater_equal;
}

auto parseLessOrEqual(Expression* expr)->Expression* {
  cout << "parsing less or equal" << endl;
  skipCurrent(Kind::LesserOrEqual);
  auto less_equal = new Relational(Kind::LesserOrEqual);
  less_equal->lhs = expr;
  less_equal->rhs = parseExpression();
  return less_equal;
}

// + -
auto parseAddOrSubtract()->Expression* {
  auto parsed = parseMultiplicationOrDivideOrModulo();
  auto kind = current->kind;
  if (kind != Kind::Add && kind != Kind::Subtract) 
    return parsed;

  if (skipCurrentIf(Kind::Add)) {
    cout << "add: +" << endl;
    
    auto add = new Arithmetic(Kind::Add);
    add->lhs = parsed;
    add->rhs = parseMultiplicationOrDivideOrModulo(); 

    kind = current->kind;
    while (skipCurrentIf(Kind::Add) || skipCurrentIf(Kind::Subtract)) {
      if (kind == Kind::Add) {
        cout << "add: +" << endl;
        auto temp_add = add;
        add = new Arithmetic(Kind::Add);
        add->lhs = temp_add;
        add->rhs = parseMultiplicationOrDivideOrModulo();
      } else {
        cout <<"subtract: -" << endl;
        auto temp_add = add;
        add = new Arithmetic(Kind::Subtract);
        add->lhs = temp_add;
        add->rhs = parseMultiplicationOrDivideOrModulo();
      }
      kind = current->kind;
    } 

    return add;   
  }
  if (skipCurrentIf(Kind::Subtract)) {
    cout << "subtract: -" << endl;
    
    auto subtract = new Arithmetic(Kind::Subtract);
    subtract->lhs = parsed;
    subtract->rhs = parseMultiplicationOrDivideOrModulo(); 

    kind = current->kind;
    while (skipCurrentIf(Kind::Add) || skipCurrentIf(Kind::Subtract)) {
      if (kind == Kind::Add) {
        cout << "add: +" << endl;
        auto temp_subtract = subtract;
        subtract = new Arithmetic(Kind::Add);
        subtract->lhs = temp_subtract;
        subtract->rhs = parseMultiplicationOrDivideOrModulo();
      } else {
        cout << "subtract: -" << endl;
        auto temp_subtract = subtract;
        subtract = new Arithmetic(Kind::Subtract);
        subtract->lhs = temp_subtract;
        subtract->rhs = parseMultiplicationOrDivideOrModulo();
      }
      kind = current->kind;
    } 

    return subtract;   
  }
}

// * /
auto parseMultiplicationOrDivideOrModulo()->Expression* {
  auto parsed = parseIncrementOrDecrement();
  
  if (skipCurrentIf(Kind::Multiply)) return parseMultiply(parsed);
  if (skipCurrentIf(Kind::Divide))   return parseDivide(parsed);
  if (skipCurrentIf(Kind::Modulo))   return parseModulo(parsed);

  return parsed;
}

auto parseMultiply(Expression* expr)->Expression* {
  cout << "multiply: " << current->code << endl;
  auto multiply = new Arithmetic(Kind::Multiply);
  multiply->lhs = expr;
  multiply->rhs = parseIncrementOrDecrement();

  if (skipCurrentIf(Kind::Multiply)) return parseMultiply(multiply);
  if (skipCurrentIf(Kind::Divide))   return parseDivide(multiply);
  if (skipCurrentIf(Kind::Modulo))   return parseModulo(multiply);

  return multiply;
}
auto parseDivide(Expression* expr)->Expression* {
  auto divide = new Arithmetic(Kind::Divide);
  divide->lhs = expr;
  divide->rhs = parseIncrementOrDecrement();
  
  if (skipCurrentIf(Kind::Multiply)) return parseMultiply(divide);
  if (skipCurrentIf(Kind::Divide))   return parseDivide(divide);
  if (skipCurrentIf(Kind::Modulo))   return parseModulo(divide);
  
  return divide;
}
auto parseModulo(Expression* expr)->Expression* {
  auto modulo = new Arithmetic(Kind::Modulo);
  modulo->lhs = expr;
  modulo->rhs = parseElementAccessOrFunctionCall();
  
  if (skipCurrentIf(Kind::Multiply)) return parseMultiply(modulo);
  if (skipCurrentIf(Kind::Divide))   return parseDivide(modulo);
  if (skipCurrentIf(Kind::Modulo))   return parseModulo(modulo);
  
  return modulo;
}


auto parseIncrementOrDecrement()->Expression* {
  auto parsed = parseElementAccessOrFunctionCall();
  if (current->kind == Kind::Increment) return parseIncrement(parsed);
  if (current->kind == Kind::Decrement) return parseDecrement(parsed);

  return parsed;
}

auto parseIncrement(Expression* expr)->Expression* {
  cout << "parsing increment..." << endl;
  auto increment = new Unary(Kind::Increment);
  // expr이 GetVariable이었다는 것은 increment operator가 identifier의 suffix라는 것이다.
  if (auto get_variable = dynamic_cast<GetVariable*>(expr)) {
    increment->sub = get_variable;
    increment->lexical_environment = current_block_scope;
    skipCurrent(Kind::Increment);
    return increment;
  }
}

auto parseDecrement(Expression* expr)->Expression* {
  cout << "parsing decrement..." << endl;
  auto decrement = new Unary(Kind::Decrement);
  if (auto get_variable = dynamic_cast<GetVariable*>(expr)) {
    decrement->sub = get_variable;
    decrement->lexical_environment = current_block_scope;
    skipCurrent(Kind::Decrement);
    return decrement;
  }
}

// [] ()
auto parseElementAccessOrFunctionCall()->Expression* {
  auto parsed = parseVariableOrLiterals();

  if (skipCurrentIf(Kind::LeftBracket)) return parseGetElement(parsed);
  if (skipCurrentIf(Kind::LeftParen))   return parseCall(parsed);
  if (skipCurrentIf(Kind::Dot))         return parseMethod(parsed);
  
  return parsed;
}

auto parseGetElement(Expression* expr)->Expression* {
    cout << "parsing GetElement..." << endl;
    auto getter = new GetElement();
    getter->sub = expr;
    getter->index = parseNot();
    skipCurrent(Kind::RightBracket);
    
    if (skipCurrentIf(Kind::LeftBracket)) return parseGetElement(getter);
    if (skipCurrentIf(Kind::LeftParen))   return parseCall(getter);
    if (skipCurrentIf(Kind::Dot))         return parseMethod(getter);
    
    return getter;
}

auto parseCall(Expression* expr)->Expression* {
    cout << "parsing Call..." << endl;
    auto call = new Call();
    call->sub = expr;
    call->arguments = parseArguments();
    skipCurrent(Kind::RightParen);

    if (skipCurrentIf(Kind::LeftBracket)) return parseGetElement(call);
    if (skipCurrentIf(Kind::LeftParen))   return parseCall(call);
    if (skipCurrentIf(Kind::Dot))         return parseMethod(call);
    
    return call;
}

auto parseMethod(Expression* expr)->Expression* {
    cout << "accessing method: " << current->code << endl;

    auto method = new Method();

    method->this_ptr = expr;
    method->method = current->code;
    cout << "method name: " << current->code << endl;
    skipCurrent(Kind::Identifier);
    
    cout << "method parsing complete: " << current->code << endl;
    if (skipCurrentIf(Kind::LeftBracket)) return parseGetElement(method);
    if (skipCurrentIf(Kind::LeftParen))   return parseCall(method);
    if (skipCurrentIf(Kind::Dot))         return parseMethod(method);
    
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
      cout << "string literal: " << current->code << endl;
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
      cout << "parsing identifier: " << current->code << endl;
      // 이름은 variable이지만 정확히 말하면 identifier다
      // 미리 정의된 함수의 identifier일 수도 있기 때문에 변수라는 말이 어울리지 않을 수도 있지만
      // 함수도 무조건 값으로 취급하는 js의 특성과 잘 맞는 부분이기도 하다
      auto getter = new GetVariable();
      getter->name = current->code;
      getter->lexical_environment = current_block_scope;
      skipCurrent(Kind::Identifier);
      
      return getter;
    }

    case Kind::TrueLiteral:{
      skipCurrent(Kind::TrueLiteral);
      return new BooleanLiteral(true);
    }
    case Kind::FalseLiteral:{
      skipCurrent(Kind::FalseLiteral);
      return new BooleanLiteral(false);
    }
    case Kind::NullLiteral:{
      skipCurrent(Kind::NullLiteral);
      return new NullLiteral();
    }
    case Kind::UndefinedLiteral:{
      skipCurrent(Kind::UndefinedLiteral);
      return new Undefined();
    }
    // 이 곳의 function을 parse_block의 function과 착각하면 안된다.
    // parse_block: Statement, 이 곳: Expression
    // 예를 들어 함수의 인자로 들어가거나 변수에 할당되는 함수들이 FunctionExpression들이다.
    case Kind::Function:{
      cout << "Function Expression detected: " << current->code << endl;
      skipCurrent(Kind::Function);
      auto function = new FunctionExpression();
      function->upper_scope = current_block_scope;
      current_block_scope = function;
      // if하는 이유는 expression인 function이라도 이름을 가질 수 있기 때문
      if (current->kind == Kind::Identifier) {
        function->name = current->code;
        skipCurrent(Kind::Identifier);
      }
      skipCurrent(Kind::LeftParen);
      function->parameters = parseParameters();
      skipCurrent(Kind::RightParen);
      skipCurrent(Kind::LeftBrace);
      function->block = parseBlock();
      skipCurrent(Kind::RightBrace);

      current_block_scope = function->upper_scope;

      return function;
    }
    default:
      return parsed;
  }
}


// 최후순위 평가 대상: array literal과 object literal
// array나 object내의 값은 할당이 없으므로 parseAnd부터 재귀를 돈다
auto parseArrayLiteralOrObjectLiteral()->Expression* {
  if (current->kind == Kind::LeftBracket) {
    cout << "Array literal found: " << current->code << endl; 
    skipCurrent(Kind::LeftBracket);
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
    cout << "Object literal found" << endl;
    auto object = new ObjectLiteral();
    
    while (current->kind != Kind::RightBrace) {
      auto kind = current->kind;
      // js와 비슷하게 따옴표 없는 숫자와 문자도 그냥 문자열로 자동 전환
      if (kind == Kind::NumberLiteral || kind == Kind::StringLiteral || kind == Kind::Identifier) {
        string key = current->code;
        skipCurrent(current->kind);
        skipCurrent(Kind::Colon);
        Expression* value = parseNot();
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