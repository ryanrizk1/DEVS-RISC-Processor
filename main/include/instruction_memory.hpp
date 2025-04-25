#ifndef INSTRUCTION_MEMORY_HPP
#define INSTRUCTION_MEMORY_HPP

#include <string>
#include <array>
#include <limits>
#include <iostream>
#include "cadmium/modeling/devs/atomic.hpp"

using namespace cadmium;

struct instructionMemory_State {
    double sigma;
    int pc;
    std::string instruction;
    std::array<std::string, 32> instructions 
      = { "1110000000000", "1110000010001", "1110000100010" };
    instructionMemory_State()
      : sigma(std::numeric_limits<double>::infinity()), 
        pc(0), instruction(instructions[0])
    {}
};

#ifndef NO_LOGGING
inline std::ostream& operator<<(std::ostream &out, const instructionMemory_State& state) {
    out << "{σ=" << state.sigma << ", pc=" << state.pc << "}";
    return out;
}
#endif

class instructionMemory : public Atomic<instructionMemory_State> {
public:
    Port<int>         clk;         // clock
    Port<int>         pc;          // program counter input
    Port<std::string> instruction; // fetched instruction output

    instructionMemory(const std::string &id)
      : Atomic<instructionMemory_State>(id, instructionMemory_State())
    {
        clk         = addInPort<int>("clk");
        pc          = addInPort<int>("pc");
        instruction = addOutPort<std::string>("instruction");
    }

    void internalTransition(instructionMemory_State& state) const override {
        state.sigma = std::numeric_limits<double>::infinity();
    }

    void externalTransition(instructionMemory_State& state, double /*e*/) const override {
        // on rising edge of clk, fetch whichever address is on pc
        if (!clk->empty() && clk->getBag().back() == 1) {
            if (!pc->empty()) {
                state.pc = pc->getBag().back();
                // wrap around if we go past our small test program
                if (state.pc < 0 || state.pc >= (int)state.instructions.size()) {
                    state.pc = 0;
                }
                state.instruction = state.instructions[state.pc];
                state.sigma = 0.1;
            }
        }
    }

    void output(const instructionMemory_State& state) const override {
        std::cout << "[i_mem] sending instruction: " 
                  << state.instruction << std::endl;
        instruction->addMessage(state.instruction);
    }

    [[nodiscard]] double timeAdvance(const instructionMemory_State& state) const override {
        return state.sigma;
    }
};

#endif // INSTRUCTION_MEMORY_HPP
