#include "model_example/state.h"

MyState::MyState(bound_t n)
    : AbstractState(N_VARS, -1, N_ARRAYS, {0, -1, 0}, { n, n, bound_t(n-1) } ), _N(n)
{
    assert(n > 0);
}

MyState::MyState(const MyState* s)
    : AbstractState( s->getStateVars() ), _N( s->getN() )  { }

MyState::MyState(proc_t n, const state_variables &s, std::vector<assignment> &ass)
    : AbstractState(s), _N(n)
{
    for(assignment a : ass) {
        if(a.array < 0)
            sv(a.index) = a.value;
        else
            sv(a.array, a.index) = a.value;
    }
}

MyState* MyState::clone() const {
    return new MyState(this);
}
