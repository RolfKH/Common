#ifndef GRAPH_VISUAL_PROPERTIES_INCLUDED
#define GRAPH_VISUAL_PROPERTIES_INCLUDED


/*!
* @author		RKH
* @date		06.06.2017
* @copyright (C)Spiro Medical AS 2013 - 2017
*/

#define MAX_THICKNESS		6

const int bodyPositionColoursLevel[][3] = {
	255	,0		,0	,			// levelTypeUpper
	128	,255	,0	,			// LevelTypeLower
	19	,96		,160,			// levelTypeMulti
	128	,128	, 64,			// levelTypeUndef
	192	,192	,192,			// levelTypeSum
	233	,200	,10	,			// levelTypeCount
};

const int bodyPositionColoursPosition[][3] = {
	255	,0		,0	,			// posSupine,
	128	,255	,0	,			// posLeft,
	19	,96		,160,			// posRight,	
	128	,128	, 64,			// posProne,	
	255	,192	,0,				// posUpright,	
	233	,200	,10	,			// posUndefined,
	0	,0		,0	,			// posSum,
	0	,0		,0	,			// posCount
};

typedef struct anAxisDef {
	float min,max;
	float tick;
	int labelInterval;
} AXIS_DEF;

class CCurvePlotSettings
{
public:
	CCurvePlotSettings(CString _nameInReg);
	CCurvePlotSettings(CString _nameInReg,CString _title);
	~CCurvePlotSettings();
	CString getNameInReg(void);
	CString getTitle(void);
	void setThickness(float _th);
	void setTransparencyPerc(int _perc);
	void setColour(COLORREF _c);
	double getThickness(void);
	int getTransparencyPerc(void);
	double *getThicknessP(void);
	long *getTransparencyPercP(void);
	COLORREF getColour(void);
	COLORREF *getColourP(void);
	void toReg(CString _key);
	bool fromReg(CString _key);
	void setNeedToSave(bool _set);
	void setTitle(CString _title);
protected:
	bool needSave;
	CString nameInReg;
	CString title;
	COLORREF col;
	double thickness;
	long transparencyPerc;
};

class CButtonPlotSettings
{
public:
	CButtonPlotSettings(bool _dualPolarity,CString _regSubkey);
	CButtonPlotSettings(bool _dualPolarity,CString _regSubkey,CString _title);
	~CButtonPlotSettings();
	CString getTitle(void);
	CString getRegSubkey(void);
	COLORREF getBackgrColour(void);
	COLORREF *getBackgrColourP(void);
	float getXmin(void);
	float getYmin(void);
	float getXmax(void);
	float getYmax(void);
	int getXlabelInterval(void);
	int getYlabelInterval(void);
	float getXtick(void);
	float getYtick(void);
	
	void setBackgrColour(COLORREF _c);
	void setXmin(float _v);
	void setYmin(float _v);
	void setXmax(float _v);
	void setYmax(float _v);
	void setXlabelInterval(int _i);
	void setYlabelInterval(int _i);
	void setXtick(float _v);
	void setYtick(float _v);

	void setCurveThickness(CString _name,float _thickness);
	void setCurveTransparencyPerc(CString _name,int _trp);
	void setCurveColour(CString _name,COLORREF _c);
	float getCurveThickness(CString _name);
	int getCurveTransparencyPerc(CString _name);
	COLORREF getCurveColour(CString _name);
	CArray <CCurvePlotSettings *,CCurvePlotSettings *> *getCurves(void);
	void setCurveTitle(CString _name,CString _title,CString _engineeringUnit = _T("Unit"));
protected:
	bool dualPolarity;
	void fromReg(void);
	CString title;
	COLORREF background;
	AXIS_DEF x,y;
	CString regSubKey;
	bool needSave;
	CArray <CCurvePlotSettings *,CCurvePlotSettings *> curves;
	CCurvePlotSettings *getCurve(CString _nameinReg);
};


#endif