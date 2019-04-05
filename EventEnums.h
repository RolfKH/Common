#ifndef EVENT_ENUMS_DEFINED
#define EVENT_ENUMS_DEFINED

/*!
 * @author		RKH
 * @date		19.04.17
 * @copyright	Spiro Medical AS 2015-2017
 */

//---Level enums
enum eventsWithoutLevel {
	evArousal,
	evSpO2,
	evManual,
	evCentral,
	evCentralHypo,
	evAwake,
	evStartStop,
	evExcluded,
	evSwallow,
	evImported,
	evCountWithoutLevel
};

enum eventsWithoutLevelForEventsFileVersion20 {
	ev20Arousal,
	ev20SpO2,
	ev20Manual,
	ev20Central,
	ev20CentralHypo,
	ev20Awake,
	ev20StartStop,
	ev20Excluded,
	ev20Swallow,
	//ev20Imported,
	ev20CountWithoutLevel
};

enum eventsWithLevel {
	evMix,
	evObstr,
	evObstrHypo,
	evRERA,
	evSnoringAllLevels,
	evSnoringUpperLevel,
	evSnoringLowerLevel,
	evSnoringMultiLevel,
	evSnoringUndefLevel,
	evCountWithLevel
};

enum eventsWithLevelForEventsFileVersion20 {
	ev20Mix,
	ev20Obstr,
	ev20ObstrHypo,
	ev20RERA,
	ev20Snoring,
	ev20CountWithLevel
};

enum analysisSteps {
	stepExclude,
	stepAwake,
	stepSwallow,
	stepDesat,
	stepApnea,
	stepArousal,
	stepRERA,
	stepSnoring
};

enum levelTypesForBrushesAndGeometries {
	bgLevelTypeUpper,
	bgLevelTypeLower,
	bgLevelTypeMulti,
	bgLevelTypeUndef,
	bgLevelTypeCount
};

#endif