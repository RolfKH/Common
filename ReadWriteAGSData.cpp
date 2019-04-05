/*!
	@file		ReadWriteAGSData.cpp
	
*/

/*!
* @author		RKH
* @date		06.06.2017
* @copyright (C)Spiro Medical AS 2013 - 2017
*/

#include "stdafx.h"
#include "ReadWriteAGSData.h"
#include <math.h>

extern CString swVersion;

int AGSMajorVersionRead = 0;

CReadWriteAGSData::CReadWriteAGSData()
{
	minorFileVersion = AGS_FILE_MINOR_VERSION;
	majorFileVersion = AGS_FILE_MAJOR_VERSION;
	pt		= NULL;
	dct		= NULL;
	op		= NULL;
	rd		= NULL;
	
	battData	= NULL;
	cmData		= NULL;
	amData		= NULL;
	bpXYZData	= NULL;
	acData		= NULL;
	torsoAcData = NULL;
	oxData		= NULL;
	bpData		= NULL;
	cathData	= NULL;
	respData	= NULL;
	mMarkerData = NULL;
	
	neckParameters		= NULL;
	wristParameters		= NULL;
	cathParameters		= NULL;
	torsoParameters		= NULL;

	spareApnBlock	= NULL;
	apnFileHeader	= NULL;
	m_setupData		= NULL;
	m_LoggerData	= NULL;
	m_CathData		= NULL;

	AG200TypeData	= false;
}

CReadWriteAGSData::~CReadWriteAGSData()
{
}

CString CReadWriteAGSData::getMinSWversionToReadFile(int _majorFileVersion, int _minorFileVersion)
{
	CString s = _T("?");
	switch (_majorFileVersion) {
	case 1:
		s = _T("1.8");
		break;
	case 2:
		s = _T("1.9");
		break;
	case 3:
		s = _T("3.1");
		break;
	case 4:
		s = _T("3.5");
		break;
	case 5:
		s = _T("4.0");
		break;
	case 6:
		s = _T("4.0");
		break;
	case 7:
		s = _T("4.1");
		break;
	case 8:
		s = _T("4.3");
		break;
	case 9:
		s = _T("4.5.1");
		break;
	case 10:
		s = _T("4.5.2");
		break;
	case 11:
		s = _T("4.5.3");
		break;
	case 12:
		s = _T("4.5.4");
		break;
	case 13:
		s = _T("4.5.5");
		break;
	case 14:
		s = _T("5.6.1");
		break;
	}
	return s;
}

bool CReadWriteAGSData::checkVersionCompatibility(bool _issueWarning)
{
	if (_T("6.1.1.0") == swVersion) {
		if ((2 == majorFileVersion) && (0 == minorFileVersion)) return true;
		if ((3 == majorFileVersion) && (0 == minorFileVersion)) return true;
		if ((4 == majorFileVersion) && (0 == minorFileVersion)) return true;
		if ((5 == majorFileVersion) && (0 == minorFileVersion)) return true;
		if ((6 == majorFileVersion) && (0 == minorFileVersion)) return true;
		if ((7 == majorFileVersion) && (0 == minorFileVersion)) return true;
		if ((8 == majorFileVersion) && (0 == minorFileVersion)) return true;
		if ((9 == majorFileVersion) && (0 == minorFileVersion)) return true;
		if ((10 == majorFileVersion) && (0 == minorFileVersion)) return true;
		if ((11 == majorFileVersion) && (0 == minorFileVersion)) return true;
		if ((12 == majorFileVersion) && (0 == minorFileVersion)) return true;
		if ((13 == majorFileVersion) && (0 == minorFileVersion)) return true;
		if ((14 == majorFileVersion) && (0 == minorFileVersion)) return true;
		if ((15 == majorFileVersion) && (0 == minorFileVersion)) return true; // New in version 6.1.1
	}
	else if (_T("6.1.0.0") == swVersion) {
		if ((2 == majorFileVersion) && (0 == minorFileVersion)) return true;
		if ((3 == majorFileVersion) && (0 == minorFileVersion)) return true;
		if ((4 == majorFileVersion) && (0 == minorFileVersion)) return true;
		if ((5 == majorFileVersion) && (0 == minorFileVersion)) return true;
		if ((6 == majorFileVersion) && (0 == minorFileVersion)) return true;
		if ((7 == majorFileVersion) && (0 == minorFileVersion)) return true;
		if ((8 == majorFileVersion) && (0 == minorFileVersion)) return true;
		if ((9 == majorFileVersion) && (0 == minorFileVersion)) return true;
		if ((10 == majorFileVersion) && (0 == minorFileVersion)) return true;
		if ((11 == majorFileVersion) && (0 == minorFileVersion)) return true;
		if ((12 == majorFileVersion) && (0 == minorFileVersion)) return true;
		if ((13 == majorFileVersion) && (0 == minorFileVersion)) return true;
		if ((14 == majorFileVersion) && (0 == minorFileVersion)) return true;
	}
	else if (_T("6.0.0.0") == swVersion) {
		if ((2 == majorFileVersion) && (0 == minorFileVersion)) return true;
		if ((3 == majorFileVersion) && (0 == minorFileVersion)) return true;
		if ((4 == majorFileVersion) && (0 == minorFileVersion)) return true;
		if ((5 == majorFileVersion) && (0 == minorFileVersion)) return true;
		if ((6 == majorFileVersion) && (0 == minorFileVersion)) return true;
		if ((7 == majorFileVersion) && (0 == minorFileVersion)) return true;
		if ((8 == majorFileVersion) && (0 == minorFileVersion)) return true;
		if ((9 == majorFileVersion) && (0 == minorFileVersion)) return true;
		if ((10 == majorFileVersion) && (0 == minorFileVersion)) return true;
		if ((11 == majorFileVersion) && (0 == minorFileVersion)) return true;
		if ((12 == majorFileVersion) && (0 == minorFileVersion)) return true;
		if ((13 == majorFileVersion) && (0 == minorFileVersion)) return true;
		if ((14 == majorFileVersion) && (0 == minorFileVersion)) return true;
	}
	else if (_T("5.7.1.0") == swVersion) {
		if ((2 == majorFileVersion) && (0 == minorFileVersion)) return true;
		if ((3 == majorFileVersion) && (0 == minorFileVersion)) return true;
		if ((4 == majorFileVersion) && (0 == minorFileVersion)) return true;
		if ((5 == majorFileVersion) && (0 == minorFileVersion)) return true;
		if ((6 == majorFileVersion) && (0 == minorFileVersion)) return true;
		if ((7 == majorFileVersion) && (0 == minorFileVersion)) return true;
		if ((8 == majorFileVersion) && (0 == minorFileVersion)) return true;
		if ((9 == majorFileVersion) && (0 == minorFileVersion)) return true;
		if ((10 == majorFileVersion) && (0 == minorFileVersion)) return true;
		if ((11 == majorFileVersion) && (0 == minorFileVersion)) return true;
		if ((12 == majorFileVersion) && (0 == minorFileVersion)) return true;
		if ((13 == majorFileVersion) && (0 == minorFileVersion)) return true;
		if ((14 == majorFileVersion) && (0 == minorFileVersion)) return true;
	}
	else if (_T("5.7.0.0") == swVersion) {
		if ((2 == majorFileVersion) && (0 == minorFileVersion)) return true;
		if ((3 == majorFileVersion) && (0 == minorFileVersion)) return true;
		if ((4 == majorFileVersion) && (0 == minorFileVersion)) return true;
		if ((5 == majorFileVersion) && (0 == minorFileVersion)) return true;
		if ((6 == majorFileVersion) && (0 == minorFileVersion)) return true;
		if ((7 == majorFileVersion) && (0 == minorFileVersion)) return true;
		if ((8 == majorFileVersion) && (0 == minorFileVersion)) return true;
		if ((9 == majorFileVersion) && (0 == minorFileVersion)) return true;
		if ((10 == majorFileVersion) && (0 == minorFileVersion)) return true;
		if ((11 == majorFileVersion) && (0 == minorFileVersion)) return true;
		if ((12 == majorFileVersion) && (0 == minorFileVersion)) return true;
		if ((13 == majorFileVersion) && (0 == minorFileVersion)) return true;
		if ((14 == majorFileVersion) && (0 == minorFileVersion)) return true;
	}
	else if (_T("5.6.2.0") == swVersion) {
		if ((2 == majorFileVersion) && (0 == minorFileVersion)) return true;
		if ((3 == majorFileVersion) && (0 == minorFileVersion)) return true;
		if ((4 == majorFileVersion) && (0 == minorFileVersion)) return true;
		if ((5 == majorFileVersion) && (0 == minorFileVersion)) return true;
		if ((6 == majorFileVersion) && (0 == minorFileVersion)) return true;
		if ((7 == majorFileVersion) && (0 == minorFileVersion)) return true;
		if ((8 == majorFileVersion) && (0 == minorFileVersion)) return true;
		if ((9 == majorFileVersion) && (0 == minorFileVersion)) return true;
		if ((10 == majorFileVersion) && (0 == minorFileVersion)) return true;
		if ((11 == majorFileVersion) && (0 == minorFileVersion)) return true;
		if ((12 == majorFileVersion) && (0 == minorFileVersion)) return true;
		if ((13 == majorFileVersion) && (0 == minorFileVersion)) return true;
		if ((14 == majorFileVersion) && (0 == minorFileVersion)) return true;
	}
	else if (_T("5.6.1.0") == swVersion) {
		if ((2 == majorFileVersion) && (0 == minorFileVersion)) return true;
		if ((3 == majorFileVersion) && (0 == minorFileVersion)) return true;
		if ((4 == majorFileVersion) && (0 == minorFileVersion)) return true;
		if ((5 == majorFileVersion) && (0 == minorFileVersion)) return true;
		if ((6 == majorFileVersion) && (0 == minorFileVersion)) return true;
		if ((7 == majorFileVersion) && (0 == minorFileVersion)) return true;
		if ((8 == majorFileVersion) && (0 == minorFileVersion)) return true;
		if ((9 == majorFileVersion) && (0 == minorFileVersion)) return true;
		if ((10 == majorFileVersion) && (0 == minorFileVersion)) return true;
		if ((11 == majorFileVersion) && (0 == minorFileVersion)) return true;
		if ((12 == majorFileVersion) && (0 == minorFileVersion)) return true;
		if ((13 == majorFileVersion) && (0 == minorFileVersion)) return true;
		if ((14 == majorFileVersion) && (0 == minorFileVersion)) return true;
	}
	else if (_T("5.6.0.0") == swVersion) {
		if ((2 == majorFileVersion) && (0 == minorFileVersion)) return true;
		if ((3 == majorFileVersion) && (0 == minorFileVersion)) return true;
		if ((4 == majorFileVersion) && (0 == minorFileVersion)) return true;
		if ((5 == majorFileVersion) && (0 == minorFileVersion)) return true;
		if ((6 == majorFileVersion) && (0 == minorFileVersion)) return true;
		if ((7 == majorFileVersion) && (0 == minorFileVersion)) return true;
		if ((8 == majorFileVersion) && (0 == minorFileVersion)) return true;
		if ((9 == majorFileVersion) && (0 == minorFileVersion)) return true;
		if ((10 == majorFileVersion) && (0 == minorFileVersion)) return true;
		if ((11 == majorFileVersion) && (0 == minorFileVersion)) return true;
		if ((12 == majorFileVersion) && (0 == minorFileVersion)) return true;
		if ((13 == majorFileVersion) && (0 == minorFileVersion)) return true;
	}
	else if (_T("5.5.0.0") == swVersion) {
		if ((2 == majorFileVersion) && (0 == minorFileVersion)) return true;
		if ((3 == majorFileVersion) && (0 == minorFileVersion)) return true;
		if ((4 == majorFileVersion) && (0 == minorFileVersion)) return true;
		if ((5 == majorFileVersion) && (0 == minorFileVersion)) return true;
		if ((6 == majorFileVersion) && (0 == minorFileVersion)) return true;
		if ((7 == majorFileVersion) && (0 == minorFileVersion)) return true;
		if ((8 == majorFileVersion) && (0 == minorFileVersion)) return true;
		if ((9 == majorFileVersion) && (0 == minorFileVersion)) return true;
		if ((10 == majorFileVersion) && (0 == minorFileVersion)) return true;
		if ((11 == majorFileVersion) && (0 == minorFileVersion)) return true;
		if ((12 == majorFileVersion) && (0 == minorFileVersion)) return true;
		if ((13 == majorFileVersion) && (0 == minorFileVersion)) return true;
	}
	else if (_T("5.4.0.0") == swVersion) {
		if ((2 == majorFileVersion) && (0 == minorFileVersion)) return true;
		if ((3 == majorFileVersion) && (0 == minorFileVersion)) return true;
		if ((4 == majorFileVersion) && (0 == minorFileVersion)) return true;
		if ((5 == majorFileVersion) && (0 == minorFileVersion)) return true;
		if ((6 == majorFileVersion) && (0 == minorFileVersion)) return true;
		if ((7 == majorFileVersion) && (0 == minorFileVersion)) return true;
		if ((8 == majorFileVersion) && (0 == minorFileVersion)) return true;
		if ((9 == majorFileVersion) && (0 == minorFileVersion)) return true;
		if ((10 == majorFileVersion) && (0 == minorFileVersion)) return true;
		if ((11 == majorFileVersion) && (0 == minorFileVersion)) return true;
		if ((12 == majorFileVersion) && (0 == minorFileVersion)) return true;
		if ((13 == majorFileVersion) && (0 == minorFileVersion)) return true;
	}
	else if (_T("5.3.0.0") == swVersion) {
		if ((2 == majorFileVersion) && (0 == minorFileVersion)) return true;
		if ((3 == majorFileVersion) && (0 == minorFileVersion)) return true;
		if ((4 == majorFileVersion) && (0 == minorFileVersion)) return true;
		if ((5 == majorFileVersion) && (0 == minorFileVersion)) return true;
		if ((6 == majorFileVersion) && (0 == minorFileVersion)) return true;
		if ((7 == majorFileVersion) && (0 == minorFileVersion)) return true;
		if ((8 == majorFileVersion) && (0 == minorFileVersion)) return true;
		if ((9 == majorFileVersion) && (0 == minorFileVersion)) return true;
		if ((10 == majorFileVersion) && (0 == minorFileVersion)) return true;
		if ((11 == majorFileVersion) && (0 == minorFileVersion)) return true;
		if ((12 == majorFileVersion) && (0 == minorFileVersion)) return true;
		if ((13 == majorFileVersion) && (0 == minorFileVersion)) return true;
	}
	else if (_T("5.2.2.0") == swVersion) {
		if ((2 == majorFileVersion) && (0 == minorFileVersion)) return true;
		if ((3 == majorFileVersion) && (0 == minorFileVersion)) return true;
		if ((4 == majorFileVersion) && (0 == minorFileVersion)) return true;
		if ((5 == majorFileVersion) && (0 == minorFileVersion)) return true;
		if ((6 == majorFileVersion) && (0 == minorFileVersion)) return true;
		if ((7 == majorFileVersion) && (0 == minorFileVersion)) return true;
		if ((8 == majorFileVersion) && (0 == minorFileVersion)) return true;
		if ((9 == majorFileVersion) && (0 == minorFileVersion)) return true;
		if ((10 == majorFileVersion) && (0 == minorFileVersion)) return true;
		if ((11 == majorFileVersion) && (0 == minorFileVersion)) return true;
		if ((12 == majorFileVersion) && (0 == minorFileVersion)) return true;
		if ((13 == majorFileVersion) && (0 == minorFileVersion)) return true;
	}
	else if (_T("5.2.1.0") == swVersion) {
		if ((2 == majorFileVersion) && (0 == minorFileVersion)) return true;
		if ((3 == majorFileVersion) && (0 == minorFileVersion)) return true;
		if ((4 == majorFileVersion) && (0 == minorFileVersion)) return true;
		if ((5 == majorFileVersion) && (0 == minorFileVersion)) return true;
		if ((6 == majorFileVersion) && (0 == minorFileVersion)) return true;
		if ((7 == majorFileVersion) && (0 == minorFileVersion)) return true;
		if ((8 == majorFileVersion) && (0 == minorFileVersion)) return true;
		if ((9 == majorFileVersion) && (0 == minorFileVersion)) return true;
		if ((10 == majorFileVersion) && (0 == minorFileVersion)) return true;
		if ((11 == majorFileVersion) && (0 == minorFileVersion)) return true;
		if ((12 == majorFileVersion) && (0 == minorFileVersion)) return true;
		if ((13 == majorFileVersion) && (0 == minorFileVersion)) return true;
	}
	else if (_T("5.2.0.0") == swVersion) {
		if ((2 == majorFileVersion) && (0 == minorFileVersion)) return true;
		if ((3 == majorFileVersion) && (0 == minorFileVersion)) return true;
		if ((4 == majorFileVersion) && (0 == minorFileVersion)) return true;
		if ((5 == majorFileVersion) && (0 == minorFileVersion)) return true;
		if ((6 == majorFileVersion) && (0 == minorFileVersion)) return true;
		if ((7 == majorFileVersion) && (0 == minorFileVersion)) return true;
		if ((8 == majorFileVersion) && (0 == minorFileVersion)) return true;
		if ((9 == majorFileVersion) && (0 == minorFileVersion)) return true;
		if ((10 == majorFileVersion) && (0 == minorFileVersion)) return true;
		if ((11 == majorFileVersion) && (0 == minorFileVersion)) return true;
		if ((12 == majorFileVersion) && (0 == minorFileVersion)) return true;
		if ((13 == majorFileVersion) && (0 == minorFileVersion)) return true;
	}
	else if (_T("5.1.1.0") == swVersion) {
		if ((2 == majorFileVersion) && (0 == minorFileVersion)) return true;
		if ((3 == majorFileVersion) && (0 == minorFileVersion)) return true;
		if ((4 == majorFileVersion) && (0 == minorFileVersion)) return true;
		if ((5 == majorFileVersion) && (0 == minorFileVersion)) return true;
		if ((6 == majorFileVersion) && (0 == minorFileVersion)) return true;
		if ((7 == majorFileVersion) && (0 == minorFileVersion)) return true;
		if ((8 == majorFileVersion) && (0 == minorFileVersion)) return true;
		if ((9 == majorFileVersion) && (0 == minorFileVersion)) return true;
		if ((10 == majorFileVersion) && (0 == minorFileVersion)) return true;
		if ((11 == majorFileVersion) && (0 == minorFileVersion)) return true;
		if ((12 == majorFileVersion) && (0 == minorFileVersion)) return true;
		if ((13 == majorFileVersion) && (0 == minorFileVersion)) return true;
	}
	else if (_T("5.1.0.0") == swVersion) {
		if ((2 == majorFileVersion) && (0 == minorFileVersion)) return true;
		if ((3 == majorFileVersion) && (0 == minorFileVersion)) return true;
		if ((4 == majorFileVersion) && (0 == minorFileVersion)) return true;
		if ((5 == majorFileVersion) && (0 == minorFileVersion)) return true;
		if ((6 == majorFileVersion) && (0 == minorFileVersion)) return true;
		if ((7 == majorFileVersion) && (0 == minorFileVersion)) return true;
		if ((8 == majorFileVersion) && (0 == minorFileVersion)) return true;
		if ((9 == majorFileVersion) && (0 == minorFileVersion)) return true;
		if ((10 == majorFileVersion) && (0 == minorFileVersion)) return true;
		if ((11 == majorFileVersion) && (0 == minorFileVersion)) return true;
		if ((12 == majorFileVersion) && (0 == minorFileVersion)) return true;
		if ((13 == majorFileVersion) && (0 == minorFileVersion)) return true;
	}
	else if (_T("5.0.0.0") == swVersion) {
		if ((2 == majorFileVersion) && (0 == minorFileVersion)) return true;
		if ((3 == majorFileVersion) && (0 == minorFileVersion)) return true;
		if ((4 == majorFileVersion) && (0 == minorFileVersion)) return true;
		if ((5 == majorFileVersion) && (0 == minorFileVersion)) return true;
		if ((6 == majorFileVersion) && (0 == minorFileVersion)) return true;
		if ((7 == majorFileVersion) && (0 == minorFileVersion)) return true;
		if ((8 == majorFileVersion) && (0 == minorFileVersion)) return true;
		if ((9 == majorFileVersion) && (0 == minorFileVersion)) return true;
		if ((10 == majorFileVersion) && (0 == minorFileVersion)) return true;
		if ((11 == majorFileVersion) && (0 == minorFileVersion)) return true;
		if ((12 == majorFileVersion) && (0 == minorFileVersion)) return true;
		if ((13 == majorFileVersion) && (0 == minorFileVersion)) return true;
	}
	else if (_T("4.5.20.0") == swVersion) {
		if ((2 == majorFileVersion) && (0 == minorFileVersion)) return true;
		if ((3 == majorFileVersion) && (0 == minorFileVersion)) return true;
		if ((4 == majorFileVersion) && (0 == minorFileVersion)) return true;
		if ((5 == majorFileVersion) && (0 == minorFileVersion)) return true;
		if ((6 == majorFileVersion) && (0 == minorFileVersion)) return true;
		if ((7 == majorFileVersion) && (0 == minorFileVersion)) return true;
		if ((8 == majorFileVersion) && (0 == minorFileVersion)) return true;
		if ((9 == majorFileVersion) && (0 == minorFileVersion)) return true;
		if ((10 == majorFileVersion) && (0 == minorFileVersion)) return true;
		if ((11 == majorFileVersion) && (0 == minorFileVersion)) return true;
		if ((12 == majorFileVersion) && (0 == minorFileVersion)) return true;
		if ((13 == majorFileVersion) && (0 == minorFileVersion)) return true;
	}
	else if (_T("4.5.19.0") == swVersion) {
		if ((2 == majorFileVersion) && (0 == minorFileVersion)) return true;
		if ((3 == majorFileVersion) && (0 == minorFileVersion)) return true;
		if ((4 == majorFileVersion) && (0 == minorFileVersion)) return true;
		if ((5 == majorFileVersion) && (0 == minorFileVersion)) return true;
		if ((6 == majorFileVersion) && (0 == minorFileVersion)) return true;
		if ((7 == majorFileVersion) && (0 == minorFileVersion)) return true;
		if ((8 == majorFileVersion) && (0 == minorFileVersion)) return true;
		if ((9 == majorFileVersion) && (0 == minorFileVersion)) return true;
		if ((10 == majorFileVersion) && (0 == minorFileVersion)) return true;
		if ((11 == majorFileVersion) && (0 == minorFileVersion)) return true;
		if ((12 == majorFileVersion) && (0 == minorFileVersion)) return true;
		if ((13 == majorFileVersion) && (0 == minorFileVersion)) return true;
	}
	else if (_T("4.5.18.0") == swVersion) {
		if ((2 == majorFileVersion) && (0 == minorFileVersion)) return true;
		if ((3 == majorFileVersion) && (0 == minorFileVersion)) return true;
		if ((4 == majorFileVersion) && (0 == minorFileVersion)) return true;
		if ((5 == majorFileVersion) && (0 == minorFileVersion)) return true;
		if ((6 == majorFileVersion) && (0 == minorFileVersion)) return true;
		if ((7 == majorFileVersion) && (0 == minorFileVersion)) return true;
		if ((8 == majorFileVersion) && (0 == minorFileVersion)) return true;
		if ((9 == majorFileVersion) && (0 == minorFileVersion)) return true;
		if ((10 == majorFileVersion) && (0 == minorFileVersion)) return true;
		if ((11 == majorFileVersion) && (0 == minorFileVersion)) return true;
		if ((12 == majorFileVersion) && (0 == minorFileVersion)) return true;
		if ((13 == majorFileVersion) && (0 == minorFileVersion)) return true;
	}
	else if (_T("4.5.17.0") == swVersion) {
		if ((2 == majorFileVersion) && (0 == minorFileVersion)) return true;
		if ((3 == majorFileVersion) && (0 == minorFileVersion)) return true;
		if ((4 == majorFileVersion) && (0 == minorFileVersion)) return true;
		if ((5 == majorFileVersion) && (0 == minorFileVersion)) return true;
		if ((6 == majorFileVersion) && (0 == minorFileVersion)) return true;
		if ((7 == majorFileVersion) && (0 == minorFileVersion)) return true;
		if ((8 == majorFileVersion) && (0 == minorFileVersion)) return true;
		if ((9 == majorFileVersion) && (0 == minorFileVersion)) return true;
		if ((10 == majorFileVersion) && (0 == minorFileVersion)) return true;
		if ((11 == majorFileVersion) && (0 == minorFileVersion)) return true;
		if ((12 == majorFileVersion) && (0 == minorFileVersion)) return true;
		if ((13 == majorFileVersion) && (0 == minorFileVersion)) return true;
	}
	else if (_T("4.5.16.0") == swVersion) {
		if ((2 == majorFileVersion) && (0 == minorFileVersion)) return true;
		if ((3 == majorFileVersion) && (0 == minorFileVersion)) return true;
		if ((4 == majorFileVersion) && (0 == minorFileVersion)) return true;
		if ((5 == majorFileVersion) && (0 == minorFileVersion)) return true;
		if ((6 == majorFileVersion) && (0 == minorFileVersion)) return true;
		if ((7 == majorFileVersion) && (0 == minorFileVersion)) return true;
		if ((8 == majorFileVersion) && (0 == minorFileVersion)) return true;
		if ((9 == majorFileVersion) && (0 == minorFileVersion)) return true;
		if ((10 == majorFileVersion) && (0 == minorFileVersion)) return true;
		if ((11 == majorFileVersion) && (0 == minorFileVersion)) return true;
		if ((12 == majorFileVersion) && (0 == minorFileVersion)) return true;
		if ((13 == majorFileVersion) && (0 == minorFileVersion)) return true;
	}
	else if (_T("4.5.15.0") == swVersion) {
		if ((2 == majorFileVersion) && (0 == minorFileVersion)) return true;
		if ((3 == majorFileVersion) && (0 == minorFileVersion)) return true;
		if ((4 == majorFileVersion) && (0 == minorFileVersion)) return true;
		if ((5 == majorFileVersion) && (0 == minorFileVersion)) return true;
		if ((6 == majorFileVersion) && (0 == minorFileVersion)) return true;
		if ((7 == majorFileVersion) && (0 == minorFileVersion)) return true;
		if ((8 == majorFileVersion) && (0 == minorFileVersion)) return true;
		if ((9 == majorFileVersion) && (0 == minorFileVersion)) return true;
		if ((10 == majorFileVersion) && (0 == minorFileVersion)) return true;
		if ((11 == majorFileVersion) && (0 == minorFileVersion)) return true;
		if ((12 == majorFileVersion) && (0 == minorFileVersion)) return true;
		if ((13 == majorFileVersion) && (0 == minorFileVersion)) return true;
	}
	else if (_T("4.5.14.0") == swVersion) {
		if ((2 == majorFileVersion) && (0 == minorFileVersion)) return true;
		if ((3 == majorFileVersion) && (0 == minorFileVersion)) return true;
		if ((4 == majorFileVersion) && (0 == minorFileVersion)) return true;
		if ((5 == majorFileVersion) && (0 == minorFileVersion)) return true;
		if ((6 == majorFileVersion) && (0 == minorFileVersion)) return true;
		if ((7 == majorFileVersion) && (0 == minorFileVersion)) return true;
		if ((8 == majorFileVersion) && (0 == minorFileVersion)) return true;
		if ((9 == majorFileVersion) && (0 == minorFileVersion)) return true;
		if ((10 == majorFileVersion) && (0 == minorFileVersion)) return true;
		if ((11 == majorFileVersion) && (0 == minorFileVersion)) return true;
		if ((12 == majorFileVersion) && (0 == minorFileVersion)) return true;
		if ((13 == majorFileVersion) && (0 == minorFileVersion)) return true;
	}
	else if (_T("4.5.13.0") == swVersion) {
		if ((2 == majorFileVersion) && (0 == minorFileVersion)) return true;
		if ((3 == majorFileVersion) && (0 == minorFileVersion)) return true;
		if ((4 == majorFileVersion) && (0 == minorFileVersion)) return true;
		if ((5 == majorFileVersion) && (0 == minorFileVersion)) return true;
		if ((6 == majorFileVersion) && (0 == minorFileVersion)) return true;
		if ((7 == majorFileVersion) && (0 == minorFileVersion)) return true;
		if ((8 == majorFileVersion) && (0 == minorFileVersion)) return true;
		if ((9 == majorFileVersion) && (0 == minorFileVersion)) return true;
		if ((10 == majorFileVersion) && (0 == minorFileVersion)) return true;
		if ((11 == majorFileVersion) && (0 == minorFileVersion)) return true;
		if ((12 == majorFileVersion) && (0 == minorFileVersion)) return true;
		if ((13 == majorFileVersion) && (0 == minorFileVersion)) return true;
	}
	else if (_T("4.5.12.0") == swVersion) {
		if ((2 == majorFileVersion) && (0 == minorFileVersion)) return true;
		if ((3 == majorFileVersion) && (0 == minorFileVersion)) return true;
		if ((4 == majorFileVersion) && (0 == minorFileVersion)) return true;
		if ((5 == majorFileVersion) && (0 == minorFileVersion)) return true;
		if ((6 == majorFileVersion) && (0 == minorFileVersion)) return true;
		if ((7 == majorFileVersion) && (0 == minorFileVersion)) return true;
		if ((8 == majorFileVersion) && (0 == minorFileVersion)) return true;
		if ((9 == majorFileVersion) && (0 == minorFileVersion)) return true;
		if ((10 == majorFileVersion) && (0 == minorFileVersion)) return true;
		if ((11 == majorFileVersion) && (0 == minorFileVersion)) return true;
		if ((12 == majorFileVersion) && (0 == minorFileVersion)) return true;
		if ((13 == majorFileVersion) && (0 == minorFileVersion)) return true;
	}
	else if (_T("4.5.11.0") == swVersion) {
		if ((2 == majorFileVersion) && (0 == minorFileVersion)) return true;
		if ((3 == majorFileVersion) && (0 == minorFileVersion)) return true;
		if ((4 == majorFileVersion) && (0 == minorFileVersion)) return true;
		if ((5 == majorFileVersion) && (0 == minorFileVersion)) return true;
		if ((6 == majorFileVersion) && (0 == minorFileVersion)) return true;
		if ((7 == majorFileVersion) && (0 == minorFileVersion)) return true;
		if ((8 == majorFileVersion) && (0 == minorFileVersion)) return true;
		if ((9 == majorFileVersion) && (0 == minorFileVersion)) return true;
		if ((10 == majorFileVersion) && (0 == minorFileVersion)) return true;
		if ((11 == majorFileVersion) && (0 == minorFileVersion)) return true;
		if ((12 == majorFileVersion) && (0 == minorFileVersion)) return true;
		if ((13 == majorFileVersion) && (0 == minorFileVersion)) return true;
	}
	else if (_T("4.5.10.0") == swVersion) {
		if ((2 == majorFileVersion) && (0 == minorFileVersion)) return true;
		if ((3 == majorFileVersion) && (0 == minorFileVersion)) return true;
		if ((4 == majorFileVersion) && (0 == minorFileVersion)) return true;
		if ((5 == majorFileVersion) && (0 == minorFileVersion)) return true;
		if ((6 == majorFileVersion) && (0 == minorFileVersion)) return true;
		if ((7 == majorFileVersion) && (0 == minorFileVersion)) return true;
		if ((8 == majorFileVersion) && (0 == minorFileVersion)) return true;
		if ((9 == majorFileVersion) && (0 == minorFileVersion)) return true;
		if ((10 == majorFileVersion) && (0 == minorFileVersion)) return true;
		if ((11 == majorFileVersion) && (0 == minorFileVersion)) return true;
		if ((12 == majorFileVersion) && (0 == minorFileVersion)) return true;
		if ((13 == majorFileVersion) && (0 == minorFileVersion)) return true;
	}
	else if (_T("4.5.9.0") == swVersion) {
		if ((2 == majorFileVersion) && (0 == minorFileVersion)) return true;
		if ((3 == majorFileVersion) && (0 == minorFileVersion)) return true;
		if ((4 == majorFileVersion) && (0 == minorFileVersion)) return true;
		if ((5 == majorFileVersion) && (0 == minorFileVersion)) return true;
		if ((6 == majorFileVersion) && (0 == minorFileVersion)) return true;
		if ((7 == majorFileVersion) && (0 == minorFileVersion)) return true;
		if ((8 == majorFileVersion) && (0 == minorFileVersion)) return true;
		if ((9 == majorFileVersion) && (0 == minorFileVersion)) return true;
		if ((10 == majorFileVersion) && (0 == minorFileVersion)) return true;
		if ((11 == majorFileVersion) && (0 == minorFileVersion)) return true;
		if ((12 == majorFileVersion) && (0 == minorFileVersion)) return true;
		if ((13 == majorFileVersion) && (0 == minorFileVersion)) return true;
	}
	else if (_T("4.5.8.0") == swVersion) {
		if ((2 == majorFileVersion) && (0 == minorFileVersion)) return true;
		if ((3 == majorFileVersion) && (0 == minorFileVersion)) return true;
		if ((4 == majorFileVersion) && (0 == minorFileVersion)) return true;
		if ((5 == majorFileVersion) && (0 == minorFileVersion)) return true;
		if ((6 == majorFileVersion) && (0 == minorFileVersion)) return true;
		if ((7 == majorFileVersion) && (0 == minorFileVersion)) return true;
		if ((8 == majorFileVersion) && (0 == minorFileVersion)) return true;
		if ((9 == majorFileVersion) && (0 == minorFileVersion)) return true;
		if ((10 == majorFileVersion) && (0 == minorFileVersion)) return true;
		if ((11 == majorFileVersion) && (0 == minorFileVersion)) return true;
		if ((12 == majorFileVersion) && (0 == minorFileVersion)) return true;
		if ((13 == majorFileVersion) && (0 == minorFileVersion)) return true;
	}
	else if (_T("4.5.7.0") == swVersion) {
		if ((2 == majorFileVersion) && (0 == minorFileVersion)) return true;
		if ((3 == majorFileVersion) && (0 == minorFileVersion)) return true;
		if ((4 == majorFileVersion) && (0 == minorFileVersion)) return true;
		if ((5 == majorFileVersion) && (0 == minorFileVersion)) return true;
		if ((6 == majorFileVersion) && (0 == minorFileVersion)) return true;
		if ((7 == majorFileVersion) && (0 == minorFileVersion)) return true;
		if ((8 == majorFileVersion) && (0 == minorFileVersion)) return true;
		if ((9 == majorFileVersion) && (0 == minorFileVersion)) return true;
		if ((10 == majorFileVersion) && (0 == minorFileVersion)) return true;
		if ((11 == majorFileVersion) && (0 == minorFileVersion)) return true;
		if ((12 == majorFileVersion) && (0 == minorFileVersion)) return true;
		if ((13 == majorFileVersion) && (0 == minorFileVersion)) return true;
	}
	else if (_T("4.5.6.0") == swVersion) {
		if ((2 == majorFileVersion) && (0 == minorFileVersion)) return true;
		if ((3 == majorFileVersion) && (0 == minorFileVersion)) return true;
		if ((4 == majorFileVersion) && (0 == minorFileVersion)) return true;
		if ((5 == majorFileVersion) && (0 == minorFileVersion)) return true;
		if ((6 == majorFileVersion) && (0 == minorFileVersion)) return true;
		if ((7 == majorFileVersion) && (0 == minorFileVersion)) return true;
		if ((8 == majorFileVersion) && (0 == minorFileVersion)) return true;
		if ((9 == majorFileVersion) && (0 == minorFileVersion)) return true;
		if ((10 == majorFileVersion) && (0 == minorFileVersion)) return true;
		if ((11 == majorFileVersion) && (0 == minorFileVersion)) return true;
		if ((12 == majorFileVersion) && (0 == minorFileVersion)) return true;
		if ((13 == majorFileVersion) && (0 == minorFileVersion)) return true;
	}
	else if (_T("1.6.0.0") == swVersion) {
		if ((1 == majorFileVersion) && (0 == minorFileVersion)) return true;
	}
	else if (_T("1.7.0.0") == swVersion) {
		if ((1 == majorFileVersion) && (0 == minorFileVersion)) return true;
	}
	else if (_T("1.8.0.0") == swVersion) {
		if ((1 == majorFileVersion) && (0 == minorFileVersion)) return true;
	}
	else if (_T("1.9.0.0") == swVersion) {
		if ((2 == majorFileVersion) && (0 == minorFileVersion)) return true;
	}
	else if (_T("1.10.0.0") == swVersion) {
		if ((2 == majorFileVersion) && (0 == minorFileVersion)) return true;
		if ((3 == majorFileVersion) && (0 == minorFileVersion)) return true;
	}
	else if (_T("1.11.0.0") == swVersion) {
		if ((2 == majorFileVersion) && (0 == minorFileVersion)) return true;
		if ((3 == majorFileVersion) && (0 == minorFileVersion)) return true;
	}
	else if (_T("2.0.0.0") == swVersion) {
		if ((2 == majorFileVersion) && (0 == minorFileVersion)) return true;
		if ((3 == majorFileVersion) && (0 == minorFileVersion)) return true;
	}
	else if (_T("2.1.0.0") == swVersion) {
		if ((2 == majorFileVersion) && (0 == minorFileVersion)) return true;
		if ((3 == majorFileVersion) && (0 == minorFileVersion)) return true;
	}
	else if (_T("2.2.0.0") == swVersion) {
		if ((2 == majorFileVersion) && (0 == minorFileVersion)) return true;
		if ((3 == majorFileVersion) && (0 == minorFileVersion)) return true;
	}
	else if (_T("2.3.0.0") == swVersion) {
		if ((2 == majorFileVersion) && (0 == minorFileVersion)) return true;
		if ((3 == majorFileVersion) && (0 == minorFileVersion)) return true;
	}
	else if (_T("2.4.0.0") == swVersion) {
		if ((2 == majorFileVersion) && (0 == minorFileVersion)) return true;
		if ((3 == majorFileVersion) && (0 == minorFileVersion)) return true;
	}
	else if (_T("2.5.0.0") == swVersion) {
		if ((2 == majorFileVersion) && (0 == minorFileVersion)) return true;
		if ((3 == majorFileVersion) && (0 == minorFileVersion)) return true;
	}
	else if (_T("2.6.0.0") == swVersion) {
		if ((2 == majorFileVersion) && (0 == minorFileVersion)) return true;
		if ((3 == majorFileVersion) && (0 == minorFileVersion)) return true;
	}
	else if (_T("2.7.0.0") == swVersion) {
		if ((2 == majorFileVersion) && (0 == minorFileVersion)) return true;
		if ((3 == majorFileVersion) && (0 == minorFileVersion)) return true;
	}	
	else if (_T("2.8.0.0") == swVersion) {
		if ((2 == majorFileVersion) && (0 == minorFileVersion)) return true;
		if ((3 == majorFileVersion) && (0 == minorFileVersion)) return true;
	}
	else if (_T("2.9.0.0") == swVersion) {
		if ((2 == majorFileVersion) && (0 == minorFileVersion)) return true;
		if ((3 == majorFileVersion) && (0 == minorFileVersion)) return true;
	}
	else if (_T("2.10.0.0") == swVersion) {
		if ((2 == majorFileVersion) && (0 == minorFileVersion)) return true;
		if ((3 == majorFileVersion) && (0 == minorFileVersion)) return true;
	}
	else if (_T("2.11.0.0") == swVersion) {
		if ((2 == majorFileVersion) && (0 == minorFileVersion)) return true;
		if ((3 == majorFileVersion) && (0 == minorFileVersion)) return true;
	}
	else if (_T("2.12.0.0") == swVersion) {
		if ((2 == majorFileVersion) && (0 == minorFileVersion)) return true;
		if ((3 == majorFileVersion) && (0 == minorFileVersion)) return true;
	}
	else if (_T("2.13.0.0") == swVersion) {
		if ((2 == majorFileVersion) && (0 == minorFileVersion)) return true;
		if ((3 == majorFileVersion) && (0 == minorFileVersion)) return true;
	}
	else if (_T("3.0.0.0") == swVersion) {
		if ((2 == majorFileVersion) && (0 == minorFileVersion)) return true;
		if ((3 == majorFileVersion) && (0 == minorFileVersion)) return true;
	}
	else if (_T("3.1.0.0") == swVersion) {
		if ((2 == majorFileVersion) && (0 == minorFileVersion)) return true;
		if ((3 == majorFileVersion) && (0 == minorFileVersion)) return true;
	}
	else if (_T("3.2.0.0") == swVersion) {
		if ((2 == majorFileVersion) && (0 == minorFileVersion)) return true;
		if ((3 == majorFileVersion) && (0 == minorFileVersion)) return true;
		if ((4 == majorFileVersion) && (0 == minorFileVersion)) return true;
	}
	else if (_T("3.3.0.0") == swVersion) {
		if ((2 == majorFileVersion) && (0 == minorFileVersion)) return true;
		if ((3 == majorFileVersion) && (0 == minorFileVersion)) return true;
		if ((4 == majorFileVersion) && (0 == minorFileVersion)) return true;
	}
	else if (_T("3.4.0.0") == swVersion) {
		if ((2 == majorFileVersion) && (0 == minorFileVersion)) return true;
		if ((3 == majorFileVersion) && (0 == minorFileVersion)) return true;
		if ((4 == majorFileVersion) && (0 == minorFileVersion)) return true;
	}
	else if (_T("3.5.0.0") == swVersion) {
		if ((2 == majorFileVersion) && (0 == minorFileVersion)) return true;
		if ((3 == majorFileVersion) && (0 == minorFileVersion)) return true;
		if ((4 == majorFileVersion) && (0 == minorFileVersion)) return true;
	}
	else if (_T("3.6.0.0") == swVersion) {
		if ((2 == majorFileVersion) && (0 == minorFileVersion)) return true;
		if ((3 == majorFileVersion) && (0 == minorFileVersion)) return true;
		if ((4 == majorFileVersion) && (0 == minorFileVersion)) return true;
		if ((5 == majorFileVersion) && (0 == minorFileVersion)) return true;
		if ((6 == majorFileVersion) && (0 == minorFileVersion)) return true;
	}
	else if (_T("3.7.0.0") == swVersion) {
		if ((2 == majorFileVersion) && (0 == minorFileVersion)) return true;
		if ((3 == majorFileVersion) && (0 == minorFileVersion)) return true;
		if ((4 == majorFileVersion) && (0 == minorFileVersion)) return true;
		if ((5 == majorFileVersion) && (0 == minorFileVersion)) return true;
		if ((6 == majorFileVersion) && (0 == minorFileVersion)) return true;
	}
	else if (_T("3.8.0.0") == swVersion) {
		if ((2 == majorFileVersion) && (0 == minorFileVersion)) return true;
		if ((3 == majorFileVersion) && (0 == minorFileVersion)) return true;
		if ((4 == majorFileVersion) && (0 == minorFileVersion)) return true;
		if ((5 == majorFileVersion) && (0 == minorFileVersion)) return true;
		if ((6 == majorFileVersion) && (0 == minorFileVersion)) return true;
	}
	else if (_T("3.9.0.0") == swVersion) {
		if ((2 == majorFileVersion) && (0 == minorFileVersion)) return true;
		if ((3 == majorFileVersion) && (0 == minorFileVersion)) return true;
		if ((4 == majorFileVersion) && (0 == minorFileVersion)) return true;
		if ((5 == majorFileVersion) && (0 == minorFileVersion)) return true;
		if ((6 == majorFileVersion) && (0 == minorFileVersion)) return true;
	}
	else if (_T("3.10.0.0") == swVersion) {
		if ((2 == majorFileVersion) && (0 == minorFileVersion)) return true;
		if ((3 == majorFileVersion) && (0 == minorFileVersion)) return true;
		if ((4 == majorFileVersion) && (0 == minorFileVersion)) return true;
		if ((5 == majorFileVersion) && (0 == minorFileVersion)) return true;
		if ((6 == majorFileVersion) && (0 == minorFileVersion)) return true;
	}
	else if (_T("3.11.0.0") == swVersion) {
		if ((2 == majorFileVersion) && (0 == minorFileVersion)) return true;
		if ((3 == majorFileVersion) && (0 == minorFileVersion)) return true;
		if ((4 == majorFileVersion) && (0 == minorFileVersion)) return true;
		if ((5 == majorFileVersion) && (0 == minorFileVersion)) return true;
		if ((6 == majorFileVersion) && (0 == minorFileVersion)) return true;
	}
	else if (_T("3.12.0.0") == swVersion) {
		if ((2 == majorFileVersion) && (0 == minorFileVersion)) return true;
		if ((3 == majorFileVersion) && (0 == minorFileVersion)) return true;
		if ((4 == majorFileVersion) && (0 == minorFileVersion)) return true;
		if ((5 == majorFileVersion) && (0 == minorFileVersion)) return true;
		if ((6 == majorFileVersion) && (0 == minorFileVersion)) return true;
	}
	else if (_T("3.13.0.0") == swVersion) {
		if ((2 == majorFileVersion) && (0 == minorFileVersion)) return true;
		if ((3 == majorFileVersion) && (0 == minorFileVersion)) return true;
		if ((4 == majorFileVersion) && (0 == minorFileVersion)) return true;
		if ((5 == majorFileVersion) && (0 == minorFileVersion)) return true;
		if ((6 == majorFileVersion) && (0 == minorFileVersion)) return true;
	}
	else if (_T("4.0.0.0") == swVersion) {
		if ((2 == majorFileVersion) && (0 == minorFileVersion)) return true;
		if ((3 == majorFileVersion) && (0 == minorFileVersion)) return true;
		if ((4 == majorFileVersion) && (0 == minorFileVersion)) return true;
		if ((5 == majorFileVersion) && (0 == minorFileVersion)) return true;
		if ((6 == majorFileVersion) && (0 == minorFileVersion)) return true;
	}
	else if (_T("4.1.0.0") == swVersion) {
		if ((2 == majorFileVersion) && (0 == minorFileVersion)) return true;
		if ((3 == majorFileVersion) && (0 == minorFileVersion)) return true;
		if ((4 == majorFileVersion) && (0 == minorFileVersion)) return true;
		if ((5 == majorFileVersion) && (0 == minorFileVersion)) return true;
		if ((6 == majorFileVersion) && (0 == minorFileVersion)) return true;
		if ((7 == majorFileVersion) && (0 == minorFileVersion)) return true;
	}
	else if (_T("4.2.0.0") == swVersion) {
		if ((2 == majorFileVersion) && (0 == minorFileVersion)) return true;
		if ((3 == majorFileVersion) && (0 == minorFileVersion)) return true;
		if ((4 == majorFileVersion) && (0 == minorFileVersion)) return true;
		if ((5 == majorFileVersion) && (0 == minorFileVersion)) return true;
		if ((6 == majorFileVersion) && (0 == minorFileVersion)) return true;
		if ((7 == majorFileVersion) && (0 == minorFileVersion)) return true;
		if ((8 == majorFileVersion) && (0 == minorFileVersion)) return true;
		if ((9 == majorFileVersion) && (0 == minorFileVersion)) return true;
	}
	else if (_T("4.3.0.0") == swVersion) {
		if ((2 == majorFileVersion) && (0 == minorFileVersion)) return true;
		if ((3 == majorFileVersion) && (0 == minorFileVersion)) return true;
		if ((4 == majorFileVersion) && (0 == minorFileVersion)) return true;
		if ((5 == majorFileVersion) && (0 == minorFileVersion)) return true;
		if ((6 == majorFileVersion) && (0 == minorFileVersion)) return true;
		if ((7 == majorFileVersion) && (0 == minorFileVersion)) return true;
		if ((8 == majorFileVersion) && (0 == minorFileVersion)) return true;
		if ((9 == majorFileVersion) && (0 == minorFileVersion)) return true;
	}
	else if (_T("4.3.0.0") == swVersion) {
		if ((2 == majorFileVersion) && (0 == minorFileVersion)) return true;
		if ((3 == majorFileVersion) && (0 == minorFileVersion)) return true;
		if ((4 == majorFileVersion) && (0 == minorFileVersion)) return true;
		if ((5 == majorFileVersion) && (0 == minorFileVersion)) return true;
		if ((6 == majorFileVersion) && (0 == minorFileVersion)) return true;
		if ((7 == majorFileVersion) && (0 == minorFileVersion)) return true;
		if ((8 == majorFileVersion) && (0 == minorFileVersion)) return true;
		if ((9 == majorFileVersion) && (0 == minorFileVersion)) return true;
	}
	else if (_T("4.5.0.0") == swVersion) {
		if ((2 == majorFileVersion) && (0 == minorFileVersion)) return true;
		if ((3 == majorFileVersion) && (0 == minorFileVersion)) return true;
		if ((4 == majorFileVersion) && (0 == minorFileVersion)) return true;
		if ((5 == majorFileVersion) && (0 == minorFileVersion)) return true;
		if ((6 == majorFileVersion) && (0 == minorFileVersion)) return true;
		if ((7 == majorFileVersion) && (0 == minorFileVersion)) return true;
		if ((8 == majorFileVersion) && (0 == minorFileVersion)) return true;
		if ((9 == majorFileVersion) && (0 == minorFileVersion)) return true;
	}
	else if (_T("4.5.1.0") == swVersion) {
		if ((2 == majorFileVersion) && (0 == minorFileVersion)) return true;
		if ((3 == majorFileVersion) && (0 == minorFileVersion)) return true;
		if ((4 == majorFileVersion) && (0 == minorFileVersion)) return true;
		if ((5 == majorFileVersion) && (0 == minorFileVersion)) return true;
		if ((6 == majorFileVersion) && (0 == minorFileVersion)) return true;
		if ((7 == majorFileVersion) && (0 == minorFileVersion)) return true;
		if ((8 == majorFileVersion) && (0 == minorFileVersion)) return true;
		if ((9 == majorFileVersion) && (0 == minorFileVersion)) return true;
	}
	else if (_T("4.5.2.0") == swVersion) {
		if ((2 == majorFileVersion) && (0 == minorFileVersion)) return true;
		if ((3 == majorFileVersion) && (0 == minorFileVersion)) return true;
		if ((4 == majorFileVersion) && (0 == minorFileVersion)) return true;
		if ((5 == majorFileVersion) && (0 == minorFileVersion)) return true;
		if ((6 == majorFileVersion) && (0 == minorFileVersion)) return true;
		if ((7 == majorFileVersion) && (0 == minorFileVersion)) return true;
		if ((8 == majorFileVersion) && (0 == minorFileVersion)) return true;
		if ((9 == majorFileVersion) && (0 == minorFileVersion)) return true;
	}
	else if (_T("4.5.3.0") == swVersion) {
		if ((2 == majorFileVersion) && (0 == minorFileVersion)) return true;
		if ((3 == majorFileVersion) && (0 == minorFileVersion)) return true;
		if ((4 == majorFileVersion) && (0 == minorFileVersion)) return true;
		if ((5 == majorFileVersion) && (0 == minorFileVersion)) return true;
		if ((6 == majorFileVersion) && (0 == minorFileVersion)) return true;
		if ((7 == majorFileVersion) && (0 == minorFileVersion)) return true;
		if ((8 == majorFileVersion) && (0 == minorFileVersion)) return true;
		if ((9 == majorFileVersion) && (0 == minorFileVersion)) return true;
		if ((10 == majorFileVersion) && (0 == minorFileVersion)) return true;
		if ((11 == majorFileVersion) && (0 == minorFileVersion)) return true;
	}
	else if (_T("4.5.4.0") == swVersion) {
		if ((2 == majorFileVersion) && (0 == minorFileVersion)) return true;
		if ((3 == majorFileVersion) && (0 == minorFileVersion)) return true;
		if ((4 == majorFileVersion) && (0 == minorFileVersion)) return true;
		if ((5 == majorFileVersion) && (0 == minorFileVersion)) return true;
		if ((6 == majorFileVersion) && (0 == minorFileVersion)) return true;
		if ((7 == majorFileVersion) && (0 == minorFileVersion)) return true;
		if ((8 == majorFileVersion) && (0 == minorFileVersion)) return true;
		if ((9 == majorFileVersion) && (0 == minorFileVersion)) return true;
		if ((10 == majorFileVersion) && (0 == minorFileVersion)) return true;
		if ((11 == majorFileVersion) && (0 == minorFileVersion)) return true;
		if ((12 == majorFileVersion) && (0 == minorFileVersion)) return true;
	}
	else if (_T("4.5.5.0") == swVersion) {
		if ((2 == majorFileVersion) && (0 == minorFileVersion)) return true;
		if ((3 == majorFileVersion) && (0 == minorFileVersion)) return true;
		if ((4 == majorFileVersion) && (0 == minorFileVersion)) return true;
		if ((5 == majorFileVersion) && (0 == minorFileVersion)) return true;
		if ((6 == majorFileVersion) && (0 == minorFileVersion)) return true;
		if ((7 == majorFileVersion) && (0 == minorFileVersion)) return true;
		if ((8 == majorFileVersion) && (0 == minorFileVersion)) return true;
		if ((9 == majorFileVersion) && (0 == minorFileVersion)) return true;
		if ((10 == majorFileVersion) && (0 == minorFileVersion)) return true;
		if ((11 == majorFileVersion) && (0 == minorFileVersion)) return true;
		if ((12 == majorFileVersion) && (0 == minorFileVersion)) return true;
		if ((13 == majorFileVersion) && (0 == minorFileVersion)) return true;
	}
	if (_issueWarning) {
		CString s, s1;
		int nums = s.LoadString(IDS_VERSIONS_CONFLICT);
		s1.Format(s, swVersion, majorFileVersion, minorFileVersion);
		AfxMessageBox(s1);
	}
	return false;
}

void CReadWriteAGSData::setAnthropologiaData(	CPatient *_pt,
								CAttendingDoctor *_dct,
								COperator *_op,
								CReferringDoctor *_rd)
{
	pt = _pt;
	dct = _dct;
	op = _op;
	rd = _rd;
}

void CReadWriteAGSData::setAG200Parameters(	APN_SPARE_BLOCK_6xx *_spareApnBlock,
								APN_FILE_HEADER_6xx *_apnFileHeader,
								CF_SETUP_BLOCK_6xx *_m_setupData,
								LOGGER_DATA_BLOCK_6xx *_m_LoggerData,
								CATHETER_DATA_BLOCK_6xx *_m_CathData)
{
	spareApnBlock = _spareApnBlock;
	apnFileHeader = _apnFileHeader;
	m_setupData	  = _m_setupData;
	m_LoggerData  = _m_LoggerData;
	m_CathData	  = _m_CathData;
}


void CReadWriteAGSData::setParameters(	NECK_PARAMETER_SET *_neckParameters,
										WRIST_PARAMETER_SET *_wristParameters,
										CATH_PARAMETER_SET *_cathParameters,
										TORSO_PARAMETER_SET *_torsoParameters)
{
	neckParameters = _neckParameters;
	wristParameters = _wristParameters;
	cathParameters = _cathParameters;
	torsoParameters = _torsoParameters;
}

void CReadWriteAGSData::setData(	CBatteryDataSet *_battData,
									CContactMicDataSet *_cmData,
									CAirMicDataSet *_amData,
									CBodyPosXYZDataSet *_bpXYZData,
									CActimeterDataSet *_acData,
									CActimeterDataSet *_torsoAcData,
									COximeterDataSet *_oxData,
									CBodyPosDataSet *_bpData,
									CCatheterDataSet *_cathData,
									CAG200MicDataSet *_ag200MicData,
									CRespBeltDataSet *_respData, 
									CManualMarkerSet *_markerData)
{
	battData = _battData;
	cmData = _cmData;
	amData = _amData;
	bpXYZData = _bpXYZData;
	acData = _acData;
	torsoAcData = _torsoAcData;
	oxData = _oxData;
	bpData = _bpData;
	cathData = _cathData;
	ag200MicData = _ag200MicData;
	respData = _respData;
	mMarkerData = _markerData;
}

void CReadWriteAGSData::Serialize(CArchive& ar,SERIAL_NUMBER *_ser)
{
	if( ar.IsStoring() ) {
		ar << _ser->serial.unit;
		ar << _ser->serial.model;
		ar << _ser->serial.year;
		ar << _ser->serial.month;
		ar << _ser->serial.unitInBatch;
		ar << _ser->serial.serial;
	}
	else {
		ar >> _ser->serial.unit;
		ar >> _ser->serial.model;
		ar >> _ser->serial.year;
		ar >> _ser->serial.month;
		ar >> _ser->serial.unitInBatch;
		ar >> _ser->serial.serial;
	}
}

void CReadWriteAGSData::setIsAG200Data(bool _set)
{
	AG200TypeData = _set;
}

bool CReadWriteAGSData::getIsAG200Data(void)
{
	return AG200TypeData;
}

int CReadWriteAGSData::getAGSMajorVersion(void)
{
	return majorFileVersion;
}

void CReadWriteAGSData::setDoctorsNotes(CString _s)
{
	doctorsNotes = _s;
}

CString CReadWriteAGSData::getDoctorsNotes(void)
{
	return doctorsNotes;
}


int CReadWriteAGSData::getAGSMinorVersion(void)
{
	return minorFileVersion;
}

SERIAL_NUMBER CReadWriteAGSData::getTorsoSerialNumber(void)
{
	return torsoParameters->serialNumber;
}

bool CReadWriteAGSData::Serialize(CArchive& ar,bool _anthroOnly,bool _ag200TypeData)
{
	ASSERT(pt );
	ASSERT(dct);
	ASSERT(op );
	ASSERT(rd );
	
	if( ar.IsStoring() ) {
		AG200TypeData = _ag200TypeData;
		minorFileVersion = AGS_FILE_MINOR_VERSION;
		majorFileVersion = AGS_FILE_MAJOR_VERSION;
				   
		ar << majorFileVersion;
		ar << minorFileVersion;
		ar << AG200TypeData;
	}
	else {  // Is reading
		ar >> majorFileVersion;
		ar >> minorFileVersion;		
		ar >> AG200TypeData;
		AGSMajorVersionRead = majorFileVersion;
		if (!checkVersionCompatibility(_anthroOnly ? false : true)) return false;
	}
	pt->Serialize(ar);
	dct->Serialize(ar);
	op->Serialize(ar);
	rd->Serialize(ar);

	if (_anthroOnly) return true;

	if( ar.IsStoring() ) {
		
		if (AG200TypeData) {
			ASSERT(spareApnBlock);
			ASSERT(apnFileHeader);
			ASSERT(m_setupData	);
			ASSERT(m_LoggerData );
			ASSERT(m_CathData	);
			ar.Write((void *) spareApnBlock,sizeof(APN_SPARE_BLOCK_6xx));
			ar.Write((void *) apnFileHeader,sizeof(APN_FILE_HEADER_6xx));
			ar.Write((void *) m_setupData,sizeof(CF_SETUP_BLOCK_6xx));
			ar.Write((void *) m_LoggerData,sizeof(LOGGER_DATA_BLOCK_6xx));
			ar.Write((void *) m_CathData,sizeof(CATHETER_DATA_BLOCK_6xx));
		}
		else {
			ASSERT(neckParameters  );
			ASSERT(wristParameters );
			ASSERT(cathParameters  );
			ASSERT(torsoParameters );

			//---Neck prameters
			ar << neckParameters->numStarts;
			Serialize(ar,&neckParameters->serialNumber);
			ar << neckParameters->version.value;
			ar << neckParameters->model.value;
			ar << neckParameters->antiAliasingFilterFrequency;
			ar.Write((void *) neckParameters->caldBMic.data.value,4);
			ar << neckParameters->calContactMic.data;
			
			//---Wrist parameters
			ar << wristParameters->numStarts;
			Serialize(ar,&wristParameters->serialNumber);
			ar << wristParameters->version.value;
			ar << wristParameters->model.value;
			ar << wristParameters->countsPerMg;
			ar << wristParameters->actimeterSampleRate;
			ar.Write((void *) wristParameters->oximeterVersion,LENGTH_OXIMETER_REVISION);
			
			//---Catheter parameters
			ar << cathParameters->version;
			ar << cathParameters->model;
			ar << cathParameters->SampleRate;
			Serialize(ar,&cathParameters->serialNumber);
			ar << cathParameters->usedHoursCounter.hoursUsed;
			ar << cathParameters->limitQualifiedStarts;
			ar << cathParameters->minPress;
			ar << cathParameters->maxPress;
			ar << cathParameters->minTemp;
			ar << cathParameters->maxTemp;
			ar << cathParameters->Sensortyp;
			ar << cathParameters->usedMinutesSinceStart;
			ar << cathParameters->numStarts;
			ar << cathParameters->numQualifiedStarts;
			ar << cathParameters->startQualifiactionLimit;
			ar << cathParameters->FirmwareVersion;
			ar << cathParameters->FilterLength;
			
			//---Torso parameters
			ar << torsoParameters->numStarts;
			ar << torsoParameters->usedHoursCounter.hoursUsed;
			ar << torsoParameters->usedMinutesSinceStart;
			ar << torsoParameters->numQualifiedStarts;
			ar << torsoParameters->version.value;
			ar << torsoParameters->model.value;
			ar << torsoParameters->BodyPosSampleRate;
			ar << torsoParameters->BodyPosLPfilter;
			ar << torsoParameters->RespBeltGain;
			ar << torsoParameters->RespBeltLPFilter;
			ar << torsoParameters->status;
			ar << torsoParameters->programLengthHours;
			ar << torsoParameters->programRunningHours;
			Serialize(ar,&torsoParameters->serialNumber);
		}
	}
    else {	 // Is reading
		
		if (AG200TypeData) {
			ASSERT(spareApnBlock);
			ASSERT(apnFileHeader);
			ASSERT(m_setupData	);
			ASSERT(m_LoggerData );
			ASSERT(m_CathData	);
			ar.Read((void *) spareApnBlock,sizeof(APN_SPARE_BLOCK_6xx));
			ar.Read((void *) apnFileHeader,sizeof(APN_FILE_HEADER_6xx));
			ar.Read((void *) m_setupData,sizeof(CF_SETUP_BLOCK_6xx));
			ar.Read((void *) m_LoggerData,sizeof(LOGGER_DATA_BLOCK_6xx));
			ar.Read((void *) m_CathData,sizeof(CATHETER_DATA_BLOCK_6xx));
		}
		else {
			
			ASSERT(neckParameters  );
			ASSERT(wristParameters );
			ASSERT(cathParameters  );
			ASSERT(torsoParameters );
			
			//---Neck parameters
			ar >> neckParameters->numStarts;
			Serialize(ar,&neckParameters->serialNumber);
			ar >> neckParameters->version.value;
			ar >> neckParameters->model.value;
			ar >> neckParameters->antiAliasingFilterFrequency;
			ar.Read((void *) neckParameters->caldBMic.data.value,4);
			ar >> neckParameters->calContactMic.data;
			
			//---Wrist parameters
			ar >> wristParameters->numStarts;
			Serialize(ar,&wristParameters->serialNumber);
			ar >> wristParameters->version.value;
			ar >> wristParameters->model.value;
			ar >> wristParameters->countsPerMg;
			ar >> wristParameters->actimeterSampleRate;
			ar.Read((void *) wristParameters->oximeterVersion,LENGTH_OXIMETER_REVISION);
			
			//---Catheter parameters
			ar >> cathParameters->version;
			ar >> cathParameters->model;
			ar >> cathParameters->SampleRate;
			Serialize(ar,&cathParameters->serialNumber);
			ar >> cathParameters->usedHoursCounter.hoursUsed;
			ar >> cathParameters->limitQualifiedStarts;
			ar >> cathParameters->minPress;
			ar >> cathParameters->maxPress;
			ar >> cathParameters->minTemp;
			ar >> cathParameters->maxTemp;
			ar >> cathParameters->Sensortyp;
			ar >> cathParameters->usedMinutesSinceStart;
			ar >> cathParameters->numStarts;
			ar >> cathParameters->numQualifiedStarts;
			ar >> cathParameters->startQualifiactionLimit;
			ar >> cathParameters->FirmwareVersion;
			ar >> cathParameters->FilterLength;
			
			//---Torso parameters
			ar >> torsoParameters->numStarts;
			ar >> torsoParameters->usedHoursCounter.hoursUsed;
			ar >> torsoParameters->usedMinutesSinceStart;
			ar >> torsoParameters->numQualifiedStarts;
			ar >> torsoParameters->version.value;
			ar >> torsoParameters->model.value;
			ar >> torsoParameters->BodyPosSampleRate;
			ar >> torsoParameters->BodyPosLPfilter;
			ar >> torsoParameters->RespBeltGain;
			ar >> torsoParameters->RespBeltLPFilter;
			ar >> torsoParameters->status;
			ar >> torsoParameters->programLengthHours;
			ar >> torsoParameters->programRunningHours;
			Serialize(ar,&torsoParameters->serialNumber);
		}
	}

	if( ar.IsStoring() ) {
		int hasData = HAS_NO_DATA;
		if (battData->getSize() > 0)		hasData |= HAS_BATTERY_DATA;	
		if (cmData->getSize() > 0)			hasData |= HAS_CONTACT_MIC_DATA;
		if (amData->getSize() > 0)			hasData |= HAS_AIR_MIC_DATA;
		if (bpXYZData->getSize() > 0)		hasData |= HAS_BODY_POS_XYZ_DATA;	
		if (acData->getSize() > 0)			hasData |= HAS_ACTIMETER_DATA;		
		if (oxData->getSize() > 0)			hasData |= HAS_OXIMETER_DATA;		
		if (bpData->getSize() > 0)			hasData |= HAS_BODY_POS_DATA;		
		if (cathData->getSize() > 0)		hasData |= HAS_CATHETER_DATA;	
		if (respData->getSize() > 0)		hasData |= HAS_RESP_DATA;
		if (ag200MicData->getSize() > 0)	hasData |= HAS_AG200_MIC_DATA;		// Null anyway

		ar << hasData;

		// Serialize data
		if (hasData & HAS_BATTERY_DATA)			battData->Serialize(ar,majorFileVersion,minorFileVersion);
		if (hasData & HAS_CONTACT_MIC_DATA)		cmData->Serialize(ar, majorFileVersion, minorFileVersion);
		if (hasData & HAS_AIR_MIC_DATA)			amData->Serialize(ar, majorFileVersion, minorFileVersion);
		if (hasData & HAS_BODY_POS_XYZ_DATA)	bpXYZData->Serialize(ar, majorFileVersion, minorFileVersion);
		if (hasData & HAS_ACTIMETER_DATA)		acData->Serialize(ar, majorFileVersion, minorFileVersion);
		if (hasData & HAS_OXIMETER_DATA)		oxData->Serialize(ar, majorFileVersion, minorFileVersion);
		if (hasData & HAS_BODY_POS_DATA) {
			bpData->Serialize(ar, majorFileVersion, minorFileVersion);
			torsoAcData->Serialize(ar, majorFileVersion, minorFileVersion);
		}
		if (hasData & HAS_CATHETER_DATA)		cathData->Serialize(ar, majorFileVersion, minorFileVersion);
		if (hasData & HAS_RESP_DATA)			respData->Serialize(ar, majorFileVersion, minorFileVersion);
		if (hasData & HAS_AG200_MIC_DATA)		ag200MicData->Serialize(ar, majorFileVersion, minorFileVersion);

		mMarkerData->Serialize(ar,majorFileVersion, minorFileVersion);
	}
	else {  // Is reading
		int hasData;
		ar >> hasData;
		if (hasData & HAS_BATTERY_DATA)			battData->Serialize(ar,majorFileVersion,minorFileVersion);
		if (hasData & HAS_CONTACT_MIC_DATA)		cmData->Serialize(ar, majorFileVersion, minorFileVersion);
		if (hasData & HAS_AIR_MIC_DATA)			amData->Serialize(ar, majorFileVersion, minorFileVersion);
		if (hasData & HAS_BODY_POS_XYZ_DATA)	bpXYZData->Serialize(ar, majorFileVersion, minorFileVersion);

		if (majorFileVersion < 10) 
			torsoAcData->generateFromBodyXYZ(bpXYZData);

		if (hasData & HAS_ACTIMETER_DATA)		acData->Serialize(ar, majorFileVersion, minorFileVersion);
		if (hasData & HAS_OXIMETER_DATA)		oxData->Serialize(ar, majorFileVersion, minorFileVersion);
		if (hasData & HAS_BODY_POS_DATA) {
			bpData->Serialize(ar, majorFileVersion, minorFileVersion);
			if (majorFileVersion >= 10) torsoAcData->Serialize(ar, majorFileVersion, minorFileVersion);
		}
		if (hasData & HAS_CATHETER_DATA)		cathData->Serialize(ar, majorFileVersion, minorFileVersion);
		if (hasData & HAS_RESP_DATA)			respData->Serialize(ar, majorFileVersion, minorFileVersion);
		if (hasData & HAS_AG200_MIC_DATA)		ag200MicData->Serialize(ar, majorFileVersion, minorFileVersion);

		if (majorFileVersion >= 13)
			mMarkerData->Serialize(ar, majorFileVersion, minorFileVersion);
	}

	if (majorFileVersion >= 11) {
		if (ar.IsStoring()) {
			ar << doctorsNotes;
		}
		else {
			ar >> doctorsNotes;
		}
	}

	return true;
}
