#ifndef CODE
#define CODE

#include <any>

using std::any;

enum class Instruction {
    Exit, 
    Call, Alloca, Return,
    Jump, ConditionJump,
    Console,

    LogicalOr, LogicalAnd,
    Add, Subtract,
    Multiply, Divide, Modulo,

    Equal, NotEqual,
    LessThan, GreaterThan,
    LessOrEqual, GreaterOrEqual,
    Absolute, ReverseSign,
    
    GetElement, SetElement,
    GetGlobal, SetGlobal,
    GetLocal, SetLocal,
                      
    PushNull, PushBoolean,
    PushNumber, PushString,
    PushArray, PushMap,
    PopOperand
};

struct Code {
    Instruction instruction;
    any operand;
};

#endif