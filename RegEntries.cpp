
/*!
 * @author		RKH
 * @date		05.11.14
 * @copyright	Spiro Medical AS 2014
 */

#include "stdafx.h"

#include "RegEntries.h"
#include <winerror.h>

// Description: Constructor. Opens HKEY_CURRENT_USER/"Software" and opens (creates if necessary) company key (COMPANY_NAME) and product key (PRODUCT_NAME). Keeps product key open, while the other keys are closed
// Parameters: _createParentKeys : (true)  company key and product key will be created if the do not exist.
//                                 (false)  company key and product key will not be created if they do not exist
AGS_REG_ENTRIES::AGS_REG_ENTRIES(CString _mainKeyString,BOOL _createParentKeys) :
	mainKeyString(_mainKeyString),mainKey(NULL)
{
	HKEY swKey,companyKey;

	LONG ret = RegOpenKeyEx(HKEY_CURRENT_USER,SOFTWARE_NAME,0,KEY_ALL_ACCESS,&swKey);
	if (ret != ERROR_SUCCESS) return;

	if (!_createParentKeys) {
		ret = RegOpenKeyEx(swKey,COMPANY_NAME,0,KEY_ALL_ACCESS,&companyKey);
		if (ret != ERROR_SUCCESS) {
			RegCloseKey(swKey);
			return;
		}

		ret = RegOpenKeyEx(companyKey,mainKeyString,0,KEY_ALL_ACCESS,&mainKey);
		if (ret != ERROR_SUCCESS) {
			RegCloseKey(companyKey);
			RegCloseKey(swKey);
			return;
		}
		RegCloseKey(companyKey);
		RegCloseKey(swKey);
		return;
	}

	else {
		DWORD disposition;
		ret = RegCreateKeyEx(swKey,COMPANY_NAME,0,NULL,0,KEY_ALL_ACCESS,NULL,
			&companyKey,&disposition);
		if (ret != ERROR_SUCCESS) {
			RegCloseKey(swKey);
			return;
		}

		ret = RegCreateKeyEx(companyKey,mainKeyString,0,NULL,0,KEY_ALL_ACCESS,NULL,
			&mainKey,&disposition);
		if (ret != ERROR_SUCCESS) {
			RegCloseKey(companyKey);
			RegCloseKey(swKey);
			return;
		}
		RegCloseKey(companyKey);
		RegCloseKey(swKey);
		return;
	}
}

// Description: Destructor. Closes product key
AGS_REG_ENTRIES::~AGS_REG_ENTRIES(void)
{
	if (mainKey) RegCloseKey(mainKey);
}

/* Description
Creates a subkey under product key

Returns
If success : true, if unsuccessful: false

Parameters
_keyName :      Name of the subkey
_disposition :  REG_CREATED_NEW_KEY\:<P>
The key did not exist and was created.<P>
REG_OPENED_EXISTING_KEY\:<P>
The key existed and was simply opened without being
changed.                                            */
BOOL AGS_REG_ENTRIES::createSubKey(CString _keyName,DWORD *_disposition)
{ 
	if (!mainKey) return false; 

	HKEY subKey;
	LONG ret = RegCreateKeyEx(mainKey,_keyName,0,NULL,0,KEY_ALL_ACCESS,NULL,
		&subKey,_disposition);
	if (ret != ERROR_SUCCESS) return false;

	RegCloseKey(subKey);
	return true;
}

// Description: Deletes a subkey under product key
// Returns: If success : TRUE, if unsuccessful: FALSE
// Parameters: _keyName: Name of the subkey
BOOL AGS_REG_ENTRIES::deleteSubKey(CString _keyName)
{
	LONG ret = RegDeleteKey(mainKey,_keyName);
	if (ret != ERROR_SUCCESS) return FALSE;
	return TRUE;
}

// Description: Opens a subkey under product key
// Returns: If success : the key (HKEY), if unsuccessful: NULL
// Parameters: _keyName: Name of the subkey
HKEY AGS_REG_ENTRIES::openSubKey(CString _keyName)
{
	if (!mainKey) return NULL; 

	HKEY subKey;
	LONG ret = RegOpenKeyEx(mainKey,_keyName,0,KEY_ALL_ACCESS,&subKey);
	if (ret != ERROR_SUCCESS) return NULL;
	return subKey;
}

// Description: Adds a double value to a subkey
// Returns: If success : true, if unsuccessful: false
// Parameters: _keyName: Name of the subkey
//		_valName: Name of the value
//		_val : value
//	_factor: _value is multiplied with factor prior to registry storage
BOOL AGS_REG_ENTRIES::addToSubKey(CString _keyName,CString _valName,
								 double _val,int _factor)
{
	HKEY subKey = openSubKey(_keyName);
	if (!subKey) return false;

	DWORD val = (DWORD) (_val * (double) _factor);
	DWORD dLen = sizeof(DWORD);

	LONG ret = RegSetValueEx(subKey,_valName,0,REG_DWORD,(BYTE *) &val,dLen);
	RegCloseKey(subKey);
	if (ret != ERROR_SUCCESS) return false; 

	return true;
}

// Description: Adds an int value to a subkey
// Returns: If success : true, if unsuccessful: false
// Parameters: _keyName: Name of the subkey
//		_valName: Name of the value
//		_val : value
BOOL AGS_REG_ENTRIES::addToSubKey(CString _keyName,CString _valName,int _val)
{
	HKEY subKey = openSubKey(_keyName);
	if (!subKey) return false;

	DWORD val = (DWORD) _val;
	DWORD dLen = sizeof(DWORD);

	LONG ret = RegSetValueEx(subKey,_valName,0,REG_DWORD,(BYTE *) &val,dLen);
	RegCloseKey(subKey);
	if (ret != ERROR_SUCCESS) return false; 

	return true;
}

/* Description
Adds a string value to a subkey

Returns
If success : true, if unsuccessful: false

Parameters
_keyName :  Name of the subkey
_valName :  Name of the value
_txt :      the string                    */
BOOL AGS_REG_ENTRIES::addToSubKey(CString _keyName,CString _valName,CString _txt)
{
	HKEY subKey = openSubKey(_keyName);
	if (!subKey) return false;

	size_t dLen = wcslen(_txt) * sizeof(wchar_t) + 1;
	
	LONG ret = RegSetValueEx(subKey,_valName,0,REG_SZ,(BYTE *) _txt.GetBuffer(),(DWORD) dLen);
	_txt.ReleaseBuffer();

	RegCloseKey(subKey);
	if (ret != ERROR_SUCCESS) return false; 

	return true;
}

// Description: Get a double value from a subkey
// Returns: If success : true, if unsuccessful: false
// Parameters: _keyName: Name of the subkey
//		_valName: Name of the value
//		_val : pointer to the value
//	_factor: the value in the registry is divided by _factor before it is placed in _val
BOOL AGS_REG_ENTRIES::getFromSubKey(CString _keyName,CString _valName,
								   double *_val,int _factor)
{
	HKEY subKey = openSubKey(_keyName);
	if (!subKey) return false;

	DWORD dSize = sizeof(DWORD);
	DWORD type = REG_DWORD;
	DWORD dval;

	LONG ret = RegQueryValueEx(subKey,_valName,NULL,&type,(BYTE *) &dval,&dSize); 
	RegCloseKey(subKey);
	if (ret != ERROR_SUCCESS) return false; 

	*_val = (double) ((int) dval) / (double) _factor;
	return true; 
}



// Description: Get an int value from a subkey
// Returns: If success : true, if unsuccessful: false
// Parameters: _keyName: Name of the subkey
//		_valName: Name of the value
//		_val : pointer to the value
BOOL AGS_REG_ENTRIES::getFromSubKey(CString _keyName,CString _valName,int *_val)
{
	HKEY subKey = openSubKey(_keyName);
	if (!subKey) return false;

	DWORD dSize = sizeof(DWORD);
	DWORD type = REG_DWORD;
	DWORD dval;

	LONG ret = RegQueryValueEx(subKey,_valName,NULL,&type,(BYTE *) &dval,&dSize); 
	RegCloseKey(subKey);
	if (ret != ERROR_SUCCESS) return false; 

	*_val = (int) dval;
	return true; 
}

// Description: Get a string from a subkey
// Returns: If success : true, if unsuccessful: false
// Parameters: _keyName: Name of the subkey
//		_valName: Name of the value
//		_txt : pointer to the string
BOOL AGS_REG_ENTRIES::getFromSubKey(CString _keyName,CString _valName,
								   CString *_txt)
{
	HKEY subKey = openSubKey(_keyName);
	if (!subKey) return false;

	DWORD dSize = 255;
	DWORD type = REG_SZ;
	BYTE out[256];

	LONG ret = RegQueryValueEx(subKey,_valName,NULL,&type,out,&dSize); 
	RegCloseKey(subKey);
	if (ret != ERROR_SUCCESS) return false; 

	*_txt = out;
	return true; 
}

// Description: Get a string from the prodkey
// Returns: If success : true, if unsuccessful: false
// Parameters: 
//		_valName: Name of the value
//		_txt : pointer to the string
BOOL AGS_REG_ENTRIES::get(CString _valName,CString *_txt)
{
	DWORD dSize = 255;
	DWORD type = REG_SZ;
	BYTE out[256];
	LONG ret = RegQueryValueEx(mainKey,_valName,NULL,&type,out,&dSize); 

	if (ret != ERROR_SUCCESS) return false; 

	*_txt = out;
	return true; 
}

// Description: Get an int from the prodkey
// Returns: If success : true, if unsuccessful: false
// Parameters: 
//		_valName: Name of the value
//		_val : pointer to the integer
BOOL AGS_REG_ENTRIES::get(CString _valName,int *_val)
{
	DWORD dSize = sizeof(DWORD);
	DWORD type = REG_DWORD;
	DWORD dval;
	LONG ret = RegQueryValueEx(mainKey,_valName,NULL,&type,(BYTE *) &dval,&dSize); 

	if (ret != ERROR_SUCCESS) return false; 

	*_val = (int) dval;
	return true; 
}

// Description: Get a double from the prodkey
// Returns: If success : true, if unsuccessful: false
// Parameters: 
//		_valName: Name of the value
//		_val : pointer to the double
//	_factor: Factor to apply to and from the registry
BOOL AGS_REG_ENTRIES::get(CString _valName,double *_val,int _factor)
{
	int vali = 0;
	BOOL ok = get(_valName,&vali);
	if (ok) *_val = (double) vali / (double) _factor;
	return ok;
}

// Description: Add a string to the prodkey
// Returns: If success : true, if unsuccessful: false
// Parameters: 
//		_valName: Name of the value
//		_txt : the string
BOOL AGS_REG_ENTRIES::add(CString _valName,CString _txt)
{
	DWORD dLen = _txt.GetLength() + 1;

	LONG ret = RegSetValueEx(mainKey,_valName,0,REG_SZ,(BYTE *) _txt.GetBuffer(),dLen);
	_txt.ReleaseBuffer();
	if (ret != ERROR_SUCCESS) return false; 

	return true;
}

// Description: Add an integer to the prodkey
// Returns: If success : true, if unsuccessful: false
// Parameters: 
//		_valName: Name of the value
//		_val : the integer
BOOL AGS_REG_ENTRIES::add(CString _valName,int _val)
{
	DWORD dLen = sizeof(DWORD);
	DWORD v = (DWORD) _val;
	LONG ret = RegSetValueEx(mainKey,_valName,0,REG_DWORD,(BYTE *) &v,dLen);
	if (ret != ERROR_SUCCESS) return false; 

	return true;
}

// Description: Add a double to the prodkey
// Returns: If success : true, if unsuccessful: false
// Parameters: 
//		_valName: Name of the value
//		_val : the double
//		_factor: the factor to apply to and from the registry
BOOL AGS_REG_ENTRIES::add(CString _valName,double _val,int _factor)
{
	DWORD v = (DWORD) (_val * _factor);
	BOOL ok = add(_valName,(int) v);
	return ok;
}

/*
Description: Reads the serial port names from registry and fills the string array
Parameters: _ports : Pointer to the string array of port names
Returns: TRUE if OK, FALSE if not
*/
BOOL AGS_REG_ENTRIES::getSerialPorts(CArray <CString,CString> *_ports)
{
	HKEY hwKey,dMap,scomm;
	LONG ret = RegOpenKeyEx(HKEY_LOCAL_MACHINE,_T("HARDWARE"),0,KEY_READ,&hwKey);
	if (ret != ERROR_SUCCESS) return FALSE;

	ret = RegOpenKeyEx(hwKey,_T("DEVICEMAP"),0,KEY_READ,&dMap);
	if (ret != ERROR_SUCCESS) {
		RegCloseKey(hwKey);
		return FALSE;
	}

	ret = RegOpenKeyEx(dMap,_T("SERIALCOMM"),0,KEY_READ,&scomm);
	if (ret != ERROR_SUCCESS) {
		RegCloseKey(dMap);
		RegCloseKey(hwKey);
		return FALSE;
	}

	int index = 0;
	do {
		char valName[255];
		DWORD len = 255;
		DWORD type = REG_SZ;
		BYTE comName[10];
		DWORD dlen = 10;

		ret = RegEnumValue(scomm,index++,(LPTSTR) valName,&len,NULL,&type,comName,&dlen);
		if (ret != ERROR_NO_MORE_ITEMS) {
			CString s = (LPTSTR) comName; //(char *) comName;
			_ports->Add(s);
		}
	} while (ret != ERROR_NO_MORE_ITEMS);

	RegCloseKey(dMap);
	RegCloseKey(hwKey);
	RegCloseKey(scomm);

	return TRUE;
}

/*
Description: Reads the subkeys from registry and fills the string array
Parameters: _ports : Pointer to the string array of port names
Returns: TRUE if OK, FALSE if not
*/
BOOL AGS_REG_ENTRIES::getSubKeys(CString _keyName,CArray <CString,CString> *_keys)
{
	HKEY gKey;
	LONG ret = RegOpenKeyEx(mainKey,_T("SERIALCOMM"),0,KEY_READ,&gKey);
	if (ret != ERROR_SUCCESS) 
		return FALSE;

	int index = 0;
	do {
		char valName[255];
		DWORD len = 255;
		DWORD type = REG_SZ;
		BYTE name[10];
		DWORD dlen = 10;

		ret = RegEnumValue(gKey,index++,(LPTSTR) valName,&len,NULL,&type,name,&dlen);
		if (ret != ERROR_NO_MORE_ITEMS) {
			CString s = (LPTSTR) name; 
			_keys->Add(s);
		}
	} while (ret != ERROR_NO_MORE_ITEMS);

	RegCloseKey(gKey);
	return TRUE;
}





