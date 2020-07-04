#ifndef EXAMPLE_STATE_H
#define EXAMPLE_STATE_H

#include "model_abstract/state.h"
#include "model_example/state_vars.h"

//small struct to hold values to be assigned
struct assignment {
    int array; //to hold either SINGLE_VARS or ARRAY_VARS
    proc_t index;
    proc_t value;

    assignment() : array(-1), index(0), value(0) { }
    assignment(int a) : array(a) { }
    assignment(int a, proc_t i, proc_t v) : array(a), index(i), value(v) { }

    void set(const proc_t i, proc_t v) {
        index = i;
        value = v;
    }
};

class MyState : public AbstractState {
    private:
        /*  Universal/internal variables (among states) */
        const proc_t _N;

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
