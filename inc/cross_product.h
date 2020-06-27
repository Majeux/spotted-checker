#ifndef CROSS_PRODUCT_H
#define CROSS_PRODUCT_H

#include <spot/twa/twa.hh>

#include "aliases.h"
// #include "cross_state.h"
#include "cross_unicity_table.h"

class CrossProduct {
    private:
        /*  The two automata we compute the product of */
        const_Kripke A_;
        const_Automaton B_;

        //adaption of state_unicity_table that tracks seen state_pairs
        //initialialize any state_pair with seen(pair)
        cross_unicity_table seen;

        std::stack< state_pair > S;
        std::stack< iter_pair >  S_it;
        // std::set< CrossState > visited;

        void computeInitial();
        void increment();
        void visit(const spot::state* a, const spot::state* b);

    public:
        CrossProduct(const_Kripke A, const_Automaton B);

        void operator()();

};

#endif
