#include "cross_product.h"

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

void CrossProduct::print_trans(state_pair from, state_pair to) {
    std::cout << "<" << A_->format_state(from.a_) << ", " << B_->format_state(from.b_)
              << "> --> <"
              << A_->format_state(to.a_) << ", " << B_->format_state(to.b_) << '>' << std::endl;
}

void CrossProduct::computeInitial() {
    while(!I.empty()) {
        I.top().a_->destroy();
        I.top().b_->destroy();
        I.pop();
    }

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

        if(!placed) //A_0 is never managed by a unicity table, destroy manually
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

void CrossProduct::increment(spot::twa_succ_iterator* A_it, spot::twa_succ_iterator* B_it) {
    if(!B_it->next()) { //finished inner loop
        if(A_it->next()) { //continue outer loop
            //reset inner loop
            assert(B_it->first());
            //NOTE first() should be true for any iterator inserted by CrossProduct::visit()
        }
    }
}

void CrossProduct::crosspop(std::stack< state_pair > &state_stack,
                            std::stack< iter_pair > &it_stack) {
    spot::twa_succ_iterator* A_it = it_stack.top().first;
    spot::twa_succ_iterator* B_it = it_stack.top().second;

    A_->release_iter(A_it); B_->release_iter(B_it);
    state_stack.pop(); it_stack.pop();
}

void CrossProduct::visit(   const spot::state* a, const spot::state* b,
                            unicity_table_base &visited,
                            std::stack< state_pair > &state_stack,
                            std::stack< iter_pair > &it_stack) {

    state_pair cross_state = visited.is_new(a, b);

    if( cross_state.a_ && cross_state.b_ ) { //was not visited before
        spot::twa_succ_iterator* A_it = A_->succ_iter(cross_state.a_ );
        spot::twa_succ_iterator* B_it = B_->succ_iter(cross_state.b_);

        if(A_it->first() && B_it->first()) {
            state_stack.emplace(cross_state);
            it_stack.emplace(A_it, B_it);
        }
        else {
            A_->release_iter(A_it);
            B_->release_iter(B_it);
        }
    }
}

bool CrossProduct::cycle(const spot::state* s_acc, const spot::state* q_acc) {
    visit(s_acc, q_acc, seen_cycle, C, C_it);

    while(!C.empty()) {
        spot::twa_succ_iterator* A_it = C_it.top().first;

        spot::twa_succ_iterator* B_it = C_it.top().second;

        if(A_it->done()) {
            crosspop(C, C_it);
        }
        else {
            const spot::state* t = A_it->dst();
            const spot::state* p = B_it->dst();

            bdd L_t = A_->state_condition(t);
            bdd q_p = B_it->cond();

            increment(A_it, B_it);

            if( (L_t & q_p) != bddfalse ) {
                state_pair_equal eq; //equality functor
                //check if we loop back to s_acc, q_acc or a predecessor
                if( eq( {s_acc, q_acc}, {t, p} ) || seen_marked.get_mark(t,p)) {
                    C.push(seen_marked(t, p));
                    return true;
                }

                visit(t, p, seen_cycle, C, C_it);
            }
            else {
                t->destroy();
                p->destroy();
            }// L_t & q_p == bddfalse
        }// !done()
    }

    return false;
}

void CrossProduct::crossDFS() {
    computeInitial();

    while(!I.empty()) {
        visit(I.top().a_, I.top().b_, seen_marked, S, S_it);
        I.pop();

        while(!S.empty()) {
            assert(S.size() == S_it.size());

            spot::twa_succ_iterator* A_it = S_it.top().first;
            spot::twa_succ_iterator* B_it = S_it.top().second;

            //NOTE current state is <s,q>
            const spot::state* s = S.top().a_;
            const spot::state* q = S.top().b_;

            if(A_it->done()) {
                /*  NOTE all successors in 's' x 'q' visited */
                crosspop(S, S_it);
            }// done()
            else {
                /*  NOTE computing successors to <s,q> */

                //TODO check if <t,p> (in 'next(s) x next(q)') is in next(<s,q>)
                const spot::state* t = A_it->dst();
                const spot::state* p = B_it->dst();

                //TODO if( <t,p> (in 'next(s) x next(q)') is in next(<s,q>) )
                    //visit(t, p, seen_marked, S, S_it);

                //advances loop over 'next(s) X next(q)'
                increment(A_it, B_it);

            }// !done()
        }// while S
    }// while I
}

bool CrossProduct::accept() {
    //possible crossover between tables, prevent double destruction of states
    seen_cycle.set_co_table(&seen_marked);

    computeInitial();

    while(!I.empty()) {
        visit(I.top().a_, I.top().b_, seen_marked, S, S_it);
        I.pop();

        while(!S.empty()) {
            assert(S.size() == S_it.size());

            // Transistion from A_
            spot::twa_succ_iterator* A_it = S_it.top().first;
            const spot::state* s = S.top().a_;
            //Transition from B_
            spot::twa_succ_iterator* B_it = S_it.top().second;
            const spot::state* q = S.top().b_;

            if(A_it->done()) { //outer loop as finished, we pop this state
                if( B_->state_is_accepting(q) )
                    if(cycle(s, q))
                        return true;

                crosspop(S, S_it);
                seen_marked.mark(s,q, false); //unmark <s,q>, it is no longer on stack
            }// done()
            else {
                const spot::state* t = A_it->dst();
                const spot::state* p = B_it->dst();

                //Get Label for t and transition label for q->p
                bdd L_t = A_->state_condition(t);
                bdd q_p = B_it->cond();

                //check if Label(t) matches Label(q->p)
                if( (L_t & q_p) != bddfalse )
                    visit(t, p, seen_marked, S, S_it);
                else { //states are not managed by unicity table, destroy manually
                    t->destroy();
                    p->destroy();
                }// L_t & q_p == bddfalse

                //advances loop over 'next(s) X next(q)'
                increment(A_it, B_it);
            }// !done()
        }// while S
    }// while I

    return false;
}

void CrossProduct::trace() {
    std::stack< state_pair > s = S;
    std::stack< state_pair > c = C;
    std::stack< state_pair > s_print;
    std::stack< state_pair > c_print;

    state_pair link;
    if(!c.empty()) {
        link.a_ = c.top().a_;
        link.b_ = c.top().b_;
    }

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
    state_pair_equal eq; //equality functor
    std::cerr << "######################\n\tPath:\n######################\n";
    while(!s_print.empty()) {
        const spot::state* s = s_print.top().a_;

        if( eq(link, s_print.top()) )
            std::cout << "\t\t\t\t\t\t<---- Cycle loops back here!" << std::endl;
        std::cout << A_->format_state(s) << " | ";
        spot::bdd_print_formula(std::cout, A_->get_dict(), A_->state_condition(s));
        std::cout << std::endl << "\t------>" << std::endl;

        s_print.pop();
    }
    std::cout << "######################\n\tCycle to Path\n######################\n";
    while(!c_print.empty()) {
        const spot::state* s = c_print.top().a_;

        std::cout << A_->format_state(s) << " | ";
        spot::bdd_print_formula(std::cout, A_->get_dict(), A_->state_condition(s));
        std::cout << std::endl << "\t------>" << std::endl;

        c_print.pop();
    }

}
