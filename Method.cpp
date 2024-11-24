#include <cstring> // memcpy
#include <iostream> // cerr
#include "Method.hpp"

#define LOG(...) std::cerr << __VA_ARGS__ << '\n';
#define LOG(...) 

Local Executor::BadLocal = {};

void Executor::Execute(int N) {
    // check methods_ is not empty
    this->operator[](0) = N;
    Execute(StartMethodIdx, 0, 1);
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

        LOG("Copy arg: " << argIdx << ' ' << (this->operator[](argIdx)));
    }

    Frame oldFrame = AllocateLocals(method.localNumber);

    LOG("locals[0]: " << this->operator[](0));

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
                        LOG("Mul: " << ops.rd << ops.lhs << ops.rhs);
                    };

        case Instruction::Opcode::Add:
            return [](Executor& executor, Operand operand) {
                        Operand3 ops = std::get<Operand3>(operand);
                        executor[ops.rd] = executor[ops.lhs] + executor[ops.rhs];
                        LOG("Add: " << ops.rd << ops.lhs << ops.rhs);
                    };

        case Instruction::Opcode::Sub:
            return [](Executor& executor, Operand operand) {
                        Operand3 ops = std::get<Operand3>(operand);
                        executor[ops.rd] = executor[ops.lhs] - executor[ops.rhs];
                        LOG("Sub: " << ops.rd << ops.lhs << ops.rhs);
                    };

        case Instruction::Opcode::Cmpg:
            return [](Executor& executor, Operand operand) {
                        Operand3 ops = std::get<Operand3>(operand);
                        executor[ops.rd] = executor[ops.lhs] > executor[ops.rhs];
                        LOG("Cmpg: " << ops.rd << ops.lhs << ops.rhs);
                    };

        case Instruction::Opcode::Cmpge:
            return [](Executor& executor, Operand operand) {
                        Operand3 ops = std::get<Operand3>(operand);
                        executor[ops.rd] = (executor[ops.lhs] >= executor[ops.rhs]);
                        LOG(executor[ops.lhs] << ' ' << executor[ops.rhs]);
                        LOG("Cmpge: " << ops.rd << ops.lhs << ops.rhs);
                        LOG("Result: " << executor[ops.rd]); 
                    };

        case Instruction::Opcode::Mov:
            return [](Executor& executor, Operand operand) {
                        Operand2 ops = std::get<Operand2>(operand);
                        executor[ops.rd] = ops.val;
                        LOG(executor[ops.rd]);
                        LOG("Mov: " << ops.rd << ops.val);
                    };

        case Instruction::Opcode::Jmpt:
            return [](Executor& executor, Operand operand) {
                        Operand2 ops = std::get<Operand2>(operand);
                        if (executor[ops.rd] != 0) {
                            executor.currentInstruction += ops.val;
                        }
                        LOG("Jmpt: " << ops.rd << ops.val);
                    };

        case Instruction::Opcode::Jmp:
            return [](Executor& executor, Operand operand) {
                        Operand1 ops = std::get<Operand1>(operand);
                        executor.currentInstruction += ops.r;
                        LOG("Jmp: " << ops.r);
                    };

        case Instruction::Opcode::Call:
            return [](Executor& executor, Operand operand) {
                        Operand4 ops = std::get<Operand4>(operand);
                        LOG("Call: " << ops.rd << ops.methodIdx << ops.rangeStart << ops.rangeEnd);
                        Local ret = executor.Execute(ops.methodIdx, ops.rangeStart, ops.rangeEnd);
                        executor[ops.rd] = ret;
                    };

        case Instruction::Opcode::Ret:
            return [](Executor& executor, Operand operand) {
                        Operand1 ops = std::get<Operand1>(operand);
                        executor.ret = executor[ops.r];
                        LOG("Ret: " << ops.r << ", val = " << executor[ops.r]);
                    };
        default:
            std::cerr << "Unkown opcode: " << (int) opcode << '\n';
            return [](Executor& executor, Operand operand) {
                        ; // set some error flag mb
                    };
    }
}
