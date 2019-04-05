#ifndef ANTHROPOLOGIA_INCLUDED
#define ANTHROPOLOGIA_INCLUDED



/*!
* @author		RKH
* @date		06.06.2017
* @copyright (C)Spiro Medical AS 2013 - 2017
*/

#include "AES128.h"
#include "dataMMFiles.h"
#include "AGSpiroDefs.h"
#include "CommonResource.h"

#define TEST_STR		_T("Sommer og Vinter")

//---Default encrypted password (decrypted: "AGSPassword")
const CString defpwDecrypted = _T("AGSPassword_____");
const unsigned char defpw[/*AES128_MAX_KEY_LENGTH = 16*/] =
{
	0xae,0x8b,0xb6,0xb9,
	0x54,0x95,0x0b,0x36,
	0x4a,0x03,0x4f,0xc2,
	0x85,0x87,0x29,0x5c,	
};

class CAttendingDoctor 
{
public:
	CAttendingDoctor();
	~CAttendingDoctor();
	virtual void clear(void);
	CString function;
	CString firstName;
	CString middleName;
	CString lastName;
	CString notes;
	CString Email;
	CString tel;
	CString address;
	void toRegistry(void);
	void fromRegistry(void);
	void Serialize(CArchive& ar);
protected:
	CString staffType;
};

class COperator
{
public:
	COperator();
	~COperator();
	virtual void clear(void);
	CString function;
	CString firstName;
	CString middleName;
	CString lastName;
	CString notes;
	CString Email;
	CString tel;
	CString address;
	void toRegistry(void);
	void fromRegistry(void);
	void Serialize(CArchive& ar);
protected:
	CString staffType;
};

class CReferringDoctor
{
public:
	CReferringDoctor();
	~CReferringDoctor();
	virtual void clear(void);
	COleDateTime dateOfReferral;
	void Serialize(CArchive& ar);
	CString firstName;
	CString middleName;
	CString lastName;
	CString notes;
	CString Email;
	CString tel;
	CString address;
};


class CMyGridItemPW : public CXTPPropertyGridItem
{
public:
	CMyGridItemPW(UINT nID,
    LPCTSTR strValue = NULL, 
    CString* pBindString = NULL);
	virtual void OnValueChanged(CString strValue);
	void usePWMask(bool _on);
protected:
};


class CPatient 
{
public:
	CPatient();
	CPatient(CString _pw);
	~CPatient();
	virtual void clear(void);
	bool savePW(void);
	bool readPW(void);
	BOOL savePassword,showPassword;
	bool encryptData(void);
	bool decryptData(void);
	void Serialize(CArchive& ar);
	void setPassword(CString _pw);
	CString getPassword(void);
	
	COleDateTime dateTimeOfAGS,dateOfBirth;
	CString gender,id;
	CString recordingID;
	unsigned char encrypted[AES128_MAX_KEY_LENGTH + 1];
	long ESSresult;
	double BMI,neckCirc;
	CString firstName;
	CString middleName;
	CString lastName;
	CString notes;
	CString Email;
	CString tel;
	CString address;
	static bool getIsPasswordOK(CString _pw,CString *_errmess);
protected:
	CEncryptedString eAddress,eFirstName,eMiddleName,eLastName,eID,eTestStr;
	CEncryptedString eEmail, eTel;
	CString testStr;
	CString password;
	unsigned char customerKey[AES128_MAX_KEY_LENGTH + 1];
};

#endif