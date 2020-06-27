#ifndef CROSS_PRODUCT_H
#define CROSS_PRODUCT_H

#include <spot/twa/twa.hh>
#include <spot/twa/twaproduct.hh>

#include "aliases.h"
// #include "cross_state.h"
#include "cross_unicity_table.h"

using ItPair = std::pair<   spot::twa_succ_iterator*,
                            spot::twa_succ_iterator*    >;

class CrossProduct {
    private:
        /*  The two automata we compute the product of */
        const_Automaton A_;
        const_Automaton B_;

        /*  seen(state*)    destroys the state if it was seen before and returns
                            a reference to the previous sight
            is_new(state*)  returns nullptre if seen before */
        // spot::state_unicity_table* A_seen;
        // spot::state_unicity_table* B_seen;
        cross_unicity_table seen;

        std::stack< state_pair > S;
        std::stack< ItPair > S_it;
        // std::set< CrossState > visited;

        void computeInitial();
        state_pair next();

    public:
        CrossProduct() {
            // A_seen = new spot::state_unicity_table();
            // B_seen = new spot::state_unicity_table();
            // CrossState::A_seen = A_seen;
            // CrossState::B_seen = B_seen;

            computeInitial();
        }

};

#endif
