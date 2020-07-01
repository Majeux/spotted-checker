#ifndef CHECKER_H
#define CHECKER_H

#include <spot/kripke/kripkegraph.hh>
#include <spot/twaalgos/dot.hh>
#include <spot/twaalgos/hoa.hh>
#include <spot/twaalgos/translate.hh>
#include <spot/twaalgos/emptiness.hh>
#include <spot/tl/parse.hh>
#include <spot/twa/twa.hh>
#include <spot/twa/twaproduct.hh>

#include <iostream>
#include <fstream>
#include <sstream>
#include <vector>
#include <stack>
#include <algorithm>

#include "cross_product.h"
#include "aliases.h"

struct Edge { State from; State to; bdd cond; };

struct model_info {
    State States;
    State Initial;

    std::vector<std::string>          Symbols;
    std::vector< std::vector<bool> >  Labels;
    std::vector< std::vector<State> > Transitions;

    friend std::ostream& operator << (std::ostream& os, const model_info& m);
};


class Checker {
    public:
        Checker();
        //Making explicit Kripke models
        static bool read_kripke(std::string filename, model_info& model);
        static explicit_Kripke make_explicit(const model_info& m);
        static explicit_Kripke explicit_door_kripke();
        static const_Kripke explicit_traffic();
        static const_Kripke explicit_traffic2();

        //Making explicit property buchi automata
        static explicit_Automaton defineBuchi(const_Automaton model);
        /* Implementation of !mutex for 3 critical sections */
        static explicit_Automaton defineMutex3(const_Automaton model);

        static explicit_Automaton defineTrafficBuchi(const_Automaton model);

        static void spotVerify(const_Kripke model, const std::string formula);
        static void spotVerify(const_Kripke model, explicit_Automaton neg_formula);
        static void myVerify(const_Kripke model, const std::string formula);
        static void myVerify(const_Kripke model, explicit_Automaton neg_formula);

    private:
        static State crossState(State a, State b);

        static explicit_Automaton initBuchi( const_Automaton model,
                                            unsigned n,
                                            State init);
        static explicit_Automaton buildBuchi(explicit_Automaton aut,
                                            const std::vector<State> &init,
                                            const std::vector<Edge> edges);
};

#endif
