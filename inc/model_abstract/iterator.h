#ifndef ABSTRACT_IT_H
#define ABSTRACT_IT_H

#include "model_abstract/state.h"

/*  This abstract class implements some of the required abstract
    functions for spot::kripke_succ_iterator in order to simplify model
    implementation. This classes functionality connects to the
    AbstractKripke::succ_iter function to repurpose existing iterators.
*/
class AbstractIterator: public spot::kripke_succ_iterator {
    protected:
        state_variables sv;

    public:
        AbstractIterator(const AbstractState* s, bdd cond) :
            kripke_succ_iterator(cond), sv(s->getStateVars()) { }

        /*  Repurpose an existing iterator for another state
            Avoids additional new/delete
        */
        void recycle(const AbstractState* s, bdd cond) {
            //other variables are invariant among iterators
            //or are auxiliary

            sv.variables_ = s->getVars();
            sv.arrays_ = s->getArrays();
            //indices should remain constant throughout states

            spot::kripke_succ_iterator::recycle(cond);
        }
};

#endif
