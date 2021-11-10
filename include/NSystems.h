#pragma once

#include <iostream>
#include <cstring>
#include <sstream>

class NSystems
{
private:

public:
  NSystems();
  ~NSystems();

  std::string octToHex(std::string);
  std::string binToHex(std::string);
  std::string decToBinComplement2(std::string);
  std::string decToHex(std::string);
  std::string getHex(std::string);
  int hexToDec(std::string);


};
