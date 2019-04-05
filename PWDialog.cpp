// PWDialog.cpp : implementation file
//

#include "stdafx.h"
#include "PWDialog.h"
#include "afxdialogex.h"
#include "Anthropologia.h"


/*!
* @author		RKH
* @date		21.11.2018
* @copyright (C)Spiro Medical AS 2013 - 2018
*/

// CPWDialog dialog

IMPLEMENT_DYNAMIC(CPWDialog, CDialog)

CPWDialog::CPWDialog(CString _pw,CWnd* pParent /*=NULL*/)
	: pw(_pw),CDialog(IDD_PW_DIALOG2, pParent)
{
	pw.TrimRight(ASCII_SPACE);	// This will be filled up with spaces again when key is set
								// in AES128.cpp
	passwordOK = CPatient::getIsPasswordOK(pw,&errmess);
	savePW = false;
	startingBlank = false;
}

CPWDialog::~CPWDialog()
{
}

CString CPWDialog::getPW(bool *_savePW)
{
	*_savePW = savePW;
	return pw;
}

void CPWDialog::startWithBlankPW(void)
{
	pw = _T("");
	savePW = true;
	startingBlank = true;
}

void CPWDialog::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	DDX_Control(pDX, IDC_SAVE_PW2, m_savePW);
	DDX_Control(pDX, IDC_EDIT_PW2, m_newPW);
	DDX_Control(pDX, IDC_SHOW_PW2, m_showPW);
}


BEGIN_MESSAGE_MAP(CPWDialog, CDialog)
	ON_BN_CLICKED(IDC_SHOW_PW2, &CPWDialog::OnClickedShowPw)
	ON_EN_KILLFOCUS(IDC_EDIT_PW2, &CPWDialog::OnKillfocusEditPw)
	ON_BN_CLICKED(IDC_SAVE_PW2, &CPWDialog::OnClickedSavePw)
	ON_EN_UPDATE(IDC_EDIT_PW2,&CPWDialog::OnPWUdate)
	ON_WM_CHAR()
END_MESSAGE_MAP()


// CPWDialog message handlers


BOOL CPWDialog::OnInitDialog()
{
	CDialog::OnInitDialog();

	// TODO:  Add extra initialization here
	m_newPW.LimitText(16);
	m_newPW.SetWindowText(pw);

	CButton *bt = (CButton *)GetDlgItem(IDOK);
	passwordOK = CPatient::getIsPasswordOK(pw,&errmess);
	bt->EnableWindow(passwordOK ? TRUE : FALSE);
	m_savePW.EnableWindow(passwordOK ? TRUE : FALSE);

	if (startingBlank) m_savePW.ShowWindow(FALSE);
	else m_savePW.SetChecked(savePW ? TRUE : FALSE);

	m_showPW.SetChecked(FALSE);

	return TRUE;  // return TRUE unless you set the focus to a control
				  // EXCEPTION: OCX Property Pages should return FALSE
}


void CPWDialog::OnClickedShowPw()
{
	// TODO: Add your control notification handler code here
	if (m_showPW.GetChecked()) m_newPW.SetPasswordChar(0);
	else 
		m_newPW.SetPasswordChar(_T('*'));

	m_newPW.RedrawWindow();
}


void CPWDialog::OnKillfocusEditPw()
{
	// TODO: Add your control notification handler code here
	CString newpw;
	m_newPW.GetWindowText(newpw);
	if (newpw != pw) {
		pw = newpw;
		CButton *bt = (CButton *)GetDlgItem(IDOK);
		passwordOK = CPatient::getIsPasswordOK(pw, &errmess);
		bt->EnableWindow(passwordOK ? TRUE : FALSE); 
		if (!passwordOK) {
			m_savePW.SetChecked(FALSE);
			AfxMessageBox(errmess);
		}
		m_savePW.EnableWindow(passwordOK ? TRUE : FALSE);
	}
}


BOOL CPWDialog::PreTranslateMessage(MSG* pMsg)
{
	// TODO: Add your specialized code here and/or call the base class
	if (VK_RETURN == pMsg->wParam) {

		CString newpw;
		m_newPW.GetWindowText(newpw);
		if (newpw != pw) {
			pw = newpw;
			CButton *bt = (CButton *)GetDlgItem(IDOK);
			passwordOK = CPatient::getIsPasswordOK(pw, &errmess);
			bt->EnableWindow(passwordOK ? TRUE : FALSE);
			if (!passwordOK) {
				m_savePW.SetChecked(FALSE);
				AfxMessageBox(errmess);
			}
			m_savePW.EnableWindow(passwordOK ? TRUE : FALSE);
		}
		if (!passwordOK) return FALSE;
		else return TRUE;
	}
	return CDialog::PreTranslateMessage(pMsg);
}


void CPWDialog::OnOK()
{
	// TODO: Add your specialized code here and/or call the base class
	m_newPW.GetWindowText(pw);
	CButton *bt = (CButton *)GetDlgItem(IDOK);
	passwordOK = CPatient::getIsPasswordOK(pw, &errmess);
	bt->EnableWindow(passwordOK ? TRUE : FALSE);
	m_savePW.EnableWindow(passwordOK ? TRUE : FALSE);
	if (!passwordOK) {
		AfxMessageBox(errmess);
		return;
	}

	CDialog::OnOK();
}

void CPWDialog::OnPWUdate()
{
	CString s;
	m_newPW.GetWindowText(s);

	CButton *bt = (CButton *)GetDlgItem(IDOK);
	passwordOK = CPatient::getIsPasswordOK(s, &errmess);
	bt->EnableWindow(passwordOK ? TRUE : FALSE);
	m_savePW.EnableWindow(passwordOK ? TRUE : FALSE);
}

void CPWDialog::OnClickedSavePw()
{
	// TODO: Add your control notification handler code here
	savePW = TRUE == m_savePW.GetChecked();
}


void CPWDialog::OnChar(UINT nChar, UINT nRepCnt, UINT nFlags)
{
	// TODO: Add your message handler code here and/or call default
	

	CDialog::OnChar(nChar, nRepCnt, nFlags);
}


////////////////////////////////////////////////////

CXTPEditPW::CXTPEditPW()
{
}

CXTPEditPW::~CXTPEditPW()
{
}

BEGIN_MESSAGE_MAP(CXTPEditPW, CXTPEdit)
	ON_WM_CHAR()
END_MESSAGE_MAP()

void CXTPEditPW::OnChar(UINT nChar, UINT nRepCnt, UINT nFlags)
{
	if ((nChar <= 'Z') && (nChar >= 'A')) return CXTPEdit::OnChar(nChar, nRepCnt, nFlags);
	if ((nChar <= 'z') && (nChar >= 'a')) return CXTPEdit::OnChar(nChar, nRepCnt, nFlags);
	if ((nChar <= '9') && (nChar >= '0')) return CXTPEdit::OnChar(nChar, nRepCnt, nFlags);

	//if (VK_DELETE == nChar) return CXTPEdit::OnChar(nChar, nRepCnt, nFlags);
	if (VK_BACK == nChar) return CXTPEdit::OnChar(nChar, nRepCnt, nFlags);
}
