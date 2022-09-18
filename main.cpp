#include <string>
#include <vector>
#include <sstream>
#include <iostream>
#include "Token.h"
#include "Main.h"

using std::string;
using std::vector;
// https://stackoverflow.com/questions/22514855/arrow-operator-in-function-heading

void interpret(Program*);

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

  vector<Token>  tokenList = scan(sourceCode);
  Program* syntaxTree = parse(tokenList);

  pintSyntaxTree(syntaxTree);
  interpret(syntaxTree);
}

