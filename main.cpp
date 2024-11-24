#include <array>
#include "Method.hpp"

int main() {
/*
    Create 2 methods: main and factorial
    Execute them
*/

    Instruction* mainInstrs[] = {
        new Mul(),
        new Cmpg()
    };

    Method main{(Instruction**) mainInstrs, std::size(mainInstrs)};

    Method methods[] = {main};
    Executor executor{methods, std::size(methods)};

    executor.Execute();
}