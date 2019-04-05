#pragma once
#include "afxwin.h"

/*!
* @author		RKH
* @date		06.06.2017
* @copyright (C)Spiro Medical AS 2013 - 2017
*/

class CXTPEditPW : public CXTPEdit
{
public:
	CXTPEditPW();
	~CXTPEditPW();
	afx_msg void OnChar(UINT nChar, UINT nRepCnt, UINT nFlags);
protected:

	DECLARE_MESSAGE_MAP()
};

// CPWDialog dialog

class CPWDialog : public CDialog
{
	DECLARE_DYNAMIC(CPWDialog)

public:
	CPWDialog(CString _pw,CWnd* pParent = NULL);   // standard constructor
	virtual ~CPWDialog();
	CString getPW(bool *_savePW);
	void startWithBlankPW(void);
// Dialog Data
#ifdef AFX_DESIGN_TIME
	enum { IDD = IDD_PW_DIALOG2 };
#endif

protected:
	CString pw; 
	CString errmess;
	bool startingBlank;
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support

	DECLARE_MESSAGE_MAP()
protected:
	// Click to save password
	CXTPButton m_savePW;
	// Enter new PW here
	CXTPEditPW m_newPW;
	// Check this to show password
	CXTPButton m_showPW;

	bool passwordOK;
	bool savePW;
public:
	virtual BOOL OnInitDialog();
	afx_msg void OnClickedShowPw();
	afx_msg void OnKillfocusEditPw();
	virtual BOOL PreTranslateMessage(MSG* pMsg);
	virtual void OnOK();
	afx_msg void OnClickedSavePw();
	afx_msg void OnPWUdate();
	afx_msg void OnChar(UINT nChar, UINT nRepCnt, UINT nFlags);
};
