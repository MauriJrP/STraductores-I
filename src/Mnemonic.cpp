#include "../include/Mnemonic.h"

using namespace std;

Mnemonic::Mnemonic()
{
}

Mnemonic::Mnemonic(std::unordered_map<std::string, std::string> am) : addressModes(am)
{}

Mnemonic::~Mnemonic()
{
}

std::string Mnemonic::getHexOpr(std::string opr)
{
  string imm = "";
  if (opr[0] == '#') {
    opr = opr.substr(1);
    imm = "#";
  }

  if (opr[0] == '$') return imm + opr;
  else if (opr[0] == '@') opr = '$' + nSystems.octToHex(opr);
  else if (opr[0] == '%') opr = '$' + nSystems.binToHex(opr);
  else opr = '$' + nSystems.decToHex(opr);

  return imm + opr;
}

std::string Mnemonic::getAddressMode(std::string opr)
{
  if (opr[0] == '#') return "IMM";

  //$ and 2 digits
  if (opr.length() <= 3 && addressModes.find("DIR") != addressModes.end()) return "DIR";
  else if (opr.length() <= 5 && addressModes.find("EXT") != addressModes.end()) return "EXT";
  else return addressModes.begin()->first;
  // else return "Error";
}

int Mnemonic::getInstructionLenght(string am)
{
  int count{ 1 };
  string objectCode = (am == "") ? addressModes.begin()->second : addressModes[am];

  for (int i = 0; i < objectCode.length(); i++) {
    if (objectCode[i] == ' ') ++count;
  }

  return count;
}

void Mnemonic::addAddressMode(std::string am, std::string oc)
{
  addressModes[am] = oc;
}

void Mnemonic::clear()
{
  addressModes.clear();
}