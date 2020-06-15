#include <iostream>

#include <spot/twaalgos/dot.hh>

#include "checker.h"

int main() {
    Checker checker;
    std::cerr << "Printing test kripke" << std::endl;
    spot::print_dot(std::cout, checker.explicit_door_kripke());

    std::cerr << std::endl << "Printing test kripke from file" << std::endl;
    model_info model;
    if(checker.read_kripke("kripkes/kripke-door", model)) {
        std::cerr << model << std::endl;

        spot::print_dot(std::cout, checker.make_explicit(model));
    }

    std::cerr << std::endl << "-- done" << std::endl;
    return 1;
}
