#if !defined MEMMAPPED_FILES_H_INCLUDED
#define MEMMAPPED_FILES_H_INCLUDED
//
// memMappedFiles.h
// Date: 15.8.04
// By: RKH
//
// Note: all mem mapped files and their semaphores are created by the UDP components
//		16.2.05 RKH. only one file for IO of ES data, used both on DIU and viewer
//		13.6.05 RKH Lots of stuff now over in commandMMFiles class and inhereted classes
//		04.09.06 RKH Added aFloasMission setup
//////////////////////////////////////////////////////////////////////////////////////

#define FLOAS_MISSION_SETUP_VERSION 100

//***********************************
//	Data structures 
//***********************************
typedef struct aFloasMissionSetup {
	int version;
	CString ID;
	int captureWidth,captureHeight;
	bool promptForWaypointAnnotation;
	int outputFormat;
	CString waypointFilesDir;
	BOOL dumpWPsToFile;
	int spare[9];
} FLOAS_MISSION_SETUP;

//******************************
//   2D formats
//*******************************
#ifndef FORMATS_2D_DEFINED
	#define FORMAT_2D_GIF		1
	#define FORMAT_2D_BMP		2
	#define FORMAT_2D_TIFF		3
	#define FORMAT_2D_JPEG		4
	#define FORMAT_2D_PCX		5
	#define FORMATS_2D_DEFINED 1
#endif

//*****************************
//* File Names
//*****************************

#define MF_IN_F180DATA		"mfInF180data" // DIU <- F180

#define MF_MOTION_MATRICES	"mfMotionMatrices" // For data exchange between processes on DIU

#define MF_INPUT_NAV_DATA			"mfInputNavData" // Contains the input nav data

#define MF_OBJ_EXPORT_				"mfObjExport_" // First part of object export file name
#define MF_OBJ_IMPORT_				"mfObjImport_" // First part of object import file name
#define MF_OBJ_LIST					"mfObjList" // File with list of objects and files 
#define MF_IMPOBJ_LIST				"mfImportObjList" // File with input enabled objects
#define MF_INPUT_ONE_CHNL_DATA		"mfInputOneChnlData" // File with single channel data from sonar head
#define MF_INPUT_SONAR_DATA			"mfInputSonarData" // File with data from sonar


//*****************************
//* File max sizes
//*****************************

#define MOTION_MATRICES_SIZE	2000	// Max size
#define MF_OBJ_IMPORT_SIZE		1000	// Max size
#define MF_OBJ_EXPORT_SIZE		1000	// Max size
#define MF_OBJ_LIST_SIZE		10000	// Max size 
#define INPUT_NAV_DATA_SIZE		2000    // Max size
#define INPUT_SONAR_DATA_SIZE	300000	// Max Size
#define ONE_CHNL_DATA_SIZE		80000   // Max size

//*****************************
//* Semaphore Names
//*****************************

#define SEM_MOTION_MATRICES			"semMotionMatrices" // For data exchange between processes on DIU

#define SEM_INPUT_NAV_DATA			"semInputNavData" // Protects input navigation data file

#define SEM_ES_IMPORT_TO_OBJ_FILE	"semESImportToObjFile" // Protects mem mapped file for motion import
#define SEM_ES_EXPORT_FROM_OBJ_FILE	"semESExportFromObjFile" // Protects mem mapped file for motion export

#define SEM_ES_EXPORT_LIST			"semESExportList" // Protects file with listing of exporting objects
#define SEM_ES_IMPORT_LIST			"semESImportList" // Protects file with listing of importing objects

#define SEM_INPUT_ONE_CHNL_DATA		"semInputOneChnlSonarData" // Protects single channel data file
#define SEM_INPUT_SONAR_DATA		"semInputSonarData" // Protects sonar data input file

#define SEM_SENDING_ES200_DATA		"semSendingES200Data" // Do not post message if data are being sent

//***************************************
//* Length of object names in list files
//***************************************
#define OBJ_NAMES_LIST_LENGTH		256  // Max object name + 14

//******************************
//* Messages
//******************************

#define MESS_MOTION_MATRICES		"messMotionMatrices" // For data exchange between processes on DIU

#define MESS_TIMEOUT_ON_PORT		"messTimeoutOnPort" // When timeout on a serial port has occurred

#define MESS_OBJ_IN_UPDATE			"messObjInUpdate" // When input to an object has changed
#define MESS_OBJ_OUT_UPDATE			"messObjOutUpdate" // When output from any object has changed

#endif