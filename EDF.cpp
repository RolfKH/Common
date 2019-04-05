
#include "stdafx.h"
#include "EDF.h"
#include "EDFHeader.h"
#include "agMessages.h"

EDFData::EDFData(void)
{
	binRecords = NULL;
	hasData = false;
	progressMessage = RegisterWindowMessage(AG_PROGRESS_MESSAGE);
}

EDFData::~EDFData(void)
{
	if (binRecords) delete [] binRecords;
}

bool EDFData::getHasData(void)
{
	return hasData;
}

EDF_BINARY_HEADER *EDFData::getHeader(void)
{
	return &binHeader;
}

int EDFData::getEnabledSignals(void)
{
	int num = 0;
	for (int i = 0 ; i < binHeader.numSignalsInRecord ; i++) if (binRecords[i].show) num++;
	return num;
}

EDF_BINARY_RECORD *EDFData::getRecords(void)
{
	return binRecords;
}

void EDFData::showDialogHeader(void)
{
	CEDFHeader2 dlg(fileName,binHeader,binRecords);
	
	dlg.DoModal();
}

bool EDFData::read(CFile *_pF)
{
	CWaitCursor wc;
	
	if (hasData) {	
		if (binRecords) delete [] binRecords;
		hasData = false;
	}
	fileName = _pF->GetFileName();
	
	UINT num = _pF->Read(&header,sizeof(header));

	::PostMessage(HWND_BROADCAST,progressMessage,0L,0L);
	
	binHeader.version = ::CString(header.version,sizeof(header.version));
	binHeader.version = binHeader.version.SpanExcluding(_T(" "));
	binHeader.patient = ::CString(header.patient,sizeof(binHeader.patient));
	binHeader.recordingID = ::CString(header.recordingID,sizeof(binHeader.recordingID));
	binHeader.numBytesInHeaderRecord = _ttoi(::CString(header.numBytesInHeaderRecord, sizeof(header.numBytesInHeaderRecord)));
	binHeader.reserved = ::CString(header.reserved,sizeof(header.reserved));
	binHeader.numDataRecords = _ttoi(::CString (header.numDataRecords,sizeof(header.numDataRecords)));
	binHeader.secRecordLength = _ttoi(::CString(header.secRecordLength,sizeof(header.secRecordLength)));
	binHeader.numSignalsInRecord = _ttoi(::CString(header.numSignalsInRecord,sizeof(header.numSignalsInRecord)));

	CString sdate = ::CString(header.startDate,sizeof(header.startDate));
	CString stime = ::CString(header.startTime,sizeof(header.startTime));
	binHeader.startDateAndTime = dateAndTimeToCTime(sdate,stime);

	binRecords = new EDF_BINARY_RECORD[binHeader.numSignalsInRecord]; 
	EDF_RECORD *edfr = new EDF_RECORD[binHeader.numSignalsInRecord];
	
	for (int i = 0 ; i < binHeader.numSignalsInRecord ; i++) num = _pF->Read(&edfr[i].label,sizeof(edfr[0].label));
	for (int i = 0 ; i < binHeader.numSignalsInRecord ; i++) num = _pF->Read(&edfr[i].transducerType,sizeof(edfr[0].transducerType));
	for (int i = 0 ; i < binHeader.numSignalsInRecord ; i++) num = _pF->Read(&edfr[i].physicalDimension,sizeof(edfr[0].physicalDimension));
	for (int i = 0 ; i < binHeader.numSignalsInRecord ; i++) num = _pF->Read(&edfr[i].physicalMinimum,sizeof(edfr[0].physicalMinimum));
	for (int i = 0 ; i < binHeader.numSignalsInRecord ; i++) num = _pF->Read(&edfr[i].physicalMaximum,sizeof(edfr[0].physicalMaximum));
	
	for (int i = 0 ; i < binHeader.numSignalsInRecord ; i++) num = _pF->Read(&edfr[i].digitalMinimum,sizeof(edfr[0].digitalMinimum));
	for (int i = 0 ; i < binHeader.numSignalsInRecord ; i++) num = _pF->Read(&edfr[i].digitalMaximum,sizeof(edfr[0].digitalMaximum));
	for (int i = 0 ; i < binHeader.numSignalsInRecord ; i++) num = _pF->Read(&edfr[i].prefiltering,sizeof(edfr[0].prefiltering));
	for (int i = 0 ; i < binHeader.numSignalsInRecord ; i++) num = _pF->Read(&edfr[i].numSamples,sizeof(edfr[0].numSamples));
	for (int i = 0 ; i < binHeader.numSignalsInRecord ; i++) num = _pF->Read(&edfr[i].reserved,sizeof(edfr[0].reserved));
	
	long perc = 10;
	::PostMessage(HWND_BROADCAST,progressMessage,perc,0L);

	for (int i = 0 ; i < binHeader.numSignalsInRecord ; i++) {
		makeBinaryRecordHeader(binHeader.secRecordLength,edfr[i],&binRecords[i]);
	}
	
	delete [] edfr;

	/*
	Conversion: 
		offset = (physicalMax + physicalMin) / 2
		gain = (digitalMax - digitalMin) / (physicalMax - physicalMin)

		digital = (physical - offset) * gain
		physical = offset + digital / gain
	*/
	//---Read the data
	int step = binHeader.numDataRecords / 10;
	long percStep = 8;
	int index = 0;
	for (int k = 0 ; k < binHeader.numDataRecords ; k++) {
		for (int i = 0 ; i < binHeader.numSignalsInRecord ; i++) {
			short *buffer = new short[binRecords[i].numSamples];
			size_t size = sizeof(short) * binRecords[i].numSamples;
			_pF->Read(buffer,size);
			short *bP = buffer;
			for (int j = 0 ; j < binRecords[i].numSamples ; j++) 
				binRecords[i].data.push_back((float) *bP++);
		    
			delete [] buffer;
		}
		index++;
		if (index > step) {
			index = 0;
			perc += 8;
			::PostMessage(HWND_BROADCAST,progressMessage,(long) perc,0L);
		}
	}

	for (int i = 0 ; i < binHeader.numSignalsInRecord ; i++) {
		float gain = binRecords[i].gain;
		float offset = binRecords[i].offset;
		transform(binRecords[i].data.begin(),binRecords[i].data.end(),binRecords[i].data.begin(),LinConvertFunc <float> (gain,offset));
	}

	::PostMessage(HWND_BROADCAST,progressMessage,99L,0L);
	::Sleep(500);
	::PostMessage(HWND_BROADCAST,progressMessage,100L,0L);

	hasData = true;
	return true;
}

CTime EDFData::dateAndTimeToCTime(CString _d,CString _t)
{
	CString sdate = _d;
	sdate.TrimLeft(_T(" "));
	sdate.TrimRight(_T(" "));
	CString dayS = sdate.SpanExcluding(_T("."));
	sdate = sdate.Right(sdate.GetLength() - dayS.GetLength() - 1);
	CString monS = sdate.SpanExcluding(_T("."));
	sdate = sdate.Right(sdate.GetLength() - monS.GetLength() - 1);
	int day = _ttoi(dayS);
	int mon = _ttoi(monS);
	int year = _ttoi(sdate);
	if ((year <= 99) && (year >= 85)) year += 1900;
	else year += 2000;

	CString stime = _t;
	stime.TrimLeft(_T(" "));
	stime.TrimRight(_T(" "));
	CString hS = stime.SpanExcluding(_T("."));
	stime = stime.Right(stime.GetLength() - hS.GetLength() - 1);
	CString mS = stime.SpanExcluding(_T("."));
	stime = stime.Right(sdate.GetLength() - mS.GetLength() - 1);
	int h = _ttoi(hS);
	int m = _ttoi(mS);
	int s = _ttoi(stime);
	
	CTime t(year,mon,day,h,m,s);
	return t;
}

void EDFData::makeBinaryRecordHeader(int _secRecordLength,EDF_RECORD _edfr,EDF_BINARY_RECORD *_brec)
{
	_brec->show = TRUE;
	_brec->label = ::CString(_edfr.label,sizeof(_edfr.label));
	_brec->label.TrimRight(_T(" "));
	_brec->label.TrimLeft(_T(" "));
	_brec->transducerType = ::CString(_edfr.transducerType,sizeof(_edfr.transducerType));
	_brec->transducerType.TrimRight(_T(" "));
	_brec->transducerType.TrimLeft(_T(" "));
	_brec->physicalDimension = ::CString(_edfr.physicalDimension,sizeof(_edfr.physicalDimension));
	_brec->physicalDimension.TrimRight(_T(" "));
	_brec->physicalDimension.TrimLeft(_T(" "));
	_brec->physicalMinimum = _ttof(::CString(_edfr.physicalMinimum,sizeof(_edfr.physicalMinimum)));
	_brec->physicalMaximum = _ttof(::CString(_edfr.physicalMaximum,sizeof(_edfr.physicalMaximum)));
	_brec->digitalMinimum = _ttof(::CString(_edfr.digitalMinimum,sizeof(_edfr.digitalMinimum)));
	_brec->digitalMaximum = _ttof(::CString(_edfr.digitalMaximum,sizeof(_edfr.digitalMaximum)));
	_brec->prefiltering = ::CString(_edfr.prefiltering,sizeof(_edfr.prefiltering));
	_brec->prefiltering.TrimRight(_T(" "));
	_brec->prefiltering.TrimLeft(_T(" "));
	_brec->numSamples = _ttoi(::CString(_edfr.numSamples,sizeof(_edfr.numSamples)));
	_brec->reserved = ::CString(_edfr.reserved,sizeof(_edfr.reserved));	

	_brec->sampFreq = (double) _brec->numSamples / _secRecordLength;
	_brec->msSampInt = (float) 1000.0f / (float) _brec->sampFreq;		// To be computed - in ms
	
	_brec->offset = (float) (_brec->physicalMaximum + _brec->physicalMinimum) / 2.0f;
	_brec->gain = (float) (_brec->digitalMaximum - _brec->digitalMinimum) / (float) (_brec->physicalMaximum - _brec->physicalMinimum);

}