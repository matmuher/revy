#pragma once

#include <variant> // operand
#include <functional> // execute
#include <array>

/*
    Control execution of bytecode
    Allocate method memory
    Execute bytecode

    Contains:
        
        + stack frame pointer
        + method pointer
        + instruction pointer
*/


/*
    Storage for locals
*/
struct Frame {
    int start{};
    int size{};

    int currentMethod;
    int currentInstruction;

    int returnIdx;

    Frame* previousFrame;
};

class Method;
using Local = float;
class Instruction;

class Executor {
    static Local BadLocal;
    static int constexpr StartMethodIdx = 0;
    static int constexpr LocalsSize = 300;

    Local* locals_;
    Frame* currentFrame_;

    Method* methods_;
    int methodsNumber_;
    int currentMethod_;

    void AllocateLocals(int localsSize, int method, int retIdx);
    void FreeLocals();

public:

    Local ret;
    int currentInstruction;

    Local& operator[] (int localIdx);

    void Aller(int N = 5);

    Executor(Method* methods, int methodsNumber);
};

/*
    Storage of bytecode

    Contains:

        + bytecode (sequence of instructions)
        + number of used local variables
*/
struct Method {
    Instruction* instructions;
    int instructionsNumber;

    int localNumber;

    Method(Instruction* instructions, int instructionsNumber, int localNumber);
    Method() {};
};

struct Operand4 {
    int rd;
    int methodIdx;
    int rangeStart;
    int rangeEnd;
};

struct Operand3 {
    int rd;
    int lhs;
    int rhs;
};

struct Operand2 {
    int rd;
    int val;
};

struct Operand1 {
    int r;
};

using Operand = std::variant<Operand4, Operand3, Operand2, Operand1>;

/*
    Contains info about instruction execution
*/
struct Instruction {

    enum class Opcode {
        Mul = 0,
        Cmpg,
        Cmpge,
        Call,
        Ret,
        Mov,
        Add,
        Sub,
        Jmpt,
        Jmp,
        Number
    };

    Opcode opcode;
    Operand operand;
};
