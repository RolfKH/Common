#pragma once

/*!
* @author		RKH
* @date		06.06.2017
* @copyright (C)Spiro Medical AS 2013 - 2017
*/

#include "CommonResource.h"
#include "GraphVisualProperties.h"


// CGraphSettings dialog

class CGraphSettings : public CXTResizeDialog
{
	DECLARE_DYNAMIC(CGraphSettings)

public:
	CGraphSettings(CWnd* pParent = NULL);   // standard constructor
	virtual ~CGraphSettings();

// Dialog Data
	enum { IDD = IDD_GRAPH_SETTINGS };

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support

	CXTPOfficeBorder <CXTPPropertyGrid,false> m_gProps;

	afx_msg LRESULT OnGridNotify(WPARAM, LPARAM);

	CArray <CButtonPlotSettings *,CButtonPlotSettings *> settingsArray;
	CWnd *myCreator;
	bool getUseThis(CString const _s);

public:
	DECLARE_MESSAGE_MAP()
	virtual BOOL OnInitDialog();
	virtual void OnOK();
	virtual BOOL DestroyWindow();
};
