#ifndef MAIN_MEMORY_HPP
#define MAIN_MEMORY_HPP

#include "cadmium/modeling/devs/coupled.hpp"
#include "instruction_memory.hpp"
#include "data_memory.hpp"


using namespace cadmium;

struct main_memory : public Coupled{
    Port<int> clk;
    Port<int> rst;

    Port<int> pc;
    Port<std::string> instruction;
    
    Port<std::string> dataMemAddrIn;
    Port<std::string> dataMemAddrOut;
    Port<std::string> dataIn;
    Port<std::string> dataOut;

    main_memory(const std::string& id) : Coupled(id){
        auto instrMem = addComponent<instructionMemory>("instructionMemory");
        auto dataMem = addComponent<dataMemory>("dataMemory");

        pc = addInPort<int>("pc");
        instruction = addOutPort<std::string>("instruction");

        dataMemAddrIn = addInPort<std::string>("dataMemAddrIn");
        dataMemAddrOut = addOutPort<std::string>("dataMemAddrOut");
        dataIn = addInPort<std::string>("dataIn");
        dataOut = addOutPort<std::string>("dataOut");

        clk = addInPort<int>("clk");
        rst = addInPort<int>("rst");

        addCoupling(this->clk, instrMem->clk);
        addCoupling(this->clk,   dataMem->clk);

        addCoupling(this->rst,   dataMem->rst);


        addCoupling(this->pc, instrMem->pc);
        addCoupling(instrMem->instruction, this->instruction);

        addCoupling(this->dataMemAddrIn, dataMem->dataMemAddrIn);
        addCoupling(this->dataMemAddrOut, dataMem->dataMemAddrOut);
        addCoupling(this->dataIn, dataMem->dataIn);
        addCoupling(dataMem->dataOut, this->dataOut);


    }
};
#endif
