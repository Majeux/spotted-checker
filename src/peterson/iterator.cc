#include "peterson/iterator.h"

PetersonState* PetersonIterator::dst() const {
    assert(_i < _N);

    //containers are copy constructed in PetersonState,
    //delay assignment until after
    assignment assign_pc, assign_lvl, assign_lte;

    switch (pc[_i]) {
        case 0: //initialize loop
            assign_lvl.set(_i, 0);
            assign_pc.set(_i, 1);
            return new PetersonState(_N, pc, assign_pc,
                                         level, assign_lvl,
                                         last_to_enter);
        case 1: //check loop bound
            assert(level[_i] >= 0);

            if((size_t)level[_i] < _N - 1)
                assign_pc.set(_i, 2);
            else
                assign_pc.set(_i, 4);

            return new PetersonState(_N, pc, assign_pc,
                                         level,
                                         last_to_enter);
        case 2: //loop body
            assign_lte.set(level[_i], _i);
            assign_pc.set(_i, 3);
            return new PetersonState(_N, pc, assign_pc,
                                         level,
                                         last_to_enter, assign_lte);
        case 3: //loop wait
            if(last_to_enter[level[_i]] == _i && false)
                return new PetersonState(_N, pc, level, last_to_enter);

            assign_lvl.set(_i, level[_i] + 1);
            assign_pc.set(_i, 1);
            return new PetersonState(_N, pc, assign_pc,
                                         level, assign_lvl,
                                         last_to_enter);
        case 4: //critical section
            assign_lte.set(level[_i], -1);
            assign_pc.set(_i, 0);
            return new PetersonState(_N, pc, assign_pc,
                                         level,
                                         last_to_enter, assign_lte);
    }
    return NULL;
}
