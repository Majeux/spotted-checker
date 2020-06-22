#ifndef TEMPLATE_KRIPKE_H
#define TEMPLATE_KRIPKE_H

#include "model_template/iterator.h"

/*  Abstract Class that implements a subset of the required virtual functions of
    spot::kripke that remain relatively constant through different implementations
    and are technical details rather than any pertaining to a model.
*/

class TemplateKripke: public spot::kripke {
    private:
        //TODO: bdd variables to check properties
    public:
        TemplateKripke(const spot::bdd_dict_ptr& d) : spot::kripke(d) {
            //register properties with bdds:
            bdd var = bdd_ithvar( register_ap("var_name") );
        }

        /*  TemplateIterator cannot be constructed since it is virtual
            This wrapper must be implemented in a child class to construct a new
            iterator
        */
        virtual TemplateIterator* makeIterator(const spot::state* s, bdd condition) const = 0;

        /*  Create an iterator over the successors of 's'
            Repurposes an old iterator from the iter_cache_ if it is available
            Otherwise create a new Iterator
        */
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
