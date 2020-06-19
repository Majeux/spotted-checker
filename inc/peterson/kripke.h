#ifndef PETERSON_KRIPKE_H
#define PETERSON_KRIPKE_H

#include "peterson/iterator.h"

class PetersonKripke: public spot::kripke
{
    private:
        const size_t _N;
        bdd* crit; //is process i < _N in critical section (for verifying mutex)
        bdd* wait; //is process i < _N waiting to access critical section (for verifying non-starvation)

    public:
    PetersonKripke(size_t n, const spot::bdd_dict_ptr& d)
        : spot::kripke(d), _N(n)
    {
        crit = new bdd[_N];
        wait = new bdd[_N];

        for(size_t i = 0; i < _N; i++) {
            std::string critical = "crit"; critical += '0' + i;
            std::string waiting  = "wait"; waiting  += '0' + i;

            crit[i] = bdd_ithvar( register_ap(critical) );
            wait[i] = bdd_ithvar( register_ap(waiting) );
        }
    }

    ~PetersonKripke() {
        delete[] crit;
        delete[] wait;
    }

    PetersonState* get_init_state() const override
    {
        return new PetersonState(_N);
    }

    //recycling prevents delete/new overhead
    PetersonIterator* succ_iter(const spot::state* s) const override {
        const PetersonState* state = static_cast<const PetersonState*>(s);
        bdd condition = state_condition(state);

        if(iter_cache_) { //reuse old iterator if available
            PetersonIterator* i = static_cast<PetersonIterator*>(iter_cache_);
            iter_cache_ = nullptr;
            i->recycle(state, condition);

            return i;
        }

        return new PetersonIterator(state, condition);
    }

    bdd state_condition(const spot::state* s) const override
    {
        const PetersonState* state = static_cast<const PetersonState*>(s);

        const std::vector<proc>* pc  = state->getPC();
        const std::vector<proc>* lvl = state->getLVL();

        bdd crit_condition = (*pc)[0] == 4 ? crit[0] : !crit[0];
        bdd wait_condition = (*lvl)[0] > -1 && (*lvl)[0] < 4 ? wait[0] : !wait[0];

        for(size_t i = 1; i < _N; i++) {
            crit_condition &= (*pc)[i] == 4 ? crit[i] : !crit[i];
            wait_condition &= (*lvl)[i] > -1 && (*lvl)[i] < 4 ? wait[i] : !wait[i];
        }

        return crit_condition & wait_condition;
    }

    std::string format_state(const spot::state* s) const override
    {
        auto state = static_cast<const PetersonState*>(s);
        std::ostringstream out;
        out << "pc = [ ";
        for(auto i : *(state->getPC())) out << (long int)i << ", ";
        out << " ]" << std::endl
            << "level = [ ";
        for(auto i : *(state->getLVL())) out << (long int)i << ", ";
        out << " ] "
            << "last_to_enter = [ ";
        for(auto i : *(state->getLTE())) out << (long unsigned)i << ", ";
        out << " ]" << std::endl;

        return out.str();
    }
};

#endif
