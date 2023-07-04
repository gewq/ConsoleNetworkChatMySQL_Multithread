/**
\file FileIsEmpty_Exception.h
\brief Класс FileIsEmpty_Exception - класс-обработчик исключения "Файл пуст"
*/

#pragma once

#include <string>
#include <exception>

class FileIsEmpty_Exception : public std::exception {
  public:
    FileIsEmpty_Exception() = delete;

    /**
    \param[in] fileName Название файла
    */
    explicit FileIsEmpty_Exception(const std::string& fileName);

    virtual const char* what() const noexcept override;

  private:
    std::string errorMessage_;	///<Сообщение об ошибке
};