#include <algorithm>

#include "model_example/iterator.h"

TemplateState* MyIterator::dst() const {
    assert(_i <= _N);
    assert(_i > 0);

    //shorthands for arrays
    const singles_list* p   = &(state.arrays[pc]);
    const singles_list* lvl = &(state.arrays[level]);
    const singles_list* lte = &(state.arrays[last_to_enter]);

    proc i = _i - 1;
    proc l = ((*lvl))[i];
    //prepare search for greater level than level[i]
    auto begin = (*lvl).begin(), end = (*lvl).end();
    auto E_greater = [=, k = proc(0)] (proc k_val) mutable {
        return k++ != i && k_val >= l;
    };
    //containers are copy constructed in MyState,
    //delay assignment until after
    std::vector<assignment> assign;

    switch ((*p)[i]) {
        case 0: //initialize loop
            assign.push_back({level, i, 0});
            assign.push_back({pc, i, 1});

            break;
        case 1: //check loop bound
            assert(l >= 0);

            if(l < _N - 1)
                assign.push_back({pc, i, 2});
            else
                assign.push_back({pc, i, 4});

            break;
        case 2: //loop body
            assign.push_back({last_to_enter, l, i});
            assign.push_back({pc, i, 3});

            break;
        case 3: //loop wait
            if((*lte)[l] == i && std::any_of(begin, end, E_greater) )
                break;

            assign.push_back({level, i, (proc)(l+1)});
            assign.push_back({pc, i, 1});

            break;
        case 4: //critical section
            assign.push_back({level, i, -1});
            assign.push_back({pc, i, 0});

            break;
    }

    return new MyState(_N, state, assign);
}
