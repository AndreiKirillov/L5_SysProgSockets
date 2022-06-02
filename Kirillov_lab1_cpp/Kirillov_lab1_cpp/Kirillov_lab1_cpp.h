#pragma once

#include "resource.h"
#include "framework.h"
#include <afxsock.h>

using namespace std;

// тип задачи для обработки сервером
enum class Task
{
	start_thread, stop_thread, process_message, check_server
};

struct header // заголовок для сообщения
{
    Task task_code;
    int thread_id;
    int message_size;
};

struct confirm_header // заголовок для подтверждения
{
    int confirm_status;
    int threads_count;
};

void ThreadFunction(int thread_id, HANDLE finish_event, HANDLE receive_msg_event, weak_ptr<string>&& ptr_to_message);

void ProcessMessage(shared_ptr<string>& message);

inline header ReadHeader(CSocket& reading_source)
{
	header header_from_client;

	reading_source.Receive(&header_from_client, sizeof(header));
	return header_from_client;
}

inline std::string ReadMessage(CSocket& reading_source, const header& h)
{
	std::vector <char> v(h.message_size);

	reading_source.Receive(&v[0], h.message_size);

	return std::string(&v[0], h.message_size);
}

inline void SendConfirm(CSocket& sending_sock, const confirm_header& h)     // Функция отправки подтверждения клиенту
{
	sending_sock.Send(&h, sizeof(confirm_header));
}

