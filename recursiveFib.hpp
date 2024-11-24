#pragma once

#include "Method.hpp"

enum class Methods {
    main = 0,
    fib,
    Number
};

Method getRecursiveFib() {

    using enum Instruction::Opcode;


    Instruction recursiveFibInstr[] = {
        {Mov, Operand2{ .rd = 1,
                        .val = 2}},

        {Mov, Operand2{ .rd = 2,
                        .val = 1}},      

        {Cmpg, Operand3{.rd = 3,
                        .lhs = 0,
                        .rhs = 1}},

        {Jmpt, Operand2{.rd = 3,
                        .val = 1}},

        {Ret, Operand1{.r = 2}},

        {Mov, Operand2{ .rd = 4,
                        .val = -1}},   

        {Add, Operand3 { .rd = 5,
                            .lhs = 4,
                            .rhs = 0
        }},

        {Call, Operand4{.rd = 5,
                        .methodIdx = (int) Methods::fib,
                        .rangeStart = 5,
                        .rangeEnd = 6}},


        {Add, Operand3 { .rd = 6,
                            .lhs = 4,
                            .rhs = 0
        }},

        {Add, Operand3 { .rd = 6,
                            .lhs = 4,
                            .rhs = 6
        }},

        {Call, Operand4{.rd = 6,
                        .methodIdx = (int) Methods::fib,
                        .rangeStart = 6,
                        .rangeEnd = 7}},

        {Add, Operand3 {.rd = 7,
                        .lhs = 5,
                        .rhs = 6
        }},

        Instruction{Ret, Operand1{ .r = 7}}
    };
    Method recursiveFib{(Instruction*) recursiveFibInstr, std::size(recursiveFibInstr), 10};

    return recursiveFib;
}