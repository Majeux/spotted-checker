#ifndef CROSS_UNICITY_H
#define CROSS_UNICITY_H

#include <spot/twa/twa.hh>

#include <set>
#include <map>

#include "aliases.h"

struct state_pair {
    const spot::state* a_;
    const spot::state* b_;

    state_pair(const spot::state* a, const spot::state* b) : a_(a), b_(b) {}
};

struct state_tuple {
    const spot::state* a_;
    const spot::state* b_;
    mutable bool* mark_;

    state_tuple(const spot::state* a, const spot::state* b, bool* m) : a_(a), b_(b), mark_(m) {}

    void set_mark(bool m) const { *mark_ = m; }
};

/*  Hash functor for hashing a pair of state pointers.
Based on the boost hash_combine function, adapted for two hashes. */
struct state_pair_hash {
    size_t operator()(state_pair s) const {
        assert(s.a_ != nullptr); assert(s.b_ != nullptr);

        size_t h = s.a_->hash() + 0x9e3779b9 + (0<<6) + (0>>2);
        return h ^(s.b_->hash() + 0x9e3779b9 + (h<<6) + (h>>2));
    }
};

struct state_pair_equal {
    bool operator()(state_pair l, state_pair r) const {
        assert(l.a_ != nullptr); assert(l.b_ != nullptr);
        assert(r.a_ != nullptr); assert(r.b_ != nullptr);

        return l.a_->compare(r.a_) == 0 && l.b_->compare(r.b_) == 0;
    }
};

/*  Hash functor for hashing a tuple of state pointers.
Based on the boost hash_combine function, adapted for two hashes. */
// struct state_tuple_hash {
//     size_t operator()(state_tuple s) const {
//         assert(s.a_ != nullptr); assert(s.b_ != nullptr);
//
//         size_t h = s.a_->hash() + 0x9e3779b9 + (0<<6) + (0>>2);
//         return h ^(s.b_->hash() + 0x9e3779b9 + (h<<6) + (h>>2));
//     }
// };
//
// struct state_tuple_equal {
//     bool operator()(state_tuple l, state_tuple r) const {
//         assert(l.a_ != nullptr); assert(l.b_ != nullptr);
//         assert(r.a_ != nullptr); assert(r.b_ != nullptr);
//         //NOTE the boolean is not an identifier
//         return l.a_->compare(r.a_) == 0 && l.b_->compare(r.b_) == 0;
//     }
// };


class unicity_table_base {
    protected:
        unicity_table_base* other = nullptr;
    public:
        void set_co_table(unicity_table_base* t) { other = t; }
        virtual bool contains(const spot::state* s1, const spot::state* s2) const = 0;
        virtual bool contains(state_pair s) const = 0;
        virtual const state_pair operator()(const spot::state* s1,
                                            const spot::state* s2) = 0;
        virtual const state_pair operator()(state_pair s) = 0;
        virtual const state_pair is_new(const spot::state* s1,
                                        const spot::state* s2) = 0;
        virtual const state_pair is_new(state_pair s) = 0;
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

        const state_pair operator()(state_pair s) override  {
            return operator()(s.a_, s.b_);
        }

        const state_pair is_new(const spot::state* s1, const spot::state* s2) override {
            auto p = set.emplace(s1, s2);
            if(!p.second) {
                s1->destroy();
                s2->destroy();
                return {nullptr,nullptr};
            }
            return *p.first;
        }

        const state_pair is_new(state_pair s) override {
            return is_new(s.a_, s.b_);
        }

        bool contains(const spot::state* s1, const spot::state* s2) const override {
            return contains({s1, s2});
        }

        bool contains(state_pair s) const override {
            return set.find(s) != set.end();
        }

        void flush() {
            for(cross_state_set::iterator i = set.begin(); i != set.end();) {
                cross_state_set::iterator old = i++;

                if(other == nullptr || !other->contains(*old)) {
                    if(old->a_)
                        old->a_->destroy();

                    if(old->b_)
                        old->b_->destroy();
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
    using marked_state_set = std::unordered_map< state_pair, bool,
                                                state_pair_hash,
                                                state_pair_equal    >;
    private:
        marked_state_set set;

    public:
        const state_pair operator()(const spot::state* s1, const spot::state* s2) override {
            return operator()({s1, s2});
        }

        const state_pair operator()(state_pair s) override {
            //mark as true when pushed = when registered
            auto p = set.emplace(s, true);
            if(!p.second) {
                s.a_->destroy();
                s.b_->destroy();
            }

            return {p.first->first.a_, p.first->first.b_};
        }

        /*  Set the mark of a state_pair (usually to false)
            Returns null,null is the pair does not exist */
        state_pair mark(const spot::state* s1, const spot::state* s2, bool m) {
            return mark({s1, s2}, m);
        }

        const state_pair mark(state_pair s, bool m) {
            marked_state_set::iterator p = set.find(s);

            if(p == set.end())
                return {nullptr, nullptr};

            p->second = m;

            //TODO rm
            // std::cerr << "mark set: " << p->second << std::endl;

            return {p->first.a_, p->first.b_};
        }

        bool get_mark(const spot::state* s1, const spot::state* s2) const {
            return get_mark({s1, s2});
        }

        bool get_mark(state_pair s) const {
            auto p = set.find(s); //bool doesnt matter

            if(p == set.end())
                return false; //not yet seen states are always unmarked

            return p->second;
        }

        const state_pair is_new(const spot::state* s1, const spot::state* s2) override {
            return is_new({s1, s2});
        }

        const state_pair is_new(state_pair s) override {
            auto p = set.emplace(s, true);

            if(!p.second) { //not new
                s.a_->destroy();
                s.b_->destroy();
                return {nullptr,nullptr};
            }

            return {p.first->first.a_, p.first->first.b_};
        }

        bool contains(const spot::state* s1, const spot::state* s2) const override {
            return contains({s1, s2});
        }

        bool contains(state_pair s) const override {
            return set.find(s) != set.end();
        }

        void flush() {
            for(marked_state_set::iterator i = set.begin(); i != set.end();) {
                marked_state_set::iterator old = i++;
                if(old->first.a_)
                    old->first.a_->destroy();
                if(old->first.b_)
                    old->first.b_->destroy();
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
