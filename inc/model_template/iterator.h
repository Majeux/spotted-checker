#ifndef TEMPLATE_IT_H
#define TEMPLATE_IT_H

#include "model_template/state.h"

class TemplateIterator: public spot::kripke_succ_iterator {
    protected:
        state_variables state;

    public:
        TemplateIterator(const TemplateState* s, bdd cond) :
            kripke_succ_iterator(cond), state(s->getStateVars()) { }

        //to avoid reallocating iterators
        void recycle(const TemplateState* s, bdd cond) {
            //other variables are invariant among iterators
            //or are auxiliary

            state.variables = *(s->getVars());
            state.arrays = *(s->getArrays());
            //indices should remain constant throughout states

            spot::kripke_succ_iterator::recycle(cond);
        }
};

#endif
