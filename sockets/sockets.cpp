// sockets.cpp: определяет процедуры инициализации для библиотеки DLL.
//

#include "pch.h"
#include "framework.h"
#include "sockets.h"
#include <iostream>

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

CSocket server;
CSocket client_socket;

__declspec(dllexport) void __stdcall ServerStart()
{
	server.Create(12345);
}

__declspec(dllexport) void __stdcall WaitForConnection()
{
	if (!server.Listen())
		return;
	client_socket = 
}


__declspec(dllexport) void __stdcall test() 
{
	std::cout << "dll connected";
}