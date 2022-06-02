#pragma once
#include "framework.h"
#include "Kirillov_lab1_cpp.h"
#include "ThreadStorage.h"
#include "Connection.h"
#include <afxsock.h>

	//__declspec(dllimport) header __stdcall ReadHeader(SOCKET hSock);
	//__declspec(dllimport) std::string __stdcall ReadMessage(SOCKET hSock, const header& h);
	//__declspec(dllimport) void __stdcall SendConfirm(SOCKET hSock, const confirm_header& h);

// ����� �������, ������� ����� ������������ ����������� ��������
class Server
{
private:
	CSocket _server;

	set<unique_ptr<Connection>> _connections;
	ThreadStorage _working_threads;
	mutex mtx_for_working_threads;             // �������������� ������ � ������� �������


	void ProcessClient(SOCKET hSock, int client_id);
	void CloseClient(int client_id);
public:
	Server();
	~Server();

	bool StartUp();

	void WaitForConnection();

	int GetClientsCount();
};

