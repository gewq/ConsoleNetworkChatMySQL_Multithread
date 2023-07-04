#include "RequestHandler.h"



RequestHandler::RequestHandler() : threadPool_()
{
}



RequestHandler::~RequestHandler()
{
}



void RequestHandler::pushRequest(FuncType task, int arg)
{
  threadPool_.pushTask(task, arg);
}