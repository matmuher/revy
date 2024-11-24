#pragma once

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
};

class Method;
using Local = float;
class Instruction;

class Executor {
    static Local BadLocal;
    static int constexpr StartMethodIdx = 0;
    static int constexpr LocalsSize = 10;

    Local* locals_;
    Frame currentFrame_;

    Method* methods_;
    int methodsNumber_;
    int currentMethod_;
    int currentInstruction_;

    Frame AllocateLocals(int localsSize);
    void FreeLocals(Frame oldFrame);
    void Execute(int methodIdx);

public:

    Local& operator[] (int localIdx);

    void Execute();

    Executor(Method* methods, int methodsNumber);
};

/*
    Storage of bytecode

    Contains:

        + bytecode (sequence of instructions)
        + number of used local variables
*/
struct Method {
    Instruction** instructions;
    int instructionsNumber;

    int localNumber;

    Method(Instruction** instructions, int instructionsNumber);
    Method() {};
};

/*
    Contains info about instruction execution
*/
class Instruction {
public:
    virtual void Execute(Executor& executor) = 0;
};

struct Operand3 {
    int rd;
    int lhs;
    int rhs;
};

struct Mul : public Instruction, public Operand3 {
    void Execute(Executor& executor) override;
};

struct Cmpg : public Instruction, public Operand3 {
    void Execute(Executor& executor) override;
};


