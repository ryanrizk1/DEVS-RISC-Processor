#ifndef DECODE_UNIT_HPP
#define DECODE_UNIT_HPP

#include <string>
#include <iostream>
#include "cadmium/modeling/devs/atomic.hpp"

using namespace cadmium;

struct decodeUnit_State {
    double sigma;
    std::string instruction;
    std::string dataMemAddr;
    std::string opcode;
    std::string opnda;
    std::string opndb;
    std::string dest;
    explicit decodeUnit_State(): sigma(std::numeric_limits<double>::infinity()), instruction("000000000000000000"), dataMemAddr("0000"), opcode("0000"), opnda("000"), opndb("000"), dest("000"){
    }
};

#ifndef NO_LOGGING
std::ostream& operator<<(std::ostream &out, const decodeUnit_State& state) {
    out  << "{sigma: " << state.sigma << ", instruction: " << state.instruction << "}";
    return out;
}
#endif

class decodeUnit : public Atomic<decodeUnit_State> {
    public:

    Port<int> clk;
    Port<int> rst;
    Port<std::string> instruction;
    Port<std::string> dataMemAddr;
    Port<std::string> opcode;
    Port<std::string> opnda;
    Port<std::string> opndb;
    Port<std::string> dest;

    decodeUnit(const std::string id) : Atomic<decodeUnit_State>(id, decodeUnit_State()) {
        clk = addInPort<int>("clk");
        rst = addInPort<int>("rst");
        instruction = addInPort<std::string>("instruction");
        dataMemAddr = addOutPort<std::string>("dataMemAddr");
        opcode = addOutPort<std::string>("opcode");
        opnda = addOutPort<std::string>("opnda");
        opndb = addOutPort<std::string>("opndb");
        dest = addOutPort<std::string>("dest");
    }

    void internalTransition(decodeUnit_State& state) const override {
        state.sigma = std::numeric_limits<double>::infinity();
    }

    void externalTransition(decodeUnit_State& state, double e) const override {
        if(!clk->empty()){
            //std::cout << "[d_unit]: clk" << std::endl;
            if(!rst->empty()){
                state.opcode = "0000";
                state.dataMemAddr = "0000";
                state.opnda = "000";
                state.opndb = "000";
                state.dest = "000";
            } else if(!instruction->empty()){ ////////////////////////// FIX SUBSTR!!
                state.instruction = instruction->getBag().back();
                std::cout << "[d_unit]: instruction received from i_unit: " << state.instruction << std::endl;
                state.opcode = state.instruction.substr(0, 4); 
                std::cout << "[d_unit]: opcode is " << state.opcode << std::endl;
                if(state.opcode == "1110"){ // ld instruction
                    std::cout << "[d_unit]: ld instruction" << std::endl;
                    state.dataMemAddr = state.instruction.substr(4, 4);
                    state.dest = state.instruction.substr(0, 3);
                    state.opnda = "000";
                    state.opndb = "000";
                } else if(state.opcode == "1111"){ // st instruction
                    std::cout << "[d_unit]: st instruction" << std::endl;
                    state.dataMemAddr = state.instruction.substr(0, 4);
                    state.dest = state.instruction.substr(4, 3);
                    state.opnda = "000";
                    state.opndb = "000";
                } else {
                    state.dataMemAddr = "0000";
                    state.opnda = state.instruction.substr(6, 3);
                    state.opndb = state.instruction.substr(3, 3);
                    state.dest = state.instruction.substr(0, 3);
                }
            }
            state.sigma = state.sigma - e;
        }
    }
    
    void output(const decodeUnit_State& state) const override {
        opcode->addMessage(state.opcode);
        dataMemAddr->addMessage(state.dataMemAddr);
        opnda->addMessage(state.opnda);
        opndb->addMessage(state.opndb);
        dest->addMessage(state.dest);
    }

    [[nodiscard]] double timeAdvance(const decodeUnit_State& state) const override {     
        return state.sigma;
    }
};

#endif