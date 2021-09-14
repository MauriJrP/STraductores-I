#include <iostream>
#include <fstream>
#include <cstring>
#include <unordered_map>

#include "../include/Mnemonic.h"

using namespace std;

void createFile(string fileName) {
  fstream file;

  file.open(fileName, ios::out);
  if (!file.is_open())
  {
    //Error al abrir el archivo
    file.open(fileName, ios::out);

    if (!file.good())
    {
      //Error al crear el archivo
    }
    else
    {
      file.close();
    }
  }
  file.close();
}

int main(int argc, char const* argv[])
{
  ifstream iFile;
  fstream file;

  string iFileName;
  string fileName;
  string stringBuffer;
  string mnemonicName;
  string mnemonicOpr;
  string addressMode;
  int instructionLength;
  int intBuffer;

  int constexpr begin = 16384; // 16384 = 40
  int address = begin;
  unordered_map < string, Mnemonic > mnemonics; //Source Form | mnemonic

  Mnemonic mnemonicBuffer = Mnemonic();

  //ABA
  mnemonicBuffer.addAddressMode("INH", "18 06");
  mnemonics["ABA"] = mnemonicBuffer;
  mnemonicBuffer.clear();

  //ABX
  mnemonicBuffer.addAddressMode("IDX", "1A E5");
  mnemonics["ABX"] = mnemonicBuffer;
  mnemonicBuffer.clear();

  //ABY
  mnemonicBuffer.addAddressMode("IDX", "19 ED");
  mnemonics["ABY"] = mnemonicBuffer;
  mnemonicBuffer.clear();

  //ADCA
  mnemonicBuffer.addAddressMode("IMM", "89 ii");
  mnemonicBuffer.addAddressMode("DIR", "99 dd");
  mnemonicBuffer.addAddressMode("EXT", "B9 hh ll");
  mnemonics["ADCA"] = mnemonicBuffer;
  mnemonicBuffer.clear();

  //ADCB
  mnemonicBuffer.addAddressMode("IMM", "C9 ii");
  mnemonicBuffer.addAddressMode("DIR", "D9 dd");
  mnemonicBuffer.addAddressMode("EXT", "F9 hh ll");
  mnemonics["ADCB"] = mnemonicBuffer;
  mnemonicBuffer.clear();

  //ADDA
  mnemonicBuffer.addAddressMode("IMM", "8B ii");
  mnemonicBuffer.addAddressMode("DIR", "9B dd");
  mnemonicBuffer.addAddressMode("EXT", "BB hh ll");
  mnemonics["ADDA"] = mnemonicBuffer;
  mnemonicBuffer.clear();

  //ADDB
  mnemonicBuffer.addAddressMode("IMM", "CB ii");
  mnemonicBuffer.addAddressMode("DIR", "DB dd");
  mnemonicBuffer.addAddressMode("EXT", "FB hh ll");
  mnemonics["ADDB"] = mnemonicBuffer;
  mnemonicBuffer.clear();

  //ADDD
  mnemonicBuffer.addAddressMode("IMM", "C3 jj kk");
  mnemonicBuffer.addAddressMode("DIR", "D3 dd");
  mnemonicBuffer.addAddressMode("EXT", "F3 hh ll");
  mnemonics["ADDD"] = mnemonicBuffer;
  mnemonicBuffer.clear();

  //ANDA
  mnemonicBuffer.addAddressMode("IMM", "84 ii");
  mnemonicBuffer.addAddressMode("DIR", "94 dd");
  mnemonicBuffer.addAddressMode("EXT", "B4 hh ll");
  mnemonics["ANDA"] = mnemonicBuffer;
  mnemonicBuffer.clear();

  //ANDB
  mnemonicBuffer.addAddressMode("IMM", "C4 ii");
  mnemonicBuffer.addAddressMode("DIR", "D4 ii");
  mnemonicBuffer.addAddressMode("EXT", "F4 hh ll");
  mnemonics["ANDB"] = mnemonicBuffer;
  mnemonicBuffer.clear();

  // //ABX
  // mnemonicBuffer.addAddressMode("", "");
  // mnemonics["ABX"] = mnemonicBuffer;
  // mnemonicBuffer.clear();



  do {
    cout << "Nombre del archivo: ";
    cin >> iFileName;
    iFile.open(iFileName, ios::in);
  } while (!iFile.is_open());
  // iFileName = "x.asm";
  // iFile.open(iFileName, ios::in);


  fileName = iFileName.substr(0, iFileName.find(".")) + ".lst";
  createFile(fileName);
  file.open(fileName, ios::in | ios::out);

  getline(iFile, stringBuffer); // ORG instruction
  file << stringBuffer << '\n';

  getline(iFile, stringBuffer);
  while (!iFile.eof()) {
    intBuffer = stringBuffer.find(" ");
    if (intBuffer == -1) { // inherent mode
      instructionLength = mnemonics[stringBuffer].getInstructionLenght();
      file << hex << address << " " << stringBuffer << " INH (LI=" << instructionLength << ")\n";
    }
    else { // any other mode
      mnemonicName = stringBuffer.substr(0, intBuffer);
      mnemonicOpr = stringBuffer.substr(intBuffer + 1);
      string hexOpr = mnemonics[mnemonicName].getHexOpr(mnemonicOpr);
      addressMode = mnemonics[mnemonicName].getAddressMode(hexOpr);
      // cout << "address: " << addressMode << hexOpr << endl;
      if (addressMode != "Error")
        instructionLength = mnemonics[mnemonicName].getInstructionLenght(addressMode);
      else instructionLength = mnemonics[mnemonicName].getInstructionLenght();
      file << hex << address << " " << stringBuffer << " " << addressMode << " (LI=" << instructionLength << ")\n";
    }
    address += instructionLength;

    getline(iFile, stringBuffer);
  }

  file << hex << address + 1 << " " << stringBuffer; // END instruction

  file.close();
  iFile.close();

}