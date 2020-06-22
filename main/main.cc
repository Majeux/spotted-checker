#include <iostream>

#include "checker.h"
#include "peterson/kripke.h"
#include "model_example/kripke.h"

int main() {
    Checker checker;
    spot::bdd_dict_ptr dict = spot::make_bdd_dict();
    const proc N = 4;
    auto pk = std::make_shared<PetersonKripke>(N, dict);
    // auto pk = std::make_shared<MyKripke>(N, dict);

    auto starvation = [=] () {
        std::ostringstream formula;

        for(proc i = 0; i < N; i++) { //starvation free: any process tthat starts waiting, gets access
            if(i > 0)
                formula << " && ";

            formula << pk->waiting_string(i) << " -> F("
                    << pk->critical_string(i) << " && !" << pk->waiting_string(i) << ")";
        }
        return formula.str();
    };

    auto mutex = [=] () {
        std::ostringstream formula;
        std::ostringstream formula2;

        formula << "G( (";
        for(size_t i = 0; i < N; i++) { //starvation free: any process tthat starts waiting, gets access
            if(i > 0) {
                formula  << " && ";
                formula2 << " xor ";
            }

            formula  << '!' << pk->critical_string(i);
            formula2 << pk->critical_string(i);
        }
        formula << ") || (" << formula2.str() << ") )";
        return formula.str();
    };

    // std::cerr << "Printing test kripke" << std::endl;
    // spot::print_dot(std::cout, checker.explicit_door_kripke());
    //
    // std::cerr << std::endl << "Printing test kripke from file" << std::endl;
    // model_info model;
    //
    // if(checker.read_kripke("kripkes/kripke-door", model))
    //     spot::print_dot(std::cout, checker.make_explicit(model));
    // else
    //     std::cerr << "-- READ ERROR" << std::endl;
    //
    // std::cerr << std::endl << "-- done" << std::endl;

    // std::string formula = "G( (!crit0 && !crit1) || crit0 xor crit1)"; //mutex
    checker.verify(pk, mutex());

    checker.verify(pk, starvation());

    spot::print_dot(std::cout, pk);

    return 1;
}
