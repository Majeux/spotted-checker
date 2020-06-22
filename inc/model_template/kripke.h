#ifndef TEMPLATE_KRIPKE_H
#define TEMPLATE_KRIPKE_H

#include "model_template/iterator.h"

class TemplateKripke: public spot::kripke {
    private:
        //TODO: bdd variables to check properties
    public:
        TemplateKripke(const spot::bdd_dict_ptr& d) : spot::kripke(d) {
            //register properties with bdds:
            bdd var = bdd_ithvar( register_ap("var_name") );
        }

        virtual TemplateIterator* makeIterator(const spot::state* s, bdd condition) const = 0;

        //recycling prevents delete/new overhead
        TemplateIterator* succ_iter(const spot::state* s) const override {
            const TemplateState* state = static_cast<const TemplateState*>(s);
            bdd condition = state_condition(state);

            if(iter_cache_) { //reuse old iterator if available
                TemplateIterator* i = static_cast<TemplateIterator*>(iter_cache_);
                iter_cache_ = nullptr;
                i->recycle(state, condition);

                return i;
            }

            return makeIterator(state, condition);
        }
};

#endif
