
#include "stdafx.h"
#include "dataMMFiles.h"

/*
Description: Constructor
Parameters: 
	_fileName: Name of the file. Mutex and event names will be based on this
	_direction: Either DIR_WRITE or DIR_READ
*/

CDataMMFiles::CDataMMFiles(unsigned long _maxSize,CString _fileName)
{	
	fileName = _fileName;
	innerMutexName = _T("INNERMUTEX_");
	innerMutexName += fileName;
	outerMutexName = _T("OUTERMUTEX_");
	outerMutexName += fileName;
	eventName = _T("EVENT_");
	eventName += fileName;
	maxSize = _maxSize;

	innerMutexHandle = outerMutexHandle = fileHandle = NULL;
}

/*
Description: Default constructor. Sets direction to DIR_WRITE and name to "AfileName". These can be changed with a call to setNameAndDirection() before being used
*/
CDataMMFiles::CDataMMFiles(void)
{	
	fileName = _T("AfileName");
	innerMutexName = _T("INNERMUTEX_");
	innerMutexName += fileName;
	outerMutexName = _T("OUTERMUTEX_");
	outerMutexName += fileName;
	eventName = _T("EVENT_");
	eventName += fileName;

	innerMutexHandle = outerMutexHandle = fileHandle = NULL;
}

/*
Description: Destructor
*/
CDataMMFiles::~CDataMMFiles()
{	
	if (innerMutexHandle) CloseHandle(innerMutexHandle);
	if (outerMutexHandle) CloseHandle(outerMutexHandle);
	if (fileHandle) CloseHandle(fileHandle);
}

/*
Description: Returns a the event name. Use this to create an event to wait on
Returns: The name of the event. Constructed from the filename
*/
CString CDataMMFiles::getEventName(void) { return eventName; }

/*
Description: Reads from mem mapped file. Handles are checked and created/opened as required. Pulls multiple count semaphore first, the waits for single access to read.
Parameters: 
			_sArray: String array where the strings are appended. The array needs not be empty initially
			_timeout: Timeout in milliseconds
Returns: The number of strings that were read
*/
int CDataMMFiles::read(CArray <CString,CString> *_sArray,DWORD _timeout)
//*************************************************************************
{
	int numRead = 0;
	if (!init()) {
#ifdef _DEBUG
		CString s = _T("Could not init the memfile ");
		s += fileName;
		AfxMessageBox(s);
#endif
		return 0;
	}

	//---Read
	DWORD waitRes = WaitForSingleObject(innerMutexHandle,_timeout);
	if (waitRes == WAIT_OBJECT_0) {
		unsigned char *pView = (unsigned char *) MapViewOfFile(fileHandle,FILE_MAP_READ,0,0,0);
		if (pView) {
			CString s;
			unsigned char zero = 0;
			int loc = 0;
			int count;
			memcpy(&count,pView + loc,sizeof(count));
			loc += sizeof(count);
			int len = 0;
			do {
				s = pView + loc;
				len = s.GetLength();
				if (len) {
					_sArray->Add(s);
					numRead++;
				} 
				loc += len + 1;
			} while (len);
			UnmapViewOfFile(pView);
		}
		ReleaseMutex(innerMutexHandle);
	}

	return numRead;
}

/*
Description: Writes to mem mapped file. Handles are checked and created/opened as required. Waits for multiple count semaphore first (to ensure that all readers are done), then waits for single access, clears the file and then writes.
Parameters: 
		_sArray: String array with the strings. All the strings are written.
		_innerTimeout: Timeout in milliseconds
		_outerTimeout : Timeout in ms
		_remove: true if the string should be removed after being written 
Returns: The number of strings that were written
*/
int CDataMMFiles::write(CArray <CString,CString> *_sArray,DWORD _innerTimeout,DWORD _outerTimeout,bool _remove)
//****************************************************************************************************************
{
	int numWritten = 0;
	if (!init()) {
#ifdef _DEBUG
		CString s = _T("Could not init the memfile ");
		s += fileName;
		AfxMessageBox(s);
#endif
		return 0;
	}
	
	DWORD waitResOuter = WaitForSingleObject(outerMutexHandle,_outerTimeout);
	if (waitResOuter != WAIT_OBJECT_0) return 0;

	//---Write
	bool didWrite = false;
	DWORD waitRes = WaitForSingleObject(innerMutexHandle,_innerTimeout);
	if (waitRes == WAIT_OBJECT_0) {
		unsigned char *pView = (unsigned char *) MapViewOfFile(fileHandle,FILE_MAP_WRITE,0,0,0);
		if (pView) {
			int count = (int) _sArray->GetCount();
			CString s;
			unsigned char zero = 0;
			int loc = 0;
			memcpy(pView + loc,&count,sizeof(count));
			loc += sizeof(count);
			int lastWritten = count - 1;
			for (int i = 0 ; i < count ; i++) {
				s = _sArray->GetAt(i);
				int len = s.GetLength();

				if ((unsigned long) (loc + len + 2) > maxSize) {
					lastWritten = i;
					break;
				}
				
				memcpy(pView + loc,s.GetBuffer(1),len);
				loc += len;
				memcpy(pView + loc,&zero,1);
				loc += 1;
				numWritten++;
			}
			if (_remove) {
				for (int j = lastWritten ; j >= 0 ; j--) {
					_sArray->RemoveAt(j);
				}
			}
			memcpy(pView + loc,&zero,1);
			didWrite = true;
			UnmapViewOfFile(pView);
		}
		ReleaseMutex(innerMutexHandle);
	}

	//---Signal
	if (didWrite) {
		CEvent event(0,TRUE,eventName);
		event.SetEvent();
	}
	
	ReleaseMutex(outerMutexHandle);

	return numWritten;
}


/*
Description: Writes to mem mapped file. Handles are checked and created/opened as required. Waits for multiple count semaphore first (to ensure that all readers are done), then waits for single access, clears the file and then writes.
Parameters: 
		_sArray: String array with the strings. All the strings are written.
		_innerTimeout: Timeout in milliseconds
		_outerTimeout : Timeout in ms
Returns: The number of strings that were written
*/
int CDataMMFiles::write(CString _s,DWORD _innerTimeout,DWORD _outerTimeout)
//*****************************************************************************
{
	int numWritten = 0;
	if (!init()) {
#ifdef _DEBUG
		CString s = _T("Could not init the memfile ");
		s += fileName;
		AfxMessageBox(s);
#endif
		return 0;
	}
	
	DWORD waitResOuter = WaitForSingleObject(outerMutexHandle,_outerTimeout);
	if (waitResOuter != WAIT_OBJECT_0) return 0;

	//---Write
	bool didWrite = false;
	DWORD waitRes = WaitForSingleObject(innerMutexHandle,_innerTimeout);
	if (waitRes == WAIT_OBJECT_0) {
		unsigned char *pView = (unsigned char *) MapViewOfFile(fileHandle,FILE_MAP_WRITE,0,0,0);
		if (pView) {
			int count = 1;
			unsigned char zero = 0;
			int loc = 0;
			memcpy(pView + loc,&count,sizeof(count));
			loc += sizeof(count);
			int len = _s.GetLength();

			if ((unsigned long ) (loc + len + 2) <= maxSize) {
				memcpy(pView + loc,_s.GetBuffer(1),len);
				loc += len;
				memcpy(pView + loc,&zero,1);
				loc += 1;
				memcpy(pView + loc,&zero,1);
				numWritten = 1;
				didWrite = true;
			}
			UnmapViewOfFile(pView);
		}
		ReleaseMutex(innerMutexHandle);
	}

	//---Signal
	if (didWrite) {
		CEvent event(0,TRUE,eventName);
		event.SetEvent();
	}
	
	ReleaseMutex(outerMutexHandle);

	return numWritten;
}

/*
Description: Check if handles and files are available. Creates or opens as required
Parameters: _direction: DIR_READ if the file shall be used for reading, DIR_WRITE if the file shall be used for writing.
Returns: true if you can go ahead and use the handles, falkse if not.
*/
bool CDataMMFiles::init(void)
//******************************
{
	if (!innerMutexHandle) {
		innerMutexHandle = CreateMutex(NULL,      // no security attributes
								  FALSE,     // initially not owned
								  innerMutexName);
		if (!innerMutexHandle) return false;
	}
	if (!outerMutexHandle) {
		outerMutexHandle = CreateMutex(NULL,      // no security attributes
								  FALSE,     // initially not owned
								  outerMutexName);
		if (!outerMutexHandle) return false;
	}

	if (!fileHandle) {
		fileHandle = OpenFileMapping(FILE_MAP_ALL_ACCESS,FALSE,fileName);
		if (!fileHandle) {
			fileHandle = CreateFileMapping(INVALID_HANDLE_VALUE,NULL,PAGE_READWRITE,
											0,maxSize,fileName);
		}
		if (!fileHandle) return false;
	}
	return true;
}

/////////////////////////////////////////////////////////////////////////

CESGenericMMFile::CESGenericMMFile(unsigned long _maxSize,CString _name) :
			CDataMMFiles(_maxSize,_name)
//***********************************************************************
{
}

CESGenericMMFile::~CESGenericMMFile() {}


/*
Description: Writes to mem mapped file from a buffer. Handles are checked and created/opened as required. Waits for multiple count semaphore first (to ensure that all readers are done), then waits for single access, clears the file and then writes.
Parameters: 
		_buf: The buffer
		_size: Size of the buffer
		_innerTimeout: Timeout in milliseconds (inner mutex)
		_outerTimeout: Timeout in milliseconds (outer mutex)
Returns: The number of bytes that were written
*/
int CESGenericMMFile::write(void *_buf,int _size,DWORD _innerTimeout,DWORD _outerTimeout)
//****************************************************************************************
{
	int numWritten = 0;
	if (!init()) {
#ifdef _DEBUG
		CString s = _T("Could not init the memfile ");
		s += fileName;
		AfxMessageBox(s);
#endif
		return 0;
	}
	
	DWORD waitResOuter = WaitForSingleObject(outerMutexHandle,_outerTimeout);
	if (waitResOuter != WAIT_OBJECT_0) return 0;

	//---Write
	bool didWrite = false;
	DWORD waitRes = WaitForSingleObject(innerMutexHandle,_innerTimeout);
	if (waitRes == WAIT_OBJECT_0) {
		unsigned char *pView = (unsigned char *) MapViewOfFile(fileHandle,FILE_MAP_WRITE,0,0,0);
		if (pView) {
			memcpy(pView,(void *) _buf,_size);
			didWrite = true;
			numWritten += _size;
			UnmapViewOfFile(pView);
		}
		ReleaseMutex(innerMutexHandle);
	}

	//---Signal
	if (didWrite) {
		CEvent event(0,TRUE,eventName);
		event.SetEvent();
	}
	
	ReleaseMutex(outerMutexHandle);

	return numWritten;
}

/*
Description: Reads from the file to a buffer. Handles are checked and created/opened as required. Pulls multiple count semaphore first, the waits for single access to read.
Parameters: 
			_buf: Pointer to buffer
			_size: Size to read
			_timeout: Timeout in milliseconds
Returns: The number of bytes that were read
*/
int CESGenericMMFile::read(void *_buf,int _size,DWORD _timeout)
//*************************************************************
{
	int numRead = 0;
	if (!init()) {
#ifdef _DEBUG
		CString s = _T("Could not init the memfile ");
		s += fileName;
		AfxMessageBox(s);
#endif
		return 0;
	}

	//---Read
	DWORD waitRes = WaitForSingleObject(innerMutexHandle,_timeout);
	if (waitRes == WAIT_OBJECT_0) {
		void *pView = MapViewOfFile(fileHandle,FILE_MAP_READ,0,0,0);
		if (pView) {
			memcpy(_buf,pView,_size);
			numRead += _size;
			UnmapViewOfFile(pView);
		}
		ReleaseMutex(innerMutexHandle);
	}

	return numRead;
}

/*
Description: Returns existence status
Returns: true if it exsts, false if not
*/
bool CESGenericMMFile::getFileExists(void)
{
	fileHandle = OpenFileMapping(FILE_MAP_ALL_ACCESS,FALSE,fileName);

	return fileHandle ? true : false;
}

/*
Description: Returns the max file size
*/
unsigned long CESGenericMMFile::getMaxFileLength(void) 
//*****************************************************
{
	return maxSize;
}

/*
Description: Check if handles and files are available. Creates or opens as required
			Direction: DIR_READ if the file shall be used for reading, DIR_WRITE if the file shall be used for writing.
Returns: true if you can go ahead and use the handles, false if not.
*/
bool CESGenericMMFile::init(void)
//*******************************
{
	if (!innerMutexHandle) {
		innerMutexHandle = CreateMutex(NULL,      // no security attributes
								  FALSE,     // initially not owned
								  innerMutexName);
		if (!innerMutexHandle) return false;
	}
	if (!outerMutexHandle) {
		outerMutexHandle = CreateMutex(NULL,      // no security attributes
								  FALSE,     // initially not owned
								  outerMutexName);
		if (!outerMutexHandle) return false;
	}

	if (!fileHandle) {
		fileHandle = OpenFileMapping(FILE_MAP_ALL_ACCESS,FALSE,fileName);
		if (!fileHandle) {
			fileHandle = CreateFileMapping(INVALID_HANDLE_VALUE,NULL,PAGE_READWRITE,
											0,maxSize,fileName);
		}
		if (!fileHandle) return false;
	}
	return true;
}
