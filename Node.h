#ifndef NODE
#define NODE

#include<vector>
#include<string>
#include<string>
#include<map>
#include<any>
#include"Token.h"

using std::vector;
using std::string;
using std::map;
using std::any;
/**
 * @brief 
 * if나 for문 등 내부에 문을 포함할 수 있으면 복합문, return 처럼 다른 문을 포함할 수 없으면 단일문이다.
 * 그러나 단일문이라고 해도 expression은 포함할 수 있다. 예를 들어 return 문에 식을 넣을 수 있는 것처럼 말이다.
 * 결론적으로 statement는 statement과 expression을 포함할 수 있다.
 */

// 문
struct Statement {
  virtual void interpret() = 0;
};
// 식
struct Expression {
  virtual any interpret() = 0;
};
struct Judge {
  bool is_truthy(Expression*);
};
// undefined
struct Undefined:Expression {
   any interpret();
};
// 변수가 초기화 되었는지의 여부와 값
struct VariableState {
  bool initialized;
  Expression* value;
};

struct LexicalEnvironment {
    // 상위 스코프. global이 아닌 이상 모든 Block은 이 값이 nullptr이 아니다.
  LexicalEnvironment* upper_scope;
  // var, let, const를 나누는 방안도 생각해보았으나, 변수 접근이 단지 식별자만 가지고 이루어진다는 점을 생각해보면 비효율 적인 방식
  map<Kind, map<string, VariableState>> variables = {
    {Kind::Constant, map<string, VariableState>()},
    {Kind::Let, map<string, VariableState>()},
    {Kind::Variable, map<string, VariableState>()}
  };
};

// Function은 복합문이다. 그러므로 Statement를 상속받는다.
// todo: parameter들을 scope var variable로 할당해야함
// 이 후 함수를 호출할 때 받은 arg들을 매핑
struct Function: LexicalEnvironment, Statement {
  // 함수 이름
  string name;
  // 파라미터들 이름
  vector<string> parameters;
  // 함수는 if나 for와 같이 복합문이므로 block안에 다른 Statement를 가질 수 있다.
  vector<Statement*> block;
  
  void interpret();
};




// Return은 단일문이다. 그러므로 Statement를 상속받는다.
struct Return: Statement {
  Return(Expression* expr):expr(expr){};
  // Return은 단일문이다. 그러므로 다른 Statement를 가질 수 없다. 그러나 Expression은 가질 수 있다.
  Expression* expr;
  void interpret();
};

// 변수의 선언
struct Declare: Statement {
  Declare(Kind decl_type): decl_type(decl_type) {}
  LexicalEnvironment* lexical_environment;
  // initialized의 쓸모는 const일 경우 딱 하나 뿐이다
  // 그 이유는 const가 var와 let과는 달리 선언과 동시에 초기화 되어야하기 때문인데,
  // 컴파일러의 구조 상 선언과 할당이 분리 되어있기 때문에 const는 선언 다음에 바로 할당이 위치해야 한다는 뜻이다
  // 그렇다면, initialized가 false일 경우 첫 할당만을 허용할 수 있다.
  // const와 다르게 let, var는 initialized가 false이건 true건 할당할 수 있다. 그래서 initialized는 const를 위한 것이다
  // 다만 let과 var도 init값에 따라 컨트롤 할 여지가 있으므로 정보에는 포함시킨다
  Kind decl_type;
  // 변수 이름
  string name;
  void interpret();
};

// for 문. 변수의 선언, 조건식, 증감식, 실행할 문 리스트를 가진다.
struct For: LexicalEnvironment, Statement, Judge {
  string starting_point_name;
  // for문을 위한 변수 선언(흔히 사용하는 i v등을 떠올려보자)
  vector<Statement*> starting_point;
  // 조건식
  Expression* condition;
  // 증감식
  Expression* expression;
  // for는 복합문이므로 Statement를 멤버로 가진다
  vector<Statement*> block;
  void interpret();
};

// 반복을 중지하는 break이다
struct Break: Statement {
  void interpret();
};
// 다음 순서 반복을 위한 continue이다
struct Continue: Statement {
  void interpret();
}; 

// If는 복합문이므로 다른 Statement를 멤버로 가진다
struct If: LexicalEnvironment, Statement, Judge {
  Expression* condition;
  vector<Statement*> block;
  vector<Statement*> else_block;
  void interpret();
};

// console
struct Console: Statement {
  void sequencePrint();
  // 개행 여부를 표현한다. 기본값 false이다
  string console_method;
  // 출력할 식 리스트
  vector<Expression*> arguments;
  void interpret();
};

/**
 * @brief ExpressionStatement
 * 너무 기니 '식문'이라고 명명하고 설명을 하겠다.
 * 예를 들어, 1 + 2를 출력하거나 return문에 식으로 포함시킨다고 가정하자.
 * 이 때 1 + 2의 결과값 3은 '소비'된다.
 * 그러나 문에 포함시키지 않고 그냥 적어도 연산은 일어나겠지만 그 결과값이 소비되지는 않는다
 * 이런 식으로 소비되지 않는 결과값을 임의로 소비된 것으로 취급하기 위해 '식문'이 존재한다.
 * 식은 문에 포함되는 종속관계를 가지는데, 문이 없이 식이 표현되는 것을 코드 작성시에 허용하면서도 법칙을 위배하지 않기 위한 안전장치라고 생각하면 된다.
 */

struct ExpressionStatement: Statement {
  Expression* expression;
  void interpret();
};

/**
 * @brief Or & And
 * Or과 And는 이항연사자이므로 좌항과 우항을 항상 가져야한다.
 * 연산자이므로 식을 extends한다.
 */

struct Not: Expression, Judge {
  Not(Expression* expr):expr(expr) {};
  // expr은 모든 값을 가리킨다
  // js는 모든 값이은truthy or falsy이므로
  Expression* expr;
  any interpret();
};

struct Or: Expression, Judge {
  Expression* lhs;
  Expression* rhs;
  any interpret();
};
struct And: Expression, Judge {
  Expression* lhs;
  Expression* rhs;
  any interpret();
};

/**
 * @brief Relational & Arithmetic
 * 관계연산자와 산술연산자에는 여러가지 종류가 있다. !==, ==, +, - 등..
 * 그러므로 관계연산자와 산술연산자에는 이 연산자의 종류가 무엇인지를 나타내는 Kind가 포함된다.
 * 또 한 둘 다 이항연산자이므로 좌항과 우항을 지닌다
 */
struct FunctionExpression: Expression, LexicalEnvironment {
  string name;
  // 파라미터들 이름
  vector<string> parameters;
  // 함수는 if나 for와 같이 복합문이므로 block안에 다른 Statement를 가질 수 있다.
  vector<Statement*> block;
  any interpret();
};

struct DeclareFunction: Statement {
  DeclareFunction(FunctionExpression* function):function(function) {};
  FunctionExpression* function;
  void interpret();
};

struct Program {
  vector<DeclareFunction*> functions;
};

struct Relational: Expression {
  Relational(Kind kind):kind(kind) {}
  Expression* lhs;
  Expression* rhs;
  any interpret();
  private:
    Kind kind;
};

struct Arithmetic: Expression {
  Arithmetic(Kind kind): kind(kind){};
  Expression* lhs;
  Expression* rhs;
  any interpret();
  private:
    friend Judge;
    Kind kind;
};

/**
 * @brief And, Or, Relational, Arithmetic 연사자를 분리한 이유 
 * 먼저 And와 Or는 동작에서 차이가 있기 때문이다.
 * Or라면 lhs가 거짓일지라도 rhs의 식을 평가할 것이다. And라면 lhs가 false인 순간 연산을 그만둔다.
 *
 * Relational연산자와 Arithmetic은 연산불가 일때 반환값을 다르게 설정하기 위함이다.
 * 예를 들어 Arithmetic의 0으로 나누기 할 때 NaN을 반환한다거나, Relational연산을 할수 없을땐 기본값을 false로 설정한다거나 하는 것들이다.
 */

// 단한연산자
// 이곳에선 +,-기호를 사용할 것인데, 이항연산자 +,-와는 다른 목적으로 사용할 것이다.
// 예를 들어 js에서 +를 단항연산자로 사용하면 피연산자식을 number로 형 변환하는 식이다.
// 일단 이곳에서 사용되는 +는 absolute을 구하는데에 사용될 것이고, -는 부호반전을 위해 사용될 것이다.
// 상술했듯이 Unary는 두 종류가 있으므로 Kind를 가진다. 단항연산자이므로 피연산자식 한 개만 가지는 것은 자명하다.
struct Unary: Expression {
  Unary(Kind kind):kind(kind) {};
  LexicalEnvironment* lexical_environment;
  Kind kind;
  Expression* sub;
  any interpret();
};

// 함수 호출 표현식
// 인자 리스트를 통해 add(1,2)와 같은 함수 호출을 표현한다
struct Call: Expression {
  Expression* sub;
  vector<Expression*> arguments;
  any interpret();
};

// 원소 참조 표현식
// arr[0]이나 obj['name']과 같은 것들을 표현하기 위함이다
struct GetElement: Expression {
  Expression* sub;
  Expression* index;
  any interpret();
};

// 원소 수정 표현식.
// arr[1] = 'bjs';와 같다.
// GetElement와 다른 점은 새로 설정할 value뿐이다.
struct SetElement: Expression {
  Expression* sub;
  Expression* index;
  Expression* value;
  any interpret();
};

// 변수의 참조
struct GetVariable: Expression {
  LexicalEnvironment* lexical_environment;
  string name;
  any interpret();
};

struct SetVariable: Expression {
  LexicalEnvironment* lexical_environment;
  string name;
  Expression* value;
  any interpret();
  VariableState get_allocating_value();
};

// null의 범주는 자기 자신밖에 없으므로 interpret외에 따로 멤버를 가질 이유가 없다
struct NullLiteral: Expression {
  any interpret();
};

// boolean의 범주는 true와 false로 이루어져 있다. 그러므로 멤버가 필요하며, 기본값은 보수적으로 false로 설정한다.
// 참고로 C++의 primitive types들은 initial value가 없다. 아래의 노드들은 그러한 차이를 단적으로 보여주고 있는데,
// bool과 double은 멤버를 명시적으로 초기화하는 반면, string은 초기값이 존재하기 때문에 초기화식이 없이 선언과 동시에 초기화가 이루어진다.
struct BooleanLiteral: Expression {
  BooleanLiteral(bool boolean):boolean(boolean){}
  bool boolean;
  any interpret();
};

struct NumberLiteral: Expression {
  double value;
  any interpret();
};

struct StringLiteral: Expression {
  string value;
  any interpret();
};

struct ArrayLiteral: Expression {
  string array_method;
  vector<Expression*> values;
  vector<Expression*> arguments;
  any interpret();
};

struct ObjectLiteral: Expression {
  map<string, Expression*> values;
  any interpret();
};

struct Method: Expression {
  Expression* this_ptr;
  string method;
  vector<Expression*> arguments;
  any interpret();
};

#endif