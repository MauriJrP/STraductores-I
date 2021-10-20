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
  // if (num[0] == '#') num = num.substr(1);
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

std::string NSystems::getHex(std::string num)
{
  if (num[0] == '$') return num.substr(1);
  else if (num[0] == '@') return octToHex(num.substr(1));
  else if (num[0] == '%') return binToHex(num.substr(1));
  else return decToHex(num);
}

int NSystems::hexToDec(std::string n)
{
  return stoi(n, 0, 16);
}

