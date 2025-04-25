#ifndef EXEC_UNIT_GEN_HPP
#define EXEC_UNIT_GEN_HPP

#include <string>
#include <limits>
#include <iostream>
#include "cadmium/modeling/devs/atomic.hpp"

using namespace cadmium;

struct ExecUnitGenState {
    double  sigma;
    size_t  idx;
    ExecUnitGenState() : sigma(0.0), idx(0) {}
};

#ifndef NO_LOGGING
inline std::ostream& operator<<(std::ostream &out, const ExecUnitGenState &s) {
    out << "{cycle=" << s.idx << ", sigma=" << s.sigma << "}";
    return out;
}
#endif

class ExecUnitGen : public Atomic<ExecUnitGenState> {
public:
    Port<int>         clk;
    Port<int>         rst;
    Port<int>         reg_wr_vld;
    Port<std::string> opcode;
    Port<std::string> dataMemAddr;
    Port<std::string> opnda;
    Port<std::string> opndb;
    Port<std::string> destAddr;

    ExecUnitGen(const std::string &id)
      : Atomic<ExecUnitGenState>(id, ExecUnitGenState())
    {
        clk         = addOutPort<int>("clk");
        rst         = addOutPort<int>("rst");
        reg_wr_vld  = addOutPort<int>("reg_wr_vld");
        opcode      = addOutPort<std::string>("opcode");
        dataMemAddr = addOutPort<std::string>("dataMemAddr");
        opnda       = addOutPort<std::string>("opnda");
        opndb       = addOutPort<std::string>("opndb");
        destAddr    = addOutPort<std::string>("destAddr");
    }

    void internalTransition(ExecUnitGenState &s) const override {
        s.idx++;
        if (s.idx < 6) {
            s.sigma = 0.1;
        } else {
            s.sigma = std::numeric_limits<double>::infinity();
        }
    }

    void externalTransition(ExecUnitGenState&, double) const override {
        // no external inputs
    }

    void output(const ExecUnitGenState &s) const override {
        clk->addMessage(1);
        switch (s.idx) {
            case 0:  // ADD 1+2 -> R1
                rst         ->addMessage(0);
                reg_wr_vld  ->addMessage(1);
                opcode      ->addMessage("0001");
                dataMemAddr ->addMessage("0000");
                opnda       ->addMessage("0000000000001");
                opndb       ->addMessage("0000000000010");
                destAddr    ->addMessage("001");
                break;
            case 1:  // SUB 3-1 -> R2
                rst         ->addMessage(0);
                reg_wr_vld  ->addMessage(1);
                opcode      ->addMessage("0010");
                dataMemAddr ->addMessage("0000");
                opnda       ->addMessage("0000000000011");
                opndb       ->addMessage("0000000000001");
                destAddr    ->addMessage("010");
                break;
            case 2:  // AND -> R3
                rst         ->addMessage(0);
                reg_wr_vld  ->addMessage(1);
                opcode      ->addMessage("0011");
                dataMemAddr ->addMessage("0000");
                opnda       ->addMessage("1010101010101");
                opndb       ->addMessage("0101010101010");
                destAddr    ->addMessage("011");
                break;
            case 3:  // LOAD from (R3+2) -> R4
                rst         ->addMessage(0);
                reg_wr_vld  ->addMessage(1);
                opcode      ->addMessage("1110");
                dataMemAddr ->addMessage("0010");
                opnda       ->addMessage("0000000000011");
                opndb       ->addMessage("0000000000000");
                destAddr    ->addMessage("100");
                break;
            case 4:  // STORE R7 -> M[R4+3]
                rst         ->addMessage(0);
                reg_wr_vld  ->addMessage(0);
                opcode      ->addMessage("1111");
                dataMemAddr ->addMessage("0011");
                opnda       ->addMessage("0000000000100");
                opndb       ->addMessage("0000000000111");
                destAddr    ->addMessage("101");
                break;
            case 5:  // RESET
                rst         ->addMessage(1);
                reg_wr_vld  ->addMessage(0);
                opcode      ->addMessage("0000");
                dataMemAddr ->addMessage("0000");
                opnda       ->addMessage("0000000000000");
                opndb       ->addMessage("0000000000000");
                destAddr    ->addMessage("000");
                break;
            default:
                break;
        }
    }

    [[nodiscard]] double timeAdvance(const ExecUnitGenState &s) const override {
        return s.sigma;
    }
};

#endif // EXEC_UNIT_GEN_HPP
