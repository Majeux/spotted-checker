#ifndef EXAMPLE_IT_H
#define EXAMPLE_IT_H

#include "model_example/state.h"
#include "model_template/iterator.h"

class MyIterator : public TemplateIterator {
    private:
        const proc _N; //number of processes, constant among states/iterators
        proc _i;       //iterator through processes

        /*  This class has access to the identifying variables (defined in state_vars.h) through the 'state_variables state' member in TemplateState */

        //Access variable 'NAME' with:               state.singles[NAME]
        //Access element 'i' in array 'NAME' with:   state.arrays[NAME][i]
        //Get the size pf array 'NAME' with:         state.indices[NAME]

    public:
        MyIterator(const MyState* s, bdd cond) :
            TemplateIterator(s, cond), _N(s->getN()) { }

        // Set iterator, return true iff there is at least 1 successor
        bool first() override;

        bool next() override;

        bool done() const override;

        TemplateState* dst() const override;
};

#endif
