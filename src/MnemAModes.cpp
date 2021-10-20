#include "../include/MnemAModes.h"

using namespace std;

MnemAModes::MnemAModes() : Mnemonic("")
{
}

MnemAModes::MnemAModes(std::string n, std::unordered_map<std::string, std::string> am) : Mnemonic(n), addressModes(am)
{}

MnemAModes::~MnemAModes()
{
}

std::string MnemAModes::getHexOpr(std::string opr)
{
  string imm = "";
  if (opr[0] == '#') {
    opr = opr.substr(1);
    imm = "#";
  }
  // cout << opr;

  if (opr[0] == '$') return imm + opr;
  else if (opr[0] == '@') opr = '$' + nSystems.octToHex(opr.substr(1));
  else if (opr[0] == '%') opr = '$' + nSystems.binToHex(opr.substr(1));
  else opr = '$' + nSystems.decToHex(opr);

  return imm + opr;
}

std::string MnemAModes::getAddressMode()
{
  return addressModes.begin()->first;
}

std::string MnemAModes::getAddressMode(std::string opr)
{
  if (opr[0] == '#') return "IMM";

  //$ and 2 digits
  if (opr.length() <= 3 && addressModes.find("DIR") != addressModes.end()) return "DIR";
  else if (opr.length() <= 5 && addressModes.find("EXT") != addressModes.end()) return "EXT";
  else return addressModes.begin()->first;
  // else return "Error";
}

int MnemAModes::getInstructionLenght(string am)
{
  int count{ 1 };
  string objectCode = (am == "") ? addressModes.begin()->second : addressModes[am];

  for (int i = 0; i < objectCode.length(); i++) {
    if (objectCode[i] == ' ') ++count;
  }

  return count;
}

std::string MnemAModes::getMnenonicName()
{
  return Mnemonic;
}

std::string MnemAModes::getObjectCode(std::string am)
{
  return addressModes[am];
}

void MnemAModes::setMnemonicName(std::string n)
{
  Mnemonic = n;
}

void MnemAModes::addAddressMode(std::string am, std::string oc)
{
  addressModes[am] = oc;
}

void MnemAModes::clear()
{
  Mnemonic = "";
  addressModes.clear();
}