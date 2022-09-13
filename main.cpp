#include <string>
#include <vector>
#include <sstream>
#include "./Token.h"

using namespace std;

// https://stackoverflow.com/questions/22514855/arrow-operator-in-function-heading
auto scan(string)->vector<Token>;

vector<string> split(string input, char delimiter) {
    vector<string> answer;
    stringstream ss(input);
    string temp;
 
    while (getline(ss, temp, delimiter)) {
        answer.push_back(temp);
    }
 
    return answer;
}

auto main()->void {
  string sourceCode = R""""(
    function main() {
      console.log('hello world!')
    }
  )"""";

  auto tokenList = scan(sourceCode);
}

