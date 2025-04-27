#ifndef DECODE_UNIT_HPP
#define DECODE_UNIT_HPP

#include <string>
#include <iostream>
#include "cadmium/modeling/devs/atomic.hpp"

using namespace cadmium;

struct decodeUnit_State {
    double sigma;
    std::string instruction;
    std::string dataMemAddrLd;
    std::string dataMemAddrSt;
    std::string opcode;
    std::string opnda;
    std::string opndb;
    std::string dest;
    std::string ldReg;
    std::string stReg;
    explicit decodeUnit_State(): sigma(std::numeric_limits<double>::infinity()), instruction("000000000000000000"), dataMemAddrLd("0000"), opcode("0000"), opnda("000"), opndb("000"), dest("000"){
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
    Port<std::string> dataMemAddrLd;
    Port<std::string> dataMemAddrSt;
    Port<std::string> opcode;
    Port<std::string> opnda;
    Port<std::string> opndb;
    Port<std::string> dest;
    Port<std::string> ldReg;
    Port<std::string> stReg;

    decodeUnit(const std::string id) : Atomic<decodeUnit_State>(id, decodeUnit_State()) {
        clk = addInPort<int>("clk");
        rst = addInPort<int>("rst");
        instruction = addInPort<std::string>("instruction");
        dataMemAddrLd = addOutPort<std::string>("dataMemAddrLd");
        dataMemAddrSt = addOutPort<std::string>("dataMemAddrSt");
        opcode = addOutPort<std::string>("opcode");
        opnda = addOutPort<std::string>("opnda");
        opndb = addOutPort<std::string>("opndb");
        dest = addOutPort<std::string>("dest");
        ldReg = addOutPort<std::string>("ldReg");
        stReg = addOutPort<std::string>("stReg");
    }

    void internalTransition(decodeUnit_State& state) const override {
        state.sigma = std::numeric_limits<double>::infinity();
    }

    void externalTransition(decodeUnit_State& state, double e) const override {
        //if(!clk->empty()){
            //std::cout << "[d_unit]: clk" << std::endl;
            // if(!rst->empty()){
            //     state.opcode = "0000";
            //     state.dataMemAddr = "0000";
            //     state.opnda = "000";
            //     state.opndb = "000";
            //     state.dest = "000";
            // } else 
            if(!instruction->empty()){ 
                state.instruction = instruction->getBag().back();
                std::cout << "[d_unit]: instruction received from i_unit: " << state.instruction << std::endl;
                state.opcode = state.instruction.substr(0, 4); 
                std::cout << "[d_unit]: opcode is " << state.opcode << std::endl;
                if(state.opcode == "1110"){ // ld instruction
                    std::cout << "[d_unit]: ld instruction" << std::endl;
                    state.dataMemAddrLd = state.instruction.substr(5, 4);
                    state.dataMemAddrSt = "";
                    std::cout << "[d_unit]: dataMemAddr is " << state.dataMemAddrLd << std::endl;
                    state.ldReg = state.instruction.substr(10, 3);
                    state.stReg = "";
                    std::cout << "[d_unit]: ldReg is " << state.ldReg << std::endl;
                    state.opnda = "";
                    state.opndb = "";
                    state.dest = "";
                } else if(state.opcode == "1111"){ // st instruction
                    std::cout << "[d_unit]: st instruction" << std::endl;
                    state.dataMemAddrSt = state.instruction.substr(9, 4);
                    state.dataMemAddrLd = "";
                    state.stReg = state.instruction.substr(6, 3);
                    state.opnda = "";
                    state.opndb = "";
                    state.ldReg = "";
                    state.dest = "";
                } else {
                    state.opnda = state.instruction.substr(4, 3);
                    state.opndb = state.instruction.substr(7, 3);
                    state.dest = state.instruction.substr(10, 3);
                    state.dataMemAddrLd = "";
                    state.dataMemAddrSt = "";
                    state.ldReg = "";
                    state.stReg = "";
                }
            }
            state.sigma = 0.0;
        //}
    }
    
    void output(const decodeUnit_State& state) const override {
        if(!state.opcode.empty()){
            opcode->addMessage(state.opcode); // to execution unit
        } if(!state.opnda.empty()){
            opnda->addMessage(state.opnda); // to reg unit
        } if(!state.opndb.empty()){
            opndb->addMessage(state.opndb); // to reg unit
        } if(!state.dest.empty()){
            dest->addMessage(state.dest); // to execution unit
        } if(!state.dataMemAddrLd.empty()){
            dataMemAddrLd->addMessage(state.dataMemAddrLd); // to data mem unit
        } if(!state.ldReg.empty()){
            ldReg->addMessage(state.ldReg); // to data mem
        } if(!state.dataMemAddrSt.empty()){
            dataMemAddrSt->addMessage(state.dataMemAddrSt); // to data mem unit
        } if(!state.stReg.empty()){
            stReg->addMessage(state.stReg); // to reg unit
        }
    }

    [[nodiscard]] double timeAdvance(const decodeUnit_State& state) const override {     
        return state.sigma;
    }
};

#endif