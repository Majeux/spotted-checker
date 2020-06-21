#include "model_example/state.h"

MyState::MyState(proc n) : TemplateState(N_VARS, N_ARRAYS, { n, n, (proc)(n-1) }), _N(n)  {
    assert(n > 0);
    /* init state identifiers */
    state.variables.resize(N_VARS);
    state.arrays.resize(N_ARRAYS);

    // Set all the sizes of all used 'arrays' (in practice vectors for generality)
    for(size_t i = 0; i < state.arrays.size(); i++)
        state.arrays[i].resize(state.indices[i]);
}

MyState::MyState(const MyState* s) : TemplateState( s->getStateVars() ), _N( s->getN() )  { }

MyState::MyState(proc n, const state_variables &sv, std::vector<assignment> &ass) : _N(n) {
        state = sv;
        for(assignment a : ass) {
            if(a.array < 0)
                state.variables[a.index] = a.value;
            else
                state.arrays[a.array][a.index] = a.value;
        }

    }

MyState* MyState::clone() const {
    return new MyState(this);
}
