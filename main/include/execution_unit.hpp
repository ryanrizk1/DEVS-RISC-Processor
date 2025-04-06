#ifndef EXECUTION_UNIT_HPP
#define EXECUTION_UNIT_HPP

#include <string>
#include <iostream>
#include "cadmium/modeling/devs/atomic.hpp"

using namespace cadmium;

struct executionUnit_State {
    double sigma;
    std::string dataMemAddr;
    std::string opcode;
    std::string opnda;
    std::string opndb;
    std::string dest;
    std::string result;
    explicit executionUnit_State(): sigma(1){
    }
};

#ifndef NO_LOGGING
std::ostream& operator<<(std::ostream &out, const executionUnit_State& state) {
    out  << "{" << state.sigma << "}";
    return out;
}
#endif

class executionUnit : public Atomic<executionUnit_State> {
    public:

    Port<int> clk;
    Port<int> rst;

    // From Decode Unit
    Port<std::string> dataMemAddr;
    Port<std::string> opcode;
    Port<std::string> opndaAddr;
    Port<std::string> opndbAddr;
    Port<std::string> destAddr;

    // From Register Unit
    Port<std::string> opnda;
    Port<std::string> opndb;

    // To Register Unit
    Port<std::string> opndaReg;
    Port<std::string> opndbReg;
    Port<std::string> destReg;

    // To Data Memory
    Port<std::string> dataMemAddress;


    executionUnit(const std::string id) : Atomic<executionUnit_State>(id, executionUnit_State()) {
        clk = addInPort<int>("clk");
        rst = addInPort<int>("rst");

        dataMemAddr = addInPort<std::string>("dataMemAddr");
        opcode = addInPort<std::string>("opcode");
        opndaAddr = addInPort<std::string>("opndaAddr");
        opndbAddr = addInPort<std::string>("opndbAddr");
        destAddr = addInPort<std::string>("destAddr");

        opnda = addInPort<std::string>("opnda");
        opndb = addInPort<std::string>("opndb");

        opndaReg = addOutPort<std::string>("opndaReg");
        opndbReg = addOutPort<std::string>("opndbReg");
        destReg = addOutPort<std::string>("destReg");

        dataMemAddress = addOutPort<std::string>("dataMemAddress");
    }

    void internalTransition(executionUnit_State& state) const override {
        state.sigma += 1;
    }

    void externalTransition(executionUnit_State& state, double e) const override {
        if(!clk->empty()){
            if(!opcode->empty()){
                state.opcode = opcode->getBag().back();
            } 
            if(!dataMemAddr->empty()){
                state.dataMemAddr = dataMemAddr->getBag().back();
            }
            if(!opnda->empty()){
                state.opnda = opnda->getBag().back();
            }
            if(!opndb->empty()){
                state.opndb = opndb->getBag().back();
            }
            if(!dest->empty()){
                state.dest = dest->getBag().back();
            }
            if(!rst->empty()){
                state.opcode = "0000";
                state.dataMemAddr = "0000";
                state.opnda = "000";
                state.opndb = "000";
                state.dest = "000";
            }

            if(state.opcode == "1110"){

            } else if(state.opcode == "1111"){

            } else {
                
            }
            state.sigma = state.sigma - e;
        }
    }
    
    
    // output function
    void output(const executionUnit_State& state) const override {
        //your output function goes here
    }

    // time_advance function
    [[nodiscard]] double timeAdvance(const executionUnit_State& state) const override {     
            return state.sigma;
    }
};

#endif