#pragma once

#include <iostream>
#include <cstring>
#include <sstream>
#include <unordered_map>

#include "./NSystems.h"

class Mnemonic
{
  private:
  std::unordered_map<std::string, std::string> addressModes; // addressMode | objectCode
  NSystems nSystems;

  public:
  Mnemonic();
  Mnemonic(std::unordered_map<std::string, std::string>);
  ~Mnemonic();

  std::string getHexOpr(std::string);

  std::string getAddressMode(std::string);
  int getInstructionLenght(std::string am = "");

  void addAddressMode(std::string, std::string);

  void clear();

};
