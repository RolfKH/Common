#include "StdAfx.h"
#include "dumpLog.h"
#include "Events.h"
#include "RegDefs.h"

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
	fileCanBeUsed = false;

#ifndef IN_I2CCOMM
	if (!dumpToLog)
		return;
	CString heading;
	CString fnType;
#ifdef IN_AGS_BROWSER
	heading = _T("<AGSLogFile>\n<Type>Log file for AGS Browser</Type>\n");
	appName = _T("AGSBrowser.exe\n");
	fnType = _T("Browser");
#else
	heading = _T("<AGSLogFile>\n<Type>Log file for ApneaGraph Spiro Analysis</Type>\n");
	appName = _T("SpiroAnalysis.exe");
	fnType = _T("Analysis");
#endif

	m_si.Init();

#ifdef IN_AGS_BROWSER
	fileName = _T("C:\\ProgramData\\Spiro Medical\\Spiro Analysis\\LogfileBrowser.xml");
#else 
	//---Find file path
	CXTPRegistryManager reg(HKEY_LOCAL_MACHINE);
	CString s = reg.GetProfileString(regFiles, regProgramDataFolder, _T(""));
	if (s.IsEmpty()) {
		fileCanBeUsed = false;
		return;
	}
	fileName = s;
	fileName += LOG_FILE;
#endif
	
	CStdioFile f;
	BOOL OK = f.Open(fileName, CFile::modeCreate | CFile::modeWrite);
	if (!OK) {
		CString mess = _T("Could not open file ");
		mess += fileName;
		mess += _T(" for writing log data");
		AfxMessageBox(mess);
		fileCanBeUsed = false;
	}
	else {
		fileCanBeUsed = TRUE;

		f.WriteString(heading);

		f.WriteString(_T("\t<AppName>"));
		f.WriteString(appName);
		f.WriteString(_T("\t</AppName>\n"));

		f.WriteString(_T("\t<SWVersion>"));
		f.WriteString(getVersionInfo());
		f.WriteString(_T("\t</SWVersion>\n"));
		CTime dt = CTime::GetCurrentTime();
		CString ts = dt.Format(_T("\t<Time>%A, %B %d, %Y %H:%M:%S</Time>\n"));
		f.WriteString(ts);

		wchar_t user[256];
		DWORD len = 256;
		GetUserName(user, &len);
		wstring u1 = user;
		CString u2(u1.c_str());
		f.WriteString(_T("\t<User>"));
		f.WriteString(u2);
		f.WriteString(_T("\t</User>\n"));

		sysInfo(&f);
		gAdapterInfo(&f);

		// Device manager info: Windows Management Instrumentation (WMI) 
		// msdn.microsoft.com/en-us/library/aa390418(v=vs.85).aspx 
		f.Close();
	}
#endif
}

CDumpLog::~CDumpLog()
{
	dump(_T("</AGSLogFile>\n"));
}

BOOL CDumpLog::sysInfo(CStdioFile *_f)
{
	if (!fileCanBeUsed) return FALSE;

#ifndef IN_I2CCOMM
	_f->WriteString(_T("\t<SystemInfo>\n"));
	_f->WriteString(_T("\t\t<UserStatus>"));
	if (m_si.IsUserAdmin()) _f->WriteString(_T("Admin")); 
	else _f->WriteString(_T("Not admin"));
	_f->WriteString(_T("</UserStatus>\n"));

	_f->WriteString(_T("\t\t<OperatingSystem>"));
	if (m_si.IsWindowsNT()) _f->WriteString(_T("Windows NT"));
	else _f->WriteString(_T("Not Windows NT"));
	_f->WriteString(_T("</OperatingSystem>\n"));

	CString s;
	_f->WriteString(_T("\t\t<OpSystVersion>"));
	s.Format(_T("Windows major %d, minor %d, build %d, service pack %d, OS type %s"),
		m_si.GetWinMajor(), m_si.GetWinMinor(), m_si.GetBuildNumber(), m_si.GetServicePack(), m_si.GetOSType());
	_f->WriteString(s);
	_f->WriteString(_T("</OpSystVersion>\n"));

	_f->WriteString(_T("\t\t<CPUName>"));
	s.Format(_T("CPU name: %s"), m_si.GetCPUNameString());
	_f->WriteString(s);
	_f->WriteString(_T("</CPUName>\n"));


	_f->WriteString(_T("\t\t<CPU_ID>"));
	s.Format(_T("CPU identifier: %s"), m_si.GetCPUIdentifier());
	_f->WriteString(s);
	_f->WriteString(_T("</CPU_ID>\n"));


	_f->WriteString(_T("\t\t<CPUSpeed>"));
	s.Format(_T("CPU speed: %d"), m_si.GetCPUSpeed());
	_f->WriteString(s);
	_f->WriteString(_T("</CPUSpeed>\n"));


	_f->WriteString(_T("\t\t<CPUVendor>"));
	s.Format(_T("CPU vendor: %s"), m_si.GetCPUVendorIdentifier());
	_f->WriteString(s);
	_f->WriteString(_T("</CPUVendor>\n"));

	_f->WriteString(_T("\t</SystemInfo>\n"));
#endif
	return TRUE;
}

BOOL CDumpLog::gAdapterInfo(CStdioFile *_f)
{
	EnumerateAdapters(_f);
	// Don't need this info adapterInfo(_f);
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
		_f->WriteString(_T("\t<GraphicsAdapter>"));
		sf.Format(_T("%2d : "),i);
		sf += s;
		_f->WriteString(sf);
		_f->WriteString(_T("</GraphicsAdapter>\n"));

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

		_f->WriteString(_T("\t\t<AdapterInfo>\n"));

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
			_f->WriteString(_T("\t\t<Error>Error allocating memory needed to call GetAdaptersinfo</Error>\n"));

			_f->WriteString(_T("\t\t</AdapterInfo>\n"));
			return 1;
		}
		// Make an initial call to GetAdaptersInfo to get
		// the necessary size into the ulOutBufLen variable
		if (GetAdaptersInfo(pAdapterInfo, &ulOutBufLen) == ERROR_BUFFER_OVERFLOW) {
			FREE(pAdapterInfo);
			pAdapterInfo = (IP_ADAPTER_INFO *)MALLOC(ulOutBufLen);
			if (pAdapterInfo == NULL) {
				_f->WriteString(_T("\t\t<Error>Error allocating memory needed to call GetAdaptersinfo</Error>\n"));
				_f->WriteString(_T("\t\t</AdapterInfo>\n"));
				return 1;
			}
		}

		if ((dwRetVal = GetAdaptersInfo(pAdapterInfo, &ulOutBufLen)) == NO_ERROR) {
			pAdapter = pAdapterInfo;
			CString s;
			while (pAdapter) {
				CString aN(pAdapter->AdapterName);
				CString aD(pAdapter->Description);

				_f->WriteString(_T("\t\t\t<Combo>\n"));
				s.Format(_T("\t\t\t\t<Index>%d</Index>\n"), pAdapter->ComboIndex);
				_f->WriteString(s);
				s.Format(_T("\t\t\t\t<Name>%s</Name>\n"), static_cast<LPCTSTR>(aN));
				_f->WriteString(s);
				s.Format(_T("\t\t\t\t<Description>%s</Description>\n"), static_cast<LPCTSTR>(aD));
				_f->WriteString(s);
				_f->WriteString(_T("\t\t\t\t<Address>"));
				for (i = 0; i < pAdapter->AddressLength; i++) {
					if (i == (pAdapter->AddressLength - 1)) {
						s.Format(_T("%.2X"), (int)pAdapter->Address[i]);
						_f->WriteString(s);
					}
					else {
						s.Format(_T("%.2X-"), (int)pAdapter->Address[i]);
						_f->WriteString(s);
					}
				}
				_f->WriteString(_T("</Address>\n"));
				s.Format(_T("\t\t\t\t<Index>%d</Index>\n"), pAdapter->Index);
				_f->WriteString(s);
				_f->WriteString(_T("\t\t\t\t<Type>"));
				switch (pAdapter->Type) {
				case MIB_IF_TYPE_OTHER:
					_f->WriteString(_T("Other"));
					break;
				case MIB_IF_TYPE_ETHERNET:
					_f->WriteString(_T("Ethernet"));
					break;
				case MIB_IF_TYPE_TOKENRING:
					_f->WriteString(_T("Token Ring"));
					break;
				case MIB_IF_TYPE_FDDI:
					_f->WriteString(_T("FDDI"));
					break;
				case MIB_IF_TYPE_PPP:
					_f->WriteString(_T("PPP"));
					break;
				case MIB_IF_TYPE_LOOPBACK:
					_f->WriteString(_T("Loopback"));
					break;
				case MIB_IF_TYPE_SLIP:
					_f->WriteString(_T("Slip"));
					break;
				default:
					s.Format(_T("Unknown type %ld"), pAdapter->Type);
					_f->WriteString(s);
					break;
				}
				_f->WriteString(_T("</Type>\n"));

				CString aIIS(pAdapter->IpAddressList.IpAddress.String);
				CString aIIMS(pAdapter->IpAddressList.IpMask.String);
				CString GIS(pAdapter->GatewayList.IpAddress.String);
				s.Format(_T("\t\t\t\t<IPAddress>%s</IPAddress>\n"), static_cast<LPCTSTR>(aIIS));
				_f->WriteString(s);
				s.Format(_T("\t\t\t\t<IPMask>%s</IPMask>\n"), static_cast<LPCTSTR>(aIIMS));
				_f->WriteString(s);
				s.Format(_T("\t\t\t\t<Gateway>%s</Gateway>\n"), static_cast<LPCTSTR>(GIS));
				_f->WriteString(s);

				if (pAdapter->DhcpEnabled) {
					_f->WriteString(_T("\t\t\t\t<DHCPEnabled>Yes</DHCPEnabled>\n"));
					CString s1(pAdapter->DhcpServer.IpAddress.String);
					s.Format(_T("\t\t\t\t<DHCPServer>%s</DHCPServer>\n"), static_cast<LPCTSTR>(s1));
					_f->WriteString(s);
					_f->WriteString(_T("\t\t\t\t<LeaseObtained>"));
					/* Display local time */
					error = _localtime32_s(&newtime, (__time32_t*)&pAdapter->LeaseObtained);
					if (error)
						_f->WriteString(_T("Invalid Argument to _localtime32_s"));
					else {
						// Convert to an ASCII representation 
						error = asctime_s(buffer, 32, &newtime);
						if (error)
							_f->WriteString(_T("Invalid Argument to asctime_s"));
						else {
							/* asctime_s returns the string terminated by \n\0 */
							CString b1(buffer);
							s.Format(_T("%s"), static_cast<LPCTSTR>(b1));
							s.Remove('\n');
							_f->WriteString(s);
						}
					}
					_f->WriteString(_T("</LeaseObtained>\n"));

					_f->WriteString(_T("\t\t\t\t<LeaseExpires>"));
					error = _localtime32_s(&newtime, (__time32_t*)&pAdapter->LeaseExpires);
					if (error)
						_f->WriteString(_T("Invalid Argument to _localtime32_s"));
					else {
						// Convert to an ASCII representation 
						error = asctime_s(buffer, 32, &newtime);
						if (error)
							_f->WriteString(_T("Invalid Argument to asctime_s"));
						else {
							/* asctime_s returns the string terminated by \n\0 */
							CString b1(buffer);
							s.Format(_T("%s"), static_cast<LPCTSTR>(b1));
							s.Remove('\n');
							_f->WriteString(s);
						}
					}
					_f->WriteString(_T("</LeaseExpires>\n"));
				}
				else
					_f->WriteString(_T("\t\t\t\t<DHCPEnabled>No</DHCPEnabled>\n"));

				if (pAdapter->HaveWins) {
					_f->WriteString(_T("\t\t\t\t<HaveWins>Yes</HaveWins>\n"));
					CString s1(pAdapter->PrimaryWinsServer.IpAddress.String);
					CString s2(pAdapter->SecondaryWinsServer.IpAddress.String);
					s.Format(_T("\t\t\t\t<PrimaryWinsServer>%s</PrimaryWinsServer>\n"), static_cast<LPCTSTR>(s1));
					_f->WriteString(s);
					s.Format(_T("\t\t\t\t</SecondaryWinsServer>%s</SecondaryWinsServer>\n"), static_cast<LPCTSTR>(s2));
					_f->WriteString(s);
				}
				else
					_f->WriteString(_T("\t\t\t\t<HaveWins>No</HaveWins>\n"));
				pAdapter = pAdapter->Next;
				_f->WriteString(_T("\t\t\t</Combo>\n"));
			}
		}
		else {
			CString s;
			s.Format(_T("\t\t\t</Error>GetAdaptersInfo failed with error: %d</Error>\n"), dwRetVal);
			_f->WriteString(s);
		}
		if (pAdapterInfo)
			FREE(pAdapterInfo);

		_f->WriteString(_T("\t\t</AdapterInfo>\n"));
#endif
		return 0;
}

CString CDumpLog::getFileName(void)
{
	if (!fileCanBeUsed) return _T("cannot use dumpFile");

	return fileName;
}

void CDumpLog::dump(CString _s)
{
	if (!fileCanBeUsed) return;

#ifndef IN_I2CCOMM
	CStdioFile f;
	BOOL OK = f.Open(fileName, CFile::modeWrite | CFile::modeNoTruncate);
	if (OK) {
		f.Seek(0, CFile::end);
		f.WriteString(_s);
		f.Close();
	}
#endif
}

bool CDumpLog::canDmpLogFileBeUsed(void)
{
	return fileCanBeUsed;
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
			}
		}
		free(pBlock);
	}
	return vstr;
}