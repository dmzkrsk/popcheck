// reg_dll.h : main header file for the REG_DLL DLL
//

#if !defined(AFX_REG_DLL_H__ABDEA9A4_EE76_46A0_B5D6_5E4613A7F15E__INCLUDED_)
#define AFX_REG_DLL_H__ABDEA9A4_EE76_46A0_B5D6_5E4613A7F15E__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#ifndef __AFXWIN_H__
	#error include 'stdafx.h' before including this file for PCH
#endif

#include "resource.h"		// main symbols

/////////////////////////////////////////////////////////////////////////////
// CReg_dllApp
// See reg_dll.cpp for the implementation of this class
//

class CReg_dllApp : public CWinApp
{
public:
	CReg_dllApp();

// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CReg_dllApp)
	//}}AFX_VIRTUAL

	//{{AFX_MSG(CReg_dllApp)
		// NOTE - the ClassWizard will add and remove member functions here.
		//    DO NOT EDIT what you see in these blocks of generated code !
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};


/////////////////////////////////////////////////////////////////////////////

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_REG_DLL_H__ABDEA9A4_EE76_46A0_B5D6_5E4613A7F15E__INCLUDED_)
