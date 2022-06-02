// SocketClient.cpp : Этот файл содержит функцию "main". Здесь начинается и заканчивается выполнение программы.
//

#include "pch.h"
#include "framework.h"
#include "SocketClient.h"
#include <afxsock.h>

#ifdef _DEBUG
#define new DEBUG_NEW
#endif

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

	extern "C"
	{
		__declspec(dllimport) bool __stdcall ConnectToServer();
		__declspec(dllimport) bool __stdcall SendMessageToServer(const char* message, header& h);
		__declspec(dllimport) confirm_header __stdcall WaitForConfirm();
		__declspec(dllimport) void __stdcall CheckServer();
	}

// Единственный объект приложения

CWinApp theApp;

using namespace std;

int main()
{
    int nRetCode = 0;

    HMODULE hModule = ::GetModuleHandle(nullptr);

    if (hModule != nullptr)
    {
        // инициализировать MFC, а также печать и сообщения об ошибках про сбое
        if (!AfxWinInit(hModule, nullptr, ::GetCommandLine(), 0))
        {
            // TODO: вставьте сюда код для приложения.
            wprintf(L"Критическая ошибка: сбой при инициализации MFC\n");
            nRetCode = 1;
        }
        else
        {
            // TODO: вставьте сюда код для приложения.
			if (ConnectToServer())
			{
				CheckServer();
				WaitForConfirm();

				while (true)
				{
					cout << "Enter an action" << endl;
					int event_index;
					cin >> event_index;
					switch (event_index)
					{
					case 0:
					{
						header msg_header{ 0, 0, 1 };

						SendMessageToServer("", msg_header);

						confirm_header h = WaitForConfirm();
						if (h.confirm_status)
						{
							cout << "Successful" << endl
								<< "Number of threads = " << h.threads_count << endl;
						}
						else
							cout << "Error answer" << endl;
					}
					break;

					case 1:
					{
						header msg_header{ 1, 0, 0 };

						SendMessageToServer("", msg_header);

						confirm_header h = WaitForConfirm();
						if (h.confirm_status)
						{
							cout << "Successful" << endl
								<< "Number of threads = " << h.threads_count << endl;
						}
						else
							cout << "Error answer" << endl;
					}
					break;

					case 2:
					{
						cout << "Enter a message" << endl;
						string s;
						cin >> s;

						header msg_header{ 2, 0, s.size() };

						SendMessageToServer(s.c_str(), msg_header);
						if (s == "quit")
						{
							return 0;
						}
						confirm_header h = WaitForConfirm();
						if (h.confirm_status)
						{
							cout << "Successful" << endl
								<< "Number of threads = " << h.threads_count << endl;
						}
						else
							cout << "Error answer" << endl;
					}
					}
				}
			}
        }
    }
    else
    {
        // TODO: измените код ошибки в соответствии с потребностями
        wprintf(L"Критическая ошибка: сбой GetModuleHandle\n");
        nRetCode = 1;
    }

    return nRetCode;
}
