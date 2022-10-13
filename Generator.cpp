#define GLOBAL "global"
#include "Node.h"

static vector<Code> code_list;
static map<string, size_t> function_table;

auto generate(Program* program)->tuple<vector<Code>, map<string, size_t>> {
    write_code(Instruction::GetGlobal, string(GLOBAL));
    // global함수는 인자가 0개다
    write_code(Instruction::Call, static_cast<size_t>(0));
    write_code(Instruction::Exit);
    // program에는 항상 하나의 DeclareFunction만 존재하며, 그 값은 global FunctionExpression이다.
    // 모든 함수를 FunctionExpression으로 가정하는 이유는 js의 함수는 모두 값으로 취급되기 때문이다.
    // global함수는 사실 DeclareFunction으로 평가될 필요는 없으나, 모든 함수 선언을 동일하게 취급하기위하여 
    // global함수도 DeclareFunction으로 감싼후, 실제로 할당하지는 않은 채 그 내용을 즉시 실행한다.
    // 아래의 코드는 위의 설명이 표현된 것이다.
    program->functions[0]->function->generate();
    
    return {code_list, function_table};
}

// wirte_code는 단순히 명령어와 인자를 코드 리스트에 추가하고 추가된 인덱스를 반환할 뿐이다.
auto write_code(Instruction instruction)->size_t {
    code_list.push_back({instruction});
    return code_list.size() - 1;
}

auto write_code(Instruction instruction, any operand)->size_t {
    code_list.push_back({instruction, operand});
    return code_list.size() - 1;
}
/**
 * @brief Statement
 * */
// B.js에서는 모든 Function은 FunctionExpression이다. 그러므로 아래의 Statement를 상송한 Function은 사실상 사용될 일이 없다.
auto Function::generate()->void {
    // Function sturct를 generate할때 function table에 name prop으로 codelist의 size를 매핑
    // 함수의 이름과 주소(인덱스)를 매핑한다.6
    function_table[name] = code_list.size();
    // 함수를 실행한다
    for (auto& node:block) {
        node->generate();
    }
    // 모든 블락을 순회했다면 리턴생성
    write_code(Instruction::Return);
}
auto Return::generate()->void {

}
auto Declare::generate()->void {

}
auto For::generate()->void {

}
auto Break::generate()->void {

}
auto Continue::generate()->void {

}
auto If::generate()->void {

}
auto Console::generate()->void {
    if (consoleMethod == "log") {
        // 거꾸로 삽입하는 이유
        // 예를 들어, 1과 2를 출력한다고 가정해보자. 그렇다면 아래의 반복문은 2와 1순서로 넣는 것이다.
        // 이는 선형인 바이트코드 생성과 콜스택에서 기인하는데, log가 피연산자 스택을 사용하므로 맞는 순서로 스택 소비하는 순서는 반대가 된다.
        for (auto i = arguments.size(); i > 0; i--) {
            arguments[i - 1]->generate();
        }
        write_code(Instruction::Log, arguments.size());
    }
}
auto ExpressionStatement::generate()->void {
    expression->generate();
    // 식문은 소비 되는 척하기 위해 Statement로 감싸여져 있는 것이지 실제로 소비되는 것이 아니다.
    // 즉, 피연산자 스택에 값이 남는다. 이를 방지하기 위해 Pop한다
    write_code(Instruction::PopOperand);
}
/**
 * @brief Expression
 * */
auto Undefined::generate()->void {

}
auto Or::generate()->void {
    lhs->generate();
    auto or_ = write_code(Instruction::LogicalOr);
    rhs->generate();

}
auto And::generate()->void {
    
}
auto FunctionExpression::generate()->void {
    
}
auto DeclareFunction::generate()->void {
    
}
auto Relational::generate()->void {
    
}
auto Arithmetic::generate()->void {
   map<Kind, Instruction> instructions = {
    {Kind::Add,      Instruction::Add},
    {Kind::Subtract, Instruction::Subtract},
    {Kind::Multiply, Instruction::Multiply},
    {Kind::Divide,   Instruction::Divide},
    {Kind::Modulo,   Instruction::Modulo}
   };
   lhs->generate();
   rhs->generate();
   write_code(instructions[kind]); 
}
auto Unary::generate()->void {

}
auto Call::generate()->void {
    
}
auto GetElement::generate()->void {
    
}
auto SetElement::generate()->void {
    
}
auto GetVariable::generate()->void {
    
}
auto SetVariable::generate()->void {
    
}
auto NullLiteral::generate()->void {
    
}
auto BooleanLiteral::generate()->void {
    
}
auto NumberLiteral::generate()->void {
    
}
auto StringLiteral::generate()->void {
   write_code(Instruction::PushString, value); 
}
auto ArrayLiteral::generate()->void {
    
}
auto ObjectLiteral::generate()->void {
    
}
auto Method::generate()->void {
    
}