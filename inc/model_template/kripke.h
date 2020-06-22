#ifndef TEMPLATE_KRIPKE_H
#define TEMPLATE_KRIPKE_H

#include "model_template/iterator.h"

/*  This abstract class implements some of the required abstract
    functions for spot::kripke in order to simplify model implementation.
    This class mainly handles the creation of state iterators, optimized using
    SPOT's iter_cache_
*/
class TemplateKripke: public spot::kripke {
    private:
        //TODO: bdd variables to check properties
    public:
        TemplateKripke(const spot::bdd_dict_ptr& d) : spot::kripke(d) { }

        /*  TemplateIterator cannot be constructed since it is virtual
            This wrapper around a constructor must be implemented in a child
            class to construct a new iterator
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
