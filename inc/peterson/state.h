#ifndef PETERSON_STATE_H
#define PETERSON_STATE_H

#include <spot/kripke/kripke.hh>

#include <array>
#include <cassert>

struct assignment {
    size_t index;
    int value;

    void operator=(const assignment &a ) {
        index = a.index;
        value = a.value;
    }

    void set(const size_t i, const int v) {
        index = i;
        value = v;
    }
};

class PetersonState: public spot::state {
    private:
        //Constant among states
        const size_t _N;

        //State identifiers
        //use vectors to allow lexographic order and allow variable length
        std::vector<size_t> pc;
        std::vector<int> level; //TODO write as c array with lexographic <=>
        std::vector<size_t> last_to_enter;

    public:
        PetersonState(size_t n, const std::vector<size_t> &p,
                                const std::vector<int> &lv,
                                const std::vector<size_t> &lte) : _N(n), pc(p), level(lv), last_to_enter(lte)
        {
            assert(level.size() == _N);
            assert(last_to_enter.size() == _N-1);
        }

        PetersonState(size_t n, const std::vector<size_t> &p, assignment to_pc,
                                const std::vector<int> &lv,
                                const std::vector<size_t> &lte) : PetersonState(n, p, lv, lte)
        {
            pc[to_pc.index] = to_pc.value;
        }

        PetersonState(size_t n, const std::vector<size_t> &p, assignment to_pc,
                                const std::vector<int> &lv,    assignment to_lv,
                                const std::vector<size_t> &lte) : PetersonState(n, p, to_pc, lv, lte)
        {
            level[to_lv.index] = to_lv.value;
        }

        PetersonState(size_t n, const std::vector<size_t> &p, assignment to_pc,
                                const std::vector<int> &lv,
                                const std::vector<size_t> &lte,   assignment to_lte) : PetersonState(n, p, to_pc, lv, lte)
        {
            last_to_enter[to_lte.index] = to_lte.value;
        }

        PetersonState* clone() const override {
            return new PetersonState(_N, pc, level, last_to_enter);
        }

        size_t hash() const override {
            return reinterpret_cast<size_t>(this);
        }

        // compare must impose a total order
        // comparators for std::array and std::vector impose a lexographical order,
        // which is a total order if their elements (int) have a total order
        int compare(const spot::state* other) const override {
            auto o = static_cast<const PetersonState*>(other);
            // assert(o != NULL);
            //
            // if(singles < o->singles) return -1; //first <
            // if(singles > o->singles) return  1; //first > --> first ==
            // if(arrays  < o->arrays)  return -1; //first ==, second <
            //
            // return arrays > o->arrays;        //first ==, second > or ==
            return 0;
         }
};

#endif
