#define GLOBAL "global"
#include "Node.h"

static vector<Code> code_list;
static map<string, size_t> function_table;

auto generate(Program* program)->tuple<vector<Code>, map<string, size_t>> {
    write_code(Instruction::GetGlobal, string(GLOBAL));
    // global함수는 인자가 0개다
    write_code(Instruction::Call, static_cast<size_t>(0));
    write_code(Instruction::Exit);
    for (auto& node:program->functions) {
        node->generate();
    }
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
auto Function::generate()->void {
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
    for (auto i = arguments.size(); i > 0; i--) {
        arguments[i - 1]->generate();
    }
    write_code(Instruction::Console, arguments.size());
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