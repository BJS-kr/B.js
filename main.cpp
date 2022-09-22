#include "Main.h"

auto main(int argc, char** argv)->int {
  string sourceCode = 
    "function main() {"
      "print 'hello world!'"
    "}"
  ;

  auto tokenList = scan(sourceCode);
  auto syntaxTree = parse(tokenList);
  interpret(syntaxTree);

  return 0;
}

