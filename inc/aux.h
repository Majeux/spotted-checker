#ifndef AUX_H
#define AUX_H

using proc = char; //allows N up to 127, all vector values and sizes are bounded by N
using singles_list   = std::vector<proc>;
using arrays_list    = std::vector< singles_list >;

/*  Container for the variables defined above */
struct state_variables {
    singles_list variables; //Holds all single variables used
    arrays_list  arrays;    //Holds all arrays used
    singles_list indices;   //Holds lengths of the elements of 'arrays'

    //default construction with known vector sizes
    state_variables(proc var, proc ar, singles_list ind) :  variables(var),
                                                            arrays(ar),
                                                            indices(ind) {
        for(size_t i = 0; i < indices.size(); i++)
            arrays[i].resize(indices[i]);
    }

    //construction with known vector sizes and initial value for each vector
    state_variables(proc var, proc init_var,
                    proc ar, singles_list init_ar,
                    singles_list sizes) :   variables(var, init_var), arrays(ar), indices(sizes) {

        assert(ar > 0 && (size_t)ar == sizes.size());
        assert(sizes.size() == init_ar.size());
        //load initial values
        for(size_t i = 0; i < indices.size(); i++)
            arrays[i] = std::vector<proc>(sizes[i], init_ar[i]);
    }

    //copy constructor
    state_variables(const state_variables &sv) :    variables(sv.variables),
                                                    arrays(sv.arrays),
                                                    indices(sv.indices) { }

    //copy constructor for pointers
    state_variables(const state_variables* sv) : state_variables(*sv) { }

    state_variables& operator=(const state_variables &sv) {
        variables = sv.variables;
        arrays = sv.arrays;
        indices = sv.indices;

        return *this;
    }
};

//small struct to hold values to be assigned
struct assignment {
    int array;
    proc index;
    proc value;

    assignment() : array(-1), index(0), value(0) { }
    assignment(int a) : array(a) { }
    assignment(int a, proc i, proc v) : array(a), index(i), value(v) { }

    void set(const proc i, proc v) {
        index = i;
        value = v;
    }
};

#endif
