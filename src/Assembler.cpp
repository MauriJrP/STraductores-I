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
  for (char c = 'a'; c <= 'z'; c++)
    lowercaseLetters.insert(c);
  idxRegs["X"] = "00";
  idxRegs["Y"] = "01";
  idxRegs["SP"] = "10";
  idxRegs["PC"] = "11";
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
      operationCode << setfill('0') << setw(2) << hex << uppercase << stoi(byte) << " ";
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
      ssBuffer << setfill('0') << setw(4) << hex << uppercase << stoi(byte);
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
      operationCode << setfill('0') << setw(2) << hex << uppercase << stoi(byte) << " ";
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

std::string Assembler::calculateObjectCode(string objectCode, string sourceForm, string operand, string am, int actualAddress) {
  stringstream result{ "" };
  stringstream ssBuffer{ "" };
  string stringBuffer{ "" };
  string reg{ "" }; // register
  string substract{ "" };
  int decOperand{ 0 }; // operand value in decimal
  int intBuffer{ 0 };

  //* -------- ------- ------ ----- Separate operand in decOperand and register ----- ------ ------- --------
  if (am.find("IDX") == -1) {
    if (operand.find(',') != -1) { // validate if operand has 2 operands
      reg = operand.substr(0, operand.find(','));
      operand = operand.substr(operand.find(',') + 1);
    }

    if (labels.count(operand) == 1) decOperand = labels[operand]; // validate if the operand is a label
    else {
      if (operand[0] == '#') operand = operand.substr(1);
      decOperand = nSystems.hexToDec(mnemonicBuffer.getHexOpr(operand).substr(1));
    }
  }

  //* -------- ------- ------ ----- Calculate object code ----- ------ ------- --------
  for (int i = 0; i < objectCode.length(); i++) { // save the object code that doesn't need to be calculated
    if (lowercaseLetters.count(objectCode[i]) > 0) {
      stringBuffer = objectCode.substr(i); // saved to calculate the length of the object code that needs to be calculated
      break;
    }
    result << objectCode[i];
  }

  if (am == "IMM" || am == "DIR" || am == "EXT") {
    if (decOperand > 255 || stringBuffer.length() > 2) result << setfill('0') << setw(4) << hex << uppercase << decOperand;
    else result << setfill('0') << setw(2) << hex << uppercase << decOperand;
  }
  else if (am == "REL8" || am == "REL9") {
    intBuffer = decOperand - actualAddress;
    if (am == "REL9") { // only for mnemonic DBEQ & IBNE at the moment
      if (sourceForm == "DBEQ") {
        if (intBuffer >= 0) {
          if (reg == "A") result << "00 ";
          else if (reg == "B") result << "01 ";
          else if (reg == "D") result << "04 ";
          else if (reg == "X") result << "05 ";
          else if (reg == "Y") result << "06 ";
          else if (reg == "SP") result << "07 ";
        }
        else {
          if (reg == "A") result << "10 ";
          else if (reg == "B") result << "11 ";
          else if (reg == "D") result << "14 ";
          else if (reg == "X") result << "15 ";
          else if (reg == "Y") result << "16 ";
          else if (reg == "SP") result << "17 ";
        }
      }
      else if (sourceForm == "IBNE") {
        if (intBuffer >= 0) {
          if (reg == "A") result << "A0 ";
          else if (reg == "B") result << "A1 ";
          else if (reg == "D") result << "A4 ";
          else if (reg == "X") result << "A5 ";
          else if (reg == "Y") result << "A6 ";
          else if (reg == "SP") result << "A7 ";
        }
        else {
          if (reg == "A") result << "B0 ";
          else if (reg == "B") result << "B1 ";
          else if (reg == "D") result << "B4 ";
          else if (reg == "X") result << "B5 ";
          else if (reg == "Y") result << "B6 ";
          else if (reg == "SP") result << "B7 ";
        }
      }
    }
    ssBuffer << setfill('0') << setw(2) << hex << uppercase << intBuffer;
    ssBuffer.str(ssBuffer.str().substr(ssBuffer.str().length() - 2));

    if (intBuffer >= -128 && intBuffer <= 127) result << ssBuffer.str();
    else result.str("FDR");
  }
  else if (am == "REL16") {
    intBuffer = decOperand - actualAddress;
    ssBuffer << setfill('0') << setw(4) << hex << uppercase << intBuffer;
    ssBuffer.str(ssBuffer.str().substr(ssBuffer.str().length() - 4));
    if (intBuffer >= -32768 && intBuffer <= 32767) result << ssBuffer.str();
    else result.str("FDR");
  }
  else if (am == "IDX") {
    stringBuffer = operand.substr(operand.find(',') + 1);
    // cout << "Source form: " << sourceForm << " Operand: " << operand << " Register: " << operand.substr(0, operand.find(',')) << endl;
    if (idxRegs.count(operand.substr(0, operand.find(','))) != -1) { // F5
      ssBuffer << "111" << idxRegs[stringBuffer] << "1";
      if (operand.substr(0, operand.find(',')) == "A") ssBuffer << "00";
      if (operand.substr(0, operand.find(',')) == "B") ssBuffer << "01";
      if (operand.substr(0, operand.find(',')) == "D") ssBuffer << "10";
    }
    else { // F1
      ssBuffer << idxRegs[stringBuffer] << 0;
      if (operand[0] == '-') ssBuffer << "1";
      else ssBuffer << "0";
      ssBuffer << setfill('0') << setw(4) << nSystems.decToBinComplement2(operand.substr(0, operand.find(','))) << endl;
    }
    result << setfill('0') << setw(2) << uppercase << nSystems.binToHex(ssBuffer.str());
  }
  else if (am == "IDX1" || am == "IDX2") {
    stringBuffer = operand.substr(operand.find(',') + 1);
    ssBuffer << 111 << idxRegs[stringBuffer] << 0;
    if (am == "IDX1") {
      if (operand[0] == '-') ssBuffer << "01";
      else ssBuffer << "00";
      result << setfill('0') << setw(2) << uppercase << nSystems.binToHex(ssBuffer.str()) << " ";
      result << setfill('0') << setw(2) << uppercase << nSystems.decToHex(operand.substr(0, operand.find(',')));
    }
    else {
      if (operand[0] == '-') ssBuffer << "11";
      else ssBuffer << "10";
      result << setfill('0') << setw(2) << uppercase << nSystems.binToHex(ssBuffer.str()) << " ";
      result << setfill('0') << setw(4) << uppercase << nSystems.decToHex(operand.substr(0, operand.find(',')));
    }
  }
  else if (am == "[D,IDX]") {
    if (operand[1] != 'D') return "FDR";
    stringBuffer = operand.substr(operand.find(',') + 1);
    ssBuffer << "111" << idxRegs[stringBuffer.substr(0, stringBuffer.length() - 1)] << "111";
    result << setfill('0') << setw(2) << uppercase << nSystems.binToHex(ssBuffer.str()) << " ";
  }
  else if (am == "[IDX2]") {
    if (operand[1] == '-') return "FDR";
    stringBuffer = operand.substr(operand.find(',') + 1);
    ssBuffer << "111" << idxRegs[stringBuffer.substr(0, stringBuffer.length() - 1)] << "011";
    result << setfill('0') << setw(2) << uppercase << nSystems.binToHex(ssBuffer.str()) << " ";
    result << setfill('0') << setw(4) << uppercase << nSystems.decToHex(operand.substr(1, operand.find(',')));
  }
  else {
    result.str(objectCode);
    // cout << "Source form: " << sourceForm << " | Address Mode: " << am << endl;
  }

  return result.str();
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
      file << setfill('0') << setw(4) << hex << uppercase << address << " ";
      if (directive != "EQU") {
        ssBuffer << setfill('0') << setw(4) << hex << uppercase << address;
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
        try // write base object code between {}
        {
          if (operand.find(',') != -1) stoi("["); // force exception
          mnemonicBuffer.getHexOpr(operand);
          file << "{" << mnemonics[sourceForm].getObjectCode(mnemonics[sourceForm].getAddressMode(mnemonicBuffer.getHexOpr(operand))) << "} ";
        }
        catch (const std::exception& e)
        {
          cout << "operand: <" << operand << "> | ";
          cout << "AddressMode: <" << mnemonics[sourceForm].getAddressMode(operand) << ">" << endl;
          file << "{" << mnemonics[sourceForm].getObjectCode(mnemonics[sourceForm].getAddressMode(operand)) << "} ";
        }

        if (label != "") file << label << " ";
        if (operand == "") file << '*' << sourceForm << '\n';
        else file << '*' << sourceForm << " ~" << operand << '\n';

        try {
          if (idxRegs.count(operand.substr(operand.find(',') + 1)) != 0 || operand[0] == '[')
            instructionLength = mnemonics[sourceForm].getInstructionLenght(mnemonics[sourceForm].getAddressMode(operand));
          else {
            hexOpr = mnemonics[sourceForm].getHexOpr(operand);
            addressMode = mnemonics[sourceForm].getAddressMode(hexOpr);
            instructionLength = mnemonics[sourceForm].getInstructionLenght(addressMode);
          }
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
  int intBuffer{ 0 };
  string strBuffer{ "" };

  getline(iFile, strBuffer);
  while (!iFile.eof()) {
    if (strBuffer.find('{') == -1) { // There are not changes necesaries to this line
      file << strBuffer << '\n';
      getline(iFile, strBuffer);
      continue;
    }

    string objectCode{ "" };
    string sourceForm{ "" };
    string operand{ "" };
    string addressMode{ "" };
    int actualAddress{ 0 };

    //* -------- ------- ------ ----- Read data ----- ------ ------- --------
    intBuffer = strBuffer.find('{') + 1;
    objectCode = strBuffer.substr(intBuffer, strBuffer.find('}') - intBuffer);

    sourceForm = strBuffer.substr(strBuffer.find('*') + 1);
    sourceForm = sourceForm.substr(0, strBuffer.find(' '));
    if (sourceForm.back() == ' ') sourceForm = sourceForm.substr(0, sourceForm.size() - 1);

    intBuffer = strBuffer.find('~');
    if (intBuffer != -1) operand = strBuffer.substr(intBuffer + 1);

    if (operand != "") {
      if (labels.count(operand) != 0) addressMode = mnemonics[sourceForm].getAddressMode(to_string(labels[operand]));
      else if (operand.find(',') == -1) addressMode = mnemonics[sourceForm].getAddressMode(mnemonicBuffer.getHexOpr(operand));
      else addressMode = mnemonics[sourceForm].getAddressMode(operand);
    }
    else addressMode = mnemonics[sourceForm].getAddressMode();

    actualAddress = nSystems.hexToDec(strBuffer.substr(0, strBuffer.find(' '))); // only needed for REL 8, 9, 16
    if (addressMode == "REL8") actualAddress += 2;
    else if (addressMode == "REL9") actualAddress += 3;
    else if (addressMode == "REL16") actualAddress += 4;

    //* -------- ------- ------ ----- Calculate operation code ----- ------ ------- --------
    if (operand != "") objectCode = calculateObjectCode(objectCode, sourceForm, operand, addressMode, actualAddress);

    //* -------- ------- ------ ----- Write to file ----- ------ ------- --------
    intBuffer = strBuffer.find('{');
    strBuffer.replace(intBuffer, strBuffer.find('}') - intBuffer + 1, objectCode);
    strBuffer.replace(strBuffer.find('*'), 1, "");
    if (strBuffer.find('~') != -1) strBuffer.replace(strBuffer.find('~'), 1, "");
    file << strBuffer << '\n';

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

  remove("aux.lst");

}
