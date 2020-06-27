#ifndef CROSS_STATE_H
#define CROSS_STATE_H

#include <spot/twa/twa.hh>

/*  Uses state_unicity table to ensure that each state has a unique pointer.
    Tables are set in CrossProduct. */

// class CrossState {
//     friend class CrossProduct;
//
//     private:
//         const spot::state* a_;
//         const spot::state* b_;
//
//         static spot::state_unicity_table* A_seen;
//         static spot::state_unicity_table* B_seen;
//
//     public:
//         //copy-constructor
//         CrossState(spot::state* a, spot::state* b) : a_(a),     b_(b) { }
//         CrossState(const CrossState &s)            : a_(s.a_),  b_(s.b_) { }
//
//
//         bool operator==(const CrossState &s) const {
//             return a_ == s.a_ && b_ == s.b_;
//         }
//
//         const spot::state* a() const { return a_; }
//         const spot::state* b() const { return b_; }
//
//         //!NOTE use this function to initialize a CrossState
//         //      use constructor only for copying already initialized/seen states
//         static CrossState unique(spot::state* a, spot::state* b) {
//             return CrossState( (*A_seen)(a), (*B_seen)(b));
//         }
// };

#endif
