#ifndef DATA_MEM_HPP
#define DATA_MEM_HPP

#include <random>
#include <array>
#include <string>
#include <limits>
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

    dataMemory_State()
      : sigma(std::numeric_limits<double>::infinity()),
        stAddr(""), ldAddr(""), dataIn(""), dataOut(""),
        data{}
    {
        // initialize all 16 words to zero
        data.fill(std::string(13,'0'));
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

        void externalTransition(dataMemory_State& s, double /*e*/) const override {

        if (!clk->empty() && clk->getBag().back() == 1) {

            if (!rst->empty() && rst->getBag().back() == 1) {
                s.data.fill(std::string(13,'0'));
            } else {
                // STORE
                if (!dataMemAddrIn->empty()) {
                    s.stAddr = dataMemAddrIn->getBag().back();
                    if (!dataIn->empty()) {
                        int idx = std::stoi(s.stAddr, nullptr, 2);
                        if (idx >= 0 && idx < 16) {
                            s.data[idx] = dataIn->getBag().back();
                        }
                    }
                }
                // LOAD
                else if (!dataMemAddrOut->empty()) {
                    s.ldAddr = dataMemAddrOut->getBag().back();
                    int idx = std::stoi(s.ldAddr, nullptr, 2);
                    if (idx >= 0 && idx < 16) {
                        s.dataOut = s.data[idx];
                    }
                }
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