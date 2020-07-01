#include "cross_product.h"
#include <dbg.h>
CrossProduct::CrossProduct(const_Kripke A, const_explicit_Automaton B) : A_(A), B_(B) { }

CrossProduct::~CrossProduct() {
    while(!I.empty()) {
        I.top().a_->destroy();
        I.top().b_->destroy();
        I.pop();
    }

    while(!S_it.empty() || !C_it.empty()) {
        if(!S_it.empty()) {
            A_->release_iter(S_it.top().first);
            B_->release_iter(S_it.top().second);
            S_it.pop();
        }
        if(!C_it.empty()) {
            A_->release_iter(C_it.top().first);
            B_->release_iter(C_it.top().second);
            C_it.pop();
        }
    }

}

void CrossProduct::computeInitial() {
    while(!I.empty()) {
        I.top().a_->destroy();
        I.top().b_->destroy();
        I.pop();
    }

    seen.clear();
    seen_marked.clear();
    seen_cycle.clear();

    const spot::state* B_0 = B_->get_init_state();
    spot::twa_succ_iterator* B_it = B_->succ_iter(B_0);

    //for all outgoing edges from B_0
    if(B_it->first()) {
        const spot::state* A_0 = A_->get_init_state();
        bool placed = false;

        do {
            //add <A_0, q> if: B_0 --L(A_0)-> q
            if( (A_->state_condition(A_0) & B_it->cond()) != bddfalse ) {
                placed = true;
                I.emplace(A_0, B_it->dst());
            }
        } while(B_it->next());

        if(!placed)
            A_0->destroy();
    }

    B_->release_iter(B_it);
    B_0->destroy();

    /*  Alternative: easy on the eyes byt doesn't show Iterator functionality
        succ() creates an internally defined iterable
            for(auto B_it : B_->succ(B_0))
                if(A_->state_condition(A_0) & B_it->cond() != bddfalse )
                    initialStates.push_back( crossState(A_0, B_it->dst()) )
    */
}

void CrossProduct::print_trans(state_pair from, state_pair to) {
    std::cerr << "<" << A_->format_state(from.a_) << ", " << B_->format_state(from.b_)
              << "> --> <"
              << A_->format_state(to.a_) << ", " << B_->format_state(to.b_) << '>' << std::endl;
}

bool CrossProduct::cycle(const spot::state* s_acc, const spot::state* q_acc) {
    visit(s_acc, q_acc, true);
    std::cerr << "cycle search" << std::endl;
    while(!C.empty()) {
        spot::twa_succ_iterator* A_it = C_it.top().first;
        const spot::state* s = C.top().a_;
        const spot::state* t = A_it->dst();

        spot::twa_succ_iterator* B_it = C_it.top().second;
        const spot::state* q = C.top().b_;
        const spot::state* p = B_it->dst();

        bdd L_t = A_->state_condition(t);
        bdd q_p = B_it->cond();

        increment(true);

        if( (L_t & q_p) != bddfalse ) {
            state_pair_equal eq; //equality functor

            if(eq( {s_acc, q_acc}, {t, p} )) { //looped back to s_acc, q_acc
                C.push(seen_marked(t, p));
                return true;
            }

            visit(t, p, true);
        }
    }

    return false;
}

bool CrossProduct::cycle_marked(const spot::state* s_acc, const spot::state* q_acc) {
    visit(s_acc, q_acc, true);

    while(!C.empty()) {
        spot::twa_succ_iterator* A_it = C_it.top().first;
        const spot::state* s = C.top().a_;

        spot::twa_succ_iterator* B_it = C_it.top().second;
        const spot::state* q = C.top().b_;

        if(A_it->done()) {
            A_->release_iter(A_it); B_->release_iter(B_it);
            C.pop(); C_it.pop();
        }
        else {
            const spot::state* t = A_it->dst();
            const spot::state* p = B_it->dst();

            bdd L_t = A_->state_condition(t);
            bdd q_p = B_it->cond();

            increment(true);

            if( (L_t & q_p) != bddfalse ) {
                state_pair_equal eq; //equality functor
                //check if we loop back to s_acc, q_acc or a predecessor
                if( eq( {s_acc, q_acc}, {t, p} ) || seen_marked.get_mark(t,p)) {
                    //TODO rm
                    // std::cerr << "TARGET: " << A_->format_state(s_acc) << B_->format_state(q_acc) << std::endl;
                    // std::cerr << "CLOSE: " << A_->format_state(t) << B_->format_state(p) << std::endl;
                    C.push(seen_marked(t, p));
                    return true;
                }

                visit(t, p, true);
            }
            else {
                t->destroy();
                p->destroy();
            }// L_t & q_p == bddfalse
        }// !done()
    }

    return false;
}

bool CrossProduct::operator()() {
    seen_cycle.set_co_table(&seen);
    computeInitial();

    while(!I.empty()) {
        visit(I.top().a_, I.top().b_);
        I.pop();

        while(!S.empty()) {
            assert(S.size() == S_it.size());
            /*  Read top of the stack */
            // Transistion from A_
            spot::twa_succ_iterator* A_it = S_it.top().first;
            const spot::state* s = S.top().a_;
            const spot::state* t = A_it->dst();
            //Transition from B_
            spot::twa_succ_iterator* B_it = S_it.top().second;
            const spot::state* q = S.top().b_;
            const spot::state* p = B_it->dst();

            //Get Label for t and transition label for q->p
            bdd L_t = A_->state_condition(t);
            bdd q_p = B_it->cond();

            //TODO mark <s,q>

            //NOTE  all states and iterators must be fetched from their stacks
            //      before incrementing.
            bool end_dfs = increment();


            //NOTE  check if Label(t) matches Label(q->p)
            //      fetch necessary information before incrementing!
            if( (L_t & q_p) != bddfalse ) //labels do not contradict
                visit(t, p);
            else {
                t->destroy();
                p->destroy();
            }
            //we popped the stacks and finished considering <s,q>
            if( end_dfs && B_->state_is_accepting(q) )
                if(cycle(s, q))
                    return true;
        }
    }

    return false;
}

bool CrossProduct::cross_marked() {
    seen_cycle.set_co_table(&seen_marked);
    computeInitial();

    while(!I.empty()) {
        visit_marked(I.top().a_, I.top().b_);
        I.pop();

        while(!S.empty()) {
            assert(S.size() == S_it.size());
            /*  Read top of the stack */
            // Transistion from A_
            spot::twa_succ_iterator* A_it = S_it.top().first;
            const spot::state* s = S.top().a_;
            //Transition from B_
            spot::twa_succ_iterator* B_it = S_it.top().second;
            const spot::state* q = S.top().b_;

            if(A_it->done()) { //outer loop as finished, we pop this state
                if( B_->state_is_accepting(q) )
                    if(cycle_marked(s, q))
                        return true;

                A_->release_iter(A_it); B_->release_iter(B_it);
                S.pop(); S_it.pop();
                //TODO rm
                // std::cerr << '\t' << A_->format_state(s) << B_->format_state(q) << std::endl;
                seen_marked.mark(s,q, false);
            }// done()
            else {
                const spot::state* t = A_it->dst();
                const spot::state* p = B_it->dst();

                //Get Label for t and transition label for q->p
                bdd L_t = A_->state_condition(t);
                bdd q_p = B_it->cond();

                //NOTE  all states and iterators must be fetched from their stacks
                //      before incrementing.
                increment();

                //NOTE  check if Label(t) matches Label(q->p)
                //      fetch necessary information before incrementing!
                if( (L_t & q_p) != bddfalse ) //labels do not contradict
                    visit_marked(t, p);
                else {
                    t->destroy();
                    p->destroy();
                }// L_t & q_p == bddfalse
            }// !done()
        }// while S
    }// while I

    return false;
}


/*  Advances the nested for loop over states ( A_ x B_ ) */
bool CrossProduct::increment(bool cycle) {
    std::stack< iter_pair >&    it_stack    = cycle ? C_it : S_it;

    spot::twa_succ_iterator* A_it = it_stack.top().first;
    spot::twa_succ_iterator* B_it = it_stack.top().second;

    if(!B_it->next()) { //finished inner loop

        if(A_it->next()) { //continue outer loop
            //reset inner loop
            assert(B_it->first());
            //NOTE first() should be true for any iterator inserted by CrossProduct::visit()
            return false; //continue exploring this state_pair (Depth First)
        }
        else { //finished outer loop
            //NOTE state memory is managed by the cross_unicity_table
            return true;
        }
    }

    return false; //continue exploring this state_pair (Depth First)
}

/*  Push new pairs of states and iterators on their respective stacks
    if the two states have not been visited before.
    Increment must be called before visit. */
void CrossProduct::visit(const spot::state* a, const spot::state* b, bool cycle) {
    cross_unicity_table&        visited     = cycle ? seen_cycle : seen;
    std::stack< state_pair >&   stack       = cycle ? C : S;
    std::stack< iter_pair >&    it_stack    = cycle ? C_it : S_it;

    state_pair cross_state = visited.is_new(a, b);
    if( cross_state.a_ && cross_state.b_ ) { //was not visited before

        spot::twa_succ_iterator* A_it = A_->succ_iter(cross_state.a_);
        spot::twa_succ_iterator* B_it = B_->succ_iter(cross_state.b_);

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

void CrossProduct::visit_marked(const spot::state* a, const spot::state* b, bool cycle) {
    std::stack< state_pair >&   stack    = cycle ? C : S;
    std::stack< iter_pair >&    it_stack = cycle ? C_it : S_it;
    unicity_table_base* visited;
    if(cycle)
        visited = &seen_cycle;
    else
        visited = &seen_marked;

    //TODO rm
    // std::cerr << A_->format_state(a) << std::endl;
    // std::cerr << B_->format_state(b) << std::endl;

    state_pair cross_state = visited->is_new(a, b);

    if( cross_state.a_ && cross_state.b_ ) { //was not visited before
        //TODO rm
        // std::cerr << "is_new\n_______________________" << std::endl;
        spot::twa_succ_iterator* A_it = A_->succ_iter(cross_state.a_ );
        spot::twa_succ_iterator* B_it = B_->succ_iter(cross_state.b_);

        if(A_it->first() && B_it->first()) {
            stack.emplace(cross_state);
            it_stack.emplace(A_it, B_it);
        }
        else {
            A_->release_iter(A_it);
            B_->release_iter(B_it);
        }
    }
    else {
        //TODO rm
        // std::cerr << "!is_new\n_______________________" << std::endl;
    }
}

void CrossProduct::trace() {
    std::stack< state_pair > s = S;
    std::stack< state_pair > c = C;
    std::stack< state_pair > s_print;
    std::stack< state_pair > c_print;

    while(!s.empty() || !c.empty()) {
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
        const spot::state* s = s_print.top().a_;

        std::cout << A_->format_state(s) << " | ";
        spot::bdd_print_formula(std::cout, A_->get_dict(), A_->state_condition(s));
        std::cout << std::endl << "\t------>" << std::endl;

        s_print.pop();
    }
    std::cout << "_____________________\nCycle\n";
    while(!c_print.empty()) {
        const spot::state* s = c_print.top().a_;

        std::cout << A_->format_state(s) << " | ";
        spot::bdd_print_formula(std::cout, A_->get_dict(), A_->state_condition(s));
        std::cout << std::endl << "\t------>" << std::endl;

        c_print.pop();
    }

}
