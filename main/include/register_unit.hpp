#ifndef REGISTER_UNIT_HPP
#define REGISTER_UNIT_HPP

#include <array>
#include <string>
#include <limits>
#include <iostream>
#include "cadmium/modeling/devs/atomic.hpp"

using namespace cadmium;

// State: contains the 8 13-bit registers and their values. holds the address of the last read operands a and b.
struct registerUnit_State {
    double sigma = std::numeric_limits<double>::infinity();
    std::array<std::string,8> regs;
    int readA = 0, readB = 0;

    registerUnit_State() {
      // initialize all 8 registers to zero
      regs.fill(std::string(13,'0'));
    }
};

#ifndef NO_LOGGING
inline std::ostream& operator<<(std::ostream &out, const registerUnit_State& s) {
    out << "{sigma =" << s.sigma 
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
  Port<int>         reg_wr_vld; // write-enable. only writes to the reg file when this is 1.
  Port<std::string> result;     // the 13-bit data to be written in reg file
  Port<int>         dst;        // 3-bit index of the dest register 
  Port<int>         opnda_addr; // holds index of source register a 
  Port<int>         opndb_addr; // holds index of source register b

  // output ports
  Port<std::string> oprnd_a;    // holds the data from source register a
  Port<std::string> oprnd_b;    // holds the data from source register b

  registerUnit(const std::string &id)
    : Atomic<registerUnit_State>(id, registerUnit_State())
  {
    clk         = addInPort<int>("clk");
    rst         = addInPort<int>("rst");
    reg_wr_vld  = addInPort<int>("reg_wr_vld");
    result      = addInPort<std::string>("result");
    dst         = addInPort<int>("dst");
    opnda_addr  = addInPort<int>("opnda_addr");
    opndb_addr  = addInPort<int>("opndb_addr");

    oprnd_a     = addOutPort<std::string>("oprnd_a");
    oprnd_b     = addOutPort<std::string>("oprnd_b");
  }

  void internalTransition(registerUnit_State& s) const override {
    s.sigma = std::numeric_limits<double>::infinity();
  }

  // On rising clock: handle reset, writing to register file, and save read-addresses
  void externalTransition(registerUnit_State& s, double /*e*/) const override {
    if (!clk->empty() && clk->getBag().back() == 1) {
      // check if its a reset signal
      if (!rst->empty() && rst->getBag().back() == 1) {
        s.regs.fill(std::string(13,'0'));
      }
      else {
        // check if its a write back
        if (!reg_wr_vld->empty() && reg_wr_vld->getBag().back() == 1
            && !result->empty() && !dst->empty()) 
        {
          int d = dst->getBag().back();
          if (d >= 0 && d < 8) {
            s.regs[d] = result->getBag().back();
            std::cout << "[reg_unit]: dest" << d 
                      << "  " << s.regs[d] << std::endl;
          }
        }
      }
      // always save the address of the two source operands to be used in the output()
      if (!opnda_addr->empty()) s.readA = opnda_addr->getBag().back();
      if (!opndb_addr->empty()) s.readB = opndb_addr->getBag().back();
      s.sigma = 0.0;
    }
  }

  // Output the values of the two source operands. The ALU will choose which operand is valid for the current operation.
  void output(const registerUnit_State& s) const override {
    std::string a = s.regs[s.readA];
    std::string b = s.regs[s.readB];
    oprnd_a->addMessage(a);
    oprnd_b->addMessage(b);
    std::cout << "[reg_unit]: out  operand A=" << a 
              << " operand B=" << b << std::endl;
  }

  [[nodiscard]] double timeAdvance(const registerUnit_State& s) const override {
    return s.sigma;
  }
};

#endif // REGISTER_UNIT_HPP
