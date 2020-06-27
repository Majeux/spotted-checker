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

    /*  Alternative: easy on the eyes byt doesn't show Iterator functionality
        succ() creates an internally defined iterable
            for(auto B_it : B_->succ(B_0))
                if(A_->state_condition(A_0) & B_it->cond() != bddfalse )
                    initialStates.push_back( crossState(A_0, B_it->dst()) )
    */
}

void CrossProduct::operator()() {
    while(!S.empty()) {
        const spot::state* s = S.top().first;
        const spot::state* t = S_it.top().first->dst();

        const spot::state* q = S.top().second;
        const spot::state* p = S_it.top().second->dst();

        /* ... */
        std::cout << "< " << A_->format_state(s) << ", " << B_->format_state(q) << " >" << std::endl;
        /* ... */
        increment();

        if( (A_->state_condition(t) & S_it.top().second->cond()) != bddfalse )
            visit(t, p);
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

void CrossProduct::visit(const spot::state* a, const spot::state* b) {
    state_pair cross_state = seen.is_new(a, b);
    if( cross_state.first && cross_state.second ) { //was not seen before

        spot::twa_succ_iterator* A_it = A_->succ_iter(cross_state.first );
        spot::twa_succ_iterator* B_it = B_->succ_iter(cross_state.second);

        if(A_it->first() && B_it->first()) {
            S.emplace( cross_state );
            S_it.emplace(A_it, B_it);
        }
        else {
            A_->release_iter(A_it);
            B_->release_iter(B_it);
        }
    }
}
