#include <array>
#include "Method.hpp"

int main() {
/*
    Create 2 methods: main and factorial
    Execute them
*/

    Instruction mainInstrs[] = {
        Instruction{Instruction::Opcode::Mul, Operand3{0, 1, 0}},
        Instruction{Instruction::Opcode::Cmpg, Operand3{0, 0, 1}},
    };

    Method main{(Instruction*) mainInstrs, std::size(mainInstrs)};

    Method methods[] = {main};
    Executor executor{methods, std::size(methods)};

    executor.Execute();
}