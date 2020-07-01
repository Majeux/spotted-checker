#include <iostream>

#include <spot/twa/bdddict.hh>

#include "checker.h"

std::ostream& operator<< (std::ostream& os, const model_info& m) {
    os  << "model_info:" << std::endl
        << "No. States: \t" << m.States << std::endl
        << "Initial State: \t" << m.Initial << std::endl;

    os << "Symbols: \t";
    for(auto s : m.Symbols)
        os << s << ' ';

    State i = 0;
    os << std::endl << "Labels: \t" << i++ << ": ";
    for(auto v : m.Labels) {
        for(auto l : v)
            os << l << " ";
        if(i < m.Labels.size())
            os << std::endl << "\t\t" << i++ << ": ";
    }

    i = 0;
    os << std::endl << "Transitions: \t" << i++ << "-> ";
    for(auto v : m.Transitions) {
        for(auto t : v)
            os << t << " ";
        if(i < m.Transitions.size())
        os << std::endl << "\t\t" << i++ << "-> ";
    }

   return os << std::endl;
}

Checker::Checker() {
    std::cerr << "-- create Checker" << std::endl;
}

// CREATE MODEL
explicit_Kripke Checker::explicit_door_kripke() {
    spot::bdd_dict_ptr dict = spot::make_bdd_dict();
    explicit_Kripke k = spot::make_kripke_graph(dict);

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

const_Kripke Checker::explicit_traffic() {
    spot::bdd_dict_ptr dict = spot::make_bdd_dict();
    explicit_Kripke k = spot::make_kripke_graph(dict);

    bdd red    = bdd_ithvar(k->register_ap("red"));
    bdd green  = bdd_ithvar(k->register_ap("green"));

    unsigned x0 = k->new_state(red);
    unsigned x1 = k->new_state(green);

    k->set_init_state(x0);

    k->new_edge(x0, x1);

    k->new_edge(x1, x0);

    auto names = new std::vector<std::string> { "s0", "s1" };
    k->set_named_prop("state-names", names);

    return k;
}

const_Kripke Checker::explicit_traffic2() {
    spot::bdd_dict_ptr dict = spot::make_bdd_dict();
    explicit_Kripke k = spot::make_kripke_graph(dict);

    bdd False  = bdd();
    bdd red    = bdd_ithvar(k->register_ap("red"));
    bdd green  = bdd_ithvar(k->register_ap("green"));

    unsigned x0 = k->new_state(red);
    unsigned x1 = k->new_state(green);
    unsigned x2 = k->new_state((!red) & (!green));

    k->set_init_state(x0);

    k->new_edge(x0, x1);

    k->new_edge(x1, x0);

    k->new_edge(x0, x2);
    k->new_edge(x2, x0);

    auto names = new std::vector<std::string> { "s0", "s1", "s2" };
    k->set_named_prop("state-names", names);

    return k;
}

// Create a Kripke graph from a specified model using the 'Explicit' method
explicit_Kripke Checker::make_explicit(const model_info& m) {
    spot::bdd_dict_ptr dict = spot::make_bdd_dict();
    spot::kripke_graph_ptr graph = spot::make_kripke_graph(dict);

    State* states = new State[m.States];
    bdd* symbols = new bdd[m.Symbols.size()];

    // Initialize symbols
    for(uint32_t i = 0; i < m.Symbols.size(); i++)
        symbols[i] = bdd_ithvar(graph->register_ap(m.Symbols[i]));

    // Assign labels to states
    for(State s = 0; s < m.Labels.size(); s++) {
        auto temp = m.Labels[s][0] ? symbols[0] : !(symbols[0]);

        for(uint32_t i = 1; i < m.Labels[0].size(); i++)
            temp &= m.Labels[s][i] ? symbols[i] : (!symbols[i]);

        states[s] = graph->new_state(temp);
    }

    // Set states
    graph->set_init_state(states[m.Initial]);

    for(State from = 0; from < m.Transitions.size(); from++) {
        for(State to : m.Transitions[from])
            graph->new_edge(from, to);
    }

    delete[] states;
    delete[] symbols;
    
    return graph;
}

void Checker::spotVerify(const_Kripke model, const std::string formula) {
    spot::parsed_formula parsed = spot::parse_infix_psl(formula);
    spot::formula f = spot::formula::Not(parsed.f);
    explicit_Automaton f_auto = spot::translator(model->get_dict()).run(f);

    std::cout << "Formula: " << formula;
    spotVerify(model, f_auto);
}

void Checker::spotVerify(const_Kripke model, explicit_Automaton neg_formula) {

    if(auto run = model->intersecting_run(neg_formula))
        std::cout << " violated by: \n" << *run << std::endl << std::endl;
    else
        std::cout << " verified" << std::endl <<  std::endl;
}

void Checker::myVerify(const_Kripke model, const std::string formula) {
    spot::parsed_formula parsed = spot::parse_infix_psl(formula);
    spot::formula f = spot::formula::Not(parsed.f);
    explicit_Automaton f_auto = spot::translator(model->get_dict()).run(f);

    std::cout << "Formula: " << formula;
    // spot::print_dot(std::cout, f_auto);
    myVerify(model, f_auto);
}

void Checker::myVerify(const_Kripke model, explicit_Automaton neg_formula) {
    CrossProduct cross(model, neg_formula);
    if(cross.accept()) {
        std::cout << " violated by: \n";
        cross.trace();
    }
    else
        std::cout << " verified" << std::endl <<  std::endl;

}

explicit_Automaton Checker::initBuchi(const_Automaton model, unsigned n, State init) {
    //Initialize automaton
    explicit_Automaton aut = spot::make_twa_graph(model->get_dict());
    aut->prop_state_acc(true); //Sets state based acceptance
    aut->set_buchi(); //Sets accepting condition for Buchi
    aut->new_states(n);
    aut->set_init_state(init);

    return aut;
}

explicit_Automaton Checker::buildBuchi(explicit_Automaton aut, const std::vector<State> &init, const std::vector<Edge> edges) {
    bool* acc_index = new bool[edges.size()];
    bool first = true;
    size_t i;

    //A state is considered accepting if all outgoing transitions are in {0}
    for(State s : init) {
        i = 0;
        for(Edge e : edges) {
            if(e.from == s)
                acc_index[i] = true;
            else if(first)
                acc_index[i] = false;

            i++;
        }
        first = false;
    }

    i = 0;
    for(Edge e : edges) {
        if(acc_index[i++])
            aut->new_edge(e.from, e.to, e.cond, {0});
        else
            aut->new_edge(e.from, e.to, e.cond);
    }

    delete[] acc_index;

    return aut;
}

//TODO Define an explicit Buchi automata representing some property we wish to check
explicit_Automaton Checker::defineBuchi(const_Automaton model) {
    //TODO define states used
    unsigned n_states       = 2; //numbered from 0 ... n-1

    //TODO define an initial state
    State initial_state     = 0;

    //TODO define accepting states
    //NOTE an accepting state must have outgoing transitions
    std::vector<State> accepting_states = { 0 };

    //intitializes our automata in spot
    explicit_Automaton automata = initBuchi(model, n_states, initial_state);

    bdd True = !bdd();
    bdd False = bdd();
    //TODO define some propery variables used in the transitions
    //NOTE property names should correspond to the ones in the model
    bdd property1 = bdd_ithvar(automata->register_ap("property1"));
    bdd property2 = bdd_ithvar(automata->register_ap("property2"));

    //TODO define the edges in our automaton
    //NOTE construct an edge with {from State, to State, bdd property}
    std::vector<Edge> edges =
    {
        { 0, 1,  property1 & !property2 },
        { 0, 0, !property1 },
        { 1, 1, True }
        /* ... */
    };

    return buildBuchi(automata, accepting_states, edges);
}

//complement of 3 v/ariable mutex:
//eventually there are two processes in their critical section
explicit_Automaton Checker::defineMutex3(const_Automaton model) {
    unsigned n_states       = 2;
    State initial_state     = 0;
    std::vector<State> accepting_states = { 1 };

    explicit_Automaton automata = initBuchi(model, n_states, initial_state);

    bdd True = !bdd();
    bdd False = bdd();

    bdd c0 = bdd_ithvar(automata->register_ap("crit0") );
    bdd c1 = bdd_ithvar(automata->register_ap("crit1") );
    bdd c2 = bdd_ithvar(automata->register_ap("crit2") );

    std::vector<Edge> edges =
    {
        { 0, 0, !((c0 & c1) | (c0 & c2) | (c1 & c2)) },
        { 0, 1, (c0 & c1) | (c0 & c2) | (c1 & c2) },
        { 1, 1, True }
    };

    return buildBuchi(automata, accepting_states, edges);
}

explicit_Automaton Checker::defineTrafficBuchi(const_Automaton model) {
    unsigned n_states       = 3;
    State initial_state     = 0;
    std::vector<State> accepting_states = { 1 };

    explicit_Automaton automata = initBuchi(model, n_states, initial_state);

    bdd True = !bdd();
    bdd False = bdd();

    bdd red = bdd_ithvar(automata->register_ap("red") );
    bdd green = bdd_ithvar(automata->register_ap("green") );

    std::vector<Edge> edges =
    {
        { 0, 0, True },
        { 0, 1, !green },
        { 1, 1, !green },
        { 1, 2, green },
        { 2, 2, True}
    };

    return buildBuchi(automata, accepting_states, edges);
}
