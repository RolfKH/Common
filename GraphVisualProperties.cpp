#include "stdafx.h"
#include "GraphVisualProperties.h"
#include "RegDefs.h"


/*!
* @author		RKH
* @date		06.06.2017
* @copyright (C)Spiro Medical AS 2013 - 2017
*/

CCurvePlotSettings::CCurvePlotSettings(CString _nameInReg) : 
	nameInReg(_nameInReg),needSave(false),title(_T(""))
{
	col = RGB(0,0,0);
	thickness = 1.0f;
	transparencyPerc = 100;
}

CCurvePlotSettings::CCurvePlotSettings(CString _nameInReg,CString _title) : 
	nameInReg(_nameInReg),needSave(false),title(_title)
{
	col = RGB(0,0,0);
	thickness = 1.0f;
	transparencyPerc = 100;
}

CCurvePlotSettings::~CCurvePlotSettings()
{
}

CString CCurvePlotSettings::getTitle(void)
{
	return title;
}

CString CCurvePlotSettings::getNameInReg(void)
{
	return nameInReg;
}

void CCurvePlotSettings::setThickness(float _th)
{
	thickness = _th;
}
void CCurvePlotSettings::setTransparencyPerc(int _perc)
{
	transparencyPerc = _perc;
}
void CCurvePlotSettings::setColour(COLORREF _c)
{
	col = _c;
}

double CCurvePlotSettings::getThickness(void)
{
	return thickness;
}

int CCurvePlotSettings::getTransparencyPerc(void)
{
	return (int) transparencyPerc;
}

double * CCurvePlotSettings::getThicknessP(void)
{
	needSave = true;
	return &thickness;
}

long * CCurvePlotSettings::getTransparencyPercP(void)
{
	needSave = true;
	return &transparencyPerc;
}

COLORREF CCurvePlotSettings::getColour(void)
{
	return col;
}

COLORREF * CCurvePlotSettings::getColourP(void)
{
	needSave = true;
	return &col;
}

void CCurvePlotSettings::toReg(CString _key)
{
	if (needSave) {
		CXTPRegistryManager *reg = new CXTPRegistryManager;
		reg->SetRegistryKey(COMPANY_NAME,KEY_GRAPHICS);
		CString key = _key;
		key += _T("\\");
		key += nameInReg;
		BOOL OK = reg->WriteProfileColor(key,CURVE_COLOUR,&col);
		ASSERT(OK);
		int t = (int) transparencyPerc;
		OK = reg->WriteProfileInt(key,TRANSPARENCY,t);
		ASSERT(OK);
		double dbl = (double) thickness;
		OK = reg->WriteProfileDouble(key,THICKNESS,&dbl);
		ASSERT(OK);
		OK = reg->WriteProfileString(key,TITLE,title);
		ASSERT(OK);
		delete reg;
	}
}

void CCurvePlotSettings::setTitle(CString _title)
{
	title = _title;
	TRACE(_T("Title %s\n"), title);
	needSave = true;
}

void CCurvePlotSettings::setNeedToSave(bool _set)
{
	needSave = _set;
}

bool CCurvePlotSettings::fromReg(CString _key)
{
	CString key = _key;
	key += _T("\\");
	key += nameInReg;

	CXTPRegistryManager *reg = new CXTPRegistryManager;
	reg->SetRegistryKey(COMPANY_NAME,KEY_GRAPHICS);

	//---Test registry
	double dblTest;
	BOOL OKTest = reg->GetProfileDouble(key,THICKNESS,&dblTest);
	if (!OKTest) {
		delete reg;	
		reg = new CXTPRegistryManager(HKEY_LOCAL_MACHINE);
		reg->SetRegistryKey(COMPANY_NAME,KEY_GRAPHICS);
		needSave = true;
	}

	bool ret = true;
	BOOL OK = reg->GetProfileColor(key,CURVE_COLOUR,&col);
	ret = FALSE == OK ? false : ret;
	transparencyPerc = (long) reg->GetProfileInt(key,TRANSPARENCY,100);
	double dbl;
	OK = reg->GetProfileDouble(key,THICKNESS,&dbl);
	ret = FALSE == OK ? false : ret;
	thickness = (float) dbl;

	CString oldTitle = reg->GetProfileString(key,TITLE,_T(""));
	if (oldTitle != title) ret = false;

	if (title.IsEmpty()) title = oldTitle;

	delete reg;

	return ret;
}


//////////////////////////////////////////////////////////////

CButtonPlotSettings::CButtonPlotSettings(bool _dualPolarity,CString _regSubkey,CString _title) :
	dualPolarity(_dualPolarity),needSave(false),title(_title)
{
	regSubKey = GRAPH_BUTTONS;
	regSubKey += _T("\\");
	regSubKey += _regSubkey;

	background = RGB(255,255,255);

	x.labelInterval = 2;
	x.min = .0f;
	x.max = 100.0f;
	x.tick = 2.0f;
	
	y.labelInterval = 2;
	y.min = dualPolarity ? - 100.0f : .0f;
	y.max = 100.0f;
	y.tick = 10.0f;

	fromReg();

	//---Check Title
	CXTRegistryManager *reg = new CXTRegistryManager;
	reg->SetRegistryKey(COMPANY_NAME,KEY_GRAPHICS);

	//---Test registry
	CString oldTitleTest = reg->GetProfileString(regSubKey,TITLE,_T("Fail"));
	if (oldTitleTest == _T("Fail")) {
		delete reg;
		reg = new CXTRegistryManager(HKEY_LOCAL_MACHINE);
		reg->SetRegistryKey(COMPANY_NAME,KEY_GRAPHICS);
	}

	CString oldTitle = reg->GetProfileString(regSubKey,TITLE,_T("Title"));
	if (oldTitle != title) needSave = true;
	delete reg;
}

CButtonPlotSettings::CButtonPlotSettings(bool _dualPolarity,CString _regSubkey) :
	dualPolarity(_dualPolarity),needSave(false)
{
	regSubKey = GRAPH_BUTTONS;
	regSubKey += _T("\\");
	regSubKey += _regSubkey;

	background = RGB(255,255,255);

	x.labelInterval = 2;
	x.min = .0f;
	x.max = 100.0f;
	x.tick = 2.0f;
	
	y.labelInterval = 2;
	y.min = dualPolarity ? - 100.0f : .0f;
	y.max = 100.0f;
	y.tick = 10.0f;
	
	fromReg();

	//---Title
	CXTRegistryManager *reg = new CXTRegistryManager;
	reg->SetRegistryKey(COMPANY_NAME,KEY_GRAPHICS);

	//---Test registry
	CString oldTitleTest = reg->GetProfileString(regSubKey,TITLE,_T("Fail"));
	if (oldTitleTest == _T("Fail")) {
		delete reg;
		reg = new CXTRegistryManager(HKEY_LOCAL_MACHINE);
		reg->SetRegistryKey(COMPANY_NAME,KEY_GRAPHICS);
	}

	title = reg->GetProfileString(regSubKey,TITLE,_T(""));
	delete reg;
}

void CButtonPlotSettings::fromReg(void)
{
	CXTRegistryManager *reg = new CXTRegistryManager;
	reg->SetRegistryKey(COMPANY_NAME,KEY_GRAPHICS);

	//---Test registry
	COLORREF colTest;
	BOOL okTest = reg->GetProfileColor(regSubKey,BACKGROUND_COLOUR,&colTest);
	if (!okTest) {
		delete reg;
		reg = new CXTRegistryManager(HKEY_LOCAL_MACHINE);
		reg->SetRegistryKey(COMPANY_NAME,KEY_GRAPHICS);
	}

	BOOL OK = reg->GetProfileColor(regSubKey,BACKGROUND_COLOUR,&background);
	if (!OK) {
		needSave = true;
	}
	double dbl;

	//---Read curves
	CStringArray kArray;
	reg->EnumKeys(regSubKey,kArray);
	int num = kArray.GetCount();
	for (int i = 0 ; i < num ; i++) {
		CCurvePlotSettings *cP = new CCurvePlotSettings(kArray.GetAt(i));
		bool ok = cP->fromReg(regSubKey);
		needSave = false == ok ? true : needSave;
		curves.Add(cP);
	}
	needSave = 0 == num ? true : needSave;

	//---Axes
	OK = reg->GetProfileDouble(regSubKey,MINIMUM_X,&dbl);
	if (OK) x.min = (float) dbl;
	else needSave = true;
	OK = reg->GetProfileDouble(regSubKey,MAXIMUM_X,&dbl);
	if (OK) x.max = (float) dbl;
	else needSave = true;
	OK = reg->GetProfileDouble(regSubKey,TICK_X,&dbl);
	if (OK) x.tick = (float) dbl;
	else needSave = true;
	x.labelInterval = reg->GetProfileInt(regSubKey,LABEL_INTERVAL_X,2);
	
	OK = reg->GetProfileDouble(regSubKey,MINIMUM_Y,&dbl);
	if (OK) y.min = (float) dbl;
	else needSave = true;
	OK = reg->GetProfileDouble(regSubKey,MAXIMUM_Y,&dbl);
	if (OK) y.max = (float) dbl;
	else needSave = true;
	OK = reg->GetProfileDouble(regSubKey,TICK_Y,&dbl);
	if (OK) y.tick = (float) dbl;
	else needSave = true;
	y.labelInterval = reg->GetProfileInt(regSubKey,LABEL_INTERVAL_Y,2);
	delete reg;
}

CButtonPlotSettings::~CButtonPlotSettings()
{
	if (needSave) {		
		CXTRegistryManager *reg = new CXTRegistryManager;

		reg->SetRegistryKey(COMPANY_NAME,KEY_GRAPHICS);
		BOOL OK = reg->WriteProfileColor(regSubKey,BACKGROUND_COLOUR,&background);
		ASSERT(OK);

		//---Title
		OK = reg->WriteProfileString(regSubKey,TITLE,title);
		ASSERT(OK);

		//---Axes
		double dbl = (double) x.min;
		OK = reg->WriteProfileDouble(regSubKey,MINIMUM_X,&dbl);
		ASSERT(OK);
		dbl = (double) x.max;
		OK = reg->WriteProfileDouble(regSubKey,MAXIMUM_X,&dbl);
		ASSERT(OK);
		dbl = (double) x.tick;
		OK = reg->WriteProfileDouble(regSubKey,TICK_X,&dbl);
		ASSERT(OK);
		OK = reg->WriteProfileInt(regSubKey,LABEL_INTERVAL_X,x.labelInterval);
		
		dbl = (double) y.min;
		OK = reg->WriteProfileDouble(regSubKey,MINIMUM_Y,&dbl);
		ASSERT(OK);
		dbl = (double) y.max;
		OK = reg->WriteProfileDouble(regSubKey,MAXIMUM_Y,&dbl);
		ASSERT(OK);
		dbl = (double) y.tick;
		OK = reg->WriteProfileDouble(regSubKey,TICK_Y,&dbl);
		ASSERT(OK);
		OK = reg->WriteProfileInt(regSubKey,LABEL_INTERVAL_Y,y.labelInterval);
		delete reg;
	}

	int num = curves.GetCount();
	for (int i = num - 1 ; i >= 0 ; i--) {
		CCurvePlotSettings *cP = curves.GetAt(i);

		//---To registry
		if (needSave) 
			cP->setNeedToSave(needSave);
		cP->toReg(regSubKey);

		delete cP;
	}
}

CString CButtonPlotSettings::getRegSubkey(void)
{
	return regSubKey;
}

CString CButtonPlotSettings::getTitle(void)
{
	return title;
}

COLORREF * CButtonPlotSettings::getBackgrColourP(void)
{
	needSave = true;
	return &background;
}

COLORREF CButtonPlotSettings::getBackgrColour(void)
{
	return background;
}

float CButtonPlotSettings::getXmin(void)
{
	return x.min;
}

float CButtonPlotSettings::getYmin(void)
{
	return y.min;
}

float CButtonPlotSettings::getXmax(void)
{
	return x.max;
}

float CButtonPlotSettings::getYmax(void)
{
	return y.max;
}

int CButtonPlotSettings::getXlabelInterval(void)
{
	return x.labelInterval;
}

int CButtonPlotSettings::getYlabelInterval(void)
{
	return y.labelInterval;
}

float CButtonPlotSettings::getXtick(void)
{
	return x.tick;
}

float CButtonPlotSettings::getYtick(void)
{
	return y.tick;
}


void CButtonPlotSettings::setBackgrColour(COLORREF _c)
{
	background = _c;
	needSave = true;
}

void CButtonPlotSettings::setXmin(float _v)
{
	x.min = _v;
	needSave = true;
}

void CButtonPlotSettings::setYmin(float _v)
{
	y.min = _v;
	needSave = true;
}

void CButtonPlotSettings::setXmax(float _v)
{
	x.max = _v;
	needSave = true;
}

void CButtonPlotSettings::setYmax(float _v)
{
	y.max = _v;
	needSave = true;
}

void CButtonPlotSettings::setXlabelInterval(int _i)
{
	x.labelInterval = _i;
	needSave = true;
}

void CButtonPlotSettings::setYlabelInterval(int _i)
{
	y.labelInterval = _i;
	needSave = true;
}

void CButtonPlotSettings::setXtick(float _v)
{
	x.tick = _v;
	needSave = true;
}

void CButtonPlotSettings::setYtick(float _v)
{
	y.tick = _v;
	needSave = true;
}


void CButtonPlotSettings::setCurveThickness(CString _name,float _thickness)
{
	CCurvePlotSettings *cP = getCurve(_name);
	cP->setThickness(_thickness);
	needSave = true;
}

void CButtonPlotSettings::setCurveTransparencyPerc(CString _name,int _trp)
{
	CCurvePlotSettings *cP = getCurve(_name);
	cP->setTransparencyPerc(_trp);
	needSave = true;
}

void CButtonPlotSettings::setCurveColour(CString _name,COLORREF _c)
{
	CCurvePlotSettings *cP = getCurve(_name);
	cP->setColour(_c);
	needSave = true;
}

void CButtonPlotSettings::setCurveTitle(CString _name,CString _title, CString _engineeringUnit)
{
	CCurvePlotSettings *cP = getCurve(_name);
	ASSERT (cP);
	if (!cP) return;

	cP->setTitle(_title);
}

float CButtonPlotSettings::getCurveThickness(CString _name)
{
	CCurvePlotSettings *cP = getCurve(_name);
	ASSERT (cP);
	if (!cP) return .0f;
	
	float th = (float) cP->getThickness();
	return th;
}

int CButtonPlotSettings::getCurveTransparencyPerc(CString _name)
{
	CCurvePlotSettings *cP = getCurve(_name);
	ASSERT (cP);
	if (!cP) return 0;
	
	int tr = cP->getTransparencyPerc();
	return tr;
}

CArray <CCurvePlotSettings *,CCurvePlotSettings *> * CButtonPlotSettings::getCurves(void)
{
	return &curves;
}

COLORREF CButtonPlotSettings::getCurveColour(CString _name)
{
	CCurvePlotSettings *cP = getCurve(_name);
	ASSERT (cP);
	if (!cP) return RGB(0,0,0);
	
	COLORREF c = cP->getColour();
	return c;
}

CCurvePlotSettings *CButtonPlotSettings::getCurve(CString _nameInReg)
{
	int num = curves.GetCount();
	for (int i = 0 ; i < num ; i++) {
		CCurvePlotSettings *cP = curves.GetAt(i);
		if (_nameInReg == cP->getNameInReg()) 
			return cP;
	}
	CCurvePlotSettings *cP = new CCurvePlotSettings(_nameInReg);
	curves.Add(cP);
	return cP;
}