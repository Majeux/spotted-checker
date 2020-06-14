#include <iostream>

#include <spot/twa/bdddict.hh>

#include "checker.h"

Checker::Checker() {
    std::cerr << "-- create Checker" << std::endl;
}

// CREATE MODEL
spot::kripke_graph_ptr Checker::explicit_door_kripke() const {
    spot::bdd_dict_ptr dict = spot::make_bdd_dict();
    spot::kripke_graph_ptr k = spot::make_kripke_graph(dict);

    bdd open    = bdd_ithvar(k->register_ap("open"));
    bdd pushed  = bdd_ithvar(k->register_ap("pushed"));
    bdd through = bdd_ithvar(k->register_ap("through"));

    unsigned x0 = k->new_state((!open) & (!pushed) & !through);
    unsigned x1 = k->new_state(  open &    pushed & !through);
    unsigned x2 = k->new_state(  open &    pushed & through);

    k->set_init_state(x0);

    k->new_edge(x0, x0);
    k->new_edge(x0, x1);

    k->new_edge(x1, x1);
    k->new_edge(x1, x0);
    k->new_edge(x1, x2);

    k->new_edge(x2, x2);
    k->new_edge(x2, x1);

    auto names = new std::vector<std::string> { "x0", "x1", "x2" };
    k->set_named_prop("state-names", names);

    return k;
}
