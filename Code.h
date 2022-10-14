#ifndef CODE
#define CODE

#include <any>

using std::any;

enum class Instruction {
    Exit, 
    Call, Alloc, Var, Let, Const, Return,
    Jump, ConditionJump,
    Log,

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