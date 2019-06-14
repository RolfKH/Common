#ifndef AGS_EVENTS_INCLUDED
#define AGS_EVENTS_INCLUDED


/*!
* @author		RKH
* @date		10.11.2018
* @copyright (C)Spiro Medical AS 2013 - 2018
*/

/*
	Note : 
			AHindex+RERAindex = RDI
			CENTRALIndex + MIXindex + OBSTRindex + HYPOindex (central + obstructive) = AHindex

*/

#include <vector>
#include <algorithm>
#include <numeric>
#include <utility>
#include "vectorFunctions.h"
#include <stack>
#include "OldDetectionCriteriaStructures.h"
#include "EventEnums.h"

/*
	Event			Remove when colliding with
	Note : Central, Obstr, Mix cannot collide as they are found in same function findApnea()
	------------------------------------------------------------------------------------------
	Obstr															Excluded	!inBed
	Central															Excluded	!inBed
	Mix																Excluded	!inBed
	HypoC			Obstr		Central		Mix						Excluded	!inBed		Done in findApnea()
	HypoO			Obstr		Central		Mix						Excluded	!inBed		Done in findApnea()
	RERA			Obstr		Central		Mix		Hypo	Awake	Excluded	!inBed		
	Arousal													Awake	Excluded	!inBed		Done in doRERAandArousalAnalysis()
	Awake			Obstr		Central		Mix		Hypo									Done in doAwakeAnalysis()
	Snoring													Awake	Excluded	!inBed		
	SpO2													Awake	Excluded	!inBed

Note: 
	Baseline, AASM 1999: 
			Baseline is defined as the mean amplitude of stable breathing and oxygenation
			in the two minutes preceding onset of the event (in
			individuals who have a stable breathing pattern during
			sleep) or the mean amplitude of the three largest breaths in
			the two minutes preceding onset of the event (in individuals
			without a stable breathing pattern).

	Ref:	Sleep–Related Breathing Disorders in Adults:
			Recommendations for Syndrome Definition and
			Measurement Techniques in Clinical Research.
			The Report of an American Academy of Sleep Medicine Task Force. 
			SLEEP. Vol 22, No. 5, 1999

Note that in CEvents there are results from all 6 criteria combinations. They are all saved to disk.
All public functions act according to the current detection criteria.
All protected functions need to have the hypopnea and SpO2 criteria specified.
*/

#ifdef _DEBUG
#define new DEBUG_NEW
#endif

//---Actions for do and undo stacks
#define ACTION_NONE							0
#define ACTION_ADD_EVENT					1
#define ACTION_DELETE_EVENT					2
#define ACTION_CHANGE_START_STOP			3
#define ACTION_CHANGE_EXCLUDE_WINDOW		4

//---Events file version
/*
	Version 7. Used with SW ver 4.0
	Version 8. Used with SW ver 4.1 and above. 
				Included t0EnvPercentStabilityLimit as I am now calling setFlatAreasToZero for T0 also
				New awake parameter - above baseline value (actimeterAboveBaselineLimit)
	Version 9.  Added flowIncreaseAtArousal to detection criteria
	Version 10. Changed time gap between hypopnea and desaturation as flow starts to decrease earlier than it reaches 70% or 60%
	Version 11. Not used
	Version 12. Added swallow events
	Version 13. Split hypopnea events into drop30, drop40 and drop50
	Version 14. Changed detection criteria from hypoponea limit fraction (0.7, 0.6, 0.5) to droplimit (30,40,50)
	Version 15. Changed SpO2 drop limit from float (3.0/4.0) to enum (int)
	Version 16. Analysis done for all 6 criteria combinations (spO2 drop limits vs hypopnea limitand saved. 
	Version 17. Arousals and RERAs also depend on criteria
	Version 18. RERA is now child of Arousal
	Version 19. Added parameter angleCoeffObstrVsCentralHypo = ANGLE_COEFF_OBSTR_VS_CENTRAL_HYPO
	Version 20. Changed awake detection parameter GI
	Version 21. Added factors for advanced awake detection
	Version 22. Added more factors for advanced awake detection
	Version 23. Added more factors for advanced awake detection
	Version 24. Added torsoActimeter limit
	Version 25. Added imported events
	Version 26. Body pos result includes sleepTime in positions plus RERAi,OAHi,CAHi,ODI per position
	Version 27. Body pos result now includes awake time in positions.
	Version 28. Added factor to balance the use of T0 vs T1 for flow detection (flowDetectBalanceFactor)
	Version 29. Added analysis software version string (to keep track of who analysed the data)
	Version 30. Each snoring event has three additional events start/stop vectors (for lower, multi and upper)
				Added RE during snoring and corresponding data arrays
	Version 31. Added mean and stdDev for snoring at different levels, both for airmic and contactmic
	Version 32. PR results now split in awake,asleep and overall
	Version 33. PPH-data now sent to all events
	Version 34. PPH data listed with RE and RESnoring. 
				PPH and POES data output to CSV files for snoring and all over
	Version 35. Removed eventTimeLineTimeVector
				Added snoringVector
				Removed snoring array
	Version 36. Added computaion of 
					Indexes vs position
						OAindex[posCount]
						OHindex[posCount]
						RDindex[posCount]
						AHindex[posCount]
					Indexes vs level
						RERAindexVSlevel[levelTypeCount]
						OAHIindexVSlevel[levelTypeCount]
						CAHIindexVSlevel[levelTypeCount]
						ODindexVSlevel[levelTypeCount]
						OAindexVSlevel[levelTypeCount]
						OHindexVSlevel[levelTypeCount]
						AHindexVSlevel[levelTypeCount]


	Used in releases:
		SW version		Events file version
		------------------------------------
		First			7.0
		4.0				10.0
		4.5.1			20.0
		5.6.1			36
*/

#define EVENTS_BIN_MAJOR_VERSION		36
#define EVENTS_BIN_MINOR_VERSION		0

const CString reportVersionXML50 = _T("XMLdata50.xml"); // For version 20 of events.dta file, 
														// Version 4.5.1 of of analysis sw
const CString reportVersionXML60 = _T("XMLdata60.xml"); // For version 32 and above of events.dta file, 
														// Version 4.5.7 and above of of analysis sw
const CString reportVersionXML61 = _T("XMLdata61.xml"); // For version 32 and above of events.dta file, 
														// Version 5.2.2 and above of of analysis sw
const CString reportVersionXML62 = _T("XMLdata62.xml"); // For version 32 and above of events.dta file, 
														// Version 5.6.0 and above of of analysis sw

const CString reportVersionPDF50 = _T("Report50.pdf"); // For version 20 of events.dta file, 
														// Version 4.5.1 of of analysis sw
const CString reportVersionPDF60 = _T("Report60.pdf"); // For version 32 and above of events.dta file, 
														// Version 4.5.7 and above of of analysis sw
const CString reportVersionPDF61 = _T("Report61.pdf"); // For version 32 and above of events.dta file, 
													   // Version 5.2.2 and above of of analysis sw
const CString reportVersionPDF62 = _T("Report62.pdf"); // For version 32 and above of events.dta file, 
													   // Version 5.2.2 and above of of analysis sw
const CString reportVersionShortPDF10 = _T("ShortReport10.pdf");	// For version 32 and above of events.dta file, 
																	// Version 5.6.0 and above of of analysis sw

//---Current versions
const CString reportVersionPDFcurrent = reportVersionPDF62;
const CString reportVersionShortPDFcurrent = reportVersionShortPDF10;

//--A handy structure
typedef struct anEventWithLevel {
	int type;
	bool withLevel;
} EVENT_TYPE_AND_LEVEL;

//---Flags used for events file
//NOT USED ANYMORE---  #define EVENT_TYPE_NONE					0x0000
//NOT USED ANYMORE---  #define EVENT_TYPE_AWAKE					0x0001
//NOT USED ANYMORE---  #define EVENT_TYPE_SNORING				0x0002
//NOT USED ANYMORE---  #define EVENT_TYPE_SWALLOW				0x0004
//NOT USED ANYMORE---  #define EVENT_TYPE_RESPIRATORY			0x0008
//NOT USED ANYMORE---  #define EVENT_TYPE_AROUSAL				0x0010

#define EVENTS_FLAG_NONE						0x0000
#define EVENTS_FLAG_DO_AWAKE					0x0001
#define EVENTS_FLAG_CATHETER_BASED				0x0002
#define EVENTS_FLAG_BELTS_AND_CANNULA_BASED		0x0004

#define EVENT_TIMELINE_TIME_INTERVAL				0.1f	// s
#define EVENT_TIMELINE_TIME_INTERVAL_INVERSE		10.0f	// s-1  Multiply is faster than divide

//---default lengths for adding events visually
const float ahDefaultLength				= 10.0f;
const float reraDefaultLength			= 15.0f;	//
const float reDefaultLength				= 10.0f;	//  reraLength = reLength + arousalLength
const float arousalDefaultLength		= 5.0f;		//
const float swallowDefaultLength		= 5.0f;
const float importedDefaultLength		= 10.0f;
const float manualDefaultLength			= .0f;
const float percentageOfView			= .1f; // 10%

//---Registry keys and nodes
const CString regSectionCriteria					= _T("Detection Criteria");
const CString regSpO2DropLimit						= _T("SpO2 drop limit");
const CString regSplitCentralAndObstrHypo			= _T("Split central and obstr hypopnea");
const CString regPOESalarmLine						= _T("POES alarm lEVEL");
const CString regLevelDetectionMargin				= _T("Level detection margin");
const CString regApneaDefaultDuration				= _T("Apnea default duration");
// Old code : const CString regHypopneaLimitFraction				= _T("Hypopnea limit fraction");
const CString regHypopneaDroplimit					= _T("Hypopnea detection drop limit");
const CString regApneaShortCentralDuration			= _T("Apnea short central duration");
const CString regApneaShortObstrDuration			= _T("Apnea short obstructive duration");
const CString regMaxApneaDur						= _T("Max apnea duration");
const CString regMaxHypopneaDuration				= _T("Max hypopnea duration");
const CString regtimeInBedLowLimit					= _T("Time in bed low limit");
const CString regExtendedWindowForExclusionOfAwake	= _T("Extended window for exclusion of awake");
const CString regActimeterAlwaysAwakeLimit			= _T("Actimeter always awake limit");
const CString regActimeterRelativeAwakeLimit		= _T("Actimeter relative awake limit");
const CString regRespEffortFrqPeakWindowLength		= _T("Respiratory effort frequency peak detection window length");
const CString regAdmittancePeakWindowLength			= _T("Admittance peak detection window length");
const CString regPulseRatePeakWindowLength			= _T("Pulse rate peak detection window length");
const CString regPulseRateWindowSlack				= _T("Pulse rate window slack");
const CString regArousalDetectionTimeStep			= _T("Arousal detection time step");
const CString regRespFrqPeakFactor					= _T("Respiration effort frequency peak detection factor");
const CString regAdmittancePeakFactorHead			= _T("Admittance peak detection head factor");
const CString regAdmittancePeakFactorTail			= _T("Admittance peak detection tail factor");
const CString regPulseRatePeakIncrease				= _T("Pulse rate peak increase");
const CString regReIncreaseDuration					= _T("Respiratory effort increase duration");
const CString regReSeekSlack						= _T("Respiratory effort increase seek slack");
const CString regReSeekStep							= _T("Respiratory effort increase seek step");
const CString regContactMicSnoringThreshold			= _T("Contact mic snoring threshold");
const CString regLinRegRSquaredLimit				= _T("Linear regression R squared limit");
const CString regSnoringAverageWindowLength			= _T("Snoring average window length");
const CString regBaselineLength						= _T("Baseline length");
const CString regSpO2stabilityLimit					= _T("SpO2 stability limit");
const CString regT0EnvPercentStabilityLimit			= _T("T0 env percent stability limit");
const CString regT1EnvPercentStabilityLimit			= _T("T1 env percent stability limit");
const CString regPressureEnvPercentStabilityLimit	= _T("Pressure env percent stability limit");

const CString regAbdomEnvPercentStabilityLimit		= _T("Abdom env percent stability limit");
const CString regChestEnvPercentStabilityLimit		= _T("Chest env percent stability limit");
const CString regCannulaEnvPercentStabilityLimit	= _T("Cannula env percent stability limit");
const CString regBeltSumPercentStabilityLimit		= _T("Belt sum percent stability limit");

const CString regFlowStopFraction					= _T("Flow stop fraction");
const CString regPressureDeadFraction				= _T("Pressure dead fraction");	
const CString regSpO2DropSeekWindowLength			= _T("Seek time window length to find SpO2 drops associated with hypopnea");
const CString regSpO2DropSeekWindowStart			= _T("Seek time window start to find SpO2 drops associated with hypopnea");
const CString regMaxEventLength						= _T("Maximum event length");
const CString regGradientUpperLimit					= _T("Gradient upper limit");
const CString regGradientLowerLimit					= _T("Gradient lower limit");
const CString regHalfArousalLength					= _T("Half arousal length");
const CString regP0RiseCoefficientLimit				= _T("P0 rise coefficient limit");
const CString regFlowIncreaseAtArousal				= _T("Flow increase at arousal");
const CString regAngleCoeffObstrVsCentralHypo		= _T("POESEnv Angle coefficient to separate hypopnea types");
const CString regAwakeMixeFactor					= _T("Awake Mixe Factor");
const CString regAwakeObstFactor					= _T("Awake Obst Factor");
const CString regAwakeCentFactor					= _T("Awake Cent Factor");
const CString regAwakeHypoFactor					= _T("Awake Hypo Factor");
const CString regAwakeDesaFactor					= _T("Awake Desa Factor");
const CString regAwakeRERAFactor					= _T("Awake RERA Factor");
const CString regAwakeSwalFactor					= _T("Awake Swal Factor");
const CString regAwakeSnorFactor					= _T("Awake Snor Factor");
const CString regAwakeActiFactor					= _T("Awake Actimeter Factor");
const CString regAwakeTActFactor					= _T("Awake Torso Actimeter Factor");
const CString regAwakeStanFactor					= _T("Awake Standing Factor");
const CString regTorsoActimeterLimit				= _T("Torso Actimeter Limit");
const CString regFlowDetectBalanceFactor			= _T("Flow detection T0 T1 balance factor");

const CString regFiles								= _T("Files");
const CString regProgramDataFolder					= _T("Program data folder");
const CString regProgramFolder						= _T("Program folder");
const CString regFOPPath							= _T("FOP path");
const CString regFOPCommand							= _T("FOP command");
const CString regStdXSLfile							= _T("StdXSLfile");

//const CString regStdXSLfile60						= _T("StdXSLfile60");			// For version 60 - no images or notes
//const CString regStdXSLfile60Img					= _T("StdXSLfile60Img");		// For version 60 - with images
//const CString regStdXSLfile60Notes					= _T("StdXSLfile60Notes");		// For version 60 - with notes
//const CString regStdXSLfile60ImgNotes				= _T("StdXSLfile60ImgNotes");	// For version 60 - with images and notes

const CString regShortXSLfile10						= _T("ShortXSLFile10");			// Vero versoion 10 - short report

const CString regStdXSLfile61						= _T("StdXSLfile61");			// For version 61 - no images or notes
const CString regStdXSLfile61Img					= _T("StdXSLfile61Img");		// For version 61 - with images
const CString regStdXSLfile61Notes					= _T("StdXSLfile61Notes");		// For version 61 - with notes
const CString regStdXSLfile61ImgNotes				= _T("StdXSLfile61ImgNotes");	// For version 61 - with images and notes

const CString regStdXSLfile62						= _T("StdXSLfile62");			// For version 62 - no images or notes
const CString regStdXSLfile62Img					= _T("StdXSLfile62Img");		// For version 62 - with images
const CString regStdXSLfile62Notes					= _T("StdXSLfile62Notes");		// For version 62 - with notes
const CString regStdXSLfile62ImgNotes				= _T("StdXSLfile62ImgNotes");	// For version 62 - with images and notes

const CString regClinicAddressPath					= _T("ClinicAddressPath");
const CString regClinicLogoPath						= _T("ClinicLogoPath");
const CString regSpiroLogoPath						= _T("SpiroLogoPath");
//const CString regTempFolder							= _T("TempFolder");

const int closestIsLeft = 1;
const int closestIsRight = 2;

const int ppBins				= 100;						// cmH2O
const int ppHistoBinStep		= 5;						// cmH2O
const int ppHistoBins			= ppBins / ppHistoBinStep;	// cmH2O
const int ppBinNumberAbove_15	= 3;						// cmH2O

using namespace std;
typedef bool(*foo) (UINT);
typedef UINT(*foouint) (UINT);

typedef struct aVisualEvent {
	UINT type, level;
	UINT gripWhat, gripWhere;
	float from, to;
} VISUAL_EVENT;


// The function object to determine the average  PR and SpO2
class AverageOximetry
{
private:
	long num;      // The number of elements  
	double sumOx;      // The sum of the elements  
	double sumPR;
public:
	// Constructor initializes the value to multiply by  
	AverageOximetry() : num(0), sumOx(.0f),sumPR(.0f)
	{
	}

	// The function call to process the next elment  
	void operator ( ) (OXIMETER_SET elem) \
	{
		num++;      // Increment the element count  
		sumOx += (double) elem.spO2;   // Add the value to the partial sum  
		sumPR += (double) elem.pr;
	}

	// return Average  
	operator OXIMETER_SET()
	{
		OXIMETER_SET set;
		set.pr = (0 == num) ? .0f : (float)(sumPR / (double) num);
		set.spO2 = (0 == num) ? .0f : (float)(sumOx / (double)num);
		set.state = 0;
		return set;
	}
};


//const UINT evntTypeHypo			= 0x00000008;
const UINT evntTypeAH				= 0x00000037;  // Mix,Hypo Obstr, Obstr, Central, HypoCentral
const UINT evntTypeAHandRERA		= 0x00000077;  // RERA, Mix,Hypo Obstr, Obstr, Central, HypoCentral
const UINT evntTypeAllWithLevel		= 0x00000465;  // RERA, Mix,Hypo Obstr, Obstr, snoring
const UINT evntTypeAllWithoutLevel	= 0x00019B92;  // Central, Hypo Central, Arousal, SpO2Drop,ManMarker,Awake, Excluded, Swallow, Imported
const UINT evntTypeVisualEditableEvents = 0x00009D77; // Central, Mix, Obstr, Hypo Obstr, Hypo Central, SpO2Drop, Awake, Excluded, RERA, Snoring, Swallow
const UINT evntTypeNone				= 0x00000000;
const UINT evntTypeMixed			= 0x00000001;
const UINT evntTypeCentral			= 0x00000002;
const UINT evntTypeObstr			= 0x00000004;
const UINT evntTypeHypoCentral		= 0x00000010;
const UINT evntTypeHypoObstr		= 0x00000020;
const UINT evntTypeRERA				= 0x00000040;
const UINT evntTypeArousal			= 0x00000080;
const UINT evntTypeSpO2Drop			= 0x00000100;
const UINT evntTypeManMarker		= 0x00000200;
const UINT evntTypeSnoring			= 0x00000400;
const UINT evntTypeAwake			= 0x00000800;
const UINT evntTypeExcluded			= 0x00001000;
const UINT evntTypeCursorWnd		= 0x00002000;
const UINT evntTypeStartStop		= 0x00004000;
const UINT evntTypeSwallow			= 0x00008000;
const UINT evntTypeImported			= 0x00010000;
const UINT evntTypeInvalidOximetry	= 0x00020000;

const UINT evntAHLevelUpper			= 0x00100000;
const UINT evntAHLevelMulti			= 0x00200000;
const UINT evntAHLevelLower			= 0x00400000;
const UINT evntAHLevelUndef			= 0x00800000;

const UINT evntSnoringLevelUpper	= 0x01000000;
const UINT evntSnoringLevelMulti	= 0x02000000;
const UINT evntSnoringLevelLower	= 0x04000000;
const UINT evntSnoringLevelUndef	= 0x08000000;

const UINT bodyPosSwitchLeft		= 0x10000000;
const UINT bodyPosSwitchRight		= 0x20000000;
const UINT bodyPosSwitchProne		= 0x40000000;
const UINT bodyPosSwitchSupine		= 0x80000000;
const UINT bodyPosSwitchUpright		= 0x00040000;

const UINT levelTypeNone			= 0x00000000;
const UINT evntTypeAll				= 0x000FFFFF;
const UINT evntAHLevelAll			= 0x00F00000;
const UINT evntSnoringLevelAll		= 0x0F000000;
const UINT evntTypeAwakeOrExcluded = 0x00001800;

//---For awake detection
const float densityTimeStepSize				= 5.0f;		// 5 seconds
const int numSamplesForEventDensityWindow	= 60;		// 60 * 5 = 300 sec = 5 min
														// Note that (see vectorFuncions.h) :
														//---Hanning window for actimetry analysis.
														//		based on 30 sec epochs
														//		5 minutes long
														//		11 elements

const CString eventsDataFile = _T("events.dta");

const int minVectorLengthForAnalysis = 20;

#define SAO2_HIST_INTERVAL_SIZE			1.0f
#define NUM_SAO2_HIST_INTERVALS			40

//------Detection parameters
#define MINIMUM_PRESS_STD_DEV_FRACTION		4
#define APNEA_DEFAULT_DURATION				10.0f		// sec
#define RE_INCREASE_DEFAULT_DURATION		7.0f		// sec
#define RE_INCREASE_SEEK_STEP				.5f			// sec
#define RE_INCREASE_SEEK_SLACK				20.0f		// sec - was 10
#define LIN_REG_R_SQUARED_LIMIT				0.3f		// Used for RERA detection 
#define P0_RISE_COEFFICIENTLIMIT			.5f			// Used for RERA detection

#define APNEA_HALF_DURATION					5.0f		// sec
#define MAX_APNEA_DUR						180			// was 60 sec - 180 sec has been tested
#define MAX_HYPOPNEA_DUR					180			// was 60 sec - 180 sec has been tested
#define DEF_TIME_IN_BED_LOW_LIMIT			300			// 5 minutes
#define BASELINE_LENGTH						120.0f		// 2 minutes
#define BASELINE_LENGTH_SAMPLES				480			// 2 minutes, assuming 0.25 s sample rate

#define PP_DETECT_WINDOW					40			// 10 seconds, assuming 0.25 s sample rate

#define LEVEL_DETECTION_MARGIN				0.9f		// max *0.9 > second max
#define SPO2_STABILITY_LIMIT				2.0f		// +/- 2 % oxygen saturation
#define T0T1ENV_PERCENT_STABILITY_LIMIT		10.0f		// +/- 10% of median
#define PRESSENV_PERCENT_STABILITY_LIMIT	10.0f		// +/- 10% of median

#define BELTENV_PERCENT_STABILITY_LIMIT		10.0f		// +/- 10% of median
#define CANNULAENV_PERCENT_STABILITY_LIMIT	10.0f		// +/- 10% of median
#define BELT_SUM_PERCENT_STABILITY_LIMIT	10.0f		// +/- 10% of median

#define HYPO_LIMIT_FRACTION					0.7f		// 30% reduction compared to baseline
#define FLOW_STOP_FRACTION					0.1f		// 90% reduction compared to baseline
#define PRESSURE_DEAD_FRACTION				0.1f		// 90% reduction compared to baseline
#define WINDOW_LENGTH_FOR_SPO2DROP_SEEK		30.0f		// Time window length from start of event to start of SpO2 drop
#define WINDOW_START_FOR_SPO2DROP_SEEK		- 7.0f		// Min time from start of event to start of SpO2 drop Was -3
#define MAX_EVENT_LENGTH					210.0f		// 3.5 min

#define ANGLE_COEFF_OBSTR_VS_CENTRAL_HYPO	- .2f		// The b from lin reg on POESEnv.

#define LOW_FLOW_LIMIT50					0.5f		// 50% reduction
#define LOW_FLOW_LIMIT40					0.6f		// 40% reduction
#define LOW_FLOW_LIMIT30					0.7f		// 30% reduction

//---For awake detection 
#define TORSO_ACTIMETER_LIMIT				10.0f	// Old values pre 4.5.17 was 10.0f	 // Torso angle (dB) limit
#define AWAKE_MIXE_FACTOR					1.0f	// Old values pre 4.5.17 was 2.0f	 // Favoures SLEEP
#define AWAKE_OBST_FACTOR					1.0f	// Old values pre 4.5.17 was 2.0f	 // Favoures SLEEP
#define AWAKE_CENT_FACTOR					1.0f	// Old values pre 4.5.17 was 2.0f	 // Favoures SLEEP
#define AWAKE_HYPO_FACTOR					0.0f	// Old values pre 4.5.17 was 1.0f	 // Favoures SLEEP
#define AWAKE_DESA_FACTOR					10.0f   // Old values pre 4.5.17 was 1.0f	 // Favoures SLEEP
#define AWAKE_RERA_FACTOR					0.0f	// Old values pre 4.5.17 was 2.0f	 // Favoures SLEEP
#define AWAKE_SNOR_FACTOR					1.0f	// Old values pre 4.5.17 was 1.0f	 // Favoures SLEEP
#define AWAKE_SWAL_FACTOR					1.0f	// Old values pre 4.5.17 was 10.0f	 // Favoures AWAKE
#define AWAKE_ACTI_FACTOR					1.0f	// Old values pre 4.5.17 was 1.0f	 // Favoures AWAKE
#define AWAKE_TACT_FACTOR					0.0f	// Old values pre 4.5.17 was 1.0f	 // Favoures AWAKE
#define AWAKE_STAN_FACTOR					10.0f	// Old values pre 4.5.17 was 10.0f	 // Favoures AWAKE

//---For arousal detection
//	Sequence: 
//				An admittance peak
//				Followed by a pulse rate peak. Typical delay 30 seconds from peak admittance
#define RESP_EFF_FRQ_PEAK_WINDOW_LENGTH		30.0f		// seconds. 
														// Use this window length to search for resp effort frequency peaks (arousals)
#define ADMITTANCE_PEAK_WINDOW_LENGTH		20.0f		// seconds. was 40, then 20
														// Use this window length to search for admittance peaks (arousals)
#define HALF_AROUSAL_LENGTH					3.0f		// Use double of this to mark an arousal
#define PULSERATE_PEAK_WINDOW_LENGTH		30.0f		// seconds. From  Admittance peak
														// Use this window length to search for pulse rate peaks (arousals)
#define PULSERATE_WINDOW_SLACK				5.0f		// Start this length earlier to seek for pulse rate increase
#define AROUSAL_DETECTION_TIME_STEP			1.0f		//	Seconds
														// The detection windows slide this amount searching for peaks
#define RESP_FRQ_PEAK_FACTOR				1.05f		// Peak must be so much higher than highest endpoint in window
#define ADMITTANCE_PEAK_FACTOR_HEAD			2.0f		// Peak must be so much higher than startpoint in window. was 4, then 2.75, then 2.0
#define ADMITTANCE_PEAK_FACTOR_TAIL			1.1f		// Peak must be so much higher than endpoint in window. was 4, then 1.25, then 1.5, then 1.1
#define PULSERATE_PEAK_INCREASE				.02f		// Peak must be 5% higher than start
#define FLOW_INCREASE_AT_AROUSAL			5			// % increase

//---For snoring
#define CONTACT_MIC_SNORING_THRESHOLD			2.0f		// Fits file 7028 AHUS PSG/NOX
#define SNORING_AVERAGE_WINDOW_LENGTH			60.0f		// Fits file 7028 AHUS PSG/NOX (in seconds)

#define POES_ALARM_LINE							10.0f		// cmH2O
#define SPO2_DROP_LIMIT							3.5f		// 4 %
#define SPO2_LOW_OUTLIER_LIMIT					10.0f		// Allow SpO2 values of 10% below median to pass always. 

#define ACTIMETER_SATURATION_LIMIT				100.0f		// Saturation limit for the actimeter signal
#define ACTIMETER_ALWAYS_AWAKE_LIMIT			20.0f		// Always awake if actimeter is above this limit NOT USING THIS IN 4.1 and onwards
#define ACTIMETER_ABOVE_BASELINE_LIMIT			18.0f		// Awake if this amount above baseline	(was 8.0 in 
#define EXTENDED_WINDOW_FOR_EXCLUSION_OF_AWAKE	20.0f		// If an event (for example SpO2) occurs, say that
															// the patient sleeps at this time - extended with 
															// EXTENDED_WINDOW_FOR_EXCLUSION_OF_AWAKE in each direction
#define GRADIENT_UPPER_LIMIT				60.0f			// %
#define GRADIENT_LOWER_LIMIT				40.0f			// %

//---Flags used to help identify apneic events
const unsigned short zeroFlow			= 0x0001; 
const unsigned short deadPoes			= 0x0002;
const unsigned short deadPph			= 0x0004;
const unsigned short lowFlow50			= 0x0008;
const unsigned short lowFlow40			= 0x0010;
const unsigned short lowFlow30			= 0x0020;
const unsigned short decrPoes			= 0x0040;

const float centralCol[]	= {	0.5f,	0.0f,	0.0f};
const float mixedCol[]		= {	0.0f,	0.5f,	0.0f};
const float obstrCol[]		= {	0.0f,	0.0f,	0.5f};
const float obstrHypCol[]	= {	0.5f,	0.5f,	0.0f};
const float centrHypCol[]	= {	0.5f,	0.0f,	0.5f};
const float spO2Col[]		= {	0.0f,	0.5f,	0.5f};
const float reraCol[]		= {	0.5f,	0.5f,	0.0f};
const float arousalCol[]	= {	0.8f,	0.4f,	0.2f};
const float awakeCol[]		= {	0.2f,	0.4f,	0.8f};
const float snoringCol[]	= {	0.4f,	0.8f,	0.2f};
const float manualCol[]		= {	0.2f,	0.2f,	0.2f};
const float swallowCol[]	= { 0.0f,	0.0f,	0.0f};
const float importedCol[]	= { 1.0f,	0.0f,	0.0f};
const float excludedCol[]	= { .0f,	0.0f,	0.0f };

enum {
	hypopneaDropLimit50,
	hypopneaDropLimit40,
	hypopneaDropLimit30,
	numHypopneaDropLimits,
	noHypopneaDropLimit
};

enum {
	spO2DropLimit3,
	spO2DropLimit4,
	numSpO2DropLimits,
	noSpO2DropLimit
};

const float spO2TestLimit4 = 3.5f;
const float spO2TestLimit3 = 2.5f;

struct DETECTION_CRITERIA {	 // Ver 29.0
	int spO2DropLimit;
	BOOL splitCentralAndObstrHypo;
	float pOESalarmLine;
	float levelDetectionMargin;
	float apneaDefaultDuration;
	int hypopneaDropLimit;
	float apneaShortCentralDuration;
	float apneaShortObstrDuration;
	float maxApneaDur;
	float maxHypopneaDuration;
	float timeInBedLowLimit;
	float extendedWindowForExclusionOfAwake;
	float actimeterAlwaysAwakeLimit; // Not using this, 4.1 and onwards !!
	float actimeterAboveBaselineLimit;    
	float respiratoryEffortFrqPeakWindowLength;
	float admittancePeakWindowLength;
	float pulseRatePeakWindowLength;
	float pulseRateWindowSlack;
	float arousalDetectionTimeStep;
	float respFrqPeakFactor;
	float admittancePeakFactorHead;	
	float admittancePeakFactorTail;	
	float pulseRatePeakIncrease;	
	float REIncreaseDuration;
	float RESeekStepSize;
	float RESeekSlack;
	float contactMicSnoringThreshold;
	float linRegRSquaredLimit;
	float snoringAverageWindowLength;
	float baselineLength;
	float spO2stabilityLimit; 
	float t0EnvPercentStabilityLimit;
	float t1EnvPercentStabilityLimit;
	float pressureEnvPercentStabilityLimit;	float flowStopFraction;
	float pressureDeadFraction;	
	float spO2DropSeekWindowLength;
	float spO2DropSeekWindowStart;
	float maxEventLength;
	float gradientUpperLimit;
	float gradientLowerLimit;
	float halfArousalLength;
	float p0RiseCoefficientLimit;
	float flowIncreaseAtArousal; // %
	float angleCoeffObstrVsCentralHypo; 
	float torsoActimeterLimit;
	float awakeMixeFactor;
	float awakeObstFactor;
	float awakeCentFactor;
	float awakeHypoFactor;
	float awakeDesaFactor;
	float awakeRERAFactor;
	float awakeSnorFactor;
	float awakeSwalFactor;
	float awakeActiFactor;
	float awakeTActFactor;
	float awakeStanFactor;
	float flowDetectBalanceFactor;
};

//---Levels
enum levelTypes {
	levelTypeUpper,
	levelTypeLower,
	levelTypeMulti,
	levelTypeUndef,
	levelTypeSum,
	levelTypeCount
};

enum evntDetectSource {
	manualDetection,
	autoDetection
};

enum hypopneaTypes {
	hypoTypeObstr	,
	hypoTypeCentral	,
	hypoTypeSum		,
	hypoTypeCount
};

enum sleepAwakeStates {
	asleep,
	awake,
	overall,
	numSleepAwakeStates
};

struct PULSE_RATE_RESULTS
{
    float satInval;					// Number of seconds invalid data
	float average;
	float median;
	float highest;
	float lowest;
	float timeOfHighest;
	float timeOfLowest;
	float histo[pulseBinCount];				// Number of seconds in the pulse rate interval
	float percentHisto[pulseBinCount];		// Percentage in the pulse rate interval
};

enum obstrEvents {
	obstrEvntMix,
	obstrEvntObstr,
	obstrEvntHypo,
	obstrEvntRERA,
	obstrEvntSnoring,
	obstrEvntSum,
	obstrEvntCount
};

enum centralEvents {
	centralEvntCentral,
	centralEvntHypo,
	centralEvntSum,
	centralEvntCount
};

enum swappableEvents {
	swapMix,
	swapCentral,
	swapObstr,
	swapObstrHypo,
	swapCentralHypo,
	swapImported,
	swapNone
};

struct SNORING_RESULT {
	float timeAtLevel[levelTypeCount];
	float percentTimeAtLevel[levelTypeCount];
	float timeInPos[posCount];
	float percentTimeInPos[posCount];
	float totalSnoringTime;
	float percentSnoring;

	float meanAmLower;
	float meanAmMulti;
	float meanAmUpper;
	float meanCmLower;
	float meanCmMulti;
	float meanCmUpper;

	float standardDevAmLower;
	float standardDevAmMulti;
	float standardDevAmUpper;
	float standardDevCmLower;
	float standardDevCmMulti;
	float standardDevCmUpper;
};

struct SPO2_RESULTS
{
	float satHisto[satBinCount];			// Number of seconds in the saturation interval
	float percentSatHisto[satBinCount];		// Number of seconds in the saturation interval
	float satAvg;							// Average saturation
	float satMedian;						// Median saturation 
	float satLowest;						// Lowest saturation
	float satHighest;						// Highest saturation
	float timeOfSatHighest;					// Time of Lowest saturation
	float timeOfSatLowest;					// Time of Highest saturation
	int fallsHisto[satBinCount];			// Histogram. Number of drops in each level interval - using the START level
	float percentFallsHisto[satBinCount];	// Histogram. Number of drops in each level interval - using the START level
	double oxygenDesaturationIndex;			// Number of desaturations per hour
	int numDrops;
	float minutesBelowSat90;				// Minutes below 90%
	float percentBelowSat90;				// Percent of sleep time below 90%
    //---Histo sequence : 95-100,90-95,85-90,80-85,75-80,70-75,65-70,<65
};

#define EVNT_DEF_AUTO					0x0001
#define EVNT_DEF_MANUAL					0x0002
#define EVNT_DEF_MANUAL_LENGTH			0x0004
#define EVNT_DEF_MANUAL_TYPE			0x0008
#define EVNT_DEF_MANUAL_LEVEL			0x0010
#define EVNT_DEF_MANUAL_DELETE			0x0020
#define EVNT_DEF_IN_EXCLUDED_WINDOW		0x0040
#define EVNT_DEF_IN_AWAKE_WINDOW		0x0080
//#define EVNT_DEF_IN_CONFLICT_WITH_HYPO	0x0100

struct LEVEL_RESULTS
{
	int num[evCountWithLevel][levelTypeCount];
	int totalNum[levelTypeCount];
	float percentTotalNum[levelTypeCount];
	float perHour[evCountWithLevel][levelTypeCount];
	float totalPerHour[levelTypeCount];
};

/*
Respiratory distress results
These are:
	Mixed apnea
	Central Apnea
	Obstructive apnea
	Obstructive hypopnea
	Central hypopnea
	RERA

Thsi structure holds 
	number of events, 
	the event index,
	total duration,
	maximum event duration,
	average event duration,
	median event duration.
*/
struct RD_RESULTS 
{
	int obstrNum[obstrEvntCount];
	int centrNum[centralEvntCount];
	
	float obstrPerHour[obstrEvntCount];
	float centrPerHour[centralEvntCount];
	
	float obstrTotalDuration[obstrEvntCount];
	float centrTotalDuration[centralEvntCount];
	
	float obstrMaxDuration[obstrEvntCount];
	float centrMaxDuration[centralEvntCount];
	
	float obstrAverageDuration[obstrEvntCount];
	float centrAverageDuration[centralEvntCount];
	
	float obstrMedianDuration[obstrEvntCount];
	float centrMedianDuration[centralEvntCount];
};


struct BODY_POS_RESULTS {
	int central[posCount];
	int spO2[posCount];
	int centralHyp[posCount];
	float time[posCount];
	float timeSleeping[posCount];
	float timeAwake[posCount];
	int obstr[posCount][levelTypeCount];
	int mix[posCount][levelTypeCount];
	int obstrHyp[posCount][levelTypeCount];
	int RERA[posCount][levelTypeCount];
	int arousal[posCount];
	float snoringTime[posCount];
	float percentTime[posCount];
	float percentTimeSleeping[posCount];
	float percentTimeAwake[posCount];

	float percentObstrSupine[levelTypeCount];
	float percentObstrNonSupine[levelTypeCount];
	float percentMixSupine[levelTypeCount];
	float percentMixNonSupine[levelTypeCount];
	float percentObstrHypoSupine[levelTypeCount];
	float percentObstrHypoNonSupine[levelTypeCount];
	float percentRERASupine[levelTypeCount];
	float percentRERANonSupine[levelTypeCount];
	float percentCentralSupine;
	float percentCentralNonSupine;
	float percentCentralHypoSupine;
	float percentCentralHypoNonSupine;
	float percentSpO2Supine;
	float percentSpO2NonSupine;
	float percentArousalSupine;
	float percentArousalNonSupine;

	//---Indexes vs position
	float RERAindex[posCount];
	float OAHindex[posCount];
	float CAHindex[posCount];
	float ODindex[posCount];
	float OAindex[posCount]	; // New in version 36
	float OHindex[posCount]	; // New in version 36
	float RDindex[posCount]	; // New in version 36
	float AHindex[posCount]	; // New in version 36
	
	//---Indexes vs level
	float RERAindexVSlevel[levelTypeCount] ;   // New in version 36
	float OAHindexVSlevel[levelTypeCount] ;	   // New in version 36
	float OAindexVSlevel[levelTypeCount]   ;   // New in version 36
	float OHindexVSlevel[levelTypeCount]   ;   // New in version 36
	float RDindexVSlevel[levelTypeCount]   ;   // New in version 36
	float AHindexVSlevel[levelTypeCount]   ;   // New in version 36
};

struct BODY_POS_RESULTS_PREVER_36 {
	int central[posCount];
	int spO2[posCount];
	int centralHyp[posCount];
	float time[posCount];
	float timeSleeping[posCount];
	float timeAwake[posCount];
	int obstr[posCount][levelTypeCount];
	int mix[posCount][levelTypeCount];
	int obstrHyp[posCount][levelTypeCount];
	int RERA[posCount][levelTypeCount];
	int arousal[posCount];
	float snoringTime[posCount];
	float percentTime[posCount];
	float percentTimeSleeping[posCount];
	float percentTimeAwake[posCount];

	float percentObstrSupine[levelTypeCount];
	float percentObstrNonSupine[levelTypeCount];
	float percentMixSupine[levelTypeCount];
	float percentMixNonSupine[levelTypeCount];
	float percentObstrHypoSupine[levelTypeCount];
	float percentObstrHypoNonSupine[levelTypeCount];
	float percentRERASupine[levelTypeCount];
	float percentRERANonSupine[levelTypeCount];
	float percentCentralSupine;
	float percentCentralNonSupine;
	float percentCentralHypoSupine;
	float percentCentralHypoNonSupine;
	float percentSpO2Supine;
	float percentSpO2NonSupine;
	float percentArousalSupine;
	float percentArousalNonSupine;
	float RERAindex[posCount];
	float OAHindex[posCount];
	float CAHindex[posCount];
	float ODindex[posCount];
};


struct BODY_POS_RESULTS_PREVER_26 {
	int central[posCount];
	int spO2[posCount];
	int centralHyp[posCount];
	float time[posCount];
	int obstr[posCount][levelTypeCount];
	int mix[posCount][levelTypeCount];
	int obstrHyp[posCount][levelTypeCount];
	int RERA[posCount][levelTypeCount];
	int arousal[posCount];
	float snoringTime[posCount];
	float percentTime[posCount];

	float percentObstrSupine[levelTypeCount];
	float percentObstrNonSupine[levelTypeCount];
	float percentMixSupine[levelTypeCount];
	float percentMixNonSupine[levelTypeCount];
	float percentObstrHypoSupine[levelTypeCount];
	float percentObstrHypoNonSupine[levelTypeCount];
	float percentRERASupine[levelTypeCount];
	float percentRERANonSupine[levelTypeCount];
	float percentCentralSupine;
	float percentCentralNonSupine;
	float percentCentralHypoSupine;
	float percentCentralHypoNonSupine;
	float percentSpO2Supine;
	float percentSpO2NonSupine;
	float percentArousalSupine;
	float percentArousalNonSupine;
};

const float levelMargin		= 1.1f;		// 10% margin

class CEvnt
{
	friend class CEvents;
public:
	CEvnt(FLOAT _from,FLOAT _to,COleDateTime _startRecording,short _bodyPos,int _source = autoDetection);
	~CEvnt();

	static void serializeVectorFloat(CArchive & ar, vector <FLOAT>::iterator vBegin, 
										vector <FLOAT>::iterator vEnd, vector <FLOAT> *_v);
	static void serializeVectorUint(CArchive & ar, vector <UINT>::iterator vBegin, 
										vector <UINT>::iterator vEnd, vector <UINT> *_v);

	virtual void Serialize(CArchive& ar, int _majorVersion, int _minorVersion);
	virtual void Serialize20(CArchive& ar, int _majorVersion, int _minorVersion);
	CString getWarning(void);
	UINT getEventType(void);
	void setEventType(UINT _type);
	UINT getLevelType(void);
	CString getLevelDescr(void);
	void setLevelType(UINT _levelType);
	UINT getLevelTypeForMouseOver(void);
	float getFrom(void);
	float getTo(void);
	void setTo(float _to);
	void setFrom(float _from);
	float getCentreTime(void);
	short getBodyPos(void);

	void setPgradient(vector <FLOAT>::iterator _from,
					vector <FLOAT>::iterator _to,
					vector <FLOAT>::iterator _fromTime,
					vector <FLOAT>::iterator _toTime);
	void setPoesEnv(vector <FLOAT>::iterator _from,
					vector <FLOAT>::iterator _to,
					vector <FLOAT>::iterator _fromTime,
					vector <FLOAT>::iterator _toTime);
	void setPPHEnv(vector <FLOAT>::iterator _from,
					vector <FLOAT>::iterator _to,
					vector <FLOAT>::iterator _fromTime,
					vector <FLOAT>::iterator _toTime);
	void setFlow(vector <FLOAT>::iterator _from,
					vector <FLOAT>::iterator _to,
					vector <FLOAT>::iterator _fromTime,
					vector <FLOAT>::iterator _toTime);
	void setBodyPosVector(vector <FLOAT>::iterator _from,
					vector <FLOAT>::iterator _to,
					vector <FLOAT>::iterator _fromTime,
					vector <FLOAT>::iterator _toTime);
	void setContactMicVector(vector <FLOAT>::iterator _from,
					vector <FLOAT>::iterator _to,
					vector <FLOAT>::iterator _fromTime,
					vector <FLOAT>::iterator _toTime);
	void setAirMicVectors(vector <FLOAT>::iterator _from0,
					vector <FLOAT>::iterator _to0,
					vector <FLOAT>::iterator _from1,
					vector <FLOAT>::iterator _to1,
					vector <FLOAT>::iterator _from2,
					vector <FLOAT>::iterator _to2,
					vector <FLOAT>::iterator _from3,
					vector <FLOAT>::iterator _to3,
					vector <FLOAT>::iterator _fromSum,
					vector <FLOAT>::iterator _toSum,
					vector <FLOAT>::iterator _fromTime,
					vector <FLOAT>::iterator _toTime);
	virtual void doLevelAnalysis(void);
	static UINT getLevelFromPGradient(vector <FLOAT>::iterator from, vector <FLOAT>::iterator to);
	float getLength(void);
    bool getIsInEvent(float _atTime);
    int getIsInOnEdge(float _atTime,float _tolerance);
	bool isThisTheEvent(FLOAT _atTime,int *_closest);
	virtual void copyTo(CEvnt *_dest);
	virtual void copyFrom(CEvnt *_source);
	CString getTimeOfDayStart(void);
	CString getTimeOfDayStop(void);
	COleDateTime getOleTimeOfDayStart(void);
	COleDateTime getOleTimeOfDayStop(void);
    bool getHasLevelDetection(void);
	void setHasLevelDetection(bool _on);
	int getEventFlags(void);
	void setEventFlags(int _flags);
	virtual bool getIsActive(const int _currentHypoDropLimit = noHypopneaDropLimit);
	bool getManualDeleted(void);
	void setManualDeleted(bool _on);
	bool getInExcludedWindow(void);
	void setInExcludedWindow(bool _on);
	bool getInAwakeWindow(void);
	void setInAwakeWindow(bool _on);
	virtual bool considerIfInExcludeWindow(float _excludedStart, float _excludedStop);
	void setRecordingStartTimeOfDay(COleDateTime _startTOD);
	void setBodyPos(short _bodyPos);
	int getDetectionSourceManOrAuto(void);
	void setDetectionSourceManOrAuto(int _detSource);
	bool computePoesData(float *_min, float *_max, float *_avg, float *_median);
	bool computePphData(float *_min, float *_max, float *_avg, float *_median);
protected:
	bool computeVectorData(vector <FLOAT> *_v,float *_min, float *_max, float *_avg, float *_median);
	void moveEdge(FLOAT _time,int _edge);
	vector <FLOAT> pgrad,pgradTime;
	vector <FLOAT> poesEnv,poesEnvTime;
	vector <FLOAT> pphEnv, pphEnvTime;
	vector <FLOAT> flow,flowTime;
	vector <FLOAT> bpVector,bpTimeVector;
	vector <FLOAT> contactMicVector,contactMicTimeVector;
	vector <FLOAT> airMic0Vector;
	vector <FLOAT> airMic1Vector;
	vector <FLOAT> airMic2Vector;
	vector <FLOAT> airMic3Vector;
	vector <FLOAT> airMicSumVector;
	vector <FLOAT> airMicTimeVector;

	short bodyPos;

	CString warning;
	UINT eventType;			// Type of event
	UINT levelType;			// Level type
	int detectionSource;	// Detection source - manual or automatic
	FLOAT from;				// Start of event (in seconds from start)
	FLOAT to;				// End of event (in seconds from start)
	FLOAT centreTime;		// Centre of event (in seconds from start)
	FLOAT length;			// Length in seconds
	COleDateTime startRecording;
	int eventFlags;			// Contains flags about how the event was defined (detected), 
							// if it was excluded, 
							// adjusted, etc
    bool hasLevelDetection;
};

class CMixedEvnt : public CEvnt
{
public:
	CMixedEvnt(FLOAT _from,FLOAT _to,COleDateTime _startRecording,short _bodyPos,int _source = autoDetection);
	CMixedEvnt();
	~CMixedEvnt();
	virtual void Serialize(CArchive& ar, int _majorVersion, int _minorVersion);
	virtual void Serialize20(CArchive& ar, int _majorVersion, int _minorVersion);
	virtual void copyTo(CMixedEvnt *_dest);
	virtual void copyFrom(CMixedEvnt *_source);
protected:
};

class CCentralEvnt : public CEvnt
{
public:
	CCentralEvnt(FLOAT _from,FLOAT _to,COleDateTime _startRecording,short _bodyPos,int _source = autoDetection);
	CCentralEvnt();
	~CCentralEvnt();
	virtual void Serialize(CArchive& ar, int _majorVersion, int _minorVersion);
	virtual void Serialize20(CArchive& ar, int _majorVersion, int _minorVersion);
	virtual void copyTo(CCentralEvnt *_dest);
	virtual void copyFrom(CCentralEvnt *_source);
protected:
};

class CObstrEvnt : public CEvnt
{
public:
	CObstrEvnt(FLOAT _from,FLOAT _to,COleDateTime _startRecording,short _bodyPos,int _source = autoDetection);
	CObstrEvnt();
	~CObstrEvnt();
	virtual void Serialize(CArchive& ar, int _majorVersion, int _minorVersion);
	virtual void Serialize20(CArchive& ar, int _majorVersion, int _minorVersion);
	virtual void copyTo(CObstrEvnt *_dest);
	virtual void copyFrom(CObstrEvnt *_source);
protected:
};

class CHypoEvnt : public CEvnt
{
public:
	CHypoEvnt(FLOAT _from,FLOAT _to,COleDateTime _startRecording,short _bodyPos,int _dropLimit,int _source = autoDetection);
	CHypoEvnt();
	~CHypoEvnt();
	virtual void Serialize(CArchive& ar, int _majorVersion, int _minorVersion);
	virtual void Serialize20(CArchive& ar, int _majorVersion, int _minorVersion);
	int getHypoType(void);
	void setHypoType(int _hypoType);
	int findHypoType(void);
	virtual void copyTo(CHypoEvnt *_dest);
	virtual void copyFrom(CHypoEvnt *_source);
	virtual void doLevelAnalysis(void);
	
	//void setArousalPartner(float _time,bool _on);
	void setSpO2DropPartner(float _time,bool _on);
	//bool getArousalPartner(float *_time);
	bool getSpO2DropPartner(float *_time);
	void setHypoDropLimit(int _dropLimit);
	int getHypoDropLimit(void);
	bool pickFromDropLevel(int _hypoDrop);
protected:
	int hypoDropLimit;
	int hypoType;
	bool marriedToSpO2Drop;
	float partnerSpO2DropCentreTime;
};


class CRERAEvnt : public CEvnt
{
public:
	CRERAEvnt(FLOAT _from,FLOAT _to,COleDateTime _startRecording,short _bodyPos,int _source = autoDetection);
	CRERAEvnt();
	~CRERAEvnt();
	virtual void Serialize(CArchive& ar, int _majorVersion, int _minorVersion);
	virtual void Serialize20(CArchive& ar, int _majorVersion, int _minorVersion);
	virtual void copyTo(CRERAEvnt *_dest);
	virtual void copyFrom(CRERAEvnt *_source);
	void setAdmittance(vector <FLOAT>::iterator _from,
					vector <FLOAT>::iterator _to,
					vector <FLOAT>::iterator _fromTime,
					vector <FLOAT>::iterator _toTime);
	void setPulseRate(vector <FLOAT>::iterator _from,
					vector <FLOAT>::iterator _to,
					vector <FLOAT>::iterator _fromTime,
					vector <FLOAT>::iterator _toTime);
	void setRespFrq(vector <FLOAT>::iterator _from,
					vector <FLOAT>::iterator _to,
					vector <FLOAT>::iterator _fromTime,
					vector <FLOAT>::iterator _toTime);
	void setAct(vector <FLOAT>::iterator _from,
					vector <FLOAT>::iterator _to,
					vector <FLOAT>::iterator _fromTime,
					vector <FLOAT>::iterator _toTime); 
	//void setInConflictWithHypo(bool _on);
	//bool getInConflictWithHypo(void);
	//virtual bool getIsActive(void);
	void disableDueToCollWithHypo(int _hypoDropLimit);
	virtual bool getIsActive(const int _currentHypoDropLimit = noHypopneaDropLimit);
protected:
	vector <FLOAT> adm,admTime;
	vector <FLOAT> pr,prTime;
	vector <FLOAT> respFrq,respFrqTime;
	vector <FLOAT> act,actTime;
	int hypoLimitWhenDisabledDueToCollWithHypo;
};

class CArousalEvnt : public CEvnt
{
public:
	CArousalEvnt(FLOAT _from,FLOAT _to,COleDateTime _startRecording,short _bodyPos,int _source = autoDetection);
	CArousalEvnt();
	~CArousalEvnt();
	virtual void Serialize(CArchive& ar, int _majorVersion, int _minorVersion);
	virtual void Serialize20(CArchive& ar, int _majorVersion, int _minorVersion);
	virtual void copyTo(CArousalEvnt *_dest);
	virtual void copyFrom(CArousalEvnt *_source);
	void setAdmittance(vector <FLOAT>::iterator _from,
					vector <FLOAT>::iterator _to,
					vector <FLOAT>::iterator _fromTime,
					vector <FLOAT>::iterator _toTime);
	void setPulseRate(vector <FLOAT>::iterator _from,
					vector <FLOAT>::iterator _to,
					vector <FLOAT>::iterator _fromTime,
					vector <FLOAT>::iterator _toTime);
	void setRespFrq(vector <FLOAT>::iterator _from,
					vector <FLOAT>::iterator _to,
					vector <FLOAT>::iterator _fromTime,
					vector <FLOAT>::iterator _toTime);
	void setAct(vector <FLOAT>::iterator _from,
					vector <FLOAT>::iterator _to,
					vector <FLOAT>::iterator _fromTime,
					vector <FLOAT>::iterator _toTime);
	void disableDueToCollWithHypo(int _hypoDropLimit);
	int getHypoLimitWhenDisabled(void);
	bool getWasRemovedDueToCollWithHypo(const int _hypoDetCrit);
	CRERAEvnt *getRERAEvnt(void);
	void setRERAEvnt(CRERAEvnt *_reraEvnt);
	void removeRERAEvnt(void);
	virtual bool getIsActive(const int _currentHypoDropLimit = noHypopneaDropLimit);
protected:
	vector <FLOAT> adm,admTime;
	vector <FLOAT> pr,prTime;
	vector <FLOAT> respFrq,respFrqTime;
	vector <FLOAT> act,actTime;
	int hypoLimitWhenDisabledDueToCollWithHypo;
	CRERAEvnt *reraEvnt;
};

class CSpO2DropEvnt : public CEvnt
{
public:
	CSpO2DropEvnt(int _dropLimit,float _dropSize,FLOAT _beginSpO2,FLOAT _endSpO2,FLOAT _from,FLOAT _to,
		COleDateTime _startRecording,short _bodyPos,int _source = autoDetection);
	CSpO2DropEvnt(int _dropLimit);
	~CSpO2DropEvnt();
	int getDropLimit(void);
	void setDropLimit(int _limit);
	virtual void Serialize(CArchive& ar, int _majorVersion, int _minorVersion);
	virtual void Serialize20(CArchive& ar, int _majorVersion, int _minorVersion);
	virtual void copyTo(CSpO2DropEvnt *_dest);
	virtual void copyFrom(CSpO2DropEvnt *_source);
	void setHypoPartner(float _time,bool _on);
	bool getHypoPartner(float *_time);
	float getBeginSpO2(void);
	float getEndSpO2(void);
	float getSpO2DropSize(void);
protected:
	int dropLimit;
	bool marriedWithHypopnea;
	float partnerHypopneaCentreTime;
	float dropSize;
	FLOAT beginSpO2;	// The SpO2 value at the beginning of the drop
	FLOAT endSpO2;		// The SpO2 value at the end of the drop
};

class CManualMarkerEvnt : public CEvnt
{
public:
	CManualMarkerEvnt(FLOAT _from,FLOAT _to,COleDateTime _startRecording,short _bodyPos,int _source = manualDetection);
	CManualMarkerEvnt();
	~CManualMarkerEvnt();
	virtual void Serialize(CArchive& ar, int _majorVersion, int _minorVersion);
	virtual void Serialize20(CArchive& ar, int _majorVersion, int _minorVersion);
	virtual void copyTo(CManualMarkerEvnt *_dest);
	virtual void copyFrom(CManualMarkerEvnt *_source);
protected:
	CString text;
};

class CImportedEvnt : public CEvnt
{
public:
	CImportedEvnt(FLOAT _from, FLOAT _to, COleDateTime _startRecording, short _bodyPos, int _source = manualDetection);
	CImportedEvnt();
	~CImportedEvnt();
	virtual void Serialize(CArchive& ar, int _majorVersion, int _minorVersion);
	virtual void Serialize20(CArchive& ar, int _majorVersion, int _minorVersion);
	virtual void copyTo(CImportedEvnt *_dest);
	virtual void copyFrom(CImportedEvnt *_source);
protected:
	CString text;
};

class CSnoringEvnt : public CEvnt
{
public:
	CSnoringEvnt(FLOAT _from,FLOAT _to,COleDateTime _startRecording,short _bodyPos,int _source = manualDetection);
	~CSnoringEvnt();
	virtual void Serialize(CArchive& ar, int _majorVersion, int _minorVersion);
	void Serialize20(CArchive& ar, int _majorVersion, int _minorVersion);
	virtual void copyTo(CSnoringEvnt *_dest);
	virtual void copyFrom(CSnoringEvnt *_source);
	float getTimeInBodyPos(int _pos);
	float getTimeAtLevelType(int _levelType);
	void sumUpTimeInPositions(void);
	void sumUpTimeAtLevels(void);
	virtual void doLevelAnalysis(void);

	void sumUpTimeAtLevels(vector <UINT>::iterator _evntTimelineFrom, vector <UINT>::iterator _evntTimelineTo);		// Using eventtimeline vector
	void doLevelAnalysis(size_t _startDistance,vector <UINT>::iterator _evntTimelineFrom, vector <UINT>::iterator _evntTimelineTo);		// Using eventtimeline vector

	virtual bool considerManualExclusion(float _excludedStart, float _excludedStop);
	vector <FLOAT> *getLowerLevelStartStopVector(void);
	vector <FLOAT> *getUpperLevelStartStopVector(void);
	vector <FLOAT> *getMultiLevelStartStopVector(void);
	vector <FLOAT> *getUndefLevelStartStopVector(void);
	UINT getLevelAtTimeForMouseover(float _time);
	void fillAmStrengthsPerLevel(vector <UINT> *_eventTimeLineV,vector <FLOAT> *_lowV, vector <FLOAT> *_multiV, vector <FLOAT> *_upperV);
	void fillCmStrengthsPerLevel(vector <UINT> *_eventTimeLineV,vector <FLOAT> *_lowV, vector <FLOAT> *_multiV, vector <FLOAT> *_upperV);
	void fillSnoringStrengthsPerLevel(vector <UINT> *_eventTimeLineV,vector <FLOAT> *_soundVector,
		vector <FLOAT> *_soundTimeVector,vector <FLOAT> *_lowV, vector <FLOAT> *_multiV, vector <FLOAT> *_upperV);
protected:
	float timeInPos[posCount];
	float timeAtLevelType[levelTypeCount];
	vector <FLOAT> lowerLevelStartStop, multiLevelStartStop, upperLevelStartStop;
	vector <FLOAT> undefLevelStartStop;
	float originalFrom, originalTo;  // To be used to reset to original start and stop as these may be chopped off ny excluded and awake.
};

class CAwakeEvnt : public CEvnt
{
public:
	CAwakeEvnt(FLOAT _from,FLOAT _to,COleDateTime _startRecording,short _bodyPos,int _source = manualDetection);
	~CAwakeEvnt();
	virtual void Serialize(CArchive& ar, int _majorVersion, int _minorVersion);
	virtual void Serialize20(CArchive& ar, int _majorVersion, int _minorVersion);
	virtual void copyTo(CAwakeEvnt *_dest);
	virtual void copyFrom(CAwakeEvnt *_source);
	void setActimeter(vector <FLOAT>::iterator _from,
					vector <FLOAT>::iterator _to,
					vector <FLOAT>::iterator _fromTime,
					vector <FLOAT>::iterator _toTime);
	virtual bool considerManualExclusion(float _excludedStart, float _excludedStop);
protected:
	vector <FLOAT> act,actTime;
	float originalFrom, originalTo; // To be used to reset to original start and stop as these may be chopped off ny excluded and awake.
};

class CExcludedEvnt : public CEvnt
{
public:
	CExcludedEvnt(FLOAT _from,FLOAT _to,COleDateTime _startRecording,short _bodyPos,int _source = autoDetection);
	CExcludedEvnt();
	~CExcludedEvnt();
	virtual void Serialize(CArchive& ar, int _majorVersion, int _minorVersion);
	virtual void Serialize20(CArchive& ar, int _majorVersion, int _minorVersion);
	virtual void copyTo(CExcludedEvnt *_dest);
	virtual void copyFrom(CExcludedEvnt *_source);
	void setAsFirst(bool _on);
	void setAsLast(bool _on);
	bool getIsLast(void);
	bool getIsFirst(void);
protected:
	bool thisIsTheLast, thisIsTheFirst;  // The and excluded sections are used for effective start and stop
};

class CSwallowEvnt : public CEvnt
{
public:
	CSwallowEvnt(FLOAT _from, FLOAT _to, COleDateTime _startRecording, short _bodyPos, int _source = manualDetection);
	CSwallowEvnt();
	~CSwallowEvnt();
	virtual void Serialize(CArchive& ar, int _majorVersion, int _minorVersion);
	virtual void Serialize20(CArchive& ar, int _majorVersion, int _minorVersion);
	virtual void copyTo(CSwallowEvnt *_dest);
	virtual void copyFrom(CSwallowEvnt *_source);
protected:
};

class CEvents
{
public:
	CEvents();
	~CEvents();
	void setDoc(CDocument *_docP);
	bool Serialize(CArchive& ar,CString _fileID);
	CEvnt *findEvent(int _type,FLOAT _atTime,int *_closest);
	void setData(	CBatteryDataSet			*_battData		,
					CContactMicDataSet		*_cmData		,
					CAirMicDataSet			*_amData		,
					CBodyPosXYZDataSet		*_bpXYZData		,
					CActimeterDataSet		*_acData		,
					CActimeterDataSet		*_acTorsoData	,
					COximeterDataSet		*_oxData		,
					CBodyPosDataSet			*_bpData		,
					CCatheterDataSet		*_cathData		,
					CAG200MicDataSet		*_ag200MicData	,
					CRespBeltDataSet		*_respData		,
					CManualMarkerSet		*_manMarkerData	,
					COleDateTime			_startRecording);
	bool getIsVisualEventEditingActive(void);
	void setVisualEventEditingActive(bool _on);
	bool doAnalyse(bool _findStartAndStop = true);
	bool getAreEventsRespBeltBased(void);
	bool postVisualEditAnalysis(int _step);
	bool saveResults(CString _dataPath,CString fileID);
	bool readResults(CString _dataPath,CString _fileID);
	CString getAnalysisSWVersion(void);

	vector <FLOAT> *getBeginsEnds(UINT _type,UINT _level);
	vector <FLOAT> *getBeginsEnds(UINT _type);

	DETECTION_CRITERIA *getCurrentDefaultDetectionCriteria();

	SPO2_RESULTS getSpO2Results(int _sleepAwakeState);
	BODY_POS_RESULTS getBodyPosResults(void);
	PULSE_RATE_RESULTS getPulseRateResults(int _sleepAwakeState);
	LEVEL_RESULTS getLevelResults(void);
	RD_RESULTS getRDResults(void);
	SNORING_RESULT getSnoringResult(void);
	static CString secToHHMMSS(int _secs);
	static CString secToHHMM(int _secs);
	CString getSnoringTimeHHMM(void);
	void actOnEventLengthChange(void);
	void actOnEventChange(void);
	DETECTION_CRITERIA getUsedDetectionCriteria(void);
	DETECTION_CRITERIA getEditedDetectionCriteria(void);
	void setEditedDetectionCriteria(DETECTION_CRITERIA _dt,bool _onlySpO2DropOrHypopneaCrit = false);
	void recomputeFlowAndBreathingEfficiency(void);
	int getEventDetectionStatus(void);
	void changeEvent(CEvnt *_evnt,int _command);
	void moveEventEdge(CEvnt *_evnt,FLOAT _time,int _edge);
	void changeEventType(CEvnt *_evnt, int _type);
	
	int getCountMixEvents(void);
	int getCountCenEvents(void);
	int getCountObsEvents(void);
	int getCountSpoEvents(void);
	int getCountREREvents(void);
	int getCountAroEvents(void);
	int getCountManEvents(void);
	int getCountImpEvents(void);
	int getCountAwaEvents(void);
	int getCountSnoEvents(void);
	int getCountExcludedEvents(void);
	int getCountSwallowEvents(void);

	int getNumObstrHypopnea();
	int getNumCentralHypopnea();
	int getNumImportedEvents();
	
	void getPoesBins(float *_pBin,float *_pCumulBin,int _num);
	void getPphBins(float *_pBin, float *_pCumulBin, int _num);
	void getSnoringPoesBins(float *_poesBin, float *_poesCumulBin, int _num);
	void getSnoringPphBins(float *_pphBin, float *_pphCumulBin, int _num);
	int getNumREBins(void);
	void getPoesHistoBodyPosBins(float *_poesHistoBinBodyPos,int _num);
	void getPoesHistoLevelBins(float *_poesHistoLevelBins,int _num);;
	void getPphHistoBodyPosBins(float *_pphHistoBinBodyPos, int _num);
	void getPphHistoLevelBins(float *_pphHistoLevelBins, int _num);
	void getSnoringPoesHistoBodyPosBins(float *_poesHistoBinBodyPos, int _num);
	void getSnoringPoesHistoLevelBins(float *_poesHistoLevelBins, int _num);
	void getSnoringPphHistoBodyPosBins(float *_pphHistoBinBodyPos, int _num);
	void getSnoringPphHistoLevelBins(float *_pphHistoLevelBins, int _num);

	CHypoEvnt *findHypoAtTime(float _hypoTime,int *_number);

	float findPrevEvent(float _cTime,UINT _eventsToShow);
	float findNextEvent(float _cTime,UINT _eventsToShow);
	float getEffectiveStart(void);
	float getEffectiveStop(void);
	bool setEffectiveStop(float _sec);
	bool setEffectiveStart(float _sec);
	bool setEffectiveStop(int _h, int _m, int _sec);
	bool setEffectiveStart(int _h, int _m, int _sec);

	CEvnt *getIsOnEvent(int _eventsToShow,float _t,float _edgeDetectTolerance,int *_what,int *_where);
	bool getIsOnEventDuringVisualEventEditing(int _eventsToShow,float _t, CString *_descr,CString *_lvl,float *_start,float *_stop);
	CString getTimeOfDay(float _t);
	UINT getIsOnEventVisualEditing(float _t, float _edgeDetectTolerance, int *_what, int *_where,
		UINT *_levelType,
		float *_startTime = NULL, float *_stopTime = NULL); 
	UINT getIsOnEventVisualEditing_Range(const float _leftLim,const float _rightLim, UINT *_what, UINT *_where,
			UINT *_levelType,float *_startTime = NULL, float *_stopTime = NULL);
	COleDateTime getStartRecordingClockTime(void);

	float getOdiResult(void);
	float getRdiResult(void);
	float getRE15Sleeping(void);
	float getRE15Snoring(void);
	float getArousalIndexResult(void);
	float getMaiResult(void);
	float getCaiResult(void);
	float getOaiResult(void);
	float getOhiResult(void);
	float getChiResult(void);
	float getReraiResult(void);
	float getOahiResult(void);
	float getOsa_ordiResult(void); 
	float getCahiResult(void);
	CString getEffectiveSleepTimeHHMM(void);
	CString getEffectiveSleepTimeHHMMSS(void);
	float getTotalSleepTime(void);
	void setExcludedEvent(bool _setExclude,float _from, float _to,bool _addToStack = true,bool _runPostExcludeAnalysis = true);
	void redoEditAction(void);
	void undoEditAction(void);
	bool getLeftToRedo(void);
	bool getLeftToUndo(void);
	float getAwakeTime(float _from, float _to);
	float getExcludedTime(float _from, float _to);
	void getCatheterAndRespDataStatistics(float _editTimeWindowStart, float _editTimeWindowStop,
		CStringArray *_title,
		CStringArray *_unit,
		CStringArray *_max,
		CStringArray *_min,
		CStringArray *_avg,
		CStringArray *_median,
		CStringArray *_span,
		CStringArray *_stdDev);
	void getOximeterStatistics(float _editTimeWindowStart, float _editTimeWindowStop,
		CStringArray *_title,
		CStringArray *_unit,
		CStringArray *_max,
		CStringArray *_min,
		CStringArray *_avg,
		CStringArray *_median,
		CStringArray *_span,
		CStringArray *_stdDev);
	void getStat(float _start, float _stop,
		CString *_title,CString *_unit,
		vector <FLOAT> *_v, vector <FLOAT> *_t,
		CString *_max, CString *_min, CString *_avg, CString *_median, CString *_span,
		CString *_stdDev,vector <BOOL> *_pd = NULL);

	void getStatisticsForADataVector(
		float _start, float _stop,
		vector <FLOAT> *_dataVector,
		vector <FLOAT> *_timeVector,
		CString *_max,
		CString *_min,
		CString *_avg,
		CString *_median,
		CString *_span,
		CString *_stdDev,
		vector <BOOL> *_penDownVector);
	static CString getFmtString2ValidDigits(float _stdDev);
	CArray <CSpO2DropEvnt *, CSpO2DropEvnt *>			*getEventArraySpO2(void);
	CArray <CArousalEvnt *, CArousalEvnt *>				*getEventArrayArousal(void);
	CArray <CHypoEvnt *, CHypoEvnt *>					*getEventArrayHypo(void);
	CArray <CObstrEvnt *, CObstrEvnt *>					*getEventArrayObstr(void);
	CArray <CCentralEvnt *, CCentralEvnt *>				*getEventArrayCentral(void);
	CArray <CMixedEvnt *, CMixedEvnt *>					*getEventArrayMixed(void);
	CArray <CAwakeEvnt *, CAwakeEvnt *>					*getAwakeEventArray(void);
	CArray <CSnoringEvnt *, CSnoringEvnt *>				*getSnoringEventArray(void);
	CArray <CManualMarkerEvnt *, CManualMarkerEvnt *>	*getManualMarkerEventArray(void);
	CArray <CExcludedEvnt *, CExcludedEvnt *>			*getExcludedEventArray(void);
	CArray <CImportedEvnt *, CImportedEvnt *>			*getImportedEventArray(void);
	void deleteEvent(CEvnt *_evP, UINT _eventType);
	void postEditEventAnalysis(UINT _eventTypes);
	bool getDoAwake(void);
	void postChangeActions(void);
	void preChangeActions(void);
	bool addEvent(UINT _type,float _from, float _to);

	bool addImportedEvent(COleDateTime _fromTOD, COleDateTime _toTOD);
	bool addImportedEvent(COleDateTime _fromTOD, float _length);
	bool resetImportedEvents(void);

	void resetEventsAndReanalyse(void);
 	bool getDirtyFlag(void);
	void setDirtyFlag(bool _on);
	CString getEventsList(float _timeSec);
	CString getTimeOfDayAt(float _secs);
	bool getNoEditFlag(void);
	bool debugHelper(int _posInCode);
	bool copyBetweenEventTypes(CEvnt *_source, CEvnt *_dest);
	void actOnManualDelete(void);
	void actOnChangedDetectOptions(void);
	//bool anyOngoingVisualEdits(void);
	void startVisualEventEditing(UINT _eventType);
	bool getAreVisualEventsDirty(void);
	void setVisualEventsToClean(void);

	void setEditingViaDialogIsActive(bool _on);
	bool getIsEditingViaDialogActive(void);
	void removeEditedEvents(void);
	bool visualEditAddNewEvent(UINT _eventType,float _time, float _leftTimeLimit, float _rightTimeLimit,UINT _level);
	bool visualEditAdd(UINT _eventType,float _from,float _to, float _leftTimeLimit, float _rightTimeLimit,UINT _level);
	bool visualEditAddSnoring(UINT _eventType, float _from, float _to, float _leftTimeLimit, float _rightTimeLimit);
	bool visualEditSetLevel(UINT _eventType, float _time, UINT _level);
	UINT getTheEventBeingVisuallyEdited(void);
	UINT getEventsToShowDuringVisualEditing(void);
	//CString getConsolidateButtonCaption(void);
	void nowVisualEditing(const VISUAL_EVENT _evt);
	void setVisualEditingLimits(const float _from, const float _to);
	VISUAL_EVENT getVisualEditingEventByMouse(void);
	void doneVisualEditingMoves(UINT _eventType,float _from, float _to, float _leftTimeLimit, 
		float _rightTimeLimit,UINT _level);
	void doneVisualEditingSnoringMoves(UINT _eventType, float _from, float _to, 
		float _leftTimeLimit,float _rightTimeLimit);
	bool consolidateVisualEditing(void); 
	bool getConflict(UINT _typeMoving, float _begin, float _end); 
	void onOffVectorToBeginEndsVector(UINT _eventType);
	void onOffVectorToBeginEndsVector(void);
	UINT findLevelInTimeWindow(float _from, float _to);
	void getVersion(int *_majorVersion, int *_minorVersion);
	CString getPDFReportFileName(void);
	CString getPDFShortReportFileName(void);
	void fillStartsAndStopsVector(unsigned int _actOn = evntTypeAll);
	bool visualEditDeleteWholeEvent(UINT _eventType, float _t);
protected:
	bool visualEditDelete(CEvnt *_evP);
	bool visualEditDelete(UINT _eventType, float _t);
	int majorVersion, minorVersion;
	bool serialize20(CArchive& ar, int _majorVersion, int _minorVersion);
	void debugEventTimeLineVector(CString _heading);
	void debugEditEventTimeLineVector(CString _heading);
	vector <FLOAT> *getEventEditVectorToModify(UINT _eventType,UINT _levelType = 0);
	foo getEventTestVectorPredicate(UINT _eventType,UINT _levelType);
	foo getEventTestVectorPredicate(UINT _eventType);
	foouint getEventSetOperation(UINT _eventType);
	foouint getLevelSetOperation(UINT _eventType,UINT _levelType);
	foouint getAHLevelEventSetOperation(UINT _levelType);
	foouint getSnoringLevelEventSetOperation(UINT _levelType);
	foouint getEventClearOperation(UINT _eventType);
	foouint getLevelClearOperation(UINT _eventType);
	void onOffVectorToBeginEndsVector(UINT _eventType, vector <UINT> *_onOffV);
	void findLimitsBetweenExcludedEvents(const float _t, float *_leftLimit, float *_rightLimit);
	void findLimitsBetweenAwakeEvents(const float _t, float *_leftLimit, float *_rightLimit);
	//int getConsolidateStep(void);
	UINT eventsBeingVisuallyEdited;
	bool doingVisualEditingMove;
	VISUAL_EVENT eventByMouse;
	bool editingViaDialogIsActive;
	void dumpVector(vector <FLOAT> *_v);
	CString analysisSWVersion;		// The software version that produced the results
	void sortAndOrganiseEvents(int _changeFlags);
	
	bool copyMixedEventTo(CMixedEvnt *_source, CEvnt *_dest);
	bool copyCentralEventTo(CCentralEvnt *_source, CEvnt *_dest);
	bool copyObstrEventTo(CObstrEvnt *_source, CEvnt *_dest);
	bool copyImportedEventTo(CImportedEvnt *_source, CEvnt *_dest);
	bool copyHypoEventTo(CHypoEvnt *_source, CEvnt *_dest);

	float getFloatSpO2DropLimit(void);
	int computeCurrentHypopneaDropLimitEnum(float _detCritDrop);
	bool noEditFlag;  // Used to avoid changing the analysis if the events were saved with version 7.0 of the events file
	bool doNotSaveFlag;
	void addExcludedEventsFromTimeWindowVector(vector <TIME_WINDOW> *_v, int _spO2DropLimit);
	void convertCriteria70to100(DETECTION_CRITERIA70 *_dt70, DETECTION_CRITERIA100 *_dt);
	void convertCriteria100to200(DETECTION_CRITERIA100 *_dt100, DETECTION_CRITERIA200 *_dt);
	void convertCriteria200to290(DETECTION_CRITERIA200 *_dt200, DETECTION_CRITERIA *_dt);

	CDocument *docP;
	bool dirtyFlag;
	int flags;

	void changeEventTypeDuringEdit(CEvnt *_evnt, int _toType, int _spO2DropLimit = noSpO2DropLimit);
	int findLevelFromLevelVector(vector <int>::iterator _from, vector <int>::iterator _to);
	int findDropLimitFromDropLimitVector(vector <int>::iterator _from, vector <int>::iterator _to);

	bool removeMixed(const CEvnt *_evnt);
	bool removeImported(const CEvnt *_evnt);
	bool removeCentral(CEvnt *_evnt);
	bool removeObstr(CEvnt *_evnt);
	bool removeHypo(CEvnt *_evnt,int _spO2DropLimit);
	bool removeRERA(CEvnt *_evnt, int _spO2DropLimit);
	bool removeArousal(CEvnt *_evnt, int _spO2DropLimit);
	bool removeSpO2Drop(CEvnt *_evnt,int _spO2DropLimit);
	bool removeAwake(CEvnt *_evnt);
	bool removeSnoring(CEvnt *_evnt);

	//---These are old and removes overlaps - VERY COMPLICATED
	//void OLDsortAndOrganiseMixed(void);
	//void OLDsortAndOrganiseCentral(void);
	//void OLDsortAndOrganiseObstr(void);
	//void OLDsortAndOrganiseHypo(int _spO2Drop);
	//void OLDsortAndOrganiseArousal(int _spO2Drop);
	//void OLDsortAndOrganiseSpO2Drop(int _spO2Drop);
	//void OLDsortAndOrganiseAwake(void);
	//void OLDsortAndOrganiseSnoring(void);
	//void OLDsortAndOrganiseExcluded(void);
	//void OLDsortAndOrganiseEvntSortVector(vector <EVENT_SORT_VECTOR_ELEMENT> *_evV, bool _useLevel = false);

	//---These are new (from version 4.4) and only warns about overlaps
	void sortAndOrganiseMixed(void);
	void sortAndOrganiseCentral(void);
	void sortAndOrganiseObstr(void);
	void sortAndOrganiseHypo(int _spO2Drop);
	void sortAndOrganiseArousal(int _spO2Drop);
	void sortAndOrganiseSpO2Drop(int _spO2Drop);
	void sortAndOrganiseAwake(void);
	void sortAndOrganiseImported(void);
	void sortAndOrganiseSnoring(void);
	void sortAndOrganiseExcluded(void);
	void sortAndOrganiseManualMarkers(void);

	int getNumObstrHypopnea(int _hypoLimit, int _spO2Drop);
	int getNumCentralHypopnea(int _hypoLimit, int _spO2Drop);
	int countHypEvents(int _hypoLimit, int _spO2Drop);
	int countREREvents(int _hypoLimit, int _spO2Drop);
	int countAroEvents(int _hypoLimit, int _spO2Drop);
	int countSpoEvents(int _spO2Drop);	

	void reset(bool _clearStateVector = true);
	void clearStateVector(void);
	void resetSpO2Results(SPO2_RESULTS *_spP);
	void resetBodyPosResults(BODY_POS_RESULTS *_bpP);
	void resetLevelResults(LEVEL_RESULTS *_lvP);
	void resetRdResults(RD_RESULTS *_rdP);
	void resetEventTimelineVector(void);
	void resetSnoringVector(void);
	void resetStack(std::stack <CEvnt *> *_stack);
	void fillInEventTimelineVector(void);
	void fillInEventTimeLineVectorForType(int _type, UINT _levelType,float _from, float _to);
	void computeTotalSleepTime(void);
	//void fillEventCopyArray(void);
	void getCriteriaFromRegistry(void);
	void saveCriteriaToRegistry(void);
	CEvnt *removeAndReplaceEvent(CEvnt *_evnt);

	int getLevel(float _gradient);

	std::vector <CMemFile *> stateVector;
	int statePointer;
	void addNewState(void);

	bool analyseSwallow(void);
	bool fillManualEventArray(void);
	bool analyseSpO2(int _dropLimit);
	bool analysePulseRate(void);
	bool testIfDataOKforAnalysis(float *_startAnalysisTime, float *_stopAnalysisTime);
	void setupHelpEventVector(vector <FLOAT> *_helpVTimeP, vector <unsigned short> *_helpVP, 
									float _startAnalysisTime, float _stopAnalysisTime);
	bool findMixCentralObstrApnea(vector <FLOAT> *_helpVTimeP, vector <unsigned short> *_helpVP);
	bool findHypopnea(vector <FLOAT> *_helpVTimeP, vector <unsigned short> *_helpVP, int _spO2DropLimit);
	void doBodyPosAnalysis(int _hypoLimit, int _spO2Drop);
	void doOverallLevelSumUp(int _hypoLimit,int _spO2Drop);
	void doSnoringSumUp(void);
	//void correctTotalSleepTimeAgainstAwake(void);
	//void correctTotalSleepTimeAgainstExcluded(void);
	void doIndexAnalysis(int _hypoLimit, int _spO2Drop);		// ODI, RDI, AHI
	void doAwakeAnalysis(void);
	bool doAdvancedAwakeAnalysis(void);
	void doSnoringAnalysis(void);
	void doManualMarkerAnalysis(void);
	void doRERAandArousalAnalysis(void);
	bool getDoesFlowRiseOld(float _from,float _to);
	bool getDoesFlowRise(float _from, float _to);
	void postExcludeAnalysis(void);

	float getMedian(vector <FLOAT> *_v);
	bool flowDataToEvent(CEvnt *_eV,vector <FLOAT> *_v,vector <FLOAT> *_t,FLOAT _from,FLOAT _to);
	bool gradientDataToEvent(CEvnt *_eV,vector <FLOAT> *_v,vector <FLOAT> *_t,FLOAT _from,FLOAT _to);
	bool poesEnvDataToEvent(CEvnt *_eV,vector <FLOAT> *_v,vector <FLOAT> *_t,FLOAT _from,FLOAT _to);
	bool pphEnvDataToEvent(CEvnt *_eV, vector <FLOAT> *_v, vector <FLOAT> *_t, FLOAT _from, FLOAT _to);
	bool bodyDataToEvent(CEvnt *_eV,vector <FLOAT> *_v,vector <FLOAT> *_t,FLOAT _from,FLOAT _to);
	bool contactMicDataToEvent(CEvnt *_eV,vector <FLOAT> *_v,vector <FLOAT> *_t,FLOAT _from,FLOAT _to);
	bool airMicDataToEvent(CEvnt *_eV,
						vector <FLOAT> *_v0,
						vector <FLOAT> *_v1,
						vector <FLOAT> *_v2,
						vector <FLOAT> *_v3,
						vector <FLOAT> *_vSum,
						vector <FLOAT> *_t,FLOAT _from,FLOAT _to);
	void toSpO2FallsHistogram(float _saturation,int _dropLimit,int _sleepAwakeState);
	bool areAlmostEqual(int _a, int _b, int _c);
	bool areAlmostEqual(int _a, int _b);

	bool getIsAwake(float _atTime);
	bool getIsExcluded(float _atTime);
	bool getIsSnoring(float _atTime);

	void enableArousals(int _hypoLimit, int _spO2DropLimit);
	//void enableRERA(int _hypoLimit, int _spO2DropLimit);
	void enableHypo(int _hypoLimit, int _spO2DropLimit);
	void enableObstr(void);
	void enableCentral(void);
	void enableMixed(void);
	void enableSwallow(void);
	void enableSpO2Drops(int _spO2DropLimit);

	void removeConflictingHypos(int _spO2DropLimit);
	void removeArousalsWhenAnotherEvent(int _hypoLimit,int _spO2DropLimit);
	void deactivateArousalsWhenExcludedOrAwake(int _hypoLimit, int _spO2DropLimit);
	void removeHypoWhenAnotherEvent(int _spO2DropLimit);
	void removeHypoWhenObstrCentralMixEvent(int _spO2DropLimit);
	void removeObstrWhenAnotherEvent(void);
	void removeCentralWhenAnotherEvent(void);
	void removeMixedWhenAnotherEvent(void);
	void removeSnoringWhenAnotherEvent(void);
	void disableSwallowWhenExcluded(void);
	void disableArousalsWhenAwakeOrExcluded(int _spO2DropLimit);
	void disableReraWhenAwakeOrExcluded(void);
	void disableHypoWhenAwakeOrExcluded(int _hypoLimit,int _spO2DropLimit);
	void disableObstrWhenAwakeOrExcluded(void);
	void disableCentralWhenAwakeOrExcluded(void);
	void disableMixedWhenAwakeOrExcluded(void);
	void disableSpO2DropsWhenAwakeOrExcluded(int _spO2DropLimit);
	void fillAwakeEventArrayFromEventTimelineVector(void);
	void fillDesatEventArrayFromEventTimelineVector(void);
	void fillMixedEventArrayFromEventTimelineVector(void);
	void fillCentralEventArrayFromEventTimelineVector(void);
	void fillObstrEventArrayFromEventTimelineVector(void);
	void fillHypoEventArrayFromEventTimelineVector(void);
	void fillSwallowArrayFromEventTimelineVector(void);
	void fillReraArrayFromEventTimelineVector(void);
	void fillSnoringEventArrayFromEventTimeLineVector(void);
	void removeRERAWhenAnotherEvent(int _spO2DropLimit);
	void removeSpO2DropsWhenAnotherEvent(int _spO2DropLimit);
	void prepareSpO2TimelineVectorAndSumupStatistics(void);
	void sumUpSpO2(int _dropLimit);

	void POESEnvBinning(void);
	void POESEnvBinningForSnoring(void);
	void PPHEnvBinning(void);
	void PPHEnvBinningForSnoring(void);

	bool getCollisionWithObstr(float _from, float _to);
	bool getCollisionWithCentral(float _from, float _to);
	bool getCollisionWithMix(float _from, float _to);
	bool getCollisionWithHypo(float _from, float _to, int _hypoLimit,int _spO2DropLimit);
	bool getCollisionWithRERA(float _from, float _to, int _hypoLimit,int _spO2DropLimit);
	bool getCollisionWithAwake(float _from, float _to);
	bool getCollisionWithSnoring(float _from, float _to);
	bool getCollisionWithSpO2Drop(float _from, float _to);
	bool getCollisionWithArousal(float _from, float _to, int _hypoLimit, int _spO2DropLimit);
	bool getCollisionWithExcluded(float _from, float _to);
	bool getCollisionWithPrePostBedtime(float _from, float _to);

	bool getIsInsideMixed(float _from, float _to);
	bool getIsInsideCentral(float _from, float _to);
	bool getIsInsideObstructive(float _from, float _to);
	bool getIsInsideHypopnea(float _from, float _to, int _hypoLimit, int _spO2DropLimit);

	void removeRERAAtTime(float _cTime,int _hypoLimit, int _spO2DropLimit);
	//void deactivateRERAAtTime(float _cTime, int _hypoLimit, int _spO2DropLimit);
	void findEffectiveStartAndStop(void);
	CSpO2DropEvnt *findSpO2DropToMarry(float _from, float _to, float *_center,int _spO2DropLimit);

	CBatteryDataSet			*battData;
	CContactMicDataSet		*cmData;
	CAirMicDataSet			*amData;
	CBodyPosXYZDataSet		*bpXYZData;
	CActimeterDataSet		*acData;
	CActimeterDataSet		*torsoAcData;
	COximeterDataSet		*oxData;
	CBodyPosDataSet			*bpData;
	CCatheterDataSet		*catheterData;
	CAG200MicDataSet		*ag200MicData;
	CRespBeltDataSet		*respData;
	CManualMarkerSet		*mMarkerData;

	//---For help during analysis
	vector <OXIMETER_SET> oximeterTimelineVector;

	//---Data to be saved and read
	CArray <CSpO2DropEvnt *, CSpO2DropEvnt *>			spO2DropEventArray3;			// With 3% drop
	CArray <CSpO2DropEvnt *, CSpO2DropEvnt *>			spO2DropEventArray4;			// With 4% drop
	CArray <CSpO2DropEvnt *, CSpO2DropEvnt *>			*spO2DropEventArrayCurrentP;	// The one we currently use

	CArray <CHypoEvnt *, CHypoEvnt *>					hypoEventArrayDesat3;			// With 3% desaturation drop
	CArray <CHypoEvnt *, CHypoEvnt *>					hypoEventArrayDesat4;			// With 4% desaturation drop
	CArray <CHypoEvnt *, CHypoEvnt *>					*hypoEventArrayCurrentP;		// The one we currently use

	CArray <CArousalEvnt *, CArousalEvnt *>				arousalEventArrayDesat3;		// With 3% desaturation drop
	CArray <CArousalEvnt *, CArousalEvnt *>				arousalEventArrayDesat4;		// With 4% desaturation drop
	CArray <CArousalEvnt *, CArousalEvnt *>				*arousalEventArrayCurrentP;		// The one we currently use

	CArray <CObstrEvnt *, CObstrEvnt *>					obstrEventArray;
	CArray <CCentralEvnt *, CCentralEvnt *>				centralEventArray;
	CArray <CMixedEvnt *, CMixedEvnt *>					mixedEventArray;
	CArray <CAwakeEvnt *, CAwakeEvnt *>					awakeEventArray;
	CArray <CSnoringEvnt *, CSnoringEvnt *>				snoringEventArray;
	CArray <CManualMarkerEvnt *, CManualMarkerEvnt *>	manualMarkerEventArray;
	CArray <CImportedEvnt *, CImportedEvnt *>			importedEventArray;
	CArray <CExcludedEvnt *, CExcludedEvnt *>			excludedEventArray;
	CArray <CSwallowEvnt *, CSwallowEvnt *>				swallowEventArray;

	vector <UINT> eventTimelineVector;
	vector <FLOAT> EventsBeginsEnds;
	vector <FLOAT> eventsWithLevelBeginsEnds[evCountWithLevel][bgLevelTypeCount];
	vector <FLOAT> eventsWithoutLevelBeginsEnds[evCountWithoutLevel];

	vector <FLOAT> editEventsWithLevelBeginsEnds[evCountWithLevel][bgLevelTypeCount];
	vector <FLOAT> editEventsWithoutLevelBeginsEnds[evCountWithoutLevel];
	vector <UINT> editEventTimelineVector;

	vector <UINT> snoringVector;
	void fillSnoringVector(void);

	void fillExcludedEventArrayFromStartsStopsVector(vector <FLOAT> *_v);
	void fillExcludedEventArrayFromEventTimelineVector(void);

	//---POES histograms
	float poesBin[ppBins];
	float poesCumulBin[ppBins];
	float poesHistoBinLevel[ppHistoBins][levelTypeCount];
	float poesHistoBinBodyPos[ppHistoBins][posCount];
	float snoringPoesBin[ppBins];
	float snoringPoesCumulBin[ppBins];
	float snoringPoesHistoBinLevel[ppHistoBins][levelTypeCount];
	float snoringPoesHistoBinBodyPos[ppHistoBins][posCount];

	//---PPH histograms
	float pphBin[ppBins];
	float pphCumulBin[ppBins];
	float pphHistoBinLevel[ppHistoBins][levelTypeCount];
	float pphHistoBinBodyPos[ppHistoBins][posCount];
	float snoringPphBin[ppBins];
	float snoringPphCumulBin[ppBins];
	float snoringPphHistoBinLevel[ppHistoBins][levelTypeCount];
	float snoringPphHistoBinBodyPos[ppHistoBins][posCount];

	float getDefaultLengthOfEventToAdd(UINT _etl,float _tWidth);

	COleDateTime startRecordingClockTime;
	float effectiveStartTime,effectiveStopTime;		// Requires 5 min or so in bed (forwards and backwards)
	float maxStopTime;								// No more data after this time
	float totalSleepTime;
	
	PULSE_RATE_RESULTS pulseRateResults[numSleepAwakeStates];

	//---These results depends on detection criteria. We will do it all first time
	SPO2_RESULTS spO2Res[numSpO2DropLimits][numSleepAwakeStates];
	BODY_POS_RESULTS bodyPosResults[numHypopneaDropLimits][numSpO2DropLimits];
	LEVEL_RESULTS lvlResults[numHypopneaDropLimits][numSpO2DropLimits];
	RD_RESULTS rdResults[numHypopneaDropLimits][numSpO2DropLimits];
	SNORING_RESULT snoringResult;

	float rdiResult[numHypopneaDropLimits][numSpO2DropLimits];		// osa_ordi + cai
	float arousalIndexResult[numHypopneaDropLimits][numSpO2DropLimits];
	float maiResult, caiResult, oaiResult;
	float ohiResult[numHypopneaDropLimits][numSpO2DropLimits];
	float chiResult[numHypopneaDropLimits][numSpO2DropLimits];
	float reraiResult[numHypopneaDropLimits][numSpO2DropLimits];
	float cahiResult[numHypopneaDropLimits][numSpO2DropLimits];		// chi + cai
	float oahiResult[numHypopneaDropLimits][numSpO2DropLimits];		// mai+oai+ohi
	float osa_ordiResult[numHypopneaDropLimits][numSpO2DropLimits]; // oahi + rerai

	int numObstrHypopnea[numHypopneaDropLimits][numSpO2DropLimits];
	int numCentralHypopnea[numHypopneaDropLimits][numSpO2DropLimits];

	DETECTION_CRITERIA detectionCriteria;		// Detection criteria used for the analysis
	DETECTION_CRITERIA editedDetectionCriteria; // Detection criteria manipulated by the user
												// Once the user starts a new analysis, detectionCriteria is set equal to editedDetectionCriteria
												// edited detection criteria are also
	DETECTION_CRITERIA currentDefaultDetectionCriteria;		// Detection criteria that are currently the default
															// Once the user resets the anaylsis, detection criteria are set to this 
													
	
	int eventDefinition;						// Contains flags about how the events were defined (detected)

	void serializeSpO2Results(CArchive& ar, SPO2_RESULTS *_sp, int _majorVersion, int _minorVersion);
	void serializeBodyPosResults(CArchive& ar, BODY_POS_RESULTS *_bp, int _majorVersion, int _minorVersion);
	void serializePulseRateResults(CArchive& ar, PULSE_RATE_RESULTS *_pr, int _majorVersion, int _minorVersion);
	void serializeLevelResults(	CArchive& ar, LEVEL_RESULTS *_lv, int _majorVersion, int _minorVersion);
	void serializeRdResults(CArchive& ar, RD_RESULTS *_rd, int _majorVersion, int _minorVersion);
	void serializeSnoringResults(CArchive& ar, SNORING_RESULT *_sn, int _majorVersion, int _minorVersion);

	//---For reading old versions of the events data file
	void serializeLevelResults20(CArchive& ar, LEVEL_RESULTS *_lv, int _majorVersion, int _minorVersion);
	void serializeBodyPosResultsPre36(CArchive& ar, BODY_POS_RESULTS *_bp, int _majorVersion, int _minorVersion);
	void serializeBodyPosResults20(CArchive& ar, BODY_POS_RESULTS *_bp, int _majorVersion, int _minorVersion);

	void evaluateAllEventsWrtExclusionWindow(CExcludedEvnt *_eP, int _spO2DropLimit);
	void unExcludeAllEvents(int _spO2DropLimit);

	bool generateVectorsForAwakeDet(void);
	void fillDensityVector(vector <bool> *_onV, vector <FLOAT> *_densV,float _minutes);
	void fillEventOnVector(vector <bool> *_dV, vector <FLOAT> *_dT, CArray <CEvnt *, CEvnt *> *_eA);
	void fillSnoringOnVector(vector <bool> *_dV, vector <FLOAT> *_dT, CArray <CSnoringEvnt *, CSnoringEvnt *> *_eA);
	void fillActimeterOnVector(vector <bool> *_dV, vector <FLOAT> *_dT, vector <FLOAT> *_baseline,
								vector <FLOAT> *_act, vector <FLOAT> *_actTime);
	void fillTorsoActimeterOnVector(vector <bool> *_dV, vector <FLOAT> *_dT, 
								vector <FLOAT> *_act, vector <FLOAT> *_actTime);
	void fillEventOnVector(vector <bool> *_dV, vector <FLOAT> *_dT, CArray <CArousalEvnt *, CArousalEvnt *> *_eA);

	void fillUprightOnVector(vector <bool> *_dV, vector <FLOAT> *_dT,CBodyPosDataSet *_bSet);
	vector <FLOAT> densityMixe;
	vector <FLOAT> densityObst;
	vector <FLOAT> densityCent;
	vector <FLOAT> densityHypo;
	vector <FLOAT> densityDesa;
	vector <FLOAT> densityRERA;
	vector <FLOAT> densitySnor;
	vector <FLOAT> densityActi;
	vector <FLOAT> densityTAct;
	vector <FLOAT> densitySwal;
	vector <FLOAT> densityStan;
	vector <FLOAT> densitySumm;
	vector <FLOAT> densityTime;

	bool visualEventEditingIsActive;
};

#endif
