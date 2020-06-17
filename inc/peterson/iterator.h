#ifndef PETERSON_IT_H
#define PETERSON_IT_H

#include "peterson/state.h"

class PetersonIterator: public spot::kripke_succ_iterator {
    private:
        const size_t _N; //number of processes, constant among states/iterators
        size_t _i;       //iterator through processes

        //State identifiers
        std::vector<size_t> pc; //track what state/line of peterson we are in
        std::vector<int> level; //TODO write as c array with lexographic <=>
        std::vector<size_t> last_to_enter;

    public:
        PetersonIterator(PetersonState* s, bdd cond) :
            kripke_succ_iterator(cond), _N(s->getN()),
            pc(            *(s->getPC())  ),
            level(         *(s->getLVL()) ),
            last_to_enter( *(s->getLTE()) ) { }

        // Set iterator, return true iff there is at least 1 successor
        bool first() override {
            assert(_N > 0);
            _i = _N - 1;
            return _i > 0;
        }

        bool next() override {
            assert(_i > 0);
            return --_i > 0; //switch to next process
        }

        bool done() const override {
            return _i == 0;
        }

        PetersonState* dst() const override;

        void recycle(const PetersonState* s, bdd cond)
        {
            pc = *(s->getPC());
            level = *(s->getLVL());
            last_to_enter = *(s->getLTE());

            spot::kripke_succ_iterator::recycle(cond);
        }
};

#endif
