#ifndef CHECKER_H
#define CHECKER_H

#include <spot/kripke/kripkegraph.hh>

#include <fstream>
#include <sstream>
#include <vector>

using kripke_ptr = spot::kripke_graph_ptr;
using state      = uint32_t;


struct model_info {
    state States;
    state Initial;

    std::vector<std::string>          Symbols;
    std::vector< std::vector<bool> >  Labels;
    std::vector< std::vector<state> > Transitions;
};

class Checker {
    public:
        Checker();

    /* TODO Read Kripke model
        * Build automata
            - Explicit
            - On-the-fly,parametrized
                > Implement state:
                (Completely CONSTANT for generic Kripke)
                    + One or more state identifiers: 1 state-id                 (CONSTANT)
                    + clone() to duplicate the state                            (CONSTANT)
                    + hash() gives unique hash key                              (CONSTANT)
                    + compare() decide some total order                         (CONSTANT)

                > Implement kripke_suc_iterator, defines edges:
                (Will be CONSTANT depending on implementation scheme, parametrize next state)
                    + store some i (edge label),  index in vector               (CONSTANT)
                    + first() to set initial state and return if successors     (PARAM: vector<dst>)
                        Index 0 in vector, return |vector| > 1
                    + next() progress i and return if successors                (PARAM: vector<dst>)
                        Increment i return if i < |vector|
                    + done() return if in final state                           (PARAM: vector<dst>)
                        return if i >= |vector|
                    + dst() return destination of current edge (push on stack)  (PARAM: vector<dst>)
                        return new state(vector[i])
                    + recycle() copy paste                                      (CONSTANT)
                >
        * Recognize state (variables)
    */

    kripke_ptr explicit_door_kripke() const;
    model_info read_kripke(std::string filename) const;

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
