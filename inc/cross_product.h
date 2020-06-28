#ifndef CROSS_PRODUCT_H
#define CROSS_PRODUCT_H

#include <spot/twa/twa.hh>
#include <spot/twa/twagraph.hh>

#include "aliases.h"
// #include "cross_state.h"
#include "cross_unicity_table.h"

class CrossProduct {
    private:
        /*  The two automata we compute the product of */
        const_Kripke A_;
        const_explicit_Automaton B_;

        //adaption of state_unicity_table that tracks seen state_pairs
        //initialialize any state_pair with seen(pair)
        cross_unicity_table seen;
        cross_unicity_table seen_cycle;

        std::stack< state_pair > S;
        std::stack< iter_pair >  S_it;

        std::stack< state_pair > C;
        std::stack< iter_pair >  C_it;
        // std::set< CrossState > visited;

        void print_trans(state_pair from, state_pair to);
        void computeInitial();
        bool increment(bool cycle = false);
        void visit(const spot::state* a, const spot::state* b, bool cycle = false);
        bool cycle(const spot::state* s, const spot::state* q);

    public:
        CrossProduct(const_Kripke A, const_explicit_Automaton B);

        std::stack<state_pair> operator()();

};

#endif
