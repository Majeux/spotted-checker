#ifndef MODEL_IT_H
#define MODEL_IT_H

#include "model_abstract/iterator.h"
#include "model_empty/state.h"

/*  This class will iterate over all outgoing transitions from a sv. The state we
    consider is defined by the pointer passed at construction.
*/
class ModelIterator : public AbstractIterator {
    private:
        /*  TODO
            Define variables that are invariant among states or independent
            of a state as normal.

            TODO define variables that uniquely identify a state in state_vars.h
            by listing their names

            NOTE from AbstractIterator:
            state_variables sv;
            Use underlying state variables as shown in inc/aux.h */

    public:
        ModelIterator(const ModelState* s, bdd cond) : AbstractIterator(s, cond) { }

        /*  TODO
            Initialize the iterator to consider the first edge from the state
            describe by sv (if it exists)
            Return true if there are any successors
        */
        bool first() override;

        /*  TODO
            Advance the iterator to output the next successor on the next dts() call
            Return true if there are any more successors afterwards */
        bool next() override;

        /*  TODO
            Return true if there are no more transitions to be considered
        */
        bool done() const override;

        /*  TODO
            Return a new state corresponding to the edge that
            the iterator is currently considering
            Deletion is the responsibility of the caller
        */
        AbstractState* dst() const override;
};

#endif
