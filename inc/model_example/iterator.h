#ifndef EXAMPLE_IT_H
#define EXAMPLE_IT_H

#include "model_example/state.h"
#include "model_abstract/iterator.h"

class MyIterator : public AbstractIterator {
    private:
        const proc_t _N; //number of proc_tesses, constant among states/iterators
        proc_t count;     //number of outgoing edges
        proc_t _i;       //iterator through proc_tesses
        bool* do_i;  //if proc_tess _i has an edge

        /*  This class has access to the identifying variables (defined in state_vars.h) through the 'state_variables state' member in AbstractState */

        //Access variable 'NAME' with:               sv.singles[NAME]
        //Access element 'i' in array 'NAME' with:   sv.arrays[NAME][i]
        //Get the size pf array 'NAME' with:         sv.indices[NAME]

    public:
        MyIterator(const MyState* s, bdd cond) :
            AbstractIterator(s, cond), _N(s->getN()) {
            do_i = new bool[_N]; }

        ~MyIterator();

        // Set iterator, return true iff there is at least 1 successor
        bool first() override;

        bool next() override;

        bool done() const override;

        AbstractState* dst() const override;
};

#endif
