#ifndef EXAMPLE_KRIPKE_H
#define EXAMPLE_KRIPKE_H

#include "model_abstract/kripke.h"
#include "model_example/iterator.h"

class MyKripke: public AbstractKripke {
    private:
        const proc_t _N;
        bdd* crit; //process is in critical section if pc[i] = 4
        bdd* wait; //process is waiting if pc[i] != 4 and level[i] < 0

        AbstractIterator* makeIterator(const spot::state* s, bdd condition) const override;

    public:
        MyKripke(size_t n, const spot::bdd_dict_ptr& d);

        ~MyKripke();

        MyState* get_init_state() const override;

        bdd state_condition(const spot::state* s) const override;

        std::string format_state(const spot::state* s) const override;

        static std::string critical_string(proc_t i);

        static std::string waiting_string(proc_t i);
};

#endif
