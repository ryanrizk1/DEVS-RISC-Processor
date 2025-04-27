#ifndef RESET_UNIT_HPP
#define RESET_UNIT_HPP

#include <limits>
#include <iostream>
#include "cadmium/modeling/devs/atomic.hpp"

using namespace cadmium;

// State: only schedules an output immediately after a clock tick
struct resetUnit_State {
    double sigma = std::numeric_limits<double>::infinity();
};

#ifndef NO_LOGGING
inline std::ostream& operator<<(std::ostream &out, const resetUnit_State&) {
    out << "[resetUnit]";
    return out;
}
#endif

class resetUnit : public Atomic<resetUnit_State> {
public:
    Port<int> clk;  // clock input
    Port<int> rst;  // reset output (always 0)

    resetUnit(const std::string &id)
      : Atomic<resetUnit_State>(id, resetUnit_State())
    {
        clk = addInPort<int>("clk");
        rst = addOutPort<int>("rst");
    }

    // after output, go back to waiting for next clock
    void internalTransition(resetUnit_State& s) const override {
        s.sigma = std::numeric_limits<double>::infinity();
    }

    // on rising clock edge, schedule an immediate output
    void externalTransition(resetUnit_State& s, double /*e*/) const override {
        if (!clk->empty() && clk->getBag().back() == 1) {
            s.sigma = 0.0;
        }
    }

    // emit rst=0
    void output(const resetUnit_State&) const override {
        rst->addMessage(0);
    }

    [[nodiscard]] double timeAdvance(const resetUnit_State& s) const override {
        return s.sigma;
    }
};

#endif // RESET_UNIT_HPP
