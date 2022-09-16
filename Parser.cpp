#include <vector>
#include <iostream>
#include "Token.h"
#include "Node.h"

using std::vector;
using std::cout;
using std::endl;
// Scanner.cpp의 scan함수는 Token list를 반환한다.
// parse함수는 그렇게 생성된 Token list를 EndOfToken을 마주칠 때 까지 순회한다.
// parse함수의 반환 값은 구문 트리의 루트 노드이다. 구문 트리는 프로그램을 실행할 순서가 표현되어 있다.

// static으로 선언해두고, parse함수에서 초기화한다.
static vector<Token>::iterator current;

// 이 함수는 current iterator를 전진시키기 위한 것으로
// 인자가 필요한 이유는 현재 논리상 마땅한 Token의 Kind가 일치하는지를 검증해야하기 때문이다.
// 만약 Token의 Kind가 예상했던 것과 다르다면 bjs스크립트가 잘못 작성되었거나 컴파일러 구현 자체에 결함이 있는 것이므로 구문트리는 올바르게 생성될 수 없다.
// 결론적으로 iterator를 전진시킴과 동시에 올바른 구현인지를 검증하는 함수다.
void skipCurrent(Kind kind) {
  if (current->kind != kind) {
    cout << toString(kind) << " Token kind not matches"; 
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

Expression* parseExpression() {
  
}

// "var" 문자열을 발견하여 변수 선언임을 확인하고 변수 node를 만들기 위한 함수
// function과 마찬가지로 var도 처리가 필요하지 않은 키워드이므로 current iterator를 전진시키고, 대신 Variable node를 생성한다.
Variable* parseVariable() {
  auto variable = new Variable();
  skipCurrent(Kind::Variable);

  variable->name = current->string;
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

vector<Statement*> parseBlock() {
  vector<Statement*> block;
  // 함수 안에는 또 다른 block이 있을 수도 있는데 RightBrace로 종료를 검증하면 너무 안일한 것 아닌가?
  while (current->kind != Kind::RightBrace) {
    // 함수 블록안에 존재해서는 안되는 토큰들 검증
    switch (current->kind) {
      // 일단은 var 키워드로 변수를 선언하는 처리만 되어있다.
      // 향후 let과 const도 처리하려면 이곳에서 처리를 추가하면 된다.
      // 그때 Node.h에서 Variable에 kind를 추가할지 아니면 const와 let에 해당하는 node를 따로 만들지는 고민해보자
      case Kind::Variable:
        block.push_back(parseVariable());
        break;
      case Kind::EndOfToken:
        cout << "EndOfToken kind is not allowed to use in function block. there must be some bad implementation in compiler";
        exit(1);
    }
  }
  return block;
}

// function이라는 키워드 자체는 아무 효용이 없다. 단지 함수를 선언하겠다는 것을 알리기 위해 사용된다.
// 그러므로 function 토큰은 버린다. 여기서 버린다는 것은 current를 한칸 전진시키는 것을 의미한다.
// 다만 function키워드를 통해 함수 노드를 생성해야 함을 알게 되었다(구문 트리에 넣어야하니까). 그러므로 new Function한다.
Function* parseFunction() {
    auto function = new Function();
    skipCurrent(Kind::Function);

    // 생성한 함수 노드에 이름을 붙여준다. function 키워드 이후에 온 함수 이름이다.
    function->name = current->string;
    // 함수건 변수건 이름은 식별자이다. 식별자로 판단되지 않았으면 에러이므로 skipCurrent함수로 검사해준다.
    skipCurrent(Kind::Identifier);
    // 함수의 식별자 다음엔 매개 변수들이 위치할 것이다. 매개 변수는 좌괄호로 시작하므로
    // 좌괄호를 생략한 후, 우괄호를 만나기 전까지 쉼표를 생략하며 수집하면 될 것이다.
    // 리마인드하자면, 빈 칸을 검사하지 않은 이유는 scan할때 이미 공백을 모두 생략한 토큰 리스트를 만들기 때문이다.
    skipCurrent(Kind::LeftParen);
    if (current->kind != Kind::RightParen) {
      // do-while을 사용하는 이유는 do를 통해 무조건 1번 이상의 실행을 보장받아야하기 때문이다. while을 썼다면 내용은 전혀 실행되지 못하고 넘어갔을 것이다.
      do {
        function->parameters.push_back(current->string);
        skipCurrent(Kind::Identifier);
        // 1. 식별자 하나를 parameters에 넣는다
        // 2. 다음 토큰이 Comma라면 그 다음 param이 존재한다고 확신할 수 있다
        // 3. 그러므로 skipCurrentIf는 true를 반환하고 루프를 끝내지 않는다
        // 4. 다음 토큰이 Comma가 아니라면 params를 모두 수집했다고 볼 수 있다.
        // 5. 그러므로 skipCurrentIf는 false를 반환하고 루프를 끝낸다.
      } while(skipCurrentIf(Kind::Comma));
    }
    skipCurrent(Kind::RightParen);

    // 다음은 함수의 본문이다
    // 함수의 본문은 중괄호로 시작한다. 본문 내용에 도달하기 위해 생략한다.
    skipCurrent(Kind::LeftBrace);
    function->block = parseBlock();
    skipCurrent(Kind::RightBrace);

    return function;
  }

Program* parse(vector<Token> tokens) {
  auto result = new Program();
  current = tokens.begin();

  while (current->kind != Kind::EndOfToken) {
    switch (current->kind) {
      case Kind::Function:
        // static으로 선언된 current를 참조하므로 parseFunction에는 인자가 필요하지 않다.
        // 이런 접근 방식을 맘에 들어하지 않을 수도 있겠지만 편리한건 사실이다. 다만 함수형적인 측면으로 봤을 땐 참 그렇다.
        result->functions.push_back(parseFunction());
        break;
      default:
        cout << "Cannot parse syntax tree: invalid kind of Token detected" << endl;
        exit(1);
    }    
  }
  return result;
}

