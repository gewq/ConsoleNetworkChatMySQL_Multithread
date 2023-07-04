#include "ThreadPool.h"
#include <iostream>


ThreadPool::ThreadPool()
{
  std::unique_lock<std::mutex> l(mutex_);
  work_ = true;
  //Максимальное количество одновременно выполняемых потоков
  const unsigned int MAX_THREADS = std::thread::hardware_concurrency();
  threads_.reserve(MAX_THREADS);
  for(size_t i=0; i<MAX_THREADS; ++i){
    threads_.push_back(std::thread(&ThreadPool::process, this));
  }
}



ThreadPool::~ThreadPool()
{
  std::unique_lock<std::mutex> l(mutex_);
  work_ = false;
  l.unlock();
  //Разбудить все потоки, находящиеся в режиме ожидания
  eventHolder_.notify_all();

  for(auto& t: threads_){
    t.join(); //Дождаться завершения работы все потоков
  }
}



void ThreadPool::pushTask(FuncType task, int arg)
{
  TaskType newTask([=](){task(arg);});
  std::unique_lock<std::mutex> l(mutex_);
  taskQueue_.push(newTask);

  //Оповестить случайный поток о новой задаче
  eventHolder_.notify_one();
}



void ThreadPool::process()
{
  while(true){
    std::unique_lock<std::mutex> locker(mutex_);

    //Указатель на функцию (задачу) для выполнения потоком
    TaskType taskToDo;

    //Нет задач для выполнения и Флаг работы сброшен - останов работы
    if (taskQueue_.empty() && !work_){
      return; //Завершить работу
    }

    //Нет задач для выполнения
    if (taskQueue_.empty()){
      //Ждать новой задачи или команды на останов
      eventHolder_.wait(locker, [&](){return !taskQueue_.empty() || !work_;});
    }

    //Есть задачи для выполнения
    if (!taskQueue_.empty()){
      //Извлечь задачу
      taskToDo = taskQueue_.front();
      taskQueue_.pop();
    }
    //Освободить очередь задач для других потоков
    locker.unlock();

    //Выполнить задачу
    taskToDo();
  }
}