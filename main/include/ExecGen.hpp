#ifndef EXEC_UNIT_GEN_HPP
#define EXEC_UNIT_GEN_HPP

#include <string>
#include <vector>
#include <limits>
#include <iostream>
#include <tuple>
#include "cadmium/modeling/devs/atomic.hpp"

using namespace cadmium;
using std::string;
using std::vector;
using std::tuple;
using std::get;

struct ExecUnitGenState {
    double sigma;                   // time until next event
    int    testIdx;                 // which vector we're on
    vector<tuple<string,string,string,string>> tests;
    ExecUnitGenState()
      : sigma(0.0)
      , testIdx(0)
      , tests{
          // { opcode, opnda,       opndb,       destAddr }
          {"0001","00100010","01000100","011"}, // ADD  R1+R2→R3
          {"0010","01100110","00100010","100"}, // SUB  R3−R1→R4
          {"0011","01000100","01100110","101"}  // AND  R2&R3→R5
      }
    {}
};

#ifndef NO_LOGGING
inline std::ostream& operator<<(std::ostream &out, const ExecUnitGenState& s) {
    out << "{sigma=" << s.sigma << ", idx=" << s.testIdx << "}";
    return out;
}
#endif

class ExecUnitGen : public Atomic<ExecUnitGenState> {
public:
    // outputs that match ExecutionUnit's input ports
    Port<string> opcode, opnda, opndb, destAddr;
    Port<int>    reg_wr_vld;

    ExecUnitGen(const string &id) 
      : Atomic<ExecUnitGenState>(id, ExecUnitGenState())
    {
        opcode     = addOutPort<string>("opcode");
        opnda      = addOutPort<string>("opnda");
        opndb      = addOutPort<string>("opndb");
        destAddr   = addOutPort<string>("destAddr");
        reg_wr_vld = addOutPort<int>("reg_wr_vld");
    }

    void internalTransition(ExecUnitGenState &s) const override {
        s.testIdx++;
        if (s.testIdx < (int)s.tests.size()) {
            s.sigma = 0.1;  // schedule next test in 0.1 time units
        } else {
            s.sigma = std::numeric_limits<double>::infinity();
        }
    }

    void externalTransition(ExecUnitGenState&, double) const override {
        // no external inputs
    }

    void output(const ExecUnitGenState &s) const override {
        if (s.testIdx < (int)s.tests.size()) {
            auto &t = s.tests[s.testIdx];
            opcode    ->addMessage(get<0>(t));
            opnda     ->addMessage(get<1>(t));
            opndb     ->addMessage(get<2>(t));
            destAddr  ->addMessage(get<3>(t));
            reg_wr_vld->addMessage(1);
            std::cout << "[ExecUnitGen] test " << s.testIdx
                      << ": opcode=" << get<0>(t)
                      << " A=" << get<1>(t)
                      << " B=" << get<2>(t)
                      << " dest=" << get<3>(t)
                      << std::endl;
        }
    }

    [[nodiscard]] double timeAdvance(const ExecUnitGenState &s) const override {
        return s.sigma;
    }
};

#endif // EXEC_UNIT_GEN_HPP
