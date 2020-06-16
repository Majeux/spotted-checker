#ifndef PETERSON_H
#define PETERSON_H

#include <spot/kripke/kripke.hh>

#include <array>

struct assigment {
        bool assign;
        size_t index;
        int value;
};

class PetersonState: public spot::state {
    private:
        //Constant among states
        const size_t N;

        //State identifiers
        //use vectors to allow lexographic order and allow variable length
        std::vector<int> level;
        std::vector<int> last_to_enter;

    public:
        PetersonState(size_t n, std::vector<int> &lv, std::vector<int> &lte) : N(n), level(lv), last_to_enter(lte) { }

        PetersonState(size_t n, std::vector<int> &lv, assignment to_lv, std::vector<int> &lte) : PetersonState(n, lv, lte) {
            level[to_lv.index] = to_lv.value;
        }

        PetersonState(size_t n, std::vector<int> &lv, std::vector<int> &lte, assignment to_lte) : PetersonState(n, lv, lte) {
            last_to_enter[to_lte.index] = to_lte.value;
        }

        MyState* clone() const override {
            return new demo_state(N, level, last_to_enter);
        }

        size_t hash() const override {
            return reinterpret_cast<size_t>(this);
        }

        // compare must impose a total order
        // comparators for std::array and std::vector impose a lexographical order,
        // which is a total order if their elements (int) have a total order
        int compare(const spot::state* other) const override {
            auto o = static_cast<const MyState*>(other);

            if(singles < o->singles) return -1; //first <
            if(singles > o->singles) return  1; //first > --> first ==
            if(arrays  < o->arrays)  return -1; //first ==, second <

            return arrays > o->arrays;        //first ==, second > or ==

         }
};

#endif
