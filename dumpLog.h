#pragma once

#include "MibAccess.h"
#include "SysInfo.h"
#include <dxgi.h>
#include <vector>

/*!
* @author		RKH
* @date		06.06.2017
* @copyright (C)Spiro Medical AS 2013 - 2017
*/

using namespace std;


class CDumpLog
{
public: 
	CDumpLog();
	void dump(CString _s);
	CString getFileName();
protected:

#ifndef IN_I2CCOMM
	CSysInfo m_si;
#endif

	CString fileName;
	BOOL adapterInfo(CStdioFile *_f);
	BOOL gAdapterInfo(CStdioFile *_f);
	BOOL sysInfo(CStdioFile *_f); 
	vector <IDXGIAdapter*> EnumerateAdapters(CStdioFile *_f);
	CString getVersionInfo(void);
	CString appName;
};