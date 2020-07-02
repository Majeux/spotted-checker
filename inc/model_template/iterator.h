#ifndef TEMPLATE_IT_H
#define TEMPLATE_IT_H

#include "model_template/state.h"

/*  This abstract class implements some of the required abstract
    functions for spot::kripke_succ_iterator in order to simplify model
    implementation. This classes functionality connects to the
    TemplateKripke::succ_iter function to repurpose existing iterators.
*/
class TemplateIterator: public spot::kripke_succ_iterator {
    protected:
        state_variables state;

    public:
        TemplateIterator(const TemplateState* s, bdd cond) :
            kripke_succ_iterator(cond), state(s->getStateVars()) { }

        /*  Repurpose an existing iterator for another state
            Avoids additional new/delete
        */
        void recycle(const TemplateState* s, bdd cond) {
            //other variables are invariant among iterators
            //or are auxiliary

            state.variables_ = *(s->getVars());
            state.arrays_ = *(s->getArrays());
            //indices should remain constant throughout states

            spot::kripke_succ_iterator::recycle(cond);
        }
};

#endif
