// Kirillov_lab1_cpp.cpp : Этот файл содержит функцию "main". Здесь начинается и заканчивается выполнение программы.
//

//#define _MAIN true
//#define _WORKING false

#include "pch.h"
#include "framework.h"
#include "Kirillov_lab1_cpp.h"
#include "ThreadKirillov.h"
#include "ThreadStorage.h"
#include "FileMapping.h"
#include "Server.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#endif

// Единственный объект приложения

CWinApp theApp;

using namespace std;

shared_mutex data_mtx;       // будет синхронизировать доступ к анонимному каналу
mutex console_mtx;    // будет синхронизировать работу консоли
HANDLE confirm_finish_of_thread_event = CreateEventA(NULL, FALSE, FALSE, NULL);  // будет сообщать о завершении потока

shared_ptr<string> ptr_global_message;

// Функция обработки сообщения в главном потоке
void ProcessMessage(shared_ptr<string>& message)
{
    shared_lock<shared_mutex> data_for_reading_lock(data_mtx); // общая блокировка, потоки могут читать данные

    lock_guard<mutex> lock_console(console_mtx);
    if (message->empty())
        cout << "MAIN THREAD FAIL: Message wasn't received or empty!" << endl;
    else
    {
        cout << "Main Thread RECEIVED Message" << endl <<
            "Size: " << message->size() << endl <<
            "Message: " << *message << endl;
    }
}

// Функция выполнения в потоке
void ThreadFunction(int thread_id, HANDLE finish_event, HANDLE receive_msg_event, weak_ptr<string>&& ptr_to_message)
{
    console_mtx.lock();
    cout << "Thread №" + to_string(thread_id) + " START" << endl;
    console_mtx.unlock();

    HANDLE hControlEvents[2] = {receive_msg_event, finish_event};
    while (true)
    {
        int event_index = WaitForMultipleObjects(2, hControlEvents, FALSE, INFINITE);     // Ждём сигнал от события
        switch (event_index)
        {
        case 0:// событие получения сообщения
        {
            shared_lock<shared_mutex> data_for_reading_lock(data_mtx); // общая блокировка, данные можно только читать из потоков

            auto received_message = ptr_to_message.lock(); // пытаемся получить доступ к сообщению из weak_ptr

            if(received_message == nullptr)  // если указатель оказался висячим и данные удалены
            {
                lock_guard<mutex> lock_console(console_mtx);
                cout << "ERROR: Thread №" + to_string(thread_id) + " can't read message, because the message was deleted" << endl;
            }
            else  
            {
                if(received_message->empty())  
                {
                    lock_guard<mutex> lock_console(console_mtx);
                    cout << "ERROR: Thread №" + to_string(thread_id) + " can't read message is empty" << endl;
                    
                }
                else
                {
                    unique_lock<mutex> console_lock(console_mtx);
                    cout << "Thread №" + to_string(thread_id) + " RECEIVED Message" << endl;
                    console_lock.unlock();

                    // вывод в файл
                    ofstream outfile;
                    outfile.open("C:/repository/SysProg/L4_SysProg/OutputData/" + to_string(thread_id) + ".txt");
                    if (outfile.is_open())
                    {
                        outfile << "Message size: " << received_message->size() << endl;
                        outfile << "Message:" << endl << *received_message;
                        outfile.close();
                    }
                }
            }
        }
        break;

        case 1: // событие завершения потока
        {
            lock_guard<mutex> lock_console(console_mtx);
            cout << "Thread №" + to_string(thread_id) + " IS CLOSED" << endl;
            SetEvent(confirm_finish_of_thread_event);
            return;
        }
        }
    }
}

void CloseAllObjects(list<HANDLE> handles)    // Освобождение русурсов всех объектов ядра
{
    for (auto event : handles)
    {
        CloseHandle(event);
    }
}

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
            setlocale(LC_ALL, "Russian");

            Server main_server;

            if (!main_server.StartUp())                   // Запускаем сервер
            {
                cout << "\tServer can not start!" << endl;
                cin;
                return 0;
            }

            ptr_global_message = make_shared<string>(); // память под будущие сообщения

            while (true)
            {
                unique_lock<mutex> console_lock(console_mtx);
                cout << ".............Searching new connection............." << endl;
                console_lock.unlock();

                main_server.WaitForConnection();

                console_lock.lock();
                cout << "New client connected to server!" << endl
                    << "Number of clients: " << main_server.GetClientsCount() << endl;
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