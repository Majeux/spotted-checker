#ifndef STATE_TEMPLATE_H
#define STATE_TEMPLATE_H

#include <spot/kripke/kripke.hh>

#include <array>

const size_t N = 4;

//Names of all used variables
/* Single integers */
enum SINGLE_VARS {  //Names for single ints
    N_process,
    x,
    y,
    S_COUNT  };      //Number of single variables. Keep S_COUNT last!

/* Arrays of integers*/
enum ARRAY_VARS {   //Names of integer arrays
    level,
    last_to_visit,
    A_COUNT  };     //Number of integer arrays. Keep Count last!

/* Sizes for our integer arrays */
std::array<size_t, A_COUNT> ARRAY_SIZE {N, N-1};

using single_list = std::array<int, S_COUNT>;
using array_list    = std::array< std::vector<int>, A_COUNT >;

class MyState: public spot::state {
    private:
        single_list variables;  //Array that holds all integer variables used
                                //Access variable 'NAME' with singles[NAME]

        array_list     arrays;  //Array that holds all integer arrays used
                                //Access element 'i' in array 'NAME' with arrays[NAME][i]

    public:
        MyState() {
            // Set all the sizes of all used 'arrays' (in practice vectors for generality)
            for(size_t i = 0; i < arrays.size(); i++)
                arrays[i].resize(ARRAY_SIZE[i]);
        }

        MyState* clone() const override {
            return new MyState();
        }

        size_t hash() const override {
            return reinterpret_cast<size_t>(this);
        }

        // compare must impose a total order
        // comparators for std::array and std::vector impose a lexographical order,
        // which is a total order if their elements (int) have a total order
        int compare(const spot::state* other) const override {
            auto o = static_cast<const MyState*>(other);

            if(variables < o->variables) return -1; //first <
            if(variables > o->variables) return  1; //first > --> first ==
            if(arrays  < o->arrays)  return -1; //first ==, second <

            return arrays > o->arrays;        //first ==, second > or ==

         }
};

#endif
