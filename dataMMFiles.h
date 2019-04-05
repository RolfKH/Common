#ifndef CONTROL_MEMMAPS_INCLUDED
#define CONTROL_MEMMAPS_INCLUDED
/*
Class that handles all mem maps. IN/OUT refers to directions relative to control panel

	GRAPH_SETTINGS
	                          
If this mem mapped file is not found, Analysis reads settings from xml file

If this mem mapped file is found, Analysis takes settings from the mem mapped file (and writes changes back)

							
	
	MUTEX use                  ^
                               |
	                   *******---********
					   *                *		  
					   *                *   One reader or one writer allowed in. 
					   *    Inner       *   A writer that exits signals reader before releasing new writers
					   *                *   so that all readers can queue up before a new writer is let in
					   *                *
					   *******---********
                       *                *   Readers have to queue up here
					   *   Outer room   *   One writer allowed to queue up here
					   *                *
					   *******---********
                                             Writers have to queue up here
					       Outside             
*/
#include <afxtempl.h>
#include <afxmt.h>
#include <sys/timeb.h>

//----BASE names
#define APN_GRAPH_VIEW_SETTINGS			"APN_GRAPH_VIEW_SETTINGS"		// Mem mapped file name.

const CString apnMMFileName = _T("APNEAGRAPH_MMFILE");

//---Directions
#define DIR_READ		1
#define DIR_WRITE		2

#define MM_FILES_STD_READ_TIMEOUT				10
#define MM_FILES_STD_INNER_WRITE_TIMEOUT		10
#define MM_FILES_STD_OUTER_WRITE_TIMEOUT		10

class CDataMMFiles {
public:
	CDataMMFiles(unsigned long _maxSize,CString _fileName);
	CDataMMFiles(void);
	~CDataMMFiles();
	int read(CArray <CString,CString> *_sArray,DWORD _timeout);
	int write(CArray <CString,CString> *_sArray,DWORD _innerTimeout,DWORD _outerTimeout,bool _remove = false);
	int write(CString _s,DWORD _innerTimeout,DWORD _outerTimeout);
	CString getEventName(void);
protected:
	unsigned long maxSize;
	virtual bool init(void);
	CString fileName,innerMutexName,outerMutexName,eventName;

	HANDLE innerMutexHandle,outerMutexHandle;
	HANDLE fileHandle;
};



class CESGenericMMFile : public CDataMMFiles
{
public:
	CESGenericMMFile(unsigned long _maxSize,CString _name);
	~CESGenericMMFile();
    int write(void *_buf,int _size,DWORD _innerTimeout,DWORD _outerTimeout);
	int read(void *_buf,int _size,DWORD _timeout);
	unsigned long getMaxFileLength(void);
	bool getFileExists(void);
protected:
	virtual bool init(void);
	CString name;
};
#endif