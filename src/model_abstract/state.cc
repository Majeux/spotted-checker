#include "model_abstract/state.h"

#define FNV_OFFSET 0xcbf29ce484222325
#define FNV_PRIME  0x100000001b3

//fnv-1a hash function, iterating over all state identifiers
size_t AbstractState::hash() const {
    assert(sizeof(size_t)*8 == 64);

    size_t h = FNV_OFFSET;
    //we only XOR and multiply, can treat signed char as unsigned
    auto fnv1a = [&h] (proc_t i) { h ^= i; h *= FNV_PRIME; };

    std::for_each(sv.variables_.begin(), sv.variables_.end(), fnv1a);
    //indices are part of the identity of 'arrays', no need to consider
    for(const std::vector<proc_t> &a : sv.arrays_)
        std::for_each(a.begin(), a.end(), fnv1a);

    return h;
}

// compare must impose a total order
// comparators for std::array and std::vector impose a lexographical order,
// which is a total order if their elements (int) have a total order
int AbstractState::compare(const spot::state* other) const {
    auto o = static_cast<const AbstractState*>(other);

    if(sv.variables_ < o->sv.variables_) return -1; //first <
    if(sv.variables_ > o->sv.variables_) return  1; //first > --> first ==
    if(sv.arrays_    < o->sv.arrays_)  return -1; //first ==, second <
    //indices are part of the identity of 'arrays', no need to consider

    return sv.arrays_ > o->sv.arrays_;        //first ==, second > or ==

 }
