#pragma once

#include <iostream>
#include <iomanip> // setfill, setw
#include <fstream>
#include <cstring>
#include <unordered_map>
#include <unordered_set>

#include "../include/MnemAModes.h"
#include "../include/NSystems.h"
#include "../include/FileManager.h"

constexpr int BEGIN = 0;


class Assembler
{
private:

  MnemAModes mnemonicBuffer = MnemAModes();
  FileManager myFileManager;
  NSystems nSystems;

  std::unordered_map < std::string, MnemAModes > mnemonics; //Source Form | Address Modes
  std::unordered_set < std::string > directives;

  enum machineState {
    normal,
    text,
    identification,
    sourceForm,
    operand,
    label,
    directive,
    endOfLine
  };

  void clasifyText(bool&, bool&, std::string&, std::string&, std::string&, std::string&, std::string&);
  void resetValues(bool&, bool&, std::string&, std::string&, std::string&, std::string&, std::string&, std::string&, int&);

  std::string dcbDirective(std::string&);
  std::string dcwDirective(std::string&);
  std::string bszDirective(std::string&);
  std::string fccDirective(std::string&);
  std::string fillDirective(std::string&);

  int address;

  std::string parseObjectCode(std::string);
  void loadMnemonics();

public:
  Assembler();
  ~Assembler();

  void assemble(std::string); //Generate object code

};
