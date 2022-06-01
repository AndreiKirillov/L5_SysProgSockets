// sockets.cpp: определяет процедуры инициализации для библиотеки DLL.
//

#include "pch.h"
#include "framework.h"
#include "sockets.h"
#include <string>
#include <vector>

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

	if (!AfxSocketInit())
	{
		AfxMessageBox(IDP_SOCKETS_INIT_FAILED);
		return FALSE;
	}

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

CSocket client_socket;

__declspec(dllexport) header __stdcall ReadHeader(CSocket& reading_source)
{
	header header_from_client;
	reading_source.Receive(&header_from_client, sizeof(int));
	return header_from_client;
}

__declspec(dllexport) std::string __stdcall ReadMessage(CSocket& reading_source, const header& h)
{
	std::vector <char> v(h.message_size);

	reading_source.Receive(&v[0], h.message_size);

	return std::string(&v[0], h.message_size);
}

__declspec(dllexport) void __stdcall SendConfirm(CSocket& sending_sock, const confirm_header& h)     // Функция отправки подтверждения клиенту
{
	sending_sock.Send(&h, sizeof(confirm_header));
}

extern "C" 
{
	__declspec(dllexport) header __stdcall ReadHeader()   // Чтение заголовка клиентом
	{
		return header();
	}
}
