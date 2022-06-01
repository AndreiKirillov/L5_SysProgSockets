// sockets.h: основной файл заголовка для библиотеки DLL sockets
//

#pragma once

#ifndef __AFXWIN_H__
	#error "включить pch.h до включения этого файла в PCH"
#endif

#include "resource.h"		// основные символы


// CsocketsApp
// Реализацию этого класса см. в файле sockets.cpp
//

class CsocketsApp : public CWinApp
{
public:
	CsocketsApp();

// Переопределение
public:
	virtual BOOL InitInstance();

	DECLARE_MESSAGE_MAP()
};
