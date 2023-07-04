#include "Server.h"

#include <thread>
#include <mutex>

#include "../DataBase/DataBase.h"
#include "../Network/Network.h"
#include "../Log/Log.h"


namespace{
  //Коды запросов серверу
  enum Command{
    NOTHING,
    IS_LOGIN_REGISTERED,
    IS_PASSWORD_RIGHT,
    IS_NICKNAME_REGISTERED,
    REQUEST_NICKNAME,
    REQUEST_ALL_NICKNAMES,
    REQUEST_NUMBER_USERS,
    REQUEST_MESSAGES,
    ADD_USER,
    ADD_MESSAGE,
    REMOVE_USER
  };

  //MAX размер пересылаемых сообщений
  const uint16_t MAX_LENGTH_MESSAGE = 1024;

  //Защита от одновременного доступа к базе данных несколькими потоками
  std::mutex mutex;
}


/**
Прочитать входящее сообщение от клиента
\param[in] clientSocketID Дескриптор сокета клиента
*/
static std::string readMessage(int clientSocketID);

/**
Отправить сообщение клиенту
\param[in] clientSocketID Дескриптор сокета клиента
*/
static void sendMessage(int clientSocketID,
                        const std::string& message);

/**
Обработать запрос и отправить ответ клиенту
\param[in] clientSocketID Дескриптор сокета клиента
\param[in] request Запрос клиента в виде строки
*/
static void handleRequest(int clientSocketID,
                          const std::string& request);



void server::handle(int clientSocketID)
{
  //Прочитать сообщение
  const std::string input = readMessage(clientSocketID);

  //Обработать и вернуть клиенту ответ
  handleRequest(clientSocketID, input);
}



static std::string readMessage(int clientSocketID)
{
  //Буфер сообщений
  char inputBuffer[MAX_LENGTH_MESSAGE];

  //Прочитать сообщение в буфер
  read(clientSocketID, inputBuffer, MAX_LENGTH_MESSAGE);

  const std::string message = inputBuffer;
  return message;
}



static void sendMessage(int clientSocketID,
                        const std::string& message)
{
  write(clientSocketID, message.c_str(), MAX_LENGTH_MESSAGE);
}                  




/**
Проверить Логин на наличие в базе и отправить клиенту ответ
\param[in] request Запрос клиента в виде строки
\return Ответ клиенту
*/
static std::string isLoginRegistered(const std::string& request);

/**
Проверить правильный ли Пароль и отправить клиенту ответ
\param[in] request Запрос клиента в виде строки
\return Ответ клиенту
*/
static std::string isPasswordRight(const std::string& request);

/**
Проверить Ник на наличие в базе и отправить клиенту ответ
\param[in] request Запрос клиента в виде строки
\return Ответ клиенту
*/
static std::string isNicknameRegistered(const std::string& request);

/**
Прислать Ник по Логину и отправить клиенту ответ
\param[in] request Запрос клиента в виде строки
\return Ответ клиенту
*/
static std::string sendNickname(const std::string& request);

/**
Прислать клиенту Ники всех пользователей
\return Ответ клиенту
*/
static std::string sendAllNicknames();

/**
Прислать клиенту количество зарегистрированных пользователей
\return Ответ клиенту
*/
static std::string sendNumberUsers();

/**
Прислать клиенту сообщения пользователю
\param[in] request Запрос клиента в виде строки
\return Ответ клиенту
*/
static std::string sendMessages(const std::string& request);

/**
Добавить пользователя в Базу и отправить клиенту ответ
\param[in] request Запрос клиента в виде строки
\return Ответ клиенту
*/
static std::string addUser(const std::string& request);

/**
Добавить сообщение пользователю в Базу и отправить клиенту ответ
\param[in] request Запрос клиента в виде строки
\return Ответ клиенту
*/
static std::string addMessage(const std::string& request);

/**
Удалить аккаунт пользователя по Логину и отправить клиенту ответ
\param[in] request Запрос клиента в виде строки
\return Ответ клиенту
*/
static std::string removeUser(const std::string& request);



static void handleRequest(int clientSocketID,
                          const std::string& request)
{
  //Message - Код_Команды|АРГУМЕНТ_1:...
  //Определить код команды - от начала строки до первого '|'
  const std::string codeCommand = request.substr(0, request.find("|"));
  try {
    int command = std::stoi(codeCommand);
    std::string response = "false";
    switch (command){
      case IS_LOGIN_REGISTERED: {
        response = isLoginRegistered(request);
        break;
      }
      case IS_PASSWORD_RIGHT: {
        response = isPasswordRight(request);
        break;
      }
      case IS_NICKNAME_REGISTERED: {
        response = isNicknameRegistered(request);
        break;
      }
      case REQUEST_NICKNAME: {
        response = sendNickname(request);
        break;
      }
      case REQUEST_ALL_NICKNAMES: {
        response = sendAllNicknames();
        break;
      }
      case REQUEST_NUMBER_USERS: {
        response = sendNumberUsers();
        break;
      }
      case REQUEST_MESSAGES: {
        response = sendMessages(request);
        break;
      }
      case ADD_USER: {
        response = addUser(request);
        break;
      }
      case ADD_MESSAGE: {
        response = addMessage(request);
        break;
      }
      case REMOVE_USER: {
        response = removeUser(request);
        break;
      }
      default:
        break;
    }
    //Отправить ответ клиенту
    sendMessage(clientSocketID, response);
  }
  catch (const std::invalid_argument&) {
  }
}



/**
Распарсить строку на слова по разделителю
\param[out] result Строки в которые поместить результат парсинга
\param[in] input Входная строка
\param[in] delimiter Разделитель
*/
static void parse (std::shared_ptr<std::vector<std::string> > result,
                  const std::string& input,
                  const std::string& delimiter);


static std::string isLoginRegistered(const std::string& request)
{
  //request - Код_Команды|LOGIN|
  std::string message = request;

  //Распарсить входное сообщение
  auto result = std::make_shared<std::vector<std::string> >();
  parse(result, message, "|");
  const std::string login = result->at(1);

  std::string response = "false";
  std::unique_lock<std::mutex> guard(mutex);
  //Проверить Логин в базе
  if (database::isLoginRegistered(login)){
    response = "true";
  }
  guard.unlock();

  return response;
}



static std::string isPasswordRight(const std::string& request)
{
  //request - Код_Команды|LOGIN|HASHPASSWORD|
  std::string message = request;

  //Распарсить входное сообщение
  auto result = std::make_shared<std::vector<std::string> >();
  parse(result, message, "|");
  const std::string login = result->at(1);
  const std::string passwordHash = result->at(2);

  std::string response = "false";
  std::unique_lock<std::mutex> guard(mutex);
  //Проверить Пароль в базе
  if (database::isPasswordRight(login, passwordHash)){
    response = "true";
  }
  guard.unlock();

  return response;
}



static std::string isNicknameRegistered(const std::string& request)
{
  //request - Код_Команды|NICKNAME|
  std::string message = request;

  //Распарсить входное сообщение
  auto result = std::make_shared<std::vector<std::string> >();
  parse(result, message, "|");
  const std::string nickname = result->at(1);

  std::string response = "false";
  std::unique_lock<std::mutex> guard(mutex);
  //Проверить Ник в базе
  if (database::isNicknameRegistered(nickname)){
    response = "true";
  }
  guard.unlock();
  
  return response;
}



static std::string sendNickname(const std::string& request)
{
  //request - Код_Команды|LOGIN|
  std::string message = request;

  //Распарсить входное сообщение
  auto result = std::make_shared<std::vector<std::string> >();
  parse(result, message, "|");
  const std::string login = result->at(1);

  //Получить Ник из Базы
  std::unique_lock<std::mutex> guard(mutex);
  const std::string nickname = database::getNickname(login);
  guard.unlock();
  std::string response = "";
  if (nickname.empty()){
    response = "false";
  }
  else{
    response = nickname;
  }

  return response;
}



static std::string sendAllNicknames()
{
  //Message - Код_Команды
  auto nicknames = std::make_shared<std::vector<std::string> >();
  std::unique_lock<std::mutex> guard(mutex);
  database::loadUserNames(nicknames);
  guard.unlock();

  //Собрать Ники в ответное сообщение
  std::string response = "";
  for (const auto& name : *nicknames) {
    response += name + "|";
	}
  
  return response;
}



static std::string sendNumberUsers()
{
  //Message - Код_Команды
  //Запросить в Базе
  std::unique_lock<std::mutex> guard(mutex);
  std::string response = std::to_string(database::getNumberUsers());
  guard.unlock();

  return response;
}



static std::string sendMessages(const std::string& request)
{
  //request - Код_Команды|LOGIN|
  std::string message = request;

  //Распарсить входное сообщение
  auto result = std::make_shared<std::vector<std::string> >();
  parse(result, message, "|");
  const std::string login = result->at(1);

  //Загрузить сообщения
  auto messagesToUser = std::make_shared<std::list<Message> >();
  std::unique_lock<std::mutex> guard(mutex);
  database::loadMessages(login, messagesToUser);
  guard.unlock();

  //Сформировать ответное сообщение в формате
  //NICK_FROM:MESSAGE:|NICK_FROM:MESSAGE:|...
  std::string response = "";
  if (!messagesToUser->empty()){
    for (const auto& msg : *messagesToUser) {
      response += msg.getNameFrom() + ":" + msg.getText() + ":|";
    }
  }
  
  return response;
}



static std::string addUser(const std::string& request)
{
  //Message - Код_Команды|NICKNAME|LOGIN|HASHPASSWORD|
  std::string message = request;

  //Распарсить входное сообщение
  auto result = std::make_shared<std::vector<std::string> >();
  parse(result, message, "|");
  const std::string name = result->at(1);
  const std::string login = result->at(2);
  const std::string passwordHash = result->at(3);

  //Добавить в базу
  std::unique_lock<std::mutex> guard(mutex);
  database::addUser(name, login, passwordHash);
  guard.unlock();

  const std::string response = "true";
  return response;
}



static std::string addMessage(const std::string& request)
{
  //input - Код_Команды|NICKNAME_TO|NICKNAME_FROM|MESSAGE|
  std::string input = request;

  //Распарсить входное сообщение
  auto result = std::make_shared<std::vector<std::string> >();
  parse(result, input, "|");
  const std::string receiver = result->at(1);
  const std::string sender = result->at(2);
  const std::string text = result->at(3);

  //Добавить сообщение в Базу
  std::unique_lock<std::mutex> guard(mutex);
  database::addMessage(sender, receiver, text);
  guard.unlock();

  //Вывод сообщения в файл лога
  const std::string toLog = sender + " -> " + receiver + " : " + text;
  logger::write(toLog);

  const std::string response = "true";
  return response;
}



static std::string removeUser(const std::string& request)
{
  //request - Код_Команды|LOGIN|
  std::string message = request;

  //Распарсить входное сообщение
  auto result = std::make_shared<std::vector<std::string> >();
  parse(result, message, "|");
  const std::string login = result->at(1);

  //Удалить аккаунт пользователя
  std::unique_lock<std::mutex> guard(mutex);
  database::removeUser(login);
  guard.unlock();

  const std::string response = "true";
  return response;
}



static void parse (std::shared_ptr<std::vector<std::string> > result,
                  const std::string& input,
                  const std::string& delimiter)
{
  result->clear();
  std::string string = input;
  while (!string.empty()){
    std::string value = string.substr(0, string.find(delimiter));
    string = string.substr(string.find(delimiter)+1);
    result->push_back(value);
  }
}