#include<vector>
#include<string>
#include<string>
#include<map>
#include"./Kind.h"

using std::vector;
using std::string;
using std::map;
/**
 * @brief 
 * if나 for문 등 내부에 문을 포함할 수 있으면 복합문, return 처럼 다른 문을 포함할 수 없으면 단일문이다.
 * 그러나 단일문이라고 해도 expression은 포함할 수 있다. 예를 들어 return 문에 식을 넣을 수 있는 것처럼 말이다.
 * 결론적으로 statement는 statement과 expression을 포함할 수 있다.
 */
struct Program {
  vector<Function*> functions;
};

// 문
struct Statement {};
// 식
struct Expression {};
// Function은 복합문이다. 그러므로 Statement를 상속받는다.
struct Function: Statement {
  // 함수 이름
  string name;
  // 파라미터들 이름
  vector<string> parameters;
  // 함수는 if나 for와 같이 복합문이므로 block안에 다른 Statement를 가질 수 있다.
  vector<Statement*> block;
};

// Return은 단일문이다. 그러므로 Statement를 상속받는다.
struct Return: Statement {
  // Return은 단일문이다. 그러므로 다른 Statement를 가질 수 없다. 그러나 Expression은 가질 수 있다.
  Expression* expression;
};

// 변수의 선언
struct Variable: Statement {
  // 변수 이름
  string name;
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
  vector<Statement*> block;
};

// 반복을 중지하는 break이다
struct Break: Statement {};
// 다음 순서 반복을 위한 continue이다
struct Continue: Statement {}; 

// If는 복합문이므로 다른 Statement를 멤버로 가진다
struct If: Statement {
  Expression* conditions;
  vector<Statement*> block;
  vector<Statement*> elseBlock;
};

// print
struct Print: Statement {
  // 개행 여부를 표현한다. 기본값 false이다
  bool lineFeed = false;
  // 출력할 식 리스트
  vector<Expression*> arguments;
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
};

/**
 * @brief Or & And
 * Or과 And는 이항연사자이므로 좌항과 우항을 항상 가져야한다.
 * 연산자이므로 식을 extends한다.
 */
struct Or: Expression {
  Expression* lhs;
  Expression* rhs;
};
struct And: Expression {
  Expression* lhs;
  Expression* rhs;
};

/**
 * @brief Relational & Arithmetic
 * 관계연산자와 산술연산자에는 여러가지 종류가 있다. !==, ==, +, - 등..
 * 그러므로 관계연산자와 산술연산자에는 이 연산자의 종류가 무엇인지를 나타내는 Kind가 포함된다.
 * 또 한 둘 다 이항연산자이므로 좌항과 우항을 지닌다
 */
struct Relational: Expression {
  Kind kind;
  Expression* lhs;
  Expression* rhs;
};

struct Arithmetic: Expression {
  Kind kind;
  Expression* lhs;
  Expression* rhs;
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
  Kind kind;
  Expression* sub;
};

// 함수 호출 표현식
// 인자 리스트를 통해 add(1,2)와 같은 함수 호출을 표현한다
struct Call: Expression {
  Expression* sub;
  vector<Expression*> arguments;
};

// 원소 참조 표현식
// arr[0]이나 obj['name']과 같은 것들을 표현하기 위함이다
struct GetElement: Expression {
  Expression* sub;
  Expression* index;
};

// 원소 수정 표현식.
// arr[1] = 'bjs';와 같다.
// GetElement와 다른 점은 새로 설정할 value뿐이다.
struct SetElement: Expression {
  Expression* sub;
  Expression* index;
  Expression* value;
};

// 변수의 참조와 수정
struct GetVariable: Expression {
  string name;
};

struct SetVariable: Expression {
  string name;
  Expression* value;
};

// null의 범주는 자기 자신밖에 없으므로 따로 멤버를 가질 이유가 없다
struct NullLiteral: Expression {};

// boolean의 범주는 true와 false로 이루어져 있다. 그러므로 멤버가 필요하며, 기본값은 보수적으로 false로 설정한다.
// 참고로 C++의 primitive types들은 initial value가 없다. 아래의 노드들은 그러한 차이를 단적으로 보여주고 있는데,
// bool과 double은 멤버를 명시적으로 초기화하는 반면, string은 초기값이 존재하기 때문에 초기화식이 없이 선언과 동시에 초기화가 이루어진다.
struct BooleanLiteral: Expression {
  bool value = false;
};

struct NumberLiteral: Expression {
  double value = 0.0;
};

struct StringLiteral: Expression {
  string string;
};

struct ArrayLiteral: Expression {
  vector<Expression*> values;
};

struct MapLiteral: Expression {
  map<string, Expression*> values;
};

