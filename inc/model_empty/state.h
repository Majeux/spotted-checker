#ifndef MODEL_STATE_H
#define MODEL_STATE_H

#include "model_abstract/state.h"
#include "model_empty/state_vars.h"

class ModelState : public AbstractState {
    private:
        /*  TODO
            Define variables that are invariant among or independent of a State
            here as normal.


            This class contains identifying variables (of type proc_t) for a state
            TODO define them in state_vars.h
            Access them through the 'AbstractIterator::state' member


            state_variables sv;
            Access to this struct is inherited from AbstractState
            Use underlying state variables as shown in aux.h */


    public:
        /*  TODO
            Return an exact copy of this state
        */
        ModelState* clone() const override;

        /*  TODO
            Possibly auxiliary methods
        */
};


#endif
