#ifndef ALIAS_H
#define ALIAS_H

#include <spot/twa/twa.hh>
#include <spot/kripke/kripkegraph.hh>
#include <spot/kripke/kripke.hh>
#include <stack>

using const_Kripke                = spot::const_kripke_ptr;
using explicit_Kripke             = spot::kripke_graph_ptr;
using Automaton                   = spot::twa_ptr;
using const_Automaton             = spot::const_twa_ptr;
using explicit_Automaton          = spot::twa_graph_ptr;
using const_explicit_Automaton    = spot::const_twa_graph_ptr;
using State                       = uint32_t;

using state_pair = std::pair<   const spot::state*,
                                const spot::state*  >;
using iter_pair  = std::pair<   spot::twa_succ_iterator*,
                                spot::twa_succ_iterator*    >;
#endif
