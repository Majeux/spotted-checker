#ifndef S_VARS_H
#define S_VARS_H

using proc = char; //allows N up to 127, all vector values and sizes are bounded by N
using singles_list   = std::vector<proc>;
using arrays_list    = std::vector< std::vector<proc> >;

//small struct to hold values to be assigned
struct assignment {
    int array;
    proc index;
    proc value;

    assignment() : array(-1), index(0), value(0) { }
    assignment(int a) : array(a) { }

    void set(const proc i, proc v) {
        index = i;
        value = v;
    }
};

//container for the above defined variables
struct state_variables {
    /*
        Variables with these names will be available for use in an implementation of
        TemplateState and will be used to uniquely identify such a state.
    */
    enum SINGLE_VARS
    {   //TODO: Define the names of the variables you wish to use here
        x,
        y,
        z,
        //////////////////////////////////////////////////////
        N_VARS //Number of single variables. Keep N_VARS last!
    };

    enum ARRAY_VARS
    {   //TODO: Define the names of the arrays you wish to use here
        list_a,
        list_b,
        list_c,
        ///////////////////////////////////////////
        N_ARRAYS //Number of arrays. N_ARRAYS last!
    };

    singles_list          variables; //Holds all single variables used
    arrays_list           arrays;    //Holds all arrays used
    const singles_list    indices;   //Holds lengths of the elements of 'arrays'
};

#endif
