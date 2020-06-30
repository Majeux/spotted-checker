#ifndef CROSS_UNICITY_H
#define CROSS_UNICITY_H

#include <spot/twa/twa.hh>

#include <set>

#include "aliases.h"

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

using state_tuple = std::tuple< const spot::state*, const spot::state*, bool* >;

/*  Hash functor for hashing a tuple of state pointers.
Based on the boost hash_combine function, adapted for two hashes. */
struct state_tuple_hash {
    size_t operator()(state_tuple s) const {
        assert(std::get<0>(s) != nullptr); assert(std::get<1>(s) != nullptr);

        size_t h = std::get<0>(s)->hash() + 0x9e3779b9 + (0<<6) + (0>>2);
        return h ^(std::get<1>(s)->hash() + 0x9e3779b9 + (h<<6) + (h>>2));
    }
};

struct state_tuple_equal {
    bool operator()(state_tuple a, state_tuple b) const {
        assert(std::get<0>(a) != nullptr); assert(std::get<1>(a) != nullptr);
        assert(std::get<0>(b) != nullptr); assert(std::get<1>(b) != nullptr);
        //NOTE the boolean is not an identifier
        return std::get<0>(a)->compare(std::get<0>(b)) == 0 && std::get<1>(a)->compare(std::get<1>(b)) == 0;
    }
};


class unicity_table_base {
    protected:
        unicity_table_base* other = nullptr;
    public:
        void set_co_table(unicity_table_base* t) { other = t; }
        virtual bool contains(state_pair) const = 0;
        virtual const state_pair operator()(const spot::state* s1,
                                            const spot::state* s2) = 0;
        virtual const state_pair operator()(state_pair p) = 0;
        virtual const state_pair is_new(const spot::state* s1,
                                        const spot::state* s2) = 0;
        virtual const state_pair is_new(state_pair p) = 0;
        virtual size_t size() const = 0;
};

/*  Class derived from the SPOT_API state_unicity_table from "spot/twa/twa.hh"
    to accept pairs of pairs of state pointers
*/
class cross_unicity_table : public unicity_table_base {
    using cross_state_set = std::unordered_set< state_pair,
                                                state_pair_hash,
                                                state_pair_equal    >;
    private:
        cross_state_set set;

    public:
        const state_pair operator()(const spot::state* s1, const spot::state* s2) override {
            auto p = set.emplace(s1, s2);
            if(!p.second) {
                s1->destroy();
                s2->destroy();
            }
            return *p.first;
        }

        const state_pair operator()(state_pair p) override  {
            return operator()(p.first, p.second);
        }

        const state_pair is_new(const spot::state* s1, const spot::state* s2) override {
            auto p = set.emplace(s1, s2);
            if(!p.second) {
                s1->destroy();
                s2->destroy();
                return std::make_pair(nullptr,nullptr);
            }
            return *p.first;
        }

        const state_pair is_new(state_pair p) override {
            return is_new(p.first, p.second);
        }

        bool contains(state_pair p) const override {
            return set.find(p) != set.end();
        }

        void flush() {
            for(cross_state_set::iterator i = set.begin(); i != set.end();) {
                state_pair old = *(i++);

                if(other == nullptr || !other->contains(old)) {
                    if(old.first)
                        old.first->destroy();

                    if(old.second)
                        old.second->destroy();
                }
            }
        }

        ~cross_unicity_table() { flush(); }

        size_t size() const override { return set.size(); }

        void clear() {
            flush();
            set.clear();
        }
};

/*  Class derived from the SPOT_API state_unicity_table from "spot/twa/twa.hh"
    to accept pairs of pairs of state pointers.
    Each pair of states has a boolean mark attached to it. This denotes if during
    a search for a cycle through state '<s,q>', the corresponding state_pair can reach
    '<s,q>'.
*/
class marked_unicity_table : public unicity_table_base {
    using marked_state_set = std::unordered_set< state_tuple,
                                                state_tuple_hash,
                                                state_tuple_equal    >;
    private:
        marked_state_set set;

    public:
        const state_pair operator()(const spot::state* s1, const spot::state* s2) override {
            bool* mark = new bool(true); //mark as true when pushed = when registered
            auto p = set.emplace(s1, s2, mark);
            if(!p.second) {
                s1->destroy();
                s2->destroy();
                delete mark;
            }
            return std::make_pair(std::get<0>(*p.first), std::get<1>(*p.first));
        }

        const state_pair operator()(state_pair p) override {
            return operator()(p.first, p.second);
        }

        /*  Set the mark of a state_pair (usually to false)
            Returns null,null is the pair does not exist */
        state_pair mark(const spot::state* s1, const spot::state* s2, bool m) {
            bool temp = false;
            auto p = set.find(std::make_tuple(s1, s2, &temp));

            if(p == set.end())
                return std::make_pair(nullptr, nullptr);

            bool* mark = std::get<2>(*p);
            *mark = m;

            return std::make_pair(std::get<0>(*p), std::get<1>(*p));
        }

        const state_pair mark(state_pair p, bool m) {
            return mark(p.first, p.second, m);
        }

        bool get_mark(const spot::state* s1, const spot::state* s2) {
            bool temp = false;
            auto p = set.find(std::make_tuple(s1, s2, &temp)); //bool doesnt matter

            if(p == set.end())
                return false; //not yet seen states are always unmarked

            return std::get<2>(*p);
        }

        bool get_mark(state_pair p) {
            return get_mark(p.first, p.second);
        }

        const state_pair is_new(const spot::state* s1, const spot::state* s2) override {
            bool* mark = new bool(true);
            auto p = set.emplace(s1, s2, mark);
            if(!p.second) { //not new
                s1->destroy();
                s2->destroy();
                delete mark;
                return std::make_pair(nullptr,nullptr);
            }
            return std::make_pair(std::get<0>(*p.first), std::get<1>(*p.first));
        }

        const state_pair is_new(state_pair p) override {
            return is_new(p.first, p.second);
        }

        bool contains(state_pair p) const override {
            bool temp = true;
            state_tuple t = std::make_tuple(p.first, p.second, &temp);
            return set.find(t) != set.end();
        }

        void flush() {
            for(marked_state_set::iterator i = set.begin(); i != set.end();) {
                state_tuple old = *(i++);
                if(std::get<0>(old))
                    std::get<0>(old)->destroy();
                if(std::get<1>(old))
                    std::get<1>(old)->destroy();

                delete std::get<2>(old);
            }
        }

        ~marked_unicity_table() { flush(); }

        size_t size() const override { return set.size(); }

        void clear() {
            flush();
            set.clear();
        }
};


#endif
