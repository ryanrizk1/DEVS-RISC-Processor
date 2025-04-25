#ifndef INSTRUNIT_GEN_HPP
#define INSTRUNIT_GEN_HPP

#include <string>
#include <vector>
#include <limits>
#include <iostream>
#include "cadmium/modeling/devs/atomic.hpp"

using namespace cadmium;

// State for test generator
struct InstrUnitGenState {
    double sigma;               // time until next event
    int    cycleCount;          // output cycle we are on
    std::vector<std::string> instrQueue; // test instructions

    InstrUnitGenState() 
      : sigma(0.1)
      , cycleCount(0)
      , instrQueue{
          "1110000000000",  // test #1 load opr
          "0001000100010",  // test #2 ALU opr
          "1111001000011"   // test #3 st opr
      } 
    {}
};

#ifndef NO_LOGGING
inline std::ostream& operator<<(std::ostream &out, const InstrUnitGenState& s) {
    out << "{sigma =" << s.sigma << ", cycle=" << s.cycleCount << "}";
    return out;
}
#endif

class InstrUnitGen : public cadmium::Atomic<InstrUnitGenState> {
public:
    Port<int>         clk;
    Port<int>         rst;
    Port<std::string> instruction;

    InstrUnitGen(const std::string &id)
      : Atomic<InstrUnitGenState>(id, InstrUnitGenState())
    {
        clk         = addOutPort<int>("clk");
        rst         = addOutPort<int>("rst");
        instruction = addOutPort<std::string>("instruction");
    }

    void internalTransition(InstrUnitGenState& s) const override {
        s.cycleCount++;
        if (s.cycleCount <= static_cast<int>(s.instrQueue.size())) {
            s.sigma = 0.1;            
        } else {
            s.sigma = std::numeric_limits<double>::infinity();
        }
    }

    void externalTransition(InstrUnitGenState&, double) const override {}

    void output(const InstrUnitGenState& s) const override {
        clk->addMessage(1);

        if (s.cycleCount == 0) {
            // first cycle: assert reset 
            rst->addMessage(1);
        } else {
            // next cycles: deassert reset
            rst->addMessage(0);
            // send instruction
            if (s.cycleCount <= static_cast<int>(s.instrQueue.size())) {
                const auto &instr = s.instrQueue[s.cycleCount - 1];
                instruction->addMessage(instr);
                std::cout 
                  << "[InstrUnitGen] cycle " << s.cycleCount 
                  << ": rst=0, instr=" << instr << std::endl;
            }
        }
    }

    // Time advance
    [[nodiscard]] double timeAdvance(const InstrUnitGenState& s) const override {
        return s.sigma;
    }
};

#endif // INSTRUNIT_GEN_HPP
