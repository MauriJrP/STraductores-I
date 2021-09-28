#include "../include/FileManager.h"

using namespace std;

FileManager::FileManager()
{
}

FileManager::~FileManager()
{
}

void FileManager::createFile(string fileName)
{
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
