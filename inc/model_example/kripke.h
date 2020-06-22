#ifndef EXAMPLE_KRIPKE_H
#define EXAMPLE_KRIPKE_H

#include "model_template/kripke.h"
#include "model_example/iterator.h"

class MyKripke: public TemplateKripke {
    private:
        const proc _N;
        bdd* crit; //is process i < _N in critical section (for verifying mutex)
        bdd* wait; //is process i < _N waiting to access critical section (for verifying non-starvation)

        TemplateIterator* makeIterator(const spot::state* s, bdd condition) const override;

    public:
        MyKripke(size_t n, const spot::bdd_dict_ptr& d);

        ~MyKripke();

        MyState* get_init_state() const override;

        bdd state_condition(const spot::state* s) const override;

        std::string format_state(const spot::state* s) const override;

        std::string critical_string(proc i) const;

        std::string waiting_string(proc i) const;
};

#endif
