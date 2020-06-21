#ifndef EXAMPLE_STATE_H
#define EXAMPLE_STATE_H

#include "model_template/state.h"

class MyState : public TemplateState {
    private:
        /* Universal/internal variables (among states) */
        const proc _N;

        /* This class has the following state indentifying variables and arrays */
        enum SINGLE_VARS //Names for single values
        {
            //TODO: Define the names of the variables you wish to use here
            //////////////////////////////////////////////////////
            N_VARS //Number of single variables. NOTE: Keep N_VARS last!
        };

        enum ARRAY_VARS //Names of arrays
        {
            //TODO Define the names of the arrays you wish to use here
            pc,
            level,
            last_to_enter,
            ///////////////////////////////////////////
            N_ARRAYS //Number of arrays. NOTE: Keep N_ARRAYS last!
        };

        //Access variable 'NAME' with:               state.singles[NAME]
        //Access element 'i' in array 'NAME' with:   state.arrays[NAME][i]
        //Get the size pf array 'NAME' with:         state.indices[NAME]

    public:
        MyState(proc n);
        MyState(const MyState* s);
        MyState(proc n, const state_variables &sv, std::vector<assignment> &ass);

        //construct a new copy of this state
        MyState* clone() const override;

        proc getN() const { return _N; }
};


#endif
