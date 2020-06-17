#include <iostream>

#include <spot/twaalgos/dot.hh>

#include "checker.h"
#include "peterson/kripke.h"

int main() {

    Checker checker;
    std::cerr << "Printing test kripke" << std::endl;
    spot::print_dot(std::cout, checker.explicit_door_kripke());

    std::cerr << std::endl << "Printing test kripke from file" << std::endl;
    model_info model;

    if(checker.read_kripke("kripkes/kripke-door", model))
        spot::print_dot(std::cout, checker.make_explicit(model));
    else
        std::cerr << "-- READ ERROR" << std::endl;

    std::cerr << std::endl << "-- done" << std::endl;
    return 1;
}
