#include "peterson/state.h"

#define FNV_OFFSET 0xcbf29ce484222325
#define FNV_PRIME  0x100000001b3

//fnv-1a hash function, iterating over all state identifiers
size_t PetersonState::hash() const {
    assert(sizeof(size_t)*8 == 64);

    size_t h = FNV_OFFSET;
    //we only XOR and multiply, can treat signed char as unsigned
    auto fnv1a = [&h] (proc i) { h ^= i; h *= FNV_PRIME; };

    std::for_each(pc.begin(), pc.end(), fnv1a);
    std::for_each(level.begin(), level.end(), fnv1a);
    std::for_each(last_to_enter.begin(), last_to_enter.end(), fnv1a);

    return h;
}

int PetersonState::compare(const spot::state* other) const {
    auto o = static_cast<const PetersonState*>(other);
    assert(o != NULL);

    if(pc < o->pc) return -1;
    if(pc > o->pc) return  1;
    //first >< --> first ==
    if(level < o->level)  return -1;
    if(level > o->level)  return  1;
    //second >< --> second ==
    if(last_to_enter < o->last_to_enter)  return -1;

    return last_to_enter > o->last_to_enter; //first ==, second ==
 }

 bool* PetersonState::getInCrit(proc N) const {
     assert(N > 0);
     assert((size_t)N == level.size());
     bool* in_crit = new bool[N];

     for(size_t i = 0; i < (size_t)N; i++)
        in_crit[i] = pc[i] == 4;

    return in_crit;
 }
