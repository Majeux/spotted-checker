#ifndef PETERSON_KRIPKE_H
#define PETERSON_KRIPKE_H

#include "peterson/iterator.h"

class PetersonKripke: public spot::kripke
{
    private:
        const size_t _N;
        bdd* crit;

        bdd odd_x_;
    public:
    PetersonKripke(size_t n, const spot::bdd_dict_ptr& d)
        : spot::kripke(d), _N(n)
    {
        crit = new bdd[_N];
        odd_x_ = bdd_ithvar(register_ap("odd_x"));
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
        auto state = static_cast<const PetersonState*>(s);
        bdd condition = state_condition(ss);

        if(iter_cache) { //reuse old iterator if available
            auto i = static_cast<PetersonIterator*>(iter_cache);
            iter_cache = nullptr;
            i->recycle(state, condition);

            return i;
        }

        return new PetersonIterator(state, condition);
    }

    bdd state_condition(const spot::state* s) const override
    {
        //TODO
        // auto ss = static_cast<const PetersonState*>(s);
        return odd_x_;
    }

    std::string format_state(const spot::state* s) const override
    {
        auto ss = static_cast<const PetersonState*>(s);
        std::ostringstream out;
        // out << "(x = " << ss->get_x() << ", y = " << ss->get_y() << ')';
        return out.str();
    }
};

#endif
