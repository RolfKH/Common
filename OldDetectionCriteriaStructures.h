#ifndef AGS_EVENTS_DETCRIT_INCLUDED
#define AGS_EVENTS_DETCRIT_INCLUDED


/*!
* @author		RKH
* @date			24.03.17
* @copyright	Spiro Medical AS 2015-2017

See events.h for version overview
*/

struct DETECTION_CRITERIA70 {
	float spO2DropLimit;
	BOOL splitCentralAndObstrHypo;
	float pOESalarmLine;
	float levelDetectionMargin;
	float apneaDefaultDuration;
	float hypopneaLimitFraction;
	float apneaShortCentralDuration;
	float apneaShortObstrDuration;
	float maxApneaDur;
	float maxHypopneaDuration;
	float timeInBedLowLimit;
	float extendedWindowForExclusionOfAwake;
	float actimeterAlwaysAwakeLimit;
	float actimeterRelativeAwakeLimit;
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
	float t1EnvPercentStabilityLimit;
	float pressureEnvPercentStabilityLimit;
	float flowStopFraction;
	float pressureDeadFraction;
	float spO2DropSeekWindowLength;
	float spO2DropSeekWindowStart;
	float maxEventLength;
	float gradientUpperLimit;
	float gradientLowerLimit;
	float halfArousalLength;
	float p0RiseCoefficientLimit;
};

struct DETECTION_CRITERIA100 {	// Ver 12.0
	float spO2DropLimit;
	BOOL splitCentralAndObstrHypo;
	float pOESalarmLine;
	float levelDetectionMargin;
	float apneaDefaultDuration;
	float hypopneaLimitFraction;
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
	float pressureEnvPercentStabilityLimit;
	float flowStopFraction;
	float pressureDeadFraction;
	float spO2DropSeekWindowLength;
	float spO2DropSeekWindowStart;
	float maxEventLength;
	float gradientUpperLimit;
	float gradientLowerLimit;
	float halfArousalLength;
	float p0RiseCoefficientLimit;
	float flowIncreaseAtArousal; // %
};

struct DETECTION_CRITERIA200 {	// Ver 20.0
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
	float pressureEnvPercentStabilityLimit;
	float flowStopFraction;
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
};

#endif