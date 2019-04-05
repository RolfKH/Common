#include "StdAfx.h"
#include "dumpLog.h"
#include "..\Common\Events.h"


/*!
* @author		RKH
* @date		06.06.2017
* @copyright (C)Spiro Medical AS 2013 - 2017
*/

#include <winsock2.h>
#include <iphlpapi.h>
#include <stdio.h>
#include <stdlib.h>
#pragma comment(lib, "IPHLPAPI.lib")

#include <strsafe.h>

#define MALLOC(x) HeapAlloc(GetProcessHeap(), 0, (x))
#define FREE(x) HeapFree(GetProcessHeap(), 0, (x))

extern int dumpToLog;

CDumpLog::CDumpLog()
{
#ifndef IN_I2CCOMM
	if (!dumpToLog)
		return;
	CString heading;

#ifdef IN_AGS_BROWSER
	heading = _T("Log file for AGS Browser\n");
	appName = _T("AGSBrowser.exe");
#else
	heading = _T("Log file for ApneaGraph Spiro Analysis\n");
	appName = _T("SpiroAnalysis.exe");
#endif

	m_si.Init();

	wstring TempPath;
	wchar_t wcharPath[MAX_PATH];
	if (GetTempPathW(MAX_PATH, wcharPath))
		TempPath = wcharPath;

	CString folder(TempPath.c_str());
	WIN32_FIND_DATA FindFileData;
	int i = 0;
	HANDLE hFind = NULL;
	CString s;
	do {

#ifdef IN_AGS_BROWSER
		s.Format(_T("AGSBrowserDump%03d.txt"), i++);
#else
		s.Format(_T("AGSAnalysisDump%03d.txt"), i++);
#endif

		fileName = folder;
		fileName += s;
		hFind = FindFirstFile(fileName, &FindFileData);
	} while (INVALID_HANDLE_VALUE != hFind);

	CStdioFile f;
	BOOL OK = f.Open(fileName, CFile::modeCreate | CFile::modeWrite);
	if (!OK) {
		CString mess = _T("Could not open file ");
		mess += fileName;
		mess += _T(" for writing dump log data");
		AfxMessageBox(mess);
	}
	else {
		f.WriteString(heading);
		f.WriteString(getVersionInfo());
		CTime dt = CTime::GetCurrentTime();
		CString ts = dt.Format(_T("%A, %B %d, %Y %H:%M:%S\n"));
		f.WriteString(ts);

		wchar_t user[256];
		DWORD len = 256;
		GetUserName(user, &len);
		wstring u1 = user;
		CString u2(u1.c_str());
		u2 += _T("\n");
		f.WriteString(_T("User : "));
		f.WriteString(u2);

		sysInfo(&f);
		gAdapterInfo(&f);

		// Device manager info: Windows Management Instrumentation (WMI) 
		// msdn.microsoft.com/en-us/library/aa390418(v=vs.85).aspx 

		f.WriteString(_T("-------------------------------------------------\n"));
		f.Close();
	}
#endif
}

BOOL CDumpLog::sysInfo(CStdioFile *_f)
{
#ifndef IN_I2CCOMM
	if (m_si.IsUserAdmin()) _f->WriteString(_T("...User is admin\n")); 
	else _f->WriteString(_T("...User is not admin\n"));

	if (m_si.IsWindowsNT()) _f->WriteString(_T("...Windows NT\n"));
	else _f->WriteString(_T("...Not Windows NT\n"));

	CString s;
	s.Format(_T("...Windows major %d, minor %d, build %d, service pack %d, OS type %s\n"),
		m_si.GetWinMajor(), m_si.GetWinMinor(), m_si.GetBuildNumber(), m_si.GetServicePack(), m_si.GetOSType());
	_f->WriteString(s);
	
	s.Format(_T("...CPU name      : %s\n"), m_si.GetCPUNameString());
	_f->WriteString(s);
	s.Format(_T("...CPU identifier: %s\n"), m_si.GetCPUIdentifier());
	_f->WriteString(s);
	s.Format(_T("...CPU speed     : %d\n"), m_si.GetCPUSpeed());
	_f->WriteString(s);
	s.Format(_T("...CPU vendor    : %s\n"), m_si.GetCPUVendorIdentifier());
	_f->WriteString(s);

#endif
	return TRUE;
}

BOOL CDumpLog::gAdapterInfo(CStdioFile *_f)
{
	EnumerateAdapters(_f);
	return TRUE;
}

vector <IDXGIAdapter*> CDumpLog::EnumerateAdapters(CStdioFile *_f)
{
	IDXGIAdapter * pAdapter;
	std::vector <IDXGIAdapter*> vAdapters;
	IDXGIFactory* pFactory = NULL;


	// Create a DXGIFactory object.
	if (FAILED(CreateDXGIFactory(__uuidof(IDXGIFactory), (void**)&pFactory)))
	{
		return vAdapters;
	}


	for (UINT i = 0;
	pFactory->EnumAdapters(i, &pAdapter) != DXGI_ERROR_NOT_FOUND;
		++i)
	{
		DXGI_ADAPTER_DESC ad;
		pAdapter->GetDesc(&ad);
		CString s(ad.Description);
		CString sf;
		sf.Format(_T("___Graphics adapter %2d : "),i);
		sf += s;
		sf += _T("\n");
		_f->WriteString(sf);

		vAdapters.push_back(pAdapter);
	}


	if (pFactory)
	{
		pFactory->Release();
	}

	return vAdapters;

}

BOOL CDumpLog::adapterInfo(CStdioFile *_f)
{
#ifndef IN_I2CCOMM
		/* Declare and initialize variables */

		// It is possible for an adapter to have multiple
		// IPv4 addresses, gateways, and secondary WINS servers
		// assigned to the adapter. 
		//
		// Note that this sample code only prints out the 
		// first entry for the IP address/mask, and gateway, and
		// the primary and secondary WINS server for each adapter. 

		_f->WriteString(_T("IP Adapter info:\n"));
		_f->WriteString(_T("********************\n"));

		PIP_ADAPTER_INFO pAdapterInfo;
		PIP_ADAPTER_INFO pAdapter = NULL;
		DWORD dwRetVal = 0;
		UINT i;

		/* variables used to print DHCP time info */
		struct tm newtime;
		char buffer[32];
		errno_t error;

		ULONG ulOutBufLen = sizeof(IP_ADAPTER_INFO);
		pAdapterInfo = (IP_ADAPTER_INFO *)MALLOC(sizeof(IP_ADAPTER_INFO));
		if (pAdapterInfo == NULL) {
			_f->WriteString(_T("Error allocating memory needed to call GetAdaptersinfo\n"));
			return 1;
		}
		// Make an initial call to GetAdaptersInfo to get
		// the necessary size into the ulOutBufLen variable
		if (GetAdaptersInfo(pAdapterInfo, &ulOutBufLen) == ERROR_BUFFER_OVERFLOW) {
			FREE(pAdapterInfo);
			pAdapterInfo = (IP_ADAPTER_INFO *)MALLOC(ulOutBufLen);
			if (pAdapterInfo == NULL) {
				_f->WriteString(_T("Error allocating memory needed to call GetAdaptersinfo\n"));
				return 1;
			}
		}

		if ((dwRetVal = GetAdaptersInfo(pAdapterInfo, &ulOutBufLen)) == NO_ERROR) {
			pAdapter = pAdapterInfo;
			CString s;
			while (pAdapter) {
				CString aN(pAdapter->AdapterName);
				CString aD(pAdapter->Description);
				s.Format(_T("\tComboIndex: \t%d\n"), pAdapter->ComboIndex);
				_f->WriteString(s);
				s.Format(_T("\tAdapter Name: \t%s\n"), static_cast<LPCTSTR>(aN));
				_f->WriteString(s);
				s.Format(_T("\tAdapter Desc: \t%s\n"), static_cast<LPCTSTR>(aD));
				_f->WriteString(s);
				_f->WriteString(_T("\tAdapter Addr: \t"));
				for (i = 0; i < pAdapter->AddressLength; i++) {
					if (i == (pAdapter->AddressLength - 1)) {
						s.Format(_T("%.2X\n"), (int)pAdapter->Address[i]);
						_f->WriteString(s);
					}
					else {
						s.Format(_T("%.2X-"), (int)pAdapter->Address[i]);
						_f->WriteString(s);
					}
				}
				s.Format(_T("\tIndex: \t%d\n"), pAdapter->Index);
				_f->WriteString(s);
				_f->WriteString(_T("\tType: \t"));
				switch (pAdapter->Type) {
				case MIB_IF_TYPE_OTHER:
					_f->WriteString(_T("Other\n"));
					break;
				case MIB_IF_TYPE_ETHERNET:
					_f->WriteString(_T("Ethernet\n"));
					break;
				case MIB_IF_TYPE_TOKENRING:
					_f->WriteString(_T("Token Ring\n"));
					break;
				case MIB_IF_TYPE_FDDI:
					_f->WriteString(_T("FDDI\n"));
					break;
				case MIB_IF_TYPE_PPP:
					_f->WriteString(_T("PPP\n"));
					break;
				case MIB_IF_TYPE_LOOPBACK:
					_f->WriteString(_T("Lookback\n"));
					break;
				case MIB_IF_TYPE_SLIP:
					_f->WriteString(_T("Slip\n"));
					break;
				default:
					s.Format(_T("Unknown type %ld\n"), pAdapter->Type);
					_f->WriteString(s);
					break;
				}

				CString aIIS(pAdapter->IpAddressList.IpAddress.String);
				CString aIIMS(pAdapter->IpAddressList.IpMask.String);
				CString GIS(pAdapter->GatewayList.IpAddress.String);
				s.Format(_T("\tIP Address: \t%s\n"), static_cast<LPCTSTR>(aIIS));
				_f->WriteString(s);
				s.Format(_T("\tIP Mask: \t%s\n"), static_cast<LPCTSTR>(aIIMS));
				_f->WriteString(s);
				s.Format(_T("\tGateway: \t%s\n"), static_cast<LPCTSTR>(GIS));
				_f->WriteString(s);
				_f->WriteString(_T("\t***\n"));

				if (pAdapter->DhcpEnabled) {
					_f->WriteString(_T("\tDHCP Enabled: Yes\n"));
					CString s1(pAdapter->DhcpServer.IpAddress.String);
					s.Format(_T("\t  DHCP Server: \t%s\n"), static_cast<LPCTSTR>(s1));
					_f->WriteString(s);
					_f->WriteString(_T("\t  Lease Obtained: "));
					/* Display local time */
					error = _localtime32_s(&newtime, (__time32_t*)&pAdapter->LeaseObtained);
					if (error)
						_f->WriteString(_T("Invalid Argument to _localtime32_s\n"));
					else {
						// Convert to an ASCII representation 
						error = asctime_s(buffer, 32, &newtime);
						if (error)
							_f->WriteString(_T("Invalid Argument to asctime_s\n"));
						else {
							/* asctime_s returns the string terminated by \n\0 */
							CString b1(buffer);
							s.Format(_T("%s"), static_cast<LPCTSTR>(b1));
							_f->WriteString(s);
						}
					}

					_f->WriteString(_T("\t  Lease Expires:  "));
					error = _localtime32_s(&newtime, (__time32_t*)&pAdapter->LeaseExpires);
					if (error)
						_f->WriteString(_T("Invalid Argument to _localtime32_s\n"));
					else {
						// Convert to an ASCII representation 
						error = asctime_s(buffer, 32, &newtime);
						if (error)
							_f->WriteString(_T("Invalid Argument to asctime_s\n"));
						else {
							/* asctime_s returns the string terminated by \n\0 */
							CString b1(buffer);
							s.Format(_T("%s"), static_cast<LPCTSTR>(b1));
							_f->WriteString(s);
						}
					}
				}
				else
					_f->WriteString(_T("\tDHCP Enabled: No\n"));

				if (pAdapter->HaveWins) {
					_f->WriteString(_T("\tHave Wins: Yes\n"));
					CString s1(pAdapter->PrimaryWinsServer.IpAddress.String);
					CString s2(pAdapter->SecondaryWinsServer.IpAddress.String);
					s.Format(_T("\t  Primary Wins Server:    %s\n"), static_cast<LPCTSTR>(s1));
					_f->WriteString(s);
					s.Format(_T("\t  Secondary Wins Server:  %s\n"), static_cast<LPCTSTR>(s2));
					_f->WriteString(s);
				}
				else
					_f->WriteString(_T("\tHave Wins: No\n"));
				pAdapter = pAdapter->Next;
				_f->WriteString(_T("\n"));
			}
		}
		else {
			CString s;
			s.Format(_T("GetAdaptersInfo failed with error: %d\n"), dwRetVal);
			_f->WriteString(s);
		}
		if (pAdapterInfo)
			FREE(pAdapterInfo);

		_f->WriteString(_T("********************\n"));
#endif
		return 0;
}

CString CDumpLog::getFileName(void)
{
	return fileName;
}

void CDumpLog::dump(CString _s)
{
#ifndef IN_I2CCOMM
	if (!dumpToLog) 
		return;
	if (fileName.IsEmpty())
		return;

	CStdioFile f;
	BOOL OK = f.Open(fileName, CFile::modeWrite | CFile::modeNoTruncate);
	if (!OK) {
		CString mess = _T("Could write to file ");
		mess += fileName;
		mess += _T(" for writing dump log data");
		AfxMessageBox(mess);
	}
	else {
		f.Seek(0, CFile::end);
		CString s = _s;
		s += _T("\n");
		f.WriteString(s);
		f.Close();
	}
#endif
}

CString CDumpLog::getVersionInfo(void)
{
	CString vstr = _T("Unknown version and build");

	DWORD handle;
	DWORD size = ::GetFileVersionInfoSize(appName, &handle);
	if (size) {
		void *pBlock = (void *)malloc(size);
		BOOL ok = FALSE;
		if (NULL == pBlock) {
			free(pBlock);
			return vstr;
		}
		if (!handle) {
			ok = ::GetFileVersionInfo(appName,handle, size, pBlock); // Param 2 is ignored
		}
		else return vstr;

		if (ok) {
			HRESULT hr;
			struct LANGANDCODEPAGE {
				WORD wLanguage;
				WORD wCodePage;
			} *lpTranslate;

			// Read the list of languages and code pages.
			UINT cbTranslate;
			VerQueryValue(pBlock,
				TEXT("\\VarFileInfo\\Translation"),
				(LPVOID*)&lpTranslate,
				&cbTranslate);

			// Read the file description for each language and code page.
			// If there are more languages and codepages, the strings will 
			// be set to the last one
			for (unsigned i = 0; i < (cbTranslate / sizeof(struct LANGANDCODEPAGE)); i++) {
				TCHAR SubBlock[60];

				//---Copyright
				hr = StringCchPrintf(SubBlock, 50,
					TEXT("\\StringFileInfo\\%04x%04x\\LegalCopyright"),
					lpTranslate[i].wLanguage,
					lpTranslate[i].wCodePage);
				if (FAILED(hr)) break;
				// Retrieve copyright for language and code page "i". 
				UINT dwBytes;
				TCHAR *lpBuffer;
				VerQueryValue(pBlock,
					SubBlock,
					(LPVOID *)&lpBuffer,
					&dwBytes);

				//---Product name
				hr = StringCchPrintf(SubBlock, 50,
					TEXT("\\StringFileInfo\\%04x%04x\\ProductName"),
					lpTranslate[i].wLanguage,
					lpTranslate[i].wCodePage);
				if (FAILED(hr)) break;
				VerQueryValue(pBlock,
					SubBlock,
					(LPVOID *)&lpBuffer,
					&dwBytes);

				//---Version
				hr = StringCchPrintf(SubBlock, 50,
					TEXT("\\StringFileInfo\\%04x%04x\\ProductVersion"),
					lpTranslate[i].wLanguage,
					lpTranslate[i].wCodePage);
				if (FAILED(hr)) break;
				VerQueryValue(pBlock,
					SubBlock,
					(LPVOID *)&lpBuffer,
					&dwBytes);

				vstr = _T("Version : ");
				vstr += _T(" ");
				vstr += lpBuffer;
				char buildDate[128] = __DATE__;
				CString bs;
				bs = _T(" Built : ");
				vstr += _T("  ");
				vstr += bs;
				vstr += _T("  ");
				vstr += buildDate;
				vstr += _T("\n");
			}
		}
		free(pBlock);
	}
	return vstr;
}