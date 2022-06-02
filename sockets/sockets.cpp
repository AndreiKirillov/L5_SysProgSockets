// sockets.cpp: определяет процедуры инициализации для библиотеки DLL.
//

#include "pch.h"
#include "framework.h"
#include "sockets.h"
#include <afxsock.h>
#include <string>
#include <vector>
#include <memory>

#ifdef _DEBUG
#define new DEBUG_NEW
#endif

//
//TODO: если эта библиотека DLL динамически связана с библиотеками DLL MFC,
//		все функции, экспортированные из данной DLL-библиотеки, которые выполняют вызовы к
//		MFC, должны содержать макрос AFX_MANAGE_STATE в
//		самое начало функции.
//
//		Например:
//
//		extern "C" BOOL PASCAL EXPORT ExportedFunction()
//		{
//			AFX_MANAGE_STATE(AfxGetStaticModuleState());
//			// тело нормальной функции
//		}
//
//		Важно, чтобы данный макрос был представлен в каждой
//		функции до вызова MFC.  Это означает, что
//		должен стоять в качестве первого оператора в
//		функции и предшествовать даже любым объявлениям переменных объекта,
//		поскольку их конструкторы могут выполнять вызовы к MFC
//		DLL.
//
//		В Технических указаниях MFC 33 и 58 содержатся более
//		подробные сведения.
//

// CsocketsApp

BEGIN_MESSAGE_MAP(CsocketsApp, CWinApp)
END_MESSAGE_MAP()


// Создание CsocketsApp

CsocketsApp::CsocketsApp()
{
	// TODO: добавьте код создания,
	// Размещает весь важный код инициализации в InitInstance
}


// Единственный объект CsocketsApp

CsocketsApp theApp;


// Инициализация CsocketsApp

BOOL CsocketsApp::InitInstance()
{
	CWinApp::InitInstance();

	return TRUE;
}

// структура для заголовка сообщения
struct header
{
	int event_code;
	int thread_id;
	int message_size;
};

struct confirm_header // заголовок для подтверждения
{
	int confirm_status;
	int threads_count;
};

std::shared_ptr<CSocket> client_socket;

//__declspec(dllexport) header __stdcall ReadHeader(CSocket& reading_source)
//{
//	header header_from_client;
//
//	reading_source.Receive(&header_from_client, sizeof(int));
//	return header_from_client;
//}
//
//__declspec(dllexport) std::string __stdcall ReadMessage(CSocket& reading_source, const header& h)
//{
//	std::vector <char> v(h.message_size);
//
//	reading_source.Receive(&v[0], h.message_size);
//
//	return std::string(&v[0], h.message_size);
//}
//
//__declspec(dllexport) void __stdcall SendConfirm(CSocket& sending_sock, const confirm_header& h)     // Функция отправки подтверждения клиенту
//{
//	sending_sock.Send(&h, sizeof(confirm_header));
//}

extern "C" 
{
	__declspec(dllexport) bool __stdcall ConnectToServer()
	{
		AfxWinInit(::GetModuleHandle(NULL), NULL, ::GetCommandLine(), 0);
		AfxSocketInit();
		client_socket = std::make_shared<CSocket>();
		client_socket->Create();
		if (!client_socket->Connect("127.0.0.1", 12345))
		{
			return false;
		}
		return true;
	}

	__declspec(dllexport) bool __stdcall SendMessageToServer(const char* message, header& msg_header)  
	{
		if (client_socket->Send(&msg_header, sizeof(header)) == SOCKET_ERROR)
			return false;

		if (client_socket->Send(message, msg_header.message_size) == SOCKET_ERROR)
			return false;
		else
			return true;
	}

	__declspec(dllexport) confirm_header __stdcall WaitForConfirm()
	{
		confirm_header confirm;

		int server_answer = client_socket->Receive(&confirm, sizeof(confirm_header));
		if (server_answer == SOCKET_ERROR || server_answer == 0)
			return confirm_header{ 0,0 };
		else
			return confirm;
	}

	__declspec(dllexport) void __stdcall CheckServer()
	{
		header request{ 3, 0, 0 };
		client_socket->Send(&request, sizeof(header));
	}

	__declspec(dllexport) void __stdcall DisconnectFromServer()
	{
		client_socket->Close();
	}
}
