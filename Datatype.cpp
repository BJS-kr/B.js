#include "Datatype.h"

// 이곳의 함수들은 동적언어를 표방하기 위해 존재한다. 
// 타이핑이 없는 언어는 항상 any여야하는데, 그 와중에도 연산시에는 데이터 타입이 필요하므로
// 타입 캐스팅이 필요하다. 매번 반복되는 타입캐스팅 함수들을 이곳에 작성한 것이다.
using std::any;
using std::any_cast;
using std::string;
using std::ostream;

/**
 * @brief Type checking
 * */
auto isString(any value)->bool {
  return value.type() == typeid(string);
}
auto isNumber(any value)->bool {
  return value.type() == typeid(double);
}
auto isArray(any value)->bool {
  return value.type() == typeid(ArrayLiteral*);
}
auto isObject(any value)->bool {
  return value.type() == typeid(ObjectLiteral*);
}
auto isFunctionExpression(any value)->bool {
  return value.type() == typeid(FunctionExpression*);
}
auto isGetVariable(any value)->bool {
  return value.type() == typeid(GetVariable*);
}
auto isUndefined(any value)->bool {
  return value.type() == typeid(Undefined*);
}
auto isConsole(any value)->bool {
  return value.type() == typeid(Console*);
}
auto isBool(any value)->bool {
  return value.type() == typeid(bool);
}

/**
 * @brief Type casting
*/
auto toString(any value)->string {
  return any_cast<string>(value);
}
auto toNumber(any value)->double {
  return any_cast<double>(value);
}
auto toArray(any value)->ArrayLiteral* {
  return any_cast<ArrayLiteral*>(value);
}
auto toObject(any value)->ObjectLiteral* {
  return any_cast<ObjectLiteral*>(value);
}
auto toFunctionExpression(any value)->FunctionExpression* {
  return any_cast<FunctionExpression*>(value);
}
auto toGetVariable(any value)->GetVariable* {
  return any_cast<GetVariable*>(value);
}
auto toConsole(any value)->Console* {
  return any_cast<Console*>(value);
}
auto toBool(any value)->bool {
  return any_cast<bool>(value);
}

/**
 * @brief Utility
*/
auto doubleToString(double num)->string {
  stringstream ss;
  ss << fixed << setprecision(0) << num;
  return ss.str();
}

// cout은 any를 해석할 수 없으므로 연산자 오버로딩
// return value가 reference일 때: https://www딩tutorialspoint.com/cplusplus/returning_values_by_reference.htm
// reference value를 return 해야 insertion들을 combine할 수 있음: https://learn.microsoft.com/en-us/cpp/standard-library/overloading-the-output-operator-for-your-own-classes?view=msvc-170
ostream& operator<<(ostream& stream, any& value) {
  if (isString(value)) stream << toString(value);
  if (isNumber(value)) stream << toNumber(value);
  if (isBool(value)) {
    if (toBool(value)) stream << "true";
    else stream << "false";
  }
  if (isArray(value)) {
    auto array = toArray(value);
    string result = "[ ";
    auto values_size = array->values.size();
    for (int i = 0; i < values_size; i++) {
      auto element = array->values[i]->interpret();
      result += element;
      if (i != values_size - 1) result += ", ";
    }
    result += " ]";
    stream << result;
  }
  if (isUndefined(value)) stream << "undefined";

  return stream;
}

string& operator+=(string& lhs, any& rhs) {
  if (isString(rhs)) lhs += toString(rhs);
  if (isNumber(rhs)) lhs += to_string(toNumber(rhs));
  return lhs;
}

