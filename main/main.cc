#include <iostream>
#include <functional>

#include "dbg.h"
#include "checker.h"
#include "peterson/kripke.h" //PetersonKripke
#include "model_example/kripke.h" //MyKripke

int main() {
    Checker checker;
    spot::bdd_dict_ptr dict = spot::make_bdd_dict();
    const proc N = 2;
    // auto pk = std::make_shared<PetersonKripke>(N, dict);
    auto pk = std::make_shared<MyKripke>(N, dict);

    auto starvation = [=] (std::function<std::string(proc)> crit, std::function<std::string(proc)> wait) {
        std::ostringstream formula;
        formula << "G( ";
        for(proc i = 0; i < N; i++) { //starvation free: any process tthat starts waiting, gets access
            if(i > 0)
                formula << " && ";

            formula << "(" << wait(i) << " -> F("
                    << crit(i) << "))";
        }
        formula << " )";
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

    std::function<std::string(proc)> critical = std::bind(pk->critical_string, std::placeholders::_1);
    std::function<std::string(proc)> waiting  = std::bind(pk->waiting_string, std::placeholders::_1);

    std::cout << "MUTEX" << std::endl;
    Checker::verify( pk, mutex(critical) );
    std::cout << "STARVE" << std::endl;
    Checker::verify( pk, starvation(critical, waiting) );
    // Checker::verify( pk, "G( wait0 -> F(crit0) )" );
    // Checker::verify( pk, "F(crit0 && crit 1)");

    // std::cout << "------------------------" << std::endl
    //           << "MY TURN" << std::endl
    //           << "------------------------" << std::endl;

    std::cout << "MUTEX" << std::endl;
    Checker::myVerify( pk, mutex(critical) );
    std::cout << "STARVE" << std::endl;
    // Checker::myVerify( pk, " G(wait0 -> F(crit0)) " );
    // spot::print_dot(std::cout, pk);
    Checker::myVerify( pk, starvation(critical, waiting) );
    // std::cout << "DOUBLE CRIT" << std::endl;
    // Checker::myVerify( pk, "F(crit0 && crit 1)");

    // spot::print_dot(std::cout, checker.defineMutex3(pk));
    // spot::print_hoa(std::cout, checker.defineMutex3(pk));

    // auto traffic = Checker::explicit_traffic();
    // auto traffic_buchi = Checker::defineTrafficBuchi(traffic);
    //
    // CrossProduct cross(traffic, traffic_buchi);
    //
    // std::stack< state_pair > s = cross();
    // //
    // std::cerr << s.size() << std::endl;
    // //
    // cross.trace();

    return 1;
}
