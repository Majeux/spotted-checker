#ifndef CHECKER_H
#define CHECKER_H

#include <spot/kripke/kripkegraph.hh>
#include <spot/twaalgos/dot.hh>
#include <spot/twaalgos/hoa.hh>
#include <spot/twaalgos/translate.hh>
#include <spot/twaalgos/emptiness.hh>
#include <spot/tl/parse.hh>
#include <spot/twa/twa.hh>
#include <spot/twa/twaproduct.hh>

#include <iostream>
#include <fstream>
#include <sstream>
#include <vector>
#include <stack>
#include <algorithm>

#include "cross_product.h"
#include "aliases.h"

struct Edge { State from; State to; bdd cond; };

struct model_info {
    State States;
    State Initial;

    std::vector<std::string>          Symbols;
    std::vector< std::vector<bool> >  Labels;
    std::vector< std::vector<State> > Transitions;

    friend std::ostream& operator << (std::ostream& os, const model_info& m);
};


class Checker {
    public:
        Checker();

        //Making explicit Kripke models
        static bool read_kripke(std::string filename, model_info& model);
        static explicit_Kripke  make_explicit(const model_info& m);
        static explicit_Kripke  explicit_door_kripke();

        //Making explicit property buchi automata
        /* TODO Abstracts creation of buchi automaton with spot */
        static explicit_Automaton defineBuchi(const_Automaton model);
        /* Implementation of !mutex for 3 critical sections */
        static explicit_Automaton defineMutex3(const_Automaton model);

        static void verify(const_Automaton model, const std::string formula);

        /* Build cross product of a model and negated property */
        static explicit_Automaton cross(const_Automaton model, const_explicit_Automaton neg_property) {
            explicit_Automaton crossed = spot::make_twa_graph(model->get_dict());

            /*  unicity table tracks if some state has been seen before
                seen(state*):
                    if state* was seen before: destroy copy and return previous
                    if not: return state*
                seen.is_new(state*),
                    same but returns nullptr if seen before
                all tracked states are freed upon destruction */
            spot::state_unicity_table m_seen;
            spot::state_unicity_table p_seen;
            std::stack< std::pair<  const spot::state*,
                                    spot::twa_succ_iterator*>   > m_stack;
            std::stack< std::pair<  const spot::state*,
                                    spot::twa_succ_iterator*>   > p_stack;

            State model_i = 0; //the ith state of model
            State prop_i = 0;  //the ith state of property
            // crossed->set_init_state(crossState(model_i, prop_i));

            auto model_visit = [&](const spot::state* s) {
                if(m_seen.is_new(s)) {
                    model_i++; //number each state as we iterate through
                    spot::twa_succ_iterator* it  = model->succ_iter(s);
                    if(it->first())
                        m_stack.emplace(s, it);
                    else
                        model->release_iter(it);
                }
            };
            auto prop_visit = [&](const spot::state* s) {
                if(p_seen.is_new(s)) {
                    prop_i++; //number each state as we iterate through
                    spot::twa_succ_iterator* it  = neg_property->succ_iter(s);
                    if(it->first())

                        p_stack.emplace(s, it);
                    else
                        neg_property->release_iter(it);
                }
            };

            // State current_cross = crossed_get_init_state();
            model_visit(model->get_init_state());
            /*  iterate over model on-the-fly */
            while(!m_stack.empty()) {
                //recall these functions from ModelKripke
                const spot::state*          m_from = m_stack.top().first;
                spot::twa_succ_iterator*    m_it = m_stack.top().second;
                // const spot::state*          m_to = m_it->dst();

                while(!p_stack.empty()) {
                    const spot::state*          p_from = p_stack.top().first;
                    spot::twa_succ_iterator*    p_it = p_stack.top().second;
                    // const spot::state*          p_to = p_it->dst();

                    /* TODO cross it */
                    //buchi->is_accepting(buchi->state_number(state))
                    // neg_property->index_of_state(p_from);

                    if(!p_it->next()) { //state exhausted
                        neg_property->release_iter(p_it);
                        p_stack.pop();
                    }

                    prop_visit(p_from);
                }

                if(!m_it->next()) { //state exhausted
                    model->release_iter(m_it);
                    m_stack.pop();
                }

                model_visit(m_from);
            }

            return crossed;
        }

    private:
        /* create new state number from two state numbers */
        static State crossState(State a, State b);

        static explicit_Automaton initBuchi( const_Automaton model,
                                            unsigned n,
                                            State init);
        static explicit_Automaton buildBuchi(explicit_Automaton aut,
                                            const std::vector<State> &init,
                                            const std::vector<Edge> edges);

        /* dfs acceptance */
};

#endif
