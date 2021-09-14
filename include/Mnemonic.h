#pragma once

#include <iostream>
#include <cstring>
#include <sstream>
#include <unordered_map>

// using namespace std;

class Mnemonic
{
private:
  // std::string sourceForm;
  std::unordered_map<std::string, std::string> addressModes; // addressMode | objectCode
public:
  Mnemonic();
  Mnemonic(std::unordered_map<std::string, std::string>);
  ~Mnemonic();

  std::string octToHex(std::string);
  std::string binToHex(std::string);
  std::string decToHex(std::string);

  std::string getHexOpr(std::string);

  std::string getAddressMode(std::string);
  int getInstructionLenght(std::string am = "");

  void addAddressMode(std::string, std::string);

  void clear();

};
