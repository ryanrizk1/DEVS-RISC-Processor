#ifndef INSTRUCTION_UNIT_HPP
#define INSTRUCTION_UNIT_HPP

#include <string>
#include <iostream>
#include "cadmium/modeling/devs/atomic.hpp"

using namespace cadmium;

struct instructionUnit_State {
    double sigma;
    int pc;
    std::string ir;
    explicit instructionUnit_State(): sigma(0.1), pc(0), ir("0000000000000"){
    }
};

#ifndef NO_LOGGING
std::ostream& operator<<(std::ostream &out, const instructionUnit_State& state) {
    out  << "{sigma: " << state.sigma << ", pc: " << state.pc << ", ir: " << state.ir << "}";
    return out;
}
#endif

class instructionUnit : public Atomic<instructionUnit_State> {
    public:
    
    Port<int> clk;
    Port<int> rst;
    Port<std::string> instruction;
    Port<int> pc;
    Port<std::string> ir;

    instructionUnit(const std::string id) : Atomic<instructionUnit_State>(id, instructionUnit_State()) {
        clk = addInPort<int>("clk");
        rst = addInPort<int>("rst");
        instruction = addInPort<std::string>("instruction");
        pc = addOutPort<int>("pc");
        ir = addOutPort<std::string>("ir");
    }

    void internalTransition(instructionUnit_State& state) const override {
        //state.sigma = std::numeric_limits<double>::infinity();
        state.sigma = 0.1;
    }

    void externalTransition(instructionUnit_State& state, double e) const override {
        if(!clk->empty()){
            //std::cout << "[i_unit]: clk" << std::endl;
            if(!rst->empty()){
                state.pc = 0;
                state.ir = "000000000000000000";
                state.sigma = 0.1;
            }
            if(!instruction->empty()){
                state.ir = instruction->getBag().back();
                std::cout << "[i_unit]: instruction received from i_mem: " << state.ir << std::endl;
                state.pc++;
                if(state.pc == 2){
                    state.pc = 0;
                }
                
            }
            state.sigma = state.sigma - e;
        }
    }
    
    void output(const instructionUnit_State& state) const override {
        pc->addMessage(state.pc);
        ir->addMessage(state.ir);
        std::cout << "[i_unit]: instruction sent to d_unit: " << state.ir << std::endl;
    }

    [[nodiscard]] double timeAdvance(const instructionUnit_State& state) const override {     
        return state.sigma;
    }
};

#endif