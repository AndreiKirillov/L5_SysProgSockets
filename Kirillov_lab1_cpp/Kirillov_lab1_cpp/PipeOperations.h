#pragma once
#include "framework.h"
#include "Kirillov_lab1_cpp.h"

using namespace std;

inline header ReadHeader(HANDLE hPipe)
{
	DWORD dwDone;
	header h;
	ReadFile(hPipe, &h, sizeof(header), &dwDone, NULL);
	return h;
}

inline string ReadMessage(HANDLE hPipe, const header& h)
{
	DWORD dwDone;
	vector <char> v(h.message_size);
	ReadFile(hPipe, &v[0], h.message_size, &dwDone, NULL);
	return string(&v[0], h.message_size);
}

inline void SendConfirm(HANDLE hPipe, const confirm_header& h)     // Функция отправки подтверждения клиенту
{
	DWORD dwDone;

	WriteFile(hPipe, &h, sizeof(confirm_header), &dwDone, NULL);
	//if (bFlush)
		//FlushFileBuffers(hPipe);
}

