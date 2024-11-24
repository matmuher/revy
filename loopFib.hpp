#pragma once

#include "Method.hpp"

Method getLoopFib() {

    using enum Instruction::Opcode;

    static Instruction loopFibInstr[] = { // [0] = N
        {Mov, Operand2{ .rd = 1,
                        .val = 2}}, // [1] = 2

        {Mov, Operand2{ .rd = 2,
                        .val = 1}}, // [2] = 1

        {Cmpg, Operand3{.rd = 3,
                        .lhs = 0,
                        .rhs = 1}}, // N > 2 ?

        {Jmpt, Operand2{.rd = 3,
                        .val = 1}}, // if yes -> skip return

        {Ret, Operand1{.r = 2}}, // return 1

        {Mov, Operand2{ .rd = 4,
                        .val = 1}}, // prelast = 1

        {Mov, Operand2{ .rd = 5,
                        .val = 1}}, // last = 1
        
        {Sub, Operand3{ .rd = 6,
                        .lhs = 0,
                        .rhs = 1}}, // counter = N - 2

        {Cmpge, Operand3{.rd = 7,
                        .lhs = 6,
                        .rhs = 2}}, // counter >= 1 ?


        {Jmpt, Operand2{.rd = 7,
                        .val = 1}}, // if yes - skip ret

        {Ret, Operand1{ .r = 5}}, // ret last

        {Add, Operand3{ .rd = 5,
                        .lhs = 4,
                        .rhs = 5}}, // new_last = prelast + last

        {Sub, Operand3{.rd = 4,
                        .lhs = 5,
                        .rhs = 4}}, // prelast = new_last - prelast

        {Sub, Operand3{ .rd = 6,
                        .lhs = 6,
                        .rhs = 2}}, // counter--

        {Jmp, Operand1{.r = -7}} // jump to "counter >= 1 ?"
    };
    static Method loopFib{(Instruction*) loopFibInstr, std::size(loopFibInstr), 10};

    return loopFib;
}