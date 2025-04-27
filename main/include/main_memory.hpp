#ifndef MAIN_MEMORY_HPP
#define MAIN_MEMORY_HPP

#include "cadmium/modeling/devs/coupled.hpp"
#include "instruction_memory.hpp"
#include "data_memory.hpp"

using namespace cadmium;

struct main_memory : public Coupled{
    Port<int> clk;

    Port<int> pc;
    Port<std::string> instruction;
    
    Port<std::string> dataMemAddrIn;
    Port<std::string> dataMemAddrOut;
    Port<std::string> dataIn;
    Port<std::string> dataOut;
    Port<std::string> ldRegIn;
    Port<std::string> ldRegOut;

    main_memory(const std::string& id) : Coupled(id){
        auto instruction_mem = addComponent<instructionMemory>("instructionMemory");
        auto data = addComponent<dataMemory>("dataMemory");

        pc = addInPort<int>("pc");
        instruction = addOutPort<std::string>("instruction");

        dataMemAddrIn = addInPort<std::string>("dataMemAddrIn");
        dataMemAddrOut = addInPort<std::string>("dataMemAddrOut");
        dataIn = addInPort<std::string>("dataIn");
        dataOut = addOutPort<std::string>("dataOut");
        ldRegIn          = addInPort<std::string>("ldRegIn");
        ldRegOut         = addOutPort<std::string>("ldRegOut");



        addCoupling(this->pc, instruction_mem->pc);
        addCoupling(instruction_mem->instruction, this->instruction);

        addCoupling(this->dataMemAddrIn, data->dataMemAddrIn);
        addCoupling(this->dataMemAddrOut, data->dataMemAddrOut);
        addCoupling(this->dataIn, data->dataIn);
        addCoupling(data->dataOut, this->dataOut);

        addCoupling(this->ldRegIn, data->ldRegIn);
        addCoupling(data->ldRegOut, this->ldRegOut);
    }
};
#endif