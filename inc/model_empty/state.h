#ifndef MODEL_STATE_H
#define MODEL_STATE_H

#include "model_template/state.h"
#include "model_empty/state_vars.h"

class ModelState : public TemplateState {
    private:
        /*  TODO
            Define variables that are invariant among or independent of a State
            here as normal.
        */

        /*  This class contains identifying variables (of type proc_t) for a state
            TODO define them in state_vars.h
            Access them through the 'TemplateIterator::state' member
        */

        /*  Access a variable 'NAME' with: */
                //state.singles[NAME] = 0;
                //proc_t x = state.singles[NAME] + 1;

        /*  Access an element 'i' in array 'NAME' with: */
                //state.arrays[NAME][i] = 0;
                //proc_t x = state.arrays[NAME][i] + 1;

        /*  Get the size pf array 'NAME' with: */
                //state.indices[NAME]

    public:
        /*  TODO
            Return an exact copy of this state
        */
        ModelState* clone() const override;

        /*  TODO
            Likely some methods to retrieve information from this state
        */
};


#endif
