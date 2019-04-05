#ifndef EDF_FORMAT_SPEC_INCLUDED
#define EDF_FORMAT_SPEC_INCLUDED

#include <vector>
#include <algorithm>
using namespace std;
#include "..\Common\vectorFunctions.h"

const CString edfExt1 = _T("rec");
const CString edfExt2 = _T("edf");

typedef struct anEDFheader {
	char version[8];
	char patient[80];
	char recordingID[80];
	char startDate[8];
	char startTime[8];
	char numBytesInHeaderRecord[8];
	char reserved[44];
	char numDataRecords[8];
	char secRecordLength[8];
	char numSignalsInRecord[4];
} EDF_HEADER;

typedef struct anEDFBinaryHeader {
	CString version;
	CString patient;
	CString recordingID;
	CTime startDateAndTime;
	int numBytesInHeaderRecord;
	CString reserved;
	int numDataRecords;
	long secRecordLength;
	int numSignalsInRecord;
} EDF_BINARY_HEADER;

typedef struct anEDFrecord {
	char label[16];
	char transducerType[80];
	char physicalDimension[8];
	char physicalMinimum[8];
	char physicalMaximum[8];
	char digitalMinimum[8];
	char digitalMaximum[8];
	char prefiltering[80];
	char numSamples[8];
	char reserved[32];
} EDF_RECORD;

typedef struct anEDFBinaryRecord {
	BOOL show;
	CString label;
	CString transducerType;
	CString physicalDimension;
	double physicalMinimum;
	double physicalMaximum;
	double digitalMinimum;
	double digitalMaximum;
	CString prefiltering;
	int numSamples;
	CString reserved;
	double sampFreq;		// To be computed - in Hz
	vector <FLOAT> data;	// To be computed
	vector <FLOAT> envUpperAmpl,envLowerAmpl;
	vector <FLOAT> envUpperSample,envLowerSample;
	float msSampInt;		// To be computed - in ms
	float offset;			// To be computed
	float gain;				// To be computed
} EDF_BINARY_RECORD;

class EDFData 
{
public:
	EDFData(void);
	~EDFData(void);
	bool read(CFile *_f);
	bool getHasData(void);
	void showDialogHeader(void);
	EDF_BINARY_HEADER *getHeader(void);
	EDF_BINARY_RECORD *getRecords(void);
	int getEnabledSignals(void);
protected:
	CString fileName;
	UINT progressMessage;
	bool hasData;
	EDF_HEADER header;
	EDF_BINARY_HEADER binHeader;
	EDF_BINARY_RECORD *binRecords;
	CTime dateAndTimeToCTime(CString _d,CString _t);
	void makeBinaryRecordHeader(int _secRecordLength,EDF_RECORD _edfr,EDF_BINARY_RECORD *_brec);

	CXTPOfficeBorder<CXTPReportControl,false> table;
};


#endif