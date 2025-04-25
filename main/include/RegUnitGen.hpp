#ifndef REGISTER_UNIT_GEN_HPP
#define REGISTER_UNIT_GEN_HPP

#include <string>
#include <limits>
#include <bitset>
#include <iostream>
#include "cadmium/modeling/devs/atomic.hpp"

using namespace cadmium;

struct RegisterUnitGenState {
    double sigma = 0.1;
    int    cycle = 0;
};

#ifndef NO_LOGGING
inline std::ostream& operator<<(std::ostream &out, const RegisterUnitGenState& s) {
    out << "{sigma=" << s.sigma << ", cycle=" << s.cycle << "}";
    return out;
}
#endif

class RegisterUnitGen : public Atomic<RegisterUnitGenState> {
public:
    Port<int>         clk;
    Port<int>         rst;
    Port<int>         reg_wr_vld;
    Port<std::string> result;
    Port<std::string> dst;         //  3-bit string
    Port<std::string> opnda_addr;  // 3-bit string
    Port<std::string> opndb_addr;  // 3-bit string

    RegisterUnitGen(const std::string &id)
      : Atomic<RegisterUnitGenState>(id, RegisterUnitGenState())
    {
        clk         = addOutPort<int>("clk");
        rst         = addOutPort<int>("rst");
        reg_wr_vld  = addOutPort<int>("reg_wr_vld");
        result      = addOutPort<std::string>("result");
        dst         = addOutPort<std::string>("dst");
        opnda_addr  = addOutPort<std::string>("opnda_addr");
        opndb_addr  = addOutPort<std::string>("opndb_addr");
    }

    void internalTransition(RegisterUnitGenState& s) const override {
        s.cycle++;
        s.sigma = (s.cycle <= 4 ? 0.1 : std::numeric_limits<double>::infinity());
    }

    void externalTransition(RegisterUnitGenState&, double) const override {}

    void output(const RegisterUnitGenState& s) const override {
        clk->addMessage(1);

        // helpers to make 3-bit strings
        auto bits3 = [&](int x){ return std::bitset<3>(x).to_string(); };

        switch(s.cycle) {
          case 0:
            rst->addMessage(1);
            reg_wr_vld->addMessage(0);
            // reset
            std::cout << "[RegGen] cycle 0: RESET\n";
            break;

          case 1:
            rst->addMessage(0);
            reg_wr_vld->addMessage(1);
            result->addMessage("0000000000101");    // write data
            dst->addMessage(bits3(2));              // R2
            opnda_addr->addMessage(bits3(2));       // read R2 next
            opndb_addr->addMessage(bits3(0));       // dummy B
            std::cout << "[RegGen] cycle 1: WRITE R2 <- 0000000000101\n";
            break;

          case 2:
            rst->addMessage(0);
            reg_wr_vld->addMessage(1);
            result->addMessage("0000000000111");    // write data
            dst->addMessage(bits3(3));              // R3
            opnda_addr->addMessage(bits3(2));       // read R2
            opndb_addr->addMessage(bits3(3));       // read R3
            std::cout << "[RegGen] cycle 2: WRITE R3 <- 0000000000111\n";
            break;

          case 3:
            rst->addMessage(0);
            reg_wr_vld->addMessage(0);
            opnda_addr->addMessage(bits3(2));       // READ R2
            opndb_addr->addMessage(bits3(3));       // READ R3
            std::cout << "[RegGen] cycle 3: READ R2, R3\n";
            break;

          case 4:
          //reset again
            rst->addMessage(1);
            reg_wr_vld->addMessage(0);
            std::cout << "[RegGen] cycle 4: RESET\n";
            break;
        }
    }

    [[nodiscard]] double timeAdvance(const RegisterUnitGenState& s) const override {
        return s.sigma;
    }
};

#endif // REGISTER_UNIT_GEN_HPP
