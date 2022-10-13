#ifndef CHAR_TYPE
#define CHAR_TYPE

enum class CharType {
  Unknown,
  Notation,
  WhiteSpace,
  NumberLiteral,
  StringLiteral,
  IdentifierAndKeyword,
  OperatorAndPunctuator,
};

#endif