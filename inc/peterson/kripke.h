#ifndef PETERSON_KRIPKE_H
#define PETERSON_KRIPKE_H

#include "peterson/iterator.h"

class PetersonKripke: public spot::kripke
{
    private:
        const size_t _N;
        bdd* crit;
    public:
    PetersonKripke(size_t n, const spot::bdd_dict_ptr& d)
        : spot::kripke(d), _N(n)
    {
        crit = new bdd[_N];

        for(size_t i = 0; i < _N; i++) {
            std::string name = "crit";
            name += '0' + i;
            crit[i] = bdd_ithvar( register_ap(name) );
        }
    }

    ~PetersonKripke() {
        delete[] crit;
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

        bool* in_crit = state->getInCrit(_N);
        bdd condition = in_crit[0] ? crit[0] : !crit[0];

        for(size_t i = 1; i < _N; i++)
            condition &= in_crit[i] ? crit[i] : !crit[i];

        delete[] in_crit;
        return condition;
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
        out << " ]" << std::endl
            << "last_to_enter = [ ";
        for(auto i : *(state->getLTE())) out << (long unsigned)i << ", ";
        out << " ]" << std::endl;

        return out.str();
    }
};

#endif
