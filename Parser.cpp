#include <vector>
#include <iostream>
#include "Token.h"
#include "Node.h"
#include "Datatype.h"

using std::vector;
using std::cout;
using std::endl;
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
static auto parseExpression()->Expression*;
static auto parseExpressionStatement()->ExpressionStatement*;
static auto parseVariable()->Variable*;
static auto parseParameters()->vector<string>;
static auto parseBlock()->vector<Statement*>;
static auto parseFunction()->Function*;

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
  // 함수 안에는 또 다른 block이 있을 수도 있는데 RightBrace로 종료를 검증하면 너무 안일한 것 아닌가?
  // 라고 생각했는데 생각해보니 재귀적으로 parseBlock을 실행하면 될일 이었다.
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
        block.push_back(parseVariable());
        break;
      case Kind::Console: {
        auto console = new Console();
        auto stringLiteral = new StringLiteral();

        skipCurrent(Kind::Console);
        skipCurrent(Kind::Dot);
        console->consoleMethod = current->code;
        skipCurrent(Kind::Identifier); // console method

        skipCurrent(Kind::LeftParen);
        stringLiteral->value = current->code;
        console->arguments = { stringLiteral };
        block.push_back(console);
        skipCurrent(Kind::StringLiteral);
        skipCurrent(Kind::RightParen);
        break;
      }
        
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

// "var" 문자열을 발견하여 변수 선언임을 확인하고 변수 node를 만들기 위한 함수
// function과 마찬가지로 var도 처리가 필요하지 않은 키워드이므로 current iterator를 전진시키고, 대신 Variable node를 생성한다.
Variable* parseVariable() {
  auto variable = new Variable();
  skipCurrent(Kind::Variable);

  variable->name = current->code;
  skipCurrent(Kind::Identifier);
  skipCurrent(Kind::Assignment); // 할당연산자 = 를 말한다. 설명이 없어도 생략이 가능함을 알 것이다.
  // 사실 이는 var과 let에 한해서 js스펙에 걸맞지 않은데, const는 항상 즉시 초기화가 필요한 반면 var과 let은 초기화를 미룰 수 있기 때문이다.
  // 그러므로 Assignment를 즉시 검증하는 것은 올바르지 않고, const의 경우에만 위의 구현이 올바르다.
  // var이나 let이라면 즉시 초기화하는 경우와 미루는 경우를 둘다 판단해야하고, 미뤘다고 판단하면 expression은 undefined를 할당해야하는데 그냥 null로 퉁치겠다.
  
  variable->expression = parseExpression();
  skipCurrent(Kind::Semicolon); // 이것도 js스펙과는 맞지 않는다. js는 semicolon없이도 구문 트리를 생성할 수 있기 때문이다. 
  // 다만 세미콜론 없이도 줄바꿈으로 초기화 종료를 판단하는 것은 간단한데, 둘다 검증하면 되기 때문이다. 
  // 그렇게 하려면 skipCurrent 함수의 인자는 vector<Kind>가 되어 여러 요소를 순회하며 검증하는 식이 되어야 할것이다. 템플릿 특수화를 하던가..
  // 일단은 간단한 구현으로 완성하는게 먼저이니 굳이 구현하지 않겠다. 

  return variable;
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
// 문으로 감싸야 하는 이유는 프로그램이 '구문'을 소비하기 때문이다. Statement타입이 아니면 소비할 수 없다는 것이다.
ExpressionStatement* parseExpressionStatement() {
  auto expressionStatement = new ExpressionStatement();
  expressionStatement->expression = parseExpression();

  skipCurrent(Kind::Semicolon);

  return expressionStatement;
}

Expression* parseAssignment() {
  auto result = parseOr();
  // Or과 다르게 if로 검증하는 이유는 자명하다. 하단의 두개의 if문에서 재귀호출하여 parseAssignment하고 있기 때문이다.
  // 그 이유는 Assignment가 우결합연산이기 때문이다. 
  // a = b = 3 이라는 식을 떠올려보자. a와 b는 모두 3이 되어야 하고, 그러려면 b = 3이 먼저 평가된 후 a = b가 평가되어야 한다.
  // 그렇다면 조금 더 길게 a = b = ... z = 3이라고 생각해보자. 결국 평가되어야 하는 것은 3이다.
  // a = b를 평가하던 도중 assignment연산이 더 존재한다면 b = c를 평가하고 그 와중에 c = d를 평가다야하고.. 결국 바닥인 3을 찍고 돌아와야한다는 것이다.
  // 이런 식으로 조건에 따라 바닥부터 평가가 되돌아오게 하기 위하여 재귀호출하여 처리한다. 
  if (current->kind != Kind::Assignment) 
    return result;
  skipCurrent(Kind::Assignment);

  if (auto getVariable = dynamic_cast<GetVariable*>(result)) {
    auto varSetter = new SetVariable();
    varSetter->name = getVariable->name;
    varSetter->value = parseAssignment();
    return varSetter;
  }

  if (auto getElement = dynamic_cast<GetElement*>(result)) {
    auto elemSetter = new SetElement();
    elemSetter->sub = getElement->sub;
    elemSetter->index = getElement->index;
    elemSetter->value = parseAssignment();
    return elemSetter;
  }
  // 원소참조도 아니고, 변수도 아니라면 불가능한 연산임
  cout << "invalid assignment attempt detected";
  exit(1);
}

Expression* parseOr() {
  auto result = parseAnd();
  // LogicalOr(||) 조건은 연속으로 이어 쓰는게 가능하므로 while을 통해 LogicalOr가 존재하지 않을 때 까지 연속으로 실행한다.
  while (skipCurrentIf(Kind::LogicalOr)) {
    // Or가 연속으로 존재한다는 것은 사실, Or의 우변에 n번째 Or들이 포함된다는 것이다(좌결합 연산이므로)
    // 그러므로 Or가 nest될 수 있도록 while안에서 처리해준다
    auto tempOr = new Or();
    // 첫 루프에서 result가 lhs가 될 수 있는 이유는 애초에 current가 LogicalOr가 아니었다면 while문에 진입조차 하지 못했을 것이기 때문이다.
    tempOr->lhs = result;
    // parseAnd인 이유는 parseOr함수 첫 줄과 마찬가지로 토큰이 LogicalOr전까지만 진행되고 멈춰야하기 때문이다
    tempOr->rhs = parseAnd();
    result = tempOr;
  }
  // 즉, 함수 첫줄의 parseAnd의 result가 실행된 후 current의 위치가 ||가 아니었다면 Or객체가 아닌 다른 상위의 Expression객체였을 것이다.
  return result;
}

Expression* parseAnd() {
  return {};
}















