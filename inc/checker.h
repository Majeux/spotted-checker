#ifndef CHECKER_H
#define CHECKER_H

#include <spot/kripke/kripkegraph.hh>
#include <spot/twaalgos/dot.hh>
#include <spot/twaalgos/hoa.hh>
#include <spot/tl/parse.hh>
#include <spot/twaalgos/translate.hh>
#include <spot/twaalgos/emptiness.hh>

#include <iostream>
#include <fstream>
#include <sstream>
#include <vector>
#include <algorithm>

using explicit_Kripke            = spot::kripke_graph_ptr;
using Automata                   = spot::twa_ptr;
using const_Automata             = spot::const_twa_ptr;
using explicit_Automata          = spot::twa_graph_ptr;
using const_explicit_Automata    = spot::const_twa_graph_ptr;
using State                      = uint32_t;
struct Edge { State from; State to; bdd cond; };

struct model_info {
    State States;
    State Initial;

    std::vector<std::string>          Symbols;
    std::vector< std::vector<bool> >  Labels;
    std::vector< std::vector<State> > Transitions;

    friend std::ostream& operator << (std::ostream& os, const model_info& m);
};


class Checker {
    public:
        Checker();

    // Reading user input
    bool read_kripke(std::string filename, model_info& model) const;
    explicit_Kripke  make_explicit(const model_info& m) const;
    explicit_Kripke  explicit_door_kripke() const;

    void verify(const_Automata model, const std::string formula) const;

    explicit_Automata defineBuchi(const_Automata model) const;

    explicit_Automata defineMutex3(const_Automata model) const;

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
        explicit_Automata initBuchi(    const_Automata model,
                                        unsigned n,
                                        State init) const;

        explicit_Automata buildBuchi(   explicit_Automata aut,
                                        const std::vector<State> &init,
                                        const std::vector<Edge> edges) const;
};

#endif
