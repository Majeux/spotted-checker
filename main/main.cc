#include <iostream>

#include <spot/twaalgos/dot.hh>
#include <spot/tl/parse.hh>
#include <spot/twaalgos/translate.hh>
#include <spot/twaalgos/emptiness.hh>

#include "checker.h"
#include "peterson/kripke.h"

int main() {

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
    spot::bdd_dict_ptr dict = spot::make_bdd_dict();

    std::string formula = "F(crit0)";
    // std::string formula = "G( (!crit0 && !crit1) || crit0 xor crit1)";

    spot::parsed_formula pf = spot::parse_infix_psl(formula);
    spot::formula f = spot::formula::Not(pf.f);
    spot::twa_graph_ptr f_auto = spot::translator(dict).run(f);

    auto pk = std::make_shared<PetersonKripke>(2, dict);
    if(auto run = pk->intersecting_run(f_auto))
        std::cout << formula << " violated by: \n" << *run << std::endl;
    else
        std::cout << formula << " verified" << std::endl;

    // spot::print_dot(std::cout, k);

    return 1;
}
