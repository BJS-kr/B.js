#include "Main.h"

auto main()->int {
  string sourceCode = R""""(
    function main() {
      print 'Hello, World!';
    }
  )"""";

  auto tokenList = scan(sourceCode);
  auto syntaxTree = parse(tokenList);
  interpret(syntaxTree);

  return 0;
}

