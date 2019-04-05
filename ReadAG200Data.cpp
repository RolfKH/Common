
// ReadAG200Data.cpp : Defines the class behaviors for the application.
//

/*!
* @author		RKH
* @date		06.06.2017
* @copyright (C)Spiro Medical AS 2013 - 2017
*/

#include "stdafx.h"
#include "ReadAG200Data.h"

CReadAG200::CReadAG200(CString _fileName) : fileName(_fileName),errorFlags(NO_APN_ERROR),sampleInterval(.1f)
{
	CFile f;
	int OK = f.Open(fileName,CFile::modeRead);
	if (!OK) errorFlags |= APN_ERROR_CANNOT_OPEN;
	else {
		errorFlags |= readApnDataHeaders(&f);
	}
	f.Close();
}

CReadAG200::~CReadAG200()
{
}

int CReadAG200::getRecordingStart(COleDateTime *_start)
{
	if (NO_APN_ERROR == errorFlags) {
		COleDateTime os(start.GetYear(),start.GetMonth(),start.GetDay(),
						start.GetHour(),start.GetMinute(),start.GetSecond());
		*_start = os;
	}
	return errorFlags;
}

int CReadAG200::getRecordingStart(CTime *_start)
{
	if (NO_APN_ERROR == errorFlags) {
		*_start = start;
	}
	return errorFlags;
}

float CReadAG200::getRecordingLength(void)
{
	return (float) recordingLength.GetTotalSeconds();
}

int CReadAG200::getRecordingLength(CTimeSpan *_length)
{
	if (NO_APN_ERROR == errorFlags) {
		*_length = recordingLength;
	}
	return errorFlags;
}

int CReadAG200::getError(void)
{
	return errorFlags;
}

CPatientData *CReadAG200::getAG200PatientInfo(void)
{
	return &patient;
}

void CReadAG200::getParameters(APN_SPARE_BLOCK_6xx *_sb,APN_FILE_HEADER_6xx *_fh,CF_SETUP_BLOCK_6xx *_cf,
		LOGGER_DATA_BLOCK_6xx *_lb,CATHETER_DATA_BLOCK_6xx *_cb)
{
	memcpy((void *) _sb,(void *) &spareApnBlock,sizeof(APN_SPARE_BLOCK_6xx));
	memcpy((void *) _fh,(void *) &apnFileHeader,sizeof(APN_FILE_HEADER_6xx));
	memcpy((void *) _cf,(void *) &m_setupData,sizeof(CF_SETUP_BLOCK_6xx));
	memcpy((void *) _lb,(void *) &m_LoggerData,sizeof(LOGGER_DATA_BLOCK_6xx));
	memcpy((void *) _cb,(void *) &m_CathData,sizeof(CATHETER_DATA_BLOCK_6xx));
}

int CReadAG200::readApnData(COximeterDataSet *_oxData,
							CBodyPosDataSet *_bpData,
							CCatheterDataSet *_cathData,
							CAG200MicDataSet *_ag200MicData)
{
	CFile f;
	int OK = f.Open(fileName,CFile::modeRead);
	if (!OK) errorFlags |= APN_ERROR_CANNOT_OPEN;
	else {
		errorFlags |= readApnDataHeaders(&f);
	}
	if (errorFlags) {
		f.Close();
		return errorFlags;
	}

	//---Read the data
	FLOAT interv = (FLOAT) apnFileHeader.msSampInterval / 1000.0f;
	size_t sized = apnFileHeader.numSamples * sizeof(double);
	size_t sizes = apnFileHeader.numSamples * sizeof(short);
	double *rawDbl = new double[apnFileHeader.numSamples];
	
	double *rawPOES = new double[apnFileHeader.numSamples];
	double *rawPPH = new double[apnFileHeader.numSamples];
	double *rawT0 = new double[apnFileHeader.numSamples];
	double *rawT1 = new double[apnFileHeader.numSamples];
	double *rawSound = new double[apnFileHeader.numSamples];

	memset(rawPOES, 0, apnFileHeader.numSamples * sizeof(double));
	memset(rawDbl, 0, apnFileHeader.numSamples * sizeof(double));
	memset(rawPPH, 0, apnFileHeader.numSamples * sizeof(double));
	memset(rawT0, 0, apnFileHeader.numSamples * sizeof(double));
	memset(rawT1, 0, apnFileHeader.numSamples * sizeof(double));

	if (m_LoggerData.userChannels & KEY_C_P0) f.Read(rawPOES,sized);
	if (m_LoggerData.userChannels & KEY_C_P1) f.Read(rawDbl,sized);
	if (m_LoggerData.userChannels & KEY_C_P2) f.Read(rawPPH,sized);
	if (m_LoggerData.userChannels & KEY_C_P3) f.Read(rawDbl,sized);
	if (m_LoggerData.userChannels & KEY_C_T0) f.Read(rawT0,sized);
	if (m_LoggerData.userChannels & KEY_C_T1) f.Read(rawT1,sized);
	if (m_LoggerData.userChannels & KEY_CHEST0) f.Read(rawDbl,sized);
	if (m_LoggerData.userChannels & KEY_CHEST1) f.Read(rawDbl,sized);
	if (m_LoggerData.userChannels & KEY_ORALNASAL_T) f.Read(rawDbl,sized);
	
	FLOAT tme = .0f;
	for (int i = 0 ; i < apnFileHeader.numSamples ; i++) {
		_cathData->addToXYRaw((FLOAT) (rawPOES[i] * kPa_TO_cmH2O),(FLOAT) (FLOAT) (rawPPH[i] * kPa_TO_cmH2O),(FLOAT) rawT0[i],(FLOAT) rawT1[i],0,tme);
		tme += interv;
	}
	
	double *rawO2 = new double[apnFileHeader.numSamples];
	double *rawHR = new double[apnFileHeader.numSamples];
	short *rawFlags = new short[apnFileHeader.numSamples];

	memset(rawO2, 0, apnFileHeader.numSamples * sizeof(double));
	memset(rawHR, 0, apnFileHeader.numSamples * sizeof(double));
	memset(rawFlags, 0, apnFileHeader.numSamples * sizeof(short));

	if (m_LoggerData.userChannels & KEY_SAO2) f.Read(rawO2,sized);
	if (m_LoggerData.userChannels & KEY_HR) f.Read(rawHR,sized);
	if (m_LoggerData.userChannels & KEY_SAO2) f.Read(rawFlags,sizes);
	tme = .0f;
	for (int i = 0 ; i < apnFileHeader.numSamples ; i++) {
		_oxData->addToXYRaw(.250f,(FLOAT) rawO2[i],.0f,(FLOAT) rawHR[i],(unsigned short) rawFlags[i],tme);
		tme += interv;
	}
	_oxData->setPenDownVector(TRUE);

	short *rawBP = new short[apnFileHeader.numSamples];

	memset(rawBP, 0, apnFileHeader.numSamples * sizeof(short));

	if (m_LoggerData.userChannels & KEY_BODY_POS) f.Read(rawBP,sizes);

	//---Correct Body pos, shift 1 down
	tme = .0f;
	for (int i = 0 ; i < apnFileHeader.numSamples - 1 ; i++) {
		_bpData->addToXYRaw(rawBP[i] - 1,tme);
		tme += interv;
	}
	_bpData->addToXYRaw(rawBP[apnFileHeader.numSamples - 1] - 1, tme,true); // Last sample
	
	//---Read snoring
	int n;
	char Tmp[10] = "";
	n = f.Read( Tmp, 6 );
	if( (6 == n) && (strcmp(Tmp,"SOUND:") == 0)) {
		UINT len = f.Read(rawSound,sized);
		tme = .0f;
		for (int i = 0 ; i < apnFileHeader.numSamples ; i++) {
			_ag200MicData->addToXYRaw((FLOAT) rawSound[i],tme);
			tme += interv;
		}	
	}

	delete [] rawDbl;
	delete [] rawBP;
	delete [] rawO2;
	delete [] rawHR;
	delete [] rawPOES;
	delete [] rawPPH;
	delete [] rawT0;
	delete [] rawT1;
	delete [] rawSound;
	delete [] rawFlags; 

	return errorFlags;
}

int CReadAG200::readApnDataHeaders(CFile *_pF)
{
	UINT num = _pF->Read(&apnFileHeader,sizeof(apnFileHeader));
	num = _pF->Read(&spareApnBlock,sizeof(spareApnBlock));

	patient.readFromArchive(_pF,apnFileHeader.pcSwVersion);
	
	num = _pF->Read(&m_setupData, sizeof(CF_SETUP_BLOCK_6xx));

	//--Check if the file is OK
	if (m_setupData.size == sizeof(CF_SETUP_BLOCK_6xx)) {
		; // Accept
	}
	else {
		errorFlags |= APN_ERROR_FILE_NOT_VERSION_6XX;
		return errorFlags;
	}
	
	num = _pF->Read(&m_LoggerData, sizeof(LOGGER_DATA_BLOCK_6xx));
	num = _pF->Read(&m_CathData, sizeof(CATHETER_DATA_BLOCK_6xx));

	sampleInterval = (FLOAT) apnFileHeader.msSampInterval / 1000.0f;
	
	//---Find the start
	CTime date0 = CTime(2000,1,1,12,0,0);
	CTimeSpan span0 = CTimeSpan(m_LoggerData.startDate.daysSince01012000 - 1,0,0,0);
	date0 += span0;
	start = CTime(date0.GetYear(),date0.GetMonth(),date0.GetDay(),m_LoggerData.startTime.h,m_LoggerData.startTime.m,m_LoggerData.startTime.s);

	//---Find the stop
	CTime stop = start;
	float secDuration = (FLOAT) apnFileHeader.numSamples * sampleInterval;
	recordingLength = CTimeSpan(0,0,0,(int) secDuration);
	
	return errorFlags;
}