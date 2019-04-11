#ifndef D2CHANNEL_GRAPH_BUTTONS_INCLUDED
#define D2CHANNEL_GRAPH_BUTTONS_INCLUDED


/*!
* @author		RKH
* @date		06.06.2017
* @copyright (C)Spiro Medical AS 2013 - 2017
*/

#include "D2Graph.h"
#include "CommonResource.h"
#include "GraphVisualProperties.h"
#include "vectorFunctions.h"
#include "Events.h"

//---Default maxY ranges (used for autorange)
#define DEFAULT_RESP_FRQ_MAX		40.0f	// /min
#define DEFAULT_PR_MIN				40.0f;	// /min
#define DEFAULT_PR_MAX				100.0f	// /min
#define DEFAULT_BODY_POSXYZ_MAX		2.0f	// 
#define DEFAULT_BODY_POS_MAX		6.0f	// Covers -1 to upright
#define DEFAULT_BODY_POS_MIN		-1.0f	// Covers -1 to upright
#define DEFAULT_ACTIMETER_MAX		100.0f	// 
#define DEFAULT_AIRMIC_MAX			600.0f
#define DEFAULT_CONTACTMIC_MAX		200.0f
#define DEFAULT_CATH_TEMP_MAX		10.0f
#define DEFAULT_CATH_PRESS_MAX		60.0f
#define DEFAULT_SPO2_MAX			100.0f
#define DEFAULT_SPO2_MIN			80.0f
#define DEFAULT_PRESS_GRADIENT_MAX		100.0f;

#define MAX_SCALE_PULSE_RATE			400.0f
#define MIN_SCALE_PULSE_RATE			20.0f
#define MAX_SCALE_CONTACT_MIC			1000.0f
#define MIN_SCALE_CONTACT_MIC			.0f

const float minAbsTemp = .0f; // From 0 degC
const float maxAbsTemp = 40.0f; // To 40 degC

class CGraphBtnChannel : public CGraphButton
{
public:
	CGraphBtnChannel();
	~CGraphBtnChannel();
	void setExtremeXLimits(FLOAT _left,FLOAT _right);
	virtual void updateGraphSettings(void);
	virtual bool getHasData(void);
    void setTiledView(bool _set);
	virtual void setEventsToShow(UINT _evToShow,bool _onlyShowEvents = false);
	virtual void doEventGeometry(bool _includeGraph = false, int _type = -1, bool _withLevel = false);
	virtual void setCursorWindow(float _start, float _stop);
	void setShowXlabels(bool _on);
	CString getTitle(void);
	CString getEngineeringUnit(void);
	void setCanDoStatistics(bool _canDoStatistics);
	bool getCanDoStatistics(void); 
	virtual vector <BOOL> *getPenDownVector(void);
protected:
	bool canDoStatistics;
	virtual void setPlotTileYLimits(void);
	virtual void setData(vector <FLOAT> *_v,vector <FLOAT> *_tv = NULL) ;
	CRGLTile *plotTile;
    bool inTiledView;			// If part of a tiled view, adjust the plot area
	CRGLAxes *axes;
	CRGLBackground *backgr;
	CString timeAxisLabel;		// Label for the time axis. Shown normally, but not if button is part of a tiled view
	virtual void createCurveSettings(CButtonPlotSettings *_plotSettings);
	float getMaxYScaleFromMeanAndMax(float _mean,float _max);
	virtual float getYAutoMax(void);
public:
	virtual BOOL Create(LPCTSTR lpszCaption, DWORD dwStyle, const RECT& rect, CWnd* pParentWnd, UINT nID);
	DECLARE_MESSAGE_MAP()
	afx_msg LRESULT OnNcHitTest(CPoint point);
};

class CGraphBtnTimeAxis : public CGraphBtnChannel
{
public:
	CGraphBtnTimeAxis();
	~CGraphBtnTimeAxis();
	virtual afx_msg void OnMouseMove(UINT nFlags, CPoint point);
	virtual BOOL Create(LPCTSTR lpszCaption, DWORD dwStyle, const RECT& rect, CWnd* pParentWnd, UINT nID);
protected:
};

class CGraphBtnEventMarker : public CGraphBtnChannel
{
public:
	CGraphBtnEventMarker();
	~CGraphBtnEventMarker();
	virtual afx_msg void OnMouseMove(UINT nFlags, CPoint point);
	virtual BOOL Create(LPCTSTR lpszCaption, DWORD dwStyle, const RECT& rect, CWnd* pParentWnd, UINT nID);	
	virtual bool getHasData(void);
	virtual void setEvents(CEvents *_eP);
	virtual void doEventGeometry(bool _includeGraph = false, int _type = -1, bool _withLevel = false);
protected:
	CRGLGraphEventMarkers *central,*mixed,*centralHypo,*obstrHypo,*obstr;
	CRGLGraphEventMarkers *spO2, *rera, *arousals, *awake, *snoring, *manual, *swallow, *imported;
	CRGLGraphExcludedEventMarker *excluded;
	virtual void createCurveSettings(CButtonPlotSettings *_plotSettings);

	CToolTipCtrl m_ttipEvent;
};

class CGraphBtnBattVoltage : public CGraphBtnChannel
{
public:
	CGraphBtnBattVoltage();
	~CGraphBtnBattVoltage();
	void setData(CBatteryDataSet *_dataSet);
	virtual void setEvents(CEvents *_eP);
	virtual void updateGraphSettings(void);
	virtual float getYAutoMax(void);
	virtual bool getHasData(void);
	virtual CString getTimeAndAmplAt(LONG _x,LONG _y);
protected:
	virtual void setData(vector <FLOAT> *_v,vector <FLOAT> *_tv = NULL);
	CBatteryDataSet *dataSet;
	CRGLGraph *curveBattVolt;
	virtual void createCurveSettings(CButtonPlotSettings *_plotSettings);
};

class CGraphBtnBattCapacity : public CGraphBtnChannel
{
public:
	CGraphBtnBattCapacity();
	~CGraphBtnBattCapacity();
	void setData(CBatteryDataSet *_dataSet);
	virtual void updateGraphSettings(void);
	virtual void setEvents(CEvents *_eP);
	virtual float getYAutoMax(void);
	virtual bool getHasData(void);
	virtual CString getTimeAndAmplAt(LONG _x,LONG _y);
protected:
	void setData(vector <FLOAT> *_rc,vector <FLOAT> *_fcc,vector <FLOAT> *_tv = NULL) ;
	CBatteryDataSet *dataSet;
	CRGLGraph *curveBattRc,*curveBattFcc;
	virtual void createCurveSettings(CButtonPlotSettings *_plotSettings);
public:
	DECLARE_MESSAGE_MAP()
	afx_msg LRESULT OnNcHitTest(CPoint point);
};

class CGraphBtnBattTemperature : public CGraphBtnChannel
{
public:
	CGraphBtnBattTemperature();
	~CGraphBtnBattTemperature();
	void setData(CBatteryDataSet *_dataSet);
	virtual void updateGraphSettings(void);
	virtual void setEvents(CEvents *_eP);
	virtual float getYAutoMax(void);
	virtual bool getHasData(void);
	virtual CString getTimeAndAmplAt(LONG _x,LONG _y);
protected:
	void setData(vector <FLOAT> *_temp,vector <FLOAT> *_tv = NULL) ;
	CBatteryDataSet *dataSet;
	CRGLGraph *curveBattTemp;
	virtual void createCurveSettings(CButtonPlotSettings *_plotSettings);
};

class CGraphBtnBattCurrent : public CGraphBtnChannel
{
public:
	CGraphBtnBattCurrent();
	~CGraphBtnBattCurrent();
	void setData(CBatteryDataSet *_dataSet);
	virtual void updateGraphSettings(void);
	virtual void setEvents(CEvents *_eP);
	virtual float getYAutoMax(void);
	virtual bool getHasData(void);
	virtual CString getTimeAndAmplAt(LONG _x, LONG _y);
protected:
	void setData(vector <FLOAT> *_temp, vector <FLOAT> *_tv = NULL);
	CBatteryDataSet *dataSet;
	CRGLGraph *curveBattCurrent;
	virtual void createCurveSettings(CButtonPlotSettings *_plotSettings);
};

class CGraphBtnContactMic : public CGraphBtnChannel
{
public:
	CGraphBtnContactMic();
	~CGraphBtnContactMic();
	void setData(CContactMicDataSet *_dataSet);
	virtual void setEvents(CEvents *_eP);
	virtual void updateGraphSettings(void);
	virtual float getYAutoMax(void);
	virtual bool getHasData(void);
	virtual void setEventsToShow(UINT _evToShow, bool _onlyShowEvents = false);
	virtual void updateCurveColour(void);
protected:
	virtual void setPlotTileYLimits(void);
	void setData(vector <FLOAT> *_cm,vector <FLOAT> *_tv = NULL) ;
	CContactMicDataSet *dataSet;
	CRGLGraphSparse *curveCm;
	virtual void createCurveSettings(CButtonPlotSettings *_plotSettings);
public:
	DECLARE_MESSAGE_MAP()
	afx_msg LRESULT OnNcHitTest(CPoint point);
};

class CGraphBodyPosXYZ : public CGraphBtnChannel
{
public:
	CGraphBodyPosXYZ();
	~CGraphBodyPosXYZ();
	virtual float getYAutoMax(void);
	void setData(CBodyPosXYZDataSet *_dataSet);
	virtual void updateGraphSettings(void);
	virtual void setEvents(CEvents *_eP);
	virtual bool getHasData(void);
	virtual void setEventsToShow(UINT _evToShow, bool _onlyShowEvents = false);
	virtual CString getTimeAndAmplAt(LONG _x,LONG _y);
protected:
	void setData(vector <FLOAT> *_x,vector <FLOAT> *_y,vector <FLOAT> *_z,vector <FLOAT> *_tv = NULL) ;
	CBodyPosXYZDataSet *dataSet;
	CRGLGraph *curveX,*curveY,*curveZ;
	virtual void createCurveSettings(CButtonPlotSettings *_plotSettings);
public:
	DECLARE_MESSAGE_MAP()
	afx_msg LRESULT OnNcHitTest(CPoint point);
};

class CGraphBodyPos : public CGraphBtnChannel
{
public:
	CGraphBodyPos();
	~CGraphBodyPos();
	virtual float getYAutoMax(void);
	void setData(CBodyPosDataSet *_dataSet);
	virtual void updateGraphSettings(void);
	virtual void setEvents(CEvents *_eP);
	virtual bool getHasData(void);
	virtual void setEventsToShow(UINT _evToShow, bool _onlyShowEvents = false);
	virtual float getYAutoMin(void);
	virtual CString getTimeAndAmplAt(LONG _x,LONG _y);
	virtual CEvnt *findEvent(int _what,FLOAT _atTime,int *_closestEdge);
	virtual FLOAT getTimeValAt(LONG _x,LONG _y);
	virtual void doEventGeometry(bool _includeGraph = false, int _type = -1, bool _withLevel = false);
	virtual void updateCurveColour(void);
	virtual bool getEUAmplAt(LONG _x, LONG _y, CString *_s);
	virtual vector <BOOL> *getPenDownVector(void);
protected:
	void setData(vector <FLOAT> *_bp,vector <FLOAT> *_tv = NULL) ;
	CBodyPosDataSet *dataSet;
	CRGLGraph *curveBP;
	virtual void createCurveSettings(CButtonPlotSettings *_plotSettings);
	virtual bool isOverEvent(D2D1_POINT_2F _p,UINT _evntType,int *_what,int *_where);
	virtual void moveLeftCursor(int _move);
	virtual void moveRightCursor(int _move);
	virtual void moveEventEdge(CEvnt *_evnt,FLOAT _moveSecs,int _object);
	virtual void changeAnEvent(CEvnt *_evnt,int _command);
	virtual int getRightCursorX(void);
	virtual int getLeftCursorX(void);
public:
	virtual BOOL Create(LPCTSTR lpszCaption, DWORD dwStyle, const RECT& rect, CWnd* pParentWnd, UINT nID);
	DECLARE_MESSAGE_MAP()
	afx_msg LRESULT OnNcHitTest(CPoint point);
};

class CGraphBtnActimeter : public CGraphBtnChannel
{
public:
	CGraphBtnActimeter();
	~CGraphBtnActimeter();
	virtual float getYAutoMax(void);
	void setData(CActimeterDataSet *_actimeterDataSet);// , CActimeterDataSet *_torsoActimeterDataSet = NULL);
	virtual void updateGraphSettings(void);
	virtual void setEvents(CEvents *_eP);
	virtual bool getHasData(void);
	virtual void setEventsToShow(UINT _evToShow, bool _onlyShowEvents = false);
	virtual void updateCurveColour(void);
protected:
	void setData(vector <FLOAT> *_act,vector <FLOAT> *_tv = NULL) ;
	//void setTorsoData(vector <FLOAT> *_act, vector <FLOAT> *_tv = NULL);
	CActimeterDataSet *dataSet;
	//CActimeterDataSet *torsoActimeterDataSet;
	CRGLGraph *curveActimeter;
	//CRGLGraph *curveTorsoActimeter;
	virtual void createCurveSettings(CButtonPlotSettings *_plotSettings);
public:
	DECLARE_MESSAGE_MAP()
	afx_msg LRESULT OnNcHitTest(CPoint point);
};

class CGraphBtnTorsoActimeter : public CGraphBtnActimeter
{
public:
	CGraphBtnTorsoActimeter();
	~CGraphBtnTorsoActimeter();
	virtual bool getEUAmplAt(LONG _x, LONG _y, CString *_s);
protected:
	virtual void createCurveSettings(CButtonPlotSettings *_plotSettings);
};

class CGraphBtnAirMicAll : public CGraphBtnChannel
{
public:
	CGraphBtnAirMicAll();
	~CGraphBtnAirMicAll();
	void setData(CAirMicDataSet *_dataSet);
	virtual void setEvents(CEvents *_eP);
	virtual void updateGraphSettings(void);
	virtual float getYAutoMax(void);
	virtual bool getHasData(void);
	virtual void setEventsToShow(UINT _evToShow, bool _onlyShowEvents = false);
	virtual CString getTimeAndAmplAt(LONG _x,LONG _y);
	virtual void updateCurveColour(void);
	virtual bool getEUAmplAt(LONG _x, LONG _y, CString *_s);
protected:
	void setData(vector <FLOAT> *_c0,
		vector <FLOAT> *_c1,
		vector <FLOAT> *_c2,
		vector <FLOAT> *_c3,
		vector <FLOAT> *_tv = NULL) ;
	CAirMicDataSet *dataSet;
	CRGLGraphSparse *curve0,*curve1,*curve2,*curve3;
	CString st0, st1, st2, st3;
	//virtual void createCurveSettings(CButtonPlotSettings *_plotSettings);
public:
	virtual BOOL Create(LPCTSTR lpszCaption, DWORD dwStyle, const RECT& rect, CWnd* pParentWnd, UINT nID);
	DECLARE_MESSAGE_MAP()
	afx_msg LRESULT OnNcHitTest(CPoint point);
};

class CGraphBtnPulseRate : public CGraphBtnChannel
{
public:
	CGraphBtnPulseRate();
	~CGraphBtnPulseRate();
	virtual float getYAutoMax(void);
	virtual float getYAutoMin(void);
	void setData(COximeterDataSet *_dataSet);
	virtual void updateGraphSettings(void);
	virtual void setEvents(CEvents *_eP);
	virtual bool getHasData(void);
	virtual void setEventsToShow(UINT _evToShow, bool _onlyShowEvents = false);
	virtual CString getTimeAndAmplAt(LONG _x,LONG _y);
	virtual void updateCurveColour(void);
	virtual vector <BOOL> *getPenDownVector(void);
protected:
	virtual void setPlotTileYLimits(void);
	void setData(vector <FLOAT> *_pr,vector <FLOAT> *_tv = NULL) ;
	COximeterDataSet *dataSet;
	CRGLGraphSparse *curvePR;
	virtual void createCurveSettings(CButtonPlotSettings *_plotSettings);
public:
	DECLARE_MESSAGE_MAP()
	afx_msg LRESULT OnNcHitTest(CPoint point);
};

class CGraphBtnSpO2 : public CGraphBtnChannel
{
public:
	CGraphBtnSpO2();
	~CGraphBtnSpO2();
	virtual float getYAutoMax(void);
	virtual float getYAutoMin(void);
	void setData(COximeterDataSet *_dataSet);
	virtual void updateGraphSettings(void);
	virtual void setEvents(CEvents *_eP);
	virtual bool getHasData(void);
	virtual void setEventsToShow(UINT _evToShow, bool _onlyShowEvents = false);
	virtual CString getTimeAndAmplAt(LONG _x,LONG _y);
	virtual void updateCurveColour(void);
	virtual vector <BOOL> *getPenDownVector(void);
protected:
	void setData(vector <FLOAT> *_spO2,vector <FLOAT> *_tv = NULL) ;
	COximeterDataSet *dataSet;
	CRGLGraphSparse *curveSpO2;
	virtual void createCurveSettings(CButtonPlotSettings *_plotSettings);
public:
	DECLARE_MESSAGE_MAP()
	afx_msg LRESULT OnNcHitTest(CPoint point);
};

class CGraphBtnSingleMic : public CGraphBtnChannel
{
public:
	CGraphBtnSingleMic();
	~CGraphBtnSingleMic();
	virtual float getYAutoMax(void);
	virtual void updateGraphSettings(void);
	void setData(CAirMicDataSet *_dataSet);
	virtual void setEvents(CEvents *_eP);
	virtual bool getHasData(void);
	virtual void setEventsToShow(UINT _evToShow, bool _onlyShowEvents = false);
	virtual void updateCurveColour(void);
protected:
	void setData(vector <FLOAT> *_mic,vector <FLOAT> *_tv = NULL) ;
	CAirMicDataSet *dataSet;
	CRGLGraphSparse *curveMic;
	virtual void createCurveSettings(CButtonPlotSettings *_plotSettings);
public:
	DECLARE_MESSAGE_MAP()
	afx_msg LRESULT OnNcHitTest(CPoint point);
};


class CGraphBtn2500Mic : public CGraphBtnSingleMic
{
public:
	CGraphBtn2500Mic();
	~CGraphBtn2500Mic();
	void setData(CAirMicDataSet *_dataSet);
	virtual void updateCurveColour(void);
protected:
	void setData(vector <FLOAT> *_mic, vector <FLOAT> *_tv = NULL);
};
class CGraphBtn1000Mic : public CGraphBtnSingleMic
{
public:
	CGraphBtn1000Mic();
	~CGraphBtn1000Mic();
	void setData(CAirMicDataSet *_dataSet);
	virtual void updateCurveColour(void);
protected:
	void setData(vector <FLOAT> *_mic, vector <FLOAT> *_tv = NULL);
};
class CGraphBtn400Mic : public CGraphBtnSingleMic
{
public:
	CGraphBtn400Mic();
	~CGraphBtn400Mic();
	void setData(CAirMicDataSet *_dataSet);
	virtual void updateCurveColour(void);
protected:
	void setData(vector <FLOAT> *_mic, vector <FLOAT> *_tv = NULL);
};
class CGraphBtn160Mic : public CGraphBtnSingleMic
{
public:
	CGraphBtn160Mic();
	~CGraphBtn160Mic();
	void setData(CAirMicDataSet *_dataSet);
	virtual void updateCurveColour(void);
protected:
	void setData(vector <FLOAT> *_mic, vector <FLOAT> *_tv = NULL);
};

class CGraphBtnAG200_Mic : public CGraphBtnSingleMic
{
public:
	CGraphBtnAG200_Mic();
	~CGraphBtnAG200_Mic();
	void setData(CAG200MicDataSet *_dataSet);
	float getYAutoMax(void);
protected:
	CAG200MicDataSet *dataSet;
	void setData(vector <FLOAT> *_mic, vector <FLOAT> *_tv = NULL);
};

class CGraphBtnRespFrq : public CGraphBtnChannel
{
public:
	CGraphBtnRespFrq();
	~CGraphBtnRespFrq();
	virtual float getYAutoMax(void);
	virtual void updateGraphSettings(void);
	void setData(CCatheterDataSet *_dataSet);
	void setData(CRespBeltDataSet* _dataSet);
	virtual void setEvents(CEvents *_eP);
	virtual bool getHasData(void);
	virtual void setEventsToShow(UINT _evToShow, bool _onlyShowEvents = false);
	virtual CString getTimeAndAmplAt(LONG _x,LONG _y);
	virtual void updateCurveColour(void);
protected:
	void setData(vector <FLOAT> *_frq,vector <FLOAT> *_tv = NULL) ;
	CCatheterDataSet *dataSet;
	CRespBeltDataSet* respBeltDataSet;
	CRGLGraphSparse *curveRespFrq;
	virtual void createCurveSettings(CButtonPlotSettings *_plotSettings);
public:
	DECLARE_MESSAGE_MAP()
	afx_msg LRESULT OnNcHitTest(CPoint point);
};

class CGraphBtnAdmittance : public CGraphBtnChannel
{
public:
	CGraphBtnAdmittance();
	~CGraphBtnAdmittance();
	virtual float getYAutoMax(void);
	virtual void updateGraphSettings(void);
	void setData(CCatheterDataSet *_dataSet);
	void setData(CRespBeltDataSet* _dataSet);
	virtual void setEvents(CEvents *_eP);
	virtual bool getHasData(void);
	virtual void setEventsToShow(UINT _evToShow, bool _onlyShowEvents = false);
	virtual void updateCurveColour(void);
protected:
	void setData(vector <FLOAT> *_adm,vector <FLOAT> *_tv = NULL) ;
	CCatheterDataSet *dataSet;
	CRespBeltDataSet* respBeltDataSet;
	CRGLGraphSparse *curveAdmittance;
	virtual void createCurveSettings(CButtonPlotSettings *_plotSettings);
public:
	DECLARE_MESSAGE_MAP()
	afx_msg LRESULT OnNcHitTest(CPoint point);
};


//////////////////////////////////////////////////////////////////
//	Pressure family
//////////////////////////////////////////////////////////////////

class CGraphBtnCatheterPressure : public CGraphBtnChannel
{
public:
	CGraphBtnCatheterPressure();
	~CGraphBtnCatheterPressure();
	virtual void updateGraphSettings(void);
	virtual float getYAutoMax(void);
	void setData(CCatheterDataSet *_dataSet);
	virtual void setEvents(CEvents *_eP);
	virtual bool getHasData(void);
protected:
	void setData(vector <FLOAT> *_p,vector <FLOAT> *_tv = NULL) ;
	CCatheterDataSet *dataSet;
	virtual void createCurveSettings(CButtonPlotSettings *_plotSettings);
public:
	DECLARE_MESSAGE_MAP()
	afx_msg LRESULT OnNcHitTest(CPoint point);
};

class CGraphBtnCatheterPressurePOES : public CGraphBtnCatheterPressure
{
public:
	CGraphBtnCatheterPressurePOES();
	~CGraphBtnCatheterPressurePOES();
	virtual void updateGraphSettings(void);
	virtual float getYAutoMax(void);
	void setData(CCatheterDataSet *_dataSet);
	virtual void setEvents(CEvents *_eP);
	virtual bool getHasData(void);
	virtual void setEventsToShow(UINT _evToShow, bool _onlyShowEvents = false);
	virtual CString getTimeAndAmplAt(LONG _x,LONG _y);
	virtual void updateCurveColour(void);
protected:
	void setData(vector <FLOAT> *_p,vector <FLOAT> *_tv = NULL) ;
	CRGLGraphSparse *curvePOES;
	virtual void createCurveSettings(CButtonPlotSettings *_plotSettings);
public:
	DECLARE_MESSAGE_MAP()
	afx_msg LRESULT OnNcHitTest(CPoint point);
};

class CGraphBtnCatheterPressurePPH : public CGraphBtnCatheterPressure
{
public:
	CGraphBtnCatheterPressurePPH();
	~CGraphBtnCatheterPressurePPH();
	virtual void updateGraphSettings(void);
	virtual float getYAutoMax(void);
	void setData(CCatheterDataSet *_dataSet);
	virtual void setEvents(CEvents *_eP);
	virtual bool getHasData(void);
	virtual void setEventsToShow(UINT _evToShow, bool _onlyShowEvents = false);
	virtual CString getTimeAndAmplAt(LONG _x,LONG _y);
	virtual void updateCurveColour(void);
protected:
	void setData(vector <FLOAT> *_p,vector <FLOAT> *_tv = NULL) ;
	CRGLGraphSparse *curvePPH;
	virtual void createCurveSettings(CButtonPlotSettings *_plotSettings);
public:
	DECLARE_MESSAGE_MAP()
	afx_msg LRESULT OnNcHitTest(CPoint point);
};

class CGraphBtnPressGrad : public CGraphBtnCatheterPressure
{
public:
	CGraphBtnPressGrad();
	~CGraphBtnPressGrad();
	virtual void updateGraphSettings(void);
	virtual float getYAutoMax(void);
	void setData(CCatheterDataSet *_dataSet);
	virtual void setEvents(CEvents *_eP);
	virtual bool getHasData(void);
	virtual void setEventsToShow(UINT _evToShow, bool _onlyShowEvents = false);
	virtual CString getTimeAndAmplAt(LONG _x,LONG _y);
	virtual void updateCurveColour(void);
protected:
	void setData(vector <FLOAT> *_pUpper,vector <FLOAT> *_tv = NULL) ;
	CRGLGraphSparse *curvePUpperGradient;
	virtual void createCurveSettings(CButtonPlotSettings *_plotSettings);
public:
	DECLARE_MESSAGE_MAP()
	afx_msg LRESULT OnNcHitTest(CPoint point);
};

class CGraphBtnPOESEnv : public CGraphBtnCatheterPressure
{
public:
	CGraphBtnPOESEnv();
	~CGraphBtnPOESEnv();
	virtual void updateGraphSettings(void);
	virtual float getYAutoMax(void);
	void setData(CCatheterDataSet *_dataSet);
	virtual void setEvents(CEvents *_eP);
	virtual bool getHasData(void);
	virtual void setEventsToShow(UINT _evToShow, bool _onlyShowEvents = false);
	virtual CString getTimeAndAmplAt(LONG _x,LONG _y);
	virtual void updateCurveColour(void);
protected:
	void setData(vector <FLOAT> *_p,vector <FLOAT> *_baseline,vector <FLOAT> *_tv = NULL) ;
	CRGLGraphSparse *curvePOESEnv;
	CRGLGraphSparse *curveBaseline;
	virtual void createCurveSettings(CButtonPlotSettings *_plotSettings);
public:
	DECLARE_MESSAGE_MAP()
	afx_msg LRESULT OnNcHitTest(CPoint point);
};

class CGraphBtnPPHEnv : public CGraphBtnCatheterPressure
{
public:
	CGraphBtnPPHEnv();
	~CGraphBtnPPHEnv();
	virtual void updateGraphSettings(void);
	virtual float getYAutoMax(void);
	void setData(CCatheterDataSet *_dataSet);
	virtual void setEvents(CEvents *_eP);
	virtual bool getHasData(void);
	virtual void setEventsToShow(UINT _evToShow, bool _onlyShowEvents = false);
	virtual CString getTimeAndAmplAt(LONG _x,LONG _y);
	virtual void updateCurveColour(void);
protected:
	void setData(vector <FLOAT> *_p,vector <FLOAT> *_baseline,vector <FLOAT> *_tv = NULL) ;
	CRGLGraphSparse *curvePPHEnv;
	CRGLGraphSparse *curveBaseline;
	virtual void createCurveSettings(CButtonPlotSettings *_plotSettings);
public:
	DECLARE_MESSAGE_MAP()
	afx_msg LRESULT OnNcHitTest(CPoint point);
};

//////////////////////////////////////////////////////////////////
//	Temperature family
//////////////////////////////////////////////////////////////////

class CGraphBtnCatheterTemperature : public CGraphBtnChannel
{
public:
	CGraphBtnCatheterTemperature();
	~CGraphBtnCatheterTemperature();
	virtual void updateGraphSettings(void);
	virtual float getYAutoMax(void);
	void setData(CCatheterDataSet *_dataSet);
	virtual void setEvents(CEvents *_eP);
	virtual bool getHasData(void);
protected:
	void setData(vector <FLOAT> *_temp,vector <FLOAT> *_tv = NULL) ;
	CCatheterDataSet *dataSet;
	virtual void createCurveSettings(CButtonPlotSettings *_plotSettings);
public:
	DECLARE_MESSAGE_MAP()
	afx_msg LRESULT OnNcHitTest(CPoint point);
};

class CGraphBtnCatheterTemperatureT0 : public CGraphBtnCatheterTemperature
{
public:
	CGraphBtnCatheterTemperatureT0();
	~CGraphBtnCatheterTemperatureT0();
	virtual void updateGraphSettings(void);
	virtual float getYAutoMax(void);
	void setData(CCatheterDataSet *_dataSet);
	virtual void setEvents(CEvents *_eP);
	virtual bool getHasData(void);
	virtual void setEventsToShow(UINT _evToShow, bool _onlyShowEvents = false);
	virtual CString getTimeAndAmplAt(LONG _x,LONG _y);
	virtual void updateCurveColour(void);
protected:
	void setData(vector <FLOAT> *_temp,vector <FLOAT> *_tv = NULL) ;
	CRGLGraphSparse *curveT0;
	virtual void createCurveSettings(CButtonPlotSettings *_plotSettings);
public:
	DECLARE_MESSAGE_MAP()
	afx_msg LRESULT OnNcHitTest(CPoint point);
};

class CGraphBtnCatheterTemperatureT1 : public CGraphBtnCatheterTemperature
{
public:
	CGraphBtnCatheterTemperatureT1();
	~CGraphBtnCatheterTemperatureT1();
	virtual void updateGraphSettings(void);
	virtual float getYAutoMax(void);
	void setData(CCatheterDataSet *_dataSet);
	virtual void setEvents(CEvents *_eP);
	virtual bool getHasData(void);
	virtual void setEventsToShow(UINT _evToShow, bool _onlyShowEvents = false);
	virtual CString getTimeAndAmplAt(LONG _x,LONG _y);
	virtual void updateCurveColour(void);
protected:
	void setData(vector <FLOAT> *_temp,vector <FLOAT> *_tv = NULL) ;
	CRGLGraphSparse *curveT1;
	virtual void createCurveSettings(CButtonPlotSettings *_plotSettings);
public:
	DECLARE_MESSAGE_MAP()
	afx_msg LRESULT OnNcHitTest(CPoint point);
};

class CGraphBtnCatheterTemperatureT1Raw : public CGraphBtnCatheterTemperatureT1
{
public :
	CGraphBtnCatheterTemperatureT1Raw();
	~CGraphBtnCatheterTemperatureT1Raw();
	virtual void updateGraphSettings(void);
	void setData(CCatheterDataSet *_dataSet);
	void setYRange(void);
protected:
	void setData(vector <FLOAT> *_temp, vector <FLOAT> *_tv = NULL);
	virtual void createCurveSettings(CButtonPlotSettings *_plotSettings);
};

class CGraphBtnCatheterTemperatureT0Raw : public CGraphBtnCatheterTemperatureT0
{
public:
	CGraphBtnCatheterTemperatureT0Raw();
	~CGraphBtnCatheterTemperatureT0Raw();
	virtual void updateGraphSettings(void);
	void setData(CCatheterDataSet *_dataSet);
	void setYRange(void);
protected:
	void setData(vector <FLOAT> *_temp, vector <FLOAT> *_tv = NULL);
	virtual void createCurveSettings(CButtonPlotSettings *_plotSettings);
};

class CGraphBtnT1Env : public CGraphBtnCatheterTemperature
{
public:
	CGraphBtnT1Env();
	~CGraphBtnT1Env();
	virtual void updateGraphSettings(void);
	virtual float getYAutoMax(void);
	void setData(CCatheterDataSet *_dataSet);
	virtual void setEvents(CEvents *_eP);
	virtual bool getHasData(void);
	virtual void setEventsToShow(UINT _evToShow, bool _onlyShowEvents = false);
	virtual CString getTimeAndAmplAt(LONG _x,LONG _y);
	virtual void updateCurveColour(void);
protected:
	void setData(vector <FLOAT> *_t,vector <FLOAT> *_baseline,vector <FLOAT> *_tv) ;
	CRGLGraphSparse *curveEnv;
	CRGLGraphSparse *curveBaseline;
	virtual void createCurveSettings(CButtonPlotSettings *_plotSettings);
public:
	DECLARE_MESSAGE_MAP()
	afx_msg LRESULT OnNcHitTest(CPoint point);
};

class CGraphBtnT0Env : public CGraphBtnCatheterTemperature
{
public:
	CGraphBtnT0Env();
	~CGraphBtnT0Env();
	virtual void updateGraphSettings(void);
	virtual float getYAutoMax(void);
	void setData(CCatheterDataSet *_dataSet);
	virtual void setEvents(CEvents *_eP);
	virtual bool getHasData(void);
	virtual void setEventsToShow(UINT _evToShow, bool _onlyShowEvents = false);
	virtual CString getTimeAndAmplAt(LONG _x,LONG _y);
	virtual void updateCurveColour(void);
protected:
	void setData(vector <FLOAT> *_mic,vector <FLOAT> *_tv = NULL) ;
	CRGLGraphSparse *curveT0Env;
	virtual void createCurveSettings(CButtonPlotSettings *_plotSettings);
public:
	DECLARE_MESSAGE_MAP()
	afx_msg LRESULT OnNcHitTest(CPoint point);
};

class CGraphBtnT0plusT1 : public CGraphBtnCatheterTemperature
{
public:
	CGraphBtnT0plusT1();
	~CGraphBtnT0plusT1();
	virtual void updateGraphSettings(void);
	virtual float getYAutoMax(void);
	virtual void setData(CCatheterDataSet *_dataSet);
	virtual void setEvents(CEvents *_eP);
	virtual bool getHasData(void);
	virtual void setEventsToShow(UINT _evToShow, bool _onlyShowEvents = false);
	virtual CString getTimeAndAmplAt(LONG _x,LONG _y);
	virtual void updateCurveColour(void);
protected:
	void setData(vector <FLOAT> *_t, vector <FLOAT> *_tv = NULL) ;
	CRGLGraphSparse *curveT0plusT1;
	virtual void createCurveSettings(CButtonPlotSettings *_plotSettings);
public:
	DECLARE_MESSAGE_MAP()
	afx_msg LRESULT OnNcHitTest(CPoint point);
};

class CGraphBtnFlow: public CGraphBtnT1Env
{
public:
	CGraphBtnFlow();
	~CGraphBtnFlow();
	virtual void setData(CCatheterDataSet *_dataSet);
	virtual void doEventGeometry(bool _includeGraph = false, int _type = -1, bool _withLevel = false);
	virtual void createCurveSettings(CButtonPlotSettings *_plotSettings);
protected:
	void setData(vector <FLOAT> *_t, vector <FLOAT> *_baseline, vector <FLOAT> *_tv);
};


class CGraphBtnBreathingEfficiency : public CGraphBtnAdmittance
{
public:
	CGraphBtnBreathingEfficiency();
	~CGraphBtnBreathingEfficiency();
	virtual void setData(CCatheterDataSet *_dataSet);
	virtual void doEventGeometry(bool _includeGraph = false, int _type = -1, bool _withLevel = false);
	virtual void createCurveSettings(CButtonPlotSettings *_plotSettings);
protected:
	void setData(vector <FLOAT> *_adm, vector <FLOAT> *_tv = NULL);
};

//////////////////////////////////////////////////////////////////
//	Belt family
//////////////////////////////////////////////////////////////////

class CGraphBtnBelt : public CGraphBtnChannel
{
public:
	CGraphBtnBelt();
	~CGraphBtnBelt();
	virtual void updateGraphSettings(void);
	virtual float getYAutoMax(void);
	void setData(CRespBeltDataSet *_dataSet);
	virtual void setEvents(CEvents *_eP);
	virtual bool getHasData(void);
protected:
	void setData(vector <FLOAT> *_p,vector <FLOAT> *_tv = NULL) ;
	CRespBeltDataSet *dataSet;
	virtual void createCurveSettings(CButtonPlotSettings *_plotSettings);
public:
	DECLARE_MESSAGE_MAP()
	afx_msg LRESULT OnNcHitTest(CPoint point);
};

class CGraphBtnChestBelt : public CGraphBtnBelt
{
public:
	CGraphBtnChestBelt();
	~CGraphBtnChestBelt();
	virtual void updateGraphSettings(void);
	virtual float getYAutoMax(void);
	void setData(CRespBeltDataSet *_dataSet);
	virtual void setEvents(CEvents *_eP);
	virtual bool getHasData(void);
	virtual void setEventsToShow(UINT _evToShow, bool _onlyShowEvents = false);
protected:
	void setData(vector <FLOAT> *_p,vector <FLOAT> *_tv = NULL) ;
	CRGLGraphSparse *curveC;
	virtual void createCurveSettings(CButtonPlotSettings *_plotSettings);
public:
	DECLARE_MESSAGE_MAP()
	afx_msg LRESULT OnNcHitTest(CPoint point);
};

class CGraphBtnAbdominalBelt : public CGraphBtnBelt
{
public:
	CGraphBtnAbdominalBelt();
	~CGraphBtnAbdominalBelt();
	virtual void updateGraphSettings(void);
	virtual float getYAutoMax(void);
	void setData(CRespBeltDataSet *_dataSet);
	virtual void setEvents(CEvents *_eP);
	virtual bool getHasData(void);
	virtual void setEventsToShow(UINT _evToShow, bool _onlyShowEvents = false);
protected:
	void setData(vector <FLOAT> *_p,vector <FLOAT> *_tv = NULL) ;
	CRGLGraphSparse *curveA;
	virtual void createCurveSettings(CButtonPlotSettings *_plotSettings);
public:
	DECLARE_MESSAGE_MAP()
	afx_msg LRESULT OnNcHitTest(CPoint point);
};

class CGraphBtnBeltSum : public  CGraphBtnBelt
{
public:
	CGraphBtnBeltSum();
	~CGraphBtnBeltSum();
	virtual void updateGraphSettings(void);
	virtual float getYAutoMax(void);
	void setData(CRespBeltDataSet* _dataSet);
	virtual void setEvents(CEvents* _eP);
	virtual bool getHasData(void);
	virtual void setEventsToShow(UINT _evToShow, bool _onlyShowEvents = false);
protected:
	void setData(vector <FLOAT>* _p, vector <FLOAT>* _tv = NULL);
	CRGLGraphSparse* curveA;
	virtual void createCurveSettings(CButtonPlotSettings* _plotSettings);
public:
	DECLARE_MESSAGE_MAP()
	afx_msg LRESULT OnNcHitTest(CPoint point);
};

class CGraphBtnChestBeltEnv : public CGraphBtnBelt
{
public:
	CGraphBtnChestBeltEnv();
	~CGraphBtnChestBeltEnv();
	virtual void updateGraphSettings(void);
	virtual float getYAutoMax(void);
	void setData(CRespBeltDataSet *_dataSet);
	virtual void setEvents(CEvents *_eP);
	virtual bool getHasData(void);
	virtual void setEventsToShow(UINT _evToShow, bool _onlyShowEvents = false);
protected:
	void setData(vector <FLOAT> *_p,vector <FLOAT> *_tv = NULL) ;
	CRGLGraphSparse *curveA;
	virtual void createCurveSettings(CButtonPlotSettings *_plotSettings);
public:
	DECLARE_MESSAGE_MAP()
	afx_msg LRESULT OnNcHitTest(CPoint point);
};

class CGraphBtnAbdominalBeltEnv : public CGraphBtnBelt
{
public:
	CGraphBtnAbdominalBeltEnv();
	~CGraphBtnAbdominalBeltEnv();
	virtual void updateGraphSettings(void);
	virtual float getYAutoMax(void);
	void setData(CRespBeltDataSet *_dataSet);
	virtual void setEvents(CEvents *_eP);
	virtual bool getHasData(void);
	virtual void setEventsToShow(UINT _evToShow, bool _onlyShowEvents = false);
protected:
	void setData(vector <FLOAT> *_p,vector <FLOAT> *_tv = NULL) ;
	CRGLGraphSparse *curveA;
	virtual void createCurveSettings(CButtonPlotSettings *_plotSettings);
public:
	DECLARE_MESSAGE_MAP()
	afx_msg LRESULT OnNcHitTest(CPoint point);
};


//////////////////////////////////////////////////////////////////
//	Cannula family
//////////////////////////////////////////////////////////////////

class CGraphBtnCannula : public CGraphBtnChannel
{
public:
	CGraphBtnCannula();
	~CGraphBtnCannula();
	virtual void updateGraphSettings(void);
	virtual float getYAutoMax(void);
	void setData(CRespBeltDataSet *_dataSet);
	virtual void setEvents(CEvents *_eP);
	virtual bool getHasData(void);
	virtual void setEventsToShow(UINT _evToShow, bool _onlyShowEvents = false);
	virtual CString getTimeAndAmplAt(LONG _x,LONG _y);
protected:
	void setData(vector <FLOAT> *_p,vector <FLOAT> *_tv = NULL) ;
	CRespBeltDataSet *dataSet;
	CRGLGraphSparse *curveCa;
	virtual void createCurveSettings(CButtonPlotSettings *_plotSettings);
public:
	DECLARE_MESSAGE_MAP()
	afx_msg LRESULT OnNcHitTest(CPoint point);
};

class CGraphBtnCannulaEnv : public CGraphBtnCannula
{
public:
	CGraphBtnCannulaEnv();
	~CGraphBtnCannulaEnv();
	virtual void updateGraphSettings(void);
	virtual float getYAutoMax(void);
	void setData(CRespBeltDataSet *_dataSet);
	virtual void setEvents(CEvents *_eP);
	virtual bool getHasData(void);
	virtual void setEventsToShow(UINT _evToShow, bool _onlyShowEvents = false);
	virtual CString getTimeAndAmplAt(LONG _x,LONG _y);
protected:
	void setData(vector <FLOAT> *_p,vector <FLOAT> *_tv = NULL) ;
	CRGLGraphSparse *curveA;
	virtual void createCurveSettings(CButtonPlotSettings *_plotSettings);
public:
	DECLARE_MESSAGE_MAP()
	afx_msg LRESULT OnNcHitTest(CPoint point);
};
#endif