#ifndef MODEL_KRIPKE_H
#define MODEL_KRIPKE_H

#include "model_template/kripke.h"
#include "model_empty/iterator.h"

class ModelKripke: public TemplateKripke {
    private:
        /*  TODO
            Represent properties of a state using 'bdd' variables
        */
        bdd some_property;
        bdd other_property;

    public:
        ModelKripke(const spot::bdd_dict_ptr& d) : TemplateKripke(d) {
            //TODO
            some_property = bdd_ithvar( register_ap("some_name") );
            some_property = bdd_ithvar( register_ap("other_name") );
        }

        /*  TODO
            Construct and return the initial state of the model
        */
        ModelState* get_init_state() const override;

        /*  Wrapper for call to the constructor of ModelIterator.
            NOTE The ModelState should contain all information necessary to
            load a state into the ModelIterator. The initial internal state of
            the iterator should be set by the ModelIterator::first() function.
        */
        TemplateIterator* makeIterator(const spot::state* s, bdd condition) const override {
            //Convert parent class pointer to child pointer
            const ModelState* state = static_cast<const ModelState*>(s);

            return new ModelIterator(state, condition);
        }

        /*  TODO
            Return the combined condition of the given state
        */
        bdd state_condition(const spot::state* s) const override {
            //Convert parent class pointer to child pointer
            const ModelState* state = static_cast<const ModelState*>(s);
            bdd cond1, cond2;

            proc some_value = 1;  //obtained from s
            proc other_value = 0; //obtained from s

            if(some_value == 1) cond1 = some_property;
            else cond1 = !some_property

            if(other_value == 1) cond2 = other_property;
            else cond2 = !other_property

            return cond1 & cond2;
        }

        /*  TODO
            Return a string representing the state supplied by 's'
            Should uniquely identfiy a state, usually through the identifying variables
            Used to display a state as a graph and to display violating runs
            during model checking
        */
        std::string format_state(const spot::state* s) const override {
            auto state = static_cast<const ModelState*>(s);
            std::ostringstream out;

            out << "TODO: This is the state we are in!" << std::endl;

            return out.str();
        }
};

#endif
