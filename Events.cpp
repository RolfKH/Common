
/*!
* @author		RKH
* @date		06.06.2017
* @copyright (C)Spiro Medical AS 2013 - 2017
*/

#include "StdAfx.h"
#include "Events.h"
#include "D2Graph.h"
#include "..\Common\SpiroWaitCursor.h"
#include "..\Common\vectorFunctions.h"

extern int developerMode;
extern CString swVersion;

template<class InputIterator, class UnaryPredicate>
InputIterator find_if_not_backw(InputIterator first, InputIterator last, UnaryPredicate pred)
{
	while (first != last) {
		if (!pred(*first)) return first;
		--first;
	}
	return last;
}


/*
Description: From a float time value, generates a string in this format hh:mm:ss
*/
CString xxxxgethms(FLOAT _t)
{
	bool neg = _t < .0f ? true : false;
	FLOAT tm = neg ? -_t : _t;
	int h = (int)(tm / 3600.0f);
	float m = tm - (float)h * 3600;
	m = (float)((int)(m / 60.0f));
	int s = (int)(tm - (float)h * 3600 - m * 60.0f);

	CString st;
	if (neg) st.Format(_T("- %02d:%02.0f:%02d"), h, m, s);
	else st.Format(_T("%02d:%02.0f:%02d"), h, m, s);
	return st;
}

 // Return whether first element is smaller og equal to the second
bool fltSmaller(FLOAT elem1, FLOAT elem2)
{
	return elem1 <= elem2;
}

// Return whether first element is greater or equal to the second
bool fltGreater(FLOAT elem1, FLOAT elem2)
{
	return elem1 >= elem2;
}

//---Used for sorting oximeter values
bool sortSpO2(OXIMETER_SET el1, OXIMETER_SET el2) {
	return el1.spO2 < el2.spO2;
}

//---Used for sorting PR values
bool sortPR(OXIMETER_SET el1, OXIMETER_SET el2) {
	return el1.pr < el2.pr;
}

UINT setToEventTypeNone(UINT el) { return evntTypeNone; }

//---Or'ing bits
UINT orVectorsWithAwake(UINT el1, UINT el2)	{ return ((el1 & evntTypeAwake) | el2)	; }
UINT orVectorsWithExcluded(UINT el1, UINT el2) { return ((el1 & evntTypeExcluded) | el2); }

UINT orElementTypeNone(UINT el)			{ return el | evntTypeNone				; }
UINT orElementTypeMixed(UINT el)		{ return el | evntTypeMixed				; }
UINT orElementTypeCentral(UINT el)		{ return el | evntTypeCentral			; }
UINT orElementTypeObstr(UINT el)		{ return el | evntTypeObstr				; }
UINT orElementTypeHypoCentral(UINT el)	{ return el | evntTypeHypoCentral		; }
UINT orElementTypeHypoObstr(UINT el)	{ return el | evntTypeHypoObstr 		; }
UINT orElementTypeRERA(UINT el)			{ return el | evntTypeRERA				; }
UINT orElementTypeArousal(UINT el)		{ return el | evntTypeArousal			; }
UINT orElementTypeSpO2Drop(UINT el)		{ return el | evntTypeSpO2Drop			; }
UINT orElementTypeManMarker(UINT el)	{ return el | evntTypeManMarker			; }
UINT orElementTypeAwake(UINT el)		{ return el | evntTypeAwake				; }
UINT orElementTypeSnoring(UINT el)		{ return el | evntTypeSnoring			; }
UINT orElementTypeExcluded(UINT el)		{ return el | evntTypeExcluded			; }
UINT orElementTypeSwallow(UINT el)		{ return el | evntTypeSwallow			; }
UINT orElementTypeImported(UINT el)		{ return el | evntTypeImported			; }
UINT orAHLevelTypeUpper(UINT el)		{ return el | evntAHLevelUpper			; }
UINT orAHLevelTypeLower(UINT el)		{ return el | evntAHLevelLower			; }
UINT orAHLevelTypeMulti(UINT el)		{ return el | evntAHLevelMulti			; }
UINT orAHLevelTypeUndef(UINT el)		{ return el | evntAHLevelUndef			; }
UINT orSnoringLevelTypeUpper(UINT el)	{ return el | evntSnoringLevelUpper		; }
UINT orSnoringLevelTypeLower(UINT el)	{ return el | evntSnoringLevelLower		; }
UINT orSnoringLevelTypeMulti(UINT el)	{ return el | evntSnoringLevelMulti		; }
UINT orSnoringLevelTypeUndef(UINT el)	{ return el | evntSnoringLevelUndef		; }

UINT orBodyPosSwitchLeft(UINT el)		{ return el | bodyPosSwitchLeft;	}
UINT orBodyPosSwitchRight(UINT el)		{ return el | bodyPosSwitchRight;	}
UINT orBodyPosSwitchProne(UINT el)		{ return el | bodyPosSwitchProne;	}
UINT orBodyPosSwitchSupine(UINT el)		{ return el | bodyPosSwitchSupine;	}
UINT orBodyPosSwitchUpright(UINT el)	{ return el | bodyPosSwitchUpright; }

//---Turning bits off
UINT offElementTypeNone(UINT el)			{ return el; }
UINT offElementTypeMixed(UINT el)			{ return el &= ~evntTypeMixed;			}
UINT offElementTypeCentral(UINT el)			{ return el &= ~evntTypeCentral;		}
UINT offElementTypeObstr(UINT el)			{ return el &= ~evntTypeObstr;			}
UINT offElementTypeHypoCentral(UINT el)		{ return el &= ~evntTypeHypoCentral;	}
UINT offElementTypeHypoObstr(UINT el)		{ return el &= ~evntTypeHypoObstr;		}
UINT offElementTypeRERA(UINT el)			{ return el &= ~evntTypeRERA;			}
UINT offElementTypeArousal(UINT el)			{ return el &= ~evntTypeArousal;		}
UINT offElementTypeSpO2Drop(UINT el)		{ return el &= ~evntTypeSpO2Drop;		}
UINT offElementTypeManMarker(UINT el)		{ return el &= ~evntTypeManMarker;		}
UINT offElementTypeAwake(UINT el)			{ return el &= ~evntTypeAwake;			}
UINT offElementTypeSnoring(UINT el)			{ return el &= ~evntTypeSnoring;		}
UINT offElementTypeExcluded(UINT el)		{ return el &= ~evntTypeExcluded;		}
UINT offElementTypeSwallow(UINT el)			{ return el &= ~evntTypeSwallow;		}
UINT offElementTypeImported(UINT el)		{ return el &= ~evntTypeImported;		}
UINT offElementTypeAH(UINT el)				{ return el &= ~evntTypeAH;				}
UINT offElementTypeAHandAllLevels(UINT el) { return el &= ~(evntTypeAH | evntAHLevelAll); }

UINT offAHLevelTypeUpper(UINT el)			{ return el &= ~evntAHLevelUpper;		}
UINT offAHLevelTypeLower(UINT el)			{ return el &= ~evntAHLevelLower;		}
UINT offAHLevelTypeMulti(UINT el)			{ return el &= ~evntAHLevelMulti;		}
UINT offAHLevelTypeUndef(UINT el)			{ return el &= ~evntAHLevelUndef;		}
UINT offAHLevelAll(UINT el)					{ return el &= ~evntAHLevelAll;			}

UINT offSnoringLevelTypeUpper(UINT el)		{ return el &= ~evntSnoringLevelUpper;	}
UINT offSnoringLevelTypeLower(UINT el)		{ return el &= ~evntSnoringLevelLower;	}
UINT offSnoringLevelTypeMulti(UINT el)		{ return el &= ~evntSnoringLevelMulti;	}
UINT offSnoringLevelTypeUndef(UINT el)		{ return el &= ~evntSnoringLevelUndef;	}
UINT offSnoringLevelAll(UINT el)			{ return el &= ~evntSnoringLevelAll;	}

bool isDeadPoes (unsigned short i)			{ return ((i & deadPoes) > 0)			; }
bool isDeadPph (unsigned short i)			{ return ((i & deadPph ) > 0)			; }
bool isLowFlow50 (unsigned short i)			{ return ((i & lowFlow50 ) > 0)			; }
bool isLowFlow40(unsigned short i)			{ return ((i & lowFlow40) > 0)			; }
bool isLowFlow30(unsigned short i)			{ return ((i & lowFlow30) > 0)			; }
bool isZeroFlow (unsigned short i)			{ return ((i & zeroFlow) > 0)			; }
bool isDecrPoes (unsigned short i)			{ return ((i & decrPoes) > 0)			; }
bool isBelow40percent (float el)			{ return (el <= GRADIENT_LOWER_LIMIT)	; }
bool isAbove60percent (float el)			{ return (el >= GRADIENT_UPPER_LIMIT)	; }
bool isNear50percent (float el)				{ return ((el > GRADIENT_LOWER_LIMIT) && (el < GRADIENT_UPPER_LIMIT));	}

bool isVisuallyEditableEvent(UINT el)	{	return ((el &	evntTypeVisualEditableEvents) > 0)	; }
bool isExcluded(UINT el)				{	return ((el &	evntTypeExcluded) > 0)		; }
bool isSnoring(UINT el)					{	return ((el &	evntTypeSnoring) > 0)		; }
bool isAwake(UINT el)					{	return ((el &	evntTypeAwake) > 0)			; }
bool isMixed(UINT el)					{	return ((el &	evntTypeMixed) > 0)			; }
bool isObstr(UINT el)					{	return ((el &	evntTypeObstr) > 0)			; }
bool isCentral(UINT el)					{	return ((el &	evntTypeCentral) > 0)		; }
bool isHypoObstr(UINT el)				{	return ((el &	evntTypeHypoObstr) > 0)		; }
bool isHypoCentral(UINT el)				{	return ((el &	evntTypeHypoCentral) > 0)	; }
bool isDesaturation(UINT el)			{	return ((el &	evntTypeSpO2Drop) > 0)		; }
bool isRERA(UINT el)					{	return ((el &	evntTypeRERA) > 0)			; }
bool isRERAandNotArousal(UINT el)			{	return ((el &	evntTypeRERA) && (!(el & evntTypeArousal))); }

bool isNotRERAOrArousal(UINT el)		{	return ((!(el & evntTypeRERA)) || (el & evntTypeArousal)); }

bool isAHNotAwakeNotExcluded(UINT el)			{ return ((el &	evntTypeAH)				&& !(el & evntTypeAwake) && !(el & evntTypeExcluded)); }
bool isDesaturationNotAwakeNotExcluded(UINT el) { return ((el &	evntTypeSpO2Drop)		&& !(el & evntTypeAwake) && !(el & evntTypeExcluded)); }
bool isAwakeNotExcluded(UINT el)				{ return ((el &	evntTypeAwake)									 && !(el & evntTypeExcluded)); }
bool isManualNotExcluded(UINT el)				{ return ((el & evntTypeManMarker)		&& !(el & evntTypeExcluded)); }
bool isExcludedNotAwake(UINT el)				{ return ((el &	evntTypeExcluded)		&& !(el & evntTypeAwake)); }

bool isSnoringNotAwakeNotExcluded(UINT el)		{ return ((el &	evntTypeSnoring)		&& !(el & evntTypeAwake) && !(el & evntTypeExcluded)); }
bool isMixedNotAwakeNotExcluded(UINT el)		{ return ((el &	evntTypeMixed)			&& !(el & evntTypeAwake) && !(el & evntTypeExcluded)); }
bool isObstrNotAwakeNotExcluded(UINT el)		{ return ((el &	evntTypeObstr)			&& !(el & evntTypeAwake) && !(el & evntTypeExcluded)); }
bool isRERANotAwakeNotExcluded(UINT el)			{ return ((el &	evntTypeRERA)			&& !(el & evntTypeAwake) && !(el & evntTypeExcluded)); }
bool isHypoObstrNotAwakeNotExcluded(UINT el)	{ return ((el &	evntTypeHypoObstr)		&& !(el & evntTypeAwake) && !(el & evntTypeExcluded)); }
bool isCentralNotAwakeNotExcluded(UINT el)		{ return ((el &	evntTypeCentral)		&& !(el & evntTypeAwake) && !(el & evntTypeExcluded)); }
bool isHypoCentralNotAwakeNotExcluded(UINT el)	{ return ((el &	evntTypeHypoCentral)	&& !(el & evntTypeAwake) && !(el & evntTypeExcluded)); }
bool isArousalNotAwakeNotExcluded(UINT el)		{ return ((el &	evntTypeArousal)		&& !(el & evntTypeAwake) && !(el & evntTypeExcluded)); }
bool isSwallowNotExcluded(UINT el)				{ return ((el &	evntTypeSwallow)								 && !(el & evntTypeExcluded)); }
bool isImportedNotExcluded(UINT el)				{ return ((el &	evntTypeImported)		&& !(el & evntTypeExcluded)); }

bool isOxInvalidWhileAwake(OXIMETER_SET el)			{ return (el.state & evntTypeInvalidOximetry) && (el.state & evntTypeAwake) && !(el.state & evntTypeExcluded); }
bool isOxInvalidWhileExcluded(OXIMETER_SET el)		{ return (el.state & evntTypeInvalidOximetry) && (el.state & evntTypeExcluded); }
bool isOxInvalidWhileNotExcluded(OXIMETER_SET el)	{ return (el.state & evntTypeInvalidOximetry) && !(el.state & evntTypeExcluded); }
bool isOxInvalidWhileAsleep(OXIMETER_SET el)		{ return (el.state & evntTypeInvalidOximetry) && !((el.state & evntTypeExcluded) || (el.state & evntTypeAwake)); }
bool isOxAwake(OXIMETER_SET el)						{ return (el.state & evntTypeAwake)	&& !(el.state & evntTypeExcluded) && !(el.state & evntTypeInvalidOximetry); }
bool isOxAsleep(OXIMETER_SET el)					{ return !(el.state & evntTypeAwake) && !(el.state & evntTypeExcluded) && !(el.state & evntTypeInvalidOximetry); }
bool isOxOverall(OXIMETER_SET el)					{ return !(el.state & evntTypeExcluded) && !(el.state & evntTypeInvalidOximetry); }
bool isBelowOx90Percent(OXIMETER_SET el)			{ return (el.spO2 < 90.0f); }

bool isBPLeft(UINT el)							{ return ((el & bodyPosSwitchLeft) > 0);	}
bool isBPRight(UINT el)							{ return ((el & bodyPosSwitchRight) > 0);	}
bool isBPProne(UINT el)							{ return ((el & bodyPosSwitchProne) > 0);	}
bool isBPSupine(UINT el)						{ return ((el & bodyPosSwitchSupine) > 0);	}
bool isBPUpright(UINT el)						{ return ((el & bodyPosSwitchUpright) > 0); }

bool isBPLeftNotAwakeExcluded(UINT el)			{ return (el & bodyPosSwitchLeft)    && !(el & evntTypeAwakeOrExcluded);	}
bool isBPRightNotAwakeExcluded(UINT el)			{ return (el & bodyPosSwitchRight)   && !(el & evntTypeAwakeOrExcluded);	}
bool isBPProneNotAwakeExcluded(UINT el)			{ return (el & bodyPosSwitchProne)   && !(el & evntTypeAwakeOrExcluded);	}
bool isBPSupineNotAwakeExcluded(UINT el)		{ return (el & bodyPosSwitchSupine)  && !(el & evntTypeAwakeOrExcluded);	}
bool isBPUprightNotAwakeExcluded(UINT el)		{ return (el & bodyPosSwitchUpright) && !(el & evntTypeAwakeOrExcluded);	}

bool isSnoringBPLeftNotAwakeExcluded(UINT el)		{ return (el & bodyPosSwitchLeft)	 && (el & evntTypeSnoring)	&& !(el & evntTypeAwakeOrExcluded); }
bool isSnoringBPRightNotAwakeExcluded(UINT el)		{ return (el & bodyPosSwitchRight)	 && (el & evntTypeSnoring)	&& !(el & evntTypeAwakeOrExcluded); }
bool isSnoringBPProneNotAwakeExcluded(UINT el)		{ return (el & bodyPosSwitchProne)	 && (el & evntTypeSnoring)	&& !(el & evntTypeAwakeOrExcluded); }
bool isSnoringBPSupineNotAwakeExcluded(UINT el)		{ return (el & bodyPosSwitchSupine)	 && (el & evntTypeSnoring)	&& !(el & evntTypeAwakeOrExcluded); }
bool isSnoringBPUprightNotAwakeExcluded(UINT el)	{ return (el & bodyPosSwitchUpright) && (el & evntTypeSnoring)	&& !(el & evntTypeAwakeOrExcluded); }

bool isSnoringUpperNotAwakeExcluded(UINT el)		{ return (el & evntTypeSnoring) && (el & evntSnoringLevelUpper) && !(el & evntTypeAwakeOrExcluded); }
bool isSnoringMultiNotAwakeExcluded(UINT el) { return (el & evntTypeSnoring) && (el & evntSnoringLevelMulti) && !(el & evntTypeAwakeOrExcluded); }
bool isSnoringLowerNotAwakeExcluded(UINT el) { return (el & evntTypeSnoring) && (el & evntSnoringLevelLower) && !(el & evntTypeAwakeOrExcluded); }
bool isSnoringUndefNotAwakeExcluded(UINT el) { return (el & evntTypeSnoring) && (el & evntSnoringLevelUndef) && !(el & evntTypeAwakeOrExcluded); }


bool isSnoringNotAwakeExcluded(UINT el)				{ return (el & evntTypeSnoring) && !(el & evntTypeAwakeOrExcluded); }

bool isEvntSnoringLevelLower(UINT el) { return ((el & evntSnoringLevelLower) > 0); }
bool isEvntSnoringLevelUpper(UINT el) { return ((el & evntSnoringLevelUpper) > 0); }
bool isEvntSnoringLevelMulti(UINT el) { return ((el & evntSnoringLevelMulti) > 0); }
bool isEvntSnoringLevelUndef(UINT el) { return ((el & evntSnoringLevelUndef) > 0); }

bool isEvntSnoringLevelUpperNotAwakeExcluded(UINT el) { return (el & evntTypeSnoring) && (el & evntSnoringLevelUpper) && !(el & evntTypeAwakeOrExcluded); }
bool isEvntSnoringLevelLowerNotAwakeExcluded(UINT el) { return (el & evntTypeSnoring) && (el & evntSnoringLevelLower) && !(el & evntTypeAwakeOrExcluded); }
bool isEvntSnoringLevelMultiNotAwakeExcluded(UINT el) { return (el & evntTypeSnoring) && (el & evntSnoringLevelMulti) && !(el & evntTypeAwakeOrExcluded); }
bool isEvntSnoringLevelUndefNotAwakeExcluded(UINT el) { return (el & evntTypeSnoring) && (el & evntSnoringLevelUndef) && !(el & evntTypeAwakeOrExcluded); }

//---SpO2 while awake histogram functions
bool isOxWhileAwake95to100(OXIMETER_SET el)	{ 
	return	(!(el.state & evntTypeInvalidOximetry) &&
		!(el.state & evntTypeExcluded) &&
		(el.state & evntTypeAwake)) && (el.spO2 >= 95.0f) && (el.spO2 <= 100.0f);}
bool isOxWhileAwake90to95(OXIMETER_SET el)	{ 
	return	(!(el.state & evntTypeInvalidOximetry) &&
		!(el.state & evntTypeExcluded) &&
		(el.state & evntTypeAwake)) && (el.spO2 >= 90.0f) && (el.spO2 < 95.0f);}
bool isOxWhileAwake85to90(OXIMETER_SET el)	{ 
	return	(!(el.state & evntTypeInvalidOximetry) &&
		!(el.state & evntTypeExcluded) &&
		(el.state & evntTypeAwake)) && (el.spO2 >= 85.0f) && (el.spO2 < 90.0f);}
bool isOxWhileAwake80to85(OXIMETER_SET el) {
	return	(!(el.state & evntTypeInvalidOximetry) &&
		!(el.state & evntTypeExcluded) &&
		(el.state & evntTypeAwake)) && (el.spO2 >= 80.0f) && (el.spO2 < 85.0f);}
bool isOxWhileAwake75to80(OXIMETER_SET el) {
	return	(!(el.state & evntTypeInvalidOximetry) &&
		!(el.state & evntTypeExcluded) &&
		(el.state & evntTypeAwake)) && (el.spO2 >= 75.0f) && (el.spO2 < 80.0f);}
bool isOxWhileAwake70to75(OXIMETER_SET el) {
	return	(!(el.state & evntTypeInvalidOximetry) &&
		!(el.state & evntTypeExcluded) &&
		(el.state & evntTypeAwake)) && (el.spO2 >= 70.0f) && (el.spO2 < 75.0f);}
bool isOxWhileAwake65to70(OXIMETER_SET el) {
	return	(!(el.state & evntTypeInvalidOximetry) &&
		!(el.state & evntTypeExcluded) &&
		(el.state & evntTypeAwake)) && (el.spO2 >= 65.0f) && (el.spO2 < 70.0f);}
bool isOxWhileAwake0to65(OXIMETER_SET el) {
	return	(!(el.state & evntTypeInvalidOximetry) &&
		!(el.state & evntTypeExcluded) &&
		(el.state & evntTypeAwake)) && (el.spO2 <= 65.0f);}

//---SpO2 while asleep histogram functions
bool isOxWhileAsleep95to100(OXIMETER_SET el) {
	return	(!(el.state & evntTypeInvalidOximetry) &&
		!(el.state & evntTypeExcluded) &&
		!(el.state & evntTypeAwake)) && (el.spO2 >= 95.0f) && (el.spO2 <= 100.0f);
}
bool isOxWhileAsleep90to95(OXIMETER_SET el) {
	return	(!(el.state & evntTypeInvalidOximetry) &&
		!(el.state & evntTypeExcluded) &&
		!(el.state & evntTypeAwake)) && (el.spO2 >= 90.0f) && (el.spO2 < 95.0f);
}
bool isOxWhileAsleep85to90(OXIMETER_SET el) {
	return	(!(el.state & evntTypeInvalidOximetry) &&
		!(el.state & evntTypeExcluded) &&
		!(el.state & evntTypeAwake)) && (el.spO2 >= 85.0f) && (el.spO2 < 90.0f);
}
bool isOxWhileAsleep80to85(OXIMETER_SET el) {
	return	(!(el.state & evntTypeInvalidOximetry) &&
		!(el.state & evntTypeExcluded) &&
		!(el.state & evntTypeAwake)) && (el.spO2 >= 80.0f) && (el.spO2 < 85.0f);
}
bool isOxWhileAsleep75to80(OXIMETER_SET el) {
	return	(!(el.state & evntTypeInvalidOximetry) &&
		!(el.state & evntTypeExcluded) &&
		!(el.state & evntTypeAwake)) && (el.spO2 >= 75.0f) && (el.spO2 < 80.0f);
}
bool isOxWhileAsleep70to75(OXIMETER_SET el) {
	return	(!(el.state & evntTypeInvalidOximetry) &&
		!(el.state & evntTypeExcluded) &&
		!(el.state & evntTypeAwake)) && (el.spO2 >= 70.0f) && (el.spO2 < 75.0f);
}
bool isOxWhileAsleep65to70(OXIMETER_SET el) {
	return	(!(el.state & evntTypeInvalidOximetry) &&
		!(el.state & evntTypeExcluded) &&
		!(el.state & evntTypeAwake)) && (el.spO2 >= 65.0f) && (el.spO2 < 70.0f);
}
bool isOxWhileAsleep0to65(OXIMETER_SET el) {
	return	(!(el.state & evntTypeInvalidOximetry) &&
		!(el.state & evntTypeExcluded) &&
		!(el.state & evntTypeAwake)) && (el.spO2 <= 65.0f);
}

//---SpO2 overall histogram functions
bool isOx95to100(OXIMETER_SET el) {
	return	(!(el.state & evntTypeInvalidOximetry) &&
		!(el.state & evntTypeExcluded)) && (el.spO2 >= 95.0f) && (el.spO2 <= 100.0f);
}
bool isOx90to95(OXIMETER_SET el) {
	return	(!(el.state & evntTypeInvalidOximetry) &&
		!(el.state & evntTypeExcluded)) && (el.spO2 >= 90.0f) && (el.spO2 < 95.0f);
}
bool isOx85to90(OXIMETER_SET el) {
	return	(!(el.state & evntTypeInvalidOximetry) &&
		!(el.state & evntTypeExcluded)) && (el.spO2 >= 85.0f) && (el.spO2 < 90.0f);
}
bool isOx80to85(OXIMETER_SET el) {
	return	(!(el.state & evntTypeInvalidOximetry) &&
		!(el.state & evntTypeExcluded)) && (el.spO2 >= 80.0f) && (el.spO2 < 85.0f);
}
bool isOx75to80(OXIMETER_SET el) {
	return	(!(el.state & evntTypeInvalidOximetry) &&
		!(el.state & evntTypeExcluded)) && (el.spO2 >= 75.0f) && (el.spO2 < 80.0f);
}
bool isOx70to75(OXIMETER_SET el) {
	return	(!(el.state & evntTypeInvalidOximetry) &&
		!(el.state & evntTypeExcluded)) && (el.spO2 >= 70.0f) && (el.spO2 < 75.0f);
}
bool isOx65to70(OXIMETER_SET el) {
	return	(!(el.state & evntTypeInvalidOximetry) &&
		!(el.state & evntTypeExcluded)) && (el.spO2 >= 65.0f) && (el.spO2 < 70.0f);
}
bool isOx0to65(OXIMETER_SET el) {
	return	(!(el.state & evntTypeInvalidOximetry) &&
		!(el.state & evntTypeExcluded)) && (el.spO2 <= 65.0f);
}

//***********************************************************
//---PR while awake histogram functions
bool isPRWhileAwakeAbove120(OXIMETER_SET el) {
	return	(!(el.state & evntTypeInvalidOximetry) &&
		!(el.state & evntTypeExcluded) &&
		(el.state & evntTypeAwake)) && (el.pr >= 120.0f);
}
bool isPRWhileAwake110to120(OXIMETER_SET el) {
	return	(!(el.state & evntTypeInvalidOximetry) &&
		!(el.state & evntTypeExcluded) &&
		(el.state & evntTypeAwake)) && (el.pr >= 110.0f) && (el.pr < 120.0f);
}
bool isPRWhileAwake100to110(OXIMETER_SET el) {
	return	(!(el.state & evntTypeInvalidOximetry) &&
		!(el.state & evntTypeExcluded) &&
		(el.state & evntTypeAwake)) && (el.pr >= 100.0f) && (el.pr < 110.0f);
}
bool isPRWhileAwake90to100(OXIMETER_SET el) {
	return	(!(el.state & evntTypeInvalidOximetry) &&
		!(el.state & evntTypeExcluded) &&
		(el.state & evntTypeAwake)) && (el.pr >= 90.0f) && (el.pr < 100.0f);
}
bool isPRWhileAwake80to90(OXIMETER_SET el) {
	return	(!(el.state & evntTypeInvalidOximetry) &&
		!(el.state & evntTypeExcluded) &&
		(el.state & evntTypeAwake)) && (el.pr >= 80.0f) && (el.pr < 90.0f);
}
bool isPRWhileAwake70to80(OXIMETER_SET el) {
	return	(!(el.state & evntTypeInvalidOximetry) &&
		!(el.state & evntTypeExcluded) &&
		(el.state & evntTypeAwake)) && (el.pr >= 70.0f) && (el.pr < 80.0f);
}
bool isPRWhileAwake60to70(OXIMETER_SET el) {
	return	(!(el.state & evntTypeInvalidOximetry) &&
		!(el.state & evntTypeExcluded) &&
		(el.state & evntTypeAwake)) && (el.pr >= 60.0f) && (el.pr < 70.0f);
}
bool isPRWhileAwake0to60(OXIMETER_SET el) {
	return	(!(el.state & evntTypeInvalidOximetry) &&
		!(el.state & evntTypeExcluded) &&
		(el.state & evntTypeAwake)) && (el.pr <= 60.0f);
}

//---PR while asleep histogram functions
bool isPRWhileAsleepAbove120(OXIMETER_SET el) {
	return	(!(el.state & evntTypeInvalidOximetry) &&
		!(el.state & evntTypeExcluded) &&
		!(el.state & evntTypeAwake)) && (el.pr >= 120.0f);
}
bool isPRWhileAsleep110to120(OXIMETER_SET el) {
	return	(!(el.state & evntTypeInvalidOximetry) &&
		!(el.state & evntTypeExcluded) &&
		!(el.state & evntTypeAwake)) && (el.pr >= 110.0f) && (el.pr < 120.0f);
}
bool isPRWhileAsleep100to110(OXIMETER_SET el) {
	return	(!(el.state & evntTypeInvalidOximetry) &&
		!(el.state & evntTypeExcluded) &&
		!(el.state & evntTypeAwake)) && (el.pr >= 100.0f) && (el.pr < 110.0f);
}
bool isPRWhileAsleep90to100(OXIMETER_SET el) {
	return	(!(el.state & evntTypeInvalidOximetry) &&
		!(el.state & evntTypeExcluded) &&
		!(el.state & evntTypeAwake)) && (el.pr >= 90.0f) && (el.pr < 100.0f);
}
bool isPRWhileAsleep80to90(OXIMETER_SET el) {
	return	(!(el.state & evntTypeInvalidOximetry) &&
		!(el.state & evntTypeExcluded) &&
		!(el.state & evntTypeAwake)) && (el.pr >= 80.0f) && (el.pr < 90.0f);
}
bool isPRWhileAsleep70to80(OXIMETER_SET el) {
	return	(!(el.state & evntTypeInvalidOximetry) &&
		!(el.state & evntTypeExcluded) &&
		!(el.state & evntTypeAwake)) && (el.pr >= 70.0f) && (el.pr < 80.0f);
}
bool isPRWhileAsleep60to70(OXIMETER_SET el) {
	return	(!(el.state & evntTypeInvalidOximetry) &&
		!(el.state & evntTypeExcluded) &&
		!(el.state & evntTypeAwake)) && (el.pr >= 60.0f) && (el.pr < 70.0f);
}
bool isPRWhileAsleep0to60(OXIMETER_SET el) {
	return	(!(el.state & evntTypeInvalidOximetry) &&
		!(el.state & evntTypeExcluded) &&
		!(el.state & evntTypeAwake)) && (el.pr <= 60.0f);
}

//---PR overall histogram functions
bool isPRAbove120(OXIMETER_SET el) {
	return	(!(el.state & evntTypeInvalidOximetry) &&
		!(el.state & evntTypeExcluded)) && (el.pr >= 120.0f);
}
bool isPR110to120(OXIMETER_SET el) {
	return	(!(el.state & evntTypeInvalidOximetry) &&
		!(el.state & evntTypeExcluded)) && (el.pr >= 110.0f) && (el.pr < 120.0f);
}
bool isPR100to110(OXIMETER_SET el) {
	return	(!(el.state & evntTypeInvalidOximetry) &&
		!(el.state & evntTypeExcluded)) && (el.pr >= 100.0f) && (el.pr < 110.0f);
}
bool isPR90to100(OXIMETER_SET el) {
	return	(!(el.state & evntTypeInvalidOximetry) &&
		!(el.state & evntTypeExcluded)) && (el.pr >= 90.0f) && (el.pr < 100.0f);
}
bool isPR80to90(OXIMETER_SET el) {
	return	(!(el.state & evntTypeInvalidOximetry) &&
		!(el.state & evntTypeExcluded)) && (el.pr >= 80.0f) && (el.pr < 90.0f);
}
bool isPR70to80(OXIMETER_SET el) {
	return	(!(el.state & evntTypeInvalidOximetry) &&
		!(el.state & evntTypeExcluded)) && (el.pr >= 70.0f) && (el.pr < 80.0f);
}
bool isPR60to70(OXIMETER_SET el) {
	return	(!(el.state & evntTypeInvalidOximetry) &&
		!(el.state & evntTypeExcluded)) && (el.pr >= 60.0f) && (el.pr < 70.0f);
}
bool isPR0to60(OXIMETER_SET el) {
	return	(!(el.state & evntTypeInvalidOximetry) &&
		!(el.state & evntTypeExcluded)) && (el.pr <= 60.0f);
}

bool compareOxSpO2(OXIMETER_SET a, OXIMETER_SET b) { return a.spO2 < b.spO2; }
bool compareOxPR(OXIMETER_SET a, OXIMETER_SET b) { return a.pr < b.pr; }

//---Functions to retrieve pos or negative peaks of a certain duration
bool oxPRgreaterAsleep(OXIMETER_SET i, OXIMETER_SET j) {
	if ((i.state & evntTypeExcluded) || (i.state & evntTypeInvalidOximetry) || (i.state & evntTypeAwake)) return false;
	return (i.pr > j.pr);
}
bool oxPRgreaterAwake(OXIMETER_SET i, OXIMETER_SET j) {
	if ((i.state & evntTypeExcluded) || (i.state & evntTypeInvalidOximetry)) return false;
	if (!(i.state & evntTypeAwake)) return false;
	return (i.pr > j.pr);
}
bool oxPRgreater(OXIMETER_SET i, OXIMETER_SET j) { 
	if ((i.state & evntTypeExcluded) || (i.state & evntTypeInvalidOximetry)) return false;
	return (i.pr > j.pr); 
}
	 
bool oxPRsmallerAsleep(OXIMETER_SET i, OXIMETER_SET j) {
	if ((i.state & evntTypeExcluded) || (i.state & evntTypeInvalidOximetry) || (i.state & evntTypeAwake)) return false;
	return (i.pr < j.pr);
}
bool oxPRsmallerAwake(OXIMETER_SET i, OXIMETER_SET j) {
	if ((i.state & evntTypeExcluded) || (i.state & evntTypeInvalidOximetry)) return false;
	if (!(i.state & evntTypeAwake)) return false;
	return (i.pr < j.pr);
}
bool oxPRsmaller(OXIMETER_SET i, OXIMETER_SET j) {
	if ((i.state & evntTypeExcluded) || (i.state & evntTypeInvalidOximetry)) return false;
	return (i.pr < j.pr);
}
	 
bool oxSpO2greaterAsleep(OXIMETER_SET i, OXIMETER_SET j) {
	if ((i.state & evntTypeExcluded) || (i.state & evntTypeInvalidOximetry) || (i.state & evntTypeAwake)) return false;
	return (i.spO2 > j.spO2); 
}
bool oxSpO2greaterAwake(OXIMETER_SET i, OXIMETER_SET j) {
	if ((i.state & evntTypeExcluded) || (i.state & evntTypeInvalidOximetry)) return false;
	if (!(i.state & evntTypeAwake)) return false;
	return (i.spO2 > j.spO2); 
}
bool oxSpO2greater(OXIMETER_SET i, OXIMETER_SET j) {
	if ((i.state & evntTypeExcluded) || (i.state & evntTypeInvalidOximetry)) return false;
	return (i.spO2 > j.spO2); 
}
	 
bool oxSpO2smallerAsleep(OXIMETER_SET i, OXIMETER_SET j) {
	if ((i.state & evntTypeExcluded) || (i.state & evntTypeInvalidOximetry) || (i.state & evntTypeAwake)) return false;
	return (i.spO2 < j.spO2); 
}
bool oxSpO2smallerAwake(OXIMETER_SET i, OXIMETER_SET j) {
	if ((i.state & evntTypeExcluded) || (i.state & evntTypeInvalidOximetry)) return false;
	if (!(i.state & evntTypeAwake)) return false;
	return (i.spO2 < j.spO2); 
}
bool oxSpO2smaller(OXIMETER_SET i, OXIMETER_SET j) {
	if ((i.state & evntTypeExcluded) || (i.state & evntTypeInvalidOximetry)) return false;
	return (i.spO2 < j.spO2); 
}

//***********************************************************

bool isArousal(UINT el)					{	return ((el &	evntTypeArousal) > 0)		; }
bool isImported(UINT el)				{	return ((el &	evntTypeImported) > 0)		; }
bool isManMarker(UINT el)				{	return ((el &	evntTypeManMarker) > 0)		; }
bool isSwallow(UINT el)					{	return ((el &	evntTypeSwallow) > 0)		; }
bool isAH(UINT el)						{	return ((el &	evntTypeAH) > 0)			; }

bool isAHUpper(UINT el)					{ return ((el & evntAHLevelUpper) > 0); }
bool isAHLower(UINT el)					{ return ((el & evntAHLevelLower) > 0); }
bool isAHMulti(UINT el)					{ return ((el & evntAHLevelMulti) > 0); }
bool isAHUndef(UINT el)					{ return ((el & evntAHLevelUndef) > 0); }

bool isMixedUpper(UINT el) { return ((el & evntTypeMixed) && (el & evntAHLevelUpper)); }
bool isMixedLower(UINT el) { return ((el & evntTypeMixed) && (el & evntAHLevelLower)); }
bool isMixedMulti(UINT el) { return ((el & evntTypeMixed) && (el & evntAHLevelMulti)); }
bool isMixedUndef(UINT el) { return ((el & evntTypeMixed) && (el & evntAHLevelUndef)); }

bool isMixedUpperNotAwakeNotExcluded(UINT el) { return ((el & evntTypeMixed) && (el & evntAHLevelUpper) && !(el & evntTypeAwake) && !(el & evntTypeExcluded)); }
bool isMixedLowerNotAwakeNotExcluded(UINT el) { return ((el & evntTypeMixed) && (el & evntAHLevelLower) && !(el & evntTypeAwake) && !(el & evntTypeExcluded)); }
bool isMixedMultiNotAwakeNotExcluded(UINT el) { return ((el & evntTypeMixed) && (el & evntAHLevelMulti) && !(el & evntTypeAwake) && !(el & evntTypeExcluded)); }
bool isMixedUndefNotAwakeNotExcluded(UINT el) { return ((el & evntTypeMixed) && (el & evntAHLevelUndef) && !(el & evntTypeAwake) && !(el & evntTypeExcluded)); }

bool isObstrUpperNotAwakeNotExcluded(UINT el) { return ((el & evntTypeObstr) && (el & evntAHLevelUpper) && !(el & evntTypeAwake) && !(el & evntTypeExcluded)); }
bool isObstrLowerNotAwakeNotExcluded(UINT el) { return ((el & evntTypeObstr) && (el & evntAHLevelLower) && !(el & evntTypeAwake) && !(el & evntTypeExcluded)); }
bool isObstrMultiNotAwakeNotExcluded(UINT el) { return ((el & evntTypeObstr) && (el & evntAHLevelMulti) && !(el & evntTypeAwake) && !(el & evntTypeExcluded)); }
bool isObstrUndefNotAwakeNotExcluded(UINT el) { return ((el & evntTypeObstr) && (el & evntAHLevelUndef) && !(el & evntTypeAwake) && !(el & evntTypeExcluded)); }

bool isRERAUpperNotAwakeNotExcluded(UINT el) { return ((el & evntTypeRERA) && (el & evntAHLevelUpper) && !(el & evntTypeAwake) && !(el & evntTypeExcluded)); }
bool isRERALowerNotAwakeNotExcluded(UINT el) { return ((el & evntTypeRERA) && (el & evntAHLevelLower) && !(el & evntTypeAwake) && !(el & evntTypeExcluded)); }
bool isRERAMultiNotAwakeNotExcluded(UINT el) { return ((el & evntTypeRERA) && (el & evntAHLevelMulti) && !(el & evntTypeAwake) && !(el & evntTypeExcluded)); }
bool isRERAUndefNotAwakeNotExcluded(UINT el) { return ((el & evntTypeRERA) && (el & evntAHLevelUndef) && !(el & evntTypeAwake) && !(el & evntTypeExcluded)); }

bool isHypoObstrUpperNotAwakeNotExcluded(UINT el) { return ((el & evntTypeHypoObstr) && (el & evntAHLevelUpper) && !(el & evntTypeAwake) && !(el & evntTypeExcluded)); }
bool isHypoObstrLowerNotAwakeNotExcluded(UINT el) { return ((el & evntTypeHypoObstr) && (el & evntAHLevelLower) && !(el & evntTypeAwake) && !(el & evntTypeExcluded)); }
bool isHypoObstrMultiNotAwakeNotExcluded(UINT el) { return ((el & evntTypeHypoObstr) && (el & evntAHLevelMulti) && !(el & evntTypeAwake) && !(el & evntTypeExcluded)); }
bool isHypoObstrUndefNotAwakeNotExcluded(UINT el) { return ((el & evntTypeHypoObstr) && (el & evntAHLevelUndef) && !(el & evntTypeAwake) && !(el & evntTypeExcluded)); }

//bool isSnoringUpperNotAwakeNotExcluded(UINT el) { return ((el & evntTypeSnoring) && (el & evntSnoringLevelUpper) && !(el & evntTypeAwake) && !(el & evntTypeExcluded)); }
//bool isSnoringLowerNotAwakeNotExcluded(UINT el) { return ((el & evntTypeSnoring) && (el & evntSnoringLevelLower) && !(el & evntTypeAwake) && !(el & evntTypeExcluded)); }
//bool isSnoringMultiNotAwakeNotExcluded(UINT el) { return ((el & evntTypeSnoring) && (el & evntSnoringLevelMulti) && !(el & evntTypeAwake) && !(el & evntTypeExcluded)); }
//bool isSnoringUndefNotAwakeNotExcluded(UINT el) { return ((el & evntTypeSnoring) && (el & evntSnoringLevelUndef) && !(el & evntTypeAwake) && !(el & evntTypeExcluded)); }

bool isHypo(UINT el) { 
	if (el & evntTypeHypoCentral) return true;
	if (el & evntTypeHypoObstr) return true;
	return false;
}

bool isAsleep(UINT el) { 
	if (el & evntTypeExcluded) return false;
	if (el & evntTypeAwake) return false;
	return true;
}

COleDateTime defDateTime;

extern int doAwakeFromCmdLine;
extern bool compareEventsInTime(EVENT_SORT_VECTOR_ELEMENT e1, EVENT_SORT_VECTOR_ELEMENT e2);
extern bool compareEventsInTimeNew(EVENT_SORT_VECTOR_ELEMENT_NEW e1, EVENT_SORT_VECTOR_ELEMENT_NEW e2);

CEvents::CEvents() :
	eventDefinition(EVNT_DEF_AUTO),
	dirtyFlag(true),
	noEditFlag(false),
	doNotSaveFlag(false),
	visualEventEditingIsActive(false),
	editingViaDialogIsActive(false),
	flags(EVENTS_FLAG_NONE),
	statePointer(-1)
{	
	eventsBeingVisuallyEdited = evntTypeExcluded;

	docP = NULL;

	battData		= NULL;
	cmData			= NULL;
	amData			= NULL;
	bpXYZData		= NULL;
	acData			= NULL;
	torsoAcData		= NULL;
	oxData			= NULL;
	bpData			= NULL;
	cathData		= NULL;
	ag200MicData	= NULL;
	respData		= NULL;
	mMarkerData		= NULL;

	analysisSWVersion = _T("");

	getCriteriaFromRegistry();
	editedDetectionCriteria = detectionCriteria;
	currentDefaultDetectionCriteria = detectionCriteria; 
	
	//---Set defaults
	currentDefaultDetectionCriteria.spO2DropLimit = spO2DropLimit4;
	currentDefaultDetectionCriteria.hypopneaDropLimit = hypopneaDropLimit30;
	
	effectiveStartTime = effectiveStopTime = .0f;
	maxStopTime = .0f;

	spO2DropEventArrayCurrentP = &spO2DropEventArray4;			// 4% is default
	hypoEventArrayCurrentP = &hypoEventArrayDesat4;				// 4% is the default
	arousalEventArrayCurrentP = &arousalEventArrayDesat4;		// 4% is default

	majorVersion = EVENTS_BIN_MAJOR_VERSION;
	minorVersion = EVENTS_BIN_MINOR_VERSION;

    reset();
}

CEvents::~CEvents()
{
	reset();
	saveCriteriaToRegistry();
}

bool CEvents::getNoEditFlag(void)
{
	return noEditFlag;
}

bool CEvents::getDirtyFlag(void)
{
	return dirtyFlag;
}

void CEvents::setDirtyFlag(bool _on)
{
	dirtyFlag = _on;
}

void CEvents::postChangeActions(void)
{
	dirtyFlag = true;
}

void CEvents::preChangeActions(void)
{
}

/*
Description: Resets all events and does a new auto analysis.
Deletes history of manual editing etc.
*/
void CEvents::resetEventsAndReanalyse(void)
{
	detectionCriteria = currentDefaultDetectionCriteria;  // Use current default set of criteria
	editedDetectionCriteria = currentDefaultDetectionCriteria;

	reset(true);			// Reset also state vector (CMemFiles)

	doAnalyse(true);		// Find start and stop

	majorVersion = EVENTS_BIN_MAJOR_VERSION;
	minorVersion = EVENTS_BIN_MINOR_VERSION; 
	noEditFlag = false;
	doNotSaveFlag = false;
	setVisualEventsToClean();
}

bool CEvents::addEvent(UINT _type, float _from, float _to)
{
	vector <FLOAT> *flow = cathData->getFlowVector();
	vector <FLOAT> *flowTime = cathData->getFlowVectorTime();
	vector <FLOAT> *vpGrad = cathData->getPgradientVector();
	vector <FLOAT> *tpGrad = cathData->getPgradientVectorTime();
	vector <FLOAT> *vpoes = cathData->getPOESEnvVector();
	vector <FLOAT> *poesTime = cathData->getPOESEnvVectorTime();
	vector <FLOAT> *vpph = cathData->getPPHEnvVector();
	vector <FLOAT> *pphTime = cathData->getPPHEnvVectorTime();

	switch (_type) {
	case evntTypeMixed:
	{
		short bp = bpData ? bpData->getPos(_from, _to) : posUndefined;
		CMixedEvnt *eP = new CMixedEvnt(_from, _to, startRecordingClockTime, bp, manualDetection);
		gradientDataToEvent((CEvnt *)eP, vpGrad, tpGrad, _from, _to);
		flowDataToEvent((CEvnt *)eP, flow, flowTime, _from, _to); 
		poesEnvDataToEvent((CEvnt *)eP, vpoes, poesTime, _from, _to);
		pphEnvDataToEvent((CEvnt *)eP, vpph, pphTime, _from, _to);
		eP->doLevelAnalysis();
		mixedEventArray.Add(eP);
		sortAndOrganiseMixed();
		fillStartsAndStopsVector(evntTypeMixed);
	}
	break;
	case evntTypeCentral:
	{
		short bp = bpData ? bpData->getPos(_from, _to) : posUndefined;
		CCentralEvnt *eP = new CCentralEvnt(_from,_to, startRecordingClockTime, bp, manualDetection); 
		gradientDataToEvent((CEvnt *)eP, vpGrad, tpGrad, _from, _to);
		flowDataToEvent((CEvnt *)eP, flow, flowTime, _from, _to);
		poesEnvDataToEvent((CEvnt *)eP, vpoes, poesTime, _from, _to);
		pphEnvDataToEvent((CEvnt *)eP, vpph, pphTime, _from, _to);
		centralEventArray.Add(eP);
		sortAndOrganiseCentral();
		fillStartsAndStopsVector(evntTypeCentral);
	}
	break;
	case evntTypeObstr:
	{
		short bp = bpData ? bpData->getPos(_from, _to) : posUndefined;
		CObstrEvnt *eP = new CObstrEvnt(_from, _to, startRecordingClockTime, bp, manualDetection);
		gradientDataToEvent((CEvnt *)eP, vpGrad, tpGrad, _from, _to);
		flowDataToEvent((CEvnt *)eP, flow, flowTime, _from, _to);
		poesEnvDataToEvent((CEvnt *)eP, vpoes, poesTime, _from, _to);
		pphEnvDataToEvent((CEvnt *)eP, vpph, pphTime, _from, _to);
		eP->doLevelAnalysis();
		obstrEventArray.Add(eP);
		sortAndOrganiseObstr();
		fillStartsAndStopsVector(evntTypeObstr);
	}
	break;
	case evntTypeHypoCentral:
	{
		short bp = bpData ? bpData->getPos(_from, _to) : posUndefined;
		CHypoEvnt *eP = new CHypoEvnt(_from, _to, startRecordingClockTime, bp, manualDetection);
		gradientDataToEvent((CEvnt *)eP, vpGrad, tpGrad, _from, _to);
		flowDataToEvent((CEvnt *)eP, flow, flowTime, _from, _to);
		poesEnvDataToEvent((CEvnt *)eP, vpoes, poesTime, _from, _to);
		pphEnvDataToEvent((CEvnt *)eP, vpph, pphTime, _from, _to);
		eP->setHypoType(hypoTypeCentral);
		hypoEventArrayCurrentP->Add(eP);
		sortAndOrganiseHypo(detectionCriteria.spO2DropLimit);
		fillStartsAndStopsVector(evntTypeHypoCentral);
	}
	break;
	case evntTypeHypoObstr:
	{
		short bp = bpData ? bpData->getPos(_from, _to) : posUndefined;
		CHypoEvnt *eP = new CHypoEvnt(_from, _to, startRecordingClockTime, bp, manualDetection);
		gradientDataToEvent((CEvnt *)eP, vpGrad, tpGrad, _from, _to);
		flowDataToEvent((CEvnt *)eP, flow, flowTime, _from, _to);
		poesEnvDataToEvent((CEvnt *)eP, vpoes, poesTime, _from, _to);
		pphEnvDataToEvent((CEvnt *)eP, vpph, pphTime, _from, _to);
		eP->setHypoType(hypoTypeObstr);
		eP->doLevelAnalysis();
		hypoEventArrayCurrentP->Add(eP);
		sortAndOrganiseHypo(detectionCriteria.spO2DropLimit);
		fillStartsAndStopsVector(evntTypeHypoObstr);
	}
	break;
	case evntTypeRERA:
	{
		short bp = bpData ? bpData->getPos(_from, _to) : posUndefined;
		CArousalEvnt *aP = new CArousalEvnt(_from, _to, startRecordingClockTime, bp, manualDetection);
		arousalEventArrayCurrentP->Add(aP);

		float reraFrom = _from - APNEA_DEFAULT_DURATION;
		float reraTo = (_from + _to ) / 2.0f;
		short bp2 = bpData ? bpData->getPos(reraFrom, reraTo) : posUndefined;
		CRERAEvnt *eP = new CRERAEvnt(reraFrom, reraTo, startRecordingClockTime, bp2, manualDetection);
		gradientDataToEvent((CEvnt *)eP, vpGrad, tpGrad, reraFrom, reraTo);
		flowDataToEvent((CEvnt *)eP, flow, flowTime, reraFrom, reraTo);
		poesEnvDataToEvent((CEvnt *)eP, vpoes, poesTime, _from, _to);
		pphEnvDataToEvent((CEvnt *)eP, vpph, pphTime, _from, _to);
		eP->doLevelAnalysis();
		aP->setRERAEvnt(eP);
		
		sortAndOrganiseArousal(detectionCriteria.spO2DropLimit);
		fillStartsAndStopsVector(evntTypeArousal);
	}
	break;
	case evntTypeArousal:
	{
		short bp = bpData ? bpData->getPos(_from, _to) : posUndefined;
		CArousalEvnt *eP = new CArousalEvnt(_from, _to, startRecordingClockTime, bp, manualDetection);
		arousalEventArrayCurrentP->Add(eP);
		sortAndOrganiseArousal(detectionCriteria.spO2DropLimit);
		fillStartsAndStopsVector(evntTypeArousal);
	}
	break;
	case evntTypeSpO2Drop:
	{
		short bp = bpData ? bpData->getPos(_from, _to) : posUndefined;
		float dropSize = getFloatSpO2DropLimit();
		
		//---Find from and to SpO2
		float d0 = .0f;
		float d1 = .0f;
		if (oxData) {
			d0 = oxData->getSpO2At(_from);
			d1 = oxData->getSpO2At(_to);
		}
		CSpO2DropEvnt *eP = new CSpO2DropEvnt(detectionCriteria.spO2DropLimit,dropSize,d0,d1,_from, _to, startRecordingClockTime, bp, manualDetection);

		spO2DropEventArrayCurrentP->Add(eP);
		sortAndOrganiseSpO2Drop(detectionCriteria.spO2DropLimit);
		fillStartsAndStopsVector(evntTypeSpO2Drop);
	}
	break;
	case evntTypeSnoring:
	{
		short bp = bpData ? bpData->getPos(_from, _to) : posUndefined;
		CSnoringEvnt *eP = new CSnoringEvnt(_from, _to, startRecordingClockTime, bp, manualDetection);
		gradientDataToEvent((CEvnt *)eP, vpGrad, tpGrad, _from, _to);
		flowDataToEvent((CEvnt *)eP, flow, flowTime, _from, _to);
		bodyDataToEvent(eP, bpData->getBpVector(),
			bpData->getTimeaxisVector(),
			_from, _to);
		contactMicDataToEvent(eP, bpData->getBpVector(),
			bpData->getTimeaxisVector(),
			_from, _to);
		airMicDataToEvent(eP, amData->getAm1Vector(),
			amData->getAm2Vector(),
			amData->getAm3Vector(),
			amData->getAm4Vector(),
			amData->getAmSumVector(),
			amData->getTimeaxisVector(),
			_from, _to);
		poesEnvDataToEvent((CEvnt *)eP, vpoes, poesTime, _from, _to);
		pphEnvDataToEvent((CEvnt *)eP, vpph, pphTime, _from, _to);
		eP->sumUpTimeInPositions();
		eP->sumUpTimeAtLevels();
		eP->doLevelAnalysis();
		snoringEventArray.Add(eP);
		sortAndOrganiseSnoring();
		fillStartsAndStopsVector(evntTypeSnoring);
	}
	break;
	case evntTypeAwake:
	{
		short bp = bpData ? bpData->getPos(_from, _to) : posUndefined;
		CAwakeEvnt *eP = new CAwakeEvnt(_from, _to, startRecordingClockTime, bp, manualDetection);
		awakeEventArray.Add(eP);
		sortAndOrganiseAwake();
		fillStartsAndStopsVector(evntTypeAwake);
	}
	break;
	case evntTypeImported:
	{
		short bp = bpData ? bpData->getPos(_from, _to) : posUndefined;
		CImportedEvnt *eP = new CImportedEvnt(_from, _to, startRecordingClockTime, bp, manualDetection);
		importedEventArray.Add(eP);
		sortAndOrganiseImported();
		fillStartsAndStopsVector(evntTypeImported);
	}
	break;
	default:
		break;
	}
	postEditEventAnalysis(_type);
	return true;
}

bool CEvents::addImportedEvent(COleDateTime _fromTOD, COleDateTime _toTOD)
{
	COleDateTimeSpan length = _toTOD - _fromTOD;
	if (length.GetTotalSeconds() <= .0f) return false;

	float from, to;
	COleDateTimeSpan span0 = _fromTOD - startRecordingClockTime;
	from = (float) span0.GetTotalSeconds();

	COleDateTimeSpan span1 = _toTOD - startRecordingClockTime;
	to = (float)span1.GetTotalSeconds();

	short bp = bpData ? bpData->getPos(from, to) : posUndefined;
	CImportedEvnt *eP = new CImportedEvnt(from, to, startRecordingClockTime, bp, manualDetection);
	
	importedEventArray.Add(eP);

	sortAndOrganiseImported();
	return true;
}

bool CEvents::addImportedEvent(COleDateTime _fromTOD, float _length)
{
	if (_length <= .0f) return false;

	float from, to;
	COleDateTimeSpan span0 = _fromTOD - startRecordingClockTime;
	from = (float)span0.GetTotalSeconds();

	to = from + _length;

	short bp = bpData ? bpData->getPos(from, to) : posUndefined;
	CImportedEvnt *eP = new CImportedEvnt(from, to, startRecordingClockTime, bp, manualDetection);

	importedEventArray.Add(eP);

	sortAndOrganiseImported();
	fillStartsAndStopsVector(evntTypeImported);
	postEditEventAnalysis(evntTypeImported);
	return true;
}

bool CEvents::resetImportedEvents(void)
{
	int num = importedEventArray.GetCount();
	for (int i = num - 1; i >= 0; i--) {
		CImportedEvnt *eP = importedEventArray.GetAt(i);
		delete eP;
	}
	importedEventArray.RemoveAll();
	return true;
}

void CEvents::addNewState(void)
{
	//---Delete all above statePointer
	if (0 != stateVector.size()) {
		vector <CMemFile *>::iterator it = stateVector.begin();
		it += statePointer;
		it++;
		for (; it < stateVector.end(); ++it) {
			delete *it;
		}
		stateVector.resize(statePointer + 1);
	}

	CMemFile *mfP = new CMemFile;
	CArchive archive(mfP, CArchive::store);  // Ignore return
	CEvnt::serializeVectorUint(archive, editEventTimelineVector.begin(), editEventTimelineVector.end(), &editEventTimelineVector);
	archive.Close();
	stateVector.push_back(mfP);
	statePointer = stateVector.size() - 1;
}

/*
Description: Gets the statistics for the catheter channels, excluding exclucded and awake periods.
The following sequence:
			T1,
			T0,
			PPH,
			POES,
			Resp effort frequency
*/
void CEvents::getCatheterStatistics(float _editTimeWindowStart, float _editTimeWindowStop,
	CStringArray *_title,
	CStringArray *_unit,
	CStringArray *_max,
	CStringArray *_min,
	CStringArray *_avg,
	CStringArray *_median,
	CStringArray *_span,
	CStringArray *_stdDev)
{
	if (!cathData) return;

	vector <FLOAT> *vpoes = cathData->getPOESVector();
	vector <FLOAT> *vpph = cathData->getPPHVector();
	vector <FLOAT> *t0 = cathData->getT0Vector();
	vector <FLOAT> *t1 = cathData->getT1Vector();
	vector <FLOAT> *cT = cathData->getTimeaxisVector();
	vector <FLOAT> *f = cathData->getRespFrqVector();
	vector <FLOAT> *fT = cathData->getRespFrqVectorTime();

	ASSERT(fT->size() == f->size());
	ASSERT(cT->size() == vpoes->size());
	ASSERT(cT->size() == vpph->size());
	ASSERT(cT->size() == t0->size());
	ASSERT(cT->size() == t1->size());

	ASSERT(_editTimeWindowStop >= _editTimeWindowStart);

	CString ma, mi, avg, med, span,title,unit,stdDev;
	getStat(_editTimeWindowStart,_editTimeWindowStop, &title,&unit,t1, cT, &ma, &mi, &avg, &med, &span,&stdDev);
	_max->Add(ma);
	_min->Add(mi);
	_avg->Add(avg);
	_median->Add(med);
	_span->Add(span);
	_title->Add(title);
	_unit->Add(unit);
	_stdDev->Add(stdDev);

	getStat(_editTimeWindowStart, _editTimeWindowStop, &title,&unit,t0, cT, &ma, &mi, &avg, &med, &span,&stdDev);
	_max->Add(ma);
	_min->Add(mi);
	_avg->Add(avg);
	_median->Add(med);
	_span->Add(span);
	_title->Add(title);
	_unit->Add(unit);
	_stdDev->Add(stdDev);

	getStat(_editTimeWindowStart, _editTimeWindowStop, &title, &unit, vpph, cT, &ma, &mi, &avg, &med, &span,&stdDev);
	_max->Add(ma);
	_min->Add(mi);
	_avg->Add(avg);
	_median->Add(med);
	_span->Add(span);
	_title->Add(title);
	_unit->Add(unit);
	_stdDev->Add(stdDev);

	getStat(_editTimeWindowStart, _editTimeWindowStop, &title,&unit,vpoes, cT, &ma, &mi, &avg, &med, &span,&stdDev);
	_max->Add(ma);
	_min->Add(mi);
	_avg->Add(avg);
	_median->Add(med);
	_span->Add(span);
	_title->Add(title);
	_unit->Add(unit);
	_stdDev->Add(stdDev);

	getStat(_editTimeWindowStart, _editTimeWindowStop, &title,&unit,f, fT, &ma, &mi, &avg, &med, &span,&stdDev);
	_max->Add(ma);
	_min->Add(mi);
	_avg->Add(avg);
	_median->Add(med);
	_span->Add(span);
	_title->Add(title);
	_unit->Add(unit);
	_stdDev->Add(stdDev);
}

/*
Description:
	Computes stat parameters from the time window, excluding excluded and awake poeriods
*/
void CEvents::getStat(float _start,float _stop, CString *_title,CString *_unit,vector <FLOAT> *_v, vector <FLOAT> *_t, CString *_max, CString *_min, 
	CString *_avg, CString *_median, CString *_span, CString *_stdDev,vector <BOOL> *_pd /* = NULL */)
{
	vector <FLOAT>::iterator it0 = lower_bound(_t->begin(), _t->end(), _start);
	vector <FLOAT>::iterator it1 = lower_bound(_t->begin(), _t->end(), _stop);
	int index0 = it0 >= _t->end() ? _t->size() - 1 : distance(_t->begin(), it0);
	int index1 = it1 >= _t->end() ? _t->size() - 1 : distance(_t->begin(), it1);

	vector <FLOAT>::iterator v0 = _v->begin();
	v0 += index0;
	vector <FLOAT>::iterator v1 = _v->begin();
	v1 += index1;

	vector <FLOAT>::iterator t = _t->begin();
	t += index0;

	vector <FLOAT>::iterator v = v0;

	float sum = .0f;
	int cnt = 0;
	vector <FLOAT> tempV;
	if (!_pd) {	// No penDown
		for (; v < v1; ++v, ++t) {
			if ((!getIsAwake(*t) && !getIsExcluded(*t))) {
				cnt++;
				sum += *v;
				tempV.push_back(*v);
			}
		}
	}
	else {		// penDown
		vector <BOOL>::iterator pdi = _pd->begin();
		pdi += index0;
		for (; v < v1; ++v, ++t, ++pdi) {
			if ((!getIsAwake(*t) && !getIsExcluded(*t) && *pdi)) {
				cnt++;
				sum += *v;
				tempV.push_back(*v);
			}
		}
	}
	float avg = (float)for_each(tempV.begin(),tempV.end(), Average());
	float stdDev = (float)for_each(tempV.begin(), tempV.end(), standardDeviation(avg));

	if (0 == tempV.size()) {
		*_max = _T(" ");
		*_min = _T(" ");
		*_avg = _T(" ");
		*_median = _T(" ");
		*_span = _T(" ");
		return;
	}

	sort(tempV.begin(), tempV.end());
	float min = tempV.at(0);
	float max = tempV.at(tempV.size() - 1);
	float span = max - min;
	float median = .0f;
	int num = tempV.size();
	if ((num % 2) == 0) {
		num >>= 1;
		median = (tempV.at(num) + tempV.at(num + 1)) / 2;
	}
	else {
		num >>= 1;
		median = tempV.at(num);
	}
	CString fmtString = getFmtString2ValidDigits(stdDev);
	_min->Format(fmtString, min);
	_max->Format(fmtString, max);
	_avg->Format(fmtString, avg);
	_median->Format(fmtString, median);
	_span->Format(fmtString, span);
	_stdDev->Format(fmtString, stdDev);
	*_title = _T("++++");
	*_unit = _T("Unit");
}

void CEvents::getStatisticsForADataVector(float _start, float _stop, vector<FLOAT>* _dataVector, 
	vector<FLOAT>* _timeVector, CString * _max, CString * _min, CString * _avg, CString * _median, 
	CString * _span, CString * _stdDev,
	vector <BOOL> *_penDownVector)
{
	CString dummyTitle, dummyUnit;
	getStat(_start, _stop, &dummyTitle, &dummyUnit, _dataVector, _timeVector, _max, _min,
		_avg, _median, _span, _stdDev, _penDownVector);
}

CString CEvents::getFmtString2ValidDigits(float _stdDev)
{
	CString fmt;
	float fn = fabs(_stdDev);
	if (fn == .0f) return _T("%.3f");
		
	float powOf10 = log10(fn);
	if (powOf10 > 1.0f) fmt = _T("%.0f");		// No exponents
	else if (powOf10 > .0f) fmt = _T("%.1f");	// 1 decimal
	else fmt.Format(_T("%%.%df"), -(int)powOf10 + 2);
	return fmt;
}

/*
Description: Gets the statistics for the oximeter channels, excluding excluded and awake periods.
The following sequence:
	SpO2
	PR
*/
void CEvents::getOximeterStatistics(float _editTimeWindowStart, float _editTimeWindowStop,
	CStringArray *_title,
	CStringArray *_unit,
	CStringArray *_max,
	CStringArray *_min,
	CStringArray *_avg,
	CStringArray *_median,
	CStringArray *_span,
	CStringArray *_stdDev)
{
	vector <FLOAT> *vspo2 = oxData->getSpO2Vector();
	vector <FLOAT> *vpr = oxData->getPulseRateVector();
	vector <FLOAT> *t = oxData->getTimeaxisVector();
	vector <BOOL> *pd = oxData->getPenDownVector();

	ASSERT(t->size() == vspo2->size());
	ASSERT(t->size() == vpr->size());
	if (pd)	
		ASSERT(t->size() == pd->size());

	ASSERT(_editTimeWindowStop >= _editTimeWindowStart);

	CString ma, mi, avg, med, span, title,unit,stdDev;
	getStat(_editTimeWindowStart, _editTimeWindowStop, &title,&unit,vspo2, t, &ma, &mi, &avg, &med, &span,&stdDev,pd);
	_max->Add(ma);
	_min->Add(mi);
	_avg->Add(avg);
	_median->Add(med);
	_span->Add(span);
	_title->Add(title);
	_unit->Add(unit);
	_stdDev->Add(stdDev);

	getStat(_editTimeWindowStart, _editTimeWindowStop, &title, &unit,vpr, t, &ma, &mi, &avg, &med, &span,&stdDev,pd);
	_max->Add(ma);
	_min->Add(mi);
	_avg->Add(avg);
	_median->Add(med);
	_span->Add(span);
	_title->Add(title);
	_unit->Add(unit);
	_stdDev->Add(stdDev);
}
void CEvents::setDoc(CDocument *_docP)
{
	docP = _docP;
}

bool CEvents::readResults(CString _dataPath,CString _fileID)
{
	CString filePath = _dataPath;
	filePath += _T("\\");
	filePath += eventsDataFile;

	CFile theFile;
	int OK = theFile.Open(filePath, CFile::modeRead);
	if (!OK) return false;

	CArchive archive(&theFile, CArchive::load);
	bool idOK = Serialize(archive,_fileID);
	archive.Close();
	theFile.Close();

	postChangeActions();

	return idOK;
}

CString CEvents::getAnalysisSWVersion(void)
{
	return analysisSWVersion;
}

bool CEvents::saveResults(CString _dataPath,CString _fileID)
{

	if (doNotSaveFlag) return false;

	CString filePath = _dataPath;
	filePath += _T("\\");
	filePath += eventsDataFile;

	CFile theFile;
	int OK = theFile.Open(filePath, CFile::modeCreate | CFile::modeWrite);
	if (!OK) return false;

	CArchive archive(&theFile, CArchive::store);	// Ignore return
	Serialize(archive,_fileID);
	archive.Close();
	theFile.Close();

	return true;
}

void CEvents::serializeBodyPosResults(CArchive& ar, BODY_POS_RESULTS *_bp, int _majorVersion, int _minorVersion)
{
	if (ar.IsStoring()) {
		for (int i = 0; i < posCount; i++) ar << _bp->central[i];
		for (int i = 0; i < posCount; i++) ar << _bp->spO2[i];
		for (int i = 0; i < posCount; i++) ar << _bp->centralHyp[i];
		for (int i = 0; i < posCount; i++) ar << _bp->time[i];
		for (int i = 0; i < posCount; i++) ar << _bp->timeSleeping[i];		// New in events file version 26
		for (int i = 0; i < posCount; i++) ar << _bp->timeAwake[i];		// New in events file version 27
		for (int i = 0; i < posCount; i++) ar << _bp->arousal[i];
		for (int i = 0; i < posCount; i++) ar << _bp->snoringTime[i];
		for (int i = 0; i < posCount; i++) ar << _bp->percentTime[i];
		for (int i = 0; i < posCount; i++) ar << _bp->percentTimeSleeping[i]; // New in events file version 26
		for (int i = 0; i < posCount; i++) ar << _bp->percentTimeAwake[i]; // New in events file version 27
		for (int i = 0; i < posCount; i++) {
			ar << _bp->RERAindex[i];	// New in events file version 26
			ar << _bp->OAHindex[i];		// New in events file version 26
			ar << _bp->CAHindex[i];		// New in events file version 26
			ar << _bp->ODindex[i];		// New in events file version 26

			ar << _bp->OAindex[i];		// New in events file version 36
			ar << _bp->OHindex[i];		// New in events file version 36
			ar << _bp->RDindex[i];		// New in events file version 36
			ar << _bp->AHindex[i];		// New in events file version 36
		}

		for (int i = 0; i < posCount; i++) {
			for (int j = 0; j < levelTypeCount; j++) ar << _bp->obstr[i][j];
		}
		for (int i = 0; i < posCount; i++) {
			for (int j = 0; j < levelTypeCount; j++) ar << _bp->mix[i][j];
		}
		for (int i = 0; i < posCount; i++) {
			for (int j = 0; j < levelTypeCount; j++) ar << _bp->obstrHyp[i][j];
		}
		for (int i = 0; i < posCount; i++) {
			for (int j = 0; j < levelTypeCount; j++) ar << _bp->RERA[i][j];
		}

		for (int i = 0; i < levelTypeCount; i++) ar << _bp->percentObstrSupine[i];
		for (int i = 0; i < levelTypeCount; i++) ar << _bp->percentObstrNonSupine[i];
		for (int i = 0; i < levelTypeCount; i++) ar << _bp->percentMixSupine[i];
		for (int i = 0; i < levelTypeCount; i++) ar << _bp->percentMixNonSupine[i];
		for (int i = 0; i < levelTypeCount; i++) ar << _bp->percentObstrHypoSupine[i];
		for (int i = 0; i < levelTypeCount; i++) ar << _bp->percentObstrHypoNonSupine[i];
		for (int i = 0; i < levelTypeCount; i++) ar << _bp->percentRERASupine[i];
		for (int i = 0; i < levelTypeCount; i++) ar << _bp->percentRERANonSupine[i];

		for (int i = 0; i < levelTypeCount; i++) {
			ar << _bp->RERAindexVSlevel[i];			// New in events file version 36
			ar << _bp->OAHindexVSlevel[i];			// New in events file version 36
			ar << _bp->OAindexVSlevel[i];			// New in events file version 36
			ar << _bp->OHindexVSlevel[i];			// New in events file version 36
			ar << _bp->RDindexVSlevel[i];			// New in events file version 36
			ar << _bp->AHindexVSlevel[i];			// New in events file version 36
		}

		ar << _bp->percentCentralSupine;
		ar << _bp->percentCentralNonSupine;
		ar << _bp->percentCentralHypoSupine;
		ar << _bp->percentCentralHypoNonSupine;
		ar << _bp->percentSpO2Supine;
		ar << _bp->percentSpO2NonSupine;
		ar << _bp->percentArousalSupine;
		ar << _bp->percentArousalNonSupine;
	}
	else {  // Reading

		if (_majorVersion < 36) {
			serializeBodyPosResultsPre36(ar,_bp,_majorVersion,_minorVersion);
			return;
		}

		for (int i = 0; i < posCount; i++) ar >> _bp->central[i];
		for (int i = 0; i < posCount; i++) ar >> _bp->spO2[i];
		for (int i = 0; i < posCount; i++) ar >> _bp->centralHyp[i];
		for (int i = 0; i < posCount; i++) ar >> _bp->time[i];
		for (int i = 0; i < posCount; i++) ar >> _bp->timeSleeping[i];			// New in events file version 26
		for (int i = 0; i < posCount; i++) ar >> _bp->timeAwake[i];				// New in events file version 27
		for (int i = 0; i < posCount; i++) ar >> _bp->arousal[i];
		for (int i = 0; i < posCount; i++) ar >> _bp->snoringTime[i];
		for (int i = 0; i < posCount; i++) ar >> _bp->percentTime[i];
		for (int i = 0; i < posCount; i++) ar >> _bp->percentTimeSleeping[i];	// New in events file version 26
		for (int i = 0; i < posCount; i++) ar >> _bp->percentTimeAwake[i];	// New in events file version 27
		for (int i = 0; i < posCount; i++) {
			ar >> _bp->RERAindex[i];	// New in events file version 26
			ar >> _bp->OAHindex[i];		// New in events file version 26
			ar >> _bp->CAHindex[i];		// New in events file version 26
			ar >> _bp->ODindex[i];		// New in events file version 26

			ar >> _bp->OAindex[i];		// New in events file version 36
			ar >> _bp->OHindex[i];		// New in events file version 36
			ar >> _bp->RDindex[i];		// New in events file version 36
			ar >> _bp->AHindex[i];		// New in events file version 36
		}

		for (int i = 0; i < posCount; i++) {
			for (int j = 0; j < levelTypeCount; j++) ar >> _bp->obstr[i][j];
		}
		for (int i = 0; i < posCount; i++) {
			for (int j = 0; j < levelTypeCount; j++) ar >> _bp->mix[i][j];
		}
		for (int i = 0; i < posCount; i++) {
			for (int j = 0; j < levelTypeCount; j++) ar >> _bp->obstrHyp[i][j];
		}
		for (int i = 0; i < posCount; i++) {
			for (int j = 0; j < levelTypeCount; j++) ar >> _bp->RERA[i][j];
		}

		for (int i = 0; i < levelTypeCount; i++) ar >> _bp->percentObstrSupine[i];
		for (int i = 0; i < levelTypeCount; i++) ar >> _bp->percentObstrNonSupine[i];
		for (int i = 0; i < levelTypeCount; i++) ar >> _bp->percentMixSupine[i];
		for (int i = 0; i < levelTypeCount; i++) ar >> _bp->percentMixNonSupine[i];
		for (int i = 0; i < levelTypeCount; i++) ar >> _bp->percentObstrHypoSupine[i];
		for (int i = 0; i < levelTypeCount; i++) ar >> _bp->percentObstrHypoNonSupine[i];
		for (int i = 0; i < levelTypeCount; i++) ar >> _bp->percentRERASupine[i];
		for (int i = 0; i < levelTypeCount; i++) ar >> _bp->percentRERANonSupine[i];

		for (int i = 0; i < levelTypeCount; i++) {
			ar >> _bp->RERAindexVSlevel[i];			// New in events file version 36
			ar >> _bp->OAHindexVSlevel[i];			// New in events file version 36
			ar >> _bp->OAindexVSlevel[i];			// New in events file version 36
			ar >> _bp->OHindexVSlevel[i];			// New in events file version 36
			ar >> _bp->RDindexVSlevel[i];			// New in events file version 36
			ar >> _bp->AHindexVSlevel[i];			// New in events file version 36
		}

		ar >> _bp->percentCentralSupine;
		ar >> _bp->percentCentralNonSupine;
		ar >> _bp->percentCentralHypoSupine;
		ar >> _bp->percentCentralHypoNonSupine;
		ar >> _bp->percentSpO2Supine;
		ar >> _bp->percentSpO2NonSupine;
		ar >> _bp->percentArousalSupine;
		ar >> _bp->percentArousalNonSupine;
	}
}

void CEvents::serializeBodyPosResultsPre36(CArchive& ar, BODY_POS_RESULTS *_bp, int _majorVersion, int _minorVersion)
{
	if (ar.IsStoring()) {
		for (int i = 0 ; i < posCount ; i++) ar << _bp->central[i];
		for (int i = 0 ; i < posCount ; i++) ar << _bp->spO2[i];
		for (int i = 0 ; i < posCount ; i++) ar << _bp->centralHyp[i];
		for (int i = 0 ; i < posCount ; i++) ar << _bp->time[i];
		for (int i = 0 ; i < posCount ; i++) ar << _bp->timeSleeping[i];		// New in events file version 26
		for (int i = 0 ; i < posCount ; i++) ar << _bp->timeAwake[i];		// New in events file version 27
		for (int i = 0 ; i < posCount ; i++) ar << _bp->arousal[i];
		for (int i = 0 ; i < posCount ; i++) ar << _bp->snoringTime[i];
		for (int i = 0 ; i < posCount ; i++) ar << _bp->percentTime[i];
		for (int i = 0 ; i < posCount ; i++) ar << _bp->percentTimeSleeping[i]; // New in events file version 26
		for (int i = 0 ; i < posCount ; i++) ar << _bp->percentTimeAwake[i]; // New in events file version 27
		for (int i = 0; i < posCount; i++) {	
			ar << _bp->RERAindex[i];	// New in events file version 26
			ar << _bp->OAHindex[i];		// New in events file version 26
			ar << _bp->CAHindex[i];		// New in events file version 26
			ar << _bp->ODindex[i];		// New in events file version 26
		}

		for (int i = 0; i < posCount; i++) {
			for (int j = 0; j < levelTypeCount; j++) ar << _bp->obstr[i][j];
		}
		for (int i = 0; i < posCount; i++) {
			for (int j = 0; j < levelTypeCount; j++) ar << _bp->mix[i][j];
		}
		for (int i = 0; i < posCount; i++) {
			for (int j = 0; j < levelTypeCount; j++) ar << _bp->obstrHyp[i][j];
		}
		for (int i = 0; i < posCount; i++) {
			for (int j = 0; j < levelTypeCount; j++) ar << _bp->RERA[i][j];
		}

		for (int i = 0 ; i < levelTypeCount ; i++) ar << _bp->percentObstrSupine[i];
		for (int i = 0 ; i < levelTypeCount ; i++) ar << _bp->percentObstrNonSupine[i];
		for (int i = 0 ; i < levelTypeCount ; i++) ar << _bp->percentMixSupine[i];
		for (int i = 0 ; i < levelTypeCount ; i++) ar << _bp->percentMixNonSupine[i];
		for (int i = 0 ; i < levelTypeCount ; i++) ar << _bp->percentObstrHypoSupine[i];
		for (int i = 0 ; i < levelTypeCount ; i++) ar << _bp->percentObstrHypoNonSupine[i];
		for (int i = 0 ; i < levelTypeCount ; i++) ar << _bp->percentRERASupine[i];
		for (int i = 0 ; i < levelTypeCount ; i++) ar << _bp->percentRERANonSupine[i];

		ar << _bp->percentCentralSupine;
		ar << _bp->percentCentralNonSupine;
		ar << _bp->percentCentralHypoSupine;
		ar << _bp->percentCentralHypoNonSupine;
		ar << _bp->percentSpO2Supine;
		ar << _bp->percentSpO2NonSupine;
		ar << _bp->percentArousalSupine;
		ar << _bp->percentArousalNonSupine;
	}
	else {
		for (int i = 0; i < posCount; i++) ar >> _bp->central[i];
		for (int i = 0; i < posCount; i++) ar >> _bp->spO2[i];
		for (int i = 0; i < posCount; i++) ar >> _bp->centralHyp[i];
		for (int i = 0; i < posCount; i++) ar >> _bp->time[i];
		for (int i = 0; i < posCount; i++) ar >> _bp->timeSleeping[i];			// New in events file version 26
		for (int i = 0; i < posCount; i++) ar >> _bp->timeAwake[i];				// New in events file version 27
		for (int i = 0; i < posCount; i++) ar >> _bp->arousal[i];
		for (int i = 0; i < posCount; i++) ar >> _bp->snoringTime[i];
		for (int i = 0; i < posCount; i++) ar >> _bp->percentTime[i];
		for (int i = 0; i < posCount; i++) ar >> _bp->percentTimeSleeping[i];	// New in events file version 26
		for (int i = 0; i < posCount; i++) ar >> _bp->percentTimeAwake[i];	// New in events file version 27
		for (int i = 0; i < posCount; i++) {
			ar >> _bp->RERAindex[i];	// New in events file version 26
			ar >> _bp->OAHindex[i];		// New in events file version 26
			ar >> _bp->CAHindex[i];		// New in events file version 26
			ar >> _bp->ODindex[i];		// New in events file version 26

			_bp->OAindex[i] = .0f;		// New in events file version 36
			_bp->OHindex[i] = .0f;		// New in events file version 36
			_bp->RDindex[i] = .0f;		// New in events file version 36
			_bp->AHindex[i] = .0f;		// New in events file version 36
		}

		for (int i = 0; i < posCount; i++) {
			for (int j = 0; j < levelTypeCount; j++) ar >> _bp->obstr[i][j];
		}
		for (int i = 0; i < posCount; i++) {
			for (int j = 0; j < levelTypeCount; j++) ar >> _bp->mix[i][j];
		}
		for (int i = 0; i < posCount; i++) {
			for (int j = 0; j < levelTypeCount; j++) ar >> _bp->obstrHyp[i][j];
		}
		for (int i = 0; i < posCount; i++) {
			for (int j = 0; j < levelTypeCount; j++) ar >> _bp->RERA[i][j];
		}

		for (int i = 0; i < levelTypeCount; i++) ar >> _bp->percentObstrSupine[i];
		for (int i = 0; i < levelTypeCount; i++) ar >> _bp->percentObstrNonSupine[i];
		for (int i = 0; i < levelTypeCount; i++) ar >> _bp->percentMixSupine[i];
		for (int i = 0; i < levelTypeCount; i++) ar >> _bp->percentMixNonSupine[i];
		for (int i = 0; i < levelTypeCount; i++) ar >> _bp->percentObstrHypoSupine[i];
		for (int i = 0; i < levelTypeCount; i++) ar >> _bp->percentObstrHypoNonSupine[i];
		for (int i = 0; i < levelTypeCount; i++) ar >> _bp->percentRERASupine[i];
		for (int i = 0; i < levelTypeCount; i++) ar >> _bp->percentRERANonSupine[i];

		for (int i = 0; i < levelTypeCount; i++) {
			_bp->RERAindexVSlevel[i]= .0f;			// New in events file version 36
			_bp->OAHindexVSlevel[i] = .0f;			// New in events file version 36
			_bp->OAindexVSlevel[i] 	= .0f;		// New in events file version 36
			_bp->OHindexVSlevel[i] 	= .0f;		// New in events file version 36
			_bp->RDindexVSlevel[i] 	= .0f;		// New in events file version 36
			_bp->AHindexVSlevel[i] 	= .0f;		// New in events file version 36
		}

		ar >> _bp->percentCentralSupine;
		ar >> _bp->percentCentralNonSupine;
		ar >> _bp->percentCentralHypoSupine;
		ar >> _bp->percentCentralHypoNonSupine;
		ar >> _bp->percentSpO2Supine;
		ar >> _bp->percentSpO2NonSupine;
		ar >> _bp->percentArousalSupine;
		ar >> _bp->percentArousalNonSupine;
	}
}

void CEvents::serializeBodyPosResults20(CArchive & ar, BODY_POS_RESULTS * _bp, int _majorVersion, int _minorVersion)
{
	//---Only for reading
	ASSERT(20 == _majorVersion);
	if (20 != _majorVersion) return;

	if (!ar.IsStoring()) {
		for (int i = 0; i < posCount; i++) ar >> _bp->central[i];
		for (int i = 0; i < posCount; i++) ar >> _bp->spO2[i];
		for (int i = 0; i < posCount; i++) ar >> _bp->centralHyp[i];
		for (int i = 0; i < posCount; i++) ar >> _bp->time[i];
		//for (int i = 0; i < posCount; i++) ar >> _bp->timeSleeping[i];			// New in events file version 26
		//for (int i = 0; i < posCount; i++) ar >> _bp->timeAwake[i];				// New in events file version 27
		for (int i = 0; i < posCount; i++) ar >> _bp->arousal[i];
		for (int i = 0; i < posCount; i++) ar >> _bp->snoringTime[i];
		for (int i = 0; i < posCount; i++) ar >> _bp->percentTime[i];
		//for (int i = 0; i < posCount; i++) ar >> _bp->percentTimeSleeping[i];	// New in events file version 26
		//for (int i = 0; i < posCount; i++) ar >> _bp->percentTimeAwake[i];	// New in events file version 27
		//for (int i = 0; i < posCount; i++) {
		//	ar >> _bp->RERAindex[i];	// New in events file version 26
		//	ar >> _bp->OAHindex[i];		// New in events file version 26
		//	ar >> _bp->CAHindex[i];		// New in events file version 26
		//	ar >> _bp->ODindex[i];		// New in events file version 26
		//}

		for (int i = 0; i < posCount; i++) {
			for (int j = 0; j < levelTypeCount; j++) ar >> _bp->obstr[i][j];
		}
		for (int i = 0; i < posCount; i++) {
			for (int j = 0; j < levelTypeCount; j++) ar >> _bp->mix[i][j];
		}
		for (int i = 0; i < posCount; i++) {
			for (int j = 0; j < levelTypeCount; j++) ar >> _bp->obstrHyp[i][j];
		}
		for (int i = 0; i < posCount; i++) {
			for (int j = 0; j < levelTypeCount; j++) ar >> _bp->RERA[i][j];
		}

		for (int i = 0; i < levelTypeCount; i++) ar >> _bp->percentObstrSupine[i];
		for (int i = 0; i < levelTypeCount; i++) ar >> _bp->percentObstrNonSupine[i];
		for (int i = 0; i < levelTypeCount; i++) ar >> _bp->percentMixSupine[i];
		for (int i = 0; i < levelTypeCount; i++) ar >> _bp->percentMixNonSupine[i];
		for (int i = 0; i < levelTypeCount; i++) ar >> _bp->percentObstrHypoSupine[i];
		for (int i = 0; i < levelTypeCount; i++) ar >> _bp->percentObstrHypoNonSupine[i];
		for (int i = 0; i < levelTypeCount; i++) ar >> _bp->percentRERASupine[i];
		for (int i = 0; i < levelTypeCount; i++) ar >> _bp->percentRERANonSupine[i];

		ar >> _bp->percentCentralSupine;
		ar >> _bp->percentCentralNonSupine;
		ar >> _bp->percentCentralHypoSupine;
		ar >> _bp->percentCentralHypoNonSupine;
		ar >> _bp->percentSpO2Supine;
		ar >> _bp->percentSpO2NonSupine;
		ar >> _bp->percentArousalSupine;
		ar >> _bp->percentArousalNonSupine;
	}
}

void CEvents::serializePulseRateResults(CArchive& ar, PULSE_RATE_RESULTS *_pr, int _majorVersion, int _minorVersion)
{
	if (ar.IsStoring()) {
		ar << _pr->satInval;					
		ar << _pr->average;
		ar << _pr->median;
		ar << _pr->highest;
		ar << _pr->lowest;
		ar << _pr->timeOfHighest;
		ar << _pr->timeOfLowest;
		for (int i = 0; i < pulseBinCount; i++) ar << _pr->histo[i];
		for (int i = 0; i < pulseBinCount; i++) ar << _pr->percentHisto[i];
	}
	else {
		ar >> _pr->satInval;
		ar >> _pr->average;
		ar >> _pr->median;
		ar >> _pr->highest;
		ar >> _pr->lowest;
		ar >> _pr->timeOfHighest;
		ar >> _pr->timeOfLowest;
		for (int i = 0; i < pulseBinCount; i++) ar >> _pr->histo[i];
		for (int i = 0; i < pulseBinCount; i++) ar >> _pr->percentHisto[i];
	}
}

void CEvents::serializeLevelResults(CArchive& ar, LEVEL_RESULTS *_lv, int _majorVersion, int _minorVersion)
{
	if (ar.IsStoring()) {
		for (int i = 0; i < levelTypeCount; i++) ar << _lv->totalPerHour[i];
		for (int i = 0; i < levelTypeCount; i++) ar << _lv->totalNum[i];
		for (int i = 0; i < levelTypeCount; i++) ar << _lv->percentTotalNum[i];

		for (int i = 0; i < evCountWithLevel; i++) {
			for (int j = 0; j < levelTypeCount; j++) ar << _lv->perHour[i][j];
		}

		for (int i = 0; i < evCountWithLevel; i++) {
			for (int j = 0; j < levelTypeCount; j++) ar << _lv->num[i][j];
		}
	}
	else {
		for (int i = 0; i < levelTypeCount; i++) ar >> _lv->totalPerHour[i];
		for (int i = 0; i < levelTypeCount; i++) ar >> _lv->totalNum[i];
		for (int i = 0; i < levelTypeCount; i++) ar >> _lv->percentTotalNum[i];

		for (int i = 0; i < evCountWithLevel; i++) {
			for (int j = 0; j < levelTypeCount; j++) ar >> _lv->perHour[i][j];
		}

		for (int i = 0; i < evCountWithLevel; i++) {
			for (int j = 0; j < levelTypeCount; j++) ar >> _lv->num[i][j];
		}
	}
}

void CEvents::serializeLevelResults20(CArchive & ar, LEVEL_RESULTS * _lv, int _majorVersion, int _minorVersion)
{
	//---Only for reading
	ASSERT(20 == _majorVersion);
	if (20 != _majorVersion) return;

	if (!ar.IsStoring()) {
		for (int i = 0; i < levelTypeCount; i++) ar >> _lv->totalPerHour[i];
		for (int i = 0; i < levelTypeCount; i++) ar >> _lv->totalNum[i];
		for (int i = 0; i < levelTypeCount; i++) ar >> _lv->percentTotalNum[i];

		for (int i = 0; i < ev20CountWithLevel; i++) {
			for (int j = 0; j < levelTypeCount; j++) ar >> _lv->perHour[i][j];
		}

		for (int i = 0; i < ev20CountWithLevel; i++) {
			for (int j = 0; j < levelTypeCount; j++) ar >> _lv->num[i][j];
		}
	}
}

void CEvents::serializeRdResults(CArchive& ar, RD_RESULTS *_rd, int _majorVersion, int _minorVersion)
{
	if (ar.IsStoring()) {
		for (int i = 0; i < obstrEvntCount; i++)	ar << _rd->obstrNum[i];
		for (int i = 0; i < centralEvntCount; i++)	ar << _rd->centrNum[i];
		for (int i = 0; i < obstrEvntCount; i++)	ar << _rd->obstrPerHour[i];
		for (int i = 0; i < centralEvntCount; i++)	ar << _rd->centrPerHour[i];
		for (int i = 0; i < obstrEvntCount; i++)	ar << _rd->obstrTotalDuration[i];
		for (int i = 0; i < centralEvntCount; i++)	ar << _rd->centrTotalDuration[i];
		for (int i = 0; i < obstrEvntCount; i++)	ar << _rd->obstrMaxDuration[i];
		for (int i = 0; i < centralEvntCount; i++)	ar << _rd->centrMaxDuration[i];
		for (int i = 0; i < obstrEvntCount; i++)	ar << _rd->obstrAverageDuration[i];
		for (int i = 0; i < centralEvntCount; i++)	ar << _rd->centrAverageDuration[i];
		for (int i = 0; i < obstrEvntCount; i++)	ar << _rd->obstrMedianDuration[i];
		for (int i = 0; i < centralEvntCount; i++)	ar << _rd->centrMedianDuration[i];
	}
	else {
		for (int i = 0; i < obstrEvntCount; i++)	ar >> _rd->obstrNum[i];
		for (int i = 0; i < centralEvntCount; i++)	ar >> _rd->centrNum[i];
		for (int i = 0; i < obstrEvntCount; i++)	ar >> _rd->obstrPerHour[i];
		for (int i = 0; i < centralEvntCount; i++)	ar >> _rd->centrPerHour[i];
		for (int i = 0; i < obstrEvntCount; i++)	ar >> _rd->obstrTotalDuration[i];
		for (int i = 0; i < centralEvntCount; i++)	ar >> _rd->centrTotalDuration[i];
		for (int i = 0; i < obstrEvntCount; i++)	ar >> _rd->obstrMaxDuration[i];
		for (int i = 0; i < centralEvntCount; i++)	ar >> _rd->centrMaxDuration[i];
		for (int i = 0; i < obstrEvntCount; i++)	ar >> _rd->obstrAverageDuration[i];
		for (int i = 0; i < centralEvntCount; i++)	ar >> _rd->centrAverageDuration[i];
		for (int i = 0; i < obstrEvntCount; i++)	ar >> _rd->obstrMedianDuration[i];
		for (int i = 0; i < centralEvntCount; i++)	ar >> _rd->centrMedianDuration[i];
	}
}

void CEvents::serializeSnoringResults(CArchive& ar, SNORING_RESULT *_sn, int _majorVersion, int _minorVersion)
{
	if (ar.IsStoring()) {
		for (int i = 0; i < levelTypeCount; i++)	ar << _sn->timeAtLevel[i];
		for (int i = 0; i < levelTypeCount; i++)	ar << _sn->percentTimeAtLevel[i];
		for (int i = 0; i < posCount; i++)			ar << _sn->timeInPos[i];
		for (int i = 0; i < posCount; i++)			ar << _sn->percentTimeInPos[i];
		ar << _sn->totalSnoringTime;
		ar << _sn->percentSnoring;

		ar << _sn->meanAmLower;
		ar << _sn->meanAmMulti;
		ar << _sn->meanAmUpper;
		ar << _sn->meanCmLower;
		ar << _sn->meanCmMulti;
		ar << _sn->meanCmUpper;
		ar << _sn->standardDevAmLower;
		ar << _sn->standardDevAmMulti;
		ar << _sn->standardDevAmUpper;
		ar << _sn->standardDevCmLower;
		ar << _sn->standardDevCmMulti;
		ar << _sn->standardDevCmUpper;
	}
	else {
		for (int i = 0; i < levelTypeCount; i++)	ar >> _sn->timeAtLevel[i];
		for (int i = 0; i < levelTypeCount; i++)	ar >> _sn->percentTimeAtLevel[i];
		for (int i = 0; i < posCount; i++)			ar >> _sn->timeInPos[i];
		for (int i = 0; i < posCount; i++)			ar >> _sn->percentTimeInPos[i];
		ar >> _sn->totalSnoringTime;
		ar >> _sn->percentSnoring;

		if (31 <= _majorVersion) {
			ar >> _sn->meanAmLower;
			ar >> _sn->meanAmMulti;
			ar >> _sn->meanAmUpper;
			ar >> _sn->meanCmLower;
			ar >> _sn->meanCmMulti;
			ar >> _sn->meanCmUpper;
			ar >> _sn->standardDevAmLower;
			ar >> _sn->standardDevAmMulti;
			ar >> _sn->standardDevAmUpper;
			ar >> _sn->standardDevCmLower;
			ar >> _sn->standardDevCmMulti;
			ar >> _sn->standardDevCmUpper;
		}
		else {
			_sn->meanAmLower = .0f;
			_sn->meanAmMulti = .0f;
			_sn->meanAmUpper = .0f;
			_sn->meanCmLower = .0f;
			_sn->meanCmMulti = .0f;
			_sn->meanCmUpper = .0f;
			_sn->standardDevAmLower = .0f;
			_sn->standardDevAmMulti = .0f;
			_sn->standardDevAmUpper = .0f;
			_sn->standardDevCmLower = .0f;
			_sn->standardDevCmMulti = .0f;
			_sn->standardDevCmUpper = .0f;
		}
	}
}

/*
Description: Fill the snoring vector from the snoring array. The snoring vector is used for graph display
*/
void CEvents::fillSnoringVector(void)
{
	resetSnoringVector();

	vector <UINT>::iterator f1, f2;
	vector <float>::iterator a1, a2;
	int num = snoringEventArray.GetCount();
	for (int i = 0; i < num; i++) {
		CSnoringEvnt *eP = snoringEventArray.GetAt(i);
		f1 = snoringVector.begin();
		f1 += (size_t) (eP->from * EVENT_TIMELINE_TIME_INTERVAL_INVERSE);
		f2 = snoringVector.begin();
		f2 += (size_t)(eP->to * EVENT_TIMELINE_TIME_INTERVAL_INVERSE);
		fill(f1, f2, evntTypeSnoring);

		vector <float> *vlow = eP->getLowerLevelStartStopVector();
		if (2 <= vlow->size()) {
			a1 = vlow->begin();
			a2 = a1 + 1;
			for (int i = 0; i < (int)vlow->size(); i += 2) {
				f1 = snoringVector.begin();
				f1 += (size_t) (*a1 * EVENT_TIMELINE_TIME_INTERVAL_INVERSE);
				f2 = snoringVector.begin();
				f2 += (size_t)(*a2 * EVENT_TIMELINE_TIME_INTERVAL_INVERSE);
				transform(f1, f2,f1, orSnoringLevelTypeLower);
				a1 += 2;
				a2++;
				a2 = a2 < vlow->end() ? a2 + 1 : a2;
			}
		}
		vector <float> *vmulti = eP->getMultiLevelStartStopVector();
		if (2 <= vmulti->size()) {
			a1 = vmulti->begin();
			a2 = a1 + 1;
			for (int i = 0; i < (int)vmulti->size(); i += 2) {
				f1 = snoringVector.begin();
				f1 += (size_t)(*a1 * EVENT_TIMELINE_TIME_INTERVAL_INVERSE);
				f2 = snoringVector.begin();
				f2 += (size_t)(*a2 * EVENT_TIMELINE_TIME_INTERVAL_INVERSE);
				transform(f1, f2, f1, orSnoringLevelTypeMulti);
				a1 += 2;
				a2++;
				a2 = a2 < vmulti->end() ? a2 + 1 : a2;
			}
		}
		vector <float> *vupper = eP->getUpperLevelStartStopVector(); 
		if (2 <= vupper->size()) {
			a1 = vupper->begin();
			a2 = a1 + 1;
			for (int i = 0 ; i < (int) vupper->size() ; i += 2) {
				f1 = snoringVector.begin();
				f1 += (size_t)(*a1 * EVENT_TIMELINE_TIME_INTERVAL_INVERSE);
				f2 = snoringVector.begin();
				f2 += (size_t)(*a2 * EVENT_TIMELINE_TIME_INTERVAL_INVERSE);
				transform(f1, f2, f1, orSnoringLevelTypeUpper);
				a1 += 2;
				a2++;
				a2 = a2 < vupper->end() ? a2 + 1 : a2;				
			}
		}
		vector <float> *vundef = eP->getUndefLevelStartStopVector();
		if (2 <= vundef->size()) {
			a1 = vundef->begin();
			a2 = a1 + 1;
			for (int i = 0; i < (int)vundef->size(); i += 2) {
				f1 = snoringVector.begin();
				f1 += (size_t)(*a1 * EVENT_TIMELINE_TIME_INTERVAL_INVERSE);
				f2 = snoringVector.begin();
				f2 += (size_t)(*a2 * EVENT_TIMELINE_TIME_INTERVAL_INVERSE);
				transform(f1, f2, f1, orSnoringLevelTypeUndef);
				a1 += 2;
				a2++;
				a2 = a2 < vundef->end() ? a2 + 1 : a2;
			}
		}
	}
}

void CEvents::fillExcludedEventArrayFromStartsStopsVector(vector<FLOAT>* _v)
{
	if (1 >= _v->size()) return;

	vector <FLOAT>::iterator iStart = _v->begin();
	vector <FLOAT>::iterator iStop = _v->begin();
	iStop++;

	if (*iStart <= .0f) effectiveStartTime = *iStop;
	float lastStart = .0f;
	float lastStop = .0f;
	for (; iStop < _v->end(); ++iStart, ++iStop) {
		short bp = bpData->getPos(*iStart,*iStop);
		CExcludedEvnt *eP = new CExcludedEvnt(*iStart, *iStop, startRecordingClockTime,bp);
		lastStart = *iStart;
		lastStop = *iStop;
	}
	if (lastStop >= maxStopTime) effectiveStopTime = lastStart;
}

void CEvents::fillExcludedEventArrayFromEventTimelineVector(void)
{
	for (int i = excludedEventArray.GetCount() - 1; i >= 0; --i) {
		CExcludedEvnt *eP = excludedEventArray.GetAt(i);
		delete eP;
	}
	excludedEventArray.RemoveAll();
	effectiveStartTime = .0f;
	effectiveStopTime = maxStopTime;

	vector <UINT>::iterator i0, i1;
	i0 = eventTimelineVector.begin();
	i1 = eventTimelineVector.begin();
	while ((i0 < eventTimelineVector.end()) && (i1 < eventTimelineVector.end())) {
		i0 = find_if(i1, eventTimelineVector.end(), isExcluded);
		i1 = find_if_not(i0, eventTimelineVector.end(), isExcluded);
		if (i0 < eventTimelineVector.end()) {
			int dist0 = distance(eventTimelineVector.begin(), i0);
			int dist1 = distance(eventTimelineVector.begin(), i1);
			float from = (float) dist0 * EVENT_TIMELINE_TIME_INTERVAL;
			float to = (float) dist1 * EVENT_TIMELINE_TIME_INTERVAL;
			
			CExcludedEvnt *eP = new CExcludedEvnt;
			eP->setRecordingStartTimeOfDay(startRecordingClockTime);
			eP->setFrom(from);
			eP->setTo(to);
			excludedEventArray.Add(eP);

			if (0 == dist0) effectiveStartTime = to;
			if (eventTimelineVector.size() <= (UINT) dist1) effectiveStopTime = from;
		}
	}
}

float CEvents::getDefaultLengthOfEventToAdd(UINT _etl,float _tWidth)
{
	if (_etl &evntTypeMixed) return ahDefaultLength;
	else if (_etl & evntTypeCentral) return ahDefaultLength;
	else if (_etl & evntTypeObstr) return ahDefaultLength;
	else if (_etl & evntTypeHypoCentral) return ahDefaultLength;
	else if (_etl & evntTypeHypoObstr) return ahDefaultLength;
	else if (_etl & evntTypeRERA) return reraDefaultLength;
	else if (_etl & evntTypeArousal) return arousalDefaultLength;
	else if (_etl & evntTypeSpO2Drop) return ahDefaultLength;
	else if (_etl & evntTypeManMarker) return manualDefaultLength;
	else if (_etl & evntTypeSnoring) return percentageOfView * _tWidth;
	else if (_etl & evntTypeAwake) return percentageOfView * _tWidth;
	else if (_etl & evntTypeExcluded) return percentageOfView * _tWidth;
	else if (_etl & evntTypeSwallow) return swallowDefaultLength;
	else if (_etl & evntTypeImported) return importedDefaultLength;

	return 0.0f;
}

void CEvents::serializeSpO2Results(CArchive& ar, SPO2_RESULTS *_sp, int _majorVersion, int _minorVersion)
{
	if (ar.IsStoring()) {
		for (int i = 0; i < satBinCount; i++) ar << _sp->satHisto[i];
		for (int i = 0; i < satBinCount; i++) ar << _sp->percentSatHisto[i];
		for (int i = 0; i < satBinCount; i++) ar << _sp->fallsHisto[i];
		for (int i = 0; i < satBinCount; i++) ar << _sp->percentFallsHisto[i];
		ar << _sp->satAvg;							// Average saturation
		ar << _sp->satMedian;						// Median saturation 
		ar << _sp->satLowest;						// Lowest saturation
		ar << _sp->satHighest;						// Highest saturation
		ar << _sp->timeOfSatHighest;
		ar << _sp->timeOfSatLowest;
		ar << _sp->oxygenDesaturationIndex;			// Number of desaturations per hour
		ar << _sp->numDrops;
		ar << _sp->minutesBelowSat90;
		ar << _sp->percentBelowSat90;
	}
	else {
		for (int i = 0; i < satBinCount; i++) ar >> _sp->satHisto[i];
		for (int i = 0; i < satBinCount; i++) ar >> _sp->percentSatHisto[i];
		for (int i = 0; i < satBinCount; i++) ar >> _sp->fallsHisto[i];
		for (int i = 0; i < satBinCount; i++) ar >> _sp->percentFallsHisto[i];
		ar >> _sp->satAvg;							// Average saturation
		ar >> _sp->satMedian;						// Median saturation 
		ar >> _sp->satLowest;						// Lowest saturation
		ar >> _sp->satHighest;						// Highest saturation
		ar >> _sp->timeOfSatHighest;
		ar >> _sp->timeOfSatLowest;
		ar >> _sp->oxygenDesaturationIndex;			// Number of desaturations per hour
		ar >> _sp->numDrops;
		ar >> _sp->minutesBelowSat90;
		ar >> _sp->percentBelowSat90;
	}
}

bool CEvents::getDoAwake(void)
{
	return doAwakeFromCmdLine == TRUE ? true : false;
}

/*
Description: Depending on the changeflags, events are sorted and all overlaps removed
*/
void CEvents::sortAndOrganiseEvents(int _changeFlags)
{
	if (_changeFlags & evntTypeMixed) sortAndOrganiseMixed();
	if (_changeFlags & evntTypeCentral) sortAndOrganiseCentral();
	if (_changeFlags & evntTypeObstr) sortAndOrganiseObstr();

	if ((_changeFlags & evntTypeHypoObstr) || (_changeFlags & evntTypeHypoCentral)) {
		for (int i = 0; i < numSpO2DropLimits; i++)
			sortAndOrganiseHypo(i);
	}

	//if (_changeFlags & evntTypeRERA) sortAndOrganiseRERA(detectionCriteria.spO2DropLimit);
	if (_changeFlags & evntTypeArousal) sortAndOrganiseArousal(detectionCriteria.spO2DropLimit);
	if (_changeFlags & evntTypeSpO2Drop) sortAndOrganiseSpO2Drop(detectionCriteria.spO2DropLimit);
	if (_changeFlags & evntTypeAwake) sortAndOrganiseAwake();
	if (_changeFlags & evntTypeSnoring) sortAndOrganiseSnoring();
	if (_changeFlags & evntTypeExcluded) sortAndOrganiseExcluded();

}

/*
Description: Sets detection criteria (file format 100) according to detection criteria in format 70
*/
void CEvents::convertCriteria70to100(DETECTION_CRITERIA70 *_dt70, DETECTION_CRITERIA100 *_dt100)
{
	_dt100->actimeterAboveBaselineLimit				= .0f;   // Not in file version 70
	_dt100->actimeterAlwaysAwakeLimit				= _dt70->actimeterAlwaysAwakeLimit;
	_dt100->admittancePeakFactorHead				= _dt70->admittancePeakFactorHead;
	_dt100->admittancePeakFactorTail				= _dt70->admittancePeakFactorTail;
	_dt100->admittancePeakWindowLength				= _dt70->admittancePeakWindowLength;
	_dt100->apneaDefaultDuration					= _dt70->apneaDefaultDuration;
	_dt100->apneaShortCentralDuration				= _dt70->apneaShortCentralDuration;
	_dt100->apneaShortObstrDuration					= _dt70->apneaShortObstrDuration;
	_dt100->arousalDetectionTimeStep				= _dt70->arousalDetectionTimeStep;
	_dt100->baselineLength							= _dt70->baselineLength;
	_dt100->contactMicSnoringThreshold				= _dt70->contactMicSnoringThreshold;
	_dt100->extendedWindowForExclusionOfAwake		= _dt70->extendedWindowForExclusionOfAwake;
	_dt100->flowIncreaseAtArousal					= .0f;   // Not in file version 70
	_dt100->flowStopFraction						= _dt70->flowStopFraction;
	_dt100->gradientLowerLimit						= _dt70->gradientLowerLimit;
	_dt100->gradientUpperLimit						= _dt70->gradientUpperLimit;
	_dt100->halfArousalLength						= _dt70->halfArousalLength;

	
	_dt100->hypopneaLimitFraction					= _dt70->hypopneaLimitFraction;

	_dt100->levelDetectionMargin					= _dt70->levelDetectionMargin;
	_dt100->linRegRSquaredLimit						= _dt70->linRegRSquaredLimit;
	_dt100->maxApneaDur								= _dt70->maxApneaDur;
	_dt100->maxEventLength							= _dt70->maxEventLength;
	_dt100->maxHypopneaDuration						= _dt70->maxHypopneaDuration;
	_dt100->p0RiseCoefficientLimit					= _dt70->p0RiseCoefficientLimit;
	_dt100->pOESalarmLine							= _dt70->pOESalarmLine;
	_dt100->pressureDeadFraction					= _dt70->pressureDeadFraction;
													 
	_dt100->pressureEnvPercentStabilityLimit		= _dt70->pressureEnvPercentStabilityLimit;
	_dt100->pulseRatePeakIncrease					= _dt70->pulseRatePeakIncrease;
	_dt100->pulseRatePeakWindowLength				= _dt70->pulseRatePeakWindowLength;
	_dt100->pulseRateWindowSlack					= _dt70->pulseRateWindowSlack;
	_dt100->REIncreaseDuration						= _dt70->REIncreaseDuration;
	_dt100->RESeekSlack								= _dt70->RESeekSlack;
	_dt100->RESeekStepSize							= _dt70->RESeekStepSize;
	_dt100->respFrqPeakFactor						= _dt70->respFrqPeakFactor;
	_dt100->respiratoryEffortFrqPeakWindowLength	= _dt70->respiratoryEffortFrqPeakWindowLength;
													 
	_dt100->snoringAverageWindowLength				= _dt70->snoringAverageWindowLength;
	_dt100->splitCentralAndObstrHypo				= _dt70->splitCentralAndObstrHypo;

	int lim = (int) (_dt70->spO2DropLimit);
	if (3 == lim) _dt100->spO2DropLimit = spO2DropLimit3;
	else if (4 == lim) _dt100->spO2DropLimit = spO2DropLimit4;
	else _dt100->spO2DropLimit = spO2DropLimit4;

	_dt100->spO2DropSeekWindowLength				= _dt70->spO2DropSeekWindowLength;
	_dt100->spO2DropSeekWindowStart				= _dt70->spO2DropSeekWindowStart;
													 
	_dt100->spO2stabilityLimit					= _dt70->spO2stabilityLimit;
	_dt100->t0EnvPercentStabilityLimit			= _dt70->t1EnvPercentStabilityLimit;   // Not in file version 70
	_dt100->t1EnvPercentStabilityLimit			= _dt70->t1EnvPercentStabilityLimit;
	_dt100->timeInBedLowLimit					= _dt70->timeInBedLowLimit;
}

void CEvents::convertCriteria100to200(DETECTION_CRITERIA100 * _dt100, DETECTION_CRITERIA200 * _dt200)
{
	_dt200->angleCoeffObstrVsCentralHypo = ANGLE_COEFF_OBSTR_VS_CENTRAL_HYPO; // Not in file version 70
																			  //---Convert hypopnea criterion
	int drop = (int)(100.0f - _dt100->hypopneaLimitFraction * 100.0f);
	switch (drop) {
	case 30:
		_dt200->hypopneaDropLimit = hypopneaDropLimit30;
		break;
	case 40:
		_dt200->hypopneaDropLimit = hypopneaDropLimit40;
		break;
	case 50:
		_dt200->hypopneaDropLimit = hypopneaDropLimit50;
		break;
	default:
		_dt200->hypopneaDropLimit = noHypopneaDropLimit;
		break;
	}
}

void CEvents::convertCriteria200to290(DETECTION_CRITERIA200 * _dt200, DETECTION_CRITERIA * _dt)
{
	_dt->spO2DropLimit							= _dt200->spO2DropLimit;
	_dt->splitCentralAndObstrHypo				= _dt200->splitCentralAndObstrHypo;
	_dt->pOESalarmLine							= _dt200->pOESalarmLine;
	_dt->levelDetectionMargin					= _dt200->levelDetectionMargin;
	_dt->apneaDefaultDuration					= _dt200->apneaDefaultDuration;
	_dt->hypopneaDropLimit						= _dt200->hypopneaDropLimit;
	_dt->apneaShortCentralDuration				= _dt200->apneaShortCentralDuration;
	_dt->apneaShortObstrDuration				= _dt200->apneaShortObstrDuration;
	_dt->maxApneaDur							= _dt200->maxApneaDur;
	_dt->maxHypopneaDuration					= _dt200->maxHypopneaDuration;
	_dt->timeInBedLowLimit						= _dt200->timeInBedLowLimit;
	_dt->extendedWindowForExclusionOfAwake		= _dt200->extendedWindowForExclusionOfAwake;
	_dt->actimeterAlwaysAwakeLimit				= _dt200->actimeterAlwaysAwakeLimit; // Not using this 4.1 and onwards
	_dt->actimeterAboveBaselineLimit			= _dt200->actimeterAboveBaselineLimit;
	_dt->respiratoryEffortFrqPeakWindowLength	= _dt200->respiratoryEffortFrqPeakWindowLength;
	_dt->admittancePeakWindowLength				= _dt200->admittancePeakWindowLength;
	_dt->pulseRatePeakWindowLength				= _dt200->pulseRatePeakWindowLength;
	_dt->pulseRateWindowSlack					= _dt200->pulseRateWindowSlack;
	_dt->arousalDetectionTimeStep				= _dt200->arousalDetectionTimeStep;
	_dt->respFrqPeakFactor						= _dt200->respFrqPeakFactor;
	_dt->admittancePeakFactorHead				= _dt200->admittancePeakFactorHead;
	_dt->admittancePeakFactorTail				= _dt200->admittancePeakFactorTail;
	_dt->pulseRatePeakIncrease					= _dt200->pulseRatePeakIncrease;
	_dt->REIncreaseDuration						= _dt200->REIncreaseDuration;
	_dt->RESeekStepSize							= _dt200->RESeekStepSize;
	_dt->RESeekSlack							= _dt200->RESeekSlack;
	_dt->contactMicSnoringThreshold				= _dt200->contactMicSnoringThreshold;
	_dt->linRegRSquaredLimit					= _dt200->linRegRSquaredLimit;
	_dt->snoringAverageWindowLength				= _dt200->snoringAverageWindowLength;
	_dt->baselineLength							= _dt200->baselineLength;
	_dt->spO2stabilityLimit						= _dt200->spO2stabilityLimit;
	_dt->t0EnvPercentStabilityLimit				= _dt200->t0EnvPercentStabilityLimit;
	_dt->t1EnvPercentStabilityLimit				= _dt200->t1EnvPercentStabilityLimit;
	_dt->pressureEnvPercentStabilityLimit		= _dt200->pressureEnvPercentStabilityLimit;
	_dt->flowStopFraction						= _dt200->flowStopFraction;
	_dt->pressureDeadFraction					= _dt200->pressureDeadFraction;
	_dt->spO2DropSeekWindowLength				= _dt200->spO2DropSeekWindowLength;
	_dt->spO2DropSeekWindowStart				= _dt200->spO2DropSeekWindowStart;
	_dt->maxEventLength							= _dt200->maxEventLength;
	_dt->gradientUpperLimit						= _dt200->gradientUpperLimit;
	_dt->gradientLowerLimit						= _dt200->gradientLowerLimit;
	_dt->halfArousalLength						= _dt200->halfArousalLength;
	_dt->p0RiseCoefficientLimit					= _dt200->p0RiseCoefficientLimit;
	_dt->flowIncreaseAtArousal					= _dt200->flowIncreaseAtArousal; // %
	_dt->angleCoeffObstrVsCentralHypo			= _dt200->angleCoeffObstrVsCentralHypo;
	_dt->torsoActimeterLimit					= TORSO_ACTIMETER_LIMIT;
	_dt->awakeMixeFactor						= AWAKE_MIXE_FACTOR;
	_dt->awakeObstFactor						= AWAKE_OBST_FACTOR;
	_dt->awakeCentFactor						= AWAKE_CENT_FACTOR;
	_dt->awakeHypoFactor						= AWAKE_HYPO_FACTOR;
	_dt->awakeDesaFactor						= AWAKE_DESA_FACTOR;
	_dt->awakeRERAFactor						= AWAKE_RERA_FACTOR;
	_dt->awakeSnorFactor						= AWAKE_SNOR_FACTOR;
	_dt->awakeSwalFactor						= AWAKE_SWAL_FACTOR;
	_dt->awakeActiFactor						= AWAKE_ACTI_FACTOR;
	_dt->awakeTActFactor						= AWAKE_TACT_FACTOR;
	_dt->awakeStanFactor						= AWAKE_STAN_FACTOR;
	_dt->flowDetectBalanceFactor				= FLOW_DETECT_BALANCE_FACTOR;
}


/*
Description: Saves or reads Events.
Version and fname is recorded into the file. 
When read, the filename as read from the file must agree with the actual file name, if not returns false
*/
bool CEvents::Serialize(CArchive& ar,CString _fileID)
{
	if (ar.IsStoring()) {
		if (doNotSaveFlag) return false;

		majorVersion = EVENTS_BIN_MAJOR_VERSION;
		minorVersion = EVENTS_BIN_MINOR_VERSION;
		
		ar << majorVersion;
		ar << minorVersion;
		ar << _fileID;

		if (doAwakeFromCmdLine) flags |= EVENTS_FLAG_DO_AWAKE;

		ar << flags;

		analysisSWVersion = swVersion;
		ar << swVersion;

		//---Detection criteria
		ar.Write((void *) &detectionCriteria, sizeof(DETECTION_CRITERIA));
		ar.Write((void *) &editedDetectionCriteria, sizeof(DETECTION_CRITERIA));

		//---All over RE
		for (int i = 0; i < ppBins; i++) ar << poesBin[i];
		for (int i = 0; i < ppBins; i++) ar << poesCumulBin[i];
		for (int i = 0; i < ppHistoBins; i++) {
			for (int j = 0; j < levelTypeCount; j++) ar << poesHistoBinLevel[i][j];
		}
		for (int i = 0; i < ppHistoBins; i++) {
			for (int j = 0; j < posCount; j++) ar << poesHistoBinBodyPos[i][j];
		}

		//---Snoring RE
		for (int i = 0; i < ppBins; i++) ar << snoringPoesBin[i];
		for (int i = 0; i < ppBins; i++) ar << snoringPoesCumulBin[i];
		for (int i = 0; i < ppHistoBins; i++) {
			for (int j = 0; j < levelTypeCount; j++) ar << snoringPoesHistoBinLevel[i][j];
		}
		for (int i = 0; i < ppHistoBins; i++) {
			for (int j = 0; j < posCount; j++) ar << snoringPoesHistoBinBodyPos[i][j];
		}

		//---All over PPH
		for (int i = 0; i < ppBins; i++) ar << pphBin[i];
		for (int i = 0; i < ppBins; i++) ar << pphCumulBin[i];
		for (int i = 0; i < ppHistoBins; i++) {
			for (int j = 0; j < levelTypeCount; j++) ar << pphHistoBinLevel[i][j];
		}
		for (int i = 0; i < ppHistoBins; i++) {
			for (int j = 0; j < posCount; j++) ar << pphHistoBinBodyPos[i][j];
		}

		//---Snoring PPH
		for (int i = 0; i < ppBins; i++) ar << snoringPphBin[i];
		for (int i = 0; i < ppBins; i++) ar << snoringPphCumulBin[i];
		for (int i = 0; i < ppHistoBins; i++) {
			for (int j = 0; j < levelTypeCount; j++) ar << snoringPphHistoBinLevel[i][j];
		}
		for (int i = 0; i < ppHistoBins; i++) {
			for (int j = 0; j < posCount; j++) ar << snoringPphHistoBinBodyPos[i][j];
		}

		ar << startRecordingClockTime;
		ar << effectiveStartTime;
		ar << effectiveStopTime;		
		ar << maxStopTime;				
		ar << totalSleepTime;

		//---Criteria dependent
		for (int i = 0; i < numSpO2DropLimits; i++) {
			for (int j = 0; j < numHypopneaDropLimits; j++) {
				ar << rdiResult[j][i];			// osa_ordi + cai
				ar << ohiResult[j][i];
				ar << chiResult[j][i];
				ar << cahiResult[j][i];			// chi + cai
				ar << oahiResult[j][i];			// mai+oai+ohi
				ar << osa_ordiResult[j][i];		// oahi + rerai
				ar << numObstrHypopnea[j][i];
				ar << numCentralHypopnea[j][i];
				ar << arousalIndexResult[j][i];
				ar << reraiResult[j][i];
			}
		}
		ar << maiResult;
		ar << caiResult;
		ar << oaiResult;
		ar << eventDefinition;
	}
	else {  // Reading
		CString fileID;
		ar >> majorVersion;
		ar >> minorVersion;
		ar >> fileID;
		if (fileID != _fileID)
			return false;

		if (20 == majorVersion) {
			CString s1, s2, s3;
			int num;
			num = s1.LoadString(IDS_EVENTSREAD_MESS1);
			num = s2.LoadString(IDS_EVENTSREAD_MESS2);
			num = s3.LoadString(IDS_EVENTSREAD_MESS3);
			s1 += s2;
			s1 += s3;

			//--Want update ?
			int ret = AfxMessageBox(s1, MB_YESNO);
			if (IDYES == ret) {
				noEditFlag = false;
				doNotSaveFlag = false;
				return false;
			}
			else 
				return serialize20(ar, majorVersion, minorVersion);
		}
		else if (majorVersion < 35) {	//EVENTS_BIN_MAJOR_VERSION) {
			CString s;
			int num = s.LoadString(IDS_CANNOT_READ_OLD_EVENTS);
			AfxMessageBox(s);
			return false;
		}
		else if ((35 /*EVENTS_BIN_MAJOR_VERSION*/ == majorVersion) && (minorVersion < EVENTS_BIN_MINOR_VERSION)) {
			CString s;
			int num = s.LoadString(IDS_CANNOT_READ_OLD_EVENTS);
			AfxMessageBox(s);
			return false;
		}

		//---Flags must be the same for the analysis to be valid
		ar >> flags;
		if (doAwakeFromCmdLine && !(flags & EVENTS_FLAG_DO_AWAKE))
			return false;
		if (!doAwakeFromCmdLine && (flags & EVENTS_FLAG_DO_AWAKE))
			return false;

		if (29 <= majorVersion) {
			ar >> analysisSWVersion;
		}

		////---Check version compatibility going from release versions to current version
		///*
		//		7.0, 10.0 or 20.0 were all release versions
		//*/
		//if (((7 == majorVersion) && (0 == minorVersion)) ||
		//	((10 == majorVersion) && (0 == minorVersion)) ||
		//	((20 == majorVersion) && (0 == minorVersion)))	{
		//	//--Display changes 7 to 9
		//	CString s1, s2, s3;
		//	s1.LoadString(IDS_EVENTSREAD_MESS1);
		//	s2.LoadString(IDS_EVENTSREAD_MESS2);
		//	s3.LoadString(IDS_EVENTSREAD_MESS3);
		//	s1 += s2;
		//	s1 += s3;
		//
		//	//--Want update ?
		//	int ret = AfxMessageBox(s1, MB_YESNO);
		//	if (IDYES == ret) {
		//		noEditFlag = false;
		//		doNotSaveFlag = false;
		//		return false;
		//	}
		//	else {
		//		//---No : read and convert
		//		if ((7 == majorVersion) && (0 == minorVersion)) {
		//			DETECTION_CRITERIA70 dt70, editedDt70;
		//			DETECTION_CRITERIA100 dt100, editedDt100;
		//			DETECTION_CRITERIA200 dt200, editedDt200;
		//			ar.Read((void *)&dt70, sizeof(DETECTION_CRITERIA70));
		//			ar.Read((void *)&editedDt70, sizeof(DETECTION_CRITERIA70));
		//			convertCriteria70to100(&dt70, &dt100);
		//			convertCriteria70to100(&editedDt70, &editedDt100);
		//
		//			convertCriteria100to200(&dt100, &dt200);
		//			convertCriteria100to200(&editedDt100, &editedDt200);
		//
		//			convertCriteria200to290(&dt200, &detectionCriteria);
		//			convertCriteria200to290(&editedDt200, &editedDetectionCriteria);
		//		}
		//		else if ((10 == majorVersion) && (0 == minorVersion)) {
		//			DETECTION_CRITERIA100 dt100, editedDt100;
		//			DETECTION_CRITERIA200 dt200, editedDt200;
		//			ar.Read((void *)&dt100, sizeof(DETECTION_CRITERIA100));
		//			ar.Read((void *)&editedDt100, sizeof(DETECTION_CRITERIA100));
		//
		//			convertCriteria100to200(&dt100, &dt200);
		//			convertCriteria100to200(&editedDt100, &editedDt200);
		//
		//			convertCriteria200to290(&dt200, &detectionCriteria);
		//			convertCriteria200to290(&editedDt200, &editedDetectionCriteria);
		//		}
		//		else if ((20 == majorVersion) && (0 == minorVersion)) {
		//			DETECTION_CRITERIA200 dt200, editedDt200;
		//			ar.Read((void *)&dt200, sizeof(DETECTION_CRITERIA200));
		//			ar.Read((void *)&editedDt200, sizeof(DETECTION_CRITERIA200));
		//
		//			convertCriteria200to290(&dt200, &detectionCriteria);
		//			convertCriteria200to290(&editedDt200, &editedDetectionCriteria);
		//		}
		//		noEditFlag = true;
		//		doNotSaveFlag = true;
		//	}
		//}
		//else if (majorVersion < EVENTS_BIN_MAJOR_VERSION) {
		//	//---Message ?
		//	return false;
		//}
		//else if ((EVENTS_BIN_MAJOR_VERSION == majorVersion) && (minorVersion < EVENTS_BIN_MINOR_VERSION)) {
		//	//---Message ?
		//	return false;
		//}
		//else {
		//	ar.Read((void *)&detectionCriteria, sizeof(DETECTION_CRITERIA));
		//	ar.Read((void *)&editedDetectionCriteria, sizeof(DETECTION_CRITERIA));
		//	noEditFlag = false;
		//	doNotSaveFlag = false;
		//}

		//---Read detection criteria
		ar.Read((void *)&detectionCriteria, sizeof(DETECTION_CRITERIA));
		ar.Read((void *)&editedDetectionCriteria, sizeof(DETECTION_CRITERIA));
		noEditFlag = false;
		doNotSaveFlag = false;

		//---All over RE
		for (int i = 0; i < ppBins; i++) ar >> poesBin[i];
		for (int i = 0; i < ppBins; i++) ar >> poesCumulBin[i];
		for (int i = 0; i < ppHistoBins; i++) {
			for (int j = 0; j < levelTypeCount; j++) ar >> poesHistoBinLevel[i][j];
		}
		for (int i = 0; i < ppHistoBins; i++) {
			for (int j = 0; j < posCount; j++) ar >> poesHistoBinBodyPos[i][j];
		}

		//---Snoring RE
		if ((majorVersion >= 30) && (minorVersion >= 0)) {
			for (int i = 0; i < ppBins; i++) ar >> snoringPoesBin[i];
			for (int i = 0; i < ppBins; i++) ar >> snoringPoesCumulBin[i];
			for (int i = 0; i < ppHistoBins; i++) {
				for (int j = 0; j < levelTypeCount; j++) ar >> snoringPoesHistoBinLevel[i][j];
			}
			for (int i = 0; i < ppHistoBins; i++) {
				for (int j = 0; j < posCount; j++) ar >> snoringPoesHistoBinBodyPos[i][j];
			}
		}

		if ((majorVersion >= 34) && (minorVersion >= 0)) {
			//---All over PPH
			for (int i = 0; i < ppBins; i++) ar >> pphBin[i];
			for (int i = 0; i < ppBins; i++) ar >> pphCumulBin[i];
			for (int i = 0; i < ppHistoBins; i++) {
				for (int j = 0; j < levelTypeCount; j++) ar >> pphHistoBinLevel[i][j];
			}
			for (int i = 0; i < ppHistoBins; i++) {
				for (int j = 0; j < posCount; j++) ar >> pphHistoBinBodyPos[i][j];
			}

			//---Snoring PPH
			for (int i = 0; i < ppBins; i++) ar >> snoringPphBin[i];
			for (int i = 0; i < ppBins; i++) ar >> snoringPphCumulBin[i];
			for (int i = 0; i < ppHistoBins; i++) {
				for (int j = 0; j < levelTypeCount; j++) ar >> snoringPphHistoBinLevel[i][j];
			}
			for (int i = 0; i < ppHistoBins; i++) {
				for (int j = 0; j < posCount; j++) ar >> snoringPphHistoBinBodyPos[i][j];
			}
		}

		ar >> startRecordingClockTime;
		ar >> effectiveStartTime;
		ar >> effectiveStopTime;
		ar >> maxStopTime;
		ar >> totalSleepTime;
		   
		//---Criteria dependent
		for (int i = 0; i < numSpO2DropLimits; i++) {
			for (int j = 0; j < numHypopneaDropLimits; j++) {
				ar >> rdiResult[j][i];			// osa_ordi + cai
				ar >> ohiResult[j][i];
				ar >> chiResult[j][i];
				ar >> cahiResult[j][i];			// chi + cai
				ar >> oahiResult[j][i];			// mai+oai+ohi
				ar >> osa_ordiResult[j][i];		// oahi + rerai
				ar >> numObstrHypopnea[j][i];
				ar >> numCentralHypopnea[j][i];
				ar >> arousalIndexResult[j][i];
				ar >> reraiResult[j][i];
			}
		}
		ar >> maiResult;
		ar >> caiResult;
		ar >> oaiResult;
		ar >> eventDefinition; 
	}

	for (int i = 0; i < numSpO2DropLimits; i++) {
		for (int j = 0; j < numHypopneaDropLimits; j++) {
			serializeBodyPosResults(ar, &bodyPosResults[j][i],majorVersion,minorVersion);
		}
	}

	//---Pulse rate per sleep/awake was included in version 32
	if (ar.IsStoring()) {
		for (int j = 0; j < numSleepAwakeStates; j++) {
			serializePulseRateResults(ar, &pulseRateResults[j], majorVersion, minorVersion);
		}
	}
	else {
		if (majorVersion < 32)
			serializePulseRateResults(ar, &pulseRateResults[overall], majorVersion, minorVersion);
		else {
			for (int j = 0; j < numSleepAwakeStates; j++) {
				serializePulseRateResults(ar, &pulseRateResults[j], majorVersion, minorVersion);
			}
		}
	}

	for (int i = 0; i < numSpO2DropLimits; i++) {
		for (int j = 0; j < numHypopneaDropLimits; j++) {
			serializeLevelResults(ar, &lvlResults[j][i], majorVersion, minorVersion);
		}
	}

	for (int i = 0; i < numSpO2DropLimits; i++) {
		for (int j = 0; j < numHypopneaDropLimits; j++) {
			serializeRdResults(ar, &rdResults[j][i], majorVersion, minorVersion);
		}
	}
	serializeSnoringResults(ar, &snoringResult, majorVersion, minorVersion);

	//---SpO2 results per sleep/awake was included in version 32
	if (ar.IsStoring()) {
		for (int j = 0; j < numSleepAwakeStates; j++) {
			for (int i = 0; i < numSpO2DropLimits; i++) {
				serializeSpO2Results(ar, &spO2Res[i][j], majorVersion, minorVersion);
			}
		}
	}
	else {
		if (majorVersion < 32) {
			for (int i = 0; i < numSpO2DropLimits; i++) {
				serializeSpO2Results(ar, &spO2Res[i][overall], majorVersion, minorVersion);
			}
		}
		else {
			for (int j = 0; j < numSleepAwakeStates; j++) {
				for (int i = 0; i < numSpO2DropLimits; i++) {
					serializeSpO2Results(ar, &spO2Res[i][j], majorVersion, minorVersion);
				}
			}
		}
	}

	CEvnt::serializeVectorUint(ar, eventTimelineVector.begin(), eventTimelineVector.end(),&eventTimelineVector);
	CEvnt::serializeVectorUint(ar, snoringVector.begin(), snoringVector.end(), &snoringVector);
	CEvnt::serializeVectorFloat(ar, EventsBeginsEnds.begin(), EventsBeginsEnds.end(),&EventsBeginsEnds);

	for (int i = 0; i < evCountWithLevel; i++) {
		for (int j = 0; j < bgLevelTypeCount; j++)
			CEvnt::serializeVectorFloat(ar, eventsWithLevelBeginsEnds[i][j].begin(), 
											eventsWithLevelBeginsEnds[i][j].end(),
											&eventsWithLevelBeginsEnds[i][j]);
	}
	for (int i = 0; i < evCountWithoutLevel; i++) {
		CEvnt::serializeVectorFloat(ar, eventsWithoutLevelBeginsEnds[i].begin(),
										eventsWithoutLevelBeginsEnds[i].end(),
										&eventsWithoutLevelBeginsEnds[i]);
	}

	int num;

	//---Mixed 1
	if (ar.IsStoring()) {
		num = mixedEventArray.GetCount();
		ar << num;
		for (int i = 0; i < num; ++i) {
			CMixedEvnt *evt = mixedEventArray.GetAt(i);
			evt->Serialize(ar,majorVersion,minorVersion);
		}
	}
	else {
		ar >> num;
		for (int i = 0; i < num; ++i) {
			CMixedEvnt *evt = new CMixedEvnt;
			evt->Serialize(ar, majorVersion, minorVersion);
			mixedEventArray.Add(evt);
		}
	}

	//---Central 2
	if (ar.IsStoring()) {
		num = centralEventArray.GetCount();
		ar << num;
		for (int i = 0; i < num; ++i) {
			CCentralEvnt *evt = centralEventArray.GetAt(i);
			evt->Serialize(ar, majorVersion, minorVersion);
		}
	}
	else {
		ar >> num;
		for (int i = 0; i < num; ++i) {
			CCentralEvnt *evt = new CCentralEvnt;
			evt->Serialize(ar, majorVersion, minorVersion);
			centralEventArray.Add(evt);
		}
	}

	//---Obstr 3
	if (ar.IsStoring()) {
		num = obstrEventArray.GetCount();
		ar << num;
		for (int i = 0; i < num; ++i) {
			CObstrEvnt *evt = obstrEventArray.GetAt(i);
			evt->Serialize(ar, majorVersion, minorVersion);
		}
	}
	else {
		ar >> num;
		for (int i = 0; i < num; ++i) {
			CObstrEvnt *evt = new CObstrEvnt;
			evt->Serialize(ar, majorVersion, minorVersion);
			obstrEventArray.Add(evt);
		}
	}

	//---Hypo 4
	if (ar.IsStoring()) {
		num = hypoEventArrayDesat3.GetCount();
		ar << num;
		for (int i = 0; i < num; ++i) {
			CHypoEvnt *evt = hypoEventArrayDesat3.GetAt(i);
			evt->Serialize(ar, majorVersion, minorVersion);
		}
	}
	else {
		ar >> num;
		for (int i = 0; i < num; ++i) {
			CHypoEvnt *evt = new CHypoEvnt;
			evt->Serialize(ar, majorVersion, minorVersion);
			hypoEventArrayDesat3.Add(evt);
		}
	}

	//---Hypo 5
	if (ar.IsStoring()) {
		num = hypoEventArrayDesat4.GetCount();
		ar << num;
		for (int i = 0; i < num; ++i) {
			CHypoEvnt *evt = hypoEventArrayDesat4.GetAt(i);
			evt->Serialize(ar, majorVersion, minorVersion);
		}
	}
	else {
		ar >> num;
		for (int i = 0; i < num; ++i) {
			CHypoEvnt *evt = new CHypoEvnt;
			evt->Serialize(ar, majorVersion, minorVersion);
			hypoEventArrayDesat4.Add(evt);
		}
	}

	//---Arousal 8
	if (ar.IsStoring()) {
		num = arousalEventArrayDesat3.GetCount();
		ar << num;
		for (int i = 0; i < num; ++i) {
			CArousalEvnt *evt = arousalEventArrayDesat3.GetAt(i);
			evt->Serialize(ar, majorVersion, minorVersion);
		}
	}
	else {
		ar >> num;
		for (int i = 0; i < num; ++i) {
			CArousalEvnt *evt = new CArousalEvnt;
			evt->Serialize(ar, majorVersion, minorVersion);
			arousalEventArrayDesat3.Add(evt);
		}
	}

	//---Arousal 9
	if (ar.IsStoring()) {
		num = arousalEventArrayDesat4.GetCount();
		ar << num;
		for (int i = 0; i < num; ++i) {
			CArousalEvnt *evt = arousalEventArrayDesat4.GetAt(i);
			evt->Serialize(ar, majorVersion, minorVersion);
		}
	}
	else {
		ar >> num;
		for (int i = 0; i < num; ++i) {
			CArousalEvnt *evt = new CArousalEvnt;
			evt->Serialize(ar, majorVersion, minorVersion);
			arousalEventArrayDesat4.Add(evt);
		}
	}

	//---SpO2 10 3% drop
	if (ar.IsStoring()) {
		num = spO2DropEventArray3.GetCount();
		ar << num;
		for (int i = 0; i < num; ++i) {
			CSpO2DropEvnt *evt = spO2DropEventArray3.GetAt(i);
			evt->Serialize(ar, majorVersion, minorVersion);
		}
	}
	else {
		ar >> num;
		for (int i = 0; i < num; ++i) {
			CSpO2DropEvnt *evt = new CSpO2DropEvnt(detectionCriteria.spO2DropLimit);
			evt->Serialize(ar, majorVersion, minorVersion);
			spO2DropEventArray3.Add(evt);
		}
	}

	//---SpO2 11 4% drop
	if (ar.IsStoring()) {
		num = spO2DropEventArray4.GetCount();
		ar << num;
		for (int i = 0; i < num; ++i) {
			CSpO2DropEvnt *evt = spO2DropEventArray4.GetAt(i);
			evt->Serialize(ar, majorVersion, minorVersion);
		}
	}
	else {
		ar >> num;
		for (int i = 0; i < num; ++i) {
			CSpO2DropEvnt *evt = new CSpO2DropEvnt(detectionCriteria.spO2DropLimit);
			evt->Serialize(ar, majorVersion, minorVersion);
			spO2DropEventArray4.Add(evt);
		}
	}
	if (!ar.IsStoring()) {
		switch (detectionCriteria.spO2DropLimit) {
		case spO2DropLimit3:
			spO2DropEventArrayCurrentP = &spO2DropEventArray3;
			hypoEventArrayCurrentP = &hypoEventArrayDesat3;
			arousalEventArrayCurrentP = &arousalEventArrayDesat3;
			break;
		case spO2DropLimit4:
			spO2DropEventArrayCurrentP = &spO2DropEventArray4;
			hypoEventArrayCurrentP = &hypoEventArrayDesat4;
			arousalEventArrayCurrentP = &arousalEventArrayDesat4;
			break;
		default:
			spO2DropEventArrayCurrentP = &spO2DropEventArray4;
			hypoEventArrayCurrentP = &hypoEventArrayDesat4;
			arousalEventArrayCurrentP = &arousalEventArrayDesat4;
			break;
		}
	}

	//---Manual 12
	if (ar.IsStoring()) {
		num = manualMarkerEventArray.GetCount();
		ar << num;
		for (int i = 0; i < num; ++i) {
			CManualMarkerEvnt *evt = manualMarkerEventArray.GetAt(i);
			evt->Serialize(ar, majorVersion, minorVersion);
		}
	}
	else {
		ar >> num;
		for (int i = 0; i < num; ++i) {
			CManualMarkerEvnt *evt = new CManualMarkerEvnt;
			evt->Serialize(ar, majorVersion, minorVersion);
			manualMarkerEventArray.Add(evt);
		}
	}

	//---Awake 13
	if (ar.IsStoring()) {
		num = awakeEventArray.GetCount();
		ar << num;
		for (int i = 0; i < num; ++i) {
			CAwakeEvnt *evt = awakeEventArray.GetAt(i);
			evt->Serialize(ar, majorVersion, minorVersion);
		}
	}
	else {
		ar >> num;
		for (int i = 0; i < num; ++i) {
			CAwakeEvnt *evt = new CAwakeEvnt(.0f, .0f, defDateTime, posUndefined, autoDetection);
			evt->Serialize(ar, majorVersion, minorVersion);
			awakeEventArray.Add(evt);
		}
	}

	//---Snoring 14
	if (ar.IsStoring()) {
		num = snoringEventArray.GetCount();
		ar << num;
		for (int i = 0; i < num; ++i) {
			CSnoringEvnt *evt = snoringEventArray.GetAt(i);
			evt->Serialize(ar, majorVersion, minorVersion);
		}
	}
	else {
		ar >> num;
		for (int i = 0; i < num; ++i) {
			CSnoringEvnt *evt = new CSnoringEvnt(.0f, .0f, defDateTime, posUndefined, autoDetection);
			evt->Serialize(ar, majorVersion, minorVersion);
			snoringEventArray.Add(evt);
		}
	}

	//---Excluded 15
	if (ar.IsStoring()) {
		num = excludedEventArray.GetCount();
		ar << num;
		for (int i = 0; i < num; ++i) {
			CExcludedEvnt *evt = excludedEventArray.GetAt(i);
			evt->Serialize(ar, majorVersion, minorVersion);
		}
	}
	else {
		ar >> num;
		for (int i = 0; i < num; ++i) {
			CExcludedEvnt *evt = new CExcludedEvnt;
			evt->Serialize(ar, majorVersion, minorVersion);
			excludedEventArray.Add(evt);
		}
	}

	//---Swallow 16
	if (ar.IsStoring()) {
		num = swallowEventArray.GetCount();
		ar << num;
		for (int i = 0; i < num; ++i) {
			CSwallowEvnt *evt = swallowEventArray.GetAt(i);
			evt->Serialize(ar, majorVersion, minorVersion);
		}
	}
	else {
		ar >> num;
		for (int i = 0; i < num; ++i) {
			CSwallowEvnt *evt = new CSwallowEvnt;
			evt->Serialize(ar, majorVersion, minorVersion);
			swallowEventArray.Add(evt);
		}
	}

	//---Imported 17
	if (ar.IsStoring()) {
		num = importedEventArray.GetCount();
		ar << num;
		for (int i = 0; i < num; ++i) {
			CImportedEvnt *evt = importedEventArray.GetAt(i);
			evt->Serialize(ar, majorVersion, minorVersion);
		}
	}
	else {
		ar >> num;
		for (int i = 0; i < num; ++i) {
			CImportedEvnt *evt = new CImportedEvnt;
			evt->Serialize(ar, majorVersion, minorVersion);
			importedEventArray.Add(evt);
		}
	}

	//---Compensate for missing parameters
	if ((ar.IsLoading()) && (majorVersion < 36)) {
		for (int i = 0; i < numSpO2DropLimits; i++) {
			for (int j = 0; j < numHypopneaDropLimits; j++) {
				doBodyPosAnalysis(j, i);		// Finds how events split between body positions
			}
		}
	}
	return true;
}

float CEvents::getOdiResult(void)
{
	return (float) spO2Res[detectionCriteria.spO2DropLimit][asleep].oxygenDesaturationIndex;
}

float CEvents::getRdiResult(void)
{
	return rdiResult[detectionCriteria.hypopneaDropLimit][detectionCriteria.spO2DropLimit];
}

float CEvents::getRE15Sleeping(void)
{
	if (.0f == poesCumulBin[0]) return .0f;
	else return (float)poesCumulBin[15] * 100.0f / poesCumulBin[0];
}

float CEvents::getRE15Snoring(void)
{
	if (.0f == snoringPoesCumulBin[0]) return .0f;
	else return (float) snoringPoesCumulBin[15] * 100.0f / snoringPoesCumulBin[0];
}

float CEvents::getArousalIndexResult(void)
{
	return arousalIndexResult[detectionCriteria.hypopneaDropLimit][detectionCriteria.spO2DropLimit];
}

float CEvents::getMaiResult(void)
{
	return maiResult;
}

float CEvents::getCaiResult(void)
{
	return caiResult;
}

float CEvents::getOaiResult(void)
{
	return oaiResult;
}

float CEvents::getOhiResult(void)
{
	return ohiResult[detectionCriteria.hypopneaDropLimit][detectionCriteria.spO2DropLimit];
}

float CEvents::getChiResult(void)
{
	return chiResult[detectionCriteria.hypopneaDropLimit][detectionCriteria.spO2DropLimit];
}

float CEvents::getCahiResult(void)
{
	return cahiResult[detectionCriteria.hypopneaDropLimit][detectionCriteria.spO2DropLimit];
}

float CEvents::getReraiResult(void)
{
	return reraiResult[detectionCriteria.hypopneaDropLimit][detectionCriteria.spO2DropLimit];
}

float CEvents::getOahiResult(void)
{
	return oahiResult[detectionCriteria.hypopneaDropLimit][detectionCriteria.spO2DropLimit];
}

float CEvents::getOsa_ordiResult(void)
{
	return osa_ordiResult[detectionCriteria.hypopneaDropLimit][detectionCriteria.spO2DropLimit];
}

CString CEvents::getTimeOfDayAt(float _secs)
{
	COleDateTime dt = startRecordingClockTime;
	int status = dt.GetStatus();
	COleDateTimeSpan span(0, 0, 0,(int) _secs);
	status = span.GetStatus();
	dt += span; 
	status = dt.GetStatus();
	CString s = dt.Format(_T("%H:%M:%S"));
	return s;
}

CString CEvents::getEventsList(float _timeSec)
{
	int position = (int) (_timeSec / EVENT_TIMELINE_TIME_INTERVAL);
	if (position < 0) return _T("");
	if (position >= (int) eventTimelineVector.size()) return _T("");

	UINT flag = eventTimelineVector.at(position);
	CString s,s0;
	int lines = 0;

	//---If excluded or awake - don not check anything else
	if (flag & evntTypeExcluded) {
		int num = s.LoadString(IDS_EXCLUDED4);
		s0 += s;
		s0 += _T("\n");
		lines++;
		return s0;
	}
	if (flag & evntTypeAwake) {
		int num = s.LoadString(IDS_AWAKE4);
		s0 += s;
		s0 += _T("\n");
		lines++;
		return s0;
	}

	if (flag & evntTypeMixed) {
		int num = s.LoadString(IDS_MIXED2);
		s0 += s;
		s0 += _T("\n");
		lines++;
	}
	if (flag & evntTypeCentral) {
		int num = s.LoadString(IDS_CENTRAL2);
		s0 += s;
		s0 += _T("\n");
		lines++;
	}
	if (flag & evntTypeObstr) {
		int num = s.LoadString(IDS_OBSTRUCTIVE2);
		s0 += s;
		s0 += _T("\n");
		lines++;
	}
	if (flag & evntTypeHypoCentral) {
		int num = s.LoadString(IDS_CENTRALHYPOPNEA2);
		s0 += s;
		s0 += _T("\n");
		lines++;
	}
	if (flag & evntTypeHypoObstr) {
		int num = s.LoadString(IDS_OBSTRHYPOPNEA2);
		s0 += s;
		s0 += _T("\n");
		lines++;
	}
	if (flag & evntTypeRERA) {
		int num = s.LoadString(IDS_RERA4);
		s0 += s;
		s0 += _T("\n");
		lines++;
	}
	if ((developerMode) && (flag & evntTypeArousal)) {
		int num = s.LoadString(IDS_AROUSAL4);
		s0 += s;
		s0 += _T("\n");
		lines++;
	}
	if (flag & evntTypeSpO2Drop) {
		int num = s.LoadString(IDS_SPO24);
		s0 += s;
		s0 += _T("\n");
		lines++;
	}
	if (flag & evntTypeManMarker) {
		int num = s.LoadString(IDS_MANUA_MARKER);
		s0 += s;
		s0 += _T("\n");
		lines++;
	}
	if (flag & evntTypeSnoring) {
		int num = s.LoadString(IDS_SNORING4);
		s0 += s;
		s0 += _T("\n");
		lines++;
	}
	if (flag & evntTypeSwallow) {
		int num = s.LoadString(IDS_SWALLOW2);
		s0 += s;
		s0 += _T("\n");
		lines++;
	}
	if (flag & evntTypeImported) {
		int num = s.LoadString(IDS_IMPORTED4);
		s0 += s;
		s0 += _T("\n");
		lines++;
	}
	while (lines < 3) {
		s0 += _T("\n");
		lines++;
	}

	return s0;
}

float CEvents::getTotalSleepTime(void)
{
	return totalSleepTime;
}

void CEvents::redoEditAction(void)
{
	if (statePointer >= (int) (stateVector.size() - 1)) return;	// At end
	statePointer++;
	if (statePointer < 0) return;  // Should be impossible

	CMemFile *mfP = stateVector.at(statePointer);
	
	mfP->SeekToBegin();

	CArchive archive(mfP, CArchive::load);
	CEvnt::serializeVectorUint(archive, editEventTimelineVector.begin(), editEventTimelineVector.end(), &editEventTimelineVector);
	archive.Close();
	if (evntTypeAH & eventsBeingVisuallyEdited) {
		onOffVectorToBeginEndsVector(evntTypeMixed, &editEventTimelineVector);
		onOffVectorToBeginEndsVector(evntTypeCentral, &editEventTimelineVector);
		onOffVectorToBeginEndsVector(evntTypeObstr, &editEventTimelineVector);
		onOffVectorToBeginEndsVector(evntTypeHypoCentral, &editEventTimelineVector);
		onOffVectorToBeginEndsVector(evntTypeHypoObstr, &editEventTimelineVector);
	}
	else onOffVectorToBeginEndsVector(eventsBeingVisuallyEdited, &editEventTimelineVector);
}

void CEvents::undoEditAction(void)
{
	if (statePointer <= 0) return;							// Only one state

	statePointer--;
	CMemFile *mfP = stateVector.at(statePointer);

	mfP->SeekToBegin();

	CArchive archive(mfP, CArchive::load); 
	CEvnt::serializeVectorUint(archive, editEventTimelineVector.begin(), editEventTimelineVector.end(), &editEventTimelineVector);
	archive.Close(); if (evntTypeAH & eventsBeingVisuallyEdited) {
		onOffVectorToBeginEndsVector(evntTypeMixed, &editEventTimelineVector);
		onOffVectorToBeginEndsVector(evntTypeCentral, &editEventTimelineVector);
		onOffVectorToBeginEndsVector(evntTypeObstr, &editEventTimelineVector);
		onOffVectorToBeginEndsVector(evntTypeHypoCentral, &editEventTimelineVector);
		onOffVectorToBeginEndsVector(evntTypeHypoObstr, &editEventTimelineVector);
	}
	else onOffVectorToBeginEndsVector(eventsBeingVisuallyEdited, &editEventTimelineVector);
}

bool CEvents::getLeftToRedo(void)
{
	int size = stateVector.size();
	if (!size) return false;

	if (statePointer < (size - 1)) return true;
	return false;
}

bool CEvents::getLeftToUndo(void)
{
	int size = stateVector.size();
	if (!size) return false;

	if (statePointer > 0) return true;
	return false;
}

/*
Descriptions: Returns time in awake in the interval
*/
float CEvents::getAwakeTime(float _from, float _to)
{
	int num = awakeEventArray.GetCount();

	float time = .0f;
	for (int i = 0; i < num; ++i) {
		CAwakeEvnt *eP = awakeEventArray.GetAt(i);
		if ((_from >= eP->getFrom()) && (_from < eP->getTo())) {
			time += _to >= eP->getTo() ? eP->getTo() - _from : _to - _from;
		}
		else if (_from < eP->getFrom() && (_to > eP->getFrom())) {
			time += _to >= eP->getTo() ? eP->getTo() - eP->getFrom() : _to - eP->getFrom();
		}
	}
	return time;
}

/*
Description: Returns the time in excluded in the interval
*/
float CEvents::getExcludedTime(float _from, float _to)
{
	int num = excludedEventArray.GetCount();
	
	float time = .0f;
	for (int i = 0; i < num; ++i) {
		CExcludedEvnt *eP = excludedEventArray.GetAt(i);
		if ((_from >= eP->getFrom()) && (_from < eP->getTo())) {
			time += _to >= eP->getTo() ? eP->getTo() - _from : _to - _from;
		}
		else if (_from < eP->getFrom() && (_to > eP->getFrom())) {
			time += _to >= eP->getTo() ? eP->getTo() - eP->getFrom() : _to - eP->getFrom();
		}
	}
	return time;
}

/*
Description: Adds excluded events and resets according to time window vector, typically from catheter data
*/
void CEvents::addExcludedEventsFromTimeWindowVector(vector <TIME_WINDOW> *_v, int _spO2DropLimit)
{
	if (0 == _v->size()) return;

	//---Add new Excluded events
	int count = _v->size();
	vector <TIME_WINDOW>::iterator iT = _v->begin();
	for (; iT < _v->end(); ++iT) {
		CExcludedEvnt *eP = new CExcludedEvnt;
		eP->setRecordingStartTimeOfDay(startRecordingClockTime);
		eP->setFrom(iT->begin);
		eP->setTo(iT->end);
		excludedEventArray.Add(eP);
	}
	sortAndOrganiseEvents(evntTypeExcluded);
	
	//---Remove exclusion from all events
	unExcludeAllEvents(_spO2DropLimit);
	
	//--Set events according to excluded windows
	int num = excludedEventArray.GetCount();
	for (int i = num - 1; i >= 0; i--) {
		CExcludedEvnt *eP = excludedEventArray.GetAt(i);
		evaluateAllEventsWrtExclusionWindow(eP,_spO2DropLimit);
	}
}

/*
Description: Forces the time window between _from and _to to be excluded (_setExclude == true) or included (_setExclude == false)
*/
void CEvents::setExcludedEvent(bool _setExclude, float _from, float _to,bool _addToStack/* = true*/, bool _runPostExcludeAnalysis /*= true*/)
{
	//---Copy current status
	vector <TIME_WINDOW> v;
	int num = excludedEventArray.GetCount();
	for (int i = 0; i < num; i++) {
		CExcludedEvnt *eP = excludedEventArray.GetAt(i);
		TIME_WINDOW tw;
		tw.begin = eP->getFrom();
		tw.end = eP->getTo();
		v.push_back(tw);
	}

	//---Generate an OnOff Vector with bool values. False if included, true if excluded. One value per 1/10 sec.
	//	This now represents the excluded events
	
	//---Skip if length < 1 second
	if (maxStopTime < 1.0f) return;

	size_t num10 = (size_t) (maxStopTime * 10.0f);
	vector <bool> onOffV;
	onOffV.resize(num10, false);
	int numEvt = excludedEventArray.GetCount();
	for (int i = 0; i < numEvt; i++) {
		CExcludedEvnt *evP = excludedEventArray.GetAt(i);
		size_t pos0 = (size_t) (evP->getFrom() * 10.0f);
		size_t pos1 = (size_t) (evP->getTo() * 10.f);
		vector <bool>::iterator bI = onOffV.begin();
		if (pos0 >= onOffV.size()) break;
		bI += pos0;
		size_t fillLength = pos1 >= onOffV.size() ? onOffV.size() - pos0 : pos1 - pos0;
		fill_n(bI, fillLength, true);
	}

	//---Modify the OnOff Vector according to the new time window
	size_t newFrom = _from < .0f ? 0 : (size_t)(_from * 10.0f);
	size_t newTo = _to < .0f ? 0 : (size_t)(_to * 10.0f);
	vector <bool>::iterator bI = onOffV.begin();
	if (newFrom < onOffV.size()) {
		bI += newFrom;
		size_t fillLength = newTo >= onOffV.size() ? onOffV.size() - newFrom : newTo - newFrom;
		fill_n(bI, fillLength, _setExclude);
	}

	// Delete the excluded events
	num = excludedEventArray.GetCount();
	for (int i = num - 1; i >= 0; i--) {
		CExcludedEvnt *eP = excludedEventArray.GetAt(i);
		delete eP;
	}
	excludedEventArray.RemoveAll();

	//---Build new Excluded events array
	
	vector <bool>::iterator bI2 = onOffV.begin();
	vector <bool>::iterator bI1 = onOffV.begin();
	int pos = 0;
	float evStart, evStop;
	evStart = evStop = .0f;
	pos++;
	bI2++;
	bool exStatus = *bI1 == true ? true : false;
	for (; bI2 < onOffV.end(); ++bI2, ++pos, ++bI1) {
		if ((false == *bI1) && (true == *bI2)) {
			evStart = (float)pos / 10.f;
			exStatus = true;
		}
		else if ((true == *bI1) && (false == *bI2)) {
			evStop = (float)pos / 10.f;
			exStatus = false;
			CExcludedEvnt *eP = new CExcludedEvnt;
			eP->setRecordingStartTimeOfDay(startRecordingClockTime);
			eP->setFrom(evStart);
			eP->setTo(evStop);
			if (evStart <= .0f) eP->setAsFirst(true);			// This should be OK.
			if (fabs(evStop - maxStopTime) <= 2.0f)
				eP->setAsLast(true);		// Only has to be close
			excludedEventArray.Add(eP); 
		}
	}
	if (true == exStatus) {
		evStop = (float)pos / 10.f;
		exStatus = false;
		CExcludedEvnt *eP = new CExcludedEvnt;
		eP->setRecordingStartTimeOfDay(startRecordingClockTime);
		eP->setFrom(evStart);
		eP->setTo(evStop);
		if (evStart <= .0f) eP->setAsFirst(true);	// This should be OK
		if (fabs(evStop - maxStopTime) <= 2.0f)
			eP->setAsLast(true);		// Only has to be close
		excludedEventArray.Add(eP);
	}

	//---Remove exclusion from all events
	unExcludeAllEvents(detectionCriteria.spO2DropLimit);

	//--Set events according to excluded windows
	int numExcl = excludedEventArray.GetCount();
	for (int i = numExcl - 1; i >= 0; i--) {
		CExcludedEvnt *eP = excludedEventArray.GetAt(i);
		evaluateAllEventsWrtExclusionWindow(eP,detectionCriteria.spO2DropLimit);
	}

	//---Update effStart and effStop 
	if (2 <= numExcl) {	// There must be 2 or more excluded events, last and first
		for (int i = numExcl - 1; i >= 0; i--) {
			CExcludedEvnt *eP = excludedEventArray.GetAt(i);
			if (true == eP->getIsFirst()) effectiveStartTime = eP->getTo();
			if (true == eP->getIsLast()) effectiveStopTime = eP->getFrom();
		}
	}

	if (_runPostExcludeAnalysis) 
		postExcludeAnalysis();
}

CString CEvents::getEffectiveSleepTimeHHMM(void)
{
	CString s;
	int h = (int)(totalSleepTime / 3600);
	float min = (float)(totalSleepTime - h * 3600) / 60.0f;
	s.Format(_T("%1d:%02d"), h, (int)min);
	return s;
}

CString CEvents::getEffectiveSleepTimeHHMMSS(void)
{
	CString s;
	int h = (int)(totalSleepTime / 3600);
	float min = (float)(totalSleepTime - h * 3600) / 60.0f;
	float sec = (float)(totalSleepTime - h * 3600 - (int)min * 60);
	s.Format(_T("%02d:%02d:%02d"), h, (int)min, (int)sec);
	return s;
}

void CEvents::getCriteriaFromRegistry(void)
{
	//---Read from Registry
	CXTPRegistryManager reg;
	double d;

	BOOL OK;
	
	detectionCriteria.spO2DropLimit = reg.GetProfileInt(regSectionCriteria, regSpO2DropLimit,spO2DropLimit4);
	
	detectionCriteria.hypopneaDropLimit = (int) reg.GetProfileInt(regSectionCriteria, regHypopneaDroplimit,hypopneaDropLimit30);

	detectionCriteria.splitCentralAndObstrHypo	= TRUE; // reg.GetProfileInt(regSectionCriteria,regSplitCentralAndObstrHypo,FALSE);
	
	OK = reg.GetProfileDouble(regSectionCriteria,regPOESalarmLine,&d);
	detectionCriteria.pOESalarmLine = OK ? (float) d : POES_ALARM_LINE;
	OK = reg.GetProfileDouble(regSectionCriteria,regLevelDetectionMargin,&d);
	detectionCriteria.levelDetectionMargin = OK ? (float) d : LEVEL_DETECTION_MARGIN;
	OK = reg.GetProfileDouble(regSectionCriteria,regApneaDefaultDuration,&d);
	detectionCriteria.apneaDefaultDuration = OK ? (float) d : APNEA_DEFAULT_DURATION;
	OK = reg.GetProfileDouble(regSectionCriteria,regApneaShortCentralDuration,&d);
	detectionCriteria.apneaShortCentralDuration = OK ? (float) d : APNEA_HALF_DURATION;
	OK = reg.GetProfileDouble(regSectionCriteria,regApneaShortObstrDuration,&d);
	detectionCriteria.apneaShortObstrDuration = OK ? (float) d : APNEA_HALF_DURATION;
	OK = reg.GetProfileDouble(regSectionCriteria,regMaxApneaDur,&d);
	detectionCriteria.maxApneaDur = OK ? (float) d : MAX_APNEA_DUR;
	OK = reg.GetProfileDouble(regSectionCriteria,regMaxHypopneaDuration,&d);
	detectionCriteria.maxHypopneaDuration = OK ? (float) d : MAX_HYPOPNEA_DUR;
	OK = reg.GetProfileDouble(regSectionCriteria,regtimeInBedLowLimit,&d);
	detectionCriteria.timeInBedLowLimit = OK ? (float) d : DEF_TIME_IN_BED_LOW_LIMIT;	
	OK = reg.GetProfileDouble(regSectionCriteria,regExtendedWindowForExclusionOfAwake,&d);
	detectionCriteria.extendedWindowForExclusionOfAwake = OK ? (float) d : EXTENDED_WINDOW_FOR_EXCLUSION_OF_AWAKE;
	OK = reg.GetProfileDouble(regSectionCriteria,regActimeterAlwaysAwakeLimit,&d);
	detectionCriteria.actimeterAlwaysAwakeLimit = OK ? (float) d : ACTIMETER_ALWAYS_AWAKE_LIMIT;
	OK = reg.GetProfileDouble(regSectionCriteria,regActimeterRelativeAwakeLimit,&d);
	detectionCriteria.actimeterAboveBaselineLimit = OK ? (float) d : ACTIMETER_ABOVE_BASELINE_LIMIT;
	OK = reg.GetProfileDouble(regSectionCriteria,regRespEffortFrqPeakWindowLength,&d);
	detectionCriteria.respiratoryEffortFrqPeakWindowLength = OK ? (float) d : RESP_EFF_FRQ_PEAK_WINDOW_LENGTH;
	OK = reg.GetProfileDouble(regSectionCriteria,regAdmittancePeakWindowLength,&d);
	detectionCriteria.admittancePeakWindowLength = OK ? (float) d : ADMITTANCE_PEAK_WINDOW_LENGTH;
	OK = reg.GetProfileDouble(regSectionCriteria,regPulseRatePeakWindowLength,&d);
	detectionCriteria.pulseRatePeakWindowLength = OK ? (float) d : PULSERATE_PEAK_WINDOW_LENGTH;

	OK = reg.GetProfileDouble(regSectionCriteria,regPulseRateWindowSlack,&d);
	detectionCriteria.pulseRateWindowSlack = OK ? (float) d : PULSERATE_WINDOW_SLACK;

	OK = reg.GetProfileDouble(regSectionCriteria,regArousalDetectionTimeStep,&d);
	detectionCriteria.arousalDetectionTimeStep = OK ? (float) d : AROUSAL_DETECTION_TIME_STEP;
	OK = reg.GetProfileDouble(regSectionCriteria,regRespFrqPeakFactor,&d);
	detectionCriteria.respFrqPeakFactor = OK ? (float) d : RESP_FRQ_PEAK_FACTOR;

	OK = reg.GetProfileDouble(regSectionCriteria,regAdmittancePeakFactorHead,&d);
	detectionCriteria.admittancePeakFactorHead = OK ? (float) d : ADMITTANCE_PEAK_FACTOR_HEAD;
	OK = reg.GetProfileDouble(regSectionCriteria,regAdmittancePeakFactorTail,&d);
	detectionCriteria.admittancePeakFactorTail = OK ? (float) d : ADMITTANCE_PEAK_FACTOR_TAIL;

	OK = reg.GetProfileDouble(regSectionCriteria,regPulseRatePeakIncrease,&d);
	detectionCriteria.pulseRatePeakIncrease = OK ? (float) d : PULSERATE_PEAK_INCREASE;		
	OK = reg.GetProfileDouble(regSectionCriteria,regReIncreaseDuration,&d);
	detectionCriteria.REIncreaseDuration = OK ? (float) d : RE_INCREASE_DEFAULT_DURATION;	
	
	OK = reg.GetProfileDouble(regSectionCriteria,regReSeekStep,&d);
	detectionCriteria.RESeekStepSize = OK ? (float) d : RE_INCREASE_SEEK_STEP;
	OK = reg.GetProfileDouble(regSectionCriteria,regReSeekSlack,&d);
	detectionCriteria.RESeekSlack = OK ? (float) d : RE_INCREASE_SEEK_SLACK;
	
	OK = reg.GetProfileDouble(regSectionCriteria,regContactMicSnoringThreshold,&d);
	detectionCriteria.contactMicSnoringThreshold = OK ? (float) d : CONTACT_MIC_SNORING_THRESHOLD;
	OK = reg.GetProfileDouble(regSectionCriteria,regLinRegRSquaredLimit,&d);
	detectionCriteria.linRegRSquaredLimit = OK ? (float) d : LIN_REG_R_SQUARED_LIMIT;
	OK = reg.GetProfileDouble(regSectionCriteria,regSnoringAverageWindowLength,&d);
	detectionCriteria.snoringAverageWindowLength = OK ? (float) d : SNORING_AVERAGE_WINDOW_LENGTH;
	OK = reg.GetProfileDouble(regSectionCriteria,regBaselineLength,&d);
	detectionCriteria.baselineLength = OK ? (float) d : BASELINE_LENGTH;
	OK = reg.GetProfileDouble(regSectionCriteria,regSpO2stabilityLimit,&d);
	detectionCriteria.spO2stabilityLimit = OK ? (float) d : SPO2_STABILITY_LIMIT;

	OK = reg.GetProfileDouble(regSectionCriteria, regT0EnvPercentStabilityLimit, &d);
	detectionCriteria.t0EnvPercentStabilityLimit = OK ? (float)d : T0T1ENV_PERCENT_STABILITY_LIMIT;
	OK = reg.GetProfileDouble(regSectionCriteria,regT1EnvPercentStabilityLimit,&d);
	detectionCriteria.t1EnvPercentStabilityLimit = OK ? (float) d : T0T1ENV_PERCENT_STABILITY_LIMIT;

	OK = reg.GetProfileDouble(regSectionCriteria,regPressureEnvPercentStabilityLimit,&d);
	detectionCriteria.pressureEnvPercentStabilityLimit = OK ? (float) d : PRESSENV_PERCENT_STABILITY_LIMIT;
	OK = reg.GetProfileDouble(regSectionCriteria,regFlowStopFraction,&d);
	detectionCriteria.flowStopFraction = OK ? (float) d : FLOW_STOP_FRACTION;
	OK = reg.GetProfileDouble(regSectionCriteria,regPressureDeadFraction,&d);
	detectionCriteria.pressureDeadFraction = OK ? (float) d : PRESSURE_DEAD_FRACTION;
	OK = reg.GetProfileDouble(regSectionCriteria,regSpO2DropSeekWindowLength,&d);
	detectionCriteria.spO2DropSeekWindowLength = OK ? (float) d : WINDOW_LENGTH_FOR_SPO2DROP_SEEK ;
	
	OK = reg.GetProfileDouble(regSectionCriteria,regSpO2DropSeekWindowStart,&d);
	detectionCriteria.spO2DropSeekWindowStart = OK ? (float) d : WINDOW_START_FOR_SPO2DROP_SEEK ;

	OK = reg.GetProfileDouble(regSectionCriteria,regMaxEventLength,&d);
	detectionCriteria.maxEventLength = OK ? (float) d : MAX_EVENT_LENGTH ;

	OK = reg.GetProfileDouble(regSectionCriteria,regGradientUpperLimit,&d);
	detectionCriteria.gradientUpperLimit = OK ? (float) d : GRADIENT_UPPER_LIMIT;
	
	OK = reg.GetProfileDouble(regSectionCriteria,regGradientLowerLimit,&d);
	detectionCriteria.gradientLowerLimit = OK ? (float) d : GRADIENT_LOWER_LIMIT;
	
	OK = reg.GetProfileDouble(regSectionCriteria,regHalfArousalLength,&d);
	detectionCriteria.halfArousalLength = OK ? (float) d : HALF_AROUSAL_LENGTH;

	OK = reg.GetProfileDouble(regSectionCriteria, regP0RiseCoefficientLimit, &d);
	detectionCriteria.p0RiseCoefficientLimit = OK ? (float)d : P0_RISE_COEFFICIENTLIMIT;
	
	OK = reg.GetProfileDouble(regSectionCriteria, regFlowIncreaseAtArousal, &d);
	detectionCriteria.flowIncreaseAtArousal = OK ? (float)d : FLOW_INCREASE_AT_AROUSAL;

	OK = reg.GetProfileDouble(regSectionCriteria, regAngleCoeffObstrVsCentralHypo, &d);
	detectionCriteria.angleCoeffObstrVsCentralHypo = OK ? (float)d : ANGLE_COEFF_OBSTR_VS_CENTRAL_HYPO;

	OK = reg.GetProfileDouble(regSectionCriteria, regAwakeMixeFactor, &d);
	detectionCriteria.awakeMixeFactor = OK ? (float)d : AWAKE_MIXE_FACTOR;
	OK = reg.GetProfileDouble(regSectionCriteria, regAwakeObstFactor, &d);
	detectionCriteria.awakeObstFactor = OK ? (float)d : AWAKE_OBST_FACTOR;
	OK = reg.GetProfileDouble(regSectionCriteria, regAwakeCentFactor, &d);
	detectionCriteria.awakeCentFactor = OK ? (float)d : AWAKE_CENT_FACTOR;
	OK = reg.GetProfileDouble(regSectionCriteria, regAwakeHypoFactor, &d);
	detectionCriteria.awakeHypoFactor = OK ? (float)d : AWAKE_HYPO_FACTOR;
	OK = reg.GetProfileDouble(regSectionCriteria, regAwakeDesaFactor, &d);
	detectionCriteria.awakeDesaFactor = OK ? (float)d : AWAKE_DESA_FACTOR;
	OK = reg.GetProfileDouble(regSectionCriteria, regAwakeRERAFactor, &d);
	detectionCriteria.awakeRERAFactor = OK ? (float)d : AWAKE_RERA_FACTOR;
	OK = reg.GetProfileDouble(regSectionCriteria, regAwakeSwalFactor, &d);
	detectionCriteria.awakeSwalFactor = OK ? (float)d : AWAKE_SWAL_FACTOR;
	OK = reg.GetProfileDouble(regSectionCriteria, regAwakeSnorFactor, &d);
	detectionCriteria.awakeSnorFactor = OK ? (float)d : AWAKE_SNOR_FACTOR;
	OK = reg.GetProfileDouble(regSectionCriteria, regAwakeActiFactor, &d);
	detectionCriteria.awakeActiFactor = OK ? (float)d : AWAKE_ACTI_FACTOR;
	OK = reg.GetProfileDouble(regSectionCriteria, regAwakeTActFactor, &d);
	detectionCriteria.awakeTActFactor = OK ? (float)d : AWAKE_TACT_FACTOR;
	OK = reg.GetProfileDouble(regSectionCriteria, regAwakeStanFactor, &d);
	detectionCriteria.awakeStanFactor = OK ? (float)d : AWAKE_STAN_FACTOR;
	OK = reg.GetProfileDouble(regSectionCriteria, regTorsoActimeterLimit, &d);
	detectionCriteria.torsoActimeterLimit = OK ? (float)d : TORSO_ACTIMETER_LIMIT;
	OK = reg.GetProfileDouble(regSectionCriteria, regFlowDetectBalanceFactor, &d);
	detectionCriteria.flowDetectBalanceFactor = OK ? (float)d : FLOW_DETECT_BALANCE_FACTOR;

}

void CEvents::saveCriteriaToRegistry(void)
{
	//--Consider to change this
	CXTPRegistryManager reg;
	reg.WriteProfileInt(regSectionCriteria,regSpO2DropLimit, detectionCriteria.spO2DropLimit);
	reg.WriteProfileInt(regSectionCriteria,regSplitCentralAndObstrHypo,detectionCriteria.splitCentralAndObstrHypo);
	reg.WriteProfileInt(regSectionCriteria, regHypopneaDroplimit, detectionCriteria.hypopneaDropLimit);
	
	//d = (double) detectionCriteria.hypopneaLimitFraction;
	//reg.WriteProfileDouble(regSectionCriteria,regHypopneaLimitFraction,&d);

	//--- These are not for editing
	//d = (double) detectionCriteria.pOESalarmLine;
	//reg.WriteProfileDouble(regSectionCriteria,regPOESalarmLine,&d);
	//d = (double) detectionCriteria.levelDetectionMargin;
	//reg.WriteProfileDouble(regSectionCriteria,regLevelDetectionMargin,&d);
	//d = (double) detectionCriteria.apneaDefaultDuration;
	//reg.WriteProfileDouble(regSectionCriteria,regApneaDefaultDuration,&d);
	//d = (double) detectionCriteria.apneaShortCentralDuration;
	//reg.WriteProfileDouble(regSectionCriteria,regApneaShortCentralDuration,&d);
	//d = (double) detectionCriteria.apneaShortObstrDuration;
	//reg.WriteProfileDouble(regSectionCriteria,regApneaShortObstrDuration,&d);
	//d = (double) detectionCriteria.maxApneaDur;
	//reg.WriteProfileDouble(regSectionCriteria,regMaxApneaDur,&d);
	//d = (double) detectionCriteria.maxHypopneaDuration;
	//reg.WriteProfileDouble(regSectionCriteria,regMaxHypopneaDuration,&d);
	//d = (double) detectionCriteria.timeInBedLowLimit;
	//reg.WriteProfileDouble(regSectionCriteria,regtimeInBedLowLimit,&d);	
	//d = (double) detectionCriteria.extendedWindowForExclusionOfAwake;
	//reg.WriteProfileDouble(regSectionCriteria,regExtendedWindowForExclusionOfAwake,&d);	
	//d =  (double) detectionCriteria.actimeterAlwaysAwakeLimit;
	//reg.WriteProfileDouble(regSectionCriteria,regActimeterAlwaysAwakeLimit,&d);	
	//d = (double) detectionCriteria.actimeterRelativeAwakeLimit;
	//reg.WriteProfileDouble(regSectionCriteria,regActimeterRelativeAwakeLimit,&d);
	//d = (double) detectionCriteria.respiratoryEffortFrqPeakWindowLength;	
	//reg.WriteProfileDouble(regSectionCriteria,regRespEffortFrqPeakWindowLength,&d);
	//d = (double) detectionCriteria.admittancePeakWindowLength;
	//reg.WriteProfileDouble(regSectionCriteria,regAdmittancePeakWindowLength,&d);
	//d = (double) detectionCriteria.pulseRatePeakWindowLength;
	//reg.WriteProfileDouble(regSectionCriteria,regPulseRatePeakWindowLength,&d);
	//d = (double) detectionCriteria.pulseRateWindowSlack;
	//reg.WriteProfileDouble(regSectionCriteria,regPulseRateWindowSlack,&d);
	//d = (double) detectionCriteria.arousalDetectionTimeStep;
	//reg.WriteProfileDouble(regSectionCriteria,regArousalDetectionTimeStep,&d);	
	//d = (double) detectionCriteria.respFrqPeakFactor;
	//reg.WriteProfileDouble(regSectionCriteria,regRespFrqPeakFactor,&d);
	//d = (double) detectionCriteria.admittancePeakFactorHead;	
	//reg.WriteProfileDouble(regSectionCriteria,regAdmittancePeakFactorHead,&d);	
	//d = (double) detectionCriteria.pulseRatePeakIncrease;
	//reg.WriteProfileDouble(regSectionCriteria,regPulseRatePeakIncrease,&d);
	//d = (double) detectionCriteria.REIncreaseDuration;
	//reg.WriteProfileDouble(regSectionCriteria,regReIncreaseDuration,&d);
	
	//d = (double) detectionCriteria.RESeekSlack;
	//reg.WriteProfileDouble(regSectionCriteria,regReSeekSlack,&d);
	//d = (double) detectionCriteria.RESeekStepSize;
	//reg.WriteProfileDouble(regSectionCriteria,regReSeekStep,&d);

	//d = (double) detectionCriteria.contactMicSnoringThreshold;
	//reg.WriteProfileDouble(regSectionCriteria,regContactMicSnoringThreshold,&d);
	//d = (double) detectionCriteria.linRegRSquaredLimit;
	//reg.WriteProfileDouble(regSectionCriteria,regLinRegRSquaredLimit,&d);
	//d = (double) detectionCriteria.snoringAverageWindowLength;
	//reg.WriteProfileDouble(regSectionCriteria,regSnoringAverageWindowLength,&d);
	//d = (double) detectionCriteria.baselineLength;
	//reg.WriteProfileDouble(regSectionCriteria,regBaselineLength,&d);
	//d = (double) detectionCriteria.spO2stabilityLimit;
	//reg.WriteProfileDouble(regSectionCriteria,regSpO2stabilityLimit,&d);
	//d = (double) detectionCriteria.t1EnvPercentStabilityLimit;
	//reg.WriteProfileDouble(regSectionCriteria,regT1EnvPercentStabilityLimit,&d);
	//d = (double) detectionCriteria.pressureEnvPercentStabilityLimit;
	//reg.WriteProfileDouble(regSectionCriteria,regPressureEnvPercentStabilityLimit,&d);
	//d = (double) detectionCriteria.flowStopFraction;
	//reg.WriteProfileDouble(regSectionCriteria,regFlowStopFraction,&d);
	//d = (double) detectionCriteria.pressureDeadFraction;
	//reg.WriteProfileDouble(regSectionCriteria,regPressureDeadFraction,&d);
	//d = (double) detectionCriteria.spO2DropSeekWindowLength;
	//reg.WriteProfileDouble(regSectionCriteria,regSpO2DropSeekWindowLength,&d);
	//d = (double) detectionCriteria.spO2DropSeekWindowStart;
	//reg.WriteProfileDouble(regSectionCriteria,regSpO2DropSeekWindowStart,&d);
	//d = (double) detectionCriteria.maxEventLength;
	//reg.WriteProfileDouble(regSectionCriteria,regMaxEventLength,&d);
	//d = (double) detectionCriteria.gradientLowerLimit;
	//reg.WriteProfileDouble(regSectionCriteria,regGradientLowerLimit,&d);
	//d = (double) detectionCriteria.gradientUpperLimit;
	//reg.WriteProfileDouble(regSectionCriteria,regGradientUpperLimit,&d);
	//d = (double) detectionCriteria.halfArousalLength;
	//reg.WriteProfileDouble(regSectionCriteria,regHalfArousalLength,&d);

	//d = detectionCriteria.p0RiseCoefficientLimit;
	//reg.WriteProfileDouble(regSectionCriteria, regP0RiseCoefficientLimit, &d);

	//d = detectionCriteria.flowIncreaseAtArousal;
	//reg.WriteProfileDouble(regSectionCriteria, regFlowIncreaseAtArousal, &d);

	//d = detectionCriteria.angleCoeffObstrVsCentralHypo;
	//reg.WriteProfileDouble(regSectionCriteria, regAngleCoeffObstrVsCentralHypo, &d);

	//d = detectionCriteria.awakeMixeFactor;
	//reg.WriteProfileDouble(regSectionCriteria, regAwakeMixeFactor, &d);
	//
	//d = detectionCriteria.awakeObstFactor;
	//reg.WriteProfileDouble(regSectionCriteria, regAwakeObstFactor, &d);
	//
	//d = detectionCriteria.awakeCentFactor;
	//reg.WriteProfileDouble(regSectionCriteria, regAwakeCentFactor, &d);
	//
	//d = detectionCriteria.awakeHypoFactor;
	//reg.WriteProfileDouble(regSectionCriteria, regAwakeHypoFactor, &d);
	//
	//d = detectionCriteria.awakeDesaFactor;
	//reg.WriteProfileDouble(regSectionCriteria, regAwakeDesaFactor, &d);
	//
	//d = detectionCriteria.awakeRERAFactor;
	//reg.WriteProfileDouble(regSectionCriteria, regAwakeRERAFactor, &d);
	//
	//d = detectionCriteria.awakeSwalFactor;
	//reg.WriteProfileDouble(regSectionCriteria, regAwakeSwalFactor, &d);

	//
	//d = detectionCriteria.awakeSnorFactor;
	//reg.WriteProfileDouble(regSectionCriteria, regAwakeSnorFactor, &d);
	//
	//d = detectionCriteria.awakeActiFactor;
	//reg.WriteProfileDouble(regSectionCriteria, regAwakeActiFactor, &d);
	//
	//d = detectionCriteria.awakeTActFactor;
	//reg.WriteProfileDouble(regSectionCriteria, regAwakeTActFactor, &d);
	//
	//d = detectionCriteria.awakeStanFactor;
	//reg.WriteProfileDouble(regSectionCriteria, regAwakeStanFactor, &d);
	//d = detectionCriteria.torsoActimeterLimit;
	//reg.WriteProfileDouble(regSectionCriteria, regTorsoActimeterLimit, &d);
}

DETECTION_CRITERIA CEvents::getUsedDetectionCriteria(void)
{
	return detectionCriteria;
}

DETECTION_CRITERIA CEvents::getEditedDetectionCriteria(void)
{
	return editedDetectionCriteria;
}

/*
Description:
_evnt is removed (not deleted) from the event array
A new event is created wich is a copy of _evnt
This is added to the event array
*/
CEvnt *CEvents::removeAndReplaceEvent(CEvnt *_evnt)
{
	int type = _evnt->getEventType();
	if (type & evntTypeNone) return NULL;
	else if (type & evntTypeMixed)	{
		int num = mixedEventArray.GetCount();
		for (int i = 0 ; i < num ; i++) {
			CMixedEvnt *eP = mixedEventArray.GetAt(i);
			if (eP == _evnt) {
				CMixedEvnt *nE = new CMixedEvnt();
				nE->copyFrom((CMixedEvnt *) _evnt);
				mixedEventArray.RemoveAt(i);
				mixedEventArray.Add(nE);
				return (CEvnt *) nE;
			}
		}
	}
	else if (type & evntTypeCentral) {
		int num = centralEventArray.GetCount();
		for (int i = 0 ; i < num ; i++) {
			CCentralEvnt *eP = centralEventArray.GetAt(i);
			if (eP == _evnt) {
				CCentralEvnt *nE = new CCentralEvnt();
				nE->copyFrom((CCentralEvnt *) _evnt);
				centralEventArray.RemoveAt(i);
				centralEventArray.Add(nE);
				return (CEvnt *) nE;
			}
		}
	}	
	else if (type & evntTypeObstr) {
		int num = obstrEventArray.GetCount();
		for (int i = 0 ; i < num ; i++) {
			CObstrEvnt *eP = obstrEventArray.GetAt(i);
			if (eP == _evnt) {
				CObstrEvnt *nE = new CObstrEvnt();
				nE->copyFrom((CObstrEvnt *) _evnt);
				obstrEventArray.RemoveAt(i);
				obstrEventArray.Add(nE);
				return (CEvnt *) nE;
			}
		}
	}		
	else if ((type & evntTypeHypoObstr) || (type & evntTypeHypoCentral)) {
		int num = hypoEventArrayCurrentP->GetCount();
		for (int i = 0 ; i < num ; i++) {
			CHypoEvnt *eP = hypoEventArrayCurrentP->GetAt(i);
			if (eP == _evnt) {
				CHypoEvnt *nE = new CHypoEvnt();
				nE->copyFrom((CHypoEvnt *) _evnt);
				hypoEventArrayCurrentP->RemoveAt(i);
				hypoEventArrayCurrentP->Add(nE);
				return (CEvnt *) nE;
			}
		}
	}	
	else if (type & evntTypeRERA) {

		int num = arousalEventArrayCurrentP->GetCount();
		for (int i = 0; i < num; i++) {
			CArousalEvnt *eP = arousalEventArrayCurrentP->GetAt(i);
			CRERAEvnt *rP = eP->getRERAEvnt();
			if (rP == _evnt) {
				CArousalEvnt *nE = new CArousalEvnt();
				nE->copyFrom((CArousalEvnt *)_evnt);
				arousalEventArrayCurrentP->RemoveAt(i);
				arousalEventArrayCurrentP->Add(nE);
				return (CEvnt *)nE;
			}
		}
	}	
	else if (type & evntTypeArousal) {
		int num = arousalEventArrayCurrentP->GetCount();
		for (int i = 0 ; i < num ; i++) {
			CArousalEvnt *eP = arousalEventArrayCurrentP->GetAt(i);
			if (eP == _evnt) {
				CArousalEvnt *nE = new CArousalEvnt();
				nE->copyFrom((CArousalEvnt *) _evnt);
				arousalEventArrayCurrentP->RemoveAt(i);
				arousalEventArrayCurrentP->Add(nE);
				return (CEvnt *) nE;
			}
		}
	}		
	else if (type & evntTypeSpO2Drop) {
		int num = spO2DropEventArrayCurrentP->GetCount();
		for (int i = 0 ; i < num ; i++) {
			CSpO2DropEvnt *eP = spO2DropEventArrayCurrentP->GetAt(i);
			if (eP == _evnt) {
				CSpO2DropEvnt *nE = new CSpO2DropEvnt(detectionCriteria.spO2DropLimit);
				nE->copyFrom((CSpO2DropEvnt *) _evnt);
				spO2DropEventArrayCurrentP->RemoveAt(i);
				spO2DropEventArrayCurrentP->Add(nE);
				return (CEvnt *) nE;
			}
		}
	}	
	else if (type & evntTypeManMarker) {
		int num = manualMarkerEventArray.GetCount();
		for (int i = 0 ; i < num ; i++) {
			CManualMarkerEvnt *eP = manualMarkerEventArray.GetAt(i);
			if (eP == _evnt) {
				CManualMarkerEvnt *nE = new CManualMarkerEvnt();
				nE->copyFrom((CManualMarkerEvnt *) _evnt);
				manualMarkerEventArray.RemoveAt(i);
				manualMarkerEventArray.Add(nE);
				return (CEvnt *) nE;
			}
		}
	}	
	else if (type & evntTypeSnoring) {
		int num = snoringEventArray.GetCount();
		for (int i = 0 ; i < num ; i++) {
			CSnoringEvnt *eP = snoringEventArray.GetAt(i);
			if (eP == _evnt) {
				CSnoringEvnt *nE = new CSnoringEvnt(.0f, .0f, defDateTime, posUndefined, autoDetection);
				nE->copyFrom((CSnoringEvnt *) _evnt);
				snoringEventArray.RemoveAt(i);
				snoringEventArray.Add(nE);
				return (CEvnt *) nE;
			}
		}
	}		
	else if (type & evntTypeAwake) {
		int num = awakeEventArray.GetCount();
		for (int i = 0 ; i < num ; i++) {
			CAwakeEvnt *eP = awakeEventArray.GetAt(i);
			if (eP == _evnt) {
				CAwakeEvnt *nE = new CAwakeEvnt(.0f, .0f, defDateTime, posUndefined, autoDetection);
				nE->copyFrom((CAwakeEvnt *) _evnt);
				awakeEventArray.RemoveAt(i);
				awakeEventArray.Add(nE);
				return (CEvnt *) nE;
			}
		}
	}				
	else if (type & evntTypeExcluded) {
		int num = excludedEventArray.GetCount();
		for (int i = 0 ; i < num ; i++) {
			CExcludedEvnt *eP = excludedEventArray.GetAt(i);
			if (eP == _evnt) {
				CExcludedEvnt *nE = new CExcludedEvnt();
				nE->copyFrom((CExcludedEvnt *) _evnt);
				excludedEventArray.RemoveAt(i);
				excludedEventArray.Add(nE);
				return (CEvnt *) nE;
			}
		}
	}			
	return NULL;
}

int CEvents::getCountMixEvents(void)		
{ 
	int num = mixedEventArray.GetCount();
	int cnt = 0;
	for (int i = 0; i < num ; i++) {
		CMixedEvnt *evtP = mixedEventArray.GetAt(i);
		if (evtP->getIsActive()) cnt++;
	}
	return cnt; 
}

int CEvents::getCountCenEvents(void)		
{
	int num = centralEventArray.GetCount();
	int cnt = 0;
	for (int i = 0; i < num;  i++) {
		CCentralEvnt *evtP = centralEventArray.GetAt(i);
		if (evtP->getIsActive()) cnt++;
	}
	return cnt;
}

int CEvents::getCountObsEvents(void)		
{
	int num = obstrEventArray.GetCount();
	int cnt = 0;
	for (int i = 0; i < num; i++) {
		CObstrEvnt *evtP = obstrEventArray.GetAt(i);
		if (evtP->getIsActive()) cnt++;
	}
	return cnt;
}


/*
Description: Returns a float that canb be used for testing etc based on the enum value representing the SpO2DropLimit
*/
float CEvents::getFloatSpO2DropLimit(void)
{
	float ret = 4.0f;
	switch (detectionCriteria.spO2DropLimit) {
	case spO2DropLimit3:
		ret = 3.0f;
		break;
	case spO2DropLimit4:
		ret = 4.0f;
		break;
	}
	return ret;
}

/*
Description: Copies from one event type to another event type
_source: Source event
_dest: Destination event
Returns: true if ok, false if not ok
*/
bool CEvents::copyBetweenEventTypes(CEvnt *_source, CEvnt *_dest)
{
	int sourceType = _source->getEventType();
	int destType = _dest->getEventType();

	ASSERT((evntTypeMixed == sourceType) ||
		(evntTypeCentral == sourceType) ||
		(evntTypeObstr == sourceType) ||
		(evntTypeHypoObstr == sourceType) ||
		(evntTypeHypoCentral == sourceType) ||
		(evntTypeImported == sourceType));

	ASSERT(	(evntTypeMixed == destType) ||
			(evntTypeCentral == destType) ||
			(evntTypeObstr == destType) ||
			(evntTypeHypoObstr == destType) ||
			(evntTypeImported == destType));

	bool ret = false;
	switch (sourceType) {
	case evntTypeMixed :
		ret = copyMixedEventTo((CMixedEvnt *) _source,_dest);
		break;
	case evntTypeCentral :
		ret = copyCentralEventTo((CCentralEvnt *)_source,_dest);
		break;
	case evntTypeObstr :
		ret = copyObstrEventTo((CObstrEvnt *)_source,_dest);
		break;
	case evntTypeImported:
		ret = copyImportedEventTo((CImportedEvnt *)_source, _dest);
		break;
	case evntTypeHypoObstr :
	case evntTypeHypoCentral:
		ret = copyHypoEventTo((CHypoEvnt *)_source,_dest);
		break;
	default:
		break;
	}
	return ret;
}

void CEvents::actOnManualDelete(void)
{
	fillStartsAndStopsVector();
}

void CEvents::actOnChangedDetectOptions(void)
{
	fillStartsAndStopsVector();
}

//bool CEvents::anyOngoingVisualEdits(void)
//{
//	return eventsBeingVisuallyEdited != evntTypeNone;
//}

void CEvents::startVisualEventEditing(UINT _eventType)
{
	if (_eventType & evntTypeAH) {
		editEventsWithoutLevelBeginsEnds[evCentral].clear();
		editEventsWithoutLevelBeginsEnds[evCentral].resize(eventsWithoutLevelBeginsEnds[evCentral].size());
		copy(eventsWithoutLevelBeginsEnds[evCentral].begin(), eventsWithoutLevelBeginsEnds[evCentral].end(),
			editEventsWithoutLevelBeginsEnds[evCentral].begin());

		editEventsWithoutLevelBeginsEnds[evCentralHypo].clear();
		editEventsWithoutLevelBeginsEnds[evCentralHypo].resize(eventsWithoutLevelBeginsEnds[evCentralHypo].size());
		copy(eventsWithoutLevelBeginsEnds[evCentralHypo].begin(), eventsWithoutLevelBeginsEnds[evCentralHypo].end(),
			editEventsWithoutLevelBeginsEnds[evCentralHypo].begin());

		for (int i = 0; i < bgLevelTypeCount; i++) {
			editEventsWithLevelBeginsEnds[evObstr][i].clear();
			editEventsWithLevelBeginsEnds[evObstr][i].resize(eventsWithLevelBeginsEnds[evObstr][i].size());
			copy(eventsWithLevelBeginsEnds[evObstr][i].begin(), eventsWithLevelBeginsEnds[evObstr][i].end(),
				editEventsWithLevelBeginsEnds[evObstr][i].begin());

			editEventsWithLevelBeginsEnds[evMix][i].clear();
			editEventsWithLevelBeginsEnds[evMix][i].resize(eventsWithLevelBeginsEnds[evMix][i].size());
			copy(eventsWithLevelBeginsEnds[evMix][i].begin(), eventsWithLevelBeginsEnds[evMix][i].end(),
				editEventsWithLevelBeginsEnds[evMix][i].begin());

			editEventsWithLevelBeginsEnds[evObstrHypo][i].clear();
			editEventsWithLevelBeginsEnds[evObstrHypo][i].resize(eventsWithLevelBeginsEnds[evObstrHypo][i].size());
			copy(eventsWithLevelBeginsEnds[evObstrHypo][i].begin(), eventsWithLevelBeginsEnds[evObstrHypo][i].end(),
				editEventsWithLevelBeginsEnds[evObstrHypo][i].begin());
		}
	}
	else if (_eventType & evntTypeRERA) {
		for (int i = 0; i < bgLevelTypeCount; i++) {
			editEventsWithLevelBeginsEnds[evRERA][i].clear();
			editEventsWithLevelBeginsEnds[evRERA][i].resize(eventsWithLevelBeginsEnds[evRERA][i].size());
			copy(eventsWithLevelBeginsEnds[evRERA][i].begin(), eventsWithLevelBeginsEnds[evRERA][i].end(),
				editEventsWithLevelBeginsEnds[evRERA][i].begin());
		}
	}
	else if (_eventType & evntTypeSpO2Drop) {
		editEventsWithoutLevelBeginsEnds[evSpO2].clear();
		editEventsWithoutLevelBeginsEnds[evSpO2].resize(eventsWithoutLevelBeginsEnds[evSpO2].size());
		copy(eventsWithoutLevelBeginsEnds[evSpO2].begin(), eventsWithoutLevelBeginsEnds[evSpO2].end(),
			editEventsWithoutLevelBeginsEnds[evSpO2].begin());
	}
	else if (_eventType & evntTypeSnoring) {
		for (int k = evSnoringAllLevels; k <= evSnoringUndefLevel; k++) {
			for (int i = 0; i < bgLevelTypeCount; i++) {
				editEventsWithLevelBeginsEnds[k][i].clear();
				editEventsWithLevelBeginsEnds[k][i].resize(eventsWithLevelBeginsEnds[k][i].size());
				copy(eventsWithLevelBeginsEnds[k][i].begin(), eventsWithLevelBeginsEnds[k][i].end(),
					editEventsWithLevelBeginsEnds[k][i].begin());
			}
		}
	}
	else if (_eventType & evntTypeAwake) {
		editEventsWithoutLevelBeginsEnds[evAwake].clear();
		editEventsWithoutLevelBeginsEnds[evAwake].resize(eventsWithoutLevelBeginsEnds[evAwake].size());
		copy(eventsWithoutLevelBeginsEnds[evAwake].begin(), eventsWithoutLevelBeginsEnds[evAwake].end(),
			editEventsWithoutLevelBeginsEnds[evAwake].begin());
	}
	else if (_eventType & evntTypeExcluded) {
		editEventsWithoutLevelBeginsEnds[evExcluded].clear();
		editEventsWithoutLevelBeginsEnds[evExcluded].resize(eventsWithoutLevelBeginsEnds[evExcluded].size());
		copy(eventsWithoutLevelBeginsEnds[evExcluded].begin(), eventsWithoutLevelBeginsEnds[evExcluded].end(),
			editEventsWithoutLevelBeginsEnds[evExcluded].begin());
	}
	else if (_eventType & evntTypeSwallow) {
		editEventsWithoutLevelBeginsEnds[evSwallow].clear();
		editEventsWithoutLevelBeginsEnds[evSwallow].resize(eventsWithoutLevelBeginsEnds[evSwallow].size());
		copy(eventsWithoutLevelBeginsEnds[evSwallow].begin(), eventsWithoutLevelBeginsEnds[evSwallow].end(),
			editEventsWithoutLevelBeginsEnds[evSwallow].begin());
	}

	eventsBeingVisuallyEdited = _eventType;
	
	//---Make a copy of the event timeline vector for use during editing
	editEventTimelineVector.clear();
	editEventTimelineVector.resize(eventTimelineVector.size());
	copy(eventTimelineVector.begin(), eventTimelineVector.end(), editEventTimelineVector.begin());
	
	//---Reset stateVector (for redo/undo)
	clearStateVector();
	
	addNewState();
}

bool CEvents::getAreVisualEventsDirty(void)
{
	if (0 == editEventTimelineVector.size()) return false;

	pair <vector <UINT>::iterator,vector <UINT>::iterator> mypair;

	mypair = mismatch(eventTimelineVector.begin(), eventTimelineVector.end(), editEventTimelineVector.begin());
	if (mypair.first == eventTimelineVector.end()) return false;
	return true;
}

void CEvents::setVisualEventsToClean(void)
{
	if (editEventTimelineVector.size() != eventTimelineVector.size()) 
		editEventTimelineVector.resize(eventTimelineVector.size(), 0);
	copy(eventTimelineVector.begin(), eventTimelineVector.end(), editEventTimelineVector.begin());
}

void CEvents::setEditingViaDialogIsActive(bool _on)
{
	editingViaDialogIsActive = _on;
}

bool CEvents::getIsEditingViaDialogActive(void)
{
	return editingViaDialogIsActive;
}

bool CEvents::visualEditDelete(CEvnt * _evP)
{
	return visualEditDelete(_evP->getEventType(), _evP->getCentreTime());
}

bool CEvents::visualEditDelete(UINT _eventType,float _t)
{
	bool(*func) (UINT);
	func = getEventTestVectorPredicate(_eventType);

	if (!func) return false;
	
	vector <UINT>::iterator it0 = editEventTimelineVector.begin();
	int index = (int)(_t / EVENT_TIMELINE_TIME_INTERVAL);
	if (index >= ((int) editEventTimelineVector.size() - 1)) return false;
	it0 += index;
	vector <UINT>::iterator it1 = find_if_not(it0, editEventTimelineVector.end(), *func);
	while (it0 > editEventTimelineVector.begin()) {
		BOOL on = _eventType & *it0;
		if (!on) {
			++it0;
			break;
		}
		--it0;
	}
	transform(it0, it1, it0, getEventClearOperation(_eventType));
	if (evntTypeRERA == _eventType) {
		transform(it0, it1, it0, getEventClearOperation(evntTypeArousal));  // Make sure there is no arousal in the window

		//---Check for a connected arousal in the event array
		CArray <CArousalEvnt *, CArousalEvnt *> *aPa = NULL;
		if (spO2DropLimit3 == detectionCriteria.spO2DropLimit) aPa = &arousalEventArrayDesat3;
		else if (spO2DropLimit4 == detectionCriteria.spO2DropLimit) aPa = &arousalEventArrayDesat4;
		if (aPa) {
			int num = aPa->GetCount();
			for (int i = 0; i < num; i++) {
				CArousalEvnt *aP = aPa->GetAt(i);
				CRERAEvnt *rP = aP->getRERAEvnt();
				if (rP) {
					if ((_t >= rP->getFrom()) && (_t <= rP->getTo())) {
						int index0 = (int)(aP->getFrom() / EVENT_TIMELINE_TIME_INTERVAL);
						int index1 = (int)(aP->getTo() / EVENT_TIMELINE_TIME_INTERVAL);
						it0 = editEventTimelineVector.begin();
						it1 = editEventTimelineVector.begin();
						it0 += index0;
						it1 += index1;
						transform(it0, it1, it0, getEventClearOperation(evntTypeArousal));
						break;
					}
				}
			}
		}
	}
	return true;
}

void CEvents::removeEditedEvents(void)
{
	transform(editEventTimelineVector.begin(), editEventTimelineVector.end(), editEventTimelineVector.begin(),
		getEventClearOperation(eventsBeingVisuallyEdited));
	addNewState();
}

/*
Description: Adds a new event 
_eventType: Type of event
_time: Centered at this time
_leftTimeLimit: This is the start of the graphs
_rightTimeLimit: This is the end of the graphs
_level: Level of obstruction
*/
bool CEvents::visualEditAddNewEvent(UINT _eventType,float _time, float _leftTimeLimit, float _rightTimeLimit,UINT _level)
{
	if (_time < .0f) return false;
	if (_time > maxStopTime) return false;

	ASSERT(_rightTimeLimit >= _leftTimeLimit);

	float lengthToAdd = getDefaultLengthOfEventToAdd(_eventType, 
										_rightTimeLimit - _leftTimeLimit);
	float halfLength = lengthToAdd / 2.0f;
	float from = _time - halfLength;
	float to = _time + halfLength;
	UINT newLevel = _level;
	if ((evntTypeAllWithLevel & _eventType) && (levelTypeUndef == _level))
		newLevel = findLevelInTimeWindow(from, to);
	bool ok =  visualEditAdd(_eventType,from, to, _leftTimeLimit, _rightTimeLimit,newLevel);
	if (ok) addNewState();
	return ok;
}


/*
Description: Adds an event
_eventType: Type of event
_from: Starting at this time
_to: Ending at this time
_leftTimeLimit: This is the start of the graphs
_rightTimeLimit: This is the end of the graphs
_level: Level of obstruction
*/
bool CEvents::visualEditAdd(UINT _eventType,float _from, float _to, 
	float _leftTimeLimit, float _rightTimeLimit,UINT _level)
{
	debugEditEventTimeLineVector(_T("EditEventTimelineVector Before add new event"));
	int numSet = CDataSet::countSetBits(_eventType);
	if (numSet > 1) return false;  // Must be unique

	int first = (int) (_from / EVENT_TIMELINE_TIME_INTERVAL);
	int last = (int)(_to / EVENT_TIMELINE_TIME_INTERVAL);

	if (first > (int) editEventTimelineVector.size()) return false;
	if (last < 0) return false;

	vector <UINT>::iterator iT0, iT1;
	first = first < 0 ? 0 : first;
	last = last > (int) editEventTimelineVector.size() ? (int)editEventTimelineVector.size() : last;
	iT0 = editEventTimelineVector.begin() + first;
	iT1 = editEventTimelineVector.begin() + last;

	if (getConflict(_eventType, (float)first * EVENT_TIMELINE_TIME_INTERVAL, (float)last * EVENT_TIMELINE_TIME_INTERVAL)) {
		MessageBeep(MB_ICONWARNING);
		CString s;
		int num = s.LoadString(IDS_COLLISION2);
		AfxMessageBox(s);
		return false;
	}
	else {
		if (evntTypeRERA == _eventType) {
			//---Need to add a RERA for the whole part and an arousal for the last part.
			int firstRE = first;
			int lastArousal = last;
			int arousalRange = (int) (arousalDefaultLength / EVENT_TIMELINE_TIME_INTERVAL);
			int reraRange = (int)(reraDefaultLength / EVENT_TIMELINE_TIME_INTERVAL);
			
			int lim = (int)(((float) arousalRange / (float) reraRange) * (last - first));
			arousalRange = arousalRange > lim ? lim : arousalRange;

			vector <UINT>::iterator iTa;
			iTa = editEventTimelineVector.begin() + last - arousalRange;

			UINT(*funcRERA) (UINT);
			UINT(*funcArousal) (UINT);
			funcRERA = getEventSetOperation(evntTypeRERA);
			funcArousal = getEventSetOperation(evntTypeArousal);
			transform(iT0, iT1, iT0, funcRERA);
			transform(iTa, iT1, iTa, funcArousal);
		}
		else {
			UINT(*func0) (UINT);
			func0 = getEventSetOperation(_eventType);
			transform(iT0, iT1, iT0, func0);
		}

		//--Only continue if event has level
		if (!(evntTypeAllWithLevel & _eventType)) {
			return true;
		}

		//---Remove level info
 		UINT newLevel = _level;
  		if (evntTypeSnoring == _eventType) transform(iT0, iT1, iT0, offSnoringLevelAll);
		else {
			////---Adjust newlevel accoring to current target level
			//int lowerCount = count_if(iT0, iT1, isAHLower);
			//int upperCount = count_if(iT0, iT1, isAHUpper);
			//int multiCount = count_if(iT0, iT1, isAHMulti);
			//int undefCount = count_if(iT0, iT1, isAHUndef);
			//
			//if ((lowerCount > upperCount) && (lowerCount > multiCount))
			//	newLevel = levelTypeLower;
			//else if ((upperCount > lowerCount) && (upperCount > multiCount))
			//	newLevel = levelTypeUpper;
			//else if ((multiCount > lowerCount) && (multiCount > upperCount))
			//	newLevel = levelTypeMulti;

			transform(iT0, iT1, iT0, offAHLevelAll);
		}

		UINT(*func) (UINT);
		UINT bgLevelType = 0;
		switch (newLevel) {
		case levelTypeLower:
			bgLevelType = bgLevelTypeLower;
			break;
		case levelTypeMulti:
			bgLevelType = bgLevelTypeMulti;
			break;
		case levelTypeUpper:
			bgLevelType = bgLevelTypeUpper;
			break;
		case levelTypeUndef:
		default:
			bgLevelType = bgLevelTypeUndef;
			break;
		}
		func = getLevelSetOperation(_eventType, bgLevelType);
		if (func) 
			transform(iT0, iT1, iT0, func);

		debugEditEventTimeLineVector(_T("EditEventTimelineVector After add"));
		return true;
	}
}

/*
Description: Adds a snoring event using the pressure gradient to set levels (different through events)
_eventType: Type of event (must be snoring)
_from: Starting at this time
_to: Ending at this time
_leftTimeLimit: This is the start of the graphs
_rightTimeLimit: This is the end of the graphs
*/
bool CEvents::visualEditAddSnoring(UINT _eventType, float _from, float _to, float _leftTimeLimit, float _rightTimeLimit)
{
	ASSERT(evntTypeSnoring == _eventType);

	int numSet = CDataSet::countSetBits(_eventType);
	if (numSet > 1) return false;  // Must be unique

	int first = (int)(_from / EVENT_TIMELINE_TIME_INTERVAL);
	int last = (int)(_to / EVENT_TIMELINE_TIME_INTERVAL);

	if (first > (int)editEventTimelineVector.size()) return false;
	if (last < 0) return false;

	vector <UINT>::iterator iT0, iT1;
	first = first < 0 ? 0 : first;
	last = last >(int) editEventTimelineVector.size() ? (int)editEventTimelineVector.size() : last;
	iT0 = editEventTimelineVector.begin() + first;
	iT1 = editEventTimelineVector.begin() + last;

	if (getConflict(_eventType, (float)first * EVENT_TIMELINE_TIME_INTERVAL, (float)last * EVENT_TIMELINE_TIME_INTERVAL)) {
		MessageBeep(MB_ICONWARNING);
		return false;
	}
	else {
		UINT(*func0) (UINT);
		func0 = getEventSetOperation(_eventType);
		transform(iT0, iT1, iT0, func0);

		//--Only continue if event has level
		if (!(evntTypeAllWithLevel & _eventType))
			return true;

		if (!cathData) {
			transform(iT0, iT1, iT0, offAHLevelAll);
			transform(iT0, iT1, iT0, orSnoringLevelTypeUndef);
			return true;
		}
		if (!cathData->getPgradientVector()) {
			transform(iT0, iT1, iT0, offAHLevelAll);
			transform(iT0, iT1, iT0, orSnoringLevelTypeUndef);
			return true;
		}
		
		vector <FLOAT> *pgrad = cathData->getPgradientVector();
		vector <FLOAT> *pgradTime = cathData->getPgradientVectorTime();

		//---Remove level info
		transform(iT0, iT1, iT0, offSnoringLevelAll);

		//---Set level info from gradient
		vector <FLOAT>::iterator iGrad,iGradTime;
		int cnt = first;
		for (; iT0 < iT1; ++iT0, ++cnt) {

			iGradTime = lower_bound(pgradTime->begin(), pgradTime->end(),
				(float) cnt * EVENT_TIMELINE_TIME_INTERVAL);
			if (iGradTime >= pgradTime->end()) break;
			iGrad = pgrad->begin() + distance(pgradTime->begin(), iGradTime);

			if (*iGrad < GRADIENT_LOWER_LIMIT) *iT0 |= evntSnoringLevelLower;
			else if (*iGrad > GRADIENT_UPPER_LIMIT) *iT0 |= evntSnoringLevelUpper;
			else *iT0 |= evntSnoringLevelMulti;
		}
		return true;
	}
	return false;
}

/*
Description: Sets the level of an event if the type contains level
_evenType: The event type
_time: time at the cursor. Has to be inside an event
_level: The new level
*/
bool CEvents::visualEditSetLevel(UINT _eventType, float _time, UINT _level)
{
	return false;
}

UINT CEvents::findLevelInTimeWindow(float _from, float _to)
{
	if (!cathData) return levelTypeUndef;
	vector <FLOAT> *pg = cathData->getPgradientVector();
	if (!pg) return levelTypeUndef;
	vector <FLOAT> *pgt = cathData->getPgradientVectorTime();

	ASSERT(pg->size() == pgt->size());

	vector <FLOAT>::iterator it0 = lower_bound(pgt->begin(), pgt->end(), _from);
	if (it0 == pgt->end()) return levelTypeUndef;

	vector <FLOAT>::iterator it1 = lower_bound(pgt->begin(), pgt->end(), _to);
	if (it1 == pgt->end()) return levelTypeUndef;

	vector <FLOAT>::iterator i0 = pg->begin() + distance(pgt->begin(), it0);
	vector <FLOAT>::iterator i1 = pg->begin() + distance(pgt->begin(), it1);

	return CEvnt::getLevelFromPGradient(i0, i1);
}

void CEvents::getVersion(int * _majorVersion, int * _minorVersion)
{
	*_majorVersion = majorVersion;
	*_minorVersion = minorVersion;
}

CString CEvents::getPDFReportFileName(void)
{
	if (20 == majorVersion) return reportVersionPDF50;
	else return reportVersionPDFcurrent;
}

CString CEvents::getPDFShortReportFileName(void)
{
	return reportVersionShortPDFcurrent;
}

DETECTION_CRITERIA * CEvents::getCurrentDefaultDetectionCriteria()
{
	return &currentDefaultDetectionCriteria;
}

/*
Description:To read events.dta version 20 that was part of the 4.5.1 release
*/
bool CEvents::serialize20(CArchive & ar, int _majorVersion, int _minorVersion)
{
	//---Flags must be the same for the analysis to be valid
	int flags;
	ar >> flags;
	if (doAwakeFromCmdLine && !(flags & EVENTS_FLAG_DO_AWAKE))
		return false;
	if (!doAwakeFromCmdLine && (flags & EVENTS_FLAG_DO_AWAKE))
		return false;

	//---Read old format and convert
	DETECTION_CRITERIA200 dt200, editedDt200;
	ar.Read((void *)&dt200, sizeof(DETECTION_CRITERIA200));
	ar.Read((void *)&editedDt200, sizeof(DETECTION_CRITERIA200));

	convertCriteria200to290(&dt200, &detectionCriteria);
	convertCriteria200to290(&editedDt200, &editedDetectionCriteria);
	noEditFlag = true;
	doNotSaveFlag = true;

	for (int i = 0; i < ppBins; i++) ar >> poesBin[i];
	for (int i = 0; i < ppBins; i++) ar >> poesCumulBin[i];
	for (int i = 0; i < ppHistoBins; i++) {
		for (int j = 0; j < levelTypeCount; j++) ar >> poesHistoBinLevel[i][j];
	}
	for (int i = 0; i < ppHistoBins; i++) {
		for (int j = 0; j < posCount; j++) ar >> poesHistoBinBodyPos[i][j];
	}

	ar >> startRecordingClockTime;
	ar >> effectiveStartTime;
	ar >> effectiveStopTime;
	ar >> maxStopTime;
	ar >> totalSleepTime;

	//---Criteria dependent
	for (int i = 0; i < numSpO2DropLimits; i++) {
		for (int j = 0; j < numHypopneaDropLimits; j++) {
			ar >> rdiResult[j][i];			// osa_ordi + cai
			ar >> ohiResult[j][i];
			ar >> chiResult[j][i];
			ar >> cahiResult[j][i];			// chi + cai
			ar >> oahiResult[j][i];			// mai+oai+ohi
			ar >> osa_ordiResult[j][i];		// oahi + rerai
			ar >> numObstrHypopnea[j][i];
			ar >> numCentralHypopnea[j][i];
			ar >> arousalIndexResult[j][i];
			ar >> reraiResult[j][i];
		}
	}
	ar >> maiResult;
	ar >> caiResult;
	ar >> oaiResult;
	ar >> eventDefinition;

	for (int i = 0; i < numSpO2DropLimits; i++) {
		for (int j = 0; j < numHypopneaDropLimits; j++) {
			serializeBodyPosResults20(ar, &bodyPosResults[j][i], _majorVersion, _minorVersion);
		}
	}
	serializePulseRateResults(ar, &pulseRateResults[overall], _majorVersion, _minorVersion);
	memset(&pulseRateResults[awake], sizeof(PULSE_RATE_RESULTS),0);
	memset(&pulseRateResults[asleep], sizeof(PULSE_RATE_RESULTS), 0);

	for (int i = 0; i < numSpO2DropLimits; i++) {
		for (int j = 0; j < numHypopneaDropLimits; j++) {
			serializeLevelResults20(ar, &lvlResults[j][i], _majorVersion, _minorVersion);
		}
	}

	for (int i = 0; i < numSpO2DropLimits; i++) {
		for (int j = 0; j < numHypopneaDropLimits; j++) {
			serializeRdResults(ar, &rdResults[j][i], _majorVersion, _minorVersion);
		}
	}
	serializeSnoringResults(ar, &snoringResult, _majorVersion, _minorVersion);

	for (int i = 0; i < numSpO2DropLimits; i++) {
		serializeSpO2Results(ar, &spO2Res[i][overall], _majorVersion, _minorVersion);
		memset(&spO2Res[i][asleep], sizeof(SPO2_RESULTS), 0);
		memset(&spO2Res[i][awake], sizeof(SPO2_RESULTS), 0);
	}

	CEvnt::serializeVectorUint(ar, eventTimelineVector.begin(), eventTimelineVector.end(), &eventTimelineVector);
	CEvnt::serializeVectorUint(ar, snoringVector.begin(), snoringVector.end(), &snoringVector);
	CEvnt::serializeVectorFloat(ar, EventsBeginsEnds.begin(), EventsBeginsEnds.end(), &EventsBeginsEnds);

	for (int i = 0; i < ev20CountWithLevel; i++) {
		for (int j = 0; j < bgLevelTypeCount; j++)
			CEvnt::serializeVectorFloat(ar, eventsWithLevelBeginsEnds[i][j].begin(),
				eventsWithLevelBeginsEnds[i][j].end(),
				&eventsWithLevelBeginsEnds[i][j]);
	}

	for (int i = 0; i < ev20CountWithoutLevel; i++) {
		CEvnt::serializeVectorFloat(ar, eventsWithoutLevelBeginsEnds[i].begin(),
			eventsWithoutLevelBeginsEnds[i].end(),
			&eventsWithoutLevelBeginsEnds[i]);
	}

	int num;

	//---Mixed 1
	if (ar.IsStoring()) {
		num = mixedEventArray.GetCount();
		ar << num;
		for (int i = 0; i < num; ++i) {
			CMixedEvnt *evt = mixedEventArray.GetAt(i);
			evt->Serialize(ar, _majorVersion, _minorVersion);
		}
	}
	else {
		ar >> num;
		for (int i = 0; i < num; ++i) {
			CMixedEvnt *evt = new CMixedEvnt;
			evt->Serialize20(ar, _majorVersion, _minorVersion);
			mixedEventArray.Add(evt);
		}
	}

	//---Central 2
	if (ar.IsStoring()) {
		num = centralEventArray.GetCount();
		ar << num;
		for (int i = 0; i < num; ++i) {
			CCentralEvnt *evt = centralEventArray.GetAt(i);
			evt->Serialize(ar, _majorVersion, _minorVersion);
		}
	}
	else {
		ar >> num;
		for (int i = 0; i < num; ++i) {
			CCentralEvnt *evt = new CCentralEvnt;
			evt->Serialize20(ar, _majorVersion, _minorVersion);
			centralEventArray.Add(evt);
		}
	}

	//---Obstr 3
	if (ar.IsStoring()) {
		num = obstrEventArray.GetCount();
		ar << num;
		for (int i = 0; i < num; ++i) {
			CObstrEvnt *evt = obstrEventArray.GetAt(i);
			evt->Serialize(ar, _majorVersion, _minorVersion);
		}
	}
	else {
		ar >> num;
		for (int i = 0; i < num; ++i) {
			CObstrEvnt *evt = new CObstrEvnt;
			evt->Serialize20(ar, _majorVersion, _minorVersion);
			obstrEventArray.Add(evt);
		}
	}

	//---Hypo 4
	if (ar.IsStoring()) {
		num = hypoEventArrayDesat3.GetCount();
		ar << num;
		for (int i = 0; i < num; ++i) {
			CHypoEvnt *evt = hypoEventArrayDesat3.GetAt(i);
			evt->Serialize(ar, _majorVersion, _minorVersion);
		}
	}
	else {
		ar >> num;
		for (int i = 0; i < num; ++i) {
			CHypoEvnt *evt = new CHypoEvnt;
			evt->Serialize20(ar, _majorVersion, _minorVersion);
			hypoEventArrayDesat3.Add(evt);
		}
	}

	//---Hypo 5
	if (ar.IsStoring()) {
		num = hypoEventArrayDesat4.GetCount();
		ar << num;
		for (int i = 0; i < num; ++i) {
			CHypoEvnt *evt = hypoEventArrayDesat4.GetAt(i);
			evt->Serialize(ar, _majorVersion, _minorVersion);
		}
	}
	else {
		ar >> num;
		for (int i = 0; i < num; ++i) {
			CHypoEvnt *evt = new CHypoEvnt;
			evt->Serialize20(ar, _majorVersion, _minorVersion);
			hypoEventArrayDesat4.Add(evt);
		}
	}

	//---Arousal 8
	if (ar.IsStoring()) {
		num = arousalEventArrayDesat3.GetCount();
		ar << num;
		for (int i = 0; i < num; ++i) {
			CArousalEvnt *evt = arousalEventArrayDesat3.GetAt(i);
			evt->Serialize(ar,_majorVersion,_minorVersion);
		}
	}
	else {
		ar >> num;
		for (int i = 0; i < num; ++i) {
			CArousalEvnt *evt = new CArousalEvnt;
			evt->Serialize20(ar,_majorVersion, _minorVersion);
			arousalEventArrayDesat3.Add(evt);
		}
	}

	//---Arousal 9
	if (ar.IsStoring()) {
		num = arousalEventArrayDesat4.GetCount();
		ar << num;
		for (int i = 0; i < num; ++i) {
			CArousalEvnt *evt = arousalEventArrayDesat4.GetAt(i);
			evt->Serialize(ar, _majorVersion, _minorVersion);
		}
	}
	else {
		ar >> num;
		for (int i = 0; i < num; ++i) {
			CArousalEvnt *evt = new CArousalEvnt;
			evt->Serialize20(ar, _majorVersion, _minorVersion);
			arousalEventArrayDesat4.Add(evt);
		}
	}

	//---SpO2 10 3% drop
	if (ar.IsStoring()) {
		num = spO2DropEventArray3.GetCount();
		ar << num;
		for (int i = 0; i < num; ++i) {
			CSpO2DropEvnt *evt = spO2DropEventArray3.GetAt(i);
			evt->Serialize(ar, _majorVersion, _minorVersion);
		}
	}
	else {
		ar >> num;
		for (int i = 0; i < num; ++i) {
			CSpO2DropEvnt *evt = new CSpO2DropEvnt(detectionCriteria.spO2DropLimit);
			evt->Serialize20(ar, _majorVersion, _minorVersion);
			spO2DropEventArray3.Add(evt);
		}
	}

	//---SpO2 11 4% drop
	if (ar.IsStoring()) {
		num = spO2DropEventArray4.GetCount();
		ar << num;
		for (int i = 0; i < num; ++i) {
			CSpO2DropEvnt *evt = spO2DropEventArray4.GetAt(i);
			evt->Serialize(ar, _majorVersion, _minorVersion);
		}
	}
	else {
		ar >> num;
		for (int i = 0; i < num; ++i) {
			CSpO2DropEvnt *evt = new CSpO2DropEvnt(detectionCriteria.spO2DropLimit);
			evt->Serialize20(ar, _majorVersion, _minorVersion);
			spO2DropEventArray4.Add(evt);
		}
	}

	if (!ar.IsStoring()) {
		switch (detectionCriteria.spO2DropLimit) {
		case spO2DropLimit3:
			spO2DropEventArrayCurrentP = &spO2DropEventArray3;
			hypoEventArrayCurrentP = &hypoEventArrayDesat3;
			arousalEventArrayCurrentP = &arousalEventArrayDesat3;
			break;
		case spO2DropLimit4:
			spO2DropEventArrayCurrentP = &spO2DropEventArray4;
			hypoEventArrayCurrentP = &hypoEventArrayDesat4;
			arousalEventArrayCurrentP = &arousalEventArrayDesat4;
			break;
		default:
			spO2DropEventArrayCurrentP = &spO2DropEventArray4;
			hypoEventArrayCurrentP = &hypoEventArrayDesat4;
			arousalEventArrayCurrentP = &arousalEventArrayDesat4;
			break;
		}
	}

	//---Manual 12
	if (ar.IsStoring()) {
		num = manualMarkerEventArray.GetCount();
		ar << num;
		for (int i = 0; i < num; ++i) {
			CManualMarkerEvnt *evt = manualMarkerEventArray.GetAt(i);
			evt->Serialize(ar, _majorVersion, _minorVersion);
		}
	}
	else {
		ar >> num;
		for (int i = 0; i < num; ++i) {
			CManualMarkerEvnt *evt = new CManualMarkerEvnt;
			evt->Serialize20(ar, _majorVersion, _minorVersion);
			manualMarkerEventArray.Add(evt);
		}
	}

	//---Awake 13
	if (ar.IsStoring()) {
		num = awakeEventArray.GetCount();
		ar << num;
		for (int i = 0; i < num; ++i) {
			CAwakeEvnt *evt = awakeEventArray.GetAt(i);
			evt->Serialize(ar, _majorVersion, _minorVersion);
		}
	}
	else {
		ar >> num;
		for (int i = 0; i < num; ++i) {
			CAwakeEvnt *evt = new CAwakeEvnt(.0f, .0f, defDateTime, posUndefined, autoDetection);
			evt->Serialize20(ar, _majorVersion, _minorVersion);
			awakeEventArray.Add(evt);
		}
	}

	//---Snoring 14
	if (ar.IsStoring()) {
		num = snoringEventArray.GetCount();
		ar << num;
		for (int i = 0; i < num; ++i) {
			CSnoringEvnt *evt = snoringEventArray.GetAt(i);
			evt->Serialize(ar, _majorVersion, _minorVersion);
		}
	}
	else {
		ar >> num;
		for (int i = 0; i < num; ++i) {
			CSnoringEvnt *evt = new CSnoringEvnt(.0f, .0f, defDateTime, posUndefined, autoDetection);
			evt->Serialize20(ar, _majorVersion, _minorVersion);
			snoringEventArray.Add(evt);
		}
	}

	//---Excluded 15
	if (ar.IsStoring()) {
		num = excludedEventArray.GetCount();
		ar << num;
		for (int i = 0; i < num; ++i) {
			CExcludedEvnt *evt = excludedEventArray.GetAt(i);
			evt->Serialize(ar, _majorVersion, _minorVersion);
		}
	}
	else {
		ar >> num;
		for (int i = 0; i < num; ++i) {
			CExcludedEvnt *evt = new CExcludedEvnt;
			evt->Serialize20(ar, _majorVersion, _minorVersion);
			excludedEventArray.Add(evt);
		}
	}

	//---Swallow 16
	if (ar.IsStoring()) {
		num = swallowEventArray.GetCount();
		ar << num;
		for (int i = 0; i < num; ++i) {
			CSwallowEvnt *evt = swallowEventArray.GetAt(i);
			evt->Serialize(ar, _majorVersion, _minorVersion);
		}
	}
	else {
		ar >> num;
		for (int i = 0; i < num; ++i) {
			CSwallowEvnt *evt = new CSwallowEvnt;
			evt->Serialize20(ar, _majorVersion, _minorVersion);
			swallowEventArray.Add(evt);
		}
	}

	return true;
}

void CEvents::debugEventTimeLineVector(CString _heading)
{	
	
}

void CEvents::debugEditEventTimeLineVector(CString _heading)
{
}

/*
Description: Returns the vector to modify based on event type and level
_eventType: eventType
_levelType: level Type 
*/
vector <FLOAT> *CEvents::getEventEditVectorToModify(UINT _eventType, UINT _levelType /*= 0*/)
{
	vector <FLOAT> *vToModify = NULL;
	switch (_eventType) {
	case evntTypeMixed:
		vToModify = &editEventsWithLevelBeginsEnds[evMix][_levelType];
		break;
	case evntTypeCentral:
		vToModify = &editEventsWithoutLevelBeginsEnds[evCentral];
		break;
	case evntTypeObstr:
		vToModify = &editEventsWithLevelBeginsEnds[evObstr][_levelType];
		break;
	case evntTypeHypoCentral:
		vToModify = &editEventsWithoutLevelBeginsEnds[evCentralHypo];
		break;
	case evntTypeHypoObstr:
		vToModify = &editEventsWithLevelBeginsEnds[evObstrHypo][_levelType]; 
		break;
	case evntTypeRERA:
		vToModify = &editEventsWithLevelBeginsEnds[evRERA][_levelType];
		break;
	case evntTypeArousal:
		vToModify = &editEventsWithoutLevelBeginsEnds[evArousal];
		break;
	case evntTypeSpO2Drop:
		vToModify = &editEventsWithoutLevelBeginsEnds[evSpO2];
		break;
	case evntTypeSnoring:
		if (bgLevelTypeLower == _levelType)			vToModify = &editEventsWithLevelBeginsEnds[evSnoringLowerLevel][_levelType];
		else if (bgLevelTypeUpper == _levelType)	vToModify = &editEventsWithLevelBeginsEnds[evSnoringUpperLevel][_levelType];
		else if (bgLevelTypeMulti == _levelType)	vToModify = &editEventsWithLevelBeginsEnds[evSnoringMultiLevel][_levelType];
		else if (bgLevelTypeUndef == _levelType)	vToModify = &editEventsWithLevelBeginsEnds[evSnoringUndefLevel][_levelType];
		break;
	case evntTypeAwake:
		vToModify = &editEventsWithoutLevelBeginsEnds[evAwake];
		break;
	case evntTypeExcluded:
		vToModify = &editEventsWithoutLevelBeginsEnds[evExcluded];
		break;
	case evntTypeSwallow:
		vToModify = &editEventsWithoutLevelBeginsEnds[evSwallow];
		break;
	case evntTypeManMarker:
		vToModify = &editEventsWithoutLevelBeginsEnds[evManual];
		break;
	case evntTypeImported:
		vToModify = &editEventsWithoutLevelBeginsEnds[evImported];
		break;
	default:
		break;
	}
	return vToModify;
}

UINT CEvents::getTheEventBeingVisuallyEdited(void)
{
	return eventsBeingVisuallyEdited;
}

/*
Description: Returns the events to be shown during editing of events visually, these are the evnets that the user has to relate to
If excluded events are edited, show only these
If any other events are edited, show also awake and excluded.
*/
UINT CEvents::getEventsToShowDuringVisualEditing(void)
{
	if (evntTypeExcluded == eventsBeingVisuallyEdited) return evntTypeExcluded;
	else if (evntTypeSwallow == eventsBeingVisuallyEdited) {
		UINT ret = eventsBeingVisuallyEdited;
		ret |= evntTypeExcluded;
		return ret;
	}

	UINT ret = eventsBeingVisuallyEdited;
	ret |= evntTypeAwake;
	ret |= evntTypeExcluded;
	return ret;
}

/*
Not used anymore
*/
//CString CEvents::getConsolidateButtonCaption(void)
//{
//	CString s;
//	if (eventsBeingVisuallyEdited & evntTypeMixed) s = _T("xxConsolidate Apnea/Hypopnea");
//	else if (eventsBeingVisuallyEdited & evntTypeObstr) s = _T("xxConsolidate Apnea/Hypopnea");
//	else if (eventsBeingVisuallyEdited & evntTypeHypoCentral) s = _T("xxConsolidate Apnea/Hypopnea");
//	else if (eventsBeingVisuallyEdited & evntTypeCentral) s = _T("xxConsolidate Apnea/Hypopnea");
//	else if (eventsBeingVisuallyEdited & evntTypeHypoObstr) s = _T("xxConsolidate Apnea/Hypopnea");
//	else if (eventsBeingVisuallyEdited & evntTypeRERA) s = _T("xxConsolidate RERA");
//	else if (eventsBeingVisuallyEdited & evntTypeSnoring) s = _T("xxConsolidate snoring");
//	else if (eventsBeingVisuallyEdited & evntTypeArousal) s = _T("xxConsolidate arousals");
//	else if (eventsBeingVisuallyEdited & evntTypeSpO2Drop) s = _T("xxConsolidate desaturations");
//	else if (eventsBeingVisuallyEdited & evntTypeManMarker) s = _T("xxConsolidate manual markers");
//	else if (eventsBeingVisuallyEdited & evntTypeExcluded) s = _T("xxConsolidate excluded");
//	else if (eventsBeingVisuallyEdited & evntTypeAwake) s = _T("xxConsolidate awake");
//	else if (eventsBeingVisuallyEdited & evntTypeSwallow) s = _T("xxConsolidate swallow");
//	else if (eventsBeingVisuallyEdited & evntTypeImported) s = _T("xxConsolidate imported");
//	
//	return s;
//}

/*
Description: Return the predicate function for testing
_eventType: Unique event type
_levelType: Level type, one of bgLevelTypes
*/
foo CEvents::getEventTestVectorPredicate(UINT _eventType, UINT _levelType /*= 0*/)
{
	switch (_eventType) {
	case evntTypeMixed:
		if (bgLevelTypeUpper == _levelType) return &isMixedUpperNotAwakeNotExcluded;
		else if (bgLevelTypeLower == _levelType) return &isMixedLowerNotAwakeNotExcluded;
		else if (bgLevelTypeMulti == _levelType) return &isMixedMultiNotAwakeNotExcluded;
		else if (bgLevelTypeUndef == _levelType) return &isMixedUndefNotAwakeNotExcluded;
		break;
	case evntTypeObstr:
		if (bgLevelTypeUpper == _levelType)			return &isObstrUpperNotAwakeNotExcluded;
		else if (bgLevelTypeLower == _levelType)	return &isObstrLowerNotAwakeNotExcluded;
		else if (bgLevelTypeMulti == _levelType)	return &isObstrMultiNotAwakeNotExcluded;
		else if (bgLevelTypeUndef == _levelType)	return &isObstrUndefNotAwakeNotExcluded;
		break;
	case evntTypeRERA:
		if (bgLevelTypeUpper == _levelType)			return &isRERAUpperNotAwakeNotExcluded;
		else if (bgLevelTypeLower == _levelType)	return &isRERALowerNotAwakeNotExcluded;
		else if (bgLevelTypeMulti == _levelType)	return &isRERAMultiNotAwakeNotExcluded;
		else if (bgLevelTypeUndef == _levelType)	return &isRERAUndefNotAwakeNotExcluded;
		break;
	case evntTypeHypoObstr:
		if (bgLevelTypeUpper == _levelType)			return &isHypoObstrUpperNotAwakeNotExcluded;
		else if (bgLevelTypeLower == _levelType)	return &isHypoObstrLowerNotAwakeNotExcluded;
		else if (bgLevelTypeMulti == _levelType)	return &isHypoObstrMultiNotAwakeNotExcluded;
		else if (bgLevelTypeUndef == _levelType)	return &isHypoObstrUndefNotAwakeNotExcluded;
		break;
	case evntTypeCentral:
		return &isCentralNotAwakeNotExcluded;
		break;
	case evntTypeHypoCentral:
		return &isHypoCentralNotAwakeNotExcluded;
		break;
	case evntTypeSpO2Drop:
		return &isDesaturationNotAwakeNotExcluded;
		break;
	case evntTypeArousal:
		return &isArousalNotAwakeNotExcluded;
		break;
	case evntTypeAwake:
		return &isAwakeNotExcluded;
		break;
	case evntTypeExcluded:
		return &isExcluded;
		break;
	case evntTypeSnoring:
		if (bgLevelTypeUpper == _levelType)			return &isEvntSnoringLevelUpperNotAwakeExcluded;
		else if (bgLevelTypeLower == _levelType)	return &isEvntSnoringLevelLowerNotAwakeExcluded;
		else if (bgLevelTypeMulti == _levelType)	return &isEvntSnoringLevelMultiNotAwakeExcluded;
		else if (bgLevelTypeUndef == _levelType)	return &isEvntSnoringLevelUndefNotAwakeExcluded;
		break;
	case evntTypeImported:
		return &isImported;
		break;
	case evntTypeManMarker:
		return &isManMarker;
		break;
	case evntTypeSwallow:
		return &isSwallowNotExcluded;
		break;
	case evntTypeNone:
	default:
		break;
	}
	return NULL;
}

foo CEvents::getEventTestVectorPredicate(UINT _eventType)
{
	switch (_eventType) {
	case evntTypeAH:
		return &isAHNotAwakeNotExcluded;
		break;
	case evntTypeMixed:
		return &isMixedNotAwakeNotExcluded;
		break;
	case evntTypeObstr:
		return &isObstrNotAwakeNotExcluded;
		break;
	case evntTypeRERA:
		return &isRERANotAwakeNotExcluded;
		break;
	case evntTypeHypoObstr:
		return &isHypoObstrNotAwakeNotExcluded;
		break;
	case evntTypeCentral:
		return &isCentralNotAwakeNotExcluded;
		break;
	case evntTypeHypoCentral:
		return &isHypoCentralNotAwakeNotExcluded;
		break;
	case evntTypeSpO2Drop:
		return &isDesaturationNotAwakeNotExcluded;
		//return &isDesaturation; 
		break;
	case evntTypeArousal:
		return &isArousalNotAwakeNotExcluded;
		break;
	case evntTypeAwake:
		return &isAwakeNotExcluded;
		break;
	case evntTypeExcluded:
		return &isExcluded;
		break;
	case evntTypeSnoring:
		return &isSnoringNotAwakeNotExcluded;
		break;
	case evntTypeImported:
		return &isImported;
		break;
	case evntTypeManMarker:
		return &isManMarker;
		break;
	case evntTypeSwallow:
		return &isSwallowNotExcluded;
		break;
	case evntTypeNone:
	default:
		break;
	}
	return NULL;
}

foouint CEvents::getEventSetOperation(UINT _eventType)
{
	switch (_eventType) {
	case evntTypeMixed:
		return &orElementTypeMixed;
		break;
	case evntTypeCentral:
		return &orElementTypeCentral;
		break;
	case evntTypeObstr:
		return &orElementTypeObstr;
		break;
	case evntTypeHypoCentral:
		return &orElementTypeHypoCentral;
		break;
	case evntTypeHypoObstr:
		return &orElementTypeHypoObstr;
		break;
	case evntTypeSpO2Drop:
		return &orElementTypeSpO2Drop;
		break;
	case evntTypeRERA:
		return &orElementTypeRERA;
		break;
	case evntTypeArousal:
		return &orElementTypeArousal;
		break;
	case evntTypeAwake:
		return &orElementTypeAwake;
		break;
	case evntTypeExcluded:
		return &orElementTypeExcluded;
		break;
	case evntTypeSnoring:
		return &orElementTypeSnoring;
		break;
	case evntTypeImported:
		return &orElementTypeImported;
		break;
	case evntTypeSwallow:
		return &orElementTypeSwallow;
		break;
	case evntTypeManMarker:
		return &orElementTypeManMarker;
		break;
	case evntTypeNone:
	default:
		break;
	}
	return &orElementTypeNone;
}

foouint CEvents::getLevelSetOperation(UINT _eventType, UINT _levelType)
{
	switch (_eventType) {
	case evntTypeObstr:
	case evntTypeHypoObstr:
	case evntTypeMixed:
	case evntTypeRERA:
		switch (_levelType) {
		case bgLevelTypeLower:
			return &orAHLevelTypeLower;
			break;
		case bgLevelTypeUpper:
			return &orAHLevelTypeUpper;
			break;
		case bgLevelTypeMulti:
			return &orAHLevelTypeMulti;
			break;
		case bgLevelTypeUndef:
			return &orAHLevelTypeUndef;
			break;
		}
		break;
	case evntTypeSnoring:
		switch (_levelType) {
		case bgLevelTypeLower:
			return &orSnoringLevelTypeLower;
			break;
		case bgLevelTypeUpper:
			return &orSnoringLevelTypeUpper;
			break;
		case bgLevelTypeMulti:
			return &orSnoringLevelTypeMulti;
			break;
		case bgLevelTypeUndef:
			return &orSnoringLevelTypeUndef;
			break;
		}
		break;
	default:
		break;
	}
	return NULL;
}

foouint CEvents::getAHLevelEventSetOperation(UINT _levelType)
{
	switch (_levelType) {
	case levelTypeUpper:
		return &orAHLevelTypeUpper;
		break;
	case levelTypeLower:
		return &orAHLevelTypeLower;
		break;
	case levelTypeMulti:
		return &orAHLevelTypeMulti;
		break;
	case levelTypeUndef:
		return &orAHLevelTypeUndef;
		break;
	default:
		break;
	}
	return &orElementTypeNone;
}

foouint CEvents::getSnoringLevelEventSetOperation(UINT _levelType)
{
	switch (_levelType) {
	case levelTypeUpper:
		return &orSnoringLevelTypeUpper;
		break;
	case levelTypeLower:
		return &orSnoringLevelTypeLower;
		break;
	case levelTypeMulti:
		return &orSnoringLevelTypeMulti;
		break;
	case levelTypeUndef:
		return &orSnoringLevelTypeUndef;
		break;
	default:
		break;
	}
	return &orElementTypeNone;
}

foouint CEvents::getEventClearOperation(UINT _eventType)
{
	switch (_eventType) {
	case evntTypeMixed:
		return &offElementTypeMixed;
		break;
	case evntTypeCentral:
		return &offElementTypeCentral;
		break;
	case evntTypeObstr:
		return &offElementTypeObstr;
		break;
	case evntTypeHypoCentral:
		return &offElementTypeHypoCentral;
		break;
	case evntTypeHypoObstr:
		return &offElementTypeHypoObstr;
		break;
	case evntTypeSpO2Drop:
		return &offElementTypeSpO2Drop;
		break;
	case evntTypeRERA:
		return &offElementTypeRERA;
		break;
	case evntTypeArousal:
		return &offElementTypeArousal;
		break;
	case evntTypeAwake:
		return &offElementTypeAwake;
		break;
	case evntTypeExcluded:
		return &offElementTypeExcluded;
		break;
	case evntTypeSnoring:
		return &offElementTypeSnoring;
		break;
	case evntTypeImported:
		return &offElementTypeImported;
		break;
	case evntTypeSwallow:
		return &offElementTypeSwallow;
		break;
	case evntTypeManMarker:
		return &offElementTypeManMarker;
		break;
	case evntTypeAH:
		return &offElementTypeAHandAllLevels;
		break;
	case evntTypeNone:
	default:
		break;
	}
	return &offElementTypeNone;
}

foouint CEvents::getLevelClearOperation(UINT _eventType)
{
	switch (_eventType) {
	case evntTypeMixed:
	case evntTypeObstr:
	case evntTypeRERA:
	case evntTypeHypoObstr:
		return &offAHLevelAll;
		break;
	case evntTypeSnoring:
		return &offSnoringLevelAll;
		break;
	case evntTypeCentral:
	case evntTypeHypoCentral:
	case evntTypeSpO2Drop:
	case evntTypeImported:
	case evntTypeExcluded:
	case evntTypeAwake:
	case evntTypeSwallow:
	case evntTypeArousal:
	case evntTypeManMarker:
	case evntTypeNone:
	default:
		break;
	}
	return &offElementTypeNone;
}

/*
Description: Takes an onOff vector and generates a startsStops vector to be used by the graphics.
*/
void CEvents::onOffVectorToBeginEndsVector(UINT _eventType)
{
	onOffVectorToBeginEndsVector(_eventType, &editEventTimelineVector);
}

void CEvents::onOffVectorToBeginEndsVector(void)
{
	if (evntTypeAH == eventsBeingVisuallyEdited) {
		onOffVectorToBeginEndsVector(evntTypeMixed);
		onOffVectorToBeginEndsVector(evntTypeCentral);
		onOffVectorToBeginEndsVector(evntTypeObstr);
		onOffVectorToBeginEndsVector(evntTypeHypoCentral);
		onOffVectorToBeginEndsVector(evntTypeHypoObstr);
	}
	else if (evntTypeAHandRERA == eventsBeingVisuallyEdited) {
		onOffVectorToBeginEndsVector(evntTypeMixed);
		onOffVectorToBeginEndsVector(evntTypeCentral);
		onOffVectorToBeginEndsVector(evntTypeObstr);
		onOffVectorToBeginEndsVector(evntTypeHypoCentral);
		onOffVectorToBeginEndsVector(evntTypeHypoObstr);
		onOffVectorToBeginEndsVector(evntTypeRERA);
	}
	else 
		onOffVectorToBeginEndsVector(eventsBeingVisuallyEdited);
}

/*
Description: Takes an onOff vector and generates a startsStops vector to be used by the graphics.
	Only one event type at a time!!!!!!
*/
void CEvents::onOffVectorToBeginEndsVector(UINT _eventType, vector<UINT>* _onOffV)
{
	if (!_onOffV) return;
	if (evntTypeNone == _eventType) return;
	if (_onOffV->size() < 2) return;

	bool(*func) (UINT);
	func = NULL;

	if (evntTypeAHandRERA & _eventType) {

		//---Lower
		vector <FLOAT> *beV = getEventEditVectorToModify(_eventType,bgLevelTypeLower);
		func = getEventTestVectorPredicate(_eventType, bgLevelTypeLower);
		if (!func) return;
		beV->clear();
		vector <UINT>::iterator it0 = _onOffV->begin();
		vector <UINT>::iterator it1 = _onOffV->begin();
		float time0, time1;
		while (1) {
			it0 = find_if(it1, _onOffV->end(), *func);
			it1 = find_if_not(it0, _onOffV->end(), *func);
			if ((it0 < _onOffV->end()) && (it1 <= _onOffV->end())) {
				time0 = EVENT_TIMELINE_TIME_INTERVAL * distance(_onOffV->begin(), it0);
				time1 = EVENT_TIMELINE_TIME_INTERVAL * distance(_onOffV->begin(), it1);
				ASSERT(time0 < time1);
				beV->push_back(time0);
				beV->push_back(time1);
			}
			else break;
		}

		//---Upper
		beV = getEventEditVectorToModify(_eventType, bgLevelTypeUpper);
		func = getEventTestVectorPredicate(_eventType, bgLevelTypeUpper);
		if (!func) return;
		beV->clear();
		it0 = _onOffV->begin();
		it1 = _onOffV->begin();
		while (1) {
			it0 = find_if(it1, _onOffV->end(), *func);
			it1 = find_if_not(it0, _onOffV->end(), *func);
			if ((it0 < _onOffV->end()) && (it1 <= _onOffV->end())) {
				time0 = EVENT_TIMELINE_TIME_INTERVAL * distance(_onOffV->begin(), it0);
				time1 = EVENT_TIMELINE_TIME_INTERVAL * distance(_onOffV->begin(), it1);
				ASSERT(time0 < time1);
				beV->push_back(time0);
				beV->push_back(time1);
			}
			else break;
		}

		//---Multi
		beV = getEventEditVectorToModify(_eventType, bgLevelTypeMulti);
		func = getEventTestVectorPredicate(_eventType, bgLevelTypeMulti);
		if (!func) return;
		beV->clear();
		it0 = _onOffV->begin();
		it1 = _onOffV->begin();
		while (1) {
			it0 = find_if(it1, _onOffV->end(), *func);
			it1 = find_if_not(it0, _onOffV->end(), *func);
			if ((it0 < _onOffV->end()) && (it1 <= _onOffV->end())) {
				time0 = EVENT_TIMELINE_TIME_INTERVAL * distance(_onOffV->begin(), it0);
				time1 = EVENT_TIMELINE_TIME_INTERVAL * distance(_onOffV->begin(), it1);
				ASSERT(time0 < time1);
				beV->push_back(time0);
				beV->push_back(time1);
			}
			else break;
		}

		//---Undef
		beV = getEventEditVectorToModify(_eventType, bgLevelTypeUndef);
		func = getEventTestVectorPredicate(_eventType, bgLevelTypeUndef);
		if (!func) return;
		beV->clear();
		it0 = _onOffV->begin();
		it1 = _onOffV->begin();
		while (1) {
			it0 = find_if(it1, _onOffV->end(), *func);
			it1 = find_if_not(it0, _onOffV->end(), *func);
			if ((it0 < _onOffV->end()) && (it1 <= _onOffV->end())) {
				time0 = EVENT_TIMELINE_TIME_INTERVAL * distance(_onOffV->begin(), it0);
				time1 = EVENT_TIMELINE_TIME_INTERVAL * distance(_onOffV->begin(), it1);
				ASSERT(time0 < time1);
				beV->push_back(time0);
				beV->push_back(time1);
			}
			else break;
		}
	}
	else if (evntTypeSnoring == _eventType) {

		//---Lower
		vector <FLOAT> *beV = getEventEditVectorToModify(_eventType, bgLevelTypeLower);
		func = getEventTestVectorPredicate(_eventType, bgLevelTypeLower);
		if (!func) return;
		beV->clear();
		vector <UINT>::iterator it0 = _onOffV->begin();
		vector <UINT>::iterator it1 = _onOffV->begin();
		float time0, time1;
		while (1) {
			it0 = find_if(it1, _onOffV->end(), *func);
			it1 = find_if_not(it0, _onOffV->end(), *func);
			if ((it0 < _onOffV->end()) && (it1 <= _onOffV->end())) {
				time0 = EVENT_TIMELINE_TIME_INTERVAL * distance(_onOffV->begin(), it0);
				time1 = EVENT_TIMELINE_TIME_INTERVAL * distance(_onOffV->begin(), it1);
				ASSERT(time0 < time1);
				beV->push_back(time0);
				beV->push_back(time1);
			}
			else break;
		}

		//---Upper
		beV = getEventEditVectorToModify(_eventType, bgLevelTypeUpper);
		func = getEventTestVectorPredicate(_eventType, bgLevelTypeUpper);
		if (!func) return;
		beV->clear();
		it0 = _onOffV->begin();
		it1 = _onOffV->begin();
		while (1) {
			it0 = find_if(it1, _onOffV->end(), *func);
			it1 = find_if_not(it0, _onOffV->end(), *func);
			if ((it0 < _onOffV->end()) && (it1 <= _onOffV->end())) {
				time0 = EVENT_TIMELINE_TIME_INTERVAL * distance(_onOffV->begin(), it0);
				time1 = EVENT_TIMELINE_TIME_INTERVAL * distance(_onOffV->begin(), it1);
				ASSERT(time0 < time1);
				beV->push_back(time0);
				beV->push_back(time1);
			}
			else break;
		}

		//---Multi
		beV = getEventEditVectorToModify(_eventType, bgLevelTypeMulti);
		func = getEventTestVectorPredicate(_eventType, bgLevelTypeMulti);
		if (!func) return;
		beV->clear();
		it0 = _onOffV->begin();
		it1 = _onOffV->begin();
		while (1) {
			it0 = find_if(it1, _onOffV->end(), *func);
			it1 = find_if_not(it0, _onOffV->end(), *func);
			if ((it0 < _onOffV->end()) && (it1 <= _onOffV->end())) {
				time0 = EVENT_TIMELINE_TIME_INTERVAL * distance(_onOffV->begin(), it0);
				time1 = EVENT_TIMELINE_TIME_INTERVAL * distance(_onOffV->begin(), it1);
				ASSERT(time0 < time1);
				beV->push_back(time0);
				beV->push_back(time1);
			}
			else break;
		}

		//---Undef
		beV = getEventEditVectorToModify(_eventType, bgLevelTypeUndef);
		func = getEventTestVectorPredicate(_eventType, bgLevelTypeUndef);
		if (!func) return;
		beV->clear();
		it0 = _onOffV->begin();
		it1 = _onOffV->begin();
		while (1) {
			it0 = find_if(it1, _onOffV->end(), *func);
			it1 = find_if_not(it0, _onOffV->end(), *func);
			if ((it0 < _onOffV->end()) && (it1 <= _onOffV->end())) {
				time0 = EVENT_TIMELINE_TIME_INTERVAL * distance(_onOffV->begin(), it0);
				time1 = EVENT_TIMELINE_TIME_INTERVAL * distance(_onOffV->begin(), it1);
				ASSERT(time0 < time1);
				beV->push_back(time0);
				beV->push_back(time1);
			}
			else break;
		}
	}
	else {
		vector <FLOAT> *beV = getEventEditVectorToModify(_eventType);
		func = getEventTestVectorPredicate(_eventType);
		if (!func) return;
		beV->clear();
		vector <UINT>::iterator it0 = _onOffV->begin();
		vector <UINT>::iterator it1 = _onOffV->begin();
		float time0, time1;
		while (1) {
			it0 = find_if(it1, _onOffV->end(), *func);
			it1 = find_if_not(it0, _onOffV->end(), *func);
			if ((it0 < _onOffV->end()) && (it1 <= _onOffV->end())) {
				time0 = EVENT_TIMELINE_TIME_INTERVAL * distance(_onOffV->begin(), it0);
				time1 = EVENT_TIMELINE_TIME_INTERVAL * distance(_onOffV->begin(), it1);
				ASSERT(time0 < time1);
				beV->push_back(time0);
				beV->push_back(time1);
			}
			else break;
		}
	}
}

void CEvents::findLimitsBetweenExcludedEvents(const float _t, float * _leftLimit, float * _rightLimit)
{
	if (0 == excludedEventArray.GetCount()) return;
	float lim0 = .0f;
	float lim1 = maxStopTime;
	if (1 == excludedEventArray.GetCount()) {
		CExcludedEvnt *eP0 = excludedEventArray.GetAt(0);
		if (_t < eP0->getFrom()) lim1 = eP0->getFrom();
		if (_t > eP0->getTo()) lim0 = eP0->getTo();
		*_leftLimit = lim0 > *_leftLimit ? lim0 : *_leftLimit;
		*_rightLimit = lim1 < *_rightLimit ? lim1 : *_rightLimit;
	}
	else {
		for (int i = 0; i < (excludedEventArray.GetCount() - 1); i++) {
			CExcludedEvnt *eP0 = excludedEventArray.GetAt(i);
			CExcludedEvnt *eP1 = excludedEventArray.GetAt(i + 1);
			if ((_t >= eP0->getTo()) && (eP1->getFrom())) {
				lim0 = eP0->getTo();
				lim1 = eP1->getFrom();
				*_leftLimit = lim0 > *_leftLimit ? lim0 : *_leftLimit;
				*_rightLimit = lim1 < *_rightLimit ? lim1 : *_rightLimit;
				break;
			}
		}
	}
}

void CEvents::findLimitsBetweenAwakeEvents(const float _t, float * _leftLimit, float * _rightLimit)
{
}

//int CEvents::getConsolidateStep(void)
//{
//	int step = -1;
//	if (eventBeingVisuallyEdited.withLevel) {
//		switch (eventBeingVisuallyEdited.type) {
//		case evMix:
//		case evObstr:
//		case evObstrHypo:
//			step = stepApnea;
//			break;
//		case evRERA:
//			step = stepRERA;
//			break;
//		case evSnoringAllLevels:
//		case evSnoringUpperLevel:
//		case evSnoringLowerLevel:
//		case evSnoringMultiLevel:
//			step = stepSnoring;
//			break;
//		}
//	}
//	else {
//		switch (eventBeingVisuallyEdited.type) {
//		case evArousal:
//			step = stepArousal;
//			break;
//		case evSpO2:
//			step = stepDesat;
//			break;
//		case evManual:
//			break;
//		case evCentralHypo:
//		case evCentral:
//			step = stepApnea;
//			break;
//		case evAwake:
//			step = stepAwake;
//			break;
//		case evStartStop:
//		case evExcluded:
//			step = stepExclude;
//			break;
//		case evSwallow:
//			step = stepSwallow;
//			break;
//		case evImported:
//			break;
//		}
//	}
//	return step;
//}

void CEvents::nowVisualEditing(const VISUAL_EVENT _evnt)
{
	doingVisualEditingMove = true;
	eventByMouse = _evnt;
}

/*
Description: Sets the current limits of the event that is manipulated
_from: Start in seconds
_to: End in seconds
*/
void CEvents::setVisualEditingLimits(const float _from, const float _to)
{
	eventByMouse.from = _from;
	eventByMouse.to = _to;
}

VISUAL_EVENT CEvents::getVisualEditingEventByMouse(void)
{
	return eventByMouse;
}

/*
Description: From the event type vector, delete the event and create a new in new position.
Fill level according to _level
_eventType: The event type
_from: start (sec)
_to : end (sec)
_leftTimeLimit: Left side of graph window (sec)
_rightTimeLimi: Right side of graph window (sec)
_level: The level to set
*/
void CEvents::doneVisualEditingMoves(UINT _eventType,float _from, float _to,float _leftTimeLimit, 
	float _rightTimeLimit,UINT _level)
{
	visualEditDelete(_eventType,(eventByMouse.from + eventByMouse.to) / 2.0f);
	doingVisualEditingMove = false;
	UINT level = _level;
	visualEditAdd(_eventType,_from,_to, _leftTimeLimit, _rightTimeLimit,level);
	onOffVectorToBeginEndsVector(_eventType, &editEventTimelineVector);
	addNewState();
}

/*
Description: From the event type vector, delete the event and create a new in new position.
			Fill level according to pressure gradient 
_eventType: The event type (must be snoring)
_from: start (sec)
_to : end (sec)
_leftTimeLimit: Left side of graph window (sec)
_rightTimeLimi: Right side of graph window (sec)
_level: The level to set
*/
void CEvents::doneVisualEditingSnoringMoves(UINT _eventType, float _from, float _to, float _leftTimeLimit, float _rightTimeLimit)
{
	ASSERT(evntTypeSnoring == _eventType);

	visualEditDelete(_eventType, (eventByMouse.from + eventByMouse.to) / 2.0f);
	doingVisualEditingMove = false;
	visualEditAddSnoring(_eventType, _from, _to, _leftTimeLimit, _rightTimeLimit);
	onOffVectorToBeginEndsVector(_eventType, &editEventTimelineVector);
	addNewState();
}

bool CEvents::consolidateVisualEditing(void)
{
	CSpiroWaitCursor spc;

	ASSERT(editEventTimelineVector.size() == eventTimelineVector.size());

	copy(editEventTimelineVector.begin(), editEventTimelineVector.end(), eventTimelineVector.begin());

	if (eventsBeingVisuallyEdited & evntTypeExcluded)
			postVisualEditAnalysis(stepExclude);
	else if (eventsBeingVisuallyEdited & evntTypeAwake)
		postVisualEditAnalysis(stepAwake);
	else if (eventsBeingVisuallyEdited & evntTypeSpO2Drop)
		postVisualEditAnalysis(stepDesat);
	else if (eventsBeingVisuallyEdited & evntTypeAH)
		postVisualEditAnalysis(stepApnea);
	else if (eventsBeingVisuallyEdited & evntTypeRERA)
		postVisualEditAnalysis(stepRERA);
	else if (eventsBeingVisuallyEdited & evntTypeSnoring)
		postVisualEditAnalysis(stepSnoring);
	else if (eventsBeingVisuallyEdited & evntTypeSwallow)
		postVisualEditAnalysis(stepSwallow);
	
	//---Reset stateVector (for redo/undo)
	clearStateVector();

	return true;
}

/*
Description: Checks for conflict with other events
_typeMoving: The type(s) that the user tries to move or change size.
				Note that if one type is found, the check is done 
				(does not check all of there are several types)
_begin: start time of user window
_end: end time of user window
*/
bool CEvents::getConflict(UINT _typeMoving, float _begin, float _end)
{
	if (evntTypeNone == _typeMoving) return false;
	ASSERT(_begin <= _end);

	bool(*func1) (UINT);
	bool(*func2) (UINT);
	func1 = NULL;
	func2 = NULL;

	if (_typeMoving & evntTypeExcluded) return false;
	else if (_typeMoving & evntTypeAwake) {
		func1 = getEventTestVectorPredicate(evntTypeExcluded);
		if (!func1) return false;
	}
	else if (_typeMoving & evntTypeSwallow) {
		func1 = getEventTestVectorPredicate(evntTypeExcluded);
		if (!func1) return false;
	}
	else {
		func1 = getEventTestVectorPredicate(evntTypeExcluded);
		if (!func1) return false;
		func2 = getEventTestVectorPredicate(evntTypeAwake);
		if (!func2) return false;
	}
	int index0 = (int) (_begin / EVENT_TIMELINE_TIME_INTERVAL);
	int index1 = (int) (_end / EVENT_TIMELINE_TIME_INTERVAL);
	index0 = index0 < 0 ? 0 : index0;
	index1 = index1 > (int) editEventTimelineVector.size() ? (int) editEventTimelineVector.size() : index1;
	vector <UINT>::iterator iT0 = editEventTimelineVector.begin();
	vector <UINT>::iterator iT1 = iT0;
	iT0 += index0;
	iT1 += index1; 

	if (count_if(iT0, iT1, func1) > 0) {
		TRACE(_T("Collission with excluded\n"));
		return true;
	}
	if (func2) 
		if (count_if(iT0, iT1, func2) > 0) {
			TRACE(_T("Collission with awake\n"));
			return true;
		}

	//---Extra tests if AHs are being edited
	if (_typeMoving & evntTypeAH) {
		int cM = count_if(iT0,		iT1, isMixed);
		int cO = count_if(iT0,		iT1, isObstr);
		int cC = count_if(iT0,		iT1, isCentral);
		int cHC = count_if(iT0,		iT1, isHypoCentral);
		int cHO = count_if(iT0,		iT1, isHypoObstr);
		int conflictCount = 0;
		if (_typeMoving & evntTypeMixed)			conflictCount =      cO + cC + cHC + cHO;
		else if (_typeMoving & evntTypeCentral)		conflictCount = cM + cO +      cHC + cHO;
		else if (_typeMoving & evntTypeObstr)		conflictCount = cM      + cC + cHC + cHO;
		else if (_typeMoving & evntTypeHypoCentral)	conflictCount = cM + cO + cC +       cHO;
		else if (_typeMoving & evntTypeHypoObstr)	conflictCount = cM + cO + cC + cHC      ;
		if (conflictCount > 0) {
			TRACE(_T("*iT0 = 0x%06x. Collission counts TOTAL %d, cM %d, cO %d, cC %d, cHC %d, cHO %d\n"), *iT0,conflictCount,cM, cO, cC, cHC, cHO);
			return true;
		}
	}
	
	return false;
}

/*
Description: Just for debugging
*/
void CEvents::dumpVector(vector <FLOAT> *_v)
{
	vector <FLOAT>::iterator it = _v->begin();
	for (; it < _v->end(); ++it) {
		TRACE(_T("%.1f\n"), *it);
	}
}


/*
Description: Copies from the particular event type to another
Returns: true if ok, false if not
*/
bool CEvents::copyMixedEventTo(CMixedEvnt *_source, CEvnt *_dest)
{
	int destType = _dest->getEventType();

	ASSERT((evntTypeMixed == destType) ||
		(evntTypeCentral == destType) ||
		(evntTypeObstr == destType) ||
		(evntTypeHypoObstr == destType) ||
		(evntTypeHypoCentral == destType) ||
		(evntTypeImported == destType));

	bool ret = false;

	switch (destType) {
	case evntTypeMixed:
		_source->copyTo((CMixedEvnt *) _dest);
		ret = true;
		break;
	case evntTypeCentral:
		((CEvnt *) _source)->copyTo((CCentralEvnt *) _dest);
		_dest->setEventType(evntTypeCentral);
		_dest->setDetectionSourceManOrAuto(manualDetection);
		_dest->setHasLevelDetection(false);
		_dest->setEventFlags(_source->getEventFlags() | EVNT_DEF_MANUAL | EVNT_DEF_MANUAL_TYPE);
		ret = true;
		break;
	case evntTypeImported:
		((CEvnt *)_source)->copyTo((CImportedEvnt *)_dest);
		_dest->setEventType(evntTypeImported);
		_dest->setDetectionSourceManOrAuto(manualDetection);
		_dest->setHasLevelDetection(false);
		_dest->setEventFlags(_source->getEventFlags() | EVNT_DEF_MANUAL | EVNT_DEF_MANUAL_TYPE);
		ret = true;
		break;
	case evntTypeObstr:
		((CEvnt *)_source)->copyTo((CObstrEvnt *) _dest);
		_dest->setEventType(evntTypeObstr);
		_dest->setDetectionSourceManOrAuto(manualDetection);
		_dest->setHasLevelDetection(true);
		_dest->setEventFlags(_source->getEventFlags() | EVNT_DEF_MANUAL | EVNT_DEF_MANUAL_TYPE);
		ret = true;
		break;
	case evntTypeHypoObstr:
		((CEvnt *)_source)->copyTo((CHypoEvnt *) _dest);
		((CHypoEvnt *)_dest)->setHypoType(destType);
		((CHypoEvnt *)_dest)->setSpO2DropPartner(.0f, false);
		_dest->setEventType(evntTypeHypoObstr);
		_dest->setDetectionSourceManOrAuto(manualDetection);
		_dest->setHasLevelDetection(true);
		_dest->setEventFlags(_source->getEventFlags() | EVNT_DEF_MANUAL | EVNT_DEF_MANUAL_TYPE);
		ret = true;
		break;
	case evntTypeHypoCentral:
		((CEvnt *)_source)->copyTo((CHypoEvnt *) _dest);
		((CHypoEvnt *)_dest)->setHypoType(destType);
		((CHypoEvnt *)_dest)->setSpO2DropPartner(.0f, false);
		_dest->setEventType(evntTypeHypoCentral);
		_dest->setDetectionSourceManOrAuto(manualDetection);
		_dest->setHasLevelDetection(false);
		_dest->setEventFlags(_source->getEventFlags() | EVNT_DEF_MANUAL | EVNT_DEF_MANUAL_TYPE);
		ret = true;
		break;
	default:
		break;
	}

	return ret;
}

/*
Description: Copies from the particular event type to another
Returns: true if ok, false if not
*/
bool CEvents::copyCentralEventTo(CCentralEvnt *_source, CEvnt *_dest)
{
	int destType = _dest->getEventType();

	ASSERT((evntTypeMixed == destType) ||
		(evntTypeCentral == destType) ||
		(evntTypeObstr == destType) ||
		(evntTypeHypoObstr == destType) ||
		(evntTypeHypoCentral == destType) ||
		(evntTypeImported == destType));

	bool ret = false;

	switch (destType) {
	case evntTypeMixed:
		((CEvnt *)_source)->copyTo((CMixedEvnt *) _dest);
		_dest->setEventType(evntTypeMixed);
		_dest->setDetectionSourceManOrAuto(manualDetection);
		_dest->setHasLevelDetection(true);
		((CMixedEvnt *)_dest)->doLevelAnalysis();
		_dest->setEventFlags(_source->getEventFlags() | EVNT_DEF_MANUAL | EVNT_DEF_MANUAL_TYPE);
		ret = true;
		break;
	case evntTypeImported:
		((CEvnt *)_source)->copyTo((CImportedEvnt *)_dest);
		_dest->setEventType(evntTypeImported);
		_dest->setDetectionSourceManOrAuto(manualDetection);
		_dest->setHasLevelDetection(false);
		_dest->setEventFlags(_source->getEventFlags() | EVNT_DEF_MANUAL | EVNT_DEF_MANUAL_TYPE);
		ret = true;
		break;
	case evntTypeCentral:
		_source->copyTo((CCentralEvnt *)_dest);
		ret = true;
		break;
	case evntTypeObstr:
		((CEvnt *)_source)->copyTo((CObstrEvnt *) _dest);
		_dest->setEventType(evntTypeObstr);
		_dest->setDetectionSourceManOrAuto(manualDetection);
		_dest->setHasLevelDetection(true);
		((CObstrEvnt *)_dest)->doLevelAnalysis();
		_dest->setEventFlags(_source->getEventFlags() | EVNT_DEF_MANUAL | EVNT_DEF_MANUAL_TYPE);
		ret = true;
		break;
	case evntTypeHypoObstr:
		((CEvnt *)_source)->copyTo((CHypoEvnt *) _dest);
		((CHypoEvnt *)_dest)->setHypoType(destType);
		((CHypoEvnt *)_dest)->setSpO2DropPartner(.0f, false);
		_dest->setEventType(evntTypeHypoObstr);
		_dest->setDetectionSourceManOrAuto(manualDetection);
		_dest->setHasLevelDetection(true);
		((CHypoEvnt *)_dest)->doLevelAnalysis();
		_dest->setEventFlags(_source->getEventFlags() | EVNT_DEF_MANUAL | EVNT_DEF_MANUAL_TYPE);
		ret = true;
		break;
	case evntTypeHypoCentral:
		((CEvnt *)_source)->copyTo((CHypoEvnt *) _dest);
		((CHypoEvnt *)_dest)->setHypoType(destType);
		((CHypoEvnt *)_dest)->setSpO2DropPartner(.0f, false);
		_dest->setEventType(evntTypeHypoCentral);
		_dest->setDetectionSourceManOrAuto(manualDetection);
		_dest->setHasLevelDetection(false);
		_dest->setEventFlags(_source->getEventFlags() | EVNT_DEF_MANUAL | EVNT_DEF_MANUAL_TYPE);
		ret = true;
		break;
	default:
		break;
	}

	return ret;
}

/*
Description: Copies from the particular event type to another
Returns: true if ok, false if not
*/
bool CEvents::copyObstrEventTo(CObstrEvnt *_source, CEvnt *_dest)
{
	int destType = _dest->getEventType();

	ASSERT((evntTypeMixed == destType) ||
		(evntTypeCentral == destType) ||
		(evntTypeObstr == destType) ||
		(evntTypeHypoObstr == destType) ||
		(evntTypeHypoCentral == destType) ||
		(evntTypeImported == destType));

	bool ret = false;

	switch (destType) {
	case evntTypeMixed:
		((CEvnt *)_source)->copyTo((CMixedEvnt *) _dest);
		_dest->setEventType(evntTypeMixed);
		_dest->setDetectionSourceManOrAuto(manualDetection);
		_dest->setHasLevelDetection(true);
		_dest->setEventFlags(_source->getEventFlags() | EVNT_DEF_MANUAL | EVNT_DEF_MANUAL_TYPE);
		ret = true;
		break;
	case evntTypeCentral:
		((CEvnt *)_source)->copyTo((CCentralEvnt *) _dest);
		_dest->setEventType(evntTypeCentral);
		_dest->setDetectionSourceManOrAuto(manualDetection);
		_dest->setHasLevelDetection(false);
		_dest->setEventFlags(_source->getEventFlags() | EVNT_DEF_MANUAL | EVNT_DEF_MANUAL_TYPE);
		ret = true;
		break;
	case evntTypeImported:
		((CEvnt *)_source)->copyTo((CImportedEvnt *)_dest);
		_dest->setEventType(evntTypeImported);
		_dest->setDetectionSourceManOrAuto(manualDetection);
		_dest->setHasLevelDetection(false);
		_dest->setEventFlags(_source->getEventFlags() | EVNT_DEF_MANUAL | EVNT_DEF_MANUAL_TYPE);
		ret = true;
		break;
	case evntTypeObstr:
		_source->copyTo((CObstrEvnt *)_dest);
		ret = true;
		break;
	case evntTypeHypoObstr:
		((CEvnt *)_source)->copyTo((CHypoEvnt *) _dest);
		((CHypoEvnt *)_dest)->setHypoType(destType);
		((CHypoEvnt *)_dest)->setSpO2DropPartner(.0f, false);
		_dest->setEventType(evntTypeHypoObstr);
		_dest->setDetectionSourceManOrAuto(manualDetection);
		_dest->setHasLevelDetection(true);
		_dest->setEventFlags(_source->getEventFlags() | EVNT_DEF_MANUAL | EVNT_DEF_MANUAL_TYPE);
		ret = true;
		break;
	case evntTypeHypoCentral:
		((CEvnt *)_source)->copyTo((CHypoEvnt *) _dest);
		((CHypoEvnt *)_dest)->setHypoType(destType);
		((CHypoEvnt *)_dest)->setSpO2DropPartner(.0f, false);
		_dest->setEventType(evntTypeHypoCentral);
		_dest->setDetectionSourceManOrAuto(manualDetection);
		_dest->setHasLevelDetection(false);
		_dest->setEventFlags(_source->getEventFlags() | EVNT_DEF_MANUAL | EVNT_DEF_MANUAL_TYPE);
		ret = true;
		break;
	default:
		break;
	}

	return ret;
}

bool CEvents::copyImportedEventTo(CImportedEvnt * _source, CEvnt * _dest)
{
	int destType = _dest->getEventType();

	ASSERT((evntTypeMixed == destType) ||
		(evntTypeCentral == destType) ||
		(evntTypeObstr == destType) ||
		(evntTypeHypoObstr == destType) ||
		(evntTypeHypoCentral == destType) ||
		(evntTypeImported == destType));

	bool ret = false;

	switch (destType) {
	case evntTypeMixed:
		((CEvnt *)_source)->copyTo((CMixedEvnt *)_dest);
		_dest->setEventType(evntTypeMixed);
		_dest->setDetectionSourceManOrAuto(manualDetection);
		_dest->setHasLevelDetection(true);
		_dest->setEventFlags(_source->getEventFlags() | EVNT_DEF_MANUAL | EVNT_DEF_MANUAL_TYPE);
		ret = true;
		break;
	case evntTypeCentral:
		((CEvnt *)_source)->copyTo((CCentralEvnt *)_dest);
		_dest->setEventType(evntTypeCentral);
		_dest->setDetectionSourceManOrAuto(manualDetection);
		_dest->setHasLevelDetection(false);
		_dest->setEventFlags(_source->getEventFlags() | EVNT_DEF_MANUAL | EVNT_DEF_MANUAL_TYPE);
		ret = true;
		break;
	case evntTypeImported:
		_source->copyTo((CImportedEvnt *)_dest);
		ret = true;
		break;
	case evntTypeObstr:
		((CEvnt *)_source)->copyTo((CObstrEvnt *)_dest);
		_dest->setEventType(evntTypeObstr);
		_dest->setDetectionSourceManOrAuto(manualDetection);
		_dest->setHasLevelDetection(false);
		_dest->setEventFlags(_source->getEventFlags() | EVNT_DEF_MANUAL | EVNT_DEF_MANUAL_TYPE);
		ret = true;
		break;
	case evntTypeHypoObstr:
		((CEvnt *)_source)->copyTo((CHypoEvnt *)_dest);
		((CHypoEvnt *)_dest)->setHypoType(destType);
		((CHypoEvnt *)_dest)->setSpO2DropPartner(.0f, false);
		_dest->setEventType(evntTypeHypoObstr);
		_dest->setDetectionSourceManOrAuto(manualDetection);
		_dest->setHasLevelDetection(true);
		_dest->setEventFlags(_source->getEventFlags() | EVNT_DEF_MANUAL | EVNT_DEF_MANUAL_TYPE);
		ret = true;
		break;
	case evntTypeHypoCentral:
		((CEvnt *)_source)->copyTo((CHypoEvnt *)_dest);
		((CHypoEvnt *)_dest)->setHypoType(destType);
		((CHypoEvnt *)_dest)->setSpO2DropPartner(.0f, false);
		_dest->setEventType(evntTypeHypoCentral);
		_dest->setDetectionSourceManOrAuto(manualDetection);
		_dest->setHasLevelDetection(false);
		_dest->setEventFlags(_source->getEventFlags() | EVNT_DEF_MANUAL | EVNT_DEF_MANUAL_TYPE);
		ret = true;
		break;
	default:
		break;
	}

	return ret;
}

/*
Description: Copies from the particular event type to another
Returns: true if ok, false if not
*/
bool CEvents::copyHypoEventTo(CHypoEvnt *_source, CEvnt *_dest)
{
	int destType = _dest->getEventType();

	ASSERT((evntTypeMixed == destType) ||
		(evntTypeCentral == destType) ||
		(evntTypeObstr == destType) ||
		(evntTypeHypoObstr == destType) ||
		(evntTypeHypoCentral == destType) ||
		(evntTypeImported == destType));

	bool ret = false;

	switch (destType) {
	case evntTypeMixed:
		((CEvnt *)_source)->copyTo((CMixedEvnt *) _dest);
		_dest->setEventType(evntTypeMixed);
		_dest->setDetectionSourceManOrAuto(manualDetection);
		_dest->setHasLevelDetection(true);
		((CMixedEvnt *)_dest)->doLevelAnalysis();
		_dest->setEventFlags(_source->getEventFlags() | EVNT_DEF_MANUAL | EVNT_DEF_MANUAL_TYPE);
		ret = true;
		break;
	case evntTypeCentral:
		((CEvnt *)_source)->copyTo((CCentralEvnt *) _dest);
		_dest->setEventType(evntTypeCentral);
		_dest->setDetectionSourceManOrAuto(manualDetection);
		_dest->setHasLevelDetection(false);
		_dest->setEventFlags(_source->getEventFlags() | EVNT_DEF_MANUAL | EVNT_DEF_MANUAL_TYPE);
		ret = true;
		break;
	case evntTypeImported:
		((CEvnt *)_source)->copyTo((CImportedEvnt *)_dest);
		_dest->setEventType(evntTypeImported);
		_dest->setDetectionSourceManOrAuto(manualDetection);
		_dest->setHasLevelDetection(false);
		_dest->setEventFlags(_source->getEventFlags() | EVNT_DEF_MANUAL | EVNT_DEF_MANUAL_TYPE);
		ret = true;
		break;
	case evntTypeObstr:
		((CEvnt *)_source)->copyTo((CObstrEvnt *)_dest);
		_dest->setEventType(evntTypeObstr);
		_dest->setDetectionSourceManOrAuto(manualDetection);
		_dest->setHasLevelDetection(true);
		_dest->setEventFlags(_source->getEventFlags() | EVNT_DEF_MANUAL | EVNT_DEF_MANUAL_TYPE);
		ret = true;
		break;
	case evntTypeHypoObstr:
		((CEvnt *)_source)->copyTo((CHypoEvnt *) _dest);
		((CHypoEvnt *)_dest)->setHypoType(destType);
		((CHypoEvnt *)_dest)->setSpO2DropPartner(.0f, false);
		_dest->setEventType(evntTypeHypoObstr);
		_dest->setDetectionSourceManOrAuto(manualDetection);
		_dest->setHasLevelDetection(true);
		((CHypoEvnt *)_dest)->doLevelAnalysis();
		_dest->setEventFlags(_source->getEventFlags() | EVNT_DEF_MANUAL | EVNT_DEF_MANUAL_TYPE);
		ret = true;
		break;
	case evntTypeHypoCentral:
		((CEvnt *)_source)->copyTo((CHypoEvnt *) _dest);
		((CHypoEvnt *)_dest)->setHypoType(destType);
		((CHypoEvnt *)_dest)->setSpO2DropPartner(.0f, false);
		_dest->setEventType(evntTypeHypoCentral);
		_dest->setDetectionSourceManOrAuto(manualDetection);
		_dest->setHasLevelDetection(false);
		_dest->setEventFlags(_source->getEventFlags() | EVNT_DEF_MANUAL | EVNT_DEF_MANUAL_TYPE);
		ret = true;
		break;
	default:
		break;
	}

	return ret;
}

/*
Description: TRACE or report stuff during debugging
*/
bool CEvents::debugHelper(int _posInCode)
{
#ifdef _DEBUG
	int num3 = arousalEventArrayDesat3.GetCount();
	int cnt3Rera = 0;
	int cnt3Active = 0;
	for (int i = 0; i < num3; i++) {
		CArousalEvnt *evP = arousalEventArrayDesat3.GetAt(i);
		CRERAEvnt *rP = evP->getRERAEvnt();
	
		if (evP->getIsActive(detectionCriteria.hypopneaDropLimit)) {
			if (rP) cnt3Rera++;
			cnt3Active++;
		}
	}

	int num4 = arousalEventArrayDesat4.GetCount();
	int cnt4Rera = 0;
	int cnt4Active = 0;
	for (int i = 0; i < num4; i++) {
		CArousalEvnt *evP = arousalEventArrayDesat4.GetAt(i);
		CRERAEvnt *rP = evP->getRERAEvnt();
		if (evP->getIsActive(detectionCriteria.hypopneaDropLimit)) {
			if (rP) cnt4Rera++;
			cnt4Active++;
		}
	}
#endif

	return true;
}

int CEvents::computeCurrentHypopneaDropLimitEnum(float _detCritDrop)
{
	int dropLim = noHypopneaDropLimit;
	int hypoLim = (int)(_detCritDrop * 100.0f);
	switch (hypoLim) {
	case 50:
		dropLim = hypopneaDropLimit50; // (0)
		break;
	case 60:
		dropLim = hypopneaDropLimit40; // (1)
		break;
	case 70:
		dropLim = hypopneaDropLimit30; // (2)
		break;
	}
	return dropLim;
}

/*
Description: Count number of hypopneas
Parameters:
_hypoLimit: hypopnea drop limit  (enum)
_spO2Drop: SpO2 drop limt (enum)
Returns: The number
*/
int CEvents::countHypEvents(int _hypoLimit, int _spO2Drop)
{
	if (spO2DropLimit3 == _spO2Drop) {
		int num = hypoEventArrayDesat3.GetCount();
		int cnt = 0;
		numCentralHypopnea[_hypoLimit][_spO2Drop] = 0;
		numObstrHypopnea[_hypoLimit][_spO2Drop] = 0;
		for (int i = 0; i < num; i++) {
			CHypoEvnt *evtP = hypoEventArrayDesat3.GetAt(i);
			if (evtP->getIsActive() && (evtP->pickFromDropLevel(_hypoLimit))) {
				cnt++;
				if (hypoTypeObstr == evtP->getHypoType()) numObstrHypopnea[_hypoLimit][_spO2Drop]++;
				else if (hypoTypeCentral == evtP->getHypoType()) numCentralHypopnea[_hypoLimit][_spO2Drop]++;
			}
		}
		return cnt;
	}
	else if (spO2DropLimit4 == _spO2Drop) {
		int num = hypoEventArrayDesat4.GetCount();
		int cnt = 0;
		numCentralHypopnea[_hypoLimit][_spO2Drop] = 0;
		numObstrHypopnea[_hypoLimit][_spO2Drop] = 0;
		for (int i = 0; i < num; i++) {
			CHypoEvnt *evtP = hypoEventArrayDesat4.GetAt(i);
			if (evtP->getIsActive() && (evtP->pickFromDropLevel(_hypoLimit))) {
				cnt++;
				if (hypoTypeObstr == evtP->getHypoType()) numObstrHypopnea[_hypoLimit][_spO2Drop]++;
				else if (hypoTypeCentral == evtP->getHypoType()) numCentralHypopnea[_hypoLimit][_spO2Drop]++;
			}
		}
		return cnt;
	}
	return 0;
}

int CEvents::countREREvents(int _hypoLimit, int _spO2Drop)
{
	if (spO2DropLimit3 == _spO2Drop) {
		int num = arousalEventArrayDesat3.GetCount();
		int cnt = 0;
		for (int i = 0; i < num; i++) {
			CArousalEvnt *evtP = arousalEventArrayDesat3.GetAt(i);
			CRERAEvnt *rP = evtP->getRERAEvnt();
			if (evtP->getIsActive(_hypoLimit) && rP) cnt++;
		}
		return cnt;
	}
	if (spO2DropLimit4 == _spO2Drop) {
		int num = arousalEventArrayDesat4.GetCount();
		int cnt = 0;
		for (int i = 0; i < num; i++) {
			CArousalEvnt *evtP = arousalEventArrayDesat4.GetAt(i);
			CRERAEvnt *rP = evtP->getRERAEvnt();
			if (evtP->getIsActive(_hypoLimit) && rP) cnt++;
		}
		return cnt;
	}
	return 0;
}

int CEvents::getCountREREvents(void)
{
	return countREREvents(detectionCriteria.hypopneaDropLimit, detectionCriteria.spO2DropLimit);
}

int CEvents::countAroEvents(int _hypoLimit, int _spO2Drop)
{
	if (spO2DropLimit3 == _spO2Drop) {
		int num = arousalEventArrayDesat3.GetCount();
		int cnt = 0;
		for (int i = 0; i < num; i++) {
			CArousalEvnt *evtP = arousalEventArrayDesat3.GetAt(i);
			if (evtP->getIsActive(_hypoLimit)) cnt++;
		}
		return cnt;
	}
	if (spO2DropLimit4 == _spO2Drop) {
		int num = arousalEventArrayDesat4.GetCount();
		int cnt = 0;
		for (int i = 0; i < num; i++) {
			CArousalEvnt *evtP = arousalEventArrayDesat4.GetAt(i);
			if (evtP->getIsActive(_hypoLimit)) cnt++;
		}
		return cnt;
	}
	return 0;
}

int CEvents::getCountAroEvents(void)		
{
	return countAroEvents(detectionCriteria.hypopneaDropLimit, detectionCriteria.spO2DropLimit);
}

int CEvents::getCountSpoEvents(void)
{
	return countSpoEvents(detectionCriteria.spO2DropLimit);
}

int CEvents::countSpoEvents(int _spO2Drop)
{
	if (spO2DropLimit3 == _spO2Drop) {
		int num = spO2DropEventArray3.GetCount();
		int cnt = 0;
		for (int i = 0; i < num; i++) {
			CSpO2DropEvnt *evtP = spO2DropEventArray3.GetAt(i);
			if (evtP->getIsActive()) cnt++;
		}
		return cnt;
	}
	else if (spO2DropLimit4 == _spO2Drop) {
		int num = spO2DropEventArray4.GetCount();
		int cnt = 0;
		for (int i = 0; i < num; i++) {
			CSpO2DropEvnt *evtP = spO2DropEventArray4.GetAt(i);
			if (evtP->getIsActive()) cnt++;
		}
		return cnt;
	}
	return 0;
}

int CEvents::getCountManEvents(void)		
{ 
	return manualMarkerEventArray.GetCount(); 
}

int CEvents::getCountImpEvents(void)
{
	return importedEventArray.GetCount();
}

int CEvents::getCountAwaEvents(void)		
{
	int num = awakeEventArray.GetCount();
	int cnt = 0;
	for (int i = 0; i < num; i++) {
		CAwakeEvnt *evtP = awakeEventArray.GetAt(i);
		if (evtP->getIsActive()) cnt++;
	}
	return cnt;
}

int CEvents::getCountSnoEvents(void)		
{
	int num = snoringEventArray.GetCount();
	int cnt = 0;
	for (int i = 0; i < num; i++) {
		CSnoringEvnt *evtP = snoringEventArray.GetAt(i);
		if (evtP->getIsActive()) cnt++;
	}
	return cnt;
}

int CEvents::getCountSwallowEvents(void) 
{
	int num = swallowEventArray.GetCount();
	int cnt = 0;
	for (int i = 0; i < num; i++) {
		CSwallowEvnt *evtP = swallowEventArray.GetAt(i);
		if (evtP->getIsActive()) cnt++;
	}
	return cnt;
}

int CEvents::getCountExcludedEvents(void)	{ return excludedEventArray.GetCount(); }

/*
Description: Moves the edge of an event.
Make sure that the undo-stacks are used.
*/
void CEvents::moveEventEdge(CEvnt *_evnt,FLOAT _time,int _edge)
{
	CEvnt *eP = removeAndReplaceEvent(_evnt);
	if (eP) {
		eP->moveEdge(_time,_edge);
	}
}
/*
Description: Change the event type. Move from one event array to another
*/
void CEvents::changeEventType(CEvnt *_evnt, int _type)
{
	int oldType = _evnt->getEventType();

	bool ok = false;
	switch (oldType) {
		case evntTypeMixed		:
			ok = removeMixed(_evnt);
			if (ok) changeEventTypeDuringEdit(_evnt, _type,detectionCriteria.spO2DropLimit);
			break;
		case evntTypeCentral	:
			ok = removeCentral(_evnt);
			if (ok) changeEventTypeDuringEdit(_evnt, _type,detectionCriteria.spO2DropLimit);
			break;
		case evntTypeObstr		:
			ok = removeObstr(_evnt);
			if (ok) changeEventTypeDuringEdit(_evnt, _type, detectionCriteria.spO2DropLimit);
			break;
		case evntTypeHypoCentral:
			if (evntTypeHypoObstr == _type) ((CHypoEvnt *) _evnt)->setHypoType(hypoTypeObstr);
			else {
				ok = removeHypo(_evnt,detectionCriteria.spO2DropLimit);
				if (ok) changeEventTypeDuringEdit(_evnt, _type,detectionCriteria.spO2DropLimit);
			}
			break;
		case evntTypeHypoObstr  :
			if (evntTypeHypoCentral == _type) ((CHypoEvnt *)_evnt)->setHypoType(hypoTypeCentral);
			else {
				ok = removeHypo(_evnt, detectionCriteria.spO2DropLimit);
				if (ok) changeEventTypeDuringEdit(_evnt, _type, detectionCriteria.spO2DropLimit);
			}
			break;
		case evntTypeImported:
			ok = removeImported(_evnt);
			if (ok) changeEventTypeDuringEdit(_evnt, _type, detectionCriteria.spO2DropLimit);
			break;
		case evntTypeManMarker	:
		case evntTypeExcluded	:
		case evntTypeCursorWnd	:
		case evntTypeStartStop	:
		default:
			break;
	}
}

/*
Description: 
THIS IS OLD
	Sort the events according to start time.
	After that, removes all overlaps.
*/
//void CEvents::OLDsortAndOrganiseMixed(void)
//{
//	if (!cathData) return;
//
//	vector <EVENT_SORT_VECTOR_ELEMENT> evV;
//	int num = mixedEventArray.GetCount();
//	for (int i = 0; i < num; ++i) {
//		CMixedEvnt *eP = mixedEventArray.GetAt(i);
//		EVENT_SORT_VECTOR_ELEMENT eel;
//		eel.begin = eP->getFrom();
//		eel.end = eP->getTo();
//		eel.evP = NULL;
//		eel.level = eP->getLevelType();
//		eel.hypopneaDropLimit = noHypopneaDropLimit;
//		evV.push_back(eel);
//		delete(eP);
//	}
//
//	OLDsortAndOrganiseEvntSortVector(&evV,true);
//
//	//---Need these
//	vector <FLOAT> *flow = cathData->getFlowVector();
//	vector <FLOAT> *flowTime = cathData->getFlowVectorTime();
//	vector <FLOAT> *vpGrad = cathData->getPgradientVector();
//	vector <FLOAT> *tpGrad = cathData->getPgradientVectorTime();
//	vector <FLOAT> *vpoes = cathData->getPOESEnvVector();
//	vector <FLOAT> *poesTime = cathData->getPOESEnvVectorTime();
//
//	//---Fills up the array again
//	mixedEventArray.RemoveAll();
//	size_t size = evV.size();
//	vector <EVENT_SORT_VECTOR_ELEMENT>::iterator evVIt = evV.begin();
//	for (; evVIt < evV.end(); ++evVIt) {
//		EVENT_SORT_VECTOR_ELEMENT eel = *evVIt; 
//
//		short bp = bpData->getPos(eel.begin, eel.end);
//		CMixedEvnt *eP = new CMixedEvnt(eel.begin, eel.end, startRecordingClockTime, bp);
//
//		gradientDataToEvent((CEvnt *)eP, vpGrad, tpGrad, eel.begin, eel.end);
//		flowDataToEvent((CEvnt *)eP, flow, flowTime, eel.begin, eel.end); 
//		poesEnvDataToEvent((CEvnt *)eP, vpoes, poesTime, eel.begin,eel.end);
//		eP->setLevelType(eel.level);
//		mixedEventArray.Add(eP);
//	}
//}

/*
Description:
Sort the events according to start time.
After that, removes all overlaps.
This is OLD
*/
//void CEvents::OLDsortAndOrganiseCentral(void)
//{
//	if (!cathData) return;
//
//	vector <EVENT_SORT_VECTOR_ELEMENT> evV;
//	int num = centralEventArray.GetCount();
//	for (int i = 0; i < num; ++i) {
//		CCentralEvnt *eP = centralEventArray.GetAt(i);
//		EVENT_SORT_VECTOR_ELEMENT eel;
//		eel.begin = eP->getFrom();
//		eel.end = eP->getTo();
//		eel.evP = NULL;
//		eel.level = eP->getLevelType();
//		eel.hypopneaDropLimit = noHypopneaDropLimit;
//		evV.push_back(eel);
//		delete(eP);
//	}
//
//	OLDsortAndOrganiseEvntSortVector(&evV, false);
//
//	//---Need these
//	vector <FLOAT> *flow = cathData->getFlowVector();
//	vector <FLOAT> *flowTime = cathData->getFlowVectorTime();
//	vector <FLOAT> *vpGrad = cathData->getPgradientVector();
//	vector <FLOAT> *tpGrad = cathData->getPgradientVectorTime();
//	vector <FLOAT> *vpoes = cathData->getPOESEnvVector();
//	vector <FLOAT> *poesTime = cathData->getPOESEnvVectorTime();
//
//	//---Fills up the array again
//	centralEventArray.RemoveAll();
//	size_t size = evV.size();
//	vector <EVENT_SORT_VECTOR_ELEMENT>::iterator evVIt = evV.begin();
//	for (; evVIt < evV.end(); ++evVIt) {
//		EVENT_SORT_VECTOR_ELEMENT eel = *evVIt;
//
//		short bp = bpData->getPos(eel.begin, eel.end);
//		CCentralEvnt *eP = new CCentralEvnt(eel.begin, eel.end, startRecordingClockTime, bp);
//		gradientDataToEvent((CEvnt *)eP, vpGrad, tpGrad, eel.begin, eel.end);
//		flowDataToEvent((CEvnt *)eP, flow, flowTime, eel.begin, eel.end);
//		poesEnvDataToEvent((CEvnt *)eP, vpoes, poesTime, eel.begin, eel.end);
//		centralEventArray.Add(eP);
//	}
//}

/*
Description:
Sort the events according to start time.
After that, removes all overlaps.
This is OLD
*/
//void CEvents::OLDsortAndOrganiseObstr(void)
//{
//	vector <EVENT_SORT_VECTOR_ELEMENT> evV;
//	int num = obstrEventArray.GetCount();
//	for (int i = 0; i < num; ++i) {
//		CObstrEvnt *eP = obstrEventArray.GetAt(i);
//		EVENT_SORT_VECTOR_ELEMENT eel;
//		eel.begin = eP->getFrom();
//		eel.end = eP->getTo();
//		eel.evP = NULL;
//		eel.level = eP->getLevelType();
//		eel.hypopneaDropLimit = noHypopneaDropLimit;
//		evV.push_back(eel);
//		delete(eP);
//	}
//
//	OLDsortAndOrganiseEvntSortVector(&evV, true);
//
//	//---Need these
//	vector <FLOAT> *flow = cathData->getFlowVector();
//	vector <FLOAT> *flowTime = cathData->getFlowVectorTime();
//	vector <FLOAT> *vpGrad = cathData->getPgradientVector();
//	vector <FLOAT> *tpGrad = cathData->getPgradientVectorTime();
//	vector <FLOAT> *vpoes = cathData->getPOESEnvVector();
//	vector <FLOAT> *poesTime = cathData->getPOESEnvVectorTime();
//
//	//---Fills up the array again
//	obstrEventArray.RemoveAll();
//	size_t size = evV.size();
//	vector <EVENT_SORT_VECTOR_ELEMENT>::iterator evVIt = evV.begin();
//	for (; evVIt < evV.end(); ++evVIt) {
//		EVENT_SORT_VECTOR_ELEMENT eel = *evVIt;
//
//		short bp = bpData->getPos(eel.begin, eel.end);
//		CObstrEvnt *eP = new CObstrEvnt(eel.begin, eel.end, startRecordingClockTime, bp);
//
//		gradientDataToEvent((CEvnt *)eP, vpGrad, tpGrad, eel.begin, eel.end);
//		flowDataToEvent((CEvnt *)eP, flow, flowTime, eel.begin, eel.end);
//		poesEnvDataToEvent((CEvnt *)eP, vpoes, poesTime, eel.begin, eel.end);
//		eP->setLevelType(eel.level);
//		obstrEventArray.Add(eP);
//	}
//}

/*
Description:
Sort the events according to start time.
After that, removes all overlaps.
This is OLD
*/
//void CEvents::OLDsortAndOrganiseHypo(int _spO2Drop)
//{
//	CArray <CHypoEvnt *, CHypoEvnt *> *aP = NULL;
//	if (spO2DropLimit3 == _spO2Drop) aP = &hypoEventArrayDesat3;
//	else if (spO2DropLimit4 == _spO2Drop) aP = &hypoEventArrayDesat4;
//	else return;
//
//	//---Obstructive hypo
//	{
//		vector <EVENT_SORT_VECTOR_ELEMENT> evV;
//		int num = aP->GetCount();
//		for (int i = 0; i < num; ++i) {
//			CHypoEvnt *eP = aP->GetAt(i);
//			if (hypoTypeObstr == eP->getHypoType()) {
//				EVENT_SORT_VECTOR_ELEMENT eel;
//				eel.begin = eP->getFrom();
//				eel.end = eP->getTo();
//				eel.evP = NULL;
//				eel.level = eP->getLevelType();
//				eel.hypopneaDropLimit = eP->getHypoDropLimit();
//				evV.push_back(eel);
//				delete(eP);
//			}
//		}
//
//		OLDsortAndOrganiseEvntSortVector(&evV, true);
//
//		//---Need these
//		vector <FLOAT> *flow = cathData->getFlowVector();
//		vector <FLOAT> *flowTime = cathData->getFlowVectorTime();
//		vector <FLOAT> *vpGrad = cathData->getPgradientVector();
//		vector <FLOAT> *tpGrad = cathData->getPgradientVectorTime();
//		vector <FLOAT> *vpoes = cathData->getPOESEnvVector();
//		vector <FLOAT> *poesTime = cathData->getPOESEnvVectorTime();
//
//		//---Fills up the array again
//		aP->RemoveAll();
//		size_t size = evV.size();
//		vector <EVENT_SORT_VECTOR_ELEMENT>::iterator evVIt = evV.begin();
//		for (; evVIt < evV.end(); ++evVIt) {
//			EVENT_SORT_VECTOR_ELEMENT eel = *evVIt;
//
//			short bp = bpData->getPos(eel.begin, eel.end);
//			CHypoEvnt *eP = new CHypoEvnt(eel.begin, eel.end, startRecordingClockTime, bp,eel.hypopneaDropLimit);
//			gradientDataToEvent((CEvnt *)eP, vpGrad, tpGrad, eel.begin, eel.end);
//			flowDataToEvent((CEvnt *)eP, flow, flowTime, eel.begin, eel.end);
//			poesEnvDataToEvent((CEvnt *)eP, vpoes, poesTime, eel.begin, eel.end);
//			eP->setLevelType(eel.level);
//			eP->setHypoType(hypoTypeObstr);
//			aP->Add(eP);
//		}
//	}
//
//	//---Central hypo
//	{
//		vector <EVENT_SORT_VECTOR_ELEMENT> evV;
//		int num = aP->GetCount();
//		for (int i = 0; i < num; ++i) {
//			CHypoEvnt *eP = aP->GetAt(i);
//			if (hypoTypeCentral == eP->getHypoType()) {
//				EVENT_SORT_VECTOR_ELEMENT eel;
//				eel.begin = eP->getFrom();
//				eel.end = eP->getTo();
//				eel.evP = NULL;
//				eel.level = eP->getLevelType();
//				eel.hypopneaDropLimit = eP->getHypoDropLimit();
//				evV.push_back(eel);
//				delete(eP);
//			}
//		}
//
//		OLDsortAndOrganiseEvntSortVector(&evV, false);
//
//		//---Need these
//		vector <FLOAT> *flow = cathData->getFlowVector();
//		vector <FLOAT> *flowTime = cathData->getFlowVectorTime();
//		vector <FLOAT> *vpGrad = cathData->getPgradientVector();
//		vector <FLOAT> *tpGrad = cathData->getPgradientVectorTime();
//		vector <FLOAT> *vpoes = cathData->getPOESEnvVector();
//		vector <FLOAT> *poesTime = cathData->getPOESEnvVectorTime();
//
//		//---Fills up the array again
//		aP->RemoveAll();
//		size_t size = evV.size();
//		vector <EVENT_SORT_VECTOR_ELEMENT>::iterator evVIt = evV.begin();
//		for (; evVIt < evV.end(); ++evVIt) {
//			EVENT_SORT_VECTOR_ELEMENT eel = *evVIt;
//
//			short bp = bpData->getPos(eel.begin, eel.end);
//			CHypoEvnt *eP = new CHypoEvnt(eel.begin, eel.end, startRecordingClockTime, bp,eel.hypopneaDropLimit);
//			gradientDataToEvent((CEvnt *)eP, vpGrad, tpGrad, eel.begin, eel.end);
//			flowDataToEvent((CEvnt *)eP, flow, flowTime, eel.begin, eel.end);
//			poesEnvDataToEvent((CEvnt *)eP, vpoes, poesTime, eel.begin, eel.end);
//			eP->setLevelType(eel.level);
//			eP->setHypoType(hypoTypeCentral);
//			aP->Add(eP);
//		}
//	}
//}

/*
Description:
Sort the events according to start time.
After that, removes all overlaps.
This is OLD
*/
//void CEvents::OLDsortAndOrganiseArousal(int _spO2Drop)
//{
//	CArray <CArousalEvnt *, CArousalEvnt *> *aP = NULL;
//	if (spO2DropLimit3 == _spO2Drop) aP = &arousalEventArrayDesat3;
//	else if (spO2DropLimit4 == _spO2Drop) aP = &arousalEventArrayDesat4;
//	else return;
//
//	vector <EVENT_SORT_VECTOR_ELEMENT> evV;
//	int num = aP->GetCount();
//	for (int i = 0; i < num; ++i) {
//		CArousalEvnt *eP = aP->GetAt(i);
//		EVENT_SORT_VECTOR_ELEMENT eel;
//		eel.begin = eP->getFrom();
//		eel.end = eP->getTo();
//		eel.evP = NULL;
//		eel.level = eP->getLevelType();
//		eel.hypopneaDropLimit = noHypopneaDropLimit;
//		evV.push_back(eel);
//		delete(eP);
//	}
//
//	OLDsortAndOrganiseEvntSortVector(&evV, false);
//
//	//---Need these
//	vector <FLOAT> *flow = cathData->getFlowVector();
//	vector <FLOAT> *flowTime = cathData->getFlowVectorTime();
//	vector <FLOAT> *vpGrad = cathData->getPgradientVector();
//	vector <FLOAT> *tpGrad = cathData->getPgradientVectorTime();
//
//	//---Fills up the array again
//	aP->RemoveAll();
//	size_t size = evV.size();
//	vector <EVENT_SORT_VECTOR_ELEMENT>::iterator evVIt = evV.begin();
//	for (; evVIt < evV.end(); ++evVIt) {
//		EVENT_SORT_VECTOR_ELEMENT eel = *evVIt;
//
//		short bp = bpData->getPos(eel.begin, eel.end);
//		CArousalEvnt *eP = new CArousalEvnt(eel.begin, eel.end, startRecordingClockTime, bp);
//		aP->Add(eP);
//	}
//}
//
/*
Description:
Sort the events according to start time.
After that, removes all overlaps.
This is OLD
*/
//void CEvents::OLDsortAndOrganiseSpO2Drop(int _spO2Drop)
//{
//	CArray <CSpO2DropEvnt *, CSpO2DropEvnt *> *aP;
//	if (spO2DropLimit3 == _spO2Drop) aP = &spO2DropEventArray3;
//	else if (spO2DropLimit4 == _spO2Drop) aP = &spO2DropEventArray4;
//	else return;
//
//	vector <EVENT_SORT_VECTOR_ELEMENT> evV;
//	int num = aP->GetCount();
//	for (int i = 0; i < num; ++i) {
//		CSpO2DropEvnt *eP = aP->GetAt(i);
//		EVENT_SORT_VECTOR_ELEMENT eel;
//		eel.begin = eP->getFrom();
//		eel.end = eP->getTo();
//		eel.evP = NULL;
//		eel.level = eP->getLevelType();
//		eel.hypopneaDropLimit = noHypopneaDropLimit;
//		evV.push_back(eel);
//		delete(eP);
//	}
//
//	OLDsortAndOrganiseEvntSortVector(&evV, false);
//
//	//---Need these
//	vector <FLOAT> *flow = cathData->getFlowVector();
//	vector <FLOAT> *flowTime = cathData->getFlowVectorTime();
//	vector <FLOAT> *vpGrad = cathData->getPgradientVector();
//	vector <FLOAT> *tpGrad = cathData->getPgradientVectorTime();
//
//	//---Fills up the array again
//	aP->RemoveAll();
//	size_t size = evV.size();
//	vector <EVENT_SORT_VECTOR_ELEMENT>::iterator evVIt = evV.begin();
//	for (; evVIt < evV.end(); ++evVIt) {
//		EVENT_SORT_VECTOR_ELEMENT eel = *evVIt;
//
//		short bp = bpData->getPos(eel.begin, eel.end);
//		float dropSize = getFloatSpO2DropLimit();
//		CSpO2DropEvnt *eP = new CSpO2DropEvnt(_spO2Drop,dropSize,.0f,.0f,eel.begin,eel.end,
//			startRecordingClockTime, bp);
//		//---Find from and to SpO2
//		if (oxData) {
//			eP->setFrom(oxData->getSpO2At(eel.begin));
//			eP->setTo(oxData->getSpO2At(eel.end));
//		}
//		aP->Add(eP);
//	}
//}

/*
Description:
Sort the events according to start time.
After that, removes all overlaps.
This is OLD
*/
//void CEvents::OLDsortAndOrganiseAwake(void)
//{
//	vector <EVENT_SORT_VECTOR_ELEMENT> evV;
//	int num = awakeEventArray.GetCount();
//	for (int i = 0; i < num; ++i) {
//		CAwakeEvnt *eP = awakeEventArray.GetAt(i);
//		EVENT_SORT_VECTOR_ELEMENT eel;
//		eel.begin = eP->getFrom();
//		eel.end = eP->getTo();
//		eel.evP = NULL;
//		eel.level = eP->getLevelType();
//		eel.hypopneaDropLimit = noHypopneaDropLimit;
//		evV.push_back(eel);
//		delete(eP);
//	}
//
//	OLDsortAndOrganiseEvntSortVector(&evV, false);
//
//	//---Need these
//	vector <FLOAT> *flow = cathData->getFlowVector();
//	vector <FLOAT> *flowTime = cathData->getFlowVectorTime();
//	vector <FLOAT> *vpGrad = cathData->getPgradientVector();
//	vector <FLOAT> *tpGrad = cathData->getPgradientVectorTime();
//
//	//---Fills up the array again
//	awakeEventArray.RemoveAll();
//	size_t size = evV.size();
//	vector <EVENT_SORT_VECTOR_ELEMENT>::iterator evVIt = evV.begin();
//	for (; evVIt < evV.end(); ++evVIt) {
//		EVENT_SORT_VECTOR_ELEMENT eel = *evVIt;
//
//		short bp = bpData->getPos(eel.begin, eel.end);
//		CAwakeEvnt *eP = new CAwakeEvnt(eel.begin, eel.end, startRecordingClockTime, bp);
//		awakeEventArray.Add(eP);
//	}
//}


/*
Description:
Sort the events according to start time.
After that, removes all overlaps.
This is OLD
*/
//void CEvents::OLDsortAndOrganiseSnoring(void)
//{
//	vector <EVENT_SORT_VECTOR_ELEMENT> evV;
//	int num = snoringEventArray.GetCount();
//	for (int i = 0; i < num; ++i) {
//		CSnoringEvnt *eP = snoringEventArray.GetAt(i);
//		EVENT_SORT_VECTOR_ELEMENT eel;
//		eel.begin = eP->getFrom();
//		eel.end = eP->getTo();
//		eel.evP = NULL;
//		eel.level = eP->getLevelType();
//		eel.hypopneaDropLimit = noHypopneaDropLimit;
//		evV.push_back(eel);
//		delete(eP);
//	}
//
//	OLDsortAndOrganiseEvntSortVector(&evV, true);
//
//	//---Need these
//	vector <FLOAT> *flow = cathData->getFlowVector();
//	vector <FLOAT> *flowTime = cathData->getFlowVectorTime();
//	vector <FLOAT> *vpGrad = cathData->getPgradientVector();
//	vector <FLOAT> *tpGrad = cathData->getPgradientVectorTime();
//	vector <FLOAT> *vpoes = cathData->getPOESEnvVector();
//	vector <FLOAT> *poesTime = cathData->getPOESEnvVectorTime();
//
//	//---Fills up the array again
//	snoringEventArray.RemoveAll();
//	size_t size = evV.size();
//	vector <EVENT_SORT_VECTOR_ELEMENT>::iterator evVIt = evV.begin();
//	for (; evVIt < evV.end(); ++evVIt) {
//		EVENT_SORT_VECTOR_ELEMENT eel = *evVIt;
//
//		short bp = bpData->getPos(eel.begin, eel.end);
//		CSnoringEvnt *eP = new CSnoringEvnt(eel.begin, eel.end, startRecordingClockTime, bp);
//
//		gradientDataToEvent((CEvnt *)eP, vpGrad, tpGrad, eel.begin, eel.end);
//		flowDataToEvent((CEvnt *)eP, flow, flowTime, eel.begin, eel.end);
//		
//		bodyDataToEvent(eP, bpData->getBpVector(),
//			bpData->getTimeaxisVector(),
//			eel.begin, eel.end);
//		contactMicDataToEvent(eP, bpData->getBpVector(),
//			bpData->getTimeaxisVector(),
//			eel.begin, eel.end);
//		airMicDataToEvent(eP, amData->getAm1Vector(),
//			amData->getAm2Vector(),
//			amData->getAm3Vector(),
//			amData->getAm4Vector(),
//			amData->getAmSumVector(),
//			amData->getTimeaxisVector(),
//			eel.begin, eel.end);
//		poesEnvDataToEvent((CEvnt *)eP, vpoes, poesTime, eel.begin, eel.end);
//		eP->sumUpTimeInPositions();
//		eP->sumUpTimeAtLevels();
//		eP->setLevelType(eel.level);
//		snoringEventArray.Add(eP);
//	}
//}

/*
Description:
Sort the events according to start time.
After that, removes all overlaps.
This is OLD
*/
//void CEvents::OLDsortAndOrganiseExcluded(void)
//{
//	vector <EVENT_SORT_VECTOR_ELEMENT> evV;
//	int num = excludedEventArray.GetCount();
//	for (int i = 0; i < num; ++i) {
//		CExcludedEvnt *eP = excludedEventArray.GetAt(i);
//		EVENT_SORT_VECTOR_ELEMENT eel;
//		eel.begin = eP->getFrom();
//		eel.end = eP->getTo();
//		eel.evP = NULL;
//		eel.level = eP->getLevelType();
//		eel.hypopneaDropLimit = noHypopneaDropLimit;
//		evV.push_back(eel);
//		delete(eP);
//	}
//
//	OLDsortAndOrganiseEvntSortVector(&evV, false);
//
//	//---Need these
//	vector <FLOAT> *flow = cathData->getFlowVector();
//	vector <FLOAT> *flowTime = cathData->getFlowVectorTime();
//	vector <FLOAT> *vpGrad = cathData->getPgradientVector();
//	vector <FLOAT> *tpGrad = cathData->getPgradientVectorTime();
//
//	//---Fills up the array again
//	excludedEventArray.RemoveAll();
//	size_t size = evV.size();
//	vector <EVENT_SORT_VECTOR_ELEMENT>::iterator evVIt = evV.begin();
//	for (; evVIt < evV.end(); ++evVIt) {
//		EVENT_SORT_VECTOR_ELEMENT eel = *evVIt;
//
//		short bp = bpData->getPos(eel.begin, eel.end);
//		CExcludedEvnt *eP = new CExcludedEvnt(eel.begin, eel.end, startRecordingClockTime, bp);
//		excludedEventArray.Add(eP);
//	}
//}

/*
Description: Sorts events according to start time
*/
void CEvents::sortAndOrganiseMixed(void)
{
	vector <EVENT_SORT_VECTOR_ELEMENT_NEW> evV;
	int num = mixedEventArray.GetCount();
	for (int i = 0; i < num; ++i) {
		CMixedEvnt *eP = mixedEventArray.GetAt(i);
		EVENT_SORT_VECTOR_ELEMENT_NEW eel;
		eel.begin = eP->getFrom();
		eel.evP = (CEvnt *)eP;
		evV.push_back(eel);
	}
	sort(evV.begin(), evV.end(), compareEventsInTimeNew);
	mixedEventArray.RemoveAll();
	vector <EVENT_SORT_VECTOR_ELEMENT_NEW>::iterator it0;
	it0 = evV.begin();
	for (; it0 < evV.end(); ++it0) {
		mixedEventArray.Add((CMixedEvnt *)it0->evP);
	}
}

/*
Description: Sorts events according to start time
*/
void CEvents::sortAndOrganiseCentral(void)
{
	vector <EVENT_SORT_VECTOR_ELEMENT_NEW> evV;
	int num = centralEventArray.GetCount();
	for (int i = 0; i < num; ++i) {
		CCentralEvnt *eP = centralEventArray.GetAt(i);
		EVENT_SORT_VECTOR_ELEMENT_NEW eel;
		eel.begin = eP->getFrom();
		eel.evP = (CEvnt *)eP;
		evV.push_back(eel);
	}
	sort(evV.begin(), evV.end(), compareEventsInTimeNew);
	centralEventArray.RemoveAll();
	vector <EVENT_SORT_VECTOR_ELEMENT_NEW>::iterator it0;
	it0 = evV.begin();
	for (; it0 < evV.end(); ++it0) {
		centralEventArray.Add((CCentralEvnt *)it0->evP);
	}
}

/*
Description: Sorts events according to start time
*/
void CEvents::sortAndOrganiseObstr(void)
{
	vector <EVENT_SORT_VECTOR_ELEMENT_NEW> evV;
	int num = obstrEventArray.GetCount();
	for (int i = 0; i < num; ++i) {
		CObstrEvnt *eP = obstrEventArray.GetAt(i);
		EVENT_SORT_VECTOR_ELEMENT_NEW eel;
		eel.begin = eP->getFrom();
		eel.evP = (CEvnt *)eP;
		evV.push_back(eel);
	}
	sort(evV.begin(), evV.end(), compareEventsInTimeNew);
	obstrEventArray.RemoveAll();
	vector <EVENT_SORT_VECTOR_ELEMENT_NEW>::iterator it0;
	it0 = evV.begin();
	for (; it0 < evV.end(); ++it0) {
		obstrEventArray.Add((CObstrEvnt *)it0->evP);
	}
}

/*
Description: Sorts events according to start time
*/
void CEvents::sortAndOrganiseHypo(int _spO2Drop)
{
	if (spO2DropLimit3 == _spO2Drop) {
		vector <EVENT_SORT_VECTOR_ELEMENT_NEW> evV;
		int num = hypoEventArrayDesat3.GetCount();
		for (int i = 0; i < num; ++i) {
			CHypoEvnt *eP = hypoEventArrayDesat3.GetAt(i);
			EVENT_SORT_VECTOR_ELEMENT_NEW eel;
			eel.begin = eP->getFrom();
			eel.evP = (CEvnt *)eP;
			evV.push_back(eel);
		}
		sort(evV.begin(), evV.end(), compareEventsInTimeNew);
		hypoEventArrayDesat3.RemoveAll();
		vector <EVENT_SORT_VECTOR_ELEMENT_NEW>::iterator it0;
		it0 = evV.begin();
		for (; it0 < evV.end(); ++it0) {
			hypoEventArrayDesat3.Add((CHypoEvnt *)it0->evP);
		}
	}
	if (spO2DropLimit4 == _spO2Drop) {
		vector <EVENT_SORT_VECTOR_ELEMENT_NEW> evV;
		int num = hypoEventArrayDesat4.GetCount();
		for (int i = 0; i < num; ++i) {
			CHypoEvnt *eP = hypoEventArrayDesat4.GetAt(i);
			EVENT_SORT_VECTOR_ELEMENT_NEW eel;
			eel.begin = eP->getFrom();
			eel.evP = (CEvnt *)eP;
			evV.push_back(eel);
		}
		sort(evV.begin(), evV.end(), compareEventsInTimeNew);
		hypoEventArrayDesat4.RemoveAll();
		vector <EVENT_SORT_VECTOR_ELEMENT_NEW>::iterator it0;
		it0 = evV.begin();
		for (; it0 < evV.end(); ++it0) {
			hypoEventArrayDesat4.Add((CHypoEvnt *)it0->evP);
		}
	}
}

/*
Description: Sorts events according to start time
*/
void CEvents::sortAndOrganiseArousal(int _spO2Drop)
{
	if (spO2DropLimit3 == _spO2Drop) {
		vector <EVENT_SORT_VECTOR_ELEMENT_NEW> evV;
		int num = arousalEventArrayDesat3.GetCount();
		for (int i = 0; i < num; ++i) {
			CArousalEvnt *eP = arousalEventArrayDesat3.GetAt(i);
			EVENT_SORT_VECTOR_ELEMENT_NEW eel;
			eel.begin = eP->getFrom();
			eel.evP = (CEvnt *)eP;
			evV.push_back(eel);
		}
		sort(evV.begin(), evV.end(), compareEventsInTimeNew);
		arousalEventArrayDesat3.RemoveAll();
		vector <EVENT_SORT_VECTOR_ELEMENT_NEW>::iterator it0;
		it0 = evV.begin();
		for (; it0 < evV.end(); ++it0) {
			arousalEventArrayDesat3.Add((CArousalEvnt *)it0->evP);
		}
	}
	if (spO2DropLimit4 == _spO2Drop) {
		vector <EVENT_SORT_VECTOR_ELEMENT_NEW> evV;
		int num = arousalEventArrayDesat4.GetCount();
		for (int i = 0; i < num; ++i) {
			CArousalEvnt *eP = arousalEventArrayDesat4.GetAt(i);
			EVENT_SORT_VECTOR_ELEMENT_NEW eel;
			eel.begin = eP->getFrom();
			eel.evP = (CEvnt *)eP;
			evV.push_back(eel);
		}
		sort(evV.begin(), evV.end(), compareEventsInTimeNew);
		arousalEventArrayDesat4.RemoveAll();
		vector <EVENT_SORT_VECTOR_ELEMENT_NEW>::iterator it0;
		it0 = evV.begin();
		for (; it0 < evV.end(); ++it0) {
			arousalEventArrayDesat4.Add((CArousalEvnt *)it0->evP);
		}
	}
}

/*
Description: Sorts events according to start time
*/
void CEvents::sortAndOrganiseSpO2Drop(int _spO2Drop)
{
	if (spO2DropLimit3 == _spO2Drop) {
		vector <EVENT_SORT_VECTOR_ELEMENT_NEW> evV;
		int num = spO2DropEventArray3.GetCount();
		for (int i = 0; i < num; ++i) {
			CSpO2DropEvnt *eP = spO2DropEventArray3.GetAt(i);
			EVENT_SORT_VECTOR_ELEMENT_NEW eel;
			eel.begin = eP->getFrom();
			eel.evP = (CEvnt *)eP;
			evV.push_back(eel);
		}
		sort(evV.begin(), evV.end(), compareEventsInTimeNew);
		spO2DropEventArray3.RemoveAll();
		vector <EVENT_SORT_VECTOR_ELEMENT_NEW>::iterator it0;
		it0 = evV.begin();
		for (; it0 < evV.end(); ++it0) {
			spO2DropEventArray3.Add((CSpO2DropEvnt *)it0->evP);
		}
	}
	if (spO2DropLimit4 == _spO2Drop) {
		vector <EVENT_SORT_VECTOR_ELEMENT_NEW> evV;
		int num = spO2DropEventArray4.GetCount();
		for (int i = 0; i < num; ++i) {
			CSpO2DropEvnt *eP = spO2DropEventArray4.GetAt(i);
			EVENT_SORT_VECTOR_ELEMENT_NEW eel;
			eel.begin = eP->getFrom();
			eel.evP = (CEvnt *)eP;
			evV.push_back(eel);
		}
		sort(evV.begin(), evV.end(), compareEventsInTimeNew);
		spO2DropEventArray4.RemoveAll();
		vector <EVENT_SORT_VECTOR_ELEMENT_NEW>::iterator it0;
		it0 = evV.begin();
		for (; it0 < evV.end(); ++it0) {
			spO2DropEventArray4.Add((CSpO2DropEvnt *)it0->evP);
		}
	}
}

/*
Description: Sorts events according to start time
*/
void CEvents::sortAndOrganiseAwake(void)
{
	vector <EVENT_SORT_VECTOR_ELEMENT_NEW> evV;
	int num = awakeEventArray.GetCount();
	for (int i = 0; i < num; ++i) {
		CAwakeEvnt *eP = awakeEventArray.GetAt(i);
		EVENT_SORT_VECTOR_ELEMENT_NEW eel;
		eel.begin = eP->getFrom();
		eel.evP = (CEvnt *)eP;
		evV.push_back(eel);
	}
	sort(evV.begin(), evV.end(), compareEventsInTimeNew);
	awakeEventArray.RemoveAll();
	vector <EVENT_SORT_VECTOR_ELEMENT_NEW>::iterator it0;
	it0 = evV.begin();
	for (; it0 < evV.end(); ++it0) {
		awakeEventArray.Add((CAwakeEvnt *)it0->evP);
	}
}

void CEvents::sortAndOrganiseImported(void)
{
	vector <EVENT_SORT_VECTOR_ELEMENT_NEW> evV;
	int num = importedEventArray.GetCount();
	for (int i = 0; i < num; ++i) {
		CImportedEvnt *eP = importedEventArray.GetAt(i);
		EVENT_SORT_VECTOR_ELEMENT_NEW eel;
		eel.begin = eP->getFrom();
		eel.evP = (CEvnt *)eP;
		evV.push_back(eel);
	}
	sort(evV.begin(), evV.end(), compareEventsInTimeNew);
	importedEventArray.RemoveAll();
	vector <EVENT_SORT_VECTOR_ELEMENT_NEW>::iterator it0;
	it0 = evV.begin();
	for (; it0 < evV.end(); ++it0) {
		importedEventArray.Add((CImportedEvnt *)it0->evP);
	}
}

/*
Description: Sorts events according to start time
*/
void CEvents::sortAndOrganiseSnoring(void)
{
	vector <EVENT_SORT_VECTOR_ELEMENT_NEW> evV;
	int num = snoringEventArray.GetCount();
	for (int i = 0; i < num; ++i) {
		CSnoringEvnt *eP = snoringEventArray.GetAt(i);
		EVENT_SORT_VECTOR_ELEMENT_NEW eel;
		eel.begin = eP->getFrom();
		eel.evP = (CEvnt *)eP;
		evV.push_back(eel);
	}
	sort(evV.begin(), evV.end(), compareEventsInTimeNew);
	snoringEventArray.RemoveAll();
	vector <EVENT_SORT_VECTOR_ELEMENT_NEW>::iterator it0;
	it0 = evV.begin();
	for (; it0 < evV.end(); ++it0) {
		snoringEventArray.Add((CSnoringEvnt *)it0->evP);
	}
}

/*
Description: Sorts events according to start time
*/
void CEvents::sortAndOrganiseExcluded(void)
{
	vector <EVENT_SORT_VECTOR_ELEMENT_NEW> evV;
	int num = excludedEventArray.GetCount();
	for (int i = 0; i < num; ++i) {
		CExcludedEvnt *eP = excludedEventArray.GetAt(i);
		EVENT_SORT_VECTOR_ELEMENT_NEW eel;
		eel.begin = eP->getFrom();
		eel.evP = (CEvnt *)eP;
		evV.push_back(eel);
	}
	sort(evV.begin(), evV.end(), compareEventsInTimeNew);
	excludedEventArray.RemoveAll();
	vector <EVENT_SORT_VECTOR_ELEMENT_NEW>::iterator it0;
	it0 = evV.begin();
	for (; it0 < evV.end(); ++it0) {
		excludedEventArray.Add((CExcludedEvnt *)it0->evP);
	}
}

/*
Description: Sorts events according to start time
*/
void CEvents::sortAndOrganiseManualMarkers(void)
{
	vector <EVENT_SORT_VECTOR_ELEMENT_NEW> evV;
	int num = manualMarkerEventArray.GetCount();
	for (int i = 0; i < num; ++i) {
		CManualMarkerEvnt *eP = manualMarkerEventArray.GetAt(i);
		EVENT_SORT_VECTOR_ELEMENT_NEW eel;
		eel.begin = eP->getFrom();
		eel.evP = (CEvnt *)eP;
		evV.push_back(eel);
	}
	sort(evV.begin(), evV.end(), compareEventsInTimeNew);
	manualMarkerEventArray.RemoveAll();
	vector <EVENT_SORT_VECTOR_ELEMENT_NEW>::iterator it0;
	it0 = evV.begin();
	for (; it0 < evV.end(); ++it0) {
		manualMarkerEventArray.Add((CManualMarkerEvnt *)it0->evP);
	}
}

/*
Description:
Works on a vector <EVENT_SORT_VECTOR_ELEMENT>
Sorts
Removes overlaps
Deletes events that are no longer required due to overlaps

Time resolution 0.1 sec
*/
//void CEvents::OLDsortAndOrganiseEvntSortVector(vector <EVENT_SORT_VECTOR_ELEMENT> *_evV, bool _useLevel /* = false */)
//{
//	//---Sort chronologically
//	sort(_evV->begin(), _evV->end(), compareEventsInTime);
//
//	vector <bool> bv;				// On-off vector
//	vector <int> lvV;				// level vector
//	vector <int> hypoDropVector;	// Hypo drop vector
//
//	size_t length = (size_t) (maxStopTime * 10.0f);
//	bv.resize(length, false);
//	lvV.resize(length, levelTypeUndef);
//	hypoDropVector.resize(length, noHypopneaDropLimit);
//
//	vector <EVENT_SORT_VECTOR_ELEMENT>::iterator evVIt = _evV->begin(); 
//	vector <bool>::iterator it0;
//	vector <bool>::iterator it1;
//	vector <int>::iterator itLv0;
//	vector <int>::iterator itLv1;
//	vector <int>::iterator itDrop0;
//	vector <int>::iterator itDrop1;
//	for (; evVIt < _evV->end(); ++evVIt) {
//		EVENT_SORT_VECTOR_ELEMENT el = *evVIt;
//		size_t s0 = (size_t)(el.begin * 10.0f);
//		size_t s1 = (size_t)(el.end * 10.0f);
//		if (s0 < length) {
//			it0 = bv.begin();
//			it0 += s0;
//	
//			s1 = s1 < length ? s1 : length - 1;
//			it1 = bv.begin();
//			it1 += s1;
//
//			itDrop0 = hypoDropVector.begin();
//			itDrop0 += s0;
//			itDrop1 = hypoDropVector.begin();
//			itDrop1 += s1;
//	
//			//---Fill bool vector
//			fill(it0, it1, true); // Here is an event!
//
//			//---Fill hypo drop vector
//			fill(itDrop0, itDrop1, el.hypopneaDropLimit);
//
//			//---Fill level vector
//			if (_useLevel) {
//				itLv1 = lvV.begin();
//				itLv1 += s1;
//				itLv0 = lvV.begin();
//				itLv0 += s0;
//				fill(itLv0, itLv1, el.level);
//			}
//		}
//	}
//
//	//---Clear the input vector
//	_evV->clear();
//
//	//---Fill the input vector up again
//	vector <bool>::iterator bvIt = bv.begin();
//	bool inEvent = false;
//	float start, stop; 
//	vector <int>::iterator levelIt = lvV.begin();
//	vector <int>::iterator levelIt0, levelIt1;
//	vector <int>::iterator dropIt = hypoDropVector.begin();
//	vector <int>::iterator dropIt0, dropIt1;
//	for (; bvIt < bv.end(); ++bvIt, ++levelIt, ++dropIt) {
//
//		//---Entering an event here
//		if (!inEvent && *bvIt) { 
//			start = (float) distance(bv.begin(), bvIt) / 10.f;
//			levelIt0 = levelIt;
//			dropIt0 = dropIt;
//			inEvent = true;
//		}
//		//---Exits an event here. Save data to the output vector
//		else if (inEvent && !*bvIt) {
//			stop = (float)distance(bv.begin(), bvIt) / 10.f;
//			levelIt1 = levelIt;
//			dropIt1 = dropIt;
//			EVENT_SORT_VECTOR_ELEMENT el;
//			el.begin = start;
//			el.end = stop;
//			el.evP = NULL;
//			el.hypopneaDropLimit = findDropLimitFromDropLimitVector(dropIt0, dropIt1);
//			if (_useLevel)
//				el.level = findLevelFromLevelVector(levelIt0, levelIt1);
//			_evV->push_back(el);
//			inEvent = false;
//		}
//	}
//}

/*
Description: From a vector with droplimits, if something else than noHypopnea Drop limit is found, return it.
We assume that hypopneas are not overlapping as that has been fixed in a previous operation.
*/
int CEvents::findDropLimitFromDropLimitVector(vector <int>::iterator _from, vector <int>::iterator _to)
{
	int ret = noHypopneaDropLimit;

	vector <int>::iterator it = _from;
	int limit = *it;
	for (; it < _to; ++it) {
		if (hypopneaDropLimit30 == limit) return limit;
		if (hypopneaDropLimit40 == limit) return limit;
		if (hypopneaDropLimit50 == limit) return limit;
	}
	return ret;
}



int CEvents::findLevelFromLevelVector(vector <int>::iterator _from, vector <int>::iterator _to)
{
	vector <int>::iterator it = _from;
	int level = *it;
	for (; it < _to; ++it) {
		if (levelTypeUndef == level) {  // If a single undef instance is found, set to undef and leave
			level = levelTypeUndef;
			break;
		}
		if (levelTypeMulti == level) { // If a single multilevel instance is found, set to multi and leave
			level = levelTypeUndef;
			break;
		}
		if (*it != level) {  // If level changes - set to multilevel and leave
			level = levelTypeUndef;
			break;
		}
	}
	// If all were upper or lower, upper or lower is returned
	return level;
}

void CEvents::changeEventTypeDuringEdit(CEvnt *_evnt, int _toType, int _spO2DropLimit /* = noSpO2DropLimit*/)
{
	switch (_toType) {
	case evntTypeMixed:
	{
		CMixedEvnt *evP = new CMixedEvnt;
		copyBetweenEventTypes(_evnt, (CEvnt *)evP);
		delete _evnt;
		mixedEventArray.Add(evP);
	}
		break;
	case evntTypeCentral:
	{
		CCentralEvnt *evP = new CCentralEvnt;
		copyBetweenEventTypes(_evnt, (CEvnt *)evP);
		delete _evnt;
		centralEventArray.Add(evP);
	}
		break;
	case evntTypeObstr:
	{
		CObstrEvnt *evP = new CObstrEvnt;
		copyBetweenEventTypes(_evnt, (CEvnt *)evP);
		delete _evnt;
		obstrEventArray.Add(evP);
	}
		break;
	case evntTypeImported:
	{
		CImportedEvnt *evP = new CImportedEvnt;
		copyBetweenEventTypes(_evnt, (CEvnt *)evP);
		delete _evnt;
		importedEventArray.Add(evP);
	}
	break;
	case evntTypeHypoCentral:
	case evntTypeHypoObstr:
		if (spO2DropLimit3 == _spO2DropLimit) {
			CHypoEvnt *evP = new CHypoEvnt;
			copyBetweenEventTypes(_evnt, (CEvnt *)evP);
			evP->setHypoDropLimit(detectionCriteria.hypopneaDropLimit);
			evP->setHypoType(evntTypeHypoCentral == _toType ? hypoTypeCentral : hypoTypeObstr);
			delete _evnt;
			hypoEventArrayDesat3.Add((CHypoEvnt *)evP);
		}
		else if (spO2DropLimit4 == _spO2DropLimit) {
			CHypoEvnt *evP = new CHypoEvnt;
			copyBetweenEventTypes(_evnt, (CEvnt *)evP);
			evP->setHypoDropLimit(detectionCriteria.hypopneaDropLimit);
			evP->setHypoType(evntTypeHypoCentral == _toType ? hypoTypeCentral : hypoTypeObstr);
			delete _evnt;
			hypoEventArrayDesat4.Add((CHypoEvnt *)evP);
		}
		break;
	default:
		break;
	}
}

bool CEvents::removeImported(const CEvnt *_evnt)
{
	int num = importedEventArray.GetCount();
	for (int i = 0; i < num; i++) {
		CImportedEvnt *eP = importedEventArray.GetAt(i);
		if (_evnt == eP) {
			importedEventArray.RemoveAt(i);
			return true;
		}
	}
	return false;
}


bool CEvents::removeMixed(const CEvnt *_evnt)
{
	int num = mixedEventArray.GetCount();
	for (int i = 0; i < num; i++) {
		CMixedEvnt *eP = mixedEventArray.GetAt(i);
		if (_evnt == eP) {
			mixedEventArray.RemoveAt(i);
			return true;
		}
	}
	return false;
}

bool CEvents::removeCentral(CEvnt *_evnt)
{
	int num = centralEventArray.GetCount();
	for (int i = 0; i < num; i++) {
		CCentralEvnt *eP = centralEventArray.GetAt(i);
		if (_evnt == eP) {
			centralEventArray.RemoveAt(i);
			return true;
		}
	}
	return false;
}

bool CEvents::removeObstr(CEvnt *_evnt)
{
	int num = obstrEventArray.GetCount();
	for (int i = 0; i < num; i++) {
		CObstrEvnt *eP = obstrEventArray.GetAt(i);
		if (_evnt == eP) {
			obstrEventArray.RemoveAt(i);
			return true;
		}
	}
	return false;
}

bool CEvents::removeHypo(CEvnt *_evnt, int _spO2DropLimit)
{
	if (spO2DropLimit3 == _spO2DropLimit) {
		int num = hypoEventArrayDesat3.GetCount();
		for (int i = 0; i < num; i++) {
			CHypoEvnt *eP = hypoEventArrayDesat3.GetAt(i);
			if (_evnt == eP) {
				hypoEventArrayDesat3.RemoveAt(i);
				return true;
			}
		}
		return false;
	}
	if (spO2DropLimit4 == _spO2DropLimit) {
		int num = hypoEventArrayDesat4.GetCount();
		for (int i = 0; i < num; i++) {
			CHypoEvnt *eP = hypoEventArrayDesat4.GetAt(i);
			if (_evnt == eP) {
				hypoEventArrayDesat4.RemoveAt(i);
				return true;
			}
		}
		return false;
	}
	return false;
}

bool CEvents::removeRERA(CEvnt *_evnt, int _spO2DropLimit)
{
	if (spO2DropLimit3 == _spO2DropLimit) {
		int num = arousalEventArrayDesat3.GetCount();
		for (int i = 0; i < num; i++) {
			CArousalEvnt *eP = arousalEventArrayDesat3.GetAt(i);
			CRERAEvnt *rP = eP->getRERAEvnt();
			if (_evnt == rP) {
				eP->removeRERAEvnt();
				return true;
			}
		}
	}

	if (spO2DropLimit4 == _spO2DropLimit) {
		int num = arousalEventArrayDesat4.GetCount();
		for (int i = 0; i < num; i++) {
			CArousalEvnt *eP = arousalEventArrayDesat4.GetAt(i);
			CRERAEvnt *rP = eP->getRERAEvnt();
			if (_evnt == rP) {
				eP->removeRERAEvnt();
				return true;
			}
		}
	}
	return false;
}

bool CEvents::removeArousal(CEvnt *_evnt, int _spO2DropLimit)
{
	if (spO2DropLimit3 == _spO2DropLimit) {
		int num = arousalEventArrayDesat3.GetCount();
		for (int i = 0; i < num; i++) {
			CArousalEvnt *eP = arousalEventArrayDesat3.GetAt(i);
			if (_evnt == eP) {
				arousalEventArrayDesat3.RemoveAt(i);
				return true;
			}
		}
	}
	if (spO2DropLimit4 == _spO2DropLimit) {
		int num = arousalEventArrayDesat4.GetCount();
		for (int i = 0; i < num; i++) {
			CArousalEvnt *eP = arousalEventArrayDesat4.GetAt(i);
			if (_evnt == eP) {
				arousalEventArrayDesat4.RemoveAt(i);
				return true;
			}
		}
	}
	return false;
}

bool CEvents::removeSpO2Drop(CEvnt *_evnt, int _spO2DropLimit)
{
	if (spO2DropLimit3 == _spO2DropLimit) {
		int num = spO2DropEventArray3.GetCount();
		for (int i = 0; i < num; i++) {
			CSpO2DropEvnt *eP = spO2DropEventArray3.GetAt(i);
			if (_evnt == eP) {
				spO2DropEventArray3.RemoveAt(i);
				return true;
			}
		}
		return false;
	}
	else if (spO2DropLimit4 == _spO2DropLimit) {
		int num = spO2DropEventArray4.GetCount();
		for (int i = 0; i < num; i++) {
			CSpO2DropEvnt *eP = spO2DropEventArray4.GetAt(i);
			if (_evnt == eP) {
				spO2DropEventArray4.RemoveAt(i);
				return true;
			}
		}
		return false;
	}
	return false;
}

bool CEvents::removeAwake(CEvnt *_evnt)
{
	int num = awakeEventArray.GetCount();
	for (int i = 0; i < num; i++) {
		CAwakeEvnt *eP = awakeEventArray.GetAt(i);
		if (_evnt == eP) {
			awakeEventArray.RemoveAt(i);
			return true;
		}
	}
	return false;
}

bool CEvents::removeSnoring(CEvnt *_evnt)
{
	int num = snoringEventArray.GetCount();
	for (int i = 0; i < num; i++) {
		CSnoringEvnt *eP = snoringEventArray.GetAt(i);
		if (_evnt == eP) {
			snoringEventArray.RemoveAt(i);
			return true;
		}
	}
	return false;
}

/*
Description:
	Find the event
	Push the change on the stack
	Do the change
*/
void CEvents::changeEvent(CEvnt *_evnt,int _command)
{
	CEvnt *eP = removeAndReplaceEvent(_evnt);
}

/*
Description: Returns the eventDetection flags
This flag contains info on manual editing of the event
*/
int CEvents::getEventDetectionStatus(void)
{
	return eventDefinition;
}

void CEvents::setEditedDetectionCriteria(DETECTION_CRITERIA _dt, bool _onlySpO2DropOrHypopneaCrit /*= false*/)
{
	editedDetectionCriteria = _dt; 
	if (!_onlySpO2DropOrHypopneaCrit) 
		doAnalyse();
	else {
		detectionCriteria = editedDetectionCriteria;	// Everything has been computed beforedetectionCriteria = editedDetectionCriteria;

		switch (detectionCriteria.spO2DropLimit) {
		case spO2DropLimit3:
			spO2DropEventArrayCurrentP = &spO2DropEventArray3;
			hypoEventArrayCurrentP = &hypoEventArrayDesat3;
			arousalEventArrayCurrentP = &arousalEventArrayDesat3;
			break;
		case spO2DropLimit4:
			spO2DropEventArrayCurrentP = &spO2DropEventArray4;
			hypoEventArrayCurrentP = &hypoEventArrayDesat4;
			arousalEventArrayCurrentP = &arousalEventArrayDesat4;
			break;
		default:
			spO2DropEventArrayCurrentP = &spO2DropEventArray4;
			hypoEventArrayCurrentP = &hypoEventArrayDesat4;
			arousalEventArrayCurrentP = &arousalEventArrayDesat4;
			break;
		}
	}
}

void CEvents::recomputeFlowAndBreathingEfficiency(void)
{
	if (cathData) cathData->computeFlowAndBreathingEfficiencyForCatheter(detectionCriteria.flowDetectBalanceFactor);
}

/*
Description: Knowing the type of event, find the event and return the pointer to it 
plus a constant that tells which egde is closest.
*/
CEvnt *CEvents::findEvent(int _type,FLOAT _atTime,int *_closest)
{
	if (_type & OVER_EVNT_MIXED) {
		int num = mixedEventArray.GetCount();
		if (0 == num) return NULL;
		for (int i = 0 ; i < num ; i++) {
			CEvnt *eP = (CEvnt *) mixedEventArray.GetAt(i);
			if (eP->isThisTheEvent(_atTime,_closest)) return  eP;
		}
		return NULL;
	}
	if (_type & OVER_EVNT_CENTRAL) {
		int num = centralEventArray.GetCount();
		if (0 == num) return NULL;
		for (int i = 0 ; i < num ; i++) {
			CEvnt *eP = (CEvnt *) centralEventArray.GetAt(i);
			if (eP->isThisTheEvent(_atTime,_closest)) return eP;
		}
		return NULL;
	}
	if (_type & OVER_EVNT_OBSTRUCTIVE) {
		int num = obstrEventArray.GetCount();
		if (0 == num) return NULL;
		for (int i = 0 ; i < num ; i++) {
			CEvnt *eP = (CEvnt *) obstrEventArray.GetAt(i);
			if (eP->isThisTheEvent(_atTime,_closest)) return eP;
		}
		return NULL;
	}
	if ((_type & OVER_EVNT_HYPOPNEA) || (_type & OVER_EVNT_OBSTRUCTIVEHYPO) || (_type & OVER_EVNT_CENTRALHYPO)) {
		int num = hypoEventArrayCurrentP->GetCount();
		if (0 == num) return NULL;
		for (int i = 0; i < num; i++) {
			CEvnt *eP = (CEvnt *)hypoEventArrayCurrentP->GetAt(i);
			if (eP->isThisTheEvent(_atTime, _closest)) return eP;
		}
		return NULL;
	}
	if (_type & OVER_EVNT_SPO2_DROP) {
		int num = spO2DropEventArrayCurrentP->GetCount();
		if (0 == num) return NULL;
		for (int i = 0 ; i < num ; i++) {
			CSpO2DropEvnt *eP = (CSpO2DropEvnt *)spO2DropEventArrayCurrentP->GetAt(i);
			if (eP->isThisTheEvent(_atTime,_closest)) 
				return eP;
		}
		return NULL;
	}
	if (_type & OVER_EVNT_RERA) {
		int num = arousalEventArrayCurrentP->GetCount();
		if (0 == num) return NULL;
		for (int i = 0 ; i < num ; i++) {
			CArousalEvnt *eP = arousalEventArrayCurrentP->GetAt(i);
			CRERAEvnt *rP = eP->getRERAEvnt();
			if (rP) {
				if (rP->isThisTheEvent(_atTime, _closest)) return rP;
			}
		}
		return NULL;
	}
	if (_type & OVER_EVNT_AROUSAL) {
		int num = arousalEventArrayCurrentP->GetCount();
		if (0 == num) return NULL;
		for (int i = 0 ; i < num ; i++) {
			CEvnt *eP = (CEvnt *)arousalEventArrayCurrentP->GetAt(i);
			if (eP->isThisTheEvent(_atTime,_closest)) return eP;
		}
		return NULL;
	}
	
	if (_type & OVER_EVNT_SNORING) {
		int num = snoringEventArray.GetCount();
		if (0 == num) return NULL;
		for (int i = 0 ; i < num ; i++) {
			CEvnt *eP = (CEvnt *) snoringEventArray.GetAt(i);
			if (eP->isThisTheEvent(_atTime,_closest)) return eP;
		}
		return NULL;
	}
	if (_type & OVER_EVNT_AWAKE) {
		int num = awakeEventArray.GetCount();
		if (0 == num) return NULL;
		for (int i = 0 ; i < num ; i++) {
			CEvnt *eP = (CEvnt *) awakeEventArray.GetAt(i);
			if (eP->isThisTheEvent(_atTime,_closest)) return eP;
		}
		return NULL;
	}
	if (_type & OVER_EVNT_MAN_MARKER) {
		int num = manualMarkerEventArray.GetCount();
		if (0 == num) return NULL;
		for (int i = 0 ; i < num ; i++) {
			CEvnt *eP = (CEvnt *) manualMarkerEventArray.GetAt(i);
			if (eP->isThisTheEvent(_atTime,_closest)) return eP;
		}
		return NULL;
	}
	if (_type & OVER_EVNT_EXCLUDED) {
		int num = excludedEventArray.GetCount();
		if (0 == num) return NULL;
		for (int i = 0 ; i < num ; i++) {
			CEvnt *eP = (CEvnt *) excludedEventArray.GetAt(i);
			if (eP->isThisTheEvent(_atTime,_closest)) return eP;
		}
		return NULL;
	}
	return NULL;
}

void CEvents::resetStack(std::stack <CEvnt *> *_stack)
{
	while (_stack->size()) {
		CEvnt *eP = _stack->top();
		_stack->pop();
		if (eP) {
			int type = eP->getEventType();
			if (type & evntTypeMixed) 
				delete ((CMixedEvnt *) eP);
			else if (type & evntTypeCentral) 
				delete ((CCentralEvnt *) eP);
			else if (type & evntTypeObstr) 
				delete ((CObstrEvnt *) eP);
			else if (type & evntTypeSpO2Drop) 
				delete ((CSpO2DropEvnt *) eP);
			else if (type & evntTypeHypoObstr) 
				delete ((CHypoEvnt *) eP);
			else if (type & evntTypeHypoCentral)
				delete ((CHypoEvnt *)eP);
			else if (type & evntTypeRERA) 
				delete ((CRERAEvnt *) eP);
			else if (type & evntTypeArousal) 
				delete ((CArousalEvnt *) eP);
			else if (type & evntTypeManMarker) 
				delete ((CManualMarkerEvnt *) eP);
			else if (type & evntTypeSnoring) 
				delete ((CSnoringEvnt *) eP);
			else if (type & evntTypeAwake) 
				delete ((CAwakeEvnt *) eP);
			else if (type & evntTypeExcluded) 
				delete ((CExcludedEvnt *) eP);
			else 
				delete eP;
		}
	} 
}

void CEvents::unExcludeAllEvents(int _spO2DropLimit)
{
	int num = 0;

	//1
	if (spO2DropLimit3 == _spO2DropLimit) {
		num = arousalEventArrayDesat3.GetCount();
		for (int i = num - 1; i >= 0; i--) {
			CArousalEvnt *eP = arousalEventArrayDesat3.GetAt(i);
			eP->setInExcludedWindow(false);
		}
	}
	if (spO2DropLimit4 == _spO2DropLimit) {
		num = arousalEventArrayDesat4.GetCount();
		for (int i = num - 1; i >= 0; i--) {
			CArousalEvnt *eP = arousalEventArrayDesat4.GetAt(i);
			eP->setInExcludedWindow(false);
		}
	}

	//2
	num = awakeEventArray.GetCount();
	for (int i = num - 1; i >= 0; i--) {
		CAwakeEvnt *eP = awakeEventArray.GetAt(i);
		eP->setInExcludedWindow(false);
	}

	//3
	num = centralEventArray.GetCount();
	for (int i = num - 1; i >= 0; i--) {
		CCentralEvnt *eP = centralEventArray.GetAt(i);
		eP->setInExcludedWindow(false);
	}

	//4 Skip excluded event array!

	//5
	if (spO2DropLimit3 == _spO2DropLimit) {
		num = hypoEventArrayDesat3.GetCount();
		for (int i = num - 1; i >= 0; i--) {
			CHypoEvnt *eP = hypoEventArrayDesat3.GetAt(i);
			eP->setInExcludedWindow(false);
		}

	}
	else if (spO2DropLimit4 == _spO2DropLimit)
	{
		num = hypoEventArrayDesat4.GetCount();
		for (int i = num - 1; i >= 0; i--) {
			CHypoEvnt *eP = hypoEventArrayDesat4.GetAt(i);
			eP->setInExcludedWindow(false);
		}

	}

	//6
	num = manualMarkerEventArray.GetCount();
	for (int i = num - 1; i >= 0; i--) {
		CManualMarkerEvnt *eP = manualMarkerEventArray.GetAt(i);
		eP->setInExcludedWindow(false);
	}

	//7
	num = mixedEventArray.GetCount();
	for (int i = num - 1; i >= 0; i--) {
		CMixedEvnt *eP = mixedEventArray.GetAt(i);
		eP->setInExcludedWindow(false);
	}

	//8
	num = obstrEventArray.GetCount();
	for (int i = num - 1; i >= 0; i--) {
		CObstrEvnt *eP = obstrEventArray.GetAt(i);
		eP->setInExcludedWindow(false);
	}

	//9
	if (spO2DropLimit3 == _spO2DropLimit) {
		num = arousalEventArrayDesat3.GetCount();
		for (int i = num - 1; i >= 0; i--) {
			CArousalEvnt *eP = arousalEventArrayDesat3.GetAt(i);
			CRERAEvnt *rP = eP->getRERAEvnt();
			if (rP) rP->setInExcludedWindow(false);
		}
	}
	if (spO2DropLimit4 == _spO2DropLimit) {
		num = arousalEventArrayDesat4.GetCount();
		for (int i = num - 1; i >= 0; i--) {
			CArousalEvnt *eP = arousalEventArrayDesat4.GetAt(i);
			CRERAEvnt *rP = eP->getRERAEvnt();
			if (rP) rP->setInExcludedWindow(false);
		}
	}

	//10
	num = snoringEventArray.GetCount();
	for (int i = num - 1; i >= 0; i--) {
		CSnoringEvnt *eP = snoringEventArray.GetAt(i);
		eP->setInExcludedWindow(false);
	}

	//11
	num = spO2DropEventArrayCurrentP->GetCount();
	for (int i = num - 1; i >= 0; i--) {
		CSpO2DropEvnt *eP = spO2DropEventArrayCurrentP->GetAt(i);
		eP->setInExcludedWindow(false);
	}
}

void CEvents::evaluateAllEventsWrtExclusionWindow(CExcludedEvnt *_eP, int _spO2DropLimit)
{
	int num = 0;

	//1
	if (spO2DropLimit3 == _spO2DropLimit) {
		num = arousalEventArrayDesat3.GetCount();
		for (int i = num - 1; i >= 0; i--) {
			CArousalEvnt *eP = arousalEventArrayDesat3.GetAt(i);
			eP->considerIfInExcludeWindow(_eP->getFrom(), _eP->getTo());
		}
	}
	if (spO2DropLimit4 == _spO2DropLimit) {
		num = arousalEventArrayDesat4.GetCount();
		for (int i = num - 1; i >= 0; i--) {
			CArousalEvnt *eP = arousalEventArrayDesat4.GetAt(i);
			eP->considerIfInExcludeWindow(_eP->getFrom(), _eP->getTo());
		}
	}

	//2
	num = awakeEventArray.GetCount();
	for (int i = num - 1; i >= 0; i--) {
		CAwakeEvnt *eP = awakeEventArray.GetAt(i);
		eP->considerIfInExcludeWindow(_eP->getFrom(), _eP->getTo());
	}

	//3
	num = centralEventArray.GetCount();
	for (int i = num - 1; i >= 0; i--) {
		CCentralEvnt *eP = centralEventArray.GetAt(i);
		eP->considerIfInExcludeWindow(_eP->getFrom(), _eP->getTo());
	}

	//4 Skip excluded event array!

	//5
	if (spO2DropLimit3 == _spO2DropLimit) {
		num = hypoEventArrayDesat3.GetCount();
		for (int i = num - 1; i >= 0; i--) {
			CHypoEvnt *eP = hypoEventArrayDesat3.GetAt(i);
			eP->considerIfInExcludeWindow(_eP->getFrom(), _eP->getTo());
		}
	}
	else if (spO2DropLimit4 == _spO2DropLimit) {
		num = hypoEventArrayDesat4.GetCount();
		for (int i = num - 1; i >= 0; i--) {
			CHypoEvnt *eP = hypoEventArrayDesat4.GetAt(i);
			eP->considerIfInExcludeWindow(_eP->getFrom(), _eP->getTo());
		}
	}

	//6
	num = manualMarkerEventArray.GetCount();
	for (int i = num - 1; i >= 0; i--) {
		CManualMarkerEvnt *eP = manualMarkerEventArray.GetAt(i);
		eP->considerIfInExcludeWindow(_eP->getFrom(), _eP->getTo());
	}

	//7
	num = mixedEventArray.GetCount();
	for (int i = num - 1; i >= 0; i--) {
		CMixedEvnt *eP = mixedEventArray.GetAt(i);
		eP->considerIfInExcludeWindow(_eP->getFrom(), _eP->getTo());
	}

	//8
	num = obstrEventArray.GetCount();
	for (int i = num - 1; i >= 0; i--) {
		CObstrEvnt *eP = obstrEventArray.GetAt(i);
		eP->considerIfInExcludeWindow(_eP->getFrom(), _eP->getTo());
	}

	//9
	if (spO2DropLimit3 == _spO2DropLimit) {
		num = arousalEventArrayDesat3.GetCount();
		for (int i = num - 1; i >= 0; i--) {
			CArousalEvnt *eP = arousalEventArrayDesat3.GetAt(i);
			CRERAEvnt *rP = eP->getRERAEvnt();
			if (rP) rP->considerIfInExcludeWindow(_eP->getFrom(), _eP->getTo());
		}
	}
	if (spO2DropLimit4 == _spO2DropLimit) {
		num = arousalEventArrayDesat4.GetCount();
		for (int i = num - 1; i >= 0; i--) {
			CArousalEvnt *eP = arousalEventArrayDesat4.GetAt(i);
			CRERAEvnt *rP = eP->getRERAEvnt();
			if (rP) rP->considerIfInExcludeWindow(_eP->getFrom(), _eP->getTo());
		}
	}

	//10
	num = snoringEventArray.GetCount();
	for (int i = num - 1; i >= 0; i--) {
		CSnoringEvnt *eP = snoringEventArray.GetAt(i);
		eP->considerIfInExcludeWindow(_eP->getFrom(), _eP->getTo());
	}

	//11
	num = spO2DropEventArrayCurrentP->GetCount();
	for (int i = num - 1; i >= 0; i--) {
		CSpO2DropEvnt *eP = spO2DropEventArrayCurrentP->GetAt(i);
		eP->considerIfInExcludeWindow(_eP->getFrom(), _eP->getTo());
	}
}

/*
Description: Clears all events. if _clearStack is true, also the do and undo stacks are cleared and the CMemFiles are deleted.
*/
void CEvents::reset(bool _clearStateVector /* = true */)
{
	for (int i = 0; i < numSpO2DropLimits; i++) {
		for (int j = 0; j < numHypopneaDropLimits; j++) {
			rdiResult[j][i] = .0f;
			ohiResult[j][i] = .0f;
			chiResult[j][i] = .0f;
			oahiResult[j][i] = .0f;
			osa_ordiResult[j][i] = .0f;
			cahiResult[j][i] = .0f;
			numObstrHypopnea[j][i] = 0;
			numCentralHypopnea[j][i] = 0;
			arousalIndexResult[j][i] = .0f;
			reraiResult[j][i] = .0f;
		}
	}
	maiResult = caiResult = oaiResult = .0f;
	
	//---Clear the stateVector
	if (_clearStateVector) {
		clearStateVector();
	}


	int num;

	//2
	num = awakeEventArray.GetCount();	
	for (int i = num - 1 ; i >= 0 ; i--) {
		CAwakeEvnt *eP = awakeEventArray.GetAt(i);
		delete eP;
	}
	awakeEventArray.RemoveAll();

	//3
	num = centralEventArray.GetCount();
	for (int i = num - 1 ; i >= 0 ; i--) {
		CCentralEvnt *eP = centralEventArray.GetAt(i);
		delete eP;
	}
	centralEventArray.RemoveAll();

	//4
	num = excludedEventArray.GetCount();
	for (int i = num - 1 ; i >= 0 ; i--) {
		CExcludedEvnt *eP = excludedEventArray.GetAt(i);
		delete eP;
	}
	excludedEventArray.RemoveAll();
	
	//5
	num = hypoEventArrayDesat3.GetCount();	
	for (int i = num - 1 ; i >= 0 ; i--) {
		CHypoEvnt *eP = hypoEventArrayDesat3.GetAt(i);
		delete eP;
	}
	hypoEventArrayDesat3.RemoveAll();

	num = hypoEventArrayDesat4.GetCount();
	for (int i = num - 1; i >= 0; i--) {
		CHypoEvnt *eP = hypoEventArrayDesat4.GetAt(i);
		delete eP;
	}
	hypoEventArrayDesat4.RemoveAll();
	
	//6
	num = manualMarkerEventArray.GetCount();	
	for (int i = num - 1 ; i >= 0 ; i--) {
		CManualMarkerEvnt *eP = manualMarkerEventArray.GetAt(i);
		delete eP;
	}
	manualMarkerEventArray.RemoveAll();
	
	//7
	num = mixedEventArray.GetCount();
	for (int i = num - 1 ; i >= 0 ; i--) {
		CMixedEvnt *eP = mixedEventArray.GetAt(i);
		delete eP;
	}
	mixedEventArray.RemoveAll();

	//8
	num = obstrEventArray.GetCount();
	for (int i = num - 1 ; i >= 0 ; i--) {
		CObstrEvnt *eP = obstrEventArray.GetAt(i);
		delete eP;
	}
	obstrEventArray.RemoveAll();

	//9
	num = snoringEventArray.GetCount();	
	for (int i = num - 1 ; i >= 0 ; i--) {
		CSnoringEvnt *eP = snoringEventArray.GetAt(i);
		delete eP;
	}
	snoringEventArray.RemoveAll();
	
	//10
	num = spO2DropEventArray3.GetCount();	
	for (int i = num - 1 ; i >= 0 ; i--) {
		CSpO2DropEvnt *eP = spO2DropEventArray3.GetAt(i);
		delete eP;
	}
	spO2DropEventArray3.RemoveAll();	

	//11
	num = spO2DropEventArray4.GetCount();
	for (int i = num - 1; i >= 0; i--) {
		CSpO2DropEvnt *eP = spO2DropEventArray4.GetAt(i);
		delete eP;
	}
	spO2DropEventArray4.RemoveAll();
	
	//12
	num = arousalEventArrayDesat3.GetCount();
	for (int i = num - 1 ; i >= 0 ; i--) {
		CArousalEvnt *eP = arousalEventArrayDesat3.GetAt(i);
		delete eP;
	}
	arousalEventArrayDesat3.RemoveAll();
	
	//13
	num = arousalEventArrayDesat4.GetCount();
	for (int i = num - 1; i >= 0; i--) {
		CArousalEvnt *eP = arousalEventArrayDesat4.GetAt(i);
		delete eP;
	}
	arousalEventArrayDesat4.RemoveAll();

	//14
	num = swallowEventArray.GetCount();
	for (int i = num - 1; i >= 0; i--) {
		CSwallowEvnt *eP = swallowEventArray.GetAt(i);
		delete eP;
	}
	swallowEventArray.RemoveAll();

	//15
	num = importedEventArray.GetCount();
	for (int i = num - 1; i >= 0; i--) {
		CImportedEvnt *eP = importedEventArray.GetAt(i);
		delete eP;
	}
	importedEventArray.RemoveAll();

	for (int i = 0; i < numSleepAwakeStates; i++) {
		resetSpO2Results(&spO2Res[spO2DropLimit3][i]);
		resetSpO2Results(&spO2Res[spO2DropLimit4][i]);

		pulseRateResults[i].average = .0f;
		pulseRateResults[i].median = .0f;;
		pulseRateResults[i].highest = .0f;
		pulseRateResults[i].lowest = .0f;
		pulseRateResults[i].satInval = .0f;
		pulseRateResults[i].timeOfHighest = -1.0f;
		pulseRateResults[i].timeOfLowest = -1.0f;
		for (int j = 0; j < pulseBinCount; j++) {
			pulseRateResults[i].histo[j] = .0f;
			pulseRateResults[i].percentHisto[j] = .0f;
		}
	}

	resetBodyPosResults(&bodyPosResults[hypopneaDropLimit30][spO2DropLimit3]);
	resetBodyPosResults(&bodyPosResults[hypopneaDropLimit30][spO2DropLimit4]);
	resetBodyPosResults(&bodyPosResults[hypopneaDropLimit40][spO2DropLimit3]);
	resetBodyPosResults(&bodyPosResults[hypopneaDropLimit40][spO2DropLimit4]);
	resetBodyPosResults(&bodyPosResults[hypopneaDropLimit50][spO2DropLimit3]);
	resetBodyPosResults(&bodyPosResults[hypopneaDropLimit50][spO2DropLimit4]);

	resetLevelResults(&lvlResults[hypopneaDropLimit30][spO2DropLimit3]);
	resetLevelResults(&lvlResults[hypopneaDropLimit30][spO2DropLimit4]);
	resetLevelResults(&lvlResults[hypopneaDropLimit40][spO2DropLimit3]);
	resetLevelResults(&lvlResults[hypopneaDropLimit40][spO2DropLimit4]);
	resetLevelResults(&lvlResults[hypopneaDropLimit50][spO2DropLimit3]);
	resetLevelResults(&lvlResults[hypopneaDropLimit50][spO2DropLimit4]);

	resetRdResults(&rdResults[hypopneaDropLimit30][spO2DropLimit3]);
	resetRdResults(&rdResults[hypopneaDropLimit30][spO2DropLimit4]);
	resetRdResults(&rdResults[hypopneaDropLimit40][spO2DropLimit3]);
	resetRdResults(&rdResults[hypopneaDropLimit40][spO2DropLimit4]);
	resetRdResults(&rdResults[hypopneaDropLimit50][spO2DropLimit3]);
	resetRdResults(&rdResults[hypopneaDropLimit50][spO2DropLimit4]);
	
	//---Snoring data
	for (int i = 0; i < levelTypeCount ; i++) {
		snoringResult.timeAtLevel[i] = .0f;
		snoringResult.percentTimeAtLevel[i] = .0f;
	}
	for (int i = 0; i < posCount ; i++) {
		snoringResult.timeInPos[i] = .0f;
		snoringResult.percentTimeInPos[i] = .0f;
	}
	snoringResult.totalSnoringTime = .0f;
	snoringResult.percentSnoring = .0f;
	snoringResult.meanAmLower = .0f;
	snoringResult.meanAmMulti = .0f;
	snoringResult.meanAmUpper = .0f;
	snoringResult.meanCmLower = .0f;
	snoringResult.meanCmMulti = .0f;
	snoringResult.meanCmUpper = .0f;
		
	snoringResult.standardDevAmLower = .0f;
	snoringResult.standardDevAmMulti = .0f;
	snoringResult.standardDevAmUpper = .0f;
	snoringResult.standardDevCmLower = .0f;
	snoringResult.standardDevCmMulti = .0f;
	snoringResult.standardDevCmUpper = .0f;	
	
	for (int i = 0 ; i < ppBins ; ++i) {
		snoringPoesBin[i] = 0;
		snoringPoesCumulBin[i] = 0;
	}
	for (int i = 0 ; i < ppHistoBins ; ++i) {
		for (int j = 0; j < levelTypeCount; j++) {
			poesHistoBinLevel[i][j] = 0;
			snoringPoesHistoBinLevel[i][j] = 0;
		}
		for (int j = 0; j < posCount; ++j) {
			poesHistoBinBodyPos[i][j] = 0;
			snoringPoesHistoBinBodyPos[i][j] = 0;
		}
	}

	eventByMouse.from = .0f;
	eventByMouse.to = .0f;
	eventByMouse.gripWhat = 0;
	eventByMouse.gripWhere = 0;
	eventByMouse.level = levelTypeNone;
	eventByMouse.type = evntTypeNone;

	dirtyFlag = true;
}

void CEvents::clearStateVector(void)
{
	vector <CMemFile *>::iterator it = stateVector.begin();
	for (; it < stateVector.end(); ++it) {
		delete *it;
	}
	stateVector.clear();
	statePointer = -1;
}

void CEvents::resetRdResults(RD_RESULTS *_rdP)
{
	for (int i = 0; i < obstrEvntCount; i++) {
		_rdP->obstrNum[i] = 0;
		_rdP->obstrPerHour[i] = .0f;
		_rdP->obstrTotalDuration[i] = .0f;
		_rdP->obstrMaxDuration[i] = .0f;
		_rdP->obstrAverageDuration[i] = .0f;
		_rdP->obstrMedianDuration[i] = .0f;
	}
	for (int i = 0; i < centralEvntCount; i++) {
		_rdP->centrNum[i] = 0;
		_rdP->centrPerHour[i] = .0f;
		_rdP->centrTotalDuration[i] = .0f;
		_rdP->centrMaxDuration[i] = .0f;
		_rdP->centrAverageDuration[i] = .0f;
		_rdP->centrMedianDuration[i] = .0f;
	}
}

void CEvents::resetLevelResults(LEVEL_RESULTS *_lvP)
{
	for (int lv = 0; lv < levelTypeCount; lv++) {
		for (int ob = 0; ob < evCountWithLevel; ob++) {
			_lvP->num[ob][lv] = 0;
			_lvP->perHour[ob][lv] = .0f;
		}
		_lvP->totalNum[lv] = 0;
		_lvP->percentTotalNum[lv] = .0f;
		_lvP->totalPerHour[lv] = .0f;
	}
}

void CEvents::resetBodyPosResults(BODY_POS_RESULTS *_bpP)
{
	for (int i = 0; i < posCount; i++) {
		_bpP->central[i] = 0;
		_bpP->spO2[i] = 0;
		_bpP->centralHyp[i] = 0;
		_bpP->time[i] = .0f;
		_bpP->timeSleeping[i] = .0f;
		_bpP->timeAwake[i] = .0f;
		_bpP->percentTime[i] = .0f;
		_bpP->percentTimeSleeping[i] = .0f;
		_bpP->percentTimeAwake[i] = .0f;
		_bpP->snoringTime[i] = .0f;
		_bpP->arousal[i] = 0;

		_bpP->RERAindex[i] = .0f;
		_bpP->OAHindex[i] = .0f;
		_bpP->CAHindex[i] = .0f;
		_bpP->ODindex[i] = .0f;
		_bpP->OAindex[i] = .0f;
		_bpP->OHindex[i] = .0f;
		_bpP->RDindex[i] = .0f;
		_bpP->AHindex[i] = .0f;

		for (int j = 0; j < levelTypeCount; j++) {
			_bpP->obstr[i][j] = 0;
			_bpP->mix[i][j] = 0;
			_bpP->obstrHyp[i][j] = 0;
			_bpP->RERA[i][j] = 0;
		}
	}
	for (int j = 0; j < levelTypeCount; j++) {
		_bpP->percentObstrSupine[j] = .0f;
		_bpP->percentObstrNonSupine[j] = .0f;
		_bpP->percentMixSupine[j] = .0f;
		_bpP->percentMixNonSupine[j] = .0f;
		_bpP->percentObstrHypoSupine[j] = .0f;
		_bpP->percentObstrHypoNonSupine[j] = .0f;
		_bpP->percentRERASupine[j] = .0f;
		_bpP->percentRERANonSupine[j] = .0f;

		_bpP->RERAindexVSlevel[j] = .0f;
		_bpP->OAHindexVSlevel[j] = .0f;
		_bpP->OAindexVSlevel[j] = .0f;
		_bpP->OHindexVSlevel[j] = .0f;
		_bpP->RDindexVSlevel[j] = .0f;
		_bpP->AHindexVSlevel[j] = .0f;
	}
	_bpP->percentCentralSupine = .0f;
	_bpP->percentCentralNonSupine = .0f;
	_bpP->percentCentralHypoSupine = .0f;
	_bpP->percentCentralHypoNonSupine = .0f;
	_bpP->percentSpO2Supine = .0f;
	_bpP->percentSpO2NonSupine = .0f;
	_bpP->percentArousalSupine = .0f;
	_bpP->percentArousalNonSupine = .0f;
}

void CEvents::resetSpO2Results(SPO2_RESULTS *_spP)
{
	for (int i = 0; i < satBinCount; i++) {
		_spP->fallsHisto[i] = 0;
		_spP->percentFallsHisto[i] = .0f;
		_spP->satHisto[i] = .0f;
		_spP->percentSatHisto[i] = .0f;
	}
	_spP->numDrops = 0;
	_spP->oxygenDesaturationIndex = .0f;
	_spP->satAvg = .0f;
	_spP->satHighest = .0f;
	_spP->satLowest = .0f;
	_spP->satMedian = .0f;
	_spP->timeOfSatHighest = -1.0f;
	_spP->timeOfSatLowest = -1.0f;
}

void CEvents::resetEventTimelineVector(void)
{
	eventTimelineVector.clear();
	size_t size = (size_t) (maxStopTime * EVENT_TIMELINE_TIME_INTERVAL_INVERSE);
	eventTimelineVector.resize(size, evntTypeNone);
}

void CEvents::resetSnoringVector(void)
{
	snoringVector.clear();
	size_t size = (size_t) (maxStopTime * EVENT_TIMELINE_TIME_INTERVAL_INVERSE);
	snoringVector.resize(size, evntTypeNone);
}

void CEvents::setData(	CBatteryDataSet		*_battData		,
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
					COleDateTime			_startRecording)
{
	battData					= _battData			;
	cmData						= _cmData			;	
	amData						= _amData			;	
	bpXYZData					= _bpXYZData		;		
	acData						= _acData			;	
	torsoAcData					= _acTorsoData		;
	oxData						= _oxData			;	
	bpData						= _bpData			;	
	cathData					= _cathData			;	
	ag200MicData				= _ag200MicData		;
	startRecordingClockTime		= _startRecording	;
	respData					= _respData			;
	mMarkerData					= _manMarkerData	;
}

bool CEvents::getIsVisualEventEditingActive(void)
{
	return visualEventEditingIsActive;
}

void CEvents::setVisualEventEditingActive(bool _on)
{
	visualEventEditingIsActive = _on;
}

/*
Description: Checks for an SpO2 drop start between _from and _to
If yes, return pointer to the SpO2 event, else return NULL;
*/
CSpO2DropEvnt *CEvents::findSpO2DropToMarry(float _from,float _to,float *_center, int _spO2DropLimit)
{
	if (spO2DropLimit3 == _spO2DropLimit) {
		int num = spO2DropEventArray3.GetCount();
		for (int i = 0; i < num; ++i) {
			CSpO2DropEvnt *eV = spO2DropEventArray3.GetAt(i);
			if (eV->getIsActive()) {
				float myFrom = eV->getFrom();
				float myTo = eV->getTo();
				float t = (myFrom + myTo) / 2.0f;
				float t2 = 0.0f;				
				if ((myFrom >= _from) && (myFrom <= _to) && !eV->getHypoPartner(&t2)) { // t2 is != 0 if eV is already married
					*_center = t;
					return eV;
				}
			}
		}
	}
	else if (spO2DropLimit4 == _spO2DropLimit) {
		int num = spO2DropEventArray4.GetCount();
		for (int i = 0; i < num; ++i) {
			CSpO2DropEvnt *eV = spO2DropEventArray4.GetAt(i);
			if (eV->getIsActive()) {
				float myFrom = eV->getFrom();
				float myTo = eV->getTo();
				float t = (myFrom + myTo) / 2.0f;
				float t2 = 0.0f;
				if ((myFrom >= _from) && (myFrom <= _to) && !eV->getHypoPartner(&t2))  { // t2 is != 0 if eV is already married
					*_center = t;
					return eV;
				}
			}
		}
	}
	return NULL;
}

void CEvents::findEffectiveStartAndStop(void)
{
	//---Find max length of time axis
	vector <FLOAT> stops;
	stops.push_back(battData->getStop());
	stops.push_back(cmData->getStop());
	stops.push_back(amData->getStop());
	stops.push_back(bpXYZData->getStop());
	stops.push_back(acData->getStop());
	stops.push_back(torsoAcData->getStop());
	stops.push_back(oxData->getStop());
	stops.push_back(bpData->getStop());
	stops.push_back(cathData->getStop());
	stops.push_back(ag200MicData->getStop());
	stops.push_back(respData->getStop());
	sort(stops.begin(),stops.end());
	effectiveStopTime = *(stops.end() - 1);
	maxStopTime = effectiveStopTime;
	effectiveStartTime = .0f;
	//totalSleepTime = effectiveStopTime - effectiveStartTime;

	//---Require time in bed before start. If in bed for some time and then up, stop then
	vector <FLOAT> *bp = bpData->getBpVector();
	vector <FLOAT> *bpTime = bpData->getTimeaxisVector();

	if (2 > bp->size()) return;
	if (2 > bpTime->size()) return;

	vector <FLOAT>::iterator b1,bt;
	float t0,t1;
	t0 = t1 = .0f;
	bt = bpTime->begin();
	b1 = bp->begin();
	bool inBed;
	if (*b1 < posUpright) {
		inBed = true;
		t0 = *bt;
	}
	else inBed = false;
	bt = bpTime->begin() + 1;
	for (b1 = bp->begin() + 1 ; b1 < bp->end() ; b1++, bt++) {
		if (!inBed) {
			if (*b1 < posUpright) { 
				inBed = true;
				t0 = *bt;
			}
		}
		else if (inBed) {
			if (*b1 >= posUpright) { 
				inBed = false;
			}
			else t1 = *bt;
		}
		if ((t1 - t0) > detectionCriteria.timeInBedLowLimit) break;
	}
	if (t0 > t1) return;

	bt = bpTime->end() - 1;
	float tt0,tt1;
	tt0 = tt1 = .0f;
	b1 = bp->end() - 1;
	if (*b1 < posUpright) {
		inBed = true;
		tt0 = *bt;
	}
	else inBed = false;
	bt = bpTime->end() - 2;
	for (b1 = bp->end() - 2 ; b1 >= bp->begin() ; b1--, bt--) {
		if (!inBed) {
			if (*b1 < posUpright) { 
				inBed = true;
				tt0 = *bt;
			}
		}
		else if (inBed) {
			if (*b1 >= posUpright) { 
				inBed = false;
			}
			else tt1 = *bt;
		}
		if ((tt0 - tt1) > detectionCriteria.timeInBedLowLimit) break;
		if (b1 == bp->begin()) break;  // Cannot decrement when this condition is satisfied
	}
	if (tt1 > tt0) return;

	effectiveStartTime = t0;
	effectiveStopTime = tt0;
	
	effectiveStartTime = effectiveStartTime > effectiveStopTime ? effectiveStopTime : effectiveStartTime;
	//totalSleepTime = effectiveStopTime - effectiveStartTime;

	CExcludedEvnt *eP0 = new CExcludedEvnt;
	eP0->setRecordingStartTimeOfDay(startRecordingClockTime);
	eP0->setFrom(.0f);
	eP0->setTo(effectiveStartTime);
	eP0->setAsFirst(true);
	excludedEventArray.Add(eP0);

	CExcludedEvnt *eP1 = new CExcludedEvnt;
	eP1->setRecordingStartTimeOfDay(startRecordingClockTime);
	eP1->setFrom(effectiveStopTime);
	eP1->setTo(maxStopTime);
	eP1->setAsLast(true);
	excludedEventArray.Add(eP1);
}

bool CEvents::analysePulseRate(void)
{
	vector <FLOAT> *oxP = oxData->getPulseRateVector();
	vector <FLOAT> *oxT = oxData->getTimeaxisVector();

	if (oxP->size() <= 2) return false;

	ASSERT(oxP->size() == oxT->size());
		
	//---Analysis
	pulseRateResults[overall].satInval = oxData->getInvalidTime();
	oxData->getPRKeyData(&pulseRateResults[overall].average,&pulseRateResults[overall].median,&pulseRateResults[overall].lowest,&pulseRateResults[overall].highest,
		&pulseRateResults[overall].timeOfLowest,&pulseRateResults[overall].timeOfHighest);
	oxData->getPulseRateHistogramData(&pulseRateResults[overall].histo[0]);
	
	float sum = .0f;
	for (int i = 0 ; i < pulseBinCount ; i++) sum += pulseRateResults[overall].histo[i];
	
	for (int i = 0 ; i < pulseBinCount ; i++) {
		pulseRateResults[overall].percentHisto[i] = sum == .0f ? .0f : 100.0f * pulseRateResults[overall].histo[i] / sum;
	}

	return true;
}

void CEvents::sumUpSpO2(int _dropLimit)
{
	if (oximeterTimelineVector.size() == 0) return;

	for (int j = 0; j < numSleepAwakeStates; j++) {
		for (int i = 0; i < 8; i++) {
			spO2Res[_dropLimit][j].fallsHisto[i] = 0;
			spO2Res[_dropLimit][j].percentFallsHisto[i] = .0f;
			spO2Res[_dropLimit][j].satHisto[i] = .0f;
			spO2Res[_dropLimit][j].percentSatHisto[i] = .0f;
		}
	}

	int numEvents = 0;
	if (spO2DropLimit3 == _dropLimit) {
		int num = spO2DropEventArray3.GetCount();
		for (int i = 0; i < num; i++) {
			CSpO2DropEvnt *eP = spO2DropEventArray3.GetAt(i);
			if (eP->getIsActive()) {
				toSpO2FallsHistogram(eP->getBeginSpO2(), _dropLimit, overall);
				numEvents++;
			}
		}
	}
	else if (spO2DropLimit4 == _dropLimit) {
		int num = spO2DropEventArray4.GetCount();
		for (int i = 0; i < num; i++) {
			CSpO2DropEvnt *eP = spO2DropEventArray4.GetAt(i);
			if (eP->getIsActive()) {
				toSpO2FallsHistogram(eP->getBeginSpO2(), _dropLimit, overall);
				numEvents++;
			}
		}
	}

	//---Falls only occur in sleep, so set asleep = overall and awake stays at zero
	for (int i = 0; i < 8; i++) {
		spO2Res[_dropLimit][asleep].fallsHisto[i] = spO2Res[_dropLimit][overall].fallsHisto[i];
		spO2Res[_dropLimit][asleep].percentFallsHisto[i] = spO2Res[_dropLimit][overall].percentFallsHisto[i];
	}

	//---Index
	spO2Res[_dropLimit][overall].oxygenDesaturationIndex = totalSleepTime == .0f ? .0f : 60.0f * 60.0f * numEvents / totalSleepTime;
	spO2Res[_dropLimit][asleep].oxygenDesaturationIndex = spO2Res[_dropLimit][overall].oxygenDesaturationIndex;
	spO2Res[_dropLimit][awake].oxygenDesaturationIndex = .0f;

	spO2Res[_dropLimit][overall].numDrops = numEvents;
	spO2Res[_dropLimit][asleep].numDrops = numEvents;
	spO2Res[_dropLimit][awake].numDrops = 0;

	spO2Res[_dropLimit][awake].satHisto[satInvalid] = EVENT_TIMELINE_TIME_INTERVAL *
		count_if(oximeterTimelineVector.begin(), oximeterTimelineVector.end(), isOxInvalidWhileAwake);
	spO2Res[_dropLimit][asleep].satHisto[satInvalid] = EVENT_TIMELINE_TIME_INTERVAL *
		count_if(oximeterTimelineVector.begin(), oximeterTimelineVector.end(), isOxInvalidWhileAsleep);
	spO2Res[_dropLimit][overall].satHisto[satInvalid] = EVENT_TIMELINE_TIME_INTERVAL *
		count_if(oximeterTimelineVector.begin(), oximeterTimelineVector.end(), isOxInvalidWhileNotExcluded);

	pulseRateResults[awake].histo[pulseInvalid] = EVENT_TIMELINE_TIME_INTERVAL *
		count_if(oximeterTimelineVector.begin(), oximeterTimelineVector.end(), isOxInvalidWhileAwake);
	pulseRateResults[asleep].histo[pulseInvalid] = EVENT_TIMELINE_TIME_INTERVAL *
		count_if(oximeterTimelineVector.begin(), oximeterTimelineVector.end(), isOxInvalidWhileAsleep);
	pulseRateResults[overall].histo[pulseInvalid] = EVENT_TIMELINE_TIME_INTERVAL *
		count_if(oximeterTimelineVector.begin(), oximeterTimelineVector.end(), isOxInvalidWhileNotExcluded);

	spO2Res[_dropLimit][awake].fallsHisto[satInvalid] = 0;			// Meaningless, must be zero
	spO2Res[_dropLimit][asleep].fallsHisto[satInvalid] = 0;			// Meaningless, must be zero
	spO2Res[_dropLimit][overall].fallsHisto[satInvalid] = 0;		// Meaningless, must be zero

	//---SpO2 histogram
	spO2Res[_dropLimit][awake].satHisto[satFrom95to100] = EVENT_TIMELINE_TIME_INTERVAL *
		count_if(oximeterTimelineVector.begin(), oximeterTimelineVector.end(), isOxWhileAwake95to100);
	spO2Res[_dropLimit][awake].satHisto[satFrom90to95] = EVENT_TIMELINE_TIME_INTERVAL *
		count_if(oximeterTimelineVector.begin(), oximeterTimelineVector.end(), isOxWhileAwake90to95);
	spO2Res[_dropLimit][awake].satHisto[satFrom85to90] = EVENT_TIMELINE_TIME_INTERVAL *
		count_if(oximeterTimelineVector.begin(), oximeterTimelineVector.end(), isOxWhileAwake85to90);
	spO2Res[_dropLimit][awake].satHisto[satFrom80to85] = EVENT_TIMELINE_TIME_INTERVAL *
		count_if(oximeterTimelineVector.begin(), oximeterTimelineVector.end(), isOxWhileAwake80to85);
	spO2Res[_dropLimit][awake].satHisto[satFrom75to80] = EVENT_TIMELINE_TIME_INTERVAL *
		count_if(oximeterTimelineVector.begin(), oximeterTimelineVector.end(), isOxWhileAwake75to80);
	spO2Res[_dropLimit][awake].satHisto[satFrom70to75] = EVENT_TIMELINE_TIME_INTERVAL *
		count_if(oximeterTimelineVector.begin(), oximeterTimelineVector.end(), isOxWhileAwake70to75);
	spO2Res[_dropLimit][awake].satHisto[satFrom65to70] = EVENT_TIMELINE_TIME_INTERVAL *
		count_if(oximeterTimelineVector.begin(), oximeterTimelineVector.end(), isOxWhileAwake65to70);
	spO2Res[_dropLimit][awake].satHisto[satBelow65] = EVENT_TIMELINE_TIME_INTERVAL *
		count_if(oximeterTimelineVector.begin(), oximeterTimelineVector.end(), isOxWhileAwake0to65);

	spO2Res[_dropLimit][asleep].satHisto[satFrom95to100] = EVENT_TIMELINE_TIME_INTERVAL *
		count_if(oximeterTimelineVector.begin(), oximeterTimelineVector.end(), isOxWhileAsleep95to100);
	spO2Res[_dropLimit][asleep].satHisto[satFrom90to95] = EVENT_TIMELINE_TIME_INTERVAL *
		count_if(oximeterTimelineVector.begin(), oximeterTimelineVector.end(), isOxWhileAsleep90to95);
	spO2Res[_dropLimit][asleep].satHisto[satFrom85to90] = EVENT_TIMELINE_TIME_INTERVAL *
		count_if(oximeterTimelineVector.begin(), oximeterTimelineVector.end(), isOxWhileAsleep85to90);
	spO2Res[_dropLimit][asleep].satHisto[satFrom80to85] = EVENT_TIMELINE_TIME_INTERVAL *
		count_if(oximeterTimelineVector.begin(), oximeterTimelineVector.end(), isOxWhileAsleep80to85);
	spO2Res[_dropLimit][asleep].satHisto[satFrom75to80] = EVENT_TIMELINE_TIME_INTERVAL *
		count_if(oximeterTimelineVector.begin(), oximeterTimelineVector.end(), isOxWhileAsleep75to80);
	spO2Res[_dropLimit][asleep].satHisto[satFrom70to75] = EVENT_TIMELINE_TIME_INTERVAL *
		count_if(oximeterTimelineVector.begin(), oximeterTimelineVector.end(), isOxWhileAsleep70to75);
	spO2Res[_dropLimit][asleep].satHisto[satFrom65to70] = EVENT_TIMELINE_TIME_INTERVAL *
		count_if(oximeterTimelineVector.begin(), oximeterTimelineVector.end(), isOxWhileAsleep65to70);
	spO2Res[_dropLimit][asleep].satHisto[satBelow65] = EVENT_TIMELINE_TIME_INTERVAL *
		count_if(oximeterTimelineVector.begin(), oximeterTimelineVector.end(), isOxWhileAsleep0to65);

	spO2Res[_dropLimit][overall].satHisto[satFrom95to100] = EVENT_TIMELINE_TIME_INTERVAL *
		count_if(oximeterTimelineVector.begin(), oximeterTimelineVector.end(), isOx95to100);
	spO2Res[_dropLimit][overall].satHisto[satFrom90to95] = EVENT_TIMELINE_TIME_INTERVAL *
		count_if(oximeterTimelineVector.begin(), oximeterTimelineVector.end(), isOx90to95);
	spO2Res[_dropLimit][overall].satHisto[satFrom85to90] = EVENT_TIMELINE_TIME_INTERVAL *
		count_if(oximeterTimelineVector.begin(), oximeterTimelineVector.end(), isOx85to90);
	spO2Res[_dropLimit][overall].satHisto[satFrom80to85] = EVENT_TIMELINE_TIME_INTERVAL *
		count_if(oximeterTimelineVector.begin(), oximeterTimelineVector.end(), isOx80to85);
	spO2Res[_dropLimit][overall].satHisto[satFrom75to80] = EVENT_TIMELINE_TIME_INTERVAL *
		count_if(oximeterTimelineVector.begin(), oximeterTimelineVector.end(), isOx75to80);
	spO2Res[_dropLimit][overall].satHisto[satFrom70to75] = EVENT_TIMELINE_TIME_INTERVAL *
		count_if(oximeterTimelineVector.begin(), oximeterTimelineVector.end(), isOx70to75);
	spO2Res[_dropLimit][overall].satHisto[satFrom65to70] = EVENT_TIMELINE_TIME_INTERVAL *
		count_if(oximeterTimelineVector.begin(), oximeterTimelineVector.end(), isOx65to70);
	spO2Res[_dropLimit][overall].satHisto[satBelow65] = EVENT_TIMELINE_TIME_INTERVAL *
		count_if(oximeterTimelineVector.begin(), oximeterTimelineVector.end(), isOx0to65);

	//---PR histogram
	pulseRateResults[awake].histo[pulseAbove120] = EVENT_TIMELINE_TIME_INTERVAL *
		count_if(oximeterTimelineVector.begin(), oximeterTimelineVector.end(), isPRWhileAwakeAbove120);
	pulseRateResults[awake].histo[pulseFrom110to120] = EVENT_TIMELINE_TIME_INTERVAL *			 
		count_if(oximeterTimelineVector.begin(), oximeterTimelineVector.end(), isPRWhileAwake110to120);
	pulseRateResults[awake].histo[pulseFrom100to110] = EVENT_TIMELINE_TIME_INTERVAL *
		count_if(oximeterTimelineVector.begin(), oximeterTimelineVector.end(), isPRWhileAwake100to110);
	pulseRateResults[awake].histo[pulseFrom90to100] = EVENT_TIMELINE_TIME_INTERVAL *
		count_if(oximeterTimelineVector.begin(), oximeterTimelineVector.end(), isPRWhileAwake90to100);
	pulseRateResults[awake].histo[pulseFrom80to90] = EVENT_TIMELINE_TIME_INTERVAL *
		count_if(oximeterTimelineVector.begin(), oximeterTimelineVector.end(), isPRWhileAwake80to90);
	pulseRateResults[awake].histo[pulseFrom70to80] = EVENT_TIMELINE_TIME_INTERVAL *
		count_if(oximeterTimelineVector.begin(), oximeterTimelineVector.end(), isPRWhileAwake70to80);
	pulseRateResults[awake].histo[pulseFrom60to70] = EVENT_TIMELINE_TIME_INTERVAL *
		count_if(oximeterTimelineVector.begin(), oximeterTimelineVector.end(), isPRWhileAwake60to70);
	pulseRateResults[awake].histo[pulseBelow60] = EVENT_TIMELINE_TIME_INTERVAL *
		count_if(oximeterTimelineVector.begin(), oximeterTimelineVector.end(), isPRWhileAwake0to60);

	pulseRateResults[asleep].histo[pulseAbove120] = EVENT_TIMELINE_TIME_INTERVAL *
		count_if(oximeterTimelineVector.begin(), oximeterTimelineVector.end(), isPRWhileAsleepAbove120);
	pulseRateResults[asleep].histo[pulseFrom110to120] = EVENT_TIMELINE_TIME_INTERVAL *
		count_if(oximeterTimelineVector.begin(), oximeterTimelineVector.end(), isPRWhileAsleep110to120);
	pulseRateResults[asleep].histo[pulseFrom100to110] = EVENT_TIMELINE_TIME_INTERVAL *
		count_if(oximeterTimelineVector.begin(), oximeterTimelineVector.end(), isPRWhileAsleep100to110);
	pulseRateResults[asleep].histo[pulseFrom90to100] = EVENT_TIMELINE_TIME_INTERVAL *
		count_if(oximeterTimelineVector.begin(), oximeterTimelineVector.end(), isPRWhileAsleep90to100);
	pulseRateResults[asleep].histo[pulseFrom80to90] = EVENT_TIMELINE_TIME_INTERVAL *
		count_if(oximeterTimelineVector.begin(), oximeterTimelineVector.end(), isPRWhileAsleep80to90);
	pulseRateResults[asleep].histo[pulseFrom70to80] = EVENT_TIMELINE_TIME_INTERVAL *
		count_if(oximeterTimelineVector.begin(), oximeterTimelineVector.end(), isPRWhileAsleep70to80);
	pulseRateResults[asleep].histo[pulseFrom60to70] = EVENT_TIMELINE_TIME_INTERVAL *
		count_if(oximeterTimelineVector.begin(), oximeterTimelineVector.end(), isPRWhileAsleep60to70);
	pulseRateResults[asleep].histo[pulseBelow60] = EVENT_TIMELINE_TIME_INTERVAL *
		count_if(oximeterTimelineVector.begin(), oximeterTimelineVector.end(), isPRWhileAsleep0to60);

	pulseRateResults[overall].histo[pulseAbove120] = EVENT_TIMELINE_TIME_INTERVAL *
		count_if(oximeterTimelineVector.begin(), oximeterTimelineVector.end(), isPRAbove120);
	pulseRateResults[overall].histo[pulseFrom110to120] = EVENT_TIMELINE_TIME_INTERVAL *
		count_if(oximeterTimelineVector.begin(), oximeterTimelineVector.end(), isPR110to120);
	pulseRateResults[overall].histo[pulseFrom100to110] = EVENT_TIMELINE_TIME_INTERVAL *
		count_if(oximeterTimelineVector.begin(), oximeterTimelineVector.end(), isPR100to110);
	pulseRateResults[overall].histo[pulseFrom90to100] = EVENT_TIMELINE_TIME_INTERVAL *
		count_if(oximeterTimelineVector.begin(), oximeterTimelineVector.end(), isPR90to100);
	pulseRateResults[overall].histo[pulseFrom80to90] = EVENT_TIMELINE_TIME_INTERVAL *
		count_if(oximeterTimelineVector.begin(), oximeterTimelineVector.end(), isPR80to90);
	pulseRateResults[overall].histo[pulseFrom70to80] = EVENT_TIMELINE_TIME_INTERVAL *
		count_if(oximeterTimelineVector.begin(), oximeterTimelineVector.end(), isPR70to80);
	pulseRateResults[overall].histo[pulseFrom60to70] = EVENT_TIMELINE_TIME_INTERVAL *
		count_if(oximeterTimelineVector.begin(), oximeterTimelineVector.end(), isPR60to70);
	pulseRateResults[overall].histo[pulseBelow60] = EVENT_TIMELINE_TIME_INTERVAL *
		count_if(oximeterTimelineVector.begin(), oximeterTimelineVector.end(), isPR0to60);

	spO2Res[_dropLimit][overall].satHisto[satTotal] = .0f;
	spO2Res[_dropLimit][overall].fallsHisto[satTotal] = 0;

	float secondsBelowSat90[numSleepAwakeStates];
	secondsBelowSat90[awake] = .0f;
	secondsBelowSat90[asleep] = .0f;
	secondsBelowSat90[overall] = .0f;
	for (int i = 0; i < satTotal - 1; i++) {
		spO2Res[_dropLimit][awake].satHisto[satTotal] += spO2Res[_dropLimit][awake].satHisto[i];
		spO2Res[_dropLimit][asleep].satHisto[satTotal] += spO2Res[_dropLimit][asleep].satHisto[i];
		spO2Res[_dropLimit][overall].satHisto[satTotal] += spO2Res[_dropLimit][overall].satHisto[i];
		if (i >= satFrom85to90) {
			secondsBelowSat90[awake] += spO2Res[_dropLimit][awake].satHisto[i];
			secondsBelowSat90[asleep] += spO2Res[_dropLimit][asleep].satHisto[i];
			secondsBelowSat90[overall] += spO2Res[_dropLimit][overall].satHisto[i];
		}
	}
	for (int i = 0; i < satTotal - 1; i++) {
		spO2Res[_dropLimit][awake].fallsHisto[satTotal] += spO2Res[_dropLimit][awake].fallsHisto[i];
		spO2Res[_dropLimit][asleep].fallsHisto[satTotal] += spO2Res[_dropLimit][asleep].fallsHisto[i];
		spO2Res[_dropLimit][overall].fallsHisto[satTotal] += spO2Res[_dropLimit][overall].fallsHisto[i];
	}
	
	////---SpO2 percent distribution
	//for (int j = 0; j < numSleepAwakeStates; j++) {
	//	for (int i = 0; i < satTotal - 1; i++) {
	//		spO2Res[_dropLimit][j].percentSatHisto[i] = spO2Res[_dropLimit][j].satHisto[satTotal] == .0f ? .0f : 100.0f * spO2Res[_dropLimit][j].satHisto[i] / spO2Res[_dropLimit][j].satHisto[satTotal];
	//		if (i >= satFrom85to90)
	//			spO2Res[_dropLimit][j].percentBelowSat90 = spO2Res[_dropLimit][j].satHisto[satTotal] == .0f ? .0f : 100.0f * secondsBelowSat90[j] / spO2Res[_dropLimit][j].satHisto[satTotal];
	//	}
	//	spO2Res[_dropLimit][j].percentSatHisto[satTotal] = 100.0f;
	//}

	//---SpO2 falls percent distribution
	for (int j = 0; j < numSleepAwakeStates; j++) {
		for (int i = 0; i < satTotal - 1; i++)
			spO2Res[_dropLimit][j].percentFallsHisto[i] = spO2Res[_dropLimit][j].fallsHisto[satTotal] == .0f ?
			.0f : 100.0f * (float)spO2Res[_dropLimit][j].fallsHisto[i] / spO2Res[_dropLimit][j].fallsHisto[satTotal];
		spO2Res[_dropLimit][j].percentFallsHisto[satTotal] = 100.0f;
	}

	//---SPO2 percent distribution
	for (int j = 0; j < numSleepAwakeStates; j++) {
		float sumo2 = .0f;
		for (int i = 0; i < satTotal; i++) sumo2 += spO2Res[_dropLimit][j].satHisto[i];

		spO2Res[_dropLimit][j].satHisto[satTotal] = sumo2;

		for (int i = 0; i < satBinCount; i++) {
			spO2Res[_dropLimit][j].percentSatHisto[i] = sumo2 == .0f ? .0f : 100.0f * spO2Res[_dropLimit][j].satHisto[i] / sumo2;
		}
	}

	//---PR percent distribution
	for (int j = 0; j < numSleepAwakeStates; j++) {
		float sum = .0f;
		for (int i = 0; i < pulseBinCount; i++) sum += pulseRateResults[j].histo[i];

		for (int i = 0; i < pulseBinCount; i++) {
			pulseRateResults[j].percentHisto[i] = sum == .0f ? .0f : 100.0f * pulseRateResults[j].histo[i] / sum;
		}
	}
}

bool CEvents::analyseSpO2(int _dropLimit)
{
	vector <FLOAT> *oxD = oxData->getSpO2Vector();
	vector <FLOAT> *oxT = oxData->getTimeaxisVector();

	if (oxD->size() <= 2) return false;

	ASSERT(oxD->size() == oxT->size());
	
	//---Find SpO2 drops and CSpO2DropEvents
	vector <FLOAT>::iterator i0,i1,start,stop;
	i0 = oxD->begin();
	float drop = 0;

	float limit = 4.0f; 
	CArray <CSpO2DropEvnt *, CSpO2DropEvnt *> *arrayToUseP = &spO2DropEventArray4;
	switch (_dropLimit) {
	case spO2DropLimit3:
		limit = spO2TestLimit3;
		arrayToUseP = &spO2DropEventArray3;
		break;
	case spO2DropLimit4:
		limit = spO2TestLimit4;
		arrayToUseP = &spO2DropEventArray4;
		break;
	}

	int lastTopIndex = 0;
	int numEvents = 0;

	for (i1 = (oxD->begin() + 1) ; i1 < oxD->end(); i1++) {
		if (*i1 > *i0) {  // Curve tips upwards
			if (drop >= limit) {
				
				//---Move to where the drop started if the first part is flat
				float startVal = oxD->at(lastTopIndex);
				unsigned int index = lastTopIndex;
				index++;
				while ((index < oxD->size()) && (startVal == oxD->at(index))) {
					index++;
					lastTopIndex++;
				}

				//---Get bp
				start = oxT->begin();
				start += lastTopIndex;
				stop = oxT->begin();
				stop += (i1 - oxD->begin());
				short bp = bpData->getPos(start, stop);

				float from = oxT->at(lastTopIndex);
				float to = oxT->at(i1 - oxD->begin());
				if (from < to) {
					if ((oxData->getIsDataValidAtTime(to)) || (oxData->getIsDataValidAtTime(from))) {
						CSpO2DropEvnt *dE = new CSpO2DropEvnt(_dropLimit,
							drop,
							oxD->at(lastTopIndex),
							oxD->at(i1 - oxD->begin()),
							from,
							to,
							startRecordingClockTime, bp);
						arrayToUseP->Add(dE);
						numEvents++;
					}
				}
			}
			drop = .0f ; 
			lastTopIndex = i1 - oxD->begin();
		}
		else if (*i1 < *i0)  // Curve is moving down
			drop += (*i0 - *i1);
		i0++;
	}
	return true;
}

/*
Description: If a,b and c are almost equal, returns true. Returns false if not
*/
bool CEvents::areAlmostEqual(int _a,int _b,int _c)
{
	if (!areAlmostEqual(_a,_b)) return false;
	if (!areAlmostEqual(_a,_c)) return false;
	return true;
}

/*
Description: If a,b are almost equal, (+/- 20% margin)  returns true. Returns false if not
*/
bool CEvents::areAlmostEqual(int _a,int _b)
{
	int low = (int) (0.8f * _b);
	int high = (int) (1.2f * _b);
	if ((_a >= low) && (_a <= high)) return true;
	return false;
}

/*
Description: Fill a histogram with number of hits in the interval
*/
void CEvents::toSpO2FallsHistogram(float _saturation,int _dropLimit, int _sleepAwakeState)
{
	if (_saturation < 65.0f) {
		spO2Res[_dropLimit][_sleepAwakeState].fallsHisto[7]++;
		return;
	}
	if (_saturation < 70.0f) {
		spO2Res[_dropLimit][_sleepAwakeState].fallsHisto[6]++;
		return;
	}
	if (_saturation < 75.0f) {
		spO2Res[_dropLimit][_sleepAwakeState].fallsHisto[5]++;
		return;
	}
	if (_saturation < 80.0f) {
		spO2Res[_dropLimit][_sleepAwakeState].fallsHisto[4]++;
		return;
	}
	if (_saturation < 85.0f) {
		spO2Res[_dropLimit][_sleepAwakeState].fallsHisto[3]++;
		return;
	}
	if (_saturation < 90.0f) {
		spO2Res[_dropLimit][_sleepAwakeState].fallsHisto[2]++;
		return;
	}
	if (_saturation < 95.0f) {
		spO2Res[_dropLimit][_sleepAwakeState].fallsHisto[1]++;
		return;
	}
	spO2Res[_dropLimit][_sleepAwakeState].fallsHisto[0]++;
}


/*
Description: 
Returns beginning and ending of all events (begin0,end0,begin1,end1,.....
Used for events with level
_type: Type of event
_level: Level of event
*/
vector <FLOAT> *CEvents::getBeginsEnds(UINT _type,UINT _level)
{
	if (visualEventEditingIsActive) {
		if ((_type == evMix) && (eventsBeingVisuallyEdited & evntTypeMixed)) return &editEventsWithLevelBeginsEnds[evMix][_level];
		else if ((_type == evObstr) && (eventsBeingVisuallyEdited & evntTypeObstr)) return &editEventsWithLevelBeginsEnds[evObstr][_level];
		else if ((_type == evObstrHypo) && (eventsBeingVisuallyEdited & evntTypeHypoObstr)) return &editEventsWithLevelBeginsEnds[evObstrHypo][_level];
		else if ((_type == evRERA) && (eventsBeingVisuallyEdited & evntTypeRERA)) return &editEventsWithLevelBeginsEnds[evRERA][_level];
		else if ((_type == evSnoringLowerLevel) && (eventsBeingVisuallyEdited & evntTypeSnoring))
			return &editEventsWithLevelBeginsEnds[evSnoringLowerLevel][_level];
		else if ((_type == evSnoringUpperLevel) && (eventsBeingVisuallyEdited & evntTypeSnoring))
		 	return &editEventsWithLevelBeginsEnds[evSnoringUpperLevel][_level];
		else if ((_type == evSnoringMultiLevel) && (eventsBeingVisuallyEdited & evntTypeSnoring))
			return &editEventsWithLevelBeginsEnds[evSnoringMultiLevel][_level];
		else if ((_type == evSnoringUndefLevel) && (eventsBeingVisuallyEdited & evntTypeSnoring))
			return &editEventsWithLevelBeginsEnds[evSnoringUndefLevel][_level];
	}

	return &eventsWithLevelBeginsEnds[_type][_level];
}

/*
Description: 
Returns beginning and ending of all events (begin0,end0,begin1,end1,.....
Used for events without level
_type: Type of event
*/
vector <FLOAT> *CEvents::getBeginsEnds(UINT _type)
{
	if (visualEventEditingIsActive) {

		if ((_type == evArousal) && (eventsBeingVisuallyEdited & evntTypeArousal)) return &editEventsWithoutLevelBeginsEnds[evArousal];
		else if ((_type == evCentral) && (eventsBeingVisuallyEdited & evntTypeCentral)) return &editEventsWithoutLevelBeginsEnds[evCentral];
		else if ((_type == evCentralHypo) && (eventsBeingVisuallyEdited & evntTypeHypoCentral)) return &editEventsWithoutLevelBeginsEnds[evCentralHypo];
		else if ((_type == evSpO2) && (eventsBeingVisuallyEdited & evntTypeSpO2Drop)) return &editEventsWithoutLevelBeginsEnds[evSpO2];
		else if ((_type == evManual) && (eventsBeingVisuallyEdited & evntTypeManMarker)) return &editEventsWithoutLevelBeginsEnds[evManual];
		else if ((_type == evAwake) && (eventsBeingVisuallyEdited & evntTypeAwake)) return &editEventsWithoutLevelBeginsEnds[evAwake];
		else if ((_type == evExcluded) && (eventsBeingVisuallyEdited & evntTypeExcluded)) return &editEventsWithoutLevelBeginsEnds[evExcluded];
		else if ((_type == evSwallow) && (eventsBeingVisuallyEdited & evntTypeSwallow)) return &editEventsWithoutLevelBeginsEnds[evSwallow];
		else if ((_type == evImported) && (eventsBeingVisuallyEdited & evntTypeImported)) return &editEventsWithoutLevelBeginsEnds[evImported];
	}
	return &eventsWithoutLevelBeginsEnds[_type];
}

CArray <CSpO2DropEvnt *,CSpO2DropEvnt *> *CEvents::getEventArraySpO2(void)
{
	return spO2DropEventArrayCurrentP;
}

BODY_POS_RESULTS CEvents::getBodyPosResults(void)
{
	return bodyPosResults[detectionCriteria.hypopneaDropLimit][detectionCriteria.spO2DropLimit];
}

/*
Description: Protected funtion. Returns data according to criteria
*/
int CEvents::getNumObstrHypopnea(int _hypoLimit, int _spO2Drop)
{
	return numObstrHypopnea[_hypoLimit][_spO2Drop];
}

/*
Description: Protected funtion. Returns data according to criteria
*/
int CEvents::getNumCentralHypopnea(int _hypoLimit, int _spO2Drop)
{
	return numCentralHypopnea[_hypoLimit][_spO2Drop];
}

/*
Description: Public funtion. Returns currently valid data
*/
int CEvents::getNumObstrHypopnea()
{
	return numObstrHypopnea[detectionCriteria.hypopneaDropLimit][detectionCriteria.spO2DropLimit];
}

/*
Description: Public funtion. Returns currently valid data
*/
int CEvents::getNumCentralHypopnea()
{
	return numCentralHypopnea[detectionCriteria.hypopneaDropLimit][detectionCriteria.spO2DropLimit];
}

int CEvents::getNumImportedEvents()
{
	return importedEventArray.GetCount();
}

void CEvents::getPoesHistoLevelBins(float *_poesHistoLevelBins,int _num)
{
	ASSERT(ppHistoBins == _num);

	float *p0 = _poesHistoLevelBins;
	for (int i = 0 ; i < _num ; i++) {
		for (int j = 0 ; j < levelTypeCount ; j++) {
			*p0++ = poesHistoBinLevel[i][j];
		}
	}
}

void CEvents::getPphHistoBodyPosBins(float * _pphHistoBinBodyPos, int _num)
{
	ASSERT(ppHistoBins == _num);

	float *p0 = _pphHistoBinBodyPos;
	for (int i = 0; i < _num; i++) {
		for (int j = 0; j < posCount; j++) {
			*p0++ = pphHistoBinBodyPos[i][j];
		}
	}
}

void CEvents::getPphHistoLevelBins(float * _pphHistoLevelBins, int _num)
{
	ASSERT(ppHistoBins == _num);

	float *p0 = _pphHistoLevelBins;
	for (int i = 0; i < _num; i++) {
		for (int j = 0; j < levelTypeCount; j++) {
			*p0++ = pphHistoBinLevel[i][j];
		}
	}
}

void CEvents::getSnoringPoesHistoBodyPosBins(float * _poesHistoBinBodyPos, int _num)
{
	ASSERT(ppHistoBins == _num);

	float *p0 = _poesHistoBinBodyPos;
	for (int i = 0; i < _num; i++) {
		for (int j = 0; j < posCount; j++) {
			*p0++ = snoringPoesHistoBinBodyPos[i][j];
		}
	}
}

void CEvents::getSnoringPoesHistoLevelBins(float * _poesHistoLevelBins, int _num)
{
	ASSERT(ppHistoBins == _num);

	float *p0 = _poesHistoLevelBins;
	for (int i = 0; i < _num; i++) {
		for (int j = 0; j < levelTypeCount; j++) {
			*p0++ = snoringPoesHistoBinLevel[i][j];
		}
	}
}

void CEvents::getSnoringPphHistoBodyPosBins(float * _pphHistoBinBodyPos, int _num)
{
	ASSERT(ppHistoBins == _num);

	float *p0 = _pphHistoBinBodyPos;
	for (int i = 0; i < _num; i++) {
		for (int j = 0; j < posCount; j++) {
			*p0++ = snoringPphHistoBinBodyPos[i][j];
		}
	}
}

void CEvents::getSnoringPphHistoLevelBins(float * _pphHistoLevelBins, int _num)
{
	ASSERT(ppHistoBins == _num);

	float *p0 = _pphHistoLevelBins;
	for (int i = 0; i < _num; i++) {
		for (int j = 0; j < levelTypeCount; j++) {
			*p0++ = snoringPphHistoBinLevel[i][j];
		}
	}
}

void CEvents::getPoesHistoBodyPosBins(float *_poesHistoBinBodyPos,int _num)
{
	ASSERT(ppHistoBins == _num);

	float *p0 = _poesHistoBinBodyPos;
	for (int i = 0 ; i < _num ; i++) {
		for (int j = 0 ; j < posCount ; j++) {
			*p0++ = poesHistoBinBodyPos[i][j];
		}
	}
}

void CEvents::getPoesBins(float *_pBin,float *_pCumulBin,int _num)
{
	ASSERT(ppBins == _num);

	float *p0 = _pBin;
	float *p1 = _pCumulBin;
	for (int i = 0 ; i < _num ; i++) {
		*p0++ = poesBin[i];
		*p1++ = poesCumulBin[i];
	}
}

void CEvents::getPphBins(float * _pBin, float * _pCumulBin, int _num)
{
	ASSERT(ppBins == _num);

	float *p0 = _pBin;
	float *p1 = _pCumulBin;
	for (int i = 0; i < _num; i++) {
		*p0++ = pphBin[i];
		*p1++ = pphCumulBin[i];
	}
}

void CEvents::getSnoringPoesBins(float * _poesBin, float * _poesCumulBin, int _num)
{
	ASSERT(ppBins == _num);

	float *p0 = _poesBin;
	float *p1 = _poesCumulBin;
	for (int i = 0; i < _num; i++) {
		*p0++ = snoringPoesBin[i];
		*p1++ = snoringPoesCumulBin[i];
	}
}

void CEvents::getSnoringPphBins(float * _pphBin, float * _pphCumulBin, int _num)
{
	ASSERT(ppBins == _num);

	float *p0 = _pphBin;
	float *p1 = _pphCumulBin;
	for (int i = 0; i < _num; i++) {
		*p0++ = snoringPphBin[i];
		*p1++ = snoringPphCumulBin[i];
	}
}

int CEvents::getNumREBins(void)
{
	return ppBins;
}

SNORING_RESULT CEvents::getSnoringResult(void)
{
	return snoringResult;
}

CString CEvents::secToHHMMSS(int _secs)
{
	CString s;
	int h = (int)((float)_secs / 3600);
	float min = (float)(_secs - h * 3600) / 60.0f;
	float sec = (float)(_secs - h * 3600 - (int)min * 60);
	s.Format(_T("%1d:%02d:%02d"), h, (int)min, (int)sec);
	return s;
}

CString CEvents::secToHHMM(int _secs)
{
	CString s;
	int h = (int)((float)_secs / 3600);
	float min = (float)(_secs - h * 3600) / 60.0f;
	float sec = (float)(_secs - h * 3600 - (int)min * 60);
	min = sec > 30.0f ? min + 1 : min;
	s.Format(_T("%1d:%02d"), h, (int)min);
	return s;
}

CString CEvents::getSnoringTimeHHMM(void)
{
	return secToHHMM((int) snoringResult.totalSnoringTime);
}

RD_RESULTS CEvents::getRDResults(void)
{
	return rdResults[detectionCriteria.hypopneaDropLimit][detectionCriteria.spO2DropLimit];
}

LEVEL_RESULTS CEvents::getLevelResults(void)
{
	return lvlResults[detectionCriteria.hypopneaDropLimit][detectionCriteria.spO2DropLimit];
}

PULSE_RATE_RESULTS CEvents::getPulseRateResults(int _sleepAwakeState)
{
	return pulseRateResults[_sleepAwakeState];
}

SPO2_RESULTS CEvents::getSpO2Results(int _sleepAwakeState)
{
	return spO2Res[detectionCriteria.spO2DropLimit][_sleepAwakeState];
}

CArray <CArousalEvnt *,CArousalEvnt *>			*CEvents::getEventArrayArousal(void)
{
	return arousalEventArrayCurrentP;
}

CArray <CHypoEvnt *,CHypoEvnt *>	*CEvents::getEventArrayHypo(void)
{
	return hypoEventArrayCurrentP;
}

CArray <CObstrEvnt *,CObstrEvnt *>				*CEvents::getEventArrayObstr(void)
{
	return &obstrEventArray;
}

CArray <CCentralEvnt *,CCentralEvnt *>			*CEvents::getEventArrayCentral(void)
{
	return &centralEventArray;
}

CArray <CMixedEvnt *,CMixedEvnt *>	*CEvents::getEventArrayMixed(void)
{
	return &mixedEventArray;
}

CArray <CAwakeEvnt *, CAwakeEvnt *>					*CEvents::getAwakeEventArray(void)
{
	return &awakeEventArray;
}

CArray <CSnoringEvnt *, CSnoringEvnt *>				*CEvents::getSnoringEventArray(void)
{
	return &snoringEventArray;
}

CArray <CManualMarkerEvnt *, CManualMarkerEvnt *>	*CEvents::getManualMarkerEventArray(void)
{
	return &manualMarkerEventArray;
}

CArray <CExcludedEvnt *, CExcludedEvnt *>			*CEvents::getExcludedEventArray(void)
{
	return &excludedEventArray;
}

CArray <CImportedEvnt *, CImportedEvnt *>			*CEvents::getImportedEventArray(void)
{
	return &importedEventArray;
}

void CEvents::deleteEvent(CEvnt *_evP,UINT _eventType)
{
	int cnt = 0;
	switch (_eventType) {
	case evntTypeMixed :
		cnt = mixedEventArray.GetCount();
		for (int i = 0; i < cnt; i++) {
			CMixedEvnt *evP = mixedEventArray.GetAt(i);
			if (_evP == evP) {
				mixedEventArray.RemoveAt(i);
				delete evP;
				break;
			}
		}
		break;
	case evntTypeCentral:
		cnt = centralEventArray.GetCount();
		for (int i = 0; i < cnt; i++) {
			CCentralEvnt *evP = centralEventArray.GetAt(i);
			if (_evP == evP) {
				centralEventArray.RemoveAt(i);
				delete evP;
				break;
			}
		}
		break;
	case evntTypeObstr:
		cnt = obstrEventArray.GetCount();
		for (int i = 0; i < cnt; i++) {
			CObstrEvnt *evP = obstrEventArray.GetAt(i);
			if (_evP == evP) {
				obstrEventArray.RemoveAt(i);
				delete evP;
				break;
			}
		}
		break;
	case evntTypeHypoCentral:
	case evntTypeHypoObstr:
		if (hypoEventArrayCurrentP) {
			cnt = hypoEventArrayCurrentP->GetCount();
			for (int i = 0; i < cnt; i++) {
				CHypoEvnt *evP = hypoEventArrayCurrentP->GetAt(i);
				if (_evP == evP) {
					hypoEventArrayCurrentP->RemoveAt(i);
					delete evP;
					break;
				}
			}
		}
		break;
	case evntTypeSpO2Drop:
		cnt = spO2DropEventArrayCurrentP->GetCount();
		for (int i = 0; i < cnt; i++) {
			CSpO2DropEvnt *evP = spO2DropEventArrayCurrentP->GetAt(i);
			if (_evP == evP) {
				float hypoTime = .0f;
				int hypoNumber = 0;
				bool ok = evP->getHypoPartner(&hypoTime);
				CHypoEvnt *hP = findHypoAtTime(hypoTime,&hypoNumber);
				if (hP) {
					hypoEventArrayCurrentP->RemoveAt(hypoNumber);
					delete hP;
				}
				spO2DropEventArrayCurrentP->RemoveAt(i);
				delete evP;
				break;
			}
		}
		break;
	case evntTypeRERA:
		cnt = arousalEventArrayCurrentP->GetCount();
		for (int i = 0; i < cnt; i++) {
			CArousalEvnt *evP = arousalEventArrayCurrentP->GetAt(i);
			CRERAEvnt *rP = evP->getRERAEvnt();
			if (_evP == rP) {
				evP->removeRERAEvnt();
				break;
			}
		}
		break;
	case evntTypeArousal:
		cnt = arousalEventArrayCurrentP->GetCount();
		for (int i = 0; i < cnt; i++) {
			CArousalEvnt *evP = arousalEventArrayCurrentP->GetAt(i);
			if (_evP == evP) {
				arousalEventArrayCurrentP->RemoveAt(i);
				delete evP;
				break;
			}
		}
		break;
	case evntTypeAwake:
		cnt = awakeEventArray.GetCount();
		for (int i = 0; i < cnt; i++) {
			CAwakeEvnt *evP = awakeEventArray.GetAt(i);
			if (_evP == evP) {
				awakeEventArray.RemoveAt(i);
				delete evP;
				break;
			}
		}
		break;
	case evntTypeSnoring:
		cnt = snoringEventArray.GetCount();
		for (int i = 0; i < cnt; i++) {
			CSnoringEvnt *evP = snoringEventArray.GetAt(i);
			if (_evP == evP) {
				snoringEventArray.RemoveAt(i);
				delete evP;
				break;
			}
		}
		break;
	case evntTypeImported:
		cnt = importedEventArray.GetCount();
		for (int i = 0; i < cnt; i++) {
			CImportedEvnt *evP = importedEventArray.GetAt(i);
			if (_evP == evP) {
				importedEventArray.RemoveAt(i);
				delete evP;
				break;
			}
		}
		break;
	case evntTypeNone:
		break;
	default:
		break;
	}
}

CHypoEvnt *CEvents::findHypoAtTime(float _hypoTime,int *_number)
{
	int num = hypoEventArrayCurrentP->GetCount();
	for (int i = 0; i < num; i++) {
		CHypoEvnt *hP = hypoEventArrayCurrentP->GetAt(i);
		float from = hP->getFrom();
		float to = hP->getTo();
		if ((from < _hypoTime) && (to > _hypoTime)) {
			*_number = i;
			return hP;
		}
	}
	return NULL;
}

float CEvents::findNextEvent(float _cTime,UINT _eventsToShow)
{
	float newCTime = _cTime;
	vector <float> v;

	if (_eventsToShow & evntTypeSpO2Drop	) {
		int num = spO2DropEventArrayCurrentP->GetCount();
		for (int i = 0 ; i < num ; i++) {
			CSpO2DropEvnt *eP = spO2DropEventArrayCurrentP->GetAt(i);
			if (eP->getIsActive()) {
				if (eP->getCentreTime() > newCTime) {
					v.push_back(eP->getCentreTime());
					break;
				}
			}
		}
	}
	if (_eventsToShow & evntTypeHypoCentral) {
		if (hypoEventArrayCurrentP) {
			int num = hypoEventArrayCurrentP->GetCount();
			for (int i = 0; i < num; i++) {
				CHypoEvnt *eP = hypoEventArrayCurrentP->GetAt(i);
				if (eP->getIsActive() && eP->pickFromDropLevel(detectionCriteria.hypopneaDropLimit)) {
					if (eP->getHypoType() == hypoTypeCentral) {
						if (eP->getCentreTime() > newCTime) {
							v.push_back(eP->getCentreTime());
							break;
						}
					}
				}
			}
		}
	}
	if (_eventsToShow & evntTypeHypoObstr	)  {
		if (hypoEventArrayCurrentP) {
			TRACE(_T("\nNEXT\n****************\nArrayP %p\n*****************\n"), hypoEventArrayCurrentP);
			int num = hypoEventArrayCurrentP->GetCount();
			for (int i = 0; i < num; i++) {
				CHypoEvnt *eP = hypoEventArrayCurrentP->GetAt(i);
				
				if (eP->getIsActive() && eP->pickFromDropLevel(detectionCriteria.hypopneaDropLimit)) {
					if (eP->getHypoType() == hypoTypeObstr) {
						if (eP->getCentreTime() > newCTime) {
							v.push_back(eP->getCentreTime());
							break;
						}
					}
				}
			}
		}
	}
	if (_eventsToShow & evntTypeCentral		)  {
		int num = centralEventArray.GetCount();
		for (int i = 0 ; i < num ; i++) {
			CCentralEvnt *eP = centralEventArray.GetAt(i);
			if (eP->getIsActive()) {
				if (eP->getCentreTime() > newCTime) {
					v.push_back(eP->getCentreTime());
					break;
				}
			}
		}
	}
	if (_eventsToShow & evntTypeMixed		)   {
		int num = mixedEventArray.GetCount();
		for (int i = 0 ; i < num ; i++) {
			CMixedEvnt *eP = mixedEventArray.GetAt(i);
			if (eP->getIsActive()) {
				if (eP->getCentreTime() > newCTime) {
					v.push_back(eP->getCentreTime());
					break;
				}
			}
		}
	}
	if (_eventsToShow & evntTypeObstr		) {
		int num = obstrEventArray.GetCount();
		for (int i = 0 ; i < num ; i++) {
			CObstrEvnt *eP = obstrEventArray.GetAt(i);
			if (eP->getIsActive()) {
				if (eP->getCentreTime() > newCTime) {
					v.push_back(eP->getCentreTime());
					break;
				}
			}
		}
	}
	if (_eventsToShow & evntTypeRERA		) {
		if (arousalEventArrayCurrentP) {
			int num = arousalEventArrayCurrentP->GetCount();
			for (int i = 0; i < num; i++) {
				CArousalEvnt *eP = arousalEventArrayCurrentP->GetAt(i);
				CRERAEvnt *rP = eP->getRERAEvnt();
				if (eP->getIsActive(detectionCriteria.hypopneaDropLimit) && rP) {
					if (rP->getCentreTime() > newCTime) {
						v.push_back(rP->getCentreTime());
						break;
					}
				}
			}
		}
	}
	if (_eventsToShow & evntTypeArousal		) {
		if (arousalEventArrayCurrentP) {
			int num = arousalEventArrayCurrentP->GetCount();
			for (int i = 0; i < num; i++) {
				CArousalEvnt *eP = arousalEventArrayCurrentP->GetAt(i);
				if (eP->getIsActive(detectionCriteria.hypopneaDropLimit)) {
					if (eP->getCentreTime() > newCTime) {
						v.push_back(eP->getCentreTime());
						break;
					}
				}
			}
		}
	}
	if (_eventsToShow & evntTypeManMarker	) {
		int num = manualMarkerEventArray.GetCount();
		for (int i = 0 ; i < num ; i++) {
			CManualMarkerEvnt *eP = manualMarkerEventArray.GetAt(i);
			if (eP->getIsActive()) {
				if (eP->getCentreTime() > newCTime) {
					v.push_back(eP->getCentreTime());
					break;
				}
			}
		}
	}
	if (_eventsToShow & evntTypeSnoring		)  {
		int num = snoringEventArray.GetCount();
		for (int i = 0 ; i < num ; i++) {
			CSnoringEvnt *eP = snoringEventArray.GetAt(i);
			if (eP->getIsActive()) {
				if (eP->getCentreTime() > newCTime) {
					v.push_back(eP->getCentreTime());
					break;
				}
			}
		}
	}
	if (_eventsToShow & evntTypeAwake		) {
		int num = awakeEventArray.GetCount();
		for (int i = 0 ; i < num ; i++) {
			CAwakeEvnt *eP = awakeEventArray.GetAt(i);
			if (eP->getIsActive()) {
				if (eP->getCentreTime() > newCTime) {
					v.push_back(eP->getCentreTime());
					break;
				}
			}
		}
	}
	if (_eventsToShow & evntTypeSwallow) {
		int num = swallowEventArray.GetCount();
		for (int i = 0; i < num; i++) {
			CSwallowEvnt *eP = swallowEventArray.GetAt(i);
			if (eP->getIsActive()) {
				if (eP->getCentreTime() > newCTime) {
					v.push_back(eP->getCentreTime());
					break;
				}
			}
		}
	}
	if (_eventsToShow & evntTypeImported) {
		int num = importedEventArray.GetCount();
		for (int i = 0; i < num; i++) {
			CImportedEvnt *eP = importedEventArray.GetAt(i);
			if (eP->getIsActive()) {
				if (eP->getCentreTime() > newCTime) {
					v.push_back(eP->getCentreTime());
					break;
				}
			}
		}
	}
	if (!v.size()) return newCTime;
	sort(v.begin(),v.end());
	newCTime = v.at(0);
	return newCTime;
}

/*
Description: Sets effective stop. 
If <= effective start - changes nothing and returns false
If OK, returns true;
*/
bool CEvents::setEffectiveStop(int _h,int _m,int _sec)
{
	if (_h < 0) return false;
	if (_m < 0) return false;
	if (_sec < 0) return false;
	float t = (float) (_h * 3600 + _m * 60 + _sec);
	
	return setEffectiveStop(t);
}

/*
Description: Sets effective stop.
If <= effective start - changes nothing and returns false
If OK, returns true;
*/
bool CEvents::setEffectiveStop(float _sec)
{
	setExcludedEvent(false, effectiveStopTime, maxStopTime,true,false);

	float t = _sec;
	if (t <= effectiveStartTime) return false;
	t = t > maxStopTime ? maxStopTime : t;

	effectiveStopTime = t;

	setExcludedEvent(true, effectiveStopTime, maxStopTime,false,false);

	postExcludeAnalysis();
	return true;
}


/*
Description: Sets effective start.
If >= effective stop - changes nothing and returns false
If OK, returns true;
*/
bool CEvents::setEffectiveStart(float _sec)
{
	setExcludedEvent(false, .0f, effectiveStartTime,true,false);

	float t = _sec;
	if (t >= effectiveStopTime) return false;
	effectiveStartTime = t;

	setExcludedEvent(true, .0f, effectiveStartTime,false,false);

	postExcludeAnalysis();
	return true;
}

/*
Description: Sets effective start. 
If >= effective stop - changes nothing and returns false
If OK, returns true;
*/
bool CEvents::setEffectiveStart(int _h,int _m,int _sec)
{
	if (_h < 0) return false;
	if (_m < 0) return false;
	if (_sec < 0) return false;
	float t = (float) (_h * 3600 + _m * 60 + _sec);
	return setEffectiveStart(t);
}

COleDateTime CEvents::getStartRecordingClockTime(void)
{
	return startRecordingClockTime;
}
		
/*
Decsription:
	Tests if the time tag is withing an event. Tests also is it is near the edge
_eventsToShow : Flags with which events are shown
_t : The time tag
_edgeDetectionTolerance: Used to mark a time segment near the begin or end of the event - as a fraction of the event length
*_what: Loaded with what object it is
_where: Loaded with where on the object it is
*/
CEvnt *CEvents::getIsOnEvent(int _eventsToShow,float _t,float _edgeDetectTolerance,int *_what,int *_where)
{
	CEvnt *eP = NULL;

#ifdef IN_AGS_BROWSER
	return NULL;
#endif
	
	//---Start with the weakest
	if (_eventsToShow & evntTypeExcluded) {
		int num = excludedEventArray.GetCount();
		for (int i = 0 ; i < num ; ++i) {
			CExcludedEvnt *eP2 = excludedEventArray.GetAt(i);
			if (eP2->getIsInEvent(_t)) {
				*_what = OVER_EVNT_EXCLUDED;
				*_where |= OVER_EVNT;
				*_where |= eP2->getIsInOnEdge(_t, _edgeDetectTolerance);
				eP = (CEvnt *) eP2;
				return eP;
				break;
			}
		}
	}
	if (_eventsToShow & evntTypeManMarker) {
		int num = manualMarkerEventArray.GetCount();
		for (int i = 0 ; i < num ; ++i) {
			CManualMarkerEvnt *eP2 = manualMarkerEventArray.GetAt(i);
			if (eP2->getIsInEvent(_t)) {
				*_what = OVER_EVNT_MAN_MARKER;
				*_where |= OVER_EVNT;
				*_where |= eP2->getIsInOnEdge(_t, _edgeDetectTolerance);
				eP = (CEvnt *) eP2;
				return eP;
				break;
			}
		}
	}
	if (_eventsToShow & evntTypeSnoring) {
		int num = snoringEventArray.GetCount();
		for (int i = 0 ; i < num ; ++i) {
			CSnoringEvnt *eP2 = snoringEventArray.GetAt(i);
			if (eP2->getIsActive()) {
				if (eP2->getIsInEvent(_t)) {
					*_what = OVER_EVNT_SNORING; 
					*_what |= eP2->getLevelAtTimeForMouseover(_t);
					*_where |= OVER_EVNT;
					*_where |= eP2->getIsInOnEdge(_t, _edgeDetectTolerance);
					eP = (CEvnt *)eP2;
					return eP;
					break;
				}
			}
		}
	}
	if (_eventsToShow & evntTypeAwake) {
		int num = awakeEventArray.GetCount();
		for (int i = 0 ; i < num ; ++i) {
			CAwakeEvnt *eP2 = awakeEventArray.GetAt(i);
			if (eP2->getIsActive()) {
				if (eP2->getIsInEvent(_t)) {
					*_what = OVER_EVNT_AWAKE;
					*_where |= OVER_EVNT;
					*_where |= eP2->getIsInOnEdge(_t, _edgeDetectTolerance);
					eP = (CEvnt *)eP2;
					return eP;
					break;
				}
			}
		}
	}
	if (_eventsToShow & evntTypeSpO2Drop) {
		int num = spO2DropEventArrayCurrentP->GetCount();
		for (int i = 0 ; i < num ; ++i) {
			CSpO2DropEvnt *eP2 = spO2DropEventArrayCurrentP->GetAt(i);
			if (eP2->getIsActive()) {
				if (eP2->getIsInEvent(_t)) {
					*_what = OVER_EVNT_SPO2_DROP;
					*_where |= OVER_EVNT;
					*_where |= eP2->getIsInOnEdge(_t, _edgeDetectTolerance);
					eP = (CEvnt *)eP2;
					return eP;
					break;
				}
			}
		}
	}
	if ((_eventsToShow & evntTypeHypoCentral) || (_eventsToShow & evntTypeHypoObstr)) {
		if (hypoEventArrayCurrentP) {
			int num = hypoEventArrayCurrentP->GetCount();
			for (int i = 0; i < num; ++i) {
				CHypoEvnt *eP2 = hypoEventArrayCurrentP->GetAt(i);
				if (eP2->getIsActive()) {
					if (eP2->getIsInEvent(_t)) {
						*_what = OVER_EVNT_HYPOPNEA;
						int hType = eP2->getHypoType();
						if (hType == hypoTypeCentral) *_what |= OVER_EVNT_CENTRALHYPO;
						if (hType == hypoTypeObstr) {
							*_what |= OVER_EVNT_OBSTRUCTIVEHYPO;
							*_what |= eP2->getLevelTypeForMouseOver();
						}
						*_where |= OVER_EVNT;
						*_where |= eP2->getIsInOnEdge(_t, _edgeDetectTolerance);
						eP = (CEvnt *)eP2;
						return eP;
						break;
					}
				}
			}
		}
	}
	if (_eventsToShow & evntTypeObstr) {
		int num = obstrEventArray.GetCount();
		for (int i = 0 ; i < num ; ++i) {
			CObstrEvnt *eP2 = obstrEventArray.GetAt(i);
			if (eP2->getIsActive()) {
				if (eP2->getIsInEvent(_t)) {
					*_what = OVER_EVNT_OBSTRUCTIVE;
					*_what |= eP2->getLevelTypeForMouseOver();
					*_where |= OVER_EVNT;
					*_where |= eP2->getIsInOnEdge(_t, _edgeDetectTolerance);
					eP = (CEvnt *)eP2;
					return eP;
					break;
				}
			}
		}
	}
	if (_eventsToShow & evntTypeCentral) {
		int num = centralEventArray.GetCount();
		for (int i = 0 ; i < num ; ++i) {
			CCentralEvnt *eP2 = centralEventArray.GetAt(i);
			if (eP2->getIsActive()) {
				if (eP2->getIsInEvent(_t)) {
					*_what = OVER_EVNT_CENTRAL;
					*_where |= OVER_EVNT;
					*_where |= eP2->getIsInOnEdge(_t, _edgeDetectTolerance);
					eP = (CEvnt *)eP2;
					return eP;
					break;
				}
			}
		}
	}
	if (_eventsToShow & evntTypeMixed) {
		int num = mixedEventArray.GetCount();
		for (int i = 0 ; i < num ; ++i) {
			CMixedEvnt *eP2 = mixedEventArray.GetAt(i);
			if (eP2->getIsActive()) {
				if (eP2->getIsInEvent(_t)) {
					*_what = OVER_EVNT_MIXED;
					*_what |= eP2->getLevelTypeForMouseOver();
					*_where |= OVER_EVNT;
					*_where |= eP2->getIsInOnEdge(_t, _edgeDetectTolerance);
					eP = (CEvnt *)eP2;
					return eP;
					break;
				}
			}
		}
	}
	if (_eventsToShow & evntTypeRERA) {
		if (arousalEventArrayCurrentP) {
			int num = arousalEventArrayCurrentP->GetCount();
			for (int i = 0; i < num; ++i) {
				CArousalEvnt *eP2 = arousalEventArrayCurrentP->GetAt(i);
				CRERAEvnt *rP = eP2->getRERAEvnt();
				if (eP2->getIsActive(detectionCriteria.hypopneaDropLimit) && rP) {
					if (rP->getIsInEvent(_t)) {
						*_what = OVER_EVNT_RERA;
						*_what |= rP->getLevelTypeForMouseOver();
						*_where |= OVER_EVNT;
						*_where |= rP->getIsInOnEdge(_t, _edgeDetectTolerance);
						eP = (CEvnt *)rP;
						return eP;
						break;
					}
				}
			}
		}
	}
	if (_eventsToShow & evntTypeArousal) {
		if (arousalEventArrayCurrentP) {
			int num = arousalEventArrayCurrentP->GetCount();
			for (int i = 0; i < num; ++i) {
				CArousalEvnt *eP2 = arousalEventArrayCurrentP->GetAt(i);
				if (eP2->getIsActive(detectionCriteria.hypopneaDropLimit)) {
					if (eP2->getIsInEvent(_t)) {
						*_what = OVER_EVNT_AROUSAL;
						*_where |= OVER_EVNT;
						*_where |= eP2->getIsInOnEdge(_t, _edgeDetectTolerance);
						eP = (CEvnt *)eP2;
						return eP;
						break;
					}
				}
			}
		}
	}
	if (_eventsToShow & evntTypeSwallow) {
		int num = swallowEventArray.GetCount();
		for (int i = 0; i < num; ++i) {
			CSwallowEvnt *eP2 = swallowEventArray.GetAt(i);
			if (eP2->getIsActive()) {
				if (eP2->getIsInEvent(_t)) {
					*_what = OVER_EVNT_SWALLOW;
					*_where |= OVER_EVNT;
					*_where |= eP2->getIsInOnEdge(_t, _edgeDetectTolerance);
					eP = (CEvnt *)eP2;
					return eP;
					break;
				}
			}
		}
	}
	if (eP && (eP->getIsActive(detectionCriteria.hypopneaDropLimit))) {
		*_what |= OVER_EVNT;
		if (eP->getHasLevelDetection()) {
			*_what |= eP->getLevelTypeForMouseOver();
		}
		*_where |= OVER_EVNT;
		*_where |= eP->getIsInOnEdge(_t,_edgeDetectTolerance);
		return eP;
	}
	return NULL;
}

/*
Description: Used for tooltip during event editing
*/
bool CEvents::getIsOnEventDuringVisualEventEditing(int _eventsToShow,float _t, CString *_descr, 
	CString *_lvl, float *_start, float *_stop)
{
	size_t sample = (int) (EVENT_TIMELINE_TIME_INTERVAL_INVERSE * _t);
	if (sample < 0) return false;
	if (sample >= editEventTimelineVector.size()) return false;
	UINT val = editEventTimelineVector.at(sample);

	vector <UINT>::iterator f1,f2,v;
	v = editEventTimelineVector.begin();
	v += sample;

	if (!(val & _eventsToShow)) return false;

	CString s;
	if (_eventsToShow & evntTypeAH) {
		if (val & evntTypeMixed) {
			f1 = find_if_not(v, editEventTimelineVector.end(), isMixedNotAwakeNotExcluded);
			f2 = find_if_not_backw(v, editEventTimelineVector.begin(), isMixedNotAwakeNotExcluded);
			*_start = EVENT_TIMELINE_TIME_INTERVAL * distance(editEventTimelineVector.begin(), f2);
			*_stop = EVENT_TIMELINE_TIME_INTERVAL * distance(editEventTimelineVector.begin(), f1);
			int num = _descr->LoadString(IDS_MIXED4);
			if (val & evntAHLevelUpper)			num = s.LoadString(IDS_UPPER2);
			else if (val & evntAHLevelLower)	num = s.LoadString(IDS_LOWER2);
			else if (val & evntAHLevelMulti)	num = s.LoadString(IDS_MULTILEVEL2);
			else if (val & evntAHLevelUndef)	num = s.LoadString(IDS_UNDEF2);
			if (!s.IsEmpty()) {
				int num = _lvl->LoadString(IDS_LEVEL2);
				*_lvl += _T(": ");
				*_lvl += s;
			}
			return true;
		}
		else if (val & evntTypeCentral) {
			f1 = find_if_not(v, editEventTimelineVector.end(), isCentralNotAwakeNotExcluded);
			f2 = find_if_not_backw(v, editEventTimelineVector.begin(), isCentralNotAwakeNotExcluded);
			*_start = EVENT_TIMELINE_TIME_INTERVAL * distance(editEventTimelineVector.begin(), f2);
			*_stop = EVENT_TIMELINE_TIME_INTERVAL * distance(editEventTimelineVector.begin(), f1);
			int num = _descr->LoadString(IDS_CENTRAL4);
			return true;

		}
		else if (val & evntTypeObstr) {
			f1 = find_if_not(v, editEventTimelineVector.end(), isObstrNotAwakeNotExcluded);
			f2 = find_if_not_backw(v, editEventTimelineVector.begin(), isObstrNotAwakeNotExcluded);
			*_start = EVENT_TIMELINE_TIME_INTERVAL * distance(editEventTimelineVector.begin(), f2);
			*_stop = EVENT_TIMELINE_TIME_INTERVAL * distance(editEventTimelineVector.begin(), f1);
			int num = _descr->LoadString(IDS_OBSTR4);
			if (val & evntAHLevelUpper)			num = s.LoadString(IDS_UPPER2);
			else if (val & evntAHLevelLower)	num = s.LoadString(IDS_LOWER2);
			else if (val & evntAHLevelMulti)	num = s.LoadString(IDS_MULTILEVEL2);
			else if (val & evntAHLevelUndef)	num = s.LoadString(IDS_UNDEF2);
			if (!s.IsEmpty()) {
				int num = _lvl->LoadString(IDS_LEVEL2);
				*_lvl += _T(": ");
				*_lvl += s;
			}
			return true;

		}
		else if (val & evntTypeHypoCentral) {
			f1 = find_if_not(v, editEventTimelineVector.end(), isHypoCentralNotAwakeNotExcluded);
			f2 = find_if_not_backw(v, editEventTimelineVector.begin(), isHypoCentralNotAwakeNotExcluded);
			*_start = EVENT_TIMELINE_TIME_INTERVAL * distance(editEventTimelineVector.begin(), f2);
			*_stop = EVENT_TIMELINE_TIME_INTERVAL * distance(editEventTimelineVector.begin(), f1);
			int num = _descr->LoadString(IDS_CENTRALHYPO4);
			return true;
		}
		else if (val & evntTypeHypoObstr) {
			f1 = find_if_not(v, editEventTimelineVector.end(), isHypoObstrNotAwakeNotExcluded);
			f2 = find_if_not_backw(v, editEventTimelineVector.begin(), isHypoObstrNotAwakeNotExcluded);
			*_start = EVENT_TIMELINE_TIME_INTERVAL * distance(editEventTimelineVector.begin(), f2);
			*_stop = EVENT_TIMELINE_TIME_INTERVAL * distance(editEventTimelineVector.begin(), f1);
			int num = _descr->LoadString(IDS_OBSTRHYPO4);
			if (val & evntAHLevelUpper)			num = s.LoadString(IDS_UPPER2);
			else if (val & evntAHLevelLower)	num = s.LoadString(IDS_LOWER2);
			else if (val & evntAHLevelMulti)	num = s.LoadString(IDS_MULTILEVEL2);
			else if (val & evntAHLevelUndef)	num = s.LoadString(IDS_UNDEF2);
			if (!s.IsEmpty()) {
				int num = _lvl->LoadString(IDS_LEVEL2);
				*_lvl += _T(": ");
				*_lvl += s;
			}
			return true;
		}
		return false;
	}


	if (_eventsToShow & evntTypeRERA) {
		if (val & evntTypeRERA) {
			f1 = find_if_not(v, editEventTimelineVector.end(), isRERANotAwakeNotExcluded);
			f2 = find_if_not_backw(v, editEventTimelineVector.begin(), isRERANotAwakeNotExcluded);
			*_start = EVENT_TIMELINE_TIME_INTERVAL * distance(editEventTimelineVector.begin(), f2);
			*_stop = EVENT_TIMELINE_TIME_INTERVAL * distance(editEventTimelineVector.begin(), f1);
			int num = _descr->LoadString(IDS_RERA4);
			if (val & evntAHLevelUpper)			num = s.LoadString(IDS_UPPER2);
			else if (val & evntAHLevelLower)	num = s.LoadString(IDS_LOWER2);
			else if (val & evntAHLevelMulti)	num = s.LoadString(IDS_MULTILEVEL2);
			else if (val & evntAHLevelUndef)	num = s.LoadString(IDS_UNDEF2);
			if (!s.IsEmpty()) {
				num = _lvl->LoadString(IDS_LEVEL2);
				*_lvl += _T(": ");
				*_lvl += s;
			}
			return true;
		}
		return false;
	}
	
	if (_eventsToShow & evntTypeSpO2Drop) {
		if (val & evntTypeSpO2Drop) {
			f1 = find_if_not(v, editEventTimelineVector.end(), isDesaturationNotAwakeNotExcluded);
			f2 = find_if_not_backw(v, editEventTimelineVector.begin(), isDesaturationNotAwakeNotExcluded);
			*_start = EVENT_TIMELINE_TIME_INTERVAL * distance(editEventTimelineVector.begin(), f2);
			*_stop = EVENT_TIMELINE_TIME_INTERVAL * distance(editEventTimelineVector.begin(), f1);
			int num = _descr->LoadString(IDS_SPO24);
			return true;
		}
		return false;
	}

	if (_eventsToShow & evntTypeSnoring) {
		if (val & evntTypeSnoring) {
			f1 = find_if_not(v, editEventTimelineVector.end(), isSnoringNotAwakeNotExcluded);
			f2 = find_if_not_backw(v, editEventTimelineVector.begin(), isSnoringNotAwakeNotExcluded);
			*_start = EVENT_TIMELINE_TIME_INTERVAL * distance(editEventTimelineVector.begin(), f2);
			*_stop = EVENT_TIMELINE_TIME_INTERVAL * distance(editEventTimelineVector.begin(), f1);
			int num = _descr->LoadString(IDS_SNORING4);
			if (val & evntSnoringLevelUpper)		num = s.LoadString(IDS_UPPER2);
			else if (val & evntSnoringLevelLower)	num = s.LoadString(IDS_LOWER2);
			else if (val & evntSnoringLevelMulti)	num = s.LoadString(IDS_MULTILEVEL2);
			else if (val & evntSnoringLevelUndef)	num = s.LoadString(IDS_UNDEF2);
			if (!s.IsEmpty()) {
				num = _lvl->LoadString(IDS_LEVEL2);
				*_lvl += _T(": ");
				*_lvl += s;
			}
			return true;
		}
		return false;
	}

	if (_eventsToShow & evntTypeAwake) {
		if (val & evntTypeAwake) {
			f1 = find_if_not(v, editEventTimelineVector.end(), isAwakeNotExcluded);
			f2 = find_if_not_backw(v, editEventTimelineVector.begin(), isAwakeNotExcluded);
			*_start = EVENT_TIMELINE_TIME_INTERVAL * distance(editEventTimelineVector.begin(), f2);
			*_stop = EVENT_TIMELINE_TIME_INTERVAL * distance(editEventTimelineVector.begin(), f1);
			int num = _descr->LoadString(IDS_AWAKE4);
			return true;
		}
		return false;
	}
	
	if (_eventsToShow & evntTypeManMarker) {
		if (val & evntTypeManMarker) {
			f1 = find_if_not(v, editEventTimelineVector.end(), isManualNotExcluded);
			f2 = find_if_not_backw(v, editEventTimelineVector.begin(), isManualNotExcluded);
			*_start = EVENT_TIMELINE_TIME_INTERVAL * distance(editEventTimelineVector.begin(), f2);
			*_stop = EVENT_TIMELINE_TIME_INTERVAL * distance(editEventTimelineVector.begin(), f1);
			int num = _descr->LoadString(IDS_MANUA_MARKER);
			return true;
		}
		return false;
	}

	if (_eventsToShow & evntTypeSwallow) {
		if (val & evntTypeSwallow) {
			f1 = find_if_not(v, editEventTimelineVector.end(), isSwallowNotExcluded);
			f2 = find_if_not_backw(v, editEventTimelineVector.begin(), isSwallowNotExcluded);
			*_start = EVENT_TIMELINE_TIME_INTERVAL * distance(editEventTimelineVector.begin(), f2);
			*_stop = EVENT_TIMELINE_TIME_INTERVAL * distance(editEventTimelineVector.begin(), f1);
			int num = _descr->LoadString(IDS_SWALLOW2);
			return true;
		}
		return false;
	}
	
	if (_eventsToShow & evntTypeImported) {
		if (val & evntTypeImported) {
			f1 = find_if_not(v, editEventTimelineVector.end(), isImportedNotExcluded);
			f2 = find_if_not_backw(v, editEventTimelineVector.begin(), isImportedNotExcluded);
			*_start = EVENT_TIMELINE_TIME_INTERVAL * distance(editEventTimelineVector.begin(), f2);
			*_stop = EVENT_TIMELINE_TIME_INTERVAL * distance(editEventTimelineVector.begin(), f1);
			int num = _descr->LoadString(IDS_IMPORTED4);
			return true;
		}
		return false;
	}

	if (_eventsToShow & evntTypeExcluded) {
		if (val & evntTypeExcluded) {
			f1 = find_if_not(v, editEventTimelineVector.end(), isExcludedNotAwake);
			f2 = find_if_not_backw(v, editEventTimelineVector.begin(), isExcludedNotAwake);
			*_start = EVENT_TIMELINE_TIME_INTERVAL * distance(editEventTimelineVector.begin(), f2);
			*_stop = EVENT_TIMELINE_TIME_INTERVAL * distance(editEventTimelineVector.begin(), f1);
			int num = _descr->LoadString(IDS_EXCLUDED4);
			return true;
		}
		return false;
	}
	
	return false;
}

CString CEvents::getTimeOfDay(float _t)
{
	COleDateTime dt = startRecordingClockTime;
	COleDateTimeSpan span(0, 0, 0, (int) _t);
	CString s0 = dt.Format(_T("%H:%M:%S"));
	dt += span;
	CString s = dt.Format(_T("%H:%M:%S"));
	return s;
}

/*
Description: Check if a time value is on an event in the starts-stops vector.
_t: the time value to test
_edgeDetectTolerance: The tolerance (0 < tolerance < 1) related to the event length (+/- half the tolerance) used to detect edges
_what : what is hit
_where: Where is it hit, on or also on left or right
_levelType: Current leel type is returned here
_startTime : If a pointer is provided, the start time of the event is returned here
_stopTime : If a pointer is provided, the stop time of the event is returned here
Returns: The event type
*/
UINT CEvents::getIsOnEventVisualEditing(float _t, float _edgeDetectTolerance, int * _what, int * _where,
	UINT *_levelType,
	float *_startTime /* = NULL*/, float *_stopTime /*= NULL*/)
{
	*_where = OVER_NOTHING;
	*_what = OVER_NOTHING;

	int index = (int) (_t / EVENT_TIMELINE_TIME_INTERVAL);
	if (index <= 0) return evntTypeNone;
	if ((UINT) index >= editEventTimelineVector.size()) return evntTypeNone;

	vector <UINT>::iterator iT = editEventTimelineVector.begin();
	iT += index;
	UINT onEventType = *iT & eventsBeingVisuallyEdited;
	if (!onEventType) return evntTypeNone;

	//---Fetch the current level type
	if (onEventType & evntTypeSnoring) {
		*_levelType = *iT & evntSnoringLevelLower ? levelTypeLower : levelTypeUndef;
		*_levelType = *iT & evntSnoringLevelUpper ? levelTypeUpper : *_levelType;
		*_levelType = *iT & evntSnoringLevelMulti ? levelTypeMulti : *_levelType;
	}
	else {
		*_levelType = *iT & evntAHLevelLower ? levelTypeLower : levelTypeUndef;
		*_levelType = *iT & evntAHLevelUpper ? levelTypeUpper : *_levelType;
		*_levelType = *iT & evntAHLevelMulti ? levelTypeMulti : *_levelType;
	}
	
	vector <UINT>::iterator iT0, iT1;
	iT0 = iT1 = iT;
	while ((iT1 < editEventTimelineVector.end()) && (*iT1 & eventsBeingVisuallyEdited)) ++iT1;
	while ((*iT0 & eventsBeingVisuallyEdited) && (iT0 > editEventTimelineVector.begin())) --iT0;
	float start = EVENT_TIMELINE_TIME_INTERVAL * distance(editEventTimelineVector.begin(),iT0);
	float stop = EVENT_TIMELINE_TIME_INTERVAL * distance(editEventTimelineVector.begin(), iT1);
	float length = stop - start;

	float marginL = length * _edgeDetectTolerance;
	if ((_t <= (start + marginL)) && (_t >= start)) {
		*_what = OVER_EVNT;
		*_where |= OVER_LEFT_EDGE;
		if (_startTime) *_startTime = start;
		if (_stopTime) *_stopTime = stop;
	}
	else if ((_t <= stop) && (_t >= (stop - marginL))) {
		*_what = OVER_EVNT;
		*_where |= OVER_RIGHT_EDGE;
		if (_startTime) *_startTime = start;
		if (_stopTime) *_stopTime = stop;
	}
	else if ((_t >= start) && (_t <= stop)) {
		*_what = OVER_EVNT;
		*_where = OVER_EVNT;
		if (_startTime) *_startTime = start;
		if (_stopTime) *_stopTime = stop;
	}
	return onEventType;
}

/*
Description: Check if a time value is on an event in the starts-stops vector, 
			where the input parameter is a range defined by leftlimit and rightLimit
_leftLim: Left limit of range
_rightLim: Right limit of range
_what : what is hit
_where: Where is it hit, on or also on left or right
_levelType: Current leel type is returned here
_startTime : If a pointer is provided, the start time of the event is returned here
_stopTime : If a pointer is provided, the stop time of the event is returned here
Returns: The event type

Also sets the visual event structure (or resets)
*/
UINT CEvents::getIsOnEventVisualEditing_Range(const float _leftLim, const float _rightLim, UINT * _what, UINT * _where, UINT * _levelType, float * _startTime, float * _stopTime)
{
	*_where = OVER_NOTHING;
	*_what = OVER_NOTHING;

	int firstCursor = (int)(_leftLim / EVENT_TIMELINE_TIME_INTERVAL);
	firstCursor = firstCursor < 0 ? 0 : firstCursor;
	firstCursor = firstCursor >= (int) editEventTimelineVector.size() ? (int) editEventTimelineVector.size() : firstCursor;

	int lastCursor = (int)(_rightLim / EVENT_TIMELINE_TIME_INTERVAL);
	lastCursor = lastCursor < 0 ? 0 : lastCursor;
	lastCursor = lastCursor >= (int) editEventTimelineVector.size() ? (int) editEventTimelineVector.size() : lastCursor;
	
	if (firstCursor >= lastCursor) return evntTypeNone;
	
	vector <UINT>::iterator iT0 = editEventTimelineVector.begin();
	iT0 += firstCursor;
	vector <UINT>::iterator iT1 = editEventTimelineVector.begin();
	iT1 += lastCursor;

	UINT evntType = evntTypeNone;

	bool(*func) (UINT);
	func = getEventTestVectorPredicate(eventsBeingVisuallyEdited);
	if (!func) return evntType;

	vector <UINT>::iterator i0 = find_if(iT0, iT1, func);
	if (i0 == iT1) return evntType;
		
	if (i0 < iT1) {
		*_what = OVER_EVNT;
		*_where = OVER_EVNT;
		evntType = *i0 & eventsBeingVisuallyEdited;  // Has to satisfy both
		vector <UINT>::iterator i1 = find_if_not(i0, iT1, func);

		//---Fetch the current level type
		if (evntType & evntTypeSnoring) {
			UINT lvl;
			lvl = *i0 & evntSnoringLevelLower ? levelTypeLower : levelTypeUndef;
			lvl = *i0 & evntSnoringLevelUpper ? levelTypeUpper : lvl;
			lvl = *i0 & evntSnoringLevelMulti ? levelTypeMulti : lvl;
			*_levelType = lvl; 
		}
		else {
			UINT lvl;
			lvl = *i0 & evntAHLevelLower ? levelTypeLower : levelTypeUndef;
			lvl = *i0 & evntAHLevelUpper ? levelTypeUpper : lvl;
			lvl = *i0 & evntAHLevelMulti ? levelTypeMulti : lvl;
			*_levelType = lvl; 
		}
		if (i1 < iT1) *_where |= OVER_RIGHT_EDGE;
	}
	if (iT0 < i0) *_where |= OVER_LEFT_EDGE;

	if (evntTypeNone != evntType) {
		vector <UINT>::iterator iT0, iT1;
		iT0 = iT1 = i0;
		while ((iT1 < editEventTimelineVector.end()) && (*iT1 & evntType)) ++iT1;
		while ((*iT0 & evntType) && (iT0 > editEventTimelineVector.begin())) --iT0;
		float startEvent = EVENT_TIMELINE_TIME_INTERVAL * distance(editEventTimelineVector.begin(), iT0);
		float stopEvent = EVENT_TIMELINE_TIME_INTERVAL * distance(editEventTimelineVector.begin(), iT1);

		if (_startTime) *_startTime = startEvent;
		if (_stopTime) *_stopTime = stopEvent;

		//---Overrule if pointing to centre 50% of the event
		float centreMouse = (_leftLim + _rightLim) / 2.0f;
		float centreEvent = (startEvent + stopEvent) / 2.0f;
		float delta35perc = (stopEvent - startEvent) * .35f;
		if ((centreMouse < (centreEvent + delta35perc)) && (centreMouse > (centreEvent - delta35perc))) 
			*_where = OVER_EVNT;

		eventByMouse.from = startEvent;
		eventByMouse.to = stopEvent;
		eventByMouse.gripWhat = *_what;
		eventByMouse.gripWhere = *_where;
		eventByMouse.level = *_levelType;
		eventByMouse.type = evntType;
	}
	else {
		eventByMouse.from = .0f;
		eventByMouse.to = .0f;
		eventByMouse.gripWhat = 0;
		eventByMouse.gripWhere = 0;
		eventByMouse.level = levelTypeNone;
		eventByMouse.type = evntTypeNone;
	}
	
	return evntType;
}

float CEvents::getEffectiveStart(void)
{
	return effectiveStartTime;
}

float CEvents::getEffectiveStop(void)
{
	return effectiveStopTime;
}

float CEvents::findPrevEvent(float _cTime,UINT _eventsToShow)
{
	float newCTime = _cTime;
	vector <float> v;
	
	if (_eventsToShow & evntTypeSpO2Drop	) {
		int num = spO2DropEventArrayCurrentP->GetCount();
		for (int i = num - 1 ; i >= 0 ; i--) {
			CSpO2DropEvnt *eP = spO2DropEventArrayCurrentP->GetAt(i);
			if (eP->getIsActive()) {
				if (eP->getCentreTime() < newCTime) {
					v.push_back(eP->getCentreTime());
					break;
				}
			}
		}
	}
	if (_eventsToShow & evntTypeHypoCentral	)  {
		if (hypoEventArrayCurrentP) {
			int num = hypoEventArrayCurrentP->GetCount();
			for (int i = num - 1; i >= 0; i--) {
				CHypoEvnt *eP = hypoEventArrayCurrentP->GetAt(i);
				if (eP->getIsActive() && eP->pickFromDropLevel(detectionCriteria.hypopneaDropLimit)) {
					if (eP->getHypoType() == hypoTypeCentral) {
						if (eP->getCentreTime() < newCTime) {
							v.push_back(eP->getCentreTime());
							break;
						}
					}
				}
			}
		}
	}
	if (_eventsToShow & evntTypeHypoObstr	)  {
		if (hypoEventArrayCurrentP) {
			int num = hypoEventArrayCurrentP->GetCount();
			for (int i = num - 1; i >= 0; i--) {
				CHypoEvnt *eP = hypoEventArrayCurrentP->GetAt(i);
				
				if (eP->getIsActive() && eP->pickFromDropLevel(detectionCriteria.hypopneaDropLimit)) {
					if (eP->getHypoType() == hypoTypeObstr) {
						if (eP->getCentreTime() < newCTime) {
							v.push_back(eP->getCentreTime());
							break;
						}
					}
				}
			}
		}
	}
	if (_eventsToShow & evntTypeCentral		)  {
		int num = centralEventArray.GetCount();
		for (int i = num - 1 ; i >= 0 ; i--) {
			CCentralEvnt *eP = centralEventArray.GetAt(i);
			if (eP->getIsActive()) {
				if (eP->getCentreTime() < newCTime) {
					v.push_back(eP->getCentreTime());
					break;
				}
			}
		}
	}
	if (_eventsToShow & evntTypeMixed		)   {
		int num = mixedEventArray.GetCount();
		for (int i = num - 1 ; i >= 0 ; i--) {
			CMixedEvnt *eP = mixedEventArray.GetAt(i);
			if (eP->getIsActive()) {
				if (eP->getCentreTime() < newCTime) {
					v.push_back(eP->getCentreTime());
					break;
				}
			}
		}
	}
	if (_eventsToShow & evntTypeObstr		) {
		int num = obstrEventArray.GetCount();
		for (int i = num - 1 ; i >= 0 ; i--) {
			CObstrEvnt *eP = obstrEventArray.GetAt(i);
			if (eP->getIsActive()) {
				if (eP->getCentreTime() < newCTime) {
					v.push_back(eP->getCentreTime());
					break;
				}
			}
		}
	}
	if (_eventsToShow & evntTypeRERA		) {
		if (arousalEventArrayCurrentP) {
			int num = arousalEventArrayCurrentP->GetCount();
			for (int i = num - 1; i >= 0; i--) {
				CArousalEvnt *eP = arousalEventArrayCurrentP->GetAt(i);
				if (eP->getIsActive(detectionCriteria.hypopneaDropLimit)) {
					CRERAEvnt *rP = eP->getRERAEvnt();
					if (rP) {
						if (rP->getCentreTime() < newCTime) {
							v.push_back(rP->getCentreTime());
							break;
						}
					}
				}
			}
		}
	}
	if (_eventsToShow & evntTypeArousal	) {
		if (arousalEventArrayCurrentP) {
			int num = arousalEventArrayCurrentP->GetCount();
			for (int i = num - 1; i >= 0; i--) {
				CArousalEvnt *eP = arousalEventArrayCurrentP->GetAt(i);
				if (eP->getIsActive(detectionCriteria.hypopneaDropLimit)) {
					if (eP->getCentreTime() < newCTime) {
						v.push_back(eP->getCentreTime());
						break;
					}
				}
			}
		}
	}
	if (_eventsToShow & evntTypeManMarker	) {
		int num = manualMarkerEventArray.GetCount();
		for (int i = num - 1 ; i >= 0 ; i--) {
			CManualMarkerEvnt *eP = manualMarkerEventArray.GetAt(i);
			if (eP->getCentreTime() < newCTime) {
				v.push_back(eP->getCentreTime());
				break;
			}
		}
	}
	if (_eventsToShow & evntTypeSnoring		)  {
		int num = snoringEventArray.GetCount();
		for (int i = num - 1 ; i >= 0 ; i--) {
			CSnoringEvnt *eP = snoringEventArray.GetAt(i);
			if (eP->getIsActive()) {
				if (eP->getCentreTime() < newCTime) {
					v.push_back(eP->getCentreTime());
					break;
				}
			}
		}
	}
	if (_eventsToShow & evntTypeAwake		) {
		int num = awakeEventArray.GetCount();
		for (int i = num - 1 ; i >= 0 ; i--) {
			CAwakeEvnt *eP = awakeEventArray.GetAt(i);
			if (eP->getIsActive()) {
				if (eP->getCentreTime() < newCTime) {
					v.push_back(eP->getCentreTime());
					break;
				}
			}
		}
	}
	if (_eventsToShow & evntTypeSwallow) {
		int num = swallowEventArray.GetCount();
		for (int i = num - 1; i >= 0; i--) {
			CSwallowEvnt *eP = swallowEventArray.GetAt(i);
			if (eP->getIsActive()) {
				if (eP->getCentreTime() < newCTime) {
					v.push_back(eP->getCentreTime());
					break;
				}
			}
		}
	}
	if (_eventsToShow & evntTypeImported) {
		int num = importedEventArray.GetCount();
		for (int i = num - 1; i >= 0; i--) {
			CImportedEvnt *eP = importedEventArray.GetAt(i);
			if (eP->getIsActive()) {
				if (eP->getCentreTime() < newCTime) {
					v.push_back(eP->getCentreTime());
					break;
				}
			}
		}
	}

	if (!v.size()) return newCTime;
	sort(v.begin(),v.end(), fltGreater);
	newCTime = v.at(0);
	return newCTime;
}

void CEvents::actOnEventLengthChange(void)
{
	
	fillInEventTimelineVector();	// ok
	doBodyPosAnalysis(detectionCriteria.hypopneaDropLimit, detectionCriteria.spO2DropLimit);			// ok
	doOverallLevelSumUp(detectionCriteria.hypopneaDropLimit,detectionCriteria.spO2DropLimit);			// ok
	doIndexAnalysis(detectionCriteria.hypopneaDropLimit, detectionCriteria.spO2DropLimit);				// ok
	fillStartsAndStopsVector();		// ok
	eventDefinition |= (EVNT_DEF_MANUAL_LENGTH | EVNT_DEF_MANUAL) ;
}

void CEvents::actOnEventChange(void)
{
	AfxMessageBox(_T("actOnEventChange not done yet!"));
}

void CEvents::computeTotalSleepTime(void)
{
	ASSERT(eventTimelineVector.size());

	int count = count_if(eventTimelineVector.begin(), eventTimelineVector.end(), isAsleep);
	totalSleepTime = (float) count * EVENT_TIMELINE_TIME_INTERVAL;
}

void CEvents::fillInEventTimelineVector(void)
{
	ASSERT(eventTimelineVector.size());

	//---Fill in eventTimelineVector
		//float supt, leftt, rightt, pronet, uprt; /*INN*/
		//supt = leftt = rightt = pronet = uprt = .0f; /*INN*/
	transform(eventTimelineVector.begin(), eventTimelineVector.end(), eventTimelineVector.begin(), setToEventTypeNone);
	if (bpData) {
		vector <FLOAT> *tV = bpData->getTimeaxisVector();
		vector <FLOAT> *bpV = bpData->getBpVector();
		vector <FLOAT>::iterator iTt0,iTt1,iTb0;
		vector <UINT>::iterator etlV0, etlV1;
		iTb0 = bpV->begin();
		for (iTt0 = tV->begin(); iTt0 < tV->end(); ++iTt0, ++iTb0) {
			iTt1 = iTt0;
			iTt1++;
			if (iTt1 < tV->end()) {
				etlV0 = eventTimelineVector.begin() + (int) (*iTt0 / EVENT_TIMELINE_TIME_INTERVAL);
				etlV1 = eventTimelineVector.begin() + (int) (*iTt1 / EVENT_TIMELINE_TIME_INTERVAL);
				short cPos = (short) *iTb0; 
				
				switch (cPos) {
				case posSupine:
					transform(etlV0, etlV1, etlV0,orBodyPosSwitchSupine);
					break;
				case posLeft:
					transform(etlV0, etlV1, etlV0, orBodyPosSwitchLeft);
					break;
				case posRight:
					transform(etlV0, etlV1, etlV0, orBodyPosSwitchRight);
					break;
				case posProne:
					transform(etlV0, etlV1, etlV0, orBodyPosSwitchProne);
					break;
				case posUpright:
					transform(etlV0, etlV1, etlV0, orBodyPosSwitchUpright);
					break;
				case posUndefined:
				default:
					break;
				}
			}
			iTt0++;
			iTb0++;
		}
	}
	
	debugEventTimeLineVector(_T("After settoeventtypenone"));

	int num = 0;
	
	num = obstrEventArray.GetCount();
	for (int i = 0; i < num; i++) {
		CObstrEvnt *eP = obstrEventArray.GetAt(i);
		if (eP->getIsActive())
			fillInEventTimeLineVectorForType(eP->getEventType(), eP->getLevelType(),eP->getFrom(), eP->getTo());
	}
	num = centralEventArray.GetCount();
	for (int i = 0; i < num; i++) {
		CCentralEvnt *eP = centralEventArray.GetAt(i);
		if (eP->getIsActive())
			fillInEventTimeLineVectorForType(eP->getEventType(), eP->getLevelType(), eP->getFrom(), eP->getTo());
	}
	num = mixedEventArray.GetCount();
	for (int i = 0; i < num; i++) {
		CMixedEvnt *eP = mixedEventArray.GetAt(i);
		if (eP->getIsActive())
			fillInEventTimeLineVectorForType(eP->getEventType(), eP->getLevelType(), eP->getFrom(), eP->getTo());
	}
	num = awakeEventArray.GetCount();
	for (int i = 0; i < num; i++) {
		CAwakeEvnt *eP = awakeEventArray.GetAt(i);
		if (eP->getIsActive())
			fillInEventTimeLineVectorForType(eP->getEventType(), eP->getLevelType(), eP->getFrom(), eP->getTo());
	}
	num = snoringEventArray.GetCount();
	for (int i = 0; i < num; i++) {
		CSnoringEvnt *eP = snoringEventArray.GetAt(i);
		if (eP->getIsActive()) {

			fillInEventTimeLineVectorForType(eP->getEventType(), levelTypeUndef, eP->getFrom(),eP->getTo());
			
			vector <FLOAT>::iterator i0, i1,last;
			vector <FLOAT> *low = eP->getLowerLevelStartStopVector();
			if (low->size() >= 2) {
				ASSERT(low->size() % 2 == 0);
				i0 = low->begin();
				i1 = i0;
				i1++;
				while (i1 < low->end()) {
					fillInEventTimeLineVectorForType(eP->getEventType(), levelTypeLower, *i0,*i1);
					if (i1 < low->end()) {
						++i1;
						++i0; 
						if (i1 < low->end()) {
							++i1;
							++i0;
						}
					}
				}
			}
			vector <FLOAT> *upper = eP->getUpperLevelStartStopVector();
			if (upper->size() >= 2) {
				ASSERT(upper->size() % 2 == 0);
				i0 = upper->begin();
				i1 = i0;
				i1++;
				while (i1 < upper->end()) {
					fillInEventTimeLineVectorForType(eP->getEventType(), levelTypeUpper, *i0, *i1);
					if (i1 < upper->end()) {
						++i1;
						++i0;
						if (i1 < upper->end()) {
							++i1;
							++i0;
						}
					}
				}
			}

			vector <FLOAT> *multi = eP->getMultiLevelStartStopVector();
			if (multi->size() >= 2) {
				ASSERT(multi->size() % 2 == 0);
				i0 = multi->begin();
				i1 = i0;
				i1++;
				while (i1 < multi->end()) {
					fillInEventTimeLineVectorForType(eP->getEventType(), levelTypeMulti, *i0, *i1);
					if (i1 < multi->end()) {
						++i1;
						++i0;
						if (i1 < multi->end()) {
							++i1;
							++i0;
						}
					}
				}
			}
		}
	}
	num = manualMarkerEventArray.GetCount();
	for (int i = 0; i < num; i++) {
		CManualMarkerEvnt *eP = manualMarkerEventArray.GetAt(i);
		if (eP->getIsActive())
			fillInEventTimeLineVectorForType(eP->getEventType(), eP->getLevelType(), eP->getFrom(), eP->getTo());
	}
	num = excludedEventArray.GetCount();
	for (int i = 0; i < num; i++) {
		CExcludedEvnt *eP = excludedEventArray.GetAt(i);
		if (eP->getIsActive())
			fillInEventTimeLineVectorForType(eP->getEventType(), eP->getLevelType(), eP->getFrom(), eP->getTo());
	}
	num = swallowEventArray.GetCount();
	for (int i = 0; i < num; i++) {
		CSwallowEvnt *eP = swallowEventArray.GetAt(i);
		if (eP->getIsActive())
			fillInEventTimeLineVectorForType(eP->getEventType(), eP->getLevelType(), eP->getFrom(), eP->getTo());
	}
	num = importedEventArray.GetCount();
	for (int i = 0; i < num; i++) {
		CImportedEvnt *eP = importedEventArray.GetAt(i);
		if (eP->getIsActive())
			fillInEventTimeLineVectorForType(eP->getEventType(), eP->getLevelType(), eP->getFrom(), eP->getTo());
	}
	if (spO2DropLimit3 == detectionCriteria.spO2DropLimit) {
		num = spO2DropEventArray3.GetCount();
		for (int i = 0; i < num; i++) {
			CSpO2DropEvnt *eP = spO2DropEventArray3.GetAt(i);
			if (eP->getIsActive())
				fillInEventTimeLineVectorForType(eP->getEventType(), eP->getLevelType(), eP->getFrom(), eP->getTo());
		}
		num = hypoEventArrayDesat3.GetCount();
		for (int i = 0; i < num; i++) {
			CHypoEvnt *eP = hypoEventArrayDesat3.GetAt(i);
			if (eP->getIsActive(detectionCriteria.hypopneaDropLimit) && (eP->pickFromDropLevel(detectionCriteria.hypopneaDropLimit)))
				fillInEventTimeLineVectorForType(eP->getEventType(), eP->getLevelType(), eP->getFrom(), eP->getTo());
		}
		num = arousalEventArrayDesat3.GetCount();
		for (int i = 0; i < num; i++) {
			CArousalEvnt *eP = arousalEventArrayDesat3.GetAt(i);
			if (eP->getIsActive(detectionCriteria.hypopneaDropLimit)) {
				fillInEventTimeLineVectorForType(eP->getEventType(), eP->getLevelType(), eP->getFrom(), eP->getTo());
				CRERAEvnt *rP = eP->getRERAEvnt();
				if (rP) {
					fillInEventTimeLineVectorForType(rP->getEventType(), rP->getLevelType(), rP->getFrom(), rP->getTo());
				}
			}
		}
	}
	if (spO2DropLimit4 == detectionCriteria.spO2DropLimit) {
		num = spO2DropEventArray4.GetCount();
		for (int i = 0; i < num; i++) {
			CSpO2DropEvnt *eP = spO2DropEventArray4.GetAt(i);
			if (eP->getIsActive())
				fillInEventTimeLineVectorForType(eP->getEventType(), eP->getLevelType(), eP->getFrom(), eP->getTo());
		}
		num = hypoEventArrayDesat4.GetCount();
		for (int i = 0; i < num; i++) {
			CHypoEvnt *eP = hypoEventArrayDesat4.GetAt(i);
			if (eP->getIsActive(detectionCriteria.hypopneaDropLimit) && (eP->pickFromDropLevel(detectionCriteria.hypopneaDropLimit)))
				fillInEventTimeLineVectorForType(eP->getEventType(), eP->getLevelType(), eP->getFrom(), eP->getTo());
		}
		num = arousalEventArrayDesat4.GetCount();
		for (int i = 0; i < num; i++) {
			CArousalEvnt *eP = arousalEventArrayDesat4.GetAt(i);
			if (eP->getIsActive(detectionCriteria.hypopneaDropLimit)) {
				fillInEventTimeLineVectorForType(eP->getEventType(), eP->getLevelType(), eP->getFrom(), eP->getTo());
				CRERAEvnt *rP = eP->getRERAEvnt();
				if (rP) {
					fillInEventTimeLineVectorForType(rP->getEventType(), rP->getLevelType(), rP->getFrom(), rP->getTo());
				}
			}
		}
	}
	debugEventTimeLineVector(_T("After filling"));
}

void CEvents::fillInEventTimeLineVectorForType(int _type, UINT _levelType,float _from,float _to)
{
	vector <UINT>::iterator lowUint, highUint;

	int index0 = (int) (_from / EVENT_TIMELINE_TIME_INTERVAL);
	int index1 = (int) (_to / EVENT_TIMELINE_TIME_INTERVAL);
	ASSERT(index1 >= index0);
	index0 = index0 < 0 ? 0 : index0;
	index1 = index1 > (int) eventTimelineVector.size() ? (int) eventTimelineVector.size() : index1;
	lowUint = eventTimelineVector.begin();
	highUint = lowUint;
	lowUint += index0;
	highUint += index1;

	switch (_type) {
	case evntTypeRERA:
		debugEventTimeLineVector(_T("---Filling RERA1"));
		transform(lowUint, highUint, lowUint, getEventSetOperation(_type));
		debugEventTimeLineVector(_T("---Filling RERA after event set"));
		transform(lowUint, highUint, lowUint, offAHLevelAll);
		debugEventTimeLineVector(_T("---Filling RERA after off level ALL"));
		transform(lowUint, highUint, lowUint, getAHLevelEventSetOperation(_levelType));
		debugEventTimeLineVector(_T("---Filling RERA after level set"));
		break;
	case evntTypeSnoring:
		transform(lowUint, highUint, lowUint, getEventSetOperation(_type));
		transform(lowUint, highUint, lowUint, offSnoringLevelTypeUndef);
		transform(lowUint, highUint, lowUint, getSnoringLevelEventSetOperation(_levelType));
		break;
	case evntTypeMixed:
	case evntTypeObstr:
	case evntTypeHypoObstr:
		TRACE(_T("Before eventset lowInt 0x%x \n"), *lowUint);
		transform(lowUint, highUint, lowUint, getEventSetOperation(_type));
		TRACE(_T("After eventset lowInt 0x%x \n"), *lowUint);
		transform(lowUint, highUint, lowUint, getAHLevelEventSetOperation(_levelType));
		TRACE(_T("After levelset lowInt 0x%x \n"), *lowUint);
		break;
	case evntTypeCentral:
	case evntTypeHypoCentral:
	case evntTypeArousal:
	case evntTypeManMarker:
	case evntTypeSpO2Drop:
	case evntTypeAwake:
	case evntTypeExcluded:
	case evntTypeImported:
	case evntTypeSwallow:
		transform(lowUint, highUint, lowUint, getEventSetOperation(_type));
		break;
	case evntTypeNone:
		break;
	default:
		break;
	}
}

/*
Description: Asks CCathData set to fill in swallow events (start and stop) and adds additional info
*/
bool CEvents::analyseSwallow(void)
{
	cathData->resetSwallow();
	cathData->fillSwallowArray(&swallowEventArray);
	int num = swallowEventArray.GetCount();
	for (int i = 0; i < num; i++) {
		CSwallowEvnt *eP = swallowEventArray.GetAt(i);
		eP->setRecordingStartTimeOfDay(startRecordingClockTime);
		short bp = bpData->getPos(eP->getFrom(),eP->getTo());
		eP->setBodyPos(bp);
		swallowEventArray.SetAt(i, eP);
	}
	return true;
}

bool CEvents::fillManualEventArray(void)
{
	if (mMarkerData) 
		mMarkerData->fillManualEventArray(&manualMarkerEventArray);
	return true;
}

/*
Description: Does all analysis
_findStartAndStop : 
		True : Finds start and stop from data
		false: Does not change Start and Stop, assumes they are given by user
*/
bool CEvents::doAnalyse(bool _findStartAndStop /* = true */)
{
	CSpiroWaitCursor curW;

	reset();	// Everything is reset
	detectionCriteria = editedDetectionCriteria;

	switch (detectionCriteria.spO2DropLimit) {
	case spO2DropLimit3:
		spO2DropEventArrayCurrentP = &spO2DropEventArray3;
		hypoEventArrayCurrentP = &hypoEventArrayDesat3; 
		arousalEventArrayCurrentP = &arousalEventArrayDesat3;
		break;
	case spO2DropLimit4:
		spO2DropEventArrayCurrentP = &spO2DropEventArray4;
		hypoEventArrayCurrentP = &hypoEventArrayDesat4;
		arousalEventArrayCurrentP = &arousalEventArrayDesat4;
		break;
	default:
		spO2DropEventArrayCurrentP = &spO2DropEventArray4;
		hypoEventArrayCurrentP = &hypoEventArrayDesat4;
		arousalEventArrayCurrentP = &arousalEventArrayDesat4;
		break;
	}
	if (_findStartAndStop) {
		findEffectiveStartAndStop();							// Will set total sleep time (first estimate)
		addExcludedEventsFromTimeWindowVector(cathData->getInvalidTimeWindowsVector(),detectionCriteria.spO2DropLimit);
	}
	resetEventTimelineVector();
	resetSnoringVector();

	//---These are COMPLETE analysis. The ENTIRE time window is analysed!
	fillManualEventArray();
	bool ret0 = analyseSwallow();
	bool ret1a = analyseSpO2(spO2DropLimit3);						// OK, uavh av eff start og eff stop og awake
	bool ret1b = analyseSpO2(spO2DropLimit4);						// OK, uavh av eff start og eff stop og awake

	float startAnalysisTime,stopAnalysisTime;											// Preliminary
	startAnalysisTime = stopAnalysisTime = .0f;
	bool enoughData = testIfDataOKforAnalysis(&startAnalysisTime,&stopAnalysisTime);	// Check if there is enough data
	if (enoughData) {
		vector <FLOAT> helpVTime;
		vector <unsigned short> helpV;
		setupHelpEventVector(&helpVTime, &helpV,startAnalysisTime,stopAnalysisTime);
		bool ret2 = findMixCentralObstrApnea(&helpVTime, &helpV);		// OK, uavh av eff start og eff stop og awake
		bool ret3 = findHypopnea(&helpVTime, &helpV,spO2DropLimit3);	// OK, uavh av eff start og eff stop og awake
		bool ret4 = findHypopnea(&helpVTime, &helpV,spO2DropLimit4);	// OK, uavh av eff start og eff stop og awake
		doRERAandArousalAnalysis();										// OK, uavh av eff start og eff stop og awake
	}
	doManualMarkerAnalysis();		// This is empty at the moment (uavh av eff start og eff stop og awake)
	
	sortAndOrganiseEvents(evntTypeAll);

	for (int i = 0; i < numSpO2DropLimits; i++) {
		for (int j = 0; j < numHypopneaDropLimits; j++) {
			removeArousalsWhenAnotherEvent(j, i);	// Remove or deaktivate if collision with another event
		}
	}

	//---Snoring prior to awake!
	doSnoringAnalysis();			

	//---Use these for awake detection later
	bool advancedAwakeAnalysis = generateVectorsForAwakeDet();	// Regardless of awake -> this result is used for awake detection!

	//---NOW do awake analysis
	if (advancedAwakeAnalysis) {
		if (!doAdvancedAwakeAnalysis())
			doAwakeAnalysis();  // Do conventional
	}
	else 
		doAwakeAnalysis();	// Depends on excluded									

	for (int i = 0; i < numSpO2DropLimits; i++) {
		for (int j = 0; j < numHypopneaDropLimits; j++) {
			deactivateArousalsWhenExcludedOrAwake(j, i);
		}
	}

	removeHypoWhenAnotherEvent(spO2DropLimit3);		// Remove if collision with excluded or awake
	removeHypoWhenAnotherEvent(spO2DropLimit4);		// Remove if collision with excluded or awake

	removeObstrWhenAnotherEvent();					// Remove if collision with excluded or awake
	removeCentralWhenAnotherEvent();				// Remove if collision with excluded or awake
	removeMixedWhenAnotherEvent();					// Remove if collision with excluded or awake
	disableSwallowWhenExcluded();					// Disable if collision with excluded
	removeRERAWhenAnotherEvent(spO2DropLimit3);		// Remove if coll with excluded, awake, mix, central, obstr, hypo
	removeRERAWhenAnotherEvent(spO2DropLimit4);		// Remove if coll with excluded, awake, mix, central, obstr, hypo

	for (int i = 0; i < numSpO2DropLimits; i++) {
		removeSpO2DropsWhenAnotherEvent(i);			// Marks as !aktive if collision with excluded or awake
	}

	//---Now all events have been detected - time to sum up
	fillInEventTimelineVector();				// A vector with bits set for each event and for body position
	fillAwakeEventArrayFromEventTimelineVector(); // Redo awake to adjust for excluded
	removeSnoringWhenAnotherEvent();			// Remove if collision with excluded or awake (using EvntTimeline Vector for this)
	computeTotalSleepTime();					// Uses the event timeline vector

	prepareSpO2TimelineVectorAndSumupStatistics();
	sumUpSpO2(spO2DropLimit3);					// Do sumup for 3% drops
	sumUpSpO2(spO2DropLimit4);					// Do sumup for 4% drops

	// Count number of hypopneas split into central and hypo
	for (int i = 0; i < numSpO2DropLimits; i++) {
		for (int j = 0; j < numHypopneaDropLimits; j++) {
			countHypEvents(j,i);
		}
	}
	
	fillStartsAndStopsVector();					// Sets up the vectors that contain start and stop of events. Used by
												// graphs to plot events. Only with current detection criteria

	for (int i = 0; i < numSpO2DropLimits; i++) {
		for (int j = 0; j < numHypopneaDropLimits; j++) {
			doBodyPosAnalysis(j,i);		// Finds how events split between body positions
		}
	}
	
	// Counts up all events (except snoring) and computes index (per hour of sleep)	
	// Count hypopnea distribution
	for (int i = 0; i < numSpO2DropLimits; i++) {
		for (int j = 0; j < numHypopneaDropLimits; j++) {
			doOverallLevelSumUp(j, i);
		}
	}

	doSnoringSumUp();							// Counts up snoring data

	// Computes RDI, AHI, Arousal index and RERA index	
	for (int i = 0; i < numSpO2DropLimits; i++) {
		for (int j = 0; j < numHypopneaDropLimits; j++) {
			doIndexAnalysis(j,i);		// Computes RDI, AHI, Arousal index and RERA index
		}
	}
	POESEnvBinning();
	POESEnvBinningForSnoring();
	PPHEnvBinning();
	PPHEnvBinningForSnoring();
	postChangeActions();
	eventDefinition = EVNT_DEF_AUTO;	// This analysis has been by computer
	
	return true;
}

/*
Description: Redoes the analysis after editing of events
_step: The step - where to start the re-analysis
*/
bool CEvents::postVisualEditAnalysis(int _step)
{
	CSpiroWaitCursor swc;

	switch (_step) {
	case stepExclude:
		fillExcludedEventArrayFromEventTimelineVector();
		fillAwakeEventArrayFromEventTimelineVector();
		for (int dl = 0; dl < numSpO2DropLimits; dl++) {
			disableArousalsWhenAwakeOrExcluded(dl);
			for (int hl = 0; hl < numHypopneaDropLimits; hl++) {
				disableHypoWhenAwakeOrExcluded(hl, dl);
			}
		}
		disableObstrWhenAwakeOrExcluded();
		disableCentralWhenAwakeOrExcluded();
		disableMixedWhenAwakeOrExcluded();
		disableSwallowWhenExcluded();
		for (int dl = 0; dl < numSpO2DropLimits; dl++) {
			disableSpO2DropsWhenAwakeOrExcluded(dl);
		}

		//---Events are ok now - time to sum up
		for (int dl = 0; dl < numSpO2DropLimits; dl++) {
			for (int hl = 0; hl < numHypopneaDropLimits; hl++) {
				doBodyPosAnalysis(hl, dl);
				doOverallLevelSumUp(hl, dl);
			}
		}			
		computeTotalSleepTime();
		prepareSpO2TimelineVectorAndSumupStatistics();	
		for (int dl = 0; dl < numSpO2DropLimits; dl++) {
			sumUpSpO2(dl);	
		}
		removeSnoringWhenAnotherEvent();
		doSnoringSumUp();
		for (int dl = 0; dl < numSpO2DropLimits; dl++) {
			for (int hl = 0; hl < numHypopneaDropLimits; hl++) {
				countHypEvents(hl,dl);
				doIndexAnalysis(hl,dl);
			}
		}
		POESEnvBinning();
		POESEnvBinningForSnoring();
		PPHEnvBinning();
		PPHEnvBinningForSnoring();
		postChangeActions();
		eventDefinition = EVNT_DEF_MANUAL;
		fillStartsAndStopsVector();
		break;
	case stepAwake:
		fillAwakeEventArrayFromEventTimelineVector();
		for (int dl = 0; dl < numSpO2DropLimits; dl++) {
			disableArousalsWhenAwakeOrExcluded(dl);
			for (int hl = 0; hl < numHypopneaDropLimits; hl++) {
				disableHypoWhenAwakeOrExcluded(hl, dl);
			}
		}
		disableObstrWhenAwakeOrExcluded();
		disableCentralWhenAwakeOrExcluded();
		disableMixedWhenAwakeOrExcluded();
		for (int dl = 0; dl < numSpO2DropLimits; dl++) {
			disableSpO2DropsWhenAwakeOrExcluded(dl);
		}

		//---Events are ok now - time to sum up
		for (int dl = 0; dl < numSpO2DropLimits; dl++) {
			for (int hl = 0; hl < numHypopneaDropLimits; hl++) {
				doBodyPosAnalysis(hl, dl);
				doOverallLevelSumUp(hl, dl);
			}
		}
		computeTotalSleepTime();
		prepareSpO2TimelineVectorAndSumupStatistics();	// Covers all criteria
		for (int dl = 0; dl < numSpO2DropLimits; dl++) {
			sumUpSpO2(dl);		// Depends on criteria
		}
		removeSnoringWhenAnotherEvent();
		doSnoringSumUp();
		for (int dl = 0; dl < numSpO2DropLimits; dl++) {
			for (int hl = 0; hl < numHypopneaDropLimits; hl++) {
				countHypEvents(hl, dl);
				doIndexAnalysis(hl, dl);
			}
		}
		POESEnvBinning();
		POESEnvBinningForSnoring();
		PPHEnvBinning();
		PPHEnvBinningForSnoring();
		postChangeActions();
		eventDefinition = EVNT_DEF_MANUAL;		
		fillStartsAndStopsVector();
		break;
	case stepDesat:
		fillDesatEventArrayFromEventTimelineVector();
		prepareSpO2TimelineVectorAndSumupStatistics();
		sumUpSpO2(detectionCriteria.spO2DropLimit); 
		doBodyPosAnalysis(detectionCriteria.hypopneaDropLimit,detectionCriteria.spO2DropLimit);
		doOverallLevelSumUp(detectionCriteria.hypopneaDropLimit, detectionCriteria.spO2DropLimit);
		postChangeActions();
		eventDefinition = EVNT_DEF_MANUAL;
		fillStartsAndStopsVector();
		break;
	case stepApnea:
		fillMixedEventArrayFromEventTimelineVector();
		fillCentralEventArrayFromEventTimelineVector();
		fillObstrEventArrayFromEventTimelineVector();
		fillHypoEventArrayFromEventTimelineVector();
		countHypEvents(detectionCriteria.hypopneaDropLimit, detectionCriteria.spO2DropLimit);
		doIndexAnalysis(detectionCriteria.hypopneaDropLimit, detectionCriteria.spO2DropLimit);
		doBodyPosAnalysis(detectionCriteria.hypopneaDropLimit, detectionCriteria.spO2DropLimit);
		doOverallLevelSumUp(detectionCriteria.hypopneaDropLimit, detectionCriteria.spO2DropLimit);
		postChangeActions();
		eventDefinition = EVNT_DEF_MANUAL;
		fillStartsAndStopsVector();
		break;
	case stepSwallow:
		fillSwallowArrayFromEventTimelineVector();
		fillStartsAndStopsVector();
		break;
	case stepRERA:
		fillReraArrayFromEventTimelineVector();
		doIndexAnalysis(detectionCriteria.hypopneaDropLimit, detectionCriteria.spO2DropLimit);
		doBodyPosAnalysis(detectionCriteria.hypopneaDropLimit, detectionCriteria.spO2DropLimit);
		doOverallLevelSumUp(detectionCriteria.hypopneaDropLimit, detectionCriteria.spO2DropLimit);
		postChangeActions();
		eventDefinition = EVNT_DEF_MANUAL;
		fillStartsAndStopsVector();
		break;
	case stepSnoring:
		fillSnoringEventArrayFromEventTimeLineVector();
		removeSnoringWhenAnotherEvent();
		doSnoringSumUp();
		POESEnvBinningForSnoring();
		doBodyPosAnalysis(detectionCriteria.hypopneaDropLimit, detectionCriteria.spO2DropLimit);
		doOverallLevelSumUp(detectionCriteria.hypopneaDropLimit, detectionCriteria.spO2DropLimit);
		postChangeActions();
		eventDefinition = EVNT_DEF_MANUAL;
		fillStartsAndStopsVector();
		break;	
	}
	return false;
}

/*
Description: Used after editing in pop-up dialogue and after import of manual events
*/
void CEvents::postEditEventAnalysis(UINT _eventTypes)
{
	/*
	If excluded changes, redo awake
	If awake changes, redo all others
	If snoring changes, don't do snoring
	*/

	//if (evntTypeImported == _eventTypes) return; //*INN*

	sortAndOrganiseEvents(evntTypeAll);

	if (evntTypeExcluded & _eventTypes) {
		doAwakeAnalysis();																		// Depends on excluded	
	}
	if ((evntTypeAwake & _eventTypes) || (evntTypeExcluded & _eventTypes)) {

		//--Enable or disable events depending on awake and excluded
		for (int dl = 0; dl < numSpO2DropLimits; dl++) {
			for (int hl = 0; hl < numHypopneaDropLimits; hl++) {
				enableArousals(hl, dl);	// Enable or disable depending on collision with excluded or awake
				//enableRERA(detectionCriteria.hypopneaDropLimit, detectionCriteria.spO2DropLimit);		// Enable or disable depending on collision with excluded or awake
				enableHypo(hl, dl);		// Enable or disable depending on collision with excluded or awake
			}
		}
		enableObstr();																			// Enable or disable depending on collision with excluded or awake
		enableCentral();																		// Enable or disable depending on collision with excluded or awake
		enableMixed();																			// Enable or disable depending on collision with excluded or awake
		enableSwallow();
		for (int dl = 0; dl < numSpO2DropLimits; dl++) {
			enableSpO2Drops(dl);		// Enable or disable depending on collision with excluded or awake
		}
	}

	//---Now all events have been detected - time to sum up
	fillInEventTimelineVector();				// A vector with bits set for each event. Uses event copy array
	removeSnoringWhenAnotherEvent();
	computeTotalSleepTime();					// Uses the eventTimeline vector

	prepareSpO2TimelineVectorAndSumupStatistics();
	for (int dl = 0; dl < numSpO2DropLimits; dl++) {
		sumUpSpO2(dl);
	}
	//analysePulseRate();

	//---Count number of hypopneas split into central and hypo
	for (int i = 0; i < numSpO2DropLimits; i++) {
		for (int j = 0; j < numHypopneaDropLimits; j++) {
			countHypEvents(j, i);
		}
	}

	fillStartsAndStopsVector();					// Sets up the vectors that contain start and stop of events. Used by
												// graphs to plot events. Only uses active events
	doBodyPosAnalysis(detectionCriteria.hypopneaDropLimit, detectionCriteria.spO2DropLimit);	// Finds how events split between body positions
	doOverallLevelSumUp(detectionCriteria.hypopneaDropLimit, detectionCriteria.spO2DropLimit);	// Counts up all events (except snoring) and computes index (per hour of sleep)	
	doSnoringSumUp();							// Counts up snoring data
	doIndexAnalysis(detectionCriteria.hypopneaDropLimit, detectionCriteria.spO2DropLimit);		// Computes RDI, AHI, Arousal index and RERA index
	eventDefinition |= EVNT_DEF_MANUAL;			// This analysis has been by operator
	POESEnvBinning();
	POESEnvBinningForSnoring(); 
	PPHEnvBinning();
	PPHEnvBinningForSnoring();
	postChangeActions();
}

void CEvents::postExcludeAnalysis(void)
{
	postEditEventAnalysis(evntTypeExcluded);
}

void CEvents::fillStartsAndStopsVector(unsigned int _actOn /* = ACT_ON_EVT_ALL */)
{
	///////////////////////////////////////
	//---Fill in starts and stops vectors
	///////////////////////////////////////
	ASSERT(arousalEventArrayCurrentP);
	ASSERT(hypoEventArrayCurrentP);
	ASSERT(spO2DropEventArrayCurrentP);

	for (int i = 0; i < evCountWithoutLevel; i++) 
		eventsWithoutLevelBeginsEnds[i].clear();
	for (int i = 0; i < evCountWithLevel; i++) {
		for (int j = 0; j < bgLevelTypeCount; j++) {
			eventsWithLevelBeginsEnds[i][j].clear();
		}
	}

	//---Obstr Hypopnea
	if ((_actOn & evntTypeHypoObstr) && (hypoEventArrayCurrentP)) {
		int numev = hypoEventArrayCurrentP->GetCount();
		for (int i = 0; i < numev; i++) {
			CHypoEvnt *eP = hypoEventArrayCurrentP->GetAt(i);
			if (eP->getIsActive() && (eP->pickFromDropLevel(detectionCriteria.hypopneaDropLimit))) {
				int lType = eP->getLevelType();
				int ht = ((CHypoEvnt *)eP)->getHypoType();

				if (hypoTypeObstr == ht) {
					switch (lType) {
					case levelTypeLower:
						eventsWithLevelBeginsEnds[evObstrHypo][bgLevelTypeLower].push_back(eP->getFrom());
						eventsWithLevelBeginsEnds[evObstrHypo][bgLevelTypeLower].push_back(eP->getTo());
						break;
					case levelTypeUpper:
						eventsWithLevelBeginsEnds[evObstrHypo][bgLevelTypeUpper].push_back(eP->getFrom());
						eventsWithLevelBeginsEnds[evObstrHypo][bgLevelTypeUpper].push_back(eP->getTo());
						break;
					case levelTypeMulti:
						eventsWithLevelBeginsEnds[evObstrHypo][bgLevelTypeMulti].push_back(eP->getFrom());
						eventsWithLevelBeginsEnds[evObstrHypo][bgLevelTypeMulti].push_back(eP->getTo());
						break;
					case levelTypeUndef:
						eventsWithLevelBeginsEnds[evObstrHypo][bgLevelTypeUndef].push_back(eP->getFrom());
						eventsWithLevelBeginsEnds[evObstrHypo][bgLevelTypeUndef].push_back(eP->getTo());
						break;
					default:
						break;
					}
				}
			}
		}
		for (int i = 0; i < bgLevelTypeCount; i++)
			sort(eventsWithLevelBeginsEnds[evObstrHypo][i].begin(), eventsWithLevelBeginsEnds[evObstrHypo][i].end());
	}

	//---Snoring
	if (_actOn & evntTypeSnoring) {

		vector <UINT>::iterator f1, f2,v;
		float t1, t2;

		//---All levels for use on top indicator 
		eventsWithLevelBeginsEnds[evSnoringAllLevels][0].clear();
		v = snoringVector.begin();
		do {
			f1 = find_if(v, snoringVector.end(), isSnoringNotAwakeExcluded);
			if (f1 != snoringVector.end()) {
				f2 = find_if_not(f1, snoringVector.end(), isSnoringNotAwakeExcluded);
				t1 = (float) distance(snoringVector.begin(),f1) * EVENT_TIMELINE_TIME_INTERVAL;
				t2 = (float) distance(snoringVector.begin(), f2) * EVENT_TIMELINE_TIME_INTERVAL;
				eventsWithLevelBeginsEnds[evSnoringAllLevels][0].push_back(t1);
				eventsWithLevelBeginsEnds[evSnoringAllLevels][0].push_back(t2);
				v = f2;
			}
		} while (f1 != snoringVector.end());

		//---Upper level for use on top indicator 
		eventsWithLevelBeginsEnds[evSnoringUpperLevel][bgLevelTypeUpper].clear();
		v = snoringVector.begin();
		do {
			f1 = find_if(v, snoringVector.end(), isSnoringUpperNotAwakeExcluded);
			if (f1 != snoringVector.end()) {
				f2 = find_if_not(f1, snoringVector.end(), isSnoringUpperNotAwakeExcluded);
				t1 = (float)distance(snoringVector.begin(), f1) * EVENT_TIMELINE_TIME_INTERVAL;
				t2 = (float)distance(snoringVector.begin(), f2) * EVENT_TIMELINE_TIME_INTERVAL;
				eventsWithLevelBeginsEnds[evSnoringUpperLevel][bgLevelTypeUpper].push_back(t1);
				eventsWithLevelBeginsEnds[evSnoringUpperLevel][bgLevelTypeUpper].push_back(t2);
				v = f2;
			}
		} while (f1 != snoringVector.end());

		//---Multi levels for use on top indicator 
		eventsWithLevelBeginsEnds[evSnoringMultiLevel][bgLevelTypeMulti].clear();
		v = snoringVector.begin();
		do {
			f1 = find_if(v, snoringVector.end(), isSnoringMultiNotAwakeExcluded);
			if (f1 != snoringVector.end()) {
				f2 = find_if_not(f1, snoringVector.end(), isSnoringMultiNotAwakeExcluded);
				t1 = (float)distance(snoringVector.begin(), f1) * EVENT_TIMELINE_TIME_INTERVAL;
				t2 = (float)distance(snoringVector.begin(), f2) * EVENT_TIMELINE_TIME_INTERVAL;
				eventsWithLevelBeginsEnds[evSnoringMultiLevel][bgLevelTypeMulti].push_back(t1);
				eventsWithLevelBeginsEnds[evSnoringMultiLevel][bgLevelTypeMulti].push_back(t2);
				v = f2;
			}
		} while (f1 != snoringVector.end());

		//---Lower levels for use on top indicator 
		eventsWithLevelBeginsEnds[evSnoringLowerLevel][bgLevelTypeLower].clear();
		v = snoringVector.begin();
		do {
			f1 = find_if(v, snoringVector.end(), isSnoringLowerNotAwakeExcluded);
			if (f1 != snoringVector.end()) {
				f2 = find_if_not(f1, snoringVector.end(), isSnoringLowerNotAwakeExcluded);
				t1 = (float)distance(snoringVector.begin(), f1) * EVENT_TIMELINE_TIME_INTERVAL;
				t2 = (float)distance(snoringVector.begin(), f2) * EVENT_TIMELINE_TIME_INTERVAL;
				eventsWithLevelBeginsEnds[evSnoringLowerLevel][bgLevelTypeLower].push_back(t1);
				eventsWithLevelBeginsEnds[evSnoringLowerLevel][bgLevelTypeLower].push_back(t2);
				v = f2;
			}
		} while (f1 != snoringVector.end());

		//---Undef levels for use on top indicator 
		eventsWithLevelBeginsEnds[evSnoringUndefLevel][bgLevelTypeUndef].clear();
		v = snoringVector.begin();
		do {
			f1 = find_if(v, snoringVector.end(), isSnoringUndefNotAwakeExcluded);
			if (f1 != snoringVector.end()) {
				f2 = find_if_not(f1, snoringVector.end(), isSnoringUndefNotAwakeExcluded);
				t1 = (float)distance(snoringVector.begin(), f1) * EVENT_TIMELINE_TIME_INTERVAL;
				t2 = (float)distance(snoringVector.begin(), f2) * EVENT_TIMELINE_TIME_INTERVAL;
				eventsWithLevelBeginsEnds[evSnoringUndefLevel][bgLevelTypeUndef].push_back(t1);
				eventsWithLevelBeginsEnds[evSnoringUndefLevel][bgLevelTypeUndef].push_back(t2);
				v = f2;
			}
		} while (f1 != snoringVector.end());

		for (int i = 0; i < bgLevelTypeCount; i++) {
			sort(eventsWithLevelBeginsEnds[evSnoringAllLevels][i].begin(), eventsWithLevelBeginsEnds[evSnoringAllLevels][i].end());
		}
	}

	//---RERA
	if (_actOn & evntTypeRERA) {
		int numev = arousalEventArrayCurrentP->GetCount();
		for (int i = 0; i < numev; i++) {
			CArousalEvnt *aP = (CArousalEvnt *)arousalEventArrayCurrentP->GetAt(i);
			CRERAEvnt *rP = aP->getRERAEvnt();
			if (aP->getIsActive(detectionCriteria.hypopneaDropLimit) && rP) {
				int lType = rP->getLevelType();
				switch (lType) {
				case levelTypeLower:
					eventsWithLevelBeginsEnds[evRERA][bgLevelTypeLower].push_back(rP->getFrom());
					eventsWithLevelBeginsEnds[evRERA][bgLevelTypeLower].push_back(rP->getTo());
					break;
				case levelTypeUpper:
					eventsWithLevelBeginsEnds[evRERA][bgLevelTypeUpper].push_back(rP->getFrom());
					eventsWithLevelBeginsEnds[evRERA][bgLevelTypeUpper].push_back(rP->getTo());
					break;
				case levelTypeMulti:
					eventsWithLevelBeginsEnds[evRERA][bgLevelTypeMulti].push_back(rP->getFrom());
					eventsWithLevelBeginsEnds[evRERA][bgLevelTypeMulti].push_back(rP->getTo());
					break;
				case levelTypeUndef:
					eventsWithLevelBeginsEnds[evRERA][bgLevelTypeUndef].push_back(rP->getFrom());
					eventsWithLevelBeginsEnds[evRERA][bgLevelTypeUndef].push_back(rP->getTo());
					break;
				default:
					break;
				}
			}
		}
		for (int i = 0; i < bgLevelTypeCount; i++) {
			sort(eventsWithLevelBeginsEnds[evRERA][i].begin(), eventsWithLevelBeginsEnds[evRERA][i].end());
		}
	}

	//---Mixed
	if (_actOn & evntTypeMixed) {
		int numev = mixedEventArray.GetCount();
		for (int i = 0; i < numev; i++) {
			CEvnt *eP = (CEvnt *)mixedEventArray.GetAt(i);
			if (eP->getIsActive()) {
				int lType = eP->getLevelType();
				switch (lType) {
				case levelTypeLower:
					eventsWithLevelBeginsEnds[evMix][bgLevelTypeLower].push_back(eP->getFrom());
					eventsWithLevelBeginsEnds[evMix][bgLevelTypeLower].push_back(eP->getTo());
					break;
				case levelTypeUpper:
					eventsWithLevelBeginsEnds[evMix][bgLevelTypeUpper].push_back(eP->getFrom());
					eventsWithLevelBeginsEnds[evMix][bgLevelTypeUpper].push_back(eP->getTo());
					break;
				case levelTypeMulti:
					eventsWithLevelBeginsEnds[evMix][bgLevelTypeMulti].push_back(eP->getFrom());
					eventsWithLevelBeginsEnds[evMix][bgLevelTypeMulti].push_back(eP->getTo());
					break;
				case levelTypeUndef:
					eventsWithLevelBeginsEnds[evMix][bgLevelTypeUndef].push_back(eP->getFrom());
					eventsWithLevelBeginsEnds[evMix][bgLevelTypeUndef].push_back(eP->getTo());
					break;
				default:
					break;
				}
			}
		}
		for (int i = 0; i < bgLevelTypeCount; i++) {
			sort(eventsWithLevelBeginsEnds[evMix][i].begin(), eventsWithLevelBeginsEnds[evMix][i].end());
		}
	}

	//---Obstr
	if (_actOn & evntTypeObstr) {
		int numev = obstrEventArray.GetCount();
		for (int i = 0; i < numev; i++) {
			CEvnt *eP = (CEvnt *)obstrEventArray.GetAt(i);
			if (eP->getIsActive()) {
				int lType = eP->getLevelType();
				switch (lType) {
				case levelTypeLower:
					eventsWithLevelBeginsEnds[evObstr][bgLevelTypeLower].push_back(eP->getFrom());
					eventsWithLevelBeginsEnds[evObstr][bgLevelTypeLower].push_back(eP->getTo());
					break;
				case levelTypeUpper:
					eventsWithLevelBeginsEnds[evObstr][bgLevelTypeUpper].push_back(eP->getFrom());
					eventsWithLevelBeginsEnds[evObstr][bgLevelTypeUpper].push_back(eP->getTo());
					break;
				case levelTypeMulti:
					eventsWithLevelBeginsEnds[evObstr][bgLevelTypeMulti].push_back(eP->getFrom());
					eventsWithLevelBeginsEnds[evObstr][bgLevelTypeMulti].push_back(eP->getTo());
					break;
				case levelTypeUndef:
					eventsWithLevelBeginsEnds[evObstr][bgLevelTypeUndef].push_back(eP->getFrom());
					eventsWithLevelBeginsEnds[evObstr][bgLevelTypeUndef].push_back(eP->getTo());
					break;
				default:
					break;
				}
			}
		}
		for (int i = 0; i < bgLevelTypeCount; i++) {
			sort(eventsWithLevelBeginsEnds[evObstr][i].begin(), eventsWithLevelBeginsEnds[evObstr][i].end());
		}
	}

	//---Without level

	//---Central
	if (_actOn & evntTypeCentral) {
		int numev = centralEventArray.GetCount();
		for (int i = 0; i < numev; i++) {
			CEvnt *eP = (CEvnt *)centralEventArray.GetAt(i);
			if (eP->getIsActive()) {
				eventsWithoutLevelBeginsEnds[evCentral].push_back(eP->getFrom());
				eventsWithoutLevelBeginsEnds[evCentral].push_back(eP->getTo());
			}
		}
		sort(eventsWithoutLevelBeginsEnds[evCentral].begin(), eventsWithoutLevelBeginsEnds[evCentral].end());
	}

	//---SpO2 drops
	if (_actOn & evntTypeSpO2Drop) {
		int numev = spO2DropEventArrayCurrentP->GetCount();
		for (int i = 0; i < numev; i++) {
			CEvnt *eP = (CEvnt *)spO2DropEventArrayCurrentP->GetAt(i);
			if (eP->getIsActive()) {
				eventsWithoutLevelBeginsEnds[evSpO2].push_back(eP->getFrom());
				eventsWithoutLevelBeginsEnds[evSpO2].push_back(eP->getTo());
			}
		}
		sort(eventsWithoutLevelBeginsEnds[evSpO2].begin(), eventsWithoutLevelBeginsEnds[evSpO2].end());

		//---Manual
		numev = manualMarkerEventArray.GetCount();
		for (int i = 0; i < numev; i++) {
			CEvnt *eP = (CEvnt *)manualMarkerEventArray.GetAt(i);
			if (eP->getIsActive()) {
				eventsWithoutLevelBeginsEnds[evManual].push_back(eP->getFrom());
				eventsWithoutLevelBeginsEnds[evManual].push_back(eP->getTo());
			}
		}
		sort(eventsWithoutLevelBeginsEnds[evManual].begin(), eventsWithoutLevelBeginsEnds[evManual].end());
	}
	
	//---Arousals
	if (_actOn & evntTypeArousal) {
		int numev = arousalEventArrayCurrentP->GetCount();
		for (int i = 0; i < numev; i++) {
			CArousalEvnt *eP = arousalEventArrayCurrentP->GetAt(i);
			if (eP->getIsActive(detectionCriteria.hypopneaDropLimit)) {
				eventsWithoutLevelBeginsEnds[evArousal].push_back(eP->getFrom());
				eventsWithoutLevelBeginsEnds[evArousal].push_back(eP->getTo());
			}
		}
		sort(eventsWithoutLevelBeginsEnds[evArousal].begin(), eventsWithoutLevelBeginsEnds[evArousal].end());
	}
	
	//---Central hypopnea
	if ((_actOn & evntTypeHypoCentral) && (hypoEventArrayCurrentP)) {
		int numev = hypoEventArrayCurrentP->GetCount();
		for (int i = 0; i < numev; i++) {
			CHypoEvnt *eP = hypoEventArrayCurrentP->GetAt(i);
			int ht = eP->getHypoType();
			if (eP->getIsActive() && (eP->pickFromDropLevel(detectionCriteria.hypopneaDropLimit))) {
				if (hypoTypeCentral == ht) {
					eventsWithoutLevelBeginsEnds[evCentralHypo].push_back(eP->getFrom());
					eventsWithoutLevelBeginsEnds[evCentralHypo].push_back(eP->getTo());
				}
			}
		}
		sort(eventsWithoutLevelBeginsEnds[evCentralHypo].begin(), eventsWithoutLevelBeginsEnds[evCentralHypo].end());
	}

	//---Awake
	if (_actOn & evntTypeAwake) {
		int numev = awakeEventArray.GetCount();
		for (int i = 0; i < numev; i++) {
			CEvnt *eP = (CEvnt *)awakeEventArray.GetAt(i);
			if (eP->getIsActive()) {
				eventsWithoutLevelBeginsEnds[evAwake].push_back(eP->getFrom());
				eventsWithoutLevelBeginsEnds[evAwake].push_back(eP->getTo());
			}
		}
		sort(eventsWithoutLevelBeginsEnds[evAwake].begin(), eventsWithoutLevelBeginsEnds[evAwake].end());
	}

	//---Excluded
	if (_actOn & evntTypeExcluded) {
		int numev = excludedEventArray.GetCount();
		for (int i = 0; i < numev; i++) {
			CEvnt *eP = (CEvnt *) excludedEventArray.GetAt(i);
			if (eP->getIsActive()) {
				eventsWithoutLevelBeginsEnds[evExcluded].push_back(eP->getFrom());
				eventsWithoutLevelBeginsEnds[evExcluded].push_back(eP->getTo());
			}
		}
		sort(eventsWithoutLevelBeginsEnds[evExcluded].begin(), eventsWithoutLevelBeginsEnds[evExcluded].end());
	}

	//---Swallow
	if (_actOn & evntTypeSwallow) {
		int numev = swallowEventArray.GetCount();
		for (int i = 0; i < numev; i++) {
			CEvnt *eP = (CEvnt *)swallowEventArray.GetAt(i);
			if (eP->getIsActive()) {
				eventsWithoutLevelBeginsEnds[evSwallow].push_back(eP->getFrom());
				eventsWithoutLevelBeginsEnds[evSwallow].push_back(eP->getTo());
			}
		}
		sort(eventsWithoutLevelBeginsEnds[evSwallow].begin(), eventsWithoutLevelBeginsEnds[evSwallow].end());
	}

	//---Imported
	if (_actOn & evntTypeImported) {
		int numev = importedEventArray.GetCount();
		for (int i = 0; i < numev; i++) {
			CEvnt *eP = (CEvnt *)importedEventArray.GetAt(i);
			if (eP->getIsActive()) {
				eventsWithoutLevelBeginsEnds[evImported].push_back(eP->getFrom());
				eventsWithoutLevelBeginsEnds[evImported].push_back(eP->getTo());
			}
		}
		sort(eventsWithoutLevelBeginsEnds[evImported].begin(), eventsWithoutLevelBeginsEnds[evImported].end());
	}

	//---Start and stop from detected bed-time
	if (_actOn & evntTypeStartStop) {
		eventsWithoutLevelBeginsEnds[evStartStop].push_back(.0f);
		eventsWithoutLevelBeginsEnds[evStartStop].push_back(effectiveStartTime);
		eventsWithoutLevelBeginsEnds[evStartStop].push_back(effectiveStopTime);
		eventsWithoutLevelBeginsEnds[evStartStop].push_back(maxStopTime);
	}
}

bool CEvents::visualEditDeleteWholeEvent(UINT _eventType, float _t)
{
	bool ok = visualEditDelete(_eventType, _t);
	if (ok) addNewState();
	return ok;
}

/*
Description: Returns centre element of a vector. This is the median IFF the vector is sorted!
Odd length vector: Returns the value of centre element
Even length vector: Returns the average f the two centre elements
*/
float CEvents::getMedian(vector <FLOAT> *_v)
{
	if (0 == _v->size()) return .0f;
	if (1 == _v->size()) return _v->at(0);

	int len = _v->size();
	int g = len % 2;

	int pos = len / 2;
	if (0 == (len % 2)) { // Even
		 float a = _v->at(pos - 1);
		 float b = _v->at(pos);
		 return (a + b) / 2.0f;
	}
	// Odd
	return _v->at(pos);
}

/*
Description: Tests if BOTH T0 and T1 increases
*/
bool CEvents::getDoesFlowRiseOld(float _from, float _to)
{
	vector <FLOAT> *t0env = cathData->getT0EnvVector();
	vector <FLOAT> *t1env = cathData->getT1EnvVector();
	vector <FLOAT> *t0envTime = cathData->getT0EnvVectorTime();
	vector <FLOAT> *t1envTime = cathData->getT1EnvVectorTime();
	if (0 == t0env->size()) return false;
	if (0 == t1env->size()) return false;

	ASSERT(t0env->size() == t0envTime->size());
	ASSERT(t1env->size() == t1envTime->size());

	vector <FLOAT>::iterator startIt, endIt;
	int startSample;
	int endSample;
	
	float factor = 1.0f + detectionCriteria.flowIncreaseAtArousal / 100.0f;

	//---T0
	startIt = lower_bound(t0envTime->begin(), t0envTime->end(), _from);
	endIt = lower_bound(t0envTime->begin(), t0envTime->end(),_to);
	if (endIt < t0envTime->end()) {
		startSample = distance(t0envTime->begin(), startIt);
		endSample = distance(t0envTime->begin(), endIt);
		if (t0env->at(endSample) <= (t0env->at(startSample) * factor)) return false;
	}
	//---T1
	startIt = lower_bound(t1envTime->begin(), t1envTime->end(), _from);
	endIt = lower_bound(t1envTime->begin(), t1envTime->end(), _to);
	if (endIt < t1envTime->end()) {
		startSample = distance(t1envTime->begin(), startIt);
		endSample = distance(t1envTime->begin(), endIt);
		if (t1env->at(endSample) <= (t1env->at(startSample) * factor)) return false;
	}
	return true;
}

/*
Description: Tests if the flow signal
*/
bool CEvents::getDoesFlowRise(float _from, float _to)
{
	vector <FLOAT> *flow = cathData->getBFlowVector();
	
	vector <FLOAT> *flowTime = cathData->getBFlowVectorTime();
	if (0 == flow->size()) return false;

	ASSERT(flowTime->size() == flow->size());

	vector <FLOAT>::iterator startIt, endIt;
	int startSample;
	int endSample;

	float factor = 1.0f + detectionCriteria.flowIncreaseAtArousal / 100.0f;

	//---T0
	startIt = lower_bound(flowTime->begin(), flowTime->end(), _from);
	endIt = lower_bound(flowTime->begin(), flowTime->end(), _to);
	if (endIt < flowTime->end()) {
		startSample = distance(flowTime->begin(), startIt);
		endSample = distance(flowTime->begin(), endIt);
		if (flow->at(endSample) <= (flow->at(startSample) * factor)) return false;
	}
	return true;
}


/*
Description: Finds all arousals and rera - independent of Spo2 drop limit and hypopnea criterion. 
These will be split later, when the events are removed due to collision with hypopnea.
*/
void CEvents::doRERAandArousalAnalysis(void)
{
	vector <FLOAT> *admittance = cathData->getAdmittanceVector();
	vector <FLOAT> *admittanceTime = cathData->getAdmittanceVectorTime();
	vector <FLOAT> *pulseRate = oxData->getPulseRateVector();
	vector <FLOAT> *pulseRateTime = oxData->getTimeaxisVector();
	vector <FLOAT> *respFrq = cathData->getRespFrqVector();
	vector <FLOAT> *respFrqTime = cathData->getRespFrqVectorTime();
	vector <FLOAT> *poes = cathData->getPOESEnvVector();
	vector <FLOAT> *poesTime = cathData->getPOESEnvVectorTime();
	vector <FLOAT> *pph = cathData->getPPHEnvVector();
	vector <FLOAT> *pphTime = cathData->getPPHEnvVectorTime();

	vector <FLOAT> *bFlow = cathData->getBFlowVector();
	vector <FLOAT> *bFlowTime = cathData->getBFlowVectorTime();
	vector <FLOAT> *bEff = cathData->getBEfficiencyVector();
	vector <FLOAT> *bEffTime = cathData->getBEfficiencyVectorTime();

	ASSERT(admittance->size() == admittanceTime->size());
	ASSERT(pulseRate->size() == pulseRateTime->size());
	ASSERT(respFrq->size() == respFrqTime->size());
	ASSERT(poes->size() == poesTime->size());
	ASSERT(bFlow->size() == bFlowTime->size());
	ASSERT(bEff->size() == bEffTime->size());

	if (0 == admittance->size()) return;
	if (0 == pulseRate->size()) return;
	if (0 == respFrq->size()) return;
	if (0 == poes->size()) return;

	if (pulseRate->size() <= 1) return; 

	vector <FLOAT>::iterator itAdmStart,itAdmEnd;
	vector <FLOAT>::iterator itAdmStartTime,itAdmEndTime;

	float timeStep = .5f;
	float lastArousalEnd = .0f;
	float halfW = ADMITTANCE_PEAK_WINDOW_LENGTH / 2.0f;

	float lastRERAStopTime = .0f;

	float stopTime = admittanceTime->at(admittanceTime->size() - 1);
	for (float theTime = 0 ; theTime <= stopTime ; theTime += timeStep) {
	
		itAdmStartTime = lower_bound(admittanceTime->begin(),admittanceTime->end(),theTime);
		itAdmEndTime = lower_bound(admittanceTime->begin(),admittanceTime->end(),theTime + detectionCriteria.admittancePeakWindowLength);

		if (itAdmEndTime >= admittanceTime->end()) {
			return;
		}

		int startAdmSample = distance(admittanceTime->begin(),itAdmStartTime);
		int endAdmSample = distance(admittanceTime->begin(),itAdmEndTime);

		itAdmStart = admittance->begin();
		itAdmEnd = itAdmStart;
		itAdmStart += startAdmSample;
		itAdmEnd += endAdmSample;

		int peakPosition = 0;
		bool peakAdm = CDataSet::findPeak2(itAdmStart, itAdmEnd,
			detectionCriteria.admittancePeakFactorHead,
			detectionCriteria.admittancePeakFactorTail,
			false, &peakPosition);
		bool tempRise = true;
		if (peakAdm) {

			vector <FLOAT>::iterator maxAdmIterator = max_element(itAdmStart, itAdmEnd);
			float timeOfMaxAdm = admittanceTime->at(distance(admittance->begin(), maxAdmIterator));

			//---In case there are more than one peak
			vector <FLOAT>::iterator firstPeakAdmIterator = CDataSet::getFirstPeak(itAdmStart, itAdmEnd);
			float timeOfFirstPeak = admittanceTime->at(distance(admittance->begin(), firstPeakAdmIterator));

			//--check for temp rise
			tempRise = getDoesFlowRise(theTime, timeOfFirstPeak);

			if (tempRise) {

				//---Qualify with pulse rate peak
				float seekPRPeakTimeEnd = timeOfFirstPeak + detectionCriteria.pulseRatePeakWindowLength;
				vector <FLOAT>::iterator itPRStartTime, itPREndTime;

				itPRStartTime = lower_bound(pulseRateTime->begin(), pulseRateTime->end(), timeOfFirstPeak -
					detectionCriteria.pulseRateWindowSlack);
				itPREndTime = lower_bound(pulseRateTime->begin(), pulseRateTime->end(), seekPRPeakTimeEnd);
				if (itPREndTime >= pulseRateTime->end()) {
					return;
				}

				int startPRSample = distance(pulseRateTime->begin(), itPRStartTime);
				int endPRSample = distance(pulseRateTime->begin(), itPREndTime);
				vector <FLOAT>::iterator itPRStart, itPREnd;
				itPRStart = pulseRate->begin();
				itPRStart += startPRSample;
				itPREnd = pulseRate->begin();
				itPREnd += endPRSample;
				if (itPREnd >= pulseRate->end()) {
					return;
				}

				float cntRamp = *itPRStart * detectionCriteria.pulseRatePeakIncrease;

				if (CDataSet::findCountRamp(itPRStart, itPREnd, cntRamp)) {

					//---Score Arousal
					if ((timeOfMaxAdm - halfW) > lastArousalEnd) {

						//--Record the event
						short bpos = bpData->getSize() ? bpData->getPos(timeOfMaxAdm - detectionCriteria.halfArousalLength,
							timeOfMaxAdm + detectionCriteria.halfArousalLength) :
							posUndefined;
						CArousalEvnt *eva = new CArousalEvnt(timeOfMaxAdm - detectionCriteria.halfArousalLength,
							timeOfMaxAdm + detectionCriteria.halfArousalLength,
							startRecordingClockTime, bpos);
						CArousalEvnt *evaCopy = new CArousalEvnt(timeOfMaxAdm - detectionCriteria.halfArousalLength,
							timeOfMaxAdm + detectionCriteria.halfArousalLength,
							startRecordingClockTime, bpos);
						CRERAEvnt *reraP = NULL;
						lastArousalEnd = timeOfMaxAdm + detectionCriteria.halfArousalLength;
						//---Add event later (after RERA detection)

						//---Adjust the time cntr for next loop
						theTime = timeOfMaxAdm + detectionCriteria.halfArousalLength + timeStep;

						//---Study RERA
						float startSeekTime = timeOfMaxAdm - detectionCriteria.REIncreaseDuration - detectionCriteria.RESeekSlack;
						float stopSeekTime = startSeekTime + detectionCriteria.REIncreaseDuration;
						vector <FLOAT>::iterator reraSeekStartIt = lower_bound(poesTime->begin(), poesTime->end(), startSeekTime);
						vector <FLOAT>::iterator reraSeekEndIt = lower_bound(poesTime->begin(), poesTime->end(), stopSeekTime);
						bool foundIncrease = false;
						for (float step = .0f; step < detectionCriteria.RESeekSlack;
						step += detectionCriteria.RESeekStepSize,
							startSeekTime += detectionCriteria.RESeekStepSize,
							stopSeekTime += detectionCriteria.RESeekStepSize) {

							reraSeekStartIt = lower_bound(poesTime->begin(), poesTime->end(), startSeekTime);
							reraSeekEndIt = lower_bound(poesTime->begin(), poesTime->end(), stopSeekTime);

							int startPoesWindow = distance(poesTime->begin(), reraSeekStartIt);
							int endPoesWindow = distance(poesTime->begin(), reraSeekEndIt);

							vector <FLOAT>::iterator itStartPoesWindow = poes->begin();
							vector <FLOAT>::iterator itEndPoesWindow = poes->begin();
							itStartPoesWindow += startPoesWindow;
							itEndPoesWindow += endPoesWindow;

							float a, b, rSq;
							bool test = CDataSet::linReg(reraSeekStartIt, reraSeekEndIt, itStartPoesWindow, itEndPoesWindow, &a, &b, &rSq);

							if (test) {
								if ((b > detectionCriteria.p0RiseCoefficientLimit) && (rSq > detectionCriteria.linRegRSquaredLimit)) foundIncrease = true;
								else {
									foundIncrease = false;
								}
							}
							if (foundIncrease) break;
						}
						if (foundIncrease) {

							//---Score RERA - avoid overlap with previous
							if (startSeekTime > lastRERAStopTime) {
								short bpos = bpData->getSize() ? bpData->getPos(startSeekTime,
									timeOfMaxAdm) : posUndefined;
								reraP = new CRERAEvnt(startSeekTime, eva->getTo(), /*timeOfMaxAdm,*/
									startRecordingClockTime, bpos);
								lastRERAStopTime = eva->getTo();

								flowDataToEvent(reraP, cathData->getFlowVector(),
									cathData->getFlowVectorTime(),
									*reraSeekStartIt, *reraSeekEndIt);
								gradientDataToEvent(reraP, cathData->getPgradientVector(),
									cathData->getPgradientVectorTime(),
									*reraSeekStartIt, *reraSeekEndIt);
								poesEnvDataToEvent((CEvnt *)reraP, poes, poesTime, *reraSeekStartIt, *reraSeekEndIt);
								pphEnvDataToEvent((CEvnt *)reraP, pph, pphTime, *reraSeekStartIt, *reraSeekEndIt);
								reraP->doLevelAnalysis();
							}
						}
						eva->setRERAEvnt(reraP);
						arousalEventArrayDesat3.Add(eva);	// Add to both. Initially there is no difference
						evaCopy->copyFrom(eva);
						arousalEventArrayDesat4.Add(evaCopy);
					}
				}
			}
		}
	}
}

/*
Description: Awake analysis based on all available data
Returns: true if densitySumm vector contains data, false if not
*/
bool CEvents::doAdvancedAwakeAnalysis(void)
{
	if (!doAwakeFromCmdLine) return false;

	vector <FLOAT> *vtac = torsoAcData->getAcVector();
	vector <FLOAT> *vac = acData->getAcVector();

	if (0 == vtac->size()) return false;

	if (0 == densitySumm.size()) return false;

	ASSERT(densityTime.size() == densitySumm.size());

	if (densityTime.size() <= 2) return false;
	
	//---Reset
	int num = awakeEventArray.GetCount();
	for (int i = num - 1; i >= 0; i--) {
		CAwakeEvnt *eP = awakeEventArray.GetAt(i);
		delete eP;
	}
	awakeEventArray.RemoveAll();

	vector <FLOAT>::iterator iT = densityTime.begin();
	vector <FLOAT>::iterator iTlast = densityTime.end() - 1;
	vector <FLOAT>::iterator iS = densitySumm.begin();
	bool isAwake = false;
	FLOAT start = .0f;
	FLOAT stop = .0f;
	for (; iT < densityTime.end(); ++iT, ++iS) {
		if (!isAwake) {
			if (*iS > .0f) {
				isAwake = true;
				start = *iT;
			}
		}
		else {
			if ((*iS <= .0f) || (iT == iTlast)) {
				isAwake = false;
				stop = *iT;
				short bpos = bpData->getPos(start,stop);
				CAwakeEvnt *aeP = new CAwakeEvnt(start, stop, startRecordingClockTime, bpos);
				awakeEventArray.Add(aeP);
			}
		}
	}
	return true;
}

/*
Description: Conventional awake analysis based on actimetry alone
*/
void CEvents::doAwakeAnalysis(void)
{
	vector <FLOAT> *vtac = torsoAcData->getAcVector();
	vector <FLOAT> *vac = acData->getAcVector();
	if (0 == vtac->size()) return;

	if (!doAwakeFromCmdLine) return;

	vector <FLOAT>::iterator vi,bi,timei;
	vector <FLOAT> *timeP = acData->getWindowedAcTimeVector();
	vector <FLOAT> *vP = acData->getWindowedAcVector();
	vector <FLOAT> *baseP = acData->getBaselineVector();
	if (0 == timeP->size()) return;

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

	int dist0 = 0;
	int dist1 = timeP->size() - 1;

	bool awake = false;
	bi = baseP->begin() + dist0;
	timei = timeP->begin() + dist0;
	float first,last;
	first = last = .0f;

	if (dist1 > dist0) {
		vi = vP->begin() + dist0;
		if (*vi > detectionCriteria.actimeterAlwaysAwakeLimit) {
			if (!awake) first = *timei;
			awake = true;
		}

		for (vi = vP->begin() + dist0 ; vi < vP->begin() + dist1 ; vi++ , bi++, timei++) {
			
			if (*vi > (*bi + detectionCriteria.actimeterAboveBaselineLimit)) {
				if (!awake) first = *timei;
				awake = true;
			}
			else if (awake) {
				last = *timei;
				short bpos = bpData->getPos(first,last);
				CAwakeEvnt *aeP = new CAwakeEvnt(first,last,startRecordingClockTime,bpos);
				awakeEventArray.Add(aeP);
				awake = false;
			}
		}
		if (awake) {
			last = *(timei - 1);
			short bpos = bpData->getPos(first,last);
			CAwakeEvnt *aeP = new CAwakeEvnt(first,last,startRecordingClockTime,bpos);
			awakeEventArray.Add(aeP);
		}
	}

	//---Force awake when body pos is upright
	vector <float> *_bpv = bpData->getBpVector();
	vector <float> *_bptime = bpData->getTimeaxisVector();
	if (_bpv->size()) {
		vector <float>::iterator bIt,tIt;
		float raiseTime,lieDownTime;
		bool didRaise = false;
		tIt = _bptime->begin();
		for (bIt = _bpv->begin() ; bIt < _bpv->end() ; ++bIt, ++tIt) {
			if ((posUpright == (short) *bIt) && (!didRaise)) {
				raiseTime = *tIt;
				didRaise = true;
			}
			else if ((posUpright == (short) *bIt) && (didRaise)) {
				lieDownTime = *tIt;
				didRaise = false;
				CAwakeEvnt *aeP = new CAwakeEvnt(raiseTime,lieDownTime,startRecordingClockTime,posUpright);
				awakeEventArray.Add(aeP);
			}
		}
	}
	
	//---Sort
	vector <EVENT_SORT_VECTOR_ELEMENT> eventsToSort;
	int num0 = awakeEventArray.GetCount();
	for (int i = 0; i < num0; ++i) {
		CAwakeEvnt *eP = awakeEventArray.GetAt(i);
		EVENT_SORT_VECTOR_ELEMENT evv;
		evv.begin = eP->getFrom();
		evv.end = eP->getTo();
		evv.evP = (void *) NULL;			// Do not need this, we will delete all old awake events anyway
		evv.level = levelTypeUndef;			// Do not need this
		eventsToSort.push_back(evv);
		delete(eP);
	}
	sort(eventsToSort.begin(), eventsToSort.end(), compareEventsInTime);
	awakeEventArray.RemoveAll();

	//---Remove enclosed events
	/*
		Set up a vector. All FALSE. Then set elements to TRUE when not awake and not in excluded array
	*/
	vector <int> onOff;
	size_t secondsLong = (size_t) timeP->at(timeP->size() - 1);
	onOff.resize(secondsLong, FALSE);
	vector <int>::iterator onOffIt = onOff.begin();
	for (; onOffIt < onOff.end() ; ++onOffIt) {
		vector <EVENT_SORT_VECTOR_ELEMENT>::iterator it = eventsToSort.begin();
		for (; it < eventsToSort.end(); ++it) {
			float time = (float) distance(onOff.begin(), onOffIt);
			if ((time < it->end) && (time >= it->begin)) {
				if (!getIsExcluded(time) && !getIsExcluded(time)) 
					*onOffIt = TRUE;
				break;
			}
		}
	}

	//---Build new awake array
	onOffIt = onOff.begin();
	bool inEvent = false;
	float start, stop;
	start = stop = .0f;
	for (; onOffIt < onOff.end(); ++onOffIt) {
		if (!inEvent && *onOffIt) {
			inEvent = true;
			start = (float)distance(onOff.begin(), onOffIt);
		}
		else if (inEvent && !*onOffIt) {
			inEvent = false;
			stop = (float)distance(onOff.begin(), onOffIt);
			short bpos = bpData->getPos(start, stop);
			CAwakeEvnt *aeP = new CAwakeEvnt(start, stop, startRecordingClockTime, bpos);
			awakeEventArray.Add(aeP);
		}
	}
}

void CEvents::doSnoringAnalysis(void)
{
	vector <FLOAT> *snoring = cmData->getCmVector();
	vector <FLOAT> *snoringTime = cmData->getTimeaxisVector();
	ASSERT(snoring->size() == snoringTime->size());

	if (0 == snoring->size()) return;
	
	//---Reset first
	int num = snoringEventArray.GetCount();
	for (int i = num - 1; i >= 0; i--) {
		CSnoringEvnt *eP = snoringEventArray.GetAt(i);
		delete eP;
	}
	snoringEventArray.RemoveAll();

	vector <FLOAT>::iterator snIt,timeIt;
	float startTime = .0f;
	float endTime = .0f;
	snIt = snoring->begin();
	
	timeIt = snoringTime->begin();

	bool snDetect = false;
	if (snDetect) startTime = effectiveStartTime;
	for ( ; snIt < snoring->end() ; snIt++ , timeIt++) {
		float upper95 = CDataSet::getUpper95PercentValue(	*timeIt - detectionCriteria.snoringAverageWindowLength / 2.0f,
											*timeIt + detectionCriteria.snoringAverageWindowLength / 2.0f,snoring,snoringTime);
		if (snDetect) {
			if ((upper95 <= detectionCriteria.contactMicSnoringThreshold)) {
				snDetect = false;
				endTime = *timeIt;
				short bpos = bpData->getSize() ? bpData->getPos(startTime,endTime) : posUndefined;
				CSnoringEvnt *ev = new CSnoringEvnt(startTime,endTime,startRecordingClockTime,bpos);
				flowDataToEvent(ev,		cathData->getFlowVector(),
										cathData->getFlowVectorTime(),
										startTime,endTime);
				gradientDataToEvent(ev,	cathData->getPgradientVector(),
										cathData->getPgradientVectorTime(),
										startTime,endTime);
				bodyDataToEvent(ev,		bpData->getBpVector(),
										bpData->getTimeaxisVector(),
										startTime,endTime);	
				contactMicDataToEvent(ev,	cmData->getCmVector(),
											cmData->getTimeaxisVector(),
											startTime,endTime);	
				airMicDataToEvent(ev,	amData->getAm1Vector(),
										amData->getAm2Vector(),
										amData->getAm3Vector(),
										amData->getAm4Vector(),
										amData->getAmSumVector(),
										amData->getTimeaxisVector(),
										startTime,endTime);
				poesEnvDataToEvent((CEvnt *)ev, cathData->getPOESEnvVector(), cathData->getPOESEnvVectorTime(), startTime, endTime);
				pphEnvDataToEvent((CEvnt *)ev, cathData->getPPHEnvVector(), cathData->getPPHEnvVectorTime(), startTime, endTime);
				ev->doLevelAnalysis();
				ev->sumUpTimeInPositions();
				ev->sumUpTimeAtLevels();
				snoringEventArray.Add(ev);
			}
		}
		else {
			if ((upper95 > detectionCriteria.contactMicSnoringThreshold)) {
				snDetect = true;
				startTime = *timeIt;
			}
		}
	}
	if (snDetect) {
		snDetect = false;
		endTime = *max_element(snoringTime->begin(),snoringTime->end());
		short bpos = bpData->getSize() ? bpData->getPos(startTime,endTime) : posUndefined;
		CSnoringEvnt *ev = new CSnoringEvnt(startTime,endTime,startRecordingClockTime,bpos);
		flowDataToEvent(ev,	cathData->getFlowVector(),
			cathData->getFlowVectorTime(),
			startTime,endTime);
		gradientDataToEvent(ev,	cathData->getPgradientVector(),
			cathData->getPgradientVectorTime(),
			startTime,endTime);
		bodyDataToEvent(ev, bpData->getBpVector(),
			bpData->getTimeaxisVector(),
			startTime, endTime);
		contactMicDataToEvent(ev, bpData->getBpVector(),
			bpData->getTimeaxisVector(),
			startTime, endTime);
		airMicDataToEvent(ev, amData->getAm1Vector(),
			amData->getAm2Vector(),
			amData->getAm3Vector(),
			amData->getAm4Vector(),
			amData->getAmSumVector(),
			amData->getTimeaxisVector(),
			startTime, endTime);
		poesEnvDataToEvent((CEvnt *)ev, cathData->getPOESEnvVector(), cathData->getPOESEnvVectorTime(), startTime, endTime);
		pphEnvDataToEvent((CEvnt *)ev, cathData->getPPHEnvVector(), cathData->getPPHEnvVectorTime(), startTime, endTime);
		ev->doLevelAnalysis();
		ev->sumUpTimeInPositions();
		ev->sumUpTimeAtLevels();
		snoringEventArray.Add(ev);
	}
	fillSnoringVector();
}

void CEvents::doManualMarkerAnalysis(void)
{
}

/*
Description:
	Combine Hypo with central, obstr, mixed
	Also excluded parts of the recording
*/
void CEvents::removeHypoWhenObstrCentralMixEvent(int _spO2DropLimit)
{
	int numh;
	
	if (spO2DropLimit3 == _spO2DropLimit) {
		numh = hypoEventArrayDesat3.GetCount();
		for (int i = numh - 1; i >= 0; --i) {
			CHypoEvnt *eH = hypoEventArrayDesat3.GetAt(i);
			float hFrom = eH->getFrom();
			float hTo = eH->getTo();
			if (getCollisionWithCentral(hFrom, hTo)) {
				hypoEventArrayDesat3.RemoveAt(i);
				delete eH;
			}
			else if (getCollisionWithMix(hFrom, hTo)) {
				hypoEventArrayDesat3.RemoveAt(i);
				delete eH;
			}
			else if (getCollisionWithObstr(hFrom, hTo)) {
				hypoEventArrayDesat3.RemoveAt(i);
				delete eH;
			}
		}
	}
	else if (spO2DropLimit4 == _spO2DropLimit) {
		numh = hypoEventArrayDesat4.GetCount();
		for (int i = numh - 1; i >= 0; --i) {
			CHypoEvnt *eH = hypoEventArrayDesat4.GetAt(i);
			float hFrom = eH->getFrom();
			float hTo = eH->getTo();
			if (getCollisionWithCentral(hFrom, hTo)) {
				hypoEventArrayDesat4.RemoveAt(i);
				delete eH;
			}
			else if (getCollisionWithMix(hFrom, hTo)) {
				hypoEventArrayDesat4.RemoveAt(i);
				delete eH;
			}
			else if (getCollisionWithObstr(hFrom, hTo)) {
				hypoEventArrayDesat4.RemoveAt(i);
				delete eH;
			}
		}
	}
}

/*
Description: Enable events depending on collisions with awake or excluded
*/
void CEvents::enableArousals(int _hypoLimit, int _spO2DropLimit)
{
	/*
	Reset first, then set if inside
	*/
	if (spO2DropLimit3 == _spO2DropLimit) {
		int num = arousalEventArrayDesat3.GetCount();
		for (int i = num - 1; i >= 0; --i) {
			CArousalEvnt *eP = arousalEventArrayDesat3.GetAt(i);
			eP->setInExcludedWindow(false);
			eP->setInAwakeWindow(false);
			if (getCollisionWithExcluded(eP->getFrom(), eP->getTo()))
				eP->setInExcludedWindow(true);
			if (getCollisionWithAwake(eP->getFrom(), eP->getTo()))
				eP->setInAwakeWindow(true);
		}
	}

	if (spO2DropLimit4 == _spO2DropLimit) {
		int num = arousalEventArrayDesat4.GetCount();
		for (int i = num - 1; i >= 0; --i) {
			CArousalEvnt *eP = arousalEventArrayDesat4.GetAt(i);
			eP->setInExcludedWindow(false);
			eP->setInAwakeWindow(false);
			if (getCollisionWithExcluded(eP->getFrom(), eP->getTo()))
				eP->setInExcludedWindow(true);
			if (getCollisionWithAwake(eP->getFrom(), eP->getTo()))
				eP->setInAwakeWindow(true);
		}
	}
}

/*
Description: Enable or disable events depending on collisions with awake or excluded
*/
void CEvents::enableHypo(int _hypoLimit, int _spO2DropLimit)
{
	/*
	Reset first, then set if inside
	*/
	int num;
	
	if (spO2DropLimit3 == _spO2DropLimit) {
		num = hypoEventArrayDesat3.GetCount();
		for (int i = num - 1; i >= 0; --i) {
			CHypoEvnt *eP = hypoEventArrayDesat3.GetAt(i);
			if (eP->pickFromDropLevel(_hypoLimit)) {
				eP->setInExcludedWindow(false);
				eP->setInAwakeWindow(false);
				if (getCollisionWithExcluded(eP->getFrom(), eP->getTo()))
					eP->setInExcludedWindow(true);
				if (getCollisionWithAwake(eP->getFrom(), eP->getTo()))
					eP->setInAwakeWindow(true);
			}
		}
		return;
	}

	if (spO2DropLimit4 == _spO2DropLimit) {
		num = hypoEventArrayDesat4.GetCount();
		for (int i = num - 1; i >= 0; --i) {
			CHypoEvnt *eP = hypoEventArrayDesat4.GetAt(i);
			if (eP->pickFromDropLevel(_hypoLimit)) {
				eP->setInExcludedWindow(false);
				eP->setInAwakeWindow(false);
				if (getCollisionWithExcluded(eP->getFrom(), eP->getTo()))
					eP->setInExcludedWindow(true);
				if (getCollisionWithAwake(eP->getFrom(), eP->getTo()))
					eP->setInAwakeWindow(true);
			}
		}
	}
}

/*
Description: Enable or disable events depending on collisions with awake or excluded
*/
void CEvents::enableObstr(void)
{
	//---Excluded and awake
	/*
	Reset first, then set if inside
	*/
	int num = obstrEventArray.GetCount();
	for (int i = num - 1; i >= 0; --i) {
		CObstrEvnt *eP = obstrEventArray.GetAt(i);
		eP->setInExcludedWindow(false);
		eP->setInAwakeWindow(false);
		if (getCollisionWithExcluded(eP->getFrom(), eP->getTo()))
			eP->setInExcludedWindow(true);
		if (getCollisionWithAwake(eP->getFrom(), eP->getTo()))
			eP->setInAwakeWindow(true);
	}
}

/*
Description: Enable or disable events depending on collisions with awake or excluded
*/
void CEvents::enableCentral(void) 
{
	/*
	Reset first, then set if inside
	*/
	int num = centralEventArray.GetCount();
	for (int i = num - 1; i >= 0; --i) {
		CCentralEvnt *eP = centralEventArray.GetAt(i);
		eP->setInExcludedWindow(false);
		eP->setInAwakeWindow(false);
		if (getCollisionWithExcluded(eP->getFrom(), eP->getTo()))
			eP->setInExcludedWindow(true);
		if (getCollisionWithAwake(eP->getFrom(), eP->getTo()))
			eP->setInAwakeWindow(true);
	}
}

/*
Description: Enable or disable events depending on collisions with awake or excluded
*/
void CEvents::enableMixed(void) 
{
	/*
	Reset first, then set if inside
	*/
	int num = mixedEventArray.GetCount();
	for (int i = num - 1; i >= 0; --i) {
		CMixedEvnt *eP = mixedEventArray.GetAt(i);
		eP->setInExcludedWindow(false);
		eP->setInAwakeWindow(false);
		if (getCollisionWithExcluded(eP->getFrom(), eP->getTo()))
			eP->setInExcludedWindow(true);
		if (getCollisionWithAwake(eP->getFrom(), eP->getTo()))
			eP->setInAwakeWindow(true);
	}
}

/*
Description: Enable or disable events depending on collisions with excluded
*/
void CEvents::enableSwallow(void)
{
	/*
	Reset first, then set if inside
	*/
	int num = swallowEventArray.GetCount();
	for (int i = num - 1; i >= 0; --i) {
		CSwallowEvnt *eP = swallowEventArray.GetAt(i);
		eP->setInExcludedWindow(false);
		if (getCollisionWithExcluded(eP->getFrom(), eP->getTo()))
			eP->setInExcludedWindow(true);
	}
}


/*
Description: Enable or disable events depending on collisions with awake or excluded
*/
void CEvents::enableSpO2Drops(int _spO2DropLimit)
{
	/*
	Reset first, then set if inside
	*/
	if (spO2DropLimit3 == _spO2DropLimit) {
		int num = spO2DropEventArray3.GetCount();
		for (int i = num - 1; i >= 0; --i) {
			CSpO2DropEvnt *eP = spO2DropEventArray3.GetAt(i);
			eP->setInExcludedWindow(false);
			eP->setInAwakeWindow(false);
			if (getCollisionWithExcluded(eP->getFrom(), eP->getTo()))
				eP->setInExcludedWindow(true);
			if (getCollisionWithAwake(eP->getFrom(), eP->getTo()))
				eP->setInAwakeWindow(true);
		}
		return;
	}

	if (spO2DropLimit4 == _spO2DropLimit) {
		int num = spO2DropEventArray4.GetCount();
		for (int i = num - 1; i >= 0; --i) {
			CSpO2DropEvnt *eP = spO2DropEventArray4.GetAt(i);
			eP->setInExcludedWindow(false);
			eP->setInAwakeWindow(false);
			if (getCollisionWithExcluded(eP->getFrom(), eP->getTo()))
				eP->setInExcludedWindow(true);
			if (getCollisionWithAwake(eP->getFrom(), eP->getTo()))
				eP->setInAwakeWindow(true);
		}
		return;
	}
}

/*
Description:
Combine this event with other excludec and awake.
*/
void CEvents::deactivateArousalsWhenExcludedOrAwake(int _hypoLimit, int _spO2DropLimit)
{
	CArray <CArousalEvnt *, CArousalEvnt *> *aP = NULL;
	if (spO2DropLimit3 == _spO2DropLimit) aP = &arousalEventArrayDesat3;
	else if (spO2DropLimit4 == _spO2DropLimit) aP = &arousalEventArrayDesat4;
	else return;

	//---Excluded and awake
	/*
	Reset first, then set if inside
	*/
	int num = aP->GetCount();
	for (int i = num - 1; i >= 0; --i) {
		CArousalEvnt *eP = aP->GetAt(i);
		eP->setInExcludedWindow(false);
		eP->setInAwakeWindow(false);
		CRERAEvnt *rP = eP->getRERAEvnt();
		if (rP) {
			rP->setInExcludedWindow(false);
			rP->setInAwakeWindow(false);
			if (getCollisionWithExcluded(rP->getFrom(), eP->getTo())) {
				rP->setInExcludedWindow(true);
				eP->setInExcludedWindow(true);
			}
			if (getCollisionWithAwake(rP->getFrom(), eP->getTo())) {
				rP->setInAwakeWindow(true);
				eP->setInAwakeWindow(true);
			}
		}
		else {
			if (getCollisionWithExcluded(eP->getFrom(), eP->getTo()))
				eP->setInExcludedWindow(true);
			if (getCollisionWithAwake(eP->getFrom(), eP->getTo()))
				eP->setInAwakeWindow(true);
		}
	}
}

/*
Description:
	Combine this event with other events plus effective bedtime.
	Also excluded parts of the recording
*/
void CEvents::removeArousalsWhenAnotherEvent(int _hypoLimit, int _spO2DropLimit)
{
	CArray <CArousalEvnt *, CArousalEvnt *> *aP = NULL;
	if (spO2DropLimit3 == _spO2DropLimit) aP = &arousalEventArrayDesat3;
	else if (spO2DropLimit4 == _spO2DropLimit) aP = &arousalEventArrayDesat4;
	else return;

	//---Mixed
	int num = aP->GetCount();
	for (int i = num - 1 ; i >= 0 ; --i) {
		CArousalEvnt *eP = aP->GetAt(i);
		CRERAEvnt *rP = eP->getRERAEvnt();
		if (rP) {
			if ((getIsInsideMixed(eP->getFrom(), eP->getTo())) || 
				(getCollisionWithMix(rP->getFrom(), rP->getTo()))) {
				aP->RemoveAt(i);
				delete eP;
			}
		}
		else if (getIsInsideMixed(eP->getFrom(),eP->getTo())) {
			aP->RemoveAt(i);
			delete eP;
		}
	}

	//---Central
	num = aP->GetCount();
	for (int i = num - 1 ; i >= 0 ; --i) {
		CArousalEvnt *eP = aP->GetAt(i);
		CRERAEvnt *rP = eP->getRERAEvnt();
		if (rP) {
			if ((getIsInsideCentral(eP->getFrom(), eP->getTo())) ||
				(getCollisionWithCentral(rP->getFrom(), rP->getTo()))) {
				aP->RemoveAt(i);
				delete eP;
			}
		}
		else if (getIsInsideCentral(eP->getFrom(), eP->getTo())) {
			aP->RemoveAt(i);
			delete eP;
		}
	}

	//---Obstructive
	num = aP->GetCount();
	for (int i = num - 1 ; i >= 0 ; --i) {
		CArousalEvnt *eP = aP->GetAt(i);
		CRERAEvnt *rP = eP->getRERAEvnt();
		if (rP) {
			if ((getIsInsideObstructive(eP->getFrom(), eP->getTo())) ||
				(getCollisionWithObstr(rP->getFrom(), rP->getTo()))) {
				aP->RemoveAt(i);
				delete eP;
			}
		}
		else if (getIsInsideObstructive(eP->getFrom(), eP->getTo())) {
			aP->RemoveAt(i);
			delete eP;
		}
	}

	//---Hypopnea
	num = aP->GetCount();
	for (int i = num - 1 ; i >= 0 ; --i) {
		CArousalEvnt *eP = aP->GetAt(i);
		CRERAEvnt *rP = eP->getRERAEvnt();
		if (rP) {
			bool arousalIsInside = getIsInsideHypopnea(eP->getFrom(), eP->getTo(), _hypoLimit, _spO2DropLimit);
			bool reraConflict = getCollisionWithHypo(rP->getFrom(), rP->getTo(), _hypoLimit, _spO2DropLimit);
			if (arousalIsInside || reraConflict) {
				eP->disableDueToCollWithHypo(_hypoLimit);
			}
			/*if (getIsInsideHypopnea(eP->getFrom(), eP->getTo(), _hypoLimit, _spO2DropLimit) ||
				getIsInsideHypopnea(rP->getFrom(), rP->getTo(), _hypoLimit, _spO2DropLimit)) {
				eP->disableDueToCollWithHypo(_hypoLimit);
			}*/
		}
		else if (getIsInsideHypopnea(eP->getFrom(), eP->getTo(), _hypoLimit, _spO2DropLimit)) {
			eP->disableDueToCollWithHypo(_hypoLimit);
		}
	}
}

/*
Description: There are hyponeas detected based on 50%, 40% and 30% drop.
But only one alternative can be active at one time. 
This is a cleaning function.
*/
void CEvents::removeConflictingHypos(int _spO2DropLimit)
{
	CArray <CHypoEvnt *, CHypoEvnt *> *hP = NULL;
	if (spO2DropLimit3 == _spO2DropLimit) hP = &hypoEventArrayDesat3;
	else if (spO2DropLimit4 == _spO2DropLimit) hP = &hypoEventArrayDesat4;
	else return;

	//---50% drops shall not be touched here

	//---40% drops shall be removed if they are in conflict with a 50% drop
	int num = hP->GetCount();
	for (int i = num - 1; i >= 0; i--) {
		CHypoEvnt *hP40 = hP->GetAt(i);
		if (hypopneaDropLimit40 == hP40->getHypoDropLimit()) {
			bool conflict = false;
			float start40 = hP40->getFrom();
			float stop40 = hP40->getTo();
			int num2 = hP->GetCount();
			for (int j = 0; j < num2; j++) {
				CHypoEvnt *hP50 = hP->GetAt(j);
				if (hypopneaDropLimit50 == hP50->getHypoDropLimit()) {
					float start50 = hP50->getFrom();
					float stop50 = hP50->getTo();
					if ((start50 > start40) && (start50 < stop40)) conflict = true;
					if ((stop50 > start40) && (stop50 < stop40)) conflict = true;
					if (conflict) break;
				}
			}
			if (conflict) {
				CHypoEvnt *toDelete = hP->GetAt(i);
				hP->RemoveAt(i);
				delete toDelete;
			}
		}
	}

	//---30% drops shall be removed if they are in conflict with a 50% drop
	num = hP->GetCount();
	for (int i = num - 1; i >= 0; i--) {
		CHypoEvnt *hP30 = hP->GetAt(i);
		if (hypopneaDropLimit30 == hP30->getHypoDropLimit()) {
			bool conflict = false;
			float start30 = hP30->getFrom();
			float stop30 = hP30->getTo();
			int num2 = hP->GetCount();
			for (int j = 0; j < num2; j++) {
				CHypoEvnt *hP50 = hP->GetAt(j);
				if (hypopneaDropLimit50 == hP50->getHypoDropLimit()) {
					float start50 = hP50->getFrom();
					float stop50 = hP50->getTo();
					if ((start50 > start30) && (start50 < stop30)) conflict = true;
					if ((stop50 > start30) && (stop50 < stop30)) conflict = true;
					if (conflict) break;
				}
			}
			if (conflict) {
				CHypoEvnt *toDelete = hP->GetAt(i);
				hP->RemoveAt(i);
				delete toDelete;
			}
		}
	}

	//---30% drops shall be removed if they are in conflict with a 40% drop
	num = hP->GetCount();
	for (int i = num - 1; i >= 0; i--) {
		CHypoEvnt *hP30 = hP->GetAt(i);
		if (hypopneaDropLimit30 == hP30->getHypoDropLimit()) {
			bool conflict = false;
			float start30 = hP30->getFrom();
			float stop30 = hP30->getTo();
			int num2 = hP->GetCount();
			for (int j = 0; j < num2; j++) {
				CHypoEvnt *hP40 = hP->GetAt(j);
				if (hypopneaDropLimit40 == hP40->getHypoDropLimit()) {
					float start40 = hP40->getFrom();
					float stop40 = hP40->getTo();
					if ((start40 > start30) && (start40 < stop30)) conflict = true;
					if ((stop40 > start30) && (stop40 < stop30)) conflict = true;
					if (conflict) break;
				}
			}
			if (conflict) {
				CHypoEvnt *toDelete = hP->GetAt(i);
				hP->RemoveAt(i);
				delete toDelete;
			}
		}
	}
}

/*
Description:
	Combine this event with other events plus effectuve bedtime.
	Also excluded parts of the recording
*/
void CEvents::removeHypoWhenAnotherEvent(int _spO2DropLimit)
{
	//---Excluded and awake
	/*
	Reset first, then set if inside
	*/
	if (spO2DropLimit3 == _spO2DropLimit) {
		int num = hypoEventArrayDesat3.GetCount();
		for (int i = num - 1; i >= 0; --i) {
			CHypoEvnt *eP = hypoEventArrayDesat3.GetAt(i);
			eP->setInExcludedWindow(false);
			eP->setInAwakeWindow(false);
			if (getCollisionWithExcluded(eP->getFrom(), eP->getTo()))
				eP->setInExcludedWindow(true);
			if (getCollisionWithAwake(eP->getFrom(), eP->getTo()))
				eP->setInAwakeWindow(true);
		}
	}
	else if (spO2DropLimit4 == _spO2DropLimit) {
		int num = hypoEventArrayDesat4.GetCount();
		for (int i = num - 1; i >= 0; --i) {
			CHypoEvnt *eP = hypoEventArrayDesat4.GetAt(i);
			eP->setInExcludedWindow(false);
			eP->setInAwakeWindow(false);
			if (getCollisionWithExcluded(eP->getFrom(), eP->getTo()))
				eP->setInExcludedWindow(true);
			if (getCollisionWithAwake(eP->getFrom(), eP->getTo()))
				eP->setInAwakeWindow(true);
		}
	}
}

/*
Description:
	Combine this event with other events plus effectuve bedtime.
	Also excluded parts of the recording
*/
void CEvents::removeObstrWhenAnotherEvent(void)
{
	//---Excluded and awake
	/*
	Reset first, then set if inside
	*/
	int num = obstrEventArray.GetCount();
	for (int i = num - 1; i >= 0; --i) {
		CObstrEvnt *eP = obstrEventArray.GetAt(i);
		eP->setInExcludedWindow(false);
		eP->setInAwakeWindow(false);
		if (getCollisionWithExcluded(eP->getFrom(), eP->getTo()))
			eP->setInExcludedWindow(true);
		if (getCollisionWithAwake(eP->getFrom(), eP->getTo()))
			eP->setInAwakeWindow(true);
	}
}

/*
Description:
	Combine this event with other events plus effectuve bedtime.
	Also excluded parts of the recording
*/
void CEvents::removeCentralWhenAnotherEvent(void)
{
	//---Excluded and awake
	/*
	Reset first, then set if inside
	*/
	int num = centralEventArray.GetCount();
	for (int i = num - 1; i >= 0; --i) {
		CCentralEvnt *eP = centralEventArray.GetAt(i);
		eP->setInExcludedWindow(false);
		eP->setInAwakeWindow(false);
		if (getCollisionWithExcluded(eP->getFrom(), eP->getTo()))
			eP->setInExcludedWindow(true);
		if (getCollisionWithAwake(eP->getFrom(), eP->getTo()))
			eP->setInAwakeWindow(true);
	}
}

/*
Description:
Disable swallow when excluded
*/
void CEvents::disableSwallowWhenExcluded(void)
{
	//---Excluded
	/*
	Reset first, then set if inside
	*/
	int num = swallowEventArray.GetCount();
	for (int i = num - 1; i >= 0; --i) {
		CSwallowEvnt *eP = swallowEventArray.GetAt(i);
		eP->setInExcludedWindow(false);
		if (getCollisionWithExcluded(eP->getFrom(), eP->getTo()))
			eP->setInExcludedWindow(true);
	}
}

void CEvents::disableArousalsWhenAwakeOrExcluded(int _spO2DropLimit)
{
	CArray <CArousalEvnt *, CArousalEvnt *> *aArrayP = NULL;
	if (spO2DropLimit3 == _spO2DropLimit) aArrayP = &arousalEventArrayDesat3;
	if (spO2DropLimit4 == _spO2DropLimit) aArrayP = &arousalEventArrayDesat4;
	
	if (!aArrayP) return;

	int num = aArrayP->GetCount();
	for (int i = num - 1; i >= 0; --i) {
		CArousalEvnt *eP = aArrayP->GetAt(i);
		eP->setInExcludedWindow(false);
		eP->setInAwakeWindow(false);
		if (getCollisionWithAwake(eP->getFrom(), eP->getTo()))
			eP->setInAwakeWindow(true);
		if (getCollisionWithExcluded(eP->getFrom(), eP->getTo()))
			eP->setInExcludedWindow(true);
	}
}

void CEvents::disableReraWhenAwakeOrExcluded(void)
{
	CArray <CArousalEvnt *, CArousalEvnt *> *aArrayP = NULL;
	if (spO2DropLimit3 == detectionCriteria.spO2DropLimit) aArrayP = &arousalEventArrayDesat3;
	if (spO2DropLimit4 == detectionCriteria.spO2DropLimit) aArrayP = &arousalEventArrayDesat4;

	if (!aArrayP) return;

	int num = aArrayP->GetCount();
	for (int i = num - 1; i >= 0; --i) {
		CArousalEvnt *eP = aArrayP->GetAt(i);
		CRERAEvnt *rP = eP->getRERAEvnt();
		if (rP) {
			rP->setInExcludedWindow(false);
			rP->setInAwakeWindow(false);
			if (getCollisionWithAwake(eP->getFrom(), eP->getTo()))
				rP->setInAwakeWindow(true);
			if (getCollisionWithExcluded(eP->getFrom(), eP->getTo()))
				eP->setInExcludedWindow(true);
		}
	}
}

void CEvents::disableHypoWhenAwakeOrExcluded(int _hypoLimit, int _spO2DropLimit)
{
	CArray <CHypoEvnt *, CHypoEvnt *> *hArrayP = NULL;
	if (spO2DropLimit3 == _spO2DropLimit) hArrayP = &hypoEventArrayDesat3;
	if (spO2DropLimit4 == _spO2DropLimit) hArrayP = &hypoEventArrayDesat4;

	if (!hArrayP) return;

	int num = hArrayP->GetCount();
	for (int i = num - 1; i >= 0; --i) {
		CHypoEvnt *eP = hArrayP->GetAt(i);
		if (eP->pickFromDropLevel(_hypoLimit)) {
			eP->setInExcludedWindow(false);
			eP->setInAwakeWindow(false);
			if (getCollisionWithAwake(eP->getFrom(), eP->getTo()))
				eP->setInAwakeWindow(true);
			if (getCollisionWithExcluded(eP->getFrom(), eP->getTo()))
				eP->setInExcludedWindow(true);
		}
	}
}

void CEvents::disableObstrWhenAwakeOrExcluded(void)
{
	int num = obstrEventArray.GetCount();
	for (int i = num - 1; i >= 0; --i) {
		CObstrEvnt *eP = obstrEventArray.GetAt(i);
		eP->setInExcludedWindow(false);
		eP->setInAwakeWindow(false);
		if (getCollisionWithAwake(eP->getFrom(), eP->getTo()))
			eP->setInAwakeWindow(true);
		if (getCollisionWithExcluded(eP->getFrom(), eP->getTo()))
			eP->setInExcludedWindow(true);
	}
}

void CEvents::disableCentralWhenAwakeOrExcluded(void)
{
	int num = centralEventArray.GetCount();
	for (int i = num - 1; i >= 0; --i) {
		CCentralEvnt *eP = centralEventArray.GetAt(i);
		eP->setInExcludedWindow(false);
		eP->setInAwakeWindow(false);
		if (getCollisionWithAwake(eP->getFrom(), eP->getTo()))
			eP->setInAwakeWindow(true);
		if (getCollisionWithExcluded(eP->getFrom(), eP->getTo()))
			eP->setInExcludedWindow(true);
	}
}

void CEvents::disableMixedWhenAwakeOrExcluded(void)
{
	int num = mixedEventArray.GetCount();
	for (int i = num - 1; i >= 0; --i) {
		CMixedEvnt *eP = mixedEventArray.GetAt(i);
		eP->setInExcludedWindow(false);
		eP->setInAwakeWindow(false);
		if (getCollisionWithAwake(eP->getFrom(), eP->getTo()))
			eP->setInAwakeWindow(true);
		if (getCollisionWithExcluded(eP->getFrom(), eP->getTo()))
			eP->setInExcludedWindow(true);
	}
}


void CEvents::disableSpO2DropsWhenAwakeOrExcluded(int _spO2DropLimit)
{
	CArray <CSpO2DropEvnt *, CSpO2DropEvnt *> *sArrayP = NULL;
	if (spO2DropLimit3 == _spO2DropLimit) sArrayP = &spO2DropEventArray3;
	if (spO2DropLimit4 == _spO2DropLimit) sArrayP = &spO2DropEventArray4;

	if (!sArrayP) return;

	int num = sArrayP->GetCount();
	for (int i = num - 1; i >= 0; --i) {
		CSpO2DropEvnt *eP = sArrayP->GetAt(i);
		eP->setInExcludedWindow(false);
		eP->setInAwakeWindow(false);
		if (getCollisionWithAwake(eP->getFrom(), eP->getTo()))
			eP->setInAwakeWindow(true);
		if (getCollisionWithExcluded(eP->getFrom(), eP->getTo()))
			eP->setInExcludedWindow(true);
	}
}

void CEvents::fillAwakeEventArrayFromEventTimelineVector(void)
{
	for (int i = awakeEventArray.GetCount() - 1; i >= 0; --i) {
		CAwakeEvnt *eP = awakeEventArray.GetAt(i);
		delete eP;
	}
	awakeEventArray.RemoveAll();

	vector <UINT>::iterator i0, i1,foundExcluded;
	i0 = eventTimelineVector.begin();
	i1 = eventTimelineVector.begin();
	while ((i0 < eventTimelineVector.end()) && (i1 < eventTimelineVector.end())) {
		i0 = find_if(i1, eventTimelineVector.end(), isAwakeNotExcluded);
		i1 = find_if_not(i0, eventTimelineVector.end(), isAwakeNotExcluded);
		if (i0 < eventTimelineVector.end()) {
			float from = (float)distance(eventTimelineVector.begin(), i0) * EVENT_TIMELINE_TIME_INTERVAL;
			float to = (float)distance(eventTimelineVector.begin(), i1) * EVENT_TIMELINE_TIME_INTERVAL;

			short bpos = bpData ? bpData->getPos(from, to) : posUndefined;
			CAwakeEvnt *aeP = new CAwakeEvnt(from,to, startRecordingClockTime, bpos);

			//---Is excluded part of the event?
			foundExcluded = find_if(i0, i1, isExcluded);
			if (i1 != foundExcluded) aeP->setInExcludedWindow(true);

			awakeEventArray.Add(aeP);
		}
	}
}

void CEvents::fillDesatEventArrayFromEventTimelineVector(void)
{
	CArray <CSpO2DropEvnt *, CSpO2DropEvnt *>	*sArrayP = NULL;
	if (detectionCriteria.spO2DropLimit == spO2DropLimit3) sArrayP = &spO2DropEventArray3;
	else if (detectionCriteria.spO2DropLimit == spO2DropLimit4) sArrayP = &spO2DropEventArray4;

	ASSERT(sArrayP);
	if (!sArrayP) return;

	for (int i = sArrayP->GetCount() - 1; i >= 0; --i) {
		CSpO2DropEvnt *eP = sArrayP->GetAt(i);
		delete eP;
	}
	sArrayP->RemoveAll();

	float dropSize = getFloatSpO2DropLimit();

	vector <UINT>::iterator i0, i1,foundAwake,foundExcluded;
	i0 = eventTimelineVector.begin();
	i1 = eventTimelineVector.begin();
	while ((i0 < eventTimelineVector.end()) && (i1 < eventTimelineVector.end())) {
		i0 = find_if(i1, eventTimelineVector.end(), isDesaturation);			
		i1 = find_if_not(i0, eventTimelineVector.end(), isDesaturation);	
		if (i0 < eventTimelineVector.end()) {
			float from = (float)distance(eventTimelineVector.begin(), i0) * EVENT_TIMELINE_TIME_INTERVAL;
			float to = (float)distance(eventTimelineVector.begin(), i1) * EVENT_TIMELINE_TIME_INTERVAL;

			short bpos = bpData ? bpData->getPos(from, to) : posUndefined;
			float d0 = .0f;
			float d1 = .0f;
			if (oxData) {
				d0 = oxData->getSpO2At(from);
				d1 = oxData->getSpO2At(to);
			}
			CSpO2DropEvnt *eP = new CSpO2DropEvnt(detectionCriteria.spO2DropLimit, dropSize, d0, d1, from, to, startRecordingClockTime, 
															bpos, manualDetection);
			//---Is awake or excluded part of the event?
			foundAwake = find_if(i0, i1, isAwake);
			if (i1 != foundAwake) eP->setInAwakeWindow(true);
			foundExcluded = find_if(i0, i1, isExcluded);
			if (i1 != foundExcluded) eP->setInExcludedWindow(true);
			sArrayP->Add(eP);
		}
	}
}

void CEvents::fillMixedEventArrayFromEventTimelineVector(void)
{
	if (!cathData) return;

	vector <FLOAT> *vpGrad = cathData->getPgradientVector();
	vector <FLOAT> *tpGrad = cathData->getPgradientVectorTime();
	vector <FLOAT> *flow = cathData->getFlowVector();
	vector <FLOAT> *flowTime = cathData->getFlowVectorTime();
	vector <FLOAT> *vpoes = cathData->getPOESEnvVector();
	vector <FLOAT> *poesTime = cathData->getPOESEnvVectorTime();
	vector <FLOAT> *vpph = cathData->getPPHEnvVector();
	vector <FLOAT> *pphTime = cathData->getPPHEnvVectorTime();

	for (int i = mixedEventArray.GetCount() - 1; i >= 0; --i) {
		CMixedEvnt *eP = mixedEventArray.GetAt(i);
		delete eP;
	}
	mixedEventArray.RemoveAll();

	vector <UINT>::iterator i0, i1,foundAwake,foundExcluded;
	i0 = eventTimelineVector.begin();
	i1 = eventTimelineVector.begin();
	while ((i0 < eventTimelineVector.end()) && (i1 < eventTimelineVector.end())) {
		i0 = find_if(i1, eventTimelineVector.end(), isMixed);
		i1 = find_if_not(i0, eventTimelineVector.end(), isMixed);
		if (i0 < eventTimelineVector.end()) {
			float from = (float)distance(eventTimelineVector.begin(), i0) * EVENT_TIMELINE_TIME_INTERVAL;
			float to = (float)distance(eventTimelineVector.begin(), i1) * EVENT_TIMELINE_TIME_INTERVAL;

			UINT levelType = levelTypeUndef;
			if (evntAHLevelUpper & *i0) levelType = levelTypeUpper;
			else if (evntAHLevelMulti & *i0) levelType = levelTypeMulti;
			else if (evntAHLevelLower & *i0) levelType = levelTypeLower;

			short bpos = bpData ? bpData->getPos(from, to) : posUndefined;
			CMixedEvnt *aP = new CMixedEvnt(from, to, startRecordingClockTime, bpos); 
			gradientDataToEvent((CEvnt *)aP, vpGrad, tpGrad, from, to);
			flowDataToEvent((CEvnt *)aP, flow, flowTime, from, to);
			poesEnvDataToEvent((CEvnt *)aP, vpoes, poesTime, from, to);
			pphEnvDataToEvent((CEvnt *)aP, vpph, pphTime, from, to);
			aP->setLevelType(levelType);

			//---Is awake or excluded part of the event?
			foundAwake = find_if(i0, i1, isAwake);
			if (i1 != foundAwake) aP->setInAwakeWindow(true);
			foundExcluded = find_if(i0, i1, isExcluded);
			if (i1 != foundExcluded) aP->setInExcludedWindow(true);

			mixedEventArray.Add(aP);
		}
	}
}

void CEvents::fillCentralEventArrayFromEventTimelineVector(void)
{
	if (!cathData) return;

	vector <FLOAT> *vpGrad = cathData->getPgradientVector();
	vector <FLOAT> *tpGrad = cathData->getPgradientVectorTime();
	vector <FLOAT> *flow = cathData->getFlowVector();
	vector <FLOAT> *flowTime = cathData->getFlowVectorTime();
	vector <FLOAT> *vpoes = cathData->getPOESEnvVector();
	vector <FLOAT> *poesTime = cathData->getPOESEnvVectorTime();
	vector <FLOAT> *vpph = cathData->getPPHEnvVector();
	vector <FLOAT> *pphTime = cathData->getPPHEnvVectorTime();

	for (int i = centralEventArray.GetCount() - 1; i >= 0; --i) {
		CCentralEvnt *eP = centralEventArray.GetAt(i);
		delete eP;
	}
	centralEventArray.RemoveAll();

	vector <UINT>::iterator i0, i1,foundAwake,foundExcluded;
	i0 = eventTimelineVector.begin();
	i1 = eventTimelineVector.begin();
	while ((i0 < eventTimelineVector.end()) && (i1 < eventTimelineVector.end())) {
		i0 = find_if(i1, eventTimelineVector.end(), isCentral);
		i1 = find_if_not(i0, eventTimelineVector.end(), isCentral);
		if (i0 < eventTimelineVector.end()) {
			float from = (float)distance(eventTimelineVector.begin(), i0) * EVENT_TIMELINE_TIME_INTERVAL;
			float to = (float)distance(eventTimelineVector.begin(), i1) * EVENT_TIMELINE_TIME_INTERVAL;

			short bpos = bpData ? bpData->getPos(from, to) : posUndefined;
			CCentralEvnt *aP = new CCentralEvnt(from, to, startRecordingClockTime, bpos);
			gradientDataToEvent((CEvnt *)aP, vpGrad, tpGrad, from, to);
			flowDataToEvent((CEvnt *)aP, flow, flowTime, from, to);
			poesEnvDataToEvent((CEvnt *)aP, vpoes, poesTime, from, to);
			pphEnvDataToEvent((CEvnt *)aP, vpph, pphTime, from, to);

			//---Is awake or excluded part of the event?
			foundAwake = find_if(i0, i1, isAwake);
			if (i1 != foundAwake) aP->setInAwakeWindow(true);
			foundExcluded = find_if(i0, i1, isExcluded);
			if (i1 != foundExcluded) aP->setInExcludedWindow(true);

			centralEventArray.Add(aP);
		}
	}
}

void CEvents::fillObstrEventArrayFromEventTimelineVector(void)
{
	if (!cathData) return;

	vector <FLOAT> *vpGrad = cathData->getPgradientVector();
	vector <FLOAT> *tpGrad = cathData->getPgradientVectorTime();
	vector <FLOAT> *flow = cathData->getFlowVector();
	vector <FLOAT> *flowTime = cathData->getFlowVectorTime();
	vector <FLOAT> *vpoes = cathData->getPOESEnvVector();
	vector <FLOAT> *poesTime = cathData->getPOESEnvVectorTime();
	vector <FLOAT> *vpph = cathData->getPPHEnvVector();
	vector <FLOAT> *pphTime = cathData->getPPHEnvVectorTime();

	for (int i = obstrEventArray.GetCount() - 1; i >= 0; --i) {
		CObstrEvnt *eP = obstrEventArray.GetAt(i);
		delete eP;
	}
	obstrEventArray.RemoveAll();

	vector <UINT>::iterator i0, i1,foundAwake,foundExcluded;
	i0 = eventTimelineVector.begin();
	i1 = eventTimelineVector.begin();
	while ((i0 < eventTimelineVector.end()) && (i1 < eventTimelineVector.end())) {
		i0 = find_if(i1, eventTimelineVector.end(), isObstr);
		i1 = find_if_not(i0, eventTimelineVector.end(), isObstr);
		if (i0 < eventTimelineVector.end()) {
			float from = (float)distance(eventTimelineVector.begin(), i0) * EVENT_TIMELINE_TIME_INTERVAL;
			float to = (float)distance(eventTimelineVector.begin(), i1) * EVENT_TIMELINE_TIME_INTERVAL;

			UINT levelType = levelTypeUndef;
			if (evntAHLevelUpper & *i0) levelType = levelTypeUpper;
			else if (evntAHLevelMulti & *i0) levelType = levelTypeMulti;
			else if (evntAHLevelLower & *i0) levelType = levelTypeLower;

			short bpos = bpData ? bpData->getPos(from, to) : posUndefined;
			CObstrEvnt *aP = new CObstrEvnt(from, to, startRecordingClockTime, bpos);

			gradientDataToEvent((CEvnt *)aP, vpGrad, tpGrad, from, to);
			flowDataToEvent((CEvnt *)aP, flow, flowTime, from, to);
			poesEnvDataToEvent((CEvnt *)aP, vpoes, poesTime, from, to);
			pphEnvDataToEvent((CEvnt *)aP, vpph, pphTime, from, to);

			aP->setLevelType(levelType);

			//---Is awake or excluded part of the event?
			foundAwake = find_if(i0, i1, isAwake);
			if (i1 != foundAwake) aP->setInAwakeWindow(true);
			foundExcluded = find_if(i0, i1, isExcluded);
			if (i1 != foundExcluded) aP->setInExcludedWindow(true);

			obstrEventArray.Add(aP);
		}
	}
}

void CEvents::fillHypoEventArrayFromEventTimelineVector(void)
{
	if (!cathData) return;

	vector <FLOAT> *vpGrad = cathData->getPgradientVector();
	vector <FLOAT> *tpGrad = cathData->getPgradientVectorTime();
	vector <FLOAT> *flow = cathData->getFlowVector();
	vector <FLOAT> *flowTime = cathData->getFlowVectorTime();
	vector <FLOAT> *vpoes = cathData->getPOESEnvVector();
	vector <FLOAT> *poesTime = cathData->getPOESEnvVectorTime();
	vector <FLOAT> *vpph = cathData->getPPHEnvVector();
	vector <FLOAT> *pphTime = cathData->getPPHEnvVectorTime();

	CArray <CHypoEvnt *, CHypoEvnt *>	*hArrayP = NULL;
	if (detectionCriteria.spO2DropLimit == spO2DropLimit3) hArrayP = &hypoEventArrayDesat3;
	else if (detectionCriteria.spO2DropLimit == spO2DropLimit4) hArrayP = &hypoEventArrayDesat4;

	if (!hArrayP) return;

	for (int i = hArrayP->GetCount() - 1; i >= 0; --i) {
		CHypoEvnt *eP = hArrayP->GetAt(i);
		delete eP;
	}
	hArrayP->RemoveAll();

	//---Obstructive
	vector <UINT>::iterator i0, i1,foundAwake,foundExcluded;
	i0 = eventTimelineVector.begin();
	i1 = eventTimelineVector.begin();
	while ((i0 < eventTimelineVector.end()) && (i1 < eventTimelineVector.end())) {
		i0 = find_if(i1, eventTimelineVector.end(), isHypoObstr);
		i1 = find_if_not(i0, eventTimelineVector.end(), isHypoObstr);
		if (i0 < eventTimelineVector.end()) {
			float from = (float)distance(eventTimelineVector.begin(), i0) * EVENT_TIMELINE_TIME_INTERVAL;
			float to = (float)distance(eventTimelineVector.begin(), i1) * EVENT_TIMELINE_TIME_INTERVAL;

			UINT levelType = levelTypeUndef;
			if (evntAHLevelUpper & *i0) levelType = levelTypeUpper;
			else if (evntAHLevelMulti & *i0) levelType = levelTypeMulti;
			else if (evntAHLevelLower & *i0) levelType = levelTypeLower;

			short bpos = bpData ? bpData->getPos(from, to) : posUndefined;
			CHypoEvnt *aP = new CHypoEvnt(from, to, startRecordingClockTime, bpos,detectionCriteria.hypopneaDropLimit);
			
			gradientDataToEvent((CEvnt *)aP, vpGrad, tpGrad, from, to);
			flowDataToEvent((CEvnt *)aP, flow, flowTime, from, to);
			poesEnvDataToEvent((CEvnt *)aP, vpoes, poesTime, from, to);
			pphEnvDataToEvent((CEvnt *)aP, vpph, pphTime, from, to);

			aP->setLevelType(levelType);
			aP->setEventType(evntTypeHypoObstr);
			aP->setHypoType(hypoTypeObstr);

			//---Is awake or excluded part of the event?
			foundAwake = find_if(i0, i1, isAwake);
			if (i1 != foundAwake) aP->setInAwakeWindow(true);
			foundExcluded = find_if(i0, i1, isExcluded);
			if (i1 != foundExcluded) aP->setInExcludedWindow(true);

			hArrayP->Add(aP);
		}
	}

	//---Central
	i0 = eventTimelineVector.begin();
	i1 = eventTimelineVector.begin();
	while ((i0 < eventTimelineVector.end()) && (i1 < eventTimelineVector.end())) {
		i0 = find_if(i1, eventTimelineVector.end(), isHypoCentral);
		i1 = find_if_not(i0, eventTimelineVector.end(), isHypoCentral);
		if (i0 < eventTimelineVector.end()) {
			float from = (float)distance(eventTimelineVector.begin(), i0) * EVENT_TIMELINE_TIME_INTERVAL;
			float to = (float)distance(eventTimelineVector.begin(), i1) * EVENT_TIMELINE_TIME_INTERVAL;

			short bpos = bpData ? bpData->getPos(from, to) : posUndefined;
			CHypoEvnt *aP = new CHypoEvnt(from, to, startRecordingClockTime, bpos, detectionCriteria.hypopneaDropLimit);

			gradientDataToEvent((CEvnt *)aP, vpGrad, tpGrad, from, to);
			flowDataToEvent((CEvnt *)aP, flow, flowTime, from, to);
			poesEnvDataToEvent((CEvnt *)aP, vpoes, poesTime, from, to);
			pphEnvDataToEvent((CEvnt *)aP, vpph, pphTime, from, to);

			aP->setEventType(evntTypeHypoCentral);
			aP->setHypoType(hypoTypeCentral);

			//---Is awake or excluded part of the event?
			foundAwake = find_if(i0, i1, isAwake);
			if (i1 != foundAwake) aP->setInAwakeWindow(true);
			foundExcluded = find_if(i0, i1, isExcluded);
			if (i1 != foundExcluded) aP->setInExcludedWindow(true);

			hArrayP->Add(aP);
		}
	}

	sortAndOrganiseHypo(detectionCriteria.spO2DropLimit);
}

void CEvents::fillSwallowArrayFromEventTimelineVector(void)
{
	for (int i = swallowEventArray.GetCount() - 1; i >= 0; --i) {
		CSwallowEvnt *eP = swallowEventArray.GetAt(i);
		delete eP;
	}
	swallowEventArray.RemoveAll();

	vector <UINT>::iterator i0, i1,foundExcluded;
	i0 = eventTimelineVector.begin();
	i1 = eventTimelineVector.begin();
	while ((i0 < eventTimelineVector.end()) && (i1 < eventTimelineVector.end())) {
		i0 = find_if(i1, eventTimelineVector.end(), isSwallow);
		i1 = find_if_not(i0, eventTimelineVector.end(), isSwallow);
		if (i0 < eventTimelineVector.end()) {
			float from = (float)distance(eventTimelineVector.begin(), i0) * EVENT_TIMELINE_TIME_INTERVAL;
			float to = (float)distance(eventTimelineVector.begin(), i1) * EVENT_TIMELINE_TIME_INTERVAL;

			short bpos = bpData ? bpData->getPos(from, to) : posUndefined;
			CSwallowEvnt *aP = new CSwallowEvnt(from, to, startRecordingClockTime, bpos);

			//---Is excluded part of the event?
			foundExcluded = find_if(i0, i1, isExcluded);
			if (i1 != foundExcluded) aP->setInExcludedWindow(true);

			swallowEventArray.Add(aP);
		}
	}
}

void CEvents::fillReraArrayFromEventTimelineVector(void)
{
	if (!cathData) return;

	vector <FLOAT> *vpGrad = cathData->getPgradientVector();
	vector <FLOAT> *tpGrad = cathData->getPgradientVectorTime();
	vector <FLOAT> *flow = cathData->getFlowVector();
	vector <FLOAT> *flowTime = cathData->getFlowVectorTime();
	vector <FLOAT> *vpoes = cathData->getPOESEnvVector();
	vector <FLOAT> *poesTime = cathData->getPOESEnvVectorTime();
	vector <FLOAT> *vpph = cathData->getPPHEnvVector();
	vector <FLOAT> *pphTime = cathData->getPPHEnvVectorTime();
	
	// Both Arousals and RERAs
	CArray <CArousalEvnt *, CArousalEvnt *> *aArrayP = NULL;
	if (spO2DropLimit3 == detectionCriteria.spO2DropLimit) aArrayP = &arousalEventArrayDesat3;
	if (spO2DropLimit4 == detectionCriteria.spO2DropLimit) aArrayP = &arousalEventArrayDesat4;

	if (!aArrayP) return;

	for (int i = aArrayP->GetCount() - 1; i >= 0; --i) {
		CArousalEvnt *eP = aArrayP->GetAt(i);
		delete eP;
	}
	aArrayP->RemoveAll();
	
	vector <UINT>::iterator i0rera, i1rera,foundAwake,foundExcluded;
	i0rera = eventTimelineVector.begin();
	i1rera = eventTimelineVector.begin();
	while ((i0rera < eventTimelineVector.end()) && (i1rera < eventTimelineVector.end())) {
		i0rera = find_if(i1rera, eventTimelineVector.end(), isRERA);
		i1rera = find_if_not(i0rera, eventTimelineVector.end(), isRERA);
		if (i0rera < eventTimelineVector.end()) {
			vector <UINT>::iterator i0arousal = find_if(i0rera, eventTimelineVector.end(), isArousal);
			vector <UINT>::iterator i1arousal = find_if_not(i0arousal, eventTimelineVector.end(), isArousal);
			if ((i0arousal < i1rera) && (i1arousal < eventTimelineVector.end())) {

				float fromArousal = (float)distance(eventTimelineVector.begin(), i0arousal) * EVENT_TIMELINE_TIME_INTERVAL;
				float toArousal = (float)distance(eventTimelineVector.begin(), i1arousal) * EVENT_TIMELINE_TIME_INTERVAL;

				float fromRera = (float)distance(eventTimelineVector.begin(), i0rera) * EVENT_TIMELINE_TIME_INTERVAL;
				float toRera = (float)distance(eventTimelineVector.begin(), i1rera) * EVENT_TIMELINE_TIME_INTERVAL;

				short bp = bpData ? bpData->getPos(fromArousal, toArousal) : posUndefined;
				CArousalEvnt *aP = new CArousalEvnt(fromArousal, toArousal, startRecordingClockTime, bp, manualDetection);
				short bpos = bpData ? bpData->getPos(fromRera, toRera) : posUndefined;
				CRERAEvnt *rP = new CRERAEvnt(fromRera, toRera, startRecordingClockTime, bpos,manualDetection);

				//---Is awake or excluded part of the arousal event?
				foundAwake = find_if(i0arousal, i1arousal, isAwake);
				if (i1arousal != foundAwake) aP->setInAwakeWindow(true);
				foundExcluded = find_if(i0arousal, i1arousal, isExcluded);
				if (i1arousal != foundExcluded) aP->setInExcludedWindow(true);

				//---Is awake or excluded part of the event?
				foundAwake = find_if(i0rera, i1rera, isAwake);
				if (i1rera != foundAwake) rP->setInAwakeWindow(true);
				foundExcluded = find_if(i0rera, i1rera, isExcluded);
				if (i1rera != foundExcluded) rP->setInExcludedWindow(true);

				UINT levelType = levelTypeUndef;
				if (evntAHLevelUpper & *i0rera) levelType = levelTypeUpper;
				else if (evntAHLevelMulti & *i0rera) levelType = levelTypeMulti;
				else if (evntAHLevelLower & *i0rera) levelType = levelTypeLower;

				rP->setLevelType(levelType);
				flowDataToEvent(rP, flow,flowTime, fromRera, toRera);
				gradientDataToEvent(rP, vpGrad,tpGrad, fromRera, toRera);
				poesEnvDataToEvent((CEvnt *)rP, vpoes, poesTime, fromRera, toRera);
				pphEnvDataToEvent((CEvnt *)rP, vpph, pphTime, fromRera, toRera);

				aP->setRERAEvnt(rP);

				aArrayP->Add(aP);
			}
		}
	}
}

/*
Description: The event time line vector is the result of editing

From the event timeline vector, the snoring event array is re-generated
*/
void CEvents::fillSnoringEventArrayFromEventTimeLineVector(void)
{
	vector <FLOAT> *vpGrad = cathData->getPgradientVector();
	vector <FLOAT> *tpGrad = cathData->getPgradientVectorTime();
	vector <FLOAT> *flow = cathData->getFlowVector();
	vector <FLOAT> *flowTime = cathData->getFlowVectorTime();
	vector <FLOAT> *vpoes = cathData->getPOESEnvVector();
	vector <FLOAT> *poesTime = cathData->getPOESEnvVectorTime();
	vector <FLOAT> *vpph = cathData->getPPHEnvVector();
	vector <FLOAT> *pphTime = cathData->getPPHEnvVectorTime();

	for (int i = snoringEventArray.GetCount() - 1; i >= 0; --i) {
		CSnoringEvnt *sE = snoringEventArray.GetAt(i);
		delete sE;
	}
	snoringEventArray.RemoveAll();

	vector <UINT>::iterator i0, i1;
	i0 = eventTimelineVector.begin();
	i1 = eventTimelineVector.begin();
	while ((i0 < eventTimelineVector.end()) && (i1 < eventTimelineVector.end())) {
		i0 = find_if(i1, eventTimelineVector.end(), isSnoringNotAwakeNotExcluded);			//---Note this. Required for splitting of snoring events
		i1 = find_if_not(i0, eventTimelineVector.end(), isSnoringNotAwakeNotExcluded);		//---Note this. Required for splitting of snoring events
		if (i0 < eventTimelineVector.end()) {
			float from = (float)distance(eventTimelineVector.begin(), i0)	* EVENT_TIMELINE_TIME_INTERVAL;
			float to = (float)distance(eventTimelineVector.begin(), i1)		* EVENT_TIMELINE_TIME_INTERVAL;

			short bpos = bpData ? bpData->getPos(from, to) : posUndefined;
			CSnoringEvnt *sP = new CSnoringEvnt(from, to, startRecordingClockTime, bpos);
			gradientDataToEvent((CEvnt *)sP, vpGrad, tpGrad, from, to);
			flowDataToEvent((CEvnt *)sP, flow, flowTime, from, to);
			bodyDataToEvent(sP, bpData->getBpVector(),
				bpData->getTimeaxisVector(),from, to);
			contactMicDataToEvent(sP, bpData->getBpVector(),
				bpData->getTimeaxisVector(),from, to);
			airMicDataToEvent(sP, amData->getAm1Vector(),
				amData->getAm2Vector(),
				amData->getAm3Vector(),
				amData->getAm4Vector(),
				amData->getAmSumVector(),
				amData->getTimeaxisVector(),
				from, to);
			poesEnvDataToEvent((CEvnt *)sP, vpoes, poesTime, from, to);
			pphEnvDataToEvent((CEvnt *)sP, vpph, pphTime, from, to);
			sP->sumUpTimeInPositions();
			sP->sumUpTimeAtLevels(i0,i1);
			sP->doLevelAnalysis(distance(eventTimelineVector.begin(),i0),i0,i1);

			snoringEventArray.Add(sP);
		}
	}
}

void CEvents::removeRERAWhenAnotherEvent(int _spO2DropLimit)
{
	CArray <CArousalEvnt *, CArousalEvnt *> *aArrayP = NULL;
	if (spO2DropLimit3 == _spO2DropLimit) aArrayP = &arousalEventArrayDesat3;
	if (spO2DropLimit4 == _spO2DropLimit) aArrayP = &arousalEventArrayDesat4;

	ASSERT(aArrayP);

	int numa = aArrayP->GetCount();
	for (int i = numa - 1; i >= 0; --i) {
		CArousalEvnt *eA = aArrayP->GetAt(i);

		CRERAEvnt *eR = eA->getRERAEvnt();
		if (eR) {
			if (getCollisionWithCentral(eR->getFrom(), eR->getTo())) {
				eA->removeRERAEvnt();
			}
			else if (getCollisionWithMix(eR->getFrom(), eR->getTo())) {
				eA->removeRERAEvnt();
			}
			else if (getCollisionWithObstr(eR->getFrom(), eR->getTo())) {
				eA->removeRERAEvnt();
			}
			else {
				if (getCollisionWithHypo(eR->getFrom(), eR->getTo(), hypopneaDropLimit30, _spO2DropLimit)) {
					eR->disableDueToCollWithHypo(hypopneaDropLimit30);
				}
				if (getCollisionWithHypo(eR->getFrom(), eR->getTo(), hypopneaDropLimit40, _spO2DropLimit)) {
					eR->disableDueToCollWithHypo(hypopneaDropLimit40);
				}
				if (getCollisionWithHypo(eR->getFrom(), eR->getTo(), hypopneaDropLimit50, _spO2DropLimit)) {
					eR->disableDueToCollWithHypo(hypopneaDropLimit50);
				}
			}
		}
	}

	//---Against excluded
	int num = aArrayP->GetCount();
	for (int i = num - 1; i >= 0; --i) {
		CArousalEvnt *aP = aArrayP->GetAt(i);
		CRERAEvnt *rE = aP->getRERAEvnt();
		aP->setInExcludedWindow(false);
		if (getCollisionWithExcluded(aP->getFrom(), aP->getTo())) {
			aP->setInExcludedWindow(true);	
		}
		if (rE) {
			rE->setInExcludedWindow(false);
			if (getCollisionWithExcluded(rE->getFrom(), rE->getTo())) {
				rE->setInExcludedWindow(true);
			}
		}
	}

	//---Against awake
	num = aArrayP->GetCount();
	for (int i = num - 1; i >= 0; --i) {
		CArousalEvnt *aP = aArrayP->GetAt(i);
		CRERAEvnt *rE = aP->getRERAEvnt();
		aP->setInAwakeWindow(false);
		if (getCollisionWithAwake(aP->getFrom(), aP->getTo())) {
			aP->setInAwakeWindow(true);
		}
		if (rE) {
			rE->setInAwakeWindow(false);
			if (getCollisionWithAwake(rE->getFrom(), rE->getTo())) {
				rE->setInAwakeWindow(true);
			}
		}
	}
}

/*
Description:
	Combine this event with other events plus effectuve bedtime.
	Also excluded parts of the recording
*/
void CEvents::removeMixedWhenAnotherEvent(void)
{
	//---Excluded and awake
	/*
	Reset first, then set if inside
	*/
	int num = mixedEventArray.GetCount();
	for (int i = num - 1; i >= 0; --i) {
		CMixedEvnt *eP = mixedEventArray.GetAt(i);
		eP->setInExcludedWindow(false);
		eP->setInAwakeWindow(false);
		if (getCollisionWithExcluded(eP->getFrom(), eP->getTo()))
			eP->setInExcludedWindow(true);
		if (getCollisionWithAwake(eP->getFrom(), eP->getTo()))
			eP->setInAwakeWindow(true);
	}
}

void CEvents::removeSnoringWhenAnotherEvent(void)
{
	transform(snoringVector.begin(), snoringVector.end(), snoringVector.begin(), offElementTypeAwake);
	transform(snoringVector.begin(), snoringVector.end(), snoringVector.begin(), offElementTypeExcluded);
	if (visualEventEditingIsActive) {
		transform(editEventTimelineVector.begin(), editEventTimelineVector.end(),
			snoringVector.begin(), snoringVector.begin(), orVectorsWithAwake);
		transform(editEventTimelineVector.begin(), editEventTimelineVector.end(),
			snoringVector.begin(), snoringVector.begin(), orVectorsWithExcluded);
	}
	else {
		transform(eventTimelineVector.begin(), eventTimelineVector.end(),
			snoringVector.begin(), snoringVector.begin(), orVectorsWithAwake);
		transform(eventTimelineVector.begin(), eventTimelineVector.end(),
			snoringVector.begin(), snoringVector.begin(), orVectorsWithExcluded);
	}
}

/*
Description: Check if person is awake at this moment
*/
bool CEvents::getIsAwake(float _atTime)
{
	int sample = (int)(_atTime * EVENT_TIMELINE_TIME_INTERVAL_INVERSE);
	sample = sample >= (int)eventTimelineVector.size() ? (int)eventTimelineVector.size() - 1 : sample;
	sample = sample < 0 ? 0 : sample;

	UINT state = eventTimelineVector.at((unsigned)sample);
	if (!(state & evntTypeAwake)) return false;
	return true;

	//int num = awakeEventArray.GetCount();
	//for (int i = 0 ; i < num ; ++i) {
	//	CAwakeEvnt *eP = awakeEventArray.GetAt(i);
	//	if ((_atTime <= eP->getTo()) && (_atTime >= eP->getFrom())) return true;
	//}
	//return false;
}


/*
Description: Check if this moment is part of an excluded window
*/
bool CEvents::getIsExcluded(float _atTime)
{
	int sample = (int)(_atTime * EVENT_TIMELINE_TIME_INTERVAL_INVERSE);
	sample = sample >= (int)eventTimelineVector.size() ? (int)eventTimelineVector.size() - 1 : sample;
	sample = sample < 0 ? 0 : sample;

	UINT state = eventTimelineVector.at((unsigned)sample);
	if (!(state & evntTypeExcluded)) return false;
	return true;

	//int num = excludedEventArray.GetCount();
	//for (int i = 0 ; i < num ; ++i) {
	//	CExcludedEvnt *eP = excludedEventArray.GetAt(i);
	//	if ((_atTime <= eP->getTo()) && (_atTime >= eP->getFrom())) return true;
	//}
	//return false;
}

bool CEvents::getIsSnoring(float _atTime)
{
	int sample = (int) (_atTime * EVENT_TIMELINE_TIME_INTERVAL_INVERSE);
	sample = sample >= (int) eventTimelineVector.size() ? (int) eventTimelineVector.size() - 1 : sample;
	sample = sample < 0 ? 0 : sample;

	UINT state = eventTimelineVector.at((unsigned) sample);
	if (state & evntTypeAwakeOrExcluded) return false;
	if (!(state & evntTypeSnoring)) return false;
	return true;

	//int num = snoringEventArray.GetCount();
	//for (int i = 0; i < num; ++i) {
	//	CSnoringEvnt *eP = snoringEventArray.GetAt(i);
	//	if ((_atTime <= eP->getTo()) && (_atTime >= eP->getFrom())) return true;
	//}
	//return false;
}

/*
Description:
	Combine this event with other events plus effectuve bedtime.
	Also excluded parts of the recording
*/
void CEvents::removeSpO2DropsWhenAnotherEvent(int _spO2DropLimit)
{
	//---Excluded and awake
	/*
	Reset first, then set if inside
	*/
	if (spO2DropLimit3 == _spO2DropLimit) {
		int num = spO2DropEventArray3.GetCount();
		for (int i = num - 1; i >= 0; --i) {
			CSpO2DropEvnt *eP = spO2DropEventArray3.GetAt(i);
			eP->setInExcludedWindow(false);
			eP->setInAwakeWindow(false);
			if (getCollisionWithExcluded(eP->getFrom(), eP->getTo()))
				eP->setInExcludedWindow(true);
			if (getCollisionWithAwake(eP->getFrom(), eP->getTo()))
				eP->setInAwakeWindow(true);
		}
	}
	else if (spO2DropLimit4 == _spO2DropLimit) {
		int num = spO2DropEventArray4.GetCount();
		for (int i = num - 1; i >= 0; --i) {
			CSpO2DropEvnt *eP = spO2DropEventArray4.GetAt(i);
			eP->setInExcludedWindow(false);
			eP->setInAwakeWindow(false);
			if (getCollisionWithExcluded(eP->getFrom(), eP->getTo()))
				eP->setInExcludedWindow(true);
			if (getCollisionWithAwake(eP->getFrom(), eP->getTo()))
				eP->setInAwakeWindow(true);
		}
	}
}
/*
Description: Creates a timeline vector with PR and SpO2 values plus flags for excluded, awake and validity
Do sum up statistics - parameters that are unaffected by SpO2 drop limit definition
*/
void CEvents::prepareSpO2TimelineVectorAndSumupStatistics(void)
{
	if (!oxData) return;

	OXIMETER_SET set;
	set.pr = .0f;
	set.spO2 = .0f;
	set.state = evntTypeInvalidOximetry;

	oximeterTimelineVector.clear();
	oximeterTimelineVector.resize(eventTimelineVector.size(), set);

	vector <FLOAT> *vspo2 = oxData->getSpO2Vector();
	vector <FLOAT> *vpr = oxData->getPulseRateVector();
	vector <FLOAT> *t = oxData->getTimeaxisVector();
	vector <BOOL> *pd = oxData->getPenDownVector();

	if (t->size() < 2) return;

	ASSERT(vspo2);
	ASSERT(vpr);
	ASSERT(t);
	ASSERT(pd);
	ASSERT(vspo2->size() == t->size());
	ASSERT(pd->size() == t->size());
	ASSERT(vpr->size() == t->size());

	vector <FLOAT>::iterator t0 = t->begin();
	vector <FLOAT>::iterator t1 = t->begin();

	vector <FLOAT>::iterator pr0 = vpr->begin();
	vector <FLOAT>::iterator pr1 = vpr->begin();

	vector <FLOAT>::iterator o20 = vspo2->begin();
	vector <FLOAT>::iterator o21 = vspo2->begin();

	vector <BOOL>::iterator pd0 = pd->begin();
	vector <BOOL>::iterator pd1 = pd->begin();

	t1++;
	pr1++;
	o21++;
	pd1++;

	int sample0 = 0;
	int sample1 = 0;
	for (; t1 < t->end(); ++t0, ++t1, ++pr0, ++pr1, ++o20, ++o21,++pd0,++pd1 ) {
		sample0 = (int)(*t0 * EVENT_TIMELINE_TIME_INTERVAL_INVERSE);
		sample1 = (int)(*t1 * EVENT_TIMELINE_TIME_INTERVAL_INVERSE);
		set.pr = *pr0;
		set.spO2 = *o20;
		set.state = *pd0 ? evntTypeNone : evntTypeInvalidOximetry;
		if ((set.pr == .0f) && (set.spO2 == .0f))
				set.state |= evntTypeInvalidOximetry;
		transform(oximeterTimelineVector.begin() + sample0, oximeterTimelineVector.begin() + sample1,
			oximeterTimelineVector.begin() + sample0,SetValue <OXIMETER_SET> (set));
	}
	vector <UINT>::iterator tli = eventTimelineVector.begin();
	vector <OXIMETER_SET>::iterator oxi = oximeterTimelineVector.begin();
	for (; tli < eventTimelineVector.end(); ++oxi, ++tli) {
		if (*tli & evntTypeExcluded) 
			oxi->state |= evntTypeExcluded;
		if (*tli & evntTypeAwake) 
			oxi->state |= evntTypeAwake;
	}

	//******************************************************
	//   Compute average, median, highest, lowest
	//*****************************************************
	
	//---Reset first
	for (int j = 0; j < numSleepAwakeStates; j++) {
		spO2Res[spO2DropLimit3][j].satAvg = .0f;
		spO2Res[spO2DropLimit3][j].satMedian = .0f;
		spO2Res[spO2DropLimit4][j].satAvg = .0f;
		spO2Res[spO2DropLimit4][j].satMedian = .0f;
		pulseRateResults[j].average = .0f;
		pulseRateResults[j].median = .0f;
		spO2Res[spO2DropLimit3][j].minutesBelowSat90 = .0f;
		spO2Res[spO2DropLimit3][j].percentBelowSat90 = .0f;
		spO2Res[spO2DropLimit3][j].satHighest = .0f;
		spO2Res[spO2DropLimit3][j].timeOfSatHighest = .0f;
		spO2Res[spO2DropLimit4][j].satHighest = .0f;
		spO2Res[spO2DropLimit4][j].timeOfSatHighest = .0f;
	}

	typedef bool(*foobool) (OXIMETER_SET);
	foobool func[3];
	func[0] = isOxAsleep;
	func[1] = isOxAwake;
	func[2] = isOxOverall;

	typedef bool(*foobool2) (OXIMETER_SET,OXIMETER_SET);
	foobool2 funcMaxPR[3];
	funcMaxPR[0] = oxPRgreaterAsleep;
	funcMaxPR[1] = oxPRgreaterAwake;
	funcMaxPR[2] = oxPRgreater;

	foobool2 funcMinPR[3];
	funcMinPR[0] = oxPRsmallerAsleep;
	funcMinPR[1] = oxPRsmallerAwake;
	funcMinPR[2] = oxPRsmaller;

	foobool2 funcMaxSpO2[3];
	funcMaxSpO2[0] = oxSpO2greaterAsleep;
	funcMaxSpO2[1] = oxSpO2greaterAwake;
	funcMaxSpO2[2] = oxSpO2greater;

	foobool2 funcMinSpO2[3];
	funcMinSpO2[0] = oxSpO2smallerAsleep;
	funcMinSpO2[1] = oxSpO2smallerAwake;
	funcMinSpO2[2] = oxSpO2smaller;

	for (int j = 0; j < numSleepAwakeStates; j++) {
		vector <OXIMETER_SET> oxHelpVector;
		int size = count_if(oximeterTimelineVector.begin(), oximeterTimelineVector.end(), func[j]);
		oxHelpVector.resize(size);
		copy_if(oximeterTimelineVector.begin(), oximeterTimelineVector.end(), oxHelpVector.begin(), func[j]);
		OXIMETER_SET meanSet = for_each(oxHelpVector.begin(), oxHelpVector.end(), AverageOximetry());
		sort(oxHelpVector.begin(), oxHelpVector.end(), sortSpO2);
		float lowestSpO2 = size > 0 ? oxHelpVector.at(0).spO2 : .0f;
		float highestSpO2 = size > 0 ? oxHelpVector.at(size - 1).spO2 : 100.0f;
		if (size > 0) {
			float samplesBelow90 = (float) count_if(oxHelpVector.begin(), oxHelpVector.end(), isBelowOx90Percent);
			float percentBelow90 = 100.0f * samplesBelow90 / size;
			float medianSpO2 = oxHelpVector.at(size / 2).spO2;
			sort(oxHelpVector.begin(), oxHelpVector.end(), sortPR);
			float lowestPR = size > 0 ? oxHelpVector.at(0).pr : .0f;
			float highestPR = size > 0 ? oxHelpVector.at(size - 1).pr : 100.0f;
			float medianPR = oxHelpVector.at(size / 2).pr;
			spO2Res[spO2DropLimit3][j].satAvg = meanSet.spO2;
			spO2Res[spO2DropLimit3][j].satMedian = medianSpO2;
			spO2Res[spO2DropLimit4][j].satAvg = meanSet.spO2;
			spO2Res[spO2DropLimit4][j].satMedian = medianSpO2;
			pulseRateResults[j].average = meanSet.pr;
			pulseRateResults[j].median = medianPR;

			spO2Res[spO2DropLimit3][j].minutesBelowSat90 = EVENT_TIMELINE_TIME_INTERVAL * samplesBelow90 / 60.0f;
			spO2Res[spO2DropLimit3][j].percentBelowSat90 = percentBelow90;
			spO2Res[spO2DropLimit4][j].minutesBelowSat90 = EVENT_TIMELINE_TIME_INTERVAL * samplesBelow90 / 60.0f;
			spO2Res[spO2DropLimit4][j].percentBelowSat90 = percentBelow90;

			//---Find max and min SpO2
			size_t minSamples = (size_t) (MIN_TIME_AT_EXTREMAL_SPO2_PR * EVENT_TIMELINE_TIME_INTERVAL_INVERSE);
			
			OXIMETER_SET oxHigh;
			oxHigh.spO2 = highestSpO2;
			oxHigh.pr = highestPR;
			oxHigh.state = 0; 

			OXIMETER_SET oxLow;
			oxLow.spO2 = lowestSpO2;
			oxLow.pr = lowestPR;
			oxLow.state = 0;
			
			//---Max SpO2
			vector <OXIMETER_SET> testVector;
			testVector.resize(minSamples, oxHigh);
			vector <OXIMETER_SET>::iterator iO = oximeterTimelineVector.end();
			while (iO == oximeterTimelineVector.end()) {
				iO = search(oximeterTimelineVector.begin(), oximeterTimelineVector.end(), 
					testVector.begin(), testVector.end(), funcMaxSpO2[j]);
				oxHigh.spO2 -= .5f;
				fill(testVector.begin(), testVector.end(), oxHigh);
			}
			size_t dist = distance(oximeterTimelineVector.begin(), iO);
			if (dist < oximeterTimelineVector.size()) {
				vector <OXIMETER_SET>::iterator iMa = max_element(iO, iO + minSamples, compareOxSpO2);
				spO2Res[spO2DropLimit3][j].satHighest = iMa->spO2;
				spO2Res[spO2DropLimit3][j].timeOfSatHighest = EVENT_TIMELINE_TIME_INTERVAL * (dist + minSamples / 2);
				spO2Res[spO2DropLimit4][j].satHighest = iMa->spO2;
				spO2Res[spO2DropLimit4][j].timeOfSatHighest = EVENT_TIMELINE_TIME_INTERVAL * (dist + minSamples / 2);
			}
			//---Min SpO2
			iO = oximeterTimelineVector.end();
			fill(testVector.begin(), testVector.end(), oxLow);
			while (iO == oximeterTimelineVector.end()) {
				iO = search(oximeterTimelineVector.begin(), oximeterTimelineVector.end(),
					testVector.begin(), testVector.end(), funcMinSpO2[j]);
				oxLow.spO2 += .5f;
				fill(testVector.begin(), testVector.end(), oxLow);
			}
			dist = distance(oximeterTimelineVector.begin(), iO);
			if (dist < oximeterTimelineVector.size()) {
				vector <OXIMETER_SET>::iterator iMi = min_element(iO, iO + minSamples, compareOxSpO2);
				spO2Res[spO2DropLimit3][j].satLowest = iMi->spO2;
				spO2Res[spO2DropLimit3][j].timeOfSatLowest = EVENT_TIMELINE_TIME_INTERVAL * (dist + minSamples / 2);
				spO2Res[spO2DropLimit4][j].satLowest = iMi->spO2;
				spO2Res[spO2DropLimit4][j].timeOfSatLowest = EVENT_TIMELINE_TIME_INTERVAL * (dist + minSamples / 2);
			}

			//---Max PR
			iO = oximeterTimelineVector.end();
			fill(testVector.begin(), testVector.end(), oxHigh);
			while (iO == oximeterTimelineVector.end()) {
				iO = search(oximeterTimelineVector.begin(), oximeterTimelineVector.end(),
					testVector.begin(), testVector.end(), funcMaxPR[j]);
				oxHigh.pr -= .5f;
				fill(testVector.begin(), testVector.end(), oxHigh);
			}
			dist = distance(oximeterTimelineVector.begin(), iO);
			if (dist < oximeterTimelineVector.size()) {
				vector <OXIMETER_SET>::iterator iMa = max_element(iO, iO + minSamples, compareOxSpO2);
				pulseRateResults[j].highest = iMa->pr;
				pulseRateResults[j].timeOfHighest = EVENT_TIMELINE_TIME_INTERVAL * (dist + minSamples / 2);
			}
			//---Min PR
			testVector.resize(minSamples, oxLow);
			iO = oximeterTimelineVector.end();
			fill(testVector.begin(), testVector.end(), oxLow);
			while (iO == oximeterTimelineVector.end()) {
				iO = search(oximeterTimelineVector.begin(), oximeterTimelineVector.end(),
					testVector.begin(), testVector.end(), funcMinPR[j]);
				oxLow.pr += .5f;
				fill(testVector.begin(), testVector.end(), oxLow);
			}
			dist = distance(oximeterTimelineVector.begin(), iO);
			if (dist < oximeterTimelineVector.size()) {
				vector <OXIMETER_SET>::iterator iMi = min_element(iO, iO + minSamples, compareOxSpO2);
				pulseRateResults[j].lowest = iMi->pr;
				pulseRateResults[j].timeOfLowest = EVENT_TIMELINE_TIME_INTERVAL * (dist + minSamples / 2);
			}
		}
	}
}

/*
Description: Index analysis
	RDI
	AHI
	ODI
*/
void CEvents::doIndexAnalysis(int _hypoLimit, int _spO2Drop)
{
	vector <FLOAT> durMix,durObstr,durHypoCentr,durHypoUndef,durHypoObstr,durCentr;

	rdResults[_hypoLimit][_spO2Drop].obstrNum[obstrEvntRERA] = 0;
	if (spO2DropLimit3 == _spO2Drop) {
		int num = arousalEventArrayDesat3.GetCount();
		for (int i = 0; i < num; i++) {
			CArousalEvnt *aP = arousalEventArrayDesat3.GetAt(i);
			CRERAEvnt *rP = aP->getRERAEvnt();
			if (aP->getIsActive(_hypoLimit) && rP) {
				rdResults[_hypoLimit][_spO2Drop].obstrNum[obstrEvntRERA]++;
			}
		}
	}
	if (spO2DropLimit4 == _spO2Drop) {
		int num = arousalEventArrayDesat4.GetCount();
		for (int i = 0; i < num; i++) {
			CArousalEvnt *aP = arousalEventArrayDesat4.GetAt(i);
			CRERAEvnt *rP = aP->getRERAEvnt();
			if (aP->getIsActive(_hypoLimit) && rP) {
				rdResults[_hypoLimit][_spO2Drop].obstrNum[obstrEvntRERA]++;
			}
		}
	}
	
	rdResults[_hypoLimit][_spO2Drop].obstrNum[obstrEvntObstr] = 0;
	rdResults[_hypoLimit][_spO2Drop].obstrTotalDuration[obstrEvntObstr] = .0f;
	durObstr.clear();
	int num = obstrEventArray.GetCount();
	for (int i = 0 ; i < num ; i++) {
		CObstrEvnt *ev = obstrEventArray.GetAt(i);
		if (ev->getIsActive()) {
			rdResults[_hypoLimit][_spO2Drop].obstrNum[obstrEvntObstr]++;
			rdResults[_hypoLimit][_spO2Drop].obstrTotalDuration[obstrEvntObstr] += ev->getLength();
			durObstr.push_back(ev->getLength());
		}
	}
	
	rdResults[_hypoLimit][_spO2Drop].obstrNum[obstrEvntMix] = 0;
	rdResults[_hypoLimit][_spO2Drop].obstrTotalDuration[obstrEvntMix] = .0f;
	durMix.clear();
	num = mixedEventArray.GetCount();
	for (int i = 0 ; i < num ; i++) {
		CMixedEvnt *ev = mixedEventArray.GetAt(i);
		if (ev->getIsActive()) {
			rdResults[_hypoLimit][_spO2Drop].obstrNum[obstrEvntMix]++;
			rdResults[_hypoLimit][_spO2Drop].obstrTotalDuration[obstrEvntMix] += ev->getLength();
			durMix.push_back(ev->getLength());
		}
	}
	
	rdResults[_hypoLimit][_spO2Drop].obstrNum[obstrEvntHypo] = 0;
	rdResults[_hypoLimit][_spO2Drop].obstrTotalDuration[obstrEvntHypo] = .0f;
	durHypoObstr.clear();
	rdResults[_hypoLimit][_spO2Drop].centrNum[centralEvntHypo] = 0;
	rdResults[_hypoLimit][_spO2Drop].centrTotalDuration[centralEvntHypo] = .0f;
	durHypoCentr.clear();
	durHypoUndef.clear();

	if (spO2DropLimit3 == _spO2Drop) {
		num = hypoEventArrayDesat3.GetCount();
		for (int i = 0; i < num; i++) {
			CHypoEvnt *ev = hypoEventArrayDesat3.GetAt(i);
			if ((ev->getIsActive() && ev->pickFromDropLevel(_hypoLimit))) {
				if (hypoTypeObstr == ev->getHypoType()) {
					rdResults[_hypoLimit][_spO2Drop].obstrNum[obstrEvntHypo]++;
					rdResults[_hypoLimit][_spO2Drop].obstrTotalDuration[obstrEvntHypo] += ev->getLength();
					durHypoObstr.push_back(ev->getLength());
				}
				if (hypoTypeCentral == ev->getHypoType()) {
					rdResults[_hypoLimit][_spO2Drop].centrNum[centralEvntHypo]++;
					rdResults[_hypoLimit][_spO2Drop].centrTotalDuration[centralEvntHypo] += ev->getLength();
					durHypoCentr.push_back(ev->getLength());
				}
			}
		}
	}
	else if (spO2DropLimit4 == _spO2Drop) {
		num = hypoEventArrayDesat4.GetCount();
		for (int i = 0; i < num; i++) {
			CHypoEvnt *ev = hypoEventArrayDesat4.GetAt(i);
			if ((ev->getIsActive() && ev->pickFromDropLevel(_hypoLimit))) {
				if (hypoTypeObstr == ev->getHypoType()) {
					rdResults[_hypoLimit][_spO2Drop].obstrNum[obstrEvntHypo]++;
					rdResults[_hypoLimit][_spO2Drop].obstrTotalDuration[obstrEvntHypo] += ev->getLength();
					durHypoObstr.push_back(ev->getLength());
				}
				if (hypoTypeCentral == ev->getHypoType()) {
					rdResults[_hypoLimit][_spO2Drop].centrNum[centralEvntHypo]++;
					rdResults[_hypoLimit][_spO2Drop].centrTotalDuration[centralEvntHypo] += ev->getLength();
					durHypoCentr.push_back(ev->getLength());
				}
			}
		}
	}
	
	rdResults[_hypoLimit][_spO2Drop].centrNum[centralEvntCentral] = 0;
	rdResults[_hypoLimit][_spO2Drop].centrTotalDuration[centralEvntCentral] = .0f;
	durCentr.clear();
	num = centralEventArray.GetCount();
	for (int i = 0 ; i < num ; i++) {
		CCentralEvnt *ev = centralEventArray.GetAt(i);
		if (ev->getIsActive()) {
			rdResults[_hypoLimit][_spO2Drop].centrNum[centralEvntCentral]++;
			rdResults[_hypoLimit][_spO2Drop].centrTotalDuration[centralEvntCentral] += ev->getLength();
			durCentr.push_back(ev->getLength());
		}
	}
	rdResults[_hypoLimit][_spO2Drop].obstrAverageDuration[obstrEvntObstr] = rdResults[_hypoLimit][_spO2Drop].obstrNum[obstrEvntObstr] ?
		rdResults[_hypoLimit][_spO2Drop].obstrTotalDuration[obstrEvntObstr] / rdResults[_hypoLimit][_spO2Drop].obstrNum[obstrEvntObstr] : .0f ;
	rdResults[_hypoLimit][_spO2Drop].obstrAverageDuration[obstrEvntMix] = rdResults[_hypoLimit][_spO2Drop].obstrNum[obstrEvntMix] ?
		rdResults[_hypoLimit][_spO2Drop].obstrTotalDuration[obstrEvntMix] / rdResults[_hypoLimit][_spO2Drop].obstrNum[obstrEvntMix] : .0f ;
	rdResults[_hypoLimit][_spO2Drop].obstrAverageDuration[obstrEvntHypo] = rdResults[_hypoLimit][_spO2Drop].obstrNum[obstrEvntHypo] ?
		rdResults[_hypoLimit][_spO2Drop].obstrTotalDuration[obstrEvntHypo] / rdResults[_hypoLimit][_spO2Drop].obstrNum[obstrEvntHypo] : .0f;

	rdResults[_hypoLimit][_spO2Drop].centrAverageDuration[centralEvntCentral] = rdResults[_hypoLimit][_spO2Drop].centrNum[centralEvntCentral] ?
		rdResults[_hypoLimit][_spO2Drop].centrTotalDuration[centralEvntCentral] / rdResults[_hypoLimit][_spO2Drop].centrNum[centralEvntCentral] : .0f ;
	rdResults[_hypoLimit][_spO2Drop].centrAverageDuration[centralEvntHypo] = rdResults[_hypoLimit][_spO2Drop].centrNum[centralEvntHypo] ?
		rdResults[_hypoLimit][_spO2Drop].centrTotalDuration[centralEvntHypo] / rdResults[_hypoLimit][_spO2Drop].centrNum[centralEvntHypo] : .0f ;

	float totalSleepHours = totalSleepTime / 3600.0f;
	rdResults[_hypoLimit][_spO2Drop].obstrPerHour[obstrEvntObstr] = totalSleepHours == .0f ? .0f :
		(float) rdResults[_hypoLimit][_spO2Drop].obstrNum[obstrEvntObstr] / totalSleepHours;
	rdResults[_hypoLimit][_spO2Drop].obstrPerHour[obstrEvntMix] = totalSleepHours == .0f ? .0f :
		(float) rdResults[_hypoLimit][_spO2Drop].obstrNum[obstrEvntMix] / totalSleepHours;
	rdResults[_hypoLimit][_spO2Drop].obstrPerHour[obstrEvntHypo] = totalSleepHours == .0f ? .0f :
		(float) rdResults[_hypoLimit][_spO2Drop].obstrNum[obstrEvntHypo] / totalSleepHours;
	rdResults[_hypoLimit][_spO2Drop].obstrPerHour[obstrEvntRERA] = totalSleepHours == .0f ? .0f :
		(float) rdResults[_hypoLimit][_spO2Drop].obstrNum[obstrEvntRERA] / totalSleepHours;

	rdResults[_hypoLimit][_spO2Drop].centrPerHour[centralEvntCentral] = totalSleepHours == .0f ? .0f :
		(float) rdResults[_hypoLimit][_spO2Drop].centrNum[centralEvntCentral] / totalSleepHours;
	rdResults[_hypoLimit][_spO2Drop].centrPerHour[centralEvntHypo] = totalSleepHours == .0f ? .0f :
		(float) rdResults[_hypoLimit][_spO2Drop].centrNum[centralEvntHypo] / totalSleepHours;

	//---Max and median
	sort(durMix.begin(),durMix.end());
	sort(durObstr.begin(),durObstr.end());
	sort(durHypoCentr.begin(),durHypoCentr.end());
	sort(durHypoUndef.begin(),durHypoUndef.end());
	sort(durHypoObstr.begin(),durHypoObstr.end());
	sort(durCentr.begin(),durCentr.end());

	rdResults[_hypoLimit][_spO2Drop].obstrMaxDuration[obstrEvntObstr]			= durObstr.size() > 0		? durObstr.at(durObstr.size() - 1)			: .0f ;
	rdResults[_hypoLimit][_spO2Drop].obstrMaxDuration[obstrEvntMix]				= durMix.size() > 0			? durMix.at(durMix.size() - 1)				: .0f ;
	rdResults[_hypoLimit][_spO2Drop].obstrMaxDuration[obstrEvntHypo]			= durHypoObstr.size() > 0	? durHypoObstr.at(durHypoObstr.size() - 1)	: .0f ;
	rdResults[_hypoLimit][_spO2Drop].centrMaxDuration[centralEvntCentral]		= durCentr.size() > 0		? durCentr.at(durCentr.size() - 1)			: .0f ;
	rdResults[_hypoLimit][_spO2Drop].centrMaxDuration[centralEvntHypo]			= durHypoCentr.size() > 0	? durHypoCentr.at(durHypoCentr.size() - 1)	: .0f ;
	
	rdResults[_hypoLimit][_spO2Drop].obstrMedianDuration[obstrEvntObstr]		= getMedian(&durObstr);
	rdResults[_hypoLimit][_spO2Drop].obstrMedianDuration[obstrEvntMix]			= getMedian(&durMix);
	rdResults[_hypoLimit][_spO2Drop].obstrMedianDuration[obstrEvntHypo]		= getMedian(&durHypoObstr);
	rdResults[_hypoLimit][_spO2Drop].centrMedianDuration[centralEvntCentral]	= getMedian(&durCentr);
	rdResults[_hypoLimit][_spO2Drop].centrMedianDuration[centralEvntHypo]		= getMedian(&durHypoCentr);	

	int totalNumber =	rdResults[_hypoLimit][_spO2Drop].obstrNum[obstrEvntObstr] +
						rdResults[_hypoLimit][_spO2Drop].obstrNum[obstrEvntMix] +
						rdResults[_hypoLimit][_spO2Drop].obstrNum[obstrEvntHypo] +
						rdResults[_hypoLimit][_spO2Drop].centrNum[centralEvntCentral] +
						rdResults[_hypoLimit][_spO2Drop].centrNum[centralEvntHypo]; 

	rdResults[_hypoLimit][_spO2Drop].obstrNum[obstrEvntSum] =	rdResults[_hypoLimit][_spO2Drop].obstrNum[obstrEvntObstr] +
										rdResults[_hypoLimit][_spO2Drop].obstrNum[obstrEvntMix] +
										rdResults[_hypoLimit][_spO2Drop].obstrNum[obstrEvntRERA] +
										rdResults[_hypoLimit][_spO2Drop].obstrNum[obstrEvntHypo];
	rdResults[_hypoLimit][_spO2Drop].obstrPerHour[obstrEvntSum] =	rdResults[_hypoLimit][_spO2Drop].obstrPerHour[obstrEvntObstr] +
											rdResults[_hypoLimit][_spO2Drop].obstrPerHour[obstrEvntMix] +
											rdResults[_hypoLimit][_spO2Drop].obstrPerHour[obstrEvntRERA] +
											rdResults[_hypoLimit][_spO2Drop].obstrPerHour[obstrEvntHypo];
	rdResults[_hypoLimit][_spO2Drop].obstrTotalDuration[obstrEvntSum] =	rdResults[_hypoLimit][_spO2Drop].obstrTotalDuration[obstrEvntObstr] +
													rdResults[_hypoLimit][_spO2Drop].obstrTotalDuration[obstrEvntMix] +
													rdResults[_hypoLimit][_spO2Drop].obstrTotalDuration[obstrEvntHypo];

	rdResults[_hypoLimit][_spO2Drop].centrNum[centralEvntSum] =	rdResults[_hypoLimit][_spO2Drop].centrNum[centralEvntCentral] +
											rdResults[_hypoLimit][_spO2Drop].centrNum[centralEvntHypo];
	rdResults[_hypoLimit][_spO2Drop].centrPerHour[centralEvntSum] =	rdResults[_hypoLimit][_spO2Drop].centrPerHour[centralEvntCentral] +
												rdResults[_hypoLimit][_spO2Drop].centrPerHour[centralEvntHypo];
	rdResults[_hypoLimit][_spO2Drop].centrTotalDuration[centralEvntSum] =	rdResults[_hypoLimit][_spO2Drop].centrTotalDuration[centralEvntCentral] +
													rdResults[_hypoLimit][_spO2Drop].centrTotalDuration[centralEvntHypo];

	if (totalSleepHours > .0f) {
		
		reraiResult[_hypoLimit][_spO2Drop] = (float) countREREvents(_hypoLimit, _spO2Drop) / totalSleepHours;
		maiResult = (float) getCountMixEvents() / totalSleepHours;
		caiResult = (float) getCountCenEvents() / totalSleepHours;
		oaiResult = (float) getCountObsEvents() / totalSleepHours;
		ohiResult[_hypoLimit][_spO2Drop] = (float) getNumObstrHypopnea(_hypoLimit,_spO2Drop) / totalSleepHours;
		chiResult[_hypoLimit][_spO2Drop] = (float) getNumCentralHypopnea(_hypoLimit, _spO2Drop) / totalSleepHours;
		oahiResult[_hypoLimit][_spO2Drop] = maiResult + oaiResult + ohiResult[_hypoLimit][_spO2Drop];
		osa_ordiResult[_hypoLimit][_spO2Drop] = oahiResult[_hypoLimit][_spO2Drop] + reraiResult[_hypoLimit][_spO2Drop];
		cahiResult[_hypoLimit][_spO2Drop] = chiResult[_hypoLimit][_spO2Drop] + caiResult;
		rdiResult[_hypoLimit][_spO2Drop] = osa_ordiResult[_hypoLimit][_spO2Drop] + caiResult + chiResult[_hypoLimit][_spO2Drop];
		arousalIndexResult[_hypoLimit][_spO2Drop] = (float) countAroEvents(_hypoLimit,_spO2Drop) / totalSleepHours;
	}
	else {
		reraiResult[_hypoLimit][_spO2Drop] = .0f;
		maiResult	= .0f;
		caiResult	= .0f;
		oaiResult	= .0f;
		ohiResult[_hypoLimit][_spO2Drop]	= .0f;
		chiResult[_hypoLimit][_spO2Drop]	= .0f;
		cahiResult[_hypoLimit][_spO2Drop] = .0f;
		oahiResult[_hypoLimit][_spO2Drop] = .0f;
		osa_ordiResult[_hypoLimit][_spO2Drop] = .0f;
		rdiResult[_hypoLimit][_spO2Drop] = .0f;
		arousalIndexResult[_hypoLimit][_spO2Drop] = .0f;
	}
}

void CEvents::doSnoringSumUp(void)
{
	for (int i = 0; i < levelTypeCount ; i++) {
		snoringResult.timeAtLevel[i] = .0f;
		snoringResult.percentTimeAtLevel[i] = .0f;
	}
	for (int i = 0; i < posCount ; i++) {
		snoringResult.timeInPos[i] = .0f;
		snoringResult.percentTimeInPos[i] = .0f;
	}
	snoringResult.totalSnoringTime = .0f;
	snoringResult.percentSnoring = .0f;

	snoringResult.standardDevAmLower = .0f;
	snoringResult.standardDevAmMulti = .0f;
	snoringResult.standardDevAmUpper = .0f;
	snoringResult.standardDevCmLower = .0f;
	snoringResult.standardDevCmMulti = .0f;
	snoringResult.standardDevCmUpper = .0f;

	snoringResult.meanAmLower = .0f;
	snoringResult.meanAmMulti = .0f;
	snoringResult.meanAmUpper = .0f;
	snoringResult.meanCmLower = .0f;
	snoringResult.meanCmMulti = .0f;
	snoringResult.meanCmUpper = .0f;

	//---Use eventTimelineVector
	snoringResult.timeInPos[posLeft]	= EVENT_TIMELINE_TIME_INTERVAL * count_if(eventTimelineVector.begin(), eventTimelineVector.end(), isSnoringBPLeftNotAwakeExcluded);
	snoringResult.timeInPos[posRight]	= EVENT_TIMELINE_TIME_INTERVAL * count_if(eventTimelineVector.begin(), eventTimelineVector.end(), isSnoringBPRightNotAwakeExcluded);
	snoringResult.timeInPos[posProne]	= EVENT_TIMELINE_TIME_INTERVAL * count_if(eventTimelineVector.begin(), eventTimelineVector.end(), isSnoringBPProneNotAwakeExcluded);
	snoringResult.timeInPos[posSupine]	= EVENT_TIMELINE_TIME_INTERVAL * count_if(eventTimelineVector.begin(), eventTimelineVector.end(), isSnoringBPSupineNotAwakeExcluded);
	snoringResult.timeInPos[posUpright] = EVENT_TIMELINE_TIME_INTERVAL * count_if(eventTimelineVector.begin(), eventTimelineVector.end(), isSnoringBPUprightNotAwakeExcluded);

	snoringResult.timeAtLevel[levelTypeUpper] += EVENT_TIMELINE_TIME_INTERVAL * count_if(eventTimelineVector.begin(), eventTimelineVector.end(), isEvntSnoringLevelUpperNotAwakeExcluded);
	snoringResult.timeAtLevel[levelTypeLower] += EVENT_TIMELINE_TIME_INTERVAL * count_if(eventTimelineVector.begin(), eventTimelineVector.end(), isEvntSnoringLevelLowerNotAwakeExcluded);
	snoringResult.timeAtLevel[levelTypeMulti] += EVENT_TIMELINE_TIME_INTERVAL * count_if(eventTimelineVector.begin(), eventTimelineVector.end(), isEvntSnoringLevelMultiNotAwakeExcluded);

	vector <FLOAT> cmStrengthAtLower, cmStrengthAtMulti, cmStrengthAtUpper;
	vector <FLOAT> amStrengthAtLower, amStrengthAtMulti, amStrengthAtUpper;
	int num = snoringEventArray.GetCount();
	for (int i = 0 ; i < num ; ++i) {
		CSnoringEvnt *eP = snoringEventArray.GetAt(i);
		if (eP->getIsActive()) {
			eP->fillAmStrengthsPerLevel(&eventTimelineVector,&amStrengthAtLower, &amStrengthAtMulti, &amStrengthAtUpper);
			eP->fillCmStrengthsPerLevel(&eventTimelineVector,&cmStrengthAtLower, &cmStrengthAtMulti, &cmStrengthAtUpper);
		}
	}
	
	if (amStrengthAtLower.size() > 0)
		snoringResult.standardDevAmLower = CDataSet::getMeanStandardDev(amStrengthAtLower.begin(), amStrengthAtLower.end(),&snoringResult.meanAmLower);
	if (amStrengthAtMulti.size() > 0)
		snoringResult.standardDevAmMulti = CDataSet::getMeanStandardDev(amStrengthAtMulti.begin(), amStrengthAtMulti.end(),&snoringResult.meanAmMulti);
	if (amStrengthAtUpper.size() > 0) 
		snoringResult.standardDevAmUpper = CDataSet::getMeanStandardDev(amStrengthAtUpper.begin(), amStrengthAtUpper.end(),&snoringResult.meanAmUpper);
	if (cmStrengthAtLower.size() > 0) 
		snoringResult.standardDevCmLower = CDataSet::getMeanStandardDev(cmStrengthAtLower.begin(), cmStrengthAtLower.end(),&snoringResult.meanCmLower);
	if (cmStrengthAtMulti.size() > 0)
		snoringResult.standardDevCmMulti = CDataSet::getMeanStandardDev(cmStrengthAtMulti.begin(), cmStrengthAtMulti.end(),&snoringResult.meanCmMulti);
	if (cmStrengthAtUpper.size() > 0) 
		snoringResult.standardDevCmUpper = CDataSet::getMeanStandardDev(cmStrengthAtUpper.begin(), cmStrengthAtUpper.end(),&snoringResult.meanCmUpper);

    float sum = snoringResult.timeAtLevel[levelTypeUpper] +
		        snoringResult.timeAtLevel[levelTypeLower] +
                snoringResult.timeAtLevel[levelTypeMulti];

    snoringResult.percentTimeAtLevel[levelTypeUpper] = sum == .0f ? .0f : 100.0f * snoringResult.timeAtLevel[levelTypeUpper] / sum;
    snoringResult.percentTimeAtLevel[levelTypeLower] = sum == .0f ? .0f : 100.0f * snoringResult.timeAtLevel[levelTypeLower] / sum;
    snoringResult.percentTimeAtLevel[levelTypeMulti] = sum == .0f ? .0f : 100.0f * snoringResult.timeAtLevel[levelTypeMulti] / sum;

    float sum2 = snoringResult.timeInPos[posSupine] +
                 snoringResult.timeInPos[posLeft]   +
                 snoringResult.timeInPos[posRight]  +
                 snoringResult.timeInPos[posProne];

    snoringResult.percentTimeInPos[posSupine] = sum2 == .0f ? .0f : 100.0f * snoringResult.timeInPos[posSupine] / sum2;
    snoringResult.percentTimeInPos[posLeft]   = sum2 == .0f ? .0f : 100.0f * snoringResult.timeInPos[posLeft]   / sum2;
    snoringResult.percentTimeInPos[posRight]  = sum2 == .0f ? .0f : 100.0f * snoringResult.timeInPos[posRight]  / sum2;
    snoringResult.percentTimeInPos[posProne]  = sum2 == .0f ? .0f : 100.0f * snoringResult.timeInPos[posProne]  / sum2;
                 
	snoringResult.totalSnoringTime = EVENT_TIMELINE_TIME_INTERVAL * count_if(eventTimelineVector.begin(), eventTimelineVector.end(), isSnoringNotAwakeExcluded);
	float slT = getTotalSleepTime();
	snoringResult.percentSnoring = slT == .0f ? .0f : 100.0f * snoringResult.totalSnoringTime / slT;
}


/*
Description: Does the level sum up. Distibution of all event in level
*/
void CEvents::doOverallLevelSumUp(int _hypoLimit, int _spO2Drop)
{
	//---Reset
	for (int i = 0; i < evCountWithLevel; i++) {
		for (int j = 0; j < levelTypeCount; j++) {
			lvlResults[_hypoLimit][_spO2Drop].num[i][j] = 0;
		}
	}

	//---Number counting
	int num = obstrEventArray.GetCount();
	for (int i = 0 ; i < num ; i++) {
		CObstrEvnt *ev = obstrEventArray.GetAt(i);
		if (ev->getIsActive())
			lvlResults[_hypoLimit][_spO2Drop].num[evObstr][ev->getLevelType()]++;
	}
	num = mixedEventArray.GetCount();
	for (int i = 0 ; i < num ; i++) {
		CMixedEvnt *ev = mixedEventArray.GetAt(i);
		if (ev->getIsActive())
			lvlResults[_hypoLimit][_spO2Drop].num[evMix][ev->getLevelType()]++;
	}
	num = snoringEventArray.GetCount();
	for (int i = 0 ; i < num ; i++) {
		CSnoringEvnt *ev = snoringEventArray.GetAt(i);
		if (ev->getIsActive())
			lvlResults[_hypoLimit][_spO2Drop].num[evSnoringAllLevels][ev->getLevelType()]++;
	}

	if (spO2DropLimit3 == _spO2Drop) {
		num = arousalEventArrayDesat3.GetCount();
		for (int i = 0; i < num; i++) {
			CArousalEvnt *aP = arousalEventArrayDesat3.GetAt(i);
			CRERAEvnt *ev = aP->getRERAEvnt();
			if (aP->getIsActive(_hypoLimit) && ev)
				lvlResults[_hypoLimit][_spO2Drop].num[evRERA][ev->getLevelType()]++;
		}
	}
	if (spO2DropLimit4 == _spO2Drop) {
		num = arousalEventArrayDesat4.GetCount();
		for (int i = 0; i < num; i++) {
			CArousalEvnt *aP = arousalEventArrayDesat4.GetAt(i);
			CRERAEvnt *ev = aP->getRERAEvnt();
			if (aP->getIsActive(_hypoLimit) && ev)
				lvlResults[_hypoLimit][_spO2Drop].num[evRERA][ev->getLevelType()]++;
		}
	}

	if (spO2DropLimit3 == _spO2Drop) {
		num = hypoEventArrayDesat3.GetCount();
		for (int i = 0; i < num; i++) {
			CHypoEvnt *ev = hypoEventArrayDesat3.GetAt(i);
			if ((hypoTypeObstr == ev->getHypoType() && (ev->pickFromDropLevel(_hypoLimit))))
				if (ev->getIsActive())
					lvlResults[_hypoLimit][_spO2Drop].num[evObstrHypo][ev->getLevelType()]++;
		}
	}
	else if(spO2DropLimit4 == _spO2Drop) {
		num = hypoEventArrayDesat4.GetCount();
		for (int i = 0; i < num; i++) {
			CHypoEvnt *ev = hypoEventArrayDesat4.GetAt(i);
			if ((hypoTypeObstr == ev->getHypoType() && (ev->pickFromDropLevel(_hypoLimit))))
				if (ev->getIsActive())
					lvlResults[_hypoLimit][_spO2Drop].num[evObstrHypo][ev->getLevelType()]++;
		}
	}

	//---Sum up
	float hoursSleep = totalSleepTime / 3600.0f;
    float sumAll = .0f;
	for (int lv = 0 ; lv < levelTypeSum ; lv++) {
		int sum = 0;
		for (int oe = 0 ; oe < evSnoringAllLevels; oe++) {
			sum += lvlResults[_hypoLimit][_spO2Drop].num[oe][lv];
			lvlResults[_hypoLimit][_spO2Drop].perHour[oe][lv] = hoursSleep > .0f ? 
				(float) lvlResults[_hypoLimit][_spO2Drop].num[oe][lv] / hoursSleep : .0f;
		}
		lvlResults[_hypoLimit][_spO2Drop].totalNum[lv] = sum;
        sumAll += (float) sum;
		lvlResults[_hypoLimit][_spO2Drop].totalPerHour[lv] = hoursSleep > .0f ? (float) sum / hoursSleep : .0f;
	}

	lvlResults[_hypoLimit][_spO2Drop].percentTotalNum[levelTypeUpper] = sumAll == .0f ? .0f : 100.0f * (float) lvlResults[_hypoLimit][_spO2Drop].totalNum[levelTypeUpper] / sumAll;
	lvlResults[_hypoLimit][_spO2Drop].percentTotalNum[levelTypeMulti] = sumAll == .0f ? .0f : 100.0f * (float) lvlResults[_hypoLimit][_spO2Drop].totalNum[levelTypeMulti] / sumAll;
	lvlResults[_hypoLimit][_spO2Drop].percentTotalNum[levelTypeLower] = sumAll == .0f ? .0f : 100.0f * (float) lvlResults[_hypoLimit][_spO2Drop].totalNum[levelTypeLower] / sumAll;
}


/*
Description: Does the body position analysis. Distibution of all event in level and body position
*/
void CEvents::doBodyPosAnalysis(int _hypoLimit, int _spO2Drop)
{
	if (!bpData) return;

	//---Total time
	bpData->computeTimeInPositions(&excludedEventArray);
	bodyPosResults[_hypoLimit][_spO2Drop].time[posLeft]			= bpData->getTimeInPosLeft();
	bodyPosResults[_hypoLimit][_spO2Drop].time[posRight]		= bpData->getTimeInPosRight();
	bodyPosResults[_hypoLimit][_spO2Drop].time[posSupine]		= bpData->getTimeInPosSupine();
	bodyPosResults[_hypoLimit][_spO2Drop].time[posProne]		= bpData->getTimeInPosProne();
	bodyPosResults[_hypoLimit][_spO2Drop].time[posUpright]		= bpData->getTimeInPosUpright();
	bodyPosResults[_hypoLimit][_spO2Drop].time[posUndefined]	= bpData->getTimeInPosUndef();
	float sumt = .0f;
	for (int j = 0 ; j < posCount - 1 ; j++) {
		sumt += bodyPosResults[_hypoLimit][_spO2Drop].time[j];
	}
	bodyPosResults[_hypoLimit][_spO2Drop].time[posSum] = sumt;
	bodyPosResults[_hypoLimit][_spO2Drop].percentTime[posLeft]		= sumt == .0f ? .0f : 100.0f * bodyPosResults[_hypoLimit][_spO2Drop].time[posLeft]		/ sumt;
	bodyPosResults[_hypoLimit][_spO2Drop].percentTime[posRight]		= sumt == .0f ? .0f : 100.0f * bodyPosResults[_hypoLimit][_spO2Drop].time[posRight]	/ sumt;
	bodyPosResults[_hypoLimit][_spO2Drop].percentTime[posSupine]	= sumt == .0f ? .0f : 100.0f * bodyPosResults[_hypoLimit][_spO2Drop].time[posSupine]	/ sumt;	
	bodyPosResults[_hypoLimit][_spO2Drop].percentTime[posProne]		= sumt == .0f ? .0f : 100.0f * bodyPosResults[_hypoLimit][_spO2Drop].time[posProne]	/ sumt;
	bodyPosResults[_hypoLimit][_spO2Drop].percentTime[posUpright]	= sumt == .0f ? .0f : 100.0f * bodyPosResults[_hypoLimit][_spO2Drop].time[posUpright]	/ sumt;
	bodyPosResults[_hypoLimit][_spO2Drop].percentTime[posSum]		= 100.0f;

	//---Total sleeping time
	bpData->computeTimeSleepingInPositions(&excludedEventArray,&awakeEventArray);
	bodyPosResults[_hypoLimit][_spO2Drop].timeSleeping[posLeft] = bpData->getTimeSleepingInPosLeft();
	bodyPosResults[_hypoLimit][_spO2Drop].timeSleeping[posRight] = bpData->getTimeSleepingInPosRight();
	bodyPosResults[_hypoLimit][_spO2Drop].timeSleeping[posSupine] = bpData->getTimeSleepingInPosSupine();
	bodyPosResults[_hypoLimit][_spO2Drop].timeSleeping[posProne] = bpData->getTimeSleepingInPosProne();
	bodyPosResults[_hypoLimit][_spO2Drop].timeSleeping[posUpright] = bpData->getTimeSleepingInPosUpright();
	bodyPosResults[_hypoLimit][_spO2Drop].timeSleeping[posUndefined] = bpData->getTimeSleepingInPosUndef();
	float sumst = .0f;
	for (int j = 0; j < posCount - 1; j++) {
		sumst += bodyPosResults[_hypoLimit][_spO2Drop].timeSleeping[j];
	}
	bodyPosResults[_hypoLimit][_spO2Drop].timeSleeping[posSum] = sumst;
	bodyPosResults[_hypoLimit][_spO2Drop].percentTimeSleeping[posLeft] = sumst == .0f ? .0f : 100.0f * bodyPosResults[_hypoLimit][_spO2Drop].timeSleeping[posLeft] / sumst;
	bodyPosResults[_hypoLimit][_spO2Drop].percentTimeSleeping[posRight] = sumst == .0f ? .0f : 100.0f * bodyPosResults[_hypoLimit][_spO2Drop].timeSleeping[posRight] / sumst;
	bodyPosResults[_hypoLimit][_spO2Drop].percentTimeSleeping[posSupine] = sumst == .0f ? .0f : 100.0f * bodyPosResults[_hypoLimit][_spO2Drop].timeSleeping[posSupine] / sumst;
	bodyPosResults[_hypoLimit][_spO2Drop].percentTimeSleeping[posProne] = sumst == .0f ? .0f : 100.0f * bodyPosResults[_hypoLimit][_spO2Drop].timeSleeping[posProne] / sumst;
	bodyPosResults[_hypoLimit][_spO2Drop].percentTimeSleeping[posUpright] = sumst == .0f ? .0f : 100.0f * bodyPosResults[_hypoLimit][_spO2Drop].timeSleeping[posUpright] / sumst;
	bodyPosResults[_hypoLimit][_spO2Drop].percentTimeSleeping[posSum] = 100.0f;

	//---Total awake time
	// Already computed
	bodyPosResults[_hypoLimit][_spO2Drop].timeAwake[posLeft] = bpData->getTimeAwakeInPosLeft();
	bodyPosResults[_hypoLimit][_spO2Drop].timeAwake[posRight] = bpData->getTimeAwakeInPosRight();
	bodyPosResults[_hypoLimit][_spO2Drop].timeAwake[posSupine] = bpData->getTimeAwakeInPosSupine();
	bodyPosResults[_hypoLimit][_spO2Drop].timeAwake[posProne] = bpData->getTimeAwakeInPosProne();
	bodyPosResults[_hypoLimit][_spO2Drop].timeAwake[posUpright] = bpData->getTimeAwakeInPosUpright();
	bodyPosResults[_hypoLimit][_spO2Drop].timeAwake[posUndefined] = bpData->getTimeAwakeInPosUndef();
	float sumaw = .0f;
	for (int j = 0; j < posCount - 1; j++) {
		sumaw += bodyPosResults[_hypoLimit][_spO2Drop].timeAwake[j];
	}
	bodyPosResults[_hypoLimit][_spO2Drop].timeAwake[posSum] = sumaw;
	bodyPosResults[_hypoLimit][_spO2Drop].percentTimeAwake[posLeft] = sumaw == .0f ? .0f : 100.0f * bodyPosResults[_hypoLimit][_spO2Drop].timeAwake[posLeft] / sumaw;
	bodyPosResults[_hypoLimit][_spO2Drop].percentTimeAwake[posRight] = sumaw == .0f ? .0f : 100.0f * bodyPosResults[_hypoLimit][_spO2Drop].timeAwake[posRight] / sumaw;
	bodyPosResults[_hypoLimit][_spO2Drop].percentTimeAwake[posSupine] = sumaw == .0f ? .0f : 100.0f * bodyPosResults[_hypoLimit][_spO2Drop].timeAwake[posSupine] / sumaw;
	bodyPosResults[_hypoLimit][_spO2Drop].percentTimeAwake[posProne] = sumaw == .0f ? .0f : 100.0f * bodyPosResults[_hypoLimit][_spO2Drop].timeAwake[posProne] / sumaw;
	bodyPosResults[_hypoLimit][_spO2Drop].percentTimeAwake[posUpright] = sumaw == .0f ? .0f : 100.0f * bodyPosResults[_hypoLimit][_spO2Drop].timeAwake[posUpright] / sumaw;
	bodyPosResults[_hypoLimit][_spO2Drop].percentTimeAwake[posSum] = 100.0f;
	
	float sumSupine = .0f;
	float sumNSupine = .0f;
	float sumAll = .0f;

	//---SpO2
	CArray <CSpO2DropEvnt *, CSpO2DropEvnt *> *sP = NULL;
	if (spO2DropLimit3 == _spO2Drop) sP = &spO2DropEventArray3;
	else if (spO2DropLimit4 == _spO2Drop) sP = &spO2DropEventArray4;
	int num = 0;
	if (sP) {
		for (int j = 0; j < posCount - 1; j++) {
			bodyPosResults[_hypoLimit][_spO2Drop].spO2[j] = 0;
		}
		int num = sP->GetCount();
		for (int i = 0; i < num; i++) {
			CSpO2DropEvnt *ev = sP->GetAt(i);
			if (ev->getIsActive()) {
				short bp = ev->getBodyPos();
				for (int j = 0; j < posCount - 1; j++) {
					bodyPosResults[_hypoLimit][_spO2Drop].spO2[j] += (bp == j);
				}
			}
		}
	}
	int sums = 0;
	for (int j = 0 ; j < posCount - 1 ; j++) {
		sums += bodyPosResults[_hypoLimit][_spO2Drop].spO2[j];
	}
	bodyPosResults[_hypoLimit][_spO2Drop].spO2[posSum] = sums;
	sumSupine =			(float) bodyPosResults[_hypoLimit][_spO2Drop].spO2[posSupine];
	sumNSupine =		(float) bodyPosResults[_hypoLimit][_spO2Drop].spO2[posLeft] + 
						(float) bodyPosResults[_hypoLimit][_spO2Drop].spO2[posRight] +
						(float) bodyPosResults[_hypoLimit][_spO2Drop].spO2[posProne];
	sumAll = sumSupine + sumNSupine;
	bodyPosResults[_hypoLimit][_spO2Drop].percentSpO2Supine = sumAll == .0f ? .0f : 100.0f * sumSupine / sumAll;
	bodyPosResults[_hypoLimit][_spO2Drop].percentSpO2NonSupine = sumAll == .0f ? .0f : 100.0f * sumNSupine / sumAll;


	//---Central
	for (int j = 0; j < posCount - 1; j++) {
		bodyPosResults[_hypoLimit][_spO2Drop].central[j] = 0;
	}
	num = centralEventArray.GetCount();
	for (int i = 0 ; i < num ; i++) {
		CCentralEvnt *ev = centralEventArray.GetAt(i);
		if (ev->getIsActive()) {
			short bp = ev->getBodyPos();
			for (int j = 0; j < posCount - 1; j++) {
				bodyPosResults[_hypoLimit][_spO2Drop].central[j] += (bp == j);
			}
		}
	}
	int sum = 0;
	for (int j = 0 ; j < posCount - 1 ; j++) {
		sum += bodyPosResults[_hypoLimit][_spO2Drop].central[j];;
	}
	bodyPosResults[_hypoLimit][_spO2Drop].central[posSum] = sum;
	sumSupine =			(float) bodyPosResults[_hypoLimit][_spO2Drop].central[posSupine];
	sumNSupine =		(float) bodyPosResults[_hypoLimit][_spO2Drop].central[posLeft] + 
						(float) bodyPosResults[_hypoLimit][_spO2Drop].central[posRight] +
						(float) bodyPosResults[_hypoLimit][_spO2Drop].central[posProne];
	sumAll = sumSupine + sumNSupine;
	bodyPosResults[_hypoLimit][_spO2Drop].percentCentralSupine = sumAll == .0f ? .0f : 100.0f * sumSupine / sumAll;
	bodyPosResults[_hypoLimit][_spO2Drop].percentCentralNonSupine = sumAll == .0f ? .0f : 100.0f * sumNSupine / sumAll;

	CArray <CHypoEvnt *, CHypoEvnt *> *hP = NULL;
	if (spO2DropLimit3 == _spO2Drop) hP = &hypoEventArrayDesat3;
	if (spO2DropLimit4 == _spO2Drop) hP = &hypoEventArrayDesat4;

	//---Hypo Central
	if (hP) {
		for (int j = 0; j < posCount - 1; j++) {
			bodyPosResults[_hypoLimit][_spO2Drop].centralHyp[j] = 0;
		}
		num = hP->GetCount();
		for (int i = 0; i < num; i++) {
			CHypoEvnt *ev = hP->GetAt(i);
			if (ev->getIsActive() && (ev->pickFromDropLevel(_hypoLimit))) {
				short bp = ev->getBodyPos();
				int hType = ev->getHypoType();
				if (hypoTypeCentral == hType) {
					for (int j = 0; j < posCount - 1; j++) {
						bodyPosResults[_hypoLimit][_spO2Drop].centralHyp[j] += (bp == j);
					}
				}
			}
		}
		int sum2 = 0;
		for (int j = 0; j < posCount - 1; j++) {
			sum2 += bodyPosResults[_hypoLimit][_spO2Drop].centralHyp[j];
		}
		bodyPosResults[_hypoLimit][_spO2Drop].centralHyp[posSum] = sum2;
		sumSupine = (float)bodyPosResults[_hypoLimit][_spO2Drop].centralHyp[posSupine];
		sumNSupine = (float)bodyPosResults[_hypoLimit][_spO2Drop].centralHyp[posLeft] +
			(float)bodyPosResults[_hypoLimit][_spO2Drop].centralHyp[posRight] +
			(float)bodyPosResults[_hypoLimit][_spO2Drop].centralHyp[posProne];
		sumAll = sumSupine + sumNSupine;
		bodyPosResults[_hypoLimit][_spO2Drop].percentCentralHypoSupine = sumAll == .0f ? .0f : 100.0f * sumSupine / sumAll;
		bodyPosResults[_hypoLimit][_spO2Drop].percentCentralHypoNonSupine = sumAll == .0f ? .0f : 100.0f * sumNSupine / sumAll;
	}

	//---Obstr Hypo
	if (hP) {
		for (int j = 0; j < posCount - 1; j++) {
			for (int i = 0; i < levelTypeCount; i++) {
				bodyPosResults[_hypoLimit][_spO2Drop].obstrHyp[j][i] = 0;
			}
		}
		num = hP->GetCount();
		for (int i = 0; i < num; i++) {
			CHypoEvnt *ev = hP->GetAt(i);
			if (ev->getIsActive() && (ev->pickFromDropLevel(_hypoLimit))) {
				short bp = ev->getBodyPos();
				UINT lv = ev->getLevelType();
				int hType = ev->getHypoType();
				if (hypoTypeObstr == hType) {
					for (int j = 0; j < posCount - 1; j++) {
						bodyPosResults[_hypoLimit][_spO2Drop].obstrHyp[j][lv] += (bp == j);
					}
				}
			}
		}
		for (int lv = 0; lv < levelTypeCount; lv++) {
			int sum3 = 0;
			for (int j = 0; j < posCount - 1; j++) {
				sum3 += bodyPosResults[_hypoLimit][_spO2Drop].obstrHyp[j][lv];
			}
			bodyPosResults[_hypoLimit][_spO2Drop].obstrHyp[posSum][lv] = sum3;
		}
		for (int j = 0; j < posCount; j++) {
			int sum4 = 0;
			for (int lv = 0; lv < levelTypeCount - 1; lv++) {
				sum4 += bodyPosResults[_hypoLimit][_spO2Drop].obstrHyp[j][lv];
			}
			bodyPosResults[_hypoLimit][_spO2Drop].obstrHyp[j][levelTypeSum] = sum4;
		}
		for (int lv = 0; lv < levelTypeCount; lv++) {
			sumSupine = (float)bodyPosResults[_hypoLimit][_spO2Drop].obstrHyp[posSupine][lv];
			sumNSupine = (float)bodyPosResults[_hypoLimit][_spO2Drop].obstrHyp[posLeft][lv] +
				(float)bodyPosResults[_hypoLimit][_spO2Drop].obstrHyp[posRight][lv] +
				(float)bodyPosResults[_hypoLimit][_spO2Drop].obstrHyp[posProne][lv];
			sumAll = sumSupine + sumNSupine;
			bodyPosResults[_hypoLimit][_spO2Drop].percentObstrHypoSupine[lv] = sumAll == .0f ? .0f : 100.0f * sumSupine / sumAll;
			bodyPosResults[_hypoLimit][_spO2Drop].percentObstrHypoNonSupine[lv] = sumAll == .0f ? .0f : 100.0f * sumNSupine / sumAll;
		}
	}

	//---Obstr
	for (int j = 0; j < posCount - 1; j++) {
		for (int i = 0; i < levelTypeCount; i++) {
			bodyPosResults[_hypoLimit][_spO2Drop].obstr[j][i] = 0;
		}
	}
	num = obstrEventArray.GetCount();
	for (int i = 0 ; i < num ; i++) {
		CObstrEvnt *ev = obstrEventArray.GetAt(i);
		if (ev->getIsActive()) {
			short bp = ev->getBodyPos();
			UINT lv = ev->getLevelType();

			for (int j = 0; j < posCount - 1; j++) {
				bodyPosResults[_hypoLimit][_spO2Drop].obstr[j][lv] += (bp == j);
			}
		}
	}
	for (int lv = 0 ; lv < levelTypeCount ; lv++) {
		int sum3 = 0;
		for (int j = 0 ; j < posCount - 1 ; j++) {
			sum3 += bodyPosResults[_hypoLimit][_spO2Drop].obstr[j][lv];
		}
		bodyPosResults[_hypoLimit][_spO2Drop].obstr[posSum][lv] = sum3;
	}
	for (int j = 0 ; j < posCount ; j++) {
		int sum4 = 0;
		for (int lv = 0 ; lv < levelTypeCount - 1 ; lv++) {
			sum4 += bodyPosResults[_hypoLimit][_spO2Drop].obstr[j][lv];
		}
		bodyPosResults[_hypoLimit][_spO2Drop].obstr[j][levelTypeSum] = sum4;
	}
	for (int lv = 0 ; lv < levelTypeCount ; lv++) {
		sumSupine =			(float) bodyPosResults[_hypoLimit][_spO2Drop].obstr[posSupine][lv];
		sumNSupine =		(float) bodyPosResults[_hypoLimit][_spO2Drop].obstr[posLeft][lv] + 
			(float) bodyPosResults[_hypoLimit][_spO2Drop].obstr[posRight][lv] +
			(float) bodyPosResults[_hypoLimit][_spO2Drop].obstr[posProne][lv];
		sumAll = sumSupine + sumNSupine;
		bodyPosResults[_hypoLimit][_spO2Drop].percentObstrSupine[lv] = sumAll == .0f ? .0f : 100.0f * sumSupine / sumAll;
		bodyPosResults[_hypoLimit][_spO2Drop].percentObstrNonSupine[lv] = sumAll == .0f ? .0f : 100.0f * sumNSupine / sumAll;
	}
	
	//---Mixed
	for (int j = 0; j < posCount - 1; j++) {
		for (int i = 0; i < levelTypeCount; i++) {
			bodyPosResults[_hypoLimit][_spO2Drop].mix[j][i] = 0;
		}
	}
	num = mixedEventArray.GetCount();
	for (int i = 0 ; i < num ; i++) {
		CMixedEvnt *ev = mixedEventArray.GetAt(i);
		if (ev->getIsActive()) {
			short bp = ev->getBodyPos();
			UINT lv = ev->getLevelType();

			for (int j = 0; j < posCount - 1; j++) {
				bodyPosResults[_hypoLimit][_spO2Drop].mix[j][lv] += (bp == j);
			}
		}
	}
	for (int lv = 0 ; lv < levelTypeCount ; lv++) {
		int sum3 = 0;
		for (int j = 0 ; j < posCount - 1 ; j++) {
			sum3 += bodyPosResults[_hypoLimit][_spO2Drop].mix[j][lv];
		}
		bodyPosResults[_hypoLimit][_spO2Drop].mix[posSum][lv] = sum3;
	}
	for (int j = 0 ; j < posCount ; j++) {
		int sum4 = 0;
		for (int lv = 0 ; lv < levelTypeCount - 1 ; lv++) {
			sum4 += bodyPosResults[_hypoLimit][_spO2Drop].mix[j][lv];
		}
		bodyPosResults[_hypoLimit][_spO2Drop].mix[j][levelTypeSum] = sum4;
	}
	for (int lv = 0 ; lv < levelTypeCount ; lv++) {
		sumSupine =			(float) bodyPosResults[_hypoLimit][_spO2Drop].mix[posSupine][lv];
		sumNSupine =		(float) bodyPosResults[_hypoLimit][_spO2Drop].mix[posLeft][lv] + 
			(float) bodyPosResults[_hypoLimit][_spO2Drop].mix[posRight][lv] +
			(float) bodyPosResults[_hypoLimit][_spO2Drop].mix[posProne][lv];
		sumAll = sumSupine + sumNSupine;
		bodyPosResults[_hypoLimit][_spO2Drop].percentMixSupine[lv] = sumAll == .0f ? .0f : 100.0f * sumSupine / sumAll;
		bodyPosResults[_hypoLimit][_spO2Drop].percentMixNonSupine[lv] = sumAll == .0f ? .0f : 100.0f * sumNSupine / sumAll;
	}

	//---RERA
	for (int j = 0; j < posCount - 1; j++) {
		for (int i = 0; i < levelTypeCount; i++) {
			bodyPosResults[_hypoLimit][_spO2Drop].RERA[j][i] = 0;
		}
	}
	CArray <CArousalEvnt *, CArousalEvnt *> *aP = NULL;
	if (spO2DropLimit3 == _spO2Drop) aP = &arousalEventArrayDesat3;
	else if (spO2DropLimit4 == _spO2Drop) aP = &arousalEventArrayDesat4;
	if (aP) {
		num = aP->GetCount();
		for (int i = 0; i < num; i++) {
			CArousalEvnt *ev = aP->GetAt(i);
			CRERAEvnt *rP = ev->getRERAEvnt();
			if (ev->getIsActive(_hypoLimit) && rP) {
				short bp = rP->getBodyPos();
				UINT lv = rP->getLevelType();

				for (int j = 0; j < posCount - 1; j++) {
					bodyPosResults[_hypoLimit][_spO2Drop].RERA[j][lv] += (bp == j);
				}
			}
		}
	}
	for (int lv = 0 ; lv < levelTypeCount ; lv++) {
		int sum3 = 0;
		for (int j = 0 ; j < posCount - 1 ; j++) {
			sum3 += bodyPosResults[_hypoLimit][_spO2Drop].RERA[j][lv];
		}
		bodyPosResults[_hypoLimit][_spO2Drop].RERA[posSum][lv] = sum3;
	}
	for (int j = 0 ; j < posCount ; j++) {
		int sum4 = 0;
		for (int lv = 0 ; lv < levelTypeCount - 1 ; lv++) {
			sum4 += bodyPosResults[_hypoLimit][_spO2Drop].RERA[j][lv];
		}
		bodyPosResults[_hypoLimit][_spO2Drop].RERA[j][levelTypeSum] = sum4;
	}
	for (int lv = 0 ; lv < levelTypeCount ; lv++) {
		sumSupine =			(float) bodyPosResults[_hypoLimit][_spO2Drop].RERA[posSupine][lv];
		sumNSupine =		(float) bodyPosResults[_hypoLimit][_spO2Drop].RERA[posLeft][lv] + 
			(float) bodyPosResults[_hypoLimit][_spO2Drop].RERA[posRight][lv] +
			(float) bodyPosResults[_hypoLimit][_spO2Drop].RERA[posProne][lv];
		sumAll = sumSupine + sumNSupine;
		bodyPosResults[_hypoLimit][_spO2Drop].percentRERASupine[lv] = sumAll == .0f ? .0f : 100.0f * sumSupine / sumAll;
		bodyPosResults[_hypoLimit][_spO2Drop].percentRERANonSupine[lv] = sumAll == .0f ? .0f : 100.0f * sumNSupine / sumAll;
	}

	//---Arousal
	for (int j = 0; j < posCount - 1; j++) {
		bodyPosResults[_hypoLimit][_spO2Drop].arousal[j] = 0;
	}
	if (aP) {
		num = aP->GetCount();
		for (int i = 0; i < num; i++) {
			CArousalEvnt *ev = aP->GetAt(i);
			if (ev->getIsActive(_hypoLimit)) {
				short bp = ev->getBodyPos();
				for (int j = 0; j < posCount - 1; j++) {
					bodyPosResults[_hypoLimit][_spO2Drop].arousal[j] += (bp == j);
				}
			}
		}
	}
	sum = 0;
	for (int j = 0 ; j < posCount - 1 ; j++) {
		sum += bodyPosResults[_hypoLimit][_spO2Drop].arousal[j];
	}
	bodyPosResults[_hypoLimit][_spO2Drop].arousal[posSum] = sum;
	sumSupine =			(float) bodyPosResults[_hypoLimit][_spO2Drop].arousal[posSupine];
	sumNSupine =		(float) bodyPosResults[_hypoLimit][_spO2Drop].arousal[posLeft] + 
						(float) bodyPosResults[_hypoLimit][_spO2Drop].arousal[posRight] +
						(float) bodyPosResults[_hypoLimit][_spO2Drop].arousal[posProne];
	sumAll = sumSupine + sumNSupine;
	bodyPosResults[_hypoLimit][_spO2Drop].percentArousalSupine = sumAll == .0f ? .0f : 100.0f * sumSupine / sumAll;
	bodyPosResults[_hypoLimit][_spO2Drop].percentArousalNonSupine = sumAll == .0f ? .0f : 100.0f * sumNSupine / sumAll;

	//---Snoring
	for (int j = 0; j < posCount - 1; j++) {
		bodyPosResults[_hypoLimit][_spO2Drop].snoringTime[j] = .0f;
	}
	num = snoringEventArray.GetCount();
	for (int i = 0 ; i < num ; i++) {
		CSnoringEvnt *ev = snoringEventArray.GetAt(i);
		if (ev->getIsActive()) {
			for (int j = 0; j < posCount - 1; j++) {
				bodyPosResults[_hypoLimit][_spO2Drop].snoringTime[j] += ev->getTimeInBodyPos(j);
			}
		}
	}
	float sumf = 0;
	for (int j = 0 ; j < posCount - 1 ; j++) {
		sumf += bodyPosResults[_hypoLimit][_spO2Drop].snoringTime[j];
	}
	bodyPosResults[_hypoLimit][_spO2Drop].snoringTime[posSum] = sumf;

	//---Indexes vs position
	float index = .0f;
	float sumRERAi = .0f;
	float sumOAHi = .0f;
	float sumCAHi = .0f;
	float sumODi = .0f;
	float sumOAi = .0f;
	float sumOHi = .0f;
	float sumRDi = .0f;
	float sumAHi = .0f;
	for (int j = 0; j < posCount - 1; j++) {
		//---RERA
		index = .0f == bodyPosResults[_hypoLimit][_spO2Drop].timeSleeping[j] ? 
			.0f : 
			3600.0f * bodyPosResults[_hypoLimit][_spO2Drop].RERA[j][levelTypeSum] / bodyPosResults[_hypoLimit][_spO2Drop].timeSleeping[j];
		bodyPosResults[_hypoLimit][_spO2Drop].RERAindex[j] = index;
		sumRERAi += index;

		//---OA
		index = .0f == bodyPosResults[_hypoLimit][_spO2Drop].timeSleeping[j] ?
		.0f :
		3600.0f * (bodyPosResults[_hypoLimit][_spO2Drop].mix[j][levelTypeSum] +
			bodyPosResults[_hypoLimit][_spO2Drop].obstr[j][levelTypeSum]) / bodyPosResults[_hypoLimit][_spO2Drop].timeSleeping[j];
		bodyPosResults[_hypoLimit][_spO2Drop].OAindex[j] = index;
		sumOAi += index;

		//---OH
		index = .0f == bodyPosResults[_hypoLimit][_spO2Drop].timeSleeping[j] ?
			.0f :
			3600.0f * (bodyPosResults[_hypoLimit][_spO2Drop].obstrHyp[j][levelTypeSum]) / bodyPosResults[_hypoLimit][_spO2Drop].timeSleeping[j];
		bodyPosResults[_hypoLimit][_spO2Drop].OHindex[j] = index;
		sumOHi += index;
		
		//---OAHI
		index = .0f == bodyPosResults[_hypoLimit][_spO2Drop].timeSleeping[j] ?
			.0f :
			3600.0f * (	bodyPosResults[_hypoLimit][_spO2Drop].mix[j][levelTypeSum] +
						bodyPosResults[_hypoLimit][_spO2Drop].obstr[j][levelTypeSum] + 
						bodyPosResults[_hypoLimit][_spO2Drop].obstrHyp[j][levelTypeSum]) / bodyPosResults[_hypoLimit][_spO2Drop].timeSleeping[j];
		bodyPosResults[_hypoLimit][_spO2Drop].OAHindex[j] = index;
		sumOAHi += index;

		//---CAHI
		index = .0f == bodyPosResults[_hypoLimit][_spO2Drop].timeSleeping[j] ?
			.0f :
			3600.0f * (bodyPosResults[_hypoLimit][_spO2Drop].central[j] +
				bodyPosResults[_hypoLimit][_spO2Drop].centralHyp[j]) / bodyPosResults[_hypoLimit][_spO2Drop].timeSleeping[j];
		bodyPosResults[_hypoLimit][_spO2Drop].CAHindex[j] = index;
		sumCAHi += index;

		//---RD
		bodyPosResults[_hypoLimit][_spO2Drop].RDindex[j] = bodyPosResults[_hypoLimit][_spO2Drop].CAHindex[j] +
			bodyPosResults[_hypoLimit][_spO2Drop].OAHindex[j] +
			bodyPosResults[_hypoLimit][_spO2Drop].RERAindex[j];
		sumRDi = sumCAHi + sumOAHi + sumRERAi;

		//---AH
		bodyPosResults[_hypoLimit][_spO2Drop].AHindex[j] = bodyPosResults[_hypoLimit][_spO2Drop].CAHindex[j] +
			bodyPosResults[_hypoLimit][_spO2Drop].OAHindex[j];
		sumAHi = sumCAHi + sumOAHi;
		
		//---ODI
		index = .0f == bodyPosResults[_hypoLimit][_spO2Drop].timeSleeping[j] ?
			.0f :
			3600.0f * bodyPosResults[_hypoLimit][_spO2Drop].spO2[j] / bodyPosResults[_hypoLimit][_spO2Drop].timeSleeping[j];
		bodyPosResults[_hypoLimit][_spO2Drop].ODindex[j] = index;
		sumODi += index;
	}
	bodyPosResults[_hypoLimit][_spO2Drop].RERAindex[posSum] = sumRERAi;
	bodyPosResults[_hypoLimit][_spO2Drop].OAHindex[posSum] = sumOAHi;
	bodyPosResults[_hypoLimit][_spO2Drop].CAHindex[posSum] = sumCAHi;
	bodyPosResults[_hypoLimit][_spO2Drop].ODindex[posSum] = sumODi;
	bodyPosResults[_hypoLimit][_spO2Drop].OAindex[posSum] = sumOAi;
	bodyPosResults[_hypoLimit][_spO2Drop].OHindex[posSum] = sumOHi;
	bodyPosResults[_hypoLimit][_spO2Drop].RDindex[posSum] = sumRDi;
	bodyPosResults[_hypoLimit][_spO2Drop].AHindex[posSum] = sumAHi;

	//---Indexes vs level
	float indexLvl = .0f;
	float sumRERALvli = .0f;
	float sumOAHLvli = .0f;
	float sumOALvli = .0f;
	float sumOHLvli = .0f;
	float sumRDLvli = .0f;
	float sumAHLvli = .0f;
	for (int lv = 0; lv < levelTypeCount; lv++) {
		//---RERA
		index = .0f == bodyPosResults[_hypoLimit][_spO2Drop].timeSleeping[posSum] ?
			.0f :
			3600.0f * bodyPosResults[_hypoLimit][_spO2Drop].RERA[posSum][lv] / bodyPosResults[_hypoLimit][_spO2Drop].timeSleeping[posSum];
		bodyPosResults[_hypoLimit][_spO2Drop].RERAindexVSlevel[lv] = index;
		sumRERALvli += index;

		//---OA
		index = .0f == bodyPosResults[_hypoLimit][_spO2Drop].timeSleeping[posSum] ?
			.0f :
			3600.0f * (bodyPosResults[_hypoLimit][_spO2Drop].mix[posSum][lv] +
				bodyPosResults[_hypoLimit][_spO2Drop].obstr[posSum][lv]) / bodyPosResults[_hypoLimit][_spO2Drop].timeSleeping[posSum];
		bodyPosResults[_hypoLimit][_spO2Drop].OAindexVSlevel[lv] = index;
		sumOALvli += index;

		//---OH
		index = .0f == bodyPosResults[_hypoLimit][_spO2Drop].timeSleeping[posSum] ?
			.0f :
			3600.0f * (bodyPosResults[_hypoLimit][_spO2Drop].obstrHyp[posSum][lv]) / bodyPosResults[_hypoLimit][_spO2Drop].timeSleeping[posSum];
		bodyPosResults[_hypoLimit][_spO2Drop].OHindexVSlevel[lv] = index;
		sumOHLvli += index;

		//---OAHI
		index = .0f == bodyPosResults[_hypoLimit][_spO2Drop].timeSleeping[posSum] ?
			.0f :
			3600.0f * (bodyPosResults[_hypoLimit][_spO2Drop].mix[posSum][lv] +
				bodyPosResults[_hypoLimit][_spO2Drop].obstr[posSum][lv] +
				bodyPosResults[_hypoLimit][_spO2Drop].obstrHyp[posSum][lv]) / bodyPosResults[_hypoLimit][_spO2Drop].timeSleeping[posSum];
		bodyPosResults[_hypoLimit][_spO2Drop].OAHindexVSlevel[lv] = index;
		sumOAHLvli += index;

		//---RD
		bodyPosResults[_hypoLimit][_spO2Drop].RDindexVSlevel[lv] = bodyPosResults[_hypoLimit][_spO2Drop].OAHindexVSlevel[lv] +
			bodyPosResults[_hypoLimit][_spO2Drop].RERAindexVSlevel[lv];
		sumRDLvli = sumOAHLvli + sumRERALvli;

		//---AH
		bodyPosResults[_hypoLimit][_spO2Drop].AHindexVSlevel[lv] = bodyPosResults[_hypoLimit][_spO2Drop].OAHindexVSlevel[lv];
		sumAHLvli = sumOAHLvli;
	}
	bodyPosResults[_hypoLimit][_spO2Drop].RERAindexVSlevel[levelTypeSum] = sumRERALvli;
	bodyPosResults[_hypoLimit][_spO2Drop].OAHindexVSlevel[levelTypeSum] = sumOAHLvli;
	bodyPosResults[_hypoLimit][_spO2Drop].OAindexVSlevel[levelTypeSum] = sumOALvli;
	bodyPosResults[_hypoLimit][_spO2Drop].OHindexVSlevel[levelTypeSum] = sumOHLvli;
	bodyPosResults[_hypoLimit][_spO2Drop].RDindexVSlevel[levelTypeSum] = sumRDLvli;
	bodyPosResults[_hypoLimit][_spO2Drop].AHindexVSlevel[levelTypeSum] = sumAHLvli;
}


/*
Description: Sends body pos data to an event
_from: Time in seconds - start of time interval
_to: Time in seconds - end of interval
_v0: The first vector where the data will be extracted
_t: The time vector corresponding to _v0 
_eV: The CEvnt to receive the data

Note that _to and _from may not correspond to a point on the _t axis, hence the use of lower_bound()/upper_bound().
Also note that the selected time window is slightly wider than the event time window length
*/
bool CEvents::bodyDataToEvent(CEvnt *_eV,vector <FLOAT> *_v,vector <FLOAT> *_t,FLOAT _from,FLOAT _to)
{
	ASSERT(_eV);
	if (!_v) return false;
	if (0 == _v->size()) return false;
	ASSERT(_t);
	ASSERT(_t->size() == _v->size());
	if (_to == _from) return false;
	ASSERT(_to > _from);

	unsigned int iFrom = lower_bound(_t->begin(),_t->end(),_from) - _t->begin();
	unsigned int iTo = upper_bound(_t->begin(),_t->end(),_to) - _t->begin();

	iFrom = iFrom > 0 ? iFrom - 1 : iFrom;
	iTo = iTo < (_t->size() - 1) ? iTo + 1 : iTo;	
	_eV->setBodyPosVector(_v->begin() + iFrom,_v->begin() + iTo,_t->begin() + iFrom,_t->begin() + iTo);	
	return true;
}

/*
Description: Sends air mic data to an event
_from: Time in seconds - start of time interval
_to: Time in seconds - end of interval
_v0: The first vector where the data will be extracted
_t: The time vector corresponding to _v0 
_eV: The CEvnt to receive the data

Note that _to and _from may not correspond to a point on the _t axis, hence the use of lower_bound()/upper_bound().
Also note that the selected time window is slightly wider than the event time window length
*/
bool CEvents::airMicDataToEvent(CEvnt *_eV,
						vector <FLOAT> *_v0,
						vector <FLOAT> *_v1,
						vector <FLOAT> *_v2,
						vector <FLOAT> *_v3,
						vector <FLOAT> *_vSum,
						vector <FLOAT> *_t,FLOAT _from,FLOAT _to)
{
	ASSERT(_eV);
	ASSERT(_t);
	if (!_v0) return false;
	if (!_v1) return false;
	if (!_v2) return false;
	if (!_v3) return false;
	if (!_vSum) return false;
	if (0 == _v0->size()) return false;
	if (0 == _v1->size()) return false;
	if (0 == _v2->size()) return false;
	if (0 == _v3->size()) return false;
	if (0 == _vSum->size()) return false;

	ASSERT(_t->size() == _v0->size());
	ASSERT(_t->size() == _v1->size());
	ASSERT(_t->size() == _v2->size());
	ASSERT(_t->size() == _v3->size());
	ASSERT(_t->size() == _vSum->size());
	if (_to == _from) return false;
	ASSERT(_to > _from);

	unsigned int iFrom = lower_bound(_t->begin(),_t->end(),_from) - _t->begin();
	unsigned int iTo = upper_bound(_t->begin(),_t->end(),_to) - _t->begin();

	iFrom = iFrom > 0 ? iFrom - 1 : iFrom;
	iTo = iTo < (_t->size() - 1) ? iTo + 1 : iTo;	
	_eV->setAirMicVectors(_v0->begin() + iFrom,_v0->begin() + iTo,
		_v1->begin() + iFrom,_v1->begin() + iTo,
		_v2->begin() + iFrom,_v2->begin() + iTo,
		_v3->begin() + iFrom,_v3->begin() + iTo,
		_vSum->begin() + iFrom,_vSum->begin() + iTo,
		_t->begin() + iFrom,_t->begin() + iTo);	
	return true;
}


/*
Description: Sends contact mic data to an event
_from: Time in seconds - start of time interval
_to: Time in seconds - end of interval
_v0: The first vector where the data will be extracted
_t: The time vector corresponding to _v0 
_eV: The CEvnt to receive the data

Note that _to and _from may not correspond to a point on the _t axis, hence the use of lower_bound()/upper_bound().
Also note that the selected time window is slightly wider than the event time window length
*/
bool CEvents::contactMicDataToEvent(CEvnt *_eV,vector <FLOAT> *_v,vector <FLOAT> *_t,FLOAT _from,FLOAT _to)
{
	ASSERT(_eV);
	if (!_v) return false;
	if (0 == _v->size()) return false;
	ASSERT(_t);
	ASSERT(_t->size() == _v->size());
	if (_to == _from) return false;
	ASSERT(_to > _from);

	unsigned int iFrom = lower_bound(_t->begin(),_t->end(),_from) - _t->begin();
	unsigned int iTo = upper_bound(_t->begin(),_t->end(),_to) - _t->begin();

	iFrom = iFrom > 0 ? iFrom - 1 : iFrom;
	iTo = iTo < (_t->size() - 1) ? iTo + 1 : iTo;	
	_eV->setContactMicVector(_v->begin() + iFrom,_v->begin() + iTo,_t->begin() + iFrom,_t->begin() + iTo);	
	return true;
}

/*
Description: Sends POES envelope data to an event
_from: Time in seconds - start of time interval
_to: Time in seconds - end of interval
_v0: The first vector where the data will be extracted
_t: The time vector corresponding to _v0 
_eV: The CEvnt to receive the data

Note that _to and _from may not correspond to a point on the _t axis, hence the use of lower_bound()/upper_bound().
Also note that the selected time window is slightly wider than the event time window length
*/
bool CEvents::poesEnvDataToEvent(CEvnt *_eV,vector <FLOAT> *_v,vector <FLOAT> *_t,FLOAT _from,FLOAT _to)
{
	ASSERT(_eV);
	if (!_v) return false;
	if (0 == _v->size()) return false;
	ASSERT(_t);
	ASSERT(_t->size() == _v->size());
	if (_to == _from) return false;
	ASSERT(_to > _from);

	unsigned int iFrom = lower_bound(_t->begin(),_t->end(),_from) - _t->begin();
	unsigned int iTo = upper_bound(_t->begin(),_t->end(),_to) - _t->begin();

	iFrom = iFrom > 0 ? iFrom - 1 : iFrom;
	iTo = iTo < (_t->size() - 1) ? iTo + 1 : iTo;	
	_eV->setPoesEnv(_v->begin() + iFrom,_v->begin() + iTo,_t->begin() + iFrom,_t->begin() + iTo);	
	return true;
}

bool CEvents::pphEnvDataToEvent(CEvnt * _eV, vector<FLOAT>* _v, vector<FLOAT>* _t, FLOAT _from, FLOAT _to)
{
	ASSERT(_eV);
	if (!_v) return false;
	if (0 == _v->size()) return false;
	ASSERT(_t);
	ASSERT(_t->size() == _v->size());
	if (_to == _from) return false;
	ASSERT(_to > _from);

	unsigned int iFrom = lower_bound(_t->begin(), _t->end(), _from) - _t->begin();
	unsigned int iTo = upper_bound(_t->begin(), _t->end(), _to) - _t->begin();

	iFrom = iFrom > 0 ? iFrom - 1 : iFrom;
	iTo = iTo < (_t->size() - 1) ? iTo + 1 : iTo;
	_eV->setPPHEnv(_v->begin() + iFrom, _v->begin() + iTo, _t->begin() + iFrom, _t->begin() + iTo);
	return true;
}

/*
Description: Sends pressure gradient data to an event
_from: Time in seconds - start of time interval
_to: Time in seconds - end of interval
_v0: The first vector where the data will be extracted
_t: The time vector corresponding to _v0 
_eV: The CEvnt to receive the data

Note that _to and _from may not correspond to a point on the _t axis, hence the use of lower_bound()/upper_bound().
Also note that the selected time window is slightly wider than the event time window length
*/
bool CEvents::gradientDataToEvent(CEvnt *_eV,vector <FLOAT> *_v,vector <FLOAT> *_t,
						  FLOAT _from,FLOAT _to)
{
	ASSERT(_eV);
	if (!_v) return false;
	if (0 == _v->size()) return false;
	ASSERT(_t);
	ASSERT(_t->size() == _v->size());
	if (_to == _from) return false;
	ASSERT(_to > _from);

	unsigned int iFrom = lower_bound(_t->begin(),_t->end(),_from) - _t->begin();
	unsigned int iTo = upper_bound(_t->begin(),_t->end(),_to) - _t->begin();

	iFrom = iFrom > 0 ? iFrom - 1 : iFrom;
	iTo = iTo < (_t->size() - 1) ? iTo + 1 : iTo;	
	_eV->setPgradient(_v->begin() + iFrom,_v->begin() + iTo,_t->begin() + iFrom,_t->begin() + iTo);	
	return true;
}

/*
Description: Sends temp data to an event
_from: Time in seconds - start of time interval
_to: Time in seconds - end of interval
_v: The vector where the data will be extracted
_t: The time vector corresponding to _v
_eV: The CEvnt to receive the data

Note that _to and _from may not correspond to a point on the _t axis, hence the use of lower_bound()/upper_bound().
Also note that the selected time window is slightly wider than the event time window length
*/
bool CEvents::flowDataToEvent(CEvnt *_eV,vector <FLOAT> *_v,vector <FLOAT> *_t,FLOAT _from,FLOAT _to)
{
	ASSERT(_eV);

	if (!_v) return false;
	if (0 == _v->size()) return false;

	ASSERT(_t);
	ASSERT(_t->size() == _v->size());
	if (_to == _from) return false;
	ASSERT(_to > _from);

	unsigned int iFrom = lower_bound(_t->begin(),_t->end(),_from) - _t->begin();
	unsigned int iTo = upper_bound(_t->begin(),_t->end(),_to) - _t->begin();
	
	iFrom = iFrom > 0 ? iFrom - 1 : iFrom;
	iTo = iTo < (_t->size() - 1) ? iTo + 1 : iTo;
	_eV->setFlow(_v->begin() + iFrom,_v->begin() + iTo,_t->begin() + iFrom,_t->begin() + iTo);
	return true;
}

/*
Description:
Fill up a vector with flags plus the corresponding time vector. This is used for detection of apneas and hypopneas
*/
void CEvents::setupHelpEventVector(vector <FLOAT> *_helpVTimeP, vector <unsigned short> *_helpVP,
									float _startAnalysisTime,float _stopAnalysisTime)
{
	vector <FLOAT> *poesTime = cathData->getPOESEnvVectorTime();
	vector <FLOAT> *pphTime = cathData->getPPHEnvVectorTime();
	vector <FLOAT> *flowTime = cathData->getFlowVectorTime();
	vector <FLOAT> *flowBaseline = cathData->getFlowBaselineVector();
	vector <FLOAT> *poesBaseline = cathData->getPOESBaselineVector();
	vector <FLOAT> *pphBaseline = cathData->getPPHBaselineVector();
	vector <FLOAT> *vpoes = cathData->getPOESEnvVector();
	vector <FLOAT> *vpph = cathData->getPPHEnvVector();
	vector <FLOAT> *flow = cathData->getFlowVector();

	//---Score central, mixed, obstructive, hypopnea, apnea. Do not care if overlap
	/*
	Set up a help vector and a correspondign time vector
	The help vector contains tag saying
	- if flow is below hypopnea limit
	- if flow is stopped
	- if pressure variation is dead in OES
	- if pressure variation is dead in PH
	- if OES pressure is decreasing
	*/
	int cnt = 0;
	float noFlowLimit;
	float hypopneaLimit50;
	float hypopneaLimit40;
	float hypopneaLimit30;
	float deadPOESLimit;
	float deadPPHLimit;
	unsigned short flags = 0;
	for (float theTime = _startAnalysisTime; theTime < _stopAnalysisTime; theTime += FILTER_DESIGN_SAMPLE_INTERVAL, cnt++) {

		vector <FLOAT>::iterator timeFlowIt = lower_bound(flowTime->begin(), flowTime->end(), theTime);
		if (timeFlowIt == flowTime->end()) break;

		vector <FLOAT>::iterator timePOESIt = lower_bound(poesTime->begin(), poesTime->end(), theTime);
		if (timePOESIt == poesTime->end()) break;

		vector <FLOAT>::iterator timePPHIt = lower_bound(pphTime->begin(), pphTime->end(), theTime);
		if (timePPHIt == pphTime->end()) break;

		//--Refresh baseline values
		unsigned int d = distance(flowTime->begin(), timeFlowIt);
		if ((d < flowBaseline->size()) && (d > 0))
			noFlowLimit = detectionCriteria.flowStopFraction * flowBaseline->at(d);
		d = distance(flowTime->begin(), timeFlowIt);
		if ((d < flowBaseline->size()) && (d > 0))
			hypopneaLimit50 = LOW_FLOW_LIMIT50 * flowBaseline->at(d);
		d = distance(flowTime->begin(), timeFlowIt);
		if ((d < flowBaseline->size()) && (d > 0))
			hypopneaLimit40 = LOW_FLOW_LIMIT40 * flowBaseline->at(d);
		d = distance(flowTime->begin(), timeFlowIt);
		if ((d < flowBaseline->size()) && (d > 0))
			hypopneaLimit30 = LOW_FLOW_LIMIT30 * flowBaseline->at(d);		
		d = distance(poesTime->begin(), timePOESIt);
		if ((d < poesBaseline->size()) && (d > 0))
			deadPOESLimit = detectionCriteria.pressureDeadFraction * poesBaseline->at(d);

		d = distance(pphTime->begin(), timePPHIt);
		if ((d < pphBaseline->size()) && (d > 0))
			deadPPHLimit = detectionCriteria.pressureDeadFraction * pphBaseline->at(d);

		_helpVTimeP->push_back(theTime);
		flags = 0;
		d = distance(flowTime->begin(), timeFlowIt);
		if ((d < flow->size()) && (d > 0)) {
			float flowValue = flow->at(d);
			if (flowValue < noFlowLimit)
				flags |= zeroFlow;
			if (flowValue < hypopneaLimit50)
				flags |= lowFlow50;
			if (flowValue < hypopneaLimit40)
				flags |= lowFlow40;
			if (flowValue < hypopneaLimit30)
				flags |= lowFlow30;

			d = distance(pphTime->begin(), timePPHIt);
			if ((d < vpph->size()) && (d > 0)) {
				float pphValue = vpph->at(d);
				if (pphValue < deadPPHLimit)
					flags |= deadPph;

				d = distance(poesTime->begin(), timePOESIt);
				if ((d < vpoes->size()) && (d > 1)) {
					float poesValue = vpoes->at(d);
					if (poesValue < deadPOESLimit)
						flags |= deadPoes;
					if (cnt > 0) {
						d -= 1;
						float poesPreValue = vpoes->at(d);
						if (poesPreValue > poesValue)
							flags |= decrPoes;
					}
				}
			}
		}
		_helpVP->push_back(flags);
	}

}

bool CEvents::testIfDataOKforAnalysis(float *_startAnalysisTime, float *_stopAnalysisTime)
{
	vector <FLOAT> *vpoes = cathData->getPOESEnvVector();
	vector <FLOAT> *vpph = cathData->getPPHEnvVector();
	vector <FLOAT> *flow = cathData->getFlowVector();
	vector <FLOAT> *poesTime = cathData->getPOESEnvVectorTime();
	vector <FLOAT> *pphTime = cathData->getPPHEnvVectorTime();
	vector <FLOAT> *flowTime = cathData->getFlowVectorTime();
	vector <FLOAT> *vpGrad = cathData->getPgradientVector();
	vector <FLOAT> *tpGrad = cathData->getPgradientVectorTime();

	vector <FLOAT> *flowBaseline = cathData->getFlowBaselineVector();
	vector <FLOAT> *poesBaseline = cathData->getPOESBaselineVector();
	vector <FLOAT> *pphBaseline = cathData->getPPHBaselineVector();

	if (minVectorLengthForAnalysis >= vpoes->size()) return false;
	if (minVectorLengthForAnalysis >= vpph->size()) return false;
	if (minVectorLengthForAnalysis >= flow->size()) return false;
	if (minVectorLengthForAnalysis >= poesTime->size()) return false;
	if (minVectorLengthForAnalysis >= pphTime->size()) return false;
	if (minVectorLengthForAnalysis >= flowTime->size()) return false;
	if (minVectorLengthForAnalysis >= vpGrad->size()) return false;
	if (minVectorLengthForAnalysis >= tpGrad->size()) return false;
	if (minVectorLengthForAnalysis >= flowBaseline->size()) return false;
	if (minVectorLengthForAnalysis >= poesBaseline->size()) return false;
	if (minVectorLengthForAnalysis >= pphBaseline->size()) return false;

	*_startAnalysisTime = .0f;		//  effectiveStartTime + detectionCriteria.baselineLength;
	vector <FLOAT> tlengths;
	tlengths.push_back(*max_element(poesTime->begin(), poesTime->end()));
	tlengths.push_back(*max_element(pphTime->begin(), pphTime->end()));
	tlengths.push_back(*max_element(flowTime->begin(), flowTime->end()));
	*_stopAnalysisTime = *min_element(tlengths.begin(), tlengths.end());

	if (*_startAnalysisTime >= *_stopAnalysisTime) return false;

	return true;
}

/*
Description: Find findMixCentralObstr Apnea
*/
bool CEvents::findMixCentralObstrApnea(vector <FLOAT> *_helpVTimeP, vector <unsigned short> *_helpVP)
{
	if (!cathData) return false;

	vector <FLOAT> *vpoes = cathData->getPOESEnvVector();
	vector <FLOAT> *poesTime = cathData->getPOESEnvVectorTime();
	vector <FLOAT> *vpph = cathData->getPPHEnvVector();
	vector <FLOAT> *flow = cathData->getFlowVector();
	vector <FLOAT> *flowTime = cathData->getFlowVectorTime();
	vector <FLOAT> *pphTime = cathData->getPPHEnvVectorTime();
	vector <FLOAT> *vpGrad = cathData->getPgradientVector();
	vector <FLOAT> *tpGrad = cathData->getPgradientVectorTime();

	vector <FLOAT> *flowBaseline = cathData->getFlowBaselineVector();
	vector <FLOAT> *poesBaseline = cathData->getPOESBaselineVector();
	vector <FLOAT> *pphBaseline = cathData->getPPHBaselineVector();

	//---Find apnea (no flow) 
	int samplesApneaDur = (int)(APNEA_DEFAULT_DURATION / FILTER_DESIGN_SAMPLE_INTERVAL);
	int samplesMaxApneaDur = (int)(MAX_APNEA_DUR / FILTER_DESIGN_SAMPLE_INTERVAL);
	int samplesCentralPartDur = (int)(APNEA_HALF_DURATION / FILTER_DESIGN_SAMPLE_INTERVAL);

	vector <unsigned short>::iterator usIt, usItEnd;
	vector <FLOAT>::iterator helpTimeIt = _helpVTimeP->begin();
	for (usIt = _helpVP->begin(); usIt < _helpVP->end() - samplesApneaDur; usIt++, helpTimeIt++) {
		usItEnd = usIt + samplesApneaDur;
		int countNoFlow = count_if(usIt, usItEnd, isZeroFlow);

		//---Extend to end
		if (countNoFlow >= samplesApneaDur) {
			while ((*usItEnd & zeroFlow) && (usItEnd < (_helpVP->end() - 1)) && (distance(usIt, usItEnd) <= samplesMaxApneaDur))
				++usItEnd;
			countNoFlow = distance(usIt, usItEnd);
		}

		if ((countNoFlow >= samplesApneaDur) && (countNoFlow < samplesMaxApneaDur)) {

			//---Count dead POES and dead PPH
			int countDeadPoes = count_if(usIt, usItEnd, isDeadPoes);
			int countDeadPph = count_if(usIt, usItEnd, isDeadPph);

			//---Central
			if (areAlmostEqual(countNoFlow, countDeadPoes, countDeadPph)) {
				FLOAT from = *helpTimeIt;
				FLOAT to = *(helpTimeIt + countNoFlow);
				FLOAT length = to - from;
				if (length < detectionCriteria.maxEventLength) {
					short bp = bpData->getPos(from, to);
					CCentralEvnt *evC = new CCentralEvnt(from, to, startRecordingClockTime, bp);
					gradientDataToEvent((CEvnt *)evC, vpGrad, tpGrad, from, to);
					flowDataToEvent((CEvnt *)evC, flow, flowTime, from, to);
					poesEnvDataToEvent((CEvnt *)evC, vpoes, poesTime, from, to);
					pphEnvDataToEvent((CEvnt *)evC, vpph, pphTime, from, to);
					centralEventArray.Add(evC);
				}
				usIt += countNoFlow;
				helpTimeIt += countNoFlow;
			}
			//---Mixed
			else if ((countDeadPoes > samplesCentralPartDur) && (countDeadPph > samplesCentralPartDur)) {
				FLOAT from = *helpTimeIt;
				FLOAT frompgrad = *(helpTimeIt + countDeadPoes);
				FLOAT to = *(helpTimeIt + countNoFlow);
				FLOAT length = to - from;
				if (length < detectionCriteria.maxEventLength) {
					short bp = bpData->getPos(from, to);
					CMixedEvnt *evM = new CMixedEvnt(from, to, startRecordingClockTime, bp);
					gradientDataToEvent((CEvnt *)evM, vpGrad, tpGrad, frompgrad, to); // Level analysis on obstructive part only!
					flowDataToEvent((CEvnt *)evM, flow, flowTime, from, to);
					poesEnvDataToEvent((CEvnt *)evM, vpoes, poesTime, from, to);
					pphEnvDataToEvent((CEvnt *)evM, vpph, pphTime, from, to);
					evM->doLevelAnalysis();
					mixedEventArray.Add(evM);
				}
				usIt += countNoFlow;
				helpTimeIt += countNoFlow;
			}
			//---Obstructive
			else {
				FLOAT from = *helpTimeIt;
				FLOAT to = *(helpTimeIt + countNoFlow);
				FLOAT length = to - from;
				if (length < detectionCriteria.maxEventLength) {
					short bp = bpData->getPos(from, to);
					CObstrEvnt *evO = new CObstrEvnt(from, to, startRecordingClockTime, bp);

					gradientDataToEvent((CEvnt *)evO, vpGrad, tpGrad, from, to);
					flowDataToEvent((CEvnt *)evO, flow, flowTime, from, to);
					poesEnvDataToEvent((CEvnt *)evO, vpoes, poesTime, from, to);
					pphEnvDataToEvent((CEvnt *)evO, vpph, pphTime, from, to);
					evO->doLevelAnalysis();
					obstrEventArray.Add(evO);
				}
				usIt += countNoFlow;
				helpTimeIt += countNoFlow;
			}
		}
	}
	return true;
}

bool CEvents::findHypopnea(vector <FLOAT> *_helpVTimeP, vector <unsigned short> *_helpVP,int _spO2DropLimit)
{
	CArray <CHypoEvnt *, CHypoEvnt *> *hP = NULL;
	if (spO2DropLimit3 == _spO2DropLimit) hP = &hypoEventArrayDesat3;
	else if (spO2DropLimit4 == _spO2DropLimit) hP = &hypoEventArrayDesat4;
	else return false;

	vector <FLOAT> *flow = cathData->getFlowVector();
	vector <FLOAT> *flowTime = cathData->getFlowVectorTime();
	vector <FLOAT> *vpGrad = cathData->getPgradientVector();
	vector <FLOAT> *tpGrad = cathData->getPgradientVectorTime();
	vector <FLOAT> *vpoes = cathData->getPOESEnvVector();
	vector <FLOAT> *poesTime = cathData->getPOESEnvVectorTime();
	vector <FLOAT> *vpph = cathData->getPPHEnvVector();
	vector <FLOAT> *pphTime = cathData->getPPHEnvVectorTime();

	int samplesApneaDur = (int)(APNEA_DEFAULT_DURATION / FILTER_DESIGN_SAMPLE_INTERVAL);
	int samplesHypopneaDur = samplesApneaDur * 2 / 3;	// Shorter because of detection and 
														// filtering process envelope curve
	int samplesMaxHypopneaDur = (int)(MAX_HYPOPNEA_DUR / FILTER_DESIGN_SAMPLE_INTERVAL);

	//----Hypopnea50
	vector <FLOAT>::iterator helpTimeIt = _helpVTimeP->begin(); 
	vector <unsigned short>::iterator usIt,usItEnd;
	for (usIt = _helpVP->begin() ; usIt < _helpVP->end() - samplesApneaDur ; usIt++, helpTimeIt++) {
		usItEnd = usIt + samplesHypopneaDur;
		int countLowFlow = count_if(usIt,usItEnd,isLowFlow50);
	
		if (countLowFlow >= samplesHypopneaDur) {
			while ((*usItEnd & lowFlow50) && (usItEnd < (_helpVP->end() - 1)) && (distance(usIt, usItEnd) <= samplesMaxHypopneaDur))
				++usItEnd;
			countLowFlow = distance(usIt, usItEnd);
		}
	
		if ((countLowFlow >= samplesHypopneaDur) && (countLowFlow < samplesMaxHypopneaDur)) {
			
			FLOAT from = *helpTimeIt;
			FLOAT to = *(helpTimeIt + countLowFlow);
			int diff = 0;
	
			//---Have to compensate length here as minimum count has been set lower due to 
			//---filtering when doing the envelope curve
			if (countLowFlow < samplesApneaDur) diff = (samplesApneaDur - countLowFlow) / 2;
			from -= FILTER_DESIGN_SAMPLE_INTERVAL * diff;
			to += FILTER_DESIGN_SAMPLE_INTERVAL * diff;
			
			//---Check for corresponding SpO2 drop
			float centerSpO2 = .0f;
			CSpO2DropEvnt *marriageCandidate = NULL;
	
			if (marriageCandidate = findSpO2DropToMarry(		from + detectionCriteria.spO2DropSeekWindowStart,
																from + detectionCriteria.spO2DropSeekWindowStart + detectionCriteria.spO2DropSeekWindowLength,
																&centerSpO2,_spO2DropLimit)) {
				
				FLOAT length = to - from;
				if (length < detectionCriteria.maxEventLength) { 
					short bp = bpData->getPos(from,to);
					CHypoEvnt *evH = new CHypoEvnt(from,to,startRecordingClockTime,bp,hypopneaDropLimit50);
				
					evH->setSpO2DropPartner(centerSpO2,true);
					marriageCandidate->setHypoPartner((from + to) / 2.0f,true);
	
					gradientDataToEvent((CEvnt *) evH,vpGrad,tpGrad,from,to);
					flowDataToEvent((CEvnt *) evH,flow,flowTime,from,to);
					poesEnvDataToEvent((CEvnt *) evH,vpoes,poesTime,from,to);
					pphEnvDataToEvent((CEvnt *)evH, vpph, pphTime, from, to);
					int type = evH->findHypoType();
					evH->doLevelAnalysis();
					hP->Add(evH);
				}
			}
		}
		usIt += countLowFlow;		 
		helpTimeIt += countLowFlow;	 
	}


	//----Hypopnea40
	helpTimeIt = _helpVTimeP->begin();
	for (usIt = _helpVP->begin(); usIt < _helpVP->end() - samplesApneaDur; usIt++, helpTimeIt++) {
		usItEnd = usIt + samplesHypopneaDur;
		int countLowFlow = count_if(usIt, usItEnd, isLowFlow40);
	
		if (countLowFlow >= samplesHypopneaDur) {
			while ((*usItEnd & lowFlow40) && (usItEnd < (_helpVP->end() - 1)) && (distance(usIt, usItEnd) <= samplesMaxHypopneaDur))
				++usItEnd;
			countLowFlow = distance(usIt, usItEnd);
		}
	
		if ((countLowFlow >= samplesHypopneaDur) && (countLowFlow < samplesMaxHypopneaDur)) {
	
			FLOAT from = *helpTimeIt;
			FLOAT to = *(helpTimeIt + countLowFlow);
			int diff = 0;
	
			//---Have to compensate length here as minimum count has been set lower due to 
			//---filtering when ding the envelope curve
			if (countLowFlow < samplesApneaDur) diff = (samplesApneaDur - countLowFlow) / 2;
			from -= FILTER_DESIGN_SAMPLE_INTERVAL * diff;
			to += FILTER_DESIGN_SAMPLE_INTERVAL * diff;
	
			//---Check for corresponding SpO2 drop
			float centerSpO2 = .0f;
			CSpO2DropEvnt *marriageCandidate = NULL;
	
			if (marriageCandidate = findSpO2DropToMarry(from + detectionCriteria.spO2DropSeekWindowStart,
				from + detectionCriteria.spO2DropSeekWindowStart + detectionCriteria.spO2DropSeekWindowLength,
				&centerSpO2,_spO2DropLimit)) {
				FLOAT length = to - from;
				if (length < detectionCriteria.maxEventLength) {
					short bp = bpData->getPos(from, to);
					CHypoEvnt *evH = new CHypoEvnt(from, to, startRecordingClockTime, bp, hypopneaDropLimit40);
	
					evH->setSpO2DropPartner(centerSpO2, true);
					marriageCandidate->setHypoPartner((from + to) / 2.0f, true);
	
					gradientDataToEvent((CEvnt *)evH, vpGrad, tpGrad, from, to);
					flowDataToEvent((CEvnt *)evH, flow, flowTime, from, to);
					poesEnvDataToEvent((CEvnt *)evH, vpoes, poesTime, from, to);
					pphEnvDataToEvent((CEvnt *)evH, vpph, pphTime, from, to);
					int type = evH->findHypoType();
					evH->doLevelAnalysis();
					hP->Add(evH);
				}
			}
		}
		usIt += countLowFlow;
		helpTimeIt += countLowFlow;
	}

	//----Hypopnea30
	helpTimeIt = _helpVTimeP->begin();
	for (usIt = _helpVP->begin(); usIt < _helpVP->end() - samplesApneaDur; usIt++, helpTimeIt++) {
		usItEnd = usIt + samplesHypopneaDur;
		int countLowFlow = count_if(usIt, usItEnd, isLowFlow30);

		if (countLowFlow >= samplesHypopneaDur) {
			while ((*usItEnd & lowFlow30) && (usItEnd < (_helpVP->end() - 1)) && (distance(usIt, usItEnd) <= samplesMaxHypopneaDur))
				++usItEnd;
			countLowFlow = distance(usIt, usItEnd);
		}

		if ((countLowFlow >= samplesHypopneaDur) && (countLowFlow < samplesMaxHypopneaDur)) {

			FLOAT from = *helpTimeIt;
			FLOAT to = *(helpTimeIt + countLowFlow);
			int diff = 0;

			//---Have to compensate length here as minimum count has been set lower due to 
			//---filtering when ding the envelope curve
			if (countLowFlow < samplesApneaDur) diff = (samplesApneaDur - countLowFlow) / 2;
			from -= FILTER_DESIGN_SAMPLE_INTERVAL * diff;
			to += FILTER_DESIGN_SAMPLE_INTERVAL * diff;

			//---Check for corresponding SpO2 drop
			float centerSpO2 = .0f;
			CSpO2DropEvnt *marriageCandidate = NULL;

			if (marriageCandidate = findSpO2DropToMarry(from + detectionCriteria.spO2DropSeekWindowStart,
				from + detectionCriteria.spO2DropSeekWindowStart + detectionCriteria.spO2DropSeekWindowLength,
				&centerSpO2,_spO2DropLimit)) {
				FLOAT length = to - from;
				if (length < detectionCriteria.maxEventLength) {
					short bp = bpData->getPos(from, to);
					CHypoEvnt *evH = new CHypoEvnt(from, to, startRecordingClockTime, bp, hypopneaDropLimit30);

					evH->setSpO2DropPartner(centerSpO2, true);
					marriageCandidate->setHypoPartner((from + to) / 2.0f, true);

					gradientDataToEvent((CEvnt *)evH, vpGrad, tpGrad, from, to);
					flowDataToEvent((CEvnt *)evH, flow, flowTime, from, to);
					poesEnvDataToEvent((CEvnt *)evH, vpoes, poesTime, from, to);
					pphEnvDataToEvent((CEvnt *)evH, vpph, pphTime, from, to);
					int type = evH->findHypoType();
					evH->doLevelAnalysis();
					hP->Add(evH);
				}
			}
		}
		usIt += countLowFlow;
		helpTimeIt += countLowFlow;
	}

	removeConflictingHypos(_spO2DropLimit); 

	//---Clean up overlapping events
	removeHypoWhenObstrCentralMixEvent(_spO2DropLimit);

	return true;
}

void CEvents::removeRERAAtTime(float _cTime, int _hypoLimit, int _spO2DropLimit)
{
	if (spO2DropLimit3 == _spO2DropLimit) {
		int num = arousalEventArrayDesat3.GetCount();
		for (int i = num - 1; i >= 0; --i) {
			CArousalEvnt *eP = arousalEventArrayDesat3.GetAt(i);
			CRERAEvnt *rP = eP->getRERAEvnt();
			if (rP) {
				float myFrom = rP->getFrom();
				float myTo = rP->getTo();
				if ((_cTime >= myFrom) && (_cTime <= myTo)) {
					eP->removeRERAEvnt();
					return;
				}
			}
		}
	}

	if (spO2DropLimit4 == _spO2DropLimit) {
		int num = arousalEventArrayDesat4.GetCount();
		for (int i = num - 1; i >= 0; --i) {
			CArousalEvnt *eP = arousalEventArrayDesat4.GetAt(i);
			CRERAEvnt *rP = eP->getRERAEvnt();
			if (rP) {
				float myFrom = rP->getFrom();
				float myTo = rP->getTo();
				if ((_cTime >= myFrom) && (_cTime <= myTo)) {
					eP->removeRERAEvnt();
					return;
				}
			}
		}
	}
}

bool CEvents::getIsInsideMixed(	float _from,float _to)
{
	int num = mixedEventArray.GetCount();
	for (int i = 0 ; i < num ; ++i) {
		CMixedEvnt *eP = mixedEventArray.GetAt(i);
		if (eP->getIsActive()) {
			float myFrom = eP->getFrom();
			float myTo = eP->getTo();
			if ((_from >= myFrom) && (_to <= myTo))
				return true;
		}
	}
	return false;
}

bool CEvents::getIsInsideCentral(float _from,float _to)
{
	int num = centralEventArray.GetCount();
	for (int i = 0 ; i < num ; ++i) {
		CCentralEvnt *eP = centralEventArray.GetAt(i);
		if (eP->getIsActive()) {
			float myFrom = eP->getFrom();
			float myTo = eP->getTo();
			if ((_from >= myFrom) && (_to <= myTo))
				return true;
		}
	}
	return false;
}

bool CEvents::getIsInsideObstructive(float _from,float _to)
{
	int num = obstrEventArray.GetCount();
	for (int i = 0 ; i < num ; ++i) {
		CObstrEvnt *eP = obstrEventArray.GetAt(i);
		if (eP->getIsActive()) {
			float myFrom = eP->getFrom();
			float myTo = eP->getTo();
			if ((_from >= myFrom) && (_to <= myTo))
				return true;
		}
	}
	return false;
}

bool CEvents::getIsInsideHypopnea(	float _from,float _to, int _hypoLimit,int _spO2DropLimit)
{
	if (spO2DropLimit3 == _spO2DropLimit) {
		int num = hypoEventArrayDesat3.GetCount();
		for (int i = 0; i < num; ++i) {
			CHypoEvnt *eP = hypoEventArrayDesat3.GetAt(i);
			if (eP->getIsActive() && eP->pickFromDropLevel(_hypoLimit)) {
				float myFrom = eP->getFrom();
				float myTo = eP->getTo();
				if ((_from >= myFrom) && (_to <= myTo))
					return true;
			}
		}
	}
	else if (spO2DropLimit4 == _spO2DropLimit) {
		int num = hypoEventArrayDesat4.GetCount();
		for (int i = 0; i < num; ++i) {
			CHypoEvnt *eP = hypoEventArrayDesat4.GetAt(i);
			if (eP->getIsActive()) {
				float myFrom = eP->getFrom();
				float myTo = eP->getTo();
				if ((_from >= myFrom) && (_to <= myTo))
					return true;
			}
		}
	}
	return false;
}


bool CEvents::getCollisionWithObstr(float _from,float _to)
{
	int num = obstrEventArray.GetCount();
	for (int i = 0 ; i < num ; ++i) {
		CObstrEvnt *eP = obstrEventArray.GetAt(i);
		if (eP->getIsActive()) {
			float myFrom = eP->getFrom();
			float myTo = eP->getTo();
			if ((_from < myFrom) && (_to > myTo))
				return true;
			if ((_from >= myFrom) && (_from <= myTo))
				return true;
			if ((_to >= myFrom) && (_to <= myTo))
				return true;
		}
	}
	return false;
}
bool CEvents::getCollisionWithCentral(	float _from,float _to)
{
	int num = centralEventArray.GetCount();
	for (int i = 0 ; i < num ; ++i) {
		CCentralEvnt *eP = centralEventArray.GetAt(i);
		if (eP->getIsActive()) {
			float myFrom = eP->getFrom();
			float myTo = eP->getTo();
			if ((_from < myFrom) && (_to > myTo))
				return true;
			if ((_from >= myFrom) && (_from <= myTo))
				return true;
			if ((_to >= myFrom) && (_to <= myTo))
				return true;
		}
	}
	return false;
}

bool CEvents::getCollisionWithMix(	float _from,float _to)
{
	int num = mixedEventArray.GetCount();
	for (int i = 0 ; i < num ; ++i) {
		CMixedEvnt *eP = mixedEventArray.GetAt(i);
		if (eP->getIsActive()) {
			float myFrom = eP->getFrom();
			float myTo = eP->getTo();
			if ((_from < myFrom) && (_to > myTo))
				return true;
			if ((_from >= myFrom) && (_from <= myTo))
				return true;
			if ((_to >= myFrom) && (_to <= myTo))
				return true;
		}
	}
	return false;
}


bool CEvents::getCollisionWithHypo(float _from, float _to, int _hypoLimit, int _spO2DropLimit)
{
	if (spO2DropLimit3 == _spO2DropLimit) {
		int num = hypoEventArrayDesat3.GetCount();
		for (int i = 0; i < num; ++i) {
			CHypoEvnt *eP = hypoEventArrayDesat3.GetAt(i);
			if (eP->getIsActive() && (eP->pickFromDropLevel(_hypoLimit))) {
				float myFrom = eP->getFrom();
				float myTo = eP->getTo();
				if ((_from < myFrom) && (_to > myTo))
					return true;
				if ((_from >= myFrom) && (_from <= myTo))
					return true;
				if ((_to >= myFrom) && (_to <= myTo))
					return true;
			}
		}
	}
	else if (spO2DropLimit4 == _spO2DropLimit) {
		int num = hypoEventArrayDesat4.GetCount();
		for (int i = 0; i < num; ++i) {
			CHypoEvnt *eP = hypoEventArrayDesat4.GetAt(i);
			if (eP->getIsActive() && (eP->pickFromDropLevel(_hypoLimit))) {
				float myFrom = eP->getFrom();
				float myTo = eP->getTo();
				if ((_from < myFrom) && (_to > myTo))
					return true;
				if ((_from >= myFrom) && (_from <= myTo))
					return true;
				if ((_to >= myFrom) && (_to <= myTo))
					return true;
			}
		}
	}

	return false;
}

bool CEvents::getCollisionWithRERA(float _from, float _to, int _hypoLimit,int _spO2DropLimit)
{
	if (spO2DropLimit3 == _spO2DropLimit) {
		int num = arousalEventArrayDesat3.GetCount();
		for (int i = 0; i < num; ++i) {
			CArousalEvnt *eP = arousalEventArrayDesat3.GetAt(i);
			CRERAEvnt *rP = eP->getRERAEvnt();
			if (eP->getIsActive(_hypoLimit) && rP) {
				float myFrom = rP->getFrom();
				float myTo = rP->getTo();
				if ((_from < myFrom) && (_to > myTo))
					return true;
				if ((_from >= myFrom) && (_from <= myTo))
					return true;
				if ((_to >= myFrom) && (_to <= myTo))
					return true;
			}
		}
	}
	if (spO2DropLimit4 == _spO2DropLimit) {
		int num = arousalEventArrayDesat4.GetCount();
		for (int i = 0; i < num; ++i) {
			CArousalEvnt *eP = arousalEventArrayDesat4.GetAt(i);
			CRERAEvnt *rP = eP->getRERAEvnt();
			if (eP->getIsActive(_hypoLimit) && rP) {
				float myFrom = rP->getFrom();
				float myTo = rP->getTo();
				if ((_from < myFrom) && (_to > myTo))
					return true;
				if ((_from >= myFrom) && (_from <= myTo))
					return true;
				if ((_to >= myFrom) && (_to <= myTo))
					return true;
			}
		}
	}
	return false;
}

bool CEvents::getCollisionWithAwake(float _from,float _to)
{
	int num = awakeEventArray.GetCount();
	for (int i = 0 ; i < num ; ++i) {
		CAwakeEvnt *eP = awakeEventArray.GetAt(i);
		if (eP->getIsActive()) {
			float myFrom = eP->getFrom();
			float myTo = eP->getTo();
			if ((_from < myFrom) && (_to > myTo))
				return true;
			if ((_from >= myFrom) && (_from <= myTo))
				return true;
			if ((_to >= myFrom) && (_to <= myTo))
				return true;
		}
	}
	return false;
}

bool CEvents::getCollisionWithSnoring(	float _from,float _to)
{
	int num = snoringEventArray.GetCount();
	for (int i = 0 ; i < num ; ++i) {
		CSnoringEvnt *eP = snoringEventArray.GetAt(i);
		if (eP->getIsActive()) {
			float myFrom = eP->getFrom();
			float myTo = eP->getTo();
			if ((_from < myFrom) && (_to > myTo))
				return true;
			if ((_from >= myFrom) && (_from <= myTo))
				return true;
			if ((_to >= myFrom) && (_to <= myTo))
				return true;
		}
	}
	return false;
}
bool CEvents::getCollisionWithSpO2Drop(	float _from,float _to)
{
	int num = spO2DropEventArrayCurrentP->GetCount();
	for (int i = 0 ; i < num ; ++i) {
		CSpO2DropEvnt *eP = spO2DropEventArrayCurrentP->GetAt(i);
		if (eP->getIsActive()) {
			float myFrom = eP->getFrom();
			float myTo = eP->getTo();
			if ((_from < myFrom) && (_to > myTo))
				return true;
			if ((_from >= myFrom) && (_from <= myTo))
				return true;
			if ((_to >= myFrom) && (_to <= myTo))
				return true;
		}
	}
	return false;
}
bool CEvents::getCollisionWithArousal(	float _from,float _to, int _hypoLimit, int _spO2DropLimit)
{
	if (spO2DropLimit3 == _spO2DropLimit) {
		int num = arousalEventArrayDesat3.GetCount();
		for (int i = 0; i < num; ++i) {
			CArousalEvnt *eP = arousalEventArrayDesat3.GetAt(i);
			if (eP->getIsActive(_hypoLimit)) {
				float myFrom = eP->getFrom();
				float myTo = eP->getTo();
				if ((_from < myFrom) && (_to > myTo))
					return true;
				if ((_from >= myFrom) && (_from <= myTo))
					return true;
				if ((_to >= myFrom) && (_to <= myTo))
					return true;
			}
		}
	}

	if (spO2DropLimit4 == _spO2DropLimit) {
		int num = arousalEventArrayDesat4.GetCount();
		for (int i = 0; i < num; ++i) {
			CArousalEvnt *eP = arousalEventArrayDesat4.GetAt(i);
			if (eP->getIsActive(_hypoLimit)) {
				float myFrom = eP->getFrom();
				float myTo = eP->getTo();
				if ((_from < myFrom) && (_to > myTo))
					return true;
				if ((_from >= myFrom) && (_from <= myTo))
					return true;
				if ((_to >= myFrom) && (_to <= myTo))
					return true;
			}
		}
	}
	return false;
}

bool CEvents::getCollisionWithExcluded(	float _from,float _to)
{
	int num = excludedEventArray.GetCount();
	for (int i = 0 ; i < num ; ++i) {
		CExcludedEvnt *eP = excludedEventArray.GetAt(i);
		float myFrom = eP->getFrom();
		float myTo = eP->getTo();
		if ((_from < myFrom) && (_to > myTo)) 
			return true;
		if ((_from >= myFrom) && (_from <= myTo)) 
			return true;
		if ((_to >= myFrom) && (_to <= myTo)) 
			return true;
	}
	return false;
}
bool CEvents::getCollisionWithPrePostBedtime(float _from,float _to)
{
	float myFrom = effectiveStartTime;
	float myTo = effectiveStopTime;

	if (_from < myFrom) 
		return true;			// Starts too early
	if (_from >= myTo) 
		return true;				// Starts too late
	if (_to < myFrom) 
		return true;				// Stops too early
	if (_to > myTo) 
		return true;				// Stops too late
	return false;
}

int CEvents::getLevel(float _gradient)
{
	if (_gradient >= detectionCriteria.gradientUpperLimit) return levelTypeUpper;
	else if (_gradient <= detectionCriteria.gradientLowerLimit) return levelTypeLower;
	else return levelTypeMulti;
}

void CEvents::POESEnvBinning(void)
{
	if (!cathData) return;

	if (1 >= cathData->getSize()) return;

	vector <FLOAT> *_v = cathData->getPOESEnvVector();
	vector <FLOAT> *_vt = cathData->getPOESEnvVectorTime();
	vector <FLOAT> *_grad = cathData->getPgradientVector();
	vector <FLOAT> *_gradt = cathData->getPgradientVectorTime();

	if (0 == _vt->size()) return;

	vector <FLOAT>::iterator vIt = _v->begin();
	vector <FLOAT>::iterator vtIt = _vt->begin();

	//---Assume regular sampling!!
	float interval = *(vtIt + 1) - *vtIt;
	
	//---Reset POES histograms
	for (int i = 0 ; i < ppBins ; ++i) {
		poesBin[i] = 0;
		poesCumulBin[i] = 0;
	}
	for (int i = 0 ; i < ppHistoBins ; ++i) {
		for (int j = 0; j < levelTypeCount; j++) poesHistoBinLevel[i][j] = 0;
		for (int j = 0; j < posCount; ++j) poesHistoBinBodyPos[i][j] = 0;
	}
	vector <FLOAT>::iterator itg;

	//---All over
	for (; vIt < _v->end(); ++vIt, ++vtIt) {
		if (!getIsExcluded(*vtIt) && !getIsAwake(*vtIt)) {
			int press = (int)*vIt;
			press = press >= ppBins ? ppBins - 1 : press;
			press = press < 0 ? 0 : press;
			poesBin[press] += 1.0f;
			int ph = (int)(*vIt / ppHistoBinStep);
			itg = lower_bound(_gradt->begin(), _gradt->end(), *vtIt);
			int lvl = levelTypeUndef;
			if (itg >= _gradt->end()) lvl = levelTypeUndef;
			else {
				float gradient = _grad->at(distance(_gradt->begin(), itg));
				lvl = getLevel(gradient);
			}
			ph = ph >= ppHistoBins ? ppHistoBins - 1 : ph;
			ph = ph < 0 ? 0 : ph;
			poesHistoBinLevel[ph][lvl] += 1.0f;
			poesHistoBinBodyPos[ph][bpData ? bpData->getPos(*vtIt, *vtIt) : posUndefined] += 1.0f;
		}
	}

	//---Compute seconds
	for (int i = 0 ; i < ppBins ; ++i) {
		poesBin[i] = interval * poesBin[i];
	}
	for (int i = 0 ; i < ppHistoBins ; ++i) {
		for (int j = 0 ; j < levelTypeCount ; j++) poesHistoBinLevel[i][j] = interval * poesHistoBinLevel[i][j];
		for (int j = 0 ; j < posCount ; ++j) poesHistoBinBodyPos[i][j] = interval * poesHistoBinBodyPos[i][j];	
	}

	//--Cumulative
	for (int i = 0 ; i < ppBins ; ++i) {
		for (int j = i ; j < ppBins ; ++j) {
			poesCumulBin[i] += poesBin[j];
		}
	}
}

void CEvents::POESEnvBinningForSnoring(void)
{
	if (!cathData) return;

	if (1 >= cathData->getSize()) return;

	vector <FLOAT> *_v = cathData->getPOESEnvVector();
	vector <FLOAT> *_vt = cathData->getPOESEnvVectorTime();

	if (0 == _vt->size()) return;

	vector <FLOAT>::iterator vIt = _v->begin();
	vector <FLOAT>::iterator vtIt = _vt->begin();

	//---Assume regular sampling!!
	float interval = *(vtIt + 1) - *vtIt;

	//---Reset
	for (int i = 0; i < ppBins; ++i) {
		snoringPoesBin[i] = 0;
		snoringPoesCumulBin[i] = 0;
	}
	for (int i = 0; i < ppHistoBins; ++i) {
		for (int j = 0; j < levelTypeCount; j++) snoringPoesHistoBinLevel[i][j] = 0;
		for (int j = 0; j < posCount; ++j) snoringPoesHistoBinBodyPos[i][j] = 0;
	}

	vIt = _v->begin();
	vtIt = _vt->begin();
	for (; vIt < _v->end(); ++vIt, ++vtIt) {
		if (getIsSnoring(*vtIt)) {
			int press = (int)*vIt;
			press = press >= ppBins ? ppBins - 1 : press;
			press = press < 0 ? 0 : press;
			snoringPoesBin[press] += 1.0f;
			int ph = (int)(*vIt / ppHistoBinStep);
			
			//---New code
			int lvl = levelTypeUndef;
			int sample = (int) (*vtIt * EVENT_TIMELINE_TIME_INTERVAL_INVERSE);
			sample = sample >= (int) eventTimelineVector.size() ? eventTimelineVector.size() - 1 : sample;
			sample = sample < 0 ? 0 : sample;
			UINT state = eventTimelineVector.at(sample);
			if (state & evntSnoringLevelUpper) lvl = levelTypeUpper;
			else if (state & evntSnoringLevelLower) lvl = levelTypeLower;
			else if (state & evntSnoringLevelMulti) lvl = levelTypeMulti;
			else if (state & evntSnoringLevelUndef) lvl = levelTypeUndef;
			//---

			ph = ph >= ppHistoBins ? ppHistoBins - 1 : ph;
			ph = ph < 0 ? 0 : ph;
			snoringPoesHistoBinLevel[ph][lvl] += 1.0f;
			snoringPoesHistoBinBodyPos[ph][bpData ? bpData->getPos(*vtIt, *vtIt) : posUndefined] += 1.0f;
		}
	}

	//---Compute seconds
	for (int i = 0; i < ppBins; ++i) {
		snoringPoesBin[i] = interval * snoringPoesBin[i];
	}
	for (int i = 0; i < ppHistoBins; ++i) {
		for (int j = 0; j < levelTypeCount; j++) snoringPoesHistoBinLevel[i][j] = interval * snoringPoesHistoBinLevel[i][j];
		for (int j = 0; j < posCount; ++j) snoringPoesHistoBinBodyPos[i][j] = interval * snoringPoesHistoBinBodyPos[i][j];
	}

	//--Cumulative
	for (int i = 0; i < ppBins; ++i) {
		for (int j = i; j < ppBins; ++j) {
			snoringPoesCumulBin[i] += snoringPoesBin[j];
		}
	}
}

void CEvents::PPHEnvBinning(void)
{
	if (!cathData) return;

	if (1 >= cathData->getSize()) return;

	vector <FLOAT> *_v = cathData->getPPHEnvVector();
	vector <FLOAT> *_vt = cathData->getPPHEnvVectorTime();
	vector <FLOAT> *_grad = cathData->getPgradientVector();
	vector <FLOAT> *_gradt = cathData->getPgradientVectorTime();

	if (0 == _vt->size()) return;

	vector <FLOAT>::iterator vIt = _v->begin();
	vector <FLOAT>::iterator vtIt = _vt->begin();

	//---Assume regular sampling!!
	float interval = *(vtIt + 1) - *vtIt;

	//---Reset PPH histograms
	for (int i = 0; i < ppBins; ++i) {
		pphBin[i] = 0;
		pphCumulBin[i] = 0;
	}
	for (int i = 0; i < ppHistoBins; ++i) {
		for (int j = 0; j < levelTypeCount; j++) pphHistoBinLevel[i][j] = 0;
		for (int j = 0; j < posCount; ++j) pphHistoBinBodyPos[i][j] = 0;
	}
	vector <FLOAT>::iterator itg;

	//---All over
	for (; vIt < _v->end(); ++vIt, ++vtIt) {
		if (!getIsExcluded(*vtIt) && !getIsAwake(*vtIt)) {
			int press = (int)*vIt;
			press = press >= ppBins ? ppBins - 1 : press;
			press = press < 0 ? 0 : press;
			pphBin[press] += 1.0f;
			int ph = (int)(*vIt / ppHistoBinStep);
			itg = lower_bound(_gradt->begin(), _gradt->end(), *vtIt);
			int lvl = levelTypeUndef;
			if (itg >= _gradt->end()) lvl = levelTypeUndef;
			else {
				float gradient = _grad->at(distance(_gradt->begin(), itg));
				lvl = getLevel(gradient);
			}
			ph = ph >= ppHistoBins ? ppHistoBins - 1 : ph;
			ph = ph < 0 ? 0 : ph;
			pphHistoBinLevel[ph][lvl] += 1.0f;
			pphHistoBinBodyPos[ph][bpData ? bpData->getPos(*vtIt, *vtIt) : posUndefined] += 1.0f;
		}
	}

	//---Compute seconds
	for (int i = 0; i < ppBins; ++i) {
		pphBin[i] = interval * pphBin[i];
	}
	for (int i = 0; i < ppHistoBins; ++i) {
		for (int j = 0; j < levelTypeCount; j++) pphHistoBinLevel[i][j] = interval * pphHistoBinLevel[i][j];
		for (int j = 0; j < posCount; ++j) pphHistoBinBodyPos[i][j] = interval * pphHistoBinBodyPos[i][j];
	}

	//--Cumulative
	for (int i = 0; i < ppBins; ++i) {
		for (int j = i; j < ppBins; ++j) {
			pphCumulBin[i] += pphBin[j];
		}
	}
}

void CEvents::PPHEnvBinningForSnoring(void)
{
	if (!cathData) return;

	if (1 >= cathData->getSize()) return;

	vector <FLOAT> *_v = cathData->getPPHEnvVector();
	vector <FLOAT> *_vt = cathData->getPPHEnvVectorTime();

	if (0 == _vt->size()) return;

	vector <FLOAT>::iterator vIt = _v->begin();
	vector <FLOAT>::iterator vtIt = _vt->begin();

	//---Assume regular sampling!!
	float interval = *(vtIt + 1) - *vtIt;

	//---Reset
	for (int i = 0; i < ppBins; ++i) {
		snoringPphBin[i] = 0;
		snoringPphCumulBin[i] = 0;
	}
	for (int i = 0; i < ppHistoBins; ++i) {
		for (int j = 0; j < levelTypeCount; j++) snoringPphHistoBinLevel[i][j] = 0;
		for (int j = 0; j < posCount; ++j) snoringPphHistoBinBodyPos[i][j] = 0;
	}

	vIt = _v->begin();
	vtIt = _vt->begin();
	for (; vIt < _v->end(); ++vIt, ++vtIt) {
		if (getIsSnoring(*vtIt)) {
			int press = (int)*vIt;
			press = press >= ppBins ? ppBins - 1 : press;
			press = press < 0 ? 0 : press;
			snoringPphBin[press] += 1.0f;
			int ph = (int)(*vIt / ppHistoBinStep);

			//---New code
			int lvl = levelTypeUndef;
			int sample = (int)(*vtIt * EVENT_TIMELINE_TIME_INTERVAL_INVERSE);
			sample = sample >= (int)eventTimelineVector.size() ? eventTimelineVector.size() - 1 : sample;
			sample = sample < 0 ? 0 : sample;
			UINT state = eventTimelineVector.at(sample);
			if (state & evntSnoringLevelUpper) lvl = levelTypeUpper;
			else if (state & evntSnoringLevelLower) lvl = levelTypeLower;
			else if (state & evntSnoringLevelMulti) lvl = levelTypeMulti;
			else if (state & evntSnoringLevelUndef) lvl = levelTypeUndef;
			//---

			ph = ph >= ppHistoBins ? ppHistoBins - 1 : ph;
			ph = ph < 0 ? 0 : ph;
			snoringPphHistoBinLevel[ph][lvl] += 1.0f;
			snoringPphHistoBinBodyPos[ph][bpData ? bpData->getPos(*vtIt, *vtIt) : posUndefined] += 1.0f;
		}
	}

	//---Compute seconds
	for (int i = 0; i < ppBins; ++i) {
		snoringPphBin[i] = interval * snoringPphBin[i];
	}
	for (int i = 0; i < ppHistoBins; ++i) {
		for (int j = 0; j < levelTypeCount; j++) snoringPphHistoBinLevel[i][j] = interval * snoringPphHistoBinLevel[i][j];
		for (int j = 0; j < posCount; ++j) snoringPphHistoBinBodyPos[i][j] = interval * snoringPphHistoBinBodyPos[i][j];
	}

	//--Cumulative
	for (int i = 0; i < ppBins; ++i) {
		for (int j = i; j < ppBins; ++j) {
			snoringPphCumulBin[i] += snoringPphBin[j];
		}
	}
}

/*
Description:
Based on an on/off vector, generate density (events per minute) vector
We assume that there are more data than _minutes  minutes!!
*/
void CEvents::fillDensityVector(vector <bool> *_onV, vector <FLOAT> *_densV, float _minutes)
{
	ASSERT(_onV->size() == _densV->size());
	ASSERT(_minutes > .0f);

	vector <bool>::iterator iO0 = _onV->begin();
	vector <bool>::iterator iO1 = _onV->begin() + numSamplesForEventDensityWindow;
	int num = count(iO0, iO1, true);
	float density = (float)num / _minutes;
	vector <FLOAT>::iterator iD0 = _densV->begin() + numSamplesForEventDensityWindow / 2;
	fill(_densV->begin(), iD0, density);
	*iD0 = density;
	for (; iO1 < _onV->end(); ++iO0, ++iO1,++iD0) {
		int num = count(iO0, iO1, true);
		density = (float)num / _minutes;
		*iD0 = density;
	}
	fill(iD0,_densV->end(), density);
}

/*
Description: Generate vector to be used with adwanced awake detection. Requires more 
than numSamplesForEventDensityWindow samples (5 minutes)
Returns:	true if ok
			false if not enough data
*/
bool CEvents::generateVectorsForAwakeDet(void)
{
	densityMixe.clear();
	densityObst.clear();
	densityCent.clear();
	densityHypo.clear();
	densityDesa.clear();
	densityRERA.clear();
	densitySwal.clear();
	densitySnor.clear();
	densityStan.clear();
	densitySumm.clear();
	densityTime.clear();

	//---Time vector
	for (float t = .0f; t < maxStopTime; t += densityTimeStepSize) {
		densityTime.push_back(t); 
	}
	densityMixe.resize(densityTime.size(), .0f);
	densityObst.resize(densityTime.size(), .0f);
	densityCent.resize(densityTime.size(), .0f);
	densityHypo.resize(densityTime.size(), .0f);
	densityDesa.resize(densityTime.size(), .0f);
	densitySnor.resize(densityTime.size(), .0f);
	densityRERA.resize(densityTime.size(), .0f);
	densitySwal.resize(densityTime.size(), .0f);
	densityActi.resize(densityTime.size(), .0f);
	densityTAct.resize(densityTime.size(), .0f);
	densityStan.resize(densityTime.size(), .0f);
	densitySumm.resize(densityTime.size(), .0f);

	//---Build vectors
	vector <bool> onMixe;
	vector <bool> onObst;
	vector <bool> onCent;
	vector <bool> onHypo;
	vector <bool> onDesa;
	vector <bool> onSnor;
	vector <bool> onRERA;
	vector <bool> onSwal;
	vector <bool> onActi;
	vector <bool> onTAct;
	vector <bool> onStan;

	onMixe.resize(densityTime.size(),false);
	onObst.resize(densityTime.size(),false);
	onCent.resize(densityTime.size(),false);
	onHypo.resize(densityTime.size(),false);
	onDesa.resize(densityTime.size(),false);
	onRERA.resize(densityTime.size(),false);
	onSwal.resize(densityTime.size(),false);
	onSnor.resize(densityTime.size(),false);
	onActi.resize(densityTime.size(),false);
	onTAct.resize(densityTime.size(),false);
	onStan.resize(densityTime.size(),false);
	fillEventOnVector(&onMixe, &densityTime, (CArray <CEvnt *, CEvnt *> *) &mixedEventArray);
	fillEventOnVector(&onObst, &densityTime, (CArray <CEvnt *, CEvnt *> *) &obstrEventArray);
	fillEventOnVector(&onCent, &densityTime, (CArray <CEvnt *, CEvnt *> *) &centralEventArray);
	fillEventOnVector(&onHypo, &densityTime, (CArray <CEvnt *, CEvnt *> *) &hypoEventArrayDesat4);
	fillEventOnVector(&onDesa, &densityTime, (CArray <CEvnt *, CEvnt *> *) &spO2DropEventArray4);
	fillEventOnVector(&onRERA, &densityTime, &arousalEventArrayDesat4);
	fillEventOnVector(&onSwal, &densityTime, (CArray <CEvnt *, CEvnt *> *) &swallowEventArray);
	fillSnoringOnVector(&onSnor, &densityTime, &snoringEventArray);
	fillActimeterOnVector(&onActi, &densityTime, acData->getBaselineVector(), 
													acData->getWindowedAcVector(), 
													acData->getWindowedAcTimeVector());
	fillTorsoActimeterOnVector(&onTAct, &densityTime, torsoAcData->getAcVector(), torsoAcData->getTimeaxisVector());
	fillUprightOnVector(&onStan, &densityTime, bpData);

	//---Density vectors
	if (numSamplesForEventDensityWindow >= densityTime.size()) return false;
	float minutes = numSamplesForEventDensityWindow * densityTimeStepSize / 60.0f;
	fillDensityVector(&onMixe, &densityMixe, minutes);
	fillDensityVector(&onObst, &densityObst, minutes);
	fillDensityVector(&onCent, &densityCent, minutes);
	fillDensityVector(&onHypo, &densityHypo, minutes);
	fillDensityVector(&onDesa, &densityDesa, minutes);
	fillDensityVector(&onRERA, &densityRERA, minutes);
	fillDensityVector(&onSwal, &densitySwal, minutes);
	fillDensityVector(&onSnor, &densitySnor, minutes);
	fillDensityVector(&onActi, &densityActi, minutes);
	fillDensityVector(&onTAct, &densityTAct, minutes);
	fillDensityVector(&onStan, &densityStan, minutes);

	//---Apply Weights
	transform(densityMixe.begin(), densityMixe.end(), densityMixe.begin(), MultValue <FLOAT>( - detectionCriteria.awakeMixeFactor)); // Direction sleep (-)
	transform(densityObst.begin(), densityObst.end(), densityObst.begin(), MultValue <FLOAT>( - detectionCriteria.awakeObstFactor)); // Direction sleep (-)
	transform(densityCent.begin(), densityCent.end(), densityCent.begin(), MultValue <FLOAT>( - detectionCriteria.awakeCentFactor)); // Direction sleep (-)
	transform(densityHypo.begin(), densityHypo.end(), densityHypo.begin(), MultValue <FLOAT>( - detectionCriteria.awakeHypoFactor)); // Direction sleep (-)
	transform(densityDesa.begin(), densityDesa.end(), densityDesa.begin(), MultValue <FLOAT>( - detectionCriteria.awakeDesaFactor)); // Direction sleep (-)
	transform(densityRERA.begin(), densityRERA.end(), densityRERA.begin(), MultValue <FLOAT>( - detectionCriteria.awakeRERAFactor)); // Direction sleep (-)
	transform(densitySnor.begin(), densitySnor.end(), densitySnor.begin(), MultValue <FLOAT>( - detectionCriteria.awakeSnorFactor)); // Direction sleep (-)
	transform(densitySwal.begin(), densitySwal.end(), densitySwal.begin(), MultValue <FLOAT>( + detectionCriteria.awakeSwalFactor)); // Direction awake (+)
	transform(densityActi.begin(), densityActi.end(), densityActi.begin(), MultValue <FLOAT>( + detectionCriteria.awakeActiFactor)); // Direction awake (+)
	transform(densityTAct.begin(), densityTAct.end(), densityTAct.begin(), MultValue <FLOAT>( + detectionCriteria.awakeTActFactor)); // Direction awake (+)
	transform(densityStan.begin(), densityStan.end(), densityStan.begin(), MultValue <FLOAT>( + detectionCriteria.awakeStanFactor)); // Direction awake (+)

	//---Make summ vector
	transform(densitySumm.begin(), densitySumm.end(), densityMixe.begin(), densitySumm.begin(), Sum <FLOAT>());
	transform(densitySumm.begin(), densitySumm.end(), densityObst.begin(), densitySumm.begin(), Sum <FLOAT>());
	transform(densitySumm.begin(), densitySumm.end(), densityCent.begin(), densitySumm.begin(), Sum <FLOAT>());
	transform(densitySumm.begin(), densitySumm.end(), densityHypo.begin(), densitySumm.begin(), Sum <FLOAT>());
	transform(densitySumm.begin(), densitySumm.end(), densityDesa.begin(), densitySumm.begin(), Sum <FLOAT>());
	transform(densitySumm.begin(), densitySumm.end(), densityRERA.begin(), densitySumm.begin(), Sum <FLOAT>());
	transform(densitySumm.begin(), densitySumm.end(), densitySnor.begin(), densitySumm.begin(), Sum <FLOAT>());
	transform(densitySumm.begin(), densitySumm.end(), densitySwal.begin(), densitySumm.begin(), Sum <FLOAT>());
	transform(densitySumm.begin(), densitySumm.end(), densityActi.begin(), densitySumm.begin(), Sum <FLOAT>());
	transform(densitySumm.begin(), densitySumm.end(), densityTAct.begin(), densitySumm.begin(), Sum <FLOAT>());
	transform(densitySumm.begin(), densitySumm.end(), densityStan.begin(), densitySumm.begin(), Sum <FLOAT>());

	return true;
}

/*
Description: Goes over event array and sets a mark in the dV vector with dT as corresponding time vector
As the pitch of the dT vector is long (typically 10 sec), two consecuitve events may be closer than 10 sec, hence
one count will be lost.
*/
void CEvents::fillEventOnVector(vector <bool> *_dV, vector <FLOAT> *_dT, CArray <CEvnt *, CEvnt *> *_eA)
{
	int num = _eA->GetCount();
	for (int i = 0; i < num; ++i) {
		CEvnt *eP = _eA->GetAt(i);
		vector <FLOAT>::iterator iT = lower_bound(_dT->begin(), _dT->end(), eP->getCentreTime());
		if (iT < _dT->end()) {
			int offset = distance(_dT->begin(), iT);
			vector <bool>::iterator iV = _dV->begin();
			iV += offset;
			*iV = true;
		}
	}
}

void CEvents::fillActimeterOnVector(vector <bool> *_dV, vector <FLOAT> *_dT, vector <FLOAT> *_baseline,
	vector <FLOAT> *_act, vector <FLOAT> *_actTime)
{
	ASSERT(_dV->size() == _dT->size());
	ASSERT(_baseline->size() == _actTime->size());
	ASSERT(_act->size() == _actTime->size());

	vector <FLOAT>::iterator theTime = _dT->begin();
	vector <bool>::iterator theOn = _dV->begin();
	vector <FLOAT>::iterator theAct = _act->begin(); 
	vector <FLOAT>::iterator baseLine = _baseline->begin();
	vector <FLOAT>::iterator i0;
	for (; theTime < _dT->end() ; ++theTime, ++theOn) {
		i0 = lower_bound(_actTime->begin(), _actTime->end(), *theTime);
		if (i0 < _actTime->end()) {
			int d0 = distance(_actTime->begin(), i0);
			FLOAT lim = detectionCriteria.actimeterAboveBaselineLimit + *(baseLine + d0);
			if (*(theAct + d0) > lim)
				*theOn = true;
		}
	}
}

void CEvents::fillTorsoActimeterOnVector(vector <bool> *_dV, vector <FLOAT> *_dT,
	vector <FLOAT> *_act, vector <FLOAT> *_actTime)
{
	//_actTime pitch << _dT pitch!!
	// Inn med detection criterion toroActimeterLimit
	ASSERT(_dV->size() == _dT->size());
	ASSERT(_act->size() == _actTime->size());

	vector <FLOAT>::iterator theTime = _dT->begin();
	vector <bool>::iterator theOn = _dV->begin();
	vector <FLOAT>::iterator theAct = _act->begin();
	vector <FLOAT>::iterator i0;
	for (; theTime < _dT->end(); ++theTime, ++theOn) {
		i0 = lower_bound(_actTime->begin(), _actTime->end(), *theTime);
		if (i0 < _actTime->end()) {
			int d0 = distance(_actTime->begin(), i0);
			if (*(theAct + d0) > detectionCriteria.torsoActimeterLimit)
				*theOn = true;
		}
	}
}


void CEvents::fillSnoringOnVector(vector <bool> *_dV, vector <FLOAT> *_dT, CArray <CSnoringEvnt *, CSnoringEvnt *> *_eA)
{
	int num = _eA->GetCount(); 
	for (int i = 0; i < num; ++i) {
		CEvnt *eP = _eA->GetAt(i);
		vector <FLOAT>::iterator iT0 = lower_bound(_dT->begin(), _dT->end(), eP->getFrom());
		vector <FLOAT>::iterator iT1 = lower_bound(_dT->begin(), _dT->end(), eP->getTo());
		if ((iT0 < _dT->end() && (iT1 < _dT->end()))) {
			int offset0 = distance(_dT->begin(), iT0);
			int offset1 = distance(_dT->begin(), iT1);
			vector <bool>::iterator iV0 = _dV->begin();
			vector <bool>::iterator iV1 = _dV->begin();
			iV0 += offset0;
			iV1 += offset1;
			fill(iV0, iV1, true);
		}
	}
}

void CEvents::fillEventOnVector(vector <bool> *_dV, vector <FLOAT> *_dT, CArray <CArousalEvnt *, CArousalEvnt *> *_eA)
{
	int num = _eA->GetCount();
	for (int i = 0; i < num; ++i) {
		CArousalEvnt *eP = _eA->GetAt(i);
		CRERAEvnt *rP = eP->getRERAEvnt();
		if (rP) {
			vector <FLOAT>::iterator iT = lower_bound(_dT->begin(), _dT->end(), rP->getCentreTime());
			if (iT < _dT->end()) {
				int offset = distance(_dT->begin(), iT);
				vector <bool>::iterator iV = _dV->begin();
				iV += offset;
				*iV = true;
			}
		}
	}
}

void CEvents::fillUprightOnVector(vector <bool> *_dV, vector <FLOAT> *_dT, CBodyPosDataSet *_bSet)
{
	if (NULL == _bSet) return;
	vector <FLOAT>::iterator iT = _dT->begin();
	for (; iT < _dT->end(); ++iT) {
		if (_bSet->getIUsUprightInWindow(*iT, *iT + densityTimeStepSize)) {
			int offset = distance(_dT->begin(), iT);
			vector <bool>::iterator iV = _dV->begin();
			iV += offset;
			*iV = true;
		}
	}
}

//////////////////////////////////////////////

CEvnt::CEvnt(FLOAT _from,FLOAT _to,COleDateTime _startRecording,
			 short _bodyPos,int _source /* = autoDetection */) :
	eventType(evntTypeNone),
	levelType(levelTypeUndef),
	detectionSource(autoDetection),
	from(_from),
	to(_to),
	bodyPos(_bodyPos),
	startRecording(_startRecording),
	eventFlags(EVNT_DEF_AUTO),
	hasLevelDetection(false)
{
	length = to - from;
	centreTime = (to + from) / 2.0f;
}

CEvnt::~CEvnt()
{

}


void CEvnt::serializeVectorFloat(CArchive & ar, vector <FLOAT>::iterator vBegin, vector <FLOAT>::iterator vEnd,
									vector <FLOAT> *_v)
{
	vector <FLOAT>::iterator it;	
	if (ar.IsStoring()) {
		int num = vEnd - vBegin;
		ar << num;
		for (it = vBegin; it < vEnd; ++it) 
			ar << *it;
	}
	else {
		_v->clear();
		int num;
		ar >> num;
		FLOAT val;
		for (int i = 0; i < num; i++) {
			ar >> val;
			_v->push_back(val);
		}
	}
}

void CEvnt::serializeVectorUint(CArchive & ar, vector <UINT>::iterator vBegin, vector <UINT>::iterator vEnd,
								vector <UINT> *_v)
{
	vector <UINT>::iterator it;
	if (ar.IsStoring()) {
		int num = vEnd - vBegin;
		ar << num;
		for (it = vBegin; it < vEnd; ++it)
			ar << *it;
	}
	else {
		_v->clear();
		int num;
		ar >> num;
		UINT val;
		for (int i = 0; i < num; i++) {
			ar >> val;
			_v->push_back(val);
		}
	}
}

void CEvnt::setTo(float _to)
{
	to = _to;
	length = to - from;
	centreTime = (to + from) / 2.0f;
}

void CEvnt::setFrom(float _from)
{
	from = _from;
	length = to - from;
	centreTime = (to + from) / 2.0f;
}

bool CEvnt::computeVectorData(vector<FLOAT> *_v, float * _min, float * _max, float * _avg, float * _median)
{
	if (0 == _v->size()) return false;

	if (1 == _v->size()) {
		*_min = _v-> at(0);
		*_max = _v->at(0);
		*_avg = _v->at(0);
		*_median = _v->at(0);
		return true;
	}

	vector <FLOAT> sorted;
	size_t sz = _v->size();
	sorted.resize(sz);
	copy(_v->begin(), _v->end(), sorted.begin());
	sort(sorted.begin(), sorted.end());
	*_max = sorted.at(sz - 1);
	*_min = sorted.at(0);
	if (0 == (sz % 2)) {
		int pos = -1 + (sz / 2);
		*_median = (sorted.at(pos) + sorted.at(pos + 1)) / 2.0f;
	}
	else {
		int pos = (int)((float)sz / 2.0f);
		*_median = sorted.at(pos);
	}
	*_avg = (float)for_each(sorted.begin(), sorted.end(), Average());
	return true;
}

void CEvnt::moveEdge(FLOAT _time,int _edge)
{
	switch (_edge) {
	case closestIsLeft :
		from += _time;
		break;
	case closestIsRight :
		to += _time;
		break;
	default:
		break;
	}
	length = to - from;
	centreTime = (to + from) / 2.0f;
	eventFlags |= (EVNT_DEF_MANUAL_LENGTH | EVNT_DEF_MANUAL);
}

	
void CEvnt::setPoesEnv(vector <FLOAT>::iterator _from,
					vector <FLOAT>::iterator _to,
					vector <FLOAT>::iterator _fromTime,
					vector <FLOAT>::iterator _toTime)
{
	poesEnv.clear();
	poesEnvTime.clear();
	vector <FLOAT>::iterator it;
	for (it = _from ; it < _to ; it++) poesEnv.push_back(*it);
	for (it = _fromTime ; it < _toTime ; it++) poesEnvTime.push_back(*it);
}

void CEvnt::setPPHEnv(vector<FLOAT>::iterator _from, vector<FLOAT>::iterator _to, vector<FLOAT>::iterator _fromTime, vector<FLOAT>::iterator _toTime)
{
	pphEnv.clear();
	pphEnvTime.clear();
	vector <FLOAT>::iterator it;
	for (it = _from; it < _to; it++) pphEnv.push_back(*it);
	for (it = _fromTime; it < _toTime; it++) pphEnvTime.push_back(*it);
}

void CEvnt::setPgradient(vector <FLOAT>::iterator _from,
					vector <FLOAT>::iterator _to,
					vector <FLOAT>::iterator _fromTime,
					vector <FLOAT>::iterator _toTime)
{
	pgrad.clear();
	pgradTime.clear();
	vector <FLOAT>::iterator it;
	for (it = _from ; it < _to ; it++) pgrad.push_back(*it);
	for (it = _fromTime ; it < _toTime ; it++) pgradTime.push_back(*it);
}


/*
Description: Checks if the time _atTime is on the edge of the event
*/
int CEvnt::getIsInOnEdge(float _atTime,float _tolerance)
{
	float delta = _tolerance * length;

	if ((_atTime >= from) && (_atTime <= (from + delta))) return (OVER_LEFT_EDGE | OVER_EDGE);

	if ((_atTime >= (to - delta)) && (_atTime <= to)) return (OVER_RIGHT_EDGE | OVER_EDGE);

	return OVER_NOTHING;
}

/*
Description: Checks if the time _atTime is within the limits of this event.
*/
bool CEvnt::getIsInEvent(float _atTime)
{
	if ((_atTime >= from) && (_atTime <= to)) return true;
	return false;
}

/*
Description: Checks if the time _atTime is within the limits of this event.
The limit is extended with 1 sec in each direction! To allow for some margin.
*/
bool CEvnt::isThisTheEvent(FLOAT _atTime,int *_closest)
{
	FLOAT low = from - 2.5f;
	FLOAT high = to + 2.5f;
	if (_atTime <= from) *_closest = closestIsLeft;
	else if (_atTime >= to) *_closest = closestIsRight;

	if ((_atTime < to) && (_atTime > from)) {
		float d1 = _atTime - from;
		float d2 = to - _atTime;
		*_closest = d1 > d2 ? closestIsRight : closestIsLeft;
		return true;
	}
	return false;
}

float CEvnt::getLength(void)
{
	return length;
}

/*
Description: This is empty, to be done in the subclasses
	Level analysis compares PPHEnv and pGradient (POES-PPH). 
	if pGradient > pphEnv, it is lower
	if pGradient < pphEnv, it is upper
	if pGradient ~ pphEnv, it is undefined 

	a 10% margin is required for the inequalities

	Can also be multilevel (future)
*/
void CEvnt::doLevelAnalysis(void)
{
	levelType = levelTypeUndef;

	if (!pgrad.size()) {
		levelType = levelTypeUndef;
		return;
	}

	ASSERT(pgrad.size() == pgradTime.size());

	levelType = getLevelFromPGradient(pgrad.begin(), pgrad.end());
}


UINT CEvnt::getLevelFromPGradient(vector<FLOAT>::iterator from, vector<FLOAT>::iterator to)
{
	int skipLength = distance(from, to) / 10;
	int numBelow40Perc = count_if(from + skipLength,to - skipLength, isBelow40percent);
	int numAbove60Perc = count_if(from + skipLength,to - skipLength, isAbove60percent);
	int numNear50Perc = count_if(from + skipLength,to - skipLength, isNear50percent);

	float totalNum = (float)distance(from, to) - 2.0f * skipLength;
	float below40 = (float)numBelow40Perc / totalNum;
	float above60 = (float)numAbove60Perc / totalNum;
	float near50 = (float)numNear50Perc / totalNum;

	UINT levelType = levelTypeUndef;

	if ((below40 > 0) && (above60 > 0)) levelType = levelTypeMulti;
	else if (below40 > .67f) levelType = levelTypeLower;
	//else if (near50 > .67f) levelType = levelTypeMulti;
	else if (above60 > .67f) levelType = levelTypeUpper;
	else levelType = levelTypeMulti;

	return levelType;
}

void CEvnt::setAirMicVectors(vector <FLOAT>::iterator _from0,
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
					vector <FLOAT>::iterator _toTime)
{
	airMic0Vector.clear();
	airMic1Vector.clear();
	airMic2Vector.clear();
	airMic3Vector.clear();
	airMicSumVector.clear();
	airMicTimeVector.clear();

	vector <FLOAT>::iterator it;
	for (it = _from0 ; it < _to0 ; it++) airMic0Vector.push_back(*it);
	for (it = _from1 ; it < _to1 ; it++) airMic1Vector.push_back(*it);
	for (it = _from2 ; it < _to2 ; it++) airMic2Vector.push_back(*it);
	for (it = _from3 ; it < _to3 ; it++) airMic3Vector.push_back(*it);
	for (it = _fromSum ; it < _toSum ; it++) airMicSumVector.push_back(*it);
	for (it = _fromTime ; it < _toTime ; it++) airMicTimeVector.push_back(*it);
}
	
void CEvnt::setContactMicVector(vector <FLOAT>::iterator _from,
					vector <FLOAT>::iterator _to,
					vector <FLOAT>::iterator _fromTime,
					vector <FLOAT>::iterator _toTime)
{
	contactMicVector.clear();
	contactMicTimeVector.clear();
	vector <FLOAT>::iterator it;
	for (it = _from ; it < _to ; it++) contactMicVector.push_back(*it);
	for (it = _fromTime ; it < _toTime ; it++) contactMicTimeVector.push_back(*it);
}

void CEvnt::setBodyPosVector(vector <FLOAT>::iterator _from,
					vector <FLOAT>::iterator _to,
					vector <FLOAT>::iterator _fromTime,
					vector <FLOAT>::iterator _toTime)
{
	bpVector.clear();
	bpTimeVector.clear();
	vector <FLOAT>::iterator it;
	for (it = _from ; it < _to ; it++) bpVector.push_back(*it);
	for (it = _fromTime ; it < _toTime ; it++) bpTimeVector.push_back(*it);
}

void CEvnt::setFlow(vector <FLOAT>::iterator _from,
					vector <FLOAT>::iterator _to,
					vector <FLOAT>::iterator _fromTime,
					vector <FLOAT>::iterator _toTime)
{
	flow.clear();
	flowTime.clear();
	vector <FLOAT>::iterator it;
	for (it = _from ; it < _to ; it++) flow.push_back(*it);
	for (it = _fromTime ; it < _toTime ; it++) flowTime.push_back(*it);
}

CString CEvnt::getWarning(void)
{
	return warning;
}

UINT CEvnt::getLevelTypeForMouseOver(void)
{
	UINT lv = getLevelType();

	UINT ret = OVER_UNDEFINED_LEVEL;
	switch (lv) {
	case levelTypeUpper:
		lv = OVER_UPPER;
		break;
	case levelTypeLower :
		lv = OVER_LOWER;
		break;
	case levelTypeMulti :
		lv = OVER_MULTI;
		break;
	case levelTypeUndef :
	default:
		lv = OVER_UNDEFINED_LEVEL;
		break;
	}
	return lv;
}


void CEvnt::setLevelType(UINT _levelType)
{
	levelType = _levelType;
}

UINT CEvnt::getLevelType(void)
{
	return levelType;
}

CString CEvnt::getLevelDescr(void)
{
	CString s;
	int num;
	switch (levelType) {
	case levelTypeUpper:
		num = s.LoadString(IDS_UPPER2);
		break;
	case levelTypeLower:
		num = s.LoadString(IDS_LOWER2);
		break;
	case levelTypeMulti:
		num = s.LoadString(IDS_MULTILEVEL2);
		break;
	case levelTypeUndef:
	default:
		num = s.LoadString(IDS_UNDEF2);
		break;
	}
	return s;
}

void CEvnt::setEventType(UINT _type)
{
	eventType = _type;
}

UINT CEvnt::getEventType(void)
{
	return eventType;
}

COleDateTime CEvnt::getOleTimeOfDayStart(void)
{
	COleDateTime dt = startRecording;
	COleDateTimeSpan span(0, 0, 0, (int)getFrom());
	dt += span;
	return dt;
}

COleDateTime CEvnt::getOleTimeOfDayStop(void)
{
	COleDateTime dt = startRecording;
	COleDateTimeSpan span(0, 0, 0, (int)getTo());
	dt += span;
	return dt;
}

CString CEvnt::getTimeOfDayStart(void)
{
	COleDateTime dt = startRecording;
	COleDateTimeSpan span(0,0,0,(int) getFrom());
	CString s0 = dt.Format(_T("%H:%M:%S"));
	dt += span;
	CString s = dt.Format(_T("%H:%M:%S"));
	return s;
}

int CEvnt::getEventFlags(void)
{
	return eventFlags;
}

void CEvnt::setEventFlags(int _flags)
{
	eventFlags = _flags;
}

void CEvnt::setHasLevelDetection(bool _on)
{
	hasLevelDetection = _on;
}

bool CEvnt::getHasLevelDetection(void)
{
	return hasLevelDetection;
}

/*
Description: Returns true if this event is valid and should be counted.
_currentHypoDropLimit is used by the arousals as arousal are disabled in relation to the hypopnea criterion 
during check for collision with hypopnea
I.e. if it has not been deleted and not in an excluded time window or in an awake time window
*/
bool CEvnt::getIsActive(const int _currentHypoDropLimit /*= noHypopneaDropLimit*/)
{
	if (eventFlags & EVNT_DEF_MANUAL_DELETE)		return false;
	if (eventFlags & EVNT_DEF_IN_EXCLUDED_WINDOW)	return false;
	if (eventFlags & EVNT_DEF_IN_AWAKE_WINDOW)		return false;

	return true;
}

/*
Description: Returns the manual deleted flag
*/
bool CEvnt::getManualDeleted(void)
{
	if (eventFlags & EVNT_DEF_MANUAL_DELETE) return true;
	else return false;
}

/*
Description: Sets or clears the manual deleted flag
*/
void CEvnt::setManualDeleted(bool _on)
{
	if (_on) eventFlags |= EVNT_DEF_MANUAL_DELETE;
	else {
		int mask = EVNT_DEF_MANUAL_DELETE;
		eventFlags &= ~mask;
	}
}

/*
Description: Returns the true if the event has been removed because of collission with excluded window
*/
bool CEvnt::getInExcludedWindow(void)
{
	if (eventFlags & EVNT_DEF_IN_EXCLUDED_WINDOW) return true;
	else return false;
}

/*
Description: Sets or clears the flag collision with excluded window
*/
void CEvnt::setInExcludedWindow(bool _on)
{
	if (_on) eventFlags |= EVNT_DEF_IN_EXCLUDED_WINDOW;
	else {
		int mask = EVNT_DEF_IN_EXCLUDED_WINDOW;
		eventFlags &= ~mask;
	}
}

/*
Description: Returns the true if the event has been removed because of collission with awake window
*/
bool CEvnt::getInAwakeWindow(void)
{
	if (eventFlags & EVNT_DEF_IN_AWAKE_WINDOW) return true;
	else return false;
}

/*
Description: Sets or clears the flag collision with awake window
*/
void CEvnt::setInAwakeWindow(bool _on)
{
	if (_on) eventFlags |= EVNT_DEF_IN_AWAKE_WINDOW;
	else {
		int mask = EVNT_DEF_IN_AWAKE_WINDOW;
		eventFlags &= ~mask;
	}
}

int CEvnt::getDetectionSourceManOrAuto(void)
{
	return detectionSource;
}

void CEvnt::setDetectionSourceManOrAuto(int _detSource)
{
	detectionSource = _detSource;
}

bool CEvnt::computePoesData(float * _min, float * _max, float * _avg, float * _median)
{
	return computeVectorData(&poesEnv, _min, _max, _avg, _median);
}

bool CEvnt::computePphData(float * _min, float * _max, float * _avg, float * _median)
{
	return computeVectorData(&pphEnv, _min, _max, _avg, _median);
}

void CEvnt::setBodyPos(short _bodyPos)
{
	bodyPos = _bodyPos;
}

void CEvnt::setRecordingStartTimeOfDay(COleDateTime _startTOD)
{
	startRecording = _startTOD;
}

/*
Description: Based on the limits _excludedStart to _excludedStop which are the limits of an excluded window,
decides whether or not the excluded flag should be set. Then sets it.

This is the default implementation: If there is an overlap -> exclude
For snoring and awake, this will be different (therefore the function is virtual)
*/
bool CEvnt::considerIfInExcludeWindow(float _excludedStart, float _excludedStop)
{
	//---Too early
	if ((_excludedStart < from) && (_excludedStop < from)) return false;

	//---Too late
	if ((_excludedStart > to) && (_excludedStop > to)) return false;

	//---There must be overlap. Default is to exclude
	setInExcludedWindow(true);
	return true;
}


CString CEvnt::getTimeOfDayStop(void)
{
	COleDateTime dt = startRecording;
	COleDateTimeSpan span(0,0,0,(int) getTo());
	CString s0 = dt.Format(_T("%H:%M:%S"));
	dt += span;
	CString s = dt.Format(_T("%H:%M:%S"));
	return s;
}

float CEvnt::getFrom(void)
{
	return from;
}

float CEvnt::getCentreTime(void)
{
	return centreTime;
}

float CEvnt::getTo(void)
{
	return to;
}

short CEvnt::getBodyPos(void)
{
	return bodyPos;
}


void CEvnt::Serialize(CArchive& ar, int _majorVersion, int _minorVersion)
{
	if (ar.IsStoring()) {
		ar << bodyPos;
		ar << warning;
		ar << eventType;			// Type of event
		ar << levelType;			// Level type
		ar << detectionSource;		// Detection source - manual or automatic
		ar << from;					// Start of event (in seconds from start)
		ar << to;					// End of event (in seconds from start)
		ar << centreTime;			// Centre of event (in seconds from start)
		ar << length;				// Length in seconds
		ar << startRecording;
		ar << eventFlags;			// Contains flags about how the event was defined (detected)
		ar << hasLevelDetection;
	}
	else {
		ar >> bodyPos;
		ar >> warning;
		ar >> eventType;			// Type of event
		ar >> levelType;			// Level type
		ar >> detectionSource;		// Detection source - manual or automatic
		ar >> from;					// Start of event (in seconds from start)
		ar >> to;					// End of event (in seconds from start)
		ar >> centreTime;			// Centre of event (in seconds from start)
		ar >> length;				// Length in seconds
		ar >> startRecording;
		ar >> eventFlags;			// Contains flags about how the event was defined (detected)
		ar >> hasLevelDetection;
	}
	serializeVectorFloat(ar, pgrad.begin(),pgrad.end(),&pgrad);
	serializeVectorFloat(ar, pgradTime.begin(),pgradTime.end(),&pgradTime);
	serializeVectorFloat(ar, poesEnv.begin(),poesEnv.end(),&poesEnv);
	serializeVectorFloat(ar, poesEnvTime.begin(), poesEnvTime.end(),&poesEnvTime);
	serializeVectorFloat(ar, pphEnv.begin(), pphEnv.end(), &pphEnv);             // New after 4.5.1
	serializeVectorFloat(ar, pphEnvTime.begin(), pphEnvTime.end(), &pphEnvTime); // New after 4.5.1
	serializeVectorFloat(ar, flow.begin(),flow.end(),&flow);
	serializeVectorFloat(ar, flowTime.begin(),flowTime.end(),&flowTime);
	serializeVectorFloat(ar, bpVector.begin(),bpVector.end(),&bpVector);
	serializeVectorFloat(ar, bpTimeVector.begin(), bpTimeVector.end(),&bpTimeVector);
	serializeVectorFloat(ar, contactMicVector.begin(), contactMicVector.end(),&contactMicVector);
	serializeVectorFloat(ar, contactMicTimeVector.begin(), contactMicTimeVector.end(),&contactMicTimeVector);
	serializeVectorFloat(ar, airMic0Vector.begin(), airMic0Vector.end(),&airMic0Vector);
	serializeVectorFloat(ar, airMic1Vector.begin(), airMic1Vector.end(),&airMic1Vector);
	serializeVectorFloat(ar, airMic2Vector.begin(), airMic2Vector.end(),&airMic2Vector);
	serializeVectorFloat(ar, airMic3Vector.begin(), airMic3Vector.end(),&airMic3Vector);
	serializeVectorFloat(ar, airMicSumVector.begin(), airMicSumVector.end(),&airMicSumVector);
	serializeVectorFloat(ar, airMicTimeVector.begin(), airMicTimeVector.end(),&airMicTimeVector);
}

void CEvnt::Serialize20(CArchive & ar, int _majorVersion, int _minorVersion)
{
	if (ar.IsStoring()) {
		ar << bodyPos;
		ar << warning;
		ar << eventType;			// Type of event
		ar << levelType;			// Level type
		ar << detectionSource;		// Detection source - manual or automatic
		ar << from;					// Start of event (in seconds from start)
		ar << to;					// End of event (in seconds from start)
		ar << centreTime;			// Centre of event (in seconds from start)
		ar << length;				// Length in seconds
		ar << startRecording;
		ar << eventFlags;			// Contains flags about how the event was defined (detected)
		ar << hasLevelDetection;
	}
	else {
		ar >> bodyPos;
		ar >> warning;
		ar >> eventType;			// Type of event
		ar >> levelType;			// Level type
		ar >> detectionSource;		// Detection source - manual or automatic
		ar >> from;					// Start of event (in seconds from start)
		ar >> to;					// End of event (in seconds from start)
		ar >> centreTime;			// Centre of event (in seconds from start)
		ar >> length;				// Length in seconds
		ar >> startRecording;
		ar >> eventFlags;			// Contains flags about how the event was defined (detected)
		ar >> hasLevelDetection;
	}
	serializeVectorFloat(ar, pgrad.begin(), pgrad.end(), &pgrad);
	serializeVectorFloat(ar, pgradTime.begin(), pgradTime.end(), &pgradTime);
	serializeVectorFloat(ar, poesEnv.begin(), poesEnv.end(), &poesEnv);
	serializeVectorFloat(ar, poesEnvTime.begin(), poesEnvTime.end(), &poesEnvTime);
	serializeVectorFloat(ar, flow.begin(), flow.end(), &flow);
	serializeVectorFloat(ar, flowTime.begin(), flowTime.end(), &flowTime);
	serializeVectorFloat(ar, bpVector.begin(), bpVector.end(), &bpVector);
	serializeVectorFloat(ar, bpTimeVector.begin(), bpTimeVector.end(), &bpTimeVector);
	serializeVectorFloat(ar, contactMicVector.begin(), contactMicVector.end(), &contactMicVector);
	serializeVectorFloat(ar, contactMicTimeVector.begin(), contactMicTimeVector.end(), &contactMicTimeVector);
	serializeVectorFloat(ar, airMic0Vector.begin(), airMic0Vector.end(), &airMic0Vector);
	serializeVectorFloat(ar, airMic1Vector.begin(), airMic1Vector.end(), &airMic1Vector);
	serializeVectorFloat(ar, airMic2Vector.begin(), airMic2Vector.end(), &airMic2Vector);
	serializeVectorFloat(ar, airMic3Vector.begin(), airMic3Vector.end(), &airMic3Vector);
	serializeVectorFloat(ar, airMicSumVector.begin(), airMicSumVector.end(), &airMicSumVector);
	serializeVectorFloat(ar, airMicTimeVector.begin(), airMicTimeVector.end(), &airMicTimeVector);
}

void CEvnt::copyTo(CEvnt *_dest)
{
	//---Copy parameters
	_dest->bodyPos				= bodyPos;
	_dest->warning				= warning;
	_dest->eventType			= eventType;
	_dest->levelType			= levelType;
	_dest->detectionSource		= detectionSource;
	_dest->from					= from;
	_dest->to					= to;
	_dest->centreTime			= centreTime;
	_dest->length				= length;
	_dest->startRecording		= startRecording;
	_dest->eventFlags			= eventFlags;
	_dest->hasLevelDetection	= hasLevelDetection;
	
	//---Clear the vectors
	_dest->pgrad.clear();
	_dest->pgradTime.clear();	
	_dest->poesEnv.clear();
	_dest->poesEnvTime.clear();

	_dest->pphEnv.clear();
	_dest->pphEnvTime.clear();

	_dest->flow.clear();
	_dest->flowTime.clear();
	_dest->bpVector.clear();
	_dest->bpTimeVector.clear();
	_dest->contactMicVector.clear();
	_dest->contactMicTimeVector.clear();
	_dest->airMic0Vector.clear();
	_dest->airMic1Vector.clear();
	_dest->airMic2Vector.clear();
	_dest->airMic3Vector.clear();
	_dest->airMicSumVector.clear();
	_dest->airMicTimeVector.clear();
	
	//---Resize the vectors
	_dest->pgrad.resize(pgrad.size(),.0f);
	_dest->pgradTime.resize(pgradTime.size(),.0f);
	_dest->poesEnv.resize(poesEnv.size(),.0f);
	_dest->poesEnvTime.resize(poesEnvTime.size(),.0f);

	_dest->pphEnv.resize(pphEnv.size(), .0f);
	_dest->pphEnvTime.resize(pphEnvTime.size(), .0f);

	_dest->flow.resize(flow.size(),.0f);
	_dest->flowTime.resize(flowTime.size(),.0f);
	_dest->bpVector.resize(bpVector.size(),.0f);
	_dest->bpTimeVector.resize(bpTimeVector.size(),.0f);
	_dest->contactMicVector.resize(contactMicVector.size(),.0f);
	_dest->contactMicTimeVector.resize(contactMicTimeVector.size(),.0f);
	_dest->airMic0Vector.resize(airMic0Vector.size(),.0f);
	_dest->airMic1Vector.resize(airMic1Vector.size(),.0f);
	_dest->airMic2Vector.resize(airMic2Vector.size(),.0f);
	_dest->airMic3Vector.resize(airMic3Vector.size(),.0f);
	_dest->airMicSumVector.resize(airMicSumVector.size(),.0f);
	_dest->airMicTimeVector.resize(airMicTimeVector.size(),.0f);
	
	//---Copy the vectors
	copy(pgrad.begin(),pgrad.end(),_dest->pgrad.begin());
	copy(pgradTime.begin(),pgradTime.end(),_dest->pgradTime.begin());
	copy(poesEnv.begin(),poesEnv.end(),_dest->poesEnv.begin());
	copy(poesEnvTime.begin(),poesEnvTime.end(),_dest->poesEnvTime.begin());

	copy(pphEnv.begin(), pphEnv.end(), _dest->pphEnv.begin());
	copy(pphEnvTime.begin(), pphEnvTime.end(), _dest->pphEnvTime.begin());

	copy(flow.begin(),flow.end(),_dest->flow.begin());
	copy(flowTime.begin(),flowTime.end(),_dest->flowTime.begin());
	copy(bpVector.begin(),bpVector.end(),_dest->bpVector.begin());
	copy(bpTimeVector.begin(),bpTimeVector.end(),_dest->bpTimeVector.begin());
	copy(contactMicVector.begin(),contactMicVector.end(),_dest->contactMicVector.begin());
	copy(contactMicTimeVector.begin(),contactMicTimeVector.end(),_dest->contactMicTimeVector.begin());
	copy(airMic0Vector.begin(),airMic0Vector.end(),_dest->airMic0Vector.begin());
	copy(airMic1Vector.begin(),airMic1Vector.end(),_dest->airMic1Vector.begin());
	copy(airMic2Vector.begin(),airMic2Vector.end(),_dest->airMic2Vector.begin());
	copy(airMic3Vector.begin(),airMic3Vector.end(),_dest->airMic3Vector.begin());
	copy(airMicSumVector.begin(),airMicSumVector.end(),_dest->airMicSumVector.begin());
	copy(airMicTimeVector.begin(),airMicTimeVector.end(),_dest->airMicTimeVector.begin());
}

void CEvnt::copyFrom(CEvnt *_source)
{
	//---Copy parameters
	bodyPos				= _source->bodyPos;
	warning				= _source->warning;
	eventType			= _source->eventType;
	levelType			= _source->levelType;
	detectionSource		= _source->detectionSource;
	from				= _source->from;
	to					= _source->to;
	centreTime			= _source->centreTime;
	length				= _source->length;
	startRecording		= _source->startRecording;
	eventFlags			= _source->eventFlags;
	hasLevelDetection	= _source->hasLevelDetection;
	
	//---Clear the vectors
	pgrad.clear();
	pgradTime.clear();	
	poesEnv.clear();
	poesEnvTime.clear();

	pphEnv.clear();
	pphEnvTime.clear();

	flow.clear();
	flowTime.clear();
	bpVector.clear();
	bpTimeVector.clear();
	contactMicVector.clear();
	contactMicTimeVector.clear();
	airMic0Vector.clear();
	airMic1Vector.clear();
	airMic2Vector.clear();
	airMic3Vector.clear();
	airMicSumVector.clear();
	airMicTimeVector.clear();
	
	//---Resize the vectors
	pgrad.resize(_source->pgrad.size(),.0f);
	pgradTime.resize(_source->pgradTime.size(),.0f);
	poesEnv.resize(_source->poesEnv.size(),.0f);
	poesEnvTime.resize(_source->poesEnvTime.size(),.0f);

	pphEnv.resize(_source->pphEnv.size(), .0f);
	pphEnvTime.resize(_source->pphEnvTime.size(), .0f);

	flow.resize(_source->flow.size(),.0f);
	flowTime.resize(_source->flowTime.size(),.0f);
	bpVector.resize(_source->bpVector.size(),.0f);
	bpTimeVector.resize(_source->bpTimeVector.size(),.0f);
	contactMicVector.resize(_source->contactMicVector.size(),.0f);
	contactMicTimeVector.resize(_source->contactMicTimeVector.size(),.0f);
	airMic0Vector.resize(_source->airMic0Vector.size(),.0f);
	airMic1Vector.resize(_source->airMic1Vector.size(),.0f);
	airMic2Vector.resize(_source->airMic2Vector.size(),.0f);
	airMic3Vector.resize(_source->airMic3Vector.size(),.0f);
	airMicSumVector.resize(_source->airMicSumVector.size(),.0f);
	airMicTimeVector.resize(_source->airMicTimeVector.size(),.0f);
	
	//---Copy the vectors
	copy(_source->pgrad.begin(),				_source->pgrad.end(),				pgrad.begin());
	copy(_source->pgradTime.begin(),			_source->pgradTime.end(),			pgradTime.begin());
	copy(_source->poesEnv.begin(),				_source->poesEnv.end(),				poesEnv.begin());
	copy(_source->poesEnvTime.begin(),			_source->poesEnvTime.end(),			poesEnvTime.begin());

	copy(_source->pphEnv.begin(),				_source->pphEnv.end(),				pphEnv.begin());
	copy(_source->pphEnvTime.begin(),			_source->pphEnvTime.end(),			pphEnvTime.begin());

	copy(_source->flow.begin(),					_source->flow.end(),				flow.begin());
	copy(_source->flowTime.begin(),				_source->flowTime.end(),			flowTime.begin());
	copy(_source->bpVector.begin(),				_source->bpVector.end(),			bpVector.begin());
	copy(_source->bpTimeVector.begin(),			_source->bpTimeVector.end(),		bpTimeVector.begin());
	copy(_source->contactMicVector.begin(),		_source->contactMicVector.end(),	contactMicVector.begin());
	copy(_source->contactMicTimeVector.begin(),	_source->contactMicTimeVector.end(),contactMicTimeVector.begin());
	copy(_source->airMic0Vector.begin(),		_source->airMic0Vector.end(),		airMic0Vector.begin());
	copy(_source->airMic1Vector.begin(),		_source->airMic1Vector.end(),		airMic1Vector.begin());
	copy(_source->airMic2Vector.begin(),		_source->airMic2Vector.end(),		airMic2Vector.begin());
	copy(_source->airMic3Vector.begin(),		_source->airMic3Vector.end(),		airMic3Vector.begin());
	copy(_source->airMicSumVector.begin(),		_source->airMicSumVector.end(),		airMicSumVector.begin());
	copy(_source->airMicTimeVector.begin(),		_source->airMicTimeVector.end(),	airMicTimeVector.begin());
}
//////////////////////////////////////////////////

CExcludedEvnt::CExcludedEvnt(FLOAT _from,FLOAT _to,COleDateTime _startRecording,
					   short _bodyPos,int _source /* = autoDetection */) :
	CEvnt(_from,_to,_startRecording,_bodyPos,_source) 
{
	eventType = evntTypeExcluded;
	thisIsTheLast = false;
	thisIsTheFirst = false;
}
	
CExcludedEvnt::CExcludedEvnt() :
	CEvnt(.0f,.0f,defDateTime,posUndefined,autoDetection) 
{
	eventType = evntTypeExcluded;
	thisIsTheLast = false;
	thisIsTheFirst = false;
}

CExcludedEvnt::~CExcludedEvnt()
{
}

void CExcludedEvnt::setAsFirst(bool _on)
{
	thisIsTheFirst = true;
}

void CExcludedEvnt::setAsLast(bool _on)
{
	thisIsTheLast = true;
}

bool CExcludedEvnt::getIsLast(void)
{
	return thisIsTheLast;
}

bool CExcludedEvnt::getIsFirst(void)
{
	return thisIsTheFirst;
}

void CExcludedEvnt::Serialize(CArchive& ar, int _majorVersion, int _minorVersion)
{
	CEvnt::Serialize(ar, _majorVersion, _minorVersion);

	if (ar.IsStoring()) {
		ar << thisIsTheFirst;
		ar << thisIsTheLast;
	}
	else {
		ar >> thisIsTheFirst;
		ar >> thisIsTheLast;
	}
}

void CExcludedEvnt::Serialize20(CArchive & ar, int _majorVersion, int _minorVersion)
{
	CEvnt::Serialize20(ar, _majorVersion, _minorVersion);

	if (ar.IsStoring()) {
		ar << thisIsTheFirst;
		ar << thisIsTheLast;
	}
	else {
		ar >> thisIsTheFirst;
		ar >> thisIsTheLast;
	}
}

void CExcludedEvnt::copyTo(CExcludedEvnt *_dest)
{
	CEvnt::copyTo((CEvnt *) _dest);
}

void CExcludedEvnt::copyFrom(CExcludedEvnt *_source)
{
	CEvnt::copyFrom((CEvnt *) _source);
}



//////////////////////////////////////////////////

CMixedEvnt::CMixedEvnt(FLOAT _from,FLOAT _to,COleDateTime _startRecording,
					   short _bodyPos,int _source /* = autoDetection */) :
	CEvnt(_from,_to,_startRecording,_bodyPos,_source) 
{
	eventType = evntTypeMixed;
	hasLevelDetection = true;
}
	
CMixedEvnt::CMixedEvnt() :
	CEvnt(.0f,.0f,defDateTime,posUndefined,autoDetection) 
{
	eventType = evntTypeMixed;
}

CMixedEvnt::~CMixedEvnt()
{
}

void CMixedEvnt::Serialize(CArchive& ar, int _majorVersion, int _minorVersion)
{
	CEvnt::Serialize(ar, _majorVersion, _minorVersion);
}

void CMixedEvnt::Serialize20(CArchive & ar, int _majorVersion, int _minorVersion)
{
	CEvnt::Serialize20(ar, _majorVersion, _minorVersion);
}


void CMixedEvnt::copyTo(CMixedEvnt *_dest)
{
	CEvnt::copyTo((CEvnt *) _dest);
}

void CMixedEvnt::copyFrom(CMixedEvnt *_source)
{
	CEvnt::copyFrom((CEvnt *) _source);
}


//////////////////////////////////////////////////

CObstrEvnt::CObstrEvnt(FLOAT _from,FLOAT _to,COleDateTime _startRecording,
					   short _bodyPos,int _source /* = autoDetection */) :
	CEvnt(_from,_to,_startRecording,_bodyPos,_source) 
{
	eventType = evntTypeObstr;
	hasLevelDetection = true;
}
	
CObstrEvnt::CObstrEvnt() :
	CEvnt(.0f,.0f,defDateTime,posUndefined,autoDetection) 
{
	eventType = evntTypeObstr;
}

CObstrEvnt::~CObstrEvnt()
{
}

void CObstrEvnt::Serialize(CArchive& ar, int _majorVersion, int _minorVersion)
{
	CEvnt::Serialize(ar, _majorVersion, _minorVersion);
}

void CObstrEvnt::Serialize20(CArchive & ar, int _majorVersion, int _minorVersion)
{
	CEvnt::Serialize20(ar, _majorVersion, _minorVersion);
}


void CObstrEvnt::copyTo(CObstrEvnt *_dest)
{
	CEvnt::copyTo((CEvnt *) _dest);
}

void CObstrEvnt::copyFrom(CObstrEvnt *_source)
{
	CEvnt::copyFrom((CEvnt *) _source);
}


//////////////////////////////////////////////////

CHypoEvnt::CHypoEvnt(FLOAT _from,FLOAT _to,COleDateTime _startRecording,
					 short _bodyPos,int _dropLimit,int _source /* = autoDetection */) :
	CEvnt(_from,_to,_startRecording,_bodyPos,_source) 
{
	hypoDropLimit = _dropLimit;
	eventType = evntTypeHypoObstr;
	hypoType = hypoTypeObstr;
	hasLevelDetection = true;
	
	marriedToSpO2Drop = false;
	partnerSpO2DropCentreTime = .0f;
}

CHypoEvnt::~CHypoEvnt()
{
}

CHypoEvnt::CHypoEvnt() :
	CEvnt(.0f,.0f,defDateTime,posUndefined,autoDetection)
{
	hypoDropLimit = hypopneaDropLimit50;
	eventType = evntTypeHypoObstr;
	hypoType = hypoTypeObstr;

	marriedToSpO2Drop = false;
	partnerSpO2DropCentreTime = .0f;
}

void CHypoEvnt::copyTo(CHypoEvnt *_dest)
{
	CEvnt::copyTo((CEvnt *) _dest);
	_dest->hypoType = hypoType;
	
	_dest->marriedToSpO2Drop			= marriedToSpO2Drop			;
	_dest->partnerSpO2DropCentreTime	= partnerSpO2DropCentreTime	;
}

void CHypoEvnt::copyFrom(CHypoEvnt *_source)
{
	CEvnt::copyFrom((CEvnt *) _source);
	hypoType = _source->hypoType;
	
	marriedToSpO2Drop				= _source->marriedToSpO2Drop			;
	partnerSpO2DropCentreTime		= _source->partnerSpO2DropCentreTime	;
}


void CHypoEvnt::setHypoType(int _hypoType)
{
	hypoType = _hypoType; 

	eventType = hypoType == hypoTypeObstr ? evntTypeHypoObstr : evntTypeHypoCentral;
	hasLevelDetection = hypoType == hypoTypeObstr ? true : false;
}

int CHypoEvnt::getHypoType(void)
{
	return hypoType;
}

void CHypoEvnt::doLevelAnalysis(void)
{
	if (hypoTypeObstr != hypoType) {
		levelType = levelTypeUndef;
		hasLevelDetection = false;
		return;
	}
	
	hasLevelDetection = true;

	CEvnt::doLevelAnalysis();
}


//void CHypoEvnt::setArousalPartner(float _time,bool _on)
//{
//	marriedToArousal = _on;
//	partnerArousalCentreTime = _time;
//}

//bool CHypoEvnt::getArousalPartner(float *_time)
//{
//	*_time = partnerArousalCentreTime;
//	return marriedToArousal;
//}

bool CHypoEvnt::getSpO2DropPartner(float *_time)
{
	*_time = partnerSpO2DropCentreTime;
	return marriedToSpO2Drop;
}

bool CHypoEvnt::pickFromDropLevel(int _hypoDrop)
{
	bool ret = false;
	switch (hypoDropLimit) {  // My drop limit
	case hypopneaDropLimit30 :
		if (hypopneaDropLimit30 == _hypoDrop) ret = true;
		break;
	case hypopneaDropLimit40 :
		if (hypopneaDropLimit30 == _hypoDrop) ret = true;
		else if (hypopneaDropLimit40 == _hypoDrop) ret = true;
		break;
	case hypopneaDropLimit50:
		if (hypopneaDropLimit30 == _hypoDrop) ret = true;
		else if (hypopneaDropLimit40 == _hypoDrop) ret = true;
		else if (hypopneaDropLimit50 == _hypoDrop) ret = true;
		break;
	case noHypopneaDropLimit:
		break;
	}
	return ret;
}

int CHypoEvnt::getHypoDropLimit(void)
{
	return hypoDropLimit;
}

void CHypoEvnt::setHypoDropLimit(int _dropLimit)
{
	hypoDropLimit = _dropLimit;
}

void CHypoEvnt::setSpO2DropPartner(float _time,bool _on)
{
	marriedToSpO2Drop = _on;
	partnerSpO2DropCentreTime = _time;
}

int CHypoEvnt::findHypoType(void)
{
	if (0 == poesEnv.size()) {
		return getHypoType();
	}
	ASSERT(poesEnvTime.size() == poesEnv.size());

	float a,b,rSquared;
	int studyLength = poesEnv.size() / 2;
	CDataSet::linReg(	poesEnvTime.begin(),poesEnvTime.begin() + studyLength,
						poesEnv.begin(),poesEnv.begin() + studyLength,
						&a,&b,&rSquared);

	hypoType = hypoTypeObstr; 
	if (b < ANGLE_COEFF_OBSTR_VS_CENTRAL_HYPO) {
		hypoType	= hypoTypeCentral;
		eventType	= evntTypeHypoCentral;
	}
	else {
		hypoType = hypoTypeObstr;
		eventType = evntTypeHypoObstr;
	}
	return getHypoType();
}

void CHypoEvnt::Serialize(CArchive& ar, int _majorVersion, int _minorVersion)
{
	CEvnt::Serialize(ar, _majorVersion, _minorVersion);

	if (ar.IsStoring()) {
		ar << hypoType;
		ar << hypoDropLimit;
		ar << marriedToSpO2Drop;
		ar << partnerSpO2DropCentreTime;
	}
	else {
		ar >> hypoType;
		ar >> hypoDropLimit;
		ar >> marriedToSpO2Drop;
		ar >> partnerSpO2DropCentreTime;
	}	
}

void CHypoEvnt::Serialize20(CArchive & ar, int _majorVersion, int _minorVersion)
{
	CEvnt::Serialize20(ar, _majorVersion, _minorVersion);

	if (ar.IsStoring()) {
		ar << hypoType;
		ar << hypoDropLimit;
		ar << marriedToSpO2Drop;
		ar << partnerSpO2DropCentreTime;
	}
	else {
		ar >> hypoType;
		ar >> hypoDropLimit;
		ar >> marriedToSpO2Drop;
		ar >> partnerSpO2DropCentreTime;
	}
}


//////////////////////////////////////////////////

CRERAEvnt::CRERAEvnt(FLOAT _from,FLOAT _to,COleDateTime _startRecording,
					 short _bodyPos,int _source /* = autoDetection */) :
	CEvnt(_from,_to,_startRecording,_bodyPos,_source) 
{
	eventType = evntTypeRERA;
	hasLevelDetection = true;
	hypoLimitWhenDisabledDueToCollWithHypo = noHypopneaDropLimit;
}
	
CRERAEvnt::CRERAEvnt() :
	CEvnt(.0f,.0f,defDateTime,posUndefined,autoDetection) 
{
	eventType = evntTypeRERA;
}

CRERAEvnt::~CRERAEvnt()
{
}

void CRERAEvnt::Serialize(CArchive& ar, int _majorVersion, int _minorVersion)
{
	CEvnt::Serialize(ar, _majorVersion, _minorVersion);

	CEvnt::serializeVectorFloat(ar, adm.begin(),adm.end(),&adm);
	CEvnt::serializeVectorFloat(ar, admTime.begin(),admTime.end(),&admTime);
	CEvnt::serializeVectorFloat(ar, pr.begin(),pr.end(),&pr);
	CEvnt::serializeVectorFloat(ar, prTime.begin(),prTime.end(),&prTime);
	CEvnt::serializeVectorFloat(ar, respFrq.begin(),respFrq.end(),&respFrq);
	CEvnt::serializeVectorFloat(ar, respFrqTime.begin(), respFrqTime.end(),&respFrqTime);
	CEvnt::serializeVectorFloat(ar, act.begin(),act.end(),&act);
	CEvnt::serializeVectorFloat(ar, actTime.begin(),actTime.end(),&actTime);
}

void CRERAEvnt::Serialize20(CArchive & ar, int _majorVersion, int _minorVersion)
{
	CEvnt::Serialize20(ar, _majorVersion, _minorVersion);

	CEvnt::serializeVectorFloat(ar, adm.begin(), adm.end(), &adm);
	CEvnt::serializeVectorFloat(ar, admTime.begin(), admTime.end(), &admTime);
	CEvnt::serializeVectorFloat(ar, pr.begin(), pr.end(), &pr);
	CEvnt::serializeVectorFloat(ar, prTime.begin(), prTime.end(), &prTime);
	CEvnt::serializeVectorFloat(ar, respFrq.begin(), respFrq.end(), &respFrq);
	CEvnt::serializeVectorFloat(ar, respFrqTime.begin(), respFrqTime.end(), &respFrqTime);
	CEvnt::serializeVectorFloat(ar, act.begin(), act.end(), &act);
	CEvnt::serializeVectorFloat(ar, actTime.begin(), actTime.end(), &actTime);
}


void CRERAEvnt::copyTo(CRERAEvnt *_dest)
{
	CEvnt::copyTo((CEvnt *) _dest);

	_dest->adm.clear();
	_dest->admTime.clear();
	_dest->pr.clear();
	_dest->prTime.clear();
	_dest->respFrq.clear();
	_dest->respFrqTime.clear();
	_dest->act.clear();
	_dest->actTime.clear();
		
	_dest->adm.resize(adm.size(),.0f);
	_dest->admTime.resize(admTime.size(),.0f);
	_dest->pr.resize(pr.size(),.0f);
	_dest->prTime.resize(prTime.size(),.0f);
	_dest->respFrq.resize(respFrq.size(),.0f);
	_dest->respFrqTime.resize(respFrqTime.size(),.0f);
	_dest->act.resize(act.size(),.0f);
	_dest->actTime.resize(actTime.size(),.0f);
	
	copy(adm.begin(),		adm.end(),				_dest->adm.begin());
	copy(admTime.begin(),	admTime.end(),			_dest->admTime.begin());
	copy(pr.begin(),		pr.end(),				_dest->pr.begin());
	copy(prTime.begin(),	prTime.end(),			_dest->prTime.begin());
	copy(respFrq.begin(),	respFrq.end(),			_dest->respFrq.begin());
	copy(respFrqTime.begin(),	respFrqTime.end(),  _dest->respFrqTime.begin());
	copy(act.begin(),		act.end(),				_dest->act.begin());
	copy(actTime.begin(),	actTime.end(),			_dest->actTime.begin());
}

void CRERAEvnt::copyFrom(CRERAEvnt *_source)
{
	CEvnt::copyFrom((CEvnt *) _source);

	adm.clear();
	admTime.clear();
	pr.clear();
	prTime.clear();
	respFrq.clear();
	respFrqTime.clear();
	act.clear();
	actTime.clear();
		
	adm.resize(_source->adm.size(),.0f);
	admTime.resize(_source->admTime.size(),.0f);
	pr.resize(_source->pr.size(),.0f);
	prTime.resize(_source->prTime.size(),.0f);
	respFrq.resize(_source->respFrq.size(),.0f);
	respFrqTime.resize(_source->respFrqTime.size(),.0f);
	act.resize(_source->act.size(),.0f);
	actTime.resize(_source->actTime.size(),.0f);
	
	copy(_source->adm.begin(),			_source->adm.end(),				adm.begin());
	copy(_source->admTime.begin(),		_source->admTime.end(),			admTime.begin());
	copy(_source->pr.begin(),			_source->pr.end(),				pr.begin());
	copy(_source->prTime.begin(),		_source->prTime.end(),			prTime.begin());
	copy(_source->respFrq.begin(),		_source->respFrq.end(),			respFrq.begin());
	copy(_source->respFrqTime.begin(),	_source->respFrqTime.end(),		respFrqTime.begin());
	copy(_source->act.begin(),			_source->act.end(),				act.begin());
	copy(_source->actTime.begin(),		_source->actTime.end(),			actTime.begin());
}


void CRERAEvnt::setAdmittance(vector <FLOAT>::iterator _from,
							  vector <FLOAT>::iterator _to,
							  vector <FLOAT>::iterator _fromTime,
							  vector <FLOAT>::iterator _toTime)
{
	adm.clear();
	admTime.clear();
	vector <FLOAT>::iterator it;
	for (it = _from ; it < _to ; it++) adm.push_back(*it);
	for (it = _fromTime ; it < _toTime ; it++) admTime.push_back(*it);
}

void CRERAEvnt::setPulseRate(vector <FLOAT>::iterator _from,
							 vector <FLOAT>::iterator _to,
							 vector <FLOAT>::iterator _fromTime,
							 vector <FLOAT>::iterator _toTime)
{
	pr.clear();
	prTime.clear();
	vector <FLOAT>::iterator it;
	for (it = _from ; it < _to ; it++) pr.push_back(*it);
	for (it = _fromTime ; it < _toTime ; it++) prTime.push_back(*it);
}

void CRERAEvnt::setRespFrq(vector <FLOAT>::iterator _from,
						   vector <FLOAT>::iterator _to,
						   vector <FLOAT>::iterator _fromTime,
						   vector <FLOAT>::iterator _toTime)
{
	respFrq.clear();
	respFrqTime.clear();
	vector <FLOAT>::iterator it;
	for (it = _from ; it < _to ; it++) respFrq.push_back(*it);
	for (it = _fromTime ; it < _toTime ; it++) respFrqTime.push_back(*it);
}

///*
//Description: Returns true if this event is valid and should be counted.
//I.e. if it has not been deleted and not in an excluded time window or in an awake time window
//The RERA event has one additional flag :EVNT_DEF_IN_CONFLICT_WITH_HYPO
//This is because deactivating an arousal because is a conflict with a hypopnea may change 
//if the hypopneacriterion changes
//*/
//bool CRERAEvnt::getIsActive(void)
//{
//	if (eventFlags & EVNT_DEF_MANUAL_DELETE)				return false;
//	if (eventFlags & EVNT_DEF_IN_EXCLUDED_WINDOW)			return false;
//	if (eventFlags & EVNT_DEF_IN_AWAKE_WINDOW)				return false;
//	if (eventFlags & EVNT_DEF_IN_CONFLICT_WITH_HYPO)		return false;
//
//	return true;
//}

//void CRERAEvnt::setInConflictWithHypo(bool _on)
//{
//	if (_on) eventFlags |= EVNT_DEF_IN_CONFLICT_WITH_HYPO;
//	else {
//		int mask = EVNT_DEF_IN_CONFLICT_WITH_HYPO;
//		eventFlags &= ~mask;
//	}
//
//}
//
//bool CRERAEvnt::getInConflictWithHypo(void)
//{
//	if (eventFlags & EVNT_DEF_IN_CONFLICT_WITH_HYPO) return true;
//	else return false;
//}

void CRERAEvnt::setAct(vector <FLOAT>::iterator _from,
					   vector <FLOAT>::iterator _to,
					   vector <FLOAT>::iterator _fromTime,
					   vector <FLOAT>::iterator _toTime)
{
	act.clear();
	actTime.clear();
	vector <FLOAT>::iterator it;
	for (it = _from ; it < _to ; it++) act.push_back(*it);
	for (it = _fromTime ; it < _toTime ; it++) actTime.push_back(*it);
}

void CRERAEvnt::disableDueToCollWithHypo(int _hypoDropLimit)
{
	hypoLimitWhenDisabledDueToCollWithHypo = _hypoDropLimit;
}

bool CRERAEvnt::getIsActive(const int _currentHypoDropLimit)
{
	ASSERT((hypopneaDropLimit50 == _currentHypoDropLimit) ||
		(hypopneaDropLimit40 == _currentHypoDropLimit) ||
		(hypopneaDropLimit30 == _currentHypoDropLimit));

	if (!CEvnt::getIsActive()) return false;  // In Awake or excluded

	switch (hypoLimitWhenDisabledDueToCollWithHypo) {
	case noHypopneaDropLimit:
		return true;
		break;
	case hypopneaDropLimit50:
		if (_currentHypoDropLimit == hypopneaDropLimit50) return false;
		else return true;
		break;
	case hypopneaDropLimit40:
		if (_currentHypoDropLimit == hypopneaDropLimit40) return false;
		else return true;
		break;
	case hypopneaDropLimit30:
		if (_currentHypoDropLimit == hypopneaDropLimit30) return false;
		else return true;
		break;
	}
	return false;  // Will never reach this point
}

//////////////////////////////////////////////////

CArousalEvnt::CArousalEvnt(FLOAT _from,FLOAT _to,COleDateTime _startRecording,
						   short _bodyPos,int _source /* = autoDetection */) :
	CEvnt(_from,_to,_startRecording,_source) 
{
	reraEvnt = NULL;
	eventType = evntTypeArousal;
	hypoLimitWhenDisabledDueToCollWithHypo = noHypopneaDropLimit;
}
	
CArousalEvnt::CArousalEvnt() :
	CEvnt(.0f,.0f,defDateTime,posUndefined,autoDetection) 
{
	reraEvnt = NULL;
	eventType = evntTypeArousal;
	hypoLimitWhenDisabledDueToCollWithHypo = noHypopneaDropLimit;
}

CArousalEvnt::~CArousalEvnt()
{
	if (reraEvnt) delete(reraEvnt);
	reraEvnt = NULL;
}

CRERAEvnt * CArousalEvnt::getRERAEvnt(void)
{
	return reraEvnt;
}

void CArousalEvnt::removeRERAEvnt(void)
{
	if (reraEvnt) delete(reraEvnt);
	reraEvnt = NULL;
}

void CArousalEvnt::setRERAEvnt(CRERAEvnt *_reraEvnt)
{
	reraEvnt = _reraEvnt;
}

bool CArousalEvnt::getIsActive(const int _currentHypoDropLimit /* = noHypopneaDropLimit*/)
{
	ASSERT((hypopneaDropLimit50 == _currentHypoDropLimit) ||
		   (hypopneaDropLimit40 == _currentHypoDropLimit) || 
		   (hypopneaDropLimit30 == _currentHypoDropLimit));

	if (!CEvnt::getIsActive()) return false;  // In Awake or excluded

	switch (hypoLimitWhenDisabledDueToCollWithHypo) {
	case noHypopneaDropLimit:
		return true;
		break;
	case hypopneaDropLimit50:
		if (_currentHypoDropLimit == hypopneaDropLimit50) return false;
		else return true;
		break;
	case hypopneaDropLimit40:
		if (_currentHypoDropLimit == hypopneaDropLimit40) return false;
		else return true;
		break;
	case hypopneaDropLimit30:
		if (_currentHypoDropLimit == hypopneaDropLimit30) return false;
		else return true;
		break;
	}
	return false;  // Will never reach this point
}

void CArousalEvnt::disableDueToCollWithHypo(int _hypoDropLimit)
{
	hypoLimitWhenDisabledDueToCollWithHypo = _hypoDropLimit;
}

int CArousalEvnt::getHypoLimitWhenDisabled(void)
{
	return hypoLimitWhenDisabledDueToCollWithHypo;
}

bool CArousalEvnt::getWasRemovedDueToCollWithHypo(const int _hypoDetCrit)
{
	return (_hypoDetCrit == hypoLimitWhenDisabledDueToCollWithHypo);
}

void CArousalEvnt::Serialize(CArchive& ar, int _majorVersion, int _minorVersion)
{
	CEvnt::Serialize(ar, _majorVersion, _minorVersion);

	if (ar.IsStoring()) {
		ar << hypoLimitWhenDisabledDueToCollWithHypo;
		bool hasRera = reraEvnt ? true : false;
		ar << hasRera;
		if (hasRera) reraEvnt->Serialize(ar, _majorVersion, _minorVersion);
	}
	else {
		ar >> hypoLimitWhenDisabledDueToCollWithHypo;
		bool hasRera = false;
		ar >> hasRera;
		if (hasRera) {
			reraEvnt = new CRERAEvnt();
			reraEvnt->Serialize(ar, _majorVersion, _minorVersion);
		}
		else reraEvnt = NULL;
	}

	CEvnt::serializeVectorFloat(ar, adm.begin(),adm.end(),&adm);
	CEvnt::serializeVectorFloat(ar, admTime.begin(),admTime.end(),&admTime);
	CEvnt::serializeVectorFloat(ar, pr.begin(),pr.end(),&pr);
	CEvnt::serializeVectorFloat(ar, prTime.begin(),prTime.end(),&prTime);
	CEvnt::serializeVectorFloat(ar, respFrq.begin(),respFrq.end(),&respFrq);
	CEvnt::serializeVectorFloat(ar, respFrqTime.begin(), respFrqTime.end(),&respFrqTime);
	CEvnt::serializeVectorFloat(ar, act.begin(),act.end(),&act);
	CEvnt::serializeVectorFloat(ar, actTime.begin(),actTime.end(),&actTime);
}

void CArousalEvnt::Serialize20(CArchive & ar, int _majorVersion, int _minorVersion)
{
	CEvnt::Serialize20(ar, _majorVersion, _minorVersion);

	if (ar.IsStoring()) {
		ar << hypoLimitWhenDisabledDueToCollWithHypo;
		bool hasRera = reraEvnt ? true : false;
		ar << hasRera;
		if (hasRera) reraEvnt->Serialize(ar, _majorVersion, _minorVersion);
	}
	else {
		ar >> hypoLimitWhenDisabledDueToCollWithHypo;
		bool hasRera = false;
		ar >> hasRera;
		if (hasRera) {
			reraEvnt = new CRERAEvnt();
			reraEvnt->Serialize20(ar, _majorVersion, _minorVersion);
		}
		else reraEvnt = NULL;
	}

	CEvnt::serializeVectorFloat(ar, adm.begin(), adm.end(), &adm);
	CEvnt::serializeVectorFloat(ar, admTime.begin(), admTime.end(), &admTime);
	CEvnt::serializeVectorFloat(ar, pr.begin(), pr.end(), &pr);
	CEvnt::serializeVectorFloat(ar, prTime.begin(), prTime.end(), &prTime);
	CEvnt::serializeVectorFloat(ar, respFrq.begin(), respFrq.end(), &respFrq);
	CEvnt::serializeVectorFloat(ar, respFrqTime.begin(), respFrqTime.end(), &respFrqTime);
	CEvnt::serializeVectorFloat(ar, act.begin(), act.end(), &act);
	CEvnt::serializeVectorFloat(ar, actTime.begin(), actTime.end(), &actTime);
}


void CArousalEvnt::copyTo(CArousalEvnt *_dest)
{
	CEvnt::copyTo((CEvnt *) _dest);

	//---Delete current reraEvnt
	_dest->removeRERAEvnt();

	if (reraEvnt) {
		CRERAEvnt *rP = new CRERAEvnt();
		reraEvnt->copyTo(rP);
		_dest->setRERAEvnt(rP);
	}

	_dest->adm.clear();
	_dest->admTime.clear();
	_dest->pr.clear();
	_dest->prTime.clear();
	_dest->respFrq.clear();
	_dest->respFrqTime.clear();
	_dest->act.clear();
	_dest->actTime.clear();
		
	_dest->adm.resize(adm.size(),.0f);
	_dest->admTime.resize(admTime.size(),.0f);
	_dest->pr.resize(pr.size(),.0f);
	_dest->prTime.resize(prTime.size(),.0f);
	_dest->respFrq.resize(respFrq.size(),.0f);
	_dest->respFrqTime.resize(respFrqTime.size(),.0f);
	_dest->act.resize(act.size(),.0f);
	_dest->actTime.resize(actTime.size(),.0f);
	
	copy(adm.begin(),		adm.end(),				_dest->adm.begin());
	copy(admTime.begin(),	admTime.end(),			_dest->admTime.begin());
	copy(pr.begin(),		pr.end(),				_dest->pr.begin());
	copy(prTime.begin(),	prTime.end(),			_dest->prTime.begin());
	copy(respFrq.begin(),	respFrq.end(),			_dest->respFrq.begin());
	copy(respFrqTime.begin(),	respFrqTime.end(),  _dest->respFrqTime.begin());
	copy(act.begin(),		act.end(),				_dest->act.begin());
	copy(actTime.begin(),	actTime.end(),			_dest->actTime.begin());
}

void CArousalEvnt::copyFrom(CArousalEvnt *_source)
{
	CEvnt::copyFrom((CEvnt *) _source);

	//---Delete current reraEvnt
	if (reraEvnt) delete(reraEvnt);
	reraEvnt = NULL;

	CRERAEvnt *rP = ((CArousalEvnt *)_source)->getRERAEvnt();
	if (rP) {
		reraEvnt = new CRERAEvnt();
		reraEvnt->copyFrom(rP);
	}

	adm.clear();
	admTime.clear();
	pr.clear();
	prTime.clear();
	respFrq.clear();
	respFrqTime.clear();
	act.clear();
	actTime.clear();
		
	adm.resize(_source->adm.size(),.0f);
	admTime.resize(_source->admTime.size(),.0f);
	pr.resize(_source->pr.size(),.0f);
	prTime.resize(_source->prTime.size(),.0f);
	respFrq.resize(_source->respFrq.size(),.0f);
	respFrqTime.resize(_source->respFrqTime.size(),.0f);
	act.resize(_source->act.size(),.0f);
	actTime.resize(_source->actTime.size(),.0f);
	
	copy(_source->adm.begin(),			_source->adm.end(),				adm.begin());
	copy(_source->admTime.begin(),		_source->admTime.end(),			admTime.begin());
	copy(_source->pr.begin(),			_source->pr.end(),				pr.begin());
	copy(_source->prTime.begin(),		_source->prTime.end(),			prTime.begin());
	copy(_source->respFrq.begin(),		_source->respFrq.end(),			respFrq.begin());
	copy(_source->respFrqTime.begin(),	_source->respFrqTime.end(),		respFrqTime.begin());
	copy(_source->act.begin(),			_source->act.end(),				act.begin());
	copy(_source->actTime.begin(),		_source->actTime.end(),			actTime.begin());
}

void CArousalEvnt::setAdmittance(vector <FLOAT>::iterator _from,
							  vector <FLOAT>::iterator _to,
							  vector <FLOAT>::iterator _fromTime,
							  vector <FLOAT>::iterator _toTime)
{
	adm.clear();
	admTime.clear();
	vector <FLOAT>::iterator it;
	for (it = _from ; it < _to ; it++) adm.push_back(*it);
	for (it = _fromTime ; it < _toTime ; it++) admTime.push_back(*it);
}

void CArousalEvnt::setPulseRate(vector <FLOAT>::iterator _from,
							 vector <FLOAT>::iterator _to,
							 vector <FLOAT>::iterator _fromTime,
							 vector <FLOAT>::iterator _toTime)
{
	pr.clear();
	prTime.clear();
	vector <FLOAT>::iterator it;
	for (it = _from ; it < _to ; it++) pr.push_back(*it);
	for (it = _fromTime ; it < _toTime ; it++) prTime.push_back(*it);
}

void CArousalEvnt::setRespFrq(vector <FLOAT>::iterator _from,
						   vector <FLOAT>::iterator _to,
						   vector <FLOAT>::iterator _fromTime,
						   vector <FLOAT>::iterator _toTime)
{
	respFrq.clear();
	respFrqTime.clear();
	vector <FLOAT>::iterator it;
	for (it = _from ; it < _to ; it++) respFrq.push_back(*it);
	for (it = _fromTime ; it < _toTime ; it++) respFrqTime.push_back(*it);
}

void CArousalEvnt::setAct(vector <FLOAT>::iterator from,
					   vector <FLOAT>::iterator to,
					   vector <FLOAT>::iterator fromTime,
					   vector <FLOAT>::iterator toTime)
{
	act.clear();
	actTime.clear();
	vector <FLOAT>::iterator it;
	for (it = from ; it < to ; it++) act.push_back(*it);
	for (it = fromTime ; it < toTime ; it++) actTime.push_back(*it);
}

////////////////////////////////////////////////////

CCentralEvnt::CCentralEvnt(FLOAT _from,FLOAT _to,COleDateTime _startRecording,
						   short _bodyPos,int _source /* = autoDetection */) :
	CEvnt(_from,_to,_startRecording,_bodyPos,_source)
{
	eventType = evntTypeCentral;
}
	
CCentralEvnt::CCentralEvnt() :
	CEvnt(.0f,.0f,defDateTime,posUndefined,autoDetection) 
{
	eventType = evntTypeCentral;
}

CCentralEvnt::~CCentralEvnt()
{
}

void CCentralEvnt::Serialize(CArchive& ar, int _majorVersion, int _minorVersion)
{
	CEvnt::Serialize(ar, _majorVersion, _minorVersion);
}

void CCentralEvnt::Serialize20(CArchive & ar, int _majorVersion, int _minorVersion)
{
	CEvnt::Serialize20(ar, _majorVersion, _minorVersion);
}


void CCentralEvnt::copyTo(CCentralEvnt *_dest)
{
	CEvnt::copyTo((CEvnt *) _dest);
}

void CCentralEvnt::copyFrom(CCentralEvnt *_source)
{
	CEvnt::copyFrom((CEvnt *) _source);
}

//////////////////////////////////////////////////////////////////////////////////////////////////

CSpO2DropEvnt::CSpO2DropEvnt(int _dropLimit,
							 FLOAT _dropSize,
							 FLOAT _beginSpO2,
							 FLOAT _endSpO2,
							 FLOAT _from,
							 FLOAT _to,
							 COleDateTime _startRecording,
							 short _bodyPos,
							 int _source /* = autoDetection */) :
	dropLimit(_dropLimit),
	dropSize(_dropSize),
	beginSpO2(_beginSpO2),
	endSpO2(_endSpO2),	
	CEvnt(_from,_to,_startRecording,_bodyPos,_source)
{
	eventType = evntTypeSpO2Drop;
	marriedWithHypopnea = false;
	partnerHypopneaCentreTime = .0f;
}
	
CSpO2DropEvnt::CSpO2DropEvnt(int _dropLimit) :
	dropLimit(_dropLimit),
	dropSize(.0f),
	beginSpO2(.0f),
	endSpO2(.0f),
	CEvnt(.0f,.0f,defDateTime,posUndefined,autoDetection) 
{
	eventType = evntTypeSpO2Drop;
	marriedWithHypopnea = false;
	partnerHypopneaCentreTime = .0f;
}

CSpO2DropEvnt::~CSpO2DropEvnt()
{
}

float CSpO2DropEvnt::getBeginSpO2(void)
{
	return beginSpO2;
}

float CSpO2DropEvnt::getEndSpO2(void)
{
	return endSpO2;
}

float CSpO2DropEvnt::getSpO2DropSize(void)
{
	return dropSize;
}

void CSpO2DropEvnt::Serialize(CArchive& ar, int _majorVersion, int _minorVersion)
{
	CEvnt::Serialize(ar, _majorVersion, _minorVersion);

	if (ar.IsStoring()) {
		ar << dropLimit;
		ar << marriedWithHypopnea;
		ar << partnerHypopneaCentreTime;
		ar << dropSize;
		ar << beginSpO2;	// The SpO2 value at the beginning of the drop
		ar << endSpO2;		// The SpO2 value at the end of the drop
	}
	else {
		ar >> dropLimit;
		ar >> marriedWithHypopnea;
		ar >> partnerHypopneaCentreTime;
		ar >> dropSize;
		ar >> beginSpO2;	// The SpO2 value at the beginning of the drop
		ar >> endSpO2;		// The SpO2 value at the end of the drop
	}
}

void CSpO2DropEvnt::Serialize20(CArchive & ar, int _majorVersion, int _minorVersion)
{
	CEvnt::Serialize20(ar, _majorVersion, _minorVersion);

	if (ar.IsStoring()) {
		ar << dropLimit;
		ar << marriedWithHypopnea;
		ar << partnerHypopneaCentreTime;
		ar << dropSize;
		ar << beginSpO2;	// The SpO2 value at the beginning of the drop
		ar << endSpO2;		// The SpO2 value at the end of the drop
	}
	else {
		ar >> dropLimit;
		ar >> marriedWithHypopnea;
		ar >> partnerHypopneaCentreTime;
		ar >> dropSize;
		ar >> beginSpO2;	// The SpO2 value at the beginning of the drop
		ar >> endSpO2;		// The SpO2 value at the end of the drop
	}
}


void CSpO2DropEvnt::copyTo(CSpO2DropEvnt *_dest)
{
	CEvnt::copyTo((CEvnt *) _dest);
	
	_dest->marriedWithHypopnea = marriedWithHypopnea;
	_dest->partnerHypopneaCentreTime = partnerHypopneaCentreTime;

	_dest->dropSize = dropSize;
	_dest->beginSpO2 = beginSpO2;
	_dest->endSpO2 = endSpO2;
}

void CSpO2DropEvnt::copyFrom(CSpO2DropEvnt *_source)
{
	CEvnt::copyFrom((CEvnt *) _source);
	
	marriedWithHypopnea =		_source->marriedWithHypopnea;
	partnerHypopneaCentreTime = _source->partnerHypopneaCentreTime;

	dropSize =		_source->dropSize;
	beginSpO2 =		_source->beginSpO2;
	endSpO2 =		_source->endSpO2;
}


int  CSpO2DropEvnt::getDropLimit(void)
{
	return dropLimit;
}

void CSpO2DropEvnt::setDropLimit(int _limit)
{
	dropLimit = _limit;
}

void CSpO2DropEvnt::setHypoPartner(float _time,bool _on)
{
	marriedWithHypopnea = _on;
	partnerHypopneaCentreTime = _time;
}

bool CSpO2DropEvnt::getHypoPartner(float *_time)
{
	*_time = partnerHypopneaCentreTime;
	return marriedWithHypopnea;
}

///////////////////////////////////////////////////////////////////////////////

CManualMarkerEvnt::CManualMarkerEvnt(FLOAT _from,FLOAT _to,COleDateTime _startRecording,
						   short _bodyPos,int _source /* = autoDetection */) :
	CEvnt(_from,_to,_startRecording,_bodyPos,_source)
{
	eventType = evntTypeManMarker;
}
	
CManualMarkerEvnt::CManualMarkerEvnt() :
	CEvnt(.0f,.0f,defDateTime,posUndefined,autoDetection) 
{
	eventType = evntTypeManMarker;
	text = _T("");
}

CManualMarkerEvnt::~CManualMarkerEvnt()
{
}

void CManualMarkerEvnt::Serialize(CArchive& ar, int _majorVersion, int _minorVersion)
{
	CEvnt::Serialize(ar, _majorVersion, _minorVersion);

	if (ar.IsStoring()) ar << text;
	else ar >> text;
}

void CManualMarkerEvnt::Serialize20(CArchive & ar, int _majorVersion, int _minorVersion)
{
	CEvnt::Serialize20(ar, _majorVersion, _minorVersion);

	if (ar.IsStoring()) ar << text;
	else ar >> text;
}


void CManualMarkerEvnt::copyTo(CManualMarkerEvnt *_dest)
{
	CEvnt::copyTo((CEvnt *)_dest);
	_dest->text = text;
}

void CManualMarkerEvnt::copyFrom(CManualMarkerEvnt *_source)
{
	CEvnt::copyFrom((CEvnt *)_source);
	text = _source->text;
}


///////////////////////////////////////////////////////////////////////////////

CImportedEvnt::CImportedEvnt(FLOAT _from, FLOAT _to, COleDateTime _startRecording,
	short _bodyPos, int _source /* = autoDetection */) :
	CEvnt(_from, _to, _startRecording, _bodyPos, _source)
{
	eventType = evntTypeImported;
}

CImportedEvnt::CImportedEvnt() :
	CEvnt(.0f, .0f, defDateTime, posUndefined, autoDetection)
{
	eventType = evntTypeImported;
	text = _T("");
}

CImportedEvnt::~CImportedEvnt()
{
}

void CImportedEvnt::Serialize(CArchive& ar, int _majorVersion, int _minorVersion)
{
	CEvnt::Serialize(ar,_majorVersion,_minorVersion);

	if (ar.IsStoring()) ar << text;
	else ar >> text;
}

void CImportedEvnt::Serialize20(CArchive & ar, int _majorVersion, int _minorVersion)
{
	CEvnt::Serialize20(ar, _majorVersion, _minorVersion);

	if (ar.IsStoring()) ar << text;
	else ar >> text;
}


void CImportedEvnt::copyTo(CImportedEvnt *_dest)
{
	CEvnt::copyTo((CEvnt *)_dest);
	_dest->text = text;
}

void CImportedEvnt::copyFrom(CImportedEvnt *_source)
{
	CEvnt::copyFrom((CEvnt *)_source);
	text = _source->text;
}

/////////////////////////////////////////////////////////////


CSnoringEvnt::CSnoringEvnt(FLOAT _from,FLOAT _to,COleDateTime _startRecording,
						   short _bodyPos,int _source /* = autoDetection */) :
	originalFrom(_from), originalTo(_to),
	CEvnt(_from,_to,_startRecording,_bodyPos,_source)
{
	hasLevelDetection = true;
	eventType = evntTypeSnoring;
	for (int i = 0 ; i < posCount ; ++i) timeInPos[i] = .0f;
	for (int i = 0 ; i < levelTypeCount ; ++i) timeAtLevelType[i] = .0f;
}
	
/*
CSnoringEvnt::CSnoringEvnt() :
	CEvnt(.0f,.0f,defDateTime,posUndefined,autoDetection) 
{
	eventType = evntTypeSnoring;
	for (int i = 0 ; i < posCount ; ++i) timeInPos[i] = .0f;
	for (int i = 0 ; i < levelTypeCount ; ++i) timeAtLevelType[i] = .0f;
}
*/

CSnoringEvnt::~CSnoringEvnt()
{
}

/*
Description:
Never exclude snoring due to exclude window. This has already been taken care of in the snoring analysis
*/
bool CSnoringEvnt::considerManualExclusion(float _excludedStart, float _excludedStop)
{
	return false;
}

vector<FLOAT>* CSnoringEvnt::getLowerLevelStartStopVector(void)
{
	return &lowerLevelStartStop;
}

vector<FLOAT>* CSnoringEvnt::getUpperLevelStartStopVector(void)
{
	return &upperLevelStartStop;
}

vector<FLOAT>* CSnoringEvnt::getMultiLevelStartStopVector(void)
{
	return &multiLevelStartStop;
}

vector<FLOAT>* CSnoringEvnt::getUndefLevelStartStopVector(void)
{
	return &undefLevelStartStop;
}

void CSnoringEvnt::doLevelAnalysis(void)
{
	CEvnt::doLevelAnalysis();

	//---No gradient data
	if (0 == pgrad.size()) {
		undefLevelStartStop.push_back(from);
		undefLevelStartStop.push_back(to);
		return;
	}

	//---Lower
	lowerLevelStartStop.clear();
	vector <FLOAT>::iterator lower1 = pgrad.begin();
	vector <FLOAT>::iterator lower2 = pgrad.begin();
	while (lower1 < pgrad.end()) {
		lower1 = find_if(lower2, pgrad.end(), isBelow40percent);
		if (lower1 < pgrad.end()) {
			lower2 = find_if_not(lower1, pgrad.end(), isBelow40percent);
			if (lower1 < (lower2 - 1)) {
				lowerLevelStartStop.push_back(pgradTime.at(distance(pgrad.begin(), lower1)));
				lowerLevelStartStop.push_back(pgradTime.at(distance(pgrad.begin(), lower2 - 1)));
			}
		}
	}

	//---Upper
	upperLevelStartStop.clear();
	vector <FLOAT>::iterator upper1 = pgrad.begin();
	vector <FLOAT>::iterator upper2 = pgrad.begin();
	while (upper1 < pgrad.end()) {
		upper1 = find_if(upper2, pgrad.end(), isAbove60percent);
		if (upper1 < pgrad.end()) {
			upper2 = find_if_not(upper1, pgrad.end(), isAbove60percent);
			if (upper1 < (upper2 - 1)) {
				upperLevelStartStop.push_back(pgradTime.at(distance(pgrad.begin(), upper1)));
				upperLevelStartStop.push_back(pgradTime.at(distance(pgrad.begin(), upper2 - 1)));
			}
		}
	}

	//---Multi
	multiLevelStartStop.clear();
	vector <FLOAT>::iterator m1 = pgrad.begin();
	vector <FLOAT>::iterator m2 = pgrad.begin();
	while (m1 < pgrad.end()) {
		m1 = find_if(m2, pgrad.end(), isNear50percent);
		if (m1 < pgrad.end()) {
			m2 = find_if_not(m1, pgrad.end(), isNear50percent);
			if (m1 < (m2 - 1)) {
				multiLevelStartStop.push_back(pgradTime.at(distance(pgrad.begin(), m1)));
				multiLevelStartStop.push_back(pgradTime.at(distance(pgrad.begin(), m2 - 1)));
			}
		}
	}
}

/*
Description: The event time line vector is the result of editing

The event timeline vector defines the levels as edittde by user, hence the gradient cannot be used
*/
void CSnoringEvnt::sumUpTimeAtLevels(vector <UINT>::iterator _evntTimelineFrom, vector <UINT>::iterator _evntTimelineTo)
{
	int cntLower, cntUpper, cntMulti, cntUndef;
	cntLower = cntUpper = cntMulti = cntUndef = 0;
	vector <UINT>::iterator iT;
	for (iT = _evntTimelineFrom; iT < _evntTimelineTo; ++iT) {
		cntLower += *iT & evntSnoringLevelLower ? 1 : 0;
		cntUpper += *iT & evntSnoringLevelUpper ? 1 : 0;
		cntMulti += *iT & evntSnoringLevelMulti ? 1 : 0;
		cntUndef += *iT & evntSnoringLevelUndef ? 1 : 0;
	}
	timeAtLevelType[levelTypeLower] = EVENT_TIMELINE_TIME_INTERVAL * cntLower;
	timeAtLevelType[levelTypeUpper] = EVENT_TIMELINE_TIME_INTERVAL * cntUpper;
	timeAtLevelType[levelTypeMulti] = EVENT_TIMELINE_TIME_INTERVAL * cntMulti;
	timeAtLevelType[levelTypeUndef] = EVENT_TIMELINE_TIME_INTERVAL * cntUndef;
}


/*
Description: The event time line vector is the result of editing

The event timeline vector defines the levels as edittde by user, hence the gradient cannot be used
*/
void CSnoringEvnt::doLevelAnalysis(size_t _startDistance,vector <UINT>::iterator _evntTimelineFrom, vector <UINT>::iterator _evntTimelineTo)
{
	vector <UINT>::iterator iT,i0,i1;

	//---Lower
	lowerLevelStartStop.clear();
	for (iT = _evntTimelineFrom; iT < _evntTimelineTo; ++iT) {
		i0 = find_if(iT, _evntTimelineTo, isEvntSnoringLevelLower);
		if (_evntTimelineTo == i0) break;
		i1 = find_if_not(i0, _evntTimelineTo, isEvntSnoringLevelLower);
		size_t offs0 = _startDistance + distance(_evntTimelineFrom, i0);
		lowerLevelStartStop.push_back(EVENT_TIMELINE_TIME_INTERVAL * offs0);
		size_t offs1 = _startDistance + distance(_evntTimelineFrom, i1);
		lowerLevelStartStop.push_back(EVENT_TIMELINE_TIME_INTERVAL * (offs1 - 1));
		iT = i1;
	}

	//---Upper
	upperLevelStartStop.clear();
	for (iT = _evntTimelineFrom; iT < _evntTimelineTo; ++iT) {
		i0 = find_if(iT, _evntTimelineTo, isEvntSnoringLevelUpper);
		if (_evntTimelineTo == i0) break;
		i1 = find_if_not(i0, _evntTimelineTo, isEvntSnoringLevelUpper);
		size_t offs0 = _startDistance + distance(_evntTimelineFrom, i0);
		upperLevelStartStop.push_back(EVENT_TIMELINE_TIME_INTERVAL * offs0);
		size_t offs1 = _startDistance + distance(_evntTimelineFrom, i1);
		upperLevelStartStop.push_back(EVENT_TIMELINE_TIME_INTERVAL * (offs1 - 1));
		iT = i1;
	}
	//---Multi
	multiLevelStartStop.clear();
	for (iT = _evntTimelineFrom; iT < _evntTimelineTo; ++iT) {
		i0 = find_if(iT, _evntTimelineTo, isEvntSnoringLevelMulti);
		if (_evntTimelineTo == i0) break;
		i1 = find_if_not(i0, _evntTimelineTo, isEvntSnoringLevelMulti);
		size_t offs0 = _startDistance + distance(_evntTimelineFrom, i0);
		multiLevelStartStop.push_back(EVENT_TIMELINE_TIME_INTERVAL * offs0);
		size_t offs1 = _startDistance + distance(_evntTimelineFrom, i1);
		multiLevelStartStop.push_back(EVENT_TIMELINE_TIME_INTERVAL * (offs1 - 1));
		iT = i1;
	}
}

UINT CSnoringEvnt::getLevelAtTimeForMouseover(float _time)
{
	if (0 == pgrad.size())	
		return OVER_UNDEFINED_LEVEL;

	vector <FLOAT>::iterator iT = lower_bound(pgradTime.begin(), pgradTime.end(), _time);
	if (pgradTime.end() == iT) return OVER_UNDEFINED_LEVEL;

	FLOAT val = pgrad.at(distance(pgradTime.begin(), iT));
	if (val > GRADIENT_UPPER_LIMIT) return OVER_UPPER;
	else if (val < GRADIENT_LOWER_LIMIT) return OVER_LOWER;
	else return OVER_MULTI;
}


void CSnoringEvnt::fillAmStrengthsPerLevel(vector <UINT> *_eventTimeLineV,vector<FLOAT>* _lowV, vector<FLOAT>* _multiV, vector<FLOAT>* _upperV)
{
	fillSnoringStrengthsPerLevel(_eventTimeLineV,&airMicSumVector, &airMicTimeVector,_lowV, _multiV, _upperV);
}

void CSnoringEvnt::fillCmStrengthsPerLevel(vector <UINT> *_eventTimeLineV,vector<FLOAT>* _lowV, vector<FLOAT>* _multiV, vector<FLOAT>* _upperV)
{
	fillSnoringStrengthsPerLevel(_eventTimeLineV,&contactMicVector, &contactMicTimeVector,_lowV, _multiV, _upperV);
}

void CSnoringEvnt::fillSnoringStrengthsPerLevel(vector <UINT> *_eventTimeLineV,vector<FLOAT>* _soundVector, 
	vector <FLOAT> *_soundTimeVector,vector<FLOAT>* _lowV, vector<FLOAT>* _multiV, vector<FLOAT>* _upperV)
{
	//---If these data are missing
	//if (0 == pgrad.size()) return;
	if (0 == _soundVector->size()) return;

	//ASSERT(pgrad.size() == pgradTime.size());
	ASSERT(_soundVector->size() == _soundVector->size());

	//vector <FLOAT>::iterator gP0, gP1, gP;
	vector <FLOAT>::iterator dP0, dP1, maxP;
	int cnt0 = -20;
	int cnt1 = cnt0 + 40;
	int cntMax = -20 + _soundVector->size();
	//int cntMaxGrad = -20 + pgrad.size();
	for (; cnt0 < cntMax; ++cnt0, ++cnt1) {
		//gP0 = cnt0 >= 0 ? pgrad.begin() + cnt0 : pgrad.begin();
		//gP1 = cnt1 < cntMaxGrad ? pgrad.begin() + cnt1 : pgrad.end();
		dP0 = cnt0 >= 0 ? _soundVector->begin() + cnt0 : _soundVector->begin();
		dP1 = cnt1 < cntMax ? _soundVector->begin() + cnt1 : _soundVector->end();

		if (dP0 == dP1) break;
		//if (gP0 == gP1) break;

		maxP = max_element(dP0, dP1);

		//---Check if snoring at this time
		int distToMaxEl = distance(dP0, maxP);
		float time = from + _soundTimeVector->at(distToMaxEl);
		int sample = (int) (time * EVENT_TIMELINE_TIME_INTERVAL_INVERSE);
		if (sample < 0) return;
		if (sample >= (int) _eventTimeLineV->size()) return;
		UINT state = _eventTimeLineV->at(sample);
		if (!(state & evntTypeSnoring)) return;
		if (state & evntTypeAwakeOrExcluded) return;

		//gP = gP0;
		//float sum = .0f;
		//for (; gP < gP1; ++gP) {
		//	sum += *gP;
		//}
		//float level = sum / distance(gP0, gP1);

		if (state & evntSnoringLevelLower) {
			_lowV->push_back(*maxP);
		}
		else if (state & evntSnoringLevelUpper) {
			_upperV->push_back(*maxP);
		}
		else if (state & evntSnoringLevelMulti) {
			_multiV->push_back(*maxP);
		}
	}
}

void CSnoringEvnt::Serialize(CArchive& ar, int _majorVersion, int _minorVersion)
{
	CEvnt::Serialize(ar, _majorVersion,_minorVersion);

	if (ar.IsStoring()) {
		ar << originalFrom;
		ar << originalTo;
	}
	else {
		ar >> originalFrom;
		ar >> originalTo;
	}

	if (ar.IsStoring()) {
		for (int i = 0; i < posCount; i++) ar << timeInPos[i];
		for (int i = 0; i < levelTypeCount; i++) ar << timeAtLevelType[i];
	}
	else {
		for (int i = 0; i < posCount; i++) ar >> timeInPos[i];
		for (int i = 0; i < levelTypeCount; i++) ar >> timeAtLevelType[i];
	}

	//---Level vectors
	if (ar.IsStoring()) {
		serializeVectorFloat(ar, lowerLevelStartStop.begin(), lowerLevelStartStop.end(), &lowerLevelStartStop);
		serializeVectorFloat(ar, multiLevelStartStop.begin(), multiLevelStartStop.end(), &multiLevelStartStop);
		serializeVectorFloat(ar, upperLevelStartStop.begin(), upperLevelStartStop.end(), &upperLevelStartStop);
		serializeVectorFloat(ar, undefLevelStartStop.begin(), undefLevelStartStop.end(), &undefLevelStartStop);
	}
	else {  // Is reading
		if ((_majorVersion >= 30) && (_minorVersion >= 0)) {
			serializeVectorFloat(ar, lowerLevelStartStop.begin(), lowerLevelStartStop.end(), &lowerLevelStartStop);
			serializeVectorFloat(ar, multiLevelStartStop.begin(), multiLevelStartStop.end(), &multiLevelStartStop);
			serializeVectorFloat(ar, upperLevelStartStop.begin(), upperLevelStartStop.end(), &upperLevelStartStop);
			serializeVectorFloat(ar, undefLevelStartStop.begin(), undefLevelStartStop.end(), &undefLevelStartStop);
		}
		else {
			lowerLevelStartStop.clear();
			multiLevelStartStop.clear();
			upperLevelStartStop.clear();
			switch (levelType) {
			case levelTypeLower :
				lowerLevelStartStop.push_back(from);
				lowerLevelStartStop.push_back(to);
				break;
			case levelTypeUpper:
				upperLevelStartStop.push_back(from);
				upperLevelStartStop.push_back(to);
				break;
			case levelTypeMulti:
				multiLevelStartStop.push_back(from);
				multiLevelStartStop.push_back(to);
				break;
			default:
				break;
			}
		}
	}
}


/*
Description: for events data file version 20
*/
void CSnoringEvnt::Serialize20(CArchive & ar, int _majorVersion, int _minorVersion)
{
	ASSERT(20 == _majorVersion);
	if (20 != _majorVersion) return;

	CEvnt::Serialize20(ar, _majorVersion, _minorVersion);

	//---Only for reading
	if (!ar.IsStoring()) {
		ar >> originalFrom;
		ar >> originalTo;
	
		for (int i = 0; i < posCount; i++) ar >> timeInPos[i];
		for (int i = 0; i < levelTypeCount; i++) ar >> timeAtLevelType[i];
	}
}

void CSnoringEvnt::copyTo(CSnoringEvnt *_dest)
{
	CEvnt::copyTo((CEvnt *) _dest);

	_dest->originalFrom = originalFrom;
	_dest->originalTo = originalTo;

	for (int i = 0 ; i < posCount ; i++) {
		_dest->timeInPos[i] = timeInPos[i];
	}
	for (int i = 0 ; i < levelTypeCount ; i++) {
		_dest->timeAtLevelType[i] = timeAtLevelType[i];
	}
}

void CSnoringEvnt::copyFrom(CSnoringEvnt *_source)
{
	CEvnt::copyFrom((CEvnt *) _source);

	originalFrom = _source->originalFrom;
	originalTo = _source->originalTo;
	
	for (int i = 0 ; i < posCount ; i++) {
		timeInPos[i] = _source->timeInPos[i];
	}
	for (int i = 0 ; i < levelTypeCount ; i++) {
		timeAtLevelType[i] = _source->timeAtLevelType[i];
	}
}


/*
Description:
	Moves a second at a time and checks level every second.
	Adds up
*/
void CSnoringEvnt::sumUpTimeAtLevels(void)
{
	vector <FLOAT>::iterator itT,itP;

	for (int i = 0 ; i < levelTypeCount ; i++) timeAtLevelType[i] = .0f;
	for (float t = from ; t <= to ; t += .1f) {

		itT = upper_bound(pgradTime.begin(),pgradTime.end(),t);
		
		if (itT >= pgradTime.end()) break;

		itP = pgrad.begin();
		itP += distance(pgradTime.begin(),itT);
		int level = levelTypeUndef;
		if (*itP <= GRADIENT_LOWER_LIMIT) level = levelTypeLower;
		else if (*itP >= GRADIENT_UPPER_LIMIT) level = levelTypeUpper;
		else level = levelTypeMulti;
		timeAtLevelType[level] += .1f;
	}
}

/*
Description:
	Moves a second at a time and checks body position every second.
	Adds up
*/
void CSnoringEvnt::sumUpTimeInPositions(void)
{
	vector <FLOAT>::iterator itT,itBp;
	ASSERT(bpVector.size() == bpTimeVector.size());

	for (int i = 0 ; i < posCount ; i++) timeInPos[i] = .0f;

	if (0 == bpVector.size()) return;
	if (1 == bpVector.size()) {
		int pos = (int) bpVector.at(0);
		timeInPos[pos] += (to - from);
		return;
	}

	for (float t = from; t <= to; t += .1f) {
		itT = upper_bound(bpTimeVector.begin(), bpTimeVector.end(), t);

		if (itT == bpTimeVector.end()) {
			int pos = (int) bpVector.at(bpVector.size() - 1);
			timeInPos[pos] += .1f;
		}
		else if (itT < bpTimeVector.end()) {
			itBp = bpVector.begin();
			itBp += distance(bpTimeVector.begin(), itT);
			int pos = (int)*itBp;
			timeInPos[pos] += .1f;
		}
	}
}

float CSnoringEvnt::getTimeInBodyPos(int _pos)
{
	ASSERT(_pos < posCount);
	return timeInPos[_pos];
}

float CSnoringEvnt::getTimeAtLevelType(int _levelType)
{
	ASSERT(_levelType < levelTypeCount);
	return timeAtLevelType[_levelType];
}
///////////////////////////////////////////////////////////////////////////////

CSwallowEvnt::CSwallowEvnt(FLOAT _from, FLOAT _to, COleDateTime _startRecording,
	short _bodyPos, int _source /* = autoDetection */) :
	CEvnt(_from, _to, _startRecording, _bodyPos, _source)
{
	eventType = evntTypeSwallow;
	hasLevelDetection = false;
}

CSwallowEvnt::CSwallowEvnt() :
	CEvnt(.0f, .0f, defDateTime, posUndefined, autoDetection)
{
	eventType = evntTypeSwallow;
	hasLevelDetection = false;
}

CSwallowEvnt::~CSwallowEvnt()
{
}

void CSwallowEvnt::Serialize(CArchive& ar, int _majorVersion, int _minorVersion)
{
	CEvnt::Serialize(ar, _majorVersion, _minorVersion);
}

void CSwallowEvnt::Serialize20(CArchive & ar, int _majorVersion, int _minorVersion)
{
	CEvnt::Serialize20(ar, _majorVersion, _minorVersion);
}


void CSwallowEvnt::copyTo(CSwallowEvnt *_dest)
{
	CEvnt::copyTo((CEvnt *)_dest);
}

void CSwallowEvnt::copyFrom(CSwallowEvnt *_source)
{
	CEvnt::copyFrom((CEvnt *)_source);
}
		
//////////////////////////////////////////////////////////////////////////


CAwakeEvnt::CAwakeEvnt(FLOAT _from,FLOAT _to,COleDateTime _startRecording,
						   short _bodyPos,int _source /* = autoDetection */) :
	originalFrom(_from), originalTo(_to),
	CEvnt(_from,_to,_startRecording,_bodyPos,_source)
{
	eventType = evntTypeAwake;
}

/*
CAwakeEvnt::CAwakeEvnt() :
	CEvnt(.0f,.0f,defDateTime,posUndefined,autoDetection) 
{
	eventType = evntTypeAwake;
}
*/

CAwakeEvnt::~CAwakeEvnt()
{
}

/*
Description: 
	Never exclude awake due to exclude window. This has already been taken care of in the awake analysis
*/
bool CAwakeEvnt::considerManualExclusion(float _excludedStart, float _excludedStop)
{
	return false;
}


void CAwakeEvnt::Serialize(CArchive& ar, int _majorVersion, int _minorVersion)
{
	CEvnt::Serialize(ar, _majorVersion, _minorVersion);

	if (ar.IsStoring()) {
		ar << originalFrom ;
		ar << originalTo;
	}
	else {
		ar >> originalFrom;
		ar >> originalTo;
	}

	CEvnt::serializeVectorFloat(ar, act.begin(),act.end(),&act);
	CEvnt::serializeVectorFloat(ar, actTime.begin(),actTime.end(),&actTime);
}

void CAwakeEvnt::Serialize20(CArchive & ar, int _majorVersion, int _minorVersion)
{
	CEvnt::Serialize20(ar, _majorVersion, _minorVersion);

	if (ar.IsStoring()) {
		ar << originalFrom;
		ar << originalTo;
	}
	else {
		ar >> originalFrom;
		ar >> originalTo;
	}

	CEvnt::serializeVectorFloat(ar, act.begin(), act.end(), &act);
	CEvnt::serializeVectorFloat(ar, actTime.begin(), actTime.end(), &actTime);
}


void CAwakeEvnt::copyTo(CAwakeEvnt *_dest)
{
	CEvnt::copyTo((CEvnt *) _dest);

	_dest->originalFrom = originalFrom;
	_dest->originalTo = originalTo;

	_dest->act.clear();
	_dest->actTime.clear();		
	
	_dest->act.resize(act.size(),.0f);
	_dest->actTime.resize(actTime.size(),.0f);
	
	copy(act.begin(),			act.end(),			_dest->act.begin());
	copy(actTime.begin(),		actTime.end(),		_dest->actTime.begin());
}

void CAwakeEvnt::copyFrom(CAwakeEvnt *_source)
{
	CEvnt::copyFrom((CEvnt *) _source);

	originalFrom = _source->originalFrom;
	originalTo = _source->originalTo;

	act.clear();
	actTime.clear();		
	
	act.resize(_source->act.size(),.0f);
	actTime.resize(_source->actTime.size(),.0f);
	
	copy(_source->act.begin(),			_source->act.end(),			act.begin());
	copy(_source->actTime.begin(),		_source->actTime.end(),		actTime.begin());
}

void CAwakeEvnt::setActimeter(vector <FLOAT>::iterator _from,
					vector <FLOAT>::iterator _to,
					vector <FLOAT>::iterator _fromTime,
					vector <FLOAT>::iterator _toTime)
{
	act.clear();
	actTime.clear();
	vector <FLOAT>::iterator it;
	for (it = _from ; it < _to ; it++) act.push_back(*it);
	for (it = _fromTime ; it < _toTime ; it++) actTime.push_back(*it);
}