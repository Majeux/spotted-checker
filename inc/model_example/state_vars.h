#ifndef S_VARS_EX_H
#define S_VARS_EX_H

using proc = char; //allows N up to 127, all vector values and sizes are bounded by N
using singles_list   = std::vector<proc>;
using arrays_list    = std::vector< singles_list >;

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

struct state_variables {
    singles_list variables; //Holds all single variables used
    arrays_list  arrays;    //Holds all arrays used
    singles_list indices;   //Holds lengths of the elements of 'arrays'

    state_variables() : variables(-1), arrays(0), indices(0) { }

    state_variables(proc v, proc a, singles_list i) : variables(v), arrays(a), indices(i) { }

    state_variables(const state_variables* sv)    : variables(sv->variables),
                                                    arrays(sv->arrays),
                                                    indices(sv->indices) {}

    state_variables& operator=(const state_variables &sv) {
        variables = sv.variables;
        arrays = sv.arrays;
        indices = sv.indices;

        return *this;
    }
};

#endif
