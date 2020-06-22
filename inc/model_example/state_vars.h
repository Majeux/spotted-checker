#ifndef S_VARS_EX_H
#define S_VARS_EX_H

using proc = char; //allows N up to 127, all vector values and sizes are bounded by N
using singles_list   = std::vector<proc>;
using arrays_list    = std::vector< singles_list >;

/*  Every state of the model is identified by its values for the variables below */
enum SINGLE_VAR //Names for single values
{
    //TODO: Define the names of the variables you wish to use here
    //////////////////////////////////////////////////////
    N_VARS //Number of single variables. NOTE: Keep N_VARS last!
};

enum ARRAY_VAR //Names of arrays
{
    //TODO Define the names of the arrays you wish to use here
    pc,
    level,
    last_to_enter,
    ///////////////////////////////////////////
    N_ARRAYS //Number of arrays. NOTE: Keep N_ARRAYS last!
};

/*  Container for the variables defined above */
struct state_variables {
    singles_list variables; //Holds all single variables used
    arrays_list  arrays;    //Holds all arrays used
    singles_list indices;   //Holds lengths of the elements of 'arrays'

    state_variables() : variables(N_VARS, -1), arrays(N_ARRAYS), indices(N_ARRAYS, 0) { }

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
