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

    main_memory(const std::string& id) : Coupled(id){
        auto instruction = addComponent<instructionMemory>("instructionMemory");
        auto data = addComponent<dataMemory>("dataMemory");

        pc = addInPort<int>("pc");
        instruction = addOutPort<std::string>("instruction");

        dataMemAddrIn = addInPort<std::string>("dataMemAddrIn");
        dataMemAddrOut = addInPort<std::string>("dataMemAddrOut");
        dataIn = addInPort<std::string>("dataIn");
        dataOut = addOutPort<std::string>("dataOut");

        addCoupling(this->pc, instruction->pc);
        addCoupling(instruction->instruction, this->instruction);

        addCoupling(this->dataMemAddrIn, data->dataMemAddrIn);
        addCoupling(this->dataMemAddrOut, data->dataMemAddrOut);
        addCoupling(this->dataIn, data->dataIn);
        addCoupling(data->dataOut, this->dataOut);
    }
}
