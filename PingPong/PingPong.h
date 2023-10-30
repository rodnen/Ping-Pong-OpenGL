
// PingPong.h: основной файл заголовка для приложения PingPong
//
#pragma once

#ifndef __AFXWIN_H__
	#error "включить pch.h до включения этого файла в PCH"
#endif

#include "resource.h"       // основные символы


// CPingPongApp:
// Сведения о реализации этого класса: PingPong.cpp
//

class CPingPongApp : public CWinAppEx
{
public:
	CPingPongApp() noexcept;


// Переопределение
public:
	virtual BOOL InitInstance();
	virtual int ExitInstance();

// Реализация
	BOOL  m_bHiColorIcons;

	virtual void PreLoadState();
	virtual void LoadCustomState();
	virtual void SaveCustomState();

	afx_msg void OnAppAbout();
	DECLARE_MESSAGE_MAP()
};

extern CPingPongApp theApp;
