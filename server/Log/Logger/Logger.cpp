#include "Logger.h"

#include <string>
#include <fstream>
#include <assert.h>
#include <shared_mutex>

#include "Exceptions/FileNotOpened_Exception.h"
#include "Exceptions/FileIsEmpty_Exception.h"

#include <iostream>


namespace{
  const std::string LOG_FILE_NAME = "Log.txt";
  std::shared_mutex sharedMutex;
}



Logger::Logger()
{
  //Открыть файл лога на чтение / запись
  file_.open(LOG_FILE_NAME, std::ios::app | std::ios::out | std::ios::in);

  if (!file_.is_open()){
    throw FileNotOpened_Exception(LOG_FILE_NAME);
  }
}



Logger::~Logger()
{
  //Закрыть файл лога
  file_.close();
}



void Logger::write(const std::string& string)
{
  //Блокировать чтение и запись другими потоками
  sharedMutex.lock();
  file_ << std::endl;
  file_ << string;
  sharedMutex.unlock();
}



std::string Logger::read()
{
  std::string readString;

  //Блокировать запись (остальные потоки-читатели не блокированы)
  sharedMutex.lock_shared();
  setLastLine();  //Установить указатель чтения на начало последней строки
  getline(file_, readString, '\n');
  sharedMutex.unlock_shared();
  
  if (readString.empty()){
    throw FileIsEmpty_Exception(LOG_FILE_NAME);
  }

  return readString;
}



//=============================================================================
void Logger::setLastLine()
{
  //Читаем по одному символу с конца файла пока не встретим первый '\n'
  //или не дойдём до начала файла

  //Установить указатель чтения за 1 символ от конца файла
  file_.seekg(-1, std::ios_base::end);
  
  bool isSearch = true;
  while(isSearch) {
    char ch;
    file_.get(ch);
    //Дошли до начала файла
    if( static_cast<unsigned int>(file_.tellg()) <= 1) {
      file_.seekg(0);
      isSearch = false;
    }

    //Нашли начало строки ближайшей к концу файла
    else if(ch == '\n') {
      isSearch = false;
    }

    //Продолжаем поиск
    else {
      //Переместить указатель чтения перед предыдущим
      file_.seekg(-2, std::ios_base::cur);
    }
  }
}