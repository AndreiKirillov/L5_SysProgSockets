#pragma once
#include "framework.h"
#include "Kirillov_lab1_cpp.h"
#include "ThreadStorage.h"
#include "PipeOperations.h"
#include "Connection.h"
#include <afxsock.h>

__declspec(dllimport) void __stdcall ServerStart();

// Класс сервера, который будет обрабатывать подключения клиентов
class Server
{
private:
	CSocket _server;

	set<unique_ptr<Connection>> _connections;
	ThreadStorage _working_threads;
	mutex mtx_for_working_threads;             // синхронизирует доступ к рабочим потокам


	void ProcessClient(SOCKET hSock, int client_id);
	void CloseClient(int client_id);
public:
	Server();
	~Server();

	bool StartUp();

	void WaitForConnection();

	int GetClientsCount();
};

