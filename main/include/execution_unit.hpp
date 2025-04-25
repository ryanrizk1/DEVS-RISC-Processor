#ifndef EXECUTION_UNIT_HPP
#define EXECUTION_UNIT_HPP

#include <string>
#include <limits>
#include <bitset>
#include <iostream>
#include "cadmium/modeling/devs/atomic.hpp"

using namespace cadmium;

struct executionUnit_State {
    double sigma;
    std::string dataMemAddr;    // direct memory index (4 bits)
    std::string opcode;         // 4 bit opcode
    std::string opnda;          // 13 bit operand A value
    std::string opndb;          // 13 bit operand B value
    std::string dest;           // 3 bit address of destination register

    std::string result;         // 13-bit ALU result
    std::string dataMemAddress; // 4-bit memory index to DM
    std::string storeData;      // 13-bit data to store

    int reg_wr_vld;             // from decode unit

    // constructor sets everything to default
    explicit executionUnit_State()
      : sigma(std::numeric_limits<double>::infinity()),
        dataMemAddr(""), opcode(""), opnda(""), opndb(""), dest(""),
        result(""), dataMemAddress(""), storeData(""), reg_wr_vld(0)
    {}
};

#ifndef NO_LOGGING
inline std::ostream& operator<<(std::ostream &out, const executionUnit_State& s) {
    out << "{sigma=" << s.sigma
        << ", opcode=" << s.opcode
        << ", A=" << s.opnda
        << ", B=" << s.opndb
        << ", dest=" << s.dest
        << ", result=" << s.result
        << ", memAddr=" << s.dataMemAddress
        << ", store=" << s.storeData
        << ", wr=" << s.reg_wr_vld
        << "}";
    return out;
}
#endif

class executionUnit : public Atomic<executionUnit_State> {
public:
    Port<int>           clk;
    Port<int>           rst;

    // From Decode Unit
    Port<std::string>   dataMemAddr; // direct 4-bit index
    Port<std::string>   opcode;
    Port<std::string>   destAddr;
    Port<int>           reg_wr_vld; 

    // From Register Unit
    Port<std::string>   opnda;
    Port<std::string>   opndb;

    // To Register Unit
    Port<std::string>   destReg;
    Port<int>           reg_wr_vld_out;

    // To Data Memory
    Port<std::string>   dataMemAddress;
    Port<std::string>   opndaReg;    // data to store

    executionUnit(const std::string &id)
      : Atomic<executionUnit_State>(id, executionUnit_State())
    {
        clk             = addInPort<int>("clk");
        rst             = addInPort<int>("rst");
        dataMemAddr     = addInPort<std::string>("dataMemAddr");
        opcode          = addInPort<std::string>("opcode");      
        opnda           = addInPort<std::string>("opnda");
        opndb           = addInPort<std::string>("opndb");
        destAddr        = addInPort<std::string>("destAddr");
        reg_wr_vld      = addInPort<int>("reg_wr_vld");

        opndaReg        = addOutPort<std::string>("opndaReg");
        destReg         = addOutPort<std::string>("destReg");
        dataMemAddress  = addOutPort<std::string>("dataMemAddress");
        reg_wr_vld_out  = addOutPort<int>("reg_wr_vld");
    }

    void internalTransition(executionUnit_State& s) const override {
        s.sigma = std::numeric_limits<double>::infinity();
    }

    void externalTransition(executionUnit_State& s, double /*e*/) const override {
        if (!clk->empty() && clk->getBag().back() == 1) {
            // check if reset
            if (!rst->empty() && rst->getBag().back() == 1) {
                s = executionUnit_State();
                return;
            }

            // get inputs
            s.dataMemAddr = dataMemAddr->getBag().back();
            s.opcode      = opcode->getBag().back();
            s.opnda       = opnda->getBag().back();
            s.opndb       = opndb->getBag().back();
            s.dest        = destAddr->getBag().back();
            s.reg_wr_vld  = reg_wr_vld->getBag().back();

            // clear any old outputs
            s.result.clear();
            s.dataMemAddress.clear();
            s.storeData.clear();

            // convert operands for ALU
            int a = std::stoi(s.opnda, nullptr, 2);
            int b = std::stoi(s.opndb, nullptr, 2);
            int val = 0;

            // ALU opr based on opcode
            if      (s.opcode == "0000") val = 0;
            else if (s.opcode == "0001") val = (a + b) & ((1<<13)-1);
            else if (s.opcode == "0010") val = (a - b) & ((1<<13)-1);
            else if (s.opcode == "0011") val = a & b;
            else if (s.opcode == "0100") val = a | b;
            else if (s.opcode == "0101") val = a ^ b;
            else if (s.opcode == "0110") val = (a + 1) & ((1<<13)-1);
            else if (s.opcode == "0111") val = (a - 1) & ((1<<13)-1);
            else if (s.opcode == "1000") val = ((~a)+1) & ((1<<13)-1);
            else if (s.opcode == "1001") val = (a << 1) & ((1<<13)-1);
            else if (s.opcode == "1010") val = (unsigned(a) >> 1) & ((1<<13)-1);
            else if (s.opcode == "1011") val = ((a<<1)|(a>>12)) & ((1<<13)-1);
            else if (s.opcode == "1100") val = ((a>>1)|(a<<12)) & ((1<<13)-1);
            else if (s.opcode == "1101") val = a;

            // if it's a LOAD, send the 4-bit index straight to memory
            if (s.opcode == "1110") {
                int idx = std::stoi(s.dataMemAddr, nullptr, 2);
                s.dataMemAddress = std::bitset<4>(idx).to_string();
                s.sigma = 0.1;
                return;
            }
            // if it's a STORE, same but include data
            if (s.opcode == "1111") {
                int idx = std::stoi(s.dataMemAddr, nullptr, 2);
                s.dataMemAddress = std::bitset<4>(idx).to_string();
                s.storeData      = s.opndb;
                s.sigma          = 0.1;
                return;
            }

            // else it's an ALU op
            s.result = std::bitset<13>(val).to_string();
            s.sigma  = 0.1;
        }
    }

    void output(const executionUnit_State& s) const override {
        // send to data memory if needed
        if (!s.dataMemAddress.empty()) {
            dataMemAddress->addMessage(s.dataMemAddress);
        }
        if (!s.storeData.empty()) {
            opndaReg->addMessage(s.storeData);
        }
        // send to register file if ALU or LOAD result
        if (!s.result.empty() || s.opcode == "1110") {
            destReg->addMessage(s.result);
            reg_wr_vld_out->addMessage(s.reg_wr_vld);
        }
    }

    [[nodiscard]] double timeAdvance(const executionUnit_State& s) const override {
        return s.sigma;
    }
};

#endif // EXECUTION_UNIT_HPP
