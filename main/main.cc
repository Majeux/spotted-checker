#include <iostream>
#include <functional>

#include "dbg.h"
#include "checker.h"
#include "peterson/kripke.h" //PetersonKripke
#include "model_example/kripke.h" //MyKripke

int main() {
    Checker checker;

    const proc N = 3;
    spot::bdd_dict_ptr dict = spot::make_bdd_dict();
    std::cout << "Peterson's with N = " << (long)N << "\n___________________" << std::endl;
    // auto pk = std::make_shared<PetersonKripke>(N, dict);
    auto pk = std::make_shared<MyKripke>(N, dict);
    // spot::print_dot(std::cout, pk);

    auto starvation = [=] (std::function<std::string(proc)> crit, std::function<std::string(proc)> wait) {
        std::ostringstream formula;
        formula << "G( ";
        for(proc i = 0; i < N; i++) { //starvation free: any process tthat starts waiting, gets access
            if(i > 0)
                formula << " || ";

            formula << "(" << wait(i) << " -> F("
                    << crit(i) << "))";
        }
        formula << " )";
        return formula.str();
    };

    auto bounded_waiting = [=] (std::function<std::string(proc)> crit, std::function<std::string(proc)> wait) {
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

    // std::cout << "MUTEX" << std::endl;
    // Checker::spotVerify( pk, mutex(critical) );
    //
    // std::cout << "STARVE" << std::endl;
    // Checker::spotVerify( pk, starvation(critical, waiting) );
    //
    // std::cout << "BOUNDED_WAITING" << std::endl;
    // Checker::spotVerify( pk, bounded_waiting(critical, waiting) );
    //
    // std::cout << "DOUBLE CRIT" << std::endl;
    // Checker::spotVerify( pk, "F(crit0 && crit 1)");
    //
    // std::cout << "------------------------" << std::endl
    //           << "MY TURN" << std::endl
    //           << "------------------------" << std::endl;

    // std::cout << "MUTEX" << std::endl;
    // Checker::myVerify( pk, mutex(critical) );
    //
    // std::cout << "STARVE" << std::endl;
    // Checker::myVerify( pk, starvation(critical, waiting) );
    Checker::myVerify( pk, "G(  (wait0 -> ( F(crit0 && !wait0) || (F(wait1 && F(crit1 && !wait1))) || (F(wait2 && F(crit2 && !wait2))) ) )"); //" && (wait1 -> ( F(crit0 && !wait0) || F(crit1 && !wait1) || F(crit2 && !wait2) ) ) && (wait2 -> ( F(crit0 && !wait0) || F(crit1 && !wait1) || F(crit2 && !wait2) ) ) )" );

    // Checker::myVerify( pk, "G( wait0 -> (wait0 W crit0) && wait1 -> (wait1 W crit1) && wait2 -> (wait2 W crit2)) )" );
    //
    // std::cout << "BOUNDED_WAITING" << std::endl;
    // Checker::myVerify( pk, bounded_waiting(critical, waiting) );
    //
    // std::cout << "DOUBLE CRIT" << std::endl;
    // Checker::myVerify( pk, "F(crit0 && crit 1)");

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
