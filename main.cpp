#include "Main.h"

auto main()->int {
  // 굳이 다른 function외의 토큰들을 분석하지 않고 무조건 mainify하는 이유
  // 만약 토큰을 function외의 것으로 처리한다고 해도, 상황은 사실상 같다.
  // 왜냐하면 js에서 딱히 엔트리 포인트가 필요 없는 이유는 global이 최상위 컨텍스트이기 때문이다. 그러니까, 애초에 interpret이 글로벌에서 일어난다는 것이다.
  // 그러므로 엔트리포인트 다변화가 의미가 없다. 차라리 function main으로 엔트리 포인트를 통일하는게 코드 복잡도를 줄이는 길
  // 이 방법이 합리적인 이유는 하나 더 있는데, main을 다시 작성하더라도 상관이 없다는 것이다.
  // 현재 인터프리터는 파싱된 함수를 루프하며 function table에 매핑하는데, main이 중복되어 지정되면 나중에 정의된, 그러니까 내부의 함수가 외부 함수를 대체한다.
  // 그러므로 main을 엔트리포인트로 사용하기만 했다면 명시했던 명시하지 않았던 같은 동작이 이루어지는 것이다.
  string sourceCode = mainify(R""""(
      console.log('Hello, World!');
      console.error('print error');
  )"""");

  auto tokenList = scan(sourceCode);
  auto syntaxTree = parse(tokenList);
  interpret(syntaxTree);

  return 0;
}

