#include "../include/NSystems.h"

using namespace std;

NSystems::NSystems()
{
}

NSystems::~NSystems()
{
}

std::string NSystems::octToHex(std::string num)
{
  int intNum = stoi(num, 0, 8);
  return decToHex(to_string(intNum));
}

std::string NSystems::binToHex(std::string num)
{
  int intNum = stoi(num, 0, 2);
  return decToHex(to_string(intNum));
}

std::string NSystems::decToHex(std::string num)
{
  stringstream result;
  result << hex << stoi(num);
  return result.str();
}

