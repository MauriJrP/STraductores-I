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

std::string MnemAModes::getHexOpr(std::string opr) // return the value in hex  with $ prefix
{
  string imm = "";
  if (opr[0] == '#') {
    opr = opr.substr(1);
    imm = "#";
  }

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

  size_t intBuffer{ opr.find(',') };
  string strBuffer{ "" };
  if (intBuffer != -1 && addressModes.find("IDX") != addressModes.end()) {
    strBuffer = opr.substr(0, intBuffer);
    if (strBuffer[0] == '[') return "[IDX2]"; // for now that only there are one with square brackets
    else if (intBuffer == 0 || (stoi(strBuffer) >= -16 && stoi(strBuffer) <= 15)) return "IDX";
    else if ((stoi(strBuffer) >= -256 && stoi(strBuffer) <= 255) || intBuffer == 0) return "IDX1";
    else if ((stoi(strBuffer) >= -32768 && stoi(strBuffer) <= 65535) || intBuffer == 0) return "IDX2";
  }
  else if (opr.length() <= 3 && addressModes.find("DIR") != addressModes.end()) return "DIR";
  else if (opr.length() <= 5 && addressModes.find("EXT") != addressModes.end()) return "EXT";

  return addressModes.begin()->first;
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