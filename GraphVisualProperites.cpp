#include "stdafx.h"
#include "GraphVisualProperties.h"
#include "RegDefs.h"


CPlotSettings::CPlotSettings(CString _regSubkey) :
	regSubKey(_regSubkey), needInit(false)
{
	background.r = 1.0f;
	background.g = 1.0f;
	background.b = 1.0f;
	
	curve.r = 1.0f;
	curve.g = .0f;
	curve.b = .0f;
	
	thickness = 1.0f;
	transparency = 1.0f;

	x.labelInterval = 2;
	x.min = .0f;
	x.max = 100.0f;
	x.tick = 2.0f;
	
	y.labelInterval = 2;
	y.min = .0f;
	y.max = 100.0f;
	y.tick = 10.0f;
	
	CXTRegistryManager reg;
	reg.SetRegistryKey(COMPANY_NAME,KEY_GRAPHICS);
	COLORREF col;
	BOOL OK = reg.GetProfileColor(regSubKey,BACKGROUND_COLOUR,&col);
	if (OK) {
		background.r = (float) GetRValue(col) / 255.0f;
		background.g = (float) GetGValue(col) / 255.0f;
		background.b = (float) GetBValue(col) / 255.0f;
	}
	OK = reg.GetProfileColor(regSubKey,CURVE_COLOUR,&col);
	if (OK) {
		curve.r = (float) GetRValue(col) / 255.0f;
		curve.g = (float) GetGValue(col) / 255.0f;
		curve.b = (float) GetBValue(col) / 255.0f;
	}
	else needInit = true;
	double dbl;
	OK = reg.GetProfileDouble(regSubKey,THICKNESS,&dbl);
	if (OK) thickness = (float) dbl;
	else needInit = true;
	OK = reg.GetProfileDouble(regSubKey,TRANSPARENCY,&dbl);
	if (OK) transparency = (float) dbl;
	else needInit = true;
	
	OK = reg.GetProfileDouble(regSubKey,MINIMUM_X,&dbl);
	if (OK) x.min = (float) dbl;
	else needInit = true;
	OK = reg.GetProfileDouble(regSubKey,MAXIMUM_X,&dbl);
	if (OK) x.max = (float) dbl;
	else needInit = true;
	OK = reg.GetProfileDouble(regSubKey,TICK_X,&dbl);
	if (OK) x.tick = (float) dbl;
	else needInit = true;
	x.labelInterval = reg.GetProfileInt(regSubKey,LABEL_INTERVAL_X,2);
	
	OK = reg.GetProfileDouble(regSubKey,MINIMUM_Y,&dbl);
	if (OK) y.min = (float) dbl;
	else needInit = true;
	OK = reg.GetProfileDouble(regSubKey,MAXIMUM_Y,&dbl);
	if (OK) y.max = (float) dbl;
	else needInit = true;
	OK = reg.GetProfileDouble(regSubKey,TICK_Y,&dbl);
	if (OK) y.tick = (float) dbl;
	else needInit = true;
	y.labelInterval = reg.GetProfileInt(regSubKey,LABEL_INTERVAL_Y,2);
}

CPlotSettings::~CPlotSettings()
{
	if (needInit) {
		CXTRegistryManager reg;
		reg.SetRegistryKey(COMPANY_NAME,KEY_GRAPHICS);
		COLORREF col = RGB((BYTE) (background.r * 255.0f),
			(BYTE) (background.g * 255.0f),
			(BYTE) (background.b * 255.0f));
		BOOL OK = reg.WriteProfileColor(regSubKey,BACKGROUND_COLOUR,&col);
		ASSERT(OK);
		
		COLORREF colc = RGB((BYTE) (curve.r * 255.0f),
			(BYTE) (curve.g * 255.0f),
			(BYTE) (curve.b * 255.0f));
		OK = reg.WriteProfileColor(regSubKey,CURVE_COLOUR,&colc);
		ASSERT(OK);

		double dbl = (double) thickness;
		OK = reg.WriteProfileDouble(regSubKey,THICKNESS,&dbl);
		ASSERT(OK);
		dbl = (double) transparency;
		OK = reg.WriteProfileDouble(regSubKey,TRANSPARENCY,&dbl);
		ASSERT(OK);

		dbl = (double) x.min;
		OK = reg.WriteProfileDouble(regSubKey,MINIMUM_X,&dbl);
		ASSERT(OK);
		dbl = (double) x.max;
		OK = reg.WriteProfileDouble(regSubKey,MAXIMUM_X,&dbl);
		ASSERT(OK);
		dbl = (double) x.tick;
		OK = reg.WriteProfileDouble(regSubKey,TICK_X,&dbl);
		ASSERT(OK);
		OK = reg.WriteProfileInt(regSubKey,LABEL_INTERVAL_X,x.labelInterval);
		
		dbl = (double) y.min;
		OK = reg.WriteProfileDouble(regSubKey,MINIMUM_Y,&dbl);
		ASSERT(OK);
		dbl = (double) y.max;
		OK = reg.WriteProfileDouble(regSubKey,MAXIMUM_Y,&dbl);
		ASSERT(OK);
		dbl = (double) y.tick;
		OK = reg.WriteProfileDouble(regSubKey,TICK_Y,&dbl);
		ASSERT(OK);
		OK = reg.WriteProfileInt(regSubKey,LABEL_INTERVAL_Y,y.labelInterval);
	}
}