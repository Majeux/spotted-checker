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

bool Checker::read_kripke(std::string filename, model_info& model) const {
    std::ifstream file (filename);
    std::string line;
    uint32_t s = 0;

    if (file.is_open()) {
        // read S

        while(std::getline (file, line)) {
            if(line[0] == '\n' || line[0] == '#')
                continue;

            istringstream ss(line);
            std::string x;

            switch(s) { //TODO clean up, class or more function calls
                case 0:
                    if(ss >> x && x == "S")
                        if( !(ss >> model.States) ) {
                            std::cerr << "-- ERROR: Missing value for S when reading: " << filename << std::endl;
                            return false;
                        }
                    else {
                        std::cerr << "-- ERROR: Missing line for S when reading: " << filename << std::endl;
                        return false;
                    }
                    s++;
                    break;

                case 1:
                    if(ss >> x && x == "I")
                        if( !(ss >> model.Initial) ) {
                            std::cerr << "-- ERROR: Missing value for I when reading: " << filename << std::endl;
                            return false;
                        }
                    else {
                        std::cerr << "-- ERROR: Missing line for I when reading: " << filename << std::endl;
                        return false;
                    }
                    s++;
                    break;

                case 2:
                    if(ss >> x && x == "P") {
                        while(ss >> x)
                             model.Symbols.push_back()

                        if( model.Symbols.size() == 0 ) {
                           std::cerr << "-- ERROR: Missing values for P when reading: " << filename << std::endl;
                           return false;
                       }
                    }
                    else {
                        std::cerr << "-- ERROR: Missing line for P when reading: " << filename << std::endl;
                        return false;
                    }
                    s++;
                    break;

                case 3:
                    if(ss >> x && x == "L") {
                        std::vector<bool> v;
                        for(int i = 0; i < model.States; i++) {
                            if(i > 0) {
                                if(!std::getline(file, line)) {
                                    std::cerr << "-- ERROR: Missing line for L when reading: " << filename << std::endl;
                                    return false;
                                }
                                ss.str(line);
                            }

                            bool b;
                            for(int j = 0; j < model.Symbols.size()) {
                                if( !(ss >> b) ) {
                                    std::cerr << "-- ERROR: Missing values for L when reading: " << filename << std::endl;
                                    return false;
                                }
                                v.push_back(b)
                            }

                            model.Labels.push_back(v);
                            v.clear();
                        }
                    } else {
                        std::cerr << "-- ERROR: Missing line for L when reading: " << filename << std::endl;
                        return false;
                    }
                    s++;
                    break;
            }
        }

        if(!(ss >> x) {
            std::cerr << "-- ERROR: Missing line when reading S from: " << filename << std::endl;
            return false;
        }
        if(x == "S")
            ss >> model.States;



        myfile.close();
    }
    else cout << "-- ERROR: Unable to open file: " << filename << std::endl;


}
