#ifndef GRAPH_CHANNEL_LIST_INCLUDED
#define GRAPH_CHANNEL_LIST_INCLUDED

/*
Copyright 2011-2013. Spiro Medical AS

Software for setup and test of graph control 
	Control of 
		Graph displays, 
		Channel selection, 
		Colours, 
		Transparency,
		Line thickness
		Engineering units

Rev	By	Description
0	RKH	Initial version
1	RKH Each channel now has many data types (raw, filtered, peak to peak, envelope, baseline, etc.
*/

#include "CommonResource.h"
#include "dataMMFiles.h"
#include <xmllite.h>

const CString regFilesStr = _T("Files");
const CString regProgramDataFolderStr	= _T("Program data folder");
const CString regProgramFolderStr	= _T("Program folder");
const CString regDefaultGraphXML = _T("DefaultGraphSettings");
const CString regLastUsedGraphXML = _T("LastUsedGraphSettings");

const int channelConfigVersion = 1;

const int ERROR_CH_ID = -1;

#include <vector>
#include <algorithm>
using namespace std;

//---Engineering unit conversions
const double FROM_kPA_TO_CMH2O	= 10.197443;
const double FROM_kPA_TO_BAR	= 0.01;
const double FROM_kPA_TO_MMHG	= 7.500615;
const double FROM_CMH2O_TO_kPA	= 0.0980638;
const double FROM_BAR_TO_kPA	= 100.0f;
const double FROM_MMHG_TO_kPA	= 0.1333224;

enum
{
	COLUMN_ITEM,
	COLUMN_CHECK,
	COLUMN_SETTING,
};

const int COLUMN_CHECK_ICON  =  2;
const int NO_SECTION		= 0xFF;
const int NO_CHANNEL		= 0xFF;

const long MIN_TRANSPARENCY = 0;
const long MAX_TRANSPARENCY = 95; // Not 100 as it will completely disappear then


//---Registry strings
const CString companyKey				= _T("Spiro Medical");
const CString profileKey				= _T("AnalysisViewStructure");
const CString regsVersion				= _T("Version");
const CString regsPageName				= _T("Page name");
const CString regsChannelColour			= _T("Colour");
const CString regsChannelThickness		= _T("Thickness");
const CString regsChannelType			= _T("Data type");
const CString regsChannelShowType		= _T("Data type to show");
const CString regsEnabled				= _T("Enabled");
const CString regsUseGrid				= _T("Use grid");
const CString regsStartTime				= _T("Start time");
const CString regsStopTime				= _T("Stop time");
const CString regsPressUnit				= _T("Pressure unit");
const CString regsTempUnit				= _T("Temperature unit");
const CString regsChannelID				= _T("Channel ID");
const CString regsChannelFamily			= _T("Channel family");
const CString regsChannelDescriptionID	= _T("Channel description ID");
const CString regsChannelTooltipID		= _T("Channel tooltip ID");
const CString regsChannelTransparency	= _T("Channel transparency");
const CString regsChannelUserNameID		= _T("Channel user name ID");
const CString regsExportImportPath		= _T("Export Import path");
const CString regsExportUnitString0		= _T("Unit0");
const CString regsExportUnitString1		= _T("Unit1");
const CString regsMaxYString			= _T("MaxY");
const CString regsMinYString			= _T("MinY");

#define DEF_PAGE_ENABLE		FALSE
#define DEF_SECTION_ENABLE	FALSE
#define DEF_USE_GRID		FALSE

#define SECS_IN_8_HOURS		28800

const int numberOfGraphPages			= 6;		// Number of graph pages
const int numberOfGraphSectionsPerPage	= 10;		// Number of sections per page

const int regNameLen		= 20;
const int minMaxYFmtLen		= 5;
const int sectionTxtLen		= 20;
const int pageTxtLen		= 20;
const int maxThickness		= 6;
const int minThickness		= 1;

/*
	eqSampleGraph has no separate timeVector, only sample interval
	xyGraph may not be equally sampled and has separate timeVector
*/
//---Data types								Graph 
const unsigned int dataTypeXYRaw		=	0x0001;		//					xyGraph
const unsigned int dataTypePeakToPeak	=	0x0004;		//					xyGraph
const unsigned int dataTypeBaselineAvg	=	0x0010;		//					xyGraph
const unsigned int dataTypeBaselineMax	=	0x0020;		//					xyGraph
const unsigned int dataTypeEnvelope		=	0x0040;		// Envelopes

typedef struct aChannelDesc {
	_TCHAR regName[regNameLen];				// Name in registry		
	int userNameStringID;					// Name string ID
	UINT dataTypes;							// Data types
	UINT showDataTypes;
	UINT thickness;							// Thickness 
	COLORREF colour;						// Colour
	UINT channelID;							// ID
	UINT family;							// Channel family
	long transparency;						// Transparency 0 - 100
	UINT tooltipStringID;					// Tooltip string ID
	UINT descriptionStringID;				// Description string ID
	UINT unitStrID0;						// Engineering unit string ID0
	UINT unitStrID1;						// Engineering unit string ID1
	double minY,maxY;						// Y axis definition
	_TCHAR minMaxYFmt[minMaxYFmtLen];		// Format string for y-axis min and max
} CH_SPEC;

//---Data set IDs
#define NO_ID			-1

enum {
	ID_P0,						// 0	
	ID_P1,						// 1	
	ID_T0,						// 2	
	ID_T1,						// 3	
	ID_P0_P1,					// 4	
	ID_T0PLUST1,				// 5	
	ID_ADMITTANCE_0,			// 6	
	ID_ADMITTANCE_1,			// 7	
	ID_SpO2,					// 8	
	ID_PR,						// 9	
	ID_BPos,					// 10	
	ID_CONTACT_SOUND,			// 11	
	ID_AIRB_SOUND,				// 12	
	ID_ACTIMETER,				// 13	
	ID_RB_THORACIC,				// 14	
	ID_RB_ABDOMINAL,			// 15	
	ID_EEG,						// 16	
	ID_CANNULAID,				// 17
	ID_RESPIRATION_FRQ,			// 18
	ID_FLOW,					// 19
	ID_BREATHING_EFFICIENCY,	// 20
	ID_ENUM_LIMIT				// 21	
};

//---Channel families
const unsigned int FAMILY_NONE			= 0x0000;
const unsigned int FAMILY_PRESS			= 0x0001;
const unsigned int FAMILY_TEMP			= 0x0002;
const unsigned int FAMILY_TEMP_PRESS	= 0x0004;	// Temp  / Press
const unsigned int FAMILY_PRESS_TEMP	= 0x0008;	// Press / Temp

const unsigned int FAMILY_SpO2			= 0x0010;
const unsigned int FAMILY_PR			= 0x0020;
const unsigned int FAMILY_BP			= 0x0040;
const unsigned int FAMILY_RAW_SOUND		= 0x0080;
const unsigned int FAMILY_DB_SOUND		= 0x0100;
const unsigned int FAMILY_ACTI			= 0x0200;
const unsigned int FAMILY_BIOPOT		= 0x0400;
const unsigned int FAMILY_STRETCH		= 0x0800;
const unsigned int FAMILY_CANNULA		= 0x1000;
const unsigned int FAMILY_FRQ			= 0x2000;

#define DEF_PRESS_UNIT		IDS_UNIT_P_CMH2O
#define DEF_TEMP_UNIT		IDS_UNIT_T_DEGC_PER_SEC

const unsigned int noType				= 0;
const unsigned int pressType			= dataTypeXYRaw | dataTypeEnvelope | dataTypePeakToPeak | dataTypeBaselineAvg | dataTypeBaselineMax;
const unsigned int tempType				= dataTypeXYRaw | dataTypeEnvelope | dataTypePeakToPeak | dataTypeBaselineAvg | dataTypeBaselineMax ;
const unsigned int admType				= dataTypePeakToPeak;
const unsigned int diffType				= dataTypePeakToPeak;
const unsigned int spO2Type				= dataTypeXYRaw;
const unsigned int prType				= dataTypeXYRaw;
const unsigned int bpType				= dataTypeXYRaw;
const unsigned int cntSoundType			= dataTypeXYRaw;
const unsigned int airSoundType			= dataTypeXYRaw;
const unsigned int actimeterType		= dataTypeXYRaw;
const unsigned int rbeltType			= dataTypeXYRaw | dataTypeEnvelope | dataTypePeakToPeak | dataTypeBaselineAvg | dataTypeBaselineMax;
const unsigned int cannulaType			= dataTypeXYRaw | dataTypeEnvelope | dataTypePeakToPeak | dataTypeBaselineAvg | dataTypeBaselineMax;
const unsigned int eegType				= dataTypeXYRaw;
const unsigned int respFrqType			= dataTypeXYRaw;
	  

//const CH_SPEC defaultChSpec[] = {  
	//  name in registry,		name string ID,			data type,		Show data type,		line thickn,	colour,					ID,					family,				transparency,	Tooltip ID,					Description ID,					Unit string ID0,    Unit string ID1,		MinY,maxY,	Format string for minMaxY
///* 0*/  {_T("A: P0"),				IDS_P0,				pressType,		noType,					1,			RGB(255,	0,	0),		ID_P0,				FAMILY_PRESS,			0,			IDS_P0_TOOLTIP,				IDS_P0_DESCRIPTION,				DEF_PRESS_UNIT,			0,				-100,100,	_T("%.2f")	},
///* 1*/	{_T("B: P1"),				IDS_P1,				pressType,		noType,					1,			RGB(255,	0,	0),		ID_P1,				FAMILY_PRESS,			0,			IDS_P1_TOOLTIP,				IDS_P1_DESCRIPTION,				DEF_PRESS_UNIT,			0,				-100,100,	_T("%.2f")	},
///* 2*/	{_T("C: T0"),				IDS_T0,				tempType,		noType,					1,			RGB(36,		0,	92),	ID_T0,				FAMILY_TEMP,			0,			IDS_T0_TOOLTIP,				IDS_T0_DESCRIPTION,				DEF_TEMP_UNIT,			0,				-100,100,	_T("%.2f")	},
///* 3*/	{_T("D: T1"),				IDS_T1,				tempType,		noType,					1,			RGB(36,		0,	92),	ID_T1,				FAMILY_TEMP,			0,			IDS_T1_TOOLTIP,				IDS_T1_DESCRIPTION,				DEF_TEMP_UNIT,			0,				-100,100,	_T("%.2f")	},
///* 4*/	{_T("E: P0-P1"),			IDS_P0_P1,			diffType,		noType,					1,			RGB(0,		255,0),		ID_P0_P1,			FAMILY_PRESS,			0,			IDS_P0_P1_TOOLTIP,			IDS_P0_P1_DESCRIPTION,			DEF_PRESS_UNIT,			0,				  -50,50,	_T("%.2f")	},
///* 5*/	{_T("F: T0+T1"),			IDS_T0PLUST1,		diffType,		noType,					1,			RGB(0,		0,	255),	ID_T0PLUST1,		FAMILY_TEMP,			0,			IDS_T0PLUST1_TOOLTIP,		IDS_T0PLUST1_DESCRIPTION,		DEF_TEMP_UNIT,			0,				-100,100,	_T("%.2f")	},
///* 7*/	{_T("G: ADMITTANCE_0"),		IDS_ADMITTANCE_T0,	admType,		noType,					1,			RGB(0,		0,	0),		ID_ADMITTANCE_0,	FAMILY_TEMP_PRESS,		0,			IDS_ADMITTANCE_T0_TOOLTIP,	IDS_ADMITTANCE_T0_DESCRIPTION,	DEF_TEMP_UNIT,			DEF_PRESS_UNIT,	    0, 1,	_T("%.2f")	},
///* 9*/	{_T("H: ADMITTANCE_1"),		IDS_ADMITTANCE_T1,	admType,		noType,					1,			RGB(0,		0,	0),		ID_ADMITTANCE_1,	FAMILY_TEMP_PRESS,		0,			IDS_ADMITTANCE_T1_TOOLTIP,	IDS_ADMITTANCE_T1_DESCRIPTION,	DEF_TEMP_UNIT,			DEF_PRESS_UNIT,	    0, 1,	_T("%.2f")	},
///* 10*/	{_T("I: SpO2"),				IDS_SpO2,			spO2Type,		noType,					1,			RGB(158,	0,	160),	ID_SpO2,			FAMILY_SpO2,			0,			IDS_SpO2_TOOLTIP,			IDS_SpO2_DESCRIPTION,			IDS_UNIT_SaO2,			0,				  60,100,	_T("%.1f")	},
///* 11*/	{_T("J: PR"),				IDS_PR,				prType,			noType,					1,			RGB(36,		192,0),		ID_PR,				FAMILY_PR,				0,			IDS_PR_TOOLTIP,				IDS_PR_DESCRIPTION,				IDS_UNIT_PR,			0,				  50,120,	_T("%.0f")	},
///* 12*/	{_T("K: BPos"),				IDS_BPos,			bpType,			noType,					1,			RGB(192,	32,	0),		ID_BPos,			FAMILY_BP,				0,			IDS_BPos_TOOLTIP,			IDS_BPos_DESCRIPTION,			IDS_UNIT_BPosXY,		0,					-1,1,	_T("%.1f")	},
///* 13*/	{_T("L: Contact Sound"),	IDS_CONTACT_SOUND,	cntSoundType,	noType,					1,			RGB(100,	0,	0),		ID_CONTACT_SOUND,	FAMILY_RAW_SOUND,		0,			IDS_CONTACT_SOUND_TOOLTIP,	IDS_CONTACT_SOUND_DESCRIPTION,	IDS_UNIT_CONTACT_SOUND,	0,					0,100,	_T("%.0f")	},
///* 14*/	{_T("M: Airborne Sound"),	IDS_AIRB_SOUND,		airSoundType,	noType,					1,			RGB(100,	100,0),		ID_AIRB_SOUND,		FAMILY_DB_SOUND,		0,			IDS_AIRB_SOUND_TOOLTIP,		IDS_AIRB_SOUND_DESCRIPTION,		IDS_UNIT_AIRB_SOUND,	0,					0,100,	_T("%.0f")	},
///* 15*/	{_T("N: Actimeter"),		IDS_ACTIMETER,		actimeterType,	noType,					1,			RGB(0,		0,	255),	ID_ACTIMETER,		FAMILY_ACTI,			0,			IDS_ACTIMETER_TOOLTIP,		IDS_ACTIMETER_DESCRIPTION,		IDS_UNIT_ACTIMETER,		0,				    -1,1,	_T("%.2f")	},
///* 16*/	{_T("O: RB thoracic"),		IDS_RB_THORACIC,	rbeltType,		noType,					1,			RGB(255,	0,	0),		ID_RB_THORACIC,		FAMILY_STRETCH,			0,			IDS_RB_THORACIC_TOOLTIP,	IDS_RB_THORACIC_DESCRIPTION,	IDS_UNIT_RB,			0,				   0,100,	_T("%.2f")	},
///* 17*/	{_T("P: RB abdominal"),		IDS_RB_ABDOMINAL,	rbeltType,		noType,					1,			RGB(255,	0,	255),	ID_RB_ABDOMINAL,	FAMILY_STRETCH,			0,			IDS_RB_ABDOMINAL_TOOLTIP,	IDS_RB_ABDOMINAL_DESCRIPTION,	IDS_UNIT_RB,			0,				   0,100,	_T("%.2f")	},
///* 18*/	{_T("Q: EEG"),				IDS_EEG,			eegType,		noType,					1,			RGB(255,	0,	255),	ID_EEG,				FAMILY_NONE,			0,			IDS_EEG,					IDS_EEG,						IDS_UNIT_NO,			0,				   0,100,	_T("%.2f")	},
///* 19*/	{_T("R: Cannula"),			IDS_CANNULA,		cannulaType,	noType,					1,			RGB(0,		0,	0),		ID_CANNULAID,		FAMILY_CANNULA,			0,			IDS_CANNULA_TOOLTIP,		IDS_CANNULA_DESCRIPTION,		IDS_UNIT_NO,			0,				   0,100,	_T("%.2f")	},
///* 20*/	{_T("S: Respiration frw"),	IDS_RESPIRATION_FRQ,respFrqType,	noType,					1,			RGB(0,		0,	0),		ID_RESPIRATION_FRQ,	FAMILY_FRQ,				0,			IDS_RESPIRATION_FRQ_TOOLTIP,IDS_RESPIRATION_FRQ_DESCRIPTION,IDS_UNIT_HZ,			0,				   0,1,		_T("%.2f")	}
//};																																	
//const int numDataSets = sizeof(defaultChSpec) / sizeof(CH_SPEC);

const int numDataSets = ID_ENUM_LIMIT;

const CString xmlComment11 = _T("\nComments\n\nPage\n\tUp to 4 pages available, from 1 to 4\n\n\tTimeAxis, start and stop:\n\t\tStart and end of graph in hours:minutes:seconds (hh:mm:ss)\n\t\tStart must be before stop\n\t\tStart of recording is 00:00:00\n\n");
const CString xmlComment12 = _T("Section\n\tUp to 10 sections available, from 1 to 10\n\tGrid: 0: do not use grid\n\t\t1: use grid\n\n\tEngineeringUnits\n\tTempUnit\n\t\t0: degC/s\n\t\t1: degF/s\n");
const CString xmlComment13 = _T("\tPressureUnit\n\t\t0: cmH2O\n\t\t1: mmHg\n\t\t2: kPa\n\t\t3: Bar\n\n");
const CString xmlComment14 = _T("Channels\n\tThe following channel tags are legal:\n");
const CString xmlComment21 = _T("\n\tColour\n\t\tIn R,G,B (Red: 0-255, Green: 0-255, Blue: 0-255)\n");
const CString xmlComment22 = _T("\tThickess\n\t\tLine thickness: Integer from 1 to 6\n\tMinimumY\n\t\tMinimum Y axis in current engineering unit\n\tMaximumY\n\t\tMaximum Y axis in current engineering unit\n");
const CString xmlComment23 = _T("\tTransparency\n\t\tFrom 0 (%) to 90 (%). 0 is not transparent, 90 is close to invisible\n");
const CString xmlComment24 = _T("\n\tNote that if maximumY is less or equal to minimumY, maximum Y is set to minimumY plus 100 (regardless of the unit selected)\n");
const CString xmlComment25 = _T("\n\n\tChannel enabled parameter is a bitstring defined as a combination of the following:\n");
const CString xmlComment26 = _T("\n\t\tdataTypeRaw          =	0000 0000 0001");
const CString xmlComment30 = _T("\n\t\tdataTypePeakToPeak   =	0000 0000 0100");
const CString xmlComment32 = _T("\n\t\tdataTypeBaselineAvg  =	0000 0001 0000");
const CString xmlComment33 = _T("\n\t\tdataTypeBaselineMax  =	0000 0010 0000\n\n");

typedef struct aSection {
	_TCHAR regName[regNameLen];
	_TCHAR sectionTxt[sectionTxtLen];
	BOOL useGrid;
	int pressUnit,tempUnit;  // Unit string IDs
	BOOL enabled;
	CH_SPEC chSpec[numDataSets];
} SECTION;

typedef struct aPage {
	_TCHAR regName[regNameLen];
	_TCHAR pageTxt[pageTxtLen];
	BOOL enabled;
	SECTION section[numberOfGraphSectionsPerPage];
	double secStart,secStop;
} PAGE;

const int MM_MMFILE_MAX_SIZE = sizeof(PAGE) * (numberOfGraphPages + 1);

class CChStorage 
{
public:
	CChStorage();
	~CChStorage();
	virtual BOOL save(const CString _file = _T(""));
	virtual BOOL read(const CString _file = _T(""));
	int getNumPages(void);
	int getSectionsPerPage(void);
	int getChannelsPerSection(void);
	int getNumEnabledPages(void);
	int getNumEnabledSections(const int _page);
	int getNumEnabledChannels(const int _page,const int _section);
	CString getPageText(const int _num);
	void setPageText(const int _num,const CString _pt);
	void setSectionText(const int _pnum,const int _snum,const CString _st);
	CString getSectionText(const int _pageNum,const int _sectionNum);
	BOOL getPageEnable(const int _num);
	void setPageEnable(const int _num,BOOL _on);
	BOOL getSectionEnable(const int _page,const int _section);
	BOOL *getSectionEnableAddr(const int _page,const int _section);
	CString getExtendedSectionText(const int _pageNum,const int _sectNum);
	CString getUnit(const int _pageNum,const int _sectNum);
	BOOL getUseGrid(const int _pageNum,const int _sectNum);
	BOOL *getUseGridAddr(const int _pageNum,const int _sectNum);
	CString getPressUnitStr(const int _pageNum,const int _sectNum);
	int getPressUnit(const int _pageNum,const int _sectNum);
	CString getTempUnitStr(const int _pageNum,const int _sectNum);
	int getTempUnit(const int _pageNum,const int _sectNum);
	CH_SPEC *getChSpecP(const int _pageNum,const int _sectNum);
	CH_SPEC *getChSpecP(const int _pageNum,const int _sectNum,const int _chID);
	void resetAll(void);
	
	void saveToFile(void);
	void readFromFile(void);
	double getTimeStart(const int _pageNum);
	double getTimeStop(const int _pageNum);
	void setTimeStart(const int _pageNum,const double _time);
	void setTimeStop(const int _pageNum,const double _time);
	CString getTimeStartStr(const int _pageNum);
	CString getTimeStopStr(const int _pageNum);
	double stringToTime(const CString _s);

	void forcePressUnit(const int _page,const int _section,const CString _newUnitStr);
	void forceTempUnit(const int _page,const int _section,const CString _newUnitStr);
	void forceMinMaxY(const int _page,const int _section,const int _channelID);
	void setThickness(const int _page,const int _section,const int _channelID,const int _thickness);
	void setDataType(const int _page,const int _section,const int _channelID,const UINT _typeID,const bool _on);
	void setTransparency(const int _page,const int _section,const int _channelID,const int _transparency);
	void setMinY(const int _page,const int _section,const int _channelID,const double _val);
	void setMaxY(const int _page,const int _section,const int _channelID,const double _val);
	
	int getPressUnitStringConstantFromIndex(const int _seqNum);
	int getTempUnitStringConstantFromIndex(const int _seqNum);
	bool updateToMemMap(void);
	bool updateFromMemMap(void);
	int channelIndexFromChannelID(const int _page,const int _section,const int _id);
protected:
	CESGenericMMFile *mmFile;
	void cstringToChar(CString _s,_TCHAR _ch[],int _maxLen);
	int getIndexFromPressUnitStringConstant(const int _sc);
	int getIndexFromTempUnitStringConstant(const int _sc);
	int configVersion;
	PAGE page[numberOfGraphPages];
	PAGE pagesSavedToReg[numberOfGraphPages];
	void copyPages(void);
	CString secToString(const double _secs);
	void changeChannelMaxMin(const int _oldUnit0,const int _newUnit0,const int _oldUnit1,const int _newUnit1,const int _page,const int _section,const int _ch);
	double fromDegCtoDegF(double _val);
	double fromDegFtoDegC(double _val);
	double getFactorToNewUnit(const int _newUnit);
	double getFactorToSIUnit(const int _oldUnit);
    int getPressUnitFromString(const CString _newUnitStr);
    int getTempUnitFromString(const CString _newUnitStr);
	int getChannelNumberFromIdString(const CString _str);
	COLORREF colorrefFromString(const CString _str);
	int getAcceptedThickness(const int _tryThis);
	long getAcceptedTransparency(const long _tryThis);
	bool checkAndCorrectUnits(void);
	bool checkAndCorrectMinMaxYValues(void);
	int getNumDataTypes(UINT _val);
};

class CChRegInterface : public CChStorage
{
public:
	CChRegInterface();
	~CChRegInterface();
	virtual BOOL save(const CString iniFile = _T(""),const bool _exit = false);
	virtual BOOL read(const CString iniFile = _T(""));
protected:
};


//---XML Stuff
enum {
	XML_TYPE_START_ELEMENT,		// 0
	XML_TYPE_VALUE,				// 1
	XML_TYPE_END_ELEMENT,		// 2
	XML_TYPE_XML_DECLARATION,	// 3
	XML_TYPE_WHITESPACE			// 4
};
const CString tagApneaGraphView		= _T("ApneaGraphView");
const CString tagPage				= _T("page");
const CString tagSection			= _T("section");
const CString tagTimeAxis			= _T("timeAxis");
const CString tagStart				= _T("start");
const CString tagStop 				= _T("stop");
const CString tagUseGrid 			= _T("useGrid");
const CString tagEngineeringUnits	= _T("engineeringUnits");
const CString tagTempUnit			= _T("tempUnit");
const CString tagPressUnit			= _T("pressUnit");
const CString tagChannel			= _T("channel");
const CString tagColor				= _T("color");
const CString tagThickness			= _T("thickness");
const CString tagTransparency		= _T("transparency");
const CString tagMinimumY			= _T("minimumY");
const CString tagMaximumY			= _T("maximumY");

const CString attrName				= _T("name");
const CString attrEnabled			= _T("enabled");
const CString attrId				= _T("id");
const CString attrVersion			= _T("version");

const CString xmlWriteVersion = _T("10.0");

class CChXMLInterface : public CChStorage
{
public:
	CChXMLInterface();
	~CChXMLInterface();
	virtual BOOL save(const CString _file = _T(""));
	virtual BOOL read(const CString _file = _T(""));
	void readDefaults(void);
protected:
	CString xmlVersion;
	CString defaultFile,lastUsedFile;
	bool foundInXML(bool _resetFlags,CString _string,int _type,CArray <CString,CString> *_attributes = NULL,CArray <CString,CString> *_values = NULL);
	HRESULT GetAttributes(IXmlReader* pReader,CArray <CString,CString> *_attributes,CArray <CString,CString> *_values);
	CString bitStringFromUint(const UINT _int);
	UINT uintFromBitString(const CString _s);
};


#endif