#ifndef PETERSON_IT_H
#define PETERSON_IT_H

#include "peterson/state.h"

class PetersonIterator: public spot::kripke_succ_iterator {
    private:
        const size_t _N; //number of processes
        size_t _i; //iterator through processes


        std::vector<size_t> pc; //track what state/line of peterson we are in
        std::vector<int> level; //TODO write as c array with lexographic <=>
        std::vector<size_t> last_to_enter;

    public:
        PetersonIterator(size_t n, bdd cond) : kripke_succ_iterator(cond), _N(n)
        {
        }

        // Set iterator, return true iff there is at least 1 successor
        bool first() override {
            _i = 0;
            return _i < _N;
        }

        bool next() override {
            assert(_i < _N);
            return ++_i < _N; //switch to next process
        }

        bool done() const override {
            return _i >= _N;
        }

        PetersonState* dst() const override;

        void recycle(bdd cond)
        {
            spot::kripke_succ_iterator::recycle(cond);
        }
};

#endif
