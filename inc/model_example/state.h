#ifndef EXAMPLE_STATE_H
#define EXAMPLE_STATE_H

#include "model_abstract/state.h"
#include "model_example/state_vars.h"

class MyState : public AbstractState {
    private:
        /*  Universal/internal variables (among states) */
        const proc_t _N;

        /*  This class has access to the identifying variables (defined in state_vars.h) through the 'state_variables state' member in AbstractState */

        //Access variable 'NAME' with:               sv.singles[NAME]
        //Access element 'i' in array 'NAME' with:   sv.arrays[NAME][i]
        //Get the size pf array 'NAME' with:         sv.indices[NAME]

    public:
        MyState(bound_t n);
        MyState(const MyState* s);
        MyState(proc_t n, const state_variables &s, std::vector<assignment> &ass);

        //construct a new copy of this state
        MyState* clone() const override;

        proc_t getN() const { return _N; }
        const singles_list& get(ARRAY_VAR name) const { return sv &name; }
};


#endif
