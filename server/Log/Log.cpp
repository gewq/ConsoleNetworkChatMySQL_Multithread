#include "Log.h"

#include "Logger/Logger.h"  //Класс работы с файлом лога


void logger::write(const std::string& string)
{
  Logger log;
  log.write(string);
}



std::string logger::read()
{
  Logger log;
  std::string readString = log.read();
  return readString;
}