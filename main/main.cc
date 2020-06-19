#include <iostream>

#include <spot/twaalgos/dot.hh>
#include <spot/tl/parse.hh>
#include <spot/twaalgos/translate.hh>
#include <spot/twaalgos/emptiness.hh>

#include "checker.h"
#include "peterson/kripke.h"

int main() {
    spot::bdd_dict_ptr dict = spot::make_bdd_dict();
    const proc N = 4;
    auto pk = std::make_shared<PetersonKripke>(N, dict);

    auto verify_starvation = [=] () {
        std::ostringstream formula;

        for(proc i = 0; i < N; i++) { //starvation free: any process tthat starts waiting, gets access
            if(i > 0)
                formula << " && ";

            formula << pk->waiting_string(i) << " -> F("
                    << pk->critical_string(i) << " && !" << pk->waiting_string(i) << ")";
        }
        return formula.str();
    };

    auto verify_mutex = [=] () {
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

    // Checker checker;
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
    std::string f_string = verify_mutex();

    spot::parsed_formula pf = spot::parse_infix_psl(f_string);
    spot::formula f = spot::formula::Not(pf.f);
    spot::twa_graph_ptr f_auto = spot::translator(dict).run(f);

    if(auto run = pk->intersecting_run(f_auto))
        std::cout << f_string << " violated by: \n" << *run << std::endl << std::endl;
    else
        std::cout << "verified:\n" << f_string << std::endl <<  std::endl;

    f_string = verify_starvation();

    pf = spot::parse_infix_psl(f_string);
    f = spot::formula::Not(pf.f);
    f_auto = spot::translator(dict).run(f);

    if(auto run = pk->intersecting_run(f_auto))
        std::cout << f_string << " violated by: \n __________________"
                  << *run << std::endl << std::endl;
    else
        std::cout << "verified:\n"<< f_string << std::endl << std::endl;

    // spot::print_dot(std::cout, pk);

    return 1;
}
