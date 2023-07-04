#include "Network.h"

#include <sys/socket.h>
#include <netinet/in.h>
#include <unistd.h>

#include "../Multithreading/RequestHandler/RequestHandler.h"
#include "../Server/Server.h"

#include "Exceptions/SocketCreation_Exception.h"
#include "Exceptions/SocketBinding_Exception.h"
#include "Exceptions/MaxClients_Exception.h"
#include "Exceptions/SocketConnection_Exception.h"



namespace{
  //Максимальное количество клиентов ожидающих ответ в очереди
  const uint16_t MAX_CLIENTS = 100;

  //ID сокета сервера
  int serverSocketID;

  //Пул потоков для обработки входящих запросов
  RequestHandler threadPool;
}



/**
Создать сокет сервера
*/
static void createServerSocket();

/**
Привязать сокет к порту
\param[in] socketID ID сокета который привязать
\param[in] port Порт к которому привязать
*/
static void bindSocket(int socketID, uint16_t port);



void network::initialize(uint16_t port)
{  
  //Создать сокет сервера
  createServerSocket();

  //Связать сокет сервера с портом
  bindSocket(serverSocketID, port);

  printf("Server is listening new connections...\n");
}



void network::handleIncomingRequests()
{
  //Сервер на приём данных
  int connection_status = listen(serverSocketID, MAX_CLIENTS);
  if (connection_status == -1){
    throw MaxClientExceeds_Exception();
  }

  struct sockaddr_in client;
  socklen_t length = sizeof(client);
  //Извлечь соединение из очереди ожидающих и создать сокет для взаимодействия с ним
  int clientSocketID = accept(serverSocketID, (struct sockaddr*)&client, &length);
  if (clientSocketID == -1){
    throw SocketConnection_Exception();
  }
  
  //Поместить сокет клиента для обработки в очередь задач пула потока
  threadPool.pushRequest(server::handle, clientSocketID);
}



void network::disconnect()
{
  close(serverSocketID);
}



//-----------------------------------------------------------------------------
static void createServerSocket()
{
  serverSocketID = socket(AF_INET, SOCK_STREAM, 0);
  if (serverSocketID == -1){
		throw SocketCreation_Exception();
  }
}



static void bindSocket(int socketID, uint16_t port)
{
  //Задать сетевые параметры сервера
  struct sockaddr_in serverAddress;
  serverAddress.sin_addr.s_addr = htonl(INADDR_ANY);  //Приём на все сетевые интерфейсы
  serverAddress.sin_port = htons(port); //Порт
  serverAddress.sin_family = AF_INET;   //IPv4

  //Привязать сокет
  int bind_status = bind(socketID,
                        (struct sockaddr*) &serverAddress,
                        sizeof(serverAddress));
  if (bind_status == -1){
    throw SocketBinding_Exception();
  }
}