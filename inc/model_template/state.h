#ifndef STATE_TEMPLATE_H
#define STATE_TEMPLATE_H

#include <spot/kripke/kripke.hh>

#include "aux.h"

/*  This abstract class implements some of the required abstract
    functions for a spot::state in order to simplify model implementation.
    The implemented functions are mostly independent from a model implementation. And with the proper use of the state_variables struct and "state_vars.h" file, they are in fact invariant.
*/
class TemplateState : public spot::state {
    protected:
        state_variables state;

    public:
        TemplateState(  proc_t var, proc_t initial_var,
                        proc_t  ar, singles_list initial_ar,
                        bound_list sizes) : state(var, initial_var,
                                                  ar, initial_ar,
                                                  sizes) { }

        TemplateState(const state_variables* sv) : state(sv) { }

        TemplateState(const state_variables &sv) : state(sv) { }

        //creates a hash based on the state_variables
        size_t hash() const override;

        // compare imposes a total order on the state_variables
        int compare(const spot::state* other) const override;

        const singles_list* getVars() const { return &(state.variables_); }
        const arrays_list*  getArrays() const { return &(state.arrays_); }
        const state_variables* getStateVars() const { return &state; }
};

#endif
