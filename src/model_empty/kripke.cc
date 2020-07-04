#include "model_empty/kripke.h"

//TODO
/* Example of creating bdd variables in the constructor */
ModelKripke::ModelKripke(const spot::bdd_dict_ptr& d) : AbstractKripke(d) {
    some_property = bdd_ithvar( register_ap("some_name") );
    some_property = bdd_ithvar( register_ap("other_name") );
}

/* Should create a valid iterator based on state by default, loads the sate_variables into the iterator */
AbstractIterator* ModelKripke::makeIterator(const spot::state* s, bdd condition) const {
    //Convert parent class pointer to child pointer
    const ModelState* state = static_cast<const ModelState*>(s);
    return new ModelIterator(state, condition);
}

//TODO
ModelState* ModelKripke::get_init_state() const {
    return nullptr;
}

//TODO
/* Example of using two proc_t values to create a state condition */
bdd ModelKripke::state_condition(const spot::state* s) const {
    //Convert parent class pointer to child pointer
    const ModelState* state = static_cast<const ModelState*>(s);
    bdd cond1, cond2;

    proc_t some_value = 1;  //obtained from s
    proc_t other_value = 0; //obtained from s

    if(some_value == 1) cond1 = some_property;
    else cond1 = !some_property;

    if(other_value == 1) cond2 = other_property;
    else cond2 = !other_property;

    return cond1 & cond2;
}

//TODO
/* Example of some constructing a string using stringstream */
std::string ModelKripke::format_state(const spot::state* s) const {
    auto state = static_cast<const ModelState*>(s);
    std::ostringstream out;

    out << "TODO: This is the state we are in!" << std::endl;

    return out.str();
}
