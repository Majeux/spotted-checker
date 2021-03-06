#include "model_example/kripke.h"

MyKripke::MyKripke(size_t n, const spot::bdd_dict_ptr& d) : AbstractKripke(d), _N(n) {
    assert(_N > 0);
    crit = new bdd[_N];
    wait = new bdd[_N];

    for(size_t i = 0; i < (size_t)_N; i++) {
        crit[i] = bdd_ithvar( register_ap(critical_string(i)) );
        wait[i] = bdd_ithvar( register_ap(waiting_string(i)) );
    }
}

MyKripke::~MyKripke() {
    std::cerr << "delete MyKripke" << std::endl;
    delete[] crit;
    delete[] wait;
}

MyState* MyKripke::get_init_state() const {
    return new MyState(_N);
}

/*  Wrapper is needed because AbstractIterator cannot be constructed as an
    abstract class. Simply allocate our implementation of AbstractIterator
    and return.
*/
AbstractIterator* MyKripke::makeIterator(const spot::state* s, bdd condition) const {
    const MyState* state = static_cast<const MyState*>(s);
    return new MyIterator(state, condition);
}

/*  Track if a proc_tess is in its critical section to verify mutex
    Track if a proc_tess is waiting to enter a critical section to verify non-starvation
*/
bdd MyKripke::state_condition(const spot::state* s) const {
    const MyState* state = static_cast<const MyState*>(s);

    const std::vector<proc_t>& p  = state->get(pc);
    const std::vector<proc_t>& lvl = state->get(level);

    bdd crit_condition = p[0] == 4 ? crit[0] : !crit[0];
    bdd wait_condition = lvl[0] > -1 && p[0] < 4 ? wait[0] : !wait[0];

    for(size_t i = 1; i < (size_t)_N; i++) {
        crit_condition &= p[i] == 4 ? crit[i] : !crit[i];
        wait_condition &= lvl[i] > -1 && p[i] < 4 ? wait[i] : !wait[i];
    }

    return crit_condition & wait_condition;
}

/*  We only case about our state_variable elements. Casting to long int would support larger proc_t sizes. */
std::string MyKripke::format_state(const spot::state* s) const {
    auto state = static_cast<const MyState*>(s);
    std::ostringstream out;
    const state_variables& sv = state->getStateVars();

    out << "pc = [ ";
    for(auto i : sv&pc )
        out << (long int)i << ", ";
    out << " ]" << std::endl
        << "level = [ ";

    for(auto i : sv&level )
        out << (long int)i << ", ";
    out << " ] "
        << "last_to_enter = [ ";

    for(auto i : sv&last_to_enter )
        out << (long int)i << ", ";
    out << " ]" << std::endl;

    return out.str();
}

/*  Auxiliary functions that define a format for our property names */
std::string MyKripke::critical_string(proc_t i) {
    std::string critical = "crit";
    critical += '0' + i;
    return critical;
}

std::string MyKripke::waiting_string(proc_t i) {
    std::string waiting = "wait";
    waiting += '0' + i;
    return waiting;
}
