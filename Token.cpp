#include <string>
#include <map>
#include "Token.h"

using std::string;
using std::map;

static map<string, Kind> stringToKind = {
  {"#unknown",    Kind::Unknown},
  {"#EndOfToken", Kind::EndOfToken},
  
  {"null",        Kind::NullLiteral},
  {"undefined",   Kind::UndefinedLiteral},
  {"true",        Kind::TrueLiteral},
  {"false",       Kind::FalseLiteral},
  {"#Number",     Kind::NumberLiteral},
  {"#String",     Kind::StringLiteral},
  {"#identifier", Kind::Identifier},

  {"function",    Kind::Function},
  {"return",      Kind::Return},

  {"var",         Kind::Variable},
  {"let",         Kind::Let},
  {"const",       Kind::Constant},

  {"for",         Kind::For},
  {"break",       Kind::Break},
  {"continue",    Kind::Continue},
  {"if",          Kind::If},
  {"else",        Kind::Else},

  {"&&",          Kind::LogicalAnd},
  {"||",          Kind::LogicalOr},

  {"=",           Kind::Assignment},
  {"+=",          Kind::AddAssignment},
  {"-=",          Kind::SubtractAssignment},

  {"+",           Kind::Add},
  {"-",           Kind::Subtract},
  {"*",           Kind::Multiply},
  {"/",           Kind::Divide},
  {"%",           Kind::Modulo},
  {"++",          Kind::Increment},
  {"--",          Kind::Decrement},

  {"===",         Kind::StrictEqual},
  {"!==",         Kind::StrictNotEqual},
  {"==",          Kind::Equal},
  {"!=",          Kind::NotEqual},
  {"<",           Kind::LesserThan},
  {">",           Kind::GreaterThan},
  {"<=",          Kind::LesserOrEqual},
  {">=",          Kind::GreaterOrEqual},

  {".",           Kind::Dot},
  {",",           Kind::Comma},
  {":",           Kind::Colon},
  {";",           Kind::Semicolon},
  {"(",           Kind::LeftParen},
  {")",           Kind::RightParen},
  {"{",           Kind::LeftBrace},
  {"}",           Kind::RightBrace},
  {"[",           Kind::LeftBracket},
  {"]",           Kind::RightBracket},
};

// IIFE로 위의 맵을 뒤집은 값을 할당
static auto kindToString = [] {
  map<Kind, string> result;
  for (auto& [key, value] : stringToKind)
    result[value] = key;
  return result;
}();


auto toKind(string string)->Kind {
  // count는 존재하는지를 검증. at은 key에 대응되는 value가져오기
  if (stringToKind.count(string)) return stringToKind.at(string);
  // Unknown by default
  return Kind::Unknown;
}

auto toString(Kind type)->string {
  if (kindToString.count(type))
    return kindToString.at(type);
  return "";
}