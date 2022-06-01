#pragma once
#include "framework.h"
#include "ThreadKirillov.h"

// �����, ����������� ������� ������ � �������� � ����
class ThreadStorage
{
private:
	std::vector<std::unique_ptr<ThreadKirillov>> _threads;

	ThreadStorage(ThreadStorage&);               // ��������� ����������� � ������������
	ThreadStorage(const ThreadStorage&);		 // ������ ������������ ����������
	ThreadStorage& operator=(ThreadStorage&);
public:
	ThreadStorage();

	int GetCount() const;

	void AddThread(std::unique_ptr<ThreadKirillov>&& somethread);

	void FinishLastThread();
	void DeleteLastThread();
	void KillAndReleaseAll();

	void ActionAll();
	void ActionThreadByID(int id);

};

