#ifndef CLOCK_HPP
#define CLOCK_HPP

#include <random>
#include <iostream>
#include "cadmium/modeling/devs/atomic.hpp"

using namespace cadmium;

struct clock_State {
    double sigma;
    int state;
    explicit clock_State(): sigma(0.05), state(0){
    }
};

#ifndef NO_LOGGING
std::ostream& operator<<(std::ostream &out, const clock_State& state) {
    out  << "{" << state.sigma << "}";
    return out;
}
#endif

class clockUnit : public Atomic<clock_State> {
    public:

    Port<int> clk;

    clockUnit(const std::string id) : Atomic<clock_State>(id, clock_State()) {
        clk = addOutPort<int>("clk");
    }

    void internalTransition(clock_State& state) const override {
        state.sigma = 0.05;
    }

    void externalTransition(clock_State& state, double e) const override {
    }
    
    void output(const clock_State& state) const override {
        clk->addMessage(1);
    }

    [[nodiscard]] double timeAdvance(const clock_State& state) const override {     
        return state.sigma;
    }
};

#endif