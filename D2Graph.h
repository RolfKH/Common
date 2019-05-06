/*!
@file		D2Graph.h
@details	Graphic layers Direct2D
@version	1.0
@remarks	Not complete
*/

/*!
* @author		RKH
* @date		06.06.2017
* @copyright (C)Spiro Medical AS 2013 - 2017
*/

#ifndef D2GRAPH_BUTTON2_INCLUDED
#define D2GRAPH_BUTTON2_INCLUDED

#include <d2d1.h>
#include <d2d1helper.h>
#include <Dwrite.h>
#include "AGSMessages.h"
#include "EventEnums.h"
#include "Events.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#endif

#pragma comment(lib,"d2d1.lib")
#pragma comment(lib,"dwrite.lib")

#define MIN_LIMIT_YRANGE		.1f

//---Timer for use with the mouse wheel
#define MWHEEL_TIMER					102
#define MWHEEL_TIMER_INTERVAL			100

//---Interval defining mouse cursor window when picking events
const int deltaX = 5;

//---For mouse control
#define OVER_NOTHING					0x00000000
#define OVER_LEFT_EDGE					0x00000001
#define OVER_RIGHT_EDGE					0x00000002
#define OVER_EDGE						0x00000004
//#define OVER_AREA						0x00000008

#define OVER_CURSOR						0x00000010
#define OVER_EVNT						0x00000020
#define OVER_EVNT_HYPOPNEA				0x00000040
#define OVER_EVNT_SPO2_DROP				0x00000080
#define OVER_EVNT_CENTRALHYPO			0x00000100
#define OVER_EVNT_OBSTRUCTIVEHYPO		0x00000200
#define OVER_EVNT_UNDEFINEDHYPO			0x00000400
#define OVER_EVNT_MIXED					0x00000800
#define OVER_EVNT_CENTRAL				0x00001000
#define OVER_EVNT_OBSTRUCTIVE			0x00002000
#define OVER_EVNT_RERA					0x00004000
#define OVER_EVNT_AROUSAL				0x00008000
#define OVER_EVNT_UPPER					0x00010000
#define OVER_EVNT_SNORING				0x00020000
#define OVER_EVNT_AWAKE					0x00040000
#define OVER_EVNT_MAN_MARKER			0x00080000
#define OVER_EVNT_EXCLUDED				0x00100000
#define OVER_EVNT_SWALLOW				0x00200000
#define OVER_UPPER						0x00400000
#define OVER_LOWER						0x00800000
#define OVER_MULTI						0x01000000
#define OVER_UNDEFINED_LEVEL			0x02000000
#define OVER_UNDEFINED_TYPE				0x04000000
#define OVER_ENTIRE_GRAPH				0x10000000

#define AUTO_Y_MAX_AT		0.95		// 95%
#define AUTO_Y_MEAN_AT		0.75		// 75%

const float defaultMinimumYRange		= .1f;			// Used in most cases, but different for pressure
const float minimumPressureYRange		= 2.0f;		// +/- 2 cmH2O for pressure

#define SPACE_FOR_X_LABELS        30			// reserved for time axis labels
#define SPACE_FOR_EVENT_MARKERS   30			// reserved for time axis labels

const float plotRectLeftPercent = 10.0f;		//!< Left of graph area in % of client rect
const float plotRectRightPercent = 90.0f;		//!< Left of graph area in % of client rect
const float plotRectTopPercent = 95.0f;			//!< Left of graph area in % of client rect
const float plotRectBottomPercent = 5.0f;		//!< Left of graph area in % of client rect

const float EDGE_DETECT_TOLERANCE = .1f;		// 10% of eventLength is used for tolerance during detection of event edges

#include <vector>
using namespace std;

#define _USE_MATH_DEFINES
#include <math.h>

const int CURS_ARROW = 0x00;
const int CURS_MOVE = 0x11;
const int CURS_SCALE = 0x12;
const int CURS_STUDY = 0x04;
const int CURS_SIZEWE = 0x13;
const int CURS_SIZENS = 0x14;
const int CURS_NO = 0x15;
const int CURS_SEE_OUTSIDE_CLIP_REGION = 0x10;

const int cursorControlNone = 0;
const int cursorControlX = 1;
const int cursorControlY = 2;

const int scaleX = 1;
const int scaleY = 2;
const int shiftX = 3;
const int shiftY = 4;

const FLOAT SEC_LIMIT_BETWEEN_GRAPH_AND_FILL = 1500.0f;
const FLOAT maxSecsOnScreen = 36000.0f;		//!< 10 hours
const FLOAT minSecsOnScreen = 10.0f;		//!< 10 seconds

const int SCREEN_LENGTH_FACTOR = 2;		//!< How long time series we will dump to the graphics board compared to how much the user wants to show
const int MAX_SCREEN_LENGTH = 200;		//!< Max hor screen resolution is 200
const int MAX_PLOT_DIST = 3;		//!< Maximum seconds between plot points (to avoid undersampling)

const int maxYTxtLength = 6;

const int PEN_UP = 1;
const int PEN_DOWN = 2;

const float eventTranspLevel = 1.0f; // .4f;		// Transparency of the event fill-ins

const float sparseDensePlotLimit = 1800.0f;			// Longer than 30 minutes, draw sparse (25% of points)
const int defaultSparseStep = 4;
const int envelopesSparseStep = 10;

#define DONT_NEED_AMPL		_T("DontNeedAmpl")

										//---Colours
const float bgCol[][3] = {
	{ .0f,.0f,1.0f },		// Lower is BLUE
	{ 1.0f,.5f,.0f },		// Upper is ORANGE
	{ .0f,1.0f,.0f },		// Multi is GREEN
	{ .0f,.0f,.0f },		// Undefined is GRAY
};

const float bgColWOLevel[][3] = {
	{ .3f,.3f,.0f },		// evArousal,
	{ .8f,.8f,.0f },		// evSpO2,
	{ .0f,.0f,.0f },		// evManual,
	{ .8f,.8f,.0f },		// evCentral,
	{ .8f,.8f,.0f },		// evCentralHypo,
	{ 1.0f,1.0f,.5f },		// evAwake,
	{ .8f,.8f,.0f },		// evStartStop,
	{ 1.0f,1.0f,1.0f },		// evExcluded,
	{ .0f,.0f,.0f },		// evSwallow,
	{ 1.0f,.0f,.0f },		// evImported,
	{ .0f,.0f,.0f },		// evCountWithoutLevel
};

const float cursorWindowOpacity = .4f;

const float opacityWithLevel[] = {
	.7f,		//evMix,
	.7f,		//evObstr,
	.7f,		//evObstrHypo,
	.7f,		//evRERA,
	.5f,		//evSnoringAllLevels,
	.5f,		//evSnoringUpperLevel,
	.5f,		//evSnoringLowerLevel,
	.5f,		//evSnoringMultiLevel,
	.5f,		//evCountWithLevel
};

const float opacityWithoutLevel[] = {
	.9f,	// evArousal,
	.5f,	// evSpO2,
	.9f,	// evManual,
	.7f,	// evCentral,
	.7f,	// evCentralHypo,
	.7f,	// evAwake,
	.5f,	// evStartStop,
	.75f,	// evExcluded,
	.6f,	// evSwallow,
	.5f,	// evImported,
	.5f,	// evCountWithoutLevel
};

//---Cursor colour
const float blackR = .0f;
const float blackG = .0f;
const float blackB = .0f;

typedef struct aTxtDescr {
	CString txt;
	D2D1_POINT_2F p0, p1;
} TEXT_DESCR;

const float fntSize = 12.0f;

const float radToDegree = 57.29578f;  //!< = 180.0f / M_PI

const int RGLLayerTypeGeneric = 1;
const int RGLLayerTypeGraph = 2;
const int RGLLayerTypeGraphFilled = 3;
const int RGLLayerTypeGraphPoints = 4;
const int RGLLayerTypeBackground = 5;
const int RGLLayerTypeAxes = 6;
const int RGLLayerTypeText = 7;
const int RGLLayerTypeGrid = 8;
const int RGLLayerTypeBar = 9;
const int RGLLayerTypeGraphSparse = 10;

const int RGLTileChangeFullRect = 0x01;
const int RGLTileChangePlotRect = 0x02;
const int RGLTileChangeYScaled = 0x04;
const int RGLTileChangeXScaled = 0x10;

const UINT keyLower				= 0x4C;	// L
const UINT keyMulti				= 0x4D;	// M
const UINT keyUpper				= 0x55;	// U

/*!
CRGLTile keeps the full Rect and the plot section in screen units
It is also defined in user engineering units accoring to plotRectScale and fullRectScale
*/
class CRGLTile
{
public:
	CRGLTile(D2D1_RECT_F _fullRect = D2D1::RectF(.0f, 100.0f, 100.0f, .0f),
		D2D1_RECT_F _plotRect = D2D1::RectF(plotRectLeftPercent, plotRectTopPercent, plotRectRightPercent, plotRectBottomPercent),
		D2D1_RECT_F _fullRectScaled = D2D1::RectF(.0f, 1.0f, 1.0f, .0f),
		D2D1_RECT_F _plotRectScaled = D2D1::RectF(.0f, 1.0f, 1.0f, .0f));
	~CRGLTile();
	D2D1_RECT_F getPlotRect(void);
	D2D1_RECT_F getFullRect(void);
	D2D1_RECT_F getPlotRectScaled(void);
	D2D1_RECT_F getFullRectScaled(void);
	FLOAT getPlotRectXScaled(void);
	void getPlotRectYScaled(FLOAT *_bottom, FLOAT *_top);
	void setPlotRectXScaled(FLOAT _left, FLOAT _right);
	void setExtremeXLimits(FLOAT _left, FLOAT _right);
	void getExtremeXLimits(FLOAT *_left, FLOAT *_right);
	virtual void setPlotRectYScaled(FLOAT _origo, FLOAT _max);
	void setPlotRectYMax(FLOAT _max);
	void setPlotRectScaled(D2D1_RECT_F _r);
	int getChangeFlag(void);
	void resetChangeFlag(void);

	bool zoomTime(float _centre, float _ratio);
	bool trackTimeScaleAboutCentre(float _ratio);
	bool trackTimeScale(int _centre, float _ratio);
	bool trackTimeShift(float _width, int _move, float _rightLimit);

	bool scaleVert(float _ratio, bool _dualPolarity);
	bool moveVert(float _height, float _move);
	bool trackY(CPoint _pt);
	void setVertScaleLimits(float _min, float _max);
	void removeVertScaleLimits(void);
	bool isPointInTile(CRect _clientRect, CPoint _pt);
	void mark(bool _on);
	bool getMark(void);
	void setLockTop(bool _on);
	void setMinimumYRange(float _my);
protected:
	float minimumYRange;
	bool marked;
	bool vertLimited;
	bool lockTop;
	float minLimit, maxLimit;
	int changeFlag;
	FLOAT extremeLeft, extremeRight;
	D2D1_RECT_F fullRect;			//!< In % of graph button client rect
	D2D1_RECT_F plotRect;			//!< In % of graph button client rect
	D2D1_RECT_F plotRectScaled;		//!< In user units
	D2D1_RECT_F fullRectScaled;		//!< In user units
};

class CRGLBand
{
public:
	CRGLBand(float _minY, float _maxY, float _r, float _g, float _b, float _transp);
	~CRGLBand();
	virtual void render(ID2D1HwndRenderTarget *hwndRenderTarget);
	virtual void CreateDeviceResources(ID2D1HwndRenderTarget *hwndRenderTarget, ID2D1Factory *_factory,
		D2D1::Matrix3x2F *_matPlotRect);
	virtual void DiscardDeviceResources(void);
	void doGeometry(ID2D1Factory *_factory, D2D1::Matrix3x2F *_matPlotRect);
	void setTile(CRGLTile *_theTileP);

	void makeTransformedGeometry(ID2D1Factory *_factory, D2D1::Matrix3x2F *_matPlotRect);
	void makeSourceGeometry(ID2D1Factory *_factory);
protected:
	float r, g, b;
	float transp;
	float minY, maxY;
	CComPtr<ID2D1SolidColorBrush> m_brush;
	ID2D1PathGeometry *dGeometry;
	ID2D1TransformedGeometry *dTransformedGeometry;
	CRGLTile *theTileP;
};

/*!
CRGLLayer is the plot type; text, axes, data graph etc.
It is here the actual mapping between screen units and user engineering units takes place
*/
class CRGLLayer
{
public:
	CRGLLayer(CString _regSection = _T(""));
	~CRGLLayer();
	virtual void render(void);
	virtual void updateMarkers(void);
	virtual void CreateDeviceResources(ID2D1HwndRenderTarget *hwndRenderTarget, ID2D1Factory *_factory);
	virtual void DiscardDeviceResources(void);
	int getLayerType(void);

	virtual void setTile(CRGLTile *_tile);
	virtual void resize(void);
	void setParent(CWnd *_parentWnd);
	virtual void launchParameterDialog(void);
	virtual CString getName(void);
	void resetTileChangeFlag(void);
	virtual void setColour(double _r, double _g, double _b);
	void setShow(bool _show);
	bool getShow(void);
	void setPageAndSection(int _page, int _section);
	void getPageAndSection(int *_page, int *_section);
	bool scaleY(float _ratio, bool _dualPolarity);
	virtual bool trackY(CPoint _pt);
	CRGLTile *getTheTile(void);
protected:
	int id;
	int page, section;
	CWnd *parentWnd;
	int type;
	ID2D1HwndRenderTarget *mp_target;
	ID2D1Factory *mp_factory;
	CString regSection;
	D2D1::Matrix3x2F matFullRect, iMatFullRect;	//!< Full rect. scaling. After this, full rect is in user units and lower left is origin
	D2D1::Matrix3x2F matPlotRect, iMatPlotRect;	//!< Full rect. scaling. After this, full rect is in user units and lower left is origin
	void setFullRectMatrices(void);
	void setPlotRectMatrices(void);
	virtual void processFullRect(void);
	virtual void processPlotRect(void);
	FLOAT getPlotRectXScaled(void);

	bool show;

	virtual void makeSourceGeometry(void);
	virtual void makeTransformedGeometry(void);
	CRGLTile *myTile;
};

class CRGLBackground : public CRGLLayer
{
public:
	CRGLBackground(CString _regSection = _T(""));
	~CRGLBackground();
	virtual void render(void);
	virtual void CreateDeviceResources(ID2D1HwndRenderTarget *hwndRenderTarget, ID2D1Factory *_factory);
	virtual void DiscardDeviceResources(void);

	virtual void launchParameterDialog(void);
	virtual CString getName(void);
	virtual void setColour(double _r, double _g, double _b);
	virtual void setColour(COLORREF _c);
	virtual void resize(void);
protected:
	CString regSection;
	double br, bg, bb;
};

class CRGLAxes : public CRGLLayer
{
public:
	CRGLAxes(CString _regSection = _T(""), bool _showYlabels = false, bool _showXlabels = false, bool _rightY = false, bool _upperX = false);
	~CRGLAxes();
	virtual void render(bool _hideYlabels);
	virtual void CreateDeviceResources(ID2D1HwndRenderTarget *hwndRenderTarget, ID2D1Factory *_factory);
	virtual void DiscardDeviceResources(void);

	virtual void resize(void);
	virtual void launchParameterDialog(void);
	virtual CString getName(void);
	void setColour(double _br, double _bg, double _bb);
	void setShowXlabels(bool _show);
	void setShowYlabels(bool _show);
	void setShowTicks(bool _show);
	void setXlabelInterval(int _xLabelInterval);
	void setXTick(double _xTick);
	void setYlabelInterval(int _yLabelInterval);
	void setYTick(double _yTick);
	void setYLabelDecimals(int _numY);
	void xAxisWizard(float _from, float _to);			// Sets tick interval and label interval
	void yAxisWizard(float _bottom, float _top);			// Sets tick interval and label interval
															//void yAxisWizard(void);								// Sets tick interval and label interval
															//bool trackTimeScaleAboutCentre(float _ratio);
	void setLabelTexts(CString _one, CString _two, CString _three, CString _four, CString _five);
	void renderEditRect(float _left, float _right);
protected:
	CString regSection;
	int showTicks;
	double ar, ag, ab;
	double athickness, atransparency,editRectTransparency;
	CComPtr<ID2D1SolidColorBrush> m_brush;
	CComPtr<ID2D1SolidColorBrush> m_brushForEditRect;
	CComPtr<ID2D1SolidColorBrush> m_brushForEditFilledRect;
	double xTick, yTick;					// Tick distance in axis units
	int xLabelInterval, yLabelInterval;	// Label interval - number of ticks between each label

	int hRange, mRange, sRange;
	int msRange;

	int yLabelDecimals;
	double tickLength;
	IDWriteTextFormat *pTextFormatX, *pTextFormatY;
	CComPtr <IDWriteFactory> pDWriteFactory;
	CString labelFmtX, labelFmtY;
	CArray <TEXT_DESCR, TEXT_DESCR> xLblArray, yLblArray;
	CStringArray yStringLabelArray;
	bool upperX, rightY;
	bool showYlabels, showXlabels;

	virtual void processFullRect(void);
	virtual void processPlotRect(void);

	ID2D1PathGeometry *aGeometry;
	ID2D1TransformedGeometry *aTransformedGeometry;
	virtual void makeSourceGeometry(void);
	virtual void makeTransformedGeometry(void);

	void generateLabelArray(void);
	void getHMSms(FLOAT _xRange, int *_h, int *_m, int *_s, int *_ms);
};

class CRGLGrid : public CRGLLayer
{
public:
	CRGLGrid(CString _regSection = _T(""));
	~CRGLGrid();
	virtual void render(void);
	virtual void CreateDeviceResources(ID2D1HwndRenderTarget *hwndRenderTarget, ID2D1Factory *_factory);
	virtual void DiscardDeviceResources(void);

	virtual void resize(void);
	virtual void launchParameterDialog(void);
	virtual CString getName(void);
protected:
	D2D1_RECT_F clipRect;
	CString regSection;
	double gr, gg, gb;
	double gthickness, gtransparency;
	CComPtr<ID2D1SolidColorBrush> m_brush;
	double xTick, yTick;

	virtual void processFullRect(void);
	virtual void processPlotRect(void);

	ID2D1PathGeometry *gGeometry;
	ID2D1TransformedGeometry *gTransformedGeometry;
	virtual void makeSourceGeometry(void);
	virtual void makeTransformedGeometry(void);
};

class CRGLText : public CRGLLayer
{
public:
	CRGLText(CString _regSection = _T(""));
	~CRGLText();
	virtual void render(bool _hideYlabels);
	virtual void CreateDeviceResources(ID2D1HwndRenderTarget *hwndRenderTarget, ID2D1Factory *_factory);
	virtual void DiscardDeviceResources(void);
	CString getEngineeringUnit(void);
	CString getTitle(void);

	virtual void launchParameterDialog(void);
	virtual CString getName(void);
	void setTexts(CString _x, CString _yRight, CString _yLeft, CString _title);
	void setXtext(CString _x);
	virtual void setTile(CRGLTile *_tile);
	virtual void resize(void);
	virtual void setColour(double _r, double _g, double _b);
protected:
	CString regSection;
	double tr, tg, tb;
	double ttransparency;
	CComPtr<ID2D1SolidColorBrush> m_brush;
	IDWriteTextFormat *pTextFormatC, *pTextFormatR, *pTextFormatL;
	CComPtr <IDWriteFactory> pDWriteFactory;
	TEXT_DESCR xTxt, yRightTxt, yLeftTxt, title;
	void setCorners(void);

	virtual void processFullRect(void);
	virtual void processPlotRect(void);
};

class CRGLGraph : public CRGLLayer
{
public:
	CRGLGraph(CString _regSection /*= _T("Graph")*/, bool _hideEvents = false);
	~CRGLGraph();
	virtual void render(void);
	virtual void renderEvents(bool _reset = false);
	virtual void updateMarkers(void);
	virtual void updateBrush(CString _parameter = _T(""));
	void setTimeSpanLimit(float _lower, float _upper);
	virtual void CreateDeviceResources(ID2D1HwndRenderTarget *hwndRenderTarget, ID2D1Factory *_factory);
	virtual void DiscardDeviceResources(void);

	bool isOver(CPoint _point);

	virtual void launchParameterDialog(void);
	void setData(vector <FLOAT> *_v, vector <FLOAT> *_tv = NULL); //!< Defines amplitude

	void setData(vector <FLOAT> *_v, vector <FLOAT> *_tv, vector <BOOL> *_penDown); //!< Defines amplitude, time and pencontrol
	void setMMarkerStartStopVector(vector <FLOAT> *_v);  // For use in AGSBrowser
	void setEvents(CEvents *_events);
	void setIDTypeFamily(const UINT _id, const UINT _type, const UINT _family);
	void getIDTypeFamily(UINT *_id, UINT *_type, UINT *_family);
	void setSampleInterval(FLOAT _sint);

	void doGeometry(bool _reloadSourceGeometry = false);
	void doEventGeometry(int _type,bool _withLevel);
	virtual void setCursorWindow(float _start, float _stop);
	float getCursorWindowStart(void);
	float getCursorWindowStop(void);
	virtual void clearData(void);
	virtual CString getName(void);
	virtual void resize(void);
	void setColour(double _r, double _g, double _b);
	void setColour(COLORREF _c);
	void setThickness(float _thick);
	void setTransparency(float _transp);
	void setTransparencyPerc(int _transp);
	virtual bool trackYScale(CPoint _pt, bool _dualPolarity);
	// Not needed virtual bool trackYShift(CPoint _pt);
	FLOAT getMin(FLOAT _timeFrom, FLOAT _timeTo);
	FLOAT getMax(FLOAT _timeFrom, FLOAT _timeTo);
	virtual void setEventsToShow(UINT _evToShow, bool _onlyShowEvents = false);
	float getTheTimeAtPixel(LONG _x);
	virtual CString getAmplAt(LONG _x, LONG _y);
	virtual CString getStringTimeAndAmplAt(LONG _x, LONG _y, CString _unit = _T(""),bool _lineBreak = false);
	CString getStringTimeAt(LONG _x, LONG _y);
	CString getStringAmplAt(LONG _x, LONG _y, CString _unit = _T(""));
	D2D1_POINT_2F getTimeAndAmplAt(LONG _x, LONG _y);
	FLOAT getTimeValAt(LONG _x, LONG _y);
	virtual bool getAmplAt(FLOAT _time, FLOAT *_val);
	static CString gethmsDec(FLOAT _t);
	static CString gethms(FLOAT _t);
	static CString getsecs(FLOAT _t);
	CString getFormattedNumber(FLOAT _v);
	bool getIsOver(CPoint _point, int *_what, int *_where);
	bool isOverEvent(D2D1_POINT_2F _p, UINT _evntType, int *_what, int *_where);
	void moveLeftCursor(int _move);
	void moveRightCursor(int _move);
	virtual void moveEventEdge(CEvnt *_evnt, FLOAT _moveSecs, int _object);
	virtual void changeAnEvent(CEvnt *_evnt, int _command);
	int getRightCursorX(void);
	int getLeftCursorX(void);
	int getScreenXfromTime(FLOAT _time);
	CString getTimeCursorLeftInfo(void);
	CString getTimeCursorRightInfo(void);
	CString getTimeCursorLengthInfo(void);
	virtual CEvnt *findEvent(int _what, FLOAT _atTime, int *_closestEdge);
	void setDebugTag(int _tag);
	void addBand(CRGLBand *_bP); 
	vector <FLOAT> *getDataVector(void);
	vector <FLOAT> *getTimeVector(void);
protected:
	bool hideEvents;
	int debugTag;
	bool showCursorWnd;
	virtual CString getAmplAt(FLOAT _time);
	float upperTimeSpanLimit, lowerTimeSpanLimit;  // Plots only between these (negative means they are not used)
	UINT dataID, dataFamily, dataType;
	D2D1_RECT_F clipRect;
	CString regSection;
	CEvents *eventsP;
	D2D1_POINT_2F leftCursorTop, leftCursorBottom;
	D2D1_POINT_2F rightCursorTop, rightCursorBottom;
	double dr, dg, db, dtransparency, dthickness;

	CComPtr<ID2D1SolidColorBrush> m_brush;
	CComPtr<ID2D1SolidColorBrush> m_brushCursor;
	CComPtr<ID2D1SolidColorBrush> m_brushCursorWindow;
	CComPtr<ID2D1SolidColorBrush> m_opacityBrush;

	CComPtr<ID2D1SolidColorBrush> m_brushStockLevel[bgLevelTypeCount];
	CComPtr<ID2D1SolidColorBrush> m_brushStockEventWOLevel[evCountWithoutLevel];

	virtual void makeSourceGeometry(bool _reload = false);
	void makeCursorGeometries(void);
	virtual void makeTransformedGeometry(void);
	virtual void makeTransformedGeometryForCursors(void);

	void makeSourceGeometryForEventsWithLevel(UINT _type, UINT _level);
	void makeSourceGeometryForEventsWithoutLevel(UINT _type);
	void makeSourceGeometryForManMarkers(void);
	void sinkStartsAndStops(ID2D1GeometrySink *_sink, vector <float> *_startsStopsP);

	UINT eventsToShowG;
	UINT eventsAlreadyShown;

	vector <FLOAT> *dataVector;
	vector <FLOAT> *timeVector;
	vector <BOOL> *penDown;
	vector <FLOAT> *mMarkerStartStopVector;  // Only for use in AGSBrowser

	FLOAT sint; // Sample interval, seconds
	FLOAT oldSourceGeometryLeft, oldSourceGeometryRight;

	ID2D1PathGeometry *dGeometry;
	ID2D1TransformedGeometry *dTransformedGeometry;

	ID2D1PathGeometry *dGeometryCursorLeft;
	ID2D1PathGeometry *dGeometryCursorRight;
	ID2D1PathGeometry *dGeometryCursorTimeWindow;
	ID2D1TransformedGeometry *dTransfGeometryCursorLeft;
	ID2D1TransformedGeometry *dTransfGeometryCursorRight;
	ID2D1TransformedGeometry *dTransfGeometryCursorTimeWindow;

	ID2D1PathGeometry *dGeometryEventWithoutLevel[evCountWithoutLevel];
	ID2D1TransformedGeometry *dTransfGeometryEventWithoutLevel[evCountWithoutLevel];
	ID2D1PathGeometry *dGeometryEventWithLevel[evCountWithLevel][bgLevelTypeCount];
	ID2D1TransformedGeometry *dTransfGeometryEventWithLevel[evCountWithLevel][bgLevelTypeCount];

	void renderEvent(UINT _evntType);

	virtual void processFullRect(void);
	virtual void processPlotRect(void);

	CArray <CRGLBand *, CRGLBand *> bandArray;
};


class CRGLGraphFilled : public CRGLGraph
{
public:
	CRGLGraphFilled(CString _regSection = _T("Graph Filled"));
	~CRGLGraphFilled();
	virtual void render(void);
	bool isOver(CPoint _point);
protected:
	virtual void makeSourceGeometry(bool _reload = false);
	virtual void makeTransformedGeometry(void);
};

class CRGLGraphSparse : public CRGLGraph
{
public:
	CRGLGraphSparse(CString _regSection = _T("Graph Sparse"), bool _hideEvents = false);
	~CRGLGraphSparse();
	virtual void render(void); 
	virtual void DiscardDeviceResources(void);
	void setSparseStepSize(const int _step);
protected:
	int sparseStep;
	ID2D1PathGeometry *dSparseGeometry;
	ID2D1TransformedGeometry *dSparseTransformedGeometry;
	virtual void makeSourceGeometry(bool _reload = false);
	virtual void makeTransformedGeometry(void);
};

class CRGLGraphEventMarkers : public CRGLGraph
{
public:
	CRGLGraphEventMarkers(CString _regSection = _T("Graph Event Markers"));
	~CRGLGraphEventMarkers();
	virtual void render(void);
	bool isOver(CPoint _point);
	void setYVal(const float _yVal);
	void setEventType(UINT _type, bool _withLevel = false);
protected:
	UINT eventEnum;
	bool withLevel;
	float yVal;
	virtual void makeSourceGeometry(bool _reload = false);
	virtual void makeTransformedGeometry(void);
	CString text;
};

class CRGLGraphExcludedEventMarker : public CRGLGraphEventMarkers
{
public:
	CRGLGraphExcludedEventMarker(CString _regSection);
	~CRGLGraphExcludedEventMarker();
protected:
	virtual void makeSourceGeometry(bool _reload = false);
};

class CRGLBar : public CRGLLayer
{
public:
	CRGLBar(CString _regSection = _T(""));
	~CRGLBar();
	virtual void render(void);
	virtual void CreateDeviceResources(ID2D1HwndRenderTarget *hwndRenderTarget, ID2D1Factory *_factory);
	virtual void DiscardDeviceResources(void);

	bool isOver(CPoint _point);

	virtual void launchParameterDialog(void);
	void setData(FLOAT _amplitude); //!< Defines amplitude
	void clearData(void);

	void doGeometry(void);
	virtual void resize(void);
	void setColour(double _r, double _g, double _b);
	void setTransparency(float _transp);
	void setBarGeometry(float _mid, float _width);
protected:
	D2D1_RECT_F clipRect;
	CString regSection;
	double drb, dgb, dbb, dtransparencyb;
	CComPtr<ID2D1SolidColorBrush> m_brush;
	FLOAT midPoint, width, amplitude;

	virtual void makeSourceGeometry(void);
	virtual void makeTransformedGeometry(void);

	ID2D1PathGeometry *dGeometry;
	ID2D1TransformedGeometry *dTransformedGeometry;

	virtual void processFullRect(void);
	virtual void processPlotRect(void);
};


class CGraphButton : public CXTPButton
{
public:
	CGraphButton(CString _regSection = _T(""));
	~CGraphButton();
	virtual void setTitle(CString _title);
	void setHideYlabels(bool _hideYLabels);
	virtual void render(bool _onlyEvents = false, bool _disableHoverIndication = false);
	virtual void updateCurveColour(void);

	void disableEventHoverIndication(void);
	virtual HRESULT CreateDeviceResources(void);

	void setDualPolarity(bool _on);

	void resizeLayers(void);
	void setCursorControl(int _cursorControl);
	void runXAxisWizard(FLOAT _left, FLOAT _right);
	void runYAxisWizard(FLOAT _bottom, FLOAT _top);
	void renderAndAutoRange(void);
	void autoRange(void);
	virtual bool getHasData(void);
	void shiftTime(int _move, bool _render = true);
	void getMinMaxX(FLOAT *_left, FLOAT *_right);
	void getMinMaxY(FLOAT *_bottom, FLOAT *_top);
	void setXaxisLimits(FLOAT _origin, FLOAT _end, bool _render);
	void setYaxisLimits(FLOAT _bottom, FLOAT _top, bool _render);
	void setEditableWindowMouseControl(BOOL _on, HCURSOR _cur);

	void setMMarkerStartStopVector(vector <FLOAT> *_v);  // For use in AGSBrowser

	virtual BOOL Create(LPCTSTR lpszCaption, DWORD dwStyle, const RECT& rect, CWnd* pParentWnd, UINT nID);

	CRGLGraph *getGraph(void);

	DECLARE_MESSAGE_MAP()

	virtual void DrawItem(LPDRAWITEMSTRUCT /*lpDrawItemStruct*/);
	virtual bool getTimeValAt(LONG _x, LONG _y,FLOAT *_time);
	virtual CEvnt *findEvent(int _what, FLOAT _atTime, int *_closestEdge);
	virtual bool getTimeAndAmplAt(LONG _x, LONG _y,CString *_s,bool _lineBreak = false);
	virtual bool getAmplAt(LONG _x, LONG _y, CString *_s);
	virtual bool getEUAmplAt(LONG _x, LONG _y, CString *_s);
	virtual bool getTimeAt(LONG _x, LONG _y, CString *_s);
	virtual bool getTimeCursorLeftInfo(CString *_s);
	virtual bool getTimeCursorRightInfo(CString *_s);
	virtual bool getTimeCursorLengthInfo(CString *_s);
	virtual bool getScreenXfromTime(FLOAT _time,int *_x);
	virtual void setEvents(CEvents *_eP);
	virtual void setCursorWindow(float _start, float _stop);
	virtual BOOL PreTranslateMessage(MSG * pMsg);

protected:
	bool hideYlabels;
	float statisticsWindowEdge0, statisticsWindowEdge1;
	bool hoverIsIndicated;
	bool getLevelFromKey(UINT *_level,bool *_findFromGradient);
	bool getTypeFromKey(UINT *_type);
	HCURSOR noCursor;
	bool visualWholeEventMoving, visualLeftEdgeMoving, visualRightEdgeMoving, hoverIndication;
	UINT visualEventMovingType, visualEventMovingHasThisLevel;
	float visualGrabDistToLeft,visualGrabDistToRight;
	void flush(void);
	BOOL enableEditableWindowMouseControl;
	HCURSOR vArrow;
	UINT eventsToShow;
	int debugTag;
	bool lockTopOfYAxis;
	CEvents *eventsP;
	CEvnt *eventPointedTo;
	int edgePointedTo;
	//CString sFrom, sTo, sLength;
	bool dualPolarity;
	bool YshiftIsAllowed;
	bool YscaleIsAllowed;
	int layerTypeToEdit;
	int cursorControl;
	CRGLLayer *layerToEdit;
	CRect clientRect;
	CString regSection;
	CString regSubKey;
	// void readAxisSettings(void);
	// void writeAxisSettings(void);
	virtual HRESULT CreateDeviceIndependentResources(void);
	virtual void DiscardDeviceResources(void);
	CComPtr <ID2D1Factory> m_factory;
	CComPtr <IDWriteFactory> pDWriteFactory;
	IDWriteTextFormat* pTextFormat;
	CComPtr <ID2D1HwndRenderTarget> m_target;
	bool doneInitIndependentResources;
	bool beginZoom;
	void shiftYAxis(int _move, bool _render = true);
	virtual void moveLeftCursor(int _move);
	virtual void moveRightCursor(int _move);
	virtual void moveEventEdge(CEvnt *_evnt, FLOAT _moveSecs, int _object);
	virtual void changeAnEvent(CEvnt *_evnt, int _command);
	
	TIME_WINDOW eventEditWindow;

	UINT messMoveX;
	UINT messCentreX;
	UINT messDisableEventHoverIndication;
	UINT messMoveCursorWindow;
	UINT messMovingX;
	UINT messMoveY;
	UINT messScaleX;
	UINT messScaleY;
	UINT messSetLeftEditWindow;
	UINT messSetRightEditWindow;
	//UINT messPointAndClickDeleteEvent;
	//UINT messPointAndClickAddEvent;

	CToolTipCtrl m_ttip;
	CFont ttFont;

	void RelayEventAndSetCursor(UINT message, WPARAM wParam, LPARAM lParam, CString _amplInfo);
	void RelayEventAndSetCursorDuringVisualEventEditing(UINT message, WPARAM wParam, LPARAM lParam, CString _amplInfo);
	int getEventName(int _what);
	int getHypoTypeName(int _what);
	CString getEventDescription(int _what);

	//---2 sec intrerval
	//ID2D1PathGeometry *dGeometry2sec;
	//ID2D1TransformedGeometry *dTransformedGeometry2sec;
	////---1 sec interval
	//ID2D1PathGeometry *dGeometry1sec;
	//ID2D1TransformedGeometry *dTransformedGeometry1sec;
	////---0.5 sec interval
	//ID2D1PathGeometry *dGeometry05sec;
	//ID2D1TransformedGeometry *dTransformedGeometry05sec;
	////---All points
	//ID2D1PathGeometry *dGeometryAll;
	//ID2D1TransformedGeometry *dTransformedGeometryAll;

	bool doneInitDependentResources;
	bool leftBtnIsDown;
	CPoint leftBtnDownPoint;
	CPoint leftPoint;
	CPoint rightPoint;
	CString title;
	int currentCursor, defaultCursor;
	virtual void renderXYPlots(bool _mouseIsOver);
	virtual LRESULT WindowProc(UINT message, WPARAM wParam, LPARAM lParam);
	CArray <CRGLLayer *, CRGLLayer *> layers;
	CArray <CRGLTile *, CRGLTile *> tiles;

	void cleanUp(void);
	void deleteLayer(CRGLLayer *_layerP);
	void discardLayerDeviceResources(void);
	void renderLayers(bool _disableHoverIndication = false);
	void renderEvents(void);
	void scaleTimeAboutCentre(CPoint _pt);
	void scaleTimeAboutCentre(int _delta);
	bool scaleYAxis(CPoint _pt);
	virtual float getYAutoMax(void);
	virtual float getYAutoMin(void);
	bool drawOnlyEvents;
public:
	bool wheelRunning;
	int wheelTimerCounter;
	UINT timerID;
	CString engineeringUnit;
	CRGLLayer *addLayer(CRGLLayer *_layer);
	CRGLTile *addTile(CRGLTile *_tile);
	void doGraphGeometries(void);
	afx_msg void OnShowWindow(BOOL bShow, UINT nStatus);
	virtual afx_msg void OnSize(UINT nType, int cx, int cy);
	afx_msg void OnPaint();
	afx_msg void OnDestroy();
	virtual afx_msg void OnMouseMove(UINT nFlags, CPoint point);
	afx_msg void OnMouseLeave();
	virtual afx_msg void OnLButtonDown(UINT nFlags, CPoint point);
	virtual afx_msg void OnRButtonDown(UINT nFlags, CPoint point);
	virtual afx_msg void OnLButtonUp(UINT nFlags, CPoint point);
	virtual afx_msg void OnRButtonUp(UINT nFlags, CPoint point);
	virtual afx_msg void OnMButtonDown(UINT nFlags, CPoint point);
	virtual afx_msg void OnMButtonUp(UINT nFlags, CPoint point);
	afx_msg BOOL OnSetCursor(CWnd* pWnd, UINT nHitTest, UINT message);
	afx_msg void OnLButtonDblClk(UINT nFlags, CPoint point);
	afx_msg void OnRButtonDblClk(UINT nFlags, CPoint point);

	virtual BOOL OnGesturePan(CPoint ptFrom, CPoint ptTo);
	virtual BOOL OnGestureTwoFingerTap(CPoint ptCenter);
	virtual BOOL OnGesturePressAndTap(CPoint ptPress, long lDelta);
	virtual BOOL OnGestureZoom(CPoint ptCenter, long lDelta);
protected:
	virtual BOOL OnCommand(WPARAM wParam, LPARAM lParam);
	//virtual bool getIsOver(CPoint _point,int *_what,int *_where);
	//int pickWhat, pickWhere;
	virtual int getRightCursorX(void);
	virtual int getLeftCursorX(void);
public:
	afx_msg int OnCreate(LPCREATESTRUCT lpCreateStruct);
	afx_msg LRESULT OnNcHitTest(CPoint point);
	afx_msg void OnContextMenu(CWnd* pWnd, CPoint point);
	afx_msg void onDefMixed();
	afx_msg void onDefMixedUpper();
	afx_msg void onDefMixedLower();
	afx_msg void onDefMixedMulti();
	afx_msg void onDefMixedUndef();
	afx_msg void onDefObstrUpper();
	afx_msg void onDefObstrLower();
	afx_msg void onDefObstrMulti();
	afx_msg void onDefObstrUndef();
	afx_msg void onDefHypoUpper();
	afx_msg void onDefHypoLower();
	afx_msg void onDefHypoMulti();
	afx_msg void onDefHypoUndef();
	afx_msg void onDefObstrHypoUpper();
	afx_msg void onDefObstrHypoLower();
	afx_msg void onDefObstrHypoMulti();
	afx_msg void onDefObstrHypoUndef();
	afx_msg void onDefCentral();
	afx_msg void onDefObstructive();
	afx_msg void onDefHypopnea();
	afx_msg void onDefObstrHypopnea();
	afx_msg void onDefCentralHypopnea();
	afx_msg void onDefSpO2Drop();
	afx_msg void onClearEvent();
	afx_msg BOOL OnMouseWheel(UINT nFlags, short zDelta, CPoint pt);
	afx_msg void OnTimer(UINT_PTR nIDEvent);
};

#endif

