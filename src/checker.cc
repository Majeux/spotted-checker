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

    state i = 0;
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
            for(state i = 0; i < model.States; i++) {
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
            for(state i = 0; i < model.States; i++) {
                if(!std::getline (file, line)) {
                    std::cerr << "-- ERROR: Missing line for Transitions: " << filename << std::endl;
                    return false;
                }
                ss.clear();
                ss.str(line);
                std::vector<state> v; state s;

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
kripke_ptr Checker::make_explicit(const model_info& m) const {
    spot::bdd_dict_ptr dict = spot::make_bdd_dict();
    spot::kripke_graph_ptr graph = spot::make_kripke_graph(dict);

    state* states = new state[m.States];
    bdd* symbols = new bdd[m.Symbols.size()];

    // Initialize symbols
    for(uint32_t i = 0; i < m.Symbols.size(); i++)
        symbols[i] = bdd_ithvar(graph->register_ap(m.Symbols[i]));

    // Assign labels to states
    for(state s = 0; s < m.Labels.size(); s++) {
        auto temp = m.Labels[s][0] ? symbols[0] : !(symbols[0]);

        for(uint32_t i = 1; i < m.Labels[0].size(); i++)
            temp &= m.Labels[s][i] ? symbols[i] : (!symbols[i]);

        states[s] = graph->new_state(temp);
    }

    // Set states
    graph->set_init_state(states[m.Initial]);

    for(state from = 0; from < m.Transitions.size(); from++) {
        for(state to : m.Transitions[from])
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
