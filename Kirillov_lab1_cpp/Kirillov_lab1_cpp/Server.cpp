#include "pch.h"
#include "Server.h"

extern mutex console_mtx;
extern shared_mutex data_mtx;
extern shared_ptr<string> ptr_global_message;
extern HANDLE confirm_finish_of_thread_event;

Server::Server(): _connections(),_working_threads()
{
}

Server::~Server()
{
}

// ������� ��������� ������������� �������, ����� ����������� � ������. 
void Server::ProcessClient(HANDLE hPipe, int client_id)
{
    header check_message = ReadHeader(hPipe);          
    if (check_message.task_code == Task::check_server)      // ���������� ������� ������������� �����������
    {
        confirm_header answer;
        answer.confirm_status = 1;
        answer.threads_count = _working_threads.GetCount();   // ������ ����� ������, ������� ������� ��������
        SendConfirm(hPipe, answer);
    }

    while (true)
    {
        //shared_lock<shared_mutex> read_threads_lock(mtx_for_working_threads);
        header client_header = ReadHeader(hPipe);
        lock_guard<mutex> lock_for_threads(mtx_for_working_threads);
        switch (client_header.task_code)
        {
        case Task::start_thread:         // ������� �������� ������
        {
            //read_threads_lock.mutex()->unlock_shared();
            //lock_guard<shared_mutex> update_threads_lock(mtx_for_working_threads);

            // ������ ������� �������, ������� ���� � �������
            for (int request_thrd_count = 0; request_thrd_count < client_header.message_size; request_thrd_count++)
            {
                std::unique_ptr<ThreadKirillov> new_thread = std::make_unique<ThreadKirillov>(); // ����� ������ ������

                // ��������� ��� ������
                int thread_id = _working_threads.GetCount() + 1;
                HANDLE thread_finish_event = CreateEventA(NULL, FALSE, FALSE, NULL);
                HANDLE thread_msg_event = CreateEventA(NULL, FALSE, FALSE, NULL);
                if (thread_finish_event == NULL || thread_msg_event == NULL)
                {
                    SendConfirm(hPipe, confirm_header{ 0,_working_threads.GetCount() }); // �������� ������� �� ������
                    break;
                }

                std::weak_ptr<string> wptr_to_message(ptr_global_message); // ��������� � ����� ���� ��������� �� ��������� �� ������

                // �������������� ������ �������� �������
                new_thread->Init(std::thread(ThreadFunction, thread_id, thread_finish_event, thread_msg_event, std::move(wptr_to_message)));
                new_thread->SetID(thread_id);
                new_thread->SetFinishEvent(thread_finish_event);
                new_thread->SetMessageEvent(thread_msg_event);

                _working_threads.AddThread(std::move(new_thread));   // �������� � ����� ��������� �������
            }
            
            //�������� ������������� � ��������� �������
            confirm_header header_for_client;
            header_for_client.confirm_status = 1;
            header_for_client.threads_count = _working_threads.GetCount();
            SendConfirm(hPipe, header_for_client);
        }
        break;

        case Task::stop_thread:              // ������� ���������� ������
        {
            //read_threads_lock.mutex()->unlock_shared();
            //lock_guard<shared_mutex> update_threads_lock(mtx_for_working_threads);
            if (_working_threads.GetCount() > 0)
            {
                _working_threads.FinishLastThread();
                WaitForSingleObject(confirm_finish_of_thread_event, INFINITE); // ��� ���������� ������
                _working_threads.DeleteLastThread();                            // ������ ����� ����� ����������� �������

                //�������� �������������
                confirm_header header_for_client;
                header_for_client.confirm_status = 1;
                header_for_client.threads_count = _working_threads.GetCount();
                SendConfirm(hPipe, header_for_client);
            }
            else
                SendConfirm(hPipe, confirm_header{ 0,_working_threads.GetCount() });
            
        }
        break;

        case Task::process_message:
        {
            std::string client_message = ReadMessage(hPipe, client_header);  // ������ ��������� �� ������������ ������
            if (client_message == "quit")
            {
                SendConfirm(hPipe, confirm_header{ 1,0 });
                DisconnectNamedPipe(hPipe);      // ���������� ������� �� �������
                CloseHandle(hPipe);
                CloseClient(client_id);         // ������� �� ��������� ����������
                lock_guard<mutex> console_lock(console_mtx);
                cout << "\tClient ID=" << client_id << " disconnected from server" << endl;
                return;
            }

            unique_lock<shared_mutex> writing_data_lock(data_mtx); // ����������� ������ �������� ��� ������ ������ ��������� 
                                                                // � ���� ������ ������ � ����� ����������� �� ������ ������
            *ptr_global_message = client_message;  
            writing_data_lock.unlock();                          // ����������� ����������� ������

            switch (client_header.thread_id)
            {
            case -1:                               // ������ �� ���� �������
            {
                ProcessMessage(ptr_global_message);
                _working_threads.ActionAll();
            }
            break;

            case 0:                                // ������ �� �������� ������
            {
                ProcessMessage(ptr_global_message);
            }
            break;

            default:                              // ������ �� ������������� ������
            {
                try
                {
                    _working_threads.ActionThreadByID(client_header.thread_id);
                }
                catch (exception ex)             // ����� ��� ������ � ������ id
                {
                    lock_guard<mutex> console_lock(console_mtx);
                    cout << ex.what() << endl;
                    SendConfirm(hPipe, confirm_header{ 0,_working_threads.GetCount() });    // �������� ������� �� ������
                }
            }
            }
            //�������� �������������
            confirm_header header_for_client;
            header_for_client.confirm_status = 1;
            header_for_client.threads_count = _working_threads.GetCount();
            SendConfirm(hPipe, header_for_client);
        }
        }
    }
}

// ������� �������� ������� �� ���������
void Server::CloseClient(int client_id)
{
    auto client = find_if(_connections.begin(), _connections.end(),
        [&](auto& connection) {return connection->GetID() == client_id; });

    if (client != _connections.end())
        _connections.erase(client);
}

// ������� ����������� ������ ������� � �������
void Server::WaitForConnection()
{
    HANDLE hPipe = CreateNamedPipeA("\\\\.\\pipe\\MyPipe_lab4", PIPE_ACCESS_DUPLEX, PIPE_TYPE_MESSAGE | PIPE_READMODE_MESSAGE | PIPE_WAIT,
        PIPE_UNLIMITED_INSTANCES, 1024, 1024, 0, NULL);

    if (!ConnectNamedPipe(hPipe, NULL))
    {
        std::lock_guard<std::mutex> console_lock(console_mtx);
        std::cout << "Server error connecting to client!" << std::endl;
    }
    else
    {
        auto new_connection = make_unique<Connection>();                                  // ������ ����� ����������
        new_connection->Start(&Server::ProcessClient, this, hPipe, new_connection->GetID());    // ��� ��������� ���������� � ��������� ������ 
        _connections.insert(std::move(new_connection));
    }
}

int Server::GetClientsCount()
{
    return _connections.size();
}

