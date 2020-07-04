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

/*  Example Creates a small explicit model: push a button -> opens a door -> walk through
    the door. Used for testing. */
explicit_Kripke Checker::explicitDoor() {
    model_info model;
    model.States = 3;
    model.Initial = 0;
    model.Symbols = {"open", "pushed", "through"};

    model.Labels = { {0, 0, 0},
                     {1, 1, 0},
                     {1, 1, 1} };

    model.Transitions = { {0, 1},
                          {0, 1, 2},
                          {1, 2} };

    return makeExplicit(model);
}

/*  Example: Creates a small explicit model: Traffic light, either red or green
    Used for testing. */
const_Kripke Checker::explicitTraffic() {
    model_info model;
    model.States = 2;
    model.Initial = 0;
    model.Symbols = {"red", "green"};

    model.Labels = { {1, 0},
                     {0, 1} };

    model.Transitions = { {1},
                          {0} };

    return makeExplicit(model);
}

/*  Example: Creates a small explicit model: Traffic light, either off, red or green
    Used for testing. */
const_Kripke Checker::explicitTraffic2() {
    model_info model;
    model.States = 3;
    model.Initial = 0;
    model.Symbols = {"red", "green"};

    model.Labels = { {1, 0},
                     {0, 1},
                     {0, 0} };

    model.Transitions = { {1, 2},
                          {0},
                          {0} };

    return makeExplicit(model);
}

/*  Create a Kripke graph from a specified model using the 'Explicit' method
    Abstracts a Spot functionality */
explicit_Kripke Checker::makeExplicit(const model_info& m) {
    assert(m.States == m.Transitions.size());
    assert(m.States == m.Labels.size());
    assert(m.Initial < m.States);

    spot::bdd_dict_ptr dict = spot::make_bdd_dict();
    spot::kripke_graph_ptr graph = spot::make_kripke_graph(dict);

    State* states = new State[m.States];
    bdd* symbols = new bdd[m.Symbols.size()];

    // Initialize symbols
    for(uint32_t i = 0; i < m.Symbols.size(); i++)
        symbols[i] = bdd_ithvar(graph->register_ap(m.Symbols[i]));

    // Assign labels to states
    for(State s = 0; s < m.Labels.size(); s++) {
        assert(m.Labels[0].size() == m.Symbols.size());
        bdd temp = m.Labels[s][0] ? symbols[0] : !(symbols[0]);

        for(uint32_t i = 1; i < m.Labels[0].size(); i++) {
            assert(m.Labels[i].size() == m.Symbols.size());
            temp &= m.Labels[s][i] ? symbols[i] : (!symbols[i]);
        }

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

/*  Parse an LTL string using the Spot translator and convert its negation
    into an automaton */
void Checker::spotVerify(const_Kripke model, const std::string formula) {
    spot::parsed_formula parsed = spot::parse_infix_psl(formula);
    spot::formula f = spot::formula::Not(parsed.f);
    explicit_Automaton f_auto = spot::translator(model->get_dict()).run(f);

    std::cout << "Formula: " << formula;
    spotVerify(model, f_auto);
}

/*  Use spot to compute an intersecting run betmeen the model and Buchi automaton
    If none exists, the inverse of the Buchi is not always true
    Else this inverse always holds */
void Checker::spotVerify(const_Kripke model, explicit_Automaton neg_formula) {
    if(auto run = model->intersecting_run(neg_formula))
        std::cout << " violated by: \n" << *run << std::endl << std::endl;
    else
        std::cout << " verified" << std::endl <<  std::endl;
}

/*  Parse an LTL string using the Spot translator and convert its negation
    into an automaton */
void Checker::myVerify(const_Kripke model, const std::string formula) {
    spot::parsed_formula parsed = spot::parse_infix_psl(formula);
    spot::formula f = spot::formula::Not(parsed.f);
    explicit_Automaton f_auto = spot::translator(model->get_dict()).run(f);

    std::cout << "Formula: " << formula;
    myVerify(model, f_auto);
}

/*  Use CrossProduct to compute an intersecting run betmeen the model and Buchi
    automaton If none exists, the inverse of the Buchi is not always true
    Else this inverse always holds */
void Checker::myVerify(const_Kripke model, explicit_Automaton neg_formula) {
    CrossProduct cross(model, neg_formula);
    if(cross.accept()) {
        std::cout << " violated by: \n";
        cross.trace();
    }
    else
        std::cout << " verified" << std::endl <<  std::endl;
}

/*  Uses Spot to initialize a Buchi automaton with 'n' states and an initial
    state 'init'. Returns a pointer to this automaton.  */
explicit_Automaton Checker::initBuchi(const_Automaton model, unsigned n, State init) {
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

/*  Example, complement of 3 process mutex:
    i.e. eventually there are two proc_tesses in their critical section */
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

/*  Example, complement of:
    if light ever turns off green, it will eventually be green again */
explicit_Automaton Checker::defineTrafficBuchi(const_Automaton model) {
    unsigned n_states       = 3;
    State initial_state     = 0;
    std::vector<State> accepting_states = { 1 };

    explicit_Automaton automaton = initBuchi(model, n_states, initial_state);

    bdd True = !bdd();
    bdd False = bdd();

    bdd red = bdd_ithvar(automaton->register_ap("red") );
    bdd green = bdd_ithvar(automaton->register_ap("green") );

    std::vector<Edge> edges =
    {
        { 0, 0, True },
        { 0, 1, !green },
        { 1, 1, !green },
        { 1, 2, green },
        { 2, 2, True}
    };

    return buildBuchi(automaton, accepting_states, edges);
}

/*  TODO Template for defining an explicit Buchi automata representing
    some property we wish to check */
explicit_Automaton Checker::defineBuchi(const_Automaton model) {
    //TODO define states used
    unsigned n_states; //numbered from 0 ... n-1

    //TODO define an initial state (unsigned integer)
    State initial_state;

    //TODO define accepting states
    //NOTE an accepting state must have outgoing transitions
    std::vector<State> accepting_states;

    //intitializes our automata in spot
    explicit_Automaton automata = initBuchi(model, n_states, initial_state);

    bdd True = bddtrue;
    bdd False = bddfalse;
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

    //build automata structure using the edges
    return buildBuchi(automata, accepting_states, edges);
}
