#ifndef CROSS_UNICITY_H
#define CROSS_UNICITY_H

#include <spot/twa/twa.hh>

#include <set>

using state_pair = std::pair< const spot::state*, const spot::state* >;

/*  Hash functor for hashing a pair of state pointers.
Based on the boost hash_combine function, adapted for two hashes. */
struct state_pair_hash {
    size_t operator()(state_pair s) const {
        assert(s.first != nullptr); assert(s.second != nullptr);

        size_t h = s.first->hash() + 0x9e3779b9 + (0<<6) + (0>>2);
        return h ^(s.second->hash() + 0x9e3779b9 + (h<<6) + (h>>2));
    }
};

struct state_pair_equal {
    bool operator()(state_pair a, state_pair b) const {
        assert(a.first != nullptr); assert(a.second != nullptr);
        assert(b.first != nullptr); assert(b.second != nullptr);

        return a.first->compare(b.first) == 0 && a.second->compare(b.second) == 0;
    }
};


/*  Class derived from the SPOT_API state_unicity_table from "spot/twa/twa.hh"
    to accept pairs of pairs of state pointers
*/
class cross_unicity_table {
    using cross_state_set = std::unordered_set< state_pair,
                                                state_pair_hash,
                                                state_pair_equal    >;
    private:
        cross_state_set set;

    public:
        const state_pair operator()(const spot::state* s1, const spot::state* s2) {
            auto p = set.emplace(s1, s2);
            if(!p.second) {
                s1->destroy();
                s2->destroy();
            }
            return *p.first;
        }

        const state_pair operator()(state_pair p) {
            return operator()(p.first, p.second);
        }

        const state_pair is_new(const spot::state* s1, const spot::state* s2) {
            auto p = set.emplace(s1, s2);
            if(!p.second) {
                s1->destroy();
                s2->destroy();
                return std::make_pair(nullptr,nullptr);
            }
            return *p.first;
        }

        const state_pair is_new(state_pair p) {
            return is_new(p.first, p.second);
        }

        ~cross_unicity_table() {
            for(cross_state_set::iterator i = set.begin(); i != set.end();) {
                cross_state_set::iterator old = i++;
                (*old).first->destroy();
                (*old).second->destroy();
            }
        }

        size_t size() const { return set.size(); }
};

#endif
