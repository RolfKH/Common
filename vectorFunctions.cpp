
#include "stdafx.h"
#include "vectorFunctions.h"
#include "Events.h"
#include "D2Graph.h"

/*!
*				
*		5.7.16		defMinPress		changed from -100 to -200
*					defMaxPress		changed from 100 to 200
*
*					Rewrote CCatheterDataSet::evaluateData(float *_percBad) - now using validTime & invalidTime 
					instead of just flags
		22.3.18  Changed _Inner_product() to inner_product()
*/

/*!
* @author		RKH
* @date		22.03.2018
* @copyright (C)Spiro Medical AS 2013 - 2018
*/

const float defMinTemp		= 200 ;		// Minimum temperature x 10 (degC) as read from TORSO_PARAM
const float defMaxTemp		= 450 ;		// Maximum temperature x 10 (degC) as read from TORSO_PARAM
const float defMinPress		= -200;		// Minimum pressure (cmH2O) as read from TORSO_PARAM
const float defMaxPress		= 200;		// Maximum pressure (cmH2O) as read from TORSO_PARAM
const float maxCathDataScale = 5000.0f;	// Maximum amplitude coming from catheter

extern int noCathFilter,noRespSensorFilter;
extern int skipBaselineDetection;
extern int skipFlatAreaDetection;

extern int oxNoFilter;
extern int oxUseRaw;


// The function object sets an element 
BOOL ConstTrue() { return TRUE; }
BOOL ConstFalse() { return FALSE; }


// The function object adds an element 
template <class Type>
class AddValue
{
   private:
      Type addVal;   // The value to add
   public:
      // Constructor initializes the value to add
      AddValue ( const Type& _Val ) : addVal ( _Val ) {
      }

      // The function call for the element 
      Type operator ( ) ( Type& elem ) const 
      {
         return elem + addVal;
      }
};

// The function object multiplies an element by a Factor
template <class Type>
class FabsAnddB
{
private:
	
public:
	// Constructor initializes the value to multiply by
	FabsAnddB() {
	}

	// The function call for the element to be multiplied
	Type operator ( ) (const Type& elem) const
	{
		Type e = elem < 0 ? -elem : elem;
		e = e < torsoActimeterLowestValue ? torsoActimeterLowestValue : e;
		e = 20 * log10(e);
		return e ;
	}
};

//// The function object multiplies an element by a Factor
//template <class Type>
//class MultValue
//{
//   private:
//      Type Factor;   // The value to multiply by
//   public:
//      // Constructor initializes the value to multiply by
//      MultValue ( const Type& _Val ) : Factor ( _Val ) {
//      }
//
//      // The function call for the element to be multiplied
//      Type operator ( ) ( Type& elem ) const 
//      {
//         return elem * Factor;
//      }
//};


// Return whether first element is greater than the second
bool fltgreater ( float elem1, float elem2 )
{
   return elem1 > elem2;
}

// Returns whether the firt time window is earlier than the other
bool compareTimeWindows(TIME_WINDOW tw1, TIME_WINDOW tw2) {
	return tw1.begin < tw2.begin;
}

bool compareEventsInTime(EVENT_SORT_VECTOR_ELEMENT e1, EVENT_SORT_VECTOR_ELEMENT e2) {
	return e1.begin < e2.begin;
}

bool compareEventsInTimeNew(EVENT_SORT_VECTOR_ELEMENT_NEW e1, EVENT_SORT_VECTOR_ELEMENT_NEW e2) {
	return e1.begin < e2.begin;
}

////////////////////////////////////

CFilter::CFilter(void) : taps(0)
{
}

CFilter::~CFilter()
{
}

/*
Description: Takes a vector and a corresponding time vector.
	1. generates a new time vector with exact sampling rate FILTER_DESIGN_SAMPLE_INTERVAL, filling out holes, adjusting
	2. generates 2 new input vector corresponding to the new time vector
	3. clear and refills the 2 input data vectors and time vector. These will now have new lengths
*/
void CFilter::filter(vector <FLOAT> *_time,
						vector <FLOAT> *_v1,
						vector <FLOAT> *_v2)
{
	if (0 == _time->size()) return;

	ASSERT(_time->size() == _v1->size());
	ASSERT(_time->size() == _v2->size());

	vector <FLOAT> newTime,newVector1,newVector2;

	float t = .0f;
	float sint = FILTER_DESIGN_SAMPLE_INTERVAL;
	float maxTime = _time->at(_time->size() - 1);
	while (t <= maxTime) {
		newTime.push_back(t);
		t += FILTER_DESIGN_SAMPLE_INTERVAL;
	}
	vector <FLOAT>::iterator i1;
	FLOAT nv1 = .0f;
	FLOAT nv2 = .0f;

	for (i1 = newTime.begin() ; i1 < newTime.end() ; i1++) {
		CDataSet::interpolate(_v1,_time,*i1,&nv1);
		newVector1.push_back(nv1);
		
		CDataSet::interpolate(_v2,_time,*i1,&nv2);
		newVector2.push_back(nv2);
	}

	_v1->clear();
	_v2->clear();
	_time->clear();

	convolve(&newVector1,_v1);
	convolve(&newVector2,_v2);
	
	int index = 0;
	for (i1 = newTime.begin() ; i1 < newTime.end() ; i1++) {
		_time->push_back(*i1);
		index++;
	}
	
	cutFromStartAndEnd(_v1,taps / 2);
	cutFromStartAndEnd(_v2,taps / 2);
	cutFromStartAndEnd(_time,taps / 2);
	shiftVectorValues(_time,FILTER_DESIGN_SAMPLE_INTERVAL * (float) taps / 2.0f);

	ASSERT(_time->size() > 0);
	ASSERT(_time->size() == _v1->size());
	ASSERT(_time->size() == _v2->size());
}


/*
Description: Takes a vector and a corresponding time vector.
	1. generates a new time vector with exact sampling rate FILTER_DESIGN_SAMPLE_INTERVAL, filling out holes, adjusting
	2. generates 3 new input vector corresponding to the new time vector
	3. clear and refills the 3 input data vectors and time vector. These will now have new lengths
*/
void CFilter::filter(vector <FLOAT> *_time,
						vector <FLOAT> *_v1,
						vector <FLOAT> *_v2,
						vector <FLOAT> *_v3)
{
	if (0 == _time->size()) return;
	ASSERT(_time->size() == _v1->size());
	ASSERT(_time->size() == _v2->size());
	ASSERT(_time->size() == _v3->size());

	vector <FLOAT> newTime,newVector1,newVector2,newVector3;

	float t = .0f;
	float sint = FILTER_DESIGN_SAMPLE_INTERVAL;
	float maxTime = _time->at(_time->size() - 1);
	while (t <= maxTime) {
		newTime.push_back(t);
		t += FILTER_DESIGN_SAMPLE_INTERVAL;
	}
	vector <FLOAT>::iterator i1;
	FLOAT nv1 = .0f;
	FLOAT nv2 = .0f;
	FLOAT nv3 = .0f;

	for (i1 = newTime.begin() ; i1 < newTime.end() ; i1++) {
		CDataSet::interpolate(_v1,_time,*i1,&nv1);
		newVector1.push_back(nv1);
		
		CDataSet::interpolate(_v2,_time,*i1,&nv2);
		newVector2.push_back(nv2);
		
		CDataSet::interpolate(_v3,_time,*i1,&nv3);
		newVector3.push_back(nv3);
	}

	_v1->clear();
	_v2->clear();
	_v3->clear();
	_time->clear();

	convolve(&newVector1,_v1);
	convolve(&newVector2,_v2);
	convolve(&newVector3,_v3);
	
	int index = 0;
	for (i1 = newTime.begin() ; i1 < newTime.end() ; i1++) {
		_time->push_back(*i1);
		index++;
	}
	
	cutFromStartAndEnd(_v1,taps / 2);
	cutFromStartAndEnd(_v2,taps / 2);
	cutFromStartAndEnd(_v3,taps / 2);
	cutFromStartAndEnd(_time,taps / 2);
	shiftVectorValues(_time,FILTER_DESIGN_SAMPLE_INTERVAL * (float) taps / 2.0f);

	ASSERT(_time->size() > 0);
	ASSERT(_time->size() == _v1->size());
	ASSERT(_time->size() == _v2->size());
	ASSERT(_time->size() == _v3->size());
}

/*
Description: Takes a vector and a corresponding time vector.
	1. generates a new time vector with exact sampling rate FILTER_DESIGN_SAMPLE_INTERVAL, filling out holes, adjusting
	2. generates 4 new input vector corresponding to the new time vector
	3. clear and refills the 4 input data vectors and time vector. These will now have new lengths
*/
void CFilter::filter(vector <FLOAT> *_time,
						vector <FLOAT> *_v1,
						vector <FLOAT> *_v2,
						vector <FLOAT> *_v3,
						vector <FLOAT> *_v4)
{
	if (0 == _time->size()) return;
	ASSERT(_time->size() == _v1->size());
	ASSERT(_time->size() == _v2->size());
	ASSERT(_time->size() == _v3->size());
	ASSERT(_time->size() == _v4->size());

	vector <FLOAT> newTime,newVector1,newVector2,newVector3,newVector4;

	float t = .0f;
	float sint = FILTER_DESIGN_SAMPLE_INTERVAL;
	float maxTime = _time->at(_time->size() - 1);
	while (t <= maxTime) {
		newTime.push_back(t);
		t += FILTER_DESIGN_SAMPLE_INTERVAL;
	}
	vector <FLOAT>::iterator i1;
	FLOAT nv1 = .0f;
	FLOAT nv2 = .0f;
	FLOAT nv3 = .0f;
	FLOAT nv4 = .0f;

	for (i1 = newTime.begin() ; i1 < newTime.end() ; i1++) {
		CDataSet::interpolate(_v1,_time,*i1,&nv1);
		newVector1.push_back(nv1);
		
		CDataSet::interpolate(_v2,_time,*i1,&nv2);
		newVector2.push_back(nv2);
		
		CDataSet::interpolate(_v3,_time,*i1,&nv3);
		newVector3.push_back(nv3);
		
		CDataSet::interpolate(_v4,_time,*i1,&nv4);
		newVector4.push_back(nv4);
	}

	_v1->clear();
	_v2->clear();
	_v3->clear();
	_v4->clear();
	_time->clear();

	convolve(&newVector1,_v1);
	convolve(&newVector2,_v2);
	convolve(&newVector3,_v3);
	convolve(&newVector4,_v4);
	
	int index = 0;
	for (i1 = newTime.begin() ; i1 < newTime.end() ; i1++) {
		_time->push_back(*i1);
		index++;
	}
	
	cutFromStartAndEnd(_v1,taps / 2);
	cutFromStartAndEnd(_v2,taps / 2);
	cutFromStartAndEnd(_v3,taps / 2);
	cutFromStartAndEnd(_v4,taps / 2);
	cutFromStartAndEnd(_time,taps / 2);
	shiftVectorValues(_time,FILTER_DESIGN_SAMPLE_INTERVAL * (float) taps / 2.0f);

	ASSERT(_time->size() > 0);
	ASSERT(_time->size() == _v1->size());
	ASSERT(_time->size() == _v2->size());
	ASSERT(_time->size() == _v3->size());
	ASSERT(_time->size() == _v4->size());
}

/*
Description: Takes a vector and a corresponding time vector.
1. generates a new time vector with exact sampling rate FILTER_DESIGN_SAMPLE_INTERVAL, filling out holes, adjusting
2. generates 6 new input vector corresponding to the new time vector
3. clear and refills the 4 input data vectors and time vector. These will now have new lengths
*/
void CFilter::filter(vector <FLOAT> *_time,
						vector <FLOAT> *_v1,
						vector <FLOAT> *_v2,
						vector <FLOAT> *_v3,
						vector <FLOAT> *_v4, 
						vector <FLOAT> *_v5,
						vector <FLOAT> *_v6)
{
	if (0 == _time->size()) return;
	ASSERT(_time->size() == _v1->size());
	ASSERT(_time->size() == _v2->size());
	ASSERT(_time->size() == _v3->size());
	ASSERT(_time->size() == _v4->size());
	ASSERT(_time->size() == _v5->size());
	ASSERT(_time->size() == _v6->size());

	vector <FLOAT> newTime, newVector1, newVector2, newVector3, newVector4, newVector5, newVector6;

	float t = .0f;
	float sint = FILTER_DESIGN_SAMPLE_INTERVAL;
	float maxTime = _time->at(_time->size() - 1);
	while (t <= maxTime) {
		newTime.push_back(t);
		t += FILTER_DESIGN_SAMPLE_INTERVAL;
	}
	vector <FLOAT>::iterator i1;
	FLOAT nv1 = .0f;
	FLOAT nv2 = .0f;
	FLOAT nv3 = .0f;
	FLOAT nv4 = .0f;
	FLOAT nv5 = .0f;
	FLOAT nv6 = .0f;

	for (i1 = newTime.begin(); i1 < newTime.end(); i1++) {
		CDataSet::interpolate(_v1, _time, *i1, &nv1);
		newVector1.push_back(nv1);

		CDataSet::interpolate(_v2, _time, *i1, &nv2);
		newVector2.push_back(nv2);

		CDataSet::interpolate(_v3, _time, *i1, &nv3);
		newVector3.push_back(nv3);

		CDataSet::interpolate(_v4, _time, *i1, &nv4);
		newVector4.push_back(nv4);

		CDataSet::interpolate(_v5, _time, *i1, &nv5);
		newVector5.push_back(nv5);

		CDataSet::interpolate(_v6, _time, *i1, &nv6);
		newVector6.push_back(nv6);
	}

	_v1->clear();
	_v2->clear();
	_v3->clear();
	_v4->clear();
	_v5->clear();
	_v6->clear();
	_time->clear();

	convolve(&newVector1, _v1);
	convolve(&newVector2, _v2);
	convolve(&newVector3, _v3);
	convolve(&newVector4, _v4);
	convolve(&newVector5, _v5);
	convolve(&newVector6, _v6);

	_time->resize(newTime.size());
	copy(newTime.begin(), newTime.end(), _time->begin());

	cutFromStartAndEnd(_v1, taps / 2);
	cutFromStartAndEnd(_v2, taps / 2);
	cutFromStartAndEnd(_v3, taps / 2);
	cutFromStartAndEnd(_v4, taps / 2);
	cutFromStartAndEnd(_v5, taps / 2);
	cutFromStartAndEnd(_v6, taps / 2);
	cutFromStartAndEnd(_time, taps / 2);
	shiftVectorValues(_time, FILTER_DESIGN_SAMPLE_INTERVAL * (float)taps / 2.0f);

	ASSERT(_time->size() > 0);
	ASSERT(_time->size() == _v1->size());
	ASSERT(_time->size() == _v2->size());
	ASSERT(_time->size() == _v3->size());
	ASSERT(_time->size() == _v4->size());
	ASSERT(_time->size() == _v5->size());
	ASSERT(_time->size() == _v6->size());
}

/*
Description: Takes a vector and a corresponding time vector.
	1. generates a new time vector with exact sampling rate FILTER_DESIGN_SAMPLE_INTERVAL, filling out holes, adjusting
	2. generates new input vector corresponding to the new time vector
	3. clear and refills the input data vector and time vector. These will now have new lengths
*/
void CFilter::filter(vector <FLOAT> *_time,vector <FLOAT> *_v)
{
	if (0 == _time->size()) return;
	ASSERT(_time->size() == _v->size());

	vector <FLOAT> newTime,newVector;

	float t = .0f;
	float sint = FILTER_DESIGN_SAMPLE_INTERVAL;
	float maxTime = _time->at(_time->size() - 1);
	while (t <= maxTime) {
		newTime.push_back(t);
		t += FILTER_DESIGN_SAMPLE_INTERVAL;
	}
	vector <FLOAT>::iterator i1;
	FLOAT nv1 = .0f;
	FLOAT nv2 = .0f;

	for (i1 = newTime.begin() ; i1 < newTime.end() ; i1++) {
		CDataSet::interpolate(_v,_time,*i1,&nv1);
		newVector.push_back(nv1);
	}

	_v->clear();
	_time->clear();

	convolve(&newVector,_v);
	
	int index = 0;
	for (i1 = newTime.begin() ; i1 < newTime.end() ; i1++) {
		_time->push_back(*i1);
		index++;
	}
	
	cutFromStartAndEnd(_v,taps / 2);
	cutFromStartAndEnd(_time,taps / 2);
	shiftVectorValues(_time,FILTER_DESIGN_SAMPLE_INTERVAL * (float) taps / 2.0f);

	ASSERT(_time->size() == _v->size());
}

/*
Description: Shifts all elements with _add. Usually used on time axis to compensate for filter delays
*/
void CFilter::shiftVectorValues(vector <FLOAT> *_time,float _add)
{
	vector <FLOAT>::iterator it;
	for (it = _time->begin() ; it < _time->end() ; it++) *it += _add;
}


/*
Description: 
		Cuts from start and end of time series
		Because taps/2 in each end becomes zero after convolution
*/
void CFilter::cutFromStartAndEnd(vector <FLOAT> *_v,int _numToCut)
{
	if (_v->size() <= (unsigned int) (_numToCut * 2)) return;

	//---From end
	_v->resize(_v->size() - _numToCut);

	reverse(_v->begin(),_v->end());

	//---From start
	_v->resize(_v->size() - _numToCut);

	reverse(_v->begin(),_v->end());
}

void CFilter::convolve(vector <FLOAT> *_in,vector <FLOAT> *_out)
{
	if (0 == _in->size()) return;

	ASSERT(0 ==_out->size());
	if ((unsigned int) (1.5f * taps) >= _in->size()) {
		vector <FLOAT>::iterator it1;
		it1 = _in->begin();
		for ( ; it1 < _in->end() ; ++it1) _out->push_back(*it1);
		return;
	}
	vector <FLOAT>::iterator it1,it2;

	it1 = _in->begin();
	int half = taps / 2;
	for (int i = 0 ; i < half ; i++) {
		_out->push_back(.0f);
		it1++;
	}
	
	it2 = it1 + taps;

	while (it2 < _in->end()) {
		_out->push_back(inner_product(it1,it2,coef.begin(),.0f,std::plus<FLOAT>(),std::multiplies<FLOAT>()));
		it1++;
		it2++;	
	}
	while (_out->size() < _in->size()) 
		_out->push_back(.0f);

	return;
}
/////////////////////////////////////////

CChebHP_01Hz_Filter::CChebHP_01Hz_Filter()
{

}

CChebHP_01Hz_Filter::~CChebHP_01Hz_Filter()
{
	taps = NtapChebHP;
	for (int i = 0; i < taps; i++) coef.push_back(FIRCoefChebHP[i]);
}

///////////////////////////////////////

CLP_1Hz_Filter::CLP_1Hz_Filter()
{
	taps = NtapLP1Hz;
	for (int i = 0 ; i < taps ; i++) coef.push_back(LP_FIRCoef1Hz[i]);
}

CLP_1Hz_Filter::~CLP_1Hz_Filter()
{
}

///////////////////////////////////////

CLP_02Hz_Filter::CLP_02Hz_Filter()
{
	taps = NtapLP02;
	for (int i = 0 ; i < taps ; i++) coef.push_back(LP02FIRCoef[i]);
}

CLP_02Hz_Filter::~CLP_02Hz_Filter()
{
}

/////////////////////////////////////

CLP_5sampleRect_Filter::CLP_5sampleRect_Filter()
{
	taps = NtapRect;
	for (int i = 0 ; i < taps ; i++) coef.push_back(FIRRectCoef[i]);
}

CLP_5sampleRect_Filter::~CLP_5sampleRect_Filter()
{
}
/////////////////////////////////////

CLP_10sampleRect_Filter::CLP_10sampleRect_Filter()
{
	taps = NtapRect2;
	for (int i = 0 ; i < taps ; i++) coef.push_back(FIRRect2Coef[i]);
}

CLP_10sampleRect_Filter::~CLP_10sampleRect_Filter()
{
}/////////////////////////////////////

CLP_15sampleRect_Filter::CLP_15sampleRect_Filter()
{
	taps = NtapRect3;
	for (int i = 0 ; i < taps ; i++) coef.push_back(FIRRect3Coef[i]);
}

CLP_15sampleRect_Filter::~CLP_15sampleRect_Filter()
{
}

////////////////////////////////

CBPFilter::CBPFilter()
{
	taps = NtapBP;
	for (int i = 0 ; i < taps ; i++) coef.push_back(BP_FIRCoef[i]);
}

CBPFilter::~CBPFilter()
{
}

////////////////////////////////

CBPCosineFilter::CBPCosineFilter()
{
	taps = NtapBPCosine;
	for (int i = 0; i < taps; i++) coef.push_back(BPCosine_FIRCoef[i]);
}

CBPCosineFilter::~CBPCosineFilter()
{
}

////////////////////////////////

CBPCheb1t012Filter::CBPCheb1t012Filter()
{
	taps = NtapCheb;
	for (int i = 0; i < taps; i++) coef.push_back(Cheb_FIRCoef[i]);
}

CBPCheb1t012Filter::~CBPCheb1t012Filter()
{
}

////////////////////////////////////////


CDataSet::CDataSet() : 
	minX(.0f),maxX(1.0f),
	dataType(noType),dataSint(.0f),
	internalName(_T("No name"))
{
}

CDataSet::~CDataSet()
{
}

void CDataSet::makeFrqVector(vector <FLOAT> *_v,vector <FLOAT> *_t,vector <FLOAT> *_resv,vector <FLOAT> *_resvtime)
{
	ASSERT(_v->size() == _t->size());

	vector <FLOAT>::iterator el1,el2;
	
	el1 = _v->begin();
	el2 = _v->begin();
	el2++;
	FLOAT max = .0f;
	FLOAT min = .0f;
	FLOAT minTime = .0f;
	FLOAT maxTime = .0f;
	FLOAT oldMaxTime = .0f;
	bool sign,oldSign;
	oldSign = sign = true;
	long sampNum = 0L;

	FLOAT candidateMax,candidateMin;
	candidateMax = candidateMin = .0f;
	FLOAT frq,diff;
	BOOL beenNegative = FALSE;
	for ( ; el2 != _v->end( ); el2++ )	{
		sign = *el2 >= *el1;
		el1++;
		if ((oldSign) && (!sign)) {	// Maximal
			candidateMax = _v->at(sampNum);
			if ((candidateMax > .0f) && (beenNegative)) {
				maxTime = _t->at(sampNum);
				diff = fabs(maxTime - oldMaxTime);
				frq = 0L == diff ? .0f : 60.0f / diff;  // In /min !
				_resv->push_back(frq);
				_resvtime->push_back((maxTime + minTime) / 2.0f);
				oldMaxTime = maxTime;
				beenNegative = FALSE;
			}
		}
		if ((!oldSign) && (sign)) {				// Minimal
			candidateMin = _v->at(sampNum);
			if (candidateMin < .0f) {
				beenNegative = TRUE;
				min = candidateMin;
				minTime = _t->at(sampNum);
			}
		}
		oldSign = sign;
		sampNum++;
	}

	//---Filter
	CLP_15sampleRect_Filter lpF;
	lpF.filter(_resvtime,_resv);

	ASSERT(_resv->size() == _resvtime->size());
}


void CDataSet::makeFrqVector(vector <FLOAT> *_v,const FLOAT _sint)
{
	if (!_v->size()) return;
	if (.0f == _sint) return;
	xyRaw.clear();
	dataSint = .0f;

	vector <FLOAT>::iterator el1,el2;
	
	el1 = _v->begin();
	el2 = _v->begin();
	el2++;
	FLOAT max = .0f;
	FLOAT min = .0f;
	FLOAT minTime = .0f;
	FLOAT maxTime = .0f;
	FLOAT oldMaxTime = .0f;
	bool sign,oldSign;
	oldSign = sign = true;
	long sampNum = 0L;

	FLOAT candidateMax,candidateMin;
	candidateMax = candidateMin = .0f;
	FLOAT frq,diff;
	BOOL beenNegative = FALSE;
	for ( ; el2 != _v->end( ); el2++ )	{
		sign = *el2 >= *el1;
		el1++;
		if ((oldSign) && (!sign)) {	// Maximal
			candidateMax = _v->at(sampNum);
			if ((candidateMax > .0f) && (beenNegative)) {
				maxTime = sampNum * _sint;
				diff = fabs(maxTime - oldMaxTime);
				frq = 0L == diff ? .0f : 1.0f / diff;
				xyRaw.push_back(frq);
				xyRawTime.push_back((maxTime + minTime) / 2.0f);
				oldMaxTime = maxTime;
				beenNegative = FALSE;
			}
		}
		if ((!oldSign) && (sign)) {	// Minimal
			candidateMin = _v->at(sampNum);
			if (candidateMin < .0f) {
				beenNegative = TRUE;
				min = candidateMin;
				minTime = sampNum * _sint;
			}
		}
		oldSign = sign;
		sampNum++;
	}
}

unsigned int CDataSet::evaluateData(void)
{
	return 0;
}

void CDataSet::Serialize(CArchive& ar, int majorFileVersion, int minorFileVersion)
{
}

float CDataSet::getStart(void)
{
	if (!getSize()) return .0f;

	float start = *timeaxis.begin();
	return start;
}

float CDataSet::getStop(void)
{
	if (!getSize()) return .0f;

	float stop = *(timeaxis.end() - 1);
	return stop;
}

/*
Description: Takes the ratio between a flow and a pressure vector with DIFFERENT time axis

	c(t1) = a(t1) / b(t1)
	- out time vector is set equal to input time vector 1 
	- Values from input vector  1 are selected as a(t1)
	- b does not have a value at t1, hence b(t1) is the interpolation between b(t`) and b(t``) where t` < t1 and t`` > t1

_press: Input Vector pressure
_flow: Input vector flow
_pressTime: Input time vector pressure
_flowTime: Input time vector flow
_resv: Output vector (length as _press)
_revtime: Output time vector
*/
void CDataSet::makeAdmittanceVector(vector <FLOAT> *_press,
									vector <FLOAT> *_flow,
									vector <FLOAT> *_pressTime,
									vector <FLOAT> *_flowTime,
									vector <FLOAT> *_resv,
									vector <FLOAT> *_resvtime)
{
	ASSERT(_press->size() == _pressTime->size());
	ASSERT(_flow->size() == _flowTime->size());
	_resv->clear();
	_resvtime->clear();

	vector <FLOAT>::iterator invector,intime,hight;
	invector = _flow->begin();
	intime = _flowTime->begin();
	FLOAT a,b,t;
	for ( ; invector != _flow->end(); invector++) {
		
		t = *intime;
		if (!interpolate(_press,_pressTime,t,&b)) break;
		a = *invector;
		if (b != .0f) {
			_resv->push_back(a / b);
			_resvtime->push_back(t);
		}

		intime++;
	}

	ASSERT(_resv->size() == _resvtime->size());
}

/*
Description:
	Computes the median and removes it from the input data
*/
void CDataSet::removeMedian(vector <FLOAT> *_v)
{
	if (0 == _v->size()) return;

	vector <FLOAT> copyV(_v->size());
	copy(_v->begin(),_v->end(),copyV.begin());
	sort(copyV.begin(),copyV.end());
	float median = copyV.at(copyV.size() / 2);
	copy(_v->begin(),_v->end(),copyV.begin());
	transform(copyV.begin(),copyV.end(),_v->begin(),AddValue <FLOAT> (- median));
}

/*
Description: Generates a baseline based on a median over preceding baseline length time
Important: It is assumed that the time vector is regular - typically 0.25 sec intervals
*/
void CDataSet::generateBaselineVectorMedian(vector <FLOAT> *_v,vector <FLOAT> *_tv,vector <FLOAT> *_outv,float _baselineLength)
{
	ASSERT(_outv);
	ASSERT(_v->size() == _tv->size());

	_outv->clear();

	vector <FLOAT>::iterator itt1;
	vector <FLOAT>::iterator it1,it0;
	it1 = _v->begin();
	for (itt1 = _tv->begin() ; itt1 < _tv->end() ; itt1++, it1++) {
		if (*itt1 < _baselineLength) _outv->push_back(.0f);
		else break;
	}
	it0 = _v->begin();

	for ( ; itt1 < _tv->end() ; itt1++, it0++, it1++) _outv->push_back(getMedian(it0,it1));

	ASSERT(_outv->size() == _v->size());
}

/*
Description: Finds median of a vector, used odd/even detection
*/
float CDataSet::median_element(vector <FLOAT>::iterator _from, vector <FLOAT>::iterator _to)
{
	ASSERT(_to > _from);
	int size = distance(_from, _to);
	if (size < 2) return .0f;

	vector <FLOAT> v;
	v.resize(size);
	copy(_from, _to, v.begin());
	sort(v.begin(), v.end());
	if (0 == (size % 2)) {
		int n = size / 2;
		return ((v.at(n) + v.at(n - 1)) / 2.0f);
	}
	int mid = size / 2;
	return v.at(mid);
}

/*
Description: Generates a baseline based on a baseline length and rules below (getIsStable). 
This is based on AASM saying that if breathing is not stable - take average of 3 largest breaths. 
Important: It is assumed that the time vector is regular - typically 0.25 sec intervals

Baselinelength BASELINE_LENGTH (120 secs (2 minutes) according to AASM)
*/
void CDataSet::generateBaselineVectorAASM(vector <FLOAT> *_v,vector <FLOAT> *_tv,vector <FLOAT> *_outv,float _percStabilityLimit,
										  float _baselineLength,bool _removeOutliers)
{
	ASSERT(_outv);
	ASSERT(_v->size() == _tv->size());

	if (!_tv->size()) return;

	//ASSERT( ((_tv->at(1) - _tv->at(0)) < .3f) && ((_tv->at(1) - _tv->at(0)) > .2f));	
	// Later (in baseline generation), 																						
	// we assume .25 sampl rate for this vector
	// Because this is after filtering!

	_outv->clear();

	vector <FLOAT>::iterator itt1;
	vector <FLOAT>::iterator it1,it0;
	it1 = _v->begin();
	for (itt1 = _tv->begin() ; itt1 < _tv->end() ; itt1++, it1++) {
		if (*itt1 < _baselineLength + *_tv->begin()) _outv->push_back(.0f);
		else break;
	}
	it0 = _v->begin();
	
	float base = .0f;
	int cnt = 0;
	for ( ; itt1 < _tv->end() ; itt1++, it0++, it1++) {
		if (4 == cnt) {
			getIsStable(it0,it1,_percStabilityLimit,&base,_removeOutliers);
			cnt = 0;
		}
		cnt++;
		_outv->push_back(base);
	}
	ASSERT(_outv->size() == _v->size());
}

/*
Description: In an input vector, _peakIt has been identified as a POSITIVE peak.

	The peak is removed including the turning points on each side of the peak.
*/
void CDataSet::removePosPeak(vector <FLOAT>::iterator _fromIt, vector <FLOAT>::iterator _toIt,
	vector <FLOAT>::iterator _peakIt)
{
	
	//---Left side. Continue until it starts dropping
	vector <FLOAT>::iterator firstIt = _peakIt;
	vector <FLOAT>::iterator it0 = _toIt;
	vector <FLOAT>::iterator it1 = _toIt;

	it0--;
	while (it0 > _fromIt) {
		if (*it0 >= *it1) {
			firstIt = it1;
			break;
		}
		--it0;
		--it1;
	}

	//---Right side. Continue until it starts dropping
	vector <FLOAT>::iterator lastIt = _peakIt;
	vector <FLOAT>::iterator it00 = _toIt;
	vector <FLOAT>::iterator it01 = _toIt;

	it00++;
	while (it00 > _fromIt) {
		if (*it00 >= *it01) {
			lastIt = it01;
			break;
		}
		++it00;
		++it01;
	}

	linInterpolate(firstIt, lastIt);
}

/*
Description: Based on two endpoints, linearly interpolate between them replacing the original data
*/
void CDataSet::linInterpolate(vector <FLOAT>::iterator _fromIt, vector <FLOAT>::iterator _toIt)
{
	FLOAT span = *_toIt - *_fromIt;
	int length = distance(_fromIt, _toIt);
	if (1 >= length) return;

	FLOAT step = span / length;
	vector <FLOAT>::iterator iT = _fromIt;

	FLOAT value = *_fromIt;
	for (; iT < _toIt; ++iT) {
		*iT = value;
		value += step;
	}
}

/*
Description: Computes mean and standard deviation for a vector 
_fromI: Begin iterator
_toI: End iterator

*_mean: Mean is retuned in this parameter

Returns: Standard deviation
*/
FLOAT CDataSet::getMeanStandardDev(vector<FLOAT>::iterator _fromI, vector<FLOAT>::iterator _toI, FLOAT * _mean)
{
	*_mean = .0f;
	FLOAT stdDev = .0f;
	vector<FLOAT>::iterator it = _fromI;
	*_mean = (float) for_each(_fromI, _toI,Average());
	stdDev = (float) for_each(_fromI, _toI, standardDeviation(*_mean));
	return stdDev;
}

/*
Description: Checks if  _t is close to any of the elements in vector _v (start,stop,start,stop,...)
_v: The vector, typicaly time values
_edgeDetectTolerance: Edge detection tolerance (related to length of event)
_what: ON_EVNT if between a start and corresponding stop
_where: May be left (at start) or right edge (at stop)
_startTime : If a pointer is provided, the start time of the event is returned here
_stopTime : If a pointer is provided, the stop time of the event is returned here
*/
bool CDataSet::getHitBetweenStartAndStop(vector<FLOAT>* _v, float _t, 
	float _edgeDetectTolerance, bool *_between, bool *_onLeft, bool *_onRight,
	float *_startTime /*= NULL*/, float *_stopTime /*= NULL*/)
{
	int length = _v->size();
	ASSERT(length % 2 == 0);

	*_between = false;
	*_onLeft = false;
	*_onRight = false;
	if (2 > _v->size()) return false;

	vector <FLOAT>::iterator iStart,iStop;
	iStart = _v->begin();
	iStop = iStart;
	iStop++;
	int halfLength = length / 2;
	int i = 0;
	while (i < halfLength) {
		float length = *iStop - *iStart;
		float halfL = length * _edgeDetectTolerance / 2.0f;
		float left = _t - halfL;
		float right = _t + halfL;
		if ((left < *iStart) && (right > *iStart)) 
			*_onLeft = true;
		else if ((left < *iStop) && (right > *iStop)) 
			*_onRight = true;
		if ((*iStart < _t) && (*iStop > _t))
			*_between = true;

		if (*_between || *_onLeft || *_onRight) {
			if (_startTime) *_startTime = *iStart;
			if (_stopTime) *_stopTime = *iStop;
			return true;
		}

		if (++i < halfLength) {
			++iStop;
			++iStart;
		}
		else break;
		if (iStop < _v->end()) {
			++iStop;
			++iStart;
		}
		else break;
	}
	return false;
}

/*
Description: Counts the number of bits in the UINT
*/
UINT CDataSet::countSetBits(UINT _n)
{
	UINT count = 0;
	while (_n)
	{
		count += _n & 1;
		_n >>= 1;
	}
	return count;
}

/*
Description: Computes the interquartile (IQR) range and the median based on an input vector.
Returns: The IQR
Mdeian is saved in *_median

*/
float CDataSet::getIQR(vector <FLOAT> *_v, float *_median)
{
	vector <FLOAT> vc;
	vc.resize(_v->size(), .0f);

	copy(_v->begin(),_v->end(), vc.begin());
	sort(vc.begin(), vc.end());

	float mid = (float) ((int) ((float)vc.size() / 2.0f));

	*_median = vc.at((int) mid);
	
	float q1 = vc.at((int)(mid * .5f));
	float q3 = vc.at((int)(mid * 1.5f));
	return q3 - q1;
}


/*
Description: 
Finds max and min of elements in vecttor. Return avagere, or midpoint, of max and min
*/
float CDataSet::getCentre(vector <FLOAT>::iterator _fromIt, vector <FLOAT>::iterator _toIt)
{
	auto result = minmax_element(_fromIt, _toIt);
	float min = *result.first;
	float max = *result.second;
	float centre = min + (max - min) / 2.0f;
	return centre;
}

/*
Description: returns median of elements in vector
				ignores odd/even detection
*/
float CDataSet::getMedian(vector <FLOAT>::iterator _fromIt,vector <FLOAT>::iterator _toIt)
{
	vector <FLOAT> v;
	v.resize(distance(_fromIt,_toIt),.0f);
	if (0 == v.size())
		return .0f;

	copy(_fromIt,_toIt,v.begin());
	sort(v.begin(),v.end());
	
	float median = v.at((int) ((float) v.size() / 2.0f));
	return median;
}

/*
Description:
	Takes the median and computes max and min limits based on median +/- _percentLimit.
	_percentLimit is in % of the median

	If ok, sets baseline equal to median
	If !ok, sets baseline equal to average of 3 largest breaths
	Returns baseline in _baseline
*/
bool CDataSet::getIsStable(vector <FLOAT>::iterator _fromIt,vector <FLOAT>::iterator _toIt,float _percentLimit,
						   float *_baseline,bool _removeOutliers)
{
	vector <FLOAT> vcopy(distance(_fromIt,_toIt)),vunsorted(distance(_fromIt,_toIt));
	if (10 >= vcopy.size()) return false;

	copy(_fromIt,_toIt,vcopy.begin());
	copy(_fromIt,_toIt,vunsorted.begin());
	sort(vcopy.begin(),vcopy.end());
	
	float median = vcopy.at((int) ((float) vcopy.size() / 2.0f));

	//---Would be needed if far outliers = 3.0 * iqr is required
    float farOutlierLimit = .0f;
    if (_removeOutliers) {
	    float q1 = vcopy.at((int) ((float) vcopy.size() / 4.0f));
	    float q3 = vcopy.at((int) (3.0f * (float) vcopy.size() / 4.0f));
	    float iqr = q3 - q1;
	    farOutlierLimit = median + 3.0f * iqr;
	}

	float fraction = _percentLimit / 100.0f;
	float max = median * (1.0f + fraction);
	float min = median * (1.0f - fraction);
	int numBelowMin = 0;	
	vector <FLOAT>::iterator firstAboveLimit = lower_bound(vcopy.begin(),vcopy.end(),min);
	if (firstAboveLimit != vcopy.end()) numBelowMin = distance(vcopy.begin(),firstAboveLimit);
	
	int numAboveMax = 0;
	vector <FLOAT>::iterator firstAboveLimit2 = upper_bound(vcopy.begin(),vcopy.end(),max);
	if (firstAboveLimit2 != vcopy.end()) numAboveMax = distance(firstAboveLimit2,vcopy.end());

	int numOutside = numBelowMin + numAboveMax;
	float percentInside = 1.0f - ((float) numOutside / (float) vcopy.size());
	bool ret = false;
	if (percentInside >= .95f) {
		ret = true;
		*_baseline = median;
	}
	else {
		//---Remove outliers and then pick the largest 3 breaths
		if (_removeOutliers) {
			vector <FLOAT>::iterator it,mI;
			for (mI = vunsorted.begin() ; mI < vunsorted.end() ; ++mI) {
				if (*mI >= farOutlierLimit) {
					*mI = .0f;
					it = mI;
					if (it > vunsorted.begin()) *(--it) = .0f;
					it = mI;
					if (it < vunsorted.end() - 1) *(++it) = .0f;
				}
			}
		}
		float sum = .0f;
		int cnt = 0;
		vector <FLOAT>::iterator it,mI;
		for (mI = vunsorted.begin() ; mI < vunsorted.end() ; ++mI) {
			vector <FLOAT>::iterator maxI = max_element(vunsorted.begin(),vunsorted.end());
			if (.0f == *maxI) break;
			sum += *maxI;
			cnt++;
			if (3 == cnt) break;

			//---Remove 4 seconds of data (16 samples)
			it = maxI;
			*it = .0f;
			if (it > vunsorted.begin()) *(--it) = .0f;
			if (it > vunsorted.begin()) *(--it) = .0f;
			if (it > vunsorted.begin()) *(--it) = .0f;
			if (it > vunsorted.begin()) *(--it) = .0f;
			if (it > vunsorted.begin()) *(--it) = .0f;
			if (it > vunsorted.begin()) *(--it) = .0f;
			if (it > vunsorted.begin()) *(--it) = .0f;
			if (it > vunsorted.begin()) *(--it) = .0f;
			it = maxI;
			if (it < vunsorted.end() - 1) *(++it) = .0f;
			if (it < vunsorted.end() - 1) *(++it) = .0f;
			if (it < vunsorted.end() - 1) *(++it) = .0f;
			if (it < vunsorted.end() - 1) *(++it) = .0f;
			if (it < vunsorted.end() - 1) *(++it) = .0f;
			if (it < vunsorted.end() - 1) *(++it) = .0f;
			if (it < vunsorted.end() - 1) *(++it) = .0f;
			if (it < vunsorted.end() - 1) *(++it) = .0f;
			if (it < vunsorted.end() - 1) *(++it) = .0f;
			if (it < vunsorted.end() - 1) *(++it) = .0f;
		}
		if (cnt > 0) *_baseline = sum / cnt;
		else *_baseline = median;
	}

	return ret;
}

/*
Description:
	Takes the median and computes max and min limits based on median +/- _limit
*/
bool CDataSet::getIsStable(vector <FLOAT>::iterator _fromIt,vector <FLOAT>::iterator _toIt,float _limit)
{
	vector <FLOAT> v;
	v.resize(distance(_fromIt,_toIt),.0f);
	if (10 >= v.size()) return false;

	copy(_fromIt,_toIt,v.begin());
	sort(v.begin(),v.end());
	float median = v.at((int) ((float) v.size() / 2.0f));
	float max = median + _limit;
	float min = median - _limit;
	int numBelowMin = 0;
	vector <FLOAT>::iterator i0 = v.begin();
	for ( ; i0 < v.end() ; i0++) {
		if (*i0 < min) numBelowMin++;
		else break;
	}
	i0 = v.end();
	i0--;
	int numAboveMax = 0;
	for ( ; i0 >= v.begin() ; i0--) {
		if (*i0 > max) numAboveMax++;
		else break;
	}

	int numOutside = numBelowMin + numAboveMax;
	float percentInside = 1.0f - ((float) numOutside / (float) v.size());
	bool ret = false;
	if (percentInside >= .95f) ret = true;
	return ret;
}


/*
Let N be the number of data points
| Y[(N - 1) / 2]                  N odd
Find median.xmed = <
	| (1 / 2) (Y[N / 2] + Y[-1 + N / 2]) N even

	Split in two groups - high and low of equal size(odd: add the median value)
	The two medians are called the first and third quartile

	Find the inter quartile range IQR = Q3 - Q1
*/
float CDataSet::getIQR(vector <FLOAT>::iterator _fromIt, vector <FLOAT>::iterator _toIt,float *_median)
{
	vector <FLOAT> v;
	v.resize(distance(_fromIt, _toIt), .0f);

	copy(_fromIt, _toIt, v.begin());
	sort(v.begin(), v.end());
	float q1 = v.at((int)((float)v.size() / 4.0f));
	float q2 = v.at((int)((float)v.size() / 2.0f));
	float q3 = v.at((int)(3.0f * (float)v.size() / 4.0f));
	
	*_median = q2;
	return (q3 - q1);
}

/*
//--Using technique for removing outliers
	    Let N be the number of data points
	                   | Y[(N-1)/2]                  N odd
	Find median. xmed = <
	                     | (1/2) (Y[N/2] + Y[- 1 + N/2]) N even

		 Split in two groups - high and low of equal size (odd: add the median value)
		 The two medians are called the first and third quartile

		 Find the inter quartile range IQR = Q3 - Q1

		 Far outliers: > 3.0 IQR from median
		 Near outliers: > 1.5 IQR from median
*/
bool CDataSet::getIsStable(vector <FLOAT>::iterator _fromIt,vector <FLOAT>::iterator _toIt)
{
	vector <FLOAT> v;
	v.resize(distance(_fromIt,_toIt),.0f);
	if (10 >= v.size()) return false;

	copy(_fromIt,_toIt,v.begin());
	sort(v.begin(),v.end());
	float q1 = v.at((int) ((float) v.size() / 4.0f));
	float q2 = v.at((int) ((float) v.size() / 2.0f));
	float q3 = v.at((int) (3.0f * (float) v.size() / 4.0f));
	float iqr = q3 - q1;
	float max = q2 + iqr * 1.5f;
	float min = q2 - iqr * 1.5f;
	int numBelowMin = 0;
	vector <FLOAT>::iterator i0 = v.begin();
	for ( ; i0 < v.end() ; i0++) {
		if (*i0 < min) numBelowMin++;
		else break;
	}
	i0 = v.end();
	i0--;
	int numAboveMax = 0;
	for ( ; i0 >= v.begin() ; i0--) {
		if (*i0 > max) numAboveMax++;
		else break;
	}

	int numOutside = numBelowMin + numAboveMax;
	float percentInside = 1.0f - ((float) numOutside / (float) v.size());
	bool ret = false;
	if (percentInside >= .95f) ret = true;
	return ret;
}

/*
Description: 
	Find the vector from _v, based on the corresponding time vector _t and time window <from>-<to>
	Copies and sort the vector
	Pick and return element number [size() * 0.9]
*/
float CDataSet::getUpper95PercentValue(float _from,float _to,vector <FLOAT> *_v,vector <FLOAT> *_t)
{
	vector <FLOAT>::iterator it0,it1;
	it0 = lower_bound(_t->begin(),_t->end(),_from);
	it1 = lower_bound(_t->begin(),_t->end(),_to);
	
	vector <FLOAT>::iterator v0,v1,valP;
	v0 = v1 = _v->begin();
	v0 += distance(_t->begin(),it0);
	v1 += distance(_t->begin(),it1);
	int newSize = distance(v0,v1);
	if (0 == newSize) return .0f;

	vector <FLOAT> cout;
	cout.resize(newSize,.0f);
	copy(v0,v1,cout.begin());
	sort(cout.begin(),cout.end());
	int pos = (int) ((float) cout.size() * .95f);
	return cout.at(pos);
}

/*
Description:
	Based on time  vector _t, find start and stop in data value vector _t
	Returns the average
*/
float CDataSet::getAverage(float _from,float _to,vector <FLOAT> *_v,vector <FLOAT> *_t)
{
	vector <FLOAT>::iterator it0,it1;
	it0 = lower_bound(_t->begin(),_t->end(),_from);
	it1 = lower_bound(_t->begin(),_t->end(),_to);
	
	vector <FLOAT>::iterator v0,v1,valP;
	v0 = v1 = _v->begin();
	v0 += distance(_t->begin(),it0);
	v1 += distance(_t->begin(),it1);
	float sum = .0f;
	int count = 0;
	for (valP = v0 ; valP < v1 ; valP++) {
		sum += *valP;
		count++;
	}
	float avg = sum / count;

	double avemod2 = for_each(v0, v1,Average());

	return avg;
}

/*
Description: Simple linear regression
Returns: 
_a: Intercept for x = 0
_b: Slope
_rSquared: Rsquared

Returns true if all OK
*/
bool CDataSet::linReg(vector <FLOAT>::iterator _xbegin,vector <FLOAT>::iterator _xend,
		vector <FLOAT>::iterator _ybegin,vector <FLOAT>::iterator _yend,float *_a,float *_b,float *_rSquared)
{
	if (1 >= distance(_xbegin,_xend)) return false;
	ASSERT(distance(_xbegin,_xend) == distance(_ybegin,_yend));

	vector <FLOAT> xCopy(distance(_xbegin,_xend));

	//---Offset x to zero
	FLOAT offs = *_xbegin;
	transform(_xbegin,_xend,xCopy.begin(),AddValue <FLOAT> (- offs));

	double n = (double) distance(_xbegin,_xend);
	double sumx = .0f;
	double sumy = .0f;
	double sumx2 = .0f;
	double sumy2 = .0f;
	double sumxy = .0f;
	vector <FLOAT>::iterator itx,ity;
	itx = xCopy.begin();
	ity = _ybegin;
	for ( ; itx < xCopy.end() ; itx++, ity++) {
		sumx += *itx;
		sumy += *ity;
		sumxy += *itx * *ity;
		sumx2 += *itx * *itx;
		sumy2 += *ity * *ity;
	}
	double v = n * sumx2 - sumx * sumx;

	if (.0f == v) return false;

	double b = (n * sumxy - sumx * sumy) / v;
	double a = (sumy - b * sumx) / n;
	double sst = sumy2 - sumy * sumy / n;

	if (.0f == sst) 
		return false;

	double sse = .0f;
	itx = xCopy.begin();
	ity = _ybegin;
	double delta = .0f;
	for ( ; itx < xCopy.end() ; itx++, ity++) {
		delta = *ity - (a + b * *itx);
		sse += delta * delta;
	}
	double rsq = 1.0f - sse / sst;
	*_a = (float) a;
	*_b = (float) b;
	*_rSquared = (float) rsq;
	
	return true;
}

/*
Description: If all samples are bigger or equal the previous, returns true
	Else returns false
*/
bool CDataSet::getIsNotDecreasing(vector <FLOAT>::iterator _begin,vector <FLOAT>::iterator _end)
{
	if (_begin == _end) return false;

	vector <FLOAT>::iterator it0,it1;
	it0 = _begin;
	it1 = it0 + 1;
	for ( ; it1 < _end ; it1++, it0++) {
		if (*it1 < *it0) return false;
	}
	return true;
}

/*
Description:
	Finds the first peak in a sequence.
	Used on admittance data as there may be a lower peak in front of the main peak, and it is the 
	first peak that triggers the following pulse rate increase
Begin: Start iterator
End : End iterator
Return: Iterator to the first peak
*/
vector <FLOAT>::iterator CDataSet::getFirstPeak(vector <FLOAT>::iterator _begin,vector <FLOAT>::iterator _end)
{
	vector <FLOAT>::iterator elI = _begin;
	vector <FLOAT>::iterator elI2 = elI;
	elI2++;
	if (elI2 < _end) {
		bool signPos = true;
		for ( ; elI < _end ; ++elI,++elI2) {
			if (*elI < *elI2) signPos = true;
			if ((*elI > *elI2) && signPos) {
				return elI;
			}
		}
	}
	return _end;
}

/*
Description:
	Compares first endpoint with maximum. 
	Maximum must be _addCount higher than first endpoint
Begin: Start iterator
End : End iterator
_addCount: The additional count

Typically this is used for increase in pulse rate (PR)
*/
bool CDataSet::findCountRamp(vector <FLOAT>::iterator _begin,vector <FLOAT>::iterator _end,float _addCount)
{
	float vbegin = *_begin;
	float peak = *max_element(_begin,_end);
	
	if (peak >= (vbegin + _addCount)) 
		return true;

	return false;
}

/*
Description:
	Compares largest endpoint with maximum. 
	1. If Max is a factor _factor higher than largest endpoint
	2. Second sample is higher than the first (if _posDerivative == true)

	If 1) and 2) are satisfied, returns true,
	else returns false;
Begin: Start iterator
End : End iterator
_factor: The Factor
_posDerivative: true if the secodn sample must be higher than the first
*/
bool CDataSet::findPeak(vector <FLOAT>::iterator _begin,vector <FLOAT>::iterator _end,float _factor,bool _posDerivative)
{
	if (_posDerivative) if (*_begin >= *(_begin + 1)) return false;

	float vbegin = *_begin;
	float vend = *_end;
	float edgemax = vbegin > vend ? vbegin : vend;
	float peak = *max_element(_begin,_end);
	if (peak > (edgemax * _factor)) return true;

	return false;
}

/*
Description:
	Compares head endpoint and tail endpoint with maximum. 
	1. If Max is a factor _factorHead higher than head endpoint
	1. If Max is a factor _factorTail higher than tail endpoint
	2. Second sample is higher than the first (if _posDerivative == true)

	If 1),2) and 3) are satisfied, returns true,
	else returns false;
Begin: Start iterator
End : End iterator
_factor: The Factor
_position: Position from begin
*/
bool CDataSet::findPeak2(vector <FLOAT>::iterator _begin,vector <FLOAT>::iterator _end,float _factorHead,float _factorTail,
						 bool _posDerivative,int *_position)
{
	if (_posDerivative) if (*_begin >= *(_begin + 1)) return false;

	float vbegin = *_begin;
	float vend = *_end;

	vector <FLOAT>::iterator maxel = max_element(_begin,_end);
	float peak = *maxel;
	*_position = maxel - _begin;
	
	if ((peak > (vbegin * _factorHead)) && (peak > (vend * _factorTail))) 
		return true;

	return false;
}

/*
Description: Take the derivative of vector _v and place in vector _v
_v: In vector. Must be >= 2
_timev: corresponding time vector. Must be same length as _v
_out: Output vector. Must be zero length
retiurn : true of ok, false if not
*/
bool CDataSet::makeDerivative(vector <FLOAT> *_v,vector <FLOAT> *_timev)
{
	ASSERT(_v->size() == _timev->size());
	if(_v->size() < 2) return false;

	vector <FLOAT> targetV;

	vector <FLOAT>::iterator i1,i2,t1,t2;
	i1 = _v->begin();
	i2 = i1 + 1;
	t1 = _timev->begin();
	t2 = t1 + 1;
	FLOAT diffv,difft;
	for (i1 = _v->begin() ; i2 < _v->end() ; ++i1, ++i2, ++t1, ++t2) {
		diffv = *i2 - *i1;
		difft = *t2 - *t1;
		targetV.push_back(difft == .0f ? 10000.0f : diffv / difft);
	}
	targetV.push_back(diffv / difft); // Just add the same to make _out the same length as _v
	
	ASSERT(targetV.size() == _v->size());

	copy(targetV.begin(),targetV.end(),_v->begin());

	return true;
}

/*
Description: Given a time value, find the place on the _t axis and use the two find the interpolated y-value from the _v vector
		_v and _t must be the same magnitude, reptresenting the x- and y-axis respectively
*_v: Vector with y-values
*_t: Time vector
_tval: Time value
*_out: the result is placed here
Return: if _tval is in the range of _t, return true, if not, return false 
*/
bool CDataSet::interpolate(vector <FLOAT> *_v,vector <FLOAT> *_t,FLOAT _tval,FLOAT *_out)
{
	ASSERT(_v->size() == _t->size());

	int index1,index2;
	FLOAT x1,x2;
	FLOAT fraction;
	FLOAT val1,val2;

	vector <FLOAT>::iterator hight;

	hight = upper_bound(_t->begin(),_t->end(),_tval);
	if (hight == _t->end()) return false;

	index2 = hight - _t->begin();

	index1 = 0 == index2 ? index2 : index2 - 1;

	x1 = _t->at(index1);
	x2 = _t->at(index2);
	fraction = x1 == x2 ? .0f : (_tval - x1) / (x2 - x1);
	val1 = _v->at(index1);
	val2 = _v->at(index2);
	*_out = val1 + (val2 - val1) * fraction;
	return true;
}


/*
Description: Takes the sum of two time series vectors with DIFFERENT time axis

	c(t1) = a(t1) + b(t1)
	- out time vector is set equal to input time vector 1 
	- Values from input vector  1 are selected as a(t1)
	- b does not have a value at t1, hence b(t1) is the interpolation between b(t`) and b(t``) where t` < t1 and t`` > t1

_v1: Input Vector 1
_t1: Time vector 1
_v2: Input Vector 2
_t2: Time vector 2
_resv: Output vector (length as _v1)
_revtime: Output time vector
*/
void CDataSet::makeSumVector(vector <FLOAT> *_v1,vector <FLOAT> *_v2,vector <FLOAT> *_t1,vector <FLOAT> *_t2,vector <FLOAT> *_resv,vector <FLOAT> *_resvtime)
{
	ASSERT(_v1->size() == _t1->size());
	ASSERT(_v2->size() == _t2->size());
	_resv->clear();
	_resvtime->clear();
	
	vector <FLOAT>::iterator invector,intime,hight;
	invector = _v1->begin();
	intime = _t1->begin();
	FLOAT a,b,t;
	for ( ; invector != _v1->end(); invector++) {
		
		t = *intime;
		if (!interpolate(_v2,_t2,t,&b)) break;
		a = *invector;
		_resv->push_back(a + b);
		_resvtime->push_back(t);

		intime++;
	}
	ASSERT(_resv->size() == _resvtime->size());
}

/*
Description: Takes the weighted sum of two time series vectors with DIFFERENT time axis

c(t1) = w1 * a(t1) + w2 *b(t1)
- out time vector is set equal to input time vector 1
- Values from input vector  1 are selected as a(t1)
- b does not have a value at t1, hence b(t1) is the interpolation between b(t`) and b(t``) where t` < t1 and t`` > t1

_weight1:	weight 1 (w1)
_weight2:	weight 2 (w2)
_v1: Input Vector 1
_t1: Time vector 1
_v2: Input Vector 2
_t2: Time vector 2
_resv: Output vector (length as _v1)
_revtime: Output time vector
*/
void CDataSet::makeWeightedSumVector(float _weight1, float _weight2, vector<FLOAT>* _v1, vector<FLOAT>* _v2, vector<FLOAT>* _t1, vector<FLOAT>* _t2, vector<FLOAT>* _resv, vector<FLOAT>* _resvtime)
{
	ASSERT(_v1->size() == _t1->size());
	ASSERT(_v2->size() == _t2->size());
	_resv->clear();
	_resvtime->clear();

	vector <FLOAT>::iterator invector, intime, hight;
	invector = _v1->begin();
	intime = _t1->begin();
	FLOAT a, b, t;
	for (; invector != _v1->end(); invector++) {

		t = *intime;
		if (!interpolate(_v2, _t2, t, &b)) break;
		a = *invector;
		_resv->push_back(_weight1 * a + _weight2 * b);
		_resvtime->push_back(t);

		intime++;
	}
	ASSERT(_resv->size() == _resvtime->size());
}


void CDataSet::makeDiffVector(vector <FLOAT> *_v1,vector <FLOAT> *_v2,vector <FLOAT> *_t1,vector <FLOAT> *_t2)
{
	PeakToPeakTime.clear();
	PeakToPeak.clear();
	ASSERT(_v1->size() == _t1->size());
	ASSERT(_v2->size() == _t2->size());

	vector <FLOAT>::iterator el1,elt1;
	elt1 = _t1->begin();
	el1 = _v1->begin();

	int index;
	PeakToPeakTime = *_t1;
	PeakToPeak.push_back(*el1 - _v2->at(0));
	for ( ; elt1 != _t1->end(); elt1++) {
		index = distance(_t2->begin(),lower_bound(_t2->begin(),_t2->end(),*elt1));
		
		if (index >= (int) _v2->size()) break;

		PeakToPeak.push_back(*el1 - _v2->at(index));
		el1++;
	}
}

void CDataSet::makeSumVector(vector <FLOAT> *_v1,vector <FLOAT> *_v2,vector <FLOAT> *_t1,vector <FLOAT> *_t2)
{
	PeakToPeakTime.clear();
	PeakToPeak.clear();
	ASSERT(_v1->size() == _t1->size());
	ASSERT(_v2->size() == _t2->size());

	vector <FLOAT>::iterator el1,elt1;
	elt1 = _t1->begin();
	el1 = _v1->begin();

	int index;
	PeakToPeakTime = *_t1;
	PeakToPeak.push_back(*el1 - _v2->at(0));
	for ( ; elt1 != _t1->end(); elt1++) {
		index = distance(_t2->begin(),lower_bound(_t2->begin(),_t2->end(),*elt1));
		
		if (index >= (int) _v2->size()) break;

		PeakToPeak.push_back(*el1 + _v2->at(index));
	}
}

/*
Description: Find the envelope curve based on a bipolar signal. Detects derivative sign change.
	Generates a new uniploar vector with a new time axis (fewer points than the input)

	Checks the slope 

	Shifts values 2 seconds to the left

	_v: Input vector
	_time: Input time axis
	_pp: Output vector
	_ppTime: Output time vector
	_avgSlope: Average slope (max - min) / (timeDiff of the two)
	_removeSwallow: Removes positive(!) peaks > 2x the previous peak. Used for swallow removal 
*/
void CDataSet::findPeakToPeak(vector <FLOAT> *_v,vector <FLOAT> *_time,vector <FLOAT> *_pp,vector <FLOAT> *_ppTime,
							  float *_avgSlope)
{
	ASSERT(_v->size() == _time->size());

	vector <FLOAT> localppTime;
	vector <FLOAT>::iterator el1,el2,elTime;
	_pp->clear();
	
	_pp->push_back(.0f);
	localppTime.push_back(.0f);
	_pp->push_back(_v->at(0));
	localppTime.push_back(.0f);
	
	el1 = _v->begin();
	el2 = _v->begin();
	elTime = _time->begin();
	el2++;
	FLOAT max = .0f;
	FLOAT min = .0f;
	FLOAT minTime = .0f;
	FLOAT maxTime = .0f;
	bool sign,oldSign;
	oldSign = sign = true;

	FLOAT candidateMax,candidateMin;
	candidateMax = candidateMin = .0f;
	BOOL beenNegative = FALSE;
	int sampNum = 0;
	float lastPP = .0f;
	float lastMax = .0f;
	float lastTime = .0f;
	float deltaTime = .0f;
	float slope = .0f;
	float slopeSum = .0f;
	int slopeCount = 0;
	for ( ; el2 != _v->end( ); ++el2, ++el1, ++elTime,++sampNum )	{
		sign = *el2 >= *el1;
		deltaTime = *elTime - lastTime;
		if ((oldSign) && (!sign)) {						// Maximal
			candidateMax = _v->at(sampNum);
			if ((candidateMax > .0f) && (beenNegative)) {
				max = candidateMax;

				_pp->push_back(max - min);
				lastPP = max - min;
				maxTime = *elTime;
				lastTime = maxTime; 
				lastMax = max;
				if (maxTime > minTime) {
					slope = lastPP / (maxTime - minTime);
					slopeSum += slope;
					slopeCount++;
				}
				localppTime.push_back(lastTime);
				beenNegative = FALSE;
			}
		}
		else if ((!oldSign) && (sign)) {				// Minimal
			candidateMin = _v->at(sampNum);
			if (candidateMin < .0f) {
				min = candidateMin;
				minTime = *elTime;
				beenNegative = TRUE;
			}
		}
		oldSign = sign;
	}

	//--_Check slope
	*_avgSlope = slopeSum / slopeCount;

	//---Shift time 2 sec left
	_ppTime->clear();
	_ppTime->resize(localppTime.size(),.0f);
	transform(localppTime.begin(),localppTime.end(),_ppTime->begin(),AddValue <FLOAT> (-2.0f));

	ASSERT(_pp->size() == _ppTime->size());
}


/*
Description: Find the envelope curve based on a bipolar signal. Detects derivative sign change.
Generates a new uniploar vector with a new time axis (fewer points than the input)

Uses a sliding 5 second window and takes the span in this window as the P-P-value

_v: Input vector
_time: Input time axis
_pp: Output vector
_ppTime: Output time vector
_removeSwallow: Removes positive(!) peaks > 2x the previous peak. Used for swallow removal
*/
void CDataSet::findPeakToPeak3(vector <FLOAT> *_v, vector <FLOAT> *_time, vector <FLOAT> *_pp, vector <FLOAT> *_ppTime)
{
	ASSERT(_v->size() == _time->size());

	if (PP_DETECT_WINDOW >= _v->size()) return;

	_pp->clear();
	_ppTime->clear();

	vector <FLOAT>::iterator it0 = _v->begin();
	vector <FLOAT>::iterator it1 = _v->begin();
	it1 += PP_DETECT_WINDOW;
	vector <FLOAT>::iterator itStop = _v->end();
	itStop -= PP_DETECT_WINDOW;

	vector <FLOAT>::iterator itTime = _time->begin();
	itTime += PP_DETECT_WINDOW / 2;
	for ( ; it0 < itStop; ++it0, ++it1, ++itTime) {
		auto result = minmax_element(it0, it1);
		FLOAT span = *result.second - *result.first;
		_pp->push_back(span);
		_ppTime->push_back(*itTime);
	}

	ASSERT(_pp->size() == _ppTime->size());
}

/*
Description: Find the envelope curve based on a bipolar signal. Detects derivative sign change.
Generates a new uniploar vector with a new time axis (fewer points than the input)

Checks the slope

Shifts values 2 or 0 seconds to the left

Also requires that an interval with the extremum in the centre, is parallel to the x-axis based on linear regression

_v: Input vector
_time: Input time axis
_pp: Output vector
_ppTime: Output time vector
_avgSlope: Average slope (max - min) / (timeDiff of the two)
_removeSwallow: Removes positive(!) peaks > 2x the previous peak. Used for swallow removal
*/
void CDataSet::findPeakToPeak2(vector <FLOAT> *_v, vector <FLOAT> *_time, vector <FLOAT> *_pp, vector <FLOAT> *_ppTime,
	float *_avgSlope)
{
	ASSERT(_v->size() == _time->size());
	
	if (_v->size() < (BASELINE_LENGTH_SAMPLES + 1)) return;

	vector <FLOAT> localppTime;
	vector <FLOAT>::iterator el1, el2, elTime,elEnd;
	_pp->clear();

	_pp->push_back(.0f);
	localppTime.push_back(.0f);
	_pp->push_back(_v->at(0));
	localppTime.push_back(.0f);

	el1 = _v->begin();
	el1 += BASELINE_LENGTH_SAMPLES / 2;

	elTime = _time->begin();
	elTime += BASELINE_LENGTH_SAMPLES / 2;

	elEnd = _v->end();
	elEnd -= BASELINE_LENGTH_SAMPLES / 2;

	el2 = el1;
	el2++;

	FLOAT max = .0f;
	FLOAT min = .0f;
	FLOAT minTime = .0f;
	FLOAT maxTime = .0f;
	bool sign, oldSign;
	oldSign = sign = true;

	FLOAT candidateMax, candidateMin;
	candidateMax = candidateMin = .0f;
	BOOL beenNegative = FALSE;
	float lastPP = .0f;
	float lastMax = .0f;
	float lastTime = .0f;
	float deltaTime = .0f;
	float slope = .0f;
	float slopeSum = .0f;
	int slopeCount = 0;
	float centre = .0f;
	float useCentre = centre;
	for (; el2 != elEnd ; ++el2, ++el1, ++elTime) {
		sign = *el2 >= *el1;
		deltaTime = *elTime - lastTime;
		if ((oldSign) && (!sign)) {						// Maximal
			candidateMax = *el1;
			centre = getCentre(el1 - (int) (BASELINE_LENGTH / 2.0f), el1 + (int) (BASELINE_LENGTH / 2.0f));
			if ((candidateMax > centre) && (beenNegative)) {

				useCentre = centre;

				max = candidateMax;

				_pp->push_back(max - min);
				lastPP = max - min;
				maxTime = *elTime;
				lastTime = maxTime;
				lastMax = max;
				if (maxTime > minTime) {
					slope = lastPP / (maxTime - minTime);
					slopeSum += slope;
					slopeCount++;
				}
				localppTime.push_back(lastTime);
				beenNegative = FALSE;
			}
		}
		else if ((!oldSign) && (sign)) {				// Minimal
			candidateMin = *el1;
			if (candidateMin < useCentre) {

				min = candidateMin;
				minTime = *elTime;
				beenNegative = TRUE;
			}
		}
		oldSign = sign;
	}

	//--_Check slope
	*_avgSlope = slopeSum / slopeCount;

	//---Shift time 2 sec left
	_ppTime->clear();
	_ppTime->resize(localppTime.size(), .0f);
	transform(localppTime.begin(), localppTime.end(), _ppTime->begin(), AddValue <FLOAT>(-0.0f));  // *INN* was -2.0f

	ASSERT(_pp->size() == _ppTime->size());
}

/*
Description: 
	Based on where _v is flat, sets corresponding time window in _pp vector to zero
	Starts with a time window equal to MIN_FLAT_AREA_TIME_WINDOW
*/
void CDataSet::setFlatAreasToZero(vector <FLOAT> *_v,vector <FLOAT> *_time,vector <FLOAT> *_env,vector <FLOAT> *_envTime,
								  vector <FLOAT> *_baseline)
{
	//---In case of no variation
	auto inResult = minmax_element(_v->begin(),_v->end());
	if (inResult.first == inResult.second) return;

	//---Find flat areas and set pp to zero
	vector <FLOAT> copyEnv(_env->size());
	copy(_env->begin(),_env->end(),copyEnv.begin());
	vector <FLOAT>::iterator elTime,startIt,stopIt,startVIt,stopVIt,baseIt;
	elTime = _time->begin();
	for ( ; elTime < _time->end() ; ++elTime) {
		float to = *elTime + MIN_FLAT_AREA_TIME_WINDOW;
		stopIt = lower_bound(_time->begin(),_time->end(),to);
		
		if (stopIt >= _time->end()) break;
		
		startVIt = _v->begin() + distance(_time->begin(),elTime);
		stopVIt = _v->begin() + distance(_time->begin(),stopIt);
		vector <FLOAT>::iterator lowB = lower_bound(_envTime->begin(),_envTime->end(),*elTime);
		if (lowB >= _envTime->end()) break;

		unsigned int dist = distance(_envTime->begin(),lowB);
		
		if (dist >= _baseline->size()) break;

		baseIt = _baseline->begin() + dist;

		auto result = minmax_element(startVIt,stopVIt);
		float min = *result.first;
		float max = *result.second;
		float span = max - min;
		float limit = *baseIt * REL_BASELINE_FACTOR_FLAT_AREA_DETECTION;
		float tWindow = .0f;
		if (span < limit) {
			do {
				++stopVIt;
				result = minmax_element(startVIt,stopVIt);
				min = *result.first;
				max = *result.second;
				span = max - min;
				++stopIt;
				tWindow = stopIt < _time->end() ? *stopIt - *elTime : .0f ;
			} while ((span < limit) && (stopVIt < _v->end()) && (tWindow < MAX_EVENT_LENGTH));
			if (stopVIt < _v->end()) {
				int d0 = distance(_envTime->begin(),lower_bound(_envTime->begin(),_envTime->end(),*elTime));
				int d1 = distance(_envTime->begin(),lower_bound(_envTime->begin(),_envTime->end(),*stopIt));
				transform(			copyEnv.begin() + d0,
									copyEnv.begin() + d1,
									copyEnv.begin() + d0,SetValue <FLOAT> (.0f));
				elTime++ = stopIt;
			}
		}
		if (elTime >= _time->end()) break;
	}	
	copy(copyEnv.begin(),copyEnv.end(),_env->begin());
	return;
}

/*
Description: Find the envelope curve based on a bipolar signal. Detects derivative sign change.
Generates a new uniploar vector with a new time axis (fewer points than the input)

_v: Input vector
_time: Input time axis
_pp: Output vector
_ppTime: Output time vector
*/

void CDataSet::findPeakToPeak(vector <FLOAT> *_v,FLOAT _sint,vector <FLOAT> *_pp,vector <FLOAT> *_ppTime)
{
	vector <FLOAT>::iterator el1,el2;
	
	_pp->push_back(.0f);
	_ppTime->push_back(.0f);
	_pp->push_back(_v->at(0));
	_ppTime->push_back(.0f);
	
	el1 = _v->begin();
	el2 = _v->begin();
	el2++;
	FLOAT max = .0f;
	FLOAT min = .0f;
	FLOAT minTime = .0f;
	FLOAT maxTime = .0f;
	bool sign,oldSign;
	oldSign = sign = true;
	long sampNum = 0L;

	FLOAT candidateMax,candidateMin;
	candidateMax = candidateMin = .0f;
	BOOL beenNegative = FALSE;
	for ( ; el2 != _v->end( ); el2++ )	{
		sign = *el2 >= *el1;
		el1++;
		if ((oldSign) && (!sign)) {	// Maximal
			candidateMax = _v->at(sampNum);
			if ((candidateMax > .0f) && (beenNegative)) {
				max = candidateMax;
				_pp->push_back(max - min);
				maxTime = sampNum * _sint;
				_ppTime->push_back((maxTime + minTime) / 2.0f);
				beenNegative = FALSE;
			}
		}
		if ((!oldSign) && (sign)) {	// Minimal
			candidateMin = _v->at(sampNum);
			if (candidateMin < .0f) {
				min = candidateMin;
				minTime = sampNum * _sint;
				beenNegative = TRUE;
			}
		}
		oldSign = sign;
		sampNum++;
	}
}

void CDataSet::findEnvelope(vector <FLOAT> *_v,FLOAT _sint,vector <FLOAT> *_env,vector <FLOAT> *_envTime)
{
	vector <FLOAT>::iterator el1,el2;
	
	_env->push_back(.0f);
	_envTime->push_back(.0f);
	_env->push_back(_v->at(0));
	_envTime->push_back(.0f);
	
	el1 = _v->begin();
	el2 = _v->begin();
	el2++;
	FLOAT max = .0f;
	FLOAT min = .0f;
	bool sign,oldSign;
	oldSign = sign = true;
	long sampNum = 0L;

	//---Pos side
	for ( ; el2 != _v->end( ); el2++ )	{
		sign = *el2 >= *el1;
		el1++;
		if ((oldSign) && (!sign)) {	// Maximal
			max = _v->at(sampNum);
			_env->push_back(max);
			_envTime->push_back(sampNum * _sint);
		}
		oldSign = sign;
		sampNum++;
	}
	_env->push_back(.0f);
	_envTime->push_back(sampNum * _sint);

	//---Neg side
	el2 =_v->end();
	el2--;
	el1 = el2;
	el1--;
	oldSign = sign = true;
	sampNum = _v->size() - 1;
	for ( ; el1 != _v->begin( ); el1-- )	{
		sign = *el2 >= *el1;
		el2--;
		if ((oldSign) && (!sign)) {	// Minimal
			min = _v->at(sampNum);
			_env->push_back(min);
			_envTime->push_back(sampNum * _sint);
		}
		oldSign = sign;
		sampNum--;
	}
	_env->push_back(.0f);
	_envTime->push_back(.0f);
}

void CDataSet::setDataType(const unsigned int _dataType,CString _internalName)
{
	dataType = _dataType;
	internalName = _internalName != _T("") ? _internalName : internalName ;
}

void CDataSet::addToXYRaw(FLOAT _val,FLOAT _time)
{
	xyRaw.push_back(_val);
	xyRawTime.push_back(_time);
}

void CDataSet::addToXYRaw(FLOAT _val)
{
	xyRaw.push_back(_val);
}

int CDataSet::getSize(void)
{
	return xyRaw.size();
}

void CDataSet::dataSetIsCompleteFromAPN(void)
{
	dataSetIsCompleteFromTorso();
}

void CDataSet::dataSetIsCompleteFromAGS(void)
{
	dataSetIsCompleteFromTorso();
}

void CDataSet::dataSetIsCompleteFromTorso(void)
{
	//---Do computations according to dataType

	//---Find peak to peak
	//---Probably completely useless code, both for AGSBrowser and Spiro Analysis- test below is never satisfied
	if (dataType & dataTypeEnvelope) {
		if (dataSint > .0f) {
			Envelope.clear();
			EnvelopeTime.clear();
			findEnvelope(&xyRaw,dataSint,&Envelope,&EnvelopeTime);
		}
		if (dataSint > .0f) {
			PeakToPeak.clear();
			PeakToPeakTime.clear();
			findPeakToPeak(&xyRaw,dataSint,&PeakToPeak,&PeakToPeakTime);
		}
	}
}

void CDataSet::clear(void)
{
	//---Clear all vectors
}

void CDataSet::setDataSint(FLOAT _dataSint)
{
	dataSint = _dataSint;
}

/*
Description: 
	Returns the basis time vector 
*/
vector <FLOAT> *CDataSet::getTimeaxisVector(void)
{
	return &timeaxis;
}


/*
Description: 
	Returns the penDown vector 
*/
vector <BOOL> *CDataSet::getPenDownVector(void)
{
	return &penDown;
}

/*
Description: 
	Returns the time vector corresponding data vector of type _type
	In case it is equal sample interval and no time axis, NULL is returned.
*/
vector <FLOAT> *CDataSet::getTimeVector(const unsigned int _type)
{
	switch (_type) {
		case dataTypeXYRaw			:  { return &xyRawTime;			break; }
		case dataTypePeakToPeak		:  { return &PeakToPeakTime;	break; }
		case dataTypeBaselineAvg	:  { return &BaselineAvgTime;	break; }
		case dataTypeBaselineMax	:  { return &BaselineMaxTime;	break; }
		case dataTypeEnvelope		:  { return &EnvelopeTime;	break; }

	}
	return NULL;
}
//
///*
//Description:
//	Provides max and min of time data - returned in _min and _max
//*/
//void CDataSet::getXLimits(FLOAT *_min,FLOAT *_max)
//{
//	*_min = minX;
//	*_max = maxX;
//}

/*
Description:
	Sets max and min of time data 
*/
void CDataSet::setXLimits(FLOAT _min,FLOAT _max)
{
	minX = _min;
	maxX = _max;
}

/*
Description: 
	Returns the sample interval.
	In case there is a time vector and no fixed sample interval, zero is returned.
*/
FLOAT CDataSet::getSampleInterval(const unsigned int _type)
{switch (_type) {
		case dataTypeXYRaw			:  { return dataSint ; break; }
		case dataTypePeakToPeak		:  { return .0f ; break; }
		case dataTypeBaselineAvg	:  { return .0f ; break; }
		case dataTypeBaselineMax	:  { return .0f ; break; }
		case dataTypeEnvelope		:  { return .0f ; break; }
	}
	return dataSint;
}

/*
Description: Returns the data vector of type _type
*/
vector <FLOAT> *CDataSet::getDataVector(const unsigned int _type)
{
	switch (_type) {
		case dataTypeXYRaw			:  { return &xyRaw;			break; }
		case dataTypePeakToPeak		:  { return &PeakToPeak;	break; }
		case dataTypeBaselineAvg	:  { return &BaselineAvg;	break; }
		case dataTypeBaselineMax	:  { return &BaselineMax;	break; }
		case dataTypeEnvelope		:  { return &Envelope;		break; }
	}
	return NULL;
}

//////////////////////////////////

CBatteryDataSet::CBatteryDataSet()
{
	maxVoltage = minVoltage = avgVoltage = .0f;
	maxCurrent = minCurrent = avgCurrent = .0f;
	maxTemp = minTemp = avgTemp = .0f;
	maxRemainingCap = minRemainingCap = avgRemainingCap = .0f;
	maxFullChargeCap = minFullChargeCap = avgFullChargeCap = .0f;
	maxRemainingTime = minRemainingTime = avgRemainingTime = .0f;
}


CBatteryDataSet::~CBatteryDataSet() 
{
}

void CBatteryDataSet::dataSetIsCompleteFromAPN(void)
{
	dataSetIsCompleteFromTorso();
}

void CBatteryDataSet::dataSetIsCompleteFromAGS(void)
{
	dataSetIsCompleteFromTorso();
}

void CBatteryDataSet::dataSetIsCompleteFromTorso(void)
{
	//---Do computations according to dataType
	if (!voltage.size()) return;
	if (!current.size()) return;
	if (!temp.size()) return;

	maxVoltage = *max_element(voltage.begin(), voltage.end()); 
	minVoltage = *min_element(voltage.begin(), voltage.end()); 
	avgVoltage = (float)for_each(voltage.begin(), voltage.end(), Average());

	maxCurrent = *max_element(current.begin(), current.end()); 
	minCurrent = *min_element(current.begin(), current.end()); 
	avgCurrent = (float)for_each(current.begin(), current.end(), Average());

	maxTemp = *max_element(temp.begin(), temp.end()); 
	minTemp = *min_element(temp.begin(), temp.end()); 
	avgTemp = (float)for_each(temp.begin(), temp.end(), Average());
}

void CBatteryDataSet::clear(void)
{
	voltage.clear();
	voltage.clear();
	current.clear();
	temp.clear();
	remainingCap.clear();
	fullChargeCap.clear();
	remainingTime.clear();
	timeaxis.clear();
}

void CBatteryDataSet::getStartValues(FLOAT *_voltage,FLOAT *_current,FLOAT *_temp,FLOAT *_remainingCap,FLOAT *_fullChargeCap,FLOAT *_remainingTime,FLOAT *_time)
{
	if (!voltage.size()) return;
	*_voltage = *voltage.begin();
	*_current = *current.begin();
	*_temp = *temp.begin();
	*_remainingCap = *remainingCap.begin();
	*_fullChargeCap = *fullChargeCap.begin();
	*_remainingTime = *remainingTime.begin();
	*_time = *timeaxis.begin();
}


void CBatteryDataSet::getEndValues(FLOAT *_voltage,FLOAT *_current,FLOAT *_temp,FLOAT *_remainingCap,FLOAT *_fullChargeCap,FLOAT *_remainingTime,FLOAT *_time)
{
	if (!voltage.size()) return;
	*_voltage = *(voltage.end() - 1);
	*_current = *(current.end() - 1);
	*_temp = *(temp.end() - 1);
	*_remainingCap = *(remainingCap.end() - 1);
	*_fullChargeCap = *(fullChargeCap.end() - 1);
	*_remainingTime = *(remainingTime.end() - 1);
	*_time = *(timeaxis.end() - 1);
}


void CBatteryDataSet::addToXYRaw(
	FLOAT _voltage,
	FLOAT _current,
	FLOAT _temp,
	FLOAT _remainingCap,
	FLOAT _fullChargeCap,
	FLOAT _remainingTime,
	FLOAT _time)
{
	voltage.push_back(_voltage);	
	current.push_back(_current);
	temp.push_back(_temp);
	remainingCap.push_back(_remainingCap);
	fullChargeCap.push_back(_fullChargeCap);
	remainingTime.push_back(_remainingTime);
	timeaxis.push_back(_time);
}

int CBatteryDataSet::getSize(void)
{
	return timeaxis.size();
}


vector <FLOAT> *CBatteryDataSet::getVoltageVector(void)
{
	return &voltage;
}

vector <FLOAT> *CBatteryDataSet::getCurrentVector(void)
{
	return &current;
}

vector <FLOAT> *CBatteryDataSet::getTemperatureVector(void)
{
	return &temp;
}

vector <FLOAT> *CBatteryDataSet::getRemainingCapacityVector(void)
{
	return &remainingCap;
}

vector <FLOAT> *CBatteryDataSet::getFullChargeCapacityVector(void)
{
	return &fullChargeCap;
}

vector <FLOAT> *CBatteryDataSet::getRemainingTimeVector(void)
{
	return &remainingTime;
}

void CBatteryDataSet::getVoltageMinMax(FLOAT *_min,FLOAT *_max,FLOAT *_avg)
{
	*_min = minVoltage;
	*_max = maxVoltage;
	*_avg = avgVoltage;
}

void CBatteryDataSet::Serialize(CArchive& ar,int _mainFileVersion, int _minorFileVersion)
{
	if (ar.IsStoring()) {

		ASSERT(timeaxis.size() == current.size());
		ASSERT(timeaxis.size() == temp.size());
		ASSERT(timeaxis.size() == remainingCap.size());
		ASSERT(timeaxis.size() == fullChargeCap.size());
		ASSERT(timeaxis.size() == remainingTime.size());

		unsigned int size = timeaxis.size();
		ar << size;
		for (unsigned int i = 0 ; i < size ; i++) {
			ar << timeaxis[i];
			ar << voltage[i];   // Added in version 4.0
			ar << current[i];
			ar << temp[i];
			ar << remainingCap[i];
			ar << fullChargeCap[i];
			ar << remainingTime[i];
		}
		ar << maxVoltage;	 // Added in version 4.0
		ar << minVoltage;	 // Added in version 4.0
		ar << avgVoltage;	 // Added in version 4.0
		ar << maxCurrent;	 // Added in version 4.0
		ar << minCurrent;	 // Added in version 4.0
		ar << avgCurrent;	 // Added in version 4.0
		ar << maxTemp;		 // Added in version 4.0
		ar << minTemp;		 // Added in version 4.0
		ar << avgTemp;		 // Added in version 4.0
	}
	else {
		timeaxis.clear();
		current.clear();
		temp.clear();
		remainingCap.clear();
		fullChargeCap.clear();
		remainingTime.clear();

		unsigned int size;
		ar >> size;
		if (_mainFileVersion >= 4) {
			FLOAT val;
			for (unsigned int i = 0; i < size; i++) {
				ar >> val;
				timeaxis.push_back(val);
				ar >> val;
				voltage.push_back(val);
				ar >> val;
				current.push_back(val);
				ar >> val;
				temp.push_back(val);
				ar >> val;
				remainingCap.push_back(val);
				ar >> val;
				fullChargeCap.push_back(val);
				ar >> val;
				remainingTime.push_back(val);
			}
			ar >> maxVoltage	;
			ar >> minVoltage	;
			ar >> avgVoltage	;
			ar >> maxCurrent	;
			ar >> minCurrent	;
			ar >> avgCurrent	;
			ar >> maxTemp		;
			ar >> minTemp		;
			ar >> avgTemp		;
		}
		else {
			FLOAT val;
			for (unsigned int i = 0; i < size; i++) {
				ar >> val;
				timeaxis.push_back(val);
				ar >> val;
				voltage.push_back(.0f);  // Must contain data as saving relies on it.
				current.push_back(val);
				ar >> val;
				temp.push_back(val);
				ar >> val;
				remainingCap.push_back(val);
				ar >> val;
				fullChargeCap.push_back(val);
				ar >> val;
				remainingTime.push_back(val);
			}
			dataSetIsCompleteFromAGS();
		}
		evaluateData();
	}
}

unsigned int CBatteryDataSet::evaluateData(void)
{
	if (!voltage.size()) return FLAG_NO_DATA;
	FLOAT v1 = *voltage.begin();
	FLOAT v2 = *(voltage.end() - 1);
	unsigned int u = FLAG_NONE;
	if (v2 < BATT_V_LIMIT) u |= FLAG_VOLTAGE_LOW;

	FLOAT rc = *(remainingCap.end() - 1);
	if (rc < BATT_REMAINING_CHARGE_CAPACITY_LIMIT) u |= FLAG_REMAINING_CAPACITY_LOW;
		
	FLOAT fc = *(fullChargeCap.end() - 1);
	if (fc < BATT_FULL_CHARGE_CAPACITY_LIMIT) u |= FLAG_FULL_CHARGE_CAPACITY_LOW;

	vector <FLOAT>::iterator it;
	int cnt = 0;
	for (it = temp.begin() ; it < temp.end() ; it++) {
		if (*it > BATT_TEMP_LIMIT_DEGC) cnt++;
	}
	if (cnt > 0) u |= FLAG_TEMP_HIGH;

	if (avgCurrent > BATT_CURRENT_LIMIT) u |= FLAG_HIGH_CURRENT;

	return u;
}

////////////////////////////////////

CContactMicDataSet::CContactMicDataSet() 
{
}

CContactMicDataSet::~CContactMicDataSet() 
{
}

void CContactMicDataSet::clear(void) 
{
	cm.clear();
	timeaxis.clear();
}

void CContactMicDataSet::addToXYRaw(FLOAT _cm,FLOAT _time)
{
	cm.push_back(_cm);
	timeaxis.push_back(_time);
}

vector <FLOAT> *CContactMicDataSet::getCmVector(void)
{
	return &cm;
}

int CContactMicDataSet::getSize(void)
{
	return timeaxis.size();
}

unsigned int CContactMicDataSet::evaluateData(FLOAT *_percBad)
{
	vector <FLOAT>::iterator it;
	int cnt = 0;
	for (it = cm.begin() ; it < cm.end() ; it++) {
		if ((*it < .0f) || (*it > CM_MAX_LIMIT)) 
			cnt++;
	}
	*_percBad = (float) 100.0f * cnt / cm.size();

	if (cnt > 1) return FLAG_BAD_CM;
	
	return 0;
}

void CContactMicDataSet::Serialize(CArchive& ar,int majorFileVersion,int minorFileVersion)
{
	if (ar.IsStoring()) {
		ASSERT(cm.size() == timeaxis.size());
		unsigned int size = timeaxis.size();
		ar << size;
		for (unsigned int i = 0 ; i < size ; i++) {
			ar << timeaxis[i];
			ar << cm[i];
		}
	}
	else {
		timeaxis.clear();
		cm.clear();

		unsigned int size;
		ar >> size;
		FLOAT val;
		for (unsigned int i = 0 ; i < size ; i++) {
			ar >> val;
			timeaxis.push_back(val);
			ar >> val;
			cm.push_back(val);
		}
		dataSetIsCompleteFromAGS();
		FLOAT percBad;
		evaluateData(&percBad);
	}
}


/////////////////////////////////////////

CAirMicDataSet::CAirMicDataSet()
{
}

CAirMicDataSet::~CAirMicDataSet()
{
}

void CAirMicDataSet::clear(void) 
{
	am1.clear();
	am2.clear();
	am3.clear();
	am4.clear();
	timeaxis.clear();
}

void CAirMicDataSet::dataSetIsCompleteFromTorso(void)
{
	ASSERT(am1.size() == timeaxis.size());
	ASSERT(am2.size() == timeaxis.size());
	ASSERT(am3.size() == timeaxis.size());
	ASSERT(am4.size() == timeaxis.size());
	amSum.clear();
	vector <FLOAT>::iterator it1,it2,it3,it4;
	it1 = am1.begin();
	it2 = am2.begin();
	it3 = am3.begin();
	it4 = am4.begin();
	for ( ; it1 < am1.end() ; it1++, it2++, it3++, it4++) {
		amSum.push_back(*it1 + *it2 + *it3 + *it4);
	}
}

void CAirMicDataSet::addToXYRaw(FLOAT _am1,FLOAT _am2,FLOAT _am3,FLOAT _am4,FLOAT _time)
{
	am1.push_back(_am1);
	am2.push_back(_am2);
	am3.push_back(_am3);
	am4.push_back(_am4);
	timeaxis.push_back(_time);
}

int CAirMicDataSet::getSize(void)
{
	return timeaxis.size();
}

vector <FLOAT> *CAirMicDataSet::getAm1Vector(void)
{
	return &am1;
}

vector <FLOAT> *CAirMicDataSet::getAm2Vector(void)
{
	return &am2;
}

vector <FLOAT> *CAirMicDataSet::getAm3Vector(void)
{
	return &am3;
}

vector <FLOAT> *CAirMicDataSet::getAm4Vector(void)
{
	return &am4;
}

vector <FLOAT> *CAirMicDataSet::getAmSumVector(void)
{
	return &amSum;
}

unsigned int CAirMicDataSet::evaluateData(FLOAT *_percBad)
{
	vector <FLOAT>::iterator it1,it2,it3,it4;
	int cnt = 0;
	it2 = am2.begin();
	it3 = am3.begin();
	it4 = am4.begin();
	for (it1 = am1.begin() ; it1 < am1.end() ; it1++) {
		if ((*it1 < .0f) || (*it1 > AM_MAX_LIMIT) ||
			(*it2 < .0f) || (*it2 > AM_MAX_LIMIT) ||
			(*it3 < .0f) || (*it3 > AM_MAX_LIMIT) ||
			(*it4 < .0f) || (*it4 > AM_MAX_LIMIT))
			cnt++;
		it2++;
		it3++;
		it4++;
	}
	*_percBad = (float) 100.0f * cnt / am1.size();

	if (cnt > 1) return FLAG_BAD_AM;
	
	return 0;
}

void CAirMicDataSet::Serialize(CArchive& ar, int majorFileVersion, int minorFileVersion)
{
	if (ar.IsStoring()) {

		ASSERT(timeaxis.size() == am1.size());
		ASSERT(timeaxis.size() == am2.size());
		ASSERT(timeaxis.size() == am3.size());
		ASSERT(timeaxis.size() == am4.size());
		ASSERT(timeaxis.size() == amSum.size());

		unsigned int size = timeaxis.size();
		ar << size;
		for (unsigned int i = 0 ; i < size ; i++) {
			ar << timeaxis[i];
			ar << am1[i];
			ar << am2[i];
			ar << am3[i];
			ar << am4[i];
			ar << amSum[i];
		}
	}
	else {
		timeaxis.clear();
		am1.clear();
		am2.clear();
		am3.clear();
		am4.clear();

		unsigned int size;
		ar >> size;
		FLOAT val;
		if (majorFileVersion >= 4) {
			for (unsigned int i = 0; i < size; i++) {
				ar >> val;
				timeaxis.push_back(val);
				ar >> val;
				am1.push_back(val);
				ar >> val;
				am2.push_back(val);
				ar >> val;
				am3.push_back(val);
				ar >> val;
				am4.push_back(val);
				ar >> val;
				amSum.push_back(val);  // Added for version 4
			}
		}
		else {
			for (unsigned int i = 0; i < size; i++) {
				ar >> val;
				timeaxis.push_back(val);
				ar >> val;
				am1.push_back(val);
				ar >> val;
				am2.push_back(val);
				ar >> val;
				am3.push_back(val);
				ar >> val;
				am4.push_back(val);
			}
			dataSetIsCompleteFromAGS();
		}
		FLOAT percBad;
		evaluateData(&percBad);
	}
}

/////////////////////////////////////////

CBodyPosXYZDataSet::CBodyPosXYZDataSet()
{
}

CBodyPosXYZDataSet::~CBodyPosXYZDataSet()
{
}

void CBodyPosXYZDataSet::clear(void) 
{
	x.clear();
	y.clear();
	z.clear();
	timeaxis.clear();
}

void CBodyPosXYZDataSet::addToXYRaw(FLOAT _x,FLOAT _y,FLOAT _z,FLOAT _time)
{
	x.push_back(_x);
	y.push_back(_y);
	z.push_back(_z);
	timeaxis.push_back(_time);
}


vector <FLOAT> *CBodyPosXYZDataSet::getxVector(void)
{
	return &x;
}

vector <FLOAT> *CBodyPosXYZDataSet::getyVector(void)
{
	return &y;
}

vector <FLOAT> *CBodyPosXYZDataSet::getzVector(void)
{
	return &z;
}

int CBodyPosXYZDataSet::getSize(void)
{
	return timeaxis.size();
}

/*
Description: Does nothing
*/
void CBodyPosXYZDataSet::dataSetIsCompleteFromAGS(void)
{
}

/*
Description: Does nothing
*/
void CBodyPosXYZDataSet::dataSetIsCompleteFromAPN(void)
{
}
/*
Description: Check the time axis for regularity and filters the x,y,z components
*/
void CBodyPosXYZDataSet::dataSetIsCompleteFromTorso(void)
{
	CLP_02Hz_Filter lpF;
	lpF.filter(&timeaxis,&x,&y,&z);
}

unsigned int CBodyPosXYZDataSet::evaluateData(FLOAT *_percBad)
{
	vector <FLOAT>::iterator itx,ity,itz;
	int cnt = 0;
	ity = y.begin();
	itz = z.begin();
	for (itx = x.begin() ; itx < x.end() ; itx++) {
		if ((*itx < BP_LOW_XYZ_LIMIT) || (*itx > BP_HIGH_XYZ_LIMIT) ||
			(*ity < BP_LOW_XYZ_LIMIT) || (*ity > BP_HIGH_XYZ_LIMIT) ||
			(*itz < BP_LOW_XYZ_LIMIT) || (*itz > BP_HIGH_XYZ_LIMIT))
			cnt++;
		ity++;
		itz++;
	}
	*_percBad = (float) 100.0f * cnt / x.size();

	if (cnt > 1) return FLAG_BAD_BP;
	
	return 0;
}

void CBodyPosXYZDataSet::Serialize(CArchive& ar, int majorFileVersion, int minorFileVersion)
{
	if (ar.IsStoring()) {

		ASSERT(timeaxis.size() == x.size());
		ASSERT(timeaxis.size() == y.size());
		ASSERT(timeaxis.size() == z.size());

		unsigned int size = timeaxis.size();
		ar << size;
		for (unsigned int i = 0 ; i < size ; i++) {
			ar << timeaxis[i];
			ar << x[i];
			ar << y[i];
			ar << y[i];
		}
	}
	else {
		timeaxis.clear();
		x.clear();
		y.clear();
		z.clear();

		unsigned int size;
		ar >> size;
		FLOAT val;
		for (unsigned int i = 0 ; i < size ; i++) {
			ar >> val;
			timeaxis.push_back(val);
			ar >> val;
			x.push_back(val);
			ar >> val;
			y.push_back(val);
			ar >> val;
			z.push_back(val);
		}
		dataSetIsCompleteFromAGS();
		FLOAT percBad;
		evaluateData(&percBad);
	}
}


/////////////////////////////////////////

CBodyPosDataSet::CBodyPosDataSet()
{
	timeInPosLeft		= .0f;
	timeInPosRight		= .0f;
	timeInPosSupine		= .0f;
	timeInPosProne		= .0f;
	timeInPosUpright	= .0f;
	timeInPosUndef		= .0f;

	timeSleepingInPosLeft = .0f;
	timeSleepingInPosRight = .0f;
	timeSleepingInPosSupine = .0f;
	timeSleepingInPosProne = .0f;
	timeSleepingInPosUpright = .0f;
	timeSleepingInPosUndef = .0f;

	timeAwakeInPosLeft = .0f;
	timeAwakeInPosRight = .0f;
	timeAwakeInPosSupine = .0f;
	timeAwakeInPosProne = .0f;
	timeAwakeInPosUpright = .0f;
	timeAwakeInPosUndef = .0f;
}

CBodyPosDataSet::~CBodyPosDataSet()
{
}

void CBodyPosDataSet::clear(void) 
{
	bp.clear();
	timeaxis.clear();
}

void CBodyPosDataSet::addToXYRaw(short _bp,FLOAT _time,bool _lastSample)
{
	static short lastVal = posUndefined;
	static FLOAT lastTime;

	if (_lastSample) {
		//---Lift pen
		bp.push_back(_bp);
		timeaxis.push_back(_time);
		penDown.push_back(FALSE);
		return;
	}

	if (0 == bp.size()) {

		bp.push_back((FLOAT) _bp);
		lastVal = _bp;
		lastTime = _time;
		timeaxis.push_back(_time);
		penDown.push_back(TRUE);
		return;
	}
	if (_bp == lastVal) {
		lastTime = _time;
		return;
	}

	//---Lift pen
	bp.push_back(lastVal);
	timeaxis.push_back(lastTime);
	penDown.push_back(FALSE);

	//---Drop pen
	bp.push_back(_bp);
	timeaxis.push_back(_time);
	penDown.push_back(TRUE);
	lastVal = _bp;
	lastTime = _time;
}

vector <FLOAT> *CBodyPosDataSet::getBpVector(void)
{
	return &bp;
}

int CBodyPosDataSet::getSize(void)
{
	return timeaxis.size();
}

float CBodyPosDataSet::getTimeInPosLeft()
{
	return timeInPosLeft;
}


float CBodyPosDataSet::getTimeInPosRight(void)
{
	return timeInPosRight;
}

float CBodyPosDataSet::getTimeInPosSupine(void)
{
	return timeInPosSupine;
}

float CBodyPosDataSet::getTimeInPosProne(void)
{
	return timeInPosProne;
}

float CBodyPosDataSet::getTimeInPosUpright(void)
{
	return timeInPosUpright;
}

float CBodyPosDataSet::getTimeInPosUndef(void)
{
	return timeInPosUndef;
}

float CBodyPosDataSet::getTimeSleepingInPosLeft(void)
{
	return timeSleepingInPosLeft;
}

float CBodyPosDataSet::getTimeSleepingInPosRight(void)
{
	return timeSleepingInPosRight;
}

float CBodyPosDataSet::getTimeSleepingInPosSupine(void)
{
	return timeSleepingInPosSupine;
}

float CBodyPosDataSet::getTimeSleepingInPosProne(void)
{
	return timeSleepingInPosProne;
}

float CBodyPosDataSet::getTimeSleepingInPosUpright(void)
{
	return timeSleepingInPosUpright;
}

float CBodyPosDataSet::getTimeSleepingInPosUndef(void)
{
	return timeSleepingInPosUndef;
}

float CBodyPosDataSet::getTimeAwakeInPosLeft(void)
{
	return timeAwakeInPosLeft;
}

float CBodyPosDataSet::getTimeAwakeInPosRight(void)
{
	return timeAwakeInPosRight;
}

float CBodyPosDataSet::getTimeAwakeInPosSupine(void)
{
	return timeAwakeInPosSupine;
}

float CBodyPosDataSet::getTimeAwakeInPosProne(void)
{
	return timeAwakeInPosProne;
}

float CBodyPosDataSet::getTimeAwakeInPosUpright(void)
{
	return timeAwakeInPosUpright;
}

float CBodyPosDataSet::getTimeAwakeInPosUndef(void)
{
	return timeAwakeInPosUndef;
}

/*
Description: return seconds excluded. The seconds must be between t0 and t1
_excludedEventArray: The excluded event array
_t0: Start of time window to check
_t1: End of time window to check
*/
float CBodyPosDataSet::getTimeExcluded(CArray <CExcludedEvnt *, CExcludedEvnt *> *_excludedEventArray, float _t0, float _t1)
{
	ASSERT(_t0 <= _t1);

	float sectionLength = .0f;
	int num = _excludedEventArray->GetCount();
	for (int i = 0; i < num; i++) {
		CExcludedEvnt *evP = _excludedEventArray->GetAt(i);
		float from = evP->getFrom();
		float to = evP->getTo();
		if ((_t1 > from) && (_t0 < to)) {  // Overlap
			float left = from > _t0 ? from : _t0;
			float right = to < _t1 ? to : _t1;
			sectionLength += right - left;
		}
	}
	return sectionLength;
}

/*
Description: return seconds awake. The seconds must be between t0 and t1

Note that we may have excluded and awake at the same time

_excludedEventArray: The excluded event array
_awakeEventArray: The awake event array
_t0: Start of time window to check
_t1: End of time window to check
*/
float CBodyPosDataSet::getTimeAwakeAndNotExcluded(CArray <CExcludedEvnt *, CExcludedEvnt *> *_excludedEventArray,
	CArray <CAwakeEvnt *, CAwakeEvnt *> *_awakeEventArray, float _t0, float _t1)
{
	ASSERT(_t0 <= _t1);

	float sectionLength = .0f;
	int num = _awakeEventArray->GetCount();
	for (int i = 0; i < num; i++) {
		CAwakeEvnt *evP = _awakeEventArray->GetAt(i);
		float from = evP->getFrom();
		float to = evP->getTo();
		if ((_t1 > from) && (_t0 < to)) {  // Overlap
			float left = from > _t0 ? from : _t0;
			float right = to < _t1 ? to : _t1;
			sectionLength += right - left;
			sectionLength -= getTimeExcluded(_excludedEventArray, left, right);
		}
	}
	return sectionLength;
}

void CBodyPosDataSet::computeTimeInPositions(CArray <CExcludedEvnt *, CExcludedEvnt *> *_excludedEventArray)
{
	ASSERT(penDown.size() == bp.size());
	ASSERT(timeaxis.size() == bp.size());

	timeInPosLeft		= .0f;
	timeInPosRight		= .0f;
	timeInPosSupine		= .0f;
	timeInPosProne		= .0f;
	timeInPosUpright	= .0f;
	timeInPosUndef		= .0f;

	vector <FLOAT>::iterator bpIt,timeIt;
	vector <BOOL>::iterator pdIt;

	pdIt = penDown.begin();
	timeIt = timeaxis.begin();
	float lastTime = .0f;
	for (bpIt = bp.begin() ; bpIt < bp.end() ; bpIt++) {
		if (TRUE == *pdIt) {
			lastTime = *timeIt;
		}
		else {
			short pos = (short) *bpIt;
			float time = *timeIt;
			switch (pos) {
			case posLeft:
				timeInPosLeft += time - lastTime;
				if (_excludedEventArray)
					timeInPosLeft -= getTimeExcluded(_excludedEventArray,lastTime,time);
				break;
			case posRight:
				timeInPosRight += time - lastTime;
				if (_excludedEventArray)
					timeInPosRight -= getTimeExcluded(_excludedEventArray, lastTime, time);
				break;
			case posProne:
				timeInPosProne += time - lastTime;
				if (_excludedEventArray)
					timeInPosProne -= getTimeExcluded(_excludedEventArray, lastTime, time);
				break;
			case posSupine:
				timeInPosSupine += time - lastTime;
				if (_excludedEventArray)
					timeInPosSupine -= getTimeExcluded(_excludedEventArray, lastTime, time);
				break;
			case posUpright:
				timeInPosUpright += time - lastTime;
				if (_excludedEventArray)
					timeInPosUpright -= getTimeExcluded(_excludedEventArray, lastTime, time);
				break;
			case posUndefined:
				timeInPosUndef += time - lastTime;
				if (_excludedEventArray)
					timeInPosUndef -= getTimeExcluded(_excludedEventArray, lastTime, time);
				break;
			default:
				break;
			}
		}
		pdIt++;
		timeIt++;
	}
}

void CBodyPosDataSet::computeTimeSleepingInPositions(CArray<CExcludedEvnt*, CExcludedEvnt*>* _excludedEventArray, CArray<CAwakeEvnt*, CAwakeEvnt*>* _awakeEventArray)
{
	ASSERT(penDown.size() == bp.size());
	ASSERT(timeaxis.size() == bp.size());

	timeSleepingInPosLeft = .0f;
	timeSleepingInPosRight = .0f;
	timeSleepingInPosSupine = .0f;
	timeSleepingInPosProne = .0f;
	timeSleepingInPosUpright = .0f;
	timeSleepingInPosUndef = .0f;

	timeAwakeInPosLeft = .0f;
	timeAwakeInPosRight = .0f;
	timeAwakeInPosSupine = .0f;
	timeAwakeInPosProne = .0f;
	timeAwakeInPosUpright = .0f;
	timeAwakeInPosUndef = .0f;

	vector <FLOAT>::iterator bpIt, timeIt;
	vector <BOOL>::iterator pdIt;

	pdIt = penDown.begin();
	timeIt = timeaxis.begin();
	float lastTime = .0f;
	for (bpIt = bp.begin(); bpIt < bp.end(); bpIt++) {
		if (TRUE == *pdIt) {
			lastTime = *timeIt;
		}
		else {
			short pos = (short)*bpIt;
			float time = *timeIt;
			switch (pos) {
			case posLeft:
				timeSleepingInPosLeft += time - lastTime;
				timeSleepingInPosLeft -= getTimeExcluded(_excludedEventArray, lastTime, time);
				timeSleepingInPosLeft -= getTimeAwakeAndNotExcluded(_excludedEventArray,
					_awakeEventArray, lastTime, time);

				timeAwakeInPosLeft += getTimeAwakeAndNotExcluded(_excludedEventArray,
					_awakeEventArray, lastTime, time);
				break;
			case posRight:
				timeSleepingInPosRight += time - lastTime;
				timeSleepingInPosRight -= getTimeExcluded(_excludedEventArray, lastTime, time);
				timeSleepingInPosRight -= getTimeAwakeAndNotExcluded(_excludedEventArray,
					_awakeEventArray, lastTime, time);

				timeAwakeInPosRight += getTimeAwakeAndNotExcluded(_excludedEventArray,
					_awakeEventArray, lastTime, time);
				break;
			case posProne:
				timeSleepingInPosProne += time - lastTime;
				timeSleepingInPosProne -= getTimeExcluded(_excludedEventArray, lastTime, time);
				timeSleepingInPosProne -= getTimeAwakeAndNotExcluded(_excludedEventArray,
					_awakeEventArray, lastTime, time);

				timeAwakeInPosProne += getTimeAwakeAndNotExcluded(_excludedEventArray,
					_awakeEventArray, lastTime, time);
				break;
			case posSupine:
				timeSleepingInPosSupine += time - lastTime;
				timeSleepingInPosSupine -= getTimeExcluded(_excludedEventArray, lastTime, time);
				timeSleepingInPosSupine -= getTimeAwakeAndNotExcluded(_excludedEventArray,
					_awakeEventArray, lastTime, time);

				timeAwakeInPosSupine += getTimeAwakeAndNotExcluded(_excludedEventArray,
					_awakeEventArray, lastTime, time);
				break;
			case posUpright:
				timeSleepingInPosUpright += time - lastTime;
				timeSleepingInPosUpright -= getTimeExcluded(_excludedEventArray, lastTime, time);
				timeSleepingInPosUpright -= getTimeAwakeAndNotExcluded(_excludedEventArray,
					_awakeEventArray, lastTime, time);

				timeAwakeInPosUpright += getTimeAwakeAndNotExcluded(_excludedEventArray,
					_awakeEventArray, lastTime, time);
				break;
			case posUndefined:
				timeSleepingInPosUndef += time - lastTime;
				timeSleepingInPosUndef -= getTimeExcluded(_excludedEventArray, lastTime, time);
				timeSleepingInPosUndef -= getTimeAwakeAndNotExcluded(_excludedEventArray,
					_awakeEventArray, lastTime, time);

				timeAwakeInPosUndef += getTimeAwakeAndNotExcluded(_excludedEventArray,
					_awakeEventArray, lastTime, time);
				break;
			default:
				break;
			}
		}
		pdIt++;
		timeIt++;
	}
}

float CBodyPosDataSet::getPercentInSupine(void)
{
	float totalTime = timeInPosSupine + timeInPosProne + timeInPosRight +
		timeInPosLeft + timeInPosUndef + timeInPosUpright;

	return totalTime == .0f ? 0.0f : 100.0f * timeInPosSupine / totalTime;
}


/*
Description: Returns true if sometime in this time window - position is upright
Input to this function must be start and stop seconds
*/
bool CBodyPosDataSet::getIUsUprightInWindow(float _startInterval, float _stopInterval)
{
	if (timeaxis.size() < 2) return false;

	vector <FLOAT>::iterator startIt = lower_bound(timeaxis.begin(), timeaxis.end(), _startInterval);
	vector <FLOAT>::iterator stopIt = lower_bound(timeaxis.begin(), timeaxis.end(), _stopInterval);
	if (stopIt >= timeaxis.end()) stopIt = timeaxis.end() - 1;

	if (stopIt < startIt) return false;
	else if (stopIt == startIt) {
		vector <FLOAT>::iterator bpi;
		bpi = bp.begin() + (startIt - timeaxis.begin());
		short pos = (short)*bpi;
		return posUpright == pos ? true : false;
	}

	vector <FLOAT>::iterator bpP, s1, s2;
	s1 = bp.begin() + distance(timeaxis.begin(),startIt);
	s2 = bp.begin() + distance(timeaxis.begin(),stopIt);
	short cPos;
	short posInInterval = 0x00;
	for (bpP = s1; bpP < s2; bpP++) {
		cPos = (short)*bpP;
		if (posUpright == cPos) return true;
	}
	return false;
}

/*
Description: Returns body position in the time interval. If the position changes in the interval, undefined is returned.
Input to this function must be start and stop seconds
*/
short CBodyPosDataSet::getPos(float _startInterval,float _stopInterval)
{
	if (timeaxis.size() < 2) return posUndefined;

	vector <FLOAT>::iterator startIt = lower_bound(timeaxis.begin(),timeaxis.end(),_startInterval);
	vector <FLOAT>::iterator stopIt = lower_bound(timeaxis.begin(),timeaxis.end(),_stopInterval);
	if (stopIt >= timeaxis.end()) stopIt = timeaxis.end() - 1;

	if (stopIt < startIt) return posUndefined;
	else if (stopIt == startIt) {
		vector <FLOAT>::iterator bpi;
		bpi = bp.begin() + (startIt - timeaxis.begin());
		return (short) *bpi;
	}
	
	vector <FLOAT>::iterator bpP,s1,s2;
	s1 = bp.begin() + (startIt - timeaxis.begin());
	s2 = bp.begin() + (stopIt - timeaxis.begin());
	short cPos;
	short posInInterval = 0x00;
	for (bpP = s1 ; bpP < s2 ; bpP++) {
		cPos = (short) *bpP;
		switch (cPos) {
		case posSupine:
			posInInterval |= 0x01;
			break;
		case posLeft:
			posInInterval |= 0x02;
			break;
		case posRight:
			posInInterval |= 0x04;
			break;
		case posProne:
			posInInterval |= 0x08;
			break;
		case posUpright:
			posInInterval |= 0x10;
			break;
		case posUndefined:
			posInInterval |= 0x20;
			break;
		default:
			break;
		}

	}
	int numbits = 0;
	numbits += ((posInInterval & 0x01) > 0);
	numbits += ((posInInterval & 0x02) > 0);
	numbits += ((posInInterval & 0x04) > 0);
	numbits += ((posInInterval & 0x08) > 0);
	numbits += ((posInInterval & 0x10) > 0);
	numbits += ((posInInterval & 0x20) > 0);
	if (numbits > 1) return posUndefined;
	if (posInInterval & 0x01) return posSupine;
	if (posInInterval & 0x02) return posLeft;
	if (posInInterval & 0x04) return posRight;
	if (posInInterval & 0x08) return posProne;
	if (posInInterval & 0x10) return posUpright;
	if (posInInterval & 0x20) return posUndefined;
	return posUndefined;
}

/*
Description: Returns body position in the time interval. If the position changes in the interval, undefined is returned.
Input to this function must be iterators to a time vector
*/
short CBodyPosDataSet::getPos(vector <FLOAT>::iterator _startInterval,vector <FLOAT>::iterator _stopInterval)
{
	vector <FLOAT>::iterator eventIterator,posTime;
	short cPos;
	short posInInterval = 0x00;
	for (eventIterator = _startInterval ; eventIterator < _stopInterval ; eventIterator++) {
		float ffff = *eventIterator;
		posTime = lower_bound(timeaxis.begin(),timeaxis.end(),*eventIterator);
		unsigned int pt = posTime - timeaxis.begin();
		
		if (pt >= bp.size()) break;

		cPos = (short) bp.at(pt);
		switch (cPos) {
		case posSupine:
			posInInterval |= 0x01;
			break;
		case posLeft:
			posInInterval |= 0x02;
			break;
		case posRight:
			posInInterval |= 0x04;
			break;
		case posProne:
			posInInterval |= 0x08;
			break;
		case posUpright:
			posInInterval |= 0x10;
			break;
		case posUndefined:
			posInInterval |= 0x20;
			break;
		default:
			break;
		}
	}
	int numbits = 0;
	numbits += ((posInInterval & 0x01) > 0);
	numbits += ((posInInterval & 0x02) > 0);
	numbits += ((posInInterval & 0x04) > 0);
	numbits += ((posInInterval & 0x08) > 0);
	numbits += ((posInInterval & 0x10) > 0);
	numbits += ((posInInterval & 0x20) > 0);
	if (numbits > 1) return posUndefined;
	if (posInInterval & 0x01) return posSupine;
	if (posInInterval & 0x02) return posLeft;
	if (posInInterval & 0x04) return posRight;
	if (posInInterval & 0x08) return posProne;
	if (posInInterval & 0x10) return posUpright;
	if (posInInterval & 0x20) return posUndefined;
	return posUndefined;
}

unsigned int CBodyPosDataSet::evaluateData(FLOAT *_percBad)
{
	vector <FLOAT>::iterator it;
	int cnt = 0;
	for (it = bp.begin() ; it < bp.end() ; it++) {
		if ((*it < BP_LOW_LIMIT) || (*it > BP_HIGH_LIMIT))			
			cnt++;
	}
	*_percBad = (float) 100.0f * cnt / bp.size();

	if (cnt > 1) return FLAG_BAD_BP;
	
	return 0;
}

void CBodyPosDataSet::generateDataFromXYZ(vector <FLOAT> *_x,
											vector <FLOAT> *_y,
											vector <FLOAT> *_z,
											vector <FLOAT> *_time)
{
	if (0 == _time->size()) return;

	ASSERT(_x->size() == _time->size());
	ASSERT(_y->size() == _time->size());
	ASSERT(_z->size() == _time->size());
	
	bp.clear();
	timeaxis.clear();
	penDown.clear();
	
	int num = _x->size();
	float xV,yV,zV,hangle,yzVmag,vangle;
	float sqSum = .0f;
	short lastPos = posSupine;
	short thisPos = posSupine;
	float lastTime = .0f;
	for (int i = 0 ; i < num ; i++) {
		xV = _x->at(i);
		yV = _y->at(i);
		zV = _z->at(i);
		sqSum = xV * xV + yV * yV + zV * zV;
		if (sqSum > .2f) {
			yzVmag = sqrt(yV * yV + zV * zV);
			vangle = atan(fabs(xV) / yzVmag);
			if (vangle > deg75) {
				//---Upright
				thisPos = posUpright;
				lastPos = thisPos;
			}
			else if ((vangle <= deg75) && (vangle > deg65)) {
				//---In hysteresis window
				thisPos = lastPos;
			}
			else if (vangle <= deg65) {
				hangle = atan2(yV,zV);
				if ((hangle < deg40) && (hangle > deg_40)) {
					thisPos = posSupine;
					lastPos = thisPos;
				}
				else if ((hangle >= deg40) && (hangle < deg50)) {
					thisPos = lastPos;
				}
				else if ((hangle >= deg50) && (hangle < deg130)) {
					thisPos = posRight;
					lastPos = thisPos;
				}
				else if ((hangle >= deg130) && (hangle < deg140)) {
					thisPos = lastPos;
				}
				else if ((hangle >= deg_50) && (hangle < deg_40)) {
					thisPos = lastPos;
				}
				else if ((hangle >= deg_130) && (hangle < deg_50)) {
					thisPos = posLeft;
					lastPos = thisPos;
				}
				else if ((hangle >= deg_140) && (hangle < deg_130)) {
					thisPos = lastPos;
				}
				else {
					thisPos = posProne;
					lastPos = thisPos;
				}
			}
			else {
				thisPos = lastPos;
			}
			addToXYRaw(thisPos,_time->at(i));
			lastTime = _time->at(i);
		}
	}
	addToXYRaw(lastPos,lastTime,true);

	ASSERT(bp.size() == timeaxis.size());
	ASSERT(penDown.size() == timeaxis.size());
}


void CBodyPosDataSet::Serialize(CArchive& ar, int majorFileVersion, int minorFileVersion)
{
	if (ar.IsStoring()) {

		ASSERT(timeaxis.size() == bp.size());

		unsigned int size = timeaxis.size();
		ar << size;
		for (unsigned int i = 0 ; i < size ; i++) {
			ar << timeaxis[i];
			ar << bp[i];
		}
		ar << timeInPosLeft		;	// Added in version 4 
		ar << timeInPosRight	;	// Added in version 4 
		ar << timeInPosSupine	;	// Added in version 4 
		ar << timeInPosProne	;	// Added in version 4 
		ar << timeInPosUpright	;	// Added in version 4 
		ar << timeInPosUndef	;	// Added in version 4 
	}
	else {
		timeaxis.clear();
		bp.clear();

		unsigned int size;
		ar >> size;
		FLOAT val;
		BOOL pen = TRUE;
		for (unsigned int i = 0 ; i < size ; i++) {
			ar >> val;
			timeaxis.push_back(val);
			penDown.push_back(pen);
			pen = !pen;
			ar >> val;
			bp.push_back(val);
		}
		if (majorFileVersion >= 4) {
			ar >> timeInPosLeft;	// Added in version 4 
			ar >> timeInPosRight;	// Added in version 4 
			ar >> timeInPosSupine;	// Added in version 4 
			ar >> timeInPosProne;	// Added in version 4 
			ar >> timeInPosUpright;	// Added in version 4 
			ar >> timeInPosUndef;	// Added in version 4 
		}
		else {
			dataSetIsCompleteFromAGS();
		}
		FLOAT percBad;
		evaluateData(&percBad);
	}
}


/////////////////////////////////////////

COximeterDataSet::COximeterDataSet()
{	
	meanO2avg2 = meanPulseRate = .0f;
	medianO2avg2 = medianPulseRate = .0f;
	minO2Avg2 = maxO2Avg2 = .0f;
	minPulseRate = maxPulseRate = .0f;
	timeAtMinO2Avg2 = timeAtMaxO2Avg2 = - 1.0f;			// Flag that these data do not exist (used only in AGS file version >=5)
	timeAtMinPulseRate = timeAtMaxPulseRate = -1.0f;	// Flag that these data do not exist (used only in AGS file version >=5)
	invalidTime = .0f;
	validTime = .0f;
}

COximeterDataSet::~COximeterDataSet()
{
}

void COximeterDataSet::clear(void) 
{
	o2avg2.clear();
	pulseRate.clear();
	flags.clear();
	timeaxis.clear();
}

/*
	Returns true if _minSamples or more consecutive elements are below or equal to _limit
	It returns as soon as a set has been found! - it does not scan the entire vector. 
	So make sure to start with a low limit!

	_actualSamples contains the actual number of samples
*/
bool COximeterDataSet::areNOrMoreBelowLimit(int _minSamples, vector <FLOAT>::iterator first, vector <FLOAT>::iterator last, size_t *_start, 
	size_t *_end, float _limit, int *_actualSamples)
{
	if (first == last) return false;

	int cnt = 0;
	vector <FLOAT>::iterator it = first;
	while (it != last) {
		if (*it <= _limit) {
			if (0 == cnt) *_start = distance(first, it);
			cnt++;
		}
		else {
			if (cnt >= _minSamples) {
				*_end = distance(first, it);
				*_actualSamples = cnt;
				return true;
			}
			cnt = 0; // Reset counter
		}
		++it;
	}
	return false;
}


/*
	Returns true if _minSamples or more consecutive elements are above or equal to _limit
	It returns as soon as a set has been found! - it does not scan the entire vector. 
	So make sure to start with a low limit!

	_actualSamples contains the actual number of samples
*/
bool COximeterDataSet::areNOrMoreAboveLimit(int _minSamples, vector <FLOAT>::iterator first, vector <FLOAT>::iterator last, size_t *_start, 
	size_t *_end, float _limit, int *_actualSamples)
{
	if (first == last) return false;

	int cnt = 0;
	vector <FLOAT>::iterator it = first;
	while (it != last) {
		if (*it >= _limit) {
			if (0 == cnt) *_start = distance(first, it);
			cnt++;
		}
		else {
			if (cnt >= _minSamples) {
				*_end = distance(first, it);
				*_actualSamples = cnt;
				return true;
			}
			cnt = 0; // Reset counter
		}
		++it;
	}
	return false;
}

/*
Description: Pushes data to vectors. Sampling rate and o2Avg8 are ignored
*/
void COximeterDataSet::addToXYRaw(FLOAT _samplingRate,FLOAT _O2avg2,FLOAT _O2avg8,FLOAT _pulseRate,unsigned short _flags,FLOAT _time)
{
	o2avg2.push_back(_O2avg2);
	pulseRate.push_back(_pulseRate);
	flags.push_back(_flags);
	timeaxis.push_back(_time);
}

/*
Description:

Source     Filtering		Find Min,max,median			Remove bad samples	Find valid time		Populate Histogram
--------------------------------------------------------------------------------------------------------------------
Torso			x					x							x					x						x
APN				x					x							x					x						x
AGS				o					x							o					o						x
*/
void COximeterDataSet::dataSetIsCompleteFromTorso(void)
{
	vector <FLOAT>::iterator i1,iPR,i2,it;
	vector <unsigned short>::iterator fi;

	if (0 == o2avg2.size()) return;
	
	ASSERT(flags.size() == o2avg2.size());
	ASSERT(flags.size() == pulseRate.size());
	ASSERT(flags.size() == timeaxis.size());

	findInvalidAndValidTimeAndFilter();
}


/*
Description: Skips data points at extremes (254 and 0) and skips points where the flags indicate error
Uses this to skip outliers (negative only for SpO2) based on n*Inner quartile range,
where n = IQR_SPO2 for SpO2. 

Filter
*/
void COximeterDataSet::findInvalidAndValidTimeAndFilter(void)
{
	if (0 == timeaxis.size()) return;
	if (oxUseRaw) {
		if (!oxNoFilter) {
			CLP_5sampleRect_Filter lpF;
			lpF.filter(&timeaxis, &o2avg2, &pulseRate);
		}
		penDown.clear();
		penDown.resize(timeaxis.size(), TRUE);
		return;
	}

	invalidTimeWindows.clear();

	vector <FLOAT>::iterator t0, t1, i1, iPR;
	t0 = timeaxis.begin();
	t1 = t0;
	t1++;
	vector <unsigned short>::iterator fi;
	fi = flags.begin();
	i1 = o2avg2.begin() + 1;
	iPR = pulseRate.begin() + 1;
	bool wasValid = NO_FLAGS == *fi ? true : false;
	TIME_WINDOW tw;
	vector <FLOAT> padP, padO, padT;
	if (!wasValid) tw.begin = *t0;
	for (fi = flags.begin() + 1; fi < flags.end(); fi++, i1++, iPR++, t0++, t1++) {
		if ((NO_FLAGS == *fi) && (*i1 > .0f) && (*i1 < 254.0f) && (*iPR > .0f) && (*iPR < 254.0f)) {
			if (!wasValid) {
				tw.end = *t1;
				invalidTimeWindows.push_back(tw);
			}
			padP.push_back(*iPR);
			padO.push_back(*i1);
			padT.push_back(*t1);
			wasValid = true;
		}
		else {
			if (!wasValid) tw.begin = *t0;
			wasValid = false;
		}
	}
	float sPo2Limit = .0f;
	if (padO.size() > 0) {
		float medianO;
		float iqrO = getIQR(&padO, &medianO);
		float lowMargin = max(IQR_SPO2 * iqrO, SPO2_LOW_OUTLIER_LIMIT);
		sPo2Limit = medianO - lowMargin;
	}

	float PRHLimit = .0f;
	float PRLLimit = 300.0f;
	if (padP.size() > 0) {
		float medianP;
		float iqrPR = getIQR(&padP, &medianP);
		PRHLimit = medianP + IQR_PR * iqrPR;
		PRLLimit = medianP - IQR_PR * iqrPR;
	}

	//---Remove SpO2 outliers. Limit is either (IQR_SPO2 * iqrO) or (SPO2_LOW_OUTLIER_LIMIT) below median
	//---SpO2 set to last good value
	i1 = padO.begin();
	t0 = padT.begin();
	iPR = padP.begin();
	float oldVal = 95.0f; // reasonable start value
	wasValid = true;
	for (; i1 < padO.end(); ++i1, ++t0,++iPR ) {
		if ((*i1 < sPo2Limit) || (*iPR > PRHLimit) || (*iPR < PRLLimit)) {
			*i1 = oldVal;
			if (wasValid) {
				tw.begin = *t0;
				wasValid = false;
			}
		}
		else {
			oldVal = *i1;
			if (!wasValid) {
				tw.end = *t0;
				wasValid = true;
				invalidTimeWindows.push_back(tw);
			}
		}
	}

	//---Sort invalidTimeWindows
	sort(invalidTimeWindows.begin(), invalidTimeWindows.end(), compareTimeWindows);

	//---Filter
	if (!oxNoFilter) {
		CLP_5sampleRect_Filter lpF;
		lpF.filter(&padT, &padO, &padP);
	}

	//---Copy back data
	o2avg2.clear();
	pulseRate.clear();
	timeaxis.clear();
	o2avg2.resize(padO.size(),.0f);
	pulseRate.resize(padP.size(), .0f);
	timeaxis.resize(padT.size(), .0f);
	flags.resize(padT.size(),0);
	copy(padO.begin(), padO.end(), o2avg2.begin());
	copy(padP.begin(), padP.end(), pulseRate.begin());
	copy(padT.begin(), padT.end(), timeaxis.begin());

	//---Sum up valid/invalid and generate penDownVector
	validTime = .0f;
	invalidTime = .0f;
	penDown.clear();
	penDown.resize(timeaxis.size(), TRUE);
	vector <TIME_WINDOW>::iterator itw;
	vector <FLOAT>::iterator it0, it1;
	vector <BOOL>::iterator bi = penDown.begin();
	itw = invalidTimeWindows.begin();
	for (; itw < invalidTimeWindows.end(); ++itw) {
		tw = *itw;
		invalidTime += (tw.end - tw.begin);

		it0 = lower_bound(timeaxis.begin(), timeaxis.end(), tw.begin);
		it1 = upper_bound(timeaxis.begin(), timeaxis.end(), tw.end);
		size_t d0 = distance(timeaxis.begin(), it0);
		size_t d1 = distance(timeaxis.begin(), it1);
		if ((d1 < timeaxis.size()) && (d0 < timeaxis.size())) {
			generate(penDown.begin() + d0, penDown.begin() + d1, ConstFalse);
		}
	}
	if (timeaxis.size() >= 1) {
		validTime = timeaxis.at(timeaxis.size() - 1) - timeaxis.at(0);
		validTime -= invalidTime;
	}
	else validTime = .0f;
}


/*
Description:

Source     Filtering		Find Min,max,median			Remove bad samples	Find valid time		Populate Histogram
--------------------------------------------------------------------------------------------------------------------
Torso			x					x							x					x						x
APN				x					x							x					x						x
AGS				o					x							o					o						x
*/

void COximeterDataSet::dataSetIsCompleteFromAGS(void)
{
	if (0 == o2avg2.size()) return;
	
	ASSERT(timeaxis.size() == o2avg2.size());
	ASSERT(timeaxis.size() == pulseRate.size());

	//---Nothing here. All was done when reading from APN or Torso
	
}


/*
Description:

Source     Filtering		Find Min,max,median			Remove bad samples	Find valid time		Populate Histogram
--------------------------------------------------------------------------------------------------------------------
Torso			x					x							x					x						x
APN				x					x							x					x						x
AGS				o					x							o					o						x
*/
void COximeterDataSet::dataSetIsCompleteFromAPN(void)
{
	if (0 == o2avg2.size()) return;
	
	ASSERT(flags.size() == o2avg2.size());
	ASSERT(flags.size() == pulseRate.size());
	ASSERT(flags.size() == timeaxis.size());

	findInvalidAndValidTimeAndFilter();
}

///*
//Description: Fill a histogram with number of seconds in the interval
//*/
//void COximeterDataSet::toPRHistogram(float _secs,float _pr)
//{
//	if (_pr < 60.0f) {
//		pulseRateHisto[pulseBelow60] += _secs;
//		return;
//	}
//	if (_pr < 70.0f) {
//		pulseRateHisto[pulseFrom60to70] += _secs;
//		return;
//	}
//	if (_pr < 80.0f) {
//		pulseRateHisto[pulseFrom70to80] += _secs;
//		return;
//	}
//	if (_pr < 90.0f) {
//		pulseRateHisto[pulseFrom80to90] += _secs;
//		return;
//	}
//	if (_pr < 100.0f) {
//		pulseRateHisto[pulseFrom90to100] += _secs;
//		return;
//	}
//	if (_pr < 110.0f) {
//		pulseRateHisto[pulseFrom100to110] += _secs;
//		return;
//	}
//	if (_pr < 120.0f) {
//		pulseRateHisto[pulseFrom110to120] += _secs;
//		return;
//	}
//	pulseRateHisto[pulseAbove120] += _secs;
//}
//
///*
//Description: Fill a histogram with number of seconds in the interval
//*/
//void COximeterDataSet::toSpO2Histogram(float _secs,float _saturation)
//{
//	if (_saturation < 65.0f) {
//		spO2Histo[7] += _secs;
//		return;
//	}
//	if (_saturation < 70.0f) {
//		spO2Histo[6] += _secs;
//		return;
//	}
//	if (_saturation < 75.0f) {
//		spO2Histo[5] += _secs;
//		return;
//	}
//	if (_saturation < 80.0f) {
//		spO2Histo[4] += _secs;
//		return;
//	}
//	if (_saturation < 85.0f) {
//		spO2Histo[3] += _secs;
//		return;
//	}
//	if (_saturation < 90.0f) {
//		spO2Histo[2] += _secs;
//		return;
//	}
//	if (_saturation < 95.0f) {
//		spO2Histo[1] += _secs;
//		return;
//	}
//	spO2Histo[0] += _secs;
//}

//void COximeterDataSet::computeKeyData(CArray <CExcludedEvnt *, CExcludedEvnt *> *_excludedEventArray,
//													CArray <CAwakeEvnt *, CAwakeEvnt *> *_awakeEventArray)
//{
//	ASSERT(timeaxis.size() == o2avg2.size());
//	ASSERT(timeaxis.size() == pulseRate.size());
//	if (timeaxis.size() <= 2) return;
//
//	vector <FLOAT> tempOV, tempPV,tempTime;
//	tempOV.resize(o2avg2.size(), .0f);
//	tempPV.resize(o2avg2.size(), .0f);
//	tempTime.resize(o2avg2.size(), .0f);
//	copy(o2avg2.begin(), o2avg2.end(), tempOV.begin());
//	copy(pulseRate.begin(), pulseRate.end(), tempPV.begin());
//	copy(timeaxis.begin(), timeaxis.end(), tempTime.begin());
//	
//	//---Set up one common exclude/awake vector
//	vector <EVENT_SORT_VECTOR_ELEMENT> exclAwakeVector;
//	int numex = _excludedEventArray->GetCount();
//	for (int i = 0; i < numex; i++) {
//		EVENT_SORT_VECTOR_ELEMENT esve; 
//		CExcludedEvnt *evP = _excludedEventArray->GetAt(i);
//		esve.begin = evP->getFrom();
//		esve.end = evP->getTo();
//		esve.evP = NULL;				// Don't need this
//		esve.level = levelTypeUndef;	// Don't need this
//		esve.hypopneaDropLimit = 0;		// Don't need this
//		exclAwakeVector.push_back(esve);
//	}
//	int numaw = _awakeEventArray->GetCount();
//	for (int i = 0; i < numaw; i++) {
//		EVENT_SORT_VECTOR_ELEMENT esve;
//		CAwakeEvnt *evP = _awakeEventArray->GetAt(i);
//		esve.begin = evP->getFrom();
//		esve.end = evP->getTo();
//		esve.evP = NULL;					// Don't need this
//		esve.level = levelTypeUndef;		// Don't need this
//		esve.hypopneaDropLimit = 0;			// Don't need this
//		exclAwakeVector.push_back(esve);
//	}
//	//--There are no overlaps here now!
//	sort(exclAwakeVector.begin(), exclAwakeVector.end(), compareEventsInTime);
//
//	//---Remove excluded parts if 1 element or more to exclude
//	if (exclAwakeVector.size() >= 1) {
//		vector <EVENT_SORT_VECTOR_ELEMENT>::iterator it = exclAwakeVector.end() - 1;
//		while (it >= exclAwakeVector.begin()) {
//
//			vector <FLOAT>::iterator maxT = max_element(tempTime.begin(), tempTime.end());
//
//			EVENT_SORT_VECTOR_ELEMENT esve = *it;
//			if (esve.end < .0f) break;
//			if (esve.begin > *maxT) break;
//
//			vector <FLOAT>::iterator ip0, ip1, ipP0, ipP1,ipt0,ipt1;
//			unsigned int p0, p1;
//			if (esve.end > *maxT)
//				p1 = distance(tempTime.begin(), tempTime.end());
//			else
//				p1 = distance(tempTime.begin(), lower_bound(tempTime.begin(), tempTime.end(), esve.end));
//			ip1 = tempOV.begin();
//			ip1 += p1;
//			ipP1 = tempPV.begin();
//			ipP1 += p1;
//			ipt1 = tempTime.begin();
//			ipt1 += p1;
//
//			if (esve.begin < .0f)
//				p0 = 0;
//			else
//				p0 = distance(tempTime.begin(), lower_bound(tempTime.begin(), tempTime.end(), esve.begin));
//			ip0 = tempOV.begin();
//			ip0 += p0;
//			ipP0 = tempPV.begin();
//			ipP0 += p0;
//			ipt0 = tempTime.begin();
//			ipt0 += p0;
//
//			tempOV.erase(ip0, ip1);
//			tempPV.erase(ipP0, ipP1);
//			tempTime.erase(ipt0, ipt1);
//			
//			if (it == exclAwakeVector.begin()) 
//				break;
//			--it;
//		}
//	}
//
//	//---Compute median and average SpO2 and pulserate
//	//---Aake has now been removed
//	if (tempTime.size() >= 1) {
//		vector <FLOAT> mO2, mPR;
//		mO2.clear();
//		mPR.clear();
//		mO2.resize(tempOV.size(), .0f);
//		mPR.resize(tempPV.size(), .0f);
//		copy(tempOV.begin(), tempOV.end(), mO2.begin());
//		copy(tempPV.begin(), tempPV.end(), mPR.begin());
//		sort(mO2.begin(), mO2.end());
//		sort(mPR.begin(), mPR.end());
//		int pos = mO2.size() / 2;
//		medianO2avg2 = mO2.at(pos);
//		medianPulseRate = mPR.at(pos); 
//		vector <FLOAT>::iterator mO2It, mPRIt;
//		mO2It = mO2.begin();
//		mPRIt = mPR.begin();
//		float sumO2 = .0f;
//		float sumPR = .0f;
//		for (; mO2It < mO2.end(); ++mO2It, ++mPRIt) {
//			sumO2 += *mO2It;
//			sumPR += *mPRIt;
//		}
//		meanO2avg2 = sumO2 / (float)mO2.size();
//		meanPulseRate = sumPR / (float)mPR.size();
//	}
//
//	//---Now require that we are below this minimum for MIN_TIME_AT_MINIMUM_SPO2 seconds!
//	maxO2Avg2 = .0f;
//	minO2Avg2 = .0f;
//	maxPulseRate = .0f;
//	minPulseRate = .0f;
//	timeAtMaxO2Avg2 = .0f;
//	timeAtMinO2Avg2 = .0f;
//	timeAtMaxPulseRate = .0f;
//	timeAtMinPulseRate = .0f;
//
//	//---Find the minimum sample interval
//	float tDiffMin = 100.0f;
//	vector <FLOAT>::iterator i10 = timeaxis.begin();
//	vector <FLOAT>::iterator i2 = i10;
//	i2++;
//	for (; i10 < timeaxis.end() - 1; ++i10,++i2) {
//		float v = *i2 - *i10;
//		tDiffMin = v < tDiffMin ? v : tDiffMin;
//	}
//
//	int minSamples = (int) ((float) MIN_TIME_AT_EXTREMAL_SPO2_PR / tDiffMin);
//	minSamples = minSamples > 2 ? minSamples - 1 : minSamples;
//
//	//---SpO2 minimum
//	for (float tstMin = .0f; tstMin < 100.0f; tstMin += 1.0f) {
//		size_t start, stop;
//		start = stop = 0;
//		int actualSamples = 0;
//		bool found = areNOrMoreBelowLimit(minSamples,tempOV.begin(), tempOV.end(), &start, &stop,tstMin, &actualSamples);
//		if (found) {
//			if ((start < tempTime.size()) && (stop < tempTime.size())) {
//				float timeSpan = tempTime.at(stop) - tempTime.at(start);
//				if (timeSpan >= MIN_TIME_AT_EXTREMAL_SPO2_PR) {
//					if (timeSpan <= (tDiffMin * actualSamples)) {
//						minO2Avg2 = tstMin;
//						timeAtMinO2Avg2 = (tempTime.at(stop) + tempTime.at(start)) / 2.0f;
//						break;
//					}
//				}
//			}
//		}
//	}
//	//---SpO2 maximum
//	for (float tstMax = 101.0f; tstMax > .0f; tstMax -= 1.0f) {
//		size_t start, stop;
//		start = stop = 0;
//		int actualSamples = 0;
//		bool found = areNOrMoreAboveLimit(minSamples, tempOV.begin(), tempOV.end(), &start, &stop, tstMax, &actualSamples);
//		if (found) {
//			if ((start < tempTime.size()) && (stop < tempTime.size())) {
//				float timeSpan = tempTime.at(stop) - tempTime.at(start);
//				if (timeSpan >= MIN_TIME_AT_EXTREMAL_SPO2_PR) {
//					if (timeSpan <= (tDiffMin * actualSamples)) {
//						maxO2Avg2 = tstMax;
//						timeAtMaxO2Avg2 = (tempTime.at(stop) + tempTime.at(start)) / 2.0f;
//						break;
//					}
//				}
//			}
//		}
//	}
//	//---PR minimum
//	for (float tstMin = .0f; tstMin < 500.0f; tstMin += 1.0f) {
//		size_t start, stop;
//		start = stop = 0;
//		int actualSamples = 0;
//		bool found = areNOrMoreBelowLimit(minSamples, tempPV.begin(), tempPV.end(), &start, &stop, tstMin, &actualSamples);
//		if (found) {
//			if ((start < tempTime.size()) && (stop < tempTime.size())) {
//				float timeSpan = tempTime.at(stop) - tempTime.at(start);
//				if (timeSpan >= MIN_TIME_AT_EXTREMAL_SPO2_PR) {
//					if (timeSpan <= (tDiffMin * actualSamples)) {
//						minPulseRate = tstMin;
//						timeAtMinPulseRate = (tempTime.at(stop) + tempTime.at(start)) / 2.0f;
//						break;
//					}
//				}
//			}
//		}
//	}
//	//---PR maximum
//	for (float tstMax = 500.0f; tstMax > .0f; tstMax -= 1.0f) {
//		size_t start, stop;
//		start = stop = 0;
//		int actualSamples = 0;
//		bool found = areNOrMoreAboveLimit(minSamples, tempPV.begin(), tempPV.end(), &start, &stop, tstMax, &actualSamples);
//		if (found) {
//			if ((start < tempTime.size()) && (stop < tempTime.size())) {
//				float timeSpan = tempTime.at(stop) - tempTime.at(start);
//				if (timeSpan >= MIN_TIME_AT_EXTREMAL_SPO2_PR) {
//					if (timeSpan <= (tDiffMin * actualSamples)) {
//						maxPulseRate = tstMax;
//						timeAtMaxPulseRate = (tempTime.at(stop) + tempTime.at(start)) / 2.0f;
//						break;
//					}
//				}
//			}
//		}
//	}
//
//	populateHistograms(&exclAwakeVector);
//}

bool COximeterDataSet::getIsExcluded(size_t _index, CArray <CExcludedEvnt *, CExcludedEvnt *> *_excludedEventArray)
{
	int num = _excludedEventArray->GetCount();

	if (_index >= timeaxis.size()) return true;
	float time = timeaxis.at(_index);

	for (int i = 0; i < num; ++i) {
		CExcludedEvnt *evP = _excludedEventArray->GetAt(i);
		if ((time > evP->getFrom()) && (time < evP->getTo()))
			return true;
	}
	return false;
}

bool COximeterDataSet::getIsExcluded(float _time, CArray <CExcludedEvnt *, CExcludedEvnt *> *_excludedEventArray)
{
	int num = _excludedEventArray->GetCount();
	for (int i = 0; i < num; ++i) {
		CExcludedEvnt *evP = _excludedEventArray->GetAt(i);
		if ((_time > evP->getFrom()) && (_time < evP->getTo())) 
			return true;
	}
	return false;
}

//void COximeterDataSet::populateHistograms(vector <EVENT_SORT_VECTOR_ELEMENT> *_exclAwakeVector)
//{
//	// Remove excluded
//	for (int i = 0; i < satBinCount; i++) {
//		spO2Histo[i] = .0f;
//	}
//	for (int i = 0; i < pulseBinCount; i++) pulseRateHisto[i] = 0;
//
//	float start = timeaxis.at(0);
//	float stop = timeaxis.at(timeaxis.size() - 1);
//	for (float t = start; t <= stop; t += 1.0f) {  // 1 sec at a time
//		
//		//---Excluded/Awake filter
//		vector <EVENT_SORT_VECTOR_ELEMENT>::iterator it = _exclAwakeVector->begin();
//		bool exclude = false;
//		for (; it < _exclAwakeVector->end(); ++it) {
//			if ((t >= it->begin) && (t <= it->end)) {
//				exclude = true;
//				break;
//			}
//		}
//
//		//---Invalid filter
//		if (!exclude) {
//			vector <TIME_WINDOW>::iterator it = invalidTimeWindows.begin();
//			for (; it < invalidTimeWindows.end(); ++it) {
//				TIME_WINDOW tw = *it;
//				if ((t >= tw.begin) && (t <= tw.end)) {
//					exclude = true;
//					break;
//				}
//			}
//		}
//
//		//---Pen up filter
//		if (!exclude) {
//			vector <FLOAT>::iterator it = lower_bound(timeaxis.begin(), timeaxis.end(), t);
//			if (it <= timeaxis.end()) {
//				if (!penDown.at(distance(timeaxis.begin(), it))) {
//					exclude = true;
//					break;
//				}
//			}
//		}
//
//		//---Add if not excluded
//		if (!exclude) {
//			float pr = getPRAt(t);
//			float spO2 = getSpO2At(t);
//			toSpO2Histogram(1.0f, spO2);		// add 1 sec
//			toPRHistogram(1.0f, pr);			// add 1 sec
//		}
//	}
//}

float COximeterDataSet::getPRAt(float _t)
{
	vector <FLOAT>::iterator it = lower_bound(timeaxis.begin(), timeaxis.end(), _t);
	if (it >= timeaxis.end()) return .0f;
	int dist = distance(timeaxis.begin(), it);
	return pulseRate.at(dist);
}

float COximeterDataSet::getSpO2At(float _t)
{
	vector <FLOAT>::iterator it = lower_bound(timeaxis.begin(), timeaxis.end(), _t);
	if (it >= timeaxis.end()) return .0f;
	int dist = distance(timeaxis.begin(), it);
	return o2avg2.at(dist);
}

void COximeterDataSet::setPenDownVector(BOOL _down)
{
	penDown.clear();
	penDown.resize(timeaxis.size(), _down);
}

void COximeterDataSet::getO2KeyData(float *_mean,float *_median,float *_min,float *_max,float *_timeOfMin,float *_timeOfMax)
{
	*_mean = meanO2avg2;
	*_median = medianO2avg2;
	*_min = minO2Avg2;
	*_max = maxO2Avg2;
	*_timeOfMax = timeAtMaxO2Avg2;
	*_timeOfMin = timeAtMinO2Avg2;
}

void COximeterDataSet::getPRKeyData(float *_mean,float *_median,float *_min,float *_max,float *_timeOfMin, float *_timeOfMax)
{
	*_mean = meanPulseRate;
	*_median = medianPulseRate;
	*_min = minPulseRate;
	*_max = maxPulseRate;
	*_timeOfMax = timeAtMaxPulseRate;
	*_timeOfMin = timeAtMinPulseRate;
}

void COximeterDataSet::getPulseRateHistogramData(float *_first)
{
	memcpy(_first,&pulseRateHisto[0],pulseBinCount * sizeof(float));
}

void COximeterDataSet::getSpO2HistogramData8(float *_first)
{
	memcpy(_first,&spO2Histo[0],8 * sizeof(float));
}

float COximeterDataSet::getInvalidTime(void)
{
	return invalidTime;
}

bool COximeterDataSet::getIsDataValidAtTime(float _time)
{
	return getIsPenDown(_time);
}

bool COximeterDataSet::getIsPenDown(float _time)
{
	vector <FLOAT>::iterator it = lower_bound(timeaxis.begin(), timeaxis.end(), _time);
	if (it >= timeaxis.end()) return false;
	return (TRUE == penDown.at(distance(timeaxis.begin(), it)));
}

float COximeterDataSet::getValidTime(void)
{
	return validTime;
}

unsigned int COximeterDataSet::evaluateData(float *_percBad)
{
	if (!flags.size()) return 0;

	vector <unsigned short>::iterator it;
	int cnt = 0;
	for (it = flags.begin() ; it < flags.end() ; it++) {
		if (*it) cnt++;
	}
	*_percBad = (float) 100.0f * cnt / flags.size();

	if (cnt > 1) return FLAG_BAD_OXIMETRY;
	return 0;
}

int COximeterDataSet::getSize(void)
{
	return timeaxis.size();
}

vector <FLOAT> *COximeterDataSet::getSpO2Vector(void)
{
	return &o2avg2;
}

vector <unsigned short> *COximeterDataSet::getFlagsVector(void)
{
	return &flags;
}

vector <FLOAT> *COximeterDataSet::getPulseRateVector(void)
{
	return &pulseRate;
}

void COximeterDataSet::Serialize(CArchive& ar, int majorFileVersion, int minorFileVersion)
{
	if (ar.IsStoring()) {

		ASSERT(timeaxis.size() == o2avg2.size());
		ASSERT(timeaxis.size() == pulseRate.size());
		ASSERT(timeaxis.size() == penDown.size());

		unsigned int size = timeaxis.size();
		ar << size;
		for (unsigned int i = 0 ; i < size ; i++) {
			ar << timeaxis[i];
			ar << o2avg2[i];
			ar << pulseRate[i];
			ar << penDown[i];
		}
		unsigned int cnt = invalidTimeWindows.size();
		ar << cnt;
		for (unsigned int i = 0; i < cnt; i++) {
			TIME_WINDOW tw = invalidTimeWindows.at(i);
			ar << tw.begin;
			ar << tw.end;
		}
		ar << validTime;
		ar << invalidTime;		
		ar << meanO2avg2		;
		ar << meanPulseRate		;
		ar << medianO2avg2 		;
		ar << medianPulseRate	;	
		ar << minO2Avg2			;
		ar << timeAtMinO2Avg2	;
		ar << maxO2Avg2			;
		ar << timeAtMaxO2Avg2	;
		ar << minPulseRate 		;
		ar << timeAtMinPulseRate;
		ar << maxPulseRate		;
		ar << timeAtMaxPulseRate;
		for (unsigned int i = 0; i < 8; i++) {
			ar << spO2Histo[i];
		}
		for (unsigned int i = 0; i < pulseBinCount; i++) {
			ar << pulseRateHisto[i];
		}
	}
	else {
		timeaxis.clear();
		o2avg2.clear();
		pulseRate.clear();

		unsigned int size;
		ar >> size;
		if (majorFileVersion >= 4) {
			FLOAT val;
			BOOL bl;
			for (unsigned int i = 0; i < size; i++) {
				ar >> val;
				timeaxis.push_back(val);
				ar >> val;
				o2avg2.push_back(val);
				ar >> val;
				pulseRate.push_back(val);
				ar >> bl;
				penDown.push_back(bl);
			}
			if (majorFileVersion >= 6) {
				unsigned int cnt;
				ar >> cnt;
				invalidTimeWindows.clear();
				for (unsigned int i = 0; i < cnt; i++) {
					TIME_WINDOW tw;
					ar >> tw.begin;
					ar >> tw.end;
					invalidTimeWindows.push_back(tw);
				}
			}
			ar >> validTime;
			ar >> invalidTime;
			ar >> meanO2avg2;
			ar >> meanPulseRate;
			ar >> medianO2avg2;
			ar >> medianPulseRate;
			ar >> minO2Avg2;
			if (majorFileVersion >= 5) ar >> timeAtMinO2Avg2;
			ar >> maxO2Avg2;
			if (majorFileVersion >= 5) ar >> timeAtMaxO2Avg2;
			ar >> minPulseRate;
			if (majorFileVersion >= 5) ar >> timeAtMinPulseRate;
			ar >> maxPulseRate;
			if (majorFileVersion >= 5) ar >> timeAtMaxPulseRate;
			for (unsigned int i = 0; i < 8; i++) {
				ar >> spO2Histo[i];
			}
			for (unsigned int i = 0; i < pulseBinCount; i++) {
				ar >> pulseRateHisto[i];
			}
			dataSetIsCompleteFromAGS();
		}
		else {
			FLOAT val;
			for (unsigned int i = 0; i < size; i++) {
				ar >> val;
				timeaxis.push_back(val);
				ar >> val;
				o2avg2.push_back(val);
				ar >> val;
				pulseRate.push_back(val);
				penDown.push_back(TRUE);
			}
			ar >> validTime;
			ar >> invalidTime;
			dataSetIsCompleteFromAGS();
		}
	}
}

/////////////////////////////////////////

CActimeterDataSet::CActimeterDataSet()
{
	for (int i = 0 ; i < hanningWindowTaps ; i++) {
		hanningWindow.push_back(hanningW[i]);
	}
}

CActimeterDataSet::~CActimeterDataSet()
{
}

void CActimeterDataSet::clear(void) 
{
	ac.clear();
	flags.clear();
	timeaxis.clear();
}

void CActimeterDataSet::addToRawComponents(FLOAT _x, FLOAT _y, FLOAT _z, FLOAT _time)
{
	x.push_back(_x);
	y.push_back(_y);
	z.push_back(_z);
	timeaxis.push_back(_time);
}

void CActimeterDataSet::addToXYRaw(FLOAT _ac,unsigned short _flags,FLOAT _time)
{
	ac.push_back(_ac);
	flags.push_back(_flags);
	timeaxis.push_back(_time);
}

vector <FLOAT> *CActimeterDataSet::getAcVector(void)
{
	return &ac;
}

vector <FLOAT> *CActimeterDataSet::getWindowedAcVector(void)
{
	return &ac30secVector;
}

vector <FLOAT> *CActimeterDataSet::getBaselineVector(void)
{
	return &baselineVector;
}

vector <FLOAT> *CActimeterDataSet::getWindowedAcTimeVector(void)
{
	return &ac30secTimeVector;
}

float CActimeterDataSet::getActimeterSatLimit(void)
{
	return actimeterSatLimit;
}

void CActimeterDataSet::generateFromBodyXYZ(CBodyPosXYZDataSet *_bpxyz)
{
	vector <FLOAT> *tV = _bpxyz->getTimeaxisVector();
	vector <FLOAT> *xV = _bpxyz->getxVector();
	vector <FLOAT> *yV = _bpxyz->getyVector();
	vector <FLOAT> *zV = _bpxyz->getzVector();

	vector <FLOAT>::iterator tIt = tV->begin();
	vector <FLOAT>::iterator xIt = xV->begin();
	vector <FLOAT>::iterator yIt = yV->begin();
	vector <FLOAT>::iterator zIt = zV->begin();
	for (; tIt < tV->end(); ++tIt, ++xIt, ++yIt, ++zIt) {
		addToRawComponents(*xIt, *yIt, *zIt, *tIt);
	}
	dataSetIsCompleteFromAGS();
	float percBad;
	evaluateData(&percBad);
}

void CActimeterDataSet::dataSetIsCompleteFromTorso(void)
{
	if (0 == timeaxis.size()) return;

	//---If x,y,z are entered individually, high pass filter and compute resultant vector
	//---This section has been added for the Torso actimeter calculation where ac.size() == 0
	if (x.size() && y.size() && z.size() && !ac.size()) {
		ac.clear();
		flags.clear();
		vector <FLOAT>::iterator tIt = timeaxis.begin();
		vector <FLOAT>::iterator xIt = x.begin();
		vector <FLOAT>::iterator yIt = y.begin();
		vector <FLOAT>::iterator zIt = z.begin();
		for (; tIt < timeaxis.end(); ++tIt, ++xIt, ++yIt, ++zIt) {
			float angle = (float) atan2((double)*yIt, (double)*xIt);
			ac.push_back(angle);
			flags.push_back(0);
		}
		makeDerivative(&ac, &timeaxis);
		transform(ac.begin(),ac.end(),ac.begin(), FabsAnddB <FLOAT> ());
		FLOAT min = *min_element(ac.begin(), ac.end());
		transform(ac.begin(), ac.end(),ac.begin(), AddValue <FLOAT>(torsoActimeterdBLowestValue));
		//CBPFilter bpFilter;
		//bpFilter.filter(&timeaxis, &ac);
		FLOAT min1 = *min_element(ac.begin(), ac.end());
		FLOAT max1 = *max_element(ac.begin(), ac.end());
		int y = 9;
	}

	float maxTime = timeaxis.at(timeaxis.size() - 1);

	//---Generate 30 sec epoch vector
	vector <FLOAT> v;
	ac30secTimeVector.clear();
	ac30secVector.clear();
	for (float t = .0f ; t <= maxTime ; t += 30.0f) {
		float accsum = .0f;
		for (float tf = t ; tf < t + 30.0f ; tf += 1.0f) {
			unsigned int dist = distance(timeaxis.begin(),lower_bound(timeaxis.begin(),timeaxis.end(),tf));
			if (dist < ac.size()) {
				accsum += ac.at(dist);
			}
		}
		ac30secTimeVector.push_back(t + 15.0f);
		v.push_back((accsum / 30.0f));
	}

	if (v.size() <= hanningWindowTaps) {
		vector <FLOAT>::iterator acIt = ac30secTimeVector.begin();
		for (; acIt < ac30secTimeVector.end(); ++acIt) {
			ac30secVector.push_back(*acIt);
			baselineVector.push_back(.0f);
		}
		return;
	}

	//---Convolve with Hanning window
	for (int i = 0 ; i < hanningWindowTaps / 2 ; i++) ac30secVector.push_back(v.at(i));
	vector <FLOAT>::iterator f1,f2;
	f1 = v.begin();
	f2 = f1 + hanningWindowTaps;
	FLOAT innerProd;
	do {
		innerProd = inner_product(f1,f2,hanningWindow.begin(),.0f, std::plus<FLOAT>(), std::multiplies<FLOAT>()) / hanningScaleFactor;
		ac30secVector.push_back(innerProd);
		f1++;
		f2++;
	} while (f2 < v.end());
	while (ac30secVector.size() < v.size()) {
		ac30secVector.push_back(*f1);
		f1++;
	}

	//---Find baseline vector
	vector <FLOAT> *vP = getWindowedAcVector();
	vector <FLOAT> *baseP = getBaselineVector();
	int size1 = ac30secTimeVector.size();
	int size2 = ac30secVector.size();
	ASSERT(ac30secTimeVector.size() == ac30secVector.size());
	int numBelow[20];
	numBelow[0] = count_if(vP->begin(), vP->end(), [](FLOAT v) { return v < 1.0f; });
	numBelow[1] = count_if(vP->begin(), vP->end(), [](FLOAT v) { return v < 2.0f; });
	numBelow[2] = count_if(vP->begin(), vP->end(), [](FLOAT v) { return v < 3.0f; });
	numBelow[3] = count_if(vP->begin(), vP->end(), [](FLOAT v) { return v < 4.0f; });
	numBelow[4] = count_if(vP->begin(), vP->end(), [](FLOAT v) { return v < 5.0f; });
	numBelow[5] = count_if(vP->begin(), vP->end(), [](FLOAT v) { return v < 6.0f; });
	numBelow[6] = count_if(vP->begin(), vP->end(), [](FLOAT v) { return v < 7.0f; });
	numBelow[7] = count_if(vP->begin(), vP->end(), [](FLOAT v) { return v < 8.0f; });
	numBelow[8] = count_if(vP->begin(), vP->end(), [](FLOAT v) { return v < 9.0f; });
	numBelow[9] = count_if(vP->begin(), vP->end(), [](FLOAT v) { return v < 10.0f; });
	numBelow[10] = count_if(vP->begin(), vP->end(), [](FLOAT v) { return v < 11.0f; });
	numBelow[11] = count_if(vP->begin(), vP->end(), [](FLOAT v) { return v < 12.0f; });
	numBelow[12] = count_if(vP->begin(), vP->end(), [](FLOAT v) { return v < 13.0f; });
	numBelow[13] = count_if(vP->begin(), vP->end(), [](FLOAT v) { return v < 14.0f; });
	numBelow[14] = count_if(vP->begin(), vP->end(), [](FLOAT v) { return v < 15.0f; });
	numBelow[15] = count_if(vP->begin(), vP->end(), [](FLOAT v) { return v < 16.0f; });
	numBelow[16] = count_if(vP->begin(), vP->end(), [](FLOAT v) { return v < 17.0f; });
	numBelow[17] = count_if(vP->begin(), vP->end(), [](FLOAT v) { return v < 18.0f; });
	numBelow[18] = count_if(vP->begin(), vP->end(), [](FLOAT v) { return v < 19.0f; });
	numBelow[19] = count_if(vP->begin(), vP->end(), [](FLOAT v) { return v < 20.0f; });
	int tenPerc = vP->size() / 10;
	int i = 0;
	for (i = 0; i < 20; i++) {
		if (numBelow[i] > tenPerc) break;
	}
	float baseline = i == 21 ? .0f : (float) i + 1.0f; // Zero means awake 100% of the time
	baseP->resize(vP->size(), baseline);

	////---Find baselineVector
	//baselineVector.clear();
	//vector <FLOAT>::iterator tb;
	//vector <FLOAT>::iterator it1,it2;
	//for (tb = ac30secTimeVector.begin() ; tb < ac30secTimeVector.end() ; tb++) {
	//	//float from = *tb - 150 ;	// - 2.5 min
	//	//float to = *tb + 150;		// + 2.5 min
	//	//it1 = lower_bound(timeaxis.begin(),timeaxis.end(),from);
	//	//it2 = lower_bound(timeaxis.begin(),timeaxis.end(),to);
	//	//it2--;  // Make sure not to point past the last element
	//	//int distFrom = it1 - timeaxis.begin();
	//	//int distTo = it2 - timeaxis.begin();
	//	//if (distFrom >= distTo) break;
	//	//float min = *min_element(ac.begin() + distFrom,ac.begin() + distTo);
	//	float min = actimeterConstantBaseline;
	//	baselineVector.push_back(min);
	//}
}

unsigned int CActimeterDataSet::evaluateData(FLOAT *_percBad)
{
	vector <FLOAT>::iterator it;
	int cnt = 0;
	for (it = ac.begin() ; it < ac.end() ; it++) {
		if ((*it < .0f) || (*it > ACTIMETER_MAX_LIMIT)) 
			cnt++;
	}
	*_percBad = (float) 100.0f * cnt / flags.size();

	if (cnt > 1) return FLAG_BAD_ACTIMETER;
	
	return 0;
}

int CActimeterDataSet::getSize(void)
{
	return timeaxis.size();
}

float CActimeterDataSet::getEpochLength(void)
{
	int size = getSize();
	int  num = size > 200 ? 200 : size ;
	if (num < 2) return .0f;

	float avg = .0f;
	for (int i = 1 ; i < num ; i++) {
		avg += timeaxis.at(i) - timeaxis.at(i - 1);
	}
	avg /= num;
	return avg;
}


void CActimeterDataSet::Serialize(CArchive& ar, int majorFileVersion, int minorFileVersion)
{
	if (ar.IsStoring()) {

		ASSERT(ac.size() == timeaxis.size());
		ASSERT(flags.size() == timeaxis.size());
		ASSERT(baselineVector.size() == ac30secTimeVector.size());
		ASSERT(ac30secVector.size() == ac30secTimeVector.size());

		unsigned int size = timeaxis.size();
		ar << size;
		for (unsigned int i = 0 ; i < size ; i++) {
			ar << timeaxis[i];
			ar << ac[i];
			ar << flags[i];					// Added in version 4.0
		}
		unsigned int size2 = ac30secTimeVector.size();
		ar << size2;
		for (unsigned int i = 0; i < size2; i++) {
			ar << baselineVector[i];		// Added in version 4.0
			ar << ac30secVector[i];			// Added in version 4.0
			ar << ac30secTimeVector[i];		// Added in version 4.0
		}
	}
	else {
		timeaxis.clear();
		ac.clear();
		flags.clear();

		unsigned int size;
		ar >> size;
		if (majorFileVersion >= 4) {
			FLOAT val;
			unsigned short us;
			for (unsigned int i = 0; i < size; i++) {
				ar >> val;
				timeaxis.push_back(val);
				ar >> val;
				ac.push_back(val);
				ar >> us;
				flags.push_back(us);
			}
			unsigned int size2;
			ar >> size2;
			for (unsigned int i = 0; i < size2; i++) {
				ar >> val;
				baselineVector.push_back(val);
				ar >> val;
				ac30secVector.push_back(val);
				ar >> val;
				ac30secTimeVector.push_back(val);
			}
		}
		else {
			FLOAT val;
			for (unsigned int i = 0; i < size; i++) {
				ar >> val;
				timeaxis.push_back(val);
				ar >> val;
				ac.push_back(val);
				flags.push_back(0);	// No flags anymore
			}
			dataSetIsCompleteFromAGS();
		}
		FLOAT percBad;
		evaluateData(&percBad);
	}
}

/////////////////////////////////////////

CCatheterDataSet::CCatheterDataSet() :
	validTime(.0f),invalidTime(.0f)
{
	ucA = ucB = ucC = ucD = ucE = NULL;
	uncalTime = NULL;

	minTemp		= defMinTemp	;		
	maxTemp		= defMaxTemp	;		
	minPress	= defMinPress	;		
	maxPress	= defMaxPress	;		

	CXTRegistryManager *reg = new CXTRegistryManager;

	//---Test registry
	double dtest;
	int OKt = reg->GetProfileDouble(regSectionCriteria,regBaselineLength,&dtest);
	if (!OKt) {
		delete reg;
		reg = new CXTRegistryManager(HKEY_LOCAL_MACHINE);
	}
	
	double d;
	int OK = reg->GetProfileDouble(regSectionCriteria,regBaselineLength,&d);
	baselineLength = OK ? (float) d : BASELINE_LENGTH;
	OK = reg->GetProfileDouble(regSectionCriteria,regT1EnvPercentStabilityLimit,&d);
	t1EnvPercentStabilityLimit = OK ? (float) d : T0T1ENV_PERCENT_STABILITY_LIMIT;
	
	OK = reg->GetProfileDouble(regSectionCriteria,regPressureEnvPercentStabilityLimit,&d);
	pressureEnvPercentStabilityLimit = OK ? (float) d : PRESSENV_PERCENT_STABILITY_LIMIT;
	delete reg;
}

CCatheterDataSet::~CCatheterDataSet()
{
}

void CCatheterDataSet::removePositiveOutliers(vector <FLOAT> *_v, vector <FLOAT> *_vTime, vector <FLOAT> *_outlTime)
{
	ASSERT(_v->size() == _vTime->size());
	if (0 == _outlTime->size()) return;
	if (2 > _v->size()) return;
	
	vector <FLOAT>::iterator vtIt;
	vector <FLOAT>::iterator oIt = _outlTime->begin();
	for (; oIt < _outlTime->end(); ++oIt) {
		vtIt = lower_bound(_vTime->begin(), _vTime->end(), *oIt);
		unsigned int offset = distance(_vTime->begin(), vtIt);
		//---Go to _v sample and replace peak with lin interpolation
		removePosPeak(_v, _vTime, offset);
	}
}

/*
Description: Removes a positive peak from _v and replaces the samples with a lin interpolation - uses _vTime for that.
*/
void CCatheterDataSet::removePosPeak(vector <FLOAT> *_v, vector <FLOAT> *_vTime, unsigned int _offs)
{
	ASSERT(_v->size() == _vTime->size());
	if (_offs >= _v->size()) return;

	vector <FLOAT>::iterator vIt = _v->begin();

	//---Make sure we are at a peak
	//---Search in pos dir
	vIt += _offs;
	while ((*vIt < *(vIt + 1)) && (vIt < _v->end())) vIt++;
	if (vIt == _v->end()) vIt--;

	//---Search in neg dir
	while ((vIt > _v->begin()) && (*vIt < *(vIt - 1))) vIt--;

	//---Find start and stop of peak
	//---Search in pos dir for derivative change
	vector <FLOAT>::iterator vItUp = vIt;
	while ((vItUp < (_v->end() - 1)) && (*vItUp > *(vItUp + 1)))
		vItUp++;

	//---Search in neg dir for derivative change
	vector <FLOAT>::iterator vItDwn = vIt;
	while ((vItDwn > _v->begin()) && (*vItDwn > *(vItDwn - 1)))
		vItDwn--;

	unsigned int start = distance(_v->begin(), vItDwn);
	unsigned int stop = distance(_v->begin(), vItUp);

	vector <FLOAT>::iterator tItStart = _vTime->begin();
	tItStart += start;
	vector <FLOAT>::iterator tItStop = _vTime->begin();
	tItStop += stop;

	//---Lin interpolation
	FLOAT timeSpan = *tItStop - *tItStart;
	FLOAT amplSpan = *vItUp - *vItDwn;
	vector <FLOAT>::iterator tIt;
	vIt = vItDwn;
	tIt = tItStart;
	while (vIt < vItUp) {
		vIt++;
		tIt++;
		*vIt = *vItDwn + (*tIt - *tItStart) * amplSpan / timeSpan;
	}
}

void CCatheterDataSet::resetSwallow(void)
{
	swallowQualification(&outlierTimePOES, &outlierTimePPH, &swallowTimePOES, &swallowTimePPH);
}

/*
Description:
		Check if there are peaks both in PPH and POES and where the time difference is 
		between SWALLOW_TIME_DIFF_LOW and SWALLOW_TIME_DIFF_HIGH
*/
void CCatheterDataSet::swallowQualification(vector <FLOAT> *_outlPOES, vector <FLOAT> *_outlPPH,
												vector <FLOAT> *_swPOES, vector <FLOAT> *_swPPH)
{
	if (0 == _outlPOES->size()) return;
	if (0 == _outlPPH->size()) return;

	_swPOES->clear();
	_swPPH->clear();

	vector <FLOAT>::iterator poesIt = _outlPOES->end();
	poesIt--;
	vector <FLOAT>::iterator matchIt;
	for (; poesIt > _outlPOES->begin(); --poesIt) {
		matchIt = lower_bound(_outlPPH->begin(), _outlPPH->end(), *poesIt);   // First element >= *poesIt
		if (matchIt > _outlPPH->begin()) {
			matchIt--;
			float diffVal = *poesIt - *matchIt;
			if ((diffVal < SWALLOW_TIME_DIFF_HIGH) && (diffVal > SWALLOW_TIME_DIFF_LOW)) {
				_swPPH->push_back(*matchIt);
				_swPOES->push_back(*poesIt);
			}
		}
	}
	sort(_swPPH->begin(), _swPPH->end());
	sort(_swPOES->begin(), _swPOES->end());
}

/*
Description: 

	Detects positive outliers in a pressure series _p and add the time value to the _swT vector.
	Uses a filtered version of the vector to detect outliers, but leaves the original unfiltered
*/
void CCatheterDataSet::positiveOutlierDetect(vector <FLOAT> *_timeaxis, vector <FLOAT> *_p, float _iqrFactor, vector <FLOAT> *_swT)
{
	ASSERT(_timeaxis->size() == _p->size());
	if (_p->size() <= MIN_NUMSAMPLES_FOR_OUTLIER_DETECTION) return;

	_swT->clear();
	vector <FLOAT> tv, pv;
	pv.resize(_p->size(),.0f);
	tv.resize(_timeaxis->size(),.0f);
	copy(_p->begin(), _p->end(), pv.begin());
	copy(_timeaxis->begin(), _timeaxis->end(), tv.begin());

	//---Filter to offset adjust
	CBPCheb1t012Filter bpF;
	bpF.filter(&tv,&pv);
	if (pv.size() <= MIN_NUMSAMPLES_FOR_OUTLIER_DETECTION) {  // Just return		
		return;
	}
	float median = .0f;
	float iqr = getIQR(pv.begin(),pv.end(), &median);
	float outlierAboveMedian = iqr * _iqrFactor;
	float maxVal = .0f;
	float outlierLim = .0f;
	vector <FLOAT>::iterator maxIt;
	float localMedian = .0f;
	do {
		maxIt = max_element(pv.begin() + 20, pv.end() - 20);
		localMedian = median_element(pv.begin() + 20, pv.end() - 20);

		maxVal = *maxIt;
		outlierLim = localMedian + outlierAboveMedian;
		if (maxVal > outlierLim) {
			int offs = distance(pv.begin(), maxIt);
			_swT->push_back(tv.at(offs));
			transform(maxIt - 20, maxIt + 20, maxIt - 20, SetValue <FLOAT>(median));
		}
	} while (maxVal > outlierLim);
	sort(_swT->begin(), _swT->end());
}

/*
Description: Takes the ratio between two time series vectors with DIFFERENT time axes
			First vector is the oesphagus pressure, second is the pharynx pressure. All envelopes.
			v output is the ratio in % of the oesophagus pressure,

	d(t2) = 100.0f * b(t2) / a(t2)				// %

	- out time vector is set equal to input time vector 2 
	- Values from input vector 1 are selected as a(t2) - the interpolation between b(t`) and b(t``) where t` < t2 and t`` > t2

_v1: Input Vector 1 (POES Env)
_t1: Time vector 1
_v2: Input Vector 2  (PPH Env)
_t2: Time vector 2
_resv: Output vector - (length as _v1)
_revtime: Common output time vector
*/
void CCatheterDataSet::makeGradientVector(vector <FLOAT> *_v1,vector <FLOAT> *_v2,vector <FLOAT> *_t1,vector <FLOAT> *_t2,
		vector <FLOAT> *_resv,vector <FLOAT> *_resvtime)
{
	ASSERT(_v1->size() == _t1->size());
	ASSERT(_v2->size() == _t2->size());
	_resv->clear();
	_resvtime->clear();
	
	vector <FLOAT>::iterator invector,intime,hight;
	invector = _v2->begin();  // PPH
	intime = _t2->begin();
	FLOAT a,b,t;
	for ( ; invector != _v2->end(); invector++, intime++) {
		
		t = *intime;
		if (!interpolate(_v1,_t1,t,&a)) break;  // a is POES
		b = *invector;  // PPH
		float ratio = a == .0f || b == .0f ? 50.0f : 100.0f * b / a ;
		_resv->push_back(ratio);
		_resvtime->push_back(t);
	}
	ASSERT(_resv->size() == _resvtime->size());
}

float CCatheterDataSet::getInvalidTime(void)
{
	return invalidTime;
}

float CCatheterDataSet::getValidTime(void)
{
	return validTime;
}

void CCatheterDataSet::clear(void) 
{
	POES.clear();
	PPH.clear();
	T0.clear();
	T1.clear();

	POESRaw.clear();
	PPHRaw.clear();
	T0Raw.clear();
	T1Raw.clear();
	rawTime.clear();

	pGradient.clear();
	respFrq.clear();
	admittance.clear();
	pGradientTime.clear();
	respFrqTime.clear();
	admittanceTime.clear();
	POESEnv.clear();
	PPHEnv.clear();
	T0Env.clear();
	T1Env.clear();
	T1EnvBaseline.clear();
	T0EnvBaseline.clear();
	bFlowBaseline.clear();
	POESEnvBaseline.clear();
	PPHEnvBaseline.clear();
	T0plusT1.clear();
	POESEnvTime	.clear();
	PPHEnvTime.clear();
	T0EnvTime.clear();
	T1EnvTime.clear();
	T0plusT1Time.clear();
	flags.clear();
	timeaxis.clear();
	outlierTimePPH.clear();
	outlierTimePOES.clear();
	swallowTimePPH.clear();
	swallowTimePOES.clear();
}

void CCatheterDataSet::addToXYRaw(FLOAT _pOES,FLOAT _pPH,FLOAT _t0,FLOAT _t1,unsigned short _flags,FLOAT _time)
{
	POES.push_back(_pOES);
	PPH.push_back(_pPH);
	T0.push_back(_t0);
	T1.push_back(_t1);
	flags.push_back(_flags);
	timeaxis.push_back(_time);
}

unsigned int CCatheterDataSet::evaluateData(float *_percBad)
{
	float totalTime = validTime + invalidTime;

	*_percBad = 100.0f * invalidTime / totalTime;

	return *_percBad > BAD_CATHETER_DATA_LIMIT ? FLAG_BAD_CATHETER : 0;
}

int CCatheterDataSet::getSize(void)
{
	return timeaxis.size();
}

vector <FLOAT> *CCatheterDataSet::getPOESVector(void)
{
	return &POES;
}

vector <FLOAT> *CCatheterDataSet::getPPHVector(void)
{
	return &PPH;
}

vector <FLOAT> *CCatheterDataSet::getT0Vector(void)
{
	return &T0;
}

vector <FLOAT> *CCatheterDataSet::getT1Vector(void)
{
	return &T1;
}
vector<FLOAT>* CCatheterDataSet::getRawTimeVector(void)
{
	return &rawTime;
}
vector <FLOAT> *CCatheterDataSet::getPOESRawVector(void)
{
	return &POES;
}

vector <FLOAT> *CCatheterDataSet::getPPHRawVector(void)
{
	return &PPH;
}

vector <FLOAT> *CCatheterDataSet::getT0RawVector(void)
{
	return &T0Raw;
}

vector <FLOAT> *CCatheterDataSet::getT1RawVector(void)
{
	return &T1Raw;
}

vector <FLOAT> *CCatheterDataSet::getRespFrqVector(void)
{
	return &respFrq;
}

vector <FLOAT> *CCatheterDataSet::getAdmittanceVector(void)
{
	if (bEfficiency.size() && bEfficiencyTime.size()) 
		return &bEfficiency;
	else 
		return &admittance;
}

vector <FLOAT> *CCatheterDataSet::getPOESEnvVector(void)
{
	return &POESEnv;
}

vector <FLOAT> *CCatheterDataSet::getPPHEnvVector(void)
{
	return &PPHEnv;
}

vector <FLOAT> *CCatheterDataSet::getT0EnvVector(void)
{
	return &T0Env;
}

vector <FLOAT> *CCatheterDataSet::getT0plusT1Vector(void)
{
	return &T0plusT1;
}

vector<FLOAT>* CCatheterDataSet::getBFlowVector(void)
{
	return &bFlow;
}

vector<FLOAT>* CCatheterDataSet::getBFlowBaseline(void)
{
	return &bFlowBaseline;
}

vector<FLOAT>* CCatheterDataSet::getBEfficiencyVector(void)
{
	return &bEfficiency;
}

vector<FLOAT>* CCatheterDataSet::getBFlowVectorTime(void)
{
	return &bFlowTime;
}

vector<FLOAT>* CCatheterDataSet::getBEfficiencyVectorTime(void)
{
	return &bEfficiencyTime;
}

vector <FLOAT> *CCatheterDataSet::getPOESBaselineVector(void)
{
	return &POESEnvBaseline;
}

vector <FLOAT> *CCatheterDataSet::getPPHBaselineVector(void)
{
	return &PPHEnvBaseline;
}

vector <FLOAT> *CCatheterDataSet::getT0EnvBaselineVector(void)
{
	return &T0EnvBaseline;
}

vector <FLOAT> *CCatheterDataSet::getT1EnvBaselineVector(void)
{
	return &T1EnvBaseline;
}

vector <FLOAT> *CCatheterDataSet::getT1EnvVector(void)
{
	return &T1Env;
}

vector <FLOAT> *CCatheterDataSet::getPgradientVector(void)
{
	return &pGradient;
}


vector <FLOAT> *CCatheterDataSet::getPgradientVectorTime(void)
{
	return &pGradientTime;
}

vector <FLOAT> *CCatheterDataSet::getRespFrqVectorTime(void)
{
	return &respFrqTime;
}

vector <FLOAT> *CCatheterDataSet::getAdmittanceVectorTime(void)
{
	if (bEfficiency.size() && bEfficiencyTime.size())
		return &bEfficiencyTime;
	else
		return &admittanceTime;
}

vector <FLOAT> *CCatheterDataSet::getPOESEnvVectorTime(void)
{
	return &POESEnvTime;
}

vector <FLOAT> *CCatheterDataSet::getPPHEnvVectorTime(void)
{
	return &PPHEnvTime;
}

vector <FLOAT> *CCatheterDataSet::getT0EnvVectorTime(void)
{
	return &T0EnvTime;
}

vector <FLOAT> *CCatheterDataSet::getT1EnvVectorTime(void)
{
	return &T1EnvTime;
}

/*
Description : Returns the vector that currently represents flow
*/
vector<FLOAT>* CCatheterDataSet::getFlowVector(void)
{
	if (bFlow.size() && bFlowTime.size())
		return &bFlow;
	else 
		return &T1Env;
}

/*
Description : Returns the vector that currently represents flow time
*/
vector<FLOAT>* CCatheterDataSet::getFlowVectorTime(void)
{
	if (bFlow.size() && bFlowTime.size())
		return &bFlowTime;
	else
		return &T1EnvTime;
}

/*
Description: Returns the baseline of the vector that is currently used as the flow vector
*/
vector<FLOAT>* CCatheterDataSet::getFlowBaselineVector(void)
{
	if (bFlowBaseline.size() && bFlowTime.size())
		return &bFlowBaseline;
	else
		return &T1EnvBaseline;
}

vector <FLOAT> *CCatheterDataSet::getT0plusT1VectorTime(void)
{
	return &T0plusT1Time;
}

/*
Description: Based on min, max of press and temp read from the TORSO parameters, 
converts uncalibrated vectors pressure and temperature.
_par: The parameter set read somewhere in the acq file
*/
bool CCatheterDataSet::calibrate(CATH_PARAMETER_SET *_par)
{
	if (NULL == ucA) return false;
	if (0 == ucA->size()) return false;

	ASSERT(ucB->size() == ucA->size());
	ASSERT(ucC->size() == ucA->size());
	ASSERT(ucD->size() == ucA->size());
	ASSERT(ucE->size() == ucA->size());
	ASSERT(uncalTime->size() == ucA->size());
	float minT,maxT,minP,maxP;
	if ((0 == _par->maxPress) || (0 == _par->minPress) || (0 == _par->maxTemp) || (0 == _par->minTemp)
		|| (_par->minPress >= _par->maxPress) || (_par->minTemp >= _par->maxTemp)) {
		CString s;
		int num = s.LoadString(IDS_ERR_CATH_PAR_SET);
		AfxMessageBox(s);
		
		minT = minTemp;
		maxT = maxTemp;
		minP = minPress;
		maxP = maxPress;
	}
	else {
		minT = (float) _par->minTemp;
		maxT = (float) _par->maxTemp;
		minP = (float) _par->minPress;
		maxP = (float) _par->maxPress;

		//---New for version 3.12 as cath parameters min max press may be +/- 2
		if ((fabs(minP) < 10) && (fabs(maxP) < 10)) {
			minP *= 100;
			maxP *= 100;
		}
	}

	int num = ucA->size();
	float pOES,pPH,t0,t1;
	unsigned short flags;
	float pDiff = maxP - minP;
	float tDiff = maxT - minT;
	for (int i = 0 ; i < num ; i++) {
		flags = (unsigned short) ucE->at(i);

		pPH		= minP + ((float) ucA->at(i) * pDiff) / maxCathDataScale;
		pOES	= minP + ((float) ucC->at(i) * pDiff) / maxCathDataScale;

		t1		= minT + ((float) ucB->at(i) * tDiff) / maxCathDataScale;
		t1		/= 10.0f;
		t0		= minT + ((float) ucD->at(i) * tDiff) / maxCathDataScale;
		t0		/= 10.0f;

		addToXYRaw(pOES,pPH,t0,t1,flags,uncalTime->at(i));
	}

	delete (ucA);
	delete (ucB);
	delete (ucC);
	delete (ucD);
	delete (ucE);
	delete (uncalTime);
	ucA = NULL;
	ucB = NULL;
	ucC = NULL;
	ucD = NULL;
	ucE = NULL;
	uncalTime = NULL;
	return true;
}

/*
Description:
Adds to uncalibrated data vectors, directly from ACQ file
*/
void CCatheterDataSet::addUncalibrated(INT16 _a, INT16 _b, INT16 _c, INT16 _d, INT16 _e, float _t)
{
	//static short oldA = 0;
	//static short oldB = 0;
	//static short oldC = 0;
	//static short oldD = 0;
	//static bool first = true;

	if (NULL == ucA) ucA = new vector <short>;
	if (NULL == ucB) ucB = new vector <short>;
	if (NULL == ucC) ucC = new vector <short>;
	if (NULL == ucD) ucD = new vector <short>;
	if (NULL == ucE) ucE = new vector <short>;
	if (NULL == uncalTime) uncalTime = new vector <FLOAT>;

	////---Avoid crazy data from catheter. This has to be revisited
	//if ((0 == _a) && (0 == _b) && (0 == _c) && (0 == _d)) 
	//	return;
	//
	//if (first) {
	//	oldA = _a;
	//	oldB = _b;
	//	oldC = _c;
	//	oldD = _d;
	//}
	//short jumpA = abs(_a - oldA);
	//short jumpB = abs(_b - oldB);
	//short jumpC = abs(_c - oldC);
	//short jumpD = abs(_d - oldD);
	//
	//oldA = _a;
	//oldB = _b;
	//oldC = _c;
	//oldD = _d;
	//
	//if (jumpA > 500)
	//	return;
	//if (jumpB > 500)
	//	return;
	//if (jumpC > 500)
	//	return;
	//if (jumpD > 500)
	//	return;

	ucA->push_back((short) _a);
	ucB->push_back((short) _b);
	ucC->push_back((short) _c);
	ucD->push_back((short) _d);
	ucE->push_back((short) _e);
	uncalTime->push_back(_t);

	//first = false;
}

void CCatheterDataSet::dataSetIsCompleteFromAPN(void)
{
	//---Set up a vector with time intervals where PPH & POES are outside -2*min and +2*max
	//---Copy data to POESRaw,PPHRaw,T0Raw,T1Raw
	POESRaw.resize(POES.size(), .0f);
	copy(POES.begin(), POES.end(), POESRaw.begin());

	PPHRaw.resize(PPH.size(), .0f);
	copy(PPH.begin(), PPH.end(), PPHRaw.begin());

	T0Raw.resize(T0.size(), .0f);
	copy(T0.begin(), T0.end(), T0Raw.begin());

	T1Raw.resize(T1.size(), .0f);
	copy(T1.begin(), T1.end(), T1Raw.begin());

	rawTime.resize(timeaxis.size(), .0f);
	copy(timeaxis.begin(), timeaxis.end(), rawTime.begin());

	//---Swallow detect. Generate outlierTimePPH,OutlierTimePOES
	positiveOutlierDetect(&timeaxis, &PPH, IQR_FACTOR_PPH_SWALLOW_DETECT, &outlierTimePPH);
	positiveOutlierDetect(&timeaxis, &POES, IQR_FACTOR_POES_SWALLOW_DETECT, &outlierTimePOES);

	//---Generate swallowTimePOES and swallowTimePPH
	swallowQualification(&outlierTimePOES, &outlierTimePPH, &swallowTimePOES, &swallowTimePPH);

	//---Generate Raw data without outliers
	POESOutlierfree.clear();
	PPHOutlierfree.clear();
	POESOutlierfree.resize(timeaxis.size(), .0f);
	PPHOutlierfree.resize(timeaxis.size(), .0f);
	copy(POESRaw.begin(), POESRaw.end(), POESOutlierfree.begin());
	copy(PPHRaw.begin(), PPHRaw.end(), PPHOutlierfree.begin());
	removePositiveOutliers(&POESOutlierfree, &timeaxis, &outlierTimePOES);
	removePositiveOutliers(&PPHOutlierfree, &timeaxis, &outlierTimePPH);

	//---Filter. Data have already been filtered, but doing this again in order to preserve relative phase
	if (!noCathFilter) {
		CBPCheb1t012Filter bpF;
		bpF.filter(&timeaxis, &POES, &PPH, &T0, &T1, &POESOutlierfree, &PPHOutlierfree);
		// Already done on APN data : makeDerivative(&T0, &timeaxis);
		// Already done on APN data : makeDerivative(&T1, &timeaxis);
	}

	removeMedian(&POES);
	removeMedian(&PPH);
	removeMedian(&T0);
	removeMedian(&T1);

	dataSetIsCompleteFromAGS(NULL);
}

void CCatheterDataSet::dataSetIsCompleteFromTorso(CATH_PARAMETER_SET *_par)
{
	//---Set up a vector with time intervals where PPH & POES are outside -2*min and +2*max
	//---Copy data to POESRaw,PPHRaw,T0Raw,T1Raw
	POESRaw.resize(POES.size(), .0f);
	copy(POES.begin(), POES.end(), POESRaw.begin());

	PPHRaw.resize(PPH.size(), .0f);
	copy(PPH.begin(), PPH.end(), PPHRaw.begin());

	T0Raw.resize(T0.size(), .0f);
	copy(T0.begin(), T0.end(), T0Raw.begin());

	T1Raw.resize(T1.size(), .0f);
	copy(T1.begin(), T1.end(), T1Raw.begin());

	rawTime.resize(timeaxis.size(), .0f);
	copy(timeaxis.begin(), timeaxis.end(), rawTime.begin());

	//---Swallow detect. Generate outlierTimePPH,OutlierTimePOES
	positiveOutlierDetect(&timeaxis, &PPH, IQR_FACTOR_PPH_SWALLOW_DETECT,&outlierTimePPH);
	positiveOutlierDetect(&timeaxis, &POES, IQR_FACTOR_POES_SWALLOW_DETECT, &outlierTimePOES);

	//---Generate swallowTimePOES and swallowTimePPH
	swallowQualification(&outlierTimePOES, &outlierTimePPH,&swallowTimePOES,&swallowTimePPH);

	//---Generate Raw data without outliers
	POESOutlierfree.clear();
	PPHOutlierfree.clear();
	POESOutlierfree.resize(timeaxis.size(), .0f);
	PPHOutlierfree.resize(timeaxis.size(), .0f);
	copy(POESRaw.begin(), POESRaw.end(), POESOutlierfree.begin());
	copy(PPHRaw.begin(), PPHRaw.end(), PPHOutlierfree.begin());
	removePositiveOutliers(&POESOutlierfree, &timeaxis, &outlierTimePOES);
	removePositiveOutliers(&PPHOutlierfree, &timeaxis, &outlierTimePPH);

	//---Filter
	if (!noCathFilter) {
		CBPCheb1t012Filter bpF;
		bpF.filter(&timeaxis, &POES, &PPH, &T0, &T1, &POESOutlierfree, &PPHOutlierfree);
		makeDerivative(&T0, &timeaxis);
		makeDerivative(&T1, &timeaxis);
	}

	removeMedian(&POES);
	removeMedian(&PPH);
	removeMedian(&T0);
	removeMedian(&T1);

	dataSetIsCompleteFromAGS(_par);
}

void CCatheterDataSet::dataSetIsCompleteFromAGS(CATH_PARAMETER_SET *_par)
{	
	if (!getSize()) return;

	if (!POES.size()) return;
	if (!PPH.size()) return;
	if (!T0.size()) return;
	if (!T1.size()) return;

	//---Do not run if all vectors have been read
	if (POESEnv.size() &&
		POESEnvTime.size() &&
		PPHEnv.size() &&
		PPHEnvTime.size() &&
		T0Env.size() &&
		T0EnvTime.size() &&
		T1Env.size() &&
		T1EnvTime.size() &&
		respFrq.size() &&
		respFrqTime.size() &&
		POESEnvBaseline.size() &&
		PPHEnvBaseline.size() &&
		T0EnvBaseline.size() &&
		T1EnvBaseline.size() &&
		pGradient.size() &&
		pGradientTime.size() &&
		T0plusT1.size() &&
		T0plusT1Time.size() &&
		admittance.size() &&
		admittanceTime.size()) return;

	if (!(POESEnv.size() && PPHEnv.size() && POESEnvTime.size() && PPHEnvTime.size())) {
		findPeakToPeak3(POESOutlierfree.size() ? &POESOutlierfree : &POES, &timeaxis, &POESEnv, &POESEnvTime);
		findPeakToPeak3(PPHOutlierfree.size() ? &PPHOutlierfree : &PPH, &timeaxis, &PPHEnv, &PPHEnvTime);
		findPeakToPeak3(&T0, &timeaxis, &T0Env, &T0EnvTime);
		findPeakToPeak3(&T1, &timeaxis, &T1Env, &T1EnvTime);

		//---LP filter Envelope and interpolate
		CLP_02Hz_Filter bpF;
		bpF.filter(&POESEnvTime, &POESEnv);
		bpF.filter(&PPHEnvTime, &PPHEnv);
		bpF.filter(&T1EnvTime, &T1Env);
		bpF.filter(&T0EnvTime, &T0Env);
	}

	if (!(respFrq.size() && respFrqTime.size())) {
		makeFrqVector(&POES, &timeaxis, &respFrq, &respFrqTime);
	}

	//---Find valid and invalid times
	invalidCTimeWindows.clear();
	if (flags.size() == timeaxis.size()) {
		validTime = .0f;
		invalidTime = .0f;
		vector <unsigned short>::iterator fi;
		vector <FLOAT>::iterator t0,t1;
		t0 = timeaxis.begin();
		t1 = t0;
		t1++;
		fi = flags.begin();
		bool wasValid = NO_FLAGS == *fi ? true : false;

		vector <FLOAT>::iterator rawTimeIt;
		bool checkPressures = ((_par != NULL) && (rawTime.size() > 0) && (PPHRaw.size() == rawTime.size()) &&
								(POESRaw.size() == rawTime.size()));
		
		bool validPressData = true;
		float maxLim = _par != NULL ? _par->maxPress * 2 : HUGE_VALF;
		float minLim = _par != NULL ? _par->minPress * 2 : -HUGE_VALF;
	
		float invalidPeriodStart = *t0;
		float invalidPeriodStop = *t1;
		for (fi = flags.begin() + 1 ; fi < flags.end() ; fi++,t1++,t0++) {
			
			validPressData = true;
			if (checkPressures) {
				rawTimeIt = lower_bound(rawTime.begin(), rawTime.end(), *t1);
				if (rawTimeIt < rawTime.end()) {
					int offs = distance(rawTime.begin(), rawTimeIt);
					if ((*(POESRaw.begin() + offs) >= maxLim) || (*(POESRaw.begin() + offs) <= minLim) ||
						(*(PPHRaw.begin() + offs) >= maxLim) || (*(PPHRaw.begin() + offs) <= minLim)) {
						validPressData = false;
					}
				}
			}

			if ((NO_FLAGS != *fi) || (!validPressData)) {
				if (!wasValid) {
					invalidTime += *t1 - *t0;
				}
				else {
					invalidPeriodStart = *t0;
				}
				wasValid = false;
			}
			else {
				if (wasValid) {
					validTime += *t1 - *t0;
				}
				else {
					invalidPeriodStop = *t1;
					TIME_WINDOW itw;
					itw.begin = invalidPeriodStart;
					itw.end = invalidPeriodStop;
					invalidCTimeWindows.push_back(itw);
				}
				wasValid = true;
			}
		}
		if (!wasValid) {
			TIME_WINDOW itw;
			itw.begin = invalidPeriodStart;
			itw.end = timeaxis.size() > 1 ? *(timeaxis.end() - 1) : invalidPeriodStart + 1;
			invalidCTimeWindows.push_back(itw);
		}
	}
	else if (timeaxis.size() >= 1) {
		validTime = timeaxis.at(timeaxis.size() - 1) - timeaxis.at(0);
		invalidTime = .0f;
	}

	//---Find baseline.
	if (!(T0EnvBaseline.size() && T1EnvBaseline.size() && POESEnvBaseline.size() && PPHEnvBaseline.size())) {
		generateBaselineVectorAASM(&T1Env, &T1EnvTime, &T1EnvBaseline, t1EnvPercentStabilityLimit, baselineLength, false);
		generateBaselineVectorAASM(&T0Env, &T0EnvTime, &T0EnvBaseline, t0EnvPercentStabilityLimit, baselineLength, false);
		generateBaselineVectorAASM(&POESEnv, &POESEnvTime, &POESEnvBaseline, pressureEnvPercentStabilityLimit, baselineLength, true);
		generateBaselineVectorAASM(&PPHEnv, &PPHEnvTime, &PPHEnvBaseline, pressureEnvPercentStabilityLimit, baselineLength, true);

		//---Find flat areas missing peak-to-peaks
		setFlatAreasToZero(&POES, &timeaxis, &POESEnv, &POESEnvTime, &POESEnvBaseline);
		setFlatAreasToZero(&PPH, &timeaxis, &PPHEnv, &PPHEnvTime, &PPHEnvBaseline);
		setFlatAreasToZero(&T0, &timeaxis, &T0Env, &T0EnvTime, &T0EnvBaseline);
		setFlatAreasToZero(&T1, &timeaxis, &T1Env, &T1EnvTime, &T1EnvBaseline);
	}
	
	//---Use Env vectors
	if (!(pGradient.size() && pGradientTime.size())) 
		makeGradientVector(&POESEnv,&PPHEnv,&POESEnvTime,&PPHEnvTime,&pGradient,&pGradientTime);
	if (!(T0plusT1.size() && T0plusT1Time.size()))
		makeSumVector(&T0Env,&T1Env,&T0EnvTime,&T1EnvTime,&T0plusT1,&T0plusT1Time);
	if (!(admittance.size() && admittanceTime.size()))
		makeAdmittanceVector(&POESEnv,&T0plusT1,&POESEnvTime,&T0plusT1Time,&admittance,&admittanceTime);
	computeFlowAndBreathingEfficiency();

	keyDataPOES.setData(&POES,&flags);
	keyDataPPH.setData(&PPH,&flags);
	keyDataAdmittance.setData(&admittance);
	keyDataPGradient.setData(&pGradient);
	keyDataPOESEnv.setData(&POESEnv);
	keyDataPPHEnv.setData(&PPHEnv);
	keyDataRespFrq.setData(&respFrq);
	keyDataT0plusT1.setData(&T0plusT1);
	keyDataBEfficiency.setData(&bEfficiency);
	keyDataBFlow.setData(&bFlow);
}


CKeyData *CCatheterDataSet::getKeyDataPOES(void)
{
	return &keyDataPOES;
}

CKeyData *CCatheterDataSet::getKeyDataPPH(void)
{
	return &keyDataPPH;
}

CKeyData *CCatheterDataSet::getKeyDataT0plusT1(void)
{
	return &keyDataT0plusT1;
}

CKeyData *CCatheterDataSet::getKeyDataPGradient(void)
{
	return &keyDataPGradient;
}

CKeyData *CCatheterDataSet::getKeyDataRespFrq(void)
{
	return &keyDataRespFrq;
}

CKeyData *CCatheterDataSet::getKeyDataAdmittance(void)
{
	return &keyDataAdmittance;
}

CKeyData *CCatheterDataSet::getKeyDataPOESEnv(void)
{
	return &keyDataPOESEnv;
}

CKeyData *CCatheterDataSet::getKeyDataPPHEnv(void)
{
	return &keyDataPPHEnv;
}

vector <TIME_WINDOW> *CCatheterDataSet::getInvalidTimeWindowsVector(void)
{
	return &invalidCTimeWindows;
}

void CCatheterDataSet::fillSwallowArray(CArray <CSwallowEvnt *, CSwallowEvnt *> *_swallowEventArray)
{
	ASSERT(swallowTimePPH.size() == swallowTimePOES.size());
	if (swallowTimePPH.size() != swallowTimePOES.size()) return;

	int num = swallowTimePPH.size();
	vector <FLOAT>::iterator tpph = swallowTimePPH.begin();
	vector <FLOAT>::iterator tpoes = swallowTimePOES.begin();
	for (; tpph < swallowTimePPH.end(); ++tpph, ++tpoes) {
		CSwallowEvnt *eP = new CSwallowEvnt();
		eP->setTo(*tpoes);
		eP->setFrom(*tpph);
		_swallowEventArray->Add(eP);
	}
}

/*
Description: Generates a vector for flow estimation. 

	Balance factor is k:  k * T1 + (1.0 - k) * T0
*/
void CCatheterDataSet::computeFlowAndBreathingEfficiency(float _balanceFactor /* = FLOW_DETECT_BALANCE_FACTOR*/)
{
	ASSERT(_balanceFactor >= .0f);
	ASSERT(_balanceFactor <= 1.0f);

	makeWeightedSumVector((1.0f - _balanceFactor),_balanceFactor,&T0Env, &T1Env, &T0EnvTime, &T1EnvTime, &bFlow, &bFlowTime);
	makeAdmittanceVector(&POESEnv, &bFlow, &POESEnvTime, &bFlowTime, &bEfficiency, &bEfficiencyTime);
	generateBaselineVectorAASM(&bFlow, &bFlowTime, &bFlowBaseline,
		(1.0f - _balanceFactor) * t0EnvPercentStabilityLimit + _balanceFactor * t1EnvPercentStabilityLimit,
		baselineLength, false);
}

void CCatheterDataSet::Serialize(CArchive& ar, int majorFileVersion, int minorFileVersion)
{
	if (ar.IsStoring()) {

		ASSERT(timeaxis.size() == POES.size());
		ASSERT(timeaxis.size() == PPH.size());
		ASSERT(timeaxis.size() == T0.size());
		ASSERT(timeaxis.size() == T1.size());

		ASSERT(rawTime.size() == POESRaw.size());
		ASSERT(rawTime.size() == PPHRaw.size());
		ASSERT(rawTime.size() == T0Raw.size());
		ASSERT(rawTime.size() == T1Raw.size());

		ASSERT(POESEnv.size() == POESEnvTime.size());
		ASSERT(PPHEnv.size() == PPHEnvTime.size());
		ASSERT(T0Env.size() == T0EnvTime.size());
		ASSERT(T1Env.size() == T1EnvTime.size());
		ASSERT(pGradient.size() == pGradientTime.size());
		ASSERT(respFrq.size() == respFrqTime.size());
		ASSERT(admittance.size() == admittanceTime.size());
		ASSERT(T0plusT1.size() == T0plusT1Time.size());
		ASSERT(T1EnvBaseline.size() == T1EnvTime.size());
		ASSERT(POESEnvBaseline.size() == POESEnvTime.size());
		ASSERT(PPHEnvBaseline.size() == PPHEnvTime.size());
		ASSERT(bEfficiency.size() == bEfficiencyTime.size());
		ASSERT(bFlow.size() == bFlowTime.size());
		ASSERT(bFlowBaseline.size() == bFlow.size());
		
		unsigned int size = timeaxis.size();
		ar << size;
		for (unsigned int i = 0 ; i < size ; i++) {
			ar << timeaxis[i];
			ar << POES[i];
			ar << PPH[i];
			ar << T0[i];
			ar << T1[i];
		}

		/////////////////////////////////////
		//---New in file version 8.0
		/////////////////////////////////////
		unsigned int sizeraw = rawTime.size();
		ar << sizeraw;
		for (unsigned int i = 0; i < sizeraw; i++) {
			ar << rawTime[i];
			ar << POESRaw[i];
			ar << PPHRaw[i];
			ar << T0Raw[i];
			ar << T1Raw[i];
		}
		unsigned int sizeOutlPPH = outlierTimePPH.size();
		ar << sizeOutlPPH;
		for (unsigned int i = 0; i < sizeOutlPPH; i++) ar << outlierTimePPH[i];

		unsigned int sizeOutlPOES = outlierTimePOES.size();
		ar << sizeOutlPOES;
		for (unsigned int i = 0; i < sizeOutlPOES; i++) ar << outlierTimePOES[i];

		unsigned int sizeSwallowPPH = swallowTimePPH.size();
		ar << sizeSwallowPPH;
		for (unsigned int i = 0; i < sizeSwallowPPH; i++) ar << swallowTimePPH[i];

		unsigned int sizeSwallowPOES = swallowTimePOES.size();
		ar << sizeSwallowPOES;
		for (unsigned int i = 0; i < sizeSwallowPOES; i++) ar << swallowTimePOES[i];
		/////////////////////////////////////

		/////////////////////////////////////
		// New in file version 9.0
		////////////////////////////////////
		unsigned int sizeICT = invalidCTimeWindows.size();
		ar << sizeICT;
		for (unsigned int i = 0; i < sizeICT; i++) {
			ar << invalidCTimeWindows[i].begin;
			ar << invalidCTimeWindows[i].end;
		}
		////////////////////////////////////

		unsigned int size2 = POESEnvTime.size();
		ar << size2;
		for (unsigned int i = 0; i < size2; i++) {
			ar << POESEnvTime[i];
			ar << POESEnv[i];
			ar << POESEnvBaseline[i];
		}
		unsigned int size3 = PPHEnvTime.size();
		ar << size3;
		for (unsigned int i = 0; i < size3; i++) {
			ar << PPHEnvTime[i];
			ar << PPHEnv[i];
			ar << PPHEnvBaseline[i];
		}
		unsigned int size4 = T0EnvTime.size();
		ar << size4;
		for (unsigned int i = 0; i < size4; i++) {
			ar << T0EnvTime[i];
			ar << T0Env[i];
			ar << T0EnvBaseline[i];
		}
		unsigned int size5 = T1EnvTime.size();
		ar << size5;
		for (unsigned int i = 0; i < size5; i++) {
			ar << T1EnvTime[i];
			ar << T1Env[i];
			ar << T1EnvBaseline[i];
		}
		unsigned int size6 = pGradientTime.size();
		ar << size6;
		for (unsigned int i = 0; i < size6; i++) {
			ar << pGradientTime[i];
			ar << pGradient[i];
		}
		unsigned int size7 = respFrqTime.size();
		ar << size7;
		for (unsigned int i = 0; i < size7; i++) {
			ar << respFrqTime[i];
			ar << respFrq[i];
		}
		unsigned int size8 = admittanceTime.size();
		ar << size8;
		for (unsigned int i = 0; i < size8; i++) {
			ar << admittanceTime[i];
			ar << admittance[i];
		}
		unsigned int size9 = T0plusT1Time.size();
		ar << size9;
		for (unsigned int i = 0; i < size9; i++) {
			ar << T0plusT1Time[i];
			ar << T0plusT1[i];
		}

		//////////////////////////////
		// New in file version 12.0
		//////////////////////////////
		unsigned int size10 = bFlowTime.size();
		ar << size10;
		for (unsigned int i = 0; i < size10; i++) {
			ar << bFlowTime[i];
			ar << bFlow[i];
			ar << bFlowBaseline[i];
		}
		unsigned int size11 = bEfficiencyTime.size();
		ar << size11;
		for (unsigned int i = 0; i < size11; i++) {
			ar << bEfficiencyTime[i];
			ar << bEfficiency[i];
		}

		ar << baselineLength					;
		ar << t0EnvPercentStabilityLimit		;
		ar << t1EnvPercentStabilityLimit		;	
		ar << pressureEnvPercentStabilityLimit	;	
		ar << minTemp							;	
		ar << maxTemp							;	
		ar << minPress							;	
		ar << maxPress							;	

		keyDataPOES.Serialize(ar, majorFileVersion, minorFileVersion);
		keyDataPPH.Serialize(ar, majorFileVersion, minorFileVersion);
		keyDataT0plusT1.Serialize(ar, majorFileVersion, minorFileVersion);
		keyDataPGradient.Serialize(ar, majorFileVersion, minorFileVersion);
		keyDataRespFrq.Serialize(ar, majorFileVersion, minorFileVersion);
		keyDataAdmittance.Serialize(ar, majorFileVersion, minorFileVersion);
		keyDataPOESEnv.Serialize(ar, majorFileVersion, minorFileVersion);
		keyDataPPHEnv.Serialize(ar, majorFileVersion, minorFileVersion);

		//---New in version 12
		keyDataBFlow.Serialize(ar, majorFileVersion, minorFileVersion);
		keyDataBEfficiency.Serialize(ar, majorFileVersion, minorFileVersion);
	}
	else {  // Is reading
		if (majorFileVersion >= 7) {
			timeaxis.clear();
			POES.clear();
			PPH.clear();
			T0.clear();
			T1.clear();
			flags.clear();

			rawTime.clear();
			POESRaw.clear();
			PPHRaw.clear();
			T0Raw.clear();
			T1Raw.clear();

			POESEnv.clear();
			POESEnvTime.clear();
			PPHEnv.clear();
			PPHEnvTime.clear();
			T0Env.clear();
			T0EnvTime.clear();
			T1Env.clear();
			T1EnvTime.clear();
			pGradient.clear();
			pGradientTime.clear();
			respFrq.clear();
			respFrqTime.clear();
			admittance.clear();
			admittanceTime.clear();
			T0plusT1.clear();
			T0plusT1Time.clear();
			T1EnvBaseline.clear();
			T1EnvTime.clear();
			POESEnvBaseline.clear();
			POESEnvTime.clear();
			PPHEnvBaseline.clear();
			PPHEnvTime.clear();
			outlierTimePPH.clear();
			outlierTimePOES.clear();
			invalidCTimeWindows.clear();

			unsigned int size;
			FLOAT val;
			ar >> size;
			for (unsigned int i = 0; i < size; i++) {
				ar >> val;
				timeaxis.push_back(val);
				ar >> val;
				POES.push_back(val);
				ar >> val;
				PPH.push_back(val);
				ar >> val;
				T0.push_back(val);
				ar >> val;
				T1.push_back(val);
				flags.push_back(0);		// No flags here anymore
			}

			////////////////////////////////////////////
			//---New in file version 8.0
			////////////////////////////////////////////
			if (majorFileVersion >= 8) {
				ar >> size;
				for (unsigned int i = 0; i < size; i++) {
					ar >> val;
					rawTime.push_back(val);
					ar >> val;
					POESRaw.push_back(val);
					ar >> val;
					PPHRaw.push_back(val);
					ar >> val;
					T0Raw.push_back(val);
					ar >> val;
					T1Raw.push_back(val);
				}
				//}
				ar >> size;
				for (unsigned int i = 0; i < size; i++) {
					ar >> val;
					outlierTimePPH.push_back(val);
				}

				ar >> size;
				for (unsigned int i = 0; i < size; i++) {
					ar >> val;
					outlierTimePOES.push_back(val);
				}

				ar >> size;
				for (unsigned int i = 0; i < size; i++) {
					ar >> val;
					swallowTimePPH.push_back(val);
				}

				ar >> size;
				for (unsigned int i = 0; i < size; i++) {
					ar >> val;
					swallowTimePOES.push_back(val);
				}
			}
			////////////////////////////////////////////

			/////////////////////////////////////
			// New in file version 9.0
			////////////////////////////////////
			if (majorFileVersion >= 9) {
				ar >> size;
				for (unsigned int i = 0; i < size; i++) {
					TIME_WINDOW tw;
					ar >> tw.begin;
					ar >> tw.end;
					invalidCTimeWindows.push_back(tw);
				}
			}
			////////////////////////////////////

			ar >> size;
			for (unsigned int i = 0; i < size; i++) {
				ar >> val;
				POESEnvTime.push_back(val);
				ar >> val;
				POESEnv.push_back(val);
				ar >> val;
				POESEnvBaseline.push_back(val);
			}

			ar >> size;
			for (unsigned int i = 0; i < size; i++) {
				ar >> val;
				PPHEnvTime.push_back(val);
				ar >> val;
				PPHEnv.push_back(val);
				ar >> val;
				PPHEnvBaseline.push_back(val);
			}

			ar >> size;
			for (unsigned int i = 0; i < size; i++) {
				ar >> val;
				T0EnvTime.push_back(val);
				ar >> val;
				T0Env.push_back(val);
				ar >> val;
				T0EnvBaseline.push_back(val);
			}

			ar >> size;
			for (unsigned int i = 0; i < size; i++) {
				ar >> val;
				T1EnvTime.push_back(val);
				ar >> val;
				T1Env.push_back(val);
				ar >> val;
				T1EnvBaseline.push_back(val);
			}

			ar >> size;
			for (unsigned int i = 0; i < size; i++) {
				ar >> val;
				pGradientTime.push_back(val);
				ar >> val;
				pGradient.push_back(val);
			}

			ar >> size;
			for (unsigned int i = 0; i < size; i++) {
				ar >> val;
				respFrqTime.push_back(val);
				ar >> val;
				respFrq.push_back(val);
			}

			ar >> size;
			for (unsigned int i = 0; i < size; i++) {
				ar >> val;
				admittanceTime.push_back(val);
				ar >> val;
				admittance.push_back(val);
			}

			ar >> size;
			for (unsigned int i = 0; i < size; i++) {
				ar >> val;
				T0plusT1Time.push_back(val);
				ar >> val;
				T0plusT1.push_back(val);
			}

			//////////////////////////////
			// New in file version 12.0
			//////////////////////////////
			if (majorFileVersion >= 12) {
				ar >> size;
				for (unsigned int i = 0; i < size; i++) {
					ar >> val;
					bFlowTime.push_back(val);
					ar >> val;
					bFlow.push_back(val);
					ar >> val;
					bFlowBaseline.push_back(val);
				}
				ar >> size;
				for (unsigned int i = 0; i < size; i++) {
					ar >> val;
					bEfficiencyTime.push_back(val);
					ar >> val;
					bEfficiency.push_back(val);
				}
			}
			else {  // Need to recompute envelopes, baselines and the zero sections
				findPeakToPeak3(POESOutlierfree.size() ? &POESOutlierfree : &POES, &timeaxis, &POESEnv, &POESEnvTime);
				findPeakToPeak3(PPHOutlierfree.size() ? &PPHOutlierfree : &PPH, &timeaxis, &PPHEnv, &PPHEnvTime);
				findPeakToPeak3(&T0, &timeaxis, &T0Env, &T0EnvTime);
				findPeakToPeak3(&T1, &timeaxis, &T1Env, &T1EnvTime);

				//---LP filter Envelope and interpolate
				CLP_02Hz_Filter bpF;
				bpF.filter(&POESEnvTime, &POESEnv);
				bpF.filter(&PPHEnvTime, &PPHEnv);
				bpF.filter(&T1EnvTime, &T1Env);
				bpF.filter(&T0EnvTime, &T0Env);

				generateBaselineVectorAASM(&T1Env, &T1EnvTime, &T1EnvBaseline, t1EnvPercentStabilityLimit, baselineLength, false);
				generateBaselineVectorAASM(&T0Env, &T0EnvTime, &T0EnvBaseline, t0EnvPercentStabilityLimit, baselineLength, false);
				generateBaselineVectorAASM(&POESEnv, &POESEnvTime, &POESEnvBaseline, pressureEnvPercentStabilityLimit, baselineLength, true);
				generateBaselineVectorAASM(&PPHEnv, &PPHEnvTime, &PPHEnvBaseline, pressureEnvPercentStabilityLimit, baselineLength, true);

				//---Find flat areas missing peak-to-peaks
				setFlatAreasToZero(&POES, &timeaxis, &POESEnv, &POESEnvTime, &POESEnvBaseline);
				setFlatAreasToZero(&PPH, &timeaxis, &PPHEnv, &PPHEnvTime, &PPHEnvBaseline);
				setFlatAreasToZero(&T0, &timeaxis, &T0Env, &T0EnvTime, &T0EnvBaseline);
				setFlatAreasToZero(&T1, &timeaxis, &T1Env, &T1EnvTime, &T1EnvBaseline);
			
				makeGradientVector(&POESEnv, &PPHEnv, &POESEnvTime, &PPHEnvTime, &pGradient, &pGradientTime);
				makeSumVector(&T0Env, &T1Env, &T0EnvTime, &T1EnvTime, &T0plusT1, &T0plusT1Time);
				makeAdmittanceVector(&POESEnv, &T0plusT1, &POESEnvTime, &T0plusT1Time, &admittance, &admittanceTime);
				computeFlowAndBreathingEfficiency(); // Use default
			}

			ar >> baselineLength;
			ar >> t0EnvPercentStabilityLimit;
			ar >> t1EnvPercentStabilityLimit;

			//--Rectify former error
			t0EnvPercentStabilityLimit = .0f == t0EnvPercentStabilityLimit ? t1EnvPercentStabilityLimit : t0EnvPercentStabilityLimit;

			ar >> pressureEnvPercentStabilityLimit;
			ar >> minTemp;
			ar >> maxTemp;
			ar >> minPress;
			ar >> maxPress;

			keyDataPOES.Serialize(ar,majorFileVersion,minorFileVersion);
			keyDataPPH.Serialize(ar, majorFileVersion, minorFileVersion); 
			keyDataT0plusT1.Serialize(ar, majorFileVersion, minorFileVersion);
			keyDataPGradient.Serialize(ar, majorFileVersion, minorFileVersion);
			keyDataRespFrq.Serialize(ar, majorFileVersion, minorFileVersion); 
			keyDataAdmittance.Serialize(ar, majorFileVersion, minorFileVersion);
			keyDataPOESEnv.Serialize(ar, majorFileVersion, minorFileVersion); 
			keyDataPPHEnv.Serialize(ar, majorFileVersion, minorFileVersion);

			//---New in file version 12
			if (majorFileVersion >= 12) {
				keyDataBFlow.Serialize(ar, majorFileVersion, minorFileVersion);
				keyDataBEfficiency.Serialize(ar, majorFileVersion, minorFileVersion);
			}
			
			dataSetIsCompleteFromAGS(NULL);

			FLOAT percBad;
			evaluateData(&percBad);
		}
		else if ((majorFileVersion >= 4) && (majorFileVersion <= 6)) {
			timeaxis.clear();
			POES.clear();
			PPH.clear();
			T0.clear();
			T1.clear();
			flags.clear();

			POESEnv.clear();
			POESEnvTime.clear();
			PPHEnv.clear();
			PPHEnvTime.clear();
			T0Env.clear();
			T0EnvTime.clear();
			T1Env.clear();
			T1EnvTime.clear();
			pGradient.clear();
			pGradientTime.clear();
			respFrq.clear();
			respFrqTime.clear();
			admittance.clear();
			admittanceTime.clear();
			T0plusT1.clear();
			T0plusT1Time.clear();
			T1EnvBaseline.clear();
			T1EnvTime.clear();
			POESEnvBaseline.clear();
			POESEnvTime.clear();
			PPHEnvBaseline.clear();
			PPHEnvTime.clear();
			outlierTimePPH.clear();
			outlierTimePOES.clear();

			unsigned int size;
			FLOAT val;
			ar >> size;
			for (unsigned int i = 0; i < size; i++) {
				ar >> val;
				timeaxis.push_back(val);
				ar >> val;
				POES.push_back(val);
				ar >> val;
				PPH.push_back(val);
				ar >> val;
				T0.push_back(val);
				ar >> val;
				T1.push_back(val);
				flags.push_back(0);		// No flags here anymore
			}

			ar >> size;
			for (unsigned int i = 0; i < size; i++) {
				ar >> val;
				//POESEnvTime.push_back(val);
				ar >> val;
				//POESEnv.push_back(val);
				ar >> val;
				//POESEnvBaseline.push_back(val);
			}

			ar >> size;
			for (unsigned int i = 0; i < size; i++) {
				ar >> val;
				//PPHEnvTime.push_back(val);
				ar >> val;
				//PPHEnv.push_back(val);
				ar >> val;
				//PPHEnvBaseline.push_back(val);
			}

			ar >> size;
			for (unsigned int i = 0; i < size; i++) {
				ar >> val;
				//T0EnvTime.push_back(val);
				ar >> val;
				//T0Env.push_back(val);
			}

			ar >> size;
			for (unsigned int i = 0; i < size; i++) {
				ar >> val;
				//T1EnvTime.push_back(val);
				ar >> val;
				//T1Env.push_back(val);
				ar >> val;
				//T1EnvBaseline.push_back(val);
			}

			ar >> size;
			for (unsigned int i = 0; i < size; i++) {
				ar >> val;
				//pGradientTime.push_back(val);
				ar >> val;
				//pGradient.push_back(val);
			}

			ar >> size;
			for (unsigned int i = 0; i < size; i++) {
				ar >> val;
				//respFrqTime.push_back(val);
				ar >> val;
				//respFrq.push_back(val);
			}

			ar >> size;
			for (unsigned int i = 0; i < size; i++) {
				ar >> val;
				//admittanceTime.push_back(val);
				ar >> val;
				//admittance.push_back(val);
			}

			ar >> size;
			for (unsigned int i = 0; i < size; i++) {
				ar >> val;
				//T0plusT1Time.push_back(val);
				ar >> val;
				//T0plusT1.push_back(val);
			}

			float dummyFloat;
			ar >> dummyFloat; // baselineLength;
			ar >> dummyFloat; // t1EnvPercentStabilityLimit;
			ar >> dummyFloat; // pressureEnvPercentStabilityLimit;
			ar >> dummyFloat; // minTemp;
			ar >> dummyFloat; // maxTemp;
			ar >> dummyFloat; // minPress;
			ar >> dummyFloat; // maxPress;

			CKeyData dummyKeyData;
			dummyKeyData.Serialize(ar, majorFileVersion, minorFileVersion);
			dummyKeyData.Serialize(ar, majorFileVersion, minorFileVersion);
			dummyKeyData.Serialize(ar, majorFileVersion, minorFileVersion);
			dummyKeyData.Serialize(ar, majorFileVersion, minorFileVersion);
			dummyKeyData.Serialize(ar, majorFileVersion, minorFileVersion);
			dummyKeyData.Serialize(ar, majorFileVersion, minorFileVersion);
			dummyKeyData.Serialize(ar, majorFileVersion, minorFileVersion);
			dummyKeyData.Serialize(ar, majorFileVersion, minorFileVersion);

			//---Need outlier free data. For old data, we need to set these equal
			//---to the filtered raw data
			POESOutlierfree.clear();
			POESOutlierfree.resize(timeaxis.size());
			PPHOutlierfree.clear();
			PPHOutlierfree.resize(timeaxis.size());
			copy(POES.begin(), POES.end(), POESOutlierfree.begin());
			copy(PPH.begin(), PPH.end(), PPHOutlierfree.begin());

			dataSetIsCompleteFromAGS(NULL);  // Do not need to check against raw data now, was done when reading from Torso
			FLOAT percBad;
			evaluateData(&percBad);
		}
		else {
			timeaxis.clear();
			POES.clear();
			PPH.clear();
			T0.clear();
			T1.clear();
			flags.clear();

			unsigned int size;
			ar >> size;
			FLOAT val;
			for (unsigned int i = 0; i < size; i++) {
				ar >> val;
				timeaxis.push_back(val);
				ar >> val;
				POES.push_back(val);
				ar >> val;
				PPH.push_back(val);
				ar >> val;
				T0.push_back(val);
				ar >> val;
				T1.push_back(val);
				flags.push_back(0);		// No flags here anymore
			}
			dataSetIsCompleteFromAGS(NULL);  // Do not need to check against raw data now, was done when reading from Torso
			FLOAT percBad;
			evaluateData(&percBad);
		}
	}
}

/////////////////////////////////////////

CAG200MicDataSet::CAG200MicDataSet()
{
}

CAG200MicDataSet::~CAG200MicDataSet()
{
}

void CAG200MicDataSet::clear(void) 
{
	ag200Mic.clear();
	timeaxis.clear();
}

void CAG200MicDataSet::addToXYRaw(FLOAT _mic,FLOAT _time)
{
	ag200Mic.push_back(_mic);
	timeaxis.push_back(_time);
}

unsigned int CAG200MicDataSet::evaluateData(float *_percBad)
{
	return 0;
}

int CAG200MicDataSet::getSize(void)
{
	return timeaxis.size();
}

vector <FLOAT> *CAG200MicDataSet::getAG200MicVector(void)
{
	return &ag200Mic;
}

void CAG200MicDataSet::Serialize(CArchive& ar, int majorFileVersion, int minorFileVersion)
{
	if (ar.IsStoring()) {

		ASSERT(timeaxis.size() == ag200Mic.size());

		unsigned int size = timeaxis.size();
		ar << size;
		for (unsigned int i = 0 ; i < size ; i++) {
			ar << timeaxis[i];
			ar << ag200Mic[i];
		}
	}
	else {
		timeaxis.clear();
		ag200Mic.clear();

		unsigned int size;
		ar >> size;
		FLOAT val;
		for (unsigned int i = 0 ; i < size ; i++) {
			ar >> val;
			timeaxis.push_back(val);
			ar >> val;
			ag200Mic.push_back(val);
		}
		dataSetIsCompleteFromAGS();
		FLOAT percBad;
		evaluateData(&percBad);
	}
}

/////////////////////////////////////////

CRespBeltDataSet::CRespBeltDataSet()
{
}

CRespBeltDataSet::~CRespBeltDataSet()
{
}

void CRespBeltDataSet::clear(void) 
{
	abdom.clear();
	chest.clear();
	cannula.clear();
	abdomEnv.clear();
	chestEnv.clear();
	cannulaEnv.clear();
	abdomEnvTime.clear();
	chestEnvTime.clear();
	cannulaEnvTime.clear();
	beltSum.clear();
	beltSumTime.clear();
	respFrq.clear();
	respFrqTime.clear();
	admittance.clear();
	admittanceTime.clear();
	timeaxis.clear();
}

void CRespBeltDataSet::addToXYRawCannula(FLOAT _cannula,FLOAT _time)
{
	cannula.push_back(_cannula);
	cannulaTime.push_back(_time);
}

void CRespBeltDataSet::addToXYRawBelts(FLOAT _abdom,FLOAT _chest,FLOAT _time)
{
	abdom.push_back(_abdom);
	chest.push_back(_chest);
	beltTime.push_back(_time);
}

unsigned int CRespBeltDataSet::evaluateData(float *_percBad)
{
	return 0;
}

bool CRespBeltDataSet::calibrate(void)
{
	vector <FLOAT>::iterator ic;
	vector <FLOAT> copyV;
	copyV.resize(cannula.size(),.0f);
	copy(cannula.begin(),cannula.end(),copyV.begin());
	cannula.clear();
	float sensitivity = (float) (- minCount + maxCount) / (maxPressure - minPressure);

	for (ic = copyV.begin() ; ic < copyV.end() ; ic++) {
		float press = (float) (*ic - minCount) / sensitivity;
		press += minPressure;
		press = - press;
		cannula.push_back(press);
	}
	return true;
}

int CRespBeltDataSet::getSize(void)
{
	return cannulaTime.size() > beltTime.size() ? cannulaTime.size() : beltTime.size() ;
}

vector <FLOAT> *CRespBeltDataSet::getAbdomVector(void)			{	return &abdom		; }
vector <FLOAT> *CRespBeltDataSet::getChestVector(void)			{	return &chest		; }
vector <FLOAT> *CRespBeltDataSet::getCannulaVector(void)		{	return &cannula		; }
vector <FLOAT> *CRespBeltDataSet::getAbdomEnvVector(void)		{	return &abdomEnv		; }
vector <FLOAT> *CRespBeltDataSet::getChestEnvVector(void)		{	return &chestEnv		; }
vector <FLOAT> *CRespBeltDataSet::getCannulaEnvVector(void)		{	return &cannulaEnv	; }
vector <FLOAT> *CRespBeltDataSet::getAbdomEnvVectorTime(void)	{	return &abdomEnvTime	; }
vector <FLOAT> *CRespBeltDataSet::getChestEnvVectorTime(void)	{	return &chestEnvTime	; }
vector <FLOAT> *CRespBeltDataSet::getCannulaEnvVectorTime(void) {	return &cannulaEnvTime; }
vector <FLOAT> *CRespBeltDataSet::getBeltSumVector(void)		{	return &beltSum		; }
vector <FLOAT> *CRespBeltDataSet::getBeltSumVectorTime(void)	{	return &beltSumTime	; }
vector <FLOAT> *CRespBeltDataSet::getRespFrqVector(void)		{	return &respFrq		; }
vector <FLOAT> *CRespBeltDataSet::getRespFrqVectorTime(void)	{	return &respFrqTime	; }
vector <FLOAT> *CRespBeltDataSet::getAdmittanceVector(void)		{	return &admittance	; }
vector <FLOAT> *CRespBeltDataSet::getAdmittanceVectorTime(void)	{	return &admittanceTime; }

vector <FLOAT> *CRespBeltDataSet::getCannulaTime(void)			{	return &cannulaTime; }
vector <FLOAT> *CRespBeltDataSet::getBeltTime(void)				{	return &beltTime; }

void CRespBeltDataSet::Serialize(CArchive& ar, int majorFileVersion, int minorFileVersion)
{
	if (ar.IsStoring()) {

		ASSERT(beltTime.size() == abdom.size());
		ASSERT(beltTime.size() == chest.size());
		ASSERT(cannulaTime.size() == cannula.size());
		ASSERT(abdomEnvTime.size() == abdomEnv.size());
		ASSERT(chestEnvTime.size() == chestEnv.size());
		ASSERT(cannulaEnvTime.size() == cannulaEnv.size());

		unsigned int size = beltTime.size();
		ar << size;
		for (unsigned int i = 0 ; i < size ; i++) {
			ar << beltTime[i];
			ar << abdom[i];
			ar << chest[i];
		}
		unsigned int sizec = cannulaTime.size();
		ar << sizec;
		for (unsigned int i = 0 ; i < sizec ; i++) {
			ar << cannulaTime[i];
			ar << cannula[i];
		}

		/////////////////////////////////////
		//---New in file version 15.0
		/////////////////////////////////////
		unsigned int sizeAB = abdomEnvTime.size();
		ar << sizeAB;
		for (unsigned int i = 0; i < sizeAB; i++) {
			ar << abdomEnvTime[i];
			ar << abdomEnv[i];
		}
		unsigned int sizeCB = chestEnvTime.size();
		ar << sizeCB;
		for (unsigned int i = 0; i < sizeCB; i++) {
			ar << chestEnvTime[i];
			ar << chestEnv[i];
		}
		unsigned int sizeCAB = cannulaEnvTime.size();
		ar << sizeCAB;
		for (unsigned int i = 0; i < sizeCAB; i++) {
			ar << cannulaEnvTime[i];
			ar << cannulaEnv[i];
		}
	}
	else {  // Reading
		clear();

		unsigned int size;
		ar >> size;
		FLOAT val;
		for (unsigned int i = 0 ; i < size ; i++) {
			ar >> val; beltTime.push_back(val);
			ar >> val; abdom.push_back(val);
			ar >> val; chest.push_back(val);
		}
		unsigned int sizec;
		ar >> sizec;
		FLOAT valc;
		for (unsigned int i = 0 ; i < sizec ; i++) {
			ar >> valc; cannulaTime.push_back(valc);
			ar >> valc; cannula.push_back(valc);
		}

		/////////////////////////////////////
		//---New in file version 15.0
		/////////////////////////////////////
		if (majorFileVersion >= 15) {
			unsigned int sizeAB;
			ar >> sizeAB;
			for (unsigned int i = 0; i < sizeAB; i++) {
				ar >> valc; abdomEnvTime.push_back(valc);
				ar >> valc; abdomEnv.push_back(valc);
			}
			unsigned int sizeCB;
			ar >> sizeCB;
			for (unsigned int i = 0; i < sizeCB; i++) {
				ar >> valc; chestEnvTime.push_back(valc);
				ar >> valc; chestEnv.push_back(valc);
			}
			unsigned int sizeCAB;
			ar >> sizeCAB;
			for (unsigned int i = 0; i < sizeCAB; i++) {
				ar >> valc; cannulaEnvTime.push_back(valc);
				ar >> valc; cannulaEnv.push_back(valc);
			}
		}

		dataSetIsCompleteFromAGS();
		FLOAT percBad;
		evaluateData(&percBad);
	}
}

void CRespBeltDataSet::dataSetIsCompleteFromAPN(void)
{
	dataSetIsCompleteFromAGS();
}

void CRespBeltDataSet::dataSetIsCompleteFromTorso(void)
{
	if (!noRespSensorFilter) {
		CBPFilter bpF;
		bpF.filter(&beltTime,&abdom,&chest);
		bpF.filter(&cannulaTime,&cannula);
	}
	dataSetIsCompleteFromAGS();
}

void CRespBeltDataSet::dataSetIsCompleteFromAGS(void)
{
	
	if (!abdom.size()) return;
	if (!cannulaTime.size()) return;
	if (!beltTime.size()) return;
	if (!chest.size()) return;
	if (!cannula.size()) return;

	auto result = std::minmax_element (abdom.begin(),abdom.end());
	float minAb = *result.first;
	float maxAb = *result.second;

	result = std::minmax_element (chest.begin(),chest.end());
	float minCh = *result.first;
	float maxCh = *result.second;

	result = std::minmax_element (cannula.begin(),cannula.end());
	float minCann = *result.first;
	float maxCann = *result.second;

	float avgSlope;
	findPeakToPeak(&abdom,&beltTime,&abdomEnv,&abdomEnvTime,&avgSlope);
	findPeakToPeak(&chest,&beltTime,&chestEnv,&chestEnvTime,&avgSlope);
	findPeakToPeak(&cannula,&cannulaTime,&cannulaEnv,&cannulaEnvTime,&avgSlope);
	makeSumVector(&abdomEnv,&chestEnv,&abdomEnvTime,&chestEnvTime,&beltSum,&beltSumTime);
	makeAdmittanceVector(&cannulaEnv,&beltSum,&cannulaEnvTime,&beltSumTime,&admittance,&admittanceTime);
	makeFrqVector(&cannula,&cannulaTime,&respFrq,&respFrqTime);
}

float CRespBeltDataSet::getStart(void)
{
	if (!getSize()) return .0f;

	float start1 = *beltTime.begin();
	float start2 = *cannulaTime.begin();
	return start1 < start2 ? start1 : start2 ;
}

float CRespBeltDataSet::getStop(void)
{
	if (!getSize()) return .0f;

	float stop1 = *(beltTime.end() - 1);
	float stop2 = *(cannulaTime.end() - 1);
	return stop1 > stop2 ? stop1 : stop2 ;
}

///////////////////////////////////////////////////////////////////////////////////////


CKeyData::CKeyData() :
						mean(.0f),
						medi(.0f),
						minimum(.0f),
						maximum(.0f),
						stdD(.0f)
{
}

CKeyData::~CKeyData()
{
}

void CKeyData::Serialize(CArchive& ar, int majorFileVersion, int minorFileVersion)
{
	if (ar.IsStoring()) {
		ar << mean;
		ar << medi;
		ar << minimum;
		ar << maximum;
		ar << stdD;
	}
	else {
		ar >> mean;
		ar >> medi;
		ar >> minimum;
		ar >> maximum;
		ar >> stdD;
	}
}

/*
Descriptions: Computes mean, max, median, minimum, maximum and standardDeviation
_v: The data vector
_f: Flags. If equal to NO_FLAG, the corresponding data value can be used. If _f is NULL, then all data are valid.
*/
void CKeyData::setData(vector <FLOAT> *_v,vector <unsigned short> *_f /* = NULL */)
{
	vector <FLOAT> strippedV;
	vector <FLOAT>::iterator i1;

	if (0 == _v->size()) {
		mean = .0f;
		medi = .0f;
		minimum = .0f;
		maximum = .0f;
		stdD = .0f;
		return;
	}

	vector <unsigned short>::iterator fi;
	i1 = _v->begin();
	double sum = .0f;
	if (_f) {
		for (fi = _f->begin() ; fi < _f->end()  && i1 < _v->end() ; fi++) {
			if (NO_FLAGS == *fi) {
				strippedV.push_back(*i1);
				sum += *i1;
			}
			i1++;
		}
	}
	else {
		strippedV = *_v;
		for (i1 = _v->begin() ; i1 < _v->end() ; i1++) 
			sum += *i1;
	}

	mean = strippedV.size() > 0 ? (float) (sum / strippedV.size()) : .0f;
	sort(strippedV.begin(),strippedV.end());
	medi = strippedV.size() > 0 ? strippedV.at(strippedV.size() / 2) : .0f;
	minimum = strippedV.size() > 0 ? strippedV.at(0) : .0f;
	maximum = strippedV.size() > 1 ? strippedV.at(strippedV.size() - 1) : minimum;
	
	//---StdDev
	if (strippedV.size() <= 1) return;
	FLOAT diff = .0f;
	FLOAT sqSum = .0f;
	for (i1 = strippedV.begin() ; i1 < strippedV.end(); i1++) {
		diff = *i1 - mean;
		sqSum += diff * diff;
	}
	stdD = sqrtf(sqSum / (strippedV.size() - 1));
}


////////////////////////////////////
//
//myVector::myVector()
//{
//	dataSint = .0f;
//	maximum = D3D10_FLOAT32_MAX;
//	minimum = .0f;
//	channelID = ID_NONE;
//}
//
//myVector::~myVector() 
//{
//}
//
//void myVector::clear(void)
//{
//	rawData.clear();
//	filteredData.clear();
//	dataSint = .0f;
//	maximum = D3D10_FLOAT32_MAX;
//	minimum = .0f;
//	average = .0f;
//	stdDev = .0f;
//}
//
//void myVector::push_backRaw(FLOAT _val) 
//{ 
//	rawData.push_back(_val);
//}
//
//void myVector::push_backFiltered(FLOAT _val) 
//{ 
//	filteredData.push_back(_val);
//}
//
//void myVector::setDataSint(FLOAT _ds) { dataSint = _ds; }
//
//FLOAT myVector::getDataSint(void) { return dataSint; }
//
//vector <FLOAT> *myVector::getData(int _typeFlag)
//{
//	switch (_typeFlag) {
//	case dataTypeRaw :		return &rawData;		break;
//	case dataTypeFiltered : return &filteredData;	break;
//	}
//	return NULL;
//}
//
//int myVector::getChannelID(void) { return channelID; }
//
//void myVector::setChannelID(const int _id) { channelID = _id; }
//
//
//void myVector::makeDifference(myVector *_v0,myVector *_v1)
//{
//	vector <FLOAT> *v1 = _v1->getData(dataTypeFiltered);
//	vector <FLOAT> *v0 = _v0->getData(dataTypeFiltered);
//	vector <FLOAT>::iterator end = v1->size() < v0->size() ? v1->end() : v0->end() ;
//	filteredData.clear();
//	
//	vector<FLOAT>::iterator iter0,iter1;
//	FLOAT diff;
//	iter1 = v1->begin();
//	for (iter0 = v0->begin() ; iter0 < end ; iter0++) {
//		diff = *iter0 - *iter1;
//		filteredData.push_back(diff);
//		iter1++;
//	}
//}
//
//void myVector::process(void)
//{
//	switch (channelID) {
//	case ID_NONE:
//		break;
//	case ID_P0 :
//	case ID_P1:
//		break;
//	case ID_T0:
//	case ID_T1:
//		derivative.clear();
//		derivative = filteredData;
//		makeDerivative(&derivative);
//		break;
//	case ID_IMPEDANCE :
//	case ID_ADMITTANCE :
//	case ID_SpO2 :
//	case ID_PR :
//	case ID_BPos :
//	case ID_CONTACT_SOUND :
//	case ID_AIRB_SOUND :
//	case ID_P0_P1 :
//	case ID_T0PLUST1 :
//	case ID_ARM :
//	case ID_RB_THORACIC :
//	case ID_RB_ABDOMINAL :
//	case ID_LEG_LEFT :
//	case ID_LEG_RIGHT :
//	case ID_EEG :
//		break;
//	}
//}
//
//void myVector::makeSum(myVector *_v0,myVector *_v1) 
//{
//	vector <FLOAT> *v1 = _v1->getData(dataTypeFiltered);
//	vector <FLOAT> *v0 = _v0->getData(dataTypeFiltered);
//	vector <FLOAT>::iterator end = v1->size() < v0->size() ? v1->end() : v0->end() ;
//	filteredData.clear();
//	
//	vector<FLOAT>::iterator iter0,iter1;
//	FLOAT diff;
//	iter1 = v1->begin();
//	for (iter0 = v0->begin() ; iter0 < end ; iter0++) {
//		diff = *iter0 + *iter1;
//		filteredData.push_back(diff);
//		iter1++;
//	}
//}
//
///*
//Description: Computes min,max,average and standard deviation of the vector between times _start and _stop
//			As no time vector is provided, equal time intervals are assumed
//Parameters:
//	_v : vector with data
//	_start: Start time in seconds
//	_stop: Stop time in seconds
//	_min, _max, _avg, stdDev: Results
//*/
//void myVector::getMaxMinAvgStdDev(vector <FLOAT> *_v,FLOAT _start,FLOAT _stop,FLOAT *_max,FLOAT *_min,FLOAT *_avg,FLOAT *_stdDev)
//{
//	//---Equal time intervals
//	ASSERT(dataSint);
//
//	if (.0f == dataSint) return;
//
//	int first = (int) ((_start < .0f ? .0f : _start) / dataSint);
//	int last = (int) (_stop / dataSint);
//	vector<FLOAT>::iterator s1,s2;
//	s1 = _v->begin() + first;
//	s2 = _v->begin() + last;
//	if (s2 >= _v->end()) s2 = _v->end();
//
//	if (s1 >= s2) return;
//
//	auto result = std::minmax_element (s1,s2);
//	*_min = *result.first;
//	*_max = *result.second;
//	
//	vector<FLOAT>::iterator iter;
//	FLOAT sum = .0f;
//	for (iter = s1 ; iter < s2; iter++) {
//		sum += *iter;
//	}
//	*_avg = sum / _v->size();
//	
//	if (_v->size() <= 1) return;
//
//	FLOAT diff = .0f;
//	FLOAT sqSum = .0f;
//	for (iter = _v->begin() ; iter < _v->end(); iter++) {
//		diff = *iter - average;
//		sqSum += diff * diff;
//	}
//	*_stdDev = sqrtf(sqSum) / (_v->size() - 1);
//	return;
//}
//
///*
//Description: Computes min,max,average and standard deviation of the entire vector
//Parameters:
//	_v : vector with data
//	_min, _max, _avg, stdDev: Results
//*/
//void myVector::getMaxMinAvgStdDev(vector <FLOAT> *_v,FLOAT *_max,FLOAT *_min,FLOAT *_avg,FLOAT *_stdDev)
//{
//	auto result = std::minmax_element (_v->begin(),_v->end());
//	*_min = *result.first;
//	*_max = *result.second;
//	vector<FLOAT>::iterator iter;
//	FLOAT sum = .0f;
//	for (iter = _v->begin() ; iter < _v->end(); iter++) {
//		sum += *iter;
//	}
//	*_avg = sum / _v->size();
//	
//	if (_v->size() <= 1) return;
//
//	FLOAT diff = .0f;
//	FLOAT sqSum = .0f;
//	for (iter = _v->begin() ; iter < _v->end(); iter++) {
//		diff = *iter - average;
//		sqSum += diff * diff;
//	}
//	*_stdDev = sqrtf(sqSum) / (_v->size() - 1);
//}
//
//
//void myVector::makeDerivative(vector <FLOAT> *_v)
//{
//	ASSERT(dataSint);
//
//	if (.0f == dataSint) return;
//
//	transform(_v->begin(),_v->end() - 1,_v->begin() + 1,_v->begin(),Difference <float> ());
//	_v->pop_back();
//	_v->push_back(.0f);
//	double sum = .0f;
//	for (unsigned int i = 0 ; i < _v->size() ; i++) sum += _v->at(i);
//	float avg = - (float) (sum / _v->size());
//	transform(_v->begin(),_v->end(),_v->begin(),Add <float> (avg));
//	float mm = 1.0f / dataSint;
//	transform(_v->begin(),_v->end(),_v->begin(),Multiply <float> (mm));
//}
//
///////////////////////////////////////////
////
////OscVector::OscVector()
////{
////}
////
////OscVector::~OscVector()
////{
////}
////
////
////void OscVector::clear(void)
////{
////	myVector::clear();
////	
////	envPos.clear();
////	envPosSec.clear();
////	envNeg.clear();
////	envNegSec.clear();
////	peakToPeak.clear();
////	peakToPeakSec.clear();
////}
////
////vector <FLOAT> *OscVector::getEnvPos(void)
////{
////	return &envPos;
////}
////vector <FLOAT> *OscVector::getEnvPosSec(void)
////{
////	return &envPosSec;
////}
////vector <FLOAT> *OscVector::getEnvNeg(void)
////{
////	return &envNeg;
////}
////vector <FLOAT> *OscVector::getEnvNegSec(void)
////{
////	return &envNegSec;
////}
////vector <FLOAT> *OscVector::getPeakToPeak(void)
////{
////	return &peakToPeak;
////}
////vector <FLOAT> *OscVector::getPeakToPeakSec(void)
////{
////	return &peakToPeakSec;
////}
////
////void OscVector::generateEnvelopeAndPP(void)
////{
////	long size = data.size();
////	if (size < 3) return;
////
////	ASSERT(dataSint);
////
////	vector <FLOAT>::iterator el1,el2;
////	bool sign,oldSign;
////	oldSign = sign = true;
////	long sampNum = 0L;
////
////	//---Start values
////	envPos.push_back(data.at(0));
////	envPosSec.push_back(.0f);
////	envNeg.push_back(data.at(0));
////	envNegSec.push_back(.0f);
////	peakToPeak.push_back(.0f);
////	el1 = data.begin();
////	el2 = data.begin();
////	el2++;
////	FLOAT max = .0f;
////	FLOAT min = .0f;
////	long minAt = 0L;
////	long maxAt = 0L;
////	for ( ; el2 != data.end( ); el2++ )	{
////		sign = *el2 >= *el1;
////		el1++;
////		if ((oldSign) && (!sign)) {	// Maximal
////			max = data.at(sampNum);
////			maxAt = sampNum;
////			envPos.push_back(max);
////			envPosSec.push_back(sampNum * dataSint);
////		}
////		if ((!oldSign) && (sign)) {	// Minimal
////			min = data.at(sampNum);
////			minAt = sampNum;
////			envNeg.push_back(min);
////			envNegSec.push_back(sampNum * dataSint);
////			peakToPeak.push_back(max - min);
////			peakToPeakSec.push_back((minAt + maxAt) * dataSint / 2.0f);
////		}
////		oldSign = sign;
////		sampNum++;
////	}
////	int y = 0;
////}
//
//////////////////////////////////////////////////////////////////////
//
//
//ImpedanceVector::ImpedanceVector()
//{
//}
//
//ImpedanceVector::~ImpedanceVector()
//{
//}
//
//void ImpedanceVector::makeImpedance(vector <FLOAT> *_p,vector <FLOAT> *_ptime,vector <FLOAT> *_t,vector <FLOAT> *_ttime)
//{
//	vector <FLOAT>::iterator p,pt,t,tt1,tt2,tend;
//	p = _p->begin();
//	pt = _ptime->begin();
//	t = _t->begin();
//	tt1 = _ttime->begin();
//	tend = _t->end();
//	tend--;
//	tt2 = tt1;
//
//	rawData.clear();
//	sec.clear();
//	FLOAT val;
//	val = *p / (*t == .0f ? LOW_TEMP : *t);
//	rawData.push_back(val);
//	pt++;
//	FLOAT distRight,distLeft,pLeft,pRight,temp,frac,press;
//	unsigned int size = _p->size() < _ptime->size() ? _p->size() - 1 : _ptime->size() - 1;
//	for (unsigned int i = 1 ; i < size ; i++) {
//		while ((*tt1 < *pt) && (tt1 < _ttime->end()) && (t < _t->end())) {
//			tt1++;
//			t++;
//		}
//		distLeft = *tt1 - *pt;
//		pt++;
//		pLeft = *p;
//		p++;
//		pRight = *p;
//		distRight = *pt - *tt1; 
//		frac = distLeft / (distLeft + distRight);
//		press = pLeft + (pRight - pLeft) * frac; 
//		temp = *t;
//		if (temp != .0f) {
//			val = press / temp;
//			rawData.push_back(val);
//			sec.push_back(*tt1);
//		}
//	}
//}
//
//vector <FLOAT> *ImpedanceVector::getTimeData(void)
//{
//	return &sec;
//}
//
//void ImpedanceVector::clear(void)
//{
//	myVector::clear();
//	
//	sec.clear();
//}
//
////////////////////////////////////////////
//
//
//AdmittanceVector::AdmittanceVector()
//{
//}
//
//AdmittanceVector::~AdmittanceVector()
//{
//}
//
//vector <FLOAT> *AdmittanceVector::getTimeData(void)
//{
//	return &sec;
//}
//
//
//void AdmittanceVector::makeAdmittance(vector <FLOAT> *_p,vector <FLOAT> *_ptime,vector <FLOAT> *_t,vector <FLOAT> *_ttime)
//{
//	vector <FLOAT>::iterator p,pt,t,tt1,tt2,tend;
//	p = _p->begin();
//	pt = _ptime->begin();
//	t = _t->begin();
//	tt1 = _ttime->begin();
//	tend = _t->end();
//	tend--;
//	tt2 = tt1;
//
//	rawData.clear();
//	sec.clear();
//	FLOAT val;
//	FLOAT temp = *t;
//	FLOAT press = *p == .0f ? LOW_PRESS : *p ; 
//	val = temp / press;
//	rawData.push_back(val);
//	pt++;
//	FLOAT distRight,distLeft,pLeft,pRight,frac;
//	unsigned int size = _p->size() < _ptime->size() ? _p->size() - 1 : _ptime->size() - 1;
//	for (unsigned int i = 1 ; i < size ; i++) {
//		while ((*tt1 < *pt) && (tt1 < _ttime->end()) && (t < _t->end())) {
//			tt1++;
//			t++;
//		}
//		distLeft = *tt1 - *pt;
//		pt++;
//		pLeft = *p;
//		p++;
//		pRight = *p;
//		distRight = *pt - *tt1; 
//		frac = distLeft / (distLeft + distRight);
//		press = pLeft + (pRight - pLeft) * frac; 
//		temp = *t;
//		if (press != .0f) {
//			val = temp / press;
//			rawData.push_back(val);
//			sec.push_back(*tt1);
//		}
//	}
//}
//
//void AdmittanceVector::clear(void)
//{
//	myVector::clear();
//	
//	sec.clear();
//}

CManualMarkerSet::CManualMarkerSet()
{
}

CManualMarkerSet::~CManualMarkerSet()
{
}

void CManualMarkerSet::dataSetIsCompleteFromTorso(void)
{
	mMarkerStartStopVector.clear();
	vector <MARK>::iterator mIt = mMarkerTimes.begin();
	for (; mIt < mMarkerTimes.end(); ++mIt) {
		mMarkerStartStopVector.push_back(mIt->time);
		mMarkerStartStopVector.push_back(mIt->time);
	}
}

void CManualMarkerSet::Serialize(CArchive & ar, int majorFileVersion, int minorFileVersion)
{
	if (ar.IsStoring()) {
		unsigned int size = mMarkerTimes.size();
		ar << size;
		for (unsigned int i = 0; i < size; i++) {
			ar << mMarkerTimes[i].text;
			ar << mMarkerTimes[i].time;
		}

	}
	else {
		mMarkerTimes.clear();
		unsigned int size;
		ar >> size;
		FLOAT fl;
		CString txt;
		for (unsigned int i = 0; i < size; i++) {
			ar >> txt;
			ar >> fl;
			MARK mrk;
			mrk.time = fl;
			mrk.text = txt;
			mMarkerTimes.push_back(mrk);
		}
	}
}

void CManualMarkerSet::add(float _time)
{
	MARK m;
	m.time = _time;
	m.text = _T("");
	mMarkerTimes.push_back(m);
}

int CManualMarkerSet::getSize(void)
{
	return mMarkerTimes.size();
}

vector<FLOAT>* CManualMarkerSet::getMMarkerVector(void)
{
	return &mMarkerStartStopVector;
}

void CManualMarkerSet::fillManualEventArray(CArray<CManualMarkerEvnt*, CManualMarkerEvnt*> *_mmArray)
{
	int num = mMarkerTimes.size();
	vector <MARK>::iterator mI = mMarkerTimes.begin();
	for (; mI < mMarkerTimes.end(); ++mI) {
		CManualMarkerEvnt *eP = new CManualMarkerEvnt();
		eP->setTo(mI->time);
		eP->setFrom(mI->time);
		_mmArray->Add(eP);
	}
}

void CManualMarkerSet::clear()
{
	mMarkerTimes.clear();
	mMarkerStartStopVector.clear();
}
