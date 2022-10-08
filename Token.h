#ifndef TOKEN
#define TOKEN

#include <string>

using std::string;
// https://stackoverflow.com/questions/18335861/why-is-enum-class-preferred-over-plain-enum
// enum class를 enum보다 prefer하는 이유는 type safety때문이다.
enum class Kind {
  Unknown, EndOfToken,
  NullLiteral, UndefinedLiteral,
  TrueLiteral, FalseLiteral,
  NumberLiteral, StringLiteral,
  Identifier,

  Function, Return,
  Variable, Constant, Let,
  For, Break, Continue,
  If, Elif, Else,
  Console,

  LogicalAnd, LogicalOr,

  Assignment, AddAssignment, SubtractAssignment,

  Add, Subtract,
  Multiply, Divide, Modulo,
  Increment, Decrement,
  StrictEqual, StrictNotEqual, Equal, NotEqual,
  LesserThan, GreaterThan,
  LesserOrEqual, GreaterOrEqual,

  Dot, Comma, Colon, Semicolon,
  LeftParen, RightParen,
  LeftBrace, RightBrace,
  LeftBracket, RightBracket
};

string toString(Kind);
Kind toKind(string);

struct Token {
  // unknown by default
  Kind kind;
  string code;
};

#endif