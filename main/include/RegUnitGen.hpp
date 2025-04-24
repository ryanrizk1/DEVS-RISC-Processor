#ifndef REGISTER_UNIT_GEN_HPP
#define REGISTER_UNIT_GEN_HPP

#include <string>
#include <limits>
#include <iostream>
#include "cadmium/modeling/devs/atomic.hpp"

using namespace cadmium;

struct RegisterUnitGenState {
    double sigma  = 0.1;
    int    cycle  = 0;
};

#ifndef NO_LOGGING
inline std::ostream& operator<<(std::ostream &out, const RegisterUnitGenState& s) {
    out << "{sigma =" << s.sigma << ", cycle=" << s.cycle << "}";
    return out;
}
#endif

class RegisterUnitGen : public Atomic<RegisterUnitGenState> {
public:
    Port<int>         clk;
    Port<int>         rst;
    Port<int>         reg_wr_vld;
    Port<std::string> result;
    Port<int>         dst;
    Port<int>         opnda_addr;
    Port<int>         opndb_addr;

    RegisterUnitGen(const std::string &id)
      : Atomic<RegisterUnitGenState>(id, RegisterUnitGenState())
    {
        clk         = addOutPort<int>("clk");
        rst         = addOutPort<int>("rst");
        reg_wr_vld  = addOutPort<int>("reg_wr_vld");
        result      = addOutPort<std::string>("result");
        dst         = addOutPort<int>("dst");
        opnda_addr  = addOutPort<int>("opnda_addr");
        opndb_addr  = addOutPort<int>("opndb_addr");
    }

    void internalTransition(RegisterUnitGenState& s) const override {
        s.cycle++;
        if (s.cycle <= 4) {
            s.sigma = 0.1;  // schedule next cycle in 0.1
        } else {
            s.sigma = std::numeric_limits<double>::infinity();
        }
    }

    void externalTransition(RegisterUnitGenState&, double) const override {
    }

    void output(const RegisterUnitGenState& s) const override {
        clk->addMessage(1);

        switch(s.cycle) {
          case 0:
            // cycle 0: reset
            rst->addMessage(1);
            reg_wr_vld->addMessage(0);
            std::cout << "[RegGen] cycle 0: RESET\n";
            break;
          case 1:
            // cycle 1: deassert reset, write 13'b0000000000101 into R2
            rst->addMessage(0);
            reg_wr_vld->addMessage(1);
            result->addMessage("0000000000101");
            dst->addMessage(2);
            opnda_addr->addMessage(2);  // read R2 after modification
            opndb_addr->addMessage(0);  // not used here
            std::cout << "[RegGen] cycle 1: WRITE R2 <- 0000000000101\n";
            break;
          case 2:
            // cycle 2: write 13'b0000000000111 into R3
            rst->addMessage(0);
            reg_wr_vld->addMessage(1);
            result->addMessage("0000000000111");
            dst->addMessage(3);
            opnda_addr->addMessage(2);  // read R3 after modification
            opndb_addr->addMessage(3);  // read R2 to check it still holds previus value
            std::cout << "[RegGen] cycle 2: WRITE R3 <- 0000000000111\n";
            break;
          case 3:
            // cycle 3: no write, just read R2 and R3
            rst->addMessage(0);
            reg_wr_vld->addMessage(0);
            opnda_addr->addMessage(2);
            opndb_addr->addMessage(3);
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
