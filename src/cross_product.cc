#include "cross_product.h"

CrossProduct::CrossProduct(const_Kripke A, const_explicit_Automaton B) : A_(A), B_(B) { }

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
    std::cerr << "<" << A_->format_state(from.first) << ", " << B_->format_state(from.second)
              << "> --> <"
              << A_->format_state(to.first) << ", " << B_->format_state(to.second) << '>' << std::endl;
}

bool CrossProduct::cycle(const spot::state* s_acc, const spot::state* q_acc) {
    visit(s_acc, q_acc, true);
    std::cerr << "cycle search" << std::endl;
    while(!C.empty()) {
        spot::twa_succ_iterator* A_it = C_it.top().first;
        const spot::state* s = C.top().first;
        const spot::state* t = A_it->dst();

        spot::twa_succ_iterator* B_it = C_it.top().second;
        const spot::state* q = C.top().second;
        const spot::state* p = B_it->dst();

        bdd L_t = A_->state_condition(t);
        bdd q_p = B_it->cond();

        increment(true);

        if( (L_t & q_p) != bddfalse ) {
            std::cerr << "cycle: \t";
            print_trans({s,q}, {t,p});
            state_pair_equal eq;

            if(eq( std::make_pair(s_acc, q_acc), std::make_pair(t, p) )) { //looped back to s_acc, q_acc
                C.push(seen_cycle(t, p));
                return true;
            }

            visit(t, p, true);
        }
    }

    return false;
}

std::stack<state_pair> CrossProduct::operator()() {
    seen.clear();
    seen_cycle.clear();

    computeInitial();

    while(!S.empty()) {
        assert(S.size() == S_it.size());
        /*  Read top of the stack */
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
        bool end_dfs = increment();

        //NOTE  check if Label(t) matches Label(q->p)
        //      fetch necessary information before incrementing!
        if( (L_t & q_p) != bddfalse ) { //labels do not contradict
            visit(t, p);
            /*  TODO <s,q> --> <t,p> is in the product
                Do some work */
            print_trans({s,q}, {t,p});

            if( end_dfs && B_->state_is_accepting(q) ) {
                if(cycle(s, q))
                    return S;
            }
        }
    }
    return S;
}

/*  Advances the nested for loop over states ( A_ x B_ ) */
bool CrossProduct::increment(bool cycle) {
    std::stack< state_pair >&   stack    = cycle ? C : S;
    std::stack< iter_pair >&    it_stack = cycle ? C_it : S_it;

    spot::twa_succ_iterator* A_it = it_stack.top().first;
    spot::twa_succ_iterator* B_it = it_stack.top().second;

    if(!B_it->next()) { //finished inner loop

        if(A_it->next()) { //continue outer loop
            //reset inner loop
            assert(B_it->first());
            //NOTE first should exist for any iterator inserted by CrossProduct::visit()
            return false; //continue exploring this state_pair (Depth First)
        }
        else { //finished outer loop
            A_->release_iter(A_it); B_->release_iter(B_it);
            stack.pop(); it_stack.pop();
            return true; //move onto next DFS
        }
    }

    return false; //continue exploring this state_pair (Depth First)
}

/*  Push new pairs of states and iterators on their respective stacks
    if the two states have not been visited before.
    Increment must be called before visit. */
void CrossProduct::visit(const spot::state* a, const spot::state* b, bool cycle) {
    cross_unicity_table&        visited  = cycle ? seen_cycle : seen;
    std::stack< state_pair >&   stack    = cycle ? C : S;
    std::stack< iter_pair >&    it_stack = cycle ? C_it : S_it;

    state_pair cross_state = visited.is_new(a, b);
    if( cross_state.first && cross_state.second ) { //was not visited before

        spot::twa_succ_iterator* A_it = A_->succ_iter(cross_state.first );
        spot::twa_succ_iterator* B_it = B_->succ_iter(cross_state.second);

        if(A_it->first() && B_it->first()) {
            stack.emplace(cross_state);
            it_stack.emplace(A_it, B_it);
        }
        else {
            A_->release_iter(A_it);
            B_->release_iter(B_it);
        }
    }
}

void CrossProduct::trace() {
    std::stack< state_pair > s = S;
    std::stack< state_pair > c = C;
    std::stack< state_pair > s_print;
    std::stack< state_pair > c_print;

    while(!s.empty() && !C.empty()) {
        if(!s.empty()) {
            s_print.push(s.top());
            s.pop();
        }
        if(!c.empty()) {
            c_print.push(c.top());
            c.pop();
        }
    }

    std::cerr << "Path:\n";
    while(!s_print.empty()) {
        std::cerr << A_->format_state(s_print.top().first) << std::endl;
        s_print.pop();
    }

    while(!c_print.empty()) {
        std::cerr << A_->format_state(c_print.top().first) << std::endl;
        c_print.pop();
    }

}
