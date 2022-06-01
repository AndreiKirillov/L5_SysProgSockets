#pragma once
#include "framework.h"

// �����, �������������� ������ ������ ������ ��� ������ � ���, ������ �������������, ����� ������ � ����� ������������ �������,
// ������� ��������� ���������� ������
class ThreadKirillov
{
private:
	int _id;
	std::thread _thread;
	HANDLE _finish_event;
	HANDLE _receive_msg_event;

	ThreadKirillov(ThreadKirillov&);            // ��������� ����������� � ������������
	ThreadKirillov(const ThreadKirillov&);      // ������ ������������ ����������
	ThreadKirillov& operator=(ThreadKirillov&);
public:
	ThreadKirillov();
	~ThreadKirillov();

	void SetID(int id);
	int GetID() const;
	void SetFinishEvent(HANDLE fin_event);
	void SetMessageEvent(HANDLE msg_event);
	
	void Init(std::thread&& some_thread) noexcept;
	void Finish();

	void Activate();
};

