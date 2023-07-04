/**
\file Network.h
\brief Модуль "Сеть" - содержит методы работы с приёмом/передачей данных по сети
*/

#pragma once

#include <iostream>


namespace network{
  /**
  Создать сервер - сетевое соединение
  \param[in] port Порт сервера
  */
  void initialize(uint16_t port);

  /**
  Принимать и отвечать на запросы клиентов
  */
  void handleIncomingRequests();

  /**
  Закрыть сетевое соединение
  */
  void disconnect();
}