#pragma once
#include<string>
// https://stackoverflow.com/questions/18335861/why-is-enum-class-preferred-over-plain-enum
// enum class를 enum보다 prefer하는 이유는 type safety때문이다.
enum class Kind {
  Unknown, EndOfToken,
  NullLiteral,
  TrueLiteral, FalseLiteral,
  NumberLiteral, StringLiteral,
  Identifier,

  Function, Return,
  Variable, Constant, Let,
  For, Break, Continue,
  If, Elif, Else,
  Print, PrintLine,

  LogicalAnd, LogicalOr,
  Assignment,
  Add, Subtract,
  Multiply, Divide, Modulo,
  Equal, NotEqual,
  LessThan, GreaterThan,
  LessOrEqual, GreaterOrEqual,

  Comma, Colon, Semicolon,
  LeftParen, RightParen,
  LeftBrace, RightBrace,
  LeftBracket, RightBracket
};

Kind toKind(std::string);