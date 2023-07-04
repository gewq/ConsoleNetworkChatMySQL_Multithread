/**
\file ThreadPool.h
\brief Класс для организации пула потоков

Потоки сгруппированы в вектор.
Задачи для выполнения помещаются в очередь.
Незанятые потоки поочерёдно извлекают задачи из очереди и выполняют их.
Помещение / извлечение задач в/из очереди защищено мьютексом

Тип задач - void function(int) - специально для возможности вызова функции
обработки запроса серверу: void server::handle(int clientSocketID)
*/


#pragma once

#include <functional>
#include <thread>
#include <mutex>
#include <vector>
#include <queue>
#include <condition_variable>


typedef std::function<void()> TaskType;
typedef void (*FuncType) (int);



class ThreadPool{
  public:
    /**
    Запустить работу потоков по отслеживанию поступления задач в очереди
    */
    ThreadPool();

    /**
    Завершить работу всех потоков
    */
    ~ThreadPool();

    /**
    Поместить задачу в очередь на выполнение
    \param[in] task Указатель на функцию для выполнения
    \param[in] arg Аргумент функции для выполнения
    */
    void pushTask(FuncType task, int arg);

  private:
    std::vector<std::thread> threads_;    ///<Потоки
    std::queue<TaskType> taskQueue_;      ///<Очередь задач для выполнения потоками
    std::mutex mutex_;                    ///<Защита доступа к очереди задач
    std::condition_variable eventHolder_; ///<Отслеживать поступление новых задач в очередь
    volatile bool work_;                  ///<Признак работы пула потоков

    /**
    Ожидание потоками поступления задач
    */
    void process();
};