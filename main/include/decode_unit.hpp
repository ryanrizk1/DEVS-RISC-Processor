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
    int reg_wr_vld = 0;
    explicit decodeUnit_State(): sigma(std::numeric_limits<double>::infinity()), instruction("000000000000000000"), dataMemAddr("0000"), opcode("0000"), opnda("000"), opndb("000"), dest("000"){
    }
};

#ifndef NO_LOGGING
std::ostream& operator<<(std::ostream &out, const decodeUnit_State& state) {
    out  << "{sigma: " << state.sigma << ", instruction: " << state.instruction << " reg_write = " << state.reg_wr_vld<< "}";
    return out;
}
#endif

class decodeUnit : public Atomic<decodeUnit_State> {
    public:

    Port<int> clk;
    Port<int> rst;
    Port<std::string> instruction;
    Port<std::string> dataMemAddr; //used for offset in load opr
    Port<std::string> opcode; //opr to be performed
    Port<std::string> opnda; //source register a
    Port<std::string> opndb; //source register b
    Port<std::string> dest; //dest reg to store result from alu opr or from load opr (write-back)
    Port<int> reg_wr_vld; //enable when there is a write back operation to ther egisters


    decodeUnit(const std::string id) : Atomic<decodeUnit_State>(id, decodeUnit_State()) {
        clk = addInPort<int>("clk");
        rst = addInPort<int>("rst");
        instruction = addInPort<std::string>("instruction");
        dataMemAddr = addOutPort<std::string>("dataMemAddr");
        opcode = addOutPort<std::string>("opcode");
        opnda = addOutPort<std::string>("opnda");
        opndb = addOutPort<std::string>("opndb");
        dest = addOutPort<std::string>("dest");
        reg_wr_vld = addOutPort<int>("reg_wr_vld");

    }

    void internalTransition(decodeUnit_State& state) const override {
        state.sigma = std::numeric_limits<double>::infinity();
    }

    void externalTransition(decodeUnit_State& state, double e) const override {
        if(!clk->empty()){
            //std::cout << "[d_unit]: clk" << std::endl;
            if(!rst->empty() && rst->getBag().back() == 1) {
                
                state.opcode      = "0000";
                state.dataMemAddr = "0000";
                state.opnda       = "000";
                state.opndb       = "000";
                state.dest        = "000";
                state.reg_wr_vld = 0;
                state.sigma       = std::numeric_limits<double>::infinity();

            
            } else if(!instruction->empty()){ ////////////////////////// FIX SUBSTR!!
                state.instruction = instruction->getBag().back();
                std::cout << "[d_unit]: instruction received from i_unit: " << state.instruction << std::endl;
                state.opcode = state.instruction.substr(0, 4); 
                std::cout << "[d_unit]: opcode is " << state.opcode << std::endl;
                if(state.opcode == "1110"){ // ld instruction

                    std::cout << "[d_unit]: ld instruction" << std::endl;
                    state.dataMemAddr = state.instruction.substr(5, 4);
                    state.dest = state.instruction.substr(10, 3);
                    state.opnda = "000";
                    state.opndb = "000";
                } else if(state.opcode == "1111"){ // st instruction
                    std::cout << "[d_unit]: st instruction" << std::endl;
                    state.dataMemAddr = state.instruction.substr(9, 4);
                    state.dest = state.instruction.substr(6, 3);
                    state.opnda = "000";
                    state.opndb = "000";
                } else {
                    std::cout << "[d_unit]: ALU instruction" << std::endl;
                    state.dataMemAddr = "0000";
                    state.opnda = state.instruction.substr(4, 3);
                    state.opndb = state.instruction.substr(7, 3);
                    state.dest = state.instruction.substr(10, 3);
                }
                bool isALUorLoad = (state.opcode != "0000" )
                && (state.opcode != "1111" );//not NOP or store
                state.reg_wr_vld = isALUorLoad ? 1 : 0;
                std::cout << "reg_write = " << state.reg_wr_vld << std::endl;

                state.sigma = 0.0;
            }
            
        }
    }
    
    void output(const decodeUnit_State& state) const override {
        opcode->addMessage(state.opcode);
        dataMemAddr->addMessage(state.dataMemAddr);
        opnda->addMessage(state.opnda);
        opndb->addMessage(state.opndb);
        dest->addMessage(state.dest);
        
        reg_wr_vld->addMessage(state.reg_wr_vld);
        

        std::cout 
          << "[d_unit]: out → opcode=" << state.opcode
          << ", opnda=" << state.opnda
          << ", opndb=" << state.opndb
          << ", dest=" << state.dest
          << ", addr=" << state.dataMemAddr
          << std::endl;
    }

    [[nodiscard]] double timeAdvance(const decodeUnit_State& state) const override {     
        return state.sigma;
    }
};

#endif