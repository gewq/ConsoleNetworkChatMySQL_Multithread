/**
\file Server.h
\brief Модуль "Сервер" - обрабатывает запросы клиентов
*/

#include <string>


namespace server{
  /**
  Обработать входящее сообщение и отправить ответ
  \param[in] clientSocketID Дескриптор сокета клиента
  */
  void handle(int clientSocketID);
}