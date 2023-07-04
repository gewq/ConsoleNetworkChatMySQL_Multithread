/**
\file FileNotOpened_Exception.h
\brief Класс FileNotOpened_Exception - класс-обработчик исключения "Файл не открыт"
*/

#pragma once

#include <string>
#include <exception>

class FileNotOpened_Exception : public std::exception {
  public:
    FileNotOpened_Exception() = delete;

    /**
    \param[in] fileName Название файла
    */
    explicit FileNotOpened_Exception(const std::string& fileName);

    virtual const char* what() const noexcept override;

  private:
    std::string errorMessage_;	///<Сообщение об ошибке
};