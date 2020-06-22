#ifndef STATE_VARS_H
#define STATE_VARS_H

/* The variables that identify a unique state in our model are defined here.
    They are the variables that will become available in the Iterator and State
    classes through a state_variable object
*/

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

    ///////////////////////////////////////////
    N_ARRAYS //Number of arrays. NOTE: Keep N_ARRAYS last!
};

#endif
