#include <string>
#include <vector>
#include "./Token.h"
#include "./char_type.h"
#include <iostream>

using namespace std;

static string::iterator current;

Token scanNumberLiteral() {
  string string;
  while (isCharType(*current, CharType::NumberLiteral)) 
    string += *current++;
  // 실수도 처리할 수 있도록 소수점 이후에 다시 while문을 실행하자
  if (*current == '.') {
    string += *current++;
    while (isCharType(*current, CharType::NumberLiteral)) 
      string += *current++;
  }

  return Token{ Kind::NumberLiteral };
};

Token scanStringLiteral() {
  string string;
  // 첫 글자는 따옴표이므로 건너뛰기 위해 1 증가
  ++current;
  while (isCharType(*current, CharType::StringLiteral))  
    string += *current++;
  
  return Token{Kind::StringLiteral, string};
};

Token scanIdentifierAndKeyword() {
  string string;
  while (isCharType(*current, CharType::IdentifierAndKeyword))
    string += *current++;
  // kind.cpp에 정의된 toKind함수는 keyword의 종류를 판별하는 함수이다.
  // 예약된 keyword가 아니라면 Unknown을 반환하는데
  // 예약된 키워드도 아니고, 문자열이나 숫자도 아니라면 식별자일 것이다
  // 그러므로 toKind가 Unknown반환시 Identifier로 Token을 생성하고,
  // 아니라면 그에 맞는 kind를 넣는다.
  Kind kind = toKind(string);
  return Token{kind == Kind::Unknown ? Kind::Identifier : kind, string};
};

Token scanOperatorAndPunctuator() {
  string string;
  while (isCharType(*current, CharType::OperatorAndPunctuator)) 
    string += *current++;
  // 연산자는 항상 뒤에서부터(가장 긴 순서대로) 검증해야 한다
  // 예를 들어보자. +=연산자를 앞에서부터 검증한다면, 나뉘어진 +와 =연산자가 두 개 있다고 인식할 것이다
  // 그러므로 string을 뒤에서부터 제거하며 일치하는 kind가 있는지 검증한다
  while (!string.empty() && toKind(string) == Kind::Unknown) {
    // 하나 주의할 것은 pop_back하는 정확한 의미이다.
    // current라는 iterator는 static으로 모든 함수들이 공통적으로 참조하고 있는 변수이다.
    // 예를 들어 연산자와 구분자가 연속으로 쓰였다고 가정하자
    // scanOperatorAndPunctuator함수는 연산자와 구분자를 구분하지 않고 string에 +=하므로
    // 두 개가 하나의 string변수에 쓰여있을 가능성이 충분하다
    // 그렇다면 일치하는 연산자나 구분자까지 iterator를 되돌린 후 거기서부터 연산을 다시 시작해야지 올바른 구문분석이 가능한것이다.
    string.pop_back();
    // 문자열을 하나 제거했으니 iterator도 되감아준다. 이후 되감긴 부분부터 scan함수의 while문이 연산을 계속 수행할 것이다.
    --current;
  }

  if (string.empty()) {
    cout << *current << "invalid operator or punctuator detected" << endl;
  }

  return Token{toKind(string), string};
};

bool isCharType(const char c, CharType type) {
  switch (type) {
    case CharType::IdentifierAndKeyword:
      return c >= '0' && c <= '9' ||
             c >= 'a' && c <= 'z' ||
             c >= 'A' && c <= 'Z';
      break;
    case CharType::StringLiteral:
      return c >= 32 && c <= 126 && c != '\'';
      break;
    case CharType::NumberLiteral:
      return c >= '0' && c <= '9';
      break;
    case CharType::OperatorAndPunctuator:
      return c >= 33 && c <= 47 ||
             c >= 58 && c <= 64 ||
             c >= 91 && c <= 96 ||
             c >= 123 && c <= 126;
      break;
    default:
      return false;
  }
}

CharType getCharType(const char c) {
  // 공백, 탭, 캐리지 리턴, 개행은 일괄적으로 WhiteSpace로 처리한다.
  if (c == ' ' || c == '\t' || c == '\r' || c == '\n') 
    return CharType::WhiteSpace;
  if (c >= '0' && c <= '9')                            
    return CharType::NumberLiteral;    
  if (c == '\'')                                                
    return CharType::StringLiteral;
  if (c >= 'a' && c <= 'z' || c >= 'A' && c <= 'Z') 
    return CharType::IdentifierAndKeyword;
  // ' 문자는 StringLiteral의 시작문자로 규정해두었으므로 제외해야 한다.
  // ASCII Code: https://theasciicode.com.ar/
  // 하단의 조건은 연산에 사용되는 특수문자들을 검증하는 것이다
  if (c >= 33 && c <= 47 && c != '\'' ||
      c >= 58 && c <= 64 ||
      c >= 91 && c <= 96 ||
      c >= 123 && c <= 126)
    return CharType::OperatorAndPunctuator;
  // Unknown by default
  return CharType::Unknown;
}


vector<Token> scan(string sourceCode) {
  vector<Token> result;
  sourceCode += '\0'; // null 문자. null에 도달하면 정지하기 위함
  current = sourceCode.begin();

  // 포인터 역참조하여 null문자 검증
  while (*current != '\0') {
    switch (getCharType(*current)) {
      case CharType::WhiteSpace:
        // 공백이라면 iterator전진! 단순히 전진만 하는 이유는 공백은 말 그대로 공백. 코드 분석 이후에는 쓸모없기 때문이다.
        ++current;
        break;
      case CharType::NumberLiteral:
        result.push_back(scanNumberLiteral());
        break;
      case CharType::StringLiteral:
        result.push_back(scanStringLiteral());
        break;
      case CharType::IdentifierAndKeyword:
        result.push_back(scanIdentifierAndKeyword());
        break;
      case CharType::OperatorAndPunctuator:
        result.push_back(scanOperatorAndPunctuator());
        break;
      default:
        cout << *current << "invalid source code character detected. program is shutting down" << endl;
        exit(1);
    }
  }
  // 마지막엔 Token의 string값을 비운채로 토큰만 푸쉬
  result.push_back({ Kind::EndOfToken });
  return result;
}