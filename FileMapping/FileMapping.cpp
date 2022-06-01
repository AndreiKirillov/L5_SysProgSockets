// FileMapping.cpp: определяет процедуры инициализации для библиотеки DLL.
//

#include "pch.h"
#include "framework.h"
#include "FileMapping.h"
#include <string>
#include <vector>


#ifdef _DEBUG
#define new DEBUG_NEW
#endif

using namespace std;
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

// CFileMappingApp

BEGIN_MESSAGE_MAP(CFileMappingApp, CWinApp)
END_MESSAGE_MAP()


// Создание CFileMappingApp

CFileMappingApp::CFileMappingApp()
{
	// TODO: добавьте код создания,
	// Размещает весь важный код инициализации в InitInstance
}


// Единственный объект CFileMappingApp

CFileMappingApp theApp;


// Инициализация CFileMappingApp

BOOL CFileMappingApp::InitInstance()
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

HANDLE hClientPipe;


extern "C"
{
	// Функция подключения клиента к серверу
	__declspec(dllexport) bool __stdcall ConnectToServer()
	{
		if (!WaitNamedPipeA("\\\\.\\pipe\\MyPipe_lab4", 3000))
		{
			return false;
		}
		else
		{
			HANDLE hPipe = CreateFileA("\\\\.\\pipe\\MyPipe_lab4", GENERIC_READ | GENERIC_WRITE, 0, NULL, OPEN_EXISTING, 0, NULL);
			if (hPipe == INVALID_HANDLE_VALUE)
				return false;
			else
			{
				hClientPipe = hPipe;      // Сохраняем хэндл канала
				return true;
			}
		}
	}

	__declspec(dllexport) bool __stdcall SendMessageToServer(const char* message, header& h)
	{
		DWORD dwWrite;
		if (!WriteFile(hClientPipe, &h, sizeof(header), &dwWrite, nullptr) || dwWrite == 0) // пишем заголовок
			return false;

		dwWrite = 0;
		if (!WriteFile(hClientPipe, message, strlen(message) + 1, &dwWrite, nullptr) || dwWrite == 0) // пишем тело сообщения
			return false;
		return true;
	}

	__declspec(dllexport) confirm_header __stdcall WaitForConfirm()
	{
		DWORD dwDone;
		confirm_header header_for_client;

		if (!ReadFile(hClientPipe, &header_for_client, sizeof(confirm_header), &dwDone, nullptr) || dwDone == 0) 
			return confirm_header{ 0,0 };
		return header_for_client;
	}

	// функция для проверки соединения с сервером, плюс сразу узнаем кол-во потоков на сервере
	__declspec(dllexport) void __stdcall CheckServer()
	{
		DWORD dwDone;
		header request{ 3, 0,0 };
		WriteFile(hClientPipe, &request, sizeof(header), &dwDone, nullptr);

		//confirm_header answer;

		//if (!TransactNamedPipe(hClientPipe, &request, sizeof(header), &answer, sizeof(confirm_header), &dwDone, nullptr))
			//return confirm_header{ 0,0 };
		//return answer;
	}
}
