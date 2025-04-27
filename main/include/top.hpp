#ifndef SAMPLE_TOP_HPP
#define SAMPLE_TOP_HPP

#include "cadmium/modeling/devs/coupled.hpp"
#include "initialize.hpp"
#include "clock.hpp"
#include "main_memory.hpp"
#include "instruction_unit.hpp"
#include "decode_unit.hpp"
#include "execution_unit.hpp"
#include "register_unit.hpp"
#include "risc_processor_gen.hpp"
#include "risc_proc.hpp"

using namespace cadmium;

struct top_coupled : public Coupled {

    top_coupled(const std::string& id) : Coupled(id) {

    //     //auto init = addComponent<initialize>("init");
    //     //auto clock = addComponent<clockUnit>("clock");
    //     auto i_unit = addComponent<instructionUnit>("i_unit");
    //     auto d_unit = addComponent<decodeUnit>("d_unit");
    //     auto e_unit = addComponent<executionUnit>("e_unit");
    //     auto r_unit = addComponent<registerUnit>("r_unit");
    auto risc = addComponent<risc_processor>("risc");
    auto mem = addComponent<main_memory>("mem");

    addCoupling(mem->instruction, risc->instrIn);
    addCoupling(risc->pc, mem->pc);
    // st: proc to mm
    addCoupling(risc->stAddrOut, mem->dataMemAddrIn);
    addCoupling(risc->stData, mem->dataIn);

    //ld
    addCoupling(risc->dataMemAddrLd, mem->dataMemAddrIn);
    addCoupling(risc->ldRegOut, mem->ldRegIn);
    addCoupling(mem->dataOut, risc->ldData);
    addCoupling(mem->ldRegOut,risc->ldRegIn);


    //     //couple iunit and decode unit
    //     addCoupling(i_unit->ir, d_unit->instruction);

    //     //couple d_unit and reg file
    //     addCoupling(d_unit->opnda, r_unit->opnda_addr);
    //     addCoupling(d_unit->opndb, r_unit->opndb_addr);
    //     addCoupling(d_unit->stReg, r_unit->stReg);
    //     addCoupling(d_unit->dataMemAddrSt, r_unit->stAddrIn);
        

    //     //couple d_unit and exec unit
    //     addCoupling(d_unit->opcode, e_unit->opcode);
    //     addCoupling(d_unit->dest, e_unit->destAddr);

    //     //couple reg_file and exec unit
    //     addCoupling(r_unit->oprnd_a ,e_unit->opnda);
    //     addCoupling(r_unit->oprnd_b ,e_unit->opndb);
    //     addCoupling(e_unit->result,r_unit->result);
    //     addCoupling(e_unit->destReg,r_unit->dst);

    //     //couple


        

    //     //couple d_unit with mm


        

    //     addCoupling(d_unit->opcode, e_unit->opcode);
    //     addCoupling(d_unit->opnda, r_unit->opnda_addr);
    //     addCoupling(d_unit->opndb, r_unit->opndb_addr);
    //     addCoupling(d_unit->dest, e_unit->destAddr);
    //     //addCoupling(d_unit->ldReg, r_unit->opnda_addr);
    //     //ld
    //     addCoupling(d_unit->dataMemAddrLd, d_mem->dataMemAddrOut);
    //     addCoupling(d_unit->ldReg, d_mem->ldRegIn);
    //     //st
    //     addCoupling(d_unit->dataMemAddrSt, r_unit->stAddrIn);
    //     addCoupling(d_unit->stReg, r_unit->stReg);

    //     addCoupling(r_unit->oprnd_a, e_unit->opnda);
    //     addCoupling(r_unit->oprnd_b, e_unit->opndb);

    //     addCoupling(e_unit->result, r_unit->result);
    //     addCoupling(e_unit->destReg, r_unit->dst);

    //     addCoupling(d_mem->dataOut, r_unit->ldData);
    //     addCoupling(d_mem->ldRegOut, r_unit->ldReg);

    //     addCoupling(r_unit->stData, d_mem->dataIn);
    //     addCoupling(r_unit->stAddrOut, d_mem->dataMemAddrIn);
    }
};

#endif