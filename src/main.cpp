//* -------- ------- ------ ----- Tasks ----- ------ ------- --------
/*
  Add validation directives for DC.B, DC.W, FCB, DCC
  Add Rel9 validation in step 1
  Separate assembler in 2 steps
  Create step 2 ( add operation code from x.lst file )
*/

#include <iostream>

#include "../include/Assembler.h"

int main(int argc, char const* argv[])
{
  Assembler myAssembler;
  if (argv[1]) myAssembler.assemble(argv[1]);
  else myAssembler.assemble("x.asm");
}