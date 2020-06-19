#include <algorithm>

#include "peterson/iterator.h"

PetersonState* PetersonIterator::dst() const {
    assert(_i <= _N);
    assert(_i > 0);

    proc i = _i - 1;
    proc l = level[i];
    //prepare search for greater level than level[i]
    auto begin = level.begin(), end = level.end();
    auto E_greater = [=, k = proc(0)] (proc k_val) mutable {
        return k++ != i && k_val >= l;
    };
    //containers are copy constructed in PetersonState,
    //delay assignment until after
    assignment assign_pc, assign_lvl, assign_lte;

    switch (pc[i]) {
        case 0: //initialize loop
            assign_lvl.set(i, 0);
            assign_pc.set(i, 1);

            return new PetersonState(_N, pc, assign_pc,
                                         level, assign_lvl,
                                         last_to_enter);
        case 1: //check loop bound
            assert(l >= 0);

            if(l < _N - 1)
                assign_pc.set(i, 2);
            else
                assign_pc.set(i, 4);

            return new PetersonState(_N, pc, assign_pc,
                                         level,
                                         last_to_enter);
        case 2: //loop body
            assign_lte.set(l, i);
            assign_pc.set(i, 3);

            return new PetersonState(_N, pc, assign_pc,
                                         level,
                                         last_to_enter, assign_lte);
        case 3: //loop wait
            if(last_to_enter[l] == i && std::any_of(begin, end, E_greater) )
                return new PetersonState(_N, pc, level, last_to_enter);

            assign_lvl.set(i, l + 1);
            assign_pc.set(i, 1);

            return new PetersonState(_N, pc, assign_pc,
                                         level, assign_lvl,
                                         last_to_enter);
        case 4: //critical section
            assign_lvl.set(i, -1);
            assign_pc.set(i, 0);

            return new PetersonState(_N, pc, assign_pc,
                                         level, assign_lvl,
                                         last_to_enter);
    }
    return NULL;
}
