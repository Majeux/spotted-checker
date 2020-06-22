#ifndef MODEL_IT_H
#define MODEL_IT_H

#include "model_template/iterator.h"
#include "model_empty/state.h"

/*  This class will iterate over all outgoing edges from a state. The state we
    consider is defined by the pointer passed at construction.
*/
class ModelIterator : public TemplateIterator {
    private:
        /*  TODO
            Define variables that are invariant among or independent of a State
            here as normal.
        */

        /*  This class contains identifying variables (of type proc) for a state
            TODO define them in state_vars.h
            Access them through the 'TemplateIterator::state' member
        */

        /*  Access a variable 'NAME' with: */
                //state.singles[NAME] = 0;
                //proc x = state.singles[NAME] + 1;

        /*  Access an element 'i' in array 'NAME' with: */
                //state.arrays[NAME][i] = 0;
                //proc x = state.arrays[NAME][i] + 1;

        /*  Get the size pf array 'NAME' with: */
                //state.indices[NAME]

    public:
        ModelIterator(const ModelState* s, bdd cond) : TemplateIterator(s, cond) { }

        /*  TODO
            Initialize the iteratorto consider the first edge (if it exists)
            Return true if there are any successors
        */
        bool first() override;

        /*  TODO
            Advance the iterator by one
            Return true if there are any more successors afterwards */
        bool next() override;

        /*  TODO
            Return true if there are no more edges to be considered
        */
        bool done() const override;

        /*  TODO
            Return a new state corresponding to the edge that
            the iterator is currently considering
        */
        TemplateState* dst() const override;
};

#endif
