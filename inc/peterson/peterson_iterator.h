#ifndef PETERSON_IT_H
#define PETERSON_IT_H

#include <peterson/peterson_state.h>

class PetersonIterator: public spot::kripke_succ_iterator {
    private:
        const size_t N_; //number of processes
        const size_t i_; //iterator through processes
        unsigned e_;     //iterator through outgoing edges of state_

        std::vector<unsigned> pc; //track what state/line of peterson we are in
        std::vector<int> level; //TODO write as c array with lexographic <=>
        std::vector<int> last_to_enter;

    public:
        demo_succ_iterator(unsigned char x, unsigned char y, bdd cond)
        : kripke_succ_iterator(cond), x_(x), y_(y)
        {
        }

        bool first_e() {
            switch (pc[i_]) {
                case 0: case 2: case 4:
                    e_ = 1; break;
                case 1: case 3: //loop bound and wait are branches
                    e_ = 2; break;
                default:
                    return false;
            }

            return true; // Note that every legal state has a successor
        }

        // Set iterator, return true iff there is at least 1 successor
        bool first() override {
            i_ = 0;
            return first_e();
        }

        bool next() override {
            if(e_ > 0)
                return --e_ > 0 || i_ < N_ - 1; //process i_ has edges, or there is another process

            if(++i_ < N) //switch to next process
                return first_e();

            return false; //went through all processes
        }

        bool done() const override {
            return i_ == N_;
        }

        demo_state* dst() const override {
            assert(i_ < N_); assert(e_ > 0);

            assignment assign_pc, assign_lvl, assign_lte;

            switch (pc[i_]) {
                case 0: //initialize loop
                    assign_lvl = {i_, 0};
                    assign_pc  = {i_, 1};
                    return PetersonState(N_, pc, assign_pc,
                                             level, assign_lvl,
                                             last_to_enter);
                case 1: //check loop bound
                    if(level[i_] < N - 1)
                        assign_pc = {i_, 2};
                    else
                        assign_pc = {i_, 4};

                    return PetersonState(N_, pc, assign_pc,
                                             level,
                                             last_to_enter);
                case 2: //loop body
                    assign_lte = {level[i_], i_};
                    assign_pc = {i_, 3};

                    return PetersonState(N_, pc, assign_pc,
                                             level,
                                             last_to_enter, assign_lte);
                case 3: //loop wait
                    if(last_to_enter[level[i_]] == i_ && find_bigger_level())
                        return PetersonState(N_, pc, level, last_to_enter);
                    else {
                        assign_lvl = {i_, level[i_] + 1};
                        assign_pc  = {i_, 1};
                        return PetersonState(N_, pc, assign_pc,
                                                 level, assign_lvl,
                                                 last_to_enter);
                    }
                case 4: //critical section
                    assign_lte = {level[i_], -1};
                    assign_pc = {i_, 0};

                    return PetersonState(N_, pc, assign_pc,
                                             level,
                                             last_to_enter, assign_lte);
            }
            return NULL;
        }

        void recycle(unsigned char x, unsigned char y, bdd cond)
        {
            spot::kripke_succ_iterator::recycle(cond);
        }
};
