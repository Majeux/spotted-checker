#ifndef EXAMPLE_KRIPKE_H
#define EXAMPLE_KRIPKE_H

#include "model_template/kripke.h"
#include "model_example/iterator.h"

class MyKripke: public TemplateKripke {
    private:
        const proc _N;
        bdd* crit; //is process i < _N in critical section (for verifying mutex)
        bdd* wait; //is process i < _N waiting to access critical section (for verifying non-starvation)

    public:
        MyKripke(size_t n, const spot::bdd_dict_ptr& d)
            : TemplateKripke(d), _N(n)
        {
            assert(_N > 0);
            crit = new bdd[_N];
            wait = new bdd[_N];

            for(size_t i = 0; i < (size_t)_N; i++) {
                crit[i] = bdd_ithvar( register_ap(critical_string(i)) );
                wait[i] = bdd_ithvar( register_ap(waiting_string(i)) );
            }
        }

        ~MyKripke() {
            delete[] crit;
            delete[] wait;
        }

        MyState* get_init_state() const override
        {
            return new MyState(_N);
        }

        TemplateIterator* makeIterator(const spot::state* s, bdd condition) const override {
            const MyState* state = static_cast<const MyState*>(s);
            return new MyIterator(state, condition);
        }

        bdd state_condition(const spot::state* s) const override
        {
            const MyState* state = static_cast<const MyState*>(s);

            const std::vector<proc>* p  = state->get(pc);
            const std::vector<proc>* lvl = state->get(level);

            bdd crit_condition = (*p)[0] == 4 ? crit[0] : !crit[0];
            bdd wait_condition = (*lvl)[0] > -1 && (*lvl)[0] < 4 ? wait[0] : !wait[0];

            for(size_t i = 1; i < (size_t)_N; i++) {
                crit_condition &= (*p)[i] == 4 ? crit[i] : !crit[i];
                wait_condition &= (*lvl)[i] > -1 && (*lvl)[i] < 4 ? wait[i] : !wait[i];
            }

            return crit_condition & wait_condition;
        }

        std::string format_state(const spot::state* s) const override
        {
            auto state = static_cast<const MyState*>(s);
            std::ostringstream out;
            const state_variables* sv = state->getStateVars();

            out << "pc = [ ";
            for(auto i : sv->arrays[pc] )
                out << (long int)i << ", ";
            out << " ]" << std::endl
                << "level = [ ";

            for(auto i : sv->arrays[level] )
                out << (long int)i << ", ";
            out << " ] "
                << "last_to_enter = [ ";

            for(auto i : sv->arrays[last_to_enter] )
                out << (long int)i << ", ";
            out << " ]" << std::endl;

            return out.str();
        }

        std::string critical_string(proc i) const {
            std::string critical = "crit";
            critical += '0' + i;
            return critical;
        }

        std::string waiting_string(proc i) const {
            std::string waiting = "wait";
            waiting += '0' + i;
            return waiting;
        }
};

#endif
