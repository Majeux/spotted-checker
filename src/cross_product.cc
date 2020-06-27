#include "cross_product.h"

CrossProduct::CrossProduct(const_Kripke A, const_Automaton B) : A_(A), B_(B) { computeInitial(); }

void CrossProduct::computeInitial() {
    const spot::state* A_0 = A_->get_init_state();
    const spot::state* B_0 = B_->get_init_state();

    //for all outgoing edges from B_0
    spot::twa_succ_iterator* B_it = B_->succ_iter(B_0);
    if(B_it->first()) {
        do {
            //add <A_0, q> if: B_0 --L(A_0)-> q
            if( (A_->state_condition(A_0) & B_it->cond()) != bddfalse )
                visit( A_0, B_it->dst() );

        } while(B_it->next());
    }

    B_->release_iter(B_it);

    /*  Alternative: easy on the eyes byt doesn't show Iterator functionality
        succ() creates an internally defined iterable
            for(auto B_it : B_->succ(B_0))
                if(A_->state_condition(A_0) & B_it->cond() != bddfalse )
                    initialStates.push_back( crossState(A_0, B_it->dst()) )
    */
}

void CrossProduct::print_trans(state_pair from, state_pair to) {
    std::cout << "<" << A_->format_state(from.first) << ", " << B_->format_state(from.second)
              << "> --> <"
              << A_->format_state(to.first) << ", " << B_->format_state(to.second) << '>' << std::endl;
}

void CrossProduct::operator()() {
    while(!S.empty()) {
        assert(S.size() == S_it.size());
        // Transistion from A_
        spot::twa_succ_iterator* A_it = S_it.top().first;
        const spot::state* s = S.top().first;
        const spot::state* t = A_it->dst();
        //Transition from B_
        spot::twa_succ_iterator* B_it = S_it.top().second;
        const spot::state* q = S.top().second;
        const spot::state* p = B_it->dst();
        //Get Label for t and transition label for q->p
        bdd L_t = A_->state_condition(t);
        bdd q_p = B_it->cond();

        //NOTE  all states and iterators must be fetched from their stacks
        //      before incrementing.
        increment();

        //NOTE  check if Label(t) matches Label(q->p)
        //      fetch necessary information before incrementing!
        if( (L_t & q_p) != bddfalse ) { //labels do not contradict
            /*  TODO <s,q> --> <t,p> is in the product
                Do some work */

            print_trans({s,q}, {t,p});
            visit(t, p);
        }
    }
}

/*  Advances the nested for loop over states ( A_ x B_ ) */
void CrossProduct::increment() {
    spot::twa_succ_iterator* A_it = S_it.top().first;
    spot::twa_succ_iterator* B_it = S_it.top().second;

    if(!B_it->next()) { //finished inner loop

        if(A_it->next()) { //continue outer loop
            //reset inner loop
            assert(B_it->first());
            //NOTE first should exist for any iterator inserted by CrossProduct::visit()
        }
        else { //finished outer loop
            A_->release_iter(A_it); B_->release_iter(B_it);
            S.pop(); S_it.pop();
        }
    }
}

/*  Push new pairs of states and iterators on their respective stacks
    if the two states have not been visited before.
    Increment must be called before visit. */
void CrossProduct::visit(const spot::state* a, const spot::state* b) {
    state_pair cross_state = seen.is_new(a, b);
    if( cross_state.first && cross_state.second ) { //was not seen before

        spot::twa_succ_iterator* A_it = A_->succ_iter(cross_state.first );
        spot::twa_succ_iterator* B_it = B_->succ_iter(cross_state.second);

        if(A_it->first() && B_it->first()) {
            S.emplace(cross_state);
            S_it.emplace(A_it, B_it);
        }
        else {
            A_->release_iter(A_it);
            B_->release_iter(B_it);
        }
    }
}
