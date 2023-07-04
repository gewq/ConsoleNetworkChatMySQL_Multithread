/**
\file Logger.h
\brief Класс для работы с файлом лога

Используется идиома RAII:
- открытие файла в конструкторе
- закрытие файла в деструкторе
Методы:
- запись строки в файл
- чтение строки из файла
*/

#pragma once

#include <fstream>


class Logger{
  public:
    /**
    Открыть файл лога
    */
    Logger();
    
    /**
    Закрыть файл лога
    */
    ~Logger();

    Logger(const Logger& other) = delete;

    Logger(Logger&& other) = delete;

    /**
    Записать строку в файл лога
    \param[in] string Строка которую записать
    */
    void write(const std::string& string);

    /**
    \return Последнюю строку из файла лога
    */
    std::string read();

  private:
    std::fstream file_;

    /**
    Установить указатель чтения файла на начало последней строки
    */
    void setLastLine();
};