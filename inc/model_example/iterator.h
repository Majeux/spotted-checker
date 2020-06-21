#ifndef EXAMPLE_IT_H
#define EXAMPLE_IT_H

#include "model_example/state.h"
#include "model_template/iterator.h"

class MyIterator : public TemplateIterator {
    private:
        const proc _N; //number of processes, constant among states/iterators
        proc _i;       //iterator through processes

        /* This class has the following state indentifying variables and arrays */
        enum SINGLE_VARS //Names for single values
        {
            //TODO: Define the names of the variables you wish to use here
            //////////////////////////////////////////////////////
            N_VARS //Number of single variables. NOTE: Keep N_VARS last!
        };

        enum ARRAY_VARS //Names of arrays
        {
            //TODO Define the names of the arrays you wish to use here
            pc,
            level,
            last_to_enter,
            ///////////////////////////////////////////
            N_ARRAYS //Number of arrays. NOTE: keep N_ARRAYS last!
        };

        //Access variable 'NAME' with:               state.singles[NAME]
        //Access element 'i' in array 'NAME' with:   state.arrays[NAME][i]
        //Get the size pf array 'NAME' with:         state.indices[NAME]

    public:
        MyIterator(const MyState* s, bdd cond) :
            TemplateIterator(s, cond), _N(s->getN()) { }

        // Set iterator, return true iff there is at least 1 successor
        bool first() override {
            assert(_N > 0);
            _i = _N;
            return _i > 0;
        }

        bool next() override {
            assert(_i > 0);
            return --_i > 0; //switch to next process
        }

        bool done() const override {
            return _i == 0;
        }

        TemplateState* dst() const override;
};

#endif
