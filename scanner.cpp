#include <string>
#include <vector>
#include "./Token.h"
#include "./char_type.h"
#include <iostream>

using namespace std;

static string::iterator current;

Token scanNumberLiteral(string::iterator current) {
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
Token scanStringLiteral();
Token scanIdentifierAndKeyword();
Token scanOperatorAndPunctuator();

bool isCharType(const char c, CharType type) {
  switch (type) {
    case CharType::NumberLiteral:
      return c >= '0' && c <= '9';
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