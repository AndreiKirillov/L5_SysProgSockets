#pragma once
#include "framework.h"
#include "Kirillov_lab1_cpp.h"
#include "ThreadStorage.h"
#include "PipeOperations.h"
#include "Connection.h"

// Класс сервера, который будет обрабатывать подключения клиентов
class Server
{
private:
	set<unique_ptr<Connection>> _connections;
	ThreadStorage _working_threads;
	mutex mtx_for_working_threads;             // синхронизирует доступ к рабочим потокам


	void ProcessClient(HANDLE hConnection, int client_id);
	void CloseClient(int client_id);
public:
	Server();
	~Server();

	void WaitForConnection();

	int GetClientsCount();
};

