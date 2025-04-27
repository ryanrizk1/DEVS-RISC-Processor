#ifndef RISC_Processor
#define RISC_Processor

#include "cadmium/modeling/devs/coupled.hpp"
#include "instruction_memory.hpp"
#include "instruction_unit.hpp"
#include "decode_unit.hpp"
#include "execution_unit.hpp"
#include "register_unit.hpp"
#include "data_memory.hpp"

using namespace cadmium;

class risc_processor : public Coupled{
public:
    Port<std::string> instrIn;    // 13-bit instruction input
    Port<std::string> aluResult;  // ALU result output
    Port<std::string> writeReg;   // Destination register index output
    Port<int> pc;

    //ports to/from memory
    Port<std::string> dataMemAddrLd; // address to load from
    Port<std::string> ldData;     // loaded data from data memory
    Port<std::string> ldRegOut;      // destination register index for ld
    Port<std::string> ldRegIn;
    Port<std::string> stAddrOut;  // store address to data memory
    Port<std::string> stData;     // data to be stored in data memory

    risc_processor(const std::string& id) : Coupled(id) {

        instrIn    = addInPort<std::string>("instrIn");
        aluResult  = addOutPort<std::string>("aluResult");
        writeReg   = addOutPort<std::string>("writeReg");
        pc = addOutPort<int>("pc");

        dataMemAddrLd = addOutPort<std::string>("dataMemAddrLd");
        ldData     = addInPort<std::string>("ldData");
        ldRegOut      = addOutPort<std::string>("ldRegOut");
        ldRegIn       = addInPort< std::string>("ldRegIn");

        stAddrOut  = addOutPort<std::string>("stAddrOut");
        stData     = addOutPort<std::string>("stData");


        //auto init = addComponent<initialize>("init");
        //auto clock = addComponent<clockUnit>("clock");
        auto i_unit = addComponent<instructionUnit>("i_unit");
        auto d_unit = addComponent<decodeUnit>("d_unit");
        auto e_unit = addComponent<executionUnit>("e_unit");
        auto r_unit = addComponent<registerUnit>("r_unit");
        //auto d_mem = addComponent<dataMemory>("d_mem");

        //couple iunit and decode unit
        addCoupling(i_unit->ir, d_unit->instruction);

        //couple d_unit and reg file
        addCoupling(d_unit->opnda, r_unit->opnda_addr);
        addCoupling(d_unit->opndb, r_unit->opndb_addr);
        addCoupling(d_unit->stReg, r_unit->stReg);
        addCoupling(d_unit->dataMemAddrSt, r_unit->stAddrIn);
        

        //couple d_unit and exec unit
        addCoupling(d_unit->opcode, e_unit->opcode);
        addCoupling(d_unit->dest, e_unit->destAddr);

        //couple reg_file and exec unit
        addCoupling(r_unit->oprnd_a ,e_unit->opnda);
        addCoupling(r_unit->oprnd_b ,e_unit->opndb);
        addCoupling(e_unit->result,r_unit->result);
        addCoupling(e_unit->destReg,r_unit->dst);



        //external coupling
        addCoupling(this->instrIn, i_unit->instruction);
        addCoupling(e_unit->result, this->aluResult);
        addCoupling(e_unit->destReg, this->writeReg);
        addCoupling(i_unit->pc, this->pc);
        
        






        // //couple decode with exec
        // addCoupling(d_unit->opcode, e_unit->opcode);
        // addCoupling(d_unit->opnda, r_unit->opnda_addr);
        // addCoupling(d_unit->opndb, r_unit->opndb_addr);
        // addCoupling(d_unit->dest, e_unit->destAddr);
        // //addCoupling(d_unit->ldReg, r_unit->opnda_addr);
        // //ld
        addCoupling(d_unit->dataMemAddrLd, this->dataMemAddrLd);
        addCoupling(d_unit->ldReg, this->ldRegOut);
        addCoupling(this->ldData,r_unit->ldData);
        addCoupling(this->ldRegIn,r_unit->ldReg);

        // addCoupling(d_unit->dataMemAddrLd, d_mem->dataMemAddrOut);
        // addCoupling(d_unit->ldReg, d_mem->ldRegIn);
        // //st
        
        addCoupling(r_unit->stAddrOut, this->stAddrOut);
        addCoupling(r_unit->stData, this->stData);


        // addCoupling(r_unit->oprnd_a, e_unit->opnda);
        // addCoupling(r_unit->oprnd_b, e_unit->opndb);

        // addCoupling(e_unit->result, r_unit->result);
        // addCoupling(e_unit->destReg, r_unit->dst);

        // addCoupling(d_mem->dataOut, r_unit->ldData);
        // addCoupling(d_mem->ldRegOut, r_unit->ldReg);

        // addCoupling(r_unit->stData, d_mem->dataIn);
        // addCoupling(r_unit->stAddrOut, d_mem->dataMemAddrIn);
    }

};

#endif