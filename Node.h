#include<vector>
#include<string>
// if나 for문 등 내부에 문을 포함할 수 있으면 복합문, return 처럼 다른 문을 포함할 수 없으면 단일문이다.
// 그러나 단일문이라고 해도 expression은 포함할 수 있다. 예를 들어 return 문에 식을 넣을 수 있는 것처럼 말이다.
// 결론적으로 statement는 statement과 expression을 포함할 수 있다.
struct Program {
  std::vector<struct Function*> functions;
};

// 문
struct Statement {};
// 식
struct Expression {};
// Function은 복합문이다. 그러므로 Statement를 상속받는다.
struct Function: Statement {
  // 함수 이름
  std::string name;
  // 파라미터들 이름
  std::vector<std::string> parameters;
  // 함수는 if나 for와 같이 복합문이므로 block안에 다른 Statement를 가질 수 있다.
  std::vector<Statement> block;
};

// Return은 단일문이다. 그러므로 Statement를 상속받는다.
struct Return: Statement {
  // Return은 단일문이다. 그러므로 다른 Statement를 가질 수 없다. 그러나 Expression은 가질 수 있다.
  Expression* expression;
};

// 변수의 선언
struct Variable: Statement {
  // 변수 이름
  std::string name;
  // 변수 선언엔 식을 넣을 수 있다
  Expression* expression; 
};

// for 문. 변수의 선언, 조건식, 증감식, 실행할 문 리스트를 가진다.
struct For: Statement {
  // for문을 위한 변수 선언(흔히 사용하는 i v등을 떠올려보자)
  Variable* variable;
  // 조건식
  Expression* condition;
  // 증감식
  Expression* expression;
  // for는 복합문이므로 Statement를 멤버로 가진다
  std::vector<Statement*> block;
};

// 반복을 중지하는 break이다
struct Break: Statement {};
// 다음 순서 반복을 위한 continue이다
struct Continue: Statement {}; 

