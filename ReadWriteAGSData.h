#ifndef READ_WRITE_AGS_DATA_INCLUDED
#define READ_WRITE_AGS_DATA_INCLUDED

/*!
	@file		ReadWriteAGSData.h
	@version	1.0 RKH
*/

/*!
* @author		RKH
* @date		06.06.2017
* @copyright (C)Spiro Medical AS 2013 - 2017
*/

#include "Anthropologia.h"
#include "vectorFunctions.h"
#include "..\..\Firmware\spiro\AGS_definitions.h"
#include "..\..\Firmware\spiro\neck\AGS_neckDefinitions.h"
#include "..\..\Firmware\spiro\wrist\AGS_wristDefinitions.h"
#include "..\..\Firmware\spiro\Torso_SD_USB_I2C\AGSTorsoDefinitions.h"
#include "..\..\Firmware\spiro\catheter\AGS_cathDefinitions.h"
#include "ReadAG200Data.h"

#define HAS_NO_DATA					0x0000
#define HAS_BATTERY_DATA			0x0001
#define HAS_CONTACT_MIC_DATA		0x0002
#define HAS_AIR_MIC_DATA			0x0004
#define HAS_BODY_POS_XYZ_DATA		0x0008
#define HAS_ACTIMETER_DATA			0x0010
#define HAS_OXIMETER_DATA			0x0020
#define HAS_BODY_POS_DATA			0x0040
#define HAS_CATHETER_DATA			0x0080
#define HAS_AG200_MIC_DATA			0x0100
#define HAS_RESP_DATA				0x0200

const int PATH_TO_AGS_FILE = 9;

/*			In SW Version
Version 1.						Pre April 2015
Version 2						Corrected bug in Serial number serialize. Unit was read/written twice and not serial
								Now the recording Patient ID has been replaced with Recording ID. Recording ID is added to patient information
Version 4						Now saves computed data to file 
								Data files with major version < 4 must be read with sw version 3.2 or higher!
Version 5						Oxymetry and Pulse Rate. Saves time of max and min
Version 6		4.0				Oxymetry. Saves invalid time windows
Version 7		4.1				New T0, T1 envelope curve. Data from previous versions are recalculated
Version 8		4.2				Catheter raw data are saved to disk
Version 9		4.2				Catheter invalid time window vector saved to disk
Version 10      4.5.2			Added torso actimeter data
Version 11		4.5.3			Added doctor`s notes
Version 12		4.5.4			Added vectors bFlow,bEff 
									Breathing flow as a weighted sum of T0 and T1
									Breathing efficiency as the corresponding admittance
Version 13		4.5.5			Added manual marker series
Version 14		5.6.1			Email and telephone of patient are now encrypted
Version 15		6.1.1			Added save and read of respbelts enviroment data, breathing efficiency and resp frequency
*/

#define AGS_FILE_MAJOR_VERSION		15
#define AGS_FILE_MINOR_VERSION		0

class CReadWriteAGSData
{
public:
	CReadWriteAGSData();
	~CReadWriteAGSData();
	void setAnthropologiaData(	CPatient *_pt,
								CAttendingDoctor *_dct,
								COperator *_op,
								CReferringDoctor *_rd);
	void setData(	CBatteryDataSet *_battData,
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
					CManualMarkerSet *_markerData);
	void setParameters(	NECK_PARAMETER_SET *_neckParameters,
						WRIST_PARAMETER_SET *_wristParameters,
						CATH_PARAMETER_SET *_cathParameters,
						TORSO_PARAMETER_SET *_torsoParameters);
	void setAG200Parameters(	APN_SPARE_BLOCK_6xx *_spareApnBlock,
								APN_FILE_HEADER_6xx *_apnFileHeader,
								CF_SETUP_BLOCK_6xx *_m_setupData,
								LOGGER_DATA_BLOCK_6xx *_m_LoggerData,
								CATHETER_DATA_BLOCK_6xx *_m_CathData);
	bool Serialize(CArchive& ar,bool _anthroOnly = false,bool _ag200TypeData = false);
	bool getIsAG200Data(void);
	void setIsAG200Data(bool _set);
	SERIAL_NUMBER getTorsoSerialNumber(void);
	int getAGSMajorVersion(void);
	int getAGSMinorVersion(void); 
	void setDoctorsNotes(CString _s);
	CString getDoctorsNotes(void);
	static CString getMinSWversionToReadFile(int _majorFileVersion, int _minorFileVersion);
protected:
	bool AG200TypeData;
	void Serialize(CArchive& ar,SERIAL_NUMBER *_ser);
	short int minorFileVersion,majorFileVersion;
	CPatient *pt;
	CAttendingDoctor *dct;
	COperator *op;
	CReferringDoctor *rd;

	CString doctorsNotes;
	
	CBatteryDataSet *battData;
	CContactMicDataSet *cmData;
	CAirMicDataSet *amData;
	CBodyPosXYZDataSet *bpXYZData;
	CActimeterDataSet *acData;
	CActimeterDataSet *torsoAcData;
	COximeterDataSet *oxData;
	CBodyPosDataSet *bpData;
	CCatheterDataSet *cathData;
	CAG200MicDataSet *ag200MicData;
	CRespBeltDataSet *respData;
	CManualMarkerSet *mMarkerData;
	
	NECK_PARAMETER_SET *neckParameters;
	WRIST_PARAMETER_SET *wristParameters;
	CATH_PARAMETER_SET *cathParameters;
	TORSO_PARAMETER_SET *torsoParameters;

	//---AG200 parameters
	APN_SPARE_BLOCK_6xx *spareApnBlock;
	APN_FILE_HEADER_6xx *apnFileHeader;
	CF_SETUP_BLOCK_6xx *m_setupData;
	LOGGER_DATA_BLOCK_6xx *m_LoggerData;
	CATHETER_DATA_BLOCK_6xx *m_CathData;

	bool checkVersionCompatibility(bool _issueWarning);
};

#endif