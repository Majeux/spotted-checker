#ifndef AUX_H
#define AUX_H

//allows N up to 127, all vector values and sizes are bounded by N in Peterson's
using proc_t        = char;
using bound_t       = unsigned char;
using singles_list  = std::vector<proc_t>;
using bound_list    = std::vector<bound_t>;
using arrays_list   = std::vector< singles_list >;

/*  Container for the variables defined above */
struct state_variables {
    singles_list variables_; //Holds all single variables used

    arrays_list  arrays_;    //Holds all arrays used
    bound_list   bounds_;   //Holds lengths of the elements of 'arrays'

    /*  Access a variable with token 'VAR_NAME' using the ()-operator:
            sv(VAR_NAME) = 0
            x = sv(VAR_NAME) + 1

        Access an element 'i' in array 'ARR_NAME' using the ()-operator:
            sv(ARR_NAME, i) = 0
            x = sv(ARR_NAME, i) + 1

        Get a reference to the underlying vector of our 'ARR_NAME' with:
            std::vector<proc_t>& a = sv.data(ARR_NAME);
        Or: std::vector<proc_t>& a = state&ARRAY;

        Get the size of array 'ARR_NAME' with:
            sv.size(ARR_NAME)
    */

    //default construction with known vector bounds
    state_variables(proc_t vars, proc_t arrs, bound_list bounds)
                    :  variables_(vars), arrays_(arrs), bounds_(bounds) {
        for(size_t i = 0; i < bounds_.size(); i++)
            arrays_[i].resize(bounds_[i]);
    }

    //construction with known vector sizes and initial value for each vector
    state_variables(proc_t vars, proc_t init_var,
                    proc_t arrs, singles_list init_ar,
                    bound_list bounds)
                    : variables_(vars, init_var), arrays_(arrs), bounds_(bounds) {

        assert(arrs > 0 && (size_t)arrs == bounds_.size());
        assert(bounds_.size() == init_ar.size());
        //load initial values
        for(size_t i = 0; i < bounds_.size(); i++)
            arrays_[i] = singles_list(bounds_[i], init_ar[i]);
    }

    //copy constructor
    state_variables(const state_variables &sv)
                    : variables_(sv.variables_), arrays_(sv.arrays_), bounds_(sv.bounds_) { }

    //copy constructor for pointers
    state_variables(const state_variables* sv) : state_variables(*sv) { }

    state_variables& operator=(const state_variables &sv) {
        variables_ = sv.variables_;
        arrays_ = sv.arrays_;
        bounds_ = sv.bounds_;

        return *this;
    }

    /*  variable access operators */
    proc_t& operator()(size_t single_name) {
        return variables_[single_name];
    }

    const proc_t& operator()(size_t single_name) const {
        return variables_[single_name];
    }

    /*  array variable access operators */
    proc_t& operator()(size_t arr_name, size_t i) {
        assert(i < bounds_[arr_name]);
        return arrays_[arr_name][i];
    }

    const proc_t& operator()(size_t arr_name, size_t i) const {
        assert(i < bounds_[arr_name]);
        return arrays_[arr_name][i];
    }

    /*  array data access operators */
    singles_list& data(size_t arr_name) {
        return arrays_[arr_name];
    }

    const singles_list& data(size_t arr_name) const {
        return arrays_[arr_name];
    }

    singles_list& operator&(size_t arr_name) {
        return arrays_[arr_name];
    }

    const singles_list& operator&(size_t arr_name) const {
        return arrays_[arr_name];
    }

    /*  array size access operators */
    bound_t size(size_t arr_name) const {
        return bounds_[arr_name];
    }
};

#endif
