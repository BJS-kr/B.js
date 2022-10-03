#include"Datatype.h"
// 이곳의 함수들은 동적언어를 표방하기 위해 존재한다. 
// 타이핑이 없는 언어는 항상 any여야하는데, 그 와중에도 연산시에는 데이터 타입이 필요하므로
// 타입 캐스팅이 필요하다. 매번 반복되는 타입캐스팅 함수들을 이곳에 작성한 것이다.

using std::any;
using std::any_cast;
using std::string;
using std::ostream;

bool isString(any value) {
  return value.type() == typeid(string);
}

string toString(any value) {
  return any_cast<string>(value);
}

bool isNumber(any value) {
  return value.type() == typeid(double);
}

double toNumber(any value) {
  return any_cast<double>(value);
}



// cout을 위한 << 연산자 오버로딩
// return value가 reference일 때: https://www.tutorialspoint.com/cplusplus/returning_values_by_reference.htm
// reference value를 return 해야 insertion들을 combine할 수 있음: https://learn.microsoft.com/en-us/cpp/standard-library/overloading-the-output-operator-for-your-own-classes?view=msvc-170
ostream& operator<<(ostream& stream, any& value) {
  if (isString(value)) stream << toString(value);
  if (isNumber(value)) stream << toNumber(value);
  return stream;
}

