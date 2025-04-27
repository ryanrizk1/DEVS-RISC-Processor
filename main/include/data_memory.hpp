#ifndef DATA_MEMORY_HPP
#define DATA_MEMORY_HPP

#include <random>
#include <iostream>
#include "cadmium/modeling/devs/atomic.hpp"

using namespace cadmium;

struct dataMemory_State {
    double sigma;
    std::string stAddr;
    std::string ldAddr;
    std::string ldReg;
    std::string stReg;
    std::string dataIn;
    std::string dataOut;
    std::array<std::string, 16> data = {"00000000", "00100010", "01000100", "10001000", "10101010", "11001100", "11111111", "00000000", "00000000", "00000000", "00000000", "00000000", "00000000", "00000000", "00000000"};
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
    Port<std::string> ldRegIn;
    Port<std::string> ldRegOut;
    Port<std::string> stReg;
    Port<std::string> dataIn;
    Port<std::string> dataOut;

    dataMemory(const std::string id) : Atomic<dataMemory_State>(id, dataMemory_State()) {
        clk = addInPort<int>("clk");
        rst = addInPort<int>("rst");
        dataMemAddrIn = addInPort<std::string>("dataMemAddrIn");
        dataMemAddrOut = addInPort<std::string>("dataMemAddrOut");
        ldRegIn = addInPort<std::string>("ldRegIn");
        ldRegOut = addOutPort<std::string>("ldRegOut");
        stReg = addInPort<std::string>("stReg");
        dataIn = addInPort<std::string>("dataIn");
        dataOut = addOutPort<std::string>("dataOut");
    }

    void internalTransition(dataMemory_State& state) const override {
        state.sigma = std::numeric_limits<double>::infinity();
    }

    void externalTransition(dataMemory_State& state, double e) const override {
        //if(!clk->empty()){
            if(!dataMemAddrIn->empty()){
                state.stAddr = dataMemAddrIn->getBag().back();
                if(!dataIn->empty()){
                    state.data[std::stoi(state.stAddr, nullptr, 2)] = dataIn->getBag().back(); 
                    std::cout << "[d_mem]: data stored: " << state.data[std::stoi(state.stAddr, nullptr, 2)] << std::endl;
                    state.sigma = 0.0;
                } 
            }
            if(!dataMemAddrOut->empty()){
                state.ldAddr = dataMemAddrOut->getBag().back();
                state.dataOut = state.data[std::stoi(state.ldAddr, nullptr, 2)];
                std::cout << "[d_mem]: data loaded to registers: " << state.dataOut << std::endl;
                if(!ldRegIn->empty()){
                    state.ldReg = ldRegIn->getBag().back();
                }
                state.stReg = "";
                state.sigma = 0.0;
            }
        //}
    }
    
    
    void output(const dataMemory_State& state) const override {
        if(!state.dataOut.empty()){
            dataOut->addMessage(state.dataOut);
        } if(!state.ldReg.empty()){
            ldRegOut->addMessage(state.ldReg);
        }
    }

    [[nodiscard]] double timeAdvance(const dataMemory_State& state) const override {     
        return state.sigma;
    }
};

#endif