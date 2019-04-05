
#ifndef REGENTRIES_DEFINED // Used to flag that the file has been included
#define REGENTRIES_DEFINED

/*!
 * @author		RKH
 * @date		05.11.14
 * @copyright	Spiro Medical AS 2014
 */


#include <afxtempl.h>
#include "regdefs.h"

class AGS_REG_ENTRIES  // Adds keys under product key. Put under HKEY_CURRENT_USER/Software
						// Also reads and sets values under keys
{
public :
	AGS_REG_ENTRIES(CString _mainKeyString,BOOL _createParentKeys = false); 
	~AGS_REG_ENTRIES(void);
	BOOL createSubKey(CString _keyName,DWORD *_disposition );	
	BOOL addToSubKey(CString _keyName,CString _valName,double _val,int _factor = 1000);
	BOOL addToSubKey(CString _keyName,CString _valName,int _val);
	BOOL addToSubKey(CString _keyName,CString _valName,CString _txt);
	BOOL getFromSubKey(CString _keyName,CString _valName,double *_val,int _factor = 1000);
	BOOL getFromSubKey(CString _keyName,CString _valName,int *_val);
	BOOL getFromSubKey(CString _keyName,CString _valName,CString *_txt);
	BOOL get(CString _valName,CString *_txt);
	BOOL get(CString _valName,int *_val);
	BOOL get(CString _valName,double *_val,int _factor = 1000);
	BOOL add(CString _valName,CString _txt);
	BOOL add(CString _valName,int _val);
	BOOL add(CString _valName,double _val,int _factor = 1000);
	BOOL deleteSubKey(CString _keyName);
	BOOL getSerialPorts(CArray <CString,CString> *_ports);
	BOOL getSubKeys(CString _keyName,CArray <CString,CString> *_keys);
protected:
	CString mainKeyString;
	HKEY openSubKey(CString _keyName);
	HKEY mainKey; // This is the main key, below the company key. It is kept open as long as the object is alive
};

#endif