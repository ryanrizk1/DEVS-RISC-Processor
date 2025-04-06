#ifndef SAMPLE_TOP_HPP
#define SAMPLE_TOP_HPP

#include "cadmium/modeling/devs/coupled.hpp"
#include "initialize.hpp"
#include "clock.hpp"
#include "instruction_memory.hpp"
#include "instruction_unit.hpp"
#include "decode_unit.hpp"

using namespace cadmium;

struct top_coupled : public Coupled {
    top_coupled(const std::string& id) : Coupled(id) {
        //auto init = addComponent<initialize>("init");
        auto clock = addComponent<clockUnit>("clock");
        auto i_mem = addComponent<instructionMemory>("i_mem");
        auto i_unit = addComponent<instructionUnit>("i_unit");
        auto d_unit = addComponent<decodeUnit>("d_unit");

        addCoupling(clock->clk, i_unit->clk);
        addCoupling(clock->clk, d_unit->clk);

        //addCoupling(init->pc_init, i_mem->init);
        addCoupling(i_mem->instruction, i_unit->instruction);
        addCoupling(i_unit->pc, i_mem->pc);
        addCoupling(i_unit->ir, d_unit->instruction);
    }
};

#endif