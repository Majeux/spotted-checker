#include <iostream>
#include <functional>

#include "checker.h"
#include "cross_product.h"
#include "peterson/kripke.h" //PetersonKripke
#include "model_example/kripke.h" //MyKripke

int main() {
    Checker checker;
    spot::bdd_dict_ptr dict = spot::make_bdd_dict();
    const proc N = 3;
    // auto pk = std::make_shared<PetersonKripke>(N, dict);
    // auto pk = std::make_shared<MyKripke>(N, dict);

    auto starvation = [=] (std::function<std::string(proc)> crit, std::function<std::string(proc)> wait) {
        std::ostringstream formula;

        for(proc i = 0; i < N; i++) { //starvation free: any process tthat starts waiting, gets access
            if(i > 0)
                formula << " && ";

            formula << wait(i) << " -> F("
                    << crit(i) << " && !" << wait(i) << ")";
        }
        return formula.str();
    };

    auto mutex = [=] (std::function<std::string(proc)> crit) {
        std::ostringstream formula;

        formula << "G( ";
        for(size_t i = 0; i < N; i++) {
            //turn off i
            formula << "(";
            for(size_t j = 0; j < N; j++) {
                if(j != i) {
                    formula << "!" << crit(j);
                    if(j < N-1 && !(j+1 == i && i == N-1) ) formula << " && ";
                    else formula << ")";
                }
            }

            if(i < N-1) formula << " || ";
            else formula << ")";
        }
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
    // checker.verify( pk, mutex(std::bind(pk->critical_string, std::placeholders::_1)) );

    // checker.verify(pk, starvation());

    // spot::print_dot(std::cout, checker.defineMutex3(pk));
    // spot::print_hoa(std::cout, checker.defineMutex3(pk));

    auto traffic = Checker::explicit_traffic();
    auto traffic_buchi = Checker::defineTrafficBuchi(traffic);

    spot::print_dot(std::cout, traffic);
    spot::print_dot(std::cout, traffic_buchi);

    CrossProduct cross(traffic, traffic_buchi);

    std::stack< state_pair > s = cross();

    std::cerr << s.size() << std::endl;

    return 1;
}
