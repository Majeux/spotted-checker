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

#include "cross_product.h"
#include "aliases.h"

using State = uint32_t;

struct Edge { State from; State to; bdd cond; };

/*  Simple struct that can be used to explicitly define a kripke model
    by passing it to make_explicit(). Mostly for small models during testing */
struct model_info {
    /*  Number of states */
    State States;

    State Initial;

    std::vector<std::string>          Symbols;
    /*  For each state: for each symbol denote which ones are true */
    std::vector< std::vector<bool> >  Labels;
    /*  For each state: a vector of all states it has a transition to */
    std::vector< std::vector<State> > Transitions;

    friend std::ostream& operator << (std::ostream& os, const model_info& m);
};


class Checker {
    public:
        Checker();
        //Making explicit Kripke models
        static explicit_Kripke make_explicit(const model_info& m);
        static explicit_Kripke explicit_door_kripke();
        static const_Kripke explicit_traffic();
        static const_Kripke explicit_traffic2();

        //Making explicit property buchi automata
        static explicit_Automaton defineBuchi(const_Automaton model);
        static explicit_Automaton defineMutex3(const_Automaton model);
        static explicit_Automaton defineTrafficBuchi(const_Automaton model);

        //Model verification
        static void spotVerify(const_Kripke model, const std::string formula);
        static void spotVerify(const_Kripke model, explicit_Automaton neg_formula);
        static void myVerify(const_Kripke model, const std::string formula);
        static void myVerify(const_Kripke model, explicit_Automaton neg_formula);

    private:
        static State crossState(State a, State b);

        //Wraps spot functionalitiy for building explicit Buchi automata
        static explicit_Automaton initBuchi( const_Automaton model,
                                            unsigned n,
                                            State init);
        static explicit_Automaton buildBuchi(explicit_Automaton aut,
                                            const std::vector<State> &init,
                                            const std::vector<Edge> edges);
};

#endif
