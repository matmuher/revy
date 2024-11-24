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

    // check that range is not wider than method parameters number
    for (int argIdx = rangeStart; argIdx < rangeEnd; argIdx++) {
        this->operator[](currentFrame_.size + (argIdx - rangeStart)) = this->operator[](argIdx);

        std::cerr << "Copy arg: " << argIdx << ' ' << (this->operator[](argIdx)) << '\n';
    }

    Frame oldFrame = AllocateLocals(method.localNumber);

    std::cerr << "locals[0]: " << this->operator[](0) << '\n';

    int oldCurrentInstruction = currentInstruction;

    currentInstruction = 0;

    while(true) {
        // check for infinity loop

        Instruction instruction = method.instructions[currentInstruction];

        auto opcodeFunc = GetOpcodeFunc(method.instructions[currentInstruction].opcode);
        auto operand = method.instructions[currentInstruction].operand;
        opcodeFunc(*this, operand);

        if (instruction.opcode == Instruction::Opcode::Ret) {
            break;
        }

        currentInstruction++;
    }

    currentInstruction = oldCurrentInstruction;
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
    // std::cerr << localIdx << " " << globalIdx << '\n';

    if (globalIdx > LocalsSize) {
        std::cerr << "Out of memory\n";
        exit(1);
        return BadLocal; // callee check: if (&ret_val == &Executor::BadLocal) {/*error processing*/;}
    };

    return locals_[globalIdx];
}

Method::Method( Instruction* instructions,
                int instructionsNumber,
                int localNumber) {
    this->instructions = new Instruction[instructionsNumber];
    
    std::memcpy(this->instructions,
                instructions,
                sizeof(instructions[0]) * instructionsNumber);

    this->instructionsNumber = instructionsNumber;
    this->localNumber = localNumber;
}

OpcodeFunc GetOpcodeFunc(Instruction::Opcode opcode) {
    
    switch(opcode) {
        case Instruction::Opcode::Mul:
            return [](Executor& executor, Operand operand) {
                        Operand3 ops = std::get<Operand3>(operand);
                        executor[ops.rd] = executor[ops.lhs] * executor[ops.rhs];
                        std::cerr << "Mul: " << ops.rd << ops.lhs << ops.rhs << '\n';
                    };

        case Instruction::Opcode::Add:
            return [](Executor& executor, Operand operand) {
                        Operand3 ops = std::get<Operand3>(operand);
                        executor[ops.rd] = executor[ops.lhs] + executor[ops.rhs];
                        std::cerr << "Add: " << ops.rd << ops.lhs << ops.rhs << '\n';
                    };

        case Instruction::Opcode::Cmpg:
            return [](Executor& executor, Operand operand) {
                        Operand3 ops = std::get<Operand3>(operand);
                        executor[ops.rd] = executor[ops.lhs] > executor[ops.rhs];
                        std::cerr << "Cmpg: " << ops.rd << ops.lhs << ops.rhs << '\n';
                    };

        case Instruction::Opcode::Cmpge:
            return [](Executor& executor, Operand operand) {
                        Operand3 ops = std::get<Operand3>(operand);
                        executor[ops.rd] = (executor[ops.lhs] >= executor[ops.rhs]);
                        std::cerr << executor[ops.lhs] << ' ' << executor[ops.rhs] << '\n';
                        std::cerr << "Cmpge: " << ops.rd << ops.lhs << ops.rhs << '\n';
                        std::cerr << "Result: " << executor[ops.rd] << '\n'; 
                    };

        case Instruction::Opcode::Mov:
            return [](Executor& executor, Operand operand) {
                        Operand2 ops = std::get<Operand2>(operand);
                        executor[ops.rd] = ops.val;
                        std::cerr << executor[ops.rd] << '\n';
                        std::cerr << "Mov: " << ops.rd << ops.val << '\n';
                    };

        case Instruction::Opcode::Jmpt:
            return [](Executor& executor, Operand operand) {
                        Operand2 ops = std::get<Operand2>(operand);
                        if (executor[ops.rd] != 0) {
                            executor.currentInstruction += ops.val;
                        }
                        std::cerr << "Jmpt: " << ops.rd << ops.val << '\n';
                    };

        case Instruction::Opcode::Call:
            return [](Executor& executor, Operand operand) {
                        Operand4 ops = std::get<Operand4>(operand);
                        std::cerr << "Call: " << ops.rd << ops.methodIdx << ops.rangeStart << ops.rangeEnd << '\n';
                        Local ret = executor.Execute(ops.methodIdx, ops.rangeStart, ops.rangeEnd);
                        executor[ops.rd] = ret;
                    };

        case Instruction::Opcode::Ret:
            return [](Executor& executor, Operand operand) {
                        Operand1 ops = std::get<Operand1>(operand);
                        executor.ret = executor[ops.r];
                        std::cerr << "Ret: " << ops.r << ", val = " << executor[ops.r] << '\n';
                    };
        default:
            std::cerr << "Unkown opcode: " << (int) opcode << '\n';
            return [](Executor& executor, Operand operand) {
                        ; // set some error flag mb
                    };
    }
}
