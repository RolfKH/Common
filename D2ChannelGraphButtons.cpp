#include "stdafx.h"


/*!
* @author		RKH
* @date		06.06.2017
* @copyright (C)Spiro Medical AS 2013 - 2017
*/

#include "D2ChannelGraphButtons.h"
#include "RegDefs.h"

extern int developerMode,advancedMode;

CGraphBtnChannel::CGraphBtnChannel() 
{
	plotTile = NULL;
	axes = NULL;
	backgr = NULL;
    inTiledView = false;
	int nums = timeAxisLabel.LoadString(IDS_TIME2);
	//enableEditableWindowMouseControl = false;
	title = _T("----");
	canDoStatistics = true;
}

CGraphBtnChannel::~CGraphBtnChannel()
{
}

BEGIN_MESSAGE_MAP(CGraphBtnChannel, CGraphButton)
	ON_WM_NCHITTEST()
END_MESSAGE_MAP()

void CGraphBtnChannel::updateGraphSettings(void)
{
	CButtonPlotSettings plotSettings(dualPolarity, regSubKey);
	ASSERT(backgr);
	backgr->setColour(plotSettings.getBackgrColour());
}


void CGraphBtnChannel::setShowXlabels(bool _on)
{
	if (axes) axes->setShowXlabels(_on);
}

CString CGraphBtnChannel::getTitle(void)
{
	return title;
}

CString CGraphBtnChannel::getEngineeringUnit(void)
{
	return engineeringUnit;
}

void CGraphBtnChannel::setCanDoStatistics(bool _canDoStatistics)
{
	canDoStatistics = _canDoStatistics;
}

bool CGraphBtnChannel::getCanDoStatistics(void)
{
	return canDoStatistics;
}

vector<BOOL>* CGraphBtnChannel::getPenDownVector(void)
{
	return NULL;
}

void CGraphBtnChannel::setTiledView(bool _set)
 {
     inTiledView = _set;
	 if (inTiledView) timeAxisLabel = _T("");
	 else {
		 int nums = timeAxisLabel.LoadString(IDS_TIME2);
	 }
 }

void CGraphBtnChannel::setExtremeXLimits(FLOAT _left,FLOAT _right)
{
	if (plotTile) {
		plotTile->setExtremeXLimits(_left,_right);
		axes->xAxisWizard(_left,_right);
		plotTile->setPlotRectXScaled(_left,_right);
	}
}

void CGraphBtnChannel::setEventsToShow(UINT _evToShow, bool _onlyShowEvents /*= false*/)
{
	eventsToShow = _evToShow;

	drawOnlyEvents = _onlyShowEvents;
	getGraph()->setEventsToShow(_evToShow);
	InvalidateRect(NULL, TRUE);
	UpdateWindow();
}

void CGraphBtnChannel::setCursorWindow(float _start, float _stop)
{
	CRGLGraph *gP = getGraph();
	if (!gP) return;
	gP->setCursorWindow(_start,_stop);
}

void CGraphBtnChannel::doEventGeometry(bool _includeGraph /* = false*/,int _type /* = -1 (ALL)*/,
													bool _withLevel /* = false*/)
{
	CRGLGraph *gP = getGraph();
	if (!gP) return;

	gP->doEventGeometry(_type,_withLevel);
	if (_includeGraph) gP->doGeometry();
}

void CGraphBtnChannel::setData(vector <FLOAT> *_v,vector <FLOAT> *_tv)
{
	// This parent function is empty
}

void CGraphBtnChannel::createCurveSettings(CButtonPlotSettings *_plotSettings)
{
	// This parent function is empty
}

/*
Description: Check for data
Returns: true if data are present, false if not
*/
bool CGraphBtnChannel::getHasData(void)
{
	return false;
}

void CGraphBtnChannel::setPlotTileYLimits(void)
{
	//---No action here. this is virtual
}

BOOL CGraphBtnChannel::Create(LPCTSTR lpszCaption, DWORD dwStyle, const RECT& rect, CWnd* pParentWnd, UINT nID)
{
	// TODO: Add your specialized code here and/or call the base class
	int ret = CGraphButton::Create(lpszCaption, dwStyle, rect, pParentWnd, nID);

	if (1 == ret) {
		CButtonPlotSettings plotSettings(dualPolarity,regSubKey,title);

		backgr = new CRGLBackground();
		backgr->setColour(plotSettings.getBackgrColour());
		addLayer(backgr);
		if (inTiledView) {
			plotTile = new CRGLTile(
				D2D1::RectF(.0f,100.0f,100.0f,.0f),     // %
				D2D1::RectF(5.0f,99.0f,99.0f,1.0f),    // %  
				D2D1::RectF(.0f,1.0f,1.0f,.0f),         // Prelim
				D2D1::RectF(.0f,1.0f,1.0f,.0f));        // Prelim
		}
		else {	
			plotTile = new CRGLTile();
		}
		setPlotTileYLimits();
		addTile(plotTile);
		
		//--Object specific
		createCurveSettings(&plotSettings);
		
		plotTile->setPlotRectYScaled(plotSettings.getYmin(),plotSettings.getYmax());
		runYAxisWizard(plotSettings.getYmin(),plotSettings.getYmax());
		plotTile->setPlotRectXScaled(plotSettings.getXmin(),plotSettings.getXmax());

		axes = new CRGLAxes();
		axes->setTile(plotTile);
		axes->setShowXlabels(inTiledView ? false : true);
		axes->setShowYlabels(true);
		axes->setShowTicks(true);
		axes->setYTick(plotSettings.getYtick());
		axes->setYlabelInterval(plotSettings.getYlabelInterval());
		axes->setXlabelInterval(plotSettings.getXlabelInterval());
		axes->setXTick(plotSettings.getXtick());
		addLayer(axes);
		runXAxisWizard(plotSettings.getXmin(),plotSettings.getXmax());
	}

	return ret;
	//return CGraphButton::Create(lpszCaption, dwStyle, rect, pParentWnd, nID);
}


float CGraphBtnChannel::getMaxYScaleFromMeanAndMax(float _mean,float _max)
{
	float m1 = _mean / (float) AUTO_Y_MEAN_AT;
	float m2 = _max / (float) AUTO_Y_MAX_AT;
	float m = m1 > m2 ? m1 : m2;
	float val = 1.0f / 10000.0f;
	while (val < m) {
		val *= 10.0f;
		for (int i = 1 ; i <= 9 ; i++) {
			float val2 = val * i;
			if (val2 > m) return val2;
		}
	}
	return val;
}

float CGraphBtnChannel::getYAutoMax(void)
{
	return 10.0f;  // Virtual. Just for the compiler
}

LRESULT CGraphBtnChannel::OnNcHitTest(CPoint point)
{
	// TODO: Add your message handler code here and/or call default

	return CGraphButton::OnNcHitTest(point);
}


//////////////////////////////////////////////////////////////////////////////////////////

CGraphBtnTimeAxis::CGraphBtnTimeAxis()
{
	regSubKey = TIME_AXIS_GRAPH;
	int nums = title.LoadString(IDS_TIME_AXIS_LABELS);
}

CGraphBtnTimeAxis::~CGraphBtnTimeAxis()
{
}

void CGraphBtnTimeAxis::OnMouseMove(UINT nFlags, CPoint point)
{
}

BOOL CGraphBtnTimeAxis::Create(LPCTSTR lpszCaption, DWORD dwStyle, const RECT& rect, CWnd* pParentWnd, UINT nID)
{
	// TODO: Add your specialized code here and/or call the base class
	int ret = CGraphButton::Create(lpszCaption, dwStyle, rect, pParentWnd, nID);

	if (1 == ret) {
		CButtonPlotSettings plotSettings(dualPolarity,regSubKey,title);

		backgr = new CRGLBackground();
		backgr->setColour(plotSettings.getBackgrColour());
		addLayer(backgr);
		plotTile = new CRGLTile(
			D2D1::RectF(.0f,100.0f,100.0f,.0f),			// %
			D2D1::RectF(5.0f,100.0f,99.0f,99.0f),		// %   
			D2D1::RectF(.0f,1.0f,1.0f,.0f),				// Prelim
			D2D1::RectF(.0f,1.0f,1.0f,.0f));			// Prelim

		addTile(plotTile);

		plotTile->setPlotRectXScaled(plotSettings.getXmin(),plotSettings.getXmax());

		axes = new CRGLAxes();
		axes->setTile(plotTile);
		axes->setShowXlabels(true);
		axes->setShowYlabels(false);
		axes->setShowTicks(true);
		axes->setYTick(1.0f);
		axes->setYlabelInterval(1);
		axes->setXlabelInterval(plotSettings.getXlabelInterval());
		axes->setXTick(plotSettings.getXtick());
		axes->setColour(.0f,.0f,.0f);
		addLayer(axes);
		runXAxisWizard(plotSettings.getXmin(),plotSettings.getXmax());
	}

	return ret;
	//return CGraphButton::Create(lpszCaption, dwStyle, rect, pParentWnd, nID);
}



//////////////////////////////////////////////////////////////

CGraphBtnBattVoltage::CGraphBtnBattVoltage() 
{
	regSubKey = BATTERY_VOLTAGE;
	curveBattVolt = NULL;
	dataSet = NULL;
	int nums = title.LoadString(IDS_BATT_VOLT_TITLE2);
}


CGraphBtnBattVoltage::~CGraphBtnBattVoltage()
{
}

void CGraphBtnBattVoltage::updateGraphSettings(void)
{
	CGraphBtnChannel::updateGraphSettings();

	//---Additionals here
	CButtonPlotSettings plotSettings(dualPolarity, regSubKey);
	curveBattVolt->setColour(plotSettings.getCurveColour(BATTERY_VOLTAGE));
	curveBattVolt->setTransparencyPerc(plotSettings.getCurveTransparencyPerc(BATTERY_VOLTAGE));
	curveBattVolt->setThickness(plotSettings.getCurveThickness(BATTERY_VOLTAGE));

	render();
}

CString CGraphBtnBattVoltage::getTimeAndAmplAt(LONG _x,LONG _y)
{
	CString s;
	CRGLGraph *gP = getGraph();
	if (!gP) return s;

	return gP->getStringTimeAndAmplAt(_x,_y,_T("Volt")); 
}

/*
Description: Check for data
Returns: true if data are present, false if not
*/
bool CGraphBtnBattVoltage::getHasData(void)
{
	if (!dataSet) return false;
	return dataSet->getSize() ? true : false;
}

void CGraphBtnBattVoltage::setData(CBatteryDataSet *_dataSet)
{
	dataSet = _dataSet;
	setData(dataSet->getVoltageVector(),dataSet->getTimeaxisVector());
}

void CGraphBtnBattVoltage::setData(vector <FLOAT> *_v,vector <FLOAT> *_tv)
{
	ASSERT(curveBattVolt);
	curveBattVolt->setData(_v,_tv);
}

void CGraphBtnBattVoltage::setEvents(CEvents *_eP)
{
	eventsP = _eP;

	ASSERT(curveBattVolt);
	curveBattVolt->setEvents(_eP);
}

void CGraphBtnBattVoltage::createCurveSettings(CButtonPlotSettings *_plotSettings)
{
	curveBattVolt = new CRGLGraph(BATTERY_VOLTAGE);

	ASSERT(curveBattVolt);
	
	CString cTitle;
	int nums = cTitle.LoadString(IDS_BATT_VOLT);
	_plotSettings->setCurveTitle(BATTERY_VOLTAGE,cTitle);

	curveBattVolt->setTile(plotTile);	
	curveBattVolt->setThickness(_plotSettings->getCurveThickness(BATTERY_VOLTAGE));
	addLayer(curveBattVolt);

	curveBattVolt->setColour(_plotSettings->getCurveColour(BATTERY_VOLTAGE));
	curveBattVolt->setTransparencyPerc(_plotSettings->getCurveTransparencyPerc(BATTERY_VOLTAGE));
	curveBattVolt->setShow(true);

	CRGLText *graphTexts = new CRGLText();
	graphTexts->setTile(plotTile);

	CString ys;
	nums = ys.LoadStringW(IDS_VOLT2);
	graphTexts->setTexts(timeAxisLabel,_T(""),ys,title);
	addLayer(graphTexts);
}

float CGraphBtnBattVoltage::getYAutoMax(void)
{
	FLOAT max,avg;
	ASSERT(dataSet);

	max = dataSet->getVoltageVector()->size() > 0 ? 
		*max_element(dataSet->getVoltageVector()->begin(), dataSet->getVoltageVector()->end()) :
		.0f; 
	avg = dataSet->getVoltageVector()->size() > 0 ? 
		(float)for_each(dataSet->getVoltageVector()->begin(), dataSet->getVoltageVector()->end(), Average()) :
		.0f;
	return getMaxYScaleFromMeanAndMax(avg,max);
}

///////////////////////////////////////////////////////////////////////

	
CGraphBtnBattCapacity::CGraphBtnBattCapacity()
{
	regSubKey = BATTERY_CAPACITY;
	curveBattRc	 = NULL;
	curveBattFcc = NULL;
	dataSet = NULL;
	int nums = title.LoadString(IDS_BATTERY_CAPACITIES2);
}

CGraphBtnBattCapacity::~CGraphBtnBattCapacity()
{
}

void CGraphBtnBattCapacity::updateGraphSettings(void)
{
	CGraphBtnChannel::updateGraphSettings();

	//---Additionals here
	CButtonPlotSettings plotSettings(dualPolarity, regSubKey);
	curveBattRc->setColour(plotSettings.getCurveColour(BATTERY_REMAINING_CAPACITY));
	curveBattRc->setTransparencyPerc(plotSettings.getCurveTransparencyPerc(BATTERY_REMAINING_CAPACITY));
	curveBattRc->setThickness(plotSettings.getCurveThickness(BATTERY_REMAINING_CAPACITY));

	curveBattFcc->setColour(plotSettings.getCurveColour(BATTERY_FULL_CHARGE_CAPACITY));
	curveBattFcc->setTransparencyPerc(plotSettings.getCurveTransparencyPerc(BATTERY_FULL_CHARGE_CAPACITY));
	curveBattFcc->setThickness(plotSettings.getCurveThickness(BATTERY_FULL_CHARGE_CAPACITY));

	render();
}


CString CGraphBtnBattCapacity::getTimeAndAmplAt(LONG _x,LONG _y)
{
	D2D1_POINT_2F dt = curveBattRc->getTimeAndAmplAt(_x,_y);
	float x;
	bool ok = curveBattRc->getAmplAt(dt.x,&x);
	if (!ok) return curveBattRc->gethms(dt.x);
	
	float y = .0f;
	ok = curveBattFcc->getAmplAt(dt.x,&y);
	
	CString s;
	s.Format(_T("%s, Rc: %s mW, Fcc: %s mW"), static_cast<LPCTSTR>(curveBattRc->gethms(dt.x)),
		static_cast<LPCTSTR>(curveBattRc->getFormattedNumber(x)),
		static_cast<LPCTSTR>(curveBattFcc->getFormattedNumber(y)));
	return s; 
}

/*
Description: Check for data
Returns: true if data are present, false if not
*/
bool CGraphBtnBattCapacity::getHasData(void)
{
	if (!dataSet) return false;
	return dataSet->getSize() ? true : false;
}

void CGraphBtnBattCapacity::setData(CBatteryDataSet *_dataSet)
{
	dataSet = _dataSet;
	setData(dataSet->getRemainingCapacityVector(),dataSet->getFullChargeCapacityVector(),dataSet->getTimeaxisVector());
}

void CGraphBtnBattCapacity::setData(vector <FLOAT> *_rc,vector <FLOAT> *_fcc,vector <FLOAT> *_tv)
{
	ASSERT(curveBattRc);
	ASSERT(curveBattFcc);
	curveBattRc->setData(_rc,_tv);
	curveBattFcc->setData(_fcc,_tv);
}

void CGraphBtnBattCapacity::setEvents(CEvents *_eP)
{
	eventsP = _eP;
	
	ASSERT(curveBattRc);
	ASSERT(curveBattFcc);
	curveBattRc->setEvents(_eP);
	curveBattFcc->setEvents(_eP);
}

void CGraphBtnBattCapacity::createCurveSettings(CButtonPlotSettings *_plotSettings)
{
	curveBattRc = new CRGLGraph(BATTERY_REMAINING_CAPACITY);
	curveBattFcc = new CRGLGraph(BATTERY_FULL_CHARGE_CAPACITY);

	ASSERT(curveBattRc);
	ASSERT(curveBattFcc);

	CString cTitle1;
	int nums = cTitle1.LoadString(IDS_REMAINING_CAPACITY2);
	_plotSettings->setCurveTitle(BATTERY_REMAINING_CAPACITY,cTitle1);

	curveBattRc->setTile(plotTile);	
	curveBattRc->setThickness(_plotSettings->getCurveThickness(BATTERY_REMAINING_CAPACITY));

	addLayer(curveBattRc);
	curveBattRc->setColour(_plotSettings->getCurveColour(BATTERY_REMAINING_CAPACITY));
	curveBattRc->setTransparencyPerc(_plotSettings->getCurveTransparencyPerc(BATTERY_REMAINING_CAPACITY));
	curveBattRc->setShow(true);
	
	CString cTitle2;
	nums = cTitle2.LoadString(IDS_FULL_CHARGE_CAPACITY2);
	_plotSettings->setCurveTitle(BATTERY_FULL_CHARGE_CAPACITY,cTitle2);

	curveBattFcc->setTile(plotTile);	
	curveBattFcc->setThickness(_plotSettings->getCurveThickness(BATTERY_FULL_CHARGE_CAPACITY));

	addLayer(curveBattFcc);
	curveBattFcc->setColour(_plotSettings->getCurveColour(BATTERY_FULL_CHARGE_CAPACITY));
	curveBattFcc->setTransparencyPerc(_plotSettings->getCurveTransparencyPerc(BATTERY_FULL_CHARGE_CAPACITY));
	curveBattFcc->setShow(true);
	
	CRGLText *graphTexts = new CRGLText();
	graphTexts->setTile(plotTile);

	CString ys;
	nums = ys.LoadStringW(IDS_mAh2);
	graphTexts->setTexts(timeAxisLabel,_T(""),ys,title);
	addLayer(graphTexts);
}


float CGraphBtnBattCapacity::getYAutoMax(void)
{
	FLOAT maxfcc,avgfcc;
	ASSERT(dataSet);
	
	maxfcc = dataSet->getFullChargeCapacityVector()->size() > 0 ?
		*max_element(dataSet->getFullChargeCapacityVector()->begin(), dataSet->getFullChargeCapacityVector()->end()) :
		.0f ; 
	avgfcc = dataSet->getFullChargeCapacityVector()->size() > 0 ?
		(float)for_each(dataSet->getFullChargeCapacityVector()->begin(), dataSet->getFullChargeCapacityVector()->end(), Average()) :
		.0f;
	
	FLOAT maxrc,avgrc;
	
	maxrc = dataSet->getRemainingCapacityVector()->size() > 0 ? 
		*max_element(dataSet->getRemainingCapacityVector()->begin(), dataSet->getRemainingCapacityVector()->end()) :
		.0f;
	avgrc = dataSet->getRemainingCapacityVector()->size() ?
		(float)for_each(dataSet->getRemainingCapacityVector()->begin(), dataSet->getRemainingCapacityVector()->end(), Average()) :
		.0f;
	return getMaxYScaleFromMeanAndMax(avgrc > avgfcc ? avgrc : avgfcc,maxrc > maxfcc ? maxrc : maxfcc);
}

///////////////////////////////////////////////////////////////////////

	
CGraphBtnBattTemperature::CGraphBtnBattTemperature()
{
	regSubKey = BATTERY_TEMPERATURE;
	curveBattTemp = NULL;
	dataSet = NULL;
	int nums = title.LoadString(IDS_BATT_TEMPERATURE2);
}

CGraphBtnBattTemperature::~CGraphBtnBattTemperature()
{
}

void CGraphBtnBattTemperature::updateGraphSettings(void)
{
	CGraphBtnChannel::updateGraphSettings();

	//---Additionals here
	CButtonPlotSettings plotSettings(dualPolarity, regSubKey);
	curveBattTemp->setColour(plotSettings.getCurveColour(BATTERY_TEMPERATURE));
	curveBattTemp->setTransparencyPerc(plotSettings.getCurveTransparencyPerc(BATTERY_TEMPERATURE));
	curveBattTemp->setThickness(plotSettings.getCurveThickness(BATTERY_TEMPERATURE));

	render();
}

CString CGraphBtnBattTemperature::getTimeAndAmplAt(LONG _x,LONG _y)
{
	CString s;
	CRGLGraph *gP = getGraph();
	if (!gP) return s;

	return gP->getStringTimeAndAmplAt(_x,_y,_T("degC")); 
}

/*
Description: Check for data
Returns: true if data are present, false if not
*/
bool CGraphBtnBattTemperature::getHasData(void)
{
	if (!dataSet) return false;
	return dataSet->getSize() ? true : false;
}

void CGraphBtnBattTemperature::setData(CBatteryDataSet *_dataSet)
{
	dataSet = _dataSet;
	setData(dataSet->getTemperatureVector(),dataSet->getTimeaxisVector());
}

void CGraphBtnBattTemperature::setData(vector <FLOAT> *_temp,vector <FLOAT> *_tv)
{
	ASSERT(curveBattTemp);
	curveBattTemp->setData(_temp,_tv);
}

void CGraphBtnBattTemperature::setEvents(CEvents *_eP)
{
	eventsP = _eP;

	ASSERT(curveBattTemp);
	curveBattTemp->setEvents(_eP);
}

void CGraphBtnBattTemperature::createCurveSettings(CButtonPlotSettings *_plotSettings)
{
	curveBattTemp = new CRGLGraph(BATTERY_TEMPERATURE);

	ASSERT(curveBattTemp);

	CString cTitle1;
	int nums = cTitle1.LoadString(IDS_BATTERY_TEMPERATURE2);
	_plotSettings->setCurveTitle(BATTERY_TEMPERATURE,cTitle1);

	curveBattTemp->setTile(plotTile);	
	curveBattTemp->setThickness(_plotSettings->getCurveThickness(BATTERY_TEMPERATURE));

	addLayer(curveBattTemp);
	curveBattTemp->setColour(_plotSettings->getCurveColour(BATTERY_TEMPERATURE));
	curveBattTemp->setTransparencyPerc(_plotSettings->getCurveTransparencyPerc(BATTERY_TEMPERATURE));
	curveBattTemp->setShow(true);
	
	CRGLText *graphTexts = new CRGLText();
	graphTexts->setTile(plotTile);

	CString ys;
	nums = ys.LoadStringW(IDS_degC2);
	graphTexts->setTexts(timeAxisLabel,_T(""),ys,title);
	addLayer(graphTexts);
}


float CGraphBtnBattTemperature::getYAutoMax(void)
{
	ASSERT(dataSet);

	FLOAT max = dataSet->getTemperatureVector()->size() > 0 ? 
		*max_element(dataSet->getTemperatureVector()->begin(), dataSet->getTemperatureVector()->end()) :
		.0f;
	FLOAT min = dataSet->getTemperatureVector()->size() > 0 ? 
		*min_element(dataSet->getTemperatureVector()->begin(), dataSet->getTemperatureVector()->end()) :
		.0f;
	FLOAT avg = dataSet->getTemperatureVector()->size() > 0 ?
		(float)for_each(dataSet->getTemperatureVector()->begin(), dataSet->getTemperatureVector()->end(), Average()) :
		.0f;
	
	FLOAT pos = fabs(max);
	FLOAT neg = fabs(min);
	return getMaxYScaleFromMeanAndMax(avg,pos > neg ? pos : neg);
}


///////////////////////////////////////////////////////////////////////


CGraphBtnBattCurrent::CGraphBtnBattCurrent()
{
	regSubKey = BATTERY_CURRENT;
	curveBattCurrent = NULL;
	dataSet = NULL;
	int nums = title.LoadString(IDS_BATTERY_CURRENT2);
}

CGraphBtnBattCurrent::~CGraphBtnBattCurrent()
{
}

void CGraphBtnBattCurrent::updateGraphSettings(void)
{
	CGraphBtnChannel::updateGraphSettings();

	//---Additionals here
	CButtonPlotSettings plotSettings(dualPolarity, regSubKey);
	curveBattCurrent->setColour(plotSettings.getCurveColour(BATTERY_CURRENT));
	curveBattCurrent->setTransparencyPerc(plotSettings.getCurveTransparencyPerc(BATTERY_CURRENT));
	curveBattCurrent->setThickness(plotSettings.getCurveThickness(BATTERY_CURRENT));

	render();
}

CString CGraphBtnBattCurrent::getTimeAndAmplAt(LONG _x, LONG _y)
{
	CString s;
	CRGLGraph *gP = getGraph();
	if (!gP) return s;

	return gP->getStringTimeAndAmplAt(_x, _y, _T("mA"));
}

/*
Description: Check for data
Returns: true if data are present, false if not
*/
bool CGraphBtnBattCurrent::getHasData(void)
{
	if (!dataSet) return false;
	return dataSet->getSize() ? true : false;
}

void CGraphBtnBattCurrent::setData(CBatteryDataSet *_dataSet)
{
	dataSet = _dataSet;
	setData(dataSet->getCurrentVector(), dataSet->getTimeaxisVector());
}

void CGraphBtnBattCurrent::setData(vector <FLOAT> *_temp, vector <FLOAT> *_tv)
{
	ASSERT(curveBattCurrent);
	curveBattCurrent->setData(_temp, _tv);
}

void CGraphBtnBattCurrent::setEvents(CEvents *_eP)
{
	eventsP = _eP;

	ASSERT(curveBattCurrent);
	curveBattCurrent->setEvents(_eP);
}

void CGraphBtnBattCurrent::createCurveSettings(CButtonPlotSettings *_plotSettings)
{
	curveBattCurrent = new CRGLGraph(BATTERY_CURRENT);

	ASSERT(curveBattCurrent);

	CString cTitle1;
	int nums = cTitle1.LoadString(IDS_BATTERY_CURRENT2);
	_plotSettings->setCurveTitle(BATTERY_CURRENT, cTitle1);

	curveBattCurrent->setTile(plotTile);
	curveBattCurrent->setThickness(_plotSettings->getCurveThickness(BATTERY_CURRENT));

	addLayer(curveBattCurrent);
	curveBattCurrent->setColour(_plotSettings->getCurveColour(BATTERY_CURRENT));
	curveBattCurrent->setTransparencyPerc(_plotSettings->getCurveTransparencyPerc(BATTERY_CURRENT));
	curveBattCurrent->setShow(true);

	CRGLText *graphTexts = new CRGLText();
	graphTexts->setTile(plotTile);

	CString ys;
	nums = ys.LoadStringW(IDS_degC2);
	graphTexts->setTexts(timeAxisLabel, _T(""), ys, title);
	addLayer(graphTexts);
}


float CGraphBtnBattCurrent::getYAutoMax(void)
{
	ASSERT(dataSet);

	FLOAT max = dataSet->getCurrentVector()->size() > 0 ?
		*max_element(dataSet->getCurrentVector()->begin(), dataSet->getCurrentVector()->end()) :
		.0f;
	FLOAT min = dataSet->getCurrentVector()->size() > 0 ?
		*min_element(dataSet->getCurrentVector()->begin(), dataSet->getCurrentVector()->end()) :
		.0f;
	FLOAT avg = dataSet->getCurrentVector()->size() > 0 ?
		(float)for_each(dataSet->getCurrentVector()->begin(), dataSet->getCurrentVector()->end(), Average()) :
		.0f;

	FLOAT pos = fabs(max);
	FLOAT neg = fabs(min);
	return getMaxYScaleFromMeanAndMax(avg, pos > neg ? pos : neg);
}

///////////////////////////////////////////////////////////////////////

	
CGraphBtnContactMic::CGraphBtnContactMic()
{
	regSubKey = TISSUE_VIBRATION;
	curveCm = NULL;
	int nums = title.LoadString(IDS_CM_TITLE2);
	dataSet = NULL;
}

CGraphBtnContactMic::~CGraphBtnContactMic()
{
}

void CGraphBtnContactMic::updateGraphSettings(void)
{
	CGraphBtnChannel::updateGraphSettings();

	//---Additionals here
	CButtonPlotSettings plotSettings(dualPolarity, regSubKey);
	curveCm->setColour(plotSettings.getCurveColour(TISSUE_VIBRATION));
	curveCm->setTransparencyPerc(plotSettings.getCurveTransparencyPerc(TISSUE_VIBRATION));
	curveCm->setThickness(plotSettings.getCurveThickness(TISSUE_VIBRATION));

	render();
}


void CGraphBtnContactMic::updateCurveColour(void)
{
	curveCm->updateBrush();
}

void CGraphBtnContactMic::setPlotTileYLimits(void)
{
	if (!plotTile) return;
	plotTile->setVertScaleLimits(MIN_SCALE_CONTACT_MIC,MAX_SCALE_CONTACT_MIC);
}
	
void CGraphBtnContactMic::setEventsToShow(UINT _evToShow, bool _onlyShowEvents /*= false*/)
{
	eventsToShow = _evToShow;
	drawOnlyEvents = _onlyShowEvents;
	curveCm->setEventsToShow(_evToShow);
	InvalidateRect(NULL,TRUE);
	UpdateWindow();
}

/*
Description: Check for data
Returns: true if data are present, false if not
*/
bool CGraphBtnContactMic::getHasData(void)
{
	if (!dataSet) return false;
	return dataSet->getSize() ? true : false;
}

void CGraphBtnContactMic::setData(CContactMicDataSet *_dataSet)
{
	dataSet = _dataSet;
	setData(dataSet->getCmVector(),dataSet->getTimeaxisVector());
}

void CGraphBtnContactMic::setData(vector <FLOAT> *_cm,vector <FLOAT> *_tv)
{
	ASSERT(curveCm);
	curveCm->setData(_cm,_tv);
}

void CGraphBtnContactMic::setEvents(CEvents *_eP)
{
	eventsP = _eP;

	ASSERT(curveCm);
	curveCm->setEvents(_eP);
}

void CGraphBtnContactMic::createCurveSettings(CButtonPlotSettings *_plotSettings)
{
	curveCm = new CRGLGraphSparse(TISSUE_VIBRATION);

	ASSERT(curveCm);

	CString cTitle1;
	int nums = cTitle1.LoadString(IDS_CONTACT_SOUND);
	_plotSettings->setCurveTitle(TISSUE_VIBRATION,cTitle1);

	curveCm->setTile(plotTile);	
	curveCm->setThickness(_plotSettings->getCurveThickness(TISSUE_VIBRATION));

	addLayer(curveCm);
	curveCm->setColour(_plotSettings->getCurveColour(TISSUE_VIBRATION));
	curveCm->setTransparencyPerc(_plotSettings->getCurveTransparencyPerc(TISSUE_VIBRATION));
	curveCm->setShow(true);
	
	CRGLText *graphTexts = new CRGLText();
	graphTexts->setTile(plotTile);

	CString ys;
	ys = _T("");
	graphTexts->setTexts(timeAxisLabel,_T(""),ys,title);
	addLayer(graphTexts);
}

float CGraphBtnContactMic::getYAutoMax(void)
{
	FLOAT max;
	ASSERT(dataSet);

	max = DEFAULT_CONTACTMIC_MAX;
	
	return max;
}

///////////////////////////////////////////////////////////////////////

	
CGraphBodyPosXYZ::CGraphBodyPosXYZ()
{
	dataSet = NULL;
	regSubKey = BODY_POSITION_XYZ;
	curveX = NULL;
	curveY = NULL;
	curveZ = NULL;
	dualPolarity = true;
	int nums = title.LoadString(IDS_BODY_POSITION2XYZ);
}

CGraphBodyPosXYZ::~CGraphBodyPosXYZ()
{
}

void CGraphBodyPosXYZ::updateGraphSettings(void)
{
	CGraphBtnChannel::updateGraphSettings();

	//---Additionals here
	CButtonPlotSettings plotSettings(dualPolarity, regSubKey);
	curveX->setColour(plotSettings.getCurveColour(BODY_POSITION_X));
	curveX->setTransparencyPerc(plotSettings.getCurveTransparencyPerc(BODY_POSITION_X));
	curveX->setThickness(plotSettings.getCurveThickness(BODY_POSITION_X));

	curveY->setColour(plotSettings.getCurveColour(BODY_POSITION_Y));
	curveY->setTransparencyPerc(plotSettings.getCurveTransparencyPerc(BODY_POSITION_Y));
	curveY->setThickness(plotSettings.getCurveThickness(BODY_POSITION_Y));

	curveZ->setColour(plotSettings.getCurveColour(BODY_POSITION_Z));
	curveZ->setTransparencyPerc(plotSettings.getCurveTransparencyPerc(BODY_POSITION_Z));
	curveZ->setThickness(plotSettings.getCurveThickness(BODY_POSITION_Z));

	render();
}

CString CGraphBodyPosXYZ::getTimeAndAmplAt(LONG _x,LONG _y)
{
	D2D1_POINT_2F dt = curveX->getTimeAndAmplAt(_x,_y);

	float x;
	bool ok = curveX->getAmplAt(dt.x,&x);
	if (!ok) return curveX->gethms(dt.x);
	
	float y = .0f;
	float z = .0f;
	ok = curveY->getAmplAt(dt.x,&y);
	ok = curveZ->getAmplAt(dt.x,&z);
	
	CString s;
	s.Format(_T("%s, x: %s g, y: %s g, z: %s g"), static_cast<LPCTSTR>(curveX->gethms(dt.x)),
		static_cast<LPCTSTR>(curveX->getFormattedNumber(x)),
		static_cast<LPCTSTR>(curveY->getFormattedNumber(y)),
		static_cast<LPCTSTR>(curveZ->getFormattedNumber(z)));
	return s; 
}

void CGraphBodyPosXYZ::setEventsToShow(UINT _evToShow, bool _onlyShowEvents /*= false*/)
{
	eventsToShow = _evToShow;
	drawOnlyEvents = _onlyShowEvents;
	curveX->setEventsToShow(_evToShow);
	InvalidateRect(NULL,TRUE);
	UpdateWindow();
}

/*
Description: Check for data
Returns: true if data are present, false if not
*/
bool CGraphBodyPosXYZ::getHasData(void)
{
	if (!dataSet) return false;
	return dataSet->getSize() ? true : false;
}

void CGraphBodyPosXYZ::setData(CBodyPosXYZDataSet *_dataSet)
{
	dataSet = _dataSet;
	ASSERT(dataSet);
	setData(dataSet->getxVector(),dataSet->getyVector(),dataSet->getzVector(),dataSet->getTimeaxisVector());
}

void CGraphBodyPosXYZ::setData(vector <FLOAT> *_x,vector <FLOAT> *_y,vector <FLOAT> *_z,vector <FLOAT> *_tv)
{
	ASSERT(curveX);
	ASSERT(curveY);
	ASSERT(curveZ);
	curveX->setData(_x,_tv);
	curveY->setData(_y,_tv);
	curveZ->setData(_z,_tv);
}


void CGraphBodyPosXYZ::setEvents(CEvents *_eP)
{
	eventsP = _eP;

	ASSERT(curveX);
	ASSERT(curveY);
	ASSERT(curveZ);
	curveX->setEvents(_eP);
	curveY->setEvents(_eP);
	curveZ->setEvents(_eP);
}

void CGraphBodyPosXYZ::createCurveSettings(CButtonPlotSettings *_plotSettings)
{
	curveX = new CRGLGraph(BODY_POSITION_X);
	curveY = new CRGLGraph(BODY_POSITION_Y);
	curveZ = new CRGLGraph(BODY_POSITION_Z);
	
	ASSERT(curveX);
	ASSERT(curveY);
	ASSERT(curveZ);

	CString cTitle1;
	int nums = cTitle1.LoadString(IDS_BP_X);
	_plotSettings->setCurveTitle(BODY_POSITION_X,cTitle1);

	curveX ->setTile(plotTile);	
	curveX ->setThickness(_plotSettings->getCurveThickness(BODY_POSITION_X));

	addLayer(curveX);
	curveX->setColour(_plotSettings->getCurveColour(BODY_POSITION_X));
	curveX->setTransparencyPerc(_plotSettings->getCurveTransparencyPerc(BODY_POSITION_X));
	curveX->setShow(true);

	CString cTitle2;
	nums = cTitle2.LoadString(IDS_BP_Y);
	_plotSettings->setCurveTitle(BODY_POSITION_Y,cTitle2);

	curveY ->setTile(plotTile);	
	curveY ->setThickness(_plotSettings->getCurveThickness(BODY_POSITION_Y));

	addLayer(curveY);
	curveY->setColour(_plotSettings->getCurveColour(BODY_POSITION_Y));
	curveY->setTransparencyPerc(_plotSettings->getCurveTransparencyPerc(BODY_POSITION_Y));
	curveY->setShow(true);

	CString cTitle3;
	nums = cTitle3.LoadString(IDS_BP_Z);
	_plotSettings->setCurveTitle(BODY_POSITION_Z,cTitle3);

	curveZ ->setTile(plotTile);	
	curveZ ->setThickness(_plotSettings->getCurveThickness(BODY_POSITION_Z));

	addLayer(curveZ);
	curveZ->setColour(_plotSettings->getCurveColour(BODY_POSITION_Z));
	curveZ->setTransparencyPerc(_plotSettings->getCurveTransparencyPerc(BODY_POSITION_Z));
	curveZ->setShow(true);
	
	CRGLText *graphTexts = new CRGLText();
	graphTexts->setTile(plotTile);

	CString ys;
	nums = ys.LoadStringW(IDS_g2);
	graphTexts->setTexts(timeAxisLabel,_T(""),ys,title);
	addLayer(graphTexts);
}


float CGraphBodyPosXYZ::getYAutoMax(void)
{
	return DEFAULT_BODY_POSXYZ_MAX;  // Always 2 
}


///////////////////////////////////////////////////////////////////////

	
CGraphBodyPos::CGraphBodyPos()
{
	YshiftIsAllowed = false;
	YscaleIsAllowed = false;
	dataSet = NULL;
	regSubKey = BODY_POSITION;
	curveBP = NULL;
	int nums = title.LoadString(IDS_BODY_POSITION3);
}

CGraphBodyPos::~CGraphBodyPos()
{
}

/*
Description: Get amplitude in engineering units
*/
bool CGraphBodyPos::getEUAmplAt(LONG _x, LONG _y, CString *_s)
{
	CRGLGraph *gP = getGraph();
	if (!gP) return false;
	CString sp = gP->getAmplAt(_x, _y);
	short bp = _wtoi(sp);
	CString pos;

	int nums;
	switch (bp) {
	case posLeft:
		nums = pos.LoadString(IDS_LEFT2);
		break;
	case posRight:
		nums = pos.LoadString(IDS_RIGHT2);
		break;
	case posProne:
		nums = pos.LoadString(IDS_PRONE2);
		break;
	case posSupine:
		nums = pos.LoadString(IDS_SUPINE2);
		break;
	case posUpright:
		nums = pos.LoadString(IDS_STANDING2);
		break;
	case posUndefined:
		nums = pos.LoadString(IDS_UNDEFINED2);
		break;
	default:
		break;
	}
	*_s = pos;
	return true;
}

vector<BOOL>* CGraphBodyPos::getPenDownVector(void)
{
	return dataSet->getPenDownVector();
}

void CGraphBodyPos::updateGraphSettings(void)
{
	CGraphBtnChannel::updateGraphSettings();

	//---Additionals here
	CButtonPlotSettings plotSettings(dualPolarity, regSubKey);
	curveBP->setColour(plotSettings.getCurveColour(regSubKey));
	curveBP->setTransparencyPerc(plotSettings.getCurveTransparencyPerc(regSubKey));
	curveBP->setThickness(plotSettings.getCurveThickness(regSubKey));

	render();
}


void CGraphBodyPos::updateCurveColour(void)
{
	curveBP->updateBrush();
}

/*
Description: Moves the left cursor
*/
void CGraphBodyPos::moveLeftCursor(int _move)
{
	curveBP->moveLeftCursor(_move);
}

/*
Description: Changes an event
_evnt: The event
_commmand: What to do (this is the menu item ID)
*/
void CGraphBodyPos::changeAnEvent(CEvnt *_evnt,int _command)
{
	curveBP->changeAnEvent(_evnt,_command);
}

/*
Description: Moves the event edge
*/
void CGraphBodyPos::moveEventEdge(CEvnt *_evnt,FLOAT _moveSecs,int _object)
{
	curveBP->moveEventEdge(_evnt,_moveSecs,_object);
}

/*
Description: Moves the left cursor
*/
void CGraphBodyPos::moveRightCursor(int _move)
{
	curveBP->moveRightCursor(_move);
}

/*
Description: Returns the position of the right cursor in screen coordinates
*/
int CGraphBodyPos::getRightCursorX(void)
{
	return curveBP->getRightCursorX();
}

/*
Description: Returns the position of the left cursor in screen coordinates
*/
int CGraphBodyPos::getLeftCursorX(void)
{
	return curveBP->getLeftCursorX();
}

void CGraphBodyPos::doEventGeometry(bool _includeGraph /*= false*/, 
									int _type  /*= -1*/, 
									bool _withLevel /* = false*/)
{
	curveBP->doEventGeometry(_type, _withLevel);
	if (_includeGraph) curveBP->doGeometry();
}

/*
Description: Returns time in seconds
_x: x in screen coordinates
_y: y in screen coordinates
Return: Time in seconds
*/
FLOAT CGraphBodyPos::getTimeValAt(LONG _x,LONG _y)
{
	return curveBP->getTimeValAt(_x,_y); 
}

CEvnt *CGraphBodyPos::findEvent(int _what,FLOAT _atTime,int *_closestEdge)
{
	ASSERT(curveBP);
	return curveBP->findEvent(_what,_atTime,_closestEdge);
}


CString CGraphBodyPos::getTimeAndAmplAt(LONG _x,LONG _y)
{
	D2D1_POINT_2F p = curveBP->getTimeAndAmplAt(_x,_y); 
	CString st = curveBP->gethms(p.x);
	
	CString pos;
	short bp = (short) p.y;
	int nums;
	switch (bp) {
	case posLeft:
		nums = pos.LoadString(IDS_LEFT2);
		break;
	case posRight:
		nums = pos.LoadString(IDS_RIGHT2);
		break;
	case posProne:
		nums = pos.LoadString(IDS_PRONE2);
		break;
	case posSupine:
		nums = pos.LoadString(IDS_SUPINE2);
		break;
	case posUpright:
		nums = pos.LoadString(IDS_STANDING2);
		break;
	case posUndefined:
		nums = pos.LoadString(IDS_UNDEFINED2);
		break;
	default:
		break;
	}
	st += _T(" ");
	st += pos;
	CString s;
	nums = s.LoadString(IDS_POSITION);
	st += _T(" ");
	st += s;
	return st;
}

void CGraphBodyPos::setEventsToShow(UINT _evToShow, bool _onlyShowEvents /*= false*/)
{
	eventsToShow = _evToShow;
	drawOnlyEvents = _onlyShowEvents;
	curveBP->setEventsToShow(_evToShow);
	InvalidateRect(NULL,TRUE);
	UpdateWindow();
}

BOOL CGraphBodyPos::Create(LPCTSTR lpszCaption, DWORD dwStyle, const RECT& rect, CWnd* pParentWnd, UINT nID)
{
	int ret = CGraphBtnChannel::Create(lpszCaption, dwStyle, rect, pParentWnd, nID);

	int nums;
	if (1 == ret) {
		CString r,l,p,s,standing;
		nums = standing.LoadString(IDS_STANDING2);
		nums = l.LoadString(IDS_LEFT2);
		nums = r.LoadStringW(IDS_RIGHT2);
		nums = s.LoadStringW(IDS_SUPINE2);
		nums = p.LoadStringW(IDS_PRONE2);
		axes->setLabelTexts(s,l,r,p,standing);
	}
	return ret;
}

/*
Description: Check for data
Returns: true if data are present, false if not
*/
bool CGraphBodyPos::getHasData(void)
{
	if (!dataSet) return false;
	return dataSet->getSize() ? true : false;
}

void CGraphBodyPos::setData(CBodyPosDataSet *_dataSet)
{
	dataSet = _dataSet;
	ASSERT(dataSet);
	setData(dataSet->getBpVector(),dataSet->getTimeaxisVector());
}


void CGraphBodyPos::setData(vector <FLOAT> *_bp,vector <FLOAT> *_tv)
{
	ASSERT(curveBP);
	curveBP->setData(_bp,_tv,dataSet->getPenDownVector());
}

void CGraphBodyPos::setEvents(CEvents *_eP)
{
	eventsP = _eP;

	ASSERT(curveBP);
	curveBP->setEvents(_eP);
}

bool CGraphBodyPos::isOverEvent(D2D1_POINT_2F _p,UINT _evntType,int *_what,int *_where)
{
	return curveBP->isOverEvent(_p,_evntType,_what,_where);
}

void CGraphBodyPos::createCurveSettings(CButtonPlotSettings *_plotSettings)
{
	curveBP = new CRGLGraph(BODY_POSITION);
	
	ASSERT(curveBP);

	_plotSettings->setCurveTitle(BODY_POSITION,title);

	curveBP ->setTile(plotTile);	
	curveBP ->setThickness(_plotSettings->getCurveThickness(regSubKey));

	addLayer(curveBP);
	
	float from = - (float) (posSupine + posLeft) / 2.0f;
	float to = (float) (posSupine + posLeft) / 2.0f;
	CRGLBand *bP = new CRGLBand(from,to,(float) bodyPositionColoursPosition[posSupine][0] / 255.0f,
						(float) bodyPositionColoursPosition[posSupine][1] / 255.0f,
						(float) bodyPositionColoursPosition[posSupine][2] / 255.0f,.3f);
	curveBP->addBand(bP);
	from = to;
	to = (float) (posLeft + posRight) / 2.0f;
	bP = new CRGLBand(from,to,(float) bodyPositionColoursPosition[posLeft][0] / 255.0f,
						(float) bodyPositionColoursPosition[posLeft][1] / 255.0f,
						(float) bodyPositionColoursPosition[posLeft][2] / 255.0f,.3f);
	curveBP->addBand(bP);
	from = to;
	to = (float) (posRight + posProne) / 2.0f;
	bP = new CRGLBand(from,to,(float) bodyPositionColoursPosition[posRight][0] / 255.0f,
						(float) bodyPositionColoursPosition[posRight][1] / 255.0f,
						(float) bodyPositionColoursPosition[posRight][2] / 255.0f,.3f);
	curveBP->addBand(bP);
	from = to;
	to = (float) (posProne + posUpright) / 2.0f;
	bP = new CRGLBand(from,to,(float) bodyPositionColoursPosition[posProne][0] / 255.0f,
						(float) bodyPositionColoursPosition[posProne][1] / 255.0f,
						(float) bodyPositionColoursPosition[posProne][2] / 255.0f,.3f); 
	curveBP->addBand(bP);
	from = to;
	to = (float) (posUpright + posUndefined) / 2.0f;
	bP = new CRGLBand(from,to,(float) bodyPositionColoursPosition[posUpright][0] / 255.0f,
						(float) bodyPositionColoursPosition[posUpright][1] / 255.0f,
						(float) bodyPositionColoursPosition[posUpright][2] / 255.0f,.3f); 
	curveBP->addBand(bP);

	curveBP->setColour(_plotSettings->getCurveColour(regSubKey));
	curveBP->setTransparencyPerc(_plotSettings->getCurveTransparencyPerc(regSubKey));
	curveBP->setShow(true);

	CRGLText *graphTexts = new CRGLText();
	graphTexts->setTile(plotTile);
	
	graphTexts->setTexts(timeAxisLabel,_T(""),_T(""),title);
	addLayer(graphTexts);
}

float CGraphBodyPos::getYAutoMin(void)
{
	return DEFAULT_BODY_POS_MIN;    // We want supine above lower frame
}

float CGraphBodyPos::getYAutoMax(void)
{
	return DEFAULT_BODY_POS_MAX;  // Always 6
}


///////////////////////////////////////////////////////////////////////

	
CGraphBtnAirMicAll::CGraphBtnAirMicAll()
{
	dataSet = NULL;
	regSubKey = AIRBORNE_SOUND;
	curve0 = NULL;
	curve1 = NULL;
	curve2 = NULL;
	curve3 = NULL;
	YshiftIsAllowed = true;
	int nums = title.LoadString(IDS_AIRMIC_TITLE2);

	nums = st0.LoadString(IDS_160_Hz);
	nums = st1.LoadString(IDS_400_Hz);
	nums = st2.LoadString(IDS_1000_Hz);
	nums = st3.LoadString(IDS_2500_Hz);
}

CGraphBtnAirMicAll::~CGraphBtnAirMicAll()
{
}

/*
Description: Get amplitude in engineering units
*/
bool CGraphBtnAirMicAll::getEUAmplAt(LONG _x, LONG _y, CString *_s)
{
	CString s0 = curve0->getAmplAt(_x, _y);
	CString s1 = curve1->getAmplAt(_x, _y);
	CString s2 = curve2->getAmplAt(_x, _y);
	CString s3 = curve3->getAmplAt(_x, _y);
	*_s = st0;
	*_s += _T(": ");
	*_s += s0;
	*_s += _T("\n"); 
	*_s += st1;
	*_s += _T(": ");
	*_s += s1;
	*_s += _T("\n");
	*_s += st2;
	*_s += _T(": ");
	*_s += s2;
	*_s += _T("\n");
	*_s += st3;
	*_s += _T(": ");
	*_s += s3;
	
	return true;
}


void CGraphBtnAirMicAll::updateGraphSettings(void)
{
	CGraphBtnChannel::updateGraphSettings();

	//---Additionals here
	CButtonPlotSettings plotSettings(dualPolarity, regSubKey);
	curve0->setColour(plotSettings.getCurveColour(AIRBORNE_SOUND_F0));
	curve0->setTransparencyPerc(plotSettings.getCurveTransparencyPerc(AIRBORNE_SOUND_F0));
	curve0->setThickness(plotSettings.getCurveThickness(AIRBORNE_SOUND_F0));

	curve1->setColour(plotSettings.getCurveColour(AIRBORNE_SOUND_F1));
	curve1->setTransparencyPerc(plotSettings.getCurveTransparencyPerc(AIRBORNE_SOUND_F1));
	curve1->setThickness(plotSettings.getCurveThickness(AIRBORNE_SOUND_F1));

	curve2->setColour(plotSettings.getCurveColour(AIRBORNE_SOUND_F2));
	curve2->setTransparencyPerc(plotSettings.getCurveTransparencyPerc(AIRBORNE_SOUND_F2));
	curve2->setThickness(plotSettings.getCurveThickness(AIRBORNE_SOUND_F2));

	curve3->setColour(plotSettings.getCurveColour(AIRBORNE_SOUND_F3));
	curve3->setTransparencyPerc(plotSettings.getCurveTransparencyPerc(AIRBORNE_SOUND_F3));
	curve3->setThickness(plotSettings.getCurveThickness(AIRBORNE_SOUND_F3));

	render();
}

void CGraphBtnAirMicAll::updateCurveColour(void)
{
	curve0->updateBrush(AIRBORNE_SOUND);
	curve1->updateBrush(AIRBORNE_SOUND);
	curve2->updateBrush(AIRBORNE_SOUND);
	curve3->updateBrush(AIRBORNE_SOUND);
}

CString CGraphBtnAirMicAll::getTimeAndAmplAt(LONG _x,LONG _y)
{
	D2D1_POINT_2F dt = curve0->getTimeAndAmplAt(_x,_y);

	float a0;
	bool ok = curve0->getAmplAt(dt.x,&a0);
	if (!ok) return curve0->gethms(dt.x);
	
	float a1 = .0f;
	float a2 = .0f;
	float a3 = .0f;
	ok = curve1->getAmplAt(dt.x,&a1);
	ok = curve2->getAmplAt(dt.x,&a2);
	ok = curve3->getAmplAt(dt.x,&a3);
	
	CString s;
	s.Format(_T("%s, 160 Hz: %s, 400 Hz: %s, 1 kHz: %s, 2.5 kHz: %s"),
		static_cast<LPCTSTR>(curve0->gethms(dt.x)),
		static_cast<LPCTSTR>(curve0->getFormattedNumber(a0)),
		static_cast<LPCTSTR>(curve1->getFormattedNumber(a1)),
		static_cast<LPCTSTR>(curve2->getFormattedNumber(a2)),
		static_cast<LPCTSTR>(curve3->getFormattedNumber(a3)));
	return s; 
}


void CGraphBtnAirMicAll::setEventsToShow(UINT _evToShow, bool _onlyShowEvents /*= false*/)
{
	eventsToShow = _evToShow;
	drawOnlyEvents = _onlyShowEvents;
	curve0->setEventsToShow(_evToShow);
	InvalidateRect(NULL,TRUE);
	UpdateWindow();
}
/*
Description: Check for data
Returns: true if data are present, false if not
*/
bool CGraphBtnAirMicAll::getHasData(void)
{
	if (!dataSet) return false;
	return dataSet->getSize() ? true : false;
}

void CGraphBtnAirMicAll::setData(CAirMicDataSet *_dataSet)
{
	dataSet = _dataSet;
	ASSERT(dataSet);
	setData(dataSet->getAm1Vector(),dataSet->getAm2Vector(),
		dataSet->getAm3Vector(),dataSet->getAm4Vector(),dataSet->getTimeaxisVector());
}

void CGraphBtnAirMicAll::setData(	vector <FLOAT> *_c0,
								vector <FLOAT> *_c1,
								vector <FLOAT> *_c2,
								vector <FLOAT> *_c3,
								vector <FLOAT> *_tv)
{
	ASSERT(curve0);
	ASSERT(curve1);
	ASSERT(curve2);
	ASSERT(curve3);
	curve0->setData(_c0,_tv);
	curve1->setData(_c1,_tv);
	curve2->setData(_c2,_tv);
	curve3->setData(_c3,_tv);
}


void CGraphBtnAirMicAll::setEvents(CEvents *_eP)
{
	eventsP = _eP;

	ASSERT(curve0);
	ASSERT(curve1);
	ASSERT(curve2);
	ASSERT(curve3);
	curve0->setEvents(_eP);
	curve1->setEvents(_eP);
	curve2->setEvents(_eP);
	curve3->setEvents(_eP);
}

BOOL CGraphBtnAirMicAll::Create(LPCTSTR lpszCaption, DWORD dwStyle, const RECT& rect, CWnd* pParentWnd, UINT nID)
{
	// TODO: Add your specialized code here and/or call the base class
	int ret = CGraphButton::Create(lpszCaption, dwStyle, rect, pParentWnd, nID);

	if (1 == ret) {
		CButtonPlotSettings plotSettings(dualPolarity, regSubKey, title);

		backgr = new CRGLBackground();
		backgr->setColour(plotSettings.getBackgrColour());
		addLayer(backgr);
		if (inTiledView) {
			plotTile = new CRGLTile(
				D2D1::RectF(.0f, 100.0f, 100.0f, .0f),     // %
				D2D1::RectF(5.0f, 99.0f, 99.0f, 1.0f),    // %  
				D2D1::RectF(.0f, 1.0f, 1.0f, .0f),         // Prelim
				D2D1::RectF(.0f, 1.0f, 1.0f, .0f));        // Prelim
		}
		else {
			plotTile = new CRGLTile();
		}
		setPlotTileYLimits();
		addTile(plotTile);

		//--Object specific
		CArray <CCurvePlotSettings *,CCurvePlotSettings*> *pset = plotSettings.getCurves();
		int num = pset->GetCount();
		for (int i = 0; i < num; i++) {
			CCurvePlotSettings *psetI = pset->GetAt(i);
			if (AIRBORNE_SOUND_F0 == psetI->getNameInReg()) {
				curve0 = new CRGLGraphSparse(AIRBORNE_SOUND_F0);
				ASSERT(curve0);

				CString cTitle0;
				int nums = cTitle0.LoadString(IDS_AIRBORNE_F0);
				curve0->setTile(plotTile);
				curve0->setThickness((float) psetI->getThickness());
				addLayer(curve0);
				curve0->setColour(psetI->getColour());
				curve0->setTransparencyPerc(psetI->getTransparencyPerc());
				curve0->setShow(true);
			}
			else if (AIRBORNE_SOUND_F1 == psetI->getNameInReg()) {
				curve1 = new CRGLGraphSparse(AIRBORNE_SOUND_F1,true);
				ASSERT(curve1);

				CString cTitle1;
				int nums = cTitle1.LoadString(IDS_AIRBORNE_F1);
				curve1->setTile(plotTile);
				curve1->setThickness((float) psetI->getThickness());
				addLayer(curve1);
				curve1->setColour(psetI->getColour());
				curve1->setTransparencyPerc(psetI->getTransparencyPerc());
				curve1->setShow(true);
			}
			else if (AIRBORNE_SOUND_F2 == psetI->getNameInReg()) {
				curve2 = new CRGLGraphSparse(AIRBORNE_SOUND_F2, true);
				ASSERT(curve2);

				CString cTitle2;
				int nums = cTitle2.LoadString(IDS_AIRBORNE_F2);
				curve2->setTile(plotTile);
				curve2->setThickness((float) psetI->getThickness());
				addLayer(curve2);
				curve2->setColour(psetI->getColour());
				curve2->setTransparencyPerc(psetI->getTransparencyPerc());
				curve2->setShow(true);
			}
			else if (AIRBORNE_SOUND_F3 == psetI->getNameInReg()) {
				curve3 = new CRGLGraphSparse(AIRBORNE_SOUND_F3, true);
				ASSERT(curve3);

				CString cTitle3;
				int nums = cTitle3.LoadString(IDS_AIRBORNE_F3);
				curve3->setTile(plotTile);
				curve3->setThickness((float) psetI->getThickness());
				addLayer(curve3);
				curve3->setColour(psetI->getColour());
				curve3->setTransparencyPerc(psetI->getTransparencyPerc());
				curve3->setShow(true);
			}
		}
		CRGLText *graphTexts = new CRGLText();
		graphTexts->setTile(plotTile);

		CString ys;
		ys = _T("");
		graphTexts->setTexts(timeAxisLabel, _T(""), ys, title);
		addLayer(graphTexts);
		//createCurveSettings(&plotSettings);

		plotTile->setPlotRectYScaled(plotSettings.getYmin(), plotSettings.getYmax());
		runYAxisWizard(plotSettings.getYmin(), plotSettings.getYmax());
		plotTile->setPlotRectXScaled(plotSettings.getXmin(), plotSettings.getXmax());

		axes = new CRGLAxes();
		axes->setTile(plotTile);
		axes->setShowXlabels(inTiledView ? false : true);
		axes->setShowYlabels(true);
		axes->setShowTicks(true);
		axes->setYTick(plotSettings.getYtick());
		axes->setYlabelInterval(plotSettings.getYlabelInterval());
		axes->setXlabelInterval(plotSettings.getXlabelInterval());
		axes->setXTick(plotSettings.getXtick());
		addLayer(axes);
		runXAxisWizard(plotSettings.getXmin(), plotSettings.getXmax());
	}

	return ret;
	//return CGraphButton::Create(lpszCaption, dwStyle, rect, pParentWnd, nID);
}

//void CGraphBtnAirMic::createCurveSettings(CButtonPlotSettings *_plotSettings)
//{
//	curve0 = new CRGLGraph(AIRBORNE_SOUND_F0);
//	curve1 = new CRGLGraph(AIRBORNE_SOUND_F1,true);
//	curve2 = new CRGLGraph(AIRBORNE_SOUND_F2,true);
//	curve3 = new CRGLGraph(AIRBORNE_SOUND_F3,true);
//	
//	ASSERT(curve0);
//	ASSERT(curve1);
//	ASSERT(curve2);
//	ASSERT(curve3);
//
//	CString cTitle0;
//	cTitle0.LoadString(IDS_AIRBORNE_F0);
//	_plotSettings->setCurveTitle(AIRBORNE_SOUND_F0,cTitle0);
//
//	curve0->setTile(plotTile);	
//	curve0->setThickness(_plotSettings->getCurveThickness(AIRBORNE_SOUND_F0));
//		 
//	addLayer(curve0);
//	curve0->setColour(_plotSettings->getCurveColour(AIRBORNE_SOUND_F0));
//	curve0->setTransparencyPerc(_plotSettings->getCurveTransparencyPerc(AIRBORNE_SOUND_F0));
//	curve0->setShow(true);
//
//	CString cTitle1;
//	cTitle1.LoadString(IDS_AIRBORNE_F1);
//	_plotSettings->setCurveTitle(AIRBORNE_SOUND_F1,cTitle1);
//
//	curve1 ->setTile(plotTile);	
//	curve1 ->setThickness(_plotSettings->getCurveThickness(AIRBORNE_SOUND_F1));
//		
//	addLayer(curve1);
//	curve1->setColour(_plotSettings->getCurveColour(AIRBORNE_SOUND_F1));
//	curve1->setTransparencyPerc(_plotSettings->getCurveTransparencyPerc(AIRBORNE_SOUND_F1));
//	curve1->setShow(true);
//
//	CString cTitle2;
//	cTitle2.LoadString(IDS_AIRBORNE_F2);
//	_plotSettings->setCurveTitle(AIRBORNE_SOUND_F2,cTitle2);
//
//	curve2 ->setTile(plotTile);	
//	curve2 ->setThickness(_plotSettings->getCurveThickness(AIRBORNE_SOUND_F2));
//
//	addLayer(curve2);
//	curve2->setColour(_plotSettings->getCurveColour(AIRBORNE_SOUND_F2));
//	curve2->setTransparencyPerc(_plotSettings->getCurveTransparencyPerc(AIRBORNE_SOUND_F2));
//	curve2->setShow(true);
//
//	CString cTitle3;
//	cTitle3.LoadString(IDS_AIRBORNE_F3);
//	_plotSettings->setCurveTitle(AIRBORNE_SOUND_F3,cTitle3);
//
//	curve3 ->setTile(plotTile);	
//	curve3 ->setThickness(_plotSettings->getCurveThickness(AIRBORNE_SOUND_F3));
//
//	addLayer(curve3);
//	curve3->setColour(_plotSettings->getCurveColour(AIRBORNE_SOUND_F3));
//	curve3->setTransparencyPerc(_plotSettings->getCurveTransparencyPerc(AIRBORNE_SOUND_F3));
//	curve3->setShow(true);
//	
//	CRGLText *graphTexts = new CRGLText();
//	graphTexts->setTile(plotTile);
//
//	CString ys;
//	ys = _T("");
//	graphTexts->setTexts(timeAxisLabel,_T(""),ys,title);
//	addLayer(graphTexts);
//}

float CGraphBtnAirMicAll::getYAutoMax(void)
{
	return DEFAULT_AIRMIC_MAX;

	/*FLOAT max1,avg1,max2,avg2,max3,avg3,max4,avg4;
	ASSERT(dataSet);
	
	max1 = dataSet->getAm1Vector()->size() > 0 ? 
		*max_element(dataSet->getAm1Vector()->begin(), dataSet->getAm1Vector()->end()) :
	-	.0f;
	avg1 = dataSet->getAm1Vector()->size() > 0 ? 
		CDataSet::getMean(dataSet->getAm1Vector()->begin(),dataSet->getAm1Vector()->end()) :
		.0f;

	max2 = dataSet->getAm2Vector()->size() > 0 ? 
		*max_element(dataSet->getAm2Vector()->begin(), dataSet->getAm2Vector()->end()) :
		.0f;
	avg2 = dataSet->getAm2Vector()->size() ? 
		CDataSet::getMean(dataSet->getAm2Vector()->begin(),dataSet->getAm2Vector()->end()) :
		.0f;

	max3 = dataSet->getAm3Vector()->size() > 0 ? 
		*max_element(dataSet->getAm3Vector()->begin(), dataSet->getAm3Vector()->end()) : 
		.0f;
	avg3 = dataSet->getAm3Vector()->size() > 0 ? 
		CDataSet::getMean(dataSet->getAm3Vector()->begin(),dataSet->getAm3Vector()->end()) :
		.0f;

	max4 = dataSet->getAm4Vector()->size() > 0 ? 
		*max_element(dataSet->getAm4Vector()->begin(), dataSet->getAm4Vector()->end()) :
		.0f;
	avg4 = dataSet->getAm4Vector()->size() > 0 ? 
		CDataSet::getMean(dataSet->getAm4Vector()->begin(),dataSet->getAm4Vector()->end()) :
		.0f;
	
	FLOAT avg = avg1;
	avg = avg1 > avg ? avg1 : avg;
	avg = avg2 > avg ? avg2 : avg;
	avg = avg3 > avg ? avg3 : avg;
	avg = avg4 > avg ? avg4 : avg;

	FLOAT max = max1;
	max = max1 > max ? max1 : max;
	max = max2 > max ? max2 : max;
	max = max3 > max ? max3 : max;
	max = max4 > max ? max4 : max;

	return getMaxYScaleFromMeanAndMax(avg,max);*/
}



///////////////////////////////////////////////////////////////////////

	
CGraphBtnPulseRate::CGraphBtnPulseRate()
{
	dataSet = NULL;
	regSubKey = PULSE_RATE;
	curvePR = NULL;
	YshiftIsAllowed = true;
	int nums = title.LoadString(IDS_PULSE_RATE_TITLE2);
}

CGraphBtnPulseRate::~CGraphBtnPulseRate()
{
}

void CGraphBtnPulseRate::updateGraphSettings(void)
{
	CGraphBtnChannel::updateGraphSettings();

	//---Additionals here
	CButtonPlotSettings plotSettings(dualPolarity, regSubKey);
	curvePR->setColour(plotSettings.getCurveColour(PULSE_RATE));
	curvePR->setTransparencyPerc(plotSettings.getCurveTransparencyPerc(PULSE_RATE));
	curvePR->setThickness(plotSettings.getCurveThickness(PULSE_RATE));

	render();
}

void CGraphBtnPulseRate::updateCurveColour(void)
{
	curvePR->updateBrush();
}

vector<BOOL>* CGraphBtnPulseRate::getPenDownVector(void)
{
	return dataSet->getPenDownVector();
}

CString CGraphBtnPulseRate::getTimeAndAmplAt(LONG _x,LONG _y)
{
	float time = curvePR->getTheTimeAtPixel(_x);
	if (!dataSet->getIsPenDown(time)) return _T("");
	else return curvePR->getStringTimeAndAmplAt(_x,_y,_T("BPM")); 
}

void CGraphBtnPulseRate::setPlotTileYLimits(void)
{
	if (!plotTile) return;
	plotTile->setVertScaleLimits(MIN_SCALE_PULSE_RATE,MAX_SCALE_PULSE_RATE);
}

void CGraphBtnPulseRate::setEventsToShow(UINT _evToShow, bool _onlyShowEvents /*= false*/)
{
	eventsToShow = _evToShow;
	drawOnlyEvents = _onlyShowEvents;
	curvePR->setEventsToShow(_evToShow);
	InvalidateRect(NULL,TRUE);
	UpdateWindow();
}

/*
Description: Check for data
Returns: true if data are present, false if not
*/
bool CGraphBtnPulseRate::getHasData(void)
{
	if (!dataSet) return false;
	return dataSet->getSize() ? true : false;
}

void CGraphBtnPulseRate::setData(COximeterDataSet *_dataSet)
{
	dataSet = _dataSet;
	ASSERT(dataSet);
	setData(dataSet->getPulseRateVector(),dataSet->getTimeaxisVector());
}

void CGraphBtnPulseRate::setData(vector <FLOAT> *_pr,vector <FLOAT> *_tv)
{
	ASSERT(curvePR);
	curvePR->setData(_pr,_tv,dataSet->getPenDownVector());
}

void CGraphBtnPulseRate::setEvents(CEvents *_eP)
{
	eventsP = _eP;

	ASSERT(curvePR);
	curvePR->setEvents(_eP);
}


void CGraphBtnPulseRate::createCurveSettings(CButtonPlotSettings *_plotSettings)
{
	curvePR = new CRGLGraphSparse(PULSE_RATE);

	ASSERT(curvePR);

	CString cTitle;
	int nums = cTitle.LoadString(IDS_PR);
	_plotSettings->setCurveTitle(PULSE_RATE,cTitle);

	curvePR->setTile(plotTile);	
	curvePR->setThickness(_plotSettings->getCurveThickness(PULSE_RATE));

	addLayer(curvePR);
	curvePR->setColour(_plotSettings->getCurveColour(PULSE_RATE));
	curvePR->setTransparencyPerc(_plotSettings->getCurveTransparencyPerc(PULSE_RATE));
	curvePR->setShow(true);
	
	CRGLText *graphTexts = new CRGLText();
	graphTexts->setTile(plotTile);

	CString ys;
	nums = ys.LoadStringW(IDS_BPM2);
	graphTexts->setTexts(timeAxisLabel,_T(""),ys,title);
	addLayer(graphTexts);
}

float CGraphBtnPulseRate::getYAutoMin(void)
{
	return DEFAULT_PR_MIN;

	/*FLOAT min;
	ASSERT(dataSet);
	
	min = dataSet->getPulseRateVector()->size() > 0 ? 
		*min_element(dataSet->getPulseRateVector()->begin(), dataSet->getPulseRateVector()->end()) :
		.0f;
	if (min < 10.0f) return .0f;
	if (min < 20.0f) return 10.0f;
	if (min < 30.0f) return 20.0f;
	if (min < 40.0f) return 30.0f;
	if (min < 50.0f) return 40.0f;
	if (min < 60.0f) return 50.0f;
	return 60.0f;*/
}

float CGraphBtnPulseRate::getYAutoMax(void)
{
	return DEFAULT_PR_MAX;

	/*FLOAT max,avg;
	ASSERT(dataSet);
	
	max = dataSet->getPulseRateVector()->size() > 0 ? 
		*max_element(dataSet->getPulseRateVector()->begin(), dataSet->getPulseRateVector()->end()) :
		.0f; 
	avg = dataSet->getPulseRateVector()->size() > 0 ? 
		CDataSet::getMean(dataSet->getPulseRateVector()->begin(),dataSet->getPulseRateVector()->end()) :
		.0f;
	
	return getMaxYScaleFromMeanAndMax(avg,max);*/
}

///////////////////////////////////////////////////////////////////////
	
CGraphBtnSpO2::CGraphBtnSpO2()
{
	dataSet = NULL;
	regSubKey = SpO2;
	curveSpO2 = NULL;
	YshiftIsAllowed = false;
	YscaleIsAllowed = false;
	
	lockTopOfYAxis = true;

	int nums = title.LoadString(IDS_SpO2_TITLE2);
}

CGraphBtnSpO2::~CGraphBtnSpO2()
{
}

void CGraphBtnSpO2::updateGraphSettings(void)
{
	CGraphBtnChannel::updateGraphSettings();

	//---Additionals here
	CButtonPlotSettings plotSettings(dualPolarity, regSubKey);
	curveSpO2->setColour(plotSettings.getCurveColour(regSubKey));
	curveSpO2->setTransparencyPerc(plotSettings.getCurveTransparencyPerc(regSubKey));
	curveSpO2->setThickness(plotSettings.getCurveThickness(regSubKey));

	render();
}

void CGraphBtnSpO2::updateCurveColour(void)
{
	curveSpO2->updateBrush();
}

vector<BOOL>* CGraphBtnSpO2::getPenDownVector(void)
{
	return dataSet->getPenDownVector();
}

CString CGraphBtnSpO2::getTimeAndAmplAt(LONG _x,LONG _y)
{
	float time = curveSpO2->getTheTimeAtPixel(_x);
	if (!dataSet->getIsPenDown(time)) return _T("");
	else return curveSpO2->getStringTimeAndAmplAt(_x,_y,_T("%")); 
}

void CGraphBtnSpO2::setEventsToShow(UINT _evToShow, bool _onlyShowEvents /*= false*/)
{
	eventsToShow = _evToShow;
	drawOnlyEvents = _onlyShowEvents;
	curveSpO2->setEventsToShow(_evToShow);
	InvalidateRect(NULL,TRUE);
	UpdateWindow();
}

/*
Description: Check for data
Returns: true if data are present, false if not
*/
bool CGraphBtnSpO2::getHasData(void)
{
	if (!dataSet) return false;
	return dataSet->getSize() ? true : false;
}

void CGraphBtnSpO2::setData(COximeterDataSet *_dataSet)
{
	dataSet = _dataSet;
	ASSERT(dataSet);
	setData(dataSet->getSpO2Vector(),dataSet->getTimeaxisVector());
}

void CGraphBtnSpO2::setData(vector <FLOAT> *_spO2,vector <FLOAT> *_tv)
{
	ASSERT(curveSpO2);
	curveSpO2->setData(_spO2,_tv,dataSet->getPenDownVector());
}

void CGraphBtnSpO2::setEvents(CEvents *_eP)
{
	eventsP = _eP;

	ASSERT(curveSpO2);
	curveSpO2->setEvents(_eP);
}

void CGraphBtnSpO2::createCurveSettings(CButtonPlotSettings *_plotSettings)
{
	curveSpO2 = new CRGLGraphSparse(SpO2);

	ASSERT(curveSpO2);

	CString cTitle;
	int nums = cTitle.LoadString(IDS_SpO2);
	_plotSettings->setCurveTitle(SpO2,cTitle);

	curveSpO2->setTile(plotTile);	
	curveSpO2->setThickness(_plotSettings->getCurveThickness(SpO2));

	addLayer(curveSpO2);
	curveSpO2->setColour(_plotSettings->getCurveColour(SpO2));
	curveSpO2->setTransparencyPerc(_plotSettings->getCurveTransparencyPerc(SpO2));
	curveSpO2->setShow(true);
	
	CRGLText *graphTexts = new CRGLText();
	graphTexts->setTile(plotTile);

	CString ys;
	nums = ys.LoadStringW(IDS_PERCENT);
	graphTexts->setTexts(timeAxisLabel,_T(""),ys,title);
	addLayer(graphTexts);
}

float CGraphBtnSpO2::getYAutoMin(void)
{
	return DEFAULT_SPO2_MIN;

	/*FLOAT min;
	ASSERT(dataSet);
	
	min = dataSet->getSpO2Vector()->size() > 0 ? 
		*min_element(dataSet->getSpO2Vector()->begin(), dataSet->getSpO2Vector()->end()) : .0f;
	if (min < 10.0f) return .0f;
	if (min < 20.0f) return 10.0f;
	if (min < 30.0f) return 20.0f;
	if (min < 40.0f) return 30.0f;
	if (min < 50.0f) return 40.0f;
	if (min < 60.0f) return 50.0f;
	return 60.0f;*/
}

float CGraphBtnSpO2::getYAutoMax(void)
{	
	return DEFAULT_SPO2_MAX;
}

///////////////////////////////////////////////////////////////////////
	
CGraphBtnCatheterPressure::CGraphBtnCatheterPressure()
{
	dataSet = NULL;
	dualPolarity = true;
}

CGraphBtnCatheterPressure::~CGraphBtnCatheterPressure()
{
}

void CGraphBtnCatheterPressure::updateGraphSettings(void)
{
	CGraphBtnChannel::updateGraphSettings();
}



/*
Description: Check for data
Returns: true if data are present, false if not
*/
bool CGraphBtnCatheterPressure::getHasData(void)
{
	if (!dataSet) return false;
	return dataSet->getSize() ? true : false;
}

void CGraphBtnCatheterPressure::setData(CCatheterDataSet *_dataSet)
{
}

void CGraphBtnCatheterPressure::setData(vector <FLOAT> *_p,vector <FLOAT> *_tv)
{
}

void CGraphBtnCatheterPressure::setEvents(CEvents *_eP)
{
	eventsP = _eP;
}

void CGraphBtnCatheterPressure::createCurveSettings(CButtonPlotSettings *_plotSettings)
{
}

float CGraphBtnCatheterPressure::getYAutoMax(void)
{
	return DEFAULT_CATH_PRESS_MAX;
}

///////////////////////////////////////////////////////////////////////
	
CGraphBtnCatheterPressurePOES::CGraphBtnCatheterPressurePOES()
{
	dataSet = NULL;
	regSubKey = PRESSURE_OESOPH;
	curvePOES = NULL;
	int nums = title.LoadString(IDS_POES_TITLE);
}

CGraphBtnCatheterPressurePOES::~CGraphBtnCatheterPressurePOES()
{
}

void CGraphBtnCatheterPressurePOES::updateGraphSettings(void)
{
	CGraphBtnCatheterPressure::updateGraphSettings();

	//---Additionals here
	CButtonPlotSettings plotSettings(dualPolarity, regSubKey);
	curvePOES->setColour(plotSettings.getCurveColour(regSubKey));
	curvePOES->setTransparencyPerc(plotSettings.getCurveTransparencyPerc(regSubKey));
	curvePOES->setThickness(plotSettings.getCurveThickness(regSubKey));

	render();
}

void CGraphBtnCatheterPressurePOES::updateCurveColour(void)
{
	curvePOES->updateBrush();
}

CString CGraphBtnCatheterPressurePOES::getTimeAndAmplAt(LONG _x,LONG _y)
{
	return curvePOES->getStringTimeAndAmplAt(_x,_y,_T("cmH2O")); 
}

void CGraphBtnCatheterPressurePOES::setEventsToShow(UINT _evToShow, bool _onlyShowEvents /*= false*/)
{
	eventsToShow = _evToShow;
	drawOnlyEvents = _onlyShowEvents;
	curvePOES->setEventsToShow(_evToShow);
	InvalidateRect(NULL,TRUE);
	UpdateWindow();
}

/*
Description: Check for data
Returns: true if data are present, false if not
*/
bool CGraphBtnCatheterPressurePOES::getHasData(void)
{
	if (!dataSet) return false;
	return dataSet->getSize() ? true : false;
}

void CGraphBtnCatheterPressurePOES::setData(CCatheterDataSet *_dataSet)
{
	dataSet = _dataSet;
	ASSERT(dataSet);
	setData(dataSet->getPOESVector(),dataSet->getTimeaxisVector());
}

void CGraphBtnCatheterPressurePOES::setData(vector <FLOAT> *_p,vector <FLOAT> *_tv)
{
	ASSERT(curvePOES);
	curvePOES->setData(_p,_tv);
}

void CGraphBtnCatheterPressurePOES::setEvents(CEvents *_eP)
{
	eventsP = _eP;

	ASSERT(curvePOES);
	curvePOES->setEvents(_eP);
}

void CGraphBtnCatheterPressurePOES::createCurveSettings(CButtonPlotSettings *_plotSettings)
{
	curvePOES = new CRGLGraphSparse(PRESSURE_OESOPH);

	ASSERT(curvePOES);

	_plotSettings->setCurveTitle(regSubKey,title);

	plotTile->setMinimumYRange(minimumPressureYRange);

	curvePOES->setTile(plotTile);	
	curvePOES->setThickness(_plotSettings->getCurveThickness(regSubKey));

	addLayer(curvePOES);
	curvePOES->setColour(_plotSettings->getCurveColour(regSubKey));
	curvePOES->setTransparencyPerc(_plotSettings->getCurveTransparencyPerc(regSubKey));
	curvePOES->setShow(true);
	
	CRGLText *graphTexts = new CRGLText();
	graphTexts->setTile(plotTile);

	CString ys;
	int nums = ys.LoadString(IDS_cmH2O);
	graphTexts->setTexts(timeAxisLabel,_T(""),ys,title);
	addLayer(graphTexts);
}

float CGraphBtnCatheterPressurePOES::getYAutoMax(void)
{
	return DEFAULT_CATH_PRESS_MAX;

	/*ASSERT(dataSet);
	
	FLOAT max = dataSet->getPOESVector()->size() > 0 ? 
		*max_element(dataSet->getPOESVector()->begin(), dataSet->getPOESVector()->end()) : .0f ; 
	FLOAT min = dataSet->getPOESVector()->size() > 0 ? 
		*min_element(dataSet->getPOESVector()->begin(), dataSet->getPOESVector()->end()) : .0f ; 
	FLOAT avg = dataSet->getPOESVector()->size() > 0 ? 
		CDataSet::getMean(dataSet->getPOESVector()->begin(),dataSet->getPOESVector()->end()) : .0f;
	
	FLOAT pos = fabs(max);
	FLOAT neg = fabs(min);
	return getMaxYScaleFromMeanAndMax(avg,pos > neg ? pos : neg);*/
}

///////////////////////////////////////////////////////////////////////
	
CGraphBtnCatheterPressurePPH::CGraphBtnCatheterPressurePPH()
{
	dataSet = NULL;
	regSubKey = PRESSURE_PPHAR;
	curvePPH = NULL;
	int nums = title.LoadString(IDS_PPH_TITLE);
}

CGraphBtnCatheterPressurePPH::~CGraphBtnCatheterPressurePPH()
{
}

void CGraphBtnCatheterPressurePPH::updateGraphSettings(void)
{
	CGraphBtnCatheterPressure::updateGraphSettings();

	//---Additionals here
	CButtonPlotSettings plotSettings(dualPolarity, regSubKey);
	curvePPH->setColour(plotSettings.getCurveColour(regSubKey));
	curvePPH->setTransparencyPerc(plotSettings.getCurveTransparencyPerc(regSubKey));
	curvePPH->setThickness(plotSettings.getCurveThickness(regSubKey));

	render();
}

void CGraphBtnCatheterPressurePPH::updateCurveColour(void)
{
	curvePPH->updateBrush();
}

CString CGraphBtnCatheterPressurePPH::getTimeAndAmplAt(LONG _x,LONG _y)
{
	return curvePPH->getStringTimeAndAmplAt(_x,_y,_T("cmH2O")); 
}

void CGraphBtnCatheterPressurePPH::setEventsToShow(UINT _evToShow, bool _onlyShowEvents /*= false*/)
{
	eventsToShow = _evToShow;
	drawOnlyEvents = _onlyShowEvents;
	curvePPH->setEventsToShow(_evToShow);
	InvalidateRect(NULL,TRUE);
	UpdateWindow();
}


/*
Description: Check for data
Returns: true if data are present, false if not
*/
bool CGraphBtnCatheterPressurePPH::getHasData(void)
{
	if (!dataSet) return false;
	return dataSet->getSize() ? true : false;
}

void CGraphBtnCatheterPressurePPH::setData(CCatheterDataSet *_dataSet)
{
	dataSet = _dataSet;
	ASSERT(dataSet);
	setData(dataSet->getPPHVector(),dataSet->getTimeaxisVector());
}

void CGraphBtnCatheterPressurePPH::setData(vector <FLOAT> *_p,vector <FLOAT> *_tv)
{
	ASSERT(curvePPH);
	curvePPH->setData(_p,_tv);
}

void CGraphBtnCatheterPressurePPH::setEvents(CEvents *_eP)
{
	eventsP = _eP;

	ASSERT(curvePPH);
	curvePPH->setEvents(_eP);
}

void CGraphBtnCatheterPressurePPH::createCurveSettings(CButtonPlotSettings *_plotSettings)
{
	curvePPH = new CRGLGraphSparse(PRESSURE_PPHAR);

	ASSERT(curvePPH);

	_plotSettings->setCurveTitle(regSubKey,title);

	curvePPH->setTile(plotTile);	
	curvePPH->setThickness(_plotSettings->getCurveThickness(regSubKey));

	addLayer(curvePPH);
	curvePPH->setColour(_plotSettings->getCurveColour(regSubKey));
	curvePPH->setTransparencyPerc(_plotSettings->getCurveTransparencyPerc(regSubKey));
	curvePPH->setShow(true);
	
	CRGLText *graphTexts = new CRGLText();
	graphTexts->setTile(plotTile);

	CString ys;
	int nums = ys.LoadString(IDS_cmH2O);
	graphTexts->setTexts(timeAxisLabel,_T(""),ys,title);
	addLayer(graphTexts);
}

float CGraphBtnCatheterPressurePPH::getYAutoMax(void)
{
	return DEFAULT_CATH_PRESS_MAX;

	/*ASSERT(dataSet);
	
	FLOAT max = dataSet->getPOESVector()->size() > 0 ? 
		*max_element(dataSet->getPOESVector()->begin(), dataSet->getPOESVector()->end()) : .0f ; 
	FLOAT min = dataSet->getPOESVector()->size() > 0 ? 
		*min_element(dataSet->getPOESVector()->begin(), dataSet->getPOESVector()->end()) : .0f ; 
	FLOAT avg = dataSet->getPOESVector()->size() > 0 ? 
		CDataSet::getMean(dataSet->getPOESVector()->begin(),dataSet->getPOESVector()->end()) : .0f ;
	
	FLOAT pos = fabs(max);
	FLOAT neg = fabs(min);
	return getMaxYScaleFromMeanAndMax(avg,pos > neg ? pos : neg);*/
}


///////////////////////////////////////////////////////////////////////
	
CGraphBtnPressGrad::CGraphBtnPressGrad()
{
	YshiftIsAllowed = false;
	YscaleIsAllowed = false;
	debugTag = 1;
	dataSet = NULL;
	regSubKey = PRESSURE_GRADIENT_RATIO;
	curvePUpperGradient = NULL;
	int nums = title.LoadString(IDS_PRESS_GRADIENT_TITLE);
	dualPolarity = false;
}

CGraphBtnPressGrad::~CGraphBtnPressGrad()
{
}

void CGraphBtnPressGrad::updateGraphSettings(void)
{
	CGraphBtnChannel::updateGraphSettings();

	//---Additionals here
	CButtonPlotSettings plotSettings(dualPolarity, regSubKey);

	curvePUpperGradient->setColour(plotSettings.getCurveColour(PRESSURE_GRADIENT_RATIO));
	curvePUpperGradient->setTransparencyPerc(plotSettings.getCurveTransparencyPerc(PRESSURE_GRADIENT_RATIO));
	curvePUpperGradient->setThickness(plotSettings.getCurveThickness(PRESSURE_GRADIENT_RATIO));

	render();
}


void CGraphBtnPressGrad::updateCurveColour(void)
{
	curvePUpperGradient->updateBrush();
}

CString CGraphBtnPressGrad::getTimeAndAmplAt(LONG _x,LONG _y)
{
	ASSERT(curvePUpperGradient);

	D2D1_POINT_2F dt = curvePUpperGradient->getTimeAndAmplAt(_x,_y);

	float upper;
	bool ok = curvePUpperGradient->getAmplAt(dt.x,&upper);
	if (!ok) return curvePUpperGradient->gethms(dt.x);
	
	float up = .0f;
	float lo = .0f;
	ok = curvePUpperGradient->getAmplAt(dt.x,&up);

	CString s;
	if (up > 100.0f) s.Format(_T("%s, > 100 %%"), static_cast<LPCTSTR>(curvePUpperGradient->gethms(dt.x)));
	else s.Format(_T("%s, %s %%"),
		static_cast<LPCTSTR>(curvePUpperGradient->gethms(dt.x)),
		static_cast<LPCTSTR>(curvePUpperGradient->getFormattedNumber(up)));
	return s; 
}

void CGraphBtnPressGrad::setEventsToShow(UINT _evToShow, bool _onlyShowEvents /*= false*/)
{
	eventsToShow = _evToShow;
	drawOnlyEvents = _onlyShowEvents;
	ASSERT(curvePUpperGradient);
	curvePUpperGradient->setEventsToShow(_evToShow);
	InvalidateRect(NULL,TRUE);
	UpdateWindow();
}


/*
Description: Check for data
Returns: true if data are present, false if not
*/
bool CGraphBtnPressGrad::getHasData(void)
{
	if (!dataSet) return false;
	return dataSet->getSize() ? true : false;
}

void CGraphBtnPressGrad::setData(CCatheterDataSet *_dataSet)
{
	dataSet = _dataSet;
	ASSERT(dataSet);
	setData(dataSet->getPgradientVector(),dataSet->getPgradientVectorTime());
}

void CGraphBtnPressGrad::setData(vector <FLOAT> *_pUpper,vector <FLOAT> *_tv)
{
	ASSERT(curvePUpperGradient);
	ASSERT(_pUpper);
	ASSERT(_tv);
	curvePUpperGradient->setData(_pUpper,_tv);
}

void CGraphBtnPressGrad::setEvents(CEvents *_eP)
{
	eventsP = _eP;

	ASSERT(curvePUpperGradient);
	curvePUpperGradient->setEvents(_eP);
}

void CGraphBtnPressGrad::createCurveSettings(CButtonPlotSettings *_plotSettings)
{
	curvePUpperGradient = new CRGLGraphSparse(PRESSURE_GRADIENT_RATIO);
	curvePUpperGradient->setSparseStepSize(envelopesSparseStep);
	curvePUpperGradient->setDebugTag(debugTag);

	ASSERT(curvePUpperGradient);
	
	CString tup;
	int nums = tup.LoadString(IDS_PPH_POES);
	_plotSettings->setCurveTitle(PRESSURE_GRADIENT_RATIO,tup);

	curvePUpperGradient->setTile(plotTile);	
	curvePUpperGradient->setThickness(_plotSettings->getCurveThickness(PRESSURE_GRADIENT_RATIO));

	addLayer(curvePUpperGradient);
	
	float from = .0f;
	float to = GRADIENT_LOWER_LIMIT;
	CRGLBand *bP = new CRGLBand(from,to,bgCol[bgLevelTypeLower][0 ],
										bgCol[bgLevelTypeLower][1 ],
										bgCol[bgLevelTypeLower][2 ],.2f);
	curvePUpperGradient->addBand(bP);
	from = to;
	to = GRADIENT_UPPER_LIMIT;
	bP = new CRGLBand(from,to,			bgCol[bgLevelTypeMulti][0 ],
										bgCol[bgLevelTypeMulti][1 ],
										bgCol[bgLevelTypeMulti][2 ],.2f);
	curvePUpperGradient->addBand(bP);
	from = to;
	to = 100.0f;
	bP = new CRGLBand(from,to,			bgCol[bgLevelTypeUpper][0 ],
										bgCol[bgLevelTypeUpper][1 ],
										bgCol[bgLevelTypeUpper][2 ],.2f);
	curvePUpperGradient->addBand(bP);

	curvePUpperGradient->setColour(_plotSettings->getCurveColour(PRESSURE_GRADIENT_RATIO));
	curvePUpperGradient->setTransparencyPerc(_plotSettings->getCurveTransparencyPerc(PRESSURE_GRADIENT_RATIO));
	curvePUpperGradient->setShow(true);
	
	CRGLText *graphTexts = new CRGLText();
	graphTexts->setTile(plotTile);

	CString ys;
	nums = ys.LoadString(IDS_PERCENT);
	graphTexts->setTexts(timeAxisLabel,_T(""),ys,title);
	addLayer(graphTexts);
}

float CGraphBtnPressGrad::getYAutoMax(void)
{
	return DEFAULT_PRESS_GRADIENT_MAX;

	/*
	FLOAT max,avg;
	ASSERT(dataSet);
	
	max = dataSet->getPOESVector()->size() > 0 ? 
		*max_element(dataSet->getPOESVector()->begin(), dataSet->getPOESVector()->end()) : .0f;
	avg = dataSet->getPOESVector()->size() > 0 ? 
		CDataSet::getMean(dataSet->getPOESVector()->begin(),dataSet->getPOESVector()->end()) : .0f;
	
	return getMaxYScaleFromMeanAndMax(avg,max);*/
}

///////////////////////////////////////////////////////////////////////
	
CGraphBtnCatheterTemperature::CGraphBtnCatheterTemperature()
{
	dataSet = NULL;
	dualPolarity = true;
}

CGraphBtnCatheterTemperature::~CGraphBtnCatheterTemperature()
{
}

BEGIN_MESSAGE_MAP(CGraphBtnCatheterTemperature, CGraphBtnChannel)
	ON_WM_NCHITTEST()
END_MESSAGE_MAP()

void CGraphBtnCatheterTemperature::updateGraphSettings(void)
{
	CGraphBtnChannel::updateGraphSettings();
}


/*
Description: Check for data
Returns: true if data are present, false if not
*/
bool CGraphBtnCatheterTemperature::getHasData(void)
{
	if (!dataSet) return false;
	return dataSet->getSize() ? true : false;
}

void CGraphBtnCatheterTemperature::setData(CCatheterDataSet *_dataSet)
{
}

void CGraphBtnCatheterTemperature::setData(vector <FLOAT> *_temp,vector <FLOAT> *_tv)
{
}

void CGraphBtnCatheterTemperature::setEvents(CEvents *_eP)
{
	eventsP = _eP;
}

void CGraphBtnCatheterTemperature::createCurveSettings(CButtonPlotSettings *_plotSettings)
{
}

float CGraphBtnCatheterTemperature::getYAutoMax(void)
{
	return DEFAULT_CATH_TEMP_MAX;
}

LRESULT CGraphBtnCatheterTemperature::OnNcHitTest(CPoint point)
{
	// TODO: Add your message handler code here and/or call default

	return CGraphBtnChannel::OnNcHitTest(point);
}

///////////////////////////////////////////////////////////////////////
	
CGraphBtnCatheterTemperatureT0::CGraphBtnCatheterTemperatureT0()
{
	dataSet = NULL;
	regSubKey = TEMPERATURE_T0;
	curveT0 = NULL;
	int nums = title.LoadString(IDS_T0_TITLE);
}

CGraphBtnCatheterTemperatureT0::~CGraphBtnCatheterTemperatureT0()
{
}

void CGraphBtnCatheterTemperatureT0::updateGraphSettings(void)
{
	CGraphBtnCatheterTemperature::updateGraphSettings();

	//---Additionals here
	CButtonPlotSettings plotSettings(dualPolarity, regSubKey);
	curveT0->setColour(plotSettings.getCurveColour(regSubKey));
	curveT0->setTransparencyPerc(plotSettings.getCurveTransparencyPerc(regSubKey));
	curveT0->setThickness(plotSettings.getCurveThickness(regSubKey));

	render();
}

void CGraphBtnCatheterTemperatureT0::updateCurveColour(void)
{
	curveT0->updateBrush();
}

CString CGraphBtnCatheterTemperatureT0::getTimeAndAmplAt(LONG _x,LONG _y)
{
	return curveT0->getStringTimeAndAmplAt(_x,_y,_T("degC/s")); 
}

void CGraphBtnCatheterTemperatureT0::setEventsToShow(UINT _evToShow, bool _onlyShowEvents /*= false*/)
{
	eventsToShow = _evToShow;
	drawOnlyEvents = _onlyShowEvents;
	curveT0->setEventsToShow(_evToShow);
	InvalidateRect(NULL,TRUE);
	UpdateWindow();
}


/*
Description: Check for data
Returns: true if data are present, false if not
*/
bool CGraphBtnCatheterTemperatureT0::getHasData(void)
{
	if (!dataSet) return false;
	return dataSet->getSize() ? true : false;
}

void CGraphBtnCatheterTemperatureT0::setData(CCatheterDataSet *_dataSet)
{
	dataSet = _dataSet;
	ASSERT(dataSet);
	setData(dataSet->getT0Vector(),dataSet->getTimeaxisVector());
}

void CGraphBtnCatheterTemperatureT0::setData(vector <FLOAT> *_temp,vector <FLOAT> *_tv)
{
	ASSERT(curveT0);
	curveT0->setData(_temp,_tv);
}

void CGraphBtnCatheterTemperatureT0::setEvents(CEvents *_eP)
{
	eventsP = _eP;

	ASSERT(curveT0);
	curveT0->setEvents(_eP);
}

void CGraphBtnCatheterTemperatureT0::createCurveSettings(CButtonPlotSettings *_plotSettings)
{
	curveT0 = new CRGLGraphSparse(TEMPERATURE_T0);

	ASSERT(curveT0);

	_plotSettings->setCurveTitle(regSubKey,title);

	curveT0->setTile(plotTile);	
	curveT0->setThickness(_plotSettings->getCurveThickness(regSubKey));

	addLayer(curveT0);
	curveT0->setColour(_plotSettings->getCurveColour(regSubKey));
	curveT0->setTransparencyPerc(_plotSettings->getCurveTransparencyPerc(regSubKey));
	curveT0->setShow(true);
	
	CRGLText *graphTexts = new CRGLText();
	graphTexts->setTile(plotTile);

	CString ys;
	int nums = ys.LoadString(IDS_DEGC_SEC);
	graphTexts->setTexts(timeAxisLabel,_T(""),ys,title);
	addLayer(graphTexts);
}

float CGraphBtnCatheterTemperatureT0::getYAutoMax(void)
{
	ASSERT(dataSet);
	
	FLOAT max = dataSet->getT0Vector()->size() > 0 ? 
		*max_element(dataSet->getT0Vector()->begin(), dataSet->getT0Vector()->end()) : .0f;
	FLOAT min = dataSet->getT0Vector()->size() > 0 ? 
		*min_element(dataSet->getT0Vector()->begin(), dataSet->getT0Vector()->end()) : .0f;
	FLOAT avg = dataSet->getT0Vector()->size() > 0 ?
		(float)for_each(dataSet->getT0Vector()->begin(), dataSet->getT0Vector()->end(), Average()) :
		.0f;
	
	FLOAT pos = fabs(max);
	FLOAT neg = fabs(min);
	return getMaxYScaleFromMeanAndMax(avg,pos > neg ? pos : neg);
}

///////////////////////////////////////////////////////////////////////
	
CGraphBtnCatheterTemperatureT1::CGraphBtnCatheterTemperatureT1()
{
	dataSet = NULL;
	regSubKey = TEMPERATURE_T1;
	curveT1 = NULL;
	int nums = title.LoadString(IDS_T1_TITLE);
}

CGraphBtnCatheterTemperatureT1::~CGraphBtnCatheterTemperatureT1()
{
}

void CGraphBtnCatheterTemperatureT1::updateGraphSettings(void)
{
	CGraphBtnCatheterTemperature::updateGraphSettings();

	//---Additionals here
	CButtonPlotSettings plotSettings(dualPolarity, regSubKey);
	curveT1->setColour(plotSettings.getCurveColour(regSubKey));
	curveT1->setTransparencyPerc(plotSettings.getCurveTransparencyPerc(regSubKey));
	curveT1->setThickness(plotSettings.getCurveThickness(regSubKey));

	render();
}

void CGraphBtnCatheterTemperatureT1::updateCurveColour(void)
{
	curveT1->updateBrush();
}

CString CGraphBtnCatheterTemperatureT1::getTimeAndAmplAt(LONG _x,LONG _y)
{
	return curveT1->getStringTimeAndAmplAt(_x,_y,_T("degC/s")); 
}

void CGraphBtnCatheterTemperatureT1::setEventsToShow(UINT _evToShow, bool _onlyShowEvents /*= false*/)
{
	eventsToShow = _evToShow;
	drawOnlyEvents = _onlyShowEvents;
	curveT1->setEventsToShow(_evToShow);
	InvalidateRect(NULL,TRUE);
	UpdateWindow();
}

/*
Description: Check for data
Returns: true if data are present, false if not
*/
bool CGraphBtnCatheterTemperatureT1::getHasData(void)
{
	if (!dataSet) return false;
	return dataSet->getSize() ? true : false;
}

void CGraphBtnCatheterTemperatureT1::setData(CCatheterDataSet *_dataSet)
{
	dataSet = _dataSet;
	ASSERT(dataSet);
	setData(dataSet->getT1Vector(),dataSet->getTimeaxisVector());
}

void CGraphBtnCatheterTemperatureT1::setData(vector <FLOAT> *_t,vector <FLOAT> *_tv)
{
	ASSERT(curveT1);
	curveT1->setData(_t,_tv);
}

void CGraphBtnCatheterTemperatureT1::setEvents(CEvents *_eP)
{
	eventsP = _eP;

	ASSERT(curveT1);
	curveT1->setEvents(_eP);
}

void CGraphBtnCatheterTemperatureT1::createCurveSettings(CButtonPlotSettings *_plotSettings)
{
	curveT1 = new CRGLGraphSparse(TEMPERATURE_T1);

	ASSERT(curveT1);

	_plotSettings->setCurveTitle(regSubKey,title);

	curveT1->setTile(plotTile);	
	curveT1->setThickness(_plotSettings->getCurveThickness(regSubKey));

	addLayer(curveT1);
	curveT1->setColour(_plotSettings->getCurveColour(regSubKey));
	curveT1->setTransparencyPerc(_plotSettings->getCurveTransparencyPerc(regSubKey));
	curveT1->setShow(true);
	
	CRGLText *graphTexts = new CRGLText();
	graphTexts->setTile(plotTile);

	CString ys;
	int nums = ys.LoadString(IDS_DEGC_SEC);
	graphTexts->setTexts(timeAxisLabel,_T(""),ys,title);
	addLayer(graphTexts);
}

float CGraphBtnCatheterTemperatureT1::getYAutoMax(void)
{
	ASSERT(dataSet);
	
	FLOAT max = dataSet->getT1Vector()->size() > 0 ? 
		*max_element(dataSet->getT1Vector()->begin(), dataSet->getT1Vector()->end()) : .0f;
	FLOAT min = dataSet->getT1Vector()->size() > 0 ? 
		*min_element(dataSet->getT1Vector()->begin(), dataSet->getT1Vector()->end()) : .0f;
	FLOAT avg = dataSet->getT1Vector()->size() > 0 ? 
		(float)for_each(dataSet->getT1Vector()->begin(), dataSet->getT1Vector()->end(), Average()) : .0f;
	
	FLOAT pos = fabs(max);
	FLOAT neg = fabs(min);
	return getMaxYScaleFromMeanAndMax(avg,pos > neg ? pos : neg);
}

///////////////////////////////////////////

/*
Description: For absolute temperature T1
*/
CGraphBtnCatheterTemperatureT1Raw::CGraphBtnCatheterTemperatureT1Raw()
{
	dataSet = NULL;
	regSubKey = TEMPERATURE_T1_RAW;
	curveT1 = NULL;
	int nums = title.LoadString(IDS_T1_TITLE_RAW);
	dualPolarity = false;
}

CGraphBtnCatheterTemperatureT1Raw::~CGraphBtnCatheterTemperatureT1Raw()
{
}

void CGraphBtnCatheterTemperatureT1Raw::updateGraphSettings(void)
{
	CGraphBtnCatheterTemperature::updateGraphSettings();

	//---Additionals here
	CButtonPlotSettings plotSettings(dualPolarity, regSubKey);
	curveT1->setColour(plotSettings.getCurveColour(regSubKey));
	curveT1->setTransparencyPerc(plotSettings.getCurveTransparencyPerc(regSubKey));
	curveT1->setThickness(plotSettings.getCurveThickness(regSubKey));

	render();
}

void CGraphBtnCatheterTemperatureT1Raw::createCurveSettings(CButtonPlotSettings * _plotSettings)
{
	curveT1 = new CRGLGraphSparse(TEMPERATURE_T1_RAW);

	ASSERT(curveT1);

	_plotSettings->setCurveTitle(regSubKey, title);

	curveT1->setTile(plotTile);
	curveT1->setThickness(_plotSettings->getCurveThickness(regSubKey));

	addLayer(curveT1);
	curveT1->setColour(_plotSettings->getCurveColour(regSubKey));
	curveT1->setTransparencyPerc(_plotSettings->getCurveTransparencyPerc(regSubKey));
	curveT1->setShow(true);

	CRGLText *graphTexts = new CRGLText();
	graphTexts->setTile(plotTile);

	CString ys;
	int nums = ys.LoadString(IDS_DEGC);
	graphTexts->setTexts(timeAxisLabel, _T(""), ys, title);
	addLayer(graphTexts);
}

void CGraphBtnCatheterTemperatureT1Raw::setData(CCatheterDataSet *_dataSet)
{
	dataSet = _dataSet;
	ASSERT(dataSet);
	setData(dataSet->getT1RawVector(), dataSet->getRawTimeVector());
}

void CGraphBtnCatheterTemperatureT1Raw::setYRange(void)
{
	if (!getHasData()) return;

	if (!m_target) return;
	render();  // Must do this to prep the graphics system

	int num = (int)layers.GetCount();

	FLOAT start = .0f;
	FLOAT stop = .0f;

	int numT = (int)tiles.GetCount();
	CRGLTile *tP = tiles.GetAt(0);
	FLOAT sta, sto;
	tP->getExtremeXLimits(&sta, &sto);
	start = sta;
	stop = sto;
	FLOAT ymin, ymax, y0, y1;
	tP->getPlotRectYScaled(&y0, &y1);
	ymin = y0;
	ymax = y1;
	for (int i = 1; i < numT; i++) {
		CRGLTile *tP = tiles.GetAt(i);
		tP->getExtremeXLimits(&sta, &sto);
		tP->getPlotRectYScaled(&y0, &y1);
		ymin = ymin < y0 ? y0 : ymin;
		ymax = ymax > y1 ? y1 : ymax;
		start = start < sta ? start : sta;
		stop = stop > sto ? stop : sto;
	}

	float yNewMax = getYAutoMax();
	float yNewMin = getYAutoMin();
	for (int i = 0; i < num; i++) {
		CRGLLayer *lP = layers.GetAt(i);
		int type = lP->getLayerType();
		if (RGLLayerTypeAxes == type) {
			((CRGLAxes *)lP)->xAxisWizard(start, stop);
			((CRGLAxes *)lP)->yAxisWizard(minAbsTemp, maxAbsTemp);
		}
		if ((RGLLayerTypeGraphSparse == type) || (RGLLayerTypeGraph == type)) {
			UINT id, type, family;
			((CRGLGraph *)lP)->getIDTypeFamily(&id, &type, &family);

			int numT = (int)tiles.GetCount();
			if (numT) {

				CRGLTile *tP = tiles.GetAt(0);
				tP->setPlotRectXScaled(start, stop);
				tP->setPlotRectYScaled(minAbsTemp, maxAbsTemp);
				for (int i = 1; i < numT; i++) {
					CRGLTile *tP = tiles.GetAt(i);
					tP->setPlotRectXScaled(start, stop);
					tP->setPlotRectYScaled(minAbsTemp, maxAbsTemp);
				}
			}
		}
		if (RGLLayerTypeGraphFilled == type) {
			UINT id, type, family;
			((CRGLGraphFilled *)lP)->getIDTypeFamily(&id, &type, &family);

			//---X axis
			int numT = (int)tiles.GetCount();
			if (numT) {
				CRGLTile *tP = tiles.GetAt(0);
				tP->setPlotRectXScaled(start, stop);
				tP->setPlotRectYScaled(minAbsTemp, maxAbsTemp);
				for (int i = 1; i < numT; i++) {
					CRGLTile *tP = tiles.GetAt(i);
					tP->setPlotRectXScaled(start, stop);
					tP->setPlotRectYScaled(minAbsTemp, maxAbsTemp);
				}
			}
		}
		if (RGLLayerTypeGraphPoints == type) {
			UINT id, type, family;
			((CRGLGraphEventMarkers *)lP)->getIDTypeFamily(&id, &type, &family);

			//---X axis
			int numT = (int)tiles.GetCount();
			if (numT) {
				CRGLTile *tP = tiles.GetAt(0);
				tP->setPlotRectXScaled(start, stop);
				tP->setPlotRectYScaled(minAbsTemp, maxAbsTemp);
				for (int i = 1; i < numT; i++) {
					CRGLTile *tP = tiles.GetAt(i);
					tP->setPlotRectXScaled(start, stop);
					tP->setPlotRectYScaled(minAbsTemp, maxAbsTemp);
				}
			}
		}
	}
	render();
}

void CGraphBtnCatheterTemperatureT1Raw::setData(vector <FLOAT> *_t, vector <FLOAT> *_tv)
{
	ASSERT(curveT1);
	curveT1->setData(_t, _tv);
}

///////////////////////////////////////////

/*
Description: For absolute temperature T0
*/
CGraphBtnCatheterTemperatureT0Raw::CGraphBtnCatheterTemperatureT0Raw()
{
	dataSet = NULL;
	regSubKey = TEMPERATURE_T0_RAW;
	curveT0 = NULL;
	int nums = title.LoadString(IDS_T0_TITLE_RAW);
	dualPolarity = false;
}

CGraphBtnCatheterTemperatureT0Raw::~CGraphBtnCatheterTemperatureT0Raw()
{
}

void CGraphBtnCatheterTemperatureT0Raw::updateGraphSettings(void)
{
	CGraphBtnCatheterTemperature::updateGraphSettings();

	//---Additionals here
	CButtonPlotSettings plotSettings(dualPolarity, regSubKey);
	curveT0->setColour(plotSettings.getCurveColour(regSubKey));
	curveT0->setTransparencyPerc(plotSettings.getCurveTransparencyPerc(regSubKey));
	curveT0->setThickness(plotSettings.getCurveThickness(regSubKey));

	render();
}

void CGraphBtnCatheterTemperatureT0Raw::createCurveSettings(CButtonPlotSettings * _plotSettings)
{
	curveT0 = new CRGLGraphSparse(TEMPERATURE_T0_RAW);

	ASSERT(curveT0);

	_plotSettings->setCurveTitle(regSubKey, title);

	curveT0->setTile(plotTile);
	curveT0->setThickness(_plotSettings->getCurveThickness(regSubKey));

	addLayer(curveT0);
	curveT0->setColour(_plotSettings->getCurveColour(regSubKey));
	curveT0->setTransparencyPerc(_plotSettings->getCurveTransparencyPerc(regSubKey));
	curveT0->setShow(true);

	CRGLText *graphTexts = new CRGLText();
	graphTexts->setTile(plotTile);

	CString ys;
	int nums = ys.LoadString(IDS_DEGC);
	graphTexts->setTexts(timeAxisLabel, _T(""), ys, title);
	addLayer(graphTexts);
}

void CGraphBtnCatheterTemperatureT0Raw::setData(CCatheterDataSet *_dataSet)
{
	dataSet = _dataSet;
	ASSERT(dataSet);
	setData(dataSet->getT0RawVector(), dataSet->getRawTimeVector());
}

void CGraphBtnCatheterTemperatureT0Raw::setYRange(void)
{
	if (!getHasData()) return;

	if (!m_target) return;
	render();  // Must do this to prep the graphics system

	int num = (int)layers.GetCount();

	FLOAT start = .0f;
	FLOAT stop = .0f;

	int numT = (int)tiles.GetCount();
	CRGLTile *tP = tiles.GetAt(0);
	FLOAT sta, sto;
	tP->getExtremeXLimits(&sta, &sto);
	start = sta;
	stop = sto;
	FLOAT ymin, ymax, y0, y1;
	tP->getPlotRectYScaled(&y0, &y1);
	ymin = y0;
	ymax = y1;
	for (int i = 1; i < numT; i++) {
		CRGLTile *tP = tiles.GetAt(i);
		tP->getExtremeXLimits(&sta, &sto);
		tP->getPlotRectYScaled(&y0, &y1);
		ymin = ymin < y0 ? y0 : ymin;
		ymax = ymax > y1 ? y1 : ymax;
		start = start < sta ? start : sta;
		stop = stop > sto ? stop : sto;
	}

	float yNewMax = getYAutoMax();
	float yNewMin = getYAutoMin();
	for (int i = 0; i < num; i++) {
		CRGLLayer *lP = layers.GetAt(i);
		int type = lP->getLayerType();
		if (RGLLayerTypeAxes == type) {
			((CRGLAxes *)lP)->xAxisWizard(start, stop);
			((CRGLAxes *)lP)->yAxisWizard(minAbsTemp, maxAbsTemp);
		}
		if ((RGLLayerTypeGraphSparse == type) || (RGLLayerTypeGraph == type)) {
			UINT id, type, family;
			((CRGLGraph *)lP)->getIDTypeFamily(&id, &type, &family);

			int numT = (int)tiles.GetCount();
			if (numT) {

				CRGLTile *tP = tiles.GetAt(0);
				tP->setPlotRectXScaled(start, stop);
				tP->setPlotRectYScaled(minAbsTemp, maxAbsTemp);
				for (int i = 1; i < numT; i++) {
					CRGLTile *tP = tiles.GetAt(i);
					tP->setPlotRectXScaled(start, stop);
					tP->setPlotRectYScaled(minAbsTemp, maxAbsTemp);
				}
			}
		}
		if (RGLLayerTypeGraphFilled == type) {
			UINT id, type, family;
			((CRGLGraphFilled *)lP)->getIDTypeFamily(&id, &type, &family);

			//---X axis
			int numT = (int)tiles.GetCount();
			if (numT) {
				CRGLTile *tP = tiles.GetAt(0);
				tP->setPlotRectXScaled(start, stop);
				tP->setPlotRectYScaled(minAbsTemp, maxAbsTemp);
				for (int i = 1; i < numT; i++) {
					CRGLTile *tP = tiles.GetAt(i);
					tP->setPlotRectXScaled(start, stop);
					tP->setPlotRectYScaled(minAbsTemp, maxAbsTemp);
				}
			}
		}
		if (RGLLayerTypeGraphPoints == type) {
			UINT id, type, family;
			((CRGLGraphEventMarkers *)lP)->getIDTypeFamily(&id, &type, &family);

			//---X axis
			int numT = (int)tiles.GetCount();
			if (numT) {
				CRGLTile *tP = tiles.GetAt(0);
				tP->setPlotRectXScaled(start, stop);
				tP->setPlotRectYScaled(minAbsTemp, maxAbsTemp);
				for (int i = 1; i < numT; i++) {
					CRGLTile *tP = tiles.GetAt(i);
					tP->setPlotRectXScaled(start, stop);
					tP->setPlotRectYScaled(minAbsTemp, maxAbsTemp);
				}
			}
		}
	}
	render();
}

void CGraphBtnCatheterTemperatureT0Raw::setData(vector <FLOAT> *_t, vector <FLOAT> *_tv)
{
	ASSERT(curveT0);
	curveT0->setData(_t, _tv);
}

///////////////////////////////////////////////////////////////////////

CGraphBtn160Mic::CGraphBtn160Mic()
{
	dataSet = NULL;
	regSubKey = KEY_160HZ;
	curveMic = NULL;
	int nums = title.LoadString(IDS_AIRBORNE_F0);
}
CGraphBtn160Mic::~CGraphBtn160Mic()
{
}

void CGraphBtn160Mic::setData(CAirMicDataSet *_dataSet)
{
	dataSet = _dataSet;
	ASSERT(dataSet);
	setData(dataSet->getAm1Vector(), dataSet->getTimeaxisVector());
}

void CGraphBtn160Mic::setData(vector <FLOAT> *_act, vector <FLOAT> *_tv)
{
	ASSERT(curveMic);
	curveMic->setData(_act, _tv);
}

void CGraphBtn160Mic::updateCurveColour(void)
{
	curveMic->updateBrush();
}
///////////////////////////////////////////////////////////////////////
CGraphBtn400Mic::CGraphBtn400Mic()
{
	dataSet = NULL;
	regSubKey = KEY_400HZ;
	curveMic = NULL;
	int nums = title.LoadString(IDS_AIRBORNE_F1);
}
CGraphBtn400Mic::~CGraphBtn400Mic()
{

}

void CGraphBtn400Mic::setData(CAirMicDataSet *_dataSet)
{
	dataSet = _dataSet;
	ASSERT(dataSet);
	setData(dataSet->getAm2Vector(), dataSet->getTimeaxisVector());
}

void CGraphBtn400Mic::setData(vector <FLOAT> *_act, vector <FLOAT> *_tv)
{
	ASSERT(curveMic);
	curveMic->setData(_act, _tv);
}

void CGraphBtn400Mic::updateCurveColour(void)
{
	curveMic->updateBrush();
}
///////////////////////////////////////////////////////////////////////
CGraphBtn1000Mic::CGraphBtn1000Mic()
{
	dataSet = NULL;
	regSubKey = KEY_1000HZ;
	curveMic = NULL;
	int nums = title.LoadString(IDS_AIRBORNE_F2);
}
CGraphBtn1000Mic::~CGraphBtn1000Mic()
{

}

void CGraphBtn1000Mic::setData(CAirMicDataSet *_dataSet)
{
	dataSet = _dataSet;
	ASSERT(dataSet);
	setData(dataSet->getAm3Vector(), dataSet->getTimeaxisVector());
}

void CGraphBtn1000Mic::setData(vector <FLOAT> *_act, vector <FLOAT> *_tv)
{
	ASSERT(curveMic);
	curveMic->setData(_act, _tv);
}

void CGraphBtn1000Mic::updateCurveColour(void)
{
	curveMic->updateBrush();
}
///////////////////////////////////////////////////////////////////////
CGraphBtn2500Mic::CGraphBtn2500Mic()
{
	dataSet = NULL;
	regSubKey = KEY_2500HZ;
	curveMic = NULL;
	int nums = title.LoadString(IDS_AIRBORNE_F3);
}
CGraphBtn2500Mic::~CGraphBtn2500Mic()
{

}

void CGraphBtn2500Mic::setData(CAirMicDataSet *_dataSet)
{
	dataSet = _dataSet;
	ASSERT(dataSet);
	setData(dataSet->getAm4Vector(), dataSet->getTimeaxisVector());
}

void CGraphBtn2500Mic::setData(vector <FLOAT> *_act, vector <FLOAT> *_tv)
{
	ASSERT(curveMic);
	curveMic->setData(_act, _tv);
}

void CGraphBtn2500Mic::updateCurveColour(void)
{
	curveMic->updateBrush();
}
///////////////////////////////////////////////////////////////////////

CGraphBtnAG200_Mic::CGraphBtnAG200_Mic()
{
	dataSet = NULL;
	regSubKey = AG200MIC;
	curveMic = NULL;
	int nums = title.LoadString(IDS_AG200_MIC_TITLE);
}

CGraphBtnAG200_Mic::~CGraphBtnAG200_Mic()
{

}

void CGraphBtnAG200_Mic::setData(CAG200MicDataSet *_dataSet)
{
	dataSet = _dataSet;
	ASSERT(dataSet);
	setData(dataSet->getAG200MicVector(), dataSet->getTimeaxisVector());
}

void CGraphBtnAG200_Mic::setData(vector <FLOAT> *_act, vector <FLOAT> *_tv)
{
	ASSERT(curveMic);
	curveMic->setData(_act, _tv);
}

float CGraphBtnAG200_Mic::getYAutoMax(void)
{
	FLOAT max, avg;
	ASSERT(dataSet);

	max = dataSet->getAG200MicVector()->size() > 0 ?
		*max_element(dataSet->getAG200MicVector()->begin(), dataSet->getAG200MicVector()->end()) : .0f;
	avg = dataSet->getAG200MicVector()->size() > 0 ?
		(float)for_each(dataSet->getAG200MicVector()->begin(), dataSet->getAG200MicVector()->end(), Average()) : .0f;

	return getMaxYScaleFromMeanAndMax(avg, max);
}


//////////////////////////////////////
	
CGraphBtnSingleMic::CGraphBtnSingleMic()
{
	dataSet = NULL;
	regSubKey = AG200MIC;
	curveMic = NULL;
	int nums = title.LoadString(IDS_AG200_MIC_TITLE);
}

CGraphBtnSingleMic::~CGraphBtnSingleMic()
{
}

void CGraphBtnSingleMic::updateGraphSettings(void)
{
	CGraphBtnChannel::updateGraphSettings();

	//---Additionals here
	CButtonPlotSettings plotSettings(dualPolarity, regSubKey);
	curveMic->setColour(plotSettings.getCurveColour(regSubKey));
	curveMic->setTransparencyPerc(plotSettings.getCurveTransparencyPerc(regSubKey));
	curveMic->setThickness(plotSettings.getCurveThickness(regSubKey));

	render();
}

void CGraphBtnSingleMic::updateCurveColour(void)
{
}

void CGraphBtnSingleMic::setEventsToShow(UINT _evToShow, bool _onlyShowEvents /*= false*/)
{
	eventsToShow = _evToShow;
	drawOnlyEvents = _onlyShowEvents;
	curveMic->setEventsToShow(_evToShow);
	InvalidateRect(NULL,TRUE);
	UpdateWindow();
}

/*
Description: Check for data
Returns: true if data are present, false if not
*/
bool CGraphBtnSingleMic::getHasData(void)
{
	if (!dataSet) return false;
	return dataSet->getSize() ? true : false;
}

void CGraphBtnSingleMic::setData(CAirMicDataSet *_dataSet)
{
}

void CGraphBtnSingleMic::setData(vector <FLOAT> *_act,vector <FLOAT> *_tv)
{
	ASSERT(curveMic);
	curveMic->setData(_act,_tv);
}

void CGraphBtnSingleMic::setEvents(CEvents *_eP)
{
	eventsP = _eP;

	ASSERT(curveMic);
	curveMic->setEvents(_eP);
}

void CGraphBtnSingleMic::createCurveSettings(CButtonPlotSettings *_plotSettings)
{
	curveMic = new CRGLGraphSparse(regSubKey);

	ASSERT(curveMic);
	
	_plotSettings->setCurveTitle(regSubKey,title);

	curveMic->setTile(plotTile);	
	curveMic->setThickness(_plotSettings->getCurveThickness(regSubKey));

	addLayer(curveMic);
	curveMic->setColour(_plotSettings->getCurveColour(regSubKey));
	curveMic->setTransparencyPerc(_plotSettings->getCurveTransparencyPerc(regSubKey));
	curveMic->setShow(true);
	
	CRGLText *graphTexts = new CRGLText();
	graphTexts->setTile(plotTile);

	CString ys;
	graphTexts->setTexts(timeAxisLabel,_T(""),_T(""),title);
	addLayer(graphTexts);
}

float CGraphBtnSingleMic::getYAutoMax(void)
{
	ASSERT(dataSet);

	FLOAT max[4], avg[4];
	max[0] = dataSet->getAm1Vector()->size() > 0 ?
		*max_element(dataSet->getAm1Vector()->begin(), dataSet->getAm1Vector()->end()) : .0f;
	avg[0] = dataSet->getAm1Vector()->size() > 0 ?
		(float)for_each(dataSet->getAm1Vector()->begin(), dataSet->getAm1Vector()->end(), Average()) : .0f;

	max[1] = dataSet->getAm2Vector()->size() > 0 ?
		*max_element(dataSet->getAm2Vector()->begin(), dataSet->getAm2Vector()->end()) : .0f;
	avg[1] = dataSet->getAm2Vector()->size() > 0 ?
		(float)for_each(dataSet->getAm2Vector()->begin(), dataSet->getAm2Vector()->end(), Average()) : .0f;

	max[2] = dataSet->getAm3Vector()->size() > 0 ?
		*max_element(dataSet->getAm3Vector()->begin(), dataSet->getAm3Vector()->end()) : .0f;
	avg[2] = dataSet->getAm3Vector()->size() > 0 ?
		(float)for_each(dataSet->getAm3Vector()->begin(), dataSet->getAm3Vector()->end(), Average()) : .0f;

	max[3] = dataSet->getAm4Vector()->size() > 0 ?
		*max_element(dataSet->getAm4Vector()->begin(), dataSet->getAm4Vector()->end()) : .0f;
	avg[3] = dataSet->getAm4Vector()->size() > 0 ?
		(float)for_each(dataSet->getAm4Vector()->begin(), dataSet->getAm4Vector()->end(), Average()) : .0f;
	
	FLOAT maxr = 2.0f;
	FLOAT avgr = 1.0f;
	if ((max[0] >= max[1]) && (max[0] >= max[2]) && (max[0] >= max[3])) {
		maxr = max[0];
		avgr = avg[0];
	}
	else if ((max[1] >= max[0]) && (max[1] >= max[2]) && (max[1] >= max[3])) {
		maxr = max[1];
		avgr = avg[1];
	}
	if ((max[2] >= max[1]) && (max[2] >= max[0]) && (max[2] >= max[3])) {
		maxr = max[2];
		avgr = avg[2];
	}
	if ((max[3] > max[1]) && (max[3] > max[2]) && (max[3] > max[0])) {
		maxr = max[3];
		avgr = avg[3];
	}
	return getMaxYScaleFromMeanAndMax(avgr,maxr);
}

/////////////////////////////////////////////////////////////////////

CGraphBtnRespFrq::CGraphBtnRespFrq()
{
	dataSet = NULL;
	regSubKey = RESP_FRQ;
	curveRespFrq = NULL;
	int nums = title.LoadString(IDS_RESP_FRQ_TITLE);
	dualPolarity = false;
}

CGraphBtnRespFrq::~CGraphBtnRespFrq()
{
}

void CGraphBtnRespFrq::updateGraphSettings(void)
{
	CGraphBtnChannel::updateGraphSettings();

	//---Additionals here
	CButtonPlotSettings plotSettings(dualPolarity, regSubKey);
	curveRespFrq->setColour(plotSettings.getCurveColour(regSubKey));
	curveRespFrq->setTransparencyPerc(plotSettings.getCurveTransparencyPerc(regSubKey));
	curveRespFrq->setThickness(plotSettings.getCurveThickness(regSubKey));

	render();
}


void CGraphBtnRespFrq::updateCurveColour(void)
{
	curveRespFrq->updateBrush();
}

CString CGraphBtnRespFrq::getTimeAndAmplAt(LONG _x,LONG _y)
{
	return curveRespFrq->getStringTimeAndAmplAt(_x,_y,_T("/min")); 
}

void CGraphBtnRespFrq::setEventsToShow(UINT _evToShow, bool _onlyShowEvents /*= false*/)
{
	eventsToShow = _evToShow;
	drawOnlyEvents = _onlyShowEvents;
	curveRespFrq->setEventsToShow(_evToShow);
	InvalidateRect(NULL,TRUE);
	UpdateWindow();
}

/*
Description: Check for data
Returns: true if data are present, false if not
*/
bool CGraphBtnRespFrq::getHasData(void)
{
	if (!dataSet) return false;
	return dataSet->getSize() ? true : false;
}

void CGraphBtnRespFrq::setData(CCatheterDataSet *_dataSet)
{
	dataSet = _dataSet;
	ASSERT(dataSet);
	setData(dataSet->getRespFrqVector(),dataSet->getRespFrqVectorTime());
}

void CGraphBtnRespFrq::setData(vector <FLOAT> *_frq,vector <FLOAT> *_tv)
{
	ASSERT(curveRespFrq);
	curveRespFrq->setData(_frq,_tv);
}

void CGraphBtnRespFrq::setEvents(CEvents *_eP)
{
	eventsP = _eP;

	ASSERT(curveRespFrq);
	curveRespFrq->setEvents(_eP);
}

void CGraphBtnRespFrq::createCurveSettings(CButtonPlotSettings *_plotSettings)
{
	curveRespFrq = new CRGLGraphSparse(RESP_FRQ);
	curveRespFrq->setSparseStepSize(envelopesSparseStep);

	ASSERT(curveRespFrq);
	
	_plotSettings->setCurveTitle(regSubKey,title);

	curveRespFrq->setTile(plotTile);	
	curveRespFrq->setThickness(_plotSettings->getCurveThickness(regSubKey));

	addLayer(curveRespFrq);
	curveRespFrq->setColour(_plotSettings->getCurveColour(regSubKey));
	curveRespFrq->setTransparencyPerc(_plotSettings->getCurveTransparencyPerc(regSubKey));
	curveRespFrq->setShow(true);
	
	CRGLText *graphTexts = new CRGLText();
	graphTexts->setTile(plotTile);

	CString ys;
	int nums = ys.LoadString(IDS_PER_MIN);
	graphTexts->setTexts(timeAxisLabel,_T(""),ys,title);
	addLayer(graphTexts);
}

float CGraphBtnRespFrq::getYAutoMax(void)
{
	return DEFAULT_RESP_FRQ_MAX; // per minute

	/*FLOAT max,avg;
	ASSERT(dataSet);
	
	max = dataSet->getRespFrqVector()->size() > 0 ? 
		*max_element(dataSet->getRespFrqVector()->begin(), dataSet->getRespFrqVector()->end()) :
	    .0f;
	avg = dataSet->getRespFrqVector()->size() > 0 ? 
		CDataSet::getMean(dataSet->getRespFrqVector()->begin(),dataSet->getRespFrqVector()->end()) :
		.0f;
	
	return getMaxYScaleFromMeanAndMax(avg,max);*/
}

/////////////////////////////////////////////////////////////////////

CGraphBtnAdmittance::CGraphBtnAdmittance()
{
	dataSet = NULL;
	regSubKey = ADMITTANCE;
	curveAdmittance = NULL;
	int nums = title.LoadString(IDS_ADMITT);
	dualPolarity = false;
}

CGraphBtnAdmittance::~CGraphBtnAdmittance()
{
}

void CGraphBtnAdmittance::updateCurveColour(void)
{
	curveAdmittance->updateBrush();
}

void CGraphBtnAdmittance::updateGraphSettings(void)
{
	CGraphBtnChannel::updateGraphSettings();

	//---Additionals here
	CButtonPlotSettings plotSettings(dualPolarity, regSubKey);
	curveAdmittance->setColour(plotSettings.getCurveColour(regSubKey));
	curveAdmittance->setTransparencyPerc(plotSettings.getCurveTransparencyPerc(regSubKey));
	curveAdmittance->setThickness(plotSettings.getCurveThickness(regSubKey));

	render();
}

void CGraphBtnAdmittance::setEventsToShow(UINT _evToShow, bool _onlyShowEvents /*= false*/)
{
	eventsToShow = _evToShow;
	drawOnlyEvents = _onlyShowEvents;
	curveAdmittance->setEventsToShow(_evToShow);
	InvalidateRect(NULL,TRUE);
	UpdateWindow();
}

/*
Description: Check for data
Returns: true if data are present, false if not
*/
bool CGraphBtnAdmittance::getHasData(void)
{
	if (!dataSet) return false;
	return dataSet->getSize() ? true : false;
}

void CGraphBtnAdmittance::setData(CCatheterDataSet *_dataSet)
{
	dataSet = _dataSet;
	ASSERT(dataSet);
	setData(dataSet->getAdmittanceVector(),dataSet->getAdmittanceVectorTime());
}

void CGraphBtnAdmittance::setData(vector <FLOAT> *_adm,vector <FLOAT> *_tv)
{
	ASSERT(curveAdmittance);
	curveAdmittance->setData(_adm,_tv);
}

void CGraphBtnAdmittance::setEvents(CEvents *_eP)
{
	eventsP = _eP;

	ASSERT(curveAdmittance);
	curveAdmittance->setEvents(_eP);
}

void CGraphBtnAdmittance::createCurveSettings(CButtonPlotSettings *_plotSettings)
{
	curveAdmittance = new CRGLGraphSparse(ADMITTANCE);
	curveAdmittance->setSparseStepSize(envelopesSparseStep);

	ASSERT(curveAdmittance);
	
	_plotSettings->setCurveTitle(regSubKey,title);

	curveAdmittance->setTile(plotTile);	
	curveAdmittance->setThickness(_plotSettings->getCurveThickness(regSubKey));

	addLayer(curveAdmittance);
	curveAdmittance->setColour(_plotSettings->getCurveColour(regSubKey));
	curveAdmittance->setTransparencyPerc(_plotSettings->getCurveTransparencyPerc(regSubKey));
	curveAdmittance->setShow(true);
	
	CRGLText *graphTexts = new CRGLText();
	graphTexts->setTile(plotTile);

	graphTexts->setTexts(timeAxisLabel,_T(""),_T(""),title);
	addLayer(graphTexts);
}

float CGraphBtnAdmittance::getYAutoMax(void)
{
	FLOAT max;
	ASSERT(dataSet);

	max = 10.0f * dataSet->getMedian(dataSet->getAdmittanceVector()->begin(), dataSet->getAdmittanceVector()->end());
	max = max <= .0f ? 1.0f : max;

	return max; 
}


///////////////////////////////////////////////////////////////////////
	
CGraphBtnPOESEnv::CGraphBtnPOESEnv()
{
	dataSet = NULL;
	regSubKey = POES_ENV;
	curvePOESEnv = NULL;
	
	curveBaseline = NULL;
	int nums = title.LoadString(IDS_POES_ENV_TITLE);
	dualPolarity = false;
}

CGraphBtnPOESEnv::~CGraphBtnPOESEnv()
{
}

void CGraphBtnPOESEnv::updateGraphSettings(void)
{
	CGraphBtnCatheterPressure::updateGraphSettings();

	//---Additionals here
	CButtonPlotSettings plotSettings(dualPolarity, regSubKey);
	curvePOESEnv->setColour(plotSettings.getCurveColour(regSubKey));
	curvePOESEnv->setTransparencyPerc(plotSettings.getCurveTransparencyPerc(regSubKey));
	curvePOESEnv->setThickness(plotSettings.getCurveThickness(regSubKey));

	curveBaseline->setColour(plotSettings.getCurveColour(POES_ENV_BASELINE));
	curveBaseline->setTransparencyPerc(plotSettings.getCurveTransparencyPerc(POES_ENV_BASELINE));
	curveBaseline->setThickness(plotSettings.getCurveThickness(POES_ENV_BASELINE));

	render();
}

void CGraphBtnPOESEnv::updateCurveColour(void)
{
	curvePOESEnv->updateBrush();
}

CString CGraphBtnPOESEnv::getTimeAndAmplAt(LONG _x,LONG _y)
{
	CString s;
	s = curvePOESEnv->getStringTimeAndAmplAt(_x,_y,_T("cmH2O")); 
	if (advancedMode) {
		CString s2;
		int nums = s2.LoadString(IDS_BASELINE);
		s += _T("\n");
		s += s2;
		s += _T(" ");
		s += curveBaseline->getStringAmplAt(_x,_y,_T("cmH2O"));
	}
	return s; 
}

void CGraphBtnPOESEnv::setEventsToShow(UINT _evToShow, bool _onlyShowEvents /*= false*/)
{
	drawOnlyEvents = _onlyShowEvents;
	eventsToShow = _evToShow;
	curvePOESEnv->setEventsToShow(_evToShow);
	InvalidateRect(NULL,TRUE);
	UpdateWindow();
}

/*
Description: Check for data
Returns: true if data are present, false if not
*/
bool CGraphBtnPOESEnv::getHasData(void)
{
	if (!dataSet) return false;
	return dataSet->getSize() ? true : false;
}

void CGraphBtnPOESEnv::setData(CCatheterDataSet *_dataSet)
{
	dataSet = _dataSet;
	ASSERT(dataSet);
	setData(dataSet->getPOESEnvVector(),dataSet->getPOESBaselineVector(),dataSet->getPOESEnvVectorTime());
}

void CGraphBtnPOESEnv::setData(vector <FLOAT> *_p,vector <FLOAT> *_baseline,vector <FLOAT> *_tv)
{
	ASSERT(curvePOESEnv);
	ASSERT(curveBaseline);
	curvePOESEnv->setData(_p,_tv);
	curveBaseline->setData(_baseline,_tv);
}

void CGraphBtnPOESEnv::setEvents(CEvents *_eP)
{
	eventsP = _eP;

	ASSERT(curvePOESEnv);
	curvePOESEnv->setEvents(_eP);
}

void CGraphBtnPOESEnv::createCurveSettings(CButtonPlotSettings *_plotSettings)
{
	curvePOESEnv = new CRGLGraphSparse(POES_ENV);
	curvePOESEnv->setSparseStepSize(envelopesSparseStep);
	curveBaseline = new CRGLGraphSparse(POES_ENV_BASELINE);
	curveBaseline->setSparseStepSize(envelopesSparseStep);

	ASSERT(curvePOESEnv);
	ASSERT(curveBaseline);

	_plotSettings->setCurveTitle(regSubKey,title);

	curvePOESEnv->setTile(plotTile);	
	curvePOESEnv->setThickness(_plotSettings->getCurveThickness(regSubKey));
	addLayer(curvePOESEnv);
	curvePOESEnv->setColour(_plotSettings->getCurveColour(regSubKey));
	curvePOESEnv->setTransparencyPerc(_plotSettings->getCurveTransparencyPerc(regSubKey));
	curvePOESEnv->setShow(true);
	
	curveBaseline->setTile(plotTile);	
	curveBaseline->setThickness(_plotSettings->getCurveThickness(POES_ENV_BASELINE));
	addLayer(curveBaseline);
	curveBaseline->setColour(_plotSettings->getCurveColour(POES_ENV_BASELINE));
	curveBaseline->setTransparencyPerc(_plotSettings->getCurveTransparencyPerc(POES_ENV_BASELINE));
	curveBaseline->setShow(true);
	
	CRGLText *graphTexts = new CRGLText();
	graphTexts->setTile(plotTile);

	CString ys;
	int nums = ys.LoadString(IDS_cmH2O);
	graphTexts->setTexts(timeAxisLabel,_T(""),ys,title);
	addLayer(graphTexts);
}

float CGraphBtnPOESEnv::getYAutoMax(void)
{
	FLOAT max,avg;
	ASSERT(dataSet);
	
	max = dataSet->getPOESEnvVector()->size() > 0 ? 
		*max_element(dataSet->getPOESEnvVector()->begin(), dataSet->getPOESEnvVector()->end()) : .0f;
	avg = dataSet->getPOESEnvVector()->size() > 0 ?
		(float)for_each(dataSet->getPOESEnvVector()->begin(), dataSet->getPOESEnvVector()->end(), Average()) : .0f;
	
	return getMaxYScaleFromMeanAndMax(avg,max);
}


///////////////////////////////////////////////////////////////////////
	
CGraphBtnPPHEnv::CGraphBtnPPHEnv()
{
	dataSet = NULL;
	regSubKey = PPH_ENV;
	curvePPHEnv = NULL;
	curveBaseline = NULL;
	int nums = title.LoadString(IDS_PPH_ENV_TITLE);
	dualPolarity = false;
}

CGraphBtnPPHEnv::~CGraphBtnPPHEnv()
{
}

void CGraphBtnPPHEnv::updateCurveColour(void)
{
	curvePPHEnv->updateBrush();
}

void CGraphBtnPPHEnv::updateGraphSettings(void)
{
	CGraphBtnCatheterPressure::updateGraphSettings();

	//---Additionals here
	CButtonPlotSettings plotSettings(dualPolarity, regSubKey);
	curvePPHEnv->setColour(plotSettings.getCurveColour(regSubKey));
	curvePPHEnv->setTransparencyPerc(plotSettings.getCurveTransparencyPerc(regSubKey));
	curvePPHEnv->setThickness(plotSettings.getCurveThickness(regSubKey));

	curveBaseline->setColour(plotSettings.getCurveColour(PPH_ENV_BASELINE));
	curveBaseline->setTransparencyPerc(plotSettings.getCurveTransparencyPerc(PPH_ENV_BASELINE));
	curveBaseline->setThickness(plotSettings.getCurveThickness(PPH_ENV_BASELINE));

	render();
}

CString CGraphBtnPPHEnv::getTimeAndAmplAt(LONG _x,LONG _y)
{
	CString s;
	s = curvePPHEnv->getStringTimeAndAmplAt(_x,_y,_T("cmH2O")); 
	if (advancedMode) {
		CString s2;
		int nums = s2.LoadString(IDS_BASELINE);
		s += _T("\n");
		s += s2;
		s += _T(" ");
		s += curveBaseline->getStringAmplAt(_x,_y,_T("cmH2O"));
	}
	return s; 
}

void CGraphBtnPPHEnv::setEventsToShow(UINT _evToShow, bool _onlyShowEvents /*= false*/)
{
	eventsToShow = _evToShow;
	drawOnlyEvents = _onlyShowEvents;
	curvePPHEnv->setEventsToShow(_evToShow);
	InvalidateRect(NULL,TRUE);
	UpdateWindow();
}

/*
Description: Check for data
Returns: true if data are present, false if not
*/
bool CGraphBtnPPHEnv::getHasData(void)
{
	if (!dataSet) return false;
	return dataSet->getSize() ? true : false;
}

void CGraphBtnPPHEnv::setData(CCatheterDataSet *_dataSet)
{
	dataSet = _dataSet;
	ASSERT(dataSet);
	setData(dataSet->getPPHEnvVector(),dataSet->getPPHBaselineVector(),dataSet->getPPHEnvVectorTime());
}

void CGraphBtnPPHEnv::setData(vector <FLOAT> *_p,vector <FLOAT> *_baseline,vector <FLOAT> *_tv)
{
	ASSERT(curvePPHEnv);
	ASSERT(curveBaseline);

	curvePPHEnv->setData(_p,_tv);
	curveBaseline->setData(_baseline,_tv);
}

void CGraphBtnPPHEnv::setEvents(CEvents *_eP)
{
	eventsP = _eP;

	ASSERT(curvePPHEnv);
	curvePPHEnv->setEvents(_eP);
}

void CGraphBtnPPHEnv::createCurveSettings(CButtonPlotSettings *_plotSettings)
{
	curvePPHEnv = new CRGLGraphSparse(PPH_ENV);
	curvePPHEnv->setSparseStepSize(envelopesSparseStep);
	curveBaseline = new CRGLGraphSparse(PPH_ENV_BASELINE);
	curveBaseline->setSparseStepSize(envelopesSparseStep);

	ASSERT(curvePPHEnv);

	_plotSettings->setCurveTitle(regSubKey,title);
	 
	curvePPHEnv->setTile(plotTile);	
	curvePPHEnv->setThickness(_plotSettings->getCurveThickness(regSubKey));
	addLayer(curvePPHEnv);
	curvePPHEnv->setColour(_plotSettings->getCurveColour(regSubKey));
	curvePPHEnv->setTransparencyPerc(_plotSettings->getCurveTransparencyPerc(regSubKey));
	curvePPHEnv->setShow(true);
	
	curveBaseline->setTile(plotTile);	
	curveBaseline->setThickness(_plotSettings->getCurveThickness(PPH_ENV_BASELINE));
	addLayer(curveBaseline);
	curveBaseline->setColour(_plotSettings->getCurveColour(PPH_ENV_BASELINE));
	curveBaseline->setTransparencyPerc(_plotSettings->getCurveTransparencyPerc(PPH_ENV_BASELINE));
	curveBaseline->setShow(true);
	
	CRGLText *graphTexts = new CRGLText();
	graphTexts->setTile(plotTile);

	CString ys;
	int nums = ys.LoadString(IDS_cmH2O);
	graphTexts->setTexts(timeAxisLabel,_T(""),ys,title);
	addLayer(graphTexts);
}

float CGraphBtnPPHEnv::getYAutoMax(void)
{
	FLOAT max,avg;
	ASSERT(dataSet);
	
	max = dataSet->getPPHEnvVector()->size() > 0 ? 
		*max_element(dataSet->getPPHEnvVector()->begin(), dataSet->getPPHEnvVector()->end()) : .0f; 
	avg = dataSet->getPPHEnvVector()->size() ?
		(float)for_each(dataSet->getPPHEnvVector()->begin(), dataSet->getPPHEnvVector()->end(), Average()) : .0f;
	
	return getMaxYScaleFromMeanAndMax(avg,max);
}


///////////////////////////////////////////////////////////////////////
	
CGraphBtnT0Env::CGraphBtnT0Env()
{
	dataSet = NULL;
	regSubKey = T0_ENV;
	curveT0Env = NULL;
	int nums = title.LoadString(IDS_T0_ENV_TITLE);
	dualPolarity = false;
}

CGraphBtnT0Env::~CGraphBtnT0Env()
{
}

void CGraphBtnT0Env::updateGraphSettings(void)
{
	CGraphBtnCatheterTemperature::updateGraphSettings();

	//---Additionals here
	CButtonPlotSettings plotSettings(dualPolarity, regSubKey);
	curveT0Env->setColour(plotSettings.getCurveColour(regSubKey));
	curveT0Env->setTransparencyPerc(plotSettings.getCurveTransparencyPerc(regSubKey));
	curveT0Env->setThickness(plotSettings.getCurveThickness(regSubKey));

	render();
}

void CGraphBtnT0Env::updateCurveColour(void)
{
	curveT0Env->updateBrush();
}

CString CGraphBtnT0Env::getTimeAndAmplAt(LONG _x,LONG _y)
{
	CString s;
	s = curveT0Env->getStringTimeAndAmplAt(_x,_y,_T("degC/s")); 
	//--No baseline here
	return s;
}

void CGraphBtnT0Env::setEventsToShow(UINT _evToShow, bool _onlyShowEvents /*= false*/)
{
	drawOnlyEvents = _onlyShowEvents;
	eventsToShow = _evToShow;
	curveT0Env->setEventsToShow(_evToShow);
	InvalidateRect(NULL,TRUE);
	UpdateWindow();
}

/*
Description: Check for data
Returns: true if data are present, false if not
*/
bool CGraphBtnT0Env::getHasData(void)
{
	if (!dataSet) return false;
	return dataSet->getSize() ? true : false;
}

void CGraphBtnT0Env::setData(CCatheterDataSet *_dataSet)
{
	dataSet = _dataSet;
	ASSERT(dataSet);
	setData(dataSet->getT0EnvVector(),dataSet->getT0EnvVectorTime());
}

void CGraphBtnT0Env::setData(vector <FLOAT> *_t,vector <FLOAT> *_tv)
{
	ASSERT(curveT0Env);
	curveT0Env->setData(_t,_tv);
}

void CGraphBtnT0Env::setEvents(CEvents *_eP)
{
	eventsP = _eP;

	ASSERT(curveT0Env);
	curveT0Env->setEvents(_eP);
}

void CGraphBtnT0Env::createCurveSettings(CButtonPlotSettings *_plotSettings)
{
	curveT0Env = new CRGLGraphSparse(T0_ENV);
	curveT0Env->setSparseStepSize(envelopesSparseStep);

	ASSERT(curveT0Env);

	_plotSettings->setCurveTitle(regSubKey,title);

	curveT0Env->setTile(plotTile);	
	curveT0Env->setThickness(_plotSettings->getCurveThickness(regSubKey));

	addLayer(curveT0Env);
	curveT0Env->setColour(_plotSettings->getCurveColour(regSubKey));
	curveT0Env->setTransparencyPerc(_plotSettings->getCurveTransparencyPerc(regSubKey));
	curveT0Env->setShow(true);
	
	CRGLText *graphTexts = new CRGLText();
	graphTexts->setTile(plotTile);

	CString ys;
	int nums = ys.LoadString(IDS_DEGC_SEC);
	graphTexts->setTexts(timeAxisLabel,_T(""),ys,title);
	addLayer(graphTexts);
}

float CGraphBtnT0Env::getYAutoMax(void)
{
	FLOAT max,avg;
	ASSERT(dataSet);
	
	max = dataSet->getT0EnvVector()->size() > 0 ? 
		*max_element(dataSet->getT0EnvVector()->begin(), dataSet->getT0EnvVector()->end()) : .0f; 
	avg = dataSet->getT0EnvVector()->size() ?
		(float)for_each(dataSet->getT0EnvVector()->begin(), dataSet->getT0EnvVector()->end(), Average()) : .0f;
	
	return getMaxYScaleFromMeanAndMax(avg,max);
}


///////////////////////////////////////////////////////////////////////
	
CGraphBtnT1Env::CGraphBtnT1Env()
{
	dataSet = NULL;
	regSubKey = T1_ENV;
	curveEnv = NULL;
	curveBaseline = NULL;
	int nums = title.LoadString(IDS_T1_ENV_TITLE);
	dualPolarity = false;
}

CGraphBtnT1Env::~CGraphBtnT1Env()
{
}

void CGraphBtnT1Env::updateCurveColour(void)
{
	curveEnv->updateBrush();
}

void CGraphBtnT1Env::updateGraphSettings(void)
{
	CGraphBtnCatheterTemperature::updateGraphSettings();

	//---Additionals here
	CButtonPlotSettings plotSettings(dualPolarity, regSubKey);
	curveEnv->setColour(plotSettings.getCurveColour(regSubKey));
	curveEnv->setTransparencyPerc(plotSettings.getCurveTransparencyPerc(regSubKey));
	curveEnv->setThickness(plotSettings.getCurveThickness(regSubKey));

	curveBaseline->setColour(plotSettings.getCurveColour(T1_ENV_BASELINE));
	curveBaseline->setTransparencyPerc(plotSettings.getCurveTransparencyPerc(T1_ENV_BASELINE));
	curveBaseline->setThickness(plotSettings.getCurveThickness(T1_ENV_BASELINE));

	render();
}

CString CGraphBtnT1Env::getTimeAndAmplAt(LONG _x,LONG _y)
{
	CString s;
	s = curveEnv->getStringTimeAndAmplAt(_x,_y,_T("degC/s")); 
	if (advancedMode) {
		CString s2;
		int nums = s2.LoadString(IDS_BASELINE);
		s += _T("\n");
		s += s2;
		s += _T(" ");
		s += curveBaseline->getStringAmplAt(_x,_y,_T("degC/s"));
	}
	return s;
}

void CGraphBtnT1Env::setEventsToShow(UINT _evToShow, bool _onlyShowEvents /*= false*/)
{
	drawOnlyEvents = _onlyShowEvents;
	eventsToShow = _evToShow;
	curveEnv->setEventsToShow(_evToShow);
	InvalidateRect(NULL,TRUE);
	UpdateWindow();
}

/*
Description: Check for data
Returns: true if data are present, false if not
*/
bool CGraphBtnT1Env::getHasData(void)
{
	if (!dataSet) return false;
	return dataSet->getSize() ? true : false;
}

void CGraphBtnT1Env::setData(CCatheterDataSet *_dataSet)
{
	dataSet = _dataSet;
	ASSERT(dataSet);
	setData(dataSet->getT1EnvVector(),dataSet->getT1EnvBaselineVector(),dataSet->getT1EnvVectorTime());
}

void CGraphBtnT1Env::setData(vector <FLOAT> *_t,vector <FLOAT> *_baseline,vector <FLOAT> *_tv)
{
	ASSERT(curveEnv);
	ASSERT(curveBaseline);
	curveEnv->setData(_t,_tv);
	curveBaseline->setData(_baseline,_tv);
}

void CGraphBtnT1Env::setEvents(CEvents *_eP)
{
	eventsP = _eP;

	ASSERT(curveEnv);
	curveEnv->setEvents(_eP);
}

void CGraphBtnT1Env::createCurveSettings(CButtonPlotSettings *_plotSettings)
{
	curveEnv = new CRGLGraphSparse(T1_ENV);
	curveEnv->setSparseStepSize(envelopesSparseStep);
	curveBaseline = new CRGLGraphSparse(T1_ENV_BASELINE);
	curveBaseline->setSparseStepSize(envelopesSparseStep);

	ASSERT(curveEnv);
	ASSERT(curveBaseline);

	_plotSettings->setCurveTitle(regSubKey,title);

	//---Curve Env
	curveEnv->setTile(plotTile);	
	curveEnv->setThickness(_plotSettings->getCurveThickness(regSubKey));
	addLayer(curveEnv);
	curveEnv->setColour(_plotSettings->getCurveColour(regSubKey));
	curveEnv->setTransparencyPerc(_plotSettings->getCurveTransparencyPerc(regSubKey));
	curveEnv->setShow(true);

	//---Curve Env baseline
	curveBaseline->setTile(plotTile);	
	curveBaseline->setThickness(_plotSettings->getCurveThickness(T1_ENV_BASELINE));
	addLayer(curveBaseline);
	curveBaseline->setColour(_plotSettings->getCurveColour(T1_ENV_BASELINE));
	curveBaseline->setTransparencyPerc(_plotSettings->getCurveTransparencyPerc(T1_ENV_BASELINE));
	curveBaseline->setShow(true);
	
	CRGLText *graphTexts = new CRGLText();
	graphTexts->setTile(plotTile);

	CString ys;
	int nums = ys.LoadString(IDS_DEGC_SEC);
	graphTexts->setTexts(timeAxisLabel,_T(""),ys,title);
	addLayer(graphTexts);
}

float CGraphBtnT1Env::getYAutoMax(void)
{
	FLOAT max,avg;
	ASSERT(dataSet);
	
	max = dataSet->getT1EnvVector()->size() > 0 ? 
		*max_element(dataSet->getT1EnvVector()->begin(), dataSet->getT1EnvVector()->end()) : .0f;
	avg = dataSet->getT1EnvVector()->size() > 0 ?
		(float)for_each(dataSet->getT1EnvVector()->begin(), dataSet->getT1EnvVector()->end(), Average()) : .0f;
	
	return getMaxYScaleFromMeanAndMax(avg,max);
}


///////////////////////////////////////////////////////////////////////
	
CGraphBtnT0plusT1::CGraphBtnT0plusT1()
{
	dataSet = NULL;
	regSubKey = T0_PLUS_T1;
	curveT0plusT1 = NULL;
	int nums = title.LoadString(IDS_T0_PLUS_T1_);
	
	dualPolarity = false;
}

CGraphBtnT0plusT1::~CGraphBtnT0plusT1()
{
}

void CGraphBtnT0plusT1::updateGraphSettings(void)
{
	CGraphBtnCatheterTemperature::updateGraphSettings();

	//---Additionals here
	CButtonPlotSettings plotSettings(dualPolarity, regSubKey);
	curveT0plusT1->setColour(plotSettings.getCurveColour(regSubKey));
	curveT0plusT1->setTransparencyPerc(plotSettings.getCurveTransparencyPerc(regSubKey));
	curveT0plusT1->setThickness(plotSettings.getCurveThickness(regSubKey));

	render();
}

void CGraphBtnT0plusT1::updateCurveColour(void)
{
	curveT0plusT1->updateBrush();
}

CString CGraphBtnT0plusT1::getTimeAndAmplAt(LONG _x,LONG _y)
{
	return curveT0plusT1->getStringTimeAndAmplAt(_x,_y,_T("degC/s")); 
}

BEGIN_MESSAGE_MAP(CGraphBtnT0plusT1, CGraphBtnCatheterTemperature)
	ON_WM_NCHITTEST()
END_MESSAGE_MAP()

void CGraphBtnT0plusT1::setEventsToShow(UINT _evToShow, bool _onlyShowEvents /*= false*/)
{
	drawOnlyEvents = _onlyShowEvents;
	eventsToShow = _evToShow;
	curveT0plusT1->setEventsToShow(_evToShow);
	InvalidateRect(NULL,TRUE);
	UpdateWindow();
}

/*
Description: Check for data
Returns: true if data are present, false if not
*/
bool CGraphBtnT0plusT1::getHasData(void)
{
	if (!dataSet) return false;
	return dataSet->getSize() ? true : false;
}

void CGraphBtnT0plusT1::setData(CCatheterDataSet *_dataSet)
{
	dataSet = _dataSet;
	ASSERT(dataSet);
	setData(dataSet->getT0plusT1Vector(),dataSet->getT0plusT1VectorTime());
}

void CGraphBtnT0plusT1::setData(vector <FLOAT> *_t,vector <FLOAT> *_tv)
{
	ASSERT(curveT0plusT1);
	curveT0plusT1->setData(_t,_tv);
}

void CGraphBtnT0plusT1::setEvents(CEvents *_eP)
{
	eventsP = _eP;

	ASSERT(curveT0plusT1);
	curveT0plusT1->setEvents(_eP);
}

void CGraphBtnT0plusT1::createCurveSettings(CButtonPlotSettings *_plotSettings)
{
	curveT0plusT1 = new CRGLGraphSparse(T0_PLUS_T1);
	curveT0plusT1->setSparseStepSize(envelopesSparseStep);

	ASSERT(curveT0plusT1);

	_plotSettings->setCurveTitle(regSubKey,title);

	curveT0plusT1->setTile(plotTile);	
	curveT0plusT1->setThickness(_plotSettings->getCurveThickness(regSubKey));

	addLayer(curveT0plusT1);
	curveT0plusT1->setColour(_plotSettings->getCurveColour(regSubKey));
	curveT0plusT1->setTransparencyPerc(_plotSettings->getCurveTransparencyPerc(regSubKey));
	curveT0plusT1->setShow(true);
	
	CRGLText *graphTexts = new CRGLText();
	graphTexts->setTile(plotTile);

	CString ys;
	int nums = ys.LoadString(IDS_DEGC_SEC);
	graphTexts->setTexts(timeAxisLabel,_T(""),ys,title);
	addLayer(graphTexts);
}

float CGraphBtnT0plusT1::getYAutoMax(void)
{
	FLOAT max,avg;
	ASSERT(dataSet);
	
	max = dataSet->getT0plusT1Vector()->size() ? 
		*max_element(dataSet->getT0plusT1Vector()->begin(), dataSet->getT0plusT1Vector()->end()) : .0f;
	avg = dataSet->getT0plusT1Vector()->size() ?
		(float)for_each(dataSet->getT0plusT1Vector()->begin(), dataSet->getT0plusT1Vector()->end(), Average()) : .0f;
	
	return getMaxYScaleFromMeanAndMax(avg,max);
}


LRESULT CGraphBtnT0plusT1::OnNcHitTest(CPoint point)
{
	// TODO: Add your message handler code here and/or call default
	
	ScreenToClient(&point);
	return CGraphBtnCatheterTemperature::OnNcHitTest(point);
}


BEGIN_MESSAGE_MAP(CGraphBtnContactMic, CGraphBtnChannel)
	ON_WM_NCHITTEST()
END_MESSAGE_MAP()


LRESULT CGraphBtnContactMic::OnNcHitTest(CPoint point)
{
	// TODO: Add your message handler code here and/or call default

	return CGraphBtnChannel::OnNcHitTest(point);
}
BEGIN_MESSAGE_MAP(CGraphBodyPos, CGraphBtnChannel)
	ON_WM_NCHITTEST()
END_MESSAGE_MAP()


LRESULT CGraphBodyPos::OnNcHitTest(CPoint point)
{
	// TODO: Add your message handler code here and/or call default

	return CGraphBtnChannel::OnNcHitTest(point);
}
BEGIN_MESSAGE_MAP(CGraphBodyPosXYZ, CGraphBtnChannel)
	ON_WM_NCHITTEST()
END_MESSAGE_MAP()


LRESULT CGraphBodyPosXYZ::OnNcHitTest(CPoint point)
{
	// TODO: Add your message handler code here and/or call default

	return CGraphBtnChannel::OnNcHitTest(point);
}

BEGIN_MESSAGE_MAP(CGraphBtnBelt, CGraphBtnChannel)
	ON_WM_NCHITTEST()
END_MESSAGE_MAP()


LRESULT CGraphBtnBelt::OnNcHitTest(CPoint point)
{
	// TODO: Add your message handler code here and/or call default

	return CGraphBtnChannel::OnNcHitTest(point);
}


BEGIN_MESSAGE_MAP(CGraphBtnActimeter, CGraphBtnChannel)
	ON_WM_NCHITTEST()
END_MESSAGE_MAP()


LRESULT CGraphBtnActimeter::OnNcHitTest(CPoint point)
{
	// TODO: Add your message handler code here and/or call default

	return CGraphBtnChannel::OnNcHitTest(point);
}
BEGIN_MESSAGE_MAP(CGraphBtnAdmittance, CGraphBtnChannel)
	ON_WM_NCHITTEST()
END_MESSAGE_MAP()


LRESULT CGraphBtnAdmittance::OnNcHitTest(CPoint point)
{
	// TODO: Add your message handler code here and/or call default

	return CGraphBtnChannel::OnNcHitTest(point);
}

BEGIN_MESSAGE_MAP(CGraphBtnSingleMic, CGraphBtnChannel)
	ON_WM_NCHITTEST()
END_MESSAGE_MAP()


LRESULT CGraphBtnSingleMic::OnNcHitTest(CPoint point)
{
	// TODO: Add your message handler code here and/or call default

	return CGraphBtnChannel::OnNcHitTest(point);
}
BEGIN_MESSAGE_MAP(CGraphBtnAirMicAll, CGraphBtnChannel)
	ON_WM_NCHITTEST()
END_MESSAGE_MAP()


LRESULT CGraphBtnAirMicAll::OnNcHitTest(CPoint point)
{
	// TODO: Add your message handler code here and/or call default

	return CGraphBtnChannel::OnNcHitTest(point);
}
BEGIN_MESSAGE_MAP(CGraphBtnBattCapacity, CGraphBtnChannel)
	ON_WM_NCHITTEST()
END_MESSAGE_MAP()


LRESULT CGraphBtnBattCapacity::OnNcHitTest(CPoint point)
{
	// TODO: Add your message handler code here and/or call default

	return CGraphBtnChannel::OnNcHitTest(point);
}

BEGIN_MESSAGE_MAP(CGraphBtnCatheterPressure, CGraphBtnChannel)
	ON_WM_NCHITTEST()
END_MESSAGE_MAP()


LRESULT CGraphBtnCatheterPressure::OnNcHitTest(CPoint point)
{
	// TODO: Add your message handler code here and/or call default

	return CGraphBtnChannel::OnNcHitTest(point);
}
BEGIN_MESSAGE_MAP(CGraphBtnCatheterPressurePOES, CGraphBtnCatheterPressure)
	ON_WM_NCHITTEST()
END_MESSAGE_MAP()


LRESULT CGraphBtnCatheterPressurePOES::OnNcHitTest(CPoint point)
{
	// TODO: Add your message handler code here and/or call default

	return CGraphBtnCatheterPressure::OnNcHitTest(point);
}
BEGIN_MESSAGE_MAP(CGraphBtnCatheterPressurePPH, CGraphBtnCatheterPressure)
	ON_WM_NCHITTEST()
END_MESSAGE_MAP()


LRESULT CGraphBtnCatheterPressurePPH::OnNcHitTest(CPoint point)
{
	// TODO: Add your message handler code here and/or call default

	return CGraphBtnCatheterPressure::OnNcHitTest(point);
}

BEGIN_MESSAGE_MAP(CGraphBtnCatheterTemperatureT0, CGraphBtnCatheterTemperature)
	ON_WM_NCHITTEST()
END_MESSAGE_MAP()


LRESULT CGraphBtnCatheterTemperatureT0::OnNcHitTest(CPoint point)
{
	// TODO: Add your message handler code here and/or call default

	return CGraphBtnCatheterTemperature::OnNcHitTest(point);
}
BEGIN_MESSAGE_MAP(CGraphBtnCatheterTemperatureT1, CGraphBtnCatheterTemperature)
	ON_WM_NCHITTEST()
END_MESSAGE_MAP()


LRESULT CGraphBtnCatheterTemperatureT1::OnNcHitTest(CPoint point)
{
	// TODO: Add your message handler code here and/or call default

	return CGraphBtnCatheterTemperature::OnNcHitTest(point);
}

BEGIN_MESSAGE_MAP(CGraphBtnPOESEnv, CGraphBtnCatheterPressure)
	ON_WM_NCHITTEST()
END_MESSAGE_MAP()


LRESULT CGraphBtnPOESEnv::OnNcHitTest(CPoint point)
{
	// TODO: Add your message handler code here and/or call default

	return CGraphBtnCatheterPressure::OnNcHitTest(point);
}
BEGIN_MESSAGE_MAP(CGraphBtnPPHEnv, CGraphBtnCatheterPressure)
	ON_WM_NCHITTEST()
END_MESSAGE_MAP()


LRESULT CGraphBtnPPHEnv::OnNcHitTest(CPoint point)
{
	// TODO: Add your message handler code here and/or call default

	return CGraphBtnCatheterPressure::OnNcHitTest(point);
}
BEGIN_MESSAGE_MAP(CGraphBtnPressGrad, CGraphBtnCatheterPressure)
	ON_WM_NCHITTEST()
END_MESSAGE_MAP()


LRESULT CGraphBtnPressGrad::OnNcHitTest(CPoint point)
{
	// TODO: Add your message handler code here and/or call default

	return CGraphBtnCatheterPressure::OnNcHitTest(point);
}
BEGIN_MESSAGE_MAP(CGraphBtnPulseRate, CGraphBtnChannel)
	ON_WM_NCHITTEST()
END_MESSAGE_MAP()


LRESULT CGraphBtnPulseRate::OnNcHitTest(CPoint point)
{
	// TODO: Add your message handler code here and/or call default

	return CGraphBtnChannel::OnNcHitTest(point);
}
BEGIN_MESSAGE_MAP(CGraphBtnRespFrq, CGraphBtnChannel)
	ON_WM_NCHITTEST()
END_MESSAGE_MAP()


LRESULT CGraphBtnRespFrq::OnNcHitTest(CPoint point)
{
	// TODO: Add your message handler code here and/or call default

	return CGraphBtnChannel::OnNcHitTest(point);
}
BEGIN_MESSAGE_MAP(CGraphBtnSpO2, CGraphBtnChannel)
	ON_WM_NCHITTEST()
END_MESSAGE_MAP()


LRESULT CGraphBtnSpO2::OnNcHitTest(CPoint point)
{
	// TODO: Add your message handler code here and/or call default

	return CGraphBtnChannel::OnNcHitTest(point);
}
BEGIN_MESSAGE_MAP(CGraphBtnT0Env, CGraphBtnCatheterTemperature)
	ON_WM_NCHITTEST()
END_MESSAGE_MAP()


LRESULT CGraphBtnT0Env::OnNcHitTest(CPoint point)
{
	// TODO: Add your message handler code here and/or call default

	return CGraphBtnCatheterTemperature::OnNcHitTest(point);
}
BEGIN_MESSAGE_MAP(CGraphBtnT1Env, CGraphBtnCatheterTemperature)
	ON_WM_NCHITTEST()
END_MESSAGE_MAP()


LRESULT CGraphBtnT1Env::OnNcHitTest(CPoint point)
{
	// TODO: Add your message handler code here and/or call default

	return CGraphBtnCatheterTemperature::OnNcHitTest(point);
}

///////////////////////////////////////////////////////////////////////
//  Resp belt family
///////////////////////////////////////////////////////////////////////
	
CGraphBtnBelt::CGraphBtnBelt()
{
	dataSet = NULL;
	dualPolarity = true;
}

CGraphBtnBelt::~CGraphBtnBelt()
{
}
//
//BEGIN_MESSAGE_MAP(CGraphBtnBelt, CGraphBtnChannel)
//	ON_WM_NCHITTEST()
//END_MESSAGE_MAP()

void CGraphBtnBelt::updateGraphSettings(void)
{
	CGraphBtnChannel::updateGraphSettings();
}


/*
Description: Check for data
Returns: true if data are present, false if not
*/
bool CGraphBtnBelt::getHasData(void)
{
	if (!dataSet) return false;
	return dataSet->getSize() ? true : false;
}

void CGraphBtnBelt::setData(CRespBeltDataSet *_dataSet)
{
}

void CGraphBtnBelt::setData(vector <FLOAT> *_temp,vector <FLOAT> *_tv)
{
}

void CGraphBtnBelt::setEvents(CEvents *_eP)
{
	eventsP = _eP;
}

void CGraphBtnBelt::createCurveSettings(CButtonPlotSettings *_plotSettings)
{
}

float CGraphBtnBelt::getYAutoMax(void)
{
	return 10.0f;
}


///////////////////////////////////////////////////////////////////////
	
CGraphBtnChestBelt::CGraphBtnChestBelt()
{
	dataSet = NULL;
	regSubKey = CHEST_BELT;
	curveC = NULL;
	int nums = title.LoadString(IDS_CHEST_TITLE);
}

BEGIN_MESSAGE_MAP(CGraphBtnChestBelt, CGraphBtnBelt)
	ON_WM_NCHITTEST()
END_MESSAGE_MAP()


LRESULT CGraphBtnChestBelt::OnNcHitTest(CPoint point)
{
	// TODO: Add your message handler code here and/or call default

	return CGraphBtnBelt::OnNcHitTest(point);
}

CGraphBtnChestBelt::~CGraphBtnChestBelt()
{
}

void CGraphBtnChestBelt::updateGraphSettings(void)
{
	CGraphBtnBelt::updateGraphSettings();

	//---Additionals here
	CButtonPlotSettings plotSettings(dualPolarity, regSubKey);
	curveC->setColour(plotSettings.getCurveColour(regSubKey));
	curveC->setTransparencyPerc(plotSettings.getCurveTransparencyPerc(regSubKey));
	curveC->setThickness(plotSettings.getCurveThickness(regSubKey));

	render();
}

void CGraphBtnChestBelt::setEventsToShow(UINT _evToShow, bool _onlyShowEvents /*= false*/)
{
	drawOnlyEvents = _onlyShowEvents;
	eventsToShow = _evToShow;
	curveC->setEventsToShow(_evToShow);
	InvalidateRect(NULL,TRUE);
	UpdateWindow();
}


/*
Description: Check for data
Returns: true if data are present, false if not
*/
bool CGraphBtnChestBelt::getHasData(void)
{
	if (!dataSet) return false;
	return dataSet->getSize() ? true : false;
}

void CGraphBtnChestBelt::setData(CRespBeltDataSet *_dataSet)
{
	dataSet = _dataSet;
	ASSERT(dataSet);
	setData(dataSet->getChestVector(),dataSet->getBeltTime());
}

void CGraphBtnChestBelt::setData(vector <FLOAT> *_temp,vector <FLOAT> *_tv)
{
	ASSERT(curveC);
	curveC->setData(_temp,_tv);
}

void CGraphBtnChestBelt::setEvents(CEvents *_eP)
{	
	eventsP = _eP;

	ASSERT(curveC);
	curveC->setEvents(_eP);
}


void CGraphBtnChestBelt::createCurveSettings(CButtonPlotSettings *_plotSettings)
{
	curveC = new CRGLGraphSparse(CHEST_BELT);

	ASSERT(curveC);

	_plotSettings->setCurveTitle(regSubKey,title);

	curveC->setTile(plotTile);	
	curveC->setThickness(_plotSettings->getCurveThickness(regSubKey));

	addLayer(curveC);
	curveC->setColour(_plotSettings->getCurveColour(regSubKey));
	curveC->setTransparencyPerc(_plotSettings->getCurveTransparencyPerc(regSubKey));
	curveC->setShow(true);
	
	CRGLText *graphTexts = new CRGLText();
	graphTexts->setTile(plotTile);

	CString ys;
	int nums = ys.LoadString(IDS_DEGC_SEC);
	graphTexts->setTexts(timeAxisLabel,_T(""),ys,title);
	addLayer(graphTexts);
}

float CGraphBtnChestBelt::getYAutoMax(void)
{
	ASSERT(dataSet);
	
	FLOAT max = dataSet->getChestVector()->size() > 0 ? 
		*max_element(dataSet->getChestVector()->begin(), dataSet->getChestVector()->end()) : .0f;
	FLOAT min = dataSet->getChestVector()->size() > 0 ? 
		*min_element(dataSet->getChestVector()->begin(), dataSet->getChestVector()->end()) : .0f;
	FLOAT avg = dataSet->getChestVector()->size() > 0 ?
		(float)for_each(dataSet->getChestVector()->begin(), dataSet->getChestVector()->end(), Average()) : .0f;
	
	FLOAT pos = fabs(max);
	FLOAT neg = fabs(min);
	return getMaxYScaleFromMeanAndMax(avg,pos > neg ? pos : neg);
}

///////////////////////////////////////////////////////////////////////
	
CGraphBtnAbdominalBelt::CGraphBtnAbdominalBelt()
{
	dataSet = NULL;
	regSubKey = ABDOM_BELT;
	curveA = NULL;
	int nums = title.LoadString(IDS_ABDOM_TITLE);
}

BEGIN_MESSAGE_MAP(CGraphBtnAbdominalBelt, CGraphBtnBelt)
	ON_WM_NCHITTEST()
END_MESSAGE_MAP()


LRESULT CGraphBtnAbdominalBelt::OnNcHitTest(CPoint point)
{
	// TODO: Add your message handler code here and/or call default

	return CGraphBtnBelt::OnNcHitTest(point);
}

CGraphBtnAbdominalBelt::~CGraphBtnAbdominalBelt()
{
}

void CGraphBtnAbdominalBelt::updateGraphSettings(void)
{
	CGraphBtnBelt::updateGraphSettings();

	//---Additionals here
	CButtonPlotSettings plotSettings(dualPolarity, regSubKey);
	curveA->setColour(plotSettings.getCurveColour(regSubKey));
	curveA->setTransparencyPerc(plotSettings.getCurveTransparencyPerc(regSubKey));
	curveA->setThickness(plotSettings.getCurveThickness(regSubKey));

	render();
}

void CGraphBtnAbdominalBelt::setEventsToShow(UINT _evToShow, bool _onlyShowEvents /*= false*/)
{
	drawOnlyEvents = _onlyShowEvents;
	eventsToShow = _evToShow;
	curveA->setEventsToShow(_evToShow);
	InvalidateRect(NULL,TRUE);
	UpdateWindow();
}

/*
Description: Check for data
Returns: true if data are present, false if not
*/
bool CGraphBtnAbdominalBelt::getHasData(void)
{
	if (!dataSet) return false;
	return dataSet->getSize() ? true : false;
}

void CGraphBtnAbdominalBelt::setData(CRespBeltDataSet *_dataSet)
{
	dataSet = _dataSet;
	ASSERT(dataSet);
	setData(dataSet->getAbdomVector(),dataSet->getBeltTime());
}

void CGraphBtnAbdominalBelt::setData(vector <FLOAT> *_t,vector <FLOAT> *_tv)
{
	ASSERT(curveA);
	curveA->setData(_t,_tv);
}

void CGraphBtnAbdominalBelt::setEvents(CEvents *_eP)
{
	eventsP = _eP;

	ASSERT(curveA);
	curveA->setEvents(_eP);
}

void CGraphBtnAbdominalBelt::createCurveSettings(CButtonPlotSettings *_plotSettings)
{
	curveA = new CRGLGraphSparse(ABDOM_BELT);

	ASSERT(curveA);

	_plotSettings->setCurveTitle(regSubKey,title);

	curveA->setTile(plotTile);	
	curveA->setThickness(_plotSettings->getCurveThickness(regSubKey));

	addLayer(curveA);
	curveA->setColour(_plotSettings->getCurveColour(regSubKey));
	curveA->setTransparencyPerc(_plotSettings->getCurveTransparencyPerc(regSubKey));
	curveA->setShow(true);
	
	CRGLText *graphTexts = new CRGLText();
	graphTexts->setTile(plotTile);

	CString ys;
	int nums = ys.LoadString(IDS_DEGC_SEC);
	graphTexts->setTexts(timeAxisLabel,_T(""),ys,title);
	addLayer(graphTexts);
}

float CGraphBtnAbdominalBelt::getYAutoMax(void)
{
	ASSERT(dataSet);
	
	FLOAT max = dataSet->getAbdomVector()->size() > 0 ? 
		*max_element(dataSet->getAbdomVector()->begin(), dataSet->getAbdomVector()->end()) : .0f;
	FLOAT min = dataSet->getAbdomVector()->size() > 0 ? 
		*min_element(dataSet->getAbdomVector()->begin(), dataSet->getAbdomVector()->end()) : .0f;
	FLOAT avg = dataSet->getAbdomVector()->size() > 0 ?
		(float)for_each(dataSet->getAbdomVector()->begin(), dataSet->getAbdomVector()->end(), Average()) : .0f;
	
	FLOAT pos = fabs(max);
	FLOAT neg = fabs(min);
	return getMaxYScaleFromMeanAndMax(avg,pos > neg ? pos : neg);
}

///////////////////////////////////////////////////////////////////////
	
CGraphBtnCannula::CGraphBtnCannula()
{
	dataSet = NULL;
	regSubKey = CANNULA;
	curveCa = NULL;
	dualPolarity = true;
	int nums = title.LoadString(IDS_CANNULA_TITLE);
}


BEGIN_MESSAGE_MAP(CGraphBtnCannula, CGraphBtnChannel)
	ON_WM_NCHITTEST()
END_MESSAGE_MAP()


LRESULT CGraphBtnCannula::OnNcHitTest(CPoint point)
{
	// TODO: Add your message handler code here and/or call default

	return CGraphBtnChannel::OnNcHitTest(point);
}

CGraphBtnCannula::~CGraphBtnCannula()
{
}

void CGraphBtnCannula::updateGraphSettings(void)
{
	CGraphBtnChannel::updateGraphSettings();

	//---Additionals here
	CButtonPlotSettings plotSettings(dualPolarity, regSubKey);
	curveCa->setColour(plotSettings.getCurveColour(regSubKey));
	curveCa->setTransparencyPerc(plotSettings.getCurveTransparencyPerc(regSubKey));
	curveCa->setThickness(plotSettings.getCurveThickness(regSubKey));

	render();
}

CString CGraphBtnCannula::getTimeAndAmplAt(LONG _x,LONG _y)
{
	return curveCa->getStringTimeAndAmplAt(_x,_y,_T("mBar")); 
}

void CGraphBtnCannula::setEventsToShow(UINT _evToShow, bool _onlyShowEvents /*= false*/)
{
	drawOnlyEvents = _onlyShowEvents;
	eventsToShow = _evToShow;
	curveCa->setEventsToShow(_evToShow);
	InvalidateRect(NULL,TRUE);
	UpdateWindow();
}

/*
Description: Check for data
Returns: true if data are present, false if not
*/
bool CGraphBtnCannula::getHasData(void)
{
	if (!dataSet) return false;
	return dataSet->getSize() ? true : false;
}

void CGraphBtnCannula::setData(CRespBeltDataSet *_dataSet)
{
	dataSet = _dataSet;
	ASSERT(dataSet);
	setData(dataSet->getCannulaVector(),dataSet->getCannulaTime());
}

void CGraphBtnCannula::setData(vector <FLOAT> *_t,vector <FLOAT> *_tv)
{
	ASSERT(curveCa);
	curveCa->setData(_t,_tv);
}

void CGraphBtnCannula::setEvents(CEvents *_eP)
{
	eventsP = _eP;

	ASSERT(curveCa);
	curveCa->setEvents(_eP);
}

void CGraphBtnCannula::createCurveSettings(CButtonPlotSettings *_plotSettings)
{
	curveCa = new CRGLGraphSparse(CANNULA);

	ASSERT(curveCa);

	_plotSettings->setCurveTitle(regSubKey,title);

	curveCa->setTile(plotTile);	
	curveCa->setThickness(_plotSettings->getCurveThickness(regSubKey));

	addLayer(curveCa);
	curveCa->setColour(_plotSettings->getCurveColour(regSubKey));
	curveCa->setTransparencyPerc(_plotSettings->getCurveTransparencyPerc(regSubKey));
	curveCa->setShow(true);
	
	CRGLText *graphTexts = new CRGLText();
	graphTexts->setTile(plotTile);

	CString ys;
	int nums = ys.LoadString(IDS_cmH2O);
	graphTexts->setTexts(timeAxisLabel,_T(""),ys,title);
	addLayer(graphTexts);
}

float CGraphBtnCannula::getYAutoMax(void)
{
	ASSERT(dataSet);
	
	FLOAT max = dataSet->getCannulaVector()->size() > 0 ? 
		*max_element(dataSet->getCannulaVector()->begin(), dataSet->getCannulaVector()->end()) : .0f;
	FLOAT min = dataSet->getCannulaVector()->size() > 0 ? 
		*min_element(dataSet->getCannulaVector()->begin(), dataSet->getCannulaVector()->end()) : .0f;
	FLOAT avg = dataSet->getCannulaVector()->size() > 0 ?
		(float)for_each(dataSet->getCannulaVector()->begin(), dataSet->getCannulaVector()->end(), Average()) : .0f;
	
	FLOAT pos = fabs(max);
	FLOAT neg = fabs(min);
	return getMaxYScaleFromMeanAndMax(avg,pos > neg ? pos : neg);
}


//////////////////////////////////////////////////////////////////////////////////////////

CGraphBtnEventMarker::CGraphBtnEventMarker() 
{
	regSubKey = EVENT_MARKER_GRAPH;
	central = mixed = centralHypo = obstrHypo = obstr = NULL;
	spO2 = rera = arousals = awake = snoring = manual = NULL;
	swallow = imported = excluded =  NULL;
}

CGraphBtnEventMarker::~CGraphBtnEventMarker()
{
}

void CGraphBtnEventMarker::OnMouseMove(UINT nFlags, CPoint point)
{
	if (!m_ttip.m_hWnd) return;

	MSG msg;
	msg.hwnd = m_hWnd;
	msg.message = WM_MOUSEMOVE;
	msg.wParam = 0; // Flags
	msg.lParam = MAKELPARAM(LOWORD(point.x), LOWORD(point.y));
	msg.time = 0;
	msg.pt.x = point.x;
	msg.pt.y = point.y;

	CString s0;
	CString title;
	int nums = title.LoadString(IDS_EVENT_MARKERS);

	if (central->isOver(point)) {
		CString s;
		nums = s.LoadString(IDS_CENTRAL4);
		s0 += s;
		s0 += _T("\n");
	}
	if (mixed->isOver(point)) {
		CString s;
		nums = s.LoadString(IDS_MIXED4);
		s0 += s;
		s0 += _T("\n");
	}
	if (obstr->isOver(point)) {
		CString s;
		nums = s.LoadString(IDS_OBSTR4);
		s0 += s;
		s0 += _T("\n");
	}
	if (obstrHypo->isOver(point)) {
		CString s;
		nums = s.LoadString(IDS_OBSTRHYPO4);
		s0 += s;
		s0 += _T("\n");
	}
	if (centralHypo->isOver(point)) {
		CString s;
		nums = s.LoadString(IDS_CENTRALHYPO4);
		s0 += s;
		s0 += _T("\n");
	}
	if (spO2->isOver(point)) {
		CString s;
		int nums = s.LoadString(IDS_SPO24);
		s0 += s;
		s0 += _T("\n");
	}
	if (rera->isOver(point)) {
		CString s;
		nums = s.LoadString(IDS_RERA4);
		s0 += s;
		s0 += _T("\n");
	}
	if (developerMode && arousals->isOver(point)) {
		CString s;
		nums = s.LoadString(IDS_AROUSAL4);
		s0 += s;
		s0 += _T("\n");
	}
	if (awake->isOver(point)) {
		CString s;
		nums = s.LoadString(IDS_AWAKE4);
		s0 += s;
		s0 += _T("\n");
	}
	if (snoring->isOver(point)) {
		CString s;
		nums = s.LoadString(IDS_SNORING4);
		s0 += s;
		s0 += _T("\n");
	}
	if (manual->isOver(point)) {
		CString s;
		nums = s.LoadString(IDS_EXCLUDED4);
		s0 += s;
		s0 += _T("\n");
	}
	if (imported->isOver(point)) {
		CString s;
		nums =s.LoadString(IDS_IMPORTED4);
		s0 += s;
		s0 += _T("\n");
	}
	if (swallow->isOver(point)) {
		CString s;
		nums = s.LoadString(IDS_SWALLOW2);
		s0 += s;
		s0 += _T("\n");
	}
	if (excluded->isOver(point)) {
		CString s;
		nums = s.LoadString(IDS_EXCLUDED2);
		s0 += s;
		s0 += _T("\n");
	}
	m_ttip.SetTitle(TTI_INFO, title);
	m_ttip.UpdateTipText(s0, this);
	m_ttip.RelayEvent(&msg);
}

void CGraphBtnEventMarker::setEvents(CEvents *_eP)
{
	eventsP = _eP;

	ASSERT(mixed			);
	ASSERT(central			);
	ASSERT(obstr			);
	ASSERT(obstrHypo		);
	ASSERT(centralHypo		);
	ASSERT(spO2				);
	ASSERT(rera				);
	ASSERT(arousals			);
	ASSERT(awake			);
	ASSERT(snoring			);
	ASSERT(manual			);
	ASSERT(swallow			);
	ASSERT(imported			);
	ASSERT(excluded			);

	central->setEvents(_eP);
	mixed->setEvents(_eP);	
	obstr->setEvents(_eP);	
	obstrHypo->setEvents(_eP);
	centralHypo->setEvents(_eP);
	spO2->setEvents(_eP);	
	rera->setEvents(_eP);	
	arousals->setEvents(_eP);
	awake->setEvents(_eP);	
	snoring->setEvents(_eP);
	manual->setEvents(_eP);
	swallow->setEvents(_eP);
	imported->setEvents(_eP);
	excluded->setEvents(_eP);
}

void CGraphBtnEventMarker::doEventGeometry(bool _includeGraph, int _type, bool _withLevel)
{
	ASSERT(mixed);
	ASSERT(central);
	ASSERT(obstr);
	ASSERT(obstrHypo);
	ASSERT(centralHypo);
	ASSERT(spO2);
	ASSERT(rera);
	ASSERT(arousals);
	ASSERT(awake);
	ASSERT(snoring);
	ASSERT(manual);
	ASSERT(swallow);
	ASSERT(imported);
	ASSERT(excluded);

	central->doEventGeometry(_type, _withLevel);
	if (_includeGraph) central->doGeometry();

	mixed->doEventGeometry(_type, _withLevel);
	if (_includeGraph) mixed->doGeometry();

	obstr->doEventGeometry(_type, _withLevel);
	if (_includeGraph) obstr->doGeometry();

	obstrHypo->doEventGeometry(_type, _withLevel);
	if (_includeGraph) obstrHypo->doGeometry();

	centralHypo->doEventGeometry(_type, _withLevel);
	if (_includeGraph) centralHypo->doGeometry();

	spO2->doEventGeometry(_type, _withLevel);
	if (_includeGraph) spO2->doGeometry();

	rera->doEventGeometry(_type, _withLevel);
	if (_includeGraph) rera->doGeometry();

	arousals->doEventGeometry(_type, _withLevel);
	if (_includeGraph) arousals->doGeometry();

	awake->doEventGeometry(_type, _withLevel);
	if (_includeGraph) awake->doGeometry();

	snoring->doEventGeometry(_type, _withLevel);
	if (_includeGraph) snoring->doGeometry();

	manual->doEventGeometry(_type, _withLevel);
	if (_includeGraph) manual->doGeometry();

	swallow->doEventGeometry(_type, _withLevel);
	if (_includeGraph) swallow->doGeometry();

	imported->doEventGeometry(_type, _withLevel);
	if (_includeGraph) imported->doGeometry();

	excluded->doEventGeometry(_type, _withLevel);
	if (_includeGraph) excluded->doGeometry();
}

bool CGraphBtnEventMarker::getHasData(void)
{
	return true; // Alwa
}

BOOL CGraphBtnEventMarker::Create(LPCTSTR lpszCaption, DWORD dwStyle, const RECT& rect, CWnd* pParentWnd, UINT nID)
{
	// TODO: Add your specialized code here and/or call the base class
	int ret = CGraphButton::Create(lpszCaption, dwStyle, rect, pParentWnd, nID);

	if (1 == ret) {
		CButtonPlotSettings plotSettings(dualPolarity,regSubKey,title);

		backgr = new CRGLBackground();
		backgr->setColour(plotSettings.getBackgrColour());
		addLayer(backgr);
		plotTile = new CRGLTile(
			D2D1::RectF(.0f,100.0f,100.0f,.0f),     // %
			D2D1::RectF(5.0f,100.0f,99.0f,.0f),    // %   
			D2D1::RectF(.0f,10.0f,1.0f,.0f),         // Prelim
			D2D1::RectF(.0f,10.0f,1.0f,.0f));        // Prelim

		addTile(plotTile);

		createCurveSettings(&plotSettings);

		plotTile->setPlotRectXScaled(plotSettings.getXmin(),plotSettings.getXmax());

		axes = new CRGLAxes();
		axes->setTile(plotTile);
		axes->setShowXlabels(false);
		axes->setShowYlabels(false);
		axes->setShowTicks(false);
		axes->setYTick(1.0f);
		axes->setYlabelInterval(1);
		axes->setXlabelInterval(plotSettings.getXlabelInterval());
		axes->setXTick(plotSettings.getXtick());
		axes->setColour(.0f,.0f,.0f);
		addLayer(axes);
		runXAxisWizard(plotSettings.getXmin(),plotSettings.getXmax());

		CString txt = _T(" "); // No body text

		if (!m_ttipEvent.Create(this)) {
			TRACE0("Unable to create tip window for EventMarker button.");
		}
		else {
			if (!m_ttipEvent.AddTool(this, LPCTSTR(txt))) {
				TRACE0("Unable to add tip for the EventMarker button.");
			}
			else {
				m_ttipEvent.SetTipBkColor(RGB(255, 255, 100));
				m_ttipEvent.Activate(TRUE);
			}
		}
	}

	return ret;
	//return CGraphButton::Create(lpszCaption, dwStyle, rect, pParentWnd, nID);
}

void CGraphBtnEventMarker::createCurveSettings(CButtonPlotSettings *_plotSettings)
{
	mixed		= new CRGLGraphEventMarkers(MIXED_MARKER);
	central		= new CRGLGraphEventMarkers(CENTR_MARKER);
	obstr		= new CRGLGraphEventMarkers(OBSTR_MARKER);
	obstrHypo	= new CRGLGraphEventMarkers(OBHYP_MARKER);
	centralHypo	= new CRGLGraphEventMarkers(CEHYP_MARKER);
	spO2		= new CRGLGraphEventMarkers(SPO2__MARKER);
	rera		= new CRGLGraphEventMarkers(RERA__MARKER);
	arousals	= new CRGLGraphEventMarkers(AROUS_MARKER);
	awake		= new CRGLGraphEventMarkers(AWAKE_MARKER);
	snoring		= new CRGLGraphEventMarkers(SNORI_MARKER);
	manual		= new CRGLGraphEventMarkers(MANUA_MARKER);
	swallow		= new CRGLGraphEventMarkers(SWALL_MARKER);
	imported	= new CRGLGraphEventMarkers(IMPOR_MARKER);
	excluded	= new CRGLGraphExcludedEventMarker(EXCLU_MARKER);

	ASSERT(mixed			);
	ASSERT(central			);
	ASSERT(obstr			);
	ASSERT(obstrHypo		);
	ASSERT(centralHypo		);
	ASSERT(spO2				);
	ASSERT(rera				);
	ASSERT(arousals			);
	ASSERT(awake			);
	ASSERT(snoring			);
	ASSERT(manual			);
	ASSERT(swallow			);
	ASSERT(imported			);

	central->setTile(plotTile);	
	mixed->setTile(plotTile);	
	obstr->setTile(plotTile);	
	obstrHypo->setTile(plotTile);	
	centralHypo->setTile(plotTile);	
	spO2->setTile(plotTile);	
	rera->setTile(plotTile);	
	arousals->setTile(plotTile);	
	awake->setTile(plotTile);	
	snoring->setTile(plotTile);	
	manual->setTile(plotTile);	
	swallow->setTile(plotTile);
	imported->setTile(plotTile);
	excluded->setTile(plotTile);

	central->setThickness(_plotSettings->getCurveThickness(			CENTR_MARKER ));
	mixed->setThickness(_plotSettings->getCurveThickness(			MIXED_MARKER ));
	obstr->setThickness(_plotSettings->getCurveThickness(			OBSTR_MARKER ));
	obstrHypo->setThickness(_plotSettings->getCurveThickness(		OBHYP_MARKER ));
	centralHypo->setThickness(_plotSettings->getCurveThickness(		CEHYP_MARKER ));
	spO2->setThickness(_plotSettings->getCurveThickness(			SPO2__MARKER ));
	rera->setThickness(_plotSettings->getCurveThickness(			RERA__MARKER ));
	arousals->setThickness(_plotSettings->getCurveThickness(		AROUS_MARKER ));
	awake->setThickness(_plotSettings->getCurveThickness(			AWAKE_MARKER ));
	snoring->setThickness(_plotSettings->getCurveThickness(			SNORI_MARKER ));
	manual->setThickness(_plotSettings->getCurveThickness(			MANUA_MARKER ));
	swallow->setThickness(_plotSettings->getCurveThickness(			SWALL_MARKER));
	imported->setThickness(_plotSettings->getCurveThickness(		IMPOR_MARKER));
	//--Excluded is set inside object

	central->setColour(centralCol[0]		,centralCol[1]		,centralCol[2]		);
	mixed->setColour(mixedCol[0]			,mixedCol[1]		,mixedCol[2]		);
	obstr->setColour(obstrCol[0]			,obstrCol[1]		,obstrCol[2]		);
	obstrHypo->setColour(obstrHypCol[0]		,obstrHypCol[1]		,obstrHypCol[2]		);
	centralHypo->setColour(centrHypCol[0]	,centrHypCol[1]		,centrHypCol[2]		);	
	spO2->setColour(spO2Col[0]				,spO2Col[1]			,spO2Col[2]			);
	rera->setColour(reraCol[0]				,reraCol[1]			,reraCol[2]			);
	arousals->setColour(arousalCol[0]		,arousalCol[1]		,arousalCol[2]		);
	awake->setColour(awakeCol[0]			,awakeCol[1]		,awakeCol[2]		);
	snoring->setColour(snoringCol[0]		,snoringCol[1]		,snoringCol[2]		);
	manual->setColour(manualCol[0]			,manualCol[1]		,manualCol[2]		);
	swallow->setColour(swallowCol[0]		,swallowCol[1]		,swallowCol[2]		);
	imported->setColour(importedCol[0]		,importedCol[1]		,importedCol[2]		);
	excluded->setColour(excludedCol[0]		, excludedCol[1]	, excludedCol[2]	);
	
	float delta = 10.0f / 15;
	float yy = 10.0f - delta / 2.0f;
	float yyExcl = 5.0f; // In centre
	mixed->setYVal(yy);			// 1
	yy -= delta;
	central->setYVal(yy);		// 2
	yy -= delta;
	obstr->setYVal(yy);			// 3
	yy -= delta;
	obstrHypo->setYVal(yy);		// 4
	yy -= delta;
	centralHypo->setYVal(yy);	// 5
	yy -= delta;
	spO2->setYVal(yy);			// 6
	yy -= delta;
	rera->setYVal(yy);			// 7
	yy -= delta;
	arousals->setYVal(yy);		// 8
	yy -= delta;
	awake->setYVal(yy);			// 9
	yy -= delta;
	snoring->setYVal(yy);		// 10
	yy -= delta;
	swallow->setYVal(yy);		// 11
	yy -= delta;
	manual->setYVal(yy);		// 12
	yy -= delta;
	imported->setYVal(yy);		// 13
	yy -= delta;
	excluded->setYVal(yyExcl);		// 14
	
	mixed->			setEventType(evMix,true);
	central->		setEventType(evCentral);
	obstr->			setEventType(evObstr,true);
	obstrHypo->		setEventType(evObstrHypo,true);
	centralHypo->	setEventType(evCentralHypo);
	spO2->			setEventType(evSpO2);
	rera->			setEventType(evRERA,true);
	arousals->		setEventType(evArousal);
	awake->			setEventType(evAwake);
	snoring->		setEventType(evSnoringAllLevels,true);
	manual->		setEventType(evManual);
	swallow->		setEventType(evSwallow);
	imported->		setEventType(evImported);
	excluded->		setEventType(evExcluded);

	addLayer(mixed			);
	addLayer(central		);
	addLayer(obstr			);
	addLayer(obstrHypo		);
	addLayer(centralHypo	);
	addLayer(spO2			);
	addLayer(rera			);
	addLayer(arousals		);
	addLayer(awake			);
	addLayer(snoring		);
	addLayer(manual			);
	addLayer(swallow		);
	addLayer(imported		);
	addLayer(excluded		);

	mixed->setShow(true);
	central->setShow(true);
	obstr->setShow(true);
	obstrHypo->setShow(true);
	centralHypo->setShow(true);
	spO2->setShow(true);
	rera->setShow(true);
	arousals->setShow(developerMode ? true : false);
	awake->setShow(true);
	snoring->setShow(true);
	manual->setShow(true);
	swallow->setShow(true);
	imported->setShow(true);
	excluded->setShow(true);
}

///////////////////////////////////////////////////////////////////////


CGraphBtnTorsoActimeter::CGraphBtnTorsoActimeter()
{
	dataSet = NULL;
	regSubKey = TORSO_ACTIMETER;
	curveActimeter = NULL;
	int nums = title.LoadString(IDS_TORSO_ACTIMETER_TITLE2);
}

CGraphBtnTorsoActimeter::~CGraphBtnTorsoActimeter()
{
}

/*
Description: Get amplitude in engineering units
*/
bool CGraphBtnTorsoActimeter::getEUAmplAt(LONG _x, LONG _y, CString *_s)
{
	CRGLGraph *gP = getGraph();
	if (!gP) return false;
	*_s = gP->getAmplAt(_x, _y);
	return true;
}

void CGraphBtnTorsoActimeter::createCurveSettings(CButtonPlotSettings *_plotSettings)
{
	curveActimeter = new CRGLGraph(regSubKey);
	ASSERT(curveActimeter);

	_plotSettings->setCurveTitle(regSubKey, title);

	//---Curve actimeter
	curveActimeter->setTile(plotTile);
	curveActimeter->setThickness(_plotSettings->getCurveThickness(regSubKey));

	addLayer(curveActimeter);
	curveActimeter->setColour(_plotSettings->getCurveColour(regSubKey));
	curveActimeter->setTransparencyPerc(_plotSettings->getCurveTransparencyPerc(regSubKey));
	curveActimeter->setShow(true);

	CRGLText *graphTexts = new CRGLText();
	graphTexts->setTile(plotTile);

	graphTexts->setTexts(timeAxisLabel, _T(""), _T(" "), title);
	addLayer(graphTexts);
}


//////////////////////////////////////////////////////////////////////

CGraphBtnActimeter::CGraphBtnActimeter()
{
	dataSet = NULL;
	regSubKey = ACTIMETER;
	curveActimeter = NULL;
	int nums = title.LoadString(IDS_ACTIMETER_TITLE2);
}

CGraphBtnActimeter::~CGraphBtnActimeter()
{
}

void CGraphBtnActimeter::updateGraphSettings(void)
{
	CGraphBtnChannel::updateGraphSettings();

	ASSERT(curveActimeter);
	CButtonPlotSettings plotSettings(dualPolarity, regSubKey);
	curveActimeter->setColour(plotSettings.getCurveColour(regSubKey));
	curveActimeter->setTransparencyPerc(plotSettings.getCurveTransparencyPerc(regSubKey));
	curveActimeter->setThickness(plotSettings.getCurveThickness(regSubKey));

	render();
}

void CGraphBtnActimeter::updateCurveColour(void)
{
	curveActimeter->updateBrush();
}

void CGraphBtnActimeter::setEventsToShow(UINT _evToShow, bool _onlyShowEvents /*= false*/)
{
	eventsToShow = _evToShow;
	drawOnlyEvents = _onlyShowEvents;
	curveActimeter->setEventsToShow(_evToShow);
	InvalidateRect(NULL, TRUE);
	UpdateWindow();
}

/*
Description: Check for data
Returns: true if data are present, false if not
*/
bool CGraphBtnActimeter::getHasData(void)
{
	if (!dataSet) return false;
	return dataSet->getSize() ? true : false;
}

void CGraphBtnActimeter::setData(CActimeterDataSet *_dataSet)  // , CActimeterDataSet *_torsoActimeterDataSet)
{
	dataSet = _dataSet; 

	//---Use processed data
	if (dataSet) 
		setData(dataSet->getWindowedAcVector(), dataSet->getWindowedAcTimeVector());
	
}

void CGraphBtnActimeter::setData(vector <FLOAT> *_act, vector <FLOAT> *_tv)
{
	ASSERT(curveActimeter);
	curveActimeter->setData(_act, _tv);
}

void CGraphBtnActimeter::setEvents(CEvents *_eP)
{
	eventsP = _eP;

	ASSERT(curveActimeter);
	curveActimeter->setEvents(_eP);
}

void CGraphBtnActimeter::createCurveSettings(CButtonPlotSettings *_plotSettings)
{
	curveActimeter = new CRGLGraph(regSubKey);
	ASSERT(curveActimeter);

	_plotSettings->setCurveTitle(regSubKey, title);

	//---Curve actimeter
	curveActimeter->setTile(plotTile);
	curveActimeter->setThickness(_plotSettings->getCurveThickness(regSubKey));

	addLayer(curveActimeter);
	curveActimeter->setColour(_plotSettings->getCurveColour(regSubKey));
	curveActimeter->setTransparencyPerc(_plotSettings->getCurveTransparencyPerc(regSubKey));
	curveActimeter->setShow(true);

	CRGLText *graphTexts = new CRGLText();
	graphTexts->setTile(plotTile);

	CString ys;
	int nums = ys.LoadStringW(IDS_mg2);
	graphTexts->setTexts(timeAxisLabel, _T(""), ys, title);
	addLayer(graphTexts);
}

float CGraphBtnActimeter::getYAutoMax(void)
{
	return DEFAULT_ACTIMETER_MAX;
}

CGraphBtnFlow::CGraphBtnFlow()
{
	dataSet = NULL;
	regSubKey = FLOW;
	curveEnv = NULL;
	curveBaseline = NULL;
	int nums = title.LoadString(IDS_FLOW_TITLE);

	dualPolarity = false;
}

CGraphBtnFlow::~CGraphBtnFlow()
{
}

void CGraphBtnFlow::setData(CCatheterDataSet * _dataSet)
{
	dataSet = _dataSet;
	ASSERT(dataSet);
	setData(dataSet->getBFlowVector(), dataSet->getBFlowBaseline(), dataSet->getBFlowVectorTime());
}

void CGraphBtnFlow::doEventGeometry(bool _includeGraph /*= false*/,
									int _type  /*= -1*/,
									bool _withLevel /* = false*/)
{
	CRGLGraph *gP = getGraph();
	if (!gP) return;

	gP->doEventGeometry(_type, _withLevel);
	if (_includeGraph) {
		curveEnv->doGeometry(true);			// Reload source geometry
		curveBaseline->doGeometry(true);	// Reload source geometry
	}
}

void CGraphBtnFlow::createCurveSettings(CButtonPlotSettings * _plotSettings)
{
	curveEnv = new CRGLGraphSparse(FLOW);
	curveEnv->setSparseStepSize(envelopesSparseStep);
	curveBaseline = new CRGLGraphSparse(FLOW_BASELINE);
	curveBaseline->setSparseStepSize(envelopesSparseStep);

	ASSERT(curveEnv);
	ASSERT(curveBaseline);

	_plotSettings->setCurveTitle(regSubKey, title);

	//---Curve Env
	curveEnv->setTile(plotTile);
	curveEnv->setThickness(_plotSettings->getCurveThickness(regSubKey));
	addLayer(curveEnv);
	curveEnv->setColour(_plotSettings->getCurveColour(regSubKey));
	curveEnv->setTransparencyPerc(_plotSettings->getCurveTransparencyPerc(regSubKey));
	curveEnv->setShow(true);

	//---Curve Env baseline
	curveBaseline->setTile(plotTile);
	curveBaseline->setThickness(_plotSettings->getCurveThickness(FLOW_BASELINE));
	addLayer(curveBaseline);
	curveBaseline->setColour(_plotSettings->getCurveColour(FLOW_BASELINE));
	curveBaseline->setTransparencyPerc(_plotSettings->getCurveTransparencyPerc(FLOW_BASELINE));
	curveBaseline->setShow(true);

	CRGLText *graphTexts = new CRGLText();
	graphTexts->setTile(plotTile);

	CString ys;
	int nums = ys.LoadString(IDS_DEGC_SEC);
	graphTexts->setTexts(timeAxisLabel, _T(""), ys, title);
	addLayer(graphTexts);
}

void CGraphBtnFlow::setData(vector<FLOAT>* _t, vector<FLOAT>* _baseline, vector<FLOAT>* _tv)
{
	ASSERT(curveEnv);
	ASSERT(curveBaseline);
	curveEnv->setData(_t, _tv);
	curveBaseline->setData(_baseline, _tv);
}


//////////////////////////////////////////////////////////////////

CGraphBtnBreathingEfficiency::CGraphBtnBreathingEfficiency()
{
	dataSet = NULL;
	regSubKey = BREATHING_EFFICIENCY;
	curveAdmittance = NULL;
	int nums = title.LoadString(IDS_BREATHING_EFFICIENCY_TITLE);
	dualPolarity = false;
}

CGraphBtnBreathingEfficiency::~CGraphBtnBreathingEfficiency()
{
}

void CGraphBtnBreathingEfficiency::setData(CCatheterDataSet * _dataSet)
{
	dataSet = _dataSet;
	ASSERT(dataSet);
	setData(dataSet->getBEfficiencyVector(), dataSet->getBEfficiencyVectorTime());
}

void CGraphBtnBreathingEfficiency::doEventGeometry(bool _includeGraph /*= false*/,
													int _type  /*= -1*/,
													bool _withLevel /* = false*/)
{
	CRGLGraph *gP = getGraph();
	if (!gP) return;

	gP->doEventGeometry(_type, _withLevel);
	if (_includeGraph) curveAdmittance->doGeometry(true);
}

void CGraphBtnBreathingEfficiency::createCurveSettings(CButtonPlotSettings * _plotSettings)
{
	curveAdmittance = new CRGLGraphSparse(BREATHING_EFFICIENCY);
	curveAdmittance->setSparseStepSize(envelopesSparseStep);

	ASSERT(curveAdmittance);

	_plotSettings->setCurveTitle(regSubKey, title);

	curveAdmittance->setTile(plotTile);
	curveAdmittance->setThickness(_plotSettings->getCurveThickness(regSubKey));

	addLayer(curveAdmittance);
	curveAdmittance->setColour(_plotSettings->getCurveColour(regSubKey));
	curveAdmittance->setTransparencyPerc(_plotSettings->getCurveTransparencyPerc(regSubKey));
	curveAdmittance->setShow(true);

	CRGLText *graphTexts = new CRGLText();
	graphTexts->setTile(plotTile);

	graphTexts->setTexts(timeAxisLabel, _T(""), _T(""), title);
	addLayer(graphTexts);
}

void CGraphBtnBreathingEfficiency::setData(vector<FLOAT>* _adm, vector<FLOAT>* _tv)
{
	ASSERT(curveAdmittance);
	curveAdmittance->setData(_adm, _tv);
}

////////////////////////////////////////////////////////////////////

CGraphBtnChestBeltEnv::CGraphBtnChestBeltEnv()
{
}

BEGIN_MESSAGE_MAP(CGraphBtnChestBeltEnv, CGraphBtnBelt)
	ON_WM_NCHITTEST()
END_MESSAGE_MAP()


LRESULT CGraphBtnChestBeltEnv::OnNcHitTest(CPoint point)
{
	// TODO: Add your message handler code here and/or call default

	return CGraphBtnBelt::OnNcHitTest(point);
}

CGraphBtnChestBeltEnv::~CGraphBtnChestBeltEnv()
{
}

void CGraphBtnChestBeltEnv::updateGraphSettings(void)
{
}

float CGraphBtnChestBeltEnv::getYAutoMax(void)
{
	return 0.0f;
}

void CGraphBtnChestBeltEnv::setData(CRespBeltDataSet * _dataSet)
{
}

void CGraphBtnChestBeltEnv::setEvents(CEvents * _eP)
{
}

bool CGraphBtnChestBeltEnv::getHasData(void)
{
	return false;
}

void CGraphBtnChestBeltEnv::setEventsToShow(UINT _evToShow, bool _onlyShowEvents)
{
}

void CGraphBtnChestBeltEnv::setData(vector<FLOAT>* _p, vector<FLOAT>* _tv)
{
}

void CGraphBtnChestBeltEnv::createCurveSettings(CButtonPlotSettings * _plotSettings)
{
}

///////////////////////////////////////////////////////////

CGraphBtnAbdominalBeltEnv::CGraphBtnAbdominalBeltEnv()
{
}

BEGIN_MESSAGE_MAP(CGraphBtnAbdominalBeltEnv, CGraphBtnBelt)
	ON_WM_NCHITTEST()
END_MESSAGE_MAP()


LRESULT CGraphBtnAbdominalBeltEnv::OnNcHitTest(CPoint point)
{
	// TODO: Add your message handler code here and/or call default

	return CGraphBtnBelt::OnNcHitTest(point);
}

CGraphBtnAbdominalBeltEnv::~CGraphBtnAbdominalBeltEnv()
{
}

void CGraphBtnAbdominalBeltEnv::updateGraphSettings(void)
{
}

float CGraphBtnAbdominalBeltEnv::getYAutoMax(void)
{
	return 0.0f;
}

void CGraphBtnAbdominalBeltEnv::setData(CRespBeltDataSet * _dataSet)
{
}

void CGraphBtnAbdominalBeltEnv::setEvents(CEvents * _eP)
{
}

bool CGraphBtnAbdominalBeltEnv::getHasData(void)
{
	return false;
}

void CGraphBtnAbdominalBeltEnv::setEventsToShow(UINT _evToShow, bool _onlyShowEvents)
{
}

void CGraphBtnAbdominalBeltEnv::setData(vector<FLOAT>* _p, vector<FLOAT>* _tv)
{
}

void CGraphBtnAbdominalBeltEnv::createCurveSettings(CButtonPlotSettings * _plotSettings)
{
}

////////////////////////////////////////////////////////////

CGraphBtnCannulaEnv::CGraphBtnCannulaEnv()
{
}

CGraphBtnCannulaEnv::~CGraphBtnCannulaEnv()
{
}

BEGIN_MESSAGE_MAP(CGraphBtnCannulaEnv, CGraphBtnCannula)
	ON_WM_NCHITTEST()
END_MESSAGE_MAP()


LRESULT CGraphBtnCannulaEnv::OnNcHitTest(CPoint point)
{
	// TODO: Add your message handler code here and/or call default

	return CGraphBtnCannula::OnNcHitTest(point);
}

void CGraphBtnCannulaEnv::updateGraphSettings(void)
{
}

float CGraphBtnCannulaEnv::getYAutoMax(void)
{
	return 0.0f;
}

void CGraphBtnCannulaEnv::setData(CRespBeltDataSet * _dataSet)
{
}

void CGraphBtnCannulaEnv::setEvents(CEvents * _eP)
{
}

bool CGraphBtnCannulaEnv::getHasData(void)
{
	return false;
}

void CGraphBtnCannulaEnv::setEventsToShow(UINT _evToShow, bool _onlyShowEvents)
{
}

CString CGraphBtnCannulaEnv::getTimeAndAmplAt(LONG _x, LONG _y)
{
	return CString();
}

void CGraphBtnCannulaEnv::setData(vector<FLOAT>* _p, vector<FLOAT>* _tv)
{
}

void CGraphBtnCannulaEnv::createCurveSettings(CButtonPlotSettings * _plotSettings)
{
}
