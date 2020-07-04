#include <iostream>
#include <functional>

#include "checker.h"
#include "model_example/kripke.h"

int main() {
    Checker checker;

    const proc_t N = 3;
    spot::bdd_dict_ptr dict = spot::make_bdd_dict();
    std::cout << "Peterson's with N = " << (long)N << "\n___________________" << std::endl;

    auto pk = std::make_shared<MyKripke>(N, dict);

    // Paramtetrized LTL formula generation
    auto starvation = [=] (std::function<std::string(proc_t)> crit, std::function<std::string(proc_t)> wait, proc_t p) {
        std::ostringstream formula;

        formula << "G(  " << wait(p) << " -> ( F(" << crit(p) << " && !" << wait(p) << ")";

        for(proc_t i = 0; i < N; i++) { //starvation free: any proc_tess tthat starts waiting, gets access
            if(i != p)
                formula << " || F(" << wait(i) << " && F(" << crit(i) << " && !" << wait(i) << "))";
        }
        formula << " )  )";
        return formula.str();
    };

    auto bounded_waiting = [=] (std::function<std::string(proc_t)> crit, std::function<std::string(proc_t)> wait) {
        std::ostringstream formula;
        formula << "G( ";
        for(proc_t i = 0; i < N; i++) { //starvation free: any proc_tess tthat starts waiting, gets access
            if(i > 0)
                formula << " && ";

            formula << "(" << wait(i) << " -> F("
                    << crit(i) << "))";
        }
        formula << " )";
        return formula.str();
    };

    auto mutex = [=] (std::function<std::string(proc_t)> crit) {
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

    std::function<std::string(proc_t)> critical = std::bind(pk->critical_string, std::placeholders::_1);
    std::function<std::string(proc_t)> waiting  = std::bind(pk->waiting_string, std::placeholders::_1);

    std::cout << "------------------------" << std::endl
              << "------------------------" << std::endl
              << "Spot verification" << std::endl
              << "------------------------" << std::endl
              << "------------------------" << std::endl;

    std::cout << "MUTEX" << std::endl;
    Checker::spotVerify( pk, mutex(critical) );

    std::cout << "STARVE" << std::endl;
    for(proc_t i = 0; i < N; i++)
        Checker::spotVerify( pk, starvation(critical, waiting, i) );

    //NOTE this should only hold for N=2
    std::cout << "BOUNDED_WAITING" << std::endl;
    Checker::spotVerify( pk, bounded_waiting(critical, waiting) );

    //NOTE should never hold
    std::cout << "DOUBLE CRIT" << std::endl;
    Checker::spotVerify( pk, "F(crit0 && crit 1)");

    std::cout << "------------------------" << std::endl
              << "------------------------" << std::endl
              << "CrossProduct verification" << std::endl
              << "------------------------" << std::endl
              << "------------------------" << std::endl;

    std::cout << "MUTEX" << std::endl;
    Checker::myVerify( pk, mutex(critical) );

    std::cout << "STARVE" << std::endl;
    for(proc_t i = 0; i < N; i++)
        Checker::myVerify( pk, starvation(critical, waiting, i) );

    //NOTE this should only hold for N=2
    std::cout << "BOUNDED_WAITING" << std::endl;
    Checker::myVerify( pk, bounded_waiting(critical, waiting) );

    //NOTE should never hold
    std::cout << "DOUBLE CRIT" << std::endl;
    Checker::myVerify( pk, "F(crit0 && crit 1)");

    return 1;
}
