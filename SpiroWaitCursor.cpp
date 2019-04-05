#include "stdafx.h"
#include "SpiroWaitCursor.h"
#include "ChannelList.h"

/*!
* @author		RKH
* @date		06.06.2017
* @copyright (C)Spiro Medical AS 2013 - 2017
*/

CMyRegMan::CMyRegMan()
{
}
CMyRegMan::~CMyRegMan()
{
}

////////////////////////////////////


CSpiroWaitCursor::CSpiroWaitCursor()
{
	CXTRegistryManager reg(HKEY_LOCAL_MACHINE);
	CString file = reg.GetProfileString(regFilesStr,regProgramDataFolderStr,_T(""));
	file += _T("\\Spiro Animated Cursor.ani");

	cwP = NULL;
	HCURSOR hCursor = LoadCursorFromFile(file);  
    if(hCursor)  
        SetCursor(hCursor);  
    else 
		cwP = new CWaitCursor;
}

CSpiroWaitCursor::~CSpiroWaitCursor()
{
	if (cwP) delete cwP;
}

////////////////////////////////////////
//
//CSpiroRightCursor::CSpiroRightCursor()
//{
//	hCursor = NULL;
//	hCursor = LoadCursor(IDC_CURSOR_RIGHT); 
//}
//
//CSpiroRightCursor::~CSpiroRightCursor()
//{
//	delete hCursor;
//}
//
/////////////////////////////////////////
//
//CSpiroLeftCursor::CSpiroLeftCursor()
//{
//	hCursor = NULL;
//}
//
//CSpiroLeftCursor::~CSpiroLeftCursor()
//{
//}