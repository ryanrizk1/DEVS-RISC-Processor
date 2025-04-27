#ifndef INSTRUCTION_MEMORY_HPP
#define INSTRUCTION_MEMORY_HPP

#include <string>
#include <array>
#include <iostream>
#include "cadmium/modeling/devs/atomic.hpp"

using namespace cadmium;

struct instructionMemory_State {
    double sigma;
    int pc;
    std::string instruction;
    // std::array<std::string, 32> instructions = {"1110000000000", "1110000010001", "1110000100010", "1110000110011", "1110001000100", "1110001010101", "1110001100110", "1110001110111", "0001000001000", "0010111110001", "0011010101010", "0100011100011", "0101100100100", "0110101000101", "0111110000110", "1000111000111", "1001000000000", "1010001000001", "1011010000010", "1100011000011", "1101100000100", "1111000001000", "1111000011001", "1111000101010", "1111000111011", "1111001001100", "1111001011101", "1111001101110", "1111001111111", "0000000000000", "0000000000000", "0000000000000"};
    std::array<std::string, 32> instructions = {
        "0001001010011",
        "0010001010011",
        "0011010011100",
        "0100011100101",
        "0101100101110",
        "1110000010001",
        "1111000010010",
        "0001010101111","0000000000000","0000000000000","0000000000000","0000000000000","0000000000000","0000000000000","0000000000000","0000000000000","0000000000000","0000000000000","0000000000000","0000000000000","0000000000000","0000000000000","0000000000000","0000000000000","0000000000000","0000000000000","0000000000000","0000000000000","0000000000000","0000000000000","0000000000000","0000000000000"};
    
    explicit instructionMemory_State(): sigma(std::numeric_limits<double>::infinity()){
    }
};

#ifndef NO_LOGGING
std::ostream& operator<<(std::ostream &out, const instructionMemory_State& state) {
    out  << "{sigma: " << state.sigma << ", pc: " << state.pc << "}";
    return out;
}
#endif

class instructionMemory : public Atomic<instructionMemory_State> {
    public:
    
    //Port<int> init;
    Port<int> pc;
    Port<std::string> instruction;

    instructionMemory(const std::string id) : Atomic<instructionMemory_State>(id, instructionMemory_State()) {
        //init = addInPort<int>("init");
        pc = addInPort<int>("pc");
        instruction = addOutPort<std::string>("instruction");
    }

    void internalTransition(instructionMemory_State& state) const override {
        state.sigma = std::numeric_limits<double>::infinity();
    }

    void externalTransition(instructionMemory_State& state, double e) const override {
        // if(!init->empty()){
        //     state.pc = init->getBag().back();
        //     state.instruction = state.instructions[state.pc];
        //     state.sigma = 0.1;
        // }
        if(!pc->empty()){
            state.pc = pc->getBag().back();
            //int loc = std::stoi(state.pc);
            state.instruction = state.instructions[state.pc]; 
            state.sigma = 0.1;
        }
    }
    
    void output(const instructionMemory_State& state) const override {
        std::cout << "[i_mem]: instruction sent to i_unit: " << state.instruction << std::endl; 
        instruction->addMessage(state.instruction);
    }

    [[nodiscard]] double timeAdvance(const instructionMemory_State& state) const override {     
        return state.sigma;
    }
};

#endif