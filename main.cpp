#include <array>
#include <iostream>
#include "Method.hpp"
#include "recursiveFib.hpp"
#include "loopFib.hpp"

int main() {

    using enum Instruction::Opcode;

    Instruction mainInstrs[] = {
        {Call, Operand4{.rd = 0,
                        .methodIdx = (int) Methods::fib,
                        .rangeStart = 0,
                        .rangeEnd = 1}},

        {Ret, Operand1{ .r = 0}}
    };
    Method start{(Instruction*) mainInstrs, std::size(mainInstrs), 3};

    Method methods[(int) Methods::Number] = {};
    methods[(int) Methods::main] = start;
    methods[(int) Methods::fib] = getLoopFib();
    Executor executor{methods, std::size(methods)};

    executor.Aller(6);
    std::cerr << "Loop fibbonachi(6) = " << executor.ret << '\n';

    methods[(int) Methods::fib] = getRecursiveFib();
    executor.Aller(7);
    std::cerr << "Recursive fibbonachi(7) = " << executor.ret << '\n';
}