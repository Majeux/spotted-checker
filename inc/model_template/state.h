#ifndef STATE_TEMPLATE_H
#define STATE_TEMPLATE_H

#include <spot/kripke/kripke.hh>

#include <array>

#include "model_example/state_vars.h"

class TemplateState : public spot::state {
    protected:
        state_variables state;

    public:
        TemplateState() { }
        TemplateState(proc v, proc a, singles_list i) : state(v, a, i) { }
        TemplateState(const state_variables* sv) : state(sv) { }

        //create a hash based on the state_variables
        size_t hash() const override;

        // compare must impose a total order on the state_variables
        int compare(const spot::state* other) const override;

        const singles_list* getVars() const { return &(state.variables); }
        const arrays_list*  getArrays() const { return &(state.arrays); }
        const state_variables* getStateVars() const { return &state; }
};

#endif
