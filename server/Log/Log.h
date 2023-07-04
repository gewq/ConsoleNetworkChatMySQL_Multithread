/**
\file Log.h
\brief Модуль взаимодействия с файлом лога

Является обёрткой вокруг класса Logger, обеспечивающего потокобезопасное
чтение/запись в файл лога
*/

#pragma once

#include <string>


namespace logger{
  /**
  Записать строку в файл лога
  \param[in] string Строка которую записать
  */
  void write(const std::string& string);

  /**
  \return Первую прочитанную строку из файла лога
  */
  std::string read();
}