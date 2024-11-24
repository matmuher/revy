#include <cstring> // memcpy
#include <iostream> // cerr
#include "Method.hpp"

Local Executor::BadLocal = {};

void Executor::Execute() {
    // check methods_ is not empty

    Execute(StartMethodIdx, 0, 0);
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


using OpcodeFunc = std::function<void(Executor&, Operand)>;
OpcodeFunc GetOpcodeFunc(Instruction::Opcode opcode);

Local Executor::Execute(int methodIdx, int rangeStart, int rangeEnd) {
    // check methodIdx is in bounds
    
    Method& method = methods_[methodIdx];
    Frame oldFrame = AllocateLocals(method.localNumber);

    for (int idx = 0; idx < method.instructionsNumber; ++idx) {
        auto opcodeFunc = GetOpcodeFunc(method.instructions[idx].opcode);
        auto operand = method.instructions[idx].operand;
        opcodeFunc(*this, operand);
    }

    FreeLocals(oldFrame);

    return ret;
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

Method::Method(Instruction* instructions, int instructionsNumber) {
    this->instructions = new Instruction[instructionsNumber];
    
    std::memcpy(this->instructions,
                instructions,
                sizeof(instructions[0]) * instructionsNumber);

    this->instructionsNumber = instructionsNumber;
}

OpcodeFunc GetOpcodeFunc(Instruction::Opcode opcode) {
    
    switch(opcode) {
        case Instruction::Opcode::Mul:
            return [](Executor& executor, Operand operand) {
                        Operand3 ops = std::get<Operand3>(operand);
                        executor[ops.rd] = executor[ops.lhs] * executor[ops.rhs];
                        std::cerr << "Mul: " << ops.rd << ops.lhs << ops.rhs << '\n';
                    };

        case Instruction::Opcode::Cmpg:
            return [](Executor& executor, Operand operand) {
                        Operand3 ops = std::get<Operand3>(operand);
                        executor[ops.rd] = executor[ops.lhs] > executor[ops.rhs];
                        std::cerr << "Cmpg: " << ops.rd << ops.lhs << ops.rhs << '\n';
                    };

        case Instruction::Opcode::Mov:
            return [](Executor& executor, Operand operand) {
                        Operand2 ops = std::get<Operand2>(operand);
                        executor[ops.rd] = executor[ops.rs];
                        std::cerr << "Mov: " << ops.rd << ops.rs << '\n';
                    };

        case Instruction::Opcode::Call:
            return [](Executor& executor, Operand operand) {
                        Operand4 ops = std::get<Operand4>(operand);
                        Local ret = executor.Execute(ops.methodIdx, ops.rangeStart, ops.rangeEnd);
                        executor[ops.rd] = ret;
                        std::cerr << "Call: " << ops.rd << ops.rangeStart << ops.rangeEnd << '\n';
                    };

        case Instruction::Opcode::Ret:
            return [](Executor& executor, Operand operand) {
                        Operand1 ops = std::get<Operand1>(operand);
                        executor.ret = executor[ops.r];
                        std::cerr << "Ret: " << ops.r << '\n';
                    };
        default:
            std::cerr << "Unkown opcode: " << (int) opcode << '\n';
            return [](Executor& executor, Operand operand) {
                        ; // set some error flag mb
                    };
    }
}
