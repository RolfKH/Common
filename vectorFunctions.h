#ifndef VECTOR_FUNCTIONS_INCLUDED
#define VECTOR_FUNCTIONS_INCLUDED

/*
Vector functions

Rev	By	Description
0	RKH	Initial version
1	RKH Each channel now has many data types (raw, filtered, peak to peak, envelope, baseline, etc.
*/

/*!
* @author		RKH
* @date		06.06.2017
* @copyright (C)Spiro Medical AS 2013 - 2017
*/

#include <vector>
#include <algorithm>
#include <numeric>
#include "ChannelList.h"

#include "..\..\Firmware\spiro\Catheter\AGS_CathDefinitions.h"
using namespace std;

class CSwallowEvnt;
class CManualMarkerEvnt;

#define IQR_SPO2		3.0f
#define IQR_PR			4.0f

#define LINREG_HALFLEN_FOR_PEAK_DETECT		4 // Used to check surroundings about max and minima in peakDetect2()
#define NUM_SAMPLES_FOR_SWALLOW_DETECT		80
#define IQR_FACTOR_PPH_SWALLOW_DETECT		3.0f
#define IQR_FACTOR_POES_SWALLOW_DETECT		3.0f
#define SWALLOW_TIME_DIFF_LOW				1.0f // 1.0 sec
#define SWALLOW_TIME_DIFF_HIGH				9.0f // 9.0 sec

#define MIN_NUMSAMPLES_FOR_OUTLIER_DETECTION	50 // 50 samples


template <class Type>
class SetValue
{
private:
	Type setVal;   // The value to set
public:
	// Constructor initializes the value to set
	SetValue(const Type& _Val) : setVal(_Val) {
	}

	// The function call for the element to set
	Type operator ( ) (Type& elem) const
	{
		return setVal;
	}
};

//---For flow detection
#define FLOW_DETECT_BALANCE_FACTOR			.6f		// k * T1 + (1.0 - k) * T0
// k = 1 means T1 is used
// k = 0 means T0 ise used
// k = 0.5 means T0 + T1 are used with equal weights

typedef struct anOximeterSet {
	float spO2, pr;
	UINT state;
} OXIMETER_SET;

class CExcludedEvnt;
class CAwakeEvnt;

//---Actimeter
const float actimeterSatLimit = 100.0f; 
const float actimeterConstantBaseline = 3.0f; 
const float torsoActimeterdBLowestValue = 40.0f;
const float torsoActimeterLowestValue = 0.01f;

//---SensorTechnics Cannula sensor
const CString sensorType = _T("HCLA0075HB");
const int maxPressure = 10;
const int minPressure = -10;
const int maxCount = 0x6CCC;
const int minCount = 0x0666;

#define LOW_PRESS		0.00001f
#define LOW_TEMP		0.00001f

#define PRESS_OUT_OF_RANGE		0x0100
#define TEMP_OUT_OF_RANGE		0x0200

#define NONE_DATA								0
#define SPO2_DATA								1
#define PR_DATA									2
#define AM_DATA									3
#define CM_DATA									4
#define AC_DATA									5
#define BP_XYZ_DATA								6
#define BP_DATA									7
#define BATT_V_DATA								8
#define BATT_REMAINING_CAPACITY_DATA			9
#define BATT_FULL_CHARGE_CAPACITY_DATA			10
#define BATT_TEMP_DATA							11
#define POES_DATA								12
#define PPH_DATA								13
#define T0_DATA									14
#define T1_DATA									15
#define AG200_MIC_DATA							16
#define T0_PLUS_T1_DATA                         17
#define PPH_ENV_DATA                            18
#define ABDOM_BELT_DATA							19
#define CHEST_BELT_DATA							20
#define CANNULA_DATA							21
#define PUPPERGRADIENT_DATA						22
#define PLOWERGRADIENT_DATA						23
#define PGRADIENT_DATA							24
#define T0_RAW_DATA								25
#define T1_RAW_DATA								26
#define BATT_CURRENT_DATA						27

#define BATT_V_LIMIT							3.7f			// V
#define BATT_CURRENT_LIMIT						50.0f			// mA
#define BATT_FULL_CHARGE_CAPACITY_LIMIT			1350.0f			// mAh
#define BATT_REMAINING_CHARGE_CAPACITY_LIMIT	1000.0f			// mAh
#define BATT_TEMP_LIMIT_DEGC					55.0f			// degC. 
																// At 60, the battery goes low power
																// At 57, the Torso goes to sleep
#define ACTIMETER_MAX_LIMIT						100.0f			// Some sensible limit
#define CM_MAX_LIMIT							100.0f			// Some sensible limit
#define AM_MAX_LIMIT							200.0f			// Some sensible limit
#define BP_LOW_XYZ_LIMIT						-2.0f			// Some sensible limit
#define BP_HIGH_XYZ_LIMIT						2.0f			//   - " -
#define BP_LOW_LIMIT							0
#define BP_HIGH_LIMIT							5

//---For peak to peak analysis 
#define PRESS_GRADIENT_LIMIT					.2f				// While doing peak to peak analysis. If the derviate of the filtered pressure curve is
																// below this value and no peaks are found, peak-to-peak is set to zero
#define PEAK_TO_PEAK_CURVE_SAMPLE_TIME			1.0f			// Second(s). The peak to peak data samples at this interval regardless. If no peak is found
																// it is set to the old pp value
#define PEAK_TO_PEAK_NO_PEAK_FOUND_LIMIT		5.0f			// If no peak is found after this and the derivate is zero, set the PP value to zero as 
																// e have a no flow or dead pressure condition
#define SWALLOW_LIMIT							2.0f			// If a positive peak is this many times higher than the old positive peak, it is 
																// skipped and replaced with the old positive peak value
//#define RSQUARED_LIMIT_FLAT_AREA				0.75f			// Require linear regression Rsquared less than this in order to declare it as flat
//#define SLOPE_LIMIT_FACTOR					0.1f			// Average slope (min to max) is divided by this value in order to declare it as flat
#define REL_BASELINE_FACTOR_FLAT_AREA_DETECTION	0.1f			// After baseline has been computed, the baseline is multiplied
																// with this value in order to see if the cure in this part is "flat"
#define MIN_FLAT_AREA_TIME_WINDOW				9.0f			// In order to set the envelopw to zero, it has to be flat for at least this time length

#define FLAG_NONE						0x0000
#define FLAG_VOLTAGE_LOW				0x0001
#define FLAG_FULL_CHARGE_CAPACITY_LOW	0x0002
#define FLAG_REMAINING_CAPACITY_LOW		0x0004
#define FLAG_TEMP_HIGH					0x0008
#define FLAG_BAD_OXIMETRY				0x0010
#define FLAG_BAD_ACTIMETER				0x0020
#define FLAG_BAD_CM						0x0040
#define FLAG_BAD_AM						0x0080
#define FLAG_BAD_BP						0x0100
#define FLAG_BAD_CATHETER				0x0200
#define FLAG_NO_DATA					0x0400
#define FLAG_HIGH_CURRENT				0x0800

#define BAD_CATHETER_DATA_LIMIT			1.0f // 1 % is acceptable

//---Oximeter flags

//---Smiths
#define NO_FLAGS						0x00
#define NO_SENSOR						0x01
#define NO_FINGER_IN_SENSOR				0x02
#define SEARCHING_FOR_PULSE				0x04
#define SEARCHING_TOO_LONG				0x08
#define LOST_PULSE						0x10

//--NONIN 3011
#define OXI_ALARM			0x38
#define OXI_DISCONNECTED	0x40
#define OXI_BAD_STATUS		0x78

const float deg75		= 1.309f;
const float deg65		= 1.135f;
const float deg45		= 0.785f;
const float deg40		= 0.698f;
const float deg50		= 0.873f;
const float deg130		= 2.269f;
const float deg140		= 2.444f;
const float deg_40		= - 0.698f;
const float deg_50		= - 0.873f;
const float deg_130		= - 2.269f;
const float deg_140		= - 2.444f;

enum pulseRateHisto {
	pulseAbove120,
	pulseFrom110to120,
	pulseFrom100to110,
	pulseFrom90to100,
	pulseFrom80to90,
	pulseFrom70to80,
	pulseFrom60to70,
	pulseBelow60,
	pulseInvalid,
	pulseBinCount
};

enum spO2SatHisto {
	satFrom95to100,
	satFrom90to95,
	satFrom85to90,
	satFrom80to85,
	satFrom75to80,
	satFrom70to75,
	satFrom65to70,
	satBelow65,
	satInvalid,
	satTotal,
	satBinCount
};

//---Positions
enum bopyPositions {
	posSupine,
	posLeft,
	posRight,	
	posProne,	
	posUpright,	
	posUndefined,
	posSum,				// Used when counting events vs body pos
	posCount
};

const CStringW posTxtXML[] = {
	L"posSupine",
	L"posLeft",
	L"posRight",	
	L"posProne",	
	L"posUpright",	
	L"posUndefined",
	L"posSum",
	L"posCount"
};

typedef struct aMark {
	float time;
	CString text;
} MARK;

typedef struct aTimeWindow {
	float begin, end;
} TIME_WINDOW;

typedef struct anEventSorterVectorElement {
	float begin;
	float end;
	void *evP;
	int level;
	int hypopneaDropLimit;
} EVENT_SORT_VECTOR_ELEMENT;

class CEvnt;

typedef struct anEventSorterVectorElementNew {
	float begin;
	CEvnt *evP;
} EVENT_SORT_VECTOR_ELEMENT_NEW;

const short sickCathHighLimit = 7000;
const short sickCathlowLimit = - 2000;
/*
Description: Computes the parameters for the vector.
*/
class CKeyData
{
public:
	CKeyData();
	~CKeyData();
	void setData(vector <FLOAT> *_v,vector <unsigned short> *_f = NULL);

	void Serialize(CArchive& ar, int majorFileVersion, int minorFileVersion);
	
	float mean;					// Mean values
	float medi;					// Median values
	float minimum;				// Minimum
	float maximum;				// Minimum
	float stdD;					// Standard deviation
};

/*
Vector process

raw->	filtered->	derivative->	removeMedian	->	envelope	->	peak-to-peak	->	baseline

Values can be added as raw, filtered, derivative. Preceding steps vectors will the be disabled.
*/


//!*--HPFilter  , cutoff 0.010,DC_NOT_INCL,HANNING,len 51--*/
const float filterBP_high[] = { 0.000000,
-0.000092f,0.000214f,-0.000983f,0.000549f,-0.003117f,0.000586f,-0.006724f,0.000000f,
-0.011849f,-0.001391f,-0.018334f,-0.003604f,-0.025820f,-0.006498f,-0.033783f,0.009791f,
-0.041588f,-0.013114f,-0.048563f,-0.016066f,-0.054074f,-0.018275f,-0.057607f,-0.019456f,
0.941176f,-0.019456f,-0.057607f,-0.018275f,-0.054074f,-0.016066f,-0.048563f,-0.013114f,
-0.041588f,-0.009791f,-0.033783f,-0.006498f,-0.025820f,-0.003604f,-0.018334f,-0.001391f,
-0.011849f,0.000000f,-0.006724f,0.000586f,-0.003117f,0.000549f,-0.000983f,0.000214f,
-0.000092f,0.000000 };

//!---LP Filter, cutoff 0.100,DC_NOT_INCL,BLACKMAN,len 11--*/
const float filterLP[] = { 0.000000,
0.003037f,0.023905f,0.077975f,0.148151f,
0.181818f,0.148151f,0.077975f,0.023905f,
0.003037f,0.000000 };

//---Hanning window for actimetry analysis.
//		based on 30 sec epochs
//		5 minutes long
//		11 elements
const int hanningWindowTaps = 11;
const float hanningScaleFactor = 5.0f;
const float hanningW[] = { 
	.0000f,
	.0955f,
	.3455f,
	.6545f,
	.9045f,
	1.000f,
	.9045f,
	.6545f,
	.3455f,
	.0955f,
	.0000f
};

#define FILTER_DESIGN_SAMPLE_INTERVAL		.25f		// Second
#define MIN_TIME_AT_EXTREMAL_SPO2_PR			2.0f		// Seconds


/**************************************************************
WinFilter version 0.8
http://www.winfilter.20m.com
akundert@hotmail.com

Filter type: High Pass
Filter model: Chebyshev
Filter order: 2
Sampling Frequency: 25 Hz
Cut Frequency: 10.000000 Hz
Pass band Ripple: 1.000000 dB
Coefficents Quantization: float

Z domain Zeros
z = 1.000000 + j 0.000000
z = 1.000000 + j 0.000000

Z domain Poles
z = -0.637119 + j -0.371744
z = -0.637119 + j 0.371744

With 0.25 Hz sampling, this means cutoff 0.1 Hz
***************************************************************/
#define NtapChebHP 31

const float FIRCoefChebHP[NtapChebHP] = {
		-0.00032306258210894567f,
		0.00119415085933417490f,
		-0.00226031092437356790f,
		0.00312924311884467360f,
		-0.00312360659749497420f,
		0.00136005116994978380f,
		0.00298063398297217060f,
		-0.01013597625796210300f,
		0.01900301579883943000f,
		-0.02625869782979638900f,
		0.02563886529747747700f,
		-0.00782526097631598350f,
		-0.03849545628973231900f,
		0.12387390411363101000f,
		-0.25341103989381780000f,
		0.32934324918815255000f,
		-0.25341103989381780000f,
		0.12387390411363101000f,
		-0.03849545628973231900f,
		-0.00782526097631598350f,
		0.02563886529747747700f,
		-0.02625869782979638900f,
		0.01900301579883943000f,
		-0.01013597625796210300f,
		0.00298063398297217060f,
		0.00136005116994978380f,
		-0.00312360659749497420f,
		0.00312924311884467360f,
		-0.00226031092437356790f,
		0.00119415085933417490f,
		-0.00032306258210894567f
	};

/////////////////////////////////////////
//  BP Chebyshev. 0.04 Hz to 0.5 Hz
////////////////////////////////////////

/**************************************************************
WinFilter version 0.8
http://www.winfilter.20m.com
akundert@hotmail.com

Filter type: Band Pass
Filter model: Chebyshev
Filter order: 4
Sampling Frequency: 100 Hz
Fc1 and Fc2 Frequencies: 1.000000 Hz and 12.000000 Hz
Pass band Ripple: 1.000000 dB
Coefficents Quantization: float

Z domain Zeros
z = -1.000000 + j 0.000000
z = -1.000000 + j 0.000000
z = -1.000000 + j 0.000000
z = -1.000000 + j 0.000000
z = 1.000000 + j 0.000000
z = 1.000000 + j 0.000000
z = 1.000000 + j 0.000000
z = 1.000000 + j 0.000000

Z domain Poles
z = 0.763699 + j -0.319376
z = 0.763699 + j 0.319376
z = 0.946875 + j -0.095657
z = 0.946875 + j 0.095657
z = 0.990482 + j -0.062373
z = 0.990482 + j 0.062373
z = 0.677499 + j -0.624654
z = 0.677499 + j 0.624654
***************************************************************/
#define NtapCheb 81

//float fir(float NewSample) {
const float Cheb_FIRCoef[NtapCheb] = {
		-0.00643715566700127880f,
		-0.00785587083648607580f,
		-0.00862041528955733230f,
		-0.00848747022968586450f,
		-0.00770991200479668150f,
		-0.00699411340410473290f,
		-0.00717798387872725220f,
		-0.00875785506553260210f,
		-0.01151642954016409000f,
		-0.01449442819923042900f,
		-0.01639950217402183100f,
		-0.01630858524144405600f,
		-0.01430362508032382300f,
		-0.01164946398954445400f,
		-0.01029960539703571800f,
		-0.01186805338144795300f,
		-0.01656704428430702900f,
		-0.02272730631337874200f,
		-0.02733177752464494000f,
		-0.02748113627056833100f,
		-0.02212570614639655300f,
		-0.01311592798784714000f,
		-0.00476891265253211620f,
		-0.00187739784358044170f,
		-0.00699858858802116300f,
		-0.01844729373146059000f,
		-0.03041568622659901100f,
		-0.03562391327268393100f,
		-0.02952398254182725400f,
		-0.01392506981038585200f,
		0.00245506992189020420f,
		0.00762071651673440400f,
		-0.00678699377294921840f,
		-0.03845774913925426000f,
		-0.07166381142167320400f,
		-0.08145261403087927800f,
		-0.04502961647585436400f,
		0.04409347703837845900f,
		0.16624419216953723000f,
		0.27573876997462043000f,
		0.32029384701309083000f,
		0.27573876997462043000f,
		0.16624419216953723000f,
		0.04409347703837845900f,
		-0.04502961647585436400f,
		-0.08145261403087927800f,
		-0.07166381142167320400f,
		-0.03845774913925426000f,
		-0.00678699377294921840f,
		0.00762071651673440400f,
		0.00245506992189020420f,
		-0.01392506981038585200f,
		-0.02952398254182725400f,
		-0.03562391327268393100f,
		-0.03041568622659901100f,
		-0.01844729373146059000f,
		-0.00699858858802116300f,
		-0.00187739784358044170f,
		-0.00476891265253211620f,
		-0.01311592798784714000f,
		-0.02212570614639655300f,
		-0.02748113627056833100f,
		-0.02733177752464494000f,
		-0.02272730631337874200f,
		-0.01656704428430702900f,
		-0.01186805338144795300f,
		-0.01029960539703571800f,
		-0.01164946398954445400f,
		-0.01430362508032382300f,
		-0.01630858524144405600f,
		-0.01639950217402183100f,
		-0.01449442819923042900f,
		-0.01151642954016409000f,
		-0.00875785506553260210f,
		-0.00717798387872725220f,
		-0.00699411340410473290f,
		-0.00770991200479668150f,
		-0.00848747022968586450f,
		-0.00862041528955733230f,
		-0.00785587083648607580f,
		-0.00643715566700127880f
	};

///////////////////////////////////////////////////////////////////////////////////////////
//		Raised Cosine Band Pass filter .04 Hz to 0.5 Hz
///////////////////////////////////////////////////////////////////////////////////////////

/**************************************************************
WinFilter version 0.8
http://www.winfilter.20m.com
akundert@hotmail.com

Filter type: Band Pass
Filter model: Raised Cosine
Roll Off Factor: 0.500000
Sampling Frequency: 100 Hz
Fc1 and Fc2 Frequencies: 1.000000 Hz and 12.000000 Hz
Coefficents Quantization: float
***************************************************************/
#define NtapBPCosine 31

//float fir(float NewSample) {
const float BPCosine_FIRCoef[NtapBPCosine] = {
		-0.03874376306471825200f,
		-0.03954785768506009800f,
		-0.02827690003315578100f,
		-0.00969650659096471700f,
		0.00648940473072731010f,
		0.01014451337589292400f,
		-0.00383429020013226380f,
		-0.03175936008973252700f,
		-0.06122291821178016400f,
		-0.07546486357596650400f,
		-0.06047545781801926700f,
		-0.01181794729590305400f,
		0.06197981194501994700f,
		0.14128851756416863000f,
		0.20207206863280291000f,
		0.22482428555245884000f,
		0.20207206863280291000f,
		0.14128851756416863000f,
		0.06197981194501994700f,
		-0.01181794729590305400f,
		-0.06047545781801926700f,
		-0.07546486357596650400f,
		-0.06122291821178016400f,
		-0.03175936008973252700f,
		-0.00383429020013226380f,
		0.01014451337589292400f,
		0.00648940473072731010f,
		-0.00969650659096471700f,
		-0.02827690003315578100f,
		-0.03954785768506009800f,
		-0.03874376306471825200f
	};


///////////////////////////////////////////////////////////////////////////////////////////
//		Band Pass filter .04 Hz to 1 Hz
///////////////////////////////////////////////////////////////////////////////////////////

/**************************************************************
WinFilter version 0.8
http://www.winfilter.20m.com
akundert@hotmail.com

Filter type: Band Pass
Filter model: Chebyshev
Filter order: 4
Sampling Frequency: 100 Hz
Fc1 and Fc2 Frequencies: 1.000000 Hz and 25.000000 Hz
Pass band Ripple: 1.000000 dB
Coefficents Quantization: float

Z domain Zeros
z = -1.000000 + j 0.000000
z = -1.000000 + j 0.000000
z = -1.000000 + j 0.000000
z = -1.000000 + j 0.000000
z = 1.000000 + j 0.000000
z = 1.000000 + j 0.000000
z = 1.000000 + j 0.000000
z = 1.000000 + j 0.000000

Z domain Poles
z = 0.383203 + j -0.474480
z = 0.383203 + j 0.474480
z = 0.006615 + j -0.875679
z = 0.006615 + j 0.875679
z = 0.932479 + j -0.092164
z = 0.932479 + j 0.092164
z = 0.989735 + j -0.062189
z = 0.989735 + j 0.062189
***************************************************************/
#define NtapBP 81

//float fir(float NewSample) {
const float BP_FIRCoef[NtapBP] = { 
        -0.00543770600393509580f,
        -0.00623031450189940810f,
        -0.00674618553548293930f,
        -0.00676293983315598510f,
        -0.00720024789140188880f,
        -0.00838197292485161160f,
        -0.00906193387935792100f,
        -0.00879000406450790130f,
        -0.00924140515790652750f,
        -0.01105846539534466100f,
        -0.01196957322945752100f,
        -0.01106870029657010600f,
        -0.01142094659792554400f,
        -0.01429733546397367500f,
        -0.01554079050393085200f,
        -0.01334168537163497500f,
        -0.01333584430935310500f,
        -0.01804418531890918800f,
        -0.01980111971535743200f,
        -0.01501070615660957600f,
        -0.01412165885169236500f,
        -0.02216226915481825700f,
        -0.02483515526575960600f,
        -0.01497091674355647200f,
        -0.01216582577128143100f,
        -0.02647939981918803100f,
        -0.03081167074890135300f,
        -0.01093832429851223000f,
        -0.00472040248795844740f,
        -0.03252266212199654600f,
        -0.04074263412142736800f,
        0.00081186964414421583f,
        0.01726599509103217800f,
        -0.03474602385879511900f,
        -0.05036421791577819200f,
        0.02273295154328777000f,
        0.01705728320093470200f,
        -0.12827340680579621000f,
        -0.07668265016363687300f,
        0.35318774213630344000f,
        0.64203777554065888000f,
        0.35318774213630344000f,
        -0.07668265016363687300f,
        -0.12827340680579621000f,
        0.01705728320093470200f,
        0.02273295154328777000f,
        -0.05036421791577819200f,
        -0.03474602385879511900f,
        0.01726599509103217800f,
        0.00081186964414421583f,
        -0.04074263412142736800f,
        -0.03252266212199654600f,
        -0.00472040248795844740f,
        -0.01093832429851223000f,
        -0.03081167074890135300f,
        -0.02647939981918803100f,
        -0.01216582577128143100f,
        -0.01497091674355647200f,
        -0.02483515526575960600f,
        -0.02216226915481825700f,
        -0.01412165885169236500f,
        -0.01501070615660957600f,
        -0.01980111971535743200f,
        -0.01804418531890918800f,
        -0.01333584430935310500f,
        -0.01334168537163497500f,
        -0.01554079050393085200f,
        -0.01429733546397367500f,
        -0.01142094659792554400f,
        -0.01106870029657010600f,
        -0.01196957322945752100f,
        -0.01105846539534466100f,
        -0.00924140515790652750f,
        -0.00879000406450790130f,
        -0.00906193387935792100f,
        -0.00838197292485161160f,
        -0.00720024789140188880f,
        -0.00676293983315598510f,
        -0.00674618553548293930f,
        -0.00623031450189940810f,
        -0.00543770600393509580f
    };

////////////////////////////////////////////////////////////////////////////////////
//			Low pass Filter 1 Hz
////////////////////////////////////////////////////////////////////////////////////


	/*************************************
WinFilter version 0.8
http://www.winfilter.20m.com
akundert@hotmail.com

Filter type: Low Pass
Filter model: Chebyshev
Filter order: 4
Sampling Frequency: 4 Hz
Cut Frequency: 1.000000 Hz
Pass band Ripple: 1.000000 dB
Coefficents Quantization: float

Z domain Zeros
z = -1.000000 + j 0.000000
z = -1.000000 + j 0.000000
z = -1.000000 + j 0.000000
z = -1.000000 + j 0.000000

Z domain Poles
z = 0.368946 + j -0.417102
z = 0.368946 + j 0.417102
z = 0.005957 + j -0.868105
z = 0.005957 + j 0.868105
***************************************************************/
#define NtapLP1Hz 81

//float fir(float NewSample) {
const float LP_FIRCoef1Hz[NtapLP1Hz] = { 
        0.00011074013365058947f,
        -0.00011442245447477164f,
        -0.00015413667860172329f,
        0.00015550306983343207f,
        0.00021456282319039946f,
        -0.00021144123787254735f,
        -0.00029875426916340416f,
        0.00028771257229658965f,
        0.00041615538050291041f,
        -0.00039188700074191808f,
        -0.00058004159758600509f,
        0.00053448536202475438f,
        0.00080913741517316140f,
        -0.00073021010899250883f,
        -0.00112994922249317960f,
        0.00099972680721461377f,
        0.00158006912342623010f,
        -0.00137245804484090050f,
        -0.00221300115928294020f,
        0.00189134760115200930f,
        0.00310638459700515170f,
        -0.00261940891423414990f,
        -0.00437564886005221370f,
        0.00364419289631757710f,
        0.00618384158561968280f,
        -0.00509215085500702970f,
        -0.00873251407997618220f,
        0.00723507486840302240f,
        0.01235743402610091200f,
        -0.01067742330632579000f,
        -0.01808134529306385300f,
        0.01572933829817250000f,
        0.02755330330010386700f,
        -0.02038584588497306300f,
        -0.03676036373954087000f,
        0.02837994505438142700f,
        0.02944594121992612200f,
        -0.09503563624722571900f,
        -0.05174833697091810900f,
        0.32772615272641492000f,
        0.58468785412891366000f,
        0.32772615272641492000f,
        -0.05174833697091810900f,
        -0.09503563624722571900f,
        0.02944594121992612200f,
        0.02837994505438142700f,
        -0.03676036373954087000f,
        -0.02038584588497306300f,
        0.02755330330010386700f,
        0.01572933829817250000f,
        -0.01808134529306385300f,
        -0.01067742330632579000f,
        0.01235743402610091200f,
        0.00723507486840302240f,
        -0.00873251407997618220f,
        -0.00509215085500702970f,
        0.00618384158561968280f,
        0.00364419289631757710f,
        -0.00437564886005221370f,
        -0.00261940891423414990f,
        0.00310638459700515170f,
        0.00189134760115200930f,
        -0.00221300115928294020f,
        -0.00137245804484090050f,
        0.00158006912342623010f,
        0.00099972680721461377f,
        -0.00112994922249317960f,
        -0.00073021010899250883f,
        0.00080913741517316140f,
        0.00053448536202475438f,
        -0.00058004159758600509f,
        -0.00039188700074191808f,
        0.00041615538050291041f,
        0.00028771257229658965f,
        -0.00029875426916340416f,
        -0.00021144123787254735f,
        0.00021456282319039946f,
        0.00015550306983343207f,
        -0.00015413667860172329f,
        -0.00011442245447477164f,
        0.00011074013365058947f
    };

/////////////////////////////////////////////////////////////
//   LP Filter 0.2 Hz
////////////////////////////////////////////////////////////
/**************************************************************
WinFilter version 0.8
http://www.winfilter.20m.com
akundert@hotmail.com

Filter type: Low Pass
Filter model: Chebyshev
Filter order: 4
Sampling Frequency: 20 Hz
Cut Frequency: 1.000000 Hz
Pass band Ripple: 1.000000 dB
Coefficents Quantization: float

Z domain Zeros
z = -1.000000 + j 0.000000
z = -1.000000 + j 0.000000
z = -1.000000 + j 0.000000
z = -1.000000 + j 0.000000

Z domain Poles
z = 0.891600 + j -0.115854
z = 0.891600 + j 0.115854
z = 0.912349 + j -0.291412
z = 0.912349 + j 0.291412
***************************************************************/
#define NtapLP02 81

//float fir(float NewSample) {
const float LP02FIRCoef[NtapLP02] = { 
        0.00337013140882983650f,
        0.00259556397637692670f,
        0.00144863710450582830f,
        -0.00000373743202025453f,
        -0.00165530856191391540f,
        -0.00336676273139298360f,
        -0.00497627724706871910f,
        -0.00631378704025020810f,
        -0.00721794039410148920f,
        -0.00755430531513368290f,
        -0.00723307044573877340f,
        -0.00622430739689701280f,
        -0.00456886090945216650f,
        -0.00238313149701498880f,
        0.00014358312182639546f,
        0.00275993175064958610f,
        0.00517154086784175300f,
        0.00706739763944368070f,
        0.00815188737904894850f,
        0.00817998534484457200f,
        0.00699244641178025680f,
        0.00454740917282409760f,
        0.00094470356425737235f,
        -0.00356062791852777010f,
        -0.00855849550010011510f,
        -0.01349956846301437500f,
        -0.01772742521268408400f,
        -0.02052627602711321400f,
        -0.02118190737982522700f,
        -0.01905194879351659900f,
        -0.01364019801811934700f,
        -0.00466870745895705230f,
        0.00785923184676342970f,
        0.02361206888831987600f,
        0.04190515964261152400f,
        0.06170307486587503500f,
        0.08165214112846923300f,
        0.10014397229711115000f,
        0.11540810575616517000f,
        0.12562911944081670000f,
        0.12925310426896131000f,
        0.12562911944081670000f,
        0.11540810575616517000f,
        0.10014397229711115000f,
        0.08165214112846923300f,
        0.06170307486587503500f,
        0.04190515964261152400f,
        0.02361206888831987600f,
        0.00785923184676342970f,
        -0.00466870745895705230f,
        -0.01364019801811934700f,
        -0.01905194879351659900f,
        -0.02118190737982522700f,
        -0.02052627602711321400f,
        -0.01772742521268408400f,
        -0.01349956846301437500f,
        -0.00855849550010011510f,
        -0.00356062791852777010f,
        0.00094470356425737235f,
        0.00454740917282409760f,
        0.00699244641178025680f,
        0.00817998534484457200f,
        0.00815188737904894850f,
        0.00706739763944368070f,
        0.00517154086784175300f,
        0.00275993175064958610f,
        0.00014358312182639546f,
        -0.00238313149701498880f,
        -0.00456886090945216650f,
        -0.00622430739689701280f,
        -0.00723307044573877340f,
        -0.00755430531513368290f,
        -0.00721794039410148920f,
        -0.00631378704025020810f,
        -0.00497627724706871910f,
        -0.00336676273139298360f,
        -0.00165530856191391540f,
        -0.00000373743202025453f,
        0.00144863710450582830f,
        0.00259556397637692670f,
        0.00337013140882983650f
    };

////////////////////////////////////////
//		 Rect for SpO2
////////////////////////////////////////
/**************************************************************
WinFilter version 0.8
http://www.winfilter.20m.com
akundert@hotmail.com

Filter type: Low Pass
Filter model: Rectangular Window
Sampling Frequency: 20 Hz
Cut Frequency: 1.000000 Hz
Coefficents Quantization: float
***************************************************************/
#define NtapRect 20

//float fir(float NewSample) {
const float FIRRectCoef[NtapRect] = { 
        0.05f,
        0.05f,
        0.05f,
        0.05f,
        0.05f,
        0.05f,
        0.05f,
        0.05f,
        0.05f,
        0.05f,
        0.05f,
        0.05f,
        0.05f,
        0.05f,
        0.05f,
        0.05f,
        0.05f,
        0.05f,
        0.05f,
        0.05f
    };

////////////////////////////////
/**************************************************************
WinFilter version 0.8
http://www.winfilter.20m.com
akundert@hotmail.com

Filter type: Low Pass
Filter model: Rectangular Window
Sampling Frequency: 20 Hz
Cut Frequency: 1.000000 Hz
Coefficents Quantization: float
***************************************************************/
#define NtapRect2 40

//float fir(float NewSample) {
const float FIRRect2Coef[NtapRect2] = { 
        0.025f,
        0.025f,
        0.025f,
        0.025f,
        0.025f,
        0.025f,
        0.025f,
        0.025f,
        0.025f,
        0.025f,
        0.025f,
        0.025f,
        0.025f,
        0.025f,
        0.025f,
        0.025f,
        0.025f,
        0.025f,
        0.025f,
        0.025f,
        0.025f,
        0.025f,
        0.025f,
        0.025f,
        0.025f,
        0.025f,
        0.025f,
        0.025f,
        0.025f,
        0.025f,
        0.025f,
        0.025f,
        0.025f,
        0.025f,
        0.025f,
        0.025f,
        0.025f,
        0.025f,
        0.025f,
        0.025f
    };


///////////////////////////////////////
/**************************************************************
WinFilter version 0.8
http://www.winfilter.20m.com
akundert@hotmail.com

Filter type: Low Pass
Filter model: Rectangular Window
Sampling Frequency: 20 Hz
Cut Frequency: 1.000000 Hz
Coefficents Quantization: float
***************************************************************/
#define NtapRect3 60

//float fir(float NewSample) {
const float FIRRect3Coef[NtapRect3] = { 
        0.016666667f,
        0.016666667f,
        0.016666667f,
        0.016666667f,
        0.016666667f,
        0.016666667f,
        0.016666667f,
        0.016666667f,
        0.016666667f,
        0.016666667f,
        0.016666667f,
        0.016666667f,
        0.016666667f,
        0.016666667f,
        0.016666667f,
        0.016666667f,
        0.016666667f,
        0.016666667f,
        0.016666667f,
        0.016666667f,
        0.016666667f,
        0.016666667f,
        0.016666667f,
        0.016666667f,
        0.016666667f,
        0.016666667f,
        0.016666667f,
        0.016666667f,
        0.016666667f,
        0.016666667f,
        0.016666667f,
        0.016666667f,
        0.016666667f,
        0.016666667f,
        0.016666667f,
        0.016666667f,
        0.016666667f,
        0.016666667f,
        0.016666667f,
        0.016666667f,
        0.016666667f,
        0.016666667f,
        0.016666667f,
        0.016666667f,
        0.016666667f,
        0.016666667f,
        0.016666667f,
        0.016666667f,
        0.016666667f,
        0.016666667f,
        0.016666667f,
        0.016666667f,
        0.016666667f,
        0.016666667f,
        0.016666667f,
        0.016666667f,
        0.016666667f,
        0.016666667f,
        0.016666667f,
        0.016666667f
    };


template <class Type>
class Multiply
{
   private:
      Type Factor;   // The value to multiply by
   public:
      // Constructor initializes the value to multiply by
      Multiply ( const Type& _Val ) : Factor ( _Val ) {
      }

      // The function call for the element to be multiplied
      Type operator ( ) ( Type& elem ) const 
	  {
		  return Factor * elem;
      }
};

// The function object to determine the average  
class Average
{
private:
	long num;      // The number of elements  
	FLOAT sum;      // The sum of the elements  
public:
	// Constructor initializes the value to multiply by  
	Average() : num(0), sum(.0f)
	{
	}

	// The function call to process the next elment  
	void operator ( ) (FLOAT elem) \
	{
		num++;      // Increment the element count  
		sum += elem;   // Add the value to the partial sum  
	}

	// return Average  
	operator double()
	{
		if (0 == num) return .0f;
		return  static_cast <double> (sum) /
			static_cast <double> (num);
	}
};

// The function object to determine the standardDeviation 
class standardDeviation
{
private:
	long num;      // The number of elements  
	float sum;      // The sum of the elements  
	float offs;
public:
	// Constructor initializes the value to multiply by  
	standardDeviation(float _offs) : num(0), sum(.0f), offs(_offs)
	{
	}

	// The function call to process the next elment  
	void operator ( ) (FLOAT elem) \
	{
		num++;      // Increment the element count  
		sum += pow((elem - offs),2);   // Add the value to the partial sum  
	}

	// return Average  
	operator double()
	{
		double std = num > 1 ? static_cast <double> (sum) /
			static_cast <double> (num - 1) : .0f;
		std = num > 1 ? sqrt(std) : .0f;
		return std;
	}
};


// The function object multiplies an element by a Factor
template <class Type>
class MultValue
{
private:
	Type Factor;   // The value to multiply by
public:
	// Constructor initializes the value to multiply by
	MultValue(const Type& _Val) : Factor(_Val) {
	}

	// The function call for the element to be multiplied
	Type operator ( ) (Type& elem) const
	{
		return elem * Factor;
	}
};

template <class Type>
class Add
{
   private:
      Type add;   // The value to add
   public:
      // Constructor initializes the value to multiply by
      Add ( const Type& _add ) : add ( _add ) {
      }

      // The function call for the element to be added
      Type operator ( ) ( Type& elem ) const 
	  {
		  return add + elem;
      }
};

template <class Type>
class Difference
{
   public:
      // Constructor initializes the value to subtract
      Difference ( void )  {
      }

      // The function call for the element to be subtracted
      Type operator ( ) ( Type& elem1, Type& elem2 ) const 
	  {
		  return elem2 - elem1;
      }
};

template <class Type>
class Sum
{
public:
	// Constructor initializes the value to subtract
	Sum(void) {
	}

	// The function call for the element to be subtracted
	Type operator ( ) (Type& elem1, Type& elem2) const
	{
		return elem2 + elem1;
	}
};

/*
Description:
	Constructor must include a set of filter coefficients, the length of these as well as the input vector and 
	corresponding time axis.
*/
class CFilter
{
public:
	CFilter(void);
	~CFilter();
	void filter(vector <FLOAT> *_time,vector <FLOAT> *_v);
	void filter(vector <FLOAT> *_time,
						vector <FLOAT> *_v1,
						vector <FLOAT> *_v2);
	void filter(vector <FLOAT> *_time,
						vector <FLOAT> *_v1,
						vector <FLOAT> *_v2,
						vector <FLOAT> *_v3);
	void filter(vector <FLOAT> *_time,
						vector <FLOAT> *_v1,
						vector <FLOAT> *_v2,
						vector <FLOAT> *_v3,
						vector <FLOAT> *_v4);
	void filter(vector <FLOAT> *_time,
						vector <FLOAT> *_v1,
						vector <FLOAT> *_v2,
						vector <FLOAT> *_v3,
						vector <FLOAT> *_v4,
						vector <FLOAT> *_v5,
						vector <FLOAT> *_v6);
protected:
	void convolve(vector <FLOAT> *_in,vector <FLOAT> *_out);
	void cutFromStartAndEnd(vector <FLOAT> *_v,int _numToCut);
	void shiftVectorValues(vector <FLOAT> *_time,float _add);
	
	int taps;
	vector <FLOAT> coef;
};

class CLP_1Hz_Filter : public CFilter
{
public:
	CLP_1Hz_Filter();
	~CLP_1Hz_Filter();
};

class CChebHP_01Hz_Filter : public CFilter
{
public:
	CChebHP_01Hz_Filter();
	~CChebHP_01Hz_Filter();
};

class CLP_02Hz_Filter : public CFilter
{
public:
	CLP_02Hz_Filter();
	~CLP_02Hz_Filter();
};

class CLP_5sampleRect_Filter : public CFilter
{
public:
	CLP_5sampleRect_Filter();
	~CLP_5sampleRect_Filter();
};

class CLP_10sampleRect_Filter : public CFilter
{
public:
	CLP_10sampleRect_Filter();
	~CLP_10sampleRect_Filter();
};
class CLP_15sampleRect_Filter : public CFilter
{
public:
	CLP_15sampleRect_Filter();
	~CLP_15sampleRect_Filter();
};

class CBPFilter : public CFilter
{
public:
	CBPFilter();
	~CBPFilter();
};

class CBPCosineFilter : public CFilter
{
public:
	CBPCosineFilter();
	~CBPCosineFilter();
};

class CBPCheb1t012Filter : public CFilter
{
public:
	CBPCheb1t012Filter();
	~CBPCheb1t012Filter();
};

class CManualMarkerSet
{
public:
	CManualMarkerSet();
	~CManualMarkerSet();
	virtual void dataSetIsCompleteFromTorso(void);
	virtual void Serialize(CArchive& ar, int majorFileVersion, int minorFileVersion);
	void add(float _time);
	int getSize(void);
	vector <FLOAT> *getMMarkerVector(void);
	void fillManualEventArray(CArray <CManualMarkerEvnt *, CManualMarkerEvnt *> *_mmArray);
	void clear();
protected:
	vector <MARK> mMarkerTimes;
	vector <FLOAT> mMarkerStartStopVector;
};

class CDataSet
{
public:
	CDataSet();
	~CDataSet();
	void setDataType(const unsigned int _dataType,CString _internalName = _T(""));
	virtual void addToXYRaw(FLOAT _val,FLOAT _time);
	void addToXYRaw(FLOAT _val);
	virtual int getSize(void);
	virtual void dataSetIsCompleteFromAGS(void);
	virtual void dataSetIsCompleteFromTorso(void);
	virtual void dataSetIsCompleteFromAPN(void);
	virtual void clear(void);
	void setDataSint(FLOAT _sint);
	vector <FLOAT> *getDataVector(const unsigned int _type);
	vector <FLOAT> *getTimeVector(const unsigned int _type);
	vector <FLOAT> *getTimeaxisVector(void);
	vector <BOOL> *getPenDownVector(void);
	FLOAT getSampleInterval(const unsigned int _type);
	//void getXLimits(FLOAT *_min,FLOAT *_max);bUse getStart and getStop instead
	void setXLimits(FLOAT _min,FLOAT _max);
	static void findEnvelope(vector <FLOAT> *_v,FLOAT _sint,vector <FLOAT> *_env,vector <FLOAT> *_envTime);
	static void findPeakToPeak(vector <FLOAT> *_v,FLOAT _sint,vector <FLOAT> *_pp,vector <FLOAT> *_ppTime);
	static void findPeakToPeak(vector <FLOAT> *_v,vector <FLOAT> *_time,vector <FLOAT> *_pp,vector <FLOAT> *_ppTime,
		float *_avgSlope);
	static void findPeakToPeak2(vector <FLOAT> *_v, vector <FLOAT> *_time, vector <FLOAT> *_pp, vector <FLOAT> *_ppTime,
		float *_avgSlope);
	static void findPeakToPeak3(vector <FLOAT> *_v, vector <FLOAT> *_time, vector <FLOAT> *_pp, vector <FLOAT> *_ppTime);
	static void setFlatAreasToZero(vector <FLOAT> *_v,vector <FLOAT> *_time,vector <FLOAT> *_env,vector <FLOAT> *_envTime,vector <FLOAT> *_baseline);
	void makeDiffVector(vector <FLOAT> *_v1,vector <FLOAT> *_v2,vector <FLOAT> *_t1,vector <FLOAT> *_t2);
	void makeSumVector(vector <FLOAT> *_v1,vector <FLOAT> *_v2,vector <FLOAT> *_t1,vector <FLOAT> *_t2);
	
	void makeSumVector(vector <FLOAT> *_v1,vector <FLOAT> *_v2,vector <FLOAT> *_t1,vector <FLOAT> *_t2,vector <FLOAT> *_resv,vector <FLOAT> *_resvtime);
	void makeWeightedSumVector(float _weight1,float _weight2,vector <FLOAT> *_v1, vector <FLOAT> *_v2, vector <FLOAT> *_t1, vector <FLOAT> *_t2, vector <FLOAT> *_resv, vector <FLOAT> *_resvtime);
	static bool interpolate(vector <FLOAT> *_v, vector <FLOAT> *_t, FLOAT _tval, FLOAT *_out);

	void makeFrqVector(vector <FLOAT> *_v,const FLOAT _sint);
	void makeFrqVector(vector <FLOAT> *_v,vector <FLOAT> *_t,vector <FLOAT> *_resv,vector <FLOAT> *_resvtime);
	void makeAdmittanceVector(vector <FLOAT> *_press,vector <FLOAT> *_flow,vector <FLOAT> *_pressTime,vector <FLOAT> *_flowTime,vector <FLOAT> *_resv,vector <FLOAT> *_resvtime);
	virtual float getStart(void);
	virtual float getStop(void);
	virtual unsigned int evaluateData(void);
	virtual void Serialize(CArchive& ar, int majorFileVersion, int minorFileVersion);
	static bool makeDerivative(vector <FLOAT> *_v,vector <FLOAT> *_timev);
	static vector <FLOAT>::iterator getFirstPeak(vector <FLOAT>::iterator _begin,vector <FLOAT>::iterator _end);
	static bool findCountRamp(vector <FLOAT>::iterator _begin,vector <FLOAT>::iterator _end,float _addCount);
	static bool findPeak(vector <FLOAT>::iterator _begin,vector <FLOAT>::iterator _end,float _factor,bool _posDerivative);
	static bool findPeak2(vector <FLOAT>::iterator _begin,vector <FLOAT>::iterator _end,float _factorHead,float _factorTail,
		bool _posDerivative,int *_position);
	static bool getIsNotDecreasing(vector <FLOAT>::iterator _begin,vector <FLOAT>::iterator _end);
	static bool linReg(vector <FLOAT>::iterator _xbegin,vector <FLOAT>::iterator _xend,
		vector <FLOAT>::iterator _ybegin,vector <FLOAT>::iterator _yend,float *_a,float *_b,float *_rSquared);
	static float getAverage(float _from,float _to,vector <FLOAT> *_v,vector <FLOAT> *_t);
	static float getUpper95PercentValue(float _from,float _to,vector <FLOAT> *_v,vector <FLOAT> *_t);
	static bool getIsStable(vector <FLOAT>::iterator _fromIt, vector <FLOAT>::iterator _toIt);
	static float getIQR(vector <FLOAT>::iterator _fromIt, vector <FLOAT>::iterator _toIt, float *_median);
	static bool getIsStable(vector <FLOAT>::iterator _fromIt,vector <FLOAT>::iterator _toIt,float _limit);
	static bool getIsStable(vector <FLOAT>::iterator _fromIt,vector <FLOAT>::iterator _toIt,float _percentLimit,float *_baseline,bool _removeOutliers);
	static float getMedian(vector <FLOAT>::iterator _fromIt,vector <FLOAT>::iterator _toIt);
	static float getCentre(vector <FLOAT>::iterator _fromIt, vector <FLOAT>::iterator _toIt);
	static float getIQR(vector <FLOAT> *_v,float *_median);
	static void	removePosPeak(vector <FLOAT>::iterator _fromIt, vector <FLOAT>::iterator _toIt, 
									vector <FLOAT>::iterator _peakIt);
	static void	linInterpolate(vector <FLOAT>::iterator _fromIt, vector <FLOAT>::iterator _toIt);
	static FLOAT getMeanStandardDev(vector <FLOAT>::iterator _fromI, vector <FLOAT>::iterator _toI, FLOAT *_mean);
	static bool getHitBetweenStartAndStop(vector <FLOAT> *_v, float _t, float _edgeDetectTolerance, 
		bool *_between,bool *_onLeft,bool *_onRight,
		float *_startTime = NULL,float *_stopTime = NULL);
	static UINT countSetBits(UINT _n);
protected:
	float median_element(vector <FLOAT>::iterator _from, vector <FLOAT>::iterator _to);
	void generateBaselineVectorAASM(vector <FLOAT> *_v,vector <FLOAT> *_tv,vector <FLOAT> *_outv,float _percStabilityLimit,float _baselineLength,bool _removeOutliers);
	void generateBaselineVectorMedian(vector <FLOAT> *_v,vector <FLOAT> *_tv,vector <FLOAT> *_outv,float _baselineLength);
	void removeMedian(vector <FLOAT> *_v);
	CString internalName;	// Useful for debugging
	unsigned int dataType;
	vector <FLOAT> timeaxis;
	FLOAT dataSint;
	FLOAT minX,maxX;
	vector <FLOAT> xyRaw			;
	vector <FLOAT> Envelope			;
	vector <FLOAT> PeakToPeak		;
	vector <FLOAT> BaselineAvg		;
	vector <FLOAT> BaselineMax		;
	vector <FLOAT> xyRawTime		;
	vector <FLOAT> EnvelopeTime		;
	vector <FLOAT> PeakToPeakTime	;
	vector <FLOAT> BaselineAvgTime	;
	vector <FLOAT> BaselineMaxTime	;
	vector <BOOL>  penDown			;
};


class CBatteryDataSet : public CDataSet
{
public:
	CBatteryDataSet();
	~CBatteryDataSet();
	virtual void clear(void);
	virtual int getSize(void);
	virtual void dataSetIsCompleteFromAGS(void);
	virtual void dataSetIsCompleteFromTorso(void);
	virtual void dataSetIsCompleteFromAPN(void);
	void getStartValues(FLOAT *_voltage,FLOAT *_current,FLOAT *_temp,FLOAT *_remainingCap,FLOAT *_fullChargeCap,FLOAT *_remainingTime,FLOAT *_time);
	void getEndValues(FLOAT *_voltage,FLOAT *_current,FLOAT *_temp,FLOAT *_remainingCap,FLOAT *_fullChargeCap,FLOAT *_remainingTime,FLOAT *_time);
	virtual void addToXYRaw(FLOAT _voltage,FLOAT _current,FLOAT _temp,FLOAT _remainingCap,FLOAT _fullChargeCap,FLOAT _remainingTime,FLOAT _time);
	
	vector <FLOAT> *getVoltageVector(void);
	vector <FLOAT> *getCurrentVector(void);
	vector <FLOAT> *getTemperatureVector(void);
	vector <FLOAT> *getRemainingCapacityVector(void);
	vector <FLOAT> *getFullChargeCapacityVector(void);
	vector <FLOAT> *getRemainingTimeVector(void);
	virtual unsigned int evaluateData(void);
	void getVoltageMinMax(FLOAT *_min,FLOAT *_max,FLOAT *_avg);
	virtual void Serialize(CArchive& ar,int _mainFileVersion,int _minorFileVersion);
protected:
	vector <FLOAT> voltage			;
	vector <FLOAT> current			;
	vector <FLOAT> temp				;
	vector <FLOAT> remainingCap		;
	vector <FLOAT> fullChargeCap	;
	vector <FLOAT> remainingTime	;
	FLOAT maxVoltage,minVoltage,avgVoltage;
	FLOAT maxCurrent,minCurrent,avgCurrent;
	FLOAT maxTemp,minTemp,avgTemp;
	FLOAT maxRemainingCap,minRemainingCap,avgRemainingCap;
	FLOAT maxFullChargeCap,minFullChargeCap,avgFullChargeCap;
	FLOAT maxRemainingTime,minRemainingTime,avgRemainingTime;
};

class CContactMicDataSet : public CDataSet
{
public:
	CContactMicDataSet();
	~CContactMicDataSet();
	virtual void clear(void);
	virtual int getSize(void);
	virtual void addToXYRaw(FLOAT _cm,FLOAT _time);
	vector <FLOAT> *getCmVector(void);
	virtual unsigned int evaluateData(FLOAT *_percBad);
	virtual void Serialize(CArchive& ar,int majorFileVersion,int minorFileVersion);
protected:
	vector <FLOAT> cm;
};

class CAirMicDataSet : public CDataSet
{
public:
	CAirMicDataSet();
	~CAirMicDataSet();
	virtual void clear(void);
	virtual int getSize(void);
	virtual void addToXYRaw(FLOAT _am1,FLOAT _am2,FLOAT _am3,FLOAT _am4,FLOAT _time);
	vector <FLOAT> *getAm1Vector(void);
	vector <FLOAT> *getAm2Vector(void);
	vector <FLOAT> *getAm3Vector(void);
	vector <FLOAT> *getAm4Vector(void);
	vector <FLOAT> *getAmSumVector(void);
	virtual void dataSetIsCompleteFromTorso(void);
	virtual unsigned int evaluateData(FLOAT *_percBad);
	virtual void Serialize(CArchive& ar, int majorFileVersion, int minorFileVersion);
protected:
	vector <FLOAT> am1,am2,am3,am4,amSum;
};

class CBodyPosXYZDataSet : public CDataSet
{
public:
	CBodyPosXYZDataSet();
	~CBodyPosXYZDataSet();
	virtual void clear(void);
	virtual int getSize(void);
	virtual void addToXYRaw(FLOAT _x,FLOAT _y,FLOAT _z,FLOAT _time);
	vector <FLOAT> *getxVector(void);
	vector <FLOAT> *getyVector(void);
	vector <FLOAT> *getzVector(void);
	virtual void dataSetIsCompleteFromAGS(void);
	virtual void dataSetIsCompleteFromTorso(void);
	virtual void dataSetIsCompleteFromAPN(void);
	virtual unsigned int evaluateData(FLOAT *_percBad);
	virtual void Serialize(CArchive& ar, int majorFileVersion, int minorFileVersion);
protected:
	vector <FLOAT> x,y,z;
};

class CBodyPosDataSet : public CDataSet
{
public:
	CBodyPosDataSet();
	~CBodyPosDataSet();
	virtual void clear(void);
	virtual int getSize(void);
	virtual void addToXYRaw(short _bp,FLOAT _time,bool _lastSample = false);
	vector <FLOAT> *getBpVector(void);
	virtual unsigned int evaluateData(FLOAT *_percBad);
	virtual void Serialize(CArchive& ar, int majorFileVersion, int minorFileVersion);
	void generateDataFromXYZ(vector <FLOAT> *_x,
							vector <FLOAT> *_y,
							vector <FLOAT> *_z,
							vector <FLOAT> *_time);
	short getPos(vector <FLOAT>::iterator _startInterval,vector <FLOAT>::iterator _stopInterval);
	short getPos(float _startInterval,float _stopInterval);
	bool getIUsUprightInWindow(float _startInterval, float _stopInterval);
	float getTimeInPosLeft(void);
	float getTimeInPosRight(void);
	float getTimeInPosSupine(void);
	float getTimeInPosProne(void);
	float getTimeInPosUpright(void);
	float getTimeInPosUndef(void);

	float getTimeSleepingInPosLeft(void);
	float getTimeSleepingInPosRight(void);
	float getTimeSleepingInPosSupine(void);
	float getTimeSleepingInPosProne(void);
	float getTimeSleepingInPosUpright(void);
	float getTimeSleepingInPosUndef(void);

	float getTimeAwakeInPosLeft(void);
	float getTimeAwakeInPosRight(void);
	float getTimeAwakeInPosSupine(void);
	float getTimeAwakeInPosProne(void);
	float getTimeAwakeInPosUpright(void);
	float getTimeAwakeInPosUndef(void);

	void computeTimeInPositions(CArray <CExcludedEvnt *, CExcludedEvnt *> *_excludedEventArray);
	void computeTimeSleepingInPositions(CArray <CExcludedEvnt *, CExcludedEvnt *> *_excludedEventArray,
										CArray <CAwakeEvnt *, CAwakeEvnt *> *_awakeEventArray);
	float getPercentInSupine(void);
protected:
	float getTimeExcluded(CArray <CExcludedEvnt *, CExcludedEvnt *> *_excludedEventArray, float _t0, float _t1);
	float getTimeAwakeAndNotExcluded(CArray <CExcludedEvnt *, CExcludedEvnt *> *_excludedEventArray,
		CArray <CAwakeEvnt *, CAwakeEvnt *> *_awakeEventArray, float _t0, float _t1);
	vector <FLOAT> bp;
	float timeInPosLeft;
	float timeInPosRight;
	float timeInPosSupine;
	float timeInPosProne;
	float timeInPosUpright;
	float timeInPosUndef;

	float timeSleepingInPosLeft;
	float timeSleepingInPosRight;
	float timeSleepingInPosSupine;
	float timeSleepingInPosProne;
	float timeSleepingInPosUpright;
	float timeSleepingInPosUndef;

	float timeAwakeInPosLeft;
	float timeAwakeInPosRight;
	float timeAwakeInPosSupine;
	float timeAwakeInPosProne;
	float timeAwakeInPosUpright;
	float timeAwakeInPosUndef;
};

class CActimeterDataSet : public CDataSet
{
public:
	CActimeterDataSet();
	~CActimeterDataSet();
	virtual void clear(void);
	virtual int getSize(void);
	float getEpochLength(void);
	virtual void addToXYRaw(FLOAT _ac,unsigned short _flags,FLOAT _time);
	void addToRawComponents(FLOAT _x,FLOAT _y,FLOAT _z, FLOAT _time);
	vector <FLOAT> *getAcVector(void);
	vector <FLOAT> *getWindowedAcVector(void);
	vector <FLOAT> *getWindowedAcTimeVector(void);
	vector <FLOAT> *getBaselineVector(void);
	virtual unsigned int evaluateData(FLOAT *_percBad);
	virtual void dataSetIsCompleteFromTorso(void);
	virtual void Serialize(CArchive& ar, int majorFileVersion, int minorFileVersion);
	float getActimeterSatLimit(void);
	void generateFromBodyXYZ(CBodyPosXYZDataSet *_bpxyz);
protected:
	vector <FLOAT> x, y, z;
	vector <FLOAT> baselineVector;		// Baseline vector
	vector <FLOAT> hanningWindow;
	vector <FLOAT> ac;
	vector <FLOAT> ac30secVector;		// 30 sec epoch vector
	vector <FLOAT> ac30secTimeVector;	// 30 sec epoch time vector
	vector <unsigned short> flags;
};

class COximeterDataSet : public CDataSet
{
public:
	COximeterDataSet();
	~COximeterDataSet();
	virtual void clear(void);
	virtual int getSize(void);
	vector <FLOAT> *getSpO2Vector(void);
	vector <FLOAT> *getPulseRateVector(void);
	vector <unsigned short> *getFlagsVector(void);
	virtual void addToXYRaw(FLOAT _samplingRate,FLOAT _O2avg2,FLOAT _O2avg8,FLOAT _pulseRate,unsigned short _flags,FLOAT _time);
	virtual unsigned int evaluateData(float *_percBad);
	virtual void Serialize(CArchive& ar, int majorFileVersion, int minorFileVersion);
	virtual void dataSetIsCompleteFromAGS(void);
	virtual void dataSetIsCompleteFromTorso(void);
	virtual void dataSetIsCompleteFromAPN(void); 
	//void computeKeyData(CArray <CExcludedEvnt *, CExcludedEvnt *> *_excludedEventArray,
	//													CArray <CAwakeEvnt *, CAwakeEvnt *> *_awakeEventArray);
	void getO2KeyData(float *_mean,float *_median,float *_min,float *_max,float *_timeOfMin,float *_timeOfMax);
	void getPRKeyData(float *_mean,float *_median,float *_min,float *_max,float *_timeOfMin,float *_timeOfMax);
	void getSpO2HistogramData8(float *_first);
	void getPulseRateHistogramData(float *_first);
	float getInvalidTime(void);
	float getValidTime(void); 
	bool getIsDataValidAtTime(float _time);
	bool getIsPenDown(float _time);
	float getSpO2At(float _t);
	void setPenDownVector(BOOL _down);
protected:
	vector <TIME_WINDOW> invalidTimeWindows;
	//void populateHistograms(vector <EVENT_SORT_VECTOR_ELEMENT> *_exclAwakeVector);
	bool getIsExcluded(float _time, CArray <CExcludedEvnt *, CExcludedEvnt *> *_excludedEventArray);
	bool getIsExcluded(size_t _index, CArray <CExcludedEvnt *, CExcludedEvnt *> *_excludedEventArray);

	static bool areNOrMoreAboveLimit(int _minSamples, vector <FLOAT>::iterator first, vector <FLOAT>::iterator last, size_t *_start, size_t *_end, float _limit, int *_actualSamples);
	static bool areNOrMoreBelowLimit(int _minSamples, vector <FLOAT>::iterator first, vector <FLOAT>::iterator last, size_t *_start, size_t *_end, float _limit, int *_actualSamples);


	void findInvalidAndValidTimeAndFilter(void);
	vector <FLOAT> o2avg2,pulseRate;
	vector <unsigned short> flags;
	//void toSpO2Histogram(float _secs,float _saturation);
	//void toPRHistogram(float _secs,float _pr);
	float meanO2avg2,meanPulseRate;						// Mean values
	float medianO2avg2,medianPulseRate;					// Median values
	float minO2Avg2,maxO2Avg2;							// Minimum and maximum of SaO2 2 pulse average
	float minPulseRate,maxPulseRate;					// Minimum and maximum of pulse rate
	float timeAtMinO2Avg2,timeAtMaxO2Avg2;				// Time at Minimum and maximum of SaO2 2 pulse average
	float timeAtMinPulseRate, timeAtMaxPulseRate;		// Time at Minimum and maximum of pulse rate
	float spO2Histo[satBinCount];						// Histogram. Number of seconds in each interval
	float pulseRateHisto[pulseBinCount];				// Histogram. Number of seconds in each interval
	float invalidTime;									// Invalid time in seconds
	float validTime;									// Valid time in  seconds
	float getPRAt(float _t);
};

class CCatheterDataSet : public CDataSet
{
public:
	CCatheterDataSet();
	~CCatheterDataSet();
	virtual void clear(void);
	virtual int getSize(void);
	vector <FLOAT> *getPOESVector(void);
	vector <FLOAT> *getPPHVector(void);
	vector <FLOAT> *getT0Vector(void);
	vector <FLOAT> *getT1Vector(void);
	vector <FLOAT> *getRawTimeVector(void);

	vector <FLOAT> *getPOESRawVector(void);
	vector <FLOAT> *getPPHRawVector(void);
	vector <FLOAT> *getT0RawVector(void);
	vector <FLOAT> *getT1RawVector(void);

	vector <FLOAT> *getPgradientVector(void);
	vector <FLOAT> *getRespFrqVector(void);
	vector <FLOAT> *getAdmittanceVector(void);
	vector <FLOAT> *getPOESEnvVector(void);
	vector <FLOAT> *getPPHEnvVector(void);
	vector <FLOAT> *getT0EnvVector(void);
	vector <FLOAT> *getT1EnvVector(void);
	vector <FLOAT> *getT1EnvBaselineVector(void);
	vector <FLOAT> *getT0EnvBaselineVector(void);
	vector <FLOAT> *getPOESBaselineVector(void);
	vector <FLOAT> *getPPHBaselineVector(void);
	vector <FLOAT> *getT0plusT1Vector(void);

	vector <FLOAT> *getBFlowVector(void);
	vector <FLOAT> *getBFlowBaseline(void);
	vector <FLOAT> *getBEfficiencyVector(void);
	vector <FLOAT> *getBFlowVectorTime(void);
	vector <FLOAT> *getBEfficiencyVectorTime(void);

	vector <FLOAT> *getPgradientVectorTime(void);
	vector <FLOAT> *getRespFrqVectorTime(void);
	vector <FLOAT> *getAdmittanceVectorTime(void);
	vector <FLOAT> *getPOESEnvVectorTime(void);
	vector <FLOAT> *getPPHEnvVectorTime(void);
	vector <FLOAT> *getT0EnvVectorTime(void);
	vector <FLOAT> *getT1EnvVectorTime(void);
	vector <FLOAT> *getT0plusT1VectorTime(void);

	vector <FLOAT> *getFlowVector(void);
	vector <FLOAT> *getFlowVectorTime(void);
	vector <FLOAT> *getFlowBaselineVector(void);

	float getInvalidTime(void);
	float getValidTime(void);
	
	virtual void dataSetIsCompleteFromAGS(CATH_PARAMETER_SET *_par);
	virtual void dataSetIsCompleteFromTorso(CATH_PARAMETER_SET *_par);
	virtual void dataSetIsCompleteFromAPN(void);
	bool calibrate(CATH_PARAMETER_SET *_par);
	virtual void addToXYRaw(FLOAT _pOES,FLOAT _pPH,FLOAT _t0,FLOAT _t1,unsigned short _flags,FLOAT _time);
	void addUncalibrated(INT16 _a,INT16 _b,INT16 _c,INT16 _d,INT16 _e,float _t);
	virtual unsigned int evaluateData(float *_percBad);
	virtual void Serialize(CArchive& ar, int majorFileVersion, int minorFileVersion);
	CKeyData *getKeyDataPOES(void);
	CKeyData *getKeyDataPPH(void);
	CKeyData *getKeyDataT0plusT1(void);
	CKeyData *getKeyDataPGradient(void);
	CKeyData *getKeyDataRespFrq(void);
	CKeyData *getKeyDataAdmittance(void);
	CKeyData *getKeyDataPOESEnv(void);
	CKeyData *getKeyDataPPHEnv(void);
	vector <TIME_WINDOW> *getInvalidTimeWindowsVector(void);
	void resetSwallow(void);
	void fillSwallowArray(CArray <CSwallowEvnt *, CSwallowEvnt *> *_swallowEventArray); 
	void computeFlowAndBreathingEfficiencyForCatheter(float _balanceFactor = FLOW_DETECT_BALANCE_FACTOR); // k * T1 + (1.0 - k) * T0);
protected:
	vector <TIME_WINDOW> invalidCTimeWindows;
	float baselineLength;					// Seconds
	float t1EnvPercentStabilityLimit;		// In percent of median
	float t0EnvPercentStabilityLimit;		// In percent of median
	float pressureEnvPercentStabilityLimit;	// In percent of median

	float minTemp;		// Minimum temperature x 10 (degC) as read from TORSO_PARAM
	float maxTemp;		// Maximum temperature x 10 (degC) as read from TORSO_PARAM
	float minPress;		// Minimum pressure (cmH2O) as read from TORSO_PARAM
	float maxPress;		// Maximum pressure (cmH2O) as read from TORSO_PARAM

	vector <short> *ucA,*ucB,*ucC,*ucD,*ucE;
	vector <FLOAT> *uncalTime;
	vector <FLOAT> POES,PPH,T0,T1;
	vector <FLOAT> POESRaw, PPHRaw, T0Raw, T1Raw,rawTime;
	vector <FLOAT> pGradient;
	vector <FLOAT> respFrq,admittance;
	vector <FLOAT> pGradientTime,respFrqTime,admittanceTime;
	vector <FLOAT> POESEnv,PPHEnv,T0Env,T1Env,T0plusT1;
	vector <FLOAT> POESEnvTime,PPHEnvTime,T0EnvTime,T1EnvTime,T0plusT1Time;
	vector <FLOAT> bFlow, bEfficiency;
	vector <FLOAT> bFlowTime, bEfficiencyTime;
	vector <FLOAT> T1EnvBaseline;
	vector <FLOAT> T0EnvBaseline;
	vector <FLOAT> POESEnvBaseline;
	vector <FLOAT> PPHEnvBaseline;
	vector <FLOAT> bFlowBaseline;
	vector <FLOAT> outlierTimePPH;
	vector <FLOAT> outlierTimePOES;
	vector <FLOAT> swallowTimePPH;
	vector <FLOAT> swallowTimePOES;
	vector <unsigned short> flags;

	vector <FLOAT> POESOutlierfree, PPHOutlierfree;

	void swallowQualification(vector <FLOAT> *_outlPOES, vector <FLOAT> *_outlPPH,
		vector <FLOAT> *_swPOES, vector <FLOAT> *_swPPH);

	void positiveOutlierDetect(vector <FLOAT> *_timeaxis, vector <FLOAT> *_p, float _iqrFactor,vector <FLOAT> *_swT);
	 
	void removePositiveOutliers(vector <FLOAT> *_v, vector <FLOAT> *_vTime, vector <FLOAT> *_outlTime);
	void removePosPeak(vector <FLOAT> *_v, vector <FLOAT> *_vTime, unsigned int _offs);
	
	void makeGradientVector(vector <FLOAT> *_v1,vector <FLOAT> *_v2,vector <FLOAT> *_t1,vector <FLOAT> *_t2,
		vector <FLOAT> *_resv,vector <FLOAT> *_resvtime);

	CKeyData keyDataPOES,keyDataPPH,keyDataT0plusT1;
	CKeyData keyDataPGradient;
	CKeyData keyDataRespFrq,keyDataAdmittance;
	CKeyData keyDataPOESEnv,keyDataPPHEnv;
	CKeyData keyDataBFlow, keyDataBEfficiency;

	float invalidTime;			// Invalid time in seconds
	float validTime;			// Valid time in  seconds
};

class CAG200MicDataSet : public CDataSet
{
public:
	CAG200MicDataSet();
	~CAG200MicDataSet();
	virtual void clear(void);
	virtual int getSize(void);
	vector <FLOAT> *getAG200MicVector(void);
	virtual void addToXYRaw(FLOAT _mic,FLOAT _time);
	virtual unsigned int evaluateData(float *_percBad);
	virtual void Serialize(CArchive& ar, int majorFileVersion, int minorFileVersion);
protected:
	vector <FLOAT> ag200Mic;
};

class CRespBeltDataSet : public CDataSet
{
public:
	CRespBeltDataSet();
	~CRespBeltDataSet();
	virtual void clear(void);
	virtual int getSize(void);
	void addToXYRawCannula(FLOAT _cannula,FLOAT _time);
	void addToXYRawBelts(FLOAT _abdom,FLOAT _chest,FLOAT _time);
	vector <FLOAT> *getAbdomVector(void)			;
	vector <FLOAT> *getChestVector(void)			;
	vector <FLOAT> *getCannulaVector(void)			;	
	vector <FLOAT> *getAbdomEnvVector(void)			;
	vector <FLOAT> *getChestEnvVector(void)			;
	vector <FLOAT> *getCannulaEnvVector(void)		;
	vector <FLOAT> *getAbdomEnvVectorTime(void)		;
	vector <FLOAT> *getChestEnvVectorTime(void)		;
	vector <FLOAT> *getCannulaEnvVectorTime(void)	;
	vector <FLOAT> *getBeltSumVector(void)			;
	vector <FLOAT> *getBeltSumVectorTime(void)		;	
	vector <FLOAT> *getRespFrqVector(void)			;
	vector <FLOAT> *getRespFrqVectorTime(void)		;	
	vector <FLOAT> *getAdmittanceVector(void)		;	
	vector <FLOAT> *getAdmittanceVectorTime(void)	;
	vector <FLOAT> *getCannulaTime(void)			;
	vector <FLOAT> *getBeltTime(void)				;
	vector <FLOAT>* getBEfficiencyVector(void)		;
	vector <FLOAT>* getBEfficiencyVectorTime(void)	;
	vector <FLOAT>* getAbdomEnvBaseline(void)		;
	vector <FLOAT>* getChestEnvBaseline(void)		;
	vector <FLOAT>* getCannulaEnvBaseline(void)		;
	vector <FLOAT>* getBeltSumBaseline(void)		;
	
	virtual void dataSetIsCompleteFromAGS(void);
	virtual void dataSetIsCompleteFromTorso(void);
	virtual void dataSetIsCompleteFromAPN(void);
	virtual unsigned int evaluateData(FLOAT *_percBad);
	bool calibrate(void);
	virtual float getStart(void);
	virtual float getStop(void);
	virtual void Serialize(CArchive& ar, int majorFileVersion, int minorFileVersion);
protected:
	vector <FLOAT> abdom			;
	vector <FLOAT> chest			;
	vector <FLOAT> cannula			;
	vector <FLOAT> abdomEnv			;
	vector <FLOAT> chestEnv			;
	vector <FLOAT> cannulaEnv		;
	vector <FLOAT> abdomEnvTime		;
	vector <FLOAT> chestEnvTime		;
	vector <FLOAT> cannulaEnvTime	;
	vector <FLOAT> beltSum			;
	vector <FLOAT> beltSumTime		;
	vector <FLOAT> respFrq			;
	vector <FLOAT> respFrqTime		;
	vector <FLOAT> admittance		;
	vector <FLOAT> admittanceTime	;
	vector <FLOAT> beltTime			;
	vector <FLOAT> cannulaTime		;

	vector <FLOAT> abdomEnvBaseline		;
	vector <FLOAT> chestEnvBaseline		;
	vector <FLOAT> cannulaEnvBaseline	;
	vector <FLOAT> beltSumBaseline		;

	float baselineLength;					// Seconds
	float abdomEnvPercentStabilityLimit;	// In percent of median
	float chestEnvPercentStabilityLimit;	// In percent of median
	float cannulaEnvPercentStabilityLimit;	// In percent of median
};


#endif
