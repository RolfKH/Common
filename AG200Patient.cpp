// Patient.cpp : implementation file
//
/*!
* @author		RKH
* @date		06.06.2017
* @copyright (C)Spiro Medical AS 2013 - 2017
*/

#include "stdafx.h"
#include "AG200Patient.h"


#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

////////////////////////////////
// Patient data

CPatientData::CPatientData()
//**************************
{
	bmi = - 1.0f;
	height = - 1.0f;
	weight = - 1.0f;
	rr = - 1.0f;
	age = - 1;
	neckCirc = .0f;
	sex = SEX_UNKNOWN;
	anonymous = FALSE;
}

CPatientData::~CPatientData()
//***************************
{
}

void CPatientData::reset(void)
//****************************
{
	firstName = "";
	familyName = "";
	setFullName();
	idCode = "";
	comments = "";
	address = "";
	tel = "";
	eMail = "";

	bmi = -1.0f;
	height = -1.0f;
	weight = -1.0f;
	rr = -1.0f;
	age = - 1;
	neckCirc = .0f;
	sex = SEX_UNKNOWN;
}


int CPatientData::getAnonymous(void) { return anonymous; }

int CPatientData::setAnonymous(int _an)
//**************************************
{ 
	int dirty = anonymous != _an ? TRUE : FALSE; 
	anonymous = _an; 
	return dirty;
}

CString CPatientData::getName(void) { return fullName; }

CString CPatientData::getComments(void) { return comments; }
CString CPatientData::getIdCode(void) { return idCode; }
CString CPatientData::getAddress(void) { return address; }
CString CPatientData::getTel(void) { return tel; }
CString CPatientData::getEmail(void) { return eMail; }
	

int CPatientData::setHospital(CString _hosp)
//******************************************
{ 
	int dirty = hospital != _hosp ? TRUE : FALSE; 
	hospital = _hosp; 
	return dirty;
}

int CPatientData::setComments(CString _c)
//****************************************
{ 
	int dirty = comments != _c ? TRUE : FALSE; 
	comments = _c; 
	return dirty;
}

int CPatientData::setIdCode(CString _id)
//****************************************
{ 
	int dirty = idCode != _id ? TRUE : FALSE; 
	idCode = _id; 
	return dirty;
}

int CPatientData::setAddress(CString _addr)
//****************************************
{ 
	int dirty = address != _addr ? TRUE : FALSE; 
	address = _addr; 
	return dirty;
}

int CPatientData::setTel(CString _tel)
//****************************************
{ 
	int dirty = tel != _tel ? TRUE : FALSE; 
	tel = _tel; 
	return dirty;
}

int CPatientData::setEmail(CString _eMail)
//****************************************
{ 
	int dirty = eMail != _eMail ? TRUE : FALSE; 
	eMail = _eMail; 
	return dirty;
}
 
int CPatientData::setFirstName(CString _fName)
//****************************************
{ 
	int dirty = firstName != _fName ? TRUE : FALSE; 
	firstName = _fName; 
	setFullName();
	return dirty;
} 

int CPatientData::setFamilyName(CString _lName)
//**********************************************
{ 
	int dirty = familyName != _lName ? TRUE : FALSE; 
	familyName = _lName; 
	setFullName();
	return dirty;
}

void CPatientData::getNames(CString *_first,CString *_family)
//***********************************************************
{
	*_first = firstName;
	*_family = familyName;
}

CString CPatientData::getHospital(void)
//*************************************
{ 
	return hospital;
}
void CPatientData::setFullName(void)
//***********************************
{
	fullName = firstName;
	fullName += " ";
	fullName += familyName;
}

void CPatientData::readFromArchive(CFile *_f,int _swVer)
//******************************************************
{
	CArchive arch(_f, CArchive::load);
	arch >> firstName;
	arch >> familyName;
	setFullName();
	arch >> idCode;
	arch >> comments;
	arch >> address;
	arch >> tel;
	arch >> eMail;

	if (_swVer >= 670) {
		arch >> anonymous;
	}

	if (_swVer >= 620) {
		arch >> age;
		arch >> sex;
		arch >> height;
		arch >> weight;
		arch >> rr;
	    arch >> neckCirc;
		computeBMI();
	}
	if (_swVer < 620) {
		bmi = - 1.0f;
		height = - 1.0f;
		weight = - 1.0f;
		rr = - 1.0f;
		age = - 1;
		neckCirc = - 1.0f;
		sex = SEX_UNKNOWN;
	}
	
	if (_swVer >= 641) {
		arch >> hospital;
	}
	if (_swVer < 641) {
	    hospital = "";
	}
}

void CPatientData::writeToArchive(CFile *_f)
//******************************************
{
	CArchive arch(_f, CArchive::store);
	arch << firstName;
	arch << familyName;
	arch << idCode;
	arch << comments;
	arch << address;
	arch << tel;
	arch << eMail;
	arch << anonymous;
	arch << age;
	arch << sex;
	arch << height;
	arch << weight;
	arch << rr;
	arch << neckCirc;
	arch << hospital;
}


int CPatientData::setWeight(double _w)
//************************************
{	
	int dirty = weight != _w ? TRUE : FALSE; 
	weight = _w; 
	computeBMI();
	return dirty;
}

int CPatientData::setHeight(double _h) 
//************************************
{	
	int dirty = height != _h ? TRUE : FALSE; 
	height = _h; 
	computeBMI();
	return dirty;
}

int CPatientData::setSex(int _sex)
//************************************
{	
	int dirty = sex != _sex ? TRUE : FALSE; 
	sex = _sex;
	return dirty;
}

int CPatientData::setNeckCirc(double _nc)
//***************************************
{
	int dirty = neckCirc != _nc ? TRUE : FALSE; 
	neckCirc = _nc; 
	return dirty;
}

int CPatientData::setRR(double _rr)
//************************************
{	
	int dirty = rr != _rr ? TRUE : FALSE; 
	rr = _rr; 
	return dirty;
}

int CPatientData::setAge(int _age)
//************************************
{	
	int dirty = age != _age ? TRUE : FALSE; 
	age = _age; 
	return dirty;
}

void CPatientData::computeBMI(void)
//*********************************
{
	double hMeter = height / 100.0f;
	bmi = weight / (hMeter * hMeter);
}


CString CPatientData::getSexS(void)
//********************************
{
	CString g;
	switch (sex) {
	case SEX_MALE :
		g = _T("Male");
		break;
	case SEX_FEMALE :
		g = _T("Female");
		break;
	default :
		g = _T("Unknown");
		break;
	}
	return g;
}

int CPatientData::getSex(void) { return sex; }

double CPatientData::getWeight(void) { 
	return weight; 
}
double CPatientData::getHeight(void) { return height; }
double CPatientData::getRR(void) { return rr; }
int CPatientData::getAge(void) { return age ; }
double CPatientData::getBMI(void) { return bmi; }
double CPatientData::getNeckCirc(void) { return neckCirc; }

