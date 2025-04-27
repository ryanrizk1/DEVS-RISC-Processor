// #ifndef REGISTER_UNIT_HPP
// #define REGISTER_UNIT_HPP

// #include <string>
// #include <iostream>
// #include "cadmium/modeling/devs/atomic.hpp"

// using namespace cadmium;

// struct registerUnit_State {
//     double sigma;
//     //you can have as many state variables as you want/ need

//     explicit registerUnit_State(): sigma(1){
//     }
// };

// #ifndef NO_LOGGING
// std::ostream& operator<<(std::ostream &out, const registerUnit_State& state) {
//     out  << "{" << state.sigma << "}";
//     return out;
// }
// #endif

// class registerUnit : public Atomic<registerUnit_State> {
//     public:

//     Port<int> clk;
//     Port<int> rst;
//     Port<std::string> result;
//     Port<std::string>

//     registerUnit(const std::string id) : Atomic<registerUnit_State>(id, registerUnit_State()) {
//         //Constructor of your atomic model. Initialize ports here.
//     }

//     // inernal transition
//     void internalTransition(registerUnit_State& state) const override {
//         //your internal transition function goes here
//         state.sigma += 1;
//     }

//     // external transition
//     void externalTransition(registerUnit_State& state, double e) const override {
//         //your external transition function hoes here
//     }
    
    
//     // output function
//     void output(const registerUnit_State& state) const override {
//         //your output function goes here
//     }

//     // time_advance function
//     [[nodiscard]] double timeAdvance(const registerUnit_State& state) const override {     
//             return state.sigma;
//     }
// };

// #endif

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
    double sigma;
    std::array<std::string,8> regs = {"00000000", "00100010", "01000100", "01100110", "10001000", "10101010", "11001100", "11111111"};
    int readA = 0, readB = 0;
    std::string dst;
    std::string dataIn;
    std::string ldData;
    std::string ldReg;
    std::string stAddr;
    std::string stData;
    std::string stReg;
    explicit registerUnit_State(): sigma(std::numeric_limits<double>::infinity()) {
      //regs.fill(std::string(13,'0'));
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
//   Port<int>         clk;        // rising edge = 1
//   Port<int>         rst;        // 1 = reset
  Port<int>         reg_wr_vld; // write-enable
  Port<std::string> result;     // 13-bit data to write
  Port<std::string> dst;        // 3-bit dest register index
  Port<std::string> opnda_addr; // 3-bit source A index
  Port<std::string> opndb_addr; // 3-bit source B index

  // output ports
  Port<std::string> oprnd_a;    // data from source A
  Port<std::string> oprnd_b;    // data from source B

  Port<std::string> ldData;
  Port<std::string> ldReg;

  Port<std::string> stAddrIn;
  Port<std::string> stReg;

  Port<std::string> stAddrOut;
  Port<std::string> stData;

  registerUnit(const std::string &id)
    : Atomic<registerUnit_State>(id, registerUnit_State())
  {
    // clk         = addInPort<int>("clk");
    // rst         = addInPort<int>("rst");
    reg_wr_vld  = addInPort<int>("reg_wr_vld");
    result      = addInPort<std::string>("result");
    dst         = addInPort<std::string>("dst");
    opnda_addr  = addInPort<std::string>("opnda_addr");
    opndb_addr  = addInPort<std::string>("opndb_addr");

    oprnd_a     = addOutPort<std::string>("oprnd_a");
    oprnd_b     = addOutPort<std::string>("oprnd_b");

    ldData = addInPort<std::string>("ldData");
    ldReg = addInPort<std::string>("ldReg");

    stAddrIn = addInPort<std::string>("stAddrIn");
    stReg = addInPort<std::string>("stReg");

    stAddrOut = addOutPort<std::string>("stAddrOut");
    stData = addOutPort<std::string>("stData");
  }

  void internalTransition(registerUnit_State& s) const override {
    s.sigma = std::numeric_limits<double>::infinity();
  }

  // On rising clock: reset, write-back, and latch read-addresses
  void externalTransition(registerUnit_State& s, double /*e*/) const override {
    //if (!clk->empty() && clk->getBag().back() == 1) {
      bool hasUpdate = false;

    //   //  reset
    //   if (!rst->empty() && rst->getBag().back() == 1) {
    //     s.regs.fill(std::string(13,'0'));
    //     hasUpdate = true;
    //   } else {
    //     // write-back if enabled
        // if (!reg_wr_vld->empty() && reg_wr_vld->getBag().back() == 1
        //     && !result->empty() && !dst->empty())
        // {
        //   int d = std::stoi(dst->getBag().back(), nullptr, 2);
        //   if (d >= 0 && d < 8) {
        //     s.regs[d] = result->getBag().back();
        //     std::cout << "[reg_unit] write R" << d 
        //               << " = " << s.regs[d] << std::endl;
        //     hasUpdate = true;
        //   }
        // }
      if(!dst->empty()){
        s.dst = dst->getBag().back();
        std::cout << "[r_unit]: destination register: " << s.dst << std::endl; 
        if(!result->empty()){
            s.dataIn = result->getBag().back();
            std::cout << "[r_unit]: dataIn is: " << s.dataIn << std::endl;
            int x = std::stoi(s.dst, nullptr, 2);
            s.regs[x] = s.dataIn;
            std::cout << "[r_unit]: registers: " << s.regs[x] << std::endl;
        }
        s.stData = "";
        s.stAddr = "";
        //std::cout << "[r_unit]: registers: " << s.regs[s.dst] << std::endl;
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

      if(!ldData->empty()){
        s.ldData = ldData->getBag().back();
        if(!ldReg->empty()){
            s.ldReg = ldReg->getBag().back();
            std::cout << "[r_unit]: data loaded is: " << s.ldData << std::endl;
            int x = std::stoi(s.ldReg, nullptr, 2);
            s.regs[x] = s.ldData;
            std::cout << "[r_unit]: registers: " << s.regs[x] << std::endl;
        }
        //s.readA = "";
        hasUpdate = true;
      }

      if(!stAddrIn->empty()){
        s.stAddr = stAddrIn->getBag().back();
        if(!stReg->empty()){
            s.stReg = stReg->getBag().back();
            s.stData = s.regs[std::stoi(s.stReg, nullptr, 2)];
        }
        //s.readA = "";
        hasUpdate = true;
      }

      // only send output if something actually changed
      s.sigma = hasUpdate
              ? 0.01
              : std::numeric_limits<double>::infinity();
    //}
  }

  // Output the two source operands
  void output(const registerUnit_State& s) const override {
    if(s.readA != 0){
        oprnd_a->addMessage(s.regs[s.readA]);
        oprnd_b->addMessage(s.regs[s.readB]);
        std::cout << "[reg_unit] out A=" << s.regs[s.readA] 
              << "  B=" << s.regs[s.readB] << std::endl;
    } if(!s.stAddr.empty()){
        stAddrOut->addMessage(s.stAddr);
    } if(!s.stData.empty()){
        stData->addMessage(s.stData);
    }
  }

  [[nodiscard]] double timeAdvance(const registerUnit_State& s) const override {
    return s.sigma;
  }
};

#endif // REGISTER_UNIT_HPP