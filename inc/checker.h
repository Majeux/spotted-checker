#ifndef CHECKER_H
#define CHECKER_H

#include <spot/kripke/kripkegraph.hh>

using kripke_ptr = spot::kripke_graph_ptr;

class Checker {
    public:
        Checker();

    /* TODO Read Kripke model
        * Build automata
            - Explicit
            - On-the-fly
        * Recognize state (variables)
    */
    kripke_ptr explicit_door_kripke() const;


    /* TODO Create property checks
        * Standard method for deadlocks/liveness and stuff
        * Parse LTL formula to evaluate model dependent property
        * Convert property to Buchi
    */

    /* TODO Evaluate model
        * Cross-product Model x !property (Check inverse of desired property)
        * Check emptiness
        * Report violation ??
    */

    private:
        spot::bdd_dict_ptr dict; //TODO possibly global dict to ensure consistency with variable naming
};

#endif
