#ifndef RISC_GENERATOR_HPP
#define RISC_GENERATOR_HPP

#include <vector>
#include <string>
#include <limits>
#include <iostream>
#include "cadmium/modeling/devs/atomic.hpp"

using namespace cadmium;

// State for RISC instruction generator that simulates instruction memory
struct RiscGeneratorState {
    double                  sigma;      // time until next event
    int                     currentPc;  // last PC seen or initialized at first push
    bool                    firstFire;  // flag for initial instruction push
    std::vector<std::string> instructions; // program image

    RiscGeneratorState()
      : sigma(0.0)            // fire immediately at t=0
      , currentPc(0)
      , firstFire(true)
      , instructions{
          "0001001010011",  // ADD  R1 + R2 → R3
          "0010011001100",  // SUB  R3 - R1 → R4
          "0011010011101"   // AND  R2 & R3 → R5
      }
    {}
};

#ifndef NO_LOGGING
inline std::ostream& operator<<(std::ostream &out, const RiscGeneratorState& s) {
    out << "{sigma=" << s.sigma
        << ", pc=" << s.currentPc
        << ", firstFire=" << (s.firstFire ? "yes" : "no")
        << "}";
    return out;
}
#endif

class RiscGenerator : public Atomic<RiscGeneratorState> {
public:
    // input: PC values from the fetch unit (instructionUnit)
    Port<int>         pc;
    // output: 13-bit instruction words
    Port<std::string> instruction;

    RiscGenerator(const std::string &id)
      : Atomic<RiscGeneratorState>(id, RiscGeneratorState())
    {
        pc        = addInPort<int>("pc");
        instruction = addOutPort<std::string>("instruction");
    }

    // Internal event: only for initial push at time 0
    void internalTransition(RiscGeneratorState &s) const override {
        s.firstFire = false;
        s.sigma     = std::numeric_limits<double>::infinity();
    }

    // External event: on each PC update, schedule immediate output
    void externalTransition(RiscGeneratorState &s, double /*e*/) const override {
        if (!pc->empty()) {
            s.currentPc = pc->getBag().back();
            s.sigma     = 0.0;    // trigger output immediately
        }
    }

    // Output the instruction at the current PC
    void output(const RiscGeneratorState &s) const override {
        if (s.currentPc >= 0 && s.currentPc < static_cast<int>(s.instructions.size())) {
            const auto &instr = s.instructions[s.currentPc];
            instruction->addMessage(instr);
            std::cout << "[RiscGenerator] -> pc=" << s.currentPc
                      << " instr=" << instr << std::endl;
        }
    }

    [[nodiscard]] double timeAdvance(const RiscGeneratorState &s) const override {
        return s.sigma;
    }
};

#endif // RISC_GENERATOR_HPP