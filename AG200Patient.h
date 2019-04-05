#if !defined(AFX_PATIENT_H__0EC37462_0BF4_11D4_A132_00A0C9090ABE__INCLUDED_)
#define AFX_PATIENT_H__0EC37462_0BF4_11D4_A132_00A0C9090ABE__INCLUDED_
/*!
	@file		AG200_Patient.h
*/

/*!
* @author		RKH
* @date		06.06.2017
* @copyright (C)Spiro Medical AS 2013 - 2017
*/

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
// Patient.h : header file
//
#include "afxwin.h"

#define SEX_MALE	1
#define SEX_FEMALE	2
#define SEX_UNKNOWN	3

/*
	Ver 670 : 
		15.5.05 RKH. Included anonymous
*/

/////////////////////////////////////////////////////////////////////////////
// CPatient dialog

//--------------------------------------------------
#define PAT_FIELD_LEN   40
#define PAT_FIELD_LIMIT 38
#define COMM_LEN        256

class CPatientData 
{
public:
	CPatientData();
	~CPatientData();
	void readFromArchive(CFile *_f,int _swVer);
	void writeToArchive(CFile *_f);
	CString getName(void);
	CString getHospital(void);
	CString getComments(void);
	CString getIdCode(void);
	CString getAddress(void);
	CString getTel(void);
	CString getEmail(void);
	double getWeight(void);
	double getHeight(void);
	int getSex(void);
	CString getSexS(void);
	double getRR(void);
	int getAge(void);
	double getBMI(void);
	double getNeckCirc(void);
	void getNames(CString *_first,CString *_family);
	
	int setComments(CString _c);
	int setIdCode(CString _id);
	int setAddress(CString _addr);
	int setHospital(CString _hosp);
	int setTel(CString _tel);
	int setEmail(CString _eMail);
	int setFirstName(CString _fName);
	int setFamilyName(CString _lName);
	int setWeight(double _w);
	int setHeight(double _h);
	int setSex(int _sex);
	int setRR(double _rr);
	int setNeckCirc(double _nc);
	int setAge(int _age);
	void reset(void);
	int getAnonymous(void);
	int setAnonymous(int _an);
protected:
	CString firstName;
	CString familyName;
	CString fullName;
	CString idCode;
	CString hospital;
	CString comments,address,tel,eMail;
	double bmi,height,weight,rr,neckCirc;
	int anonymous;
	int age;
	int sex;
	void setFullName(void);
	void computeBMI(void);
};

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_PATIENT_H__0EC37462_0BF4_11D4_A132_00A0C9090ABE__INCLUDED_)
