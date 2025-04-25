#ifndef PIPELINE_TOP_HPP
#define PIPELINE_TOP_HPP

#include "cadmium/modeling/devs/coupled.hpp"
#include "clock.hpp"
#include "reset.hpp"
#include "InstrUnitGen.hpp"
#include "instruction_unit.hpp"
#include "decode_unit.hpp"
#include "execution_unit.hpp"
#include "register_unit.hpp"

using namespace cadmium;

struct PipelineTop : public Coupled {
    PipelineTop(const std::string &id)
      : Coupled(id)
    {
        // 1) instantiate all atomics
        //auto clkGen = addComponent<clockUnit>(      "CLK_GEN");
        //auto rstGen = addComponent<resetUnit>(      "RST_GEN");
        auto iGen   = addComponent<InstrUnitGen>("I_GEN");
        auto iu     = addComponent<instructionUnit>("I_UNIT");
        auto du     = addComponent<decodeUnit>("D_UNIT");


        // 2) broadcast clock
        //addCoupling(clkGen->clk, iGen->clk);
        addCoupling(iGen->clk, iu->clk);
        addCoupling(iGen->clk, du->clk);
        // addCoupling(iGen->clk, eu->clk);
        // addCoupling(iGen->clk, ru->clk);

        // 3) broadcast reset
        //addCoupling(rstGen->rst, iGen->rst);
        addCoupling(iGen->rst, iu->rst);
        addCoupling(iGen->rst, du->rst);
        // addCoupling(iGen->rst, eu->rst);
        // addCoupling(iGen->rst, ru->rst);

        // 4) drive the Instruction Unit from the generator
        addCoupling(iGen->instruction, iu->instruction);

        // 5) drive Decode from the Instruction Unit
        addCoupling(iu->ir, du->instruction);

        // 6) Decode → Execute (control & dest)
        // addCoupling(du->opcode,      eu->opcode);
        // addCoupling(du->dataMemAddr, eu->dataMemAddr);
        // addCoupling(du->dest,        eu->destAddr);
        // addCoupling(du->reg_wr_vld,  eu->reg_wr_vld);

        //    Decode → RegisterFile (operand and destination addresses)
        // addCoupling(du->opnda, ru->opnda_addr);
        // addCoupling(du->opndb, ru->opndb_addr);
        // after you decode the dest field:
        // addCoupling(du->dest, ru->dst);


        //    RegisterFile → Execute (operand values)
        // addCoupling(ru->oprnd_a, eu->opnda);
        // addCoupling(ru->oprnd_b, eu->opndb);

        //    Execute → RegisterFile (write-back)
        // addCoupling(eu->destReg,      ru->result);
        // addCoupling(eu->reg_wr_vld_out, ru->reg_wr_vld);
    }
};

#endif // PIPELINE_TOP_HPP
