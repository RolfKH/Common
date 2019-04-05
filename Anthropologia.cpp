
/*!
* @author		RKH
* @date		06.06.2017
* @copyright (C)Spiro Medical AS 2013 - 2017
*/

#include "stdafx.h"
#include "Anthropologia.h"
#include "RegDefs.h"
#include"CommonResource.h"

extern int AGSMajorVersionRead;
extern CString globalPassword;

CAttendingDoctor::CAttendingDoctor()
{
	staffType = ATTENDING_DOCTOR;
	firstName = _T(""); 
	middleName = _T(""); 
	lastName = _T(""); 
	notes = _T(""); 
	Email = _T(""); 
	tel = _T(""); 
	address = _T(""); 
}

CAttendingDoctor::~CAttendingDoctor()
{
}

void CAttendingDoctor::clear(void)
{
	firstName = _T(""); 
	middleName = _T(""); 
	lastName = _T(""); 
	notes = _T(""); 
	Email = _T(""); 
	tel = _T(""); 
	address = _T(""); 

	function = _T("");
}


void CAttendingDoctor::toRegistry(void)
{
	CXTPRegistryManager regm;

	regm.SetRegistryKey(COMPANY_NAME,ANTHROPOLOGIA);
	BOOL OK;
	OK = regm.WriteProfileString(staffType,FIRST_NAME,firstName);
	ASSERT(OK);
	OK = regm.WriteProfileString(staffType,MIDDLE_NAME,middleName);
	ASSERT(OK);
	OK = regm.WriteProfileString(staffType,LAST_NAME,lastName);
	ASSERT(OK);
	OK = regm.WriteProfileString(staffType,NOTES,notes);
	ASSERT(OK);
	OK = regm.WriteProfileString(staffType,EMAIL,Email);
	ASSERT(OK);
	OK = regm.WriteProfileString(staffType,ADDRESS,address);
	ASSERT(OK);
	OK = regm.WriteProfileString(staffType,TELEPHONE,tel);
	ASSERT(OK);
	OK = regm.WriteProfileString(staffType,FUNCTION,function);
	ASSERT(OK);
}

void CAttendingDoctor::fromRegistry(void)
{
	CXTPRegistryManager regm;
	regm.SetRegistryKey(COMPANY_NAME,ANTHROPOLOGIA);
	firstName = regm.GetProfileString(staffType,FIRST_NAME,firstName);
	middleName = regm.GetProfileString(staffType,MIDDLE_NAME,middleName);
	lastName = regm.GetProfileString(staffType,LAST_NAME,lastName);
	notes = regm.GetProfileString(staffType,NOTES,notes);
	Email = regm.GetProfileString(staffType,EMAIL,Email);
	address = regm.GetProfileString(staffType,ADDRESS,address);
	tel = regm.GetProfileString(staffType,TELEPHONE,tel);
	function = regm.GetProfileString(staffType,FUNCTION,function);
}

void CAttendingDoctor::Serialize(CArchive& ar)
{ 
	if( ar.IsStoring() )
		ar << firstName << middleName << lastName << notes << Email << tel << address;
    else	 
		ar >> firstName >> middleName >> lastName >> notes >> Email >> tel >> address;

	if( ar.IsStoring() ) {
		ar << staffType;
		ar << function;
	}
    else {
		ar >> staffType;
		ar >> function;
	}
}

///////////////////////////////////////////////////////////


COperator::COperator()
{
	staffType = OPERATOR;
	firstName = _T(""); 
	middleName = _T(""); 
	lastName = _T(""); 
	notes = _T(""); 
	Email = _T(""); 
	tel = _T(""); 
	address = _T(""); 
}

COperator::~COperator()
{
}

void COperator::clear(void)
{
	firstName = _T(""); 
	middleName = _T(""); 
	lastName = _T(""); 
	notes = _T(""); 
	Email = _T(""); 
	tel = _T(""); 
	address = _T(""); 

	function = _T("");
}

void COperator::toRegistry(void)
{
	CXTPRegistryManager regm;

	regm.SetRegistryKey(COMPANY_NAME,ANTHROPOLOGIA);
	BOOL OK;
	OK = regm.WriteProfileString(staffType,FIRST_NAME,firstName);
	ASSERT(OK);
	OK = regm.WriteProfileString(staffType,MIDDLE_NAME,middleName);
	ASSERT(OK);
	OK = regm.WriteProfileString(staffType,LAST_NAME,lastName);
	ASSERT(OK);
	OK = regm.WriteProfileString(staffType,NOTES,notes);
	ASSERT(OK);
	OK = regm.WriteProfileString(staffType,EMAIL,Email);
	ASSERT(OK);
	OK = regm.WriteProfileString(staffType,ADDRESS,address);
	ASSERT(OK);
	OK = regm.WriteProfileString(staffType,TELEPHONE,tel);
	ASSERT(OK);
	OK = regm.WriteProfileString(staffType,FUNCTION,function);
	ASSERT(OK);
}

void COperator::fromRegistry(void)
{
	CXTPRegistryManager regm;
	regm.SetRegistryKey(COMPANY_NAME,ANTHROPOLOGIA);
	firstName = regm.GetProfileString(staffType,FIRST_NAME,firstName);
	middleName = regm.GetProfileString(staffType,MIDDLE_NAME,middleName);
	lastName = regm.GetProfileString(staffType,LAST_NAME,lastName);
	notes = regm.GetProfileString(staffType,NOTES,notes);
	Email = regm.GetProfileString(staffType,EMAIL,Email);
	address = regm.GetProfileString(staffType,ADDRESS,address);
	tel = regm.GetProfileString(staffType,TELEPHONE,tel);
	function = regm.GetProfileString(staffType,FUNCTION,function);
}

void COperator::Serialize(CArchive& ar)
{ 
	if( ar.IsStoring() )
		ar << firstName << middleName << lastName << notes << Email << tel << address;
    else	 
		ar >> firstName >> middleName >> lastName >> notes >> Email >> tel >> address;
	
	if( ar.IsStoring() ) {
		ar << staffType;
		ar << function;
	}
    else {
		ar >> staffType;
		ar >> function;
	}
}


////////////////////////////////////////////////////////


CReferringDoctor::CReferringDoctor()
{
	COleDateTime now = COleDateTime::GetCurrentTime();
	dateOfReferral = now;
	firstName = _T(""); 
	middleName = _T(""); 
	lastName = _T(""); 
	notes = _T(""); 
	Email = _T(""); 
	tel = _T(""); 
	address = _T(""); 
}

CReferringDoctor::~CReferringDoctor()
{
}
	
void CReferringDoctor::clear(void)
{
	firstName = _T(""); 
	middleName = _T(""); 
	lastName = _T(""); 
	notes = _T(""); 
	Email = _T(""); 
	tel = _T(""); 
	address = _T(""); 

	dateOfReferral.SetDate(1900,1,1);
}

void CReferringDoctor::Serialize(CArchive& ar)
{ 
	if( ar.IsStoring() )
		ar << firstName << middleName << lastName << notes << Email << tel << address;
    else	 
		ar >> firstName >> middleName >> lastName >> notes >> Email >> tel >> address;
	
	if( ar.IsStoring() ) {
		ar << dateOfReferral;
	}
    else {
		ar >> dateOfReferral;
	}
}

////////////////////////////////////////////////////

CPatient::CPatient()
{
	clear();

	readPW();

	firstName = _T(""); 
	middleName = _T(""); 
	lastName = _T(""); 
	notes = _T(""); 
	Email = _T(""); 
	tel = _T(""); 
	address = _T(""); 
}

CPatient::CPatient(CString _pw)
{
	clear();

	firstName = _T("");
	middleName = _T("");
	lastName = _T("");
	notes = _T("");
	Email = _T("");
	tel = _T("");
	address = _T("");

	setPassword(_pw);  // Sets keys used by encrypted strings
}

CPatient::~CPatient()
{
}

void CPatient::clear(void)
{	
	firstName = _T(""); 
	middleName = _T(""); 
	lastName = _T(""); 
	notes = _T(""); 
	Email = _T(""); 
	tel = _T(""); 
	address = _T(""); 

	BMI = .0f;
	neckCirc = .0f;
	ESSresult = 0;	
	savePassword = showPassword = 0;
	testStr = TEST_STR;

	COleDateTime now = COleDateTime::GetCurrentTime();
	dateOfBirth.SetDate(1900,1,1);
	dateTimeOfAGS = now;
	
	gender = _T("");
	id = _T("");
}


bool CPatient::readPW(void)
{
	//---Read password
	CXTPRegistryManager reg;
	reg.SetRegistryKey(COMPANY_NAME,SETTINGS);
	CString k;
	
	/*	Read the encrypted customer key from registry 
		---------> 
					encrypted (object public variable, 16 char long)
	*/
	unsigned char orig[AES128_MAX_KEY_LENGTH];
	for (int i = 0 ; i < AES128_MAX_KEY_LENGTH ; i++) {
		k.Format(_T("%d"),i);
		orig[i] = (unsigned char) reg.GetProfileInt(USEFUL,k, defpw[i]);
	}
	memcpy((void *) encrypted,(void *) orig,AES128_MAX_KEY_LENGTH);

	/*	Decrypt the customer key using the Spiro key
		---------> 
					decrypted (local variable 16 char long)
					customerKey (object protected variable, 16 char long)
	*/
	CAES128_2 encr;
	encr.decrypt(orig);

	unsigned char decrypted[AES128_MAX_KEY_LENGTH + 1];
	memcpy((void *) decrypted,(void *) orig,AES128_MAX_KEY_LENGTH);
	decrypted[AES128_MAX_KEY_LENGTH] = 0;
	
	memcpy((void *) customerKey,(void *) orig,AES128_MAX_KEY_LENGTH);
	customerKey[AES128_MAX_KEY_LENGTH] = 0;

	eAddress.setKey(customerKey, AES128_MAX_KEY_LENGTH);
	eFirstName.setKey(customerKey, AES128_MAX_KEY_LENGTH);
	eMiddleName.setKey(customerKey, AES128_MAX_KEY_LENGTH);
	eLastName.setKey(customerKey, AES128_MAX_KEY_LENGTH);
	eID.setKey(customerKey, AES128_MAX_KEY_LENGTH);
	eEmail.setKey(customerKey, AES128_MAX_KEY_LENGTH);
	eTel.setKey(customerKey, AES128_MAX_KEY_LENGTH);
	eTestStr.setKey(customerKey, AES128_MAX_KEY_LENGTH);

	CString ps(decrypted);
	password = ps;
	globalPassword = ps;
	return true;
}

/*
Description: Encrypts the current password and saves to registry
*/
bool CPatient::savePW(void)
{
	CStringA a(password);
	while (a.GetLength() < AES128_MAX_KEY_LENGTH) 
		a += ' ';

	unsigned char orig[AES128_MAX_KEY_LENGTH];
	for (int i = 0 ; i < AES128_MAX_KEY_LENGTH ; i++) orig[i] = a.GetAt(i);

	CAES128_2 encr;
	encr.encrypt(orig);	
	memcpy((void *) encrypted,(void *) orig,AES128_MAX_KEY_LENGTH);

	CXTPRegistryManager reg;
	reg.SetRegistryKey(COMPANY_NAME,SETTINGS);
	CString k;
	for (int i = 0 ; i < AES128_MAX_KEY_LENGTH ; i++) {
		k.Format(_T("%d"),i);
		reg.WriteProfileInt(USEFUL,k,(int) orig[i]);
	}	
	return true;
}

bool CPatient::encryptData(void)
{
	eAddress.encryptString(address);
	eFirstName.encryptString(firstName);
	eMiddleName.encryptString(middleName);
	eLastName.encryptString(lastName);
	eID.encryptString(id);
	eEmail.encryptString(Email);
	eTel.encryptString(tel);
	eTestStr.encryptString(testStr);

	CString s = eTestStr.getDecryptedString();

	return s == testStr ? true : false;
}

bool CPatient::decryptData(void) 
{
	CString tryTestStr = eTestStr.getDecryptedString();
	if (TEST_STR == tryTestStr) {
		address = eAddress.getDecryptedString();
		firstName = eFirstName.getDecryptedString();
		middleName = eMiddleName.getDecryptedString();
		lastName = eLastName.getDecryptedString();
		id = eID.getDecryptedString();
		Email = eEmail.getDecryptedString();
		tel = eTel.getDecryptedString();
	}
	else {
		CString hids;
		int nums = hids.LoadString(IDS_HIDDEN_STRING);
		address = hids;
		firstName = hids;
		middleName = hids;
		lastName = hids;
		id = hids;
		Email = hids;
		tel = hids;
	}
	return true;
}

void CPatient::Serialize(CArchive& ar)
{ 
	if( ar.IsStoring() ) {
		ar << dateTimeOfAGS << dateOfBirth << gender << id;
		ar << ESSresult << BMI << neckCirc;
		ar << recordingID;
		ar << notes;
		
		//--Encrypted
#ifndef IN_SPIRO_ANALYSIS   
        encryptData();	// Only do this in AGSBrowser
#endif
		eTestStr.Serialize(ar);
		eAddress.Serialize(ar);
		eFirstName.Serialize(ar);
		eMiddleName.Serialize(ar);
		eLastName.Serialize(ar);
		eID.Serialize(ar);
		eEmail.Serialize(ar);
		eTel.Serialize(ar);
	}
    else {	 
		ar >> dateTimeOfAGS >> dateOfBirth >> gender >> id;
		ar >> ESSresult >> BMI >> neckCirc;
		ar >> recordingID;
		if (AGSMajorVersionRead >= 3) 
			ar >> notes;
		
		//--Encrypted
		eTestStr.Serialize(ar);
		eAddress.Serialize(ar);
		eFirstName.Serialize(ar);
		eMiddleName.Serialize(ar);
		eLastName.Serialize(ar);
		eID.Serialize(ar);
		
		if (AGSMajorVersionRead >= 14) {
			eEmail.Serialize(ar);
			eTel.Serialize(ar);
		}
		CString tryTestStr = eTestStr.getDecryptedString();
		if (TEST_STR == tryTestStr) {
			address = eAddress.getDecryptedString();
			firstName = eFirstName.getDecryptedString();
			middleName = eMiddleName.getDecryptedString();
			lastName = eLastName.getDecryptedString();
			id = eID.getDecryptedString();
			if (AGSMajorVersionRead >= 14) {
				Email = eEmail.getDecryptedString();
				tel = eTel.getDecryptedString();
			}
		}
		else {
			CString hids;
			int nums = hids.LoadString(IDS_HIDDEN_STRING);
			address = hids;
			firstName = hids;
			middleName = hids;
			lastName = hids;
			id = hids;
			Email = hids;
			tel = hids;
		}
	}
}

CString CPatient::getPassword(void)
{
	return password;
}

/*
Description: Sets a new password (without save)
Sends password on to enrypted strings
*/
void CPatient::setPassword(CString _pw)
{
	CStringA a(_pw);
	while (a.GetLength() < AES128_MAX_KEY_LENGTH)
		a += ' ';

	password = a;
	globalPassword = a;

	unsigned char orig[AES128_MAX_KEY_LENGTH];
	for (int i = 0; i < AES128_MAX_KEY_LENGTH; i++) orig[i] = a.GetAt(i);

	memcpy((void *)customerKey, (void *)orig, AES128_MAX_KEY_LENGTH);
	customerKey[AES128_MAX_KEY_LENGTH] = 0;

	eAddress.setKey(customerKey, AES128_MAX_KEY_LENGTH);
	eFirstName.setKey(customerKey, AES128_MAX_KEY_LENGTH);
	eMiddleName.setKey(customerKey, AES128_MAX_KEY_LENGTH);
	eLastName.setKey(customerKey, AES128_MAX_KEY_LENGTH);
	eID.setKey(customerKey, AES128_MAX_KEY_LENGTH);
	eEmail.setKey(customerKey, AES128_MAX_KEY_LENGTH);
	eTel.setKey(customerKey, AES128_MAX_KEY_LENGTH);
	eTestStr.setKey(customerKey, AES128_MAX_KEY_LENGTH);
}

bool CPatient::getIsPasswordOK(CString _pw,CString *_errmess)
{
	CString pw = _pw;

	//---Remove trailing spaces in case this has not been done
	pw.TrimRight(ASCII_SPACE);

	int len = pw.GetLength();
	if (len > AES128_MAX_KEY_LENGTH) {
		pw = pw.Left(AES128_MAX_KEY_LENGTH);

		CString s;
		int nums = s.LoadString(IDS_PW_TOO_LONG);
		*_errmess = s;
		return false;
	}

	CString s = pw;
	int pos = pw.Find(_T(" "), 0);
	if (-1 != pos) {
		CString sw;
		int nums = sw.LoadString(IDS_SPACE_IN_PW);
		*_errmess = sw;
		return false;
	}
	
	//---Check length after trimming
	len = pw.GetLength();
	if (0 == len) {
		CString s;
		int nums = s.LoadString(IDS_ZERO_PW);
		*_errmess = s;
		return false;
	}
	else if (MIN_PW_LEN > len) {
		CString s;
		int nums = s.LoadString(IDS_TOO_SHORT_PW);
		CString mess;
		mess.Format(s, MIN_PW_LEN);
		*_errmess = mess;
		return false;
	}
	*_errmess = _T("");
	return true;
}


///////////////////////////////////

CMyGridItemPW::CMyGridItemPW(UINT nID,
    LPCTSTR strValue, 
    CString* pBindString) : CXTPPropertyGridItem(nID,strValue, pBindString)
{
	SetMask(_T("AAAAAAAAAAAAAAAA"), _T("________________"), _T('_'));
}

void CMyGridItemPW::usePWMask(bool _on)
{
	m_bPassword = _on ? TRUE : FALSE;
}


void CMyGridItemPW::OnValueChanged(CString strValue)
{
	int len = strValue.GetLength();
	if (len > AES128_MAX_KEY_LENGTH) {
		strValue = strValue.Left(AES128_MAX_KEY_LENGTH);
		Beep(1000,50);
		CString s;
		int nums = s.LoadString(IDS_PW_TOO_LONG);
		AfxMessageBox(s);
	}
	if (0 == len) {
		CString s;
		int nums = s.LoadString(IDS_ZERO_PW);
		AfxMessageBox(s);
	}

	//---Remove placeholders
	CString s = strValue;
	CString nets = s.TrimRight(_T('_'));
	int pos = nets.Find(_T("_"), 0);
	if (-1 != pos) {
		CString sw;
		int nums = sw.LoadString(IDS_SPACE_IN_PW);
		AfxMessageBox(sw);
	}
	CStringA a(strValue);  // Convert to char
	CString w(a);
	SetValue(w);
}
