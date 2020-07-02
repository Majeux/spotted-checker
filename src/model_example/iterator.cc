#include <algorithm>

#include "model_example/iterator.h"

MyIterator::~MyIterator() {
    delete[] do_i;
}

bool MyIterator::first() {
    assert(_N > 0);
    count = 0;

    proc_t p;
    auto begin = state.data(level).begin(), end = state.data(level).end();

    bool i_set = false;
    for(p = _N; p != 0; p--) {
        proc_t l = state(level, p-1);
        auto E_greater = [&, k = proc_t(0)] (proc_t k_val) mutable {
            return k++ != p-1 && k_val >= l;
        };

        if(state(pc, p-1) == 3 && state(last_to_enter, l) == p-1 && std::any_of(begin, end, E_greater) ) {
            do_i[p-1] = false; //A proc_tess in pc = 3, has no edge if it is waiting
        }
        else {
            do_i[p-1] = true;
            if(!i_set) {
                _i = p;
                i_set = true;
            }
            count++;
        }
    }

    return count > 0;
}

bool MyIterator::next() {
    if(done())
        return false;

    assert(_i > 0);
    assert(count > 0);

    if(--count == 0)
        return false;
    //go to next edge, skip waiting edges
    //NOTE there is a next edge since count was greater than 0
    do {
        _i--;
        assert(_i > 0);
    } while(!do_i[_i-1]);

    assert(do_i[_i-1]);
    return true; //switch to next proc_tess
}

bool MyIterator::done() const {
    return count == 0;
}

TemplateState* MyIterator::dst() const {
    assert(_i <= _N);
    assert(_i > 0);
    assert(count > 0);
    assert(do_i[_i-1]);

    //shorthands for arrays
    const singles_list& p   = state.arrays_[pc];
    const singles_list& lvl = state.arrays_[level];

    proc_t i = _i - 1;
    proc_t l = lvl[i];
    //prepare search for greater level than level[i]

    //containers are copy constructed in MyState,
    //delay assignment until after
    std::vector<assignment> assign;

    switch (p[i]) {
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
            //wait is handled by do_i array set in first()
            assign.push_back({level, i, (proc_t)(l+1)});
            assign.push_back({pc, i, 1});

            break;
        case 4: //critical section
            assign.push_back({level, i, -1});//exit
            assign.push_back({pc, i, 0});

            break;
    }

    return new MyState(_N, state, assign);
}
