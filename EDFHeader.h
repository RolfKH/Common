#pragma once
/*
Copyright 2011. Spiro Medical AS

Software for viewing EDF header
Rev	By	Description
0	RKH	Initial version
*/
#include "EDF.h"
#include "CommonResource.h"

// CEDFHeader dialog

class CEDFHeader : public CDialog
{
	DECLARE_DYNAMIC(CEDFHeader)

public:
	CEDFHeader(CString _fname,EDF_BINARY_HEADER _binHeader,EDF_BINARY_RECORD *_binRecords,CWnd* pParent = NULL);   // standard constructor
	virtual ~CEDFHeader();

// Dialog Data
	enum { IDD = IDD_EDF_HEADER };

protected:
	
	void adjustLayout();
	void fillView();
	EDF_BINARY_HEADER binHeader;
	EDF_BINARY_RECORD *binRecords;
	CString fileName;

	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	afx_msg int OnCreate(LPCREATESTRUCT lpCreateStruct);
	afx_msg void OnSize(UINT nType, int cx, int cy);
	afx_msg void OnReportItemClick(NMHDR * pNotifyStruct, LRESULT * result);
	
	CXTPOfficeBorder<CXTPReportControl,false> table;

	DECLARE_MESSAGE_MAP()
};

class CEDFHeader2 : public CDialog
{
	DECLARE_DYNAMIC(CEDFHeader2)

public:
	CEDFHeader2(CString _fname,EDF_BINARY_HEADER _binHeader,EDF_BINARY_RECORD *_binRecords,CWnd* pParent = NULL);   // standard constructor
	virtual ~CEDFHeader2();

// Dialog Data
	enum { IDD = IDD_EDF_HEADER };

protected:
	
	void adjustLayout();
	void fillView();
	EDF_BINARY_HEADER binHeader;
	EDF_BINARY_RECORD *binRecords;
	CString fileName;
	UINT parChngRedrawMessage;

	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	afx_msg int OnCreate(LPCREATESTRUCT lpCreateStruct);
	afx_msg void OnSize(UINT nType, int cx, int cy);
	afx_msg LRESULT OnGridNotify(WPARAM, LPARAM);
	
	CXTPOfficeBorder <CXTPPropertyGrid,false> table;

	DECLARE_MESSAGE_MAP()
};