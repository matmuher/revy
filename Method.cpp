#include <cstring> // memcpy
#include <iostream> // cerr
#include "Method.hpp"

#define LOG(...) std::cerr << __VA_ARGS__ << '\n';
#define LOG(...) 

Local Executor::BadLocal = {};

void Executor::Aller(int N) {

    currentFrame_ = new Frame{.start = 0, .size = 1};
    AllocateLocals(methods_[StartMethodIdx].localNumber, StartMethodIdx, 0); // allocate frame for main

    Executor& executor = *this;
    this->operator[](0) = N;

    while(true) {
        LOG("Current method: " << currentFrame_->currentMethod);
        LOG("Current instru: " << currentFrame_->currentInstruction);
        Method& method = methods_[currentFrame_->currentMethod];
     
        currentFrame_->currentInstruction += 1;
        int currentInstruction = currentFrame_->currentInstruction;

        // FUCK: Replace with fetch & decode
        Instruction instruction = method.instructions[currentInstruction];
        Instruction::Opcode opcode = instruction.opcode; 
        Operand operand = instruction.operand;

        switch(opcode) {
            case Instruction::Opcode::Mul:
                {
                    Operand3 ops = std::get<Operand3>(operand);
                    LOG("Mul: " << ops.rd << ops.lhs << ops.rhs);
                    executor[ops.rd] = executor[ops.lhs] * executor[ops.rhs];
                    break;
                };

            case Instruction::Opcode::Add:
                {
                    Operand3 ops = std::get<Operand3>(operand);
                    LOG("Add: " << ops.rd << ops.lhs << ops.rhs);
                    executor[ops.rd] = executor[ops.lhs] + executor[ops.rhs];
                    break;
                };

            case Instruction::Opcode::Sub:
                {
                    Operand3 ops = std::get<Operand3>(operand);
                    LOG("Sub: " << ops.rd << ops.lhs << ops.rhs);
                    executor[ops.rd] = executor[ops.lhs] - executor[ops.rhs];
                    break;
                };

            case Instruction::Opcode::Cmpg:
                {
                    Operand3 ops = std::get<Operand3>(operand);
                    LOG("Cmpg: " << ops.rd << ops.lhs << ops.rhs);
                    executor[ops.rd] = executor[ops.lhs] > executor[ops.rhs];
                    break;
                };

            case Instruction::Opcode::Cmpge:
                {
                    Operand3 ops = std::get<Operand3>(operand);
                    LOG("Cmpge: " << ops.rd << ops.lhs << ops.rhs);
                    executor[ops.rd] = (executor[ops.lhs] >= executor[ops.rhs]);
                    LOG(executor[ops.lhs] << ' ' << executor[ops.rhs]);
                    LOG("Result: " << executor[ops.rd]);
                    break;
                };

            case Instruction::Opcode::Mov:
                {
                    Operand2 ops = std::get<Operand2>(operand);
                    LOG("Mov: " << ops.rd << ops.val);
                    executor[ops.rd] = ops.val;
                    LOG(executor[ops.rd]);
                    break;
                };

            case Instruction::Opcode::Jmpt:
                {
                    Operand2 ops = std::get<Operand2>(operand);
                    LOG("Jmpt: " << ops.rd << ops.val);
                    if (executor[ops.rd] != 0) {
                        currentFrame_->currentInstruction += ops.val;
                    }
                    break;
                };

            case Instruction::Opcode::Jmp:
                {
                    Operand1 ops = std::get<Operand1>(operand);
                    LOG("Jmp: " << ops.r);
                    currentFrame_->currentInstruction += ops.r;
                    break;
                };

            case Instruction::Opcode::Call:
                {
                    Operand4 ops = std::get<Operand4>(operand);
                    LOG("Call: " << ops.rd << ops.methodIdx << ops.rangeStart << ops.rangeEnd);
                    
                    // copy range of args
                    for (int argIdx = ops.rangeStart; argIdx < ops.rangeEnd; argIdx++) {
                        executor[currentFrame_->size + (argIdx - ops.rangeStart)] = executor[argIdx];
                        LOG("Copy arg: " << argIdx << ' ' << (this->operator[](argIdx)));
                    }

                    AllocateLocals( methods_[ops.methodIdx].localNumber,
                                    ops.methodIdx,
                                    ops.rd); // allocate new frame

                    LOG("locals[0]: " << this->operator[](0));
                    break;
                };

            case Instruction::Opcode::Ret:
                {
                    Operand1 ops = std::get<Operand1>(operand);
                    LOG("Ret: " << ops.r);
                    executor.ret = executor[ops.r]; // save ret value
                    LOG("Val: " << executor.ret);
                    FreeLocals(); // restore frame

                    int returnIdx = currentFrame_->returnIdx;
                    executor[returnIdx] = executor.ret; // put ret value

                    if (currentFrame_->previousFrame == nullptr) {
                        LOG("End of execution");
                        return;
                    }

                    break;
                };

            default:
                std::cerr << "Unkown opcode: " << (int) opcode << '\n';
                {
                    ; // set some error flag mb
                };
        }
    }
}

void Executor::AllocateLocals(int localsSize, int method, int retIdx) {
    Frame* oldFrame = currentFrame_;
    oldFrame->returnIdx = retIdx; // where to put ret value

    int newLocalsStart = oldFrame->start + oldFrame->size;
    currentFrame_ = new Frame{  .start = newLocalsStart,
                                .size = localsSize,
                                .currentMethod = method,
                                .currentInstruction = -1,
                                .returnIdx = 0,
                                .previousFrame = oldFrame};
}

void Executor::FreeLocals() {
    Frame* deadFrame = currentFrame_;
    currentFrame_ = currentFrame_->previousFrame;
    delete deadFrame;
}

using OpcodeFunc = std::function<void(Executor&, Operand)>;
OpcodeFunc GetOpcodeFunc(Instruction::Opcode opcode);

Executor::Executor(Method* methods, int methodsNumber) {
    methods_ = new Method[methodsNumber];

    std::memcpy(methods_,
                methods,
                sizeof(methods[0]) * methodsNumber);

    methodsNumber_ = methodsNumber;

    locals_ = new Local[LocalsSize];
}

Local& Executor::operator[] (int localIdx) {
    int globalIdx = currentFrame_->start + localIdx;
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
