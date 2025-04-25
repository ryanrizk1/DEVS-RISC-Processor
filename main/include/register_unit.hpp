#ifndef REGISTER_UNIT_HPP
#define REGISTER_UNIT_HPP

#include <array>
#include <string>
#include <limits>
#include <iostream>
#include "cadmium/modeling/devs/atomic.hpp"

using namespace cadmium;

// State: contains the 8×13-bit registers and the last read addresses
struct registerUnit_State {
    double sigma = std::numeric_limits<double>::infinity();
    std::array<std::string,8> regs;
    int readA = 0, readB = 0;

    registerUnit_State() {
      regs.fill(std::string(13,'0'));
    }
};

#ifndef NO_LOGGING
inline std::ostream& operator<<(std::ostream &out, const registerUnit_State& s) {
    out << "{sigma=" << s.sigma 
        << ", readA=" << s.readA 
        << ", readB=" << s.readB << "}";
    return out;
}
#endif

class registerUnit : public Atomic<registerUnit_State> {
public:
  // input ports
  Port<int>         clk;        // rising edge = 1
  Port<int>         rst;        // 1 = reset
  Port<int>         reg_wr_vld; // write-enable
  Port<std::string> result;     // 13-bit data to write
  Port<std::string> dst;        // 3-bit dest register index
  Port<std::string> opnda_addr; // 3-bit source A index
  Port<std::string> opndb_addr; // 3-bit source B index

  // output ports
  Port<std::string> oprnd_a;    // data from source A
  Port<std::string> oprnd_b;    // data from source B

  registerUnit(const std::string &id)
    : Atomic<registerUnit_State>(id, registerUnit_State())
  {
    clk         = addInPort<int>("clk");
    rst         = addInPort<int>("rst");
    reg_wr_vld  = addInPort<int>("reg_wr_vld");
    result      = addInPort<std::string>("result");
    dst         = addInPort<std::string>("dst");
    opnda_addr  = addInPort<std::string>("opnda_addr");
    opndb_addr  = addInPort<std::string>("opndb_addr");

    oprnd_a     = addOutPort<std::string>("oprnd_a");
    oprnd_b     = addOutPort<std::string>("oprnd_b");
  }

  void internalTransition(registerUnit_State& s) const override {
    s.sigma = std::numeric_limits<double>::infinity();
  }

  // On rising clock: reset, write-back, and latch read-addresses
  void externalTransition(registerUnit_State& s, double /*e*/) const override {
    if (!clk->empty() && clk->getBag().back() == 1) {
      bool hasUpdate = false;

      //  reset
      if (!rst->empty() && rst->getBag().back() == 1) {
        s.regs.fill(std::string(13,'0'));
        hasUpdate = true;
      } else {
        // write-back if enabled
        if (!reg_wr_vld->empty() && reg_wr_vld->getBag().back() == 1
            && !result->empty() && !dst->empty())
        {
          int d = std::stoi(dst->getBag().back(), nullptr, 2);
          if (d >= 0 && d < 8) {
            s.regs[d] = result->getBag().back();
            std::cout << "[reg_unit] write R" << d 
                      << " = " << s.regs[d] << std::endl;
            hasUpdate = true;
          }
        }
      }

      // latch source-A index
      if (!opnda_addr->empty()) {
        s.readA = std::stoi(opnda_addr->getBag().back(), nullptr, 2);
        hasUpdate = true;
      }
      // latch source-B index
      if (!opndb_addr->empty()) {
        s.readB = std::stoi(opndb_addr->getBag().back(), nullptr, 2);
        hasUpdate = true;
      }

      // only send output if something actually changed
      s.sigma = hasUpdate
              ? 0.0
              : std::numeric_limits<double>::infinity();
    }
  }

  // Output the two source operands
  void output(const registerUnit_State& s) const override {
    oprnd_a->addMessage(s.regs[s.readA]);
    oprnd_b->addMessage(s.regs[s.readB]);
    std::cout << "[reg_unit] out A=" << s.regs[s.readA] 
              << "  B=" << s.regs[s.readB] << std::endl;
  }

  [[nodiscard]] double timeAdvance(const registerUnit_State& s) const override {
    return s.sigma;
  }
};

#endif // REGISTER_UNIT_HPP
