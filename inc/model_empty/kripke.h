#ifndef MODEL_KRIPKE_H
#define MODEL_KRIPKE_H

#include "model_abstract/kripke.h"
#include "model_empty/iterator.h"

/*  This class will combine the ModelState and ModelIterator classes to define
    the full Kripke structure. It performs state labeling, creating an initial
    state and allocation of state successor iterators.
*/
class ModelKripke: public AbstractKripke {
    private:
        /*  TODO
            Represent each of the atomic propositions (AP) as 'bdd' variables.
            These
        */
        bdd some_property;
        bdd other_property;

        /*  Wrapper for call to the constructor of ModelIterator.
            NOTE The ModelState should contain all information necessary to
            load a state into the ModelIterator. The initial internal state of
            the iterator should be set by the ModelIterator::first() function.
        */
        AbstractIterator* makeIterator(const spot::state* s, bdd condition) const override;

    public:
        ModelKripke(const spot::bdd_dict_ptr& d);

        /*  TODO
            Construct and return the initial state of the model
        */
        ModelState* get_init_state() const override;

        /*  TODO
            Return the label of the given state using the AP
        */
        bdd state_condition(const spot::state* s) const override;

        /*  TODO
            Return a string representing the state supplied by 's'
            Should uniquely identfiy a state, usually through the identifying variables
            Used to display a state as a graph and to display violating runs
            during model checking
        */
        std::string format_state(const spot::state* s) const override;

};

#endif
