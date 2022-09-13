# B.js
1. js 컴파일러 클론입니다. B.js인 이유는 제 이름의 이니셜이 bjs이기 때문입니다. 확장자도 bjs로 할 생각입니다.  
2. js를 클론하는 이유는 제가 nodejs메인이고 js와 ts를 좋아하기 때문입니다.  
3. js의 핵심 스펙들을 구현할 생각입니다. js스러운 memory allocation이나 mark & sweep garbage collection 등 입니다. event-loop은 제대로 구현할 수 있을지는 모르겠습니다.  
4. js의 full-spec을 구현할 생각은 없기 때문에 prototype system은 제외입니다.  
5. js로 컴파일 되지 않는 native ts도 고려중입니다. Go의 컨셉을 차용해볼까 합니다. 명시적으로 타입을 추론하라는 문법 등을요.  
6. 가능하면 functional programming language의 spec들을 추가할 예정입니다(2차 목표. immutable, built-in functors 등).  
7. C++을 선택한 이유는 다음과 같습니다.
   a. nodejs에는 N-API라는 기능이 있는데, native language로 작성된 파일을 nodejs native module처럼 사용할 수 있는 기능입니다. 물론 C++만 사용가능한 것은 아닙니다.
   b. libuv는 C++로 쓰여있습니다. nodejs와 C++는 뗄 수 없는 관계로, nodejs를 깊게 이해하기 위해선 libuv를 이해해야합니다.
   c. 각종 언어의 VM은 C++로 쓰인 경우가 많습니다(nodejs, java 등). VM이 정확히 무슨 역할을 하는지 이해하고 싶었습니다.
   d. 각종 운영체제는 C++로 쓰여있습니다. network의 구현도 C++로 작성되었습니다.
   e. 서버에서 사용하는 각종 DB등 기술 스택은 대부분 C++로 작성되었습니다.
   f. '서버 개발자' 혹은 'nodejs 개발자'가 아니라 '개발자'가 되고 싶습니다. 특정 영역에 국한 되기 싫어 low-level을 파고들 기회를 엿보고 있었습니다.
   
ts를 구현하면 확장자가 bts가 될 것 같아 다른 이름은 없나 고민중입니다. 의견 있으시다면 이슈 부탁드립니다.

