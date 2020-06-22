#ifndef STATE_TEMPLATE_H
#define STATE_TEMPLATE_H

#include <spot/kripke/kripke.hh>

#include "aux.h"

class TemplateState : public spot::state {
    protected:
        state_variables state;

    public:
        TemplateState(  proc var, proc initial_var,
                        proc  ar, singles_list initial_ar,
                        singles_list sizes)
                            : state(var, initial_var, ar, initial_ar, sizes) { }

        TemplateState(const state_variables* sv) : state(sv) { }

        TemplateState(const state_variables &sv) : state(sv) { }

        //create a hash based on the state_variables
        size_t hash() const override;

        // compare must impose a total order on the state_variables
        int compare(const spot::state* other) const override;

        const singles_list* getVars() const { return &(state.variables); }
        const arrays_list*  getArrays() const { return &(state.arrays); }
        const state_variables* getStateVars() const { return &state; }
};

#endif
