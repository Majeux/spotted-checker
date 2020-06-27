#include "cross_product.h"

void CrossProduct::computeInitial() {
    initialStates.clear();
    const spot::state* A_0 = A_->get_init_state();
    const spot::state* B_0 = B_->get_init_state();

    //for all outgoing edges from B_0
    spot::twa_succ_iterator* B_it = B_->succ_iter(B_0);
    if(B_it->first()) {
        do {
            //add <A_0, q> if: B_0 --L(A_0)-> q
            if(A_->state_condition(A_0) & B_it->cond() != bddfalse )
                visit( A_0, B_it->dst() )

        } while(B_it->next());
    }

    /*  Alternative: easy on the eyes byt doesn't show Iterator functionality
        succ() creates an internally defined iterable
            for(auto B_it : B_->succ(B_0))
                if(A_->state_condition(A_0) & B_it->cond() != bddfalse )
                    initialStates.push_back( crossState(A_0, B_it->dst()) )
    */
}

void CrossProduct::crossVisit() {
    while(!S.empty()) {
        const spot::state* s = S.top().a();
        const spot::state* t = S_it.top().first->dst();

        const spot::state* q = S.top().b();
        const spot::state* p = S_it.top().second->dst();

        increment();

        if(A_->state_condition(t) & S_it.top().second->cond() != bddfalse)
            visit(t, p);
    }
}

CrossState CrossProduct::increment() {

    //inner
    if(!S_it.top().second->next()) { //state exhausted
        B_->release_iter(B_it);
        B_stack.pop();
    }

    //outer
    if(!A_it->next()) { //state exhausted
        A_->release_iter(A_it);
        A_stack.pop();
    }

}

bool CrossProduct::visit(const spot::state* a, const spot::state* b) {
    CrossState new_state = CrossState::unique(a, b);
    if( visited.insert(new_State).second ) { //insertion succeeded, not visited

        spot::twa_succ_iterator* A_it = A_->succ_iter(new_state.a()),
        spot::twa_succ_iterator* B_it = B_->succ_iter(new_state.b()));

        if(A_it->first() && B_it->first()) {
            S.emplace(new_state);
            S_it.emplace(A_it, B_it);
        }
        else {
            A_->release_iter(A_it);
            B_->release_iter(B_it);
        }
    }
}
