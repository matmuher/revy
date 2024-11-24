#include <cstring> // memcpy
#include <iostream> // cerr
#include "Method.hpp"

Local Executor::BadLocal = {};

void Executor::Execute() {
    // check methods_ is not empty

    Execute(StartMethodIdx);
}

Frame Executor::AllocateLocals(int localsSize) {
    Frame oldFrame = currentFrame_;
    int newLocalsStart = oldFrame.start + oldFrame.size;
    currentFrame_ = Frame{newLocalsStart, localsSize};

    return oldFrame;
}

void Executor::FreeLocals(Frame oldFrame) {
    currentFrame_ = oldFrame;
}

void Executor::Execute(int methodIdx) {
    // check methodIdx is in bounds
    
    Method& method = methods_[methodIdx];
    Frame oldFrame = AllocateLocals(method.localNumber);

    for (int idx = 0; idx < method.instructionsNumber; ++idx) {
        method.instructions[idx]->Execute(*this);
    }

    FreeLocals(oldFrame);
}

Executor::Executor(Method* methods, int methodsNumber) {
    methods_ = new Method[methodsNumber];

    std::memcpy(methods_,
                methods,
                sizeof(methods[0]) * methodsNumber);

    methodsNumber_ = methodsNumber;

    locals_ = new Local[LocalsSize];
}

Local& Executor::operator[] (int localIdx) {
    int globalIdx = currentFrame_.start + localIdx;
    if (globalIdx > LocalsSize) {
        std::cerr << "Out of memory\n";
        return BadLocal; // callee check: if (&ret_val == &Executor::BadLocal) {/*error processing*/;}
    };

    return locals_[globalIdx];
}

Method::Method(Instruction** instructions, int instructionsNumber) {
    this->instructions = new Instruction*[instructionsNumber];
    
    std::memcpy(this->instructions,
                instructions,
                sizeof(instructions[0]) * instructionsNumber);

    this->instructionsNumber = instructionsNumber;
}

void Mul::Execute(Executor& executor) {
    std::cerr << __PRETTY_FUNCTION__ << '\n'; 
    executor[rd] = executor[lhs] * executor[rhs];
}

void Cmpg::Execute(Executor& executor) {
    std::cerr << __PRETTY_FUNCTION__ << '\n';
    executor[rd] = executor[lhs] > executor[rhs];
}