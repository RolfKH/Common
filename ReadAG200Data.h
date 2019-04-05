#ifndef READ_AG200_DATA_H_INCLUDED
#define READ_AG200_DATA_H_INCLUDED

// ReadAG200Data.h : Defines the class behaviors for the application.
//

/*!
* @author		RKH
* @date		06.06.2017
* @copyright (C)Spiro Medical AS 2013 - 2017
*/

#include <vector>

#include "AG200Patient.h"
#include "vectorFunctions.h"

using namespace std;

#define kPa_TO_cmH2O		10.1972	// cmH2O / kPa

#define OXI_ALARM			0x38
#define OXI_DISCONNECTED	0x40
#define OXI_BAD_STATUS		0x78

const CString apnExt = _T("apn");

/**********************************************
	Old definitions (versions 6.x.x and earlier)
*/

#define DATA_SOURCE_NONE		0
#define DATA_SOURCE_EDF			1
#define DATA_SOURCE_APN			2

#define KEY_C_P0			0x0001
#define KEY_C_P1			0x0002
#define KEY_C_P2			0x0004
#define KEY_C_P3			0x0008
#define KEY_C_T0			0x0010
#define KEY_C_T1			0x0020
#define KEY_CHEST0			0x0040
#define KEY_CHEST1			0x0080
#define KEY_ORALNASAL_T		0x0100
#define KEY_SAO2			0x0200
#define KEY_HR				0x0400
#define KEY_MIC				0x0800
#define KEY_BODY_POS		0x1000
#define KEY_CPAP			0x2000

#define FRIENDLY_NAME_MAX_LEN		30
#define MODEL_DESCR_LENGHT			30

#define LOGGER_DATA_ID			0x0123
#define CATH_DATA_ID			0x89AB
#define CF_SETUP_BLOCK_ID		0xA1B3
#define CF_SETUP_ALT_BLOCK_ID	0xA1B2

typedef struct aDate {
  	unsigned short daysSince01012000;
} APNEA_DATE_6xx;

typedef struct aTime {
  	unsigned short h,m,s;
} APNEA_TIME_6xx;

typedef struct aUsageLimit {
    unsigned short limitType;
	unsigned short hours;
	APNEA_DATE_6xx limitDate;
} USAGE_LIMIT_6xx;

typedef struct aSensorCalibration {
	short offset,gain;
} SENSOR_CAL_6xx;

typedef struct aCFSetupBlock {
	unsigned short size;						// size of this structure
	unsigned short id;
	unsigned short cardStatus;
	unsigned short cardSetupStatus;
	APNEA_TIME_6xx setTime;
	unsigned short daysSince01012000,dayOfWeek;
	unsigned short dataBlocks;
} CF_SETUP_BLOCK_6xx;

typedef struct anAPNFileHeader {
	int numSamples;
	int msSampInterval;
	int pcSwVersion;
} APN_FILE_HEADER_6xx;

typedef struct anAPNSpareBlock {
	int spares[100];
} APN_SPARE_BLOCK_6xx;

#define LOGGER_STRUC_SPARES			1
#define CATHETER_STRUC_SPARES		2

typedef struct aLoggerDataBlock {
	unsigned short size;									// size of this structure
	unsigned short id;
	unsigned short swVersion; // logger firmware version
	char friendlyName[FRIENDLY_NAME_MAX_LEN];
	char model[MODEL_DESCR_LENGHT];
	unsigned short serialNumber[3];				// serial number hardcoded in program memory
	unsigned short legalChannels;				// legal channel configuration
	unsigned short userChannels;				// user channel configuration
	unsigned short useCatheter;					// use catheter
	unsigned short accHours;					// accumulated hours of operation
	USAGE_LIMIT_6xx limit;							// describes limit of use (date/hours)
    APNEA_DATE_6xx nextService;						// date of next major service
	APNEA_DATE_6xx lastUsed;						// date it was used last time
	APNEA_DATE_6xx startDate;						// date to start logging
	APNEA_TIME_6xx startTime;						// time to start logging
	unsigned short dontCareDate;				// dont check date when starting
	unsigned short loggerFlags;					// autostart or manual start, P0 serial out etc
	unsigned short recLength;					// recording time
	SENSOR_CAL_6xx calMic,calBodyPos,calCPAP,calONTemp;		// Calibrations
	SENSOR_CAL_6xx calChBelt0,calChBelt1;
	unsigned short pulseOxProtocol; 			// PulseOxProtocoll
	unsigned short language;
	unsigned short tempUnit,pressUnit;
	short structVersion;
	short tempBaseline;
	unsigned short P3RelP0;
	APNEA_DATE_6xx dateOfLastCFWrite;
	APNEA_TIME_6xx timeOfLastCFWrite;
	unsigned short unitsPer_uV,unitsPer_degC;
	unsigned short P1RelP0,P2RelP0,T1RelT0;
} LOGGER_DATA_BLOCK_6xx;

typedef struct aCatheterDataBlock {
	unsigned short size;		/* size of this structure*/
	unsigned short id;
	char friendlyName[FRIENDLY_NAME_MAX_LEN];
	char model[MODEL_DESCR_LENGHT];
	unsigned short serialNumber[3];				/* serial number hardcoded in the chip*/
	unsigned short channels;					/* channel configuration*/
	unsigned short accHours;					/* accumulated hours of operation*/
	USAGE_LIMIT_6xx limit;							/* describes limit of use (date/hours)*/
	SENSOR_CAL_6xx calP0,calP1,calP2,calP3;			/* pressure sensor calibrations*/
	SENSOR_CAL_6xx calT0,calT1;						/* temperature sensor calibrations*/
    APNEA_DATE_6xx nextService;						/* date of next major service*/
	APNEA_DATE_6xx nextGainAdjust;					/* date of next gain adjust*/
	APNEA_DATE_6xx lastUsed;						/* date it was used last time*/
	unsigned short defectChannels;					/* defect channels */
	unsigned short tempUnit,pressUnit;
	APNEA_DATE_6xx dateOfLastCFWrite;
	APNEA_TIME_6xx timeOfLastCFWrite;
	unsigned short spare[CATHETER_STRUC_SPARES];
} CATHETER_DATA_BLOCK_6xx;

/************************************/

#define NO_APN_ERROR						0x0000
#define APN_ERROR_CANNOT_OPEN				0x0001
#define APN_ERROR_CANNOT_READ				0x0002
#define APN_ERROR_FILE_NOT_VERSION_6XX		0x0004

class CReadAG200 {
public:
	CReadAG200(CString _fileName);
	~CReadAG200();
	int getRecordingStart(CTime *_start);
	int getRecordingStart(COleDateTime *_start);
	int getRecordingLength(CTimeSpan *_length);
	float getRecordingLength(void);
	int getError(void);
	int readApnData(COximeterDataSet *_oxData,CBodyPosDataSet *_bpData,
					CCatheterDataSet *_cathData,CAG200MicDataSet *_ag200MicData);
	void getParameters(APN_SPARE_BLOCK_6xx *_sb,APN_FILE_HEADER_6xx *_fh,CF_SETUP_BLOCK_6xx *_cf,
		LOGGER_DATA_BLOCK_6xx *_lb,CATHETER_DATA_BLOCK_6xx *_cb);
	CPatientData *getAG200PatientInfo(void);
protected:
	int errorFlags;
	CString fileName;
	int readApnDataHeaders(CFile *_pF);
	CPatientData patient;
	FLOAT sampleInterval;
	CTime start;
	CTimeSpan recordingLength;
	
	APN_SPARE_BLOCK_6xx spareApnBlock;
	APN_FILE_HEADER_6xx apnFileHeader;
	CF_SETUP_BLOCK_6xx m_setupData;
	LOGGER_DATA_BLOCK_6xx m_LoggerData;
	CATHETER_DATA_BLOCK_6xx m_CathData;
};

#endif