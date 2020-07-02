#include "model_example/state.h"

MyState::MyState(bound_t n) : TemplateState(N_VARS, -1,
                                         N_ARRAYS, {0, -1, 0},
                                         { n, n, bound_t(n-1) } ), _N(n)
{
    assert(n > 0);
}

MyState::MyState(const MyState* s) : TemplateState( s->getStateVars() ), _N( s->getN() )  { }

MyState::MyState(proc_t n, const state_variables &sv, std::vector<assignment> &ass)
                                                        : TemplateState(sv), _N(n) {
    for(assignment a : ass) {
        if(a.array < 0)
            state(a.index) = a.value;
        else
            state(a.array, a.index) = a.value;
    }
}

MyState* MyState::clone() const {
    return new MyState(this);
}
