/**
\file RequestHandler.h
\brief Класс-обёртка пула потоков

Предоставляет интерфейс для помещения задач в пул потоков
В конструкторе выполняется инициализация пула потоков
В деструкторе выполняется завершение работы пула потоков
*/

#pragma once

#include "../ThreadPool/ThreadPool.h"



class RequestHandler{
  public:
    RequestHandler();
    
    ~RequestHandler();

    /**
    Поместить задачу в очередь на выполнение
    \param[in] task Указатель на функцию для выполнения
    \param[in] arg Аргумент функции для выполнения
    */
    void pushRequest(FuncType task, int arg);

  private:
    ThreadPool threadPool_; ///<Пул потоков
};