#ifndef MODEL_STATE_H
#define MODEL_STATE_H

#include "model_abstract/state.h"
#include "model_empty/state_vars.h"

class ModelState : public AbstractState {
    private:
        /*  TODO
            Define variables that are invariant among or independent of a State
            here as normal.
        */

        /*  This class contains identifying variables (of type proc_t) for a state
            TODO define them in state_vars.h
            Access them through the 'AbstractIterator::state' member
        */

        /*  Access a variable 'VAR_NAME' using the state_variables members:
                sv.singles[VAR_NAME] = 0;
                x = sv.singles[VAR_NAME] + 1;
            Or use the () operator:
                sv(VAR_NAME) = 0
                x = sv(VAR_NAME) + 1
        */


        /*  Access an element 'i' in array 'ARR_NAME' using state_variables members:
                sv.arrays[ARR_NAME][i] = 0;
                proc_t x = sv.arrays[ARR_NAME][i] + 1;
            Or use the () operator:
                sv(ARR_NAME, i) = 0
                x = sv(ARR_NAME, i) + 1
        */

        /*  Get a reference to the underlying vector of our 'ARR_NAME' with:
                std::vector<proc_t>& a = sv.data(ARR_NAME);
            Or:
                std::vector<proc_t>& a = state&ARRAY;

        /*  Get the size of array 'ARR_NAME' with:
                sv.bounds_[ARR_NAME]
            Or:
                sv.size(ARR_NAME)
        */


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
