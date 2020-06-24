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
explicit_Kripke Checker::explicit_door_kripke() const {
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

//TODO check if presizing possible in vectors (based on N_STATES)
// Read specification of a model from a file
bool Checker::read_kripke(std::string filename, model_info& model) const {
    std::ifstream file (filename);

    if (file.is_open()) {
        std::string line;
        // read S
        if(!std::getline (file, line)) {
            std::cerr << "-- ERROR: Missing line for N_States and Initial_State: " << filename << std::endl;
            return false;
        }

        std::istringstream ss(line); std::string x;

        if(ss >> model.States >> model.Initial) {
            //Read Symbols
            if(!std::getline (file, line)) {
                std::cerr << "-- ERROR: Missing line for Symbols: " << filename << std::endl;
                return false;
            }
            ss.clear();
            ss.str(line);

            while(ss >> x)
                model.Symbols.push_back(x);

            if(model.Symbols.size() == 0) {
                std::cerr << "-- ERROR: Missing values for Symbols: " << filename << std::endl;
                return false;
            }

            //Read Labels
            for(State i = 0; i < model.States; i++) {
                if(!std::getline (file, line)) {
                    std::cerr << "-- ERROR: Missing line for Labels: " << filename << std::endl;
                    return false;
                }
                ss.clear();
                ss.str(line);
                std::vector<bool> v; bool b;

                for(uint32_t j = 0; j < model.Symbols.size(); j++) {
                    if(ss >> b)
                        v.push_back(b);
                    else {
                        std::cerr << "-- ERROR: Missing values for Labels: " << filename << std::endl;
                        return false;
                    }
                }
                model.Labels.push_back(v); v.clear();
            }
            //Read Transitions
            for(State i = 0; i < model.States; i++) {
                if(!std::getline (file, line)) {
                    std::cerr << "-- ERROR: Missing line for Transitions: " << filename << std::endl;
                    return false;
                }
                ss.clear();
                ss.str(line);
                std::vector<State> v; State s;

                while(ss >> s)
                    v.push_back(s);

                model.Transitions.push_back(v); v.clear();
            }
        }
        else {
            std::cerr << "-- ERROR: Missing values for N_States or Initial_State: " << filename << std::endl;
            return false;
        }

        file.close();
        return true;
    }

    std::cout << "-- ERROR: Unable to open file: " << filename << std::endl;
    return false;
}

// Create a Kripke graph from a specified model using the 'Explicit' method
explicit_Kripke Checker::make_explicit(const model_info& m) const {
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

    //TODO add naming
    /*
    auto names = new std::vector<std::string> { "x0", "x1", "x2" };
    k->set_named_prop("state-names", names);
    */

    return graph;
}

void Checker::verify(const_Automata model, const std::string formula) const {
    spot::parsed_formula parsed = spot::parse_infix_psl(formula);
    spot::formula f = spot::formula::Not(parsed.f);
    explicit_Automata f_auto = spot::translator(model->get_dict()).run(parsed.f);

    spot::print_dot(std::cout, f_auto);
    if(auto run = model->intersecting_run(f_auto))
        std::cout << formula << " violated by: \n" << *run << std::endl << std::endl;
    else
        std::cout << "verified:\n" << formula << std::endl <<  std::endl;

}


explicit_Automata Checker::initBuchi(const_Automata model, unsigned n, State init) const {
    //Initialize automaton
    explicit_Automata aut = spot::make_twa_graph(model->get_dict());
    aut->prop_state_acc(true); //Sets state based acceptance
    aut->set_buchi(); //Sets accepting condition for Buchi
    aut->new_states(n);
    aut->set_init_state(init);

    return aut;
}

explicit_Automata Checker::buildBuchi(explicit_Automata aut, const std::vector<State> &init, const std::vector<Edge> edges) const {
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
explicit_Automata Checker::defineBuchi(const_Automata model) const {
    //TODO define states used
    unsigned n_states       = 2; //numbered from 0 ... n-1

    //TODO define an initial state
    State initial_state     = 0;

    //TODO define accepting states
    //NOTE an accepting state must have outgoing transitions
    std::vector<State> accepting_states = { 0 };

    //intitializes our automata in spot
    explicit_Automata automata = initBuchi(model, n_states, initial_state);

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
explicit_Automata Checker::defineMutex3(const_Automata model) const {
    unsigned n_states       = 2;
    State initial_state     = 0;
    std::vector<State> accepting_states = { 1 };

    explicit_Automata automata = initBuchi(model, n_states, initial_state);

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
