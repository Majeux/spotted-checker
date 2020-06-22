#ifndef PETERSON_STATE_H
#define PETERSON_STATE_H

#include <spot/kripke/kripke.hh>

#include <array>
#include <cassert>
#include <dbg.h>
#include "model_example/state_vars.h"

class PetersonState: public spot::state {
    private:
        //Constant among states
        const proc _N;

        //State identifiers
        //use vectors to allow lexographic order and allow variable length
        //TODO write as c array with lexographic <=>
        std::vector<proc> pc;
        std::vector<proc> level; //TODO use uint8 and shift bounds by 1 (greater available N)
        std::vector<proc> last_to_enter;

    public:
        PetersonState(proc n) : _N(n), pc(_N, 0), level(_N, -1), last_to_enter(_N-1, 0)
        {}

        PetersonState(proc n, const std::vector<proc> &p,
                              const std::vector<proc> &lv,
                              const std::vector<proc> &lte) : _N(n), pc(p), level(lv), last_to_enter(lte)
        {
            assert(_N > 0);
            assert(level.size() == (size_t)_N);
            assert(last_to_enter.size() == (size_t)(_N-1) );
        }

        PetersonState(proc n, const std::vector<proc> &p, assignment to_pc,
                              const std::vector<proc> &lv,
                              const std::vector<proc> &lte) : PetersonState(n, p, lv, lte)
        { pc[to_pc.index] = to_pc.value; }

        PetersonState(proc n, const std::vector<proc> &p, assignment to_pc,
                              const std::vector<proc> &lv,    assignment to_lv,
                              const std::vector<proc> &lte) : PetersonState(n, p, to_pc, lv, lte)
        { level[to_lv.index] = to_lv.value; }

        PetersonState(proc n, const std::vector<proc> &p, assignment to_pc,
                              const std::vector<proc> &lv,
                              const std::vector<proc> &lte,   assignment to_lte) : PetersonState(n, p, to_pc, lv, lte)
        { last_to_enter[to_lte.index] = to_lte.value; }

        PetersonState* clone() const override {
            return new PetersonState(_N, pc, level, last_to_enter);
        }

        //create hash over the identifiers for this state
        size_t hash() const override;

        // compare must impose a total order
        // comparators for std::array and std::vector impose a lexographical order,
        // which is a total order if their elements (int) have a total order
        //return -1 if *this < *other, 1 if *this > *other, 0 if *this == *other
        int compare(const spot::state* other) const override;

        proc getN() const { return _N; }
        const std::vector<proc>* getPC()  const { return &pc; }
        const std::vector<proc>*  getLVL() const { return &level; }
        const std::vector<proc>* getLTE() const { return &last_to_enter; }

        bool* getInCrit(proc N) const;
};

#endif
