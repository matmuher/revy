#include <array>
#include <iostream>
#include "Method.hpp"

enum class Methods {
    main = 0,
    fib,
    Number
};

int main() {
/*
    Create 2 methods: main and factorial
    Execute them
*/
    using enum Instruction::Opcode;

    Instruction mainInstrs[] = {
        {Mov, Operand2{ .rd = 0,
                        .val = 6}},

        {Call, Operand4{.rd = 0,
                        .methodIdx = (int) Methods::fib,
                        .rangeStart = 0,
                        .rangeEnd = 1}},

        {Ret, Operand1{ .r = 0}}
    };
    Method start{(Instruction*) mainInstrs, std::size(mainInstrs), 3};

    Instruction factInstr[] = {
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
    Method fib{(Instruction*) factInstr, std::size(factInstr), 10};

    Method methods[(int) Methods::Number] = {};
    methods[(int) Methods::main] = start;
    methods[(int) Methods::fib] = fib;

    Executor executor{methods, std::size(methods)};

    executor.Execute();

    std::cerr << executor.ret << '\n';
}