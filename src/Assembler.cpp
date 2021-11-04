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
  loadMnemonics();
}

Assembler::~Assembler()
{
}

std::string Assembler::parseObjectCode(std::string objectCode) // object code from mnemonicsDB.csv
{
  string strBuffer = "";
  for (int i = 0; i < objectCode.length(); i++) {
    strBuffer += objectCode[i];
    if (i % 2 != 0 && i != 0 && i != objectCode.length() - 1) { //odd, i != 0 and isn't last
      strBuffer += " ";
    }
  }
  return strBuffer;
}

void Assembler::loadMnemonics()
{
  MnemAModes mnemAModes = MnemAModes();
  int pos1, pos2; //comma positions
  string sourceForm, addressMode, objectCode;
  string strBuffer;

  ifstream mFile;
  mFile.open("mnemonicsDB.csv", ios::in);
  getline(mFile, strBuffer); // Titles

  getline(mFile, strBuffer);
  while (!mFile.eof()) {
    pos1 = strBuffer.find(',');
    pos2 = strBuffer.find_last_of(',');
    sourceForm = strBuffer.substr(0, pos1);
    addressMode = strBuffer.substr(pos1 + 1, pos2 - pos1 - 1);
    objectCode = parseObjectCode(strBuffer.substr(pos2 + 1));

    if (sourceForm != mnemAModes.getMnenonicName()) {
      mnemAModes.clear();
      mnemAModes.setMnemonicName(sourceForm);
    }
    mnemAModes.addAddressMode(addressMode, objectCode);
    mnemonics[sourceForm] = mnemAModes;

    getline(mFile, strBuffer); // Titles

  }
}

void Assembler::clasifyText(bool& operandFlag, bool& directiveFlag, std::string& strBuffer, std::string& operand, std::string& sourceForm, std::string& directive, std::string& label)
{
  if (operandFlag) operand = strBuffer;
  else if (directiveFlag) operand = strBuffer;
  else if (mnemonics.count(strBuffer)) {
    sourceForm = strBuffer;
    if (mnemonics[sourceForm].getAddressMode() != "INH") operandFlag = true;
  }
  else if (directives.count(strBuffer)) {
    directive = strBuffer;
    if (directive == "ORG" || directive == "EQU" || directive == "DC.B" || directive == "DC.W" || directive == "BSZ" || directive == "ZMB" || directive == "FCB" || directive == "FCC" || directive == "FILL") directiveFlag = true;
  }
  else label = strBuffer;
  strBuffer = "";
}

void Assembler::resetValues(bool& operandFlag, bool& directiveFlag, std::string& sourceForm, std::string& operand, std::string& directive, std::string& label, std::string& labelValue, std::string& operationCode, int& instructionLength)
{
  operandFlag = false;
  directiveFlag = false;
  sourceForm = "";
  operand = "";
  directive = "";
  label = "";
  labelValue = "";
  operationCode = "";
  instructionLength = 0;
}

std::string Assembler::dcbDirective(std::string& operand)
{
  stringstream operationCode{ "" };
  int bytes{ 0 };
  string byte{ "" };

  if (operand.length() == 0) {
    operationCode << "00 ";
    bytes = 1;
  }

  for (int i = 0; i < operand.length(); i++) {
    if (operand[i] == ',' || i == operand.length() - 1) {
      if (operand[i] != ',') byte += operand[i];
      if (byte[0] == '\'') byte = to_string((int)byte[1]);
      operationCode << setfill('0') << setw(2) << hex << stoi(byte) << " ";
      byte = "";
      ++bytes;
    }
    else byte += operand[i];
  }
  address += bytes;
  return operationCode.str();
}

std::string Assembler::dcwDirective(std::string& operand)
{
  stringstream operationCode{ "" };
  stringstream ssBuffer{ "" };
  int bytes{ 0 };
  string byte{ "" };

  if (operand.length() == 0) {
    operationCode << "00 00 ";
    bytes = 2;
  }

  for (int i = 0; i < operand.length(); i++) {
    if (operand[i] == ',' || i == operand.length() - 1) {
      if (operand[i] != ',') byte += operand[i];
      if (byte[0] == '\'') byte = to_string((int)byte[1]);
      ssBuffer.str(string());
      ssBuffer << setfill('0') << setw(4) << hex << stoi(byte);
      for (int j = 1; j <= ssBuffer.str().length(); j++) {
        operationCode << ssBuffer.str()[j - 1];
        if (j % 2 == 0)
          operationCode << " ";
      }
      byte = "";
      bytes += 2;
    }
    else byte += operand[i];
  }
  address += bytes;
  return operationCode.str();
}

std::string Assembler::bszDirective(std::string& operand)
{
  std::string operationCode{ "" };
  for (int i = 0; i < stoi(operand); i++) {
    operationCode += "00 ";
  }
  address += stoi(operand);
  return operationCode;
}

std::string Assembler::fccDirective(std::string& operand)
{
  stringstream operationCode{ "" };
  int bytes{ 0 };
  string byte{ "" };
  for (int i = 0; i < operand.length(); i++) {
    if (operand[i] != '/') {
      byte = to_string((int)operand[i]);
      operationCode << setfill('0') << setw(2) << hex << stoi(byte) << " ";
      byte = "";
      ++bytes;
    }
  }
  address += bytes;
  return operationCode.str();
}

std::string Assembler::fillDirective(std::string& operand) {
  stringstream operationCode{ "" };
  size_t pos{ operand.find(',') };
  int bytesToFill{ stoi(operand.substr(pos + 1)) };
  std::string constantValue{ operand.substr(0,pos) };
  for (int i = 0; i < bytesToFill; i++) {
    operationCode << setfill('0') << setw(2) << constantValue << " ";
  }
  address += bytesToFill;
  return operationCode.str();
}

void Assembler::firstStage(ifstream& iFile, fstream& file, fstream& tabsimFile) {
  string strBuffer{ "" };
  string sourceForm{ "" };
  string operand{ "" };
  string addressMode{ "" };
  string operationCode{ "" };
  string directive{ "" };
  string label{ "" };
  string labelValue{ "" };
  string hexOpr{ "" };
  stringstream ssBuffer{ "" };
  char character{ '\0' };
  int instructionLength{ 0 };
  int intBuffer{ 0 };
  bool operandFlag{ false };
  bool directiveFlag{ false };
  machineState state{ machineState::normal };

  //* -------- ------- ------ ----- Finite State Machine ----- ------ ------- --------
  character = iFile.get();
  while (!iFile.eof()) {
    if (character == '\n') state = machineState::endOfLine;
    if (character == ';') {
      getline(iFile, strBuffer);
      strBuffer = "";
      state = machineState::endOfLine;
    }

    switch (state) {
    case machineState::normal:
      if (character != ' ' && character != '\t') {
        state = machineState::text;
        strBuffer += character;
      }
      break;
    case machineState::text:
      if (character == ' ' || character == '\t') {
        state = machineState::normal;
        clasifyText(operandFlag, directiveFlag, strBuffer, operand, sourceForm, directive, label);
      }
      else strBuffer += character;

      break;
    case machineState::endOfLine:
      if (strBuffer != "") {
        state = machineState::normal;
        clasifyText(operandFlag, directiveFlag, strBuffer, operand, sourceForm, directive, label);
      }

      //* -------- ------- ------ ----- Write to lst file ----- ------ ------- --------
      file << setfill('0') << setw(4) << hex << address << " ";
      if (directive != "EQU") {
        ssBuffer << setfill('0') << setw(4) << hex << address;
        labelValue = ssBuffer.str();
        ssBuffer.str("");
      }


      if (directive != "") {
        //* -------- ------- ------ ----- Directives ----- ------ ------- --------
        if (directive == "ORG") address = nSystems.hexToDec(operand.substr(1));
        else if (directive == "START") address = 0;
        else if (directive == "EQU") labelValue = nSystems.getHex(operand);
        else if (directive == "BSZ" || directive == "ZMB") operationCode = bszDirective(operand);
        else if (directive == "DC.B" || directive == "FCB") operationCode = dcbDirective(operand);
        else if (directive == "DC.W") operationCode = dcwDirective(operand);
        else if (directive == "FCC") operationCode = fccDirective(operand);
        else if (directive == "FILL") operationCode = fillDirective(operand);

        file << operationCode;
        if (label != "") file << label << " ";
        file << directive;
        if (operand != "") file << " " << operand;
        file << endl;
      }
      else if (sourceForm != "") {
        file << "{" << mnemonics[sourceForm].getObjectCode(mnemonics[sourceForm].getAddressMode(operand)) << "} ";

        if (label != "") file << label << " ";
        if (operand == "") file << '*' << sourceForm << '\n';
        else file << '*' << sourceForm << " ~" << operand << '\n';

        try {
          hexOpr = mnemonics[sourceForm].getHexOpr(operand);
          addressMode = mnemonics[sourceForm].getAddressMode(hexOpr);
          instructionLength = mnemonics[sourceForm].getInstructionLenght(addressMode);
        }
        catch (const std::exception& e) {// operand == "" || the operand is a label of tabsim
          instructionLength = mnemonics[sourceForm].getInstructionLenght();
        }

        address += instructionLength;
      }

      if (label != "" && labelValue != "") {
        tabsimFile << label << " $" << setfill('0') << setw(4) << labelValue << '\n';
        labels[label] = nSystems.hexToDec(labelValue); //Save label values for second stage
      }
      if (directive == "END") return;
      resetValues(operandFlag, directiveFlag, sourceForm, operand, directive, label, labelValue, operationCode, instructionLength);
      break;
    default:
      break;
    }
    character = iFile.get();
  }
}

void Assembler::secondStage(std::ifstream& iFile, std::fstream& file) {
  string strBuffer{ "" };
  string operationCode{ "" };
  string sourceForm{ "" };
  string operand{ "" };

  getline(iFile, strBuffer);
  while (!iFile.eof()) {
    if (strBuffer.find('{') == -1) { // There are not changes necesaries to this line
      file << strBuffer << '\n';
      getline(iFile, strBuffer);
      continue;
    }

    getline(iFile, strBuffer);
  }
}

void Assembler::assemble(string iFileName)
{
  //* -------- ------- ------ ----- Vars ----- ------ ------- --------
  ifstream iFile;
  fstream file;
  fstream tabsimFile;
  string fileName{ "" };


  //* -------- ------- ------ ----- First Stage ----- ------ ------- --------
  iFile.open(iFileName, ios::in);
  if (!iFile.is_open()) throw std::invalid_argument("File doesn't exist");

  myFileManager.createFile("aux.lst");
  file.open("aux.lst", ios::in | ios::out);

  fileName = iFileName.substr(0, iFileName.find(".")) + ".tabsim";
  myFileManager.createFile(fileName);
  tabsimFile.open(fileName, ios::in | ios::out);

  firstStage(iFile, file, tabsimFile);

  file.close();
  iFile.close();
  tabsimFile.close();
  //* -------- ------- ------ ----- Second Stage ----- ------ ------- --------
  iFile.open("aux.lst", ios::in);

  fileName = iFileName.substr(0, iFileName.find(".")) + ".lst";
  myFileManager.createFile(fileName);
  file.open(fileName, ios::in | ios::out);

  secondStage(iFile, file);

  iFile.close();
  file.close();
  tabsimFile.close();

  // remove("aux.lst");
  // for (auto& i : labels) cout << i.first << ": " << i.second << '\n';

}
