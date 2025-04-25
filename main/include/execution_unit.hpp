#ifndef EXECUTION_UNIT_HPP
#define EXECUTION_UNIT_HPP

#include <string>
#include <iostream>
#include "cadmium/modeling/devs/atomic.hpp"

using namespace cadmium;

struct executionUnit_State {
    double sigma;
    std::string dataMemAddr; //immediate offset
    std::string opcode; // 4 bit opcode
    std::string opnda; //13 bit operand a value
    std::string opndb; //13 bit operand b value
    std::string dest; // 3 bit addres of destination register

    std::string result;     // 13-bit ALU result
    std::string dataMemAddress;       // 13-bit memory address
    std::string storeData;     // 13-bit data to store
    explicit executionUnit_State() = default;
};

#ifndef NO_LOGGING
inline std::ostream& operator<<(std::ostream &out, const executionUnit_State& s) {
    out << "{sigma =" << s.sigma
        << ", op=" << s.opcode
        << ", operand A=" << s.opnda
        << ", operand B=" << s.opndb
        << ", dest=" << s.dest
        << ", aluRes=" << s.aluResult
        << ", dataMemAddr=" << s.memAddr << "}";
    return out;
}
#endif

class executionUnit : public Atomic<executionUnit_State> {
    public:

    Port<int> clk;
    Port<int> rst;

    // From Decode Unit
    Port<std::string> dataMemAddr; //offset
    Port<std::string> opcode;
    // Port<std::string> opndaAddr;
    // Port<std::string> opndbAddr;
    Port<std::string> destAddr;

    // From Register Unit
    Port<std::string> opnda;
    Port<std::string> opndb;

    // To Register Unit
    Port<std::string> destReg;

    // To Data Memory
    Port<std::string> dataMemAddress;
    Port<std::string> opndaReg;
    Port<std::string> opndbReg;


    executionUnit(const std::string id) : Atomic<executionUnit_State>(id, executionUnit_State()) {
        clk = addInPort<int>("clk");
        rst = addInPort<int>("rst");

        dataMemAddr = addInPort<std::string>("dataMemAddr"); //4 bit offset
        opcode = addInPort<std::string>("opcode"); 
        opnda = addInPort<std::string>("opnda"); //value in oprnd a
        opndb = addInPort<std::string>("opndb"); //value in oprnd b
        destAddr = addInPort<std::string>("destAddr"); //register to write back to

        // opndaAddr = addInPort<std::string>("opndaAddr");
        // opndbAddr = addInPort<std::string>("opndbAddr");

        opndaReg = addOutPort<std::string>("opndaReg"); //connected to data mem. contains data from a store instruction.
        opndbReg = addOutPort<std::string>("opndbReg"); //
        destReg = addOutPort<std::string>("destReg"); //13 bit result to write back to register file

        dataMemAddress = addOutPort<std::string>("dataMemAddress"); //pointer to data memory
    }

    void internalTransition(executionUnit_State& state) const override {
        s.sigma = std::numeric_limits<double>::infinity();
    }

    void externalTransition(executionUnit_State& s, double /*e*/) const override {
        if (!clk->empty() && clk->getBag().back() == 1) {
            // check if reset:
            if (!rst->empty() && rst->getBag().back() == 1) {
                s = executionUnit_State();
                return;
            }

            s.dataMemAddr     = dataMemAddr  ->getBag().back();
            s.opcode          = opcode       ->getBag().back();
            s.opnda           = opnda        ->getBag().back();
            s.opndb           = opndb        ->getBag().back();
            s.dest            = destAddr     ->getBag().back();

            
            // clear outputs
            s.result.clear();
            s.dataMemAddress.clear();
            s.storeData.clear();

            // decode 4-bit opcode
            const auto code = s.opcode;
            int   a = std::stoi(s.opnda, nullptr, 2);
            int   b = std::stoi(s.opndb, nullptr, 2);
            int   val;

            if      (code == "0000") { // NOP
                val = 0;
            }
            else if (code == "0001") { // ADD
                val = (a + b) & ((1<<13)-1);
            }
            else if (code == "0010") { // SUB
                val = (a - b) & ((1<<13)-1);
            }
            else if (code == "0011") { // AND
                val = a & b;
            }
            else if (code == "0100") { // OR
                val = a | b;
            }
            else if (code == "0101") { // XOR
                val = a ^ b;
            }
            else if (code == "0110") { // INC
                val = (a + 1) & ((1<<13)-1);
            }
            else if (code == "0111") { // DEC
                val = (a - 1) & ((1<<13)-1);
            }
            else if (code == "1000") { // NEG (two's-complement)
                val = ((~a) + 1) & ((1<<13)-1);
            }
            else if (code == "1001") { // SHL (<<1)
                val = (a << 1) & ((1<<13)-1);
            }
            else if (code == "1010") { // SHR (logical >>1)
                val = (unsigned(a) >> 1) & ((1<<13)-1);
            }
            else if (code == "1011") { // ROL
                val = ((a<<1) | (a>>(13-1))) & ((1<<13)-1);
            }
            else if (code == "1100") { // ROR
                val = ((a>>1) | (a<<(13-1))) & ((1<<13)-1);
            }
            else if (code == "1101") { // PASS A
                val = a;
            }
            else if (code == "1110") { // load
                { int off = std::stoi(s.dataMemAddr, nullptr,2);
                  int addr = (a + off) & ((1<<13)-1);
                  s.dataMemAddress = std::bitset<13>(addr).to_string();
                }
                s.result = s.dataMemAddress;
                s.storeData.clear();
                s.sigma = 0.1;
                return;
            }
            else if (code == "1111") { // store
                { int off = std::stoi(s.dataMemAddr, nullptr,2);
                  int addr = (a + off) & ((1<<13)-1);
                  s.dataMemAddress = std::bitset<13>(addr).to_string();
                }
                s.storeData = s.opndb;
                s.result.clear();
                s.sigma = 0.1;
                return;
            }
            else {
                val = 0; // default 
            }

            // for all other ALU ops
            s.result = std::bitset<13>(val).to_string();
            s.dataMemAddress.clear();
            s.storeData.clear();

            s.sigma = 0.1;
        }
    }

    
    
    // output function
    void output(const executionUnit_State& state) const override {
        if (!s.dataMemAddress.empty()) dataMemAddress->addMessage(s.dataMemAddress);
        if (!s.storeData     .empty()) opndaReg      ->addMessage(s.storeData);
        destReg       ->addMessage(s.result);
    }

    // time_advance function
    [[nodiscard]] double timeAdvance(const executionUnit_State& state) const override {     
            return state.sigma;
    }
};

#endif