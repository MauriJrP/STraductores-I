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

// function that converts a decimal number to binary in complement 2

// function that gets two's complement
std::string NSystems::decToBinComplement2(std::string num) {
  stringstream binNumBuffer{ "" };
  binNumBuffer << setfill('0') << setw(5) << decToBin(num);
  string binNum = binNumBuffer.str();
  cout << "binNum: " << binNum << endl;
  bool flag = false;
  int length = binNum.length() - 1;
  for (int i = length; i >= 0; i--) {
    if (!flag) {
      if (binNum[i] == '0') continue;
      else flag = true;
    }
    else {
      if (binNum[i] == '0') binNum[i] = '1';
      else binNum[i] = '0';
    }
  }
  return binNum;
}


std::string NSystems::decToBin(std::string num)
{
  int intNum = stoi(num);
  string binNum = "";
  while (intNum > 0)
  {
    binNum = to_string(intNum % 2) + binNum;
    intNum /= 2;
  }
  return binNum;
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

// function that made a substraction of two numbers in hexadecimal
// std::string NSystems::subHex(std::string n1, std::string n2)
// {
//   int n1_dec = hexToDec(n1);
//   int n2_dec = hexToDec(n2);
//   int result = n1_dec - n2_dec;
//   return decToHex(to_string(result));
// }

