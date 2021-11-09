//* -------- ------- ------ ----- Tasks ----- ------ ------- --------

#include <iostream>

#include "../include/Assembler.h"

int main(int argc, char const* argv[])
{
  Assembler myAssembler;
  if (argv[1]) myAssembler.assemble(argv[1]);
  else myAssembler.assemble("x.asm");
}