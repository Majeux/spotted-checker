#ifndef CHECKER_H
#define CHECKER_H

#include <spot/kripke/kripkegraph.hh>
#include <spot/twaalgos/dot.hh>
#include <spot/tl/parse.hh>
#include <spot/twaalgos/translate.hh>
#include <spot/twaalgos/emptiness.hh>

#include <iostream>
#include <fstream>
#include <sstream>
#include <vector>
#include <algorithm>

using kripke_ptr = spot::kripke_graph_ptr;
using state      = uint32_t;

struct model_info {
    state States;
    state Initial;

    std::vector<std::string>          Symbols;
    std::vector< std::vector<bool> >  Labels;
    std::vector< std::vector<state> > Transitions;

    friend std::ostream& operator << (std::ostream& os, const model_info& m);
};


class Checker {
    public:
        Checker();

    // Hardcoded test
    kripke_ptr  explicit_door_kripke() const;
    // Reading user input
    bool        read_kripke(std::string filename, model_info& model) const;
    kripke_ptr  make_explicit(const model_info& m) const;
    void verify(spot::const_twa_ptr model, const std::string formula) const;


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
