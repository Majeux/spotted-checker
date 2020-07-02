#ifndef EXAMPLE_STATE_H
#define EXAMPLE_STATE_H

#include "model_template/state.h"
#include "model_example/state_vars.h"

class MyState : public TemplateState {
    private:
        /*  Universal/internal variables (among states) */
        const proc_t _N;

        /*  This class has access to the identifying variables (defined in state_vars.h) through the 'state_variables state' member in TemplateState */

        //Access variable 'NAME' with:               state.singles[NAME]
        //Access element 'i' in array 'NAME' with:   state.arrays[NAME][i]
        //Get the size pf array 'NAME' with:         state.indices[NAME]

    public:
        MyState(proc_t n);
        MyState(const MyState* s);
        MyState(proc_t n, const state_variables &sv, std::vector<assignment> &ass);

        //construct a new copy of this state
        MyState* clone() const override;

        proc_t getN() const { return _N; }
        const singles_list& get(ARRAY_VAR name) const { return state.arrays_[name]; }
};


#endif
