#ifndef REGISTER_UNIT_HPP
#define REGISTER_UNIT_HPP

#include <string>
#include <iostream>
#include "cadmium/modeling/devs/atomic.hpp"

using namespace cadmium;

struct registerUnit_State {
    double sigma;
    //you can have as many state variables as you want/ need

    explicit registerUnit_State(): sigma(1){
    }
};

#ifndef NO_LOGGING
std::ostream& operator<<(std::ostream &out, const registerUnit_State& state) {
    out  << "{" << state.sigma << "}";
    return out;
}
#endif

class registerUnit : public Atomic<registerUnit_State> {
    public:

    Port<int> clk;
    Port<int> rst;
    Port<std::string> result;
    Port<std::string>

    registerUnit(const std::string id) : Atomic<registerUnit_State>(id, registerUnit_State()) {
        //Constructor of your atomic model. Initialize ports here.
    }

    // inernal transition
    void internalTransition(registerUnit_State& state) const override {
        //your internal transition function goes here
        state.sigma += 1;
    }

    // external transition
    void externalTransition(registerUnit_State& state, double e) const override {
        //your external transition function hoes here
    }
    
    
    // output function
    void output(const registerUnit_State& state) const override {
        //your output function goes here
    }

    // time_advance function
    [[nodiscard]] double timeAdvance(const registerUnit_State& state) const override {     
            return state.sigma;
    }
};

#endif