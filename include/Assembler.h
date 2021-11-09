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
  int address;
  MnemAModes mnemonicBuffer = MnemAModes();
  FileManager myFileManager;
  NSystems nSystems;
  std::unordered_set<char> lowercaseLetters;

  std::unordered_map < std::string, MnemAModes > mnemonics; //Source Form | Address Modes
  std::unordered_set < std::string > directives;
  std::unordered_map <std::string, int > labels;

  enum machineState {
    normal,
    text,
    endOfLine
  };

  void clasifyText(bool&, bool&, std::string&, std::string&, std::string&, std::string&, std::string&);
  void resetValues(bool&, bool&, std::string&, std::string&, std::string&, std::string&, std::string&, std::string&, int&);

  std::string dcbDirective(std::string&);
  std::string dcwDirective(std::string&);
  std::string bszDirective(std::string&);
  std::string fccDirective(std::string&);
  std::string fillDirective(std::string&);


  std::string parseObjectCode(std::string);
  void loadMnemonics();

  std::string calculateObjectCode(std::string, std::string, std::string, std::string);

  void firstStage(std::ifstream&, std::fstream&, std::fstream&);
  void secondStage(std::ifstream&, std::fstream&);


public:
  Assembler();
  ~Assembler();

  void assemble(std::string); //Generate object code

};


