#include "../include/Assembler.h"

using namespace std;

Assembler::Assembler() : address(BEGIN)
{
  directives.insert("ORG");
  directives.insert("END");
  directives.insert("START");
  directives.insert("EQU");
  directives.insert("DC.B");
  directives.insert("BSZ");
  directives.insert("ZMB");
  directives.insert("FILL");
  directives.insert("DC.W");
  directives.insert("FCC");
  directives.insert("FCB");
}

Assembler::~Assembler()
{
}

std::string Assembler::parseObjectCode(std::string objectCode)
{
  string stringBuffer = "";
  for (int i = 0; i < objectCode.length(); i++) {
    stringBuffer += objectCode[i];
    if (i % 2 != 0 && i != 0 && i != objectCode.length() - 1) { //odd, i != 0 and isn't last
      stringBuffer += " ";
    }
  }
  return stringBuffer;
}

void Assembler::loadMnemonics()
{
  MnemAModes mnemAModes = MnemAModes();
  int pos1, pos2; //comma positions
  string sourceForm, addressMode, objectCode;
  string stringBuffer;

  ifstream mFile;
  mFile.open("mnemonicsDB.csv", ios::in);
  getline(mFile, stringBuffer); // Titles

  getline(mFile, stringBuffer);
  while (!mFile.eof()) {
    pos1 = stringBuffer.find(',');
    pos2 = stringBuffer.find_last_of(',');
    sourceForm = stringBuffer.substr(0, pos1);
    addressMode = stringBuffer.substr(pos1 + 1, pos2 - pos1 - 1);
    objectCode = parseObjectCode(stringBuffer.substr(pos2 + 1));

    if (sourceForm != mnemAModes.getMnenonicName()) {
      mnemAModes.clear();
      mnemAModes.setMnemonicName(sourceForm);
    }
    mnemAModes.addAddressMode(addressMode, objectCode);
    mnemonics[sourceForm] = mnemAModes;


    getline(mFile, stringBuffer); // Titles

  }
}

void Assembler::clasifyText(bool& operandFlag, bool& directiveFlag, std::string& stringBuffer, std::string& operand, std::string& sourceForm, std::string& directive, std::string& label)
{

  if (operandFlag) operand = stringBuffer;
  else if (directiveFlag) operand = stringBuffer;
  else if (mnemonics.count(stringBuffer)) {
    sourceForm = stringBuffer;
    if (mnemonics[sourceForm].getAddressMode() != "INH") operandFlag = true;
  }
  else if (directives.count(stringBuffer)) {
    directive = stringBuffer;
    if (directive == "ORG" || directive == "EQU" || directive == "DC.B" || directive == "DC.W" || directive == "BSZ" || directive == "ZMB" || directive == "FCB" || directive == "FCC" || directive == "FILL") directiveFlag = true;
  }
  else {
    label = stringBuffer;
    // if ()
  }
  stringBuffer = "";
}

void Assembler::resetValues(bool& operandFlag, bool& directiveFlag, std::string& sourceForm, std::string& operand, std::string& directive, std::string& label, std::string& labelValue)
{
  operandFlag = false;
  directiveFlag = false;
  sourceForm = "";
  operand = "";
  directive = "";
  label = "";
  labelValue = "";

}

void Assembler::assemble(string iFileName)
{
  //* -------- ------- ------ ----- Vars ----- ------ ------- --------
  ifstream iFile;
  fstream file;
  fstream tabsimFile;
  string fileName{ "" };
  string stringBuffer{ "" };
  string sourceForm{ "" };
  string operand{ "" };
  string addressMode{ "" };
  string directive{ "" };
  string label{ "" };
  string labelValue{ "" };
  string hexOpr{ "" };
  char character{ '\0' };
  int instructionLength{ 0 };
  int intBuffer{ 0 };
  bool operandFlag{ false };
  bool directiveFlag{ false };
  machineState state{ machineState::normal };

  //* -------- ------- ------ ----- Open files ----- ------ ------- --------
  loadMnemonics();
  iFile.open(iFileName, ios::in);
  if (!iFile.is_open()) throw std::invalid_argument("File doesn't exist");

  fileName = iFileName.substr(0, iFileName.find(".")) + ".lst";
  myFileManager.createFile(fileName);
  file.open(fileName, ios::in | ios::out);

  fileName = iFileName.substr(0, iFileName.find(".")) + ".tabsim";
  myFileManager.createFile(fileName);
  tabsimFile.open(fileName, ios::in | ios::out);

  //* -------- ------- ------ ----- Finite State Machine ----- ------ ------- --------
  character = iFile.get();
  while (!iFile.eof()) {
    if (character == '\n') state = machineState::endOfLine;
    if (character == ';') {
      getline(iFile, stringBuffer);
      stringBuffer = "";
      state = machineState::endOfLine;
    }

    switch (state) {
    case machineState::normal:
      if (character != ' ') {
        state = machineState::text;
        stringBuffer += character;
      }
      break;
    case machineState::text:
      if (character == ' ') {
        state = machineState::normal;
        clasifyText(operandFlag, directiveFlag, stringBuffer, operand, sourceForm, directive, label);
      }
      else stringBuffer += character;

      break;
    case machineState::endOfLine:
      if (stringBuffer != "") {
        state = machineState::normal;
        clasifyText(operandFlag, directiveFlag, stringBuffer, operand, sourceForm, directive, label);
      }

      //* -------- ------- ------ ----- Write To TABSIM file ----- ------ ------- --------
      if (directive == "EQU") labelValue = operand;
      else labelValue = to_string(address);
      if (label != "") tabsimFile << label << " $" << hex << stoi(labelValue) << '\n';
      // if (label != "") tabsimFile << label << ": " << labelValue << "\n";

      //* -------- ------- ------ ----- Write to lst file ----- ------ ------- --------
      cout << hex << address << " ";
      if (directive != "") {
        if (label != "") cout << label << " ";
        cout << directive << " " << operand;
      }
      else if (sourceForm != "")
        cout << mnemonics[sourceForm].getObjectCode(mnemonics[sourceForm].getAddressMode(operand)) << " " << sourceForm << " " << operand;
      cout << "\n";

      if (sourceForm != "") {
        if (operand == "") instructionLength = mnemonics[sourceForm].getInstructionLenght();
        else {
          hexOpr = mnemonics[sourceForm].getHexOpr(operand);
          addressMode = mnemonics[sourceForm].getAddressMode(hexOpr);
          instructionLength = mnemonics[sourceForm].getInstructionLenght(addressMode);
        }
        address += instructionLength;
      }

      //* -------- ------- ------ ----- Directives ----- ------ ------- --------
      if (directive == "ORG") address = nSystems.hexToDec(operand.substr(1));
      else if (directive == "END") return;
      else if (directive == "START") address = 0;
      // else if (directive == "EQU") labelValue = operand;

      resetValues(operandFlag, directiveFlag, sourceForm, operand, directive, label, labelValue);
      break;

    default:
      break;
    }
    character = iFile.get();
  }

  file.close();
  tabsimFile.close();
  iFile.close();
}



