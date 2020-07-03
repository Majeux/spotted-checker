#ifndef CROSS_PRODUCT_H
#define CROSS_PRODUCT_H

#include <spot/twa/twa.hh>
#include <spot/twa/twagraph.hh>
#include <spot/twa/bddprint.hh>

#include <stack>

#include "aliases.h"
#include "cross_unicity_table.h"

using iter_pair  = std::pair<   spot::twa_succ_iterator*,
                                spot::twa_succ_iterator*    >;

class CrossProduct {
    private:
        /*  The two automata we compute the product of */
        const_Kripke A_;
        const_explicit_Automaton B_;

        /*  Maintain a marked_unicity_table to track a boolean flag that
            is true if the state_pair is on the stack, i.e. if there is a path
            from this state_pair to the top of the stack */
        marked_unicity_table seen_marked;
        /*  Unicity table for DFS through pairs of states */
        cross_unicity_table seen_cycle;

        /*  Stacks for arbitrary DFS walk */
        std::stack< state_pair > I;
        std::stack< state_pair > S;
        std::stack< iter_pair >  S_it;

        /*  Additional stacks for secondary DFS cycle check */
        std::stack< state_pair > C;
        std::stack< iter_pair >  C_it;

        /*  Writes a text representation of a transition of the crossproduct
            to std::cout */
        void print_trans(state_pair from, state_pair to);

        /*  Puts the initial states of the crossproduct into I*/
        void computeInitial();

        /*  Advances the two iterators to point to the next element form
            'next(s) X next(q)' (s and q are states corresponding to the
            iterators) */
        void increment(spot::twa_succ_iterator* A_it, spot::twa_succ_iterator* B_it);

        /*  Pop function for the cross-product, i.e. popping the state and
            iterator stack (and releasing the iterators) */
        void crosspop(  std::stack< state_pair > &state_stack,
                        std::stack< iter_pair > &it_stack   );

        /*  Push <a,b> and their iterators onto 'state_stack' and 'it_stack'
            if it is not in 'visited'*/
        void visit( const spot::state* a, const spot::state* b,
                    unicity_table_base       &visited,
                    std::stack< state_pair > &state_stack,
                    std::stack< iter_pair >  &it_stack  );

        /*  Performs a DFS, starting at <s_acc, q_acc>, and returns if there
            is a cycle through an accepting state */
        bool cycle(const spot::state* s_acc, const spot::state* q_acc);

    public:
        CrossProduct(const_Kripke A, const_explicit_Automaton B);
        ~CrossProduct();

        /*  TODO Template for DFS of the states in 'A_ X B_'
            Visits all states in the product if its todos are implemented */
        void crossDFS();

        /*  Perform a nested DFS, starting at the initial states, and return if
            it has a cycle through an accepting state */
        bool accept();

        /*  Writes the contents of the S and C stack to std::cout, representing
            a w-word in Language(A_ X B_) */
        void trace();

};

#endif
