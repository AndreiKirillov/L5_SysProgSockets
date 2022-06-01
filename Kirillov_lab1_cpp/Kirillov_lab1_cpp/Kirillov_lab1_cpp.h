#pragma once

#include "resource.h"
#include "framework.h"

using namespace std;

// ��� ������ ��� ��������� ��������
enum class Task
{
	start_thread, stop_thread, process_message, check_server
};

struct header // ��������� ��� ���������
{
    Task task_code;
    int thread_id;
    int message_size;
};

struct confirm_header // ��������� ��� �������������
{
    int confirm_status;
    int threads_count;
};

void ThreadFunction(int thread_id, HANDLE finish_event, HANDLE receive_msg_event, weak_ptr<string>&& ptr_to_message);

void ProcessMessage(shared_ptr<string>& message);
