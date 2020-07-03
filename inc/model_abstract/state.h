#ifndef STATE_ABSTRACT_H
#define STATE_ABSTRACT_H

#include <spot/kripke/kripke.hh>

#include "aux.h"

/*  This abstract class implements some of the required abstract
    functions for a spot::state in order to simplify model implementation.
    The implemented functions are mostly independent from a model implementation. And with the proper use of the state_variables struct and "state_vars.h" file, they are in fact invariant.
*/
class AbstractState : public spot::state {
    protected:
        state_variables sv;

    public:
        AbstractState(  proc_t var, proc_t initial_var,
                        proc_t  ar, singles_list initial_ar,
                        bound_list sizes) : sv(var, initial_var,
                                                  ar, initial_ar,
                                                  sizes) { }

        AbstractState(const state_variables* s) : sv(s) { }

        AbstractState(const state_variables &s) : sv(s) { }

        //creates a hash based on the state_variables
        size_t hash() const override;

        // compare imposes a total order on the state_variables
        int compare(const spot::state* other) const override;

        const singles_list& getVars() const { return sv.variables_; }
        const arrays_list&  getArrays() const { return sv.arrays_; }
        const state_variables& getStateVars() const { return sv; }
};

#endif
