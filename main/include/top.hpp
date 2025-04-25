#ifndef SAMPLE_TOP_HPP
#define SAMPLE_TOP_HPP

#include "cadmium/modeling/devs/coupled.hpp"
#include "initialize.hpp"
#include "clock.hpp"
#include "instruction_memory.hpp"
#include "instruction_unit.hpp"
#include "decode_unit.hpp"
#include "InstrUnitGen.hpp"
#include "RegUnitGen.hpp"
#include "register_unit.hpp"
#include "execution_unit.hpp"
#include "ExecGen.hpp"

using namespace cadmium;

struct top_coupled : public Coupled {
    top_coupled(const std::string& id) : Coupled(id) {
        //auto init = addComponent<initialize>("init");
        //auto clock = addComponent<clockUnit>("clock");
        //auto i_mem = addComponent<instructionMemory>("i_mem");
        //auto i_unit = addComponent<instructionUnit>("i_unit");
        auto d_unit = addComponent<decodeUnit>("d_unit");
        //auto exec_unit = addComponent<executionUnit>("exec_unit");
        //auto reg_unit = addComponent<registerUnit>("reg_unit");
        auto instrUnit_gen = addComponent<InstrUnitGen>("instrUnit_gen");
        //auto regUnit_gen = addComponent<RegisterUnitGen>("regUnit_gen");
        //auto exec_gen = addComponent<ExecUnitGen>("exec_gen");
        


        //addCoupling(clock->clk, i_unit->clk);
        //addCoupling(clock->clk, d_unit->clk);


        //addCoupling(init->pc_init, i_mem->init);
        // addCoupling(i_mem->instruction, i_unit->instruction);
        // addCoupling(i_unit->pc, i_mem->pc);
        // addCoupling(i_unit->ir, d_unit->instruction);
        
        //Test Instruction Unit
        // addCoupling(instrUnit_gen->clk, i_unit->clk);
        // addCoupling(instrUnit_gen->rst, i_unit->rst);
        // addCoupling(instrUnit_gen->instruction, i_unit->instruction);

        //Test Decode Unit
        addCoupling(instrUnit_gen->clk, d_unit->clk);
        addCoupling(instrUnit_gen->rst, d_unit->rst);
        addCoupling(instrUnit_gen->instruction, d_unit->instruction);

        //Test RegFile
        // addCoupling(regUnit_gen->clk , reg_unit->clk);
        // addCoupling(regUnit_gen->rst , reg_unit->rst);
        // addCoupling(regUnit_gen->reg_wr_vld , reg_unit->reg_wr_vld);
        // addCoupling(regUnit_gen->result , reg_unit->result);
        // addCoupling(regUnit_gen->dst , reg_unit->dst);
        // addCoupling(regUnit_gen->opnda_addr , reg_unit->opnda_addr);
        // addCoupling(regUnit_gen->opndb_addr , reg_unit->opndb_addr);

        //Test execution unit
        // addCoupling(exec_gen->clk , exec_unit->clk);
        // addCoupling(exec_gen->rst , exec_unit->rst);
        // addCoupling(exec_gen->opcode , exec_unit->opcode);
        // addCoupling(exec_gen->dataMemAddr , exec_unit->dataMemAddr);
        // addCoupling(exec_gen->opnda , exec_unit->opnda);
        // addCoupling(exec_gen->opndb , exec_unit->opndb);
        // addCoupling(exec_gen->destAddr , exec_unit->destAddr);
        // addCoupling(exec_gen->reg_wr_vld, exec_unit->reg_wr_vld);


    }
};

#endif