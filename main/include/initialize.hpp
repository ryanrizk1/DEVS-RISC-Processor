#ifndef INITIALIZE_HPP
#define INITIALIZE_HPP

#include <string>
#include <iostream>
#include "cadmium/modeling/devs/atomic.hpp"

using namespace cadmium;

struct initialize_State {
    double sigma;
    bool init;
    explicit initialize_State(): sigma(0.05), init(false){
    }
};

#ifndef NO_LOGGING
std::ostream& operator<<(std::ostream &out, const initialize_State& state) {
    out  << "{" << state.sigma << "}";
    return out;
}
#endif

class initialize : public Atomic<initialize_State> {
    public:

    Port<int> pc_init;

    initialize(const std::string id) : Atomic<initialize_State>(id, initialize_State()) {
        pc_init = addOutPort<int>("pc_init");
    }

    void internalTransition(initialize_State& state) const override {
        if(state.init == false){
            state.init = true;
            state.sigma = 0.05;
        } else {
            state.sigma = std::numeric_limits<double>::infinity();
        }
    }

    void externalTransition(initialize_State& state, double e) const override {
    }
    
    void output(const initialize_State& state) const override {
        pc_init->addMessage(0);
    }

    [[nodiscard]] double timeAdvance(const initialize_State& state) const override {     
        return state.sigma;
    }
};

#endif