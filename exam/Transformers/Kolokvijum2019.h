
// Kolokvijum2019.h : main header file for the Kolokvijum2019 application
//
#pragma once

#ifndef __AFXWIN_H__
	#error "include 'pch.h' before including this file for PCH"
#endif

#include "resource.h"       // main symbols


// CKolokvijum2019App:
// See Kolokvijum2019.cpp for the implementation of this class
//

class CKolokvijum2019App : public CWinApp
{
public:
	CKolokvijum2019App() noexcept;


// Overrides
public:
	virtual BOOL InitInstance();
	virtual int ExitInstance();

// Implementation
	afx_msg void OnAppAbout();
	DECLARE_MESSAGE_MAP()
};

extern CKolokvijum2019App theApp;
