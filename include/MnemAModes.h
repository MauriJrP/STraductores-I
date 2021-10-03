#pragma once

#include <iostream>
#include <cstring>
#include <sstream>
#include <unordered_map>

#include "./NSystems.h"

class MnemAModes
{
private:
  std::string Mnemonic;
  std::unordered_map<std::string, std::string> addressModes; // addressMode | objectCode
  NSystems nSystems;

public:
  MnemAModes();
  MnemAModes(std::string, std::unordered_map<std::string, std::string>);
  ~MnemAModes();

  std::string getHexOpr(std::string);

  std::string getAddressMode();
  std::string getAddressMode(std::string);
  int getInstructionLenght(std::string am = "");
  std::string getMnenonicName();
  std::string getObjectCode(std::string);

  void setMnemonicName(std::string);

  void addAddressMode(std::string, std::string);

  void clear();

};
