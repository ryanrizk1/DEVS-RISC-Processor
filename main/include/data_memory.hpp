#ifndef ATOMIC_MODEL_HPP
#define ATOMIC_MODEL_HPP

#include <random>
#include <iostream>
#include "cadmium/modeling/devs/atomic.hpp"

using namespace cadmium;

struct dataMemory_State {
    double sigma;
    std::string stAddr;
    std::string ldAddr;
    std::string dataIn;
    std::string dataOut;
    std::array<std::string, 16> data = {};
    explicit dataMemory_State(): sigma(1){
    }
};

#ifndef NO_LOGGING
std::ostream& operator<<(std::ostream &out, const dataMemory_State& state) {
    out  << "{" << state.sigma << "}";
    return out;
}
#endif

class dataMemory : public Atomic<dataMemory_State> {
    public:

    Port<int> clk;
    Port<int> rst;
    Port<std::string> dataMemAddrIn; // store data addr
    Port<std::string> dataMemAddrOut; // load data addr
    Port<std::string> dataIn;
    Port<std::string> dataOut;

    dataMemory(const std::string id) : Atomic<dataMemory_State>(id, dataMemory_State()) {
        clk = addInPort<int>("clk");
        rst = addInPort<int>("rst");
        dataMemAddrIn = addInPort<std::string>("dataMemAddrIn");
        dataMemAddrOut = addInPort<std::string>("dataMemAddrOut");
        dataIn = addInPort<std::string>("dataIn");
        dataOut = addOutPort<std::string>("dataOut");
    }

    void internalTransition(dataMemory_State& state) const override {
        state.sigma = std::numeric_limits<double>::infinity();
    }

    void externalTransition(dataMemory_State& state, double e) const override {
        if(!clk->empty()){
            if(!dataMemAddrIn->empty()){
                state.stAddr = dataMemAddrIn->getBag().back();
                if(!dataIn->empty()){
                    state.data[state.stAddr] = dataIn->getBag().back(); //str to int for index
                } 
            }
            else if(!dataMemAddrOut->empty()){
                state.ldAddr = dataMemAddrOut->getBag().back();
                state.dataOut = state.data[state.ldAddr];
            }
        }
    }
    
    
    void output(const dataMemory_State& state) const override {
        dataOut->addMessage(state.dataOut);
    }

    [[nodiscard]] double timeAdvance(const dataMemory_State& state) const override {     
        return state.sigma;
    }
};

#endif