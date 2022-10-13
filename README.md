# B.js
현재 브랜치는 interpreter 버전입니다. compiler 버전은 codegen 브랜치를 확인하시기 바랍니다(진행중).
auto type이 많은 것은 단순히 가독성과 줄맞춤 때문입니다.
camel case를 사용한 이유는 c/c++ 내장 함수와 구분짓기 위함입니다.

모든 코드는 VS code에서 작성되었으며, c++17 기준의 코드입니다. 
Apple clang 14.0.0를 이용하여 컴파일 하였습니다.
### 직접 테스트해보고 싶으시다면
1. source.bjs에 js코드를 작성한다.
2. br.sh 스크립트를 실행한다.

## Flow
1. Scan
2. Parse
3. Interpret
## 구현된 사양
1. scope chain
2. method chaining
3. function(JS답게 Expression으로 취급. 보통 함수는 Statement로 취급됩니다)
4. strict equal등 관계 연산
5. var, let, const의 특징들
6. Array, Object(메서드는 Array의 map과 reduce뿐입니다)
7. 그 외 언어 기본 사양: 반복, 분기, 조회, 연산 우선 순위 등

## 구현되지 않은 사양
1. Arrow function -> this binding 과정이 없기 때문에 함수 형식 분리 불필요
2. Class -> 언어 사양 부족
3. Call stack & Heap 및 Mark & Sweep GC -> Virtual Machine 구현에 포함시킬 예정
4. Event-loop
5. 좀 더 똑똑한 Scanning(semicolon 없이 코드 분석 불가)
6. 그 외 JS가 제공하는 수 많은 프로토타입 및 기본 메서드들

# 구현 예
**아래는 단지 예시입니다. 동일한 수준의 연산들은 대부분 가능합니다.**
## 변수 선언의 예
#### const와 let에는 제한사항들이 그대로 적용되어있습니다
```js
var name = 'bjs';
var name = 'BJS';
const height = 181;

let weight = 90;
weight = 89;
```
## 배열 및 메서드 체인
```js
const arr = ['hi', 1, 'wow'];
console.log(arr.map(function(x) {return x + 1;}).reduce(function(acc, curr) {return acc + curr;}, ""));
```

## 함수 및 원소 조회
```js
function test(x) {
  console.log(x);
}
const arr = [test];
const obj = {func:test};

arr[0](3);
obj["func"]('hello');
```

## 반복, 분기, 스코프 분리
#### 아래의 예시에서 최하단 출력은 undefined가 출력됨
```js
const upper = 1;
for (let i = 0; i < 4; i++) {
  const lower = 0;
  if (i % 2 === 0) {
    console.log(upper, lower);
  }
}

console.log(lower);
```

## 관계 연산
```js
if ([] === []) { console.log('this will not be printed');}

const arr = [];
if (arr === arr) { console.log("this will be printed");}

console.log('1' == 1);
console.log('1' === 1);
```