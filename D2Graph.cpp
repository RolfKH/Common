#include "stdafx.h"
#include "D2Graph.h"

/*!
* @author		RKH
* @date			6.6.17
*				Removed render() from CRGLGraph::setEventsToShow() as this occured outside BeginDraw()
* @copyright (C)	Spiro Medical AS 2013-2017
*/

#define _USE_MATH_DEFINES
#include <math.h>

#include <WinUser.h>

#include <vector>
#include <algorithm>
#include <numeric>
#include "RegDefs.h"
#include "dumpLog.h"


using namespace std;

extern int WM_NEW_EVENT_LENGTH;
extern int WM_CHANGE_ONE_EVENT;
extern int WM_POST_EVENT_VISUAL_EDIT_CHANGE;

extern CDumpLog *dmpLogP;

/////////////////////////////

CRGLBand::CRGLBand(float _minY, float _maxY, float _r, float _g, float _b, float _transp) :
	r(_r), g(_g), b(_b),
	transp(_transp), minY(_minY), maxY(_maxY),
	m_brush(NULL), dGeometry(NULL), dTransformedGeometry(NULL),
	theTileP(NULL)
{
}

CRGLBand::~CRGLBand()
{
	if (dGeometry) dGeometry->Release();
	dGeometry = NULL;

	if (dTransformedGeometry) dTransformedGeometry->Release();
	dTransformedGeometry = NULL;

	DiscardDeviceResources();
}

void CRGLBand::setTile(CRGLTile *_theTileP)
{
	theTileP = _theTileP;
}

void CRGLBand::render(ID2D1HwndRenderTarget *hwndRenderTarget)
{
	ASSERT(m_brush);
	ASSERT(dTransformedGeometry);

	hwndRenderTarget->FillGeometry(dTransformedGeometry, m_brush);
}

void CRGLBand::makeSourceGeometry(ID2D1Factory *_factory)
{
	ID2D1GeometrySink *sink = NULL;

	ASSERT(theTileP);
	ASSERT(_factory);

	if (dGeometry) dGeometry->Release();
	dGeometry = NULL;

	_factory->CreatePathGeometry(&dGeometry);
	D2D1_RECT_F plotRectScaled = theTileP->getPlotRectScaled();

	if (dGeometry) {
		dGeometry->Open(&sink);
		if (sink) {

			D2D1_POINT_2F d2d1p;
			d2d1p.x = plotRectScaled.left;
			d2d1p.y = minY;
			sink->BeginFigure(d2d1p, D2D1_FIGURE_BEGIN_FILLED);
			d2d1p.y = maxY;
			sink->AddLine(d2d1p);
			d2d1p.x = plotRectScaled.right;
			sink->AddLine(d2d1p);
			d2d1p.y = minY;
			sink->AddLine(d2d1p);
			d2d1p.x = plotRectScaled.left;
			sink->AddLine(d2d1p);

			sink->Close();
			sink->Release();
			sink = NULL;
		}
	}
}

void CRGLBand::makeTransformedGeometry(ID2D1Factory *_factory, D2D1::Matrix3x2F *_matPlotRect)
{
	if (dGeometry) {
		if (dTransformedGeometry) dTransformedGeometry->Release();
		dTransformedGeometry = NULL;
		HRESULT hr = _factory->CreateTransformedGeometry(dGeometry, *_matPlotRect, &dTransformedGeometry);
	}
}

void CRGLBand::CreateDeviceResources(ID2D1HwndRenderTarget *hwndRenderTarget, ID2D1Factory *_factory,
	D2D1::Matrix3x2F *_matPlotRect)
{
	if (m_brush) m_brush.Release();
	m_brush = NULL;
	HRESULT hr = hwndRenderTarget->CreateSolidColorBrush(D2D1::ColorF(r, g, b, transp), &m_brush);

	makeTransformedGeometry(_factory, _matPlotRect);
}

void CRGLBand::DiscardDeviceResources(void)
{
	if (m_brush) m_brush.Release();
	m_brush = NULL;

	if (dGeometry) dGeometry->Release();
	dGeometry = NULL;
	if (dTransformedGeometry) dTransformedGeometry->Release();
	dTransformedGeometry = NULL;
}

void CRGLBand::doGeometry(ID2D1Factory *_factory, D2D1::Matrix3x2F *_matPlotRect)
{
	makeSourceGeometry(_factory);
	makeTransformedGeometry(_factory, _matPlotRect);
}


////////////////////////////

CRGLTile::CRGLTile(D2D1_RECT_F _fullRect /*= D2D1::RectF(.0f,100.0f,100.0f,.0f)*/,
	D2D1_RECT_F _plotRect /*= RectD2D1::RectF(15.0f,90.0f,90.0f,15.0f)*/,
	D2D1_RECT_F _fullRectScaled /*= D2D1::RectF(.0f,1.0f,1.0f,.0f)*/,
	D2D1_RECT_F _plotRectScaled /*= D2D1::RectF(.0f,1.0f,1.0f,.0f)*/) :
	fullRect(_fullRect), plotRect(_plotRect), fullRectScaled(_fullRectScaled), plotRectScaled(_plotRectScaled), marked(false)
{
	changeFlag = 0xFF;  //Signal change in the beginning
	vertLimited = false;

	minLimit = maxLimit = .0f;
	minimumYRange = defaultMinimumYRange;

	extremeLeft = plotRectScaled.left;
	extremeRight = plotRect.right;

	lockTop = false;
}

CRGLTile::~CRGLTile() {}


void CRGLTile::setExtremeXLimits(FLOAT _left, FLOAT _right)
{
	extremeLeft = _left;
	extremeRight = _right;
}

void CRGLTile::getExtremeXLimits(FLOAT *_left, FLOAT *_right)
{
	*_left = extremeLeft;
	*_right = extremeRight;
}

void CRGLTile::mark(bool _on)
{
	marked = _on;
}

bool CRGLTile::getMark(void)
{
	return marked;
}

void CRGLTile::setLockTop(bool _on)
{
	lockTop = _on;
}

void CRGLTile::setMinimumYRange(float _my)
{
	minimumYRange = _my;
}

void CRGLTile::setVertScaleLimits(float _min, float _max)
{
	vertLimited = true;
	minLimit = _min;
	maxLimit = _max;
}

void CRGLTile::removeVertScaleLimits(void)
{
	vertLimited = false;
}

bool CRGLTile::isPointInTile(CRect _clientRect, CPoint _pt)
{

	int left = (int)((float)_clientRect.Width() * plotRect.left / 100.0f);
	if (_pt.x < left) return false;
	int right = (int)((float)_clientRect.Width() * plotRect.right / 100.0f);
	if (_pt.x > right) return false;

	int top = _clientRect.Height() - (int)((float)_clientRect.Height() * plotRect.top / 100.0f);
	if (_pt.y < top) return false;
	int bottom = _clientRect.Height() - (int)((float)_clientRect.Height() * plotRect.bottom / 100.0f);
	if (_pt.y > bottom) return false;

	return true;
}

int CRGLTile::getChangeFlag(void) { return changeFlag; }

void CRGLTile::resetChangeFlag(void) { changeFlag = 0x00; }

D2D1_RECT_F CRGLTile::getPlotRect(void) { return plotRect; }

D2D1_RECT_F CRGLTile::getPlotRectScaled(void) { return plotRectScaled; }

FLOAT CRGLTile::getPlotRectXScaled(void)
{
	return (plotRectScaled.right - plotRectScaled.left);
}

void CRGLTile::setPlotRectYMax(FLOAT _max)
{
	plotRectScaled.top = _max;
	changeFlag |= RGLTileChangeYScaled;
}

void CRGLTile::setPlotRectYScaled(FLOAT _origo, FLOAT _max)
{
	plotRectScaled.top = _max;
	plotRectScaled.bottom = _origo;
	changeFlag |= RGLTileChangeYScaled;
}

void CRGLTile::getPlotRectYScaled(FLOAT *_bottom, FLOAT *_top)
{
	*_bottom = plotRectScaled.bottom;
	*_top = plotRectScaled.top;
}

/*
Description: Sets the x-axis limits in engineering units (seconds)
_left: origin
_right: end
*/
void CRGLTile::setPlotRectXScaled(FLOAT _left, FLOAT _right)
{
	plotRectScaled.right = _right;
	plotRectScaled.left = _left;
	changeFlag |= RGLTileChangeXScaled;
}

D2D1_RECT_F CRGLTile::getFullRectScaled(void) { return fullRectScaled; }

D2D1_RECT_F CRGLTile::getFullRect(void) { return fullRect; }

void CRGLTile::setPlotRectScaled(D2D1_RECT_F _r)
{
	changeFlag |= RGLTileChangePlotRect;
	changeFlag |= RGLTileChangeXScaled;
	changeFlag |= RGLTileChangeYScaled;
	plotRectScaled = _r;
}

/*
Description: Shifts the x axis time window
_width: width of the client area (full rect)
_move: Cursor move
_rightLimit: new Left must be >= right limit so data do not disappear
Returns: true is able to move, false if not.
*/
bool CRGLTile::trackTimeShift(float _width, int _move, float _rightLimit)
{
	float plotRectWidthPercent = plotRect.right - plotRect.left;
	float plotRectWidthPixels = _width * plotRectWidthPercent / 100.0f;
	float engUnitWidth = plotRectScaled.right - plotRectScaled.left;

	float engUnitMove = engUnitWidth * _move / plotRectWidthPixels;

	float newLeft = plotRectScaled.left - engUnitMove;
	float newRight = plotRectScaled.right - engUnitMove;
	if (newRight < .0f) {
		float width = plotRectScaled.right - plotRectScaled.left;
		plotRectScaled.right = 1.0f;
		plotRectScaled.left = plotRectScaled.right - width;
		changeFlag |= RGLTileChangeXScaled;
		return true;
	}
	if (newLeft > _rightLimit) {
		float width = plotRectScaled.right - plotRectScaled.left;
		plotRectScaled.left = _rightLimit - 1.0f;
		plotRectScaled.right = plotRectScaled.left + width;
		changeFlag |= RGLTileChangeXScaled;
		return true;
	}
	plotRectScaled.left -= engUnitMove;
	plotRectScaled.right -= engUnitMove;
	changeFlag |= RGLTileChangeXScaled;

	return true;
}

/*
Description: Scales the x axis time window about centre
_ratio: Ratio change
Returns: true is able to scale, false if not.
*/
bool CRGLTile::trackTimeScaleAboutCentre(float _ratio)
{
	float halfWidth = (plotRectScaled.right - plotRectScaled.left) / 2.0f;
	halfWidth /= (1.0f + _ratio);
	float centre = (plotRectScaled.right + plotRectScaled.left) / 2.0f;

	halfWidth = halfWidth > (maxSecsOnScreen / 2.0f) ? maxSecsOnScreen / 2.0f : halfWidth;
	halfWidth = halfWidth < (minSecsOnScreen / 2.0f) ? minSecsOnScreen / 2.0f : halfWidth;

	plotRectScaled.left = centre - halfWidth;
	plotRectScaled.right = centre + halfWidth;

	changeFlag |= RGLTileChangeXScaled;

	return true;
}

/*
Description: Scales the x axis time window about _centre
_ratio: Ratio to zoom
_centre: Centre to zoom about - relative to full width
Returns: true is able to scale, false if not.
*/
bool CRGLTile::zoomTime(float _centre, float _ratio)
{

	float centreToLeft = (plotRectScaled.right - plotRectScaled.left) * _centre;
	float rightToCentre = (plotRectScaled.right - plotRectScaled.left) * (1.0f - _centre);
	float about = plotRectScaled.left + centreToLeft;

	//---Zoom
	centreToLeft /= (1.0f + _ratio);
	rightToCentre /= (1.0f + _ratio);

	float newprsleft = about - centreToLeft;
	float newprsright = about + rightToCentre;
	if ((newprsright - newprsleft) > maxSecsOnScreen) return false;
	if ((newprsright - newprsleft) < minSecsOnScreen) return false;

	plotRectScaled.left = newprsleft;
	plotRectScaled.right = newprsright;

	changeFlag |= RGLTileChangeXScaled;

	return true;
}

/*
Description: Scales the x axis time window about a given x-value
_ratio: ratio to change
_centre : Centre X in client coordinates
Returns: true is able to scale, false if not.
*/
bool CRGLTile::trackTimeScale(int _centre, float _ratio)
{
	float rightSide = (float)(plotRectScaled.right - _centre);
	float leftSide = (float)(_centre - plotRectScaled.left);
	rightSide /= (1.0f + _ratio);
	leftSide /= (1.0f + _ratio);
	plotRectScaled.left = _centre - leftSide;
	plotRectScaled.right = _centre + rightSide;

	changeFlag |= RGLTileChangeXScaled;

	return true;
}

/*
Description: Shifts the y axis
_height : Total height
_move: How much to shift
Returns: true is able to shift, false if not.
*/
bool CRGLTile::moveVert(float _height, float _move)
{
	float move = (float)_move / (float)_height;
	move *= (plotRectScaled.top - plotRectScaled.bottom);
	D2D1_RECT_F oldRect = plotRectScaled;
	oldRect.top -= move;
	oldRect.bottom -= move;

	if (vertLimited) {
		if (oldRect.top > maxLimit) return false;
		if (oldRect.bottom < minLimit) return false;
	}

	plotRectScaled = oldRect;
	changeFlag |= RGLTileChangeYScaled;
	return true;
}

/*
Description: Scales the y axis
_ratio: How much to move (ratio)
Returns: true is able to move, false if not.
*/
bool CRGLTile::scaleVert(float _ratio, bool _dualPolarity)
{
	if (!_dualPolarity) {
		float yRange = plotRectScaled.top - plotRectScaled.bottom;

		if (lockTop) {
			yRange = _ratio <= -1.0 ? yRange : yRange / (1.0f + _ratio);
			if (yRange > .1f) {
				plotRectScaled.bottom = plotRectScaled.top - yRange;
			}
		}
		else {
			yRange = _ratio <= -1.0 ? yRange : yRange / (1.0f + _ratio);
			if (yRange > .1f) {
				if (vertLimited) {
					if (yRange < (maxLimit - minLimit))
						plotRectScaled.top = plotRectScaled.bottom + yRange;
				}
				else plotRectScaled.top = plotRectScaled.bottom + yRange;
			}
		}
	}
	else {
		float yRange = plotRectScaled.top - plotRectScaled.bottom;
		float yCentre = (plotRectScaled.top + plotRectScaled.bottom) / 2.0f;

		yRange /= 2.0f;

		if (lockTop) {
			yRange = _ratio == 1.0 ? yRange : yRange / (1.0f - _ratio);
			if (yRange > minimumYRange)
				plotRectScaled.bottom = plotRectScaled.top - yRange * 2.0f;
		}
		else {
			yRange = _ratio <= -1.0 ? yRange : yRange / (1.0f + _ratio);
			if (yRange > minimumYRange) {
				plotRectScaled.top = yCentre + yRange;
				plotRectScaled.bottom = yCentre - yRange;
				TRACE(_T("top %.1f, bottom %.1f\n"), plotRectScaled.top, plotRectScaled.bottom);
			}
		}
	}

	changeFlag |= RGLTileChangeYScaled;

	return true;
}


/*
Description: Tracks the mouse y-position
_pt: Mouse position
Returns: true is able to move, false if not.
*/
bool CRGLTile::trackY(CPoint _pt)
{

	changeFlag |= RGLTileChangeYScaled;

	return true;
}

////////////////////////////

CRGLLayer::CRGLLayer(CString _regSection /* = _T("Generic Layer")*/) :
	regSection(_regSection), type(RGLLayerTypeGeneric)
{
	parentWnd = NULL;
	show = true;
	page = 0;
	section = 0;
	id = 0;

	mp_target = NULL;
	mp_factory = NULL;

	matFullRect = matFullRect.Identity();
	iMatFullRect = iMatFullRect.Identity();
	matPlotRect = matPlotRect.Identity();
	iMatPlotRect = iMatPlotRect.Identity();

	myTile = NULL;
}

CRGLLayer::~CRGLLayer()
{
}

FLOAT CRGLLayer::getPlotRectXScaled(void)
{
	if (!myTile) return 1.0f;
	return myTile->getPlotRectXScaled();
}

void CRGLLayer::setShow(bool _show)
{
	show = _show;
}

void CRGLLayer::setPageAndSection(int _page, int _section)
{
	page = _page;
	section = _section;
}

void CRGLLayer::getPageAndSection(int *_page, int *_section)
{
	*_page = page;
	*_section = section;
}

bool CRGLLayer::scaleY(float _ratio, bool _dualPolarity)
{
	if (myTile) return myTile->scaleVert(_ratio, _dualPolarity);
	else return false;
}

CRGLTile *CRGLLayer::getTheTile(void)
{
	return myTile;
}

bool CRGLLayer::trackY(CPoint _pt)
{
	return false;
}

bool CRGLLayer::getShow(void) { return show; }

void CRGLLayer::setColour(double _r, double _g, double _b) { /* empty */ }

void CRGLLayer::resetTileChangeFlag(void)
{
	if (myTile) myTile->resetChangeFlag();
}

CString CRGLLayer::getName(void)
{
	CString s = regSection;
	s.MakeReverse();
	s = s.SpanExcluding(_T("\\"));
	s.MakeReverse();
	return s;
}

void CRGLLayer::setParent(CWnd *_parentWnd) { parentWnd = _parentWnd; }

void CRGLLayer::resize(void)
{
	setFullRectMatrices();
	setPlotRectMatrices();
}

void CRGLLayer::setPlotRectMatrices(void)
{
	/*
	| m11 m12 0 |   | ratioX   0     0 |
	| m21 m22 0 | = |   0    ratioY  0 |
	| m31 m32 1 |   |   dx     dy    1 |
	*/

	if (!mp_target) return;
	if (!myTile) return;

	matPlotRect = matPlotRect.Identity();

	D2D1_SIZE_F size = mp_target->GetSize();


	//---My screen coordinates - unscaled
	D2D1_RECT_F plotRect = myTile->getPlotRect();
	double y1 = size.height * plotRect.top / 100.0f;
	double y0 = size.height * plotRect.bottom / 100.0f;
	double x0 = size.width * plotRect.left / 100.0f;
	double x1 = size.width * plotRect.right / 100.0f;

	//---Original rect coordinates (with swapped y-axis)
	double X0 = .0f;
	double X1 = size.width;
	double Y1 = size.height;
	double Y0 = .0f;

	//---Position the target rect on the screen and swap the y-axis
	double ratioX = 1.0f;
	double ratioY = -1.0f;
	double dx = .0f;
	double dy = Y1;

	matPlotRect._11 = (float)ratioX;
	matPlotRect._12 = .0f;
	matPlotRect._21 = .0f;
	matPlotRect._22 = (float)ratioY;
	matPlotRect._31 = (float)dx;
	matPlotRect._32 = (float)dy;

	//---Compute scaling matrix
	D2D1_RECT_F plotRectScaled = myTile->getPlotRectScaled();
	float xScale = (float)(x1 - x0) / (plotRectScaled.right - plotRectScaled.left);
	float yScale = (float)(y1 - y0) / (plotRectScaled.top - plotRectScaled.bottom);
	float xShift = (float)x0 - plotRectScaled.left * xScale;
	float yShift = (float)y0 - plotRectScaled.bottom * yScale;
	D2D1::Matrix3x2F m(xScale, .0f, .0f, yScale, xShift, yShift);

	//---Scaling
	matPlotRect = m * matPlotRect;

	if (matPlotRect.IsInvertible()) {
		iMatPlotRect = matPlotRect;
		iMatPlotRect.Invert();
	}
}

void CRGLLayer::processFullRect(void)
{
	setFullRectMatrices();
}

void CRGLLayer::processPlotRect(void)
{
	setPlotRectMatrices();
}

void CRGLLayer::setFullRectMatrices(void)
{
	/*
	| m11 m12 0 |   | ratioX   0     0 |
	| m21 m22 0 | = |   0    ratioY  0 |
	| m31 m32 1 |   |   dx     dy    1 |
	*/
	if (!mp_target) return;
	if (!myTile) return;

	matFullRect = matFullRect.Identity();

	D2D1_SIZE_F size = mp_target->GetSize();

	//---My screen coordinates - unscaled
	D2D1_RECT_F fullRect = myTile->getFullRect();
	double y1 = size.height * fullRect.top / 100.0f;
	double y0 = size.height * fullRect.bottom / 100.0f;
	double x0 = size.width * fullRect.left / 100.0f;
	double x1 = size.width * fullRect.right / 100.0f;

	//---Original rect coordinates (with swapped y-axis)
	double X0 = .0f;
	double X1 = size.width;
	double Y1 = size.height;
	double Y0 = .0f;

	//---Position the target rect on the screen and swap the y-axis
	double ratioX = 1.0f;
	double ratioY = -1.0f;
	double dx = .0f;
	double dy = Y1;

	matFullRect._11 = (float)ratioX;
	matFullRect._12 = .0f;
	matFullRect._21 = .0f;
	matFullRect._22 = (float)ratioY;
	matFullRect._31 = (float)dx;
	matFullRect._32 = (float)dy;

	//---Compute scaling matrix
	D2D1_RECT_F fullRectScaled = myTile->getFullRectScaled();
	float xScale = (float)(x1 - x0) / (fullRectScaled.right - fullRectScaled.left);
	float yScale = (float)(y1 - y0) / (fullRectScaled.top - fullRectScaled.bottom);
	float xShift = (float)x0 - fullRectScaled.left * xScale;
	float yShift = (float)y0 - fullRectScaled.bottom * yScale;
	D2D1::Matrix3x2F m(xScale, .0f, .0f, yScale, xShift, yShift);

	//---Scaling
	matFullRect = m * matFullRect;

	if (matFullRect.IsInvertible()) {
		iMatFullRect = matFullRect;
		iMatFullRect.Invert();
	}
}

void CRGLLayer::CreateDeviceResources(ID2D1HwndRenderTarget *hwndRenderTarget, ID2D1Factory *_factory)
{
	mp_target = hwndRenderTarget;
	mp_factory = _factory;
	setFullRectMatrices();
	setPlotRectMatrices();
}

void CRGLLayer::DiscardDeviceResources(void)
{
}

void CRGLLayer::updateMarkers(void)
{
	// Do nothing here - only Graph has markers
}

void CRGLLayer::render(void)
{
	if (!myTile) return;
	int changeFlag = myTile->getChangeFlag();

	if (changeFlag == 0) return;

	if (changeFlag & RGLTileChangeFullRect) {
		processFullRect();
		updateMarkers();
	}

	else if (changeFlag & RGLTileChangePlotRect) {
		processPlotRect();
		updateMarkers();
	}

	else if ((changeFlag & RGLTileChangeYScaled) || (changeFlag & RGLTileChangeXScaled)) {
		processPlotRect();
		updateMarkers();;
	}
}

int CRGLLayer::getLayerType(void) { return type; }

void CRGLLayer::launchParameterDialog(void) {}

void CRGLLayer::setTile(CRGLTile *_tile)
{
	myTile = _tile;
	if (!myTile) return;

	if (RGLTileChangeFullRect)
		processFullRect();
	if (RGLTileChangePlotRect)
		processPlotRect();
}

void CRGLLayer::makeSourceGeometry(void) {}

void CRGLLayer::makeTransformedGeometry(void) {}

/////////////////////////////

CRGLGraphSparse::CRGLGraphSparse(CString _regSection /* = _T("Graph Sparse")*/, bool _hideEvents /* = false */) :
	CRGLGraph(_regSection,_hideEvents)
{
	type = RGLLayerTypeGraphSparse;
	dSparseGeometry = NULL;
	dSparseTransformedGeometry = NULL;
	sparseStep = defaultSparseStep;
}

CRGLGraphSparse::~CRGLGraphSparse()
{
	if (dSparseGeometry) dSparseGeometry->Release();
	if (dSparseTransformedGeometry) dSparseTransformedGeometry->Release();
}

void CRGLGraphSparse::makeTransformedGeometry(void)
{
	CRGLGraph::makeTransformedGeometry();

	if (!mp_factory) return; 

	if (dSparseGeometry) {
		if (dSparseTransformedGeometry) dSparseTransformedGeometry->Release();
		dSparseTransformedGeometry = NULL;
		HRESULT hr = mp_factory->CreateTransformedGeometry(dSparseGeometry, matPlotRect, &dSparseTransformedGeometry);
	}
}

void CRGLGraphSparse::makeSourceGeometry(bool _reload)
{
	makeCursorGeometries();
	if (eventsP) {
		for (int i = 0; i < evCountWithoutLevel; i++) makeSourceGeometryForEventsWithoutLevel(i);
		for (int i = 0; i < evCountWithLevel; i++) {
			for (int j = 0; j < bgLevelTypeCount; j++) {
				makeSourceGeometryForEventsWithLevel(i, j);
			}
		}
	}
	else {
		if (mMarkerStartStopVector)
			if (mMarkerStartStopVector->size()) makeSourceGeometryForManMarkers();
	}

	int numB = bandArray.GetCount();
	for (int i = numB - 1; i >= 0; --i) {
		CRGLBand *bP = bandArray.GetAt(i);
		bP->makeSourceGeometry(mp_factory);
	}

	if (!myTile) return;
	if (!dataVector) return;

	D2D1_RECT_F plotRectScaled = myTile->getPlotRectScaled();

	//---Check if we have to reload the source geometry
	if (!_reload) {
		if ((plotRectScaled.left > oldSourceGeometryLeft) && (plotRectScaled.right < oldSourceGeometryRight)) {
			return;
		}
	}

	int num = (int)dataVector->size();
	if (0 == num) return;
	if (!mp_factory) return;

	FLOAT dist = plotRectScaled.right - plotRectScaled.left;

	ASSERT(timeVector);
	if (timeVector->size() > 0) {
		if (dSparseGeometry) dSparseGeometry->Release();
		dSparseGeometry = NULL;
		if (dGeometry) dGeometry->Release();
		dGeometry = NULL;

		ID2D1GeometrySink *sink = NULL;
		ID2D1GeometrySink *sparseSink = NULL;

		mp_factory->CreatePathGeometry(&dSparseGeometry);
		mp_factory->CreatePathGeometry(&dGeometry);

		FLOAT requestedLength = plotRectScaled.right - plotRectScaled.left;
		FLOAT midPoint = (plotRectScaled.right + plotRectScaled.left) / 2.0f;

		oldSourceGeometryLeft = midPoint - (SCREEN_LENGTH_FACTOR * requestedLength) / 2.0f;
		oldSourceGeometryRight = midPoint + (SCREEN_LENGTH_FACTOR * requestedLength) / 2.0f;

		FLOAT startTime = oldSourceGeometryLeft;
		FLOAT stopTime = oldSourceGeometryRight;

		if (NULL == penDown) {

			//---Dense
			if (dGeometry) {
				dGeometry->Open(&sink);
				if (sink) {
					unsigned int distance1 = distance(timeVector->begin(), lower_bound(timeVector->begin(), timeVector->end(), startTime));
					unsigned int distance2 = distance(timeVector->begin(), lower_bound(timeVector->begin(), timeVector->end(), stopTime));
					if (distance1 < dataVector->size()) {
						D2D1_POINT_2F d2d1p;

						vector <FLOAT>::iterator ti = timeVector->begin();
						ti += distance1;
						vector <FLOAT>::iterator di = dataVector->begin();
						di += distance1;

						d2d1p.x = *ti;
						d2d1p.y = *di;
						sink->BeginFigure(d2d1p, D2D1_FIGURE_BEGIN_HOLLOW);
						unsigned int limit = distance2 < dataVector->size() ? distance2 : dataVector->size();

						vector <FLOAT>::iterator dlim = dataVector->begin();
						dlim += limit;

						for (; di < dlim; ++di,++ti) {
							d2d1p.x = *ti;
							d2d1p.y = *di;
							sink->AddLine(d2d1p);
						}
						sink->EndFigure(D2D1_FIGURE_END_OPEN);
						sink->Close();
						sink->Release();
						sink = NULL;
					}
				}

				//---Sparse
				if (dSparseGeometry) {
					dSparseGeometry->Open(&sparseSink);
					if (sparseSink) {
						unsigned int distance1 = distance(timeVector->begin(), lower_bound(timeVector->begin(), timeVector->end(), startTime));
						unsigned int distance2 = distance(timeVector->begin(), lower_bound(timeVector->begin(), timeVector->end(), stopTime));
						if (distance1 < dataVector->size()) {
							D2D1_POINT_2F d2d1p;

							vector <FLOAT>::iterator ti = timeVector->begin();
							ti += distance1;
							vector <FLOAT>::iterator di = dataVector->begin();
							di += distance1;

							d2d1p.x = *ti;
							d2d1p.y = *di;
							sparseSink->BeginFigure(d2d1p, D2D1_FIGURE_BEGIN_HOLLOW);
							unsigned int limit = distance2 < dataVector->size() - sparseStep ? distance2 : dataVector->size() - sparseStep; 
							
							vector <FLOAT>::iterator dlim = dataVector->begin();
							dlim += limit;

							while (di < dlim) {
								d2d1p.x = *ti;
								d2d1p.y = *di;
								sparseSink->AddLine(d2d1p);
								di += sparseStep;
								ti += sparseStep;
							}
							sparseSink->EndFigure(D2D1_FIGURE_END_OPEN);
							sparseSink->Close();
							sparseSink->Release();
							sparseSink = NULL;
						}
					}
				}
			}
		}
		if (NULL != penDown) {

			ASSERT(penDown->size() == dataVector->size());

			//---Dense
			if (dGeometry) {
				dGeometry->Open(&sink);
				if (sink) {
					D2D1_POINT_2F d2d1p;
					BOOL pDown, wasDown;
					unsigned int distance1 = distance(timeVector->begin(), lower_bound(timeVector->begin(), timeVector->end(), startTime));
					unsigned int distance2 = distance(timeVector->begin(), lower_bound(timeVector->begin(), timeVector->end(), stopTime));
					if (distance1 < dataVector->size()) {
						vector <FLOAT>::iterator ti = timeVector->begin();
						ti += distance1;
						vector <FLOAT>::iterator di = dataVector->begin();
						di += distance1;
						vector <BOOL>::iterator pdi = penDown->begin();
						pdi += distance1;

						d2d1p.x = *ti;
						d2d1p.y = *di;
						pDown = penDown->size() > distance1 ? penDown->at(distance1) : TRUE;

						if (pDown) {
							sink->BeginFigure(d2d1p, D2D1_FIGURE_BEGIN_HOLLOW);
						}
						wasDown = pDown;
						unsigned int limit = distance2 < dataVector->size() ? distance2 : dataVector->size();

						vector <FLOAT>::iterator dlim = dataVector->begin();
						dlim += limit;
						for (; di < dlim; ++di,++ti,++pdi) {
							d2d1p.x = *ti;
							d2d1p.y = *di;
							pDown = *pdi;
							if (wasDown) {
								sink->AddLine(d2d1p);
							}
							if (pDown && !wasDown) {
								sink->BeginFigure(d2d1p, D2D1_FIGURE_BEGIN_HOLLOW);
							}
							else if (!pDown && wasDown) {
								sink->EndFigure(D2D1_FIGURE_END_OPEN);
							}
							wasDown = pDown;
						}
						if (wasDown) {
							sink->EndFigure(D2D1_FIGURE_END_OPEN);
						}
					}
					sink->Close();
					sink->Release();
					sink = NULL;
				}

				//---Sparse
				if (dSparseGeometry) {
					dSparseGeometry->Open(&sparseSink);
					if (sparseSink) {
						D2D1_POINT_2F d2d1p;
						BOOL pDown, wasDown;
						unsigned int distance1 = distance(timeVector->begin(), lower_bound(timeVector->begin(), timeVector->end(), startTime));
						unsigned int distance2 = distance(timeVector->begin(), lower_bound(timeVector->begin(), timeVector->end(), stopTime));
						if (distance1 < dataVector->size()) {
							vector <FLOAT>::iterator ti = timeVector->begin();
							ti += distance1;
							vector <FLOAT>::iterator di = dataVector->begin();
							di += distance1;
							vector <BOOL>::iterator pdi = penDown->begin();
							pdi += distance1;

							d2d1p.x = *ti;
							d2d1p.y = *di;
							pDown = *pdi;

							if (pDown) {
								sparseSink->BeginFigure(d2d1p, D2D1_FIGURE_BEGIN_HOLLOW);
							}
							wasDown = pDown;
							
							unsigned int limit = distance2 < dataVector->size() - sparseStep ? distance2 : 
														dataVector->size() - sparseStep;
							vector <FLOAT>::iterator diLim = dataVector->begin();
							diLim += limit;

							while (di < diLim) {
								d2d1p.x = *ti;
								d2d1p.y = *di;
								pDown = *pdi;
								if (wasDown) {
									sparseSink->AddLine(d2d1p);
								}
								if (pDown && !wasDown) {
									sparseSink->BeginFigure(d2d1p, D2D1_FIGURE_BEGIN_HOLLOW);
								}
								else if (!pDown && wasDown) {
									sparseSink->EndFigure(D2D1_FIGURE_END_OPEN);
								}
								wasDown = pDown;
								int i = 0;
								while ((di < diLim) && (i < sparseStep)) {
									if (wasDown != *pdi) break; 
									di++;
									ti++;
									pdi++;
									i++;
								}
							}
							if (wasDown) {
								sparseSink->EndFigure(D2D1_FIGURE_END_OPEN);
							}
						}
						sparseSink->Close();
						sparseSink->Release();
						sparseSink = NULL;
					}
				}
			}
		}
	}
}

void CRGLGraphSparse::render(void)
{
	if (!show) return;

	ASSERT(mp_target);

	CRGLLayer::render();
	mp_target->Flush();

	ASSERT(m_brush);
	ASSERT(m_brushCursor);
	ASSERT(m_brushCursorWindow);
	for (int i = 0; i < bgLevelTypeCount; i++) ASSERT(m_brushStockLevel[i]);

	ASSERT(myTile);

	D2D1_RECT_F plotRectScaled = myTile->getPlotRectScaled();

	D2D1_POINT_2F p0 = D2D1::Point2F(plotRectScaled.left, plotRectScaled.top);
	D2D1_POINT_2F p1 = D2D1::Point2F(plotRectScaled.right, plotRectScaled.bottom);
	p0 = p0 * matPlotRect;
	p1 = p1 * matPlotRect;
	clipRect = D2D1::RectF(p0.x, p0.y, p1.x, p1.y);

	mp_target->PushAxisAlignedClip(clipRect, D2D1_ANTIALIAS_MODE_PER_PRIMITIVE);

	FLOAT dist = plotRectScaled.right - plotRectScaled.left;

	bool doPlot = true;
	if ((upperTimeSpanLimit >= .0f) && (dist > upperTimeSpanLimit)) doPlot = false;
	if ((lowerTimeSpanLimit >= .0f) && (dist < lowerTimeSpanLimit)) doPlot = false;

	if (doPlot) {

		int numB = bandArray.GetCount();
		for (int i = numB - 1; i >= 0; --i) {
			CRGLBand *bP = bandArray.GetAt(i);
			bP->render(mp_target);
		}

		if (dist < sparseDensePlotLimit) {
			if (dTransformedGeometry) {
				mp_target->DrawGeometry(dTransformedGeometry, m_brush, (float)dthickness);
				mp_target->Flush();
			}
		}
		else {
			if (dSparseTransformedGeometry) {
				mp_target->DrawGeometry(dSparseTransformedGeometry, m_brush, (float) dthickness);
				mp_target->Flush();
			}
		}
	}
	mp_target->PopAxisAlignedClip();
	mp_target->Flush();
}

void CRGLGraphSparse::DiscardDeviceResources(void)
{
	CRGLGraph::DiscardDeviceResources();
	if (dSparseGeometry) dSparseGeometry->Release();
	dSparseGeometry = NULL;

	if (dSparseTransformedGeometry) dSparseTransformedGeometry->Release();
	dSparseTransformedGeometry = NULL;
}

void CRGLGraphSparse::setSparseStepSize(const int _step)
{
	sparseStep = _step;
}


/////////////////////////////

CRGLGraphFilled::CRGLGraphFilled(CString _regSection /* = _T("Graph Filled") */) :
	CRGLGraph(_regSection)
{
	type = RGLLayerTypeGraphFilled;
}

CRGLGraphFilled::~CRGLGraphFilled()
{
}

void CRGLGraphFilled::makeTransformedGeometry(void)
{
	if (!mp_factory) return;

	if (dGeometry) {
		if (dTransformedGeometry) dTransformedGeometry->Release();
		dTransformedGeometry = NULL;

		HRESULT hrPos = mp_factory->CreateTransformedGeometry(dGeometry, matPlotRect, &dTransformedGeometry);
	}
}

void CRGLGraphFilled::makeSourceGeometry(bool _reload)
{
	if (!myTile) return;

	if (!mp_factory) return;

	//---Check if we have to reload the source geometry
	D2D1_RECT_F plotRectScaled = myTile->getPlotRectScaled();
	if ((plotRectScaled.left > oldSourceGeometryLeft) && (plotRectScaled.right < oldSourceGeometryRight)) {
		return;
	}

	FLOAT dist = plotRectScaled.right - plotRectScaled.left;

	//---Envelopes
	if (dGeometry) dGeometry->Release();
	dGeometry = NULL;


	ID2D1GeometrySink *sink = NULL;

	mp_factory->CreatePathGeometry(&dGeometry);
	if (dGeometry) {
		dGeometry->Open(&sink);
		if (sink) {
			ASSERT(timeVector);
			if ((timeVector->size() > 0) && (sint == .0f)) {
				vector <FLOAT>::iterator ite, itet;
				D2D1_POINT_2F d2d1p;
				itet = timeVector->begin();
				d2d1p.x = .0f;
				d2d1p.y = .0f;
				sink->BeginFigure(d2d1p, D2D1_FIGURE_BEGIN_FILLED);

				for (ite = dataVector->begin(); ite != dataVector->end(); ite++) {
					d2d1p.x = *itet;
					d2d1p.y = *ite;
					sink->AddLine(d2d1p);
					itet++;
				}

				sink->EndFigure(D2D1_FIGURE_END_CLOSED);
				sink->Close();
				sink->Release();
				sink = NULL;
			}
		}
	}
}

void CRGLGraphFilled::render(void)
{
	if (!show) return;

	CRGLLayer::render();

	if (!mp_target) return;
	if (!m_brush) return;
	if (!myTile) return;

	D2D1_RECT_F plotRectScaled = myTile->getPlotRectScaled();

	D2D1_POINT_2F p0 = D2D1::Point2F(plotRectScaled.left, plotRectScaled.top);
	D2D1_POINT_2F p1 = D2D1::Point2F(plotRectScaled.right, plotRectScaled.bottom);
	p0 = p0 * matPlotRect;
	p1 = p1 * matPlotRect;
	clipRect = D2D1::RectF(p0.x, p0.y, p1.x, p1.y);
	mp_target->PushAxisAlignedClip(clipRect, D2D1_ANTIALIAS_MODE_PER_PRIMITIVE);

	FLOAT dist = plotRectScaled.right - plotRectScaled.left;

	bool doPlot = true;
	if ((upperTimeSpanLimit >= .0f) && (dist > upperTimeSpanLimit)) doPlot = false;
	if ((lowerTimeSpanLimit >= .0f) && (dist < lowerTimeSpanLimit)) doPlot = false;

	if (doPlot && dTransformedGeometry)
		mp_target->FillGeometry(dTransformedGeometry, m_brush);

	mp_target->PopAxisAlignedClip();
}

bool CRGLGraphFilled::isOver(CPoint _point)
{
	D2D1_POINT_2F p = D2D1::Point2F((float)_point.x, (float)_point.y);

	if ((p.x < clipRect.left) || (p.x > clipRect.right)) return false;
	if ((p.y < clipRect.top) || (p.y > clipRect.bottom)) return false;

	int ret = FALSE;
	D2D1::Matrix3x2F mat = D2D1::Matrix3x2F::Identity();


	if (dTransformedGeometry) dTransformedGeometry->StrokeContainsPoint(p, 3.0f, NULL, mat, &ret);

	return ret ? true : false;
}

////////////////////////////

CRGLGraph::CRGLGraph(CString _regSection /* = _T("Graph") */, bool _hideEvents/* = false*/) :
	regSection(_regSection), hideEvents(_hideEvents)
{
	debugTag = 0;
	dr = dg = db = .0f;
	dtransparency = 1.0f;
	dthickness = 1.0f;
	type = RGLLayerTypeGraph;

	clipRect = D2D1::RectF(0,0,10,10);

	showCursorWnd = false;

	leftCursorTop.x = 3600.0f;
	leftCursorBottom.x = 3600.0f;
	rightCursorTop.x = 7200.0f;
	rightCursorBottom.x = 7200.0f;
	leftCursorTop.y = 100.0f;
	leftCursorBottom.y = .0f;
	rightCursorTop.y = 100.0f;
	rightCursorBottom.y = .0f;

	dataID = 0;
	dataType = 0;
	dataFamily = 0;

	eventsP = NULL;

	dGeometry = NULL;
	dTransformedGeometry = NULL;

	for (int i = 0; i < evCountWithoutLevel; i++) {
		dGeometryEventWithoutLevel[i] = NULL;
		dTransfGeometryEventWithoutLevel[i] = NULL;
	}
	for (int i = 0; i < evCountWithLevel; i++) {
		for (int j = 0; j < bgLevelTypeCount; j++) {
			dGeometryEventWithLevel[i][j] = NULL;
			dTransfGeometryEventWithLevel[i][j] = NULL;
		}
	}

	dGeometryCursorLeft = NULL;
	dGeometryCursorRight = NULL;
	dGeometryCursorTimeWindow = NULL;
	dTransfGeometryCursorLeft = NULL;
	dTransfGeometryCursorRight = NULL;
	dTransfGeometryCursorTimeWindow = NULL;

	dataVector = NULL;
	timeVector = NULL;
	penDown = NULL;
	mMarkerStartStopVector = NULL;

	eventsToShowG = evntTypeNone;
	eventsAlreadyShown = 0;

	sint = .0f;

	oldSourceGeometryLeft = oldSourceGeometryRight = .0f;

	m_brush = NULL;
	m_brushCursor = NULL;
	m_brushCursorWindow = NULL;
	m_opacityBrush = NULL;
	for (int i = 0; i < bgLevelTypeCount; i++) m_brushStockLevel[i] = NULL;
	for (int i = 0; i < evCountWithoutLevel; i++) m_brushStockEventWOLevel[i] = NULL;

	upperTimeSpanLimit = lowerTimeSpanLimit = -1.0f;
}

CRGLGraph::~CRGLGraph()
{
	if (m_brush) m_brush.Release();
	for (int i = 0; i < bgLevelTypeCount; i++) m_brushStockLevel[i].Release();
	for (int i = 0; i < evCountWithoutLevel; i++) m_brushStockEventWOLevel[i].Release();

	if (m_brushCursor) m_brushCursor.Release();
	if (m_brushCursorWindow) m_brushCursorWindow.Release();
	if (m_opacityBrush) m_opacityBrush.Release();

	if (dGeometry) dGeometry->Release();
	if (dTransformedGeometry) dTransformedGeometry->Release();
	if (dGeometryCursorLeft) dGeometryCursorLeft->Release();
	if (dGeometryCursorRight) dGeometryCursorRight->Release();
	if (dGeometryCursorTimeWindow) dGeometryCursorTimeWindow->Release();
	if (dTransfGeometryCursorLeft) dTransfGeometryCursorLeft->Release();
	if (dTransfGeometryCursorRight) dTransfGeometryCursorRight->Release();
	if (dTransfGeometryCursorTimeWindow) dTransfGeometryCursorTimeWindow->Release();

	for (int i = 0; i < evCountWithoutLevel; i++) {
		if (dGeometryEventWithoutLevel[i]) dGeometryEventWithoutLevel[i]->Release();
		if (dTransfGeometryEventWithoutLevel[i]) dTransfGeometryEventWithoutLevel[i]->Release();
	}
	for (int i = 0; i < evCountWithLevel; i++) {
		for (int j = 0; j < bgLevelTypeCount; j++) {
			if (dGeometryEventWithLevel[i][j]) dGeometryEventWithLevel[i][j]->Release();
			if (dTransfGeometryEventWithLevel[i][j]) dTransfGeometryEventWithLevel[i][j]->Release();
		}
	}
	int numB = bandArray.GetCount();
	for (int i = numB - 1; i >= 0; --i) {
		CRGLBand *bP = bandArray.GetAt(i);
		bP->DiscardDeviceResources();
		delete bP;
	}
}

void CRGLGraph::addBand(CRGLBand *_bP)
{
	_bP->setTile(myTile);
	_bP->CreateDeviceResources(mp_target, mp_factory, &matPlotRect);
	bandArray.Add(_bP);
}
vector<FLOAT>* CRGLGraph::getDataVector(void)
{
	return dataVector;
}
vector<FLOAT>* CRGLGraph::getTimeVector(void)
{
	return timeVector;
}
void CRGLGraph::setDebugTag(int _tag)
{
	debugTag = _tag;
}

void CRGLGraph::setIDTypeFamily(const UINT _id, const UINT _type, const UINT _family)
{
	dataID = _id;
	dataType = _type;
	dataFamily = _family;
}

void CRGLGraph::getIDTypeFamily(UINT *_id, UINT *_type, UINT *_family)
{
	*_id = dataID;
	*_type = dataType;
	*_family = dataFamily;
}


CString CRGLGraph::getName(void)
{
	CString s = regSection;
	s.MakeReverse();
	s = s.SpanExcluding(_T("\\"));
	s.MakeReverse();
	return s;
}

/*
Description: If y-axis change, markers have to be updated to cover the entire vertical extent
*/
void CRGLGraph::updateMarkers(void)
{
	makeCursorGeometries();
	if (eventsP) {
		for (int i = 0; i < evCountWithoutLevel; i++) makeSourceGeometryForEventsWithoutLevel(i);
		for (int i = 0; i < evCountWithLevel; i++) {
			for (int j = 0; j < bgLevelTypeCount; j++) {
				makeSourceGeometryForEventsWithLevel(i, j);
			}
		}
	}
	else {
		if (mMarkerStartStopVector)
			if (mMarkerStartStopVector->size()) makeSourceGeometryForManMarkers();
	}
}

/*
Description: Update the brush as this may have been changed
_parameter: used for airMic as this has sub-parameters
*/
void CRGLGraph::updateBrush(CString _parameter)
{
	CXTPRegistryManager *reg = new CXTPRegistryManager;
	CString profile = KEY_GRAPHICS;
	profile += _T("\\");
	profile += GRAPH_BUTTONS;
	profile += _T("\\");
	profile += _parameter.IsEmpty() ? regSection : _parameter;

	reg->SetRegistryKey(COMPANY_NAME, profile);
	COLORREF col;
	BOOL ok = reg->GetProfileColor(regSection, CURVE_COLOUR, &col);
	if (ok) setColour(col);

	if (m_brush) m_brush.Release();
	m_brush = NULL;
	HRESULT hr = mp_target->CreateSolidColorBrush(D2D1::ColorF((float)dr, (float)dg, (float)db,
		(float)dtransparency), &m_brush);
}
void CRGLGraph::renderEvent(UINT _evntType)
{
	if (!eventsP) return;  // In AGSBrowser, thsi is the case

	if (hideEvents) return;

	if (eventsAlreadyShown & _evntType)
		return;

	int theenum = -1;
	bool wLevel = false;
	switch (_evntType) {
	case evntTypeNone:
		break;
	case evntTypeMixed:
		theenum = evMix;
		wLevel = true;
		break;
	case evntTypeCentral:
		theenum = evCentral;
		break;
	case evntTypeObstr:
		theenum = evObstr;
		wLevel = true;
		break;
	case evntTypeHypoCentral:
		theenum = evCentralHypo;
		break;
	case evntTypeHypoObstr:
		theenum = evObstrHypo;
		wLevel = true;
		break;
	case evntTypeRERA:
		theenum = evRERA;
		wLevel = true;
		break;
	case evntTypeArousal:
		theenum = evArousal;
		break;
	case evntTypeSpO2Drop:
		theenum = evSpO2;
		break;
	case evntTypeManMarker:
		theenum = evManual;
		break;
	case evntTypeSnoring:
		theenum = evSnoringAllLevels;
		wLevel = true;
		break;
	case evntTypeAwake:
		theenum = evAwake;
		break;
	case evntTypeExcluded:
		theenum = evExcluded;
		break;
	case evntTypeSwallow:
		theenum = evSwallow;
		break;
	case evntTypeImported:
		theenum = evImported;
		break;
	default:
		break;
	}

	if (-1 == theenum) return;

	ASSERT(eventsP);
	int majorVersion = 0;
	int minorVersion = 0;
	eventsP->getVersion(&majorVersion, &minorVersion);

	if (wLevel) {
		if ((evSnoringAllLevels == theenum) && (20 < majorVersion)) { // Special case
			if (dTransfGeometryEventWithLevel[evSnoringLowerLevel][bgLevelTypeLower]) {
				m_brushStockLevel[bgLevelTypeLower]->SetOpacity(opacityWithLevel[evSnoringLowerLevel]);
				mp_target->FillGeometry(dTransfGeometryEventWithLevel[evSnoringLowerLevel][bgLevelTypeLower], m_brushStockLevel[bgLevelTypeLower]);
			}
			if (dTransfGeometryEventWithLevel[evSnoringUpperLevel][bgLevelTypeUpper]) {
				m_brushStockLevel[bgLevelTypeUpper]->SetOpacity(opacityWithLevel[evSnoringUpperLevel]);
				mp_target->FillGeometry(dTransfGeometryEventWithLevel[evSnoringUpperLevel][bgLevelTypeUpper], m_brushStockLevel[bgLevelTypeUpper]);
			}
			if (dTransfGeometryEventWithLevel[evSnoringMultiLevel][bgLevelTypeMulti]) {
				m_brushStockLevel[bgLevelTypeMulti]->SetOpacity(opacityWithLevel[evSnoringMultiLevel]);
				mp_target->FillGeometry(dTransfGeometryEventWithLevel[evSnoringMultiLevel][bgLevelTypeMulti], m_brushStockLevel[bgLevelTypeMulti]);
			}
			if (dTransfGeometryEventWithLevel[evSnoringUndefLevel][bgLevelTypeUndef]) {
				m_brushStockLevel[bgLevelTypeUndef]->SetOpacity(opacityWithLevel[evSnoringUndefLevel]);
				mp_target->FillGeometry(dTransfGeometryEventWithLevel[evSnoringUndefLevel][bgLevelTypeUndef], m_brushStockLevel[bgLevelTypeUndef]);
			}
		}
		else {
			for (int i = 0; i < bgLevelTypeCount; i++) {
				if (dTransfGeometryEventWithLevel[theenum][i]) {
					m_brushStockLevel[i]->SetOpacity(opacityWithLevel[theenum]);
					mp_target->FillGeometry(dTransfGeometryEventWithLevel[theenum][i], m_brushStockLevel[i]);
				}
			}
		}
	}
	else {
		m_brushStockEventWOLevel[theenum]->SetOpacity(opacityWithoutLevel[theenum]);
		if (dTransfGeometryEventWithoutLevel[theenum]) {
			if (evManual != theenum)
				mp_target->FillGeometry(dTransfGeometryEventWithoutLevel[theenum], m_brushStockEventWOLevel[theenum]);
			else 
				mp_target->DrawGeometry(dTransfGeometryEventWithoutLevel[theenum], m_brushStockEventWOLevel[theenum],2.0f);
		}
	}
}

void CRGLGraph::render()
{
	// OK here
	if (!show) return;

	ASSERT(mp_target);

	CRGLLayer::render();
	mp_target->Flush();

	ASSERT(m_brush);
	ASSERT(m_brushCursor);
	ASSERT(m_brushCursorWindow);
	for (int i = 0; i < bgLevelTypeCount; i++) ASSERT(m_brushStockLevel[i]);

	// OK here

	ASSERT(myTile);

	D2D1_RECT_F plotRectScaled = myTile->getPlotRectScaled();

	D2D1_POINT_2F p0 = D2D1::Point2F(plotRectScaled.left, plotRectScaled.top);
	D2D1_POINT_2F p1 = D2D1::Point2F(plotRectScaled.right, plotRectScaled.bottom);
	p0 = p0 * matPlotRect;
	p1 = p1 * matPlotRect;
	clipRect = D2D1::RectF(p0.x, p0.y, p1.x, p1.y);

	// OK here

	mp_target->PushAxisAlignedClip(clipRect, D2D1_ANTIALIAS_MODE_PER_PRIMITIVE);

	FLOAT dist = plotRectScaled.right - plotRectScaled.left;

	bool doPlot = true;
	if ((upperTimeSpanLimit >= .0f) && (dist > upperTimeSpanLimit)) doPlot = false;
	if ((lowerTimeSpanLimit >= .0f) && (dist < lowerTimeSpanLimit)) doPlot = false;

	if (doPlot) {

		int numB = bandArray.GetCount();
		for (int i = numB - 1; i >= 0; --i) {
			CRGLBand *bP = bandArray.GetAt(i);
			bP->render(mp_target);
		}

		if (dTransformedGeometry) {
			mp_target->DrawGeometry(dTransformedGeometry, m_brush, (float)dthickness);
			mp_target->Flush();
		}
	}
	mp_target->PopAxisAlignedClip();
	mp_target->Flush();
}

void CRGLGraph::renderEvents(bool _reset /* = false */)
{
	if (!show) return;

	ASSERT(mp_target);
	ASSERT(m_brushCursorWindow);
	ASSERT(m_brushCursor);
	ASSERT(m_opacityBrush);
	ASSERT(myTile);

	D2D1_RECT_F plotRectScaled = myTile->getPlotRectScaled();
	D2D1_POINT_2F p0 = D2D1::Point2F(plotRectScaled.left, plotRectScaled.top);
	D2D1_POINT_2F p1 = D2D1::Point2F(plotRectScaled.right, plotRectScaled.bottom);
	p0 = p0 * matPlotRect;
	p1 = p1 * matPlotRect;
	clipRect = D2D1::RectF(p0.x, p0.y, p1.x, p1.y);

	mp_target->PushAxisAlignedClip(clipRect, D2D1_ANTIALIAS_MODE_PER_PRIMITIVE);

	FLOAT dist = plotRectScaled.right - plotRectScaled.left;

	bool doPlot = true;
	if ((upperTimeSpanLimit >= .0f) && (dist > upperTimeSpanLimit)) doPlot = false;
	if ((lowerTimeSpanLimit >= .0f) && (dist < lowerTimeSpanLimit)) doPlot = false;
	if (!doPlot) {
		mp_target->PopAxisAlignedClip();
		return;
	}
	
	ID2D1Layer *pLayer = NULL;								
	HRESULT hr = mp_target->CreateLayer(NULL, &pLayer);		
	if (SUCCEEDED(hr)) {
	
		mp_target->PushLayer(								
			D2D1::LayerParameters(
				D2D1::InfiniteRect(),
				NULL,
				D2D1_ANTIALIAS_MODE_PER_PRIMITIVE,
				D2D1::IdentityMatrix(),
				1.0f,
				m_opacityBrush,
				D2D1_LAYER_OPTIONS_NONE),
			pLayer
			);

		if (_reset) eventsAlreadyShown = 0;  // Draw all events!

		if (eventsToShowG & evntTypeSpO2Drop) renderEvent(evntTypeSpO2Drop);
		if (eventsToShowG & evntTypeHypoCentral) renderEvent(evntTypeHypoCentral);
		if (eventsToShowG & evntTypeHypoObstr) renderEvent(evntTypeHypoObstr);
		if (eventsToShowG & evntTypeCentral) renderEvent(evntTypeCentral);
		if (eventsToShowG & evntTypeMixed) renderEvent(evntTypeMixed);
		if (eventsToShowG & evntTypeObstr) renderEvent(evntTypeObstr);
		if (eventsToShowG & evntTypeRERA) renderEvent(evntTypeRERA);
		if (eventsToShowG & evntTypeArousal) renderEvent(evntTypeArousal);
		if (eventsToShowG & evntTypeManMarker) renderEvent(evntTypeManMarker);
		if (eventsToShowG & evntTypeSnoring) renderEvent(evntTypeSnoring);
		if (eventsToShowG & evntTypeAwake) renderEvent(evntTypeAwake);
		if (eventsToShowG & evntTypeExcluded) renderEvent(evntTypeExcluded);
		if (eventsToShowG & evntTypeSwallow) renderEvent(evntTypeSwallow);
		if (eventsToShowG & evntTypeImported) renderEvent(evntTypeImported);

		if (eventsToShowG) mp_target->Flush();

		if (eventsToShowG & evntTypeCursorWnd) {
			if (dTransfGeometryCursorLeft)
				mp_target->DrawGeometry(dTransfGeometryCursorLeft, m_brushCursor, (float) 2.0f);
			if (dTransfGeometryCursorRight)
				mp_target->DrawGeometry(dTransfGeometryCursorRight, m_brushCursor, (float) 2.0f);
			if (dTransfGeometryCursorTimeWindow) {
				m_brushCursorWindow->SetOpacity(cursorWindowOpacity);
				mp_target->FillGeometry(dTransfGeometryCursorTimeWindow, m_brushCursorWindow);
			}
			mp_target->Flush();
		}
		mp_target->PopLayer();	
		pLayer->Release();
	}
	mp_target->PopAxisAlignedClip();
}

void CRGLGraph::CreateDeviceResources(ID2D1HwndRenderTarget *hwndRenderTarget, ID2D1Factory *_factory)
{
	CRGLLayer::CreateDeviceResources(hwndRenderTarget, _factory);

	int numB = bandArray.GetCount();
	for (int i = numB - 1; i >= 0; --i) {
		CRGLBand *bP = bandArray.GetAt(i);
		bP->CreateDeviceResources(mp_target, _factory, &matPlotRect);
	}

	if (m_brush) m_brush.Release();
	m_brush = NULL;
	HRESULT hr = mp_target->CreateSolidColorBrush(D2D1::ColorF((float)dr, (float)dg, (float)db,
		(float)dtransparency), &m_brush);

	if (m_brushCursor) m_brushCursor.Release();
	m_brushCursor = NULL;
	hr = mp_target->CreateSolidColorBrush(D2D1::ColorF((float) .0f, (float) .0f, (float) .0f,
		(float) 1.0f), &m_brushCursor);

	if (m_brushCursorWindow) m_brushCursorWindow.Release();
	m_brushCursorWindow = NULL;
	hr = mp_target->CreateSolidColorBrush(D2D1::ColorF((float) .0f, (float) 1.0f, (float) .0f,
		eventTranspLevel), &m_brushCursorWindow);  // Must be invisible

	if (m_opacityBrush) m_opacityBrush.Release();
	m_opacityBrush = NULL;
	hr = mp_target->CreateSolidColorBrush(D2D1::ColorF((float) .0f, (float) .0f, (float) .0f,
		1.0f), &m_opacityBrush);	

	for (int i = 0; i < bgLevelTypeCount; i++) {
		if (m_brushStockLevel[i]) m_brushStockLevel[i].Release();
		m_brushStockLevel[i] = NULL;
		hr = mp_target->CreateSolidColorBrush(D2D1::ColorF(bgCol[i][0], bgCol[i][1], bgCol[i][2], eventTranspLevel), &m_brushStockLevel[i]);
	}
	for (int i = 0; i < evCountWithoutLevel; i++) {
		if (m_brushStockEventWOLevel[i]) m_brushStockEventWOLevel[i].Release();
		m_brushStockEventWOLevel[i] = NULL;
		hr = mp_target->CreateSolidColorBrush(D2D1::ColorF(bgColWOLevel[i][0], bgColWOLevel[i][1], bgColWOLevel[i][2], 1.0f), &m_brushStockEventWOLevel[i]);
	}

	makeTransformedGeometry();
}

/*
Description: Set sample interval in seconds
_sint: Sample interval in seconds
*/
void CRGLGraph::setSampleInterval(FLOAT _sint)
{
	sint = _sint;
}

void CRGLGraph::setData(vector <FLOAT> *_v, vector <FLOAT> *_tv, vector <BOOL> *_penDown)
{
	dataVector = _v;
	timeVector = _tv;
	penDown = _penDown;

	makeSourceGeometry(true);  // Always reload
	makeTransformedGeometry();
}

void CRGLGraph::setMMarkerStartStopVector(vector<FLOAT>* _v)
{
	mMarkerStartStopVector = _v;
}

void CRGLGraph::setData(vector <FLOAT> *_v, vector <FLOAT> *_tv)
{
	dataVector = _v;
	timeVector = _tv;
	penDown = NULL;

	makeSourceGeometry(true);  // Always reload
	makeTransformedGeometry();
}

/*
Description: Sets the pointer to the event class
*/
void CRGLGraph::setEvents(CEvents *_events)
{
	eventsP = _events;

	if (eventsP) {
		for (int i = 0; i < evCountWithoutLevel; i++)
			makeSourceGeometryForEventsWithoutLevel(i);
		for (int i = 0; i < evCountWithLevel; i++) {
			for (int j = 0; j < bgLevelTypeCount; j++) {
				makeSourceGeometryForEventsWithLevel(i, j);
			}
		}
	}
}

void CRGLGraph::doEventGeometry(int _type, bool _withLevel)
{
	if (-1 == _type) {
		makeCursorGeometries();

		if (eventsP) {
			for (int i = 0; i < evCountWithoutLevel; i++) makeSourceGeometryForEventsWithoutLevel(i);
			for (int i = 0; i < evCountWithLevel; i++) {
				for (int j = 0; j < bgLevelTypeCount; j++) {
					makeSourceGeometryForEventsWithLevel(i, j);
				}
			}
		}
		else {
			if (mMarkerStartStopVector)
				if (mMarkerStartStopVector->size()) makeSourceGeometryForManMarkers();
		}

		if (!mp_factory) return;

		makeTransformedGeometryForCursors();

		for (int i = 0; i < evCountWithoutLevel; i++) {
			if (dTransfGeometryEventWithoutLevel[i]) dTransfGeometryEventWithoutLevel[i]->Release();
			dTransfGeometryEventWithoutLevel[i] = NULL;
			if (dGeometryEventWithoutLevel[i]) {
				HRESULT hr = mp_factory->CreateTransformedGeometry(dGeometryEventWithoutLevel[i],
					matPlotRect, &dTransfGeometryEventWithoutLevel[i]);
				ASSERT(hr == S_OK);
			}
		}
		for (int i = 0; i < evCountWithLevel; i++) {
			for (int j = 0; j < bgLevelTypeCount; j++) {
				if (dTransfGeometryEventWithLevel[i][j]) dTransfGeometryEventWithLevel[i][j]->Release();
				dTransfGeometryEventWithLevel[i][j] = NULL;
				if (dGeometryEventWithLevel[i][j]) {
					HRESULT hr = mp_factory->CreateTransformedGeometry(dGeometryEventWithLevel[i][j],
						matPlotRect, &dTransfGeometryEventWithLevel[i][j]);
					ASSERT(hr == S_OK);
				}
			}
		}
	}
	else {
		if (_withLevel) {
			for (int j = 0; j < bgLevelTypeCount; j++) {
				makeSourceGeometryForEventsWithLevel(_type, j);
				if (dTransfGeometryEventWithLevel[_type][j]) dTransfGeometryEventWithLevel[_type][j]->Release();
				dTransfGeometryEventWithLevel[_type][j] = NULL;
				if (dGeometryEventWithLevel[_type][j]) {
					HRESULT hr = mp_factory->CreateTransformedGeometry(dGeometryEventWithLevel[_type][j],
						matPlotRect, &dTransfGeometryEventWithLevel[_type][j]);
					ASSERT(hr == S_OK);
				}
			}
		}
		else {
			makeSourceGeometryForEventsWithoutLevel(_type); 
			if (dTransfGeometryEventWithoutLevel[_type]) dTransfGeometryEventWithoutLevel[_type]->Release();
			dTransfGeometryEventWithoutLevel[_type] = NULL;
			if (dGeometryEventWithoutLevel[_type]) {
				HRESULT hr = mp_factory->CreateTransformedGeometry(dGeometryEventWithoutLevel[_type],
					matPlotRect, &dTransfGeometryEventWithoutLevel[_type]);
				ASSERT(hr == S_OK);
			}
		}
	}
}

void CRGLGraph::doGeometry(bool _reloadSourceGeometry /* = false*/)
{
	int numB = bandArray.GetCount();
	for (int i = numB - 1; i >= 0; --i) {
		CRGLBand *bP = bandArray.GetAt(i);
		bP->doGeometry(mp_factory, &matPlotRect);
	}
	makeSourceGeometry(_reloadSourceGeometry);
	makeTransformedGeometry();
}

void CRGLGraph::DiscardDeviceResources(void)
{
	if (m_brush) m_brush.Release();
	m_brush = NULL;

	if (dGeometry) dGeometry->Release();
	dGeometry = NULL;

	if (dTransformedGeometry) dTransformedGeometry->Release();
	dTransformedGeometry = NULL;

	for (int i = 0; i < evCountWithoutLevel; i++) {
		if (dGeometryEventWithoutLevel[i]) dGeometryEventWithoutLevel[i]->Release();
		dGeometryEventWithoutLevel[i] = NULL;
		if (dTransfGeometryEventWithoutLevel[i]) dTransfGeometryEventWithoutLevel[i]->Release();
		dTransfGeometryEventWithoutLevel[i] = NULL;
	}
	for (int i = 0; i < evCountWithLevel; i++) {
		for (int j = 0; j < bgLevelTypeCount; j++) {
			if (dGeometryEventWithLevel[i][j]) dGeometryEventWithLevel[i][j]->Release();
			dGeometryEventWithLevel[i][j] = NULL;
			if (dTransfGeometryEventWithLevel[i][j]) dTransfGeometryEventWithLevel[i][j]->Release();
			dTransfGeometryEventWithLevel[i][j] = NULL;
		}
	}
	int numB = bandArray.GetCount();
	for (int i = numB - 1; i >= 0; --i) {
		CRGLBand *bP = bandArray.GetAt(i);
		bP->DiscardDeviceResources();
	}
}

bool CRGLGraph::isOver(CPoint _point)
{
	D2D1_POINT_2F p = D2D1::Point2F((float)_point.x, (float)_point.y);

	if ((p.x < clipRect.left) || (p.x > clipRect.right)) return false;
	if ((p.y < clipRect.top) || (p.y > clipRect.bottom)) return false;

	int ret = FALSE;
	D2D1::Matrix3x2F mat = D2D1::Matrix3x2F::Identity();

	if (dTransformedGeometry) dTransformedGeometry->StrokeContainsPoint(p, 3.0f, NULL, mat, &ret);

	return ret ? true : false;
}

void CRGLGraph::launchParameterDialog(void)
{
	/*	CGraphParDlg dlg;
	dlg.setParameters((float) dr,(float) dg,(float) db,(float) dtransparency,(float) dthickness);
	int dlgRet = (int) dlg.DoModal();
	if (IDOK == dlgRet) {
	float a1,a2,a3,a4,a5;
	dlg.getParameters(&a1,&a2,&a3,&a4,&a5);
	dr = (double) a1;
	dg = (double) a2;
	db = (double) a3;
	dtransparency = (double) a4;
	dthickness = (double) a5;

	if (m_brush) m_brush.Release();
	HRESULT hr = mp_target->CreateSolidColorBrush(D2D1::ColorF((float) dr,(float) dg,(float) db,
	(float) dtransparency),&m_brush);
	render();
	}*/
}

void CRGLGraph::setColour(COLORREF _c)
{
	float r = (float)GetRValue(_c) / 255.0f;
	float g = (float)GetGValue(_c) / 255.0f;
	float b = (float)GetBValue(_c) / 255.0f;
	setColour(r, g, b);
}

void CRGLGraph::setColour(double _r, double _g, double _b)
{
	dr = _r;
	dg = _g;
	db = _b;

	if (!mp_target) return;

	if (m_brush) m_brush.Release();
	m_brush = NULL;
	HRESULT hr = mp_target->CreateSolidColorBrush(D2D1::ColorF((float)dr, (float)dg, (float)db,
		(float)dtransparency), &m_brush);
}

void CRGLGraph::setThickness(float _thick)
{
	dthickness = _thick;
}

FLOAT CRGLGraph::getMin(FLOAT _timeFrom, FLOAT _timeTo)
{
	//*INN*****
	return .0f;
}

FLOAT CRGLGraph::getMax(FLOAT _timeFrom, FLOAT _timeTo)
{
	//*INN****std::minmax();
	return .0f;
}

void CRGLGraph::setTimeSpanLimit(float _lower, float _upper)
{
	upperTimeSpanLimit = _upper;
	lowerTimeSpanLimit = _lower;
}
//
//bool CRGLGraph::trackYShift(CPoint _pt) 
//{
//	D2D1_POINT_2F mousePoint = D2D1::Point2F((FLOAT) _pt.x,(FLOAT) _pt.y);
//	D2D1_POINT_2F pEngUnit = mousePoint * iMatPlotRect;
//	int sampNo = (int) (pEngUnit.x / sint);
//	if (sampNo < 0) return false;
//	if (sampNo >= (int) dataVector->size()) return false;
//
//	float val = dataVector->at(sampNo);
//	float move = pEngUnit.y - val;
//	
//	if (myTile) return myTile->moveVert(move);
//	else return false;
//}

bool CRGLGraph::trackYScale(CPoint _pt, bool _dualPolarity)
{
	D2D1_POINT_2F mousePoint = D2D1::Point2F((FLOAT)_pt.x, (FLOAT)_pt.y);
	D2D1_POINT_2F pEngUnit = mousePoint * iMatPlotRect;
	int sampNo = (int)(pEngUnit.x / sint);
	if (sampNo < 0) return false;
	if (sampNo >= (int)dataVector->size()) return false;

	float val = dataVector->at(sampNo);
	float ratio = pEngUnit.y / val;
	if (ratio < .0f) return false;

	if (myTile) return myTile->scaleVert(ratio, _dualPolarity);
	else return false;
}

void CRGLGraph::setTransparency(float _transp)
{
	dtransparency = _transp;
	
	ASSERT(mp_target);

	if (m_brush) m_brush.Release();
	m_brush = NULL;
	HRESULT hr = mp_target->CreateSolidColorBrush(D2D1::ColorF((float)dr, (float)dg, (float)db,
		(float)dtransparency), &m_brush);
}

void CRGLGraph::setTransparencyPerc(int _transp)
{
	float tr = (float)_transp / 100.0f;
	setTransparency(tr);
}

bool CRGLGraph::getAmplAt(FLOAT _time, FLOAT *_val)
{
	if (!timeVector->size()) return false;
	int last = timeVector->size() - 1;

	if (_time < timeVector->at(0)) return false;
	if (_time > timeVector->at(last)) return false;
	FLOAT out;
	CDataSet::interpolate(dataVector, timeVector, _time, &out);
	*_val = out;
	return true;
}

void CRGLGraph::moveLeftCursor(int _move)
{
	D2D1_POINT_2F screenp1 = leftCursorTop * matPlotRect;
	D2D1_POINT_2F screenp2 = leftCursorBottom * matPlotRect;
	screenp1.x += (FLOAT)_move;
	screenp2.x += (FLOAT)_move;
	leftCursorTop = screenp1 * iMatPlotRect;
	leftCursorBottom = screenp2 * iMatPlotRect;

	makeCursorGeometries();
	makeTransformedGeometryForCursors();
}

void CRGLGraph::setCursorWindow(float _left, float _right)
{
	rightCursorTop.x = _right;
	rightCursorBottom.x = _right;
	leftCursorTop.x = _left;
	leftCursorBottom.x = _left;

	makeCursorGeometries();
	makeTransformedGeometryForCursors();
}

float CRGLGraph::getCursorWindowStart(void)
{
	return leftCursorTop.x;
}

float CRGLGraph::getCursorWindowStop(void)
{
	return rightCursorTop.x;
}

void CRGLGraph::moveRightCursor(int _move)
{
	D2D1_POINT_2F screenp1 = rightCursorTop * matPlotRect;
	D2D1_POINT_2F screenp2 = rightCursorBottom * matPlotRect;
	screenp1.x += (FLOAT)_move;
	screenp2.x += (FLOAT)_move;
	rightCursorTop = screenp1 * iMatPlotRect;
	rightCursorBottom = screenp2 * iMatPlotRect;

	makeCursorGeometries();
	makeTransformedGeometryForCursors();
}

void CRGLGraph::changeAnEvent(CEvnt *_evnt, int _command)
{
	if (!eventsP) return;

	if (_evnt) {
		eventsP->changeEvent(_evnt, _command);
		eventsP->actOnEventChange();
	}
}

/*
Description: Detects the event as well as what edge and moves the edge
*/
void CRGLGraph::moveEventEdge(CEvnt *_evnt, FLOAT _moveSecs, int _object)
{
	if (!eventsP) return;

	if (_evnt) {
		eventsP->moveEventEdge(_evnt, _moveSecs, _object);
		eventsP->actOnEventLengthChange();
	}
}


/*
Description: Returns the position of the right cursor in screen coordinates
*/
int CRGLGraph::getRightCursorX(void)
{
	D2D1_POINT_2F a = rightCursorTop * matPlotRect;
	return (int)a.x;
}


/*
Description: Returns the position of the time in screen coordinates
*/
int CRGLGraph::getScreenXfromTime(FLOAT _time)
{
	D2D1_POINT_2F in;
	in.x = _time;
	in.y = .0f;
	D2D1_POINT_2F a = in * matPlotRect;
	return (int)a.x;
}

/*
Description: Returns the position of the left cursor in screen coordinates
*/
int CRGLGraph::getLeftCursorX(void)
{
	D2D1_POINT_2F a = leftCursorTop * matPlotRect;
	return (int)a.x;
}

bool CRGLGraph::isOverEvent(D2D1_POINT_2F _p, UINT _evntType, int *_what, int *_where)
{
	int theenum = -1;
	bool wLevel = false;
	UINT overType = 0;
	switch (_evntType) {
	case evntTypeNone:
		break;
	case evntTypeExcluded:
		theenum = OVER_EVNT_EXCLUDED;
		wLevel = false;
		break;
	case evntTypeMixed:
		theenum = evMix;
		overType = OVER_EVNT_MIXED;
		wLevel = true;
		break;
	case evntTypeCentral:
		theenum = evCentral;
		overType = OVER_EVNT_CENTRAL;
		break;
	case evntTypeObstr:
		theenum = evObstr;
		overType = OVER_EVNT_OBSTRUCTIVE;
		wLevel = true;
		break;
	case evntTypeHypoCentral:
		theenum = evCentralHypo;
		overType = OVER_EVNT_CENTRALHYPO;
		break;
	case evntTypeHypoObstr:
		theenum = evObstrHypo;
		overType = OVER_EVNT_OBSTRUCTIVEHYPO;
		wLevel = true;
		break;
	case evntTypeRERA:
		theenum = evRERA;
		overType = OVER_EVNT_RERA;
		wLevel = true;
		break;
	case evntTypeArousal:
		theenum = evArousal;
		overType = OVER_EVNT_AROUSAL;
		break;
	case evntTypeSpO2Drop:
		theenum = evSpO2;
		overType = OVER_EVNT_SPO2_DROP;
		break;
	case evntTypeManMarker:
		theenum = evManual;
		overType = OVER_EVNT_MAN_MARKER;
		break;
	case evntTypeSnoring:
		theenum = evSnoringAllLevels;
		overType = OVER_EVNT_SNORING;
		wLevel = true;
		break;
	case evntTypeAwake:
		theenum = evAwake;
		overType = OVER_EVNT_AWAKE;
		break;
	default:
		break;
	}
	return overType != 0 ? true : false;

	int ret = false;
	D2D1::Matrix3x2F mat = D2D1::Matrix3x2F::Identity();
	if (wLevel) {
		if (dTransfGeometryEventWithLevel[theenum][bgLevelTypeUpper])
			dTransfGeometryEventWithLevel[theenum][bgLevelTypeUpper]->StrokeContainsPoint(_p, 3.0f, NULL, mat, &ret);
		if (ret) {
			*_what |= overType;
			*_what |= OVER_EVNT;
			*_what |= OVER_UPPER;
			*_where |= OVER_EDGE;
			return true;
		}
		if (dTransfGeometryEventWithLevel[theenum][bgLevelTypeUpper])
			dTransfGeometryEventWithLevel[theenum][bgLevelTypeUpper]->FillContainsPoint(_p, mat, &ret);
		if (ret) {
			*_what |= overType;
			*_what |= OVER_EVNT;
			*_what |= OVER_UPPER;
			//*_where |= OVER_AREA;
			return true;
		}
		if (dTransfGeometryEventWithLevel[theenum][bgLevelTypeLower])
			dTransfGeometryEventWithLevel[theenum][bgLevelTypeLower]->StrokeContainsPoint(_p, 3.0f, NULL, mat, &ret);
		if (ret) {
			*_what |= overType;
			*_what |= OVER_EVNT;
			*_what |= OVER_LOWER;
			*_where |= OVER_EDGE;
			return true;
		}
		if (dTransfGeometryEventWithLevel[theenum][bgLevelTypeLower])
			dTransfGeometryEventWithLevel[theenum][bgLevelTypeLower]->FillContainsPoint(_p, mat, &ret);
		if (ret) {
			*_what |= overType;
			*_what |= OVER_EVNT;
			*_what |= OVER_LOWER;
			//*_where |= OVER_AREA;
			return true;
		}
		if (dTransfGeometryEventWithLevel[theenum][bgLevelTypeMulti])
			dTransfGeometryEventWithLevel[theenum][bgLevelTypeMulti]->StrokeContainsPoint(_p, 3.0f, NULL, mat, &ret);
		if (ret) {
			*_what |= overType;
			*_what |= OVER_EVNT;
			*_what |= OVER_MULTI;
			*_where |= OVER_EDGE;
			return true;
		}
		if (dTransfGeometryEventWithLevel[theenum][bgLevelTypeMulti])
			dTransfGeometryEventWithLevel[theenum][bgLevelTypeMulti]->FillContainsPoint(_p, mat, &ret);
		if (ret) {
			*_what |= overType;
			*_what |= OVER_EVNT;
			*_what |= OVER_MULTI;
			//*_where |= OVER_AREA;
			return true;
		}
		if (dTransfGeometryEventWithLevel[theenum][bgLevelTypeUndef])
			dTransfGeometryEventWithLevel[theenum][bgLevelTypeUndef]->StrokeContainsPoint(_p, 3.0f, NULL, mat, &ret);
		if (ret) {
			*_what |= overType;
			*_what |= OVER_EVNT;
			*_what |= OVER_UNDEFINED_LEVEL;
			*_where |= OVER_EDGE;
			return true;
		}
		if (dTransfGeometryEventWithLevel[theenum][bgLevelTypeUndef])
			dTransfGeometryEventWithLevel[theenum][bgLevelTypeUndef]->FillContainsPoint(_p, mat, &ret);
		if (ret) {
			*_what |= overType;
			*_what |= OVER_EVNT;
			*_what |= OVER_UNDEFINED_LEVEL;
			//*_where |= OVER_AREA;
			return true;
		}
	}
	else {
		if (dTransfGeometryEventWithoutLevel[theenum])
			dTransfGeometryEventWithoutLevel[theenum]->StrokeContainsPoint(_p, 3.0f, NULL, mat, &ret);
		if (ret) {
			*_what |= overType;
			*_what |= OVER_EVNT;
			*_where |= OVER_EDGE;
			return true;
		}
		if (dTransfGeometryEventWithoutLevel[theenum])
			dTransfGeometryEventWithoutLevel[theenum]->FillContainsPoint(_p, mat, &ret);
		if (ret) {
			*_what |= overType;
			*_what |= OVER_EVNT;
			//*_where |= OVER_AREA;
			return true;
		}
	}

	return false;
}


bool CRGLGraph::getIsOver(CPoint _point, int *_what, int *_where)
{
	FLOAT strokeWidth = 5.0f;

	D2D1_POINT_2F p = D2D1::Point2F((float)_point.x, (float)_point.y);

	if ((p.x < clipRect.left) || (p.x > clipRect.right)) return false;
	if ((p.y < clipRect.top) || (p.y > clipRect.bottom)) return false;

	int ret = false;
	D2D1::Matrix3x2F mat = D2D1::Matrix3x2F::Identity();
	*_what = 0;
	*_where = 0;
	if (showCursorWnd) {
		if (dTransfGeometryCursorLeft) dTransfGeometryCursorLeft->StrokeContainsPoint(p, strokeWidth, NULL, mat, &ret);
		if (ret) {
			*_what |= OVER_CURSOR;
			*_where |= OVER_LEFT_EDGE;
			*_where |= OVER_EDGE;
			return true;
		}
		if (dTransfGeometryCursorRight) dTransfGeometryCursorRight->StrokeContainsPoint(p, strokeWidth, NULL, mat, &ret);
		if (ret) {
			*_what |= OVER_CURSOR;
			*_where |= OVER_RIGHT_EDGE;
			*_where |= OVER_EDGE;
			return true;
		}
		if (dTransfGeometryCursorTimeWindow) dTransfGeometryCursorTimeWindow->FillContainsPoint(p, mat, &ret);
		if (ret) {
			*_what |= OVER_CURSOR;
			*_where |= OVER_EVNT;
			return true;
		}
	}

	//---Events - edges		
	if ((eventsToShowG & evntTypeSpO2Drop) && (isOverEvent(p, evntTypeSpO2Drop, _what, _where))) return true;
	if ((eventsToShowG & evntTypeHypoCentral) && (isOverEvent(p, evntTypeHypoCentral, _what, _where)))
		return true;
	if ((eventsToShowG & evntTypeHypoObstr) && (isOverEvent(p, evntTypeHypoObstr, _what, _where)))
		return true;
	if ((eventsToShowG & evntTypeCentral) && (isOverEvent(p, evntTypeCentral, _what, _where)))
		return true;
	if ((eventsToShowG & evntTypeMixed) && (isOverEvent(p, evntTypeMixed, _what, _where))) return true;
	if ((eventsToShowG & evntTypeObstr) && (isOverEvent(p, evntTypeObstr, _what, _where))) return true;
	if ((eventsToShowG & evntTypeRERA) && (isOverEvent(p, evntTypeRERA, _what, _where))) return true;
	if ((eventsToShowG & evntTypeArousal) && (isOverEvent(p, evntTypeArousal, _what, _where))) return true;
	if ((eventsToShowG & evntTypeManMarker) && (isOverEvent(p, evntTypeManMarker, _what, _where))) return true;
	if ((eventsToShowG & evntTypeSnoring) && (isOverEvent(p, evntTypeSnoring, _what, _where))) return true;
	if ((eventsToShowG & evntTypeAwake) && (isOverEvent(p, evntTypeAwake, _what, _where))) return true;

	return false;
}


CString CRGLGraph::getFormattedNumber(FLOAT _v)
{
	CString s;
	FLOAT vabs = fabs(_v);
	if (vabs < .01f) {
		s.Format(_T("%.4f"), _v);
		return s;
	}
	if (vabs < .1f) {
		s.Format(_T("%.3f"), _v);
		return s;
	}
	if (vabs < 1.0f) {
		s.Format(_T("%.2f"), _v);
		return s;
	}
	if (vabs < 10.0f) {
		s.Format(_T("%.1f"), _v);
		return s;
	}
	s.Format(_T("%.0f"), _v);
	return s;
}


CString CRGLGraph::getAmplAt(FLOAT _time)
{
	if (!timeVector->size()) return _T("");
	if (!dataVector->size()) return _T("");
	int last = timeVector->size() - 1;

	if (_time < timeVector->at(0)) return _T("");
	if (_time > timeVector->at(last)) return _T("");
	FLOAT out = .0f;
	CDataSet::interpolate(dataVector, timeVector, _time, &out);

	return getFormattedNumber(out);
}


/*
Description: From a float time value, generates a string in this format ss.d
*/
CString CRGLGraph::getsecs(FLOAT _t)
{
	bool neg = _t < .0f ? true : false;
	FLOAT tm = neg ? -_t : _t;
	int s = (int)tm;
	float ds = (tm - (float)((int)tm));
	ds *= 10.0f;
	CString st;
	if (neg) st.Format(_T("- %d.%01.0f"), s, ds);
	else st.Format(_T("%d.%01.0f"), s, ds);
	return st;
}

/*
Description: From a float time value, generates a string in this format hh:mm:ss.d
*/
CString CRGLGraph::gethmsDec(FLOAT _t)
{
	bool neg = _t < .0f ? true : false;
	FLOAT tm = neg ? -_t : _t;
	int h = (int)(tm / 3600.0f);
	float m = tm - (float)h * 3600;
	m = (float)((int)(m / 60.0f));
	int s = (int)(tm - (float)h * 3600 - m * 60.0f);
	float ds = (tm - (float)((int)tm));
	ds *= 100.0f;
	CString st;
	if (neg) st.Format(_T("- %02d:%02.0f:%02d.%02.0f"), h, m, s, ds);
	else st.Format(_T("%02d:%02.0f:%02d.%02.0f"), h, m, s, ds);
	return st;
}
/*
Description: From a float time value, generates a string in this format hh:mm:ss
*/
CString CRGLGraph::gethms(FLOAT _t)
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

FLOAT CRGLGraph::getTimeValAt(LONG _x, LONG _y)
{
	return getTheTimeAtPixel(_x);
}

D2D1_POINT_2F CRGLGraph::getTimeAndAmplAt(LONG _x, LONG _y)
{
	D2D1_POINT_2F in;
	in.x = (FLOAT)_x;
	in.y = (FLOAT)_y;
	D2D1_POINT_2F out;
	out.x = getTheTimeAtPixel(_x);
	getAmplAt(out.x, &out.y);
	return out;
}

CString CRGLGraph::getStringTimeAt(LONG _x, LONG _y)
{
	return CRGLGraph::gethms(getTheTimeAtPixel(_x));
}

CString CRGLGraph::getTimeCursorLeftInfo(void)
{
	return gethms(leftCursorTop.x);
}

CString CRGLGraph::getTimeCursorRightInfo(void)
{
	return gethms(rightCursorTop.x);
}

CEvnt *CRGLGraph::findEvent(int _what, FLOAT _atTime, int *_closestEdge)
{
	if (!eventsP) return NULL;
	return eventsP->findEvent(_what, _atTime, _closestEdge);
}

CString CRGLGraph::getTimeCursorLengthInfo(void)
{
	return gethms(rightCursorTop.x - leftCursorTop.x);
}

/*
Description:
Computes the time from pixel x-coordinate. iMatPlotRect cannot be used as the inversion of matrices
does not perform well on 32-bit machines.
_x: Pixel x-coordinate
Return: The time in seconds
*/
float CRGLGraph::getTheTimeAtPixel(LONG _x)
{
	CRect crect;

	if (!parentWnd) return .0f;

	parentWnd->GetClientRect(&crect);
	int width = crect.right - crect.left;
	D2D1_RECT_F prScaled = myTile->getPlotRectScaled();
	D2D1_RECT_F pr = myTile->getPlotRect();
	float leftPix = (float)width * pr.left / 100.0f;
	float rightPix = (float)width * pr.right / 100.0f;
	LONG xCoordPix = _x - (LONG)leftPix;
	float xCoord = prScaled.left + (prScaled.right - prScaled.left) * xCoordPix / (rightPix - leftPix);

	return xCoord;
}


CString CRGLGraph::getStringAmplAt(LONG _x, LONG _y, CString _unit)
{
	CString s2;
	if (!parentWnd) return _T("No parent window in CRGLGraph");
	float time = getTheTimeAtPixel(_x);
	CString a2 = getAmplAt(time);
	if (a2.IsEmpty()) return _T("");
	else s2.Format(_T("%s %s"), (LPCTSTR) a2, (LPCTSTR) _unit);

	return s2;
}

CString CRGLGraph::getAmplAt(LONG _x, LONG _y)
{
	float time = getTheTimeAtPixel(_x);
	return getAmplAt(time);
}


CString CRGLGraph::getStringTimeAndAmplAt(LONG _x, LONG _y, CString _unit, bool _lineBreak)
{
	CString s2;
	if (!parentWnd) return _T("No parent window in CRGLGraph");
	float time = getTheTimeAtPixel(_x);
	CString a2 = getAmplAt(time);
	if (a2.IsEmpty())
		s2.Format(_T("%s"), (LPCTSTR) gethms(time));
	else {
		if (_lineBreak) s2.Format(_T("%s\n%s %s"), (LPCTSTR) gethms(time), (LPCTSTR) a2, (LPCTSTR) _unit);
		else  s2.Format(_T("%s, %s %s"), (LPCTSTR) gethms(time), (LPCTSTR) a2, (LPCTSTR) _unit);
	}

	return s2;
}


void CRGLGraph::setEventsToShow(UINT _evToShow, bool _onlyShowEvents /* = false*/)
{
	eventsAlreadyShown = eventsToShowG;
	eventsToShowG = _evToShow;
	
	showCursorWnd = eventsToShowG & evntTypeCursorWnd ? true : false;
}

void CRGLGraph::resize(void)
{
	CRGLLayer::resize();
	if (!mp_factory) return;
	if (!dGeometry) return;

	makeTransformedGeometry();
}


void CRGLGraph::processFullRect(void)
{
	CRGLLayer::processFullRect();
}

void CRGLGraph::processPlotRect(void)
{
	CRGLLayer::processPlotRect();
	if (myTile) {
		int changeFlag = myTile->getChangeFlag();
		if (changeFlag & RGLTileChangeXScaled) makeSourceGeometry(); // Refill source Geometry
	}
	makeTransformedGeometry();
}

void CRGLGraph::clearData(void)
{
	if (dGeometry) dGeometry->Release();
	dGeometry = NULL;
	if (dTransformedGeometry) dTransformedGeometry->Release();
	dTransformedGeometry = NULL;
}

void CRGLGraph::makeTransformedGeometryForCursors(void)
{
	if (dGeometryCursorLeft) {
		if (dTransfGeometryCursorLeft) dTransfGeometryCursorLeft->Release();
		dTransfGeometryCursorLeft = NULL;
		HRESULT hr = mp_factory->CreateTransformedGeometry(dGeometryCursorLeft, matPlotRect, &dTransfGeometryCursorLeft);
	}

	if (dGeometryCursorRight) {
		if (dTransfGeometryCursorRight) dTransfGeometryCursorRight->Release();
		dTransfGeometryCursorRight = NULL;
		HRESULT hr = mp_factory->CreateTransformedGeometry(dGeometryCursorRight, matPlotRect, &dTransfGeometryCursorRight);
	}

	if (dGeometryCursorTimeWindow) {
		if (dTransfGeometryCursorTimeWindow) dTransfGeometryCursorTimeWindow->Release();
		dTransfGeometryCursorTimeWindow = NULL;
		HRESULT hr = mp_factory->CreateTransformedGeometry(dGeometryCursorTimeWindow, matPlotRect, &dTransfGeometryCursorTimeWindow);
	}

}

void CRGLGraph::makeTransformedGeometry(void)
{
	if (!mp_factory) return;
	makeTransformedGeometryForCursors();

	int numB = bandArray.GetCount();
	for (int i = numB - 1; i >= 0; --i) {
		CRGLBand *bP = bandArray.GetAt(i);
		bP->makeTransformedGeometry(mp_factory, &matPlotRect);
	}

	if (dGeometry) {
		if (dTransformedGeometry) dTransformedGeometry->Release();
		dTransformedGeometry = NULL;
		HRESULT hr = mp_factory->CreateTransformedGeometry(dGeometry, matPlotRect, &dTransformedGeometry);
	}

	for (int i = 0; i < evCountWithoutLevel; i++) {
		if (dGeometryEventWithoutLevel[i]) {
			if (dTransfGeometryEventWithoutLevel[i]) dTransfGeometryEventWithoutLevel[i]->Release();
			dTransfGeometryEventWithoutLevel[i] = NULL;
			HRESULT hr = mp_factory->CreateTransformedGeometry(dGeometryEventWithoutLevel[i],
				matPlotRect, &dTransfGeometryEventWithoutLevel[i]);
		}
	}
	for (int i = 0; i < evCountWithLevel; i++) {
		for (int j = 0; j < bgLevelTypeCount; j++) {
          	if (dGeometryEventWithLevel[i][j]) {
				if (dTransfGeometryEventWithLevel[i][j]) dTransfGeometryEventWithLevel[i][j]->Release();
				dTransfGeometryEventWithLevel[i][j] = NULL;
				HRESULT hr = mp_factory->CreateTransformedGeometry(dGeometryEventWithLevel[i][j],
					matPlotRect, &dTransfGeometryEventWithLevel[i][j]);
			}
		}
	}
}

void CRGLGraph::makeCursorGeometries(void)
{
	if (!myTile) return;

	if (!mp_factory) return;

	if (dGeometryCursorLeft) dGeometryCursorLeft->Release();
	dGeometryCursorLeft = NULL;

	if (dGeometryCursorRight) dGeometryCursorRight->Release();
	dGeometryCursorRight = NULL;

	if (dGeometryCursorTimeWindow) dGeometryCursorTimeWindow->Release();
	dGeometryCursorTimeWindow = NULL;

	ID2D1GeometrySink *sinkLeft = NULL;
	ID2D1GeometrySink *sinkRight = NULL;
	ID2D1GeometrySink *sinkTimeWindow = NULL;

	D2D1_RECT_F plotRectScaled = myTile->getPlotRectScaled();
	leftCursorBottom.y = plotRectScaled.bottom;
	leftCursorTop.y = plotRectScaled.top;
	rightCursorBottom.y = plotRectScaled.bottom;
	rightCursorTop.y = plotRectScaled.top;

	mp_factory->CreatePathGeometry(&dGeometryCursorLeft);
	if (dGeometryCursorLeft) {
		dGeometryCursorLeft->Open(&sinkLeft);
		if (sinkLeft) {
			sinkLeft->BeginFigure(leftCursorTop, D2D1_FIGURE_BEGIN_HOLLOW);
			sinkLeft->AddLine(leftCursorBottom);
			sinkLeft->EndFigure(D2D1_FIGURE_END_OPEN);
			sinkLeft->Close();
			sinkLeft->Release();
			sinkLeft = NULL;
		}
	}
	mp_factory->CreatePathGeometry(&dGeometryCursorRight);
	if (dGeometryCursorRight) {
		dGeometryCursorRight->Open(&sinkRight);
		if (sinkRight) {
			sinkRight->BeginFigure(rightCursorTop, D2D1_FIGURE_BEGIN_HOLLOW);
			sinkRight->AddLine(rightCursorBottom);
			sinkRight->EndFigure(D2D1_FIGURE_END_OPEN);
			sinkRight->Close();
			sinkRight->Release();
			sinkRight = NULL;
		}
	}
	mp_factory->CreatePathGeometry(&dGeometryCursorTimeWindow);
	if (dGeometryCursorTimeWindow) {
		dGeometryCursorTimeWindow->Open(&sinkTimeWindow);
		if (sinkTimeWindow) {
			sinkTimeWindow->BeginFigure(leftCursorTop, D2D1_FIGURE_BEGIN_FILLED);
			sinkTimeWindow->AddLine(rightCursorTop);
			sinkTimeWindow->AddLine(rightCursorBottom);
			sinkTimeWindow->AddLine(leftCursorBottom);
			sinkTimeWindow->AddLine(leftCursorTop);
			sinkTimeWindow->EndFigure(D2D1_FIGURE_END_OPEN);
			sinkTimeWindow->Close();
			sinkTimeWindow->Release();
			sinkTimeWindow = NULL;
		}
	}
}

void CRGLGraph::makeSourceGeometry(bool _reload)
{
	makeCursorGeometries();
	if (eventsP) {
		for (int i = 0; i < evCountWithoutLevel; i++) makeSourceGeometryForEventsWithoutLevel(i);
		for (int i = 0; i < evCountWithLevel; i++) {
			for (int j = 0; j < bgLevelTypeCount; j++) {
				makeSourceGeometryForEventsWithLevel(i, j);
			}
		}
	}
	else {
		if (mMarkerStartStopVector)
			if (mMarkerStartStopVector->size()) makeSourceGeometryForManMarkers();
	}

	int numB = bandArray.GetCount();
	for (int i = numB - 1; i >= 0; --i) {
		CRGLBand *bP = bandArray.GetAt(i);
		bP->makeSourceGeometry(mp_factory);
	}

	if (!myTile) return;
	if (!dataVector) return;

	D2D1_RECT_F plotRectScaled = myTile->getPlotRectScaled();

	//---Check if we have to reload the source geometry
	if (!_reload) {
		if ((plotRectScaled.left > oldSourceGeometryLeft) && (plotRectScaled.right < oldSourceGeometryRight)) {
			return;
		}
	}

	int num = (int)dataVector->size();
	if (0 == num) return;
	if (!mp_factory) return;

	FLOAT dist = plotRectScaled.right - plotRectScaled.left;

	if (sint != .0f) {

		if (dGeometry) dGeometry->Release();
		dGeometry = NULL;

		ID2D1GeometrySink *sink = NULL;

		mp_factory->CreatePathGeometry(&dGeometry);
		FLOAT requestedLength = plotRectScaled.right - plotRectScaled.left;
		FLOAT midPoint = (plotRectScaled.right + plotRectScaled.left) / 2.0f;

		oldSourceGeometryLeft = midPoint - (SCREEN_LENGTH_FACTOR * requestedLength) / 2.0f;
		oldSourceGeometryRight = midPoint + (SCREEN_LENGTH_FACTOR * requestedLength) / 2.0f;

		int start = (int)(oldSourceGeometryLeft / sint);
		int stop = (int)(oldSourceGeometryRight / sint);
		stop = stop > num ? num : stop;
		start = start > stop - 1 ? stop - 1 : start;
		start = start < 0 ? 0 : start;
		oldSourceGeometryLeft = start * sint;
		oldSourceGeometryRight = stop * sint;
		if (dGeometry) {
			dGeometry->Open(&sink);
			if (sink) {

				D2D1_POINT_2F d2d1p;
				d2d1p.x = start * sint; //== 0 ? .0f : plotRectScaled.left;
				d2d1p.y = dataVector->at(start);
				sink->BeginFigure(d2d1p, D2D1_FIGURE_BEGIN_HOLLOW);
				for (int i = start + 1; i < stop; i++) {
					d2d1p.x += sint;
					d2d1p.y = dataVector->at(i);
					sink->AddLine(d2d1p);
				}
				sink->EndFigure(D2D1_FIGURE_END_OPEN);
				sink->Close();
				sink->Release();
				sink = NULL;
			}
		}
	}
	else {
		ASSERT(timeVector);
		if (timeVector->size() > 0) {
			if (dGeometry) dGeometry->Release();
			dGeometry = NULL;

			ID2D1GeometrySink *sink = NULL;

			mp_factory->CreatePathGeometry(&dGeometry);
			FLOAT requestedLength = plotRectScaled.right - plotRectScaled.left;
			FLOAT midPoint = (plotRectScaled.right + plotRectScaled.left) / 2.0f;

			oldSourceGeometryLeft = midPoint - (SCREEN_LENGTH_FACTOR * requestedLength) / 2.0f;
			oldSourceGeometryRight = midPoint + (SCREEN_LENGTH_FACTOR * requestedLength) / 2.0f;

			FLOAT startTime = oldSourceGeometryLeft;
			FLOAT stopTime = oldSourceGeometryRight;

			if ((dGeometry) && (NULL == penDown)) {
				dGeometry->Open(&sink);
				if (sink) {

					D2D1_POINT_2F d2d1p;

					unsigned int distance1 = distance(timeVector->begin(), lower_bound(timeVector->begin(), timeVector->end(), startTime));
					unsigned int distance2 = distance(timeVector->begin(), lower_bound(timeVector->begin(), timeVector->end(), stopTime));
					if (distance1 < dataVector->size()) {

						vector <FLOAT>::iterator ti = timeVector->begin();
						ti += distance1;
						vector <FLOAT>::iterator di = dataVector->begin();
						di += distance1;

						d2d1p.x = *ti;
						d2d1p.y = *di;

						sink->BeginFigure(d2d1p, D2D1_FIGURE_BEGIN_HOLLOW);
	
						unsigned int limit = distance2 < dataVector->size() ? distance2 : dataVector->size();
						vector <FLOAT>::iterator dilim = dataVector->begin();
						dilim += limit;
						for (; di < dilim; ++di, ++ti) {
							d2d1p.x = *ti;
							d2d1p.y = *di;
							sink->AddLine(d2d1p);
						}
						
						sink->EndFigure(D2D1_FIGURE_END_OPEN);
					}
					sink->Close();
					sink->Release();
					sink = NULL;
				}
			}
			if ((dGeometry) && (NULL != penDown)) {

				ASSERT(penDown->size() == timeVector->size());

				dGeometry->Open(&sink);
				if (sink) {

					D2D1_POINT_2F d2d1p;
					BOOL pDown, wasDown;
					unsigned int distance1 = distance(timeVector->begin(), lower_bound(timeVector->begin(), timeVector->end(), startTime));
					unsigned int distance2 = distance(timeVector->begin(), lower_bound(timeVector->begin(), timeVector->end(), stopTime));
					if (distance1 < dataVector->size()) {

						vector <FLOAT>::iterator ti = timeVector->begin();
						ti += distance1;
						vector <FLOAT>::iterator di = dataVector->begin();
						di += distance1;
						vector <BOOL>::iterator pdi = penDown->begin();
						pdi += distance1;

						d2d1p.x = *ti;
						d2d1p.y = *di;
						pDown = *pdi;

						if (pDown) {
							sink->BeginFigure(d2d1p, D2D1_FIGURE_BEGIN_HOLLOW);
						}
						wasDown = pDown;
						unsigned int limit = distance2 < dataVector->size() ? distance2 : dataVector->size();
						vector <FLOAT>::iterator dilim = dataVector->begin();
						dilim += limit;
						for (; di < dilim; ++di,++ti,++pdi) {
							d2d1p.x = *ti;
							d2d1p.y = *di;
							pDown = *pdi;
							if (wasDown) {
								sink->AddLine(d2d1p);
							}
							if (pDown && !wasDown) {
								sink->BeginFigure(d2d1p, D2D1_FIGURE_BEGIN_HOLLOW);
							}
							else if (!pDown && wasDown) {
								sink->EndFigure(D2D1_FIGURE_END_OPEN);
							}
							wasDown = pDown;
						}
						if (wasDown) {
							sink->EndFigure(D2D1_FIGURE_END_OPEN);
						}
					}
					sink->Close();
					sink->Release();
					sink = NULL;
				}
			}
		}
	}
}

void CRGLGraph::sinkStartsAndStops(ID2D1GeometrySink *_sink, vector <float> *_startsStopsP)
{
	D2D1_RECT_F plotRectScaled = myTile->getPlotRectScaled();

	D2D1_POINT_2F d2d1p;
	vector <float>::iterator xLoc;
	xLoc = _startsStopsP->begin();

	d2d1p.x = _startsStopsP->at(0);
	d2d1p.y = plotRectScaled.bottom;
	_sink->BeginFigure(d2d1p, D2D1_FIGURE_BEGIN_FILLED);
	for (xLoc = _startsStopsP->begin(); xLoc < _startsStopsP->end(); xLoc++) {
		d2d1p.x = *xLoc;
		d2d1p.y = plotRectScaled.bottom;
		_sink->AddLine(d2d1p);

		d2d1p.x = *xLoc;
		d2d1p.y = plotRectScaled.top;
		_sink->AddLine(d2d1p);

		xLoc++;

		d2d1p.x = *xLoc;
		d2d1p.y = plotRectScaled.top;
		_sink->AddLine(d2d1p);

		d2d1p.x = *xLoc;
		d2d1p.y = plotRectScaled.bottom;
		_sink->AddLine(d2d1p);
	}
	d2d1p.x = _startsStopsP->at(0);
	d2d1p.y = plotRectScaled.bottom;
	_sink->AddLine(d2d1p);

	_sink->EndFigure(D2D1_FIGURE_END_CLOSED);
}

void CRGLGraph::makeSourceGeometryForEventsWithLevel(UINT _type, UINT _level)
{
	if (!eventsP) return;

	vector <float> *startsStopsP = eventsP->getBeginsEnds(_type, _level);

	if (dGeometryEventWithLevel[_type][_level]) dGeometryEventWithLevel[_type][_level]->Release();
	dGeometryEventWithLevel[_type][_level] = NULL;

	if (!startsStopsP->size()) return;

	ID2D1GeometrySink *sink = NULL;

	mp_factory->CreatePathGeometry(&dGeometryEventWithLevel[_type][_level]);

	if (dGeometryEventWithLevel[_type][_level]) {
		dGeometryEventWithLevel[_type][_level]->Open(&sink);
		if (sink) {
			sinkStartsAndStops(sink, startsStopsP);
			sink->Close();
			sink->Release();
			sink = NULL;
		}
	}
}

void CRGLGraph::makeSourceGeometryForManMarkers(void)
{
	if (!mMarkerStartStopVector) return;

	if (dGeometryEventWithoutLevel[evManual]) dGeometryEventWithoutLevel[evManual]->Release();
	dGeometryEventWithoutLevel[evManual] = NULL;

	if (!mMarkerStartStopVector->size()) return;

	ID2D1GeometrySink *sink = NULL;

	mp_factory->CreatePathGeometry(&dGeometryEventWithoutLevel[evManual]);

	if (dGeometryEventWithoutLevel[evManual]) {
		dGeometryEventWithoutLevel[evManual]->Open(&sink);
		if (sink) {
			sinkStartsAndStops(sink, mMarkerStartStopVector);
			sink->Close();
			sink->Release();
			sink = NULL;
		}
	}
}

void CRGLGraph::makeSourceGeometryForEventsWithoutLevel(UINT _type)
{
	if (!eventsP) return;

	vector <float> *startsStopsP = eventsP->getBeginsEnds(_type);

	if (dGeometryEventWithoutLevel[_type]) dGeometryEventWithoutLevel[_type]->Release();
	dGeometryEventWithoutLevel[_type] = NULL;

	if (!startsStopsP->size()) return;


	ID2D1GeometrySink *sink = NULL;

	mp_factory->CreatePathGeometry(&dGeometryEventWithoutLevel[_type]);

	if (dGeometryEventWithoutLevel[_type]) {
		dGeometryEventWithoutLevel[_type]->Open(&sink);
		if (sink) {
			sinkStartsAndStops(sink, startsStopsP);
			sink->Close();
			sink->Release();
			sink = NULL;
		}
	}
}

/////////////////////////////////////////////////////////////


CRGLBackground::CRGLBackground(CString _regSection /* = _T("Background") */) :
	regSection(_regSection)
{
	br = bg = bb = 1.0f;
	type = RGLLayerTypeBackground;

	if (!regSection.IsEmpty()) {
		CXTRegistryManager *reg = new CXTRegistryManager;

		//---Test registry
		double dtest;
		int OKt = reg->GetProfileDouble(regSection, _T("Red"), &dtest);
		if (!OKt) {
			delete reg;
			reg = new CXTRegistryManager(HKEY_LOCAL_MACHINE);
		}

		reg->GetProfileDouble(regSection, _T("Red"), &br);
		reg->GetProfileDouble(regSection, _T("Green"), &bg);
		reg->GetProfileDouble(regSection, _T("Blue"), &bb);
		delete reg;
	}
}

CRGLBackground::~CRGLBackground()
{
	if (!regSection.IsEmpty()) {
		CXTRegistryManager *reg = new CXTRegistryManager;
		reg->WriteProfileDouble(regSection, _T("Red"), &br);
		reg->WriteProfileDouble(regSection, _T("Green"), &bg);
		reg->WriteProfileDouble(regSection, _T("Blue"), &bb);
		delete reg;
	}
}

void CRGLBackground::setColour(COLORREF _c)
{
	double r = (double)GetRValue(_c) / 255.0f;
	double g = (double)GetGValue(_c) / 255.0f;
	double b = (double)GetBValue(_c) / 255.0f;
	setColour(r, g, b);
}

void CRGLBackground::setColour(double _r, double _g, double _b)
{
	br = _r;
	bg = _g;
	bb = _b;
}

CString CRGLBackground::getName(void)
{
	CString s = regSection;
	s.MakeReverse();
	s = s.SpanExcluding(_T("\\"));
	s.MakeReverse();
	return s;
}

void CRGLBackground::render(void)
{
	if (!show) return;
	ASSERT(mp_target);

	//---Added in 4.2
	HRESULT hres = mp_target->Flush();
	if (hres != S_OK) return;

	//CRGLLayer::render();  // Removed in 3.10 and added try {} catch {}
	try {
		mp_target->Clear(D2D1::ColorF((float)br, (float)bg, (float)bb, 1.0f));
	}
	catch (CException* e) {
		TCHAR   szCause[255];
		e->GetErrorMessage(szCause, 255);
		CString s;
		s.Format(_T("Exception thrown in CRGLBackground::render()"));
		s += _T(",\t");
		CString causeS(szCause);
		s += _T("Cause : ");
		s += causeS;
		if (dmpLogP) dmpLogP->dump(s);
		e->Delete();
	}
}

void CRGLBackground::resize(void)
{
	setFullRectMatrices();
	setPlotRectMatrices();
}

void CRGLBackground::CreateDeviceResources(ID2D1HwndRenderTarget *hwndRenderTarget, ID2D1Factory *_factory)
{
	CRGLLayer::CreateDeviceResources(hwndRenderTarget, _factory);
}

void CRGLBackground::DiscardDeviceResources(void)
{
}

void CRGLBackground::launchParameterDialog(void)
{
	/*CColourEditDlg dlg(_T("Background colour"));
	dlg.setCol(br,bg,bb);
	int ret = (int) dlg.DoModal();
	if (IDOK == ret) dlg.getCol(&br,&bg,&bb);*/
}

//////////////////////////////////////////////////////

CRGLAxes::CRGLAxes(CString _regSection /* = _T("Axes") */, bool _showYlabels/* = true*/, bool _showXlabels/* = true*/, bool _rightY/* = false*/, bool _upperX /*= false*/) :
	showXlabels(_showXlabels),
	showYlabels(_showYlabels),
	upperX(_upperX),
	rightY(_rightY),
	regSection(_regSection),
	m_brush(NULL),
	m_brushForEditRect(NULL),
	m_brushForEditFilledRect(NULL)
{
	ar = ag = ab = .0f;
	athickness = 1.0f;
	tickLength = 2.0f;
	atransparency = 1.0f;
	editRectTransparency = .5f;
	type = RGLLayerTypeAxes;

	yLabelDecimals = 1;

	aGeometry = NULL;
	aTransformedGeometry = NULL;
	pDWriteFactory = NULL;

	pTextFormatX = NULL;
	pTextFormatY = NULL;


	FLOAT xRange = getPlotRectXScaled();
	xTick = 1.0f;
	xLabelInterval = 1;

	getHMSms(xRange, &hRange, &mRange, &sRange, &msRange);

	yTick = 1.0f;
	yLabelInterval = 2;
	showTicks = FALSE;


	if (!regSection.IsEmpty()) {
		CXTRegistryManager *reg = new CXTRegistryManager;

		//---Test registry
		double dtest;
		int OKt = reg->GetProfileDouble(regSection, _T("Red"), &dtest);
		if (!OKt) {
			delete reg;
			reg = new CXTRegistryManager(HKEY_LOCAL_MACHINE);
		}

		reg->GetProfileDouble(regSection, _T("Red"), &ar);
		reg->GetProfileDouble(regSection, _T("Green"), &ag);
		reg->GetProfileDouble(regSection, _T("Blue"), &ab);
		reg->GetProfileDouble(regSection, _T("Thickness"), &athickness);
		reg->GetProfileDouble(regSection, _T("Transparency"), &atransparency);
		reg->GetProfileDouble(regSection, _T("X tick distance"), &xTick);
		reg->GetProfileDouble(regSection, _T("Y tick distance"), &yTick);
		showTicks = reg->GetProfileInt(regSection, _T("Show ticks"), showTicks);
		xLabelInterval = reg->GetProfileInt(regSection, _T("x label interval"), xLabelInterval);
		xLabelInterval = xLabelInterval == 0 ? 1 : xLabelInterval;
		yLabelInterval = reg->GetProfileInt(regSection, _T("y label interval"), yLabelInterval);
		yLabelInterval = yLabelInterval == 0 ? 1 : yLabelInterval;
		reg->GetProfileString(regSection, _T("x label fmt string"), labelFmtX);
		reg->GetProfileString(regSection, _T("y label fmt string"), labelFmtY);
		yLabelDecimals = reg->GetProfileInt(regSection, _T("y label decimals"), yLabelDecimals);
		delete reg;
	}

	setYLabelDecimals(yLabelDecimals);
}

CRGLAxes::~CRGLAxes()
{
	if (aGeometry) aGeometry->Release();
	aGeometry = NULL;

	if (aTransformedGeometry) aTransformedGeometry->Release();
	aTransformedGeometry = NULL;

	if (m_brush) m_brush.Release();
	m_brush = NULL;

	if (!regSection.IsEmpty()) {
		CXTRegistryManager *reg = new CXTRegistryManager;
		reg->WriteProfileDouble(regSection, _T("Red"), &ar);
		reg->WriteProfileDouble(regSection, _T("Green"), &ag);
		reg->WriteProfileDouble(regSection, _T("Blue"), &ab);
		reg->WriteProfileDouble(regSection, _T("Thickness"), &athickness);
		reg->WriteProfileDouble(regSection, _T("Transparency"), &atransparency);
		reg->WriteProfileDouble(regSection, _T("X tick distance"), &xTick);
		reg->WriteProfileDouble(regSection, _T("Y tick distance"), &yTick);
		reg->WriteProfileInt(regSection, _T("x label interval"), xLabelInterval);
		reg->WriteProfileInt(regSection, _T("y label interval"), yLabelInterval);
		reg->WriteProfileString(regSection, _T("x label fmt string"), labelFmtX);
		reg->WriteProfileString(regSection, _T("y label fmt string"), labelFmtY);
		reg->WriteProfileInt(regSection, _T("x label interval"), xLabelInterval);

		reg->WriteProfileInt(regSection, _T("Show ticks"), showTicks);
		delete reg;
	}
}

CString CRGLAxes::getName(void)
{
	CString s = regSection;
	s.MakeReverse();
	s = s.SpanExcluding(_T("\\"));
	s.MakeReverse();
	return s;
}

void CRGLAxes::setYlabelInterval(int _yLabelInterval)
{
	yLabelInterval = _yLabelInterval;
}

void CRGLAxes::setYTick(double _yTick)
{
	yTick = _yTick;
}

void CRGLAxes::yAxisWizard(float _bottom, float _top)
{
	float length = _top - _bottom;

	//---Check for category string labels along y axis
	int numS = yStringLabelArray.GetCount();
	if (numS) {
		yTick = 1.0f;
		yLabelInterval = 1;
		return;
	}

	if (.0f == length) return;

	//--0.5
	if (length <= .5f) {
		yTick = .05f;
		yLabelInterval = 2;
		setYLabelDecimals(2);
		return;
	}
	//--1
	if (length <= 1.0f) {
		yTick = .1f;
		yLabelInterval = 2;
		setYLabelDecimals(2);
		return;
	}
	//--2.5
	if (length <= 2.5f) {
		yTick = .25f;
		yLabelInterval = 2;
		setYLabelDecimals(2);
		return;
	}
	//--5
	if (length <= 5.0f) {
		yTick = .5f;
		yLabelInterval = 2;
		setYLabelDecimals(1);
		return;
	}
	//--10 
	if (length <= 10.0f) {
		yTick = 1.0f;
		yLabelInterval = 2;
		setYLabelDecimals(1);
		return;
	}
	//--20
	if (length <= 20.0f) {
		yTick = 2.0f;
		yLabelInterval = 2;
		setYLabelDecimals(1);
		return;
	}
	//--40
	if (length <= 40.0f) {
		yTick = 4.0f;
		yLabelInterval = 2;
		setYLabelDecimals(0);
		return;
	}
	//--50
	if (length <= 50.0f) {
		yTick = 5.0f;
		yLabelInterval = 2;
		setYLabelDecimals(0);
		return;
	}
	//--75
	if (length <= 75.0f) {
		yTick = 10.0f;
		yLabelInterval = 2;
		setYLabelDecimals(0);
		return;
	}
	// 100
	if (length <= 100.0f) {
		yTick = 10.0f;
		yLabelInterval = 2;
		return;
	}
	// 200
	if (length <= 200.0f) {
		yTick = 20.0f;
		yLabelInterval = 2;
		return;
	}
	// 400
	if (length <= 400.0f) {
		yTick = 40.0f;
		yLabelInterval = 2;
		setYLabelDecimals(0);
		return;
	}
	// 500
	if (length <= 500.0f) {
		yTick = 50.0f;
		yLabelInterval = 2;
		setYLabelDecimals(0);
		return;
	}
	//--750
	if (length <= 750.0f) {
		yTick = 100.0f;
		yLabelInterval = 2;
		setYLabelDecimals(0);
		return;
	}
	//--1000
	if (length <= 1000.0f) {
		yTick = 100.0f;
		yLabelInterval = 2;
		setYLabelDecimals(0);
		return;
	}
	//--2000
	if (length <= 2000.0f) {
		yTick = 200.0f;
		yLabelInterval = 2;
		setYLabelDecimals(0);
		return;
	}
	//--3000
	if (length <= 3000.0f) {
		yTick = 300.0f;
		yLabelInterval = 2;
		setYLabelDecimals(0);
		return;
	}
	//--4000
	if (length <= 4000.0f) {
		yTick = 400.0f;
		yLabelInterval = 2;
		setYLabelDecimals(0);
		return;
	}
	//---5000
	if (length <= 5000.0f) {
		yTick = 500.0f;
		yLabelInterval = 2;
		setYLabelDecimals(0);
		return;
	}
	//--7500
	if (length <= 7500.0f) {
		yTick = 1000.0f;
		yLabelInterval = 2;
		setYLabelDecimals(0);
		return;
	}
	//--15000
	if (length <= 15000.0f) {
		yTick = 2000.0f;
		yLabelInterval = 2;
		setYLabelDecimals(0);
		return;
	}
	//--30000
	if (length <= 30000.0f) {
		yTick = 3000.0f;
		yLabelInterval = 2;
		setYLabelDecimals(0);
		return;
	}
	//--50000
	if (length <= 50000.0f) {
		yTick = 5000.0f;
		yLabelInterval = 2;
		setYLabelDecimals(0);
		return;
	}
	//--100000
	if (length <= 100000.0f) {
		yTick = 10000.0f;
		yLabelInterval = 2;
		setYLabelDecimals(0);
		return;
	}
	//--200000
	if (length <= 200000.0f) {
		yTick = 20000.0f;
		yLabelInterval = 2;
		setYLabelDecimals(0);
		return;
	}
	//--500000
	if (length <= 500000.0f) {
		yTick = 50000.0f;
		yLabelInterval = 2;
		setYLabelDecimals(0);
		return;
	}
	//--1000000
	if (length <= 1000000.0f) {
		yTick = 100000.0f;
		yLabelInterval = 2;
		setYLabelDecimals(0);
		return;
	}
	yTick = 100000.0f;
	yLabelInterval = 2;
	setYLabelDecimals(0);
}

void CRGLAxes::xAxisWizard(float _from, float _to)
{
	float length = _to - _from;

	if (.0f == length) return;
	//--5 sec
	if (length <= 5.0f) {
		xTick = .5f;		// 1 sec
		xLabelInterval = 2;
		return;
	}
	//--10 sec
	if (length <= 10.0f) {
		xTick = 1.0f;		// 1 sec
		xLabelInterval = 2;
		return;
	}
	//--20 sec
	if (length <= 20.0f) {
		xTick = 2.0f;		// 1 sec
		xLabelInterval = 2;
		return;
	}
	//--40 sec
	if (length <= 40.0f) {
		xTick = 4.0f;		// 1 sec
		xLabelInterval = 2;
		return;
	}
	// 1 min
	if (length <= 60.0f) {
		xTick = 5.0f;		// 10 sec
		xLabelInterval = 2;
		return;
	}
	// 5 min
	if (length <= 300.0f) {
		xTick = 30.0f;		// 1 min
		xLabelInterval = 2;
		return;
	}
	// 10 min
	if (length <= 600.0f) {
		xTick = 60.0f;		// 1 min
		xLabelInterval = 2;
		return;
	}
	// 20 min
	if (length <= 1200.0f) {
		xTick = 120.0f;		// 2 min
		xLabelInterval = 2;
		return;
	}
	// 40 min
	if (length <= 2400.0f) {
		xTick = 240.0f;		// 2 min
		xLabelInterval = 2;
		return;
	}
	// 60 min
	if (length <= 3600.0f) {
		xTick = 360.0f;		// 10 min
		xLabelInterval = 2;
		return;
	}
	// 2 h
	if (length <= 7200.0f) {
		xTick = 600.0f;		// 20 min
		xLabelInterval = 2;
		return;
	}
	// 3 h
	if (length <= 10800.0f) {
		xTick = 1200.0f;		// 30 min
		xLabelInterval = 2;
		return;
	}
	// 4 h
	if (length <= 14400.0f) {
		xTick = 1200.0f;		// 30 min
		xLabelInterval = 2;
		return;
	}
	// 5 h
	if (length <= 18000.0f) {
		xTick = 1800.0f;		// 60 min
		xLabelInterval = 2;
		return;
	}
	// 6 h
	if (length <= 21600.0f) {
		xTick = 1800.0f;		// 60 min
		xLabelInterval = 2;
		return;
	}
	// 7 h
	if (length <= 25200.0f) {
		xTick = 3600.0f;		// 60 min
		xLabelInterval = 2;
		return;
	}
	// 8 h
	if (length <= 28800.0f) {
		xTick = 3600.0f;		// 60 min
		xLabelInterval = 2;
		return;
	}
	xTick = 3600.0f;			//  1 h
	xLabelInterval = 2;
}

void CRGLAxes::setXlabelInterval(int _xLabelInterval)
{
	xLabelInterval = _xLabelInterval;
}


void CRGLAxes::setXTick(double _xTick)
{
	xTick = _xTick;
}

void CRGLAxes::setShowXlabels(bool _show)
{
	showXlabels = _show;
}

void CRGLAxes::setShowTicks(bool _show)
{
	showTicks = _show;
}

void CRGLAxes::setShowYlabels(bool _show)
{
	showYlabels = _show;
}

void CRGLAxes::setColour(double _br, double _bg, double _bb)
{
	ar = _br;
	ag = _bg;
	ab = _bb;
}

void CRGLAxes::processFullRect(void)
{
	CRGLLayer::processFullRect();
}

void CRGLAxes::processPlotRect(void)
{
	CRGLLayer::processPlotRect();
	if (!myTile) return;

	FLOAT xRange = getPlotRectXScaled();

	getHMSms(xRange, &hRange, &mRange, &sRange, &msRange);

	D2D1_RECT_F plotRectScaled = myTile->getPlotRectScaled();

	makeSourceGeometry(); // *INN* Should not be necessary!!
	makeTransformedGeometry();
	generateLabelArray();
}

void CRGLAxes::makeTransformedGeometry(void)
{
	if (!mp_factory) return;
	if (!aGeometry) return;

	if (aTransformedGeometry) aTransformedGeometry->Release();
	aTransformedGeometry = NULL;
	HRESULT hr = mp_factory->CreateTransformedGeometry(aGeometry, matPlotRect, &aTransformedGeometry);
}

void CRGLAxes::renderEditRect(float _left, float _right)
{
	if (!mp_factory) return;

	D2D1_RECT_F plotRectScaled = myTile->getPlotRectScaled();
	D2D1_POINT_2F p00 = D2D1::Point2F(_left, plotRectScaled.bottom);
	D2D1_POINT_2F p01 = D2D1::Point2F(_right, plotRectScaled.bottom);
	D2D1_POINT_2F p11 = D2D1::Point2F(_right, plotRectScaled.top);
	D2D1_POINT_2F p10 = D2D1::Point2F(_left, plotRectScaled.top);

	ID2D1PathGeometry *gm = NULL;
	ID2D1TransformedGeometry *tmg = NULL;
	ID2D1GeometrySink *sinkx = NULL;
	mp_factory->CreatePathGeometry(&gm);
	if (gm) {
		gm->Open(&sinkx);
		if (sinkx) {

			sinkx->BeginFigure(p00, D2D1_FIGURE_BEGIN_FILLED);
			sinkx->AddLine(p01);
			sinkx->AddLine(p11);
			sinkx->AddLine(p10);
			sinkx->AddLine(p00);
			sinkx->EndFigure(D2D1_FIGURE_END_CLOSED);

			sinkx->Close();
			sinkx->Release();
			sinkx = NULL;
		}
		HRESULT hr = mp_factory->CreateTransformedGeometry(gm, matPlotRect, &tmg);
		if (tmg) {
			mp_target->DrawGeometry(tmg, m_brushForEditRect, 3.0f);
			mp_target->FillGeometry(tmg, m_brushForEditFilledRect);
		}
	}
	if (tmg) tmg->Release();
	if (gm) gm->Release();
}

void CRGLAxes::makeSourceGeometry(void)
{
	if (!mp_factory) return;
	if (!myTile) return;

	if (aGeometry) aGeometry->Release();
	aGeometry = NULL;

	D2D1_RECT_F plotRectScaled = myTile->getPlotRectScaled();
	D2D1_POINT_2F p00 = D2D1::Point2F(plotRectScaled.left, plotRectScaled.bottom);
	D2D1_POINT_2F p01 = D2D1::Point2F(plotRectScaled.right, plotRectScaled.bottom);
	D2D1_POINT_2F p11 = D2D1::Point2F(plotRectScaled.right, plotRectScaled.top);
	D2D1_POINT_2F p10 = D2D1::Point2F(plotRectScaled.left, plotRectScaled.top);

	ID2D1GeometrySink *sinkx = NULL;
	mp_factory->CreatePathGeometry(&aGeometry);
	if (aGeometry) {
		aGeometry->Open(&sinkx);
		if (sinkx) {

			//---Frame
			sinkx->BeginFigure(p00, D2D1_FIGURE_BEGIN_HOLLOW);
			sinkx->AddLine(p01);
			sinkx->AddLine(p11);
			sinkx->AddLine(p10);
			sinkx->AddLine(p00);
			sinkx->EndFigure(D2D1_FIGURE_END_OPEN);

			//---Zero line
			if ((plotRectScaled.top > .0f) && (plotRectScaled.bottom < .0f)) {
				D2D1_POINT_2F pZeroLeft = D2D1::Point2F(plotRectScaled.left, .0f);
				D2D1_POINT_2F pZeroRight = D2D1::Point2F(plotRectScaled.right, .0f);
				sinkx->BeginFigure(p00, D2D1_FIGURE_BEGIN_HOLLOW);
				sinkx->AddLine(pZeroLeft);
				sinkx->AddLine(pZeroRight);
				sinkx->EndFigure(D2D1_FIGURE_END_OPEN);
			}

			//---Centre line
			FLOAT mid = (plotRectScaled.left + plotRectScaled.right) / 2.0f;
			D2D1_POINT_2F pMidTop = D2D1::Point2F(mid, plotRectScaled.top);
			D2D1_POINT_2F pMidBottom = D2D1::Point2F(mid, plotRectScaled.bottom);
			sinkx->BeginFigure(pMidTop, D2D1_FIGURE_BEGIN_HOLLOW);
			sinkx->AddLine(pMidBottom);
			sinkx->EndFigure(D2D1_FIGURE_END_OPEN);

			if (showTicks) {
				float xPos = plotRectScaled.left;
				D2D1_POINT_2F p1 = upperX ? D2D1::Point2F(xPos, plotRectScaled.top) : D2D1::Point2F(xPos, plotRectScaled.bottom);
				D2D1_POINT_2F p2 = upperX ?
					D2D1::Point2F(xPos, plotRectScaled.top - (float)(tickLength + athickness) / matPlotRect._22) :
					D2D1::Point2F(xPos, plotRectScaled.bottom + (float)(tickLength + athickness) / matPlotRect._22);
				xPos += (float)xTick;
				while (xPos < plotRectScaled.right) {
					p1.x = xPos;
					p2.x = xPos;
					sinkx->BeginFigure(p1, D2D1_FIGURE_BEGIN_HOLLOW);
					sinkx->AddLine(p2);
					sinkx->EndFigure(D2D1_FIGURE_END_OPEN);
					xPos += (float)xTick;
				}

				float yPos = plotRectScaled.bottom;
				p1.x = rightY ? plotRectScaled.right : plotRectScaled.left;
				p1.y = yPos;
				p2.x = rightY ?
					plotRectScaled.right + (float)(tickLength + athickness) / matPlotRect._11 :
					plotRectScaled.left - (float)(tickLength + athickness) / matPlotRect._11;
				p2.y = yPos;
				yPos += (float)yTick;
				while (yPos < plotRectScaled.top) {
					p1.y = yPos;
					p2.y = yPos;
					sinkx->BeginFigure(p1, D2D1_FIGURE_BEGIN_HOLLOW);
					sinkx->AddLine(p2);
					sinkx->EndFigure(D2D1_FIGURE_END_OPEN);
					yPos += (float)yTick;
				}
			}

			sinkx->Close();
			sinkx->Release();
			sinkx = NULL;
		}
		generateLabelArray();
	}
}

void CRGLAxes::render(bool _hideYlabels)
{
	if (!show) return;
	CRGLLayer::render();

	if (!mp_target) return;
	if (!m_brush) return;
	if (!pTextFormatX) return;
	if (!pTextFormatY) return;
	if (!aTransformedGeometry) return;

	mp_target->DrawGeometry(aTransformedGeometry, m_brush, (float)athickness);

	//---X Labels
	if (showXlabels) {
		int num = (int)xLblArray.GetCount();
		D2D1_POINT_2F p0, p1;
		for (int i = 0; i < num; i++) {
			TEXT_DESCR td = xLblArray.GetAt(i);
			p0 = td.p0 * matPlotRect;
			p1 = td.p1 * matPlotRect;
			D2D1_RECT_F r = D2D1::RectF(p0.x, p0.y, p1.x, p1.y);
			mp_target->DrawText(td.txt, td.txt.GetLength(), pTextFormatX, r, m_brush);
		}
	}

	//---Y labels
	if (showYlabels && !_hideYlabels) {
		int num = (int)yLblArray.GetCount();
		D2D1_POINT_2F p0, p1;
		for (int i = 0; i < num; i++) {
			TEXT_DESCR td = yLblArray.GetAt(i);
			p0 = td.p0 * matPlotRect;
			p0.x = 0; // Always start at left
			p1 = td.p1 * matPlotRect;
			D2D1_RECT_F r = D2D1::RectF(p0.x, p0.y, p1.x, p1.y);
			mp_target->DrawText(td.txt, td.txt.GetLength(), pTextFormatY, r, m_brush);
		}
	}
}


void CRGLAxes::getHMSms(FLOAT _xRange, int *_h, int *_m, int *_s, int *_ms)
{
	int secs = (int)_xRange;
	float msf = 1000.0f * (_xRange - secs);
	*_ms = (int)msf;
	*_h = secs / 3600;
	int rest = secs - *_h * 3600;
	*_m = rest / 60;
	rest -= *_m * 60;
	*_s = rest;
}

void CRGLAxes::setLabelTexts(CString _one, CString _two, CString _three, CString _four, CString _five)
{
	yStringLabelArray.RemoveAll();
	yStringLabelArray.Add(_one);
	yStringLabelArray.Add(_two);
	yStringLabelArray.Add(_three);
	yStringLabelArray.Add(_four);
	yStringLabelArray.Add(_five);

	yStringLabelArray.Add(_T(""));
}

void CRGLAxes::generateLabelArray(void)
{
	if (!myTile) return;

	xLblArray.RemoveAll();
	yLblArray.RemoveAll();

	D2D1_RECT_F plotRectScaled = myTile->getPlotRectScaled();

	//---X axis	
	int h, m, s;
	int ms;
	float xPos = plotRectScaled.left;
	float yPos = upperX ?
		plotRectScaled.top - 2.0f * (float)(tickLength + athickness) / matPlotRect._22 :
		plotRectScaled.bottom + 2.0f * (float)(tickLength + athickness) / matPlotRect._22;
	int index = 0;
	float rightLim = plotRectScaled.right + (float)xTick;
	do {
		index++;
		if ((index == xLabelInterval) && ((xPos < plotRectScaled.right))) {
			TEXT_DESCR td;
			td.p0.x = .0f;
			td.p0.y = .0f;
			td.p1.x = .0f;
			td.p1.y = .0f;
			if (xPos >= .0f) {
				getHMSms(xPos, &h, &m, &s, &ms);
				if (hRange > 0) td.txt.Format(_T("%02d:%02d:%02d"), h, m, s);
				else if (mRange > 0) td.txt.Format(_T("%02d:%02d:%02d"), h, m, s);
				else td.txt.Format(_T("%02d:%02d:%d.%03d"), h, m, s, ms);
			}
			else {
				getHMSms(-xPos, &h, &m, &s, &ms);
				if (hRange > 0) td.txt.Format(_T("-%02d:%02d:%02d"), h, m, s);
				else if (mRange > 0) td.txt.Format(_T("-%02d:%02d:%02d"), h, m, s);
				else td.txt.Format(_T("-%02d:%02d:%d.%03d"), h, m, s, ms);
			}

			td.p0.x = xPos - fntSize * td.txt.GetLength() / (2.0f * matPlotRect._11);
			td.p1.x = xPos + fntSize * td.txt.GetLength() / (2.0f * matPlotRect._11);
			if (upperX) {
				td.p0.y = yPos - fntSize / matPlotRect._22;
				td.p1.y = yPos;
			}
			else {
				td.p0.y = yPos;
				td.p1.y = yPos + fntSize / matPlotRect._22;
			}
			xLblArray.Add(td);
			index = 0;
		}
		xPos += (float)xTick;
	} while (xPos < rightLim);

	//---Y axis	
	yPos = plotRectScaled.bottom;
	xPos = rightY ?
		plotRectScaled.right + 3.0f * (float)(tickLength + athickness) / matPlotRect._11 :
		plotRectScaled.left - 3.0f * (float)(tickLength + athickness) / matPlotRect._11;
	index = 0;
	do {
		yPos += (float)yTick;
		index++;
		if ((index == yLabelInterval) && (yPos < plotRectScaled.top)) {
			TEXT_DESCR td;
			td.txt.Format(labelFmtY, yPos);

			td.p0.y = yPos + fntSize / matPlotRect._22;
			td.p1.y = yPos - fntSize / matPlotRect._22;
			if (rightY) {
				td.p0.x = xPos;
				td.p1.x = xPos + fntSize * td.txt.GetLength() / (2.0f * matPlotRect._11);
			}
			else {
				td.p0.x = xPos - fntSize * td.txt.GetLength() / (2.0f * matPlotRect._11);
				td.p1.x = xPos;
			}

			yLblArray.Add(td);
			index = 0;
		}
	} while (yPos < plotRectScaled.top);

	int numS = yStringLabelArray.GetCount();
	if (numS) {
		int num = yLblArray.GetCount();
		for (int j = 1; j <= num; j++) {
			if ((num >= j) && (numS >= j)) {
				TEXT_DESCR td = yLblArray.GetAt(j - 1);
				td.txt = yStringLabelArray.GetAt(j - 1);
				yLblArray.SetAt(j - 1, td);
			}
		}
	}
}

/*
Description: Sets number of decimals in label
_numY : Number of decimals y-axis labels
*/
void CRGLAxes::setYLabelDecimals(int _numY)
{
	yLabelDecimals = _numY;

	switch (_numY) {
	case 0:
		labelFmtY = _T("%.0f");
		break;
	case 1:
		labelFmtY = _T("%.1f");
		break;
	case 2:
		labelFmtY = _T("%.2f");
		break;
	case 3:
		labelFmtY = _T("%.3f");
		break;
	case 4:
		labelFmtY = _T("%.4f");
		break;
	case 5:
		labelFmtY = _T("%.5f");
		break;
	case 6:
		labelFmtY = _T("%.6f");
		break;
	case 7:
		labelFmtY = _T("%.7f");
		break;
	case 8:
		labelFmtY = _T("%.8f");
		break;
	case 9:
		labelFmtY = _T("%.9f");
		break;
	default:
		labelFmtY = _T("%.0f");
		break;
	}
}

void CRGLAxes::CreateDeviceResources(ID2D1HwndRenderTarget *hwndRenderTarget, ID2D1Factory *_factory)
{
	CRGLLayer::CreateDeviceResources(hwndRenderTarget, _factory);
	if (m_brush) m_brush.Release();
	m_brush = NULL;
	HRESULT hr = mp_target->CreateSolidColorBrush(D2D1::ColorF((float)ar, (float)ag, (float)ab, (float)atransparency), &m_brush);

	if (m_brushForEditRect) m_brushForEditRect.Release();
	m_brushForEditRect = NULL; 
	HRESULT hr2 = mp_target->CreateSolidColorBrush(D2D1::ColorF((float)ar, (float)ag, (float)ab, (float)editRectTransparency), &m_brushForEditRect);
	HRESULT hr3 = mp_target->CreateSolidColorBrush(D2D1::ColorF(1.0f,.6f, .45f, (float) .7f), &m_brushForEditFilledRect);


	hr = DWriteCreateFactory(
		DWRITE_FACTORY_TYPE_SHARED,
		__uuidof(IDWriteFactory),
		reinterpret_cast<IUnknown**>(&pDWriteFactory)
		);
	if (SUCCEEDED(hr)) {
		hr = pDWriteFactory->CreateTextFormat(
			L"Times New Roman", // Font family name.
			NULL,        // Font collection (NULL sets it to use the system font collection).
			DWRITE_FONT_WEIGHT_REGULAR,
			DWRITE_FONT_STYLE_NORMAL,
			DWRITE_FONT_STRETCH_NORMAL,
			fntSize,
			L"en-us",
			&pTextFormatX
			);
	}
	if (SUCCEEDED(hr)) {
		hr = pTextFormatX->SetTextAlignment(DWRITE_TEXT_ALIGNMENT_CENTER);
	}
	if (SUCCEEDED(hr)) {
		hr = pTextFormatX->SetParagraphAlignment(DWRITE_PARAGRAPH_ALIGNMENT_CENTER);
	}

	if (SUCCEEDED(hr)) {
		hr = pDWriteFactory->CreateTextFormat(
			L"Times New Roman", // Font family name.
			NULL,        // Font collection (NULL sets it to use the system font collection).
			DWRITE_FONT_WEIGHT_REGULAR,
			DWRITE_FONT_STYLE_NORMAL,
			DWRITE_FONT_STRETCH_NORMAL,
			fntSize,
			L"en-us",
			&pTextFormatY
			);
	}
	if (SUCCEEDED(hr)) {
		hr = pTextFormatY->SetTextAlignment(rightY ? DWRITE_TEXT_ALIGNMENT_LEADING : DWRITE_TEXT_ALIGNMENT_TRAILING);
	}
	if (SUCCEEDED(hr)) {
		hr = pTextFormatY->SetParagraphAlignment(DWRITE_PARAGRAPH_ALIGNMENT_CENTER);
	}
	makeSourceGeometry();
	makeTransformedGeometry();
}

void CRGLAxes::DiscardDeviceResources(void)
{
	if (m_brush) m_brush.Release();
	m_brush = NULL;
	if (aGeometry) aGeometry->Release();
	aGeometry = NULL;
	if (aTransformedGeometry) aTransformedGeometry->Release();
	aTransformedGeometry = NULL;
	if (pTextFormatX) pTextFormatX->Release();
	pTextFormatX = NULL;
	if (pTextFormatY) pTextFormatY->Release();
	pTextFormatY = NULL;
	if (pDWriteFactory) pDWriteFactory.Release();
	pDWriteFactory = NULL;
}

void CRGLAxes::launchParameterDialog(void)
{
	//CXYAxesPar dlg;
	//dlg.setParX(plotRectScale.left,plotRectScale.right,xTick,xLabelInterval,xLabelDecimals,rightY);
	//dlg.setParY(plotRectScale.bottom,plotRectScale.top,yTick,yLabelInterval,yLabelDecimals,upperX);
	//dlg.setParXY(athickness,atransparency,ar,ag,ab);
	//int ret = (int) dlg.DoModal();
	//if (IDOK == ret) {
	//	double mi,ma;
	//	dlg.getParX(&mi,&ma,&xTick,&xLabelInterval,&xLabelDecimals,&rightY);
	//	plotRectScale.left = (float) mi;
	//	plotRectScale.right = (float) ma;
	//	dlg.getParY(&mi,&ma,&yTick,&yLabelInterval,&yLabelDecimals,&upperX);
	//	plotRectScale.bottom = (float) mi;
	//	plotRectScale.top = (float) ma;

	//	myTile->setPlotRectScale(plotRectScale);

	//	dlg.getParXY(&athickness,&atransparency,&ar,&ag,&ab);
	//	setFullRectMatrices();
	//	setPlotRectMatrices();
	//	makeGeometry(); // Calls generateLabel array
	//	setLabelDecimals(xLabelDecimals,yLabelDecimals);
	//	generateLabelArray();
	//	if (mp_target) {
	//		if (m_brush) m_brush.Release();
	//		m_brush = NULL;
	//		HRESULT hr = mp_target->CreateSolidColorBrush(D2D1::ColorF((float) ar,(float) ag,(float) ab,(float) atransparency),&m_brush);
	//	}
	//	if (parentWnd) parentWnd->Invalidate();
	//}
}


void CRGLAxes::resize(void)
{
	CRGLLayer::resize();
	if (!mp_factory) return;
	if (!aGeometry) return;

	makeSourceGeometry();
	makeTransformedGeometry();
	generateLabelArray();
}

//////////////////////////////////////////////////////


CRGLGrid::CRGLGrid(CString _regSection /* = _T("Axes") */) :
	regSection(_regSection)
{
	gr = gg = gb = .80f;
	gthickness = 1.0f;
	gtransparency = 1.0f;
	type = RGLLayerTypeGrid;
	m_brush = NULL;

	gGeometry = NULL;
	gTransformedGeometry = NULL;

	xTick = 1.0f;
	yTick = 1.0f;

	if (!regSection.IsEmpty()) {
		CXTRegistryManager *reg = new CXTRegistryManager;

		//---Test registry
		double dtest;
		int OKt = reg->GetProfileDouble(regSection, _T("Red"), &dtest);
		if (!OKt) {
			delete reg;
			reg = new CXTRegistryManager(HKEY_LOCAL_MACHINE);
		}

		reg->GetProfileDouble(regSection, _T("Red"), &gr);
		reg->GetProfileDouble(regSection, _T("Green"), &gg);
		reg->GetProfileDouble(regSection, _T("Blue"), &gb);
		reg->GetProfileDouble(regSection, _T("Thickness"), &gthickness);
		reg->GetProfileDouble(regSection, _T("Transparency"), &gtransparency);
		reg->GetProfileDouble(regSection, _T("X tick distance"), &xTick);
		reg->GetProfileDouble(regSection, _T("Y tick distance"), &yTick);
		delete reg;
	}
}

CRGLGrid::~CRGLGrid()
{
	DiscardDeviceResources();

	if (!regSection.IsEmpty()) {
		CXTRegistryManager *reg = new CXTRegistryManager;
		reg->WriteProfileDouble(regSection, _T("Red"), &gr);
		reg->WriteProfileDouble(regSection, _T("Green"), &gg);
		reg->WriteProfileDouble(regSection, _T("Blue"), &gb);
		reg->WriteProfileDouble(regSection, _T("Thickness"), &gthickness);
		reg->WriteProfileDouble(regSection, _T("Transparency"), &gtransparency);
		reg->WriteProfileDouble(regSection, _T("X tick distance"), &xTick);
		reg->WriteProfileDouble(regSection, _T("Y tick distance"), &yTick);
		delete reg;
	}
}

CString CRGLGrid::getName(void)
{
	CString s = regSection;
	s.MakeReverse();
	s = s.SpanExcluding(_T("\\"));
	s.MakeReverse();
	return s;
}

void CRGLGrid::render(void)
{
	if (!show) return;

	CRGLLayer::render();

	if (!mp_target) return;
	if (!m_brush) return;
	if (!gTransformedGeometry) return;

	/*D2D1_RECT_F plotRectScale = myTile->getPlotRectScale();
	D2D1_POINT_2F p0 = D2D1::Point2F(plotRectScale.left,plotRectScale.top);
	D2D1_POINT_2F p1 = D2D1::Point2F(plotRectScale.right,plotRectScale.bottom);
	p0 = p0 * matPlotRect;
	p1 = p1 * matPlotRect;
	clipRect = D2D1::RectF(p0.x,p0.y,p1.x,p1.y);*/

	mp_target->PushAxisAlignedClip(clipRect, D2D1_ANTIALIAS_MODE_PER_PRIMITIVE);
	mp_target->DrawGeometry(gTransformedGeometry, m_brush, (float)gthickness);
}

void CRGLGrid::CreateDeviceResources(ID2D1HwndRenderTarget *hwndRenderTarget, ID2D1Factory *_factory)
{
	CRGLLayer::CreateDeviceResources(hwndRenderTarget, _factory);
	if (m_brush) m_brush.Release();
	m_brush = NULL;
	HRESULT hr = mp_target->CreateSolidColorBrush(D2D1::ColorF((float)gr, (float)gg, (float)gb, (float)gtransparency), &m_brush);

	makeSourceGeometry();
	makeTransformedGeometry();
}

void CRGLGrid::DiscardDeviceResources(void)
{
	if (m_brush) m_brush.Release();
	m_brush = NULL;

	if (gGeometry) gGeometry->Release();
	gGeometry = NULL;

	if (gTransformedGeometry) gTransformedGeometry->Release();
	gTransformedGeometry = NULL;
}

void CRGLGrid::launchParameterDialog(void)
{
	//CGridPar dlg;
	//dlg.setParX(xTick);
	//dlg.setParY(yTick);
	//dlg.setParXY(gthickness,gtransparency,gr,gg,gb);
	//int ret = (int) dlg.DoModal();
	//if (IDOK == ret) {
	//	dlg.getParX(&xTick);
	//	dlg.getParY(&yTick);
	//	dlg.getParXY(&gthickness,&gtransparency,&gr,&gg,&gb);
	//	setFullRectMatrices();
	//	setPlotRectMatrices();
	//	makeGeometry(); 
	//	if (mp_target) {
	//		if (m_brush) m_brush.Release();
	//		m_brush = NULL;
	//		HRESULT hr = mp_target->CreateSolidColorBrush(D2D1::ColorF((float) gr,(float) gg,(float) gb,(float) gtransparency),&m_brush);
	//	}
	//	if (parentWnd) parentWnd->Invalidate();
	//}
}

void CRGLGrid::resize(void)
{
	CRGLLayer::resize();
	if (!mp_factory) return;
	if (!gGeometry) return;

	makeSourceGeometry();
	makeTransformedGeometry();
}

void CRGLGrid::processFullRect(void)
{
	CRGLLayer::processFullRect();
}

void CRGLGrid::processPlotRect(void)
{
	CRGLLayer::processPlotRect();
	if (!myTile) return;

	D2D1_RECT_F plotRectScaled = myTile->getPlotRectScaled();

	makeSourceGeometry();
	makeTransformedGeometry();
}

void CRGLGrid::makeTransformedGeometry(void)
{
	if (!mp_factory) return;
	if (!gGeometry) return;

	if (gTransformedGeometry) gTransformedGeometry->Release();
	gTransformedGeometry = NULL;

	HRESULT hr = mp_factory->CreateTransformedGeometry(gGeometry, matPlotRect, &gTransformedGeometry);
}

void CRGLGrid::makeSourceGeometry(void)
{
	if (!mp_factory) return;
	if (!myTile) return;

	if (gGeometry) gGeometry->Release();
	gGeometry = NULL;

	if (gTransformedGeometry) gTransformedGeometry->Release();
	gTransformedGeometry = NULL;

	D2D1_RECT_F plotRectScaled = myTile->getPlotRectScaled();

	ID2D1GeometrySink *sinkg = NULL;
	mp_factory->CreatePathGeometry(&gGeometry);
	if (gGeometry) {
		gGeometry->Open(&sinkg);
		if (sinkg) {
			float xPos = plotRectScaled.left;
			D2D1_POINT_2F p1 = D2D1::Point2F(xPos, plotRectScaled.top);
			D2D1_POINT_2F p2 = D2D1::Point2F(xPos, plotRectScaled.bottom);
			do {
				xPos += (float)xTick;
				p1.x = xPos;
				p2.x = xPos;
				sinkg->BeginFigure(p1, D2D1_FIGURE_BEGIN_HOLLOW);
				sinkg->AddLine(p2);
				sinkg->EndFigure(D2D1_FIGURE_END_OPEN);
			} while (xPos < plotRectScaled.right);

			float yPos = plotRectScaled.bottom;
			p1.x = plotRectScaled.left;
			p1.y = yPos;
			p2.x = plotRectScaled.right;
			p2.y = yPos;
			do {
				yPos += (float)yTick;
				p1.y = yPos;
				p2.y = yPos;
				sinkg->BeginFigure(p1, D2D1_FIGURE_BEGIN_HOLLOW);
				sinkg->AddLine(p2);
				sinkg->EndFigure(D2D1_FIGURE_END_OPEN);
			} while (yPos < plotRectScaled.top);

			sinkg->Close();
			sinkg->Release();
			sinkg = NULL;
		}
	}
}

////////////////////////////////////////////


CRGLText::CRGLText(CString _regSection /*= _T("Texts")*/) :
	regSection(_regSection)
{
	tr = tg = tb = .0f;
	ttransparency = 1.0f;
	type = RGLLayerTypeText;
	m_brush = NULL;
	pTextFormatC = NULL;
	pTextFormatL = NULL;
	pTextFormatR = NULL;
	pDWriteFactory = NULL;

	xTxt.txt = _T("x unit");
	yLeftTxt.txt = _T("y unit");
	yRightTxt.txt = _T("y unit");
	title.txt = _T("Title");
	setCorners();

	if (!regSection.IsEmpty()) {
		CXTRegistryManager *reg = new CXTRegistryManager;

		//---Test registry
		double dtest;
		int OKt = reg->GetProfileDouble(regSection, _T("Red"), &dtest);
		if (!OKt) {
			delete reg;
			reg = new CXTRegistryManager(HKEY_LOCAL_MACHINE);
		}

		reg->GetProfileDouble(regSection, _T("Red"), &tr);
		reg->GetProfileDouble(regSection, _T("Green"), &tg);
		reg->GetProfileDouble(regSection, _T("Blue"), &tb);
		reg->GetProfileDouble(regSection, _T("Transparency"), &ttransparency);
		delete reg;
	}
}

CRGLText::~CRGLText()
{
	if (!regSection.IsEmpty()) {
		CXTRegistryManager *reg = new CXTRegistryManager;
		reg->WriteProfileDouble(regSection, _T("Red"), &tr);
		reg->WriteProfileDouble(regSection, _T("Green"), &tg);
		reg->WriteProfileDouble(regSection, _T("Blue"), &tb);
		reg->WriteProfileDouble(regSection, _T("Transparency"), &ttransparency);
		delete reg;
	}
}

void CRGLText::setColour(double _r, double _g, double _b)
{
	tr = _r;
	tg = _g;
	tb = _b;

	if (!mp_target) {
		AfxMessageBox(_T("Could not create new colour - mp_target not defined"));
		return;
	}

	if (m_brush) m_brush.Release();
	m_brush = NULL;
	HRESULT hr = mp_target->CreateSolidColorBrush(D2D1::ColorF((float)tr, (float)tg, (float)tb, (float)ttransparency), &m_brush);
	if (!SUCCEEDED(hr)) AfxMessageBox(_T("Could not create new colour"));

	if (parentWnd) parentWnd->Invalidate();
}

void CRGLText::resize(void)
{
	CRGLLayer::resize();
	setCorners();
}

void CRGLText::processFullRect(void)
{
	CRGLLayer::processFullRect();
	setCorners();
}

void CRGLText::processPlotRect(void)
{
	CRGLLayer::processPlotRect();
}

void CRGLText::setCorners(void)
{
	if (!myTile) return;

	D2D1_RECT_F fullRectScaled = myTile->getFullRectScaled();

	float fh = matFullRect._22 == .0f ? -1 : fntSize / matFullRect._22;
	float fv = matFullRect._11 == .0f ? 1 : fntSize / matFullRect._11;

	yLeftTxt.p0 = D2D1::Point2F(fullRectScaled.left, fullRectScaled.top);
	yLeftTxt.p1 = D2D1::Point2F(fullRectScaled.left + maxYTxtLength * 2 * fv, fullRectScaled.bottom);

	yRightTxt.p0 = D2D1::Point2F(fullRectScaled.right, fullRectScaled.top);
	yRightTxt.p1 = D2D1::Point2F(fullRectScaled.right + maxYTxtLength * 2 * fv, fullRectScaled.bottom);

	xTxt.p0 = D2D1::Point2F(fullRectScaled.right - xTxt.txt.GetLength() * 2 * fv, fullRectScaled.bottom - fh);
	xTxt.p1 = D2D1::Point2F(fullRectScaled.right, fullRectScaled.bottom);

	//float fv2 = fntSize / matPlotRect._11;
	title.p0 = D2D1::Point2F(fullRectScaled.left, fullRectScaled.top);
	title.p1 = D2D1::Point2F(fullRectScaled.right, fullRectScaled.top + fh);
}

void CRGLText::setTile(CRGLTile *_tile)
{
	CRGLLayer::setTile(_tile);
	setCorners();
}

void CRGLText::setXtext(CString _x)
{
	xTxt.txt = _x;
}


void CRGLText::setTexts(CString _x, CString _yRight, CString _yLeft, CString _title)
{
	xTxt.txt = _x;
	yRightTxt.txt = _yRight;
	yLeftTxt.txt = _yLeft;
	title.txt = _title;
	setCorners();
}

CString CRGLText::getTitle(void)
{
	return title.txt;
}

CString CRGLText::getEngineeringUnit(void)
{
	return yLeftTxt.txt;
}

CString CRGLText::getName(void)
{
	CString s = regSection;
	s.MakeReverse();
	s = s.SpanExcluding(_T("\\"));
	s.MakeReverse();
	return s;
}

void CRGLText::launchParameterDialog(void)
{
	/*CTextsPar dlg;
	dlg.setTexts(xTxt.txt,yTxt.txt,title.txt);
	dlg.setPar(tr,tg,tb,ttransparency);
	int ret = (int) dlg.DoModal();
	if (IDOK == ret) {
	dlg.getTexts(&xTxt.txt,&yTxt.txt,&title.txt);
	dlg.getPar(&tr,&tg,&tb,&ttransparency);
	setFullRectMatrices();
	setPlotRectMatrices();

	if (m_brush) m_brush.Release();
	m_brush = NULL;
	HRESULT hr = mp_target->CreateSolidColorBrush(
	D2D1::ColorF((float) tr,(float) tg,(float) tb,
	(float) ttransparency),&m_brush);

	if (parentWnd) parentWnd->Invalidate();
	}*/
}

void CRGLText::DiscardDeviceResources(void)
{
	if (m_brush) m_brush.Release();
	m_brush = NULL;
	if (pTextFormatC) pTextFormatC->Release();
	pTextFormatC = NULL;
	if (pTextFormatL) pTextFormatL->Release();
	pTextFormatL = NULL;
	if (pTextFormatR) pTextFormatR->Release();
	pTextFormatR = NULL;
	if (pDWriteFactory) pDWriteFactory.Release();
	pDWriteFactory = NULL;
}

void CRGLText::CreateDeviceResources(ID2D1HwndRenderTarget *hwndRenderTarget, ID2D1Factory *_factory)
{
	CRGLLayer::CreateDeviceResources(hwndRenderTarget, _factory);
	if (m_brush) m_brush.Release();
	m_brush = NULL;
	HRESULT hr = mp_target->CreateSolidColorBrush(D2D1::ColorF((float)tr, (float)tg, (float)tb, (float)ttransparency), &m_brush);

	hr = DWriteCreateFactory(
		DWRITE_FACTORY_TYPE_SHARED,
		__uuidof(IDWriteFactory),
		reinterpret_cast<IUnknown**>(&pDWriteFactory)
		);

	if (SUCCEEDED(hr)) {
		hr = pDWriteFactory->CreateTextFormat(
			L"Times New Roman", // Font family name.
			NULL,        // Font collection (NULL sets it to use the system font collection).
			DWRITE_FONT_WEIGHT_REGULAR,
			DWRITE_FONT_STYLE_NORMAL,
			DWRITE_FONT_STRETCH_NORMAL,
			fntSize,
			L"en-us",
			&pTextFormatC
			);
	}
	if (SUCCEEDED(hr)) {
		hr = pTextFormatC->SetTextAlignment(DWRITE_TEXT_ALIGNMENT_CENTER);
	}
	if (SUCCEEDED(hr)) {
		hr = pTextFormatC->SetParagraphAlignment(DWRITE_PARAGRAPH_ALIGNMENT_CENTER);
	}

	if (SUCCEEDED(hr)) {
		hr = pDWriteFactory->CreateTextFormat(
			L"Times New Roman", // Font family name.
			NULL,        // Font collection (NULL sets it to use the system font collection).
			DWRITE_FONT_WEIGHT_REGULAR,
			DWRITE_FONT_STYLE_NORMAL,
			DWRITE_FONT_STRETCH_NORMAL,
			fntSize,
			L"en-us",
			&pTextFormatL
			);
	}
	if (SUCCEEDED(hr)) {
		hr = pTextFormatL->SetTextAlignment(DWRITE_TEXT_ALIGNMENT_LEADING);
	}
	if (SUCCEEDED(hr)) {
		hr = pTextFormatL->SetParagraphAlignment(DWRITE_PARAGRAPH_ALIGNMENT_CENTER);
	}
	if (SUCCEEDED(hr)) {
		hr = pDWriteFactory->CreateTextFormat(
			L"Times New Roman", // Font family name.
			NULL,        // Font collection (NULL sets it to use the system font collection).
			DWRITE_FONT_WEIGHT_REGULAR,
			DWRITE_FONT_STYLE_NORMAL,
			DWRITE_FONT_STRETCH_NORMAL,
			fntSize,
			L"en-us",
			&pTextFormatR
			);
	}
	if (SUCCEEDED(hr)) {
		hr = pTextFormatR->SetTextAlignment(DWRITE_TEXT_ALIGNMENT_TRAILING);
	}
	if (SUCCEEDED(hr)) {
		hr = pTextFormatR->SetParagraphAlignment(DWRITE_PARAGRAPH_ALIGNMENT_CENTER);
	}
	setCorners();
}

void CRGLText::render(bool _hideYlabels)
{
	if (!show) return;

	CRGLLayer::render();

	if (!mp_target) return;
	if (!m_brush) return;
	if (!pTextFormatC) return;
	if (!pTextFormatL) return;
	if (!pTextFormatR) return;

	//---Labels
	D2D1_POINT_2F p0, p1;
	D2D1_RECT_F r;

	p0 = xTxt.p0 * matFullRect;
	p1 = xTxt.p1 * matFullRect;
	r = D2D1::RectF(p0.x, p0.y, p1.x, p1.y);
	mp_target->DrawText(xTxt.txt, xTxt.txt.GetLength(), pTextFormatR, r, m_brush);

	p0 = yLeftTxt.p0 * matFullRect;
	p1 = yLeftTxt.p1 * matFullRect;
	r = D2D1::RectF(p0.x, p0.y, p1.x, p1.y);
	if (!_hideYlabels)
		mp_target->DrawText(yLeftTxt.txt, yLeftTxt.txt.GetLength(), pTextFormatL, r, m_brush);

	p0 = yRightTxt.p0 * matFullRect;
	p1 = yRightTxt.p1 * matFullRect;
	r = D2D1::RectF(p0.x, p0.y, p1.x, p1.y);
	if (!_hideYlabels) 
		mp_target->DrawText(yRightTxt.txt, yRightTxt.txt.GetLength(), pTextFormatL, r, m_brush);

	p0 = title.p0 * matFullRect;
	p1 = title.p1 * matFullRect;
	r = D2D1::RectF(p0.x, p0.y, p1.x, p1.y);
	mp_target->DrawText(title.txt, title.txt.GetLength(), pTextFormatC, r, m_brush);
}

/////////////////////////////////////////////

CGraphButton::CGraphButton(CString _regSection /* = _T("Untitled graph button")*/) :
	YshiftIsAllowed(false), YscaleIsAllowed(true), regSection(_regSection)
{
	eventsToShow = 0;
	debugTag = 0;
	timerID = 0;
	wheelRunning = false;
	wheelTimerCounter = 0;
	//pickWhat = OVER_NOTHING;
	//pickWhere = OVER_NOTHING;
	regSubKey = GENERIC_GRAPH_SUBKEY;
	dualPolarity = false;
	doneInitIndependentResources = false;
	doneInitDependentResources = false;
	m_target = 0;
	leftBtnIsDown = false;
	defaultCursor = CURS_ARROW;
	currentCursor = defaultCursor;
	beginZoom = false;
	enableEditableWindowMouseControl = false;
	engineeringUnit = _T("");
	hoverIsIndicated = false;
	hideYlabels = false;

	drawOnlyEvents = false;
	visualWholeEventMoving = false;
	visualLeftEdgeMoving = visualRightEdgeMoving = false;
	hoverIndication = false;
	visualGrabDistToLeft = visualGrabDistToRight = .0f;
	visualEventMovingType = evntTypeNone;
	visualEventMovingHasThisLevel = levelTypeUndef;
	
	statisticsWindowEdge0 = .0f;
	statisticsWindowEdge1 = .0f;

	eventsP = NULL;
	vArrow = NULL;

	eventEditWindow.begin = .0f;
	eventEditWindow.end = .0f;

	//sFrom.LoadString(IDS_FROM);
	//sTo.LoadString(IDS_TO);
	//sLength.LoadString(IDS_LENGTH2);

	eventPointedTo = NULL;
	edgePointedTo = closestIsLeft;

	pDWriteFactory = NULL;
	pTextFormat = NULL;

	layerTypeToEdit = 0;
	layerToEdit = NULL;

	clientRect.bottom = 100;
	clientRect.top = 0;
	clientRect.left = 0;
	clientRect.top = 100;

	leftPoint = CPoint(0, 0);
	rightPoint = CPoint(0, 0);

	cursorControl = cursorControlNone;
	lockTopOfYAxis = false;
	
	noCursor = AfxGetApp()->LoadStandardCursor(IDC_NO);

	messDisableEventHoverIndication = RegisterWindowMessage(AGS_MESSAGE_DISABLE_EVENT_HOVER_INDICATION);
	messMoveX = RegisterWindowMessage(AGS_MESSAGE_MOVE_X);
	messCentreX = RegisterWindowMessage(AGS_MESSAGE_CENTRE_X);
	messMovingX = RegisterWindowMessage(AGS_MESSAGE_MOVING_X);
	messMoveY = RegisterWindowMessage(AGS_MESSAGE_MOVE_Y);
	messScaleX = RegisterWindowMessage(AGS_MESSAGE_SCALE_X);
	messScaleY = RegisterWindowMessage(AGS_MESSAGE_SCALE_Y);
	messMoveCursorWindow = RegisterWindowMessage(AGS_MESSAGE_MOVE_CURSOR_WINDOW);
	messSetLeftEditWindow = RegisterWindowMessage(AGS_MESSAGE_SET_LEFT_EDIT_WINDOW);
	messSetRightEditWindow = RegisterWindowMessage(AGS_MESSAGE_SET_RIGHT_EDIT_WINDOW);
	//messPointAndClickAddEvent = RegisterWindowMessage(AGS_MESSAGE_POINT_AND_CLICK_ADD_EVENT);
	
	VERIFY(ttFont.CreateFont(
		8,                        // nHeight
		0,                         // nWidth
		0,                         // nEscapement
		0,                         // nOrientation
		FW_NORMAL,                 // nWeight
		FALSE,                     // bItalic
		FALSE,                     // bUnderline
		0,                         // cStrikeOut
		ANSI_CHARSET,              // nCharSet
		OUT_DEFAULT_PRECIS,        // nOutPrecision
		CLIP_DEFAULT_PRECIS,       // nClipPrecision
		DEFAULT_QUALITY,           // nQuality
		DEFAULT_PITCH | FF_SWISS,  // nPitchAndFamily
		_T("Courier")));                 // lpszFacename


	if (!doneInitIndependentResources) {
		HRESULT res = CreateDeviceIndependentResources();
		if (SUCCEEDED(res)) doneInitIndependentResources = true;
	}
}

CGraphButton::~CGraphButton()
{
	cleanUp();
	ttFont.DeleteObject();
}

/*
Description: Check for data
Returns: true if data are present, false if not
*/
bool CGraphButton::getHasData(void)
{
	return false;
}

void CGraphButton::setEditableWindowMouseControl(BOOL _on, HCURSOR _cur)
{
	enableEditableWindowMouseControl = _on;
	vArrow = _cur;
}

void CGraphButton::cleanUp(void)
{
	int numT = (int)tiles.GetCount();
	for (int i = numT - 1; i >= 0; i--) {
		CRGLTile *tP = tiles.GetAt(i);
		delete tP;
		tiles.RemoveAt(i);
	}

	int numL = (int)layers.GetCount();
	for (int i = (numL - 1); i >= 0; i--) {
		CRGLLayer *lP = layers.GetAt(i);
		deleteLayer(lP);
		layers.RemoveAt(i);
	}
}

CRGLTile *CGraphButton::addTile(CRGLTile *_tile)
{
	tiles.Add(_tile);
	_tile->setLockTop(lockTopOfYAxis);
	return _tile;
}

void CGraphButton::doGraphGeometries(void)
{
	int num = (int)layers.GetCount();
	for (int i = 0 ; i < num ; i++) {
		CRGLLayer *lay = layers.GetAt(i);
		if (RGLLayerTypeGraph == lay->getLayerType())
			((CRGLGraph *)lay)->doGeometry();
	}
}

CRGLLayer *CGraphButton::addLayer(CRGLLayer *_layer)
{
#ifdef _DEBUG
	int num = (int)layers.GetCount();
	if (0 == num) {
		int type = _layer->getLayerType();
		if (RGLLayerTypeBackground != type)
			AfxMessageBox(_T("Add the background layer first, then in the sequence of rendering"));
	}
#endif
	_layer->setParent(this);
	layers.Add(_layer);

	//---Create layer device resource
	int type = _layer->getLayerType();
	switch (type) {
	case RGLLayerTypeGeneric:
		((CRGLLayer *)_layer)->CreateDeviceResources(m_target, m_factory);
		break;
	case RGLLayerTypeGraphSparse:
		((CRGLGraphSparse *)_layer)->CreateDeviceResources(m_target, m_factory);
		break;
	case RGLLayerTypeGraph:
		((CRGLGraph *)_layer)->CreateDeviceResources(m_target, m_factory);
		break;
	case RGLLayerTypeGraphFilled:
		((CRGLGraphFilled *)_layer)->CreateDeviceResources(m_target, m_factory);
		break;
	case RGLLayerTypeGraphPoints:
		((CRGLGraphEventMarkers *)_layer)->CreateDeviceResources(m_target, m_factory);
		break;
	case RGLLayerTypeBackground:
		((CRGLBackground *)_layer)->CreateDeviceResources(m_target, m_factory);
		break;
	case RGLLayerTypeAxes:
		((CRGLAxes *)_layer)->CreateDeviceResources(m_target, m_factory);
		break;
	case RGLLayerTypeGrid:
		((CRGLGrid *)_layer)->CreateDeviceResources(m_target, m_factory);
		break;
	case RGLLayerTypeText:
		((CRGLText *)_layer)->CreateDeviceResources(m_target, m_factory);
		engineeringUnit = ((CRGLText *)_layer)->getEngineeringUnit();
		break;
	case RGLLayerTypeBar:
		((CRGLBar *)_layer)->CreateDeviceResources(m_target, m_factory);
		break;
	}

	return _layer;
}

BEGIN_MESSAGE_MAP(CGraphButton, CXTPButton)
	ON_WM_SIZE()
	ON_WM_PAINT()
	ON_WM_DESTROY()
	ON_WM_MOUSEMOVE()
	ON_WM_LBUTTONDOWN()
	ON_WM_RBUTTONDOWN()
	ON_WM_LBUTTONUP()
	ON_WM_RBUTTONUP()
	ON_WM_MBUTTONDOWN()
	ON_WM_MBUTTONUP()
	ON_WM_SETCURSOR()
	ON_WM_MOUSELEAVE()
	ON_WM_LBUTTONDBLCLK()
	ON_WM_CREATE()
	ON_WM_RBUTTONDBLCLK()

	ON_WM_NCHITTEST()
	ON_WM_CONTEXTMENU()

	ON_COMMAND(ID_DEF_MIXED, &CGraphButton::onDefMixed)
	ON_COMMAND(ID_DEF_UPPER_MIX, &CGraphButton::onDefMixedUpper)
	ON_COMMAND(ID_DEF_LOWER_MIX, &CGraphButton::onDefMixedLower)
	ON_COMMAND(ID_DEF_MULTI_MIX, &CGraphButton::onDefMixedMulti)
	ON_COMMAND(ID_DEF_UNDEF_MIX, &CGraphButton::onDefMixedUndef)
	ON_COMMAND(ID_DEF_CENTRAL, &CGraphButton::onDefCentral)
	ON_COMMAND(ID_DEF_OBSTRUCTIVE, &CGraphButton::onDefObstructive)
	ON_COMMAND(ID_DEF_HYPOPNEA, &CGraphButton::onDefHypopnea)
	ON_COMMAND(ID_DEF_OBSTRHYPOPNEA, &CGraphButton::onDefObstrHypopnea)
	ON_COMMAND(ID_DEF_CENTRALHYPOPNEA, &CGraphButton::onDefCentralHypopnea)
	ON_COMMAND(ID_DEF_SPO2DROP, &CGraphButton::onDefSpO2Drop)

	ON_COMMAND(ID_DEF_UPPER_OBSTR, &CGraphButton::onDefObstrUpper)
	ON_COMMAND(ID_DEF_LOWER_OBSTR, &CGraphButton::onDefObstrLower)
	ON_COMMAND(ID_DEF_MULTI_OBSTR, &CGraphButton::onDefObstrMulti)
	ON_COMMAND(ID_DEF_UNDEF_OBSTR, &CGraphButton::onDefObstrUndef)
	ON_COMMAND(ID_DEF_UPPER_HYPO, &CGraphButton::onDefHypoUpper)
	ON_COMMAND(ID_DEF_LOWER_HYPO, &CGraphButton::onDefHypoLower)
	ON_COMMAND(ID_DEF_MULTI_HYPO, &CGraphButton::onDefHypoMulti)
	ON_COMMAND(ID_DEF_UNDEF_HYPO, &CGraphButton::onDefHypoUndef)
	ON_COMMAND(ID_DEF_UPPER_OBSTRHYPO, &CGraphButton::onDefObstrHypoUpper)
	ON_COMMAND(ID_DEF_LOWER_OBSTRHYPO, &CGraphButton::onDefObstrHypoLower)
	ON_COMMAND(ID_DEF_MULTI_OBSTRHYPO, &CGraphButton::onDefObstrHypoMulti)
	ON_COMMAND(ID_DEF_UNDEF_OBSTRHYPO, &CGraphButton::onDefObstrHypoUndef)
	ON_COMMAND(ID_DEF_CLEAR_EVENT, &CGraphButton::onClearEvent)

	ON_WM_MOUSEWHEEL()
	ON_WM_TIMER()
END_MESSAGE_MAP()

void CGraphButton::onClearEvent()
{
	if (eventPointedTo) {
		changeAnEvent(eventPointedTo, ID_DEF_CLEAR_EVENT);
		//---Signal to all that an events has changed
		::PostMessage(GetParent()->m_hWnd, WM_CHANGE_ONE_EVENT, 0L, 0L);
	}
}

void CGraphButton::onDefCentral()
{
}

void CGraphButton::onDefObstructive()
{
}

void CGraphButton::onDefHypopnea()
{
}

void CGraphButton::onDefObstrHypopnea()
{
}

void CGraphButton::onDefCentralHypopnea()
{
}

void CGraphButton::onDefSpO2Drop()
{
}

void CGraphButton::onDefObstrUpper()
{
}

void CGraphButton::onDefObstrLower()
{
}

void CGraphButton::onDefObstrMulti()
{
}

void CGraphButton::onDefObstrUndef()
{
}

void CGraphButton::onDefHypoUpper()
{
}

void CGraphButton::onDefHypoLower()
{
}

void CGraphButton::onDefHypoMulti()
{
}

void CGraphButton::onDefHypoUndef()
{
}

void CGraphButton::onDefObstrHypoUpper()
{
}

void CGraphButton::onDefObstrHypoLower()
{
}

void CGraphButton::onDefObstrHypoMulti()
{
}

void CGraphButton::onDefObstrHypoUndef()
{
}

void CGraphButton::onDefMixed()
{
}

void CGraphButton::onDefMixedUpper()
{
}

void CGraphButton::onDefMixedLower()
{
}

void CGraphButton::onDefMixedMulti()
{
}

void CGraphButton::onDefMixedUndef()
{
}

void CGraphButton::OnDestroy()
{
	CXTPButton::OnDestroy();

	// TODO: Add your message handler code here

	if (timerID) KillTimer(timerID);
	timerID = 0;

	cleanUp();
	DiscardDeviceResources();
}

void CGraphButton::deleteLayer(CRGLLayer *_layerP)
{
	int type = _layerP->getLayerType();
	switch (type) {
	case RGLLayerTypeGeneric:
		delete _layerP;
		break;
	case RGLLayerTypeGraph:
		delete (CRGLGraph *)_layerP;
		break;
	case RGLLayerTypeGraphSparse:
		delete (CRGLGraphSparse *)_layerP;
		break;
	case RGLLayerTypeGraphFilled:
		delete (CRGLGraphFilled *)_layerP;
		break;
	case RGLLayerTypeGraphPoints:
		delete (CRGLGraphEventMarkers *)_layerP;
		break;
	case RGLLayerTypeBackground:
		delete (CRGLBackground *)_layerP;
		break;
	case RGLLayerTypeAxes:
		delete (CRGLAxes *)_layerP;
		break;
	case RGLLayerTypeGrid:
		delete (CRGLGrid *)_layerP;
		break;
	case RGLLayerTypeText:
		delete (CRGLText *)_layerP;
		break;
	case RGLLayerTypeBar:
		delete (CRGLBar *)_layerP;
		break;
	}
}


void CGraphButton::renderLayers(bool _disableHoverIndication /* = false*/)
{
	//---If general message is to remove hoverIndication, but this button has hover indication set - just return
	if (_disableHoverIndication && hoverIndication) 
		return;

	//---If general message is to remove hoverIndication, but this button has not rendered any hover, so this is not required
	if (_disableHoverIndication && !hoverIsIndicated)
		return;

	if (!m_target) return;

	int num = (int)layers.GetCount();
	for (int i = 0; i < num; i++) {
		CRGLLayer *lP = layers.GetAt(i);
		int type = lP->getLayerType();
		switch (type) {
		case RGLLayerTypeGeneric:
			((CRGLLayer *)lP)->render();
			break;
		case RGLLayerTypeGraph:
			((CRGLGraph *)lP)->render();
			break;
		case RGLLayerTypeGraphSparse:
			((CRGLGraphSparse *)lP)->render();
			break;
		case RGLLayerTypeGraphFilled:
			((CRGLGraphFilled *)lP)->render();
			break;
		case RGLLayerTypeGraphPoints:
			((CRGLGraphEventMarkers *)lP)->render();
			break;
		case RGLLayerTypeBackground:
			((CRGLBackground *)lP)->render();
			break;
		case RGLLayerTypeAxes:
			((CRGLAxes *)lP)->render(hideYlabels);
			if ((visualWholeEventMoving || visualLeftEdgeMoving || visualRightEdgeMoving) || hoverIndication) {
				((CRGLAxes *)lP)->renderEditRect(eventEditWindow.begin, eventEditWindow.end);
				hoverIsIndicated = true;
			}
			else hoverIsIndicated = false;
			break;
		case RGLLayerTypeGrid:
			((CRGLGrid *)lP)->render();
			break;
		case RGLLayerTypeText:
			((CRGLText *)lP)->render(hideYlabels);
			break;
		case RGLLayerTypeBar:
			((CRGLBar *)lP)->render();
			break;
		}
	}

	//---Render events at last
	for (int i = 0; i < num; i++) {
		CRGLLayer *lP = layers.GetAt(i);
		int type = lP->getLayerType();
		bool doneRender = false;
		switch (type) {
		case RGLLayerTypeGraph:
			((CRGLGraph *)lP)->renderEvents(true);
			doneRender = true;
			break;
		case RGLLayerTypeGraphSparse:
			((CRGLGraphSparse *)lP)->renderEvents(true);
			doneRender = true;
			break;
		case RGLLayerTypeGraphFilled:
			((CRGLGraphFilled *)lP)->renderEvents(true);
			doneRender = true;
			break;
		}
		if (doneRender) break;
	}

	//---Tile changes will now have been handled, make sure all tile change flags are reset
	for (int i = 0; i < num; i++) {
		CRGLLayer *lP = layers.GetAt(i);
		int type = lP->getLayerType();
		switch (type) {
		case RGLLayerTypeGeneric:
			((CRGLLayer *)lP)->resetTileChangeFlag();
			break;
		case RGLLayerTypeGraph:
			((CRGLGraph *)lP)->resetTileChangeFlag();
			break;
		case RGLLayerTypeGraphSparse:
			((CRGLGraphSparse *)lP)->resetTileChangeFlag();
			break;
		case RGLLayerTypeGraphFilled:
			((CRGLGraphFilled *)lP)->resetTileChangeFlag();
			break;
		case RGLLayerTypeGraphPoints:
			((CRGLGraphEventMarkers *)lP)->resetTileChangeFlag();
			break;
		case RGLLayerTypeBackground:
			((CRGLBackground *)lP)->resetTileChangeFlag();
			break;
		case RGLLayerTypeAxes:
			((CRGLAxes *)lP)->resetTileChangeFlag();
			break;
		case RGLLayerTypeGrid:
			((CRGLGrid *)lP)->resetTileChangeFlag();
			break;
		case RGLLayerTypeText:
			((CRGLText *)lP)->resetTileChangeFlag();
			break;
		case RGLLayerTypeBar:
			((CRGLBar *)lP)->resetTileChangeFlag();
			break;
		default:
			break;
		}
	}
}

void CGraphButton::renderEvents(void)
{
	if (!m_target) return;

	int num = (int)layers.GetCount();
	for (int i = 0; i < num; i++) {
		CRGLLayer *lP = layers.GetAt(i);
		int type = lP->getLayerType();
		switch (type) {
		case RGLLayerTypeGraph:
			((CRGLGraph *)lP)->renderEvents();
			break;
		case RGLLayerTypeGraphSparse:
			((CRGLGraphSparse *)lP)->renderEvents();
			break;
		default:
			break;
		}
	}
}

void CGraphButton::setCursorControl(int _cursorControl)
{
	cursorControl = _cursorControl;
}

void CGraphButton::resizeLayers(void)
{
	if (!m_target) return;

	int num = (int)layers.GetCount();
	for (int i = 0; i < num; i++) {
		CRGLLayer *lP = layers.GetAt(i);
		int type = lP->getLayerType();
		switch (type) {
		case RGLLayerTypeGeneric:
			((CRGLLayer *)lP)->resize();
			break;
		case RGLLayerTypeGraph:
			((CRGLGraph *)lP)->resize();
			break;
		case RGLLayerTypeGraphSparse:
			((CRGLGraphSparse *)lP)->resize();
			break;
		case RGLLayerTypeGraphFilled:
			((CRGLGraphFilled *)lP)->resize();
			break;
		case RGLLayerTypeGraphPoints:
			((CRGLGraphEventMarkers *)lP)->resize();
			break;
		case RGLLayerTypeBackground:
			((CRGLBackground *)lP)->resize();
			break;
		case RGLLayerTypeAxes:
			((CRGLAxes *)lP)->resize();
			break;
		case RGLLayerTypeGrid:
			((CRGLGrid *)lP)->resize();
			break;
		case RGLLayerTypeText:
			((CRGLText *)lP)->resize();
			break;
		case RGLLayerTypeBar:
			((CRGLBar *)lP)->resize();
			break;
		}
	}
}

CRGLGraph *CGraphButton::getGraph(void)
{
	int num = (int)layers.GetCount();
	for (int i = 0; i < num; i++) {
		CRGLLayer *lP = layers.GetAt(i);
		if (RGLLayerTypeGraph == lP->getLayerType()) return ((CRGLGraph *)lP);
		if (RGLLayerTypeGraphSparse == lP->getLayerType()) return ((CRGLGraphSparse *)lP);
		if (RGLLayerTypeGraphFilled == lP->getLayerType()) return ((CRGLGraph *)lP);
		if (RGLLayerTypeGraphPoints == lP->getLayerType()) return ((CRGLGraph *)lP);
	}
	return NULL;
}

bool CGraphButton::getTimeCursorLeftInfo(CString *_s)
{
	CRGLGraph *gP = getGraph();
	if (!gP) return false;
	*_s = gP->getTimeCursorLeftInfo();
	return true;
}

bool CGraphButton::getTimeCursorRightInfo(CString *_s)
{
	CRGLGraph *gP = getGraph();
	if (!gP) return false;
	*_s = gP->getTimeCursorRightInfo();
	return true;
}

bool CGraphButton::getScreenXfromTime(FLOAT _time,int *_x)
{
	CRGLGraph *gP = getGraph();
	if (!gP) return false;
	*_x = gP->getScreenXfromTime(_time);
	return true;
}

void CGraphButton::setEvents(CEvents *_eP)
{
	eventsP = _eP;
}

void CGraphButton::setCursorWindow(float _start, float _stop)
{
	CRGLGraph *gP = getGraph();
	if (!gP) return;
	gP->setCursorWindow(_start, _stop);
}

BOOL CGraphButton::PreTranslateMessage(MSG * pMsg)
{
	if (WM_KEYDOWN == pMsg->message) {
		int x = (int)pMsg->wParam;
		switch (x) {
		case 'U':
		case 'u':
		case 'L':
		case 'l':
		case 'm':
		case 'M':
		case 'a':
		case 'A':
		case '1':
		case '2':
		case '3':
		case '4':
		case '5':
		case ' ':
			return TRUE;
			break;
		default:
			break;
		}
	}
	return 0;
}

void CGraphButton::setDualPolarity(bool _on)
{
	dualPolarity = _on;
}

void CGraphButton::disableEventHoverIndication(void)
{
	render(false, true); // Not only events, disableHoiverIndication
}

bool CGraphButton::getTypeFromKey(UINT *_type)
{
	if (0x80 & GetKeyState('1')) {
		*_type = evntTypeMixed;
		return true;
	}
	else if (0x80 & GetKeyState('2')) {
		*_type = evntTypeCentral;
		return true;
	}
	else if (0x80 & GetKeyState('3')) {
		*_type = evntTypeObstr;
		return true;
	}
	else if (0x80 & GetKeyState('4')) {
		*_type = evntTypeHypoObstr;
		return true;
	}
	else if (0x80 & GetKeyState('5')) {
		*_type = evntTypeHypoCentral;
		return true;
	}
	else *_type = evntTypeNone;

	return false;
}


bool CGraphButton::getLevelFromKey(UINT *_level,bool *_findFromGradient)
{	
	*_findFromGradient = false;

 	if (0x80 & GetKeyState('L')) {
		*_level = levelTypeLower;
		return true;
	}
	else if (0x80 & GetKeyState('M')) {
		*_level = levelTypeMulti;
		return true;
	}
	else if (0x80 & GetKeyState('U')) {
		*_level = levelTypeUpper;
		return true;
	}
	else if (0x80 & GetKeyState('A')) {
		*_level = levelTypeUndef;
		*_findFromGradient = true;
		return true;
	}
	else if (0x80 & GetKeyState(VK_SPACE)) {
		*_level = levelTypeUndef;
		return true;
	}
	return false;
}

void CGraphButton::setMMarkerStartStopVector(vector<FLOAT>* _v)
{
	CRGLGraph *gP = getGraph();
	if (!gP) return;
	gP->setMMarkerStartStopVector(_v);
}

bool CGraphButton::getTimeCursorLengthInfo(CString *_s)
{
	CRGLGraph *gP = getGraph();
	if (!gP) return false;
	*_s = gP->getTimeCursorLengthInfo();
	return true;
}

bool CGraphButton::getAmplAt(LONG _x, LONG _y, CString *_s)
{
	CRGLGraph *gP = getGraph();
	if (!gP) return false;
	*_s =  gP->getAmplAt(_x, _y);
	return true;
}

/*
Description: Get amplitude in engineering units
*/
bool CGraphButton::getEUAmplAt(LONG _x, LONG _y, CString *_s)
{
	CRGLGraph *gP = getGraph();
	if (!gP) return false;
	*_s = gP->getAmplAt(_x, _y);
	*_s += _T(" ");
	*_s += engineeringUnit;
	return true;
}

bool CGraphButton::getTimeAndAmplAt(LONG _x, LONG _y,CString *_s,bool _lineBreak)
{
	CRGLGraph *gP = getGraph();
	if (!gP) return false;
	*_s = gP->getStringTimeAndAmplAt(_x, _y, _T(""),_lineBreak);
	return true;
}

/*
Description: Returns a formatted time string
_x: x in screen coordinates
_y: y in screen coordinates
_s: The string
Return: true if there is a graph, false if not
*/
bool CGraphButton::getTimeAt(LONG _x, LONG _y, CString *_s)
{
	CString s;
	CRGLGraph *gP = getGraph();
	if (!gP) return false;
	*_s = gP->getStringTimeAt(_x, _y);
	return true;
}

/*
Description: Returns time in seconds
_x: x in screen coordinates
_y: y in screen coordinates
_time: the time in seconds
Return: true if there is a graph to ask, false if not
*/
bool CGraphButton::getTimeValAt(LONG _x, LONG _y,FLOAT *_time)
{
	CString s;
	CRGLGraph *gP = getGraph();
	if (!gP) return false;

	*_time =  gP->getTimeValAt(_x, _y);
	return true;
}

CEvnt *CGraphButton::findEvent(int _what, FLOAT _atTime, int *_closestEdge)
{
	CRGLGraph *gP = getGraph();
	if (!gP) return NULL;

	return gP->findEvent(_what, _atTime, _closestEdge);
}

/*
Description: Based on the flag return from getIsOver(), returns the event description
*/
CString CGraphButton::getEventLevelDescription(int _what)
{
	CString s, sl, ot, sll, slll;

	if (!eventsP) return s;
	
	if (eventsP->getAreEventsRespBeltBased()) return s;

	if (_what & OVER_EVNT_CENTRAL) {		// Nothing to add
		return s;
	}
	if (_what & OVER_EVNT_SPO2_DROP) {		// Nothing to add
		return s;
	}
	if (_what & OVER_EVNT_SNORING) {
		int num = s.LoadString(IDS_LEVEL2);
		s += _T(": ");
		if (_what & OVER_UPPER) num = sl.LoadString(IDS_UPPER2);
		else if (_what & OVER_LOWER) num = sl.LoadString(IDS_LOWER2);
		else if (_what & OVER_MULTI) num = sl.LoadString(IDS_MULTILEVEL2);
		else if (_what & OVER_UNDEFINED_LEVEL) num = sl.LoadString(IDS_UNDEF2);
		s += sl;
		return s;
	}
	if (_what & OVER_EVNT_MIXED) {
		int num = s.LoadString(IDS_LEVEL2);
		s += _T(": ");
		if (_what & OVER_UPPER)			num = sl.LoadString(IDS_UPPER2);
		else if (_what & OVER_LOWER)	num = sl.LoadString(IDS_LOWER2);
		else if (_what & OVER_MULTI)	num = sl.LoadString(IDS_MULTILEVEL2);
		else if (_what & OVER_UNDEFINED_LEVEL) num = sl.LoadString(IDS_UNDEF2);
		s += sl;
		return s;
	}
	if (_what & OVER_EVNT_OBSTRUCTIVE) {
		int num = s.LoadString(IDS_LEVEL2);
		s += _T(": ");
		if (_what & OVER_UPPER)					num = sl.LoadString(IDS_UPPER2);
		else if (_what & OVER_LOWER)			num = sl.LoadString(IDS_LOWER2);
		else if (_what & OVER_MULTI)			num = sl.LoadString(IDS_MULTILEVEL2);
		else if (_what & OVER_UNDEFINED_LEVEL)	num = sl.LoadString(IDS_UNDEF2);
		s += sl;
		return s;
	}
	if (_what & OVER_EVNT_RERA) {
		int num = s.LoadString(IDS_LEVEL2);
		s += _T(": ");
		if (_what & OVER_UPPER)					num = sl.LoadString(IDS_UPPER2);
		else if (_what & OVER_LOWER)			num = sl.LoadString(IDS_LOWER2);
		else if (_what & OVER_MULTI)			num = sl.LoadString(IDS_MULTILEVEL2);
		else if (_what & OVER_UNDEFINED_LEVEL)	num = sl.LoadString(IDS_UNDEF2);
		s += sl;
		return s;
	}
	if (_what & OVER_EVNT_AROUSAL) {  // Nothing to add
		return s;
	}
	if (_what & OVER_EVNT_HYPOPNEA) {
		if (_what & OVER_EVNT_OBSTRUCTIVEHYPO) {
			int num = sll.LoadString(IDS_LEVEL2);
			s += sll;
			s += _T(": ");
			if (_what & OVER_UPPER)					num = sl.LoadString(IDS_UPPER2);
			else if (_what & OVER_LOWER)			num = sl.LoadString(IDS_LOWER2);
			else if (_what & OVER_MULTI)			num = sl.LoadString(IDS_MULTILEVEL2);
			else if (_what & OVER_UNDEFINED_LEVEL)	num = sl.LoadString(IDS_UNDEF2);
			s += sl;
		}
		return s;
	}
	return s;
}

/*
Description: Based on the flag return from getIsOver(), returns the ID of the string
*/
int CGraphButton::getHypoTypeName(int _what)
{
	if (_what & OVER_EVNT_OBSTRUCTIVEHYPO) return IDS_OBSTRUCTIVE3;
	if (_what & OVER_EVNT_CENTRALHYPO) return IDS_CENTRAL3;
	return IDS_UNDEF2;
}

/*
Description: Based on the flag return from getIsOver(), returns the ID of the string
*/
int CGraphButton::getEventName(int _what)
{
	if (_what & OVER_EVNT_CENTRAL) return IDS_CENTRAL2;
	if (_what & OVER_EVNT_OBSTRUCTIVEHYPO) return IDS_OBSTRHYPOPNEA2;
	if (_what & OVER_EVNT_CENTRALHYPO) return IDS_CENTRALHYPOPNEA2;
	if (_what & OVER_EVNT_HYPOPNEA) return IDS_HYPOPNEA2;
	if (_what & OVER_EVNT_MIXED) return IDS_MIXED2;
	if (_what & OVER_EVNT_SPO2_DROP) return IDS_SpO2DROP2;
	if (_what & OVER_EVNT_RERA) return IDS_RERA2;
	if (_what & OVER_EVNT_AROUSAL) return IDS_AROUSAL2;
	if (_what & OVER_EVNT_SNORING) return IDS_SNORING2;
	if (_what & OVER_EVNT_AWAKE) return IDS_AWAKE2;
	if (_what & OVER_EVNT_EXCLUDED) return IDS_EXCLUDED2;
	if (_what & OVER_EVNT_SWALLOW) return IDS_SWALLOW2;

	if (_what & OVER_EVNT_OBSTRUCTIVE) return IDS_OBSTRUCTIVE2;

	return IDS_UNDEF2;
}

void CGraphButton::RelayEventAndSetCursor(UINT message, WPARAM wParam, LPARAM lParam, CString _amplInfo)
//****************************************************************************************************
{
#ifdef IN_SPIRO_ANALYSIS

	if (!eventsP) return;

	if (!m_ttip.m_hWnd) return;
	MSG msg;
	msg.hwnd = m_hWnd;
	msg.message = message;
	msg.wParam = wParam;
	msg.lParam = lParam;
	msg.time = 0;
	msg.pt.x = LOWORD(lParam);
	msg.pt.y = HIWORD(lParam);

	eventPointedTo = NULL;

	int what, where1;
	what = where1 = 0;
	CString s, st, fs0;
	CRect rect;
	GetClientRect(&rect);
	FLOAT theTime = .0f;
	bool ok = getTimeValAt(msg.pt.x, msg.pt.y,&theTime);

	CEvnt *evP = eventsP->getIsOnEvent(eventsToShow, theTime, EDGE_DETECT_TOLERANCE, &what, &where1);
	int nums;
	if (evP) {
		
		if (where1 & OVER_EVNT) {
			CString std;

			eventPointedTo = evP;
			nums = st.LoadString(getEventName(what));

			//---Heading
			s += getEventLevelDescription(what);
			s += _T("\n");
			CString fromS, toS,lenS;
			nums = lenS.LoadString(IDS_LENGTH2S);
			nums = fromS.LoadString(IDS_FROM);
			nums = toS.LoadStringW(IDS_TO);
			s += _T("               ");
			s += fromS;
			s += _T("      ");
			s += toS;
			s += _T("     ");
			s += lenS;
			s += _T("\n");

			nums = std.LoadString(IDS_TIME_OF_DAY);
			std += _T(" ");
			std += evP->getTimeOfDayStart();
			std += _T("  ");
			std += evP->getTimeOfDayStop();
			s += std;
			s += _T("\n");

			nums = std.LoadString(IDS_REC_TIME2);
			std += _T("    ");
			std += CRGLGraph::gethms(evP->getFrom());
			std += _T("  ");
			std += CRGLGraph::gethms(evP->getTo());
			std += _T("  ");
			std += CRGLGraph::getsecs(evP->getLength());
			s += std;
			s += _T("\n\n");

			//---More info
			CRGLGraph *gP = getGraph();
			if (gP) {
				float timeSec = gP->getTheTimeAtPixel(msg.pt.x);
				s += eventsP->getEventsList(timeSec);
				CString rcS;
				nums = rcS.LoadString(IDS_REC_TIME2);
				s += rcS;
				s += _T("  ");
				CString s0;
				bool ok = getTimeAt(msg.pt.x, msg.pt.y,&s0);
				if (ok) s += s0;

				if (_amplInfo != DONT_NEED_AMPL) {
					s += _T("\n");
					nums = rcS.LoadString(IDS_AMPLITUDE);
					s += rcS;
					s += _T(" ");

					ok = getAmplAt(msg.pt.x, msg.pt.y, &s0);
					if (ok) s += s0;
				}
			}
			m_ttip.SetTitle(TTI_INFO, st);
			m_ttip.UpdateTipText(s, this);
			m_ttip.RelayEvent(&msg);
			TRACE(_T("Over event---------------\n"));
			return;
		}
		else currentCursor = CURS_ARROW;
	}

	CRGLGraph *gP = getGraph();
	if (gP) {
		float timeSec = gP->getTheTimeAtPixel(msg.pt.x);
		CString ttTitle;
		nums = ttTitle.LoadString(IDS_TOD);
		ttTitle += _T(": ");
		ttTitle += eventsP->getTimeOfDayAt(timeSec);
		m_ttip.SetTitle(TTI_INFO, ttTitle);

		s = eventsP->getEventsList(timeSec);
		s += _T("\n");
		CString rcS;
		nums = rcS.LoadString(IDS_REC_TIME2);
		s += rcS;
		s += _T("  ");
		CString s0;
		bool ok = getTimeAt(msg.pt.x, msg.pt.y,&s0);
			if (ok) s += s0;

		if (_amplInfo != DONT_NEED_AMPL) {
			s += _T("\n");
			//rcS.LoadString(IDS_AMPLITUDE);
			//s += rcS;
			//s += _T(" ");

			ok = getEUAmplAt(msg.pt.x, msg.pt.y, &s0);
			if (ok) s += s0;
		}

		currentCursor = CURS_ARROW;
		m_ttip.UpdateTipText(s, this);
		m_ttip.RelayEvent(&msg);
	}

#endif
}

void CGraphButton::RelayEventAndSetCursorDuringVisualEventEditing(UINT message, WPARAM wParam, LPARAM lParam, CString _amplInfo)
{
#ifdef IN_SPIRO_ANALYSIS

	if (!eventsP) return;

	if (!m_ttip.m_hWnd) return;
	MSG msg;
	msg.hwnd = m_hWnd;
	msg.message = message;
	msg.wParam = wParam;
	msg.lParam = lParam;
	msg.time = 0;
	msg.pt.x = LOWORD(lParam);
	msg.pt.y = HIWORD(lParam);

	int what, where1;
	what = where1 = 0;
	CString s, st, fs0;
	CRect rect;
	GetClientRect(&rect);
	FLOAT theTime = .0f;
	bool ok = getTimeValAt(msg.pt.x, msg.pt.y, &theTime);

	float startEvent = .0f;
	float stopEvent = .0f;
	CString evTxt,lvl;
	bool hit = eventsP->getIsOnEventDuringVisualEventEditing(eventsToShow,theTime, &evTxt,&lvl,&startEvent, &stopEvent);
	if (hit) {

		CString std;

		//---Heading
		CString fromS, toS, lenS;
		int nums = lenS.LoadString(IDS_LENGTH2S);
		nums = fromS.LoadString(IDS_FROM);
		nums = toS.LoadStringW(IDS_TO);
		st = evTxt;
		if (!lvl.IsEmpty()) {
			s = lvl;
			s += _T("\n");
			s += _T("               ");
		}
		else s = _T("               ");

		s += _T("               ");
		s += fromS;
		s += _T("      ");
		s += toS;
		s += _T("     ");
		s += lenS;
		s += _T("\n");

		nums = std.LoadString(IDS_TIME_OF_DAY);
		std += _T(" ");
		std += eventsP->getTimeOfDay(startEvent);
		std += _T("  ");
		std += eventsP->getTimeOfDay(stopEvent);
		s += std;
		s += _T("\n");

		nums = std.LoadString(IDS_REC_TIME2);
		std += _T("    ");
		std += CRGLGraph::gethms(startEvent);
		std += _T("  ");
		std += CRGLGraph::gethms(stopEvent);
		std += _T("  ");
		std += CRGLGraph::getsecs(stopEvent - startEvent);
		s += std;
		s += _T("\n\n");

		//---More info
		CRGLGraph *gP = getGraph();
		if (gP) {
			CString rcS;
			nums = rcS.LoadString(IDS_REC_TIME2);
			s += rcS;
			s += _T("  ");
			CString s0;
			bool ok = getTimeAt(msg.pt.x, msg.pt.y, &s0);
			if (ok) s += s0;
			s += _T("\n");
			nums = rcS.LoadString(IDS_AMPLITUDE);
			s += rcS;
			s += _T(" ");

			ok = getAmplAt(msg.pt.x, msg.pt.y, &s0);
			if (ok) s += s0;
		}
		m_ttip.SetTitle(TTI_INFO, st);
		m_ttip.UpdateTipText(s, this);
		m_ttip.RelayEvent(&msg);
		return;
	}

	CRGLGraph *gP = getGraph();
	if (gP) {
		float timeSec = gP->getTheTimeAtPixel(msg.pt.x);
		CString ttTitle;
		int nums = ttTitle.LoadString(IDS_TOD);
		ttTitle += _T(": ");
		ttTitle += eventsP->getTimeOfDayAt(timeSec);
		m_ttip.SetTitle(TTI_INFO, ttTitle);

		CString rcS;
		nums = rcS.LoadString(IDS_REC_TIME2);
		s = rcS;
		s += _T("  ");
		CString s0;
		bool ok = getTimeAt(msg.pt.x, msg.pt.y, &s0);
		if (ok) s += s0;
		s += _T("\n");

		ok = getEUAmplAt(msg.pt.x, msg.pt.y, &s0);
		if (ok) s += s0;

		currentCursor = CURS_ARROW;
		m_ttip.UpdateTipText(s, this);
		m_ttip.RelayEvent(&msg);
	}

#endif
}


void CGraphButton::shiftYAxis(int _move, bool _render)
{
	if (!YshiftIsAllowed) return;

	int numT = (int)tiles.GetCount();
	if (!numT) return;

	D2D1_SIZE_F sz = m_target->GetSize();

	CRGLTile *tP = tiles.GetAt(0);
	if (!tP->moveVert(sz.height, (float)_move)) return;  // If the first cannot move, then the rest should not be touched either

	for (int i = 1; i < numT; i++) {
		CRGLTile *tP = tiles.GetAt(i);
		tP->moveVert(sz.height, (float)_move);
	}
	if (_render) render();
}

/*
Description: Sets the x axis limits in engineering units (seconds)
_origin: Origin
_end: End
_render: Redraw if true
*/
void CGraphButton::setXaxisLimits(FLOAT _origin, FLOAT _end, bool _render)
{
	int numT = (int)tiles.GetCount();
	for (int i = 0; i < numT; i++) {
		CRGLTile *tP = tiles.GetAt(i);
		tP->setPlotRectXScaled(_origin, _end);
	}
	if (_render) render();
}

/*
Description: Sets the y axis limits in engineering units
_bottom: Origin
_top: End
_render: Redraw if true
*/
void CGraphButton::setYaxisLimits(FLOAT _bottom, FLOAT _top, bool _render)
{
	int numT = (int)tiles.GetCount();
	for (int i = 0; i < numT; i++) {
		CRGLTile *tP = tiles.GetAt(i);
		tP->setPlotRectYScaled(_bottom, _top);

		D2D1_RECT_F ddr = tP->getPlotRectScaled();
		runYAxisWizard(ddr.bottom, ddr.top);
	}
	if (_render) render();
}

void CGraphButton::shiftTime(int _move, bool _render)
{
	int numT = (int)tiles.GetCount();
	if (!numT) return;

	D2D1_SIZE_F sz = m_target->GetSize();

	FLOAT stop = 60 * 60 * 8;  // 8 hours

	CRGLTile *tP = tiles.GetAt(0);
	if (!tP->trackTimeShift(sz.width, _move, stop)) return;  // If the first cannot move, then the rest should not be touched either

	for (int i = 1; i < numT; i++) {
		CRGLTile *tP = tiles.GetAt(i);
		tP->trackTimeShift(sz.width, _move, stop);
	}
	if (_render) render();
}

void CGraphButton::discardLayerDeviceResources(void)
{
	if (!m_target) return;

	int num = (int)layers.GetCount();
	for (int i = 0; i < num; i++) {
		CRGLLayer *lP = layers.GetAt(i);
		int type = lP->getLayerType();
		switch (type) {
		case RGLLayerTypeGeneric:
			((CRGLLayer *)lP)->DiscardDeviceResources();
			break;
		case RGLLayerTypeGraph:
			((CRGLGraph *)lP)->DiscardDeviceResources();
			break;
		case RGLLayerTypeGraphSparse:
			((CRGLGraphSparse *)lP)->DiscardDeviceResources();
			break;
		case RGLLayerTypeGraphFilled:
			((CRGLGraphFilled *)lP)->DiscardDeviceResources();
			break;
		case RGLLayerTypeGraphPoints:
			((CRGLGraphEventMarkers *)lP)->DiscardDeviceResources();
			break;
		case RGLLayerTypeBackground:
			((CRGLBackground *)lP)->DiscardDeviceResources();
			break;
		case RGLLayerTypeAxes:
			((CRGLAxes *)lP)->DiscardDeviceResources();
			break;
		case RGLLayerTypeGrid:
			((CRGLGrid *)lP)->DiscardDeviceResources();
			break;
		case RGLLayerTypeText:
			((CRGLText *)lP)->DiscardDeviceResources();
			break;
		case RGLLayerTypeBar:
			((CRGLBar *)lP)->DiscardDeviceResources();
			break;
		}
	}
}

void CGraphButton::OnRButtonDblClk(UINT nFlags, CPoint point)
{
	// TODO: Add your message handler code here and/or call default
	ReleaseCapture();

	if (eventsP) {
		if (eventsP->getIsVisualEventEditingActive()) {
			int whatRGrabbed = OVER_NOTHING;
			int whereRGrabbed = OVER_NOTHING;
			FLOAT theTime;
			bool ok = getTimeValAt(point.x, point.y, &theTime);
			if (!ok) return;
			UINT level = levelTypeUndef;
			UINT evtType = eventsP->getIsOnEventVisualEditing(theTime, EDGE_DETECT_TOLERANCE, &whatRGrabbed, &whereRGrabbed,&level);
			int add = FALSE;
			bool okDeloradd = false;
			if (OVER_EVNT == whatRGrabbed) {
				okDeloradd = eventsP->visualEditDeleteWholeEvent(evtType,theTime);
				eventsP->onOffVectorToBeginEndsVector(evtType);
			}
			else {
				UINT evtType2 = eventsP->getTheEventBeingVisuallyEdited();
				if (evntTypeAH & evtType2) { // Need specific event type in this case
					UINT type;
					if (getTypeFromKey(&type))  // Read keyboard
						evtType2 = type;
					else {
						MessageBeep(MB_ICONWARNING);
						return;
					}
				}
				float leftTimeLimit, rightTimeLimit;
				getMinMaxX(&leftTimeLimit, &rightTimeLimit);
				UINT forceLevel;
				bool findFromGradient = true; // Adding new AH event will always find from gradient
				level = getLevelFromKey(&forceLevel,&findFromGradient) ? forceLevel : level;
				if (okDeloradd = eventsP->visualEditAddNewEvent(evtType2, theTime, leftTimeLimit, rightTimeLimit,level)) {
					add = TRUE; 
					eventsP->onOffVectorToBeginEndsVector(evtType2);
				}
			}
			if (okDeloradd) ::SendMessage(GetParent()->m_hWnd, WM_POST_EVENT_VISUAL_EDIT_CHANGE, (WPARAM) add, 0L);
		}
	}

	CXTPButton::OnRButtonDblClk(nFlags, point);
}


void CGraphButton::OnLButtonDblClk(UINT nFlags, CPoint point)
{
	// TODO: Add your message handler code here and/or call default

	ReleaseCapture();

	int numT = (int)tiles.GetCount();
	if (!numT) return;

	CRect r;
	GetClientRect(&r);

	//---If outside: ignore
	CRGLTile *tP;
	bool wasHit = false;
	for (int i = 0; i < numT; i++) {
		tP = tiles.GetAt(i);
		if (tP->isPointInTile(r, point)) {
			wasHit = true;
			tP->mark(true);
		}
	}
	if (!wasHit) {
		CXTPButton::OnLButtonDblClk(nFlags, point);
		return;
	}

	float time = .0f;
	if (getTimeValAt(point.x, point.y, &time))
		::PostMessage(GetParent()->m_hWnd, messCentreX, GetDlgCtrlID(), (LONG) (time * 10.0f));

	//----This feature contracts the time axis by factor of 0.5
	//float ratio = 2.0f;
	//for (int i = 0; i < numT; i++) {
	//	CRGLTile *tP = tiles.GetAt(i);
	//	tP->trackTimeScaleAboutCentre(ratio);
	//}

	//---Pick the dataset
	int numLayers = layers.GetCount();
	for (int i = 0; i < numLayers; i++) {
		CRGLLayer *lP = layers.GetAt(i);
		int type = lP->getLayerType();
		if ((RGLLayerTypeGraphSparse == type) || (RGLLayerTypeGraph == type)) {
			CRGLTile *tP = lP->getTheTile();
			if (tP->getMark()) {
				D2D1_RECT_F r = tP->getPlotRectScaled();
				FLOAT max = ((CRGLGraph *)lP)->getMax(r.left, r.right);
				FLOAT min = ((CRGLGraph *)lP)->getMin(r.left, r.right);
				//*INN******
				tP->mark(false);
			}
		}
		if (RGLLayerTypeGraphFilled == type) {
			CRGLTile *tP = lP->getTheTile();
			if (tP->getMark()) {
				D2D1_RECT_F r = tP->getPlotRectScaled();
				FLOAT max = ((CRGLGraphFilled *)lP)->getMax(r.left, r.right);
				FLOAT min = ((CRGLGraphFilled *)lP)->getMin(r.left, r.right);
				//*INN******
				tP->mark(false);
			}
		}
		if (RGLLayerTypeGraphPoints == type) {
			CRGLTile *tP = lP->getTheTile();
			if (tP->getMark()) {
				D2D1_RECT_F r = tP->getPlotRectScaled();
				FLOAT max = ((CRGLGraphEventMarkers *)lP)->getMax(r.left, r.right);
				FLOAT min = ((CRGLGraphEventMarkers *)lP)->getMin(r.left, r.right);
				//*INN******
				tP->mark(false);
			}
		}
		if (RGLLayerTypeAxes == type) {
			CRGLTile *tP = lP->getTheTile();
			D2D1_RECT_F r = tP->getPlotRectScaled();
			((CRGLAxes *)lP)->xAxisWizard(r.left, r.right);
		}
	}

	render();
	::PostMessage(GetParent()->m_hWnd, messScaleX, GetDlgCtrlID(), 0L);

	CXTPButton::OnLButtonDblClk(nFlags, point);
}

void CGraphButton::DrawItem(LPDRAWITEMSTRUCT /*lpDrawItemStruct*/)
{

	// TODO:  Add your code to draw the specified item
	if (!doneInitDependentResources) {
		doneInitDependentResources = true;
		HRESULT res2 = CreateDeviceResources();
	}
}

void CGraphButton::setTitle(CString _title)
{
	title = _title;
}

void CGraphButton::setHideYlabels(bool _hideYLabels)
{
	hideYlabels = _hideYLabels;
}

void CGraphButton::updateCurveColour(void)
{

}

BOOL CGraphButton::Create(LPCTSTR lpszCaption, DWORD dwStyle, const RECT& rect, CWnd* pParentWnd, UINT nID)
{
	// TODO: Add your specialized code here and/or call the base class
	return CXTPButton::Create(lpszCaption, dwStyle, rect, pParentWnd, nID);
}

HRESULT CGraphButton::CreateDeviceIndependentResources(void)
{
	HRESULT hr;


#if defined(DEBUG) || defined(_DEBUG)
	D2D1_FACTORY_OPTIONS options;
	options.debugLevel = D2D1_DEBUG_LEVEL_INFORMATION;
	hr = D2D1CreateFactory(
		D2D1_FACTORY_TYPE_SINGLE_THREADED,
		options, &m_factory);
#else
	hr = D2D1CreateFactory(
		D2D1_FACTORY_TYPE_SINGLE_THREADED,  // D2D1_FACTORY_TYPE_MULTI_THREADED
		&m_factory);
#endif


	//hr = D2D1CreateFactory(D2D1_FACTORY_TYPE_SINGLE_THREADED,&m_factory);
	if (SUCCEEDED(hr)) {
		hr = DWriteCreateFactory(
			DWRITE_FACTORY_TYPE_SHARED,
			__uuidof(IDWriteFactory),
			reinterpret_cast<IUnknown**>(&pDWriteFactory)
			);
		if (SUCCEEDED(hr)) {
			hr = pDWriteFactory->CreateTextFormat(
				L"Times New Roman", // Font family name.
				NULL,        // Font collection (NULL sets it to use the system font collection).
				DWRITE_FONT_WEIGHT_REGULAR,
				DWRITE_FONT_STYLE_NORMAL,
				DWRITE_FONT_STRETCH_NORMAL,
				fntSize,
				L"en-us",
				&pTextFormat
				);
		}
		if (SUCCEEDED(hr)) {
			hr = pTextFormat->SetTextAlignment(DWRITE_TEXT_ALIGNMENT_CENTER);
		}
		if (SUCCEEDED(hr)) {
			hr = pTextFormat->SetParagraphAlignment(DWRITE_PARAGRAPH_ALIGNMENT_CENTER);
		}
	}
	return S_OK;
}

HRESULT CGraphButton::CreateDeviceResources(void)
{
	HRESULT hr = S_OK;
	if (0 == m_target) {
		D2D1_SIZE_U size = D2D1::SizeU(clientRect.Width(), clientRect.Height());
		hr = m_factory->CreateHwndRenderTarget(D2D1::RenderTargetProperties(),
			D2D1::HwndRenderTargetProperties(m_hWnd, size), &m_target);
	}
	return hr;
}

LRESULT CGraphButton::WindowProc(UINT message, WPARAM wParam, LPARAM lParam)
{
	// TODO: Add your specialized code here and/or call the base class

	switch (message) {
	case WM_GESTURE:
		{
			GESTUREINFO gi;     
			ZeroMemory(&gi, sizeof(GESTUREINFO));  
			gi.cbSize = sizeof(GESTUREINFO);
			BOOL bResult  = GetGestureInfo((HGESTUREINFO) lParam,&gi);
			if (gi.dwID == GID_ZOOM) {
				if (gi.dwFlags & GF_BEGIN) beginZoom = true;
				if (gi.dwFlags & GF_END) beginZoom = false;
			}
		}
		break;
	}

	return CXTPButton::WindowProc(message, wParam, lParam);
}

void CGraphButton::OnShowWindow(BOOL bShow, UINT nStatus)
{
	CXTPButton::OnShowWindow(bShow, nStatus);

	// TODO: Add your message handler code here
}

void CGraphButton::DiscardDeviceResources(void)
{
	discardLayerDeviceResources();

	if (m_target) m_target.Release();
	m_target = NULL;
	if (pDWriteFactory) pDWriteFactory.Release();
	pDWriteFactory = NULL;
	if (pTextFormat) pTextFormat->Release();
	pTextFormat = NULL;
}

void CGraphButton::OnSize(UINT nType, int cx, int cy)
{
	CXTPButton::OnSize(nType, cx, cy);

	// TODO: Add your message handler code here

	if (0 != m_target)
	{
		HRESULT hr = m_target->Resize(D2D1::SizeU(cx, cy));
		if (FAILED(hr))
		{
			DiscardDeviceResources();
			Invalidate(FALSE);
		}
		else resizeLayers();
	}
}

void CGraphButton::flush(void)
{
	if (m_target) m_target->Flush();
}

void CGraphButton::render(bool _onlyEvents /* = false*/, bool _disableHoverIndication /* = false */)
{
	if (SUCCEEDED(CreateDeviceResources()))
	{
		if (0 == (D2D1_WINDOW_STATE_OCCLUDED & m_target->CheckWindowState()))
		{
			//---Render here
			m_target->BeginDraw();

			if (_onlyEvents) renderEvents();
			else renderLayers(_disableHoverIndication);

			D2D1_TAG tag1, tag2;
			HRESULT hr = m_target->Flush(&tag1, &tag2);

			HRESULT hr2 = m_target->EndDraw(&tag1, &tag2);
			if (D2DERR_RECREATE_TARGET == hr2)
			{
				DiscardDeviceResources();
			}
		}
	}
}

void CGraphButton::OnPaint()
{
	CPaintDC dc(this); // device context for painting
					   // TODO: Add your message handler code here
					   // Do not call CXTPButton::OnPaint() for painting messages
	if (!doneInitDependentResources) {
		doneInitDependentResources = true;
		HRESULT res2 = CreateDeviceResources();
	}
	PAINTSTRUCT paint;
	VERIFY(BeginPaint(&paint));
	try {
		render(drawOnlyEvents);  // default false
		drawOnlyEvents = false;
	}
	catch (CException* e) {
		TCHAR   szCause[255];
		e->GetErrorMessage(szCause, 255);
		CString s;
		s.Format(_T("Exception thrown in CGraphButton::OnPaint() - regSubkey "));
		s += regSubKey;
		s += _T(",\t");
		CString causeS(szCause);
		s += _T("Cause : ");
		s += causeS;
		if (dmpLogP) dmpLogP->dump(s);
		e->Delete();
	}
	EndPaint(&paint);
}

bool CGraphButton::scaleYAxis(CPoint _pt)
{
	if (!YscaleIsAllowed) {
		return false;
	}

	int numT = (int)tiles.GetCount();
	if (!numT) {
		return false;
	}

	int move = _pt.y - rightPoint.y;

	CRect r;
	GetClientRect(&r);

	if (rightPoint.y >= r.bottom) {
		return false;  // Avoid divide by zero
	}

	try {
		float ratio = (float)move / ((float)rightPoint.y - r.bottom);

		CRGLTile *tP0 = tiles.GetAt(0);
		if (!tP0->scaleVert(ratio, dualPolarity)) return false; // If the first cannot move, then the rest should not be touched either

		for (int i = 1; i < numT; i++) {
			CRGLTile *tP = tiles.GetAt(i);
			tP->scaleVert(ratio, dualPolarity);
		}

		rightPoint = _pt;
		D2D1_RECT_F ddr = tP0->getPlotRectScaled();
		runYAxisWizard(ddr.bottom, ddr.top);

		render();
	}
	catch (CException* e) {
		TCHAR   szCause[255];
		e->GetErrorMessage(szCause, 255);
		CString s;
		s.Format(_T("Exception thrown in CGraphButton::scaleYAxis(), pt (%d,%d) - regSubkey "), _pt.x, _pt.y);
		s += regSubKey;
		s += _T(",\t");
		CString causeS(szCause);
		s += _T("Cause : ");
		s += causeS;
		if (dmpLogP) dmpLogP->dump(s);
		e->Delete();
	}
	return true;
}

void CGraphButton::scaleTimeAboutCentre(int _delta)
{
	int numT = (int)tiles.GetCount();
	if (!numT) return;

	CRect r;
	GetClientRect(&r);
	float ratio = _delta < 0 ? .2f : -.2f;

	CRGLTile *tP0 = tiles.GetAt(0);
	if (!tP0->trackTimeScaleAboutCentre(ratio)) return;  // If the first cannot move, then the rest should not be touched either

	for (int i = 1; i < numT; i++) {
		CRGLTile *tP = tiles.GetAt(i);
		tP->trackTimeScaleAboutCentre(ratio);
	}
	D2D1_RECT_F ddr = tP0->getPlotRectScaled();
	runXAxisWizard(ddr.left, ddr.right);

	render();
}

void CGraphButton::scaleTimeAboutCentre(CPoint _pt)
{
	int numT = (int)tiles.GetCount();
	if (!numT) return;

	int move = _pt.x - rightPoint.x;
	CRect r;
	GetClientRect(&r);
	float centre = (float)(r.left + r.right) / 2.0f;
	float ratio = (float)move / ((float)rightPoint.x - centre);

	CRGLTile *tP0 = tiles.GetAt(0);
	if (!tP0->trackTimeScaleAboutCentre(ratio)) return;  // If the first cannot move, then the rest should not be touched either

	for (int i = 1; i < numT; i++) {
		CRGLTile *tP = tiles.GetAt(i);
		tP->trackTimeScaleAboutCentre(ratio);
	}
	rightPoint = _pt;
	D2D1_RECT_F ddr = tP0->getPlotRectScaled();
	runXAxisWizard(ddr.left, ddr.right);

	render();
}

void CGraphButton::getMinMaxY(FLOAT *_bottom, FLOAT *_top)
{
	int numT = (int)tiles.GetCount();
	if (!numT) return;
	CRGLTile *tP0 = tiles.GetAt(0);
	D2D1_RECT_F ddr = tP0->getPlotRectScaled();
	*_bottom = ddr.bottom;
	*_top = ddr.top;
}

void CGraphButton::getMinMaxX(FLOAT *_left, FLOAT *_right)
{
	int numT = (int)tiles.GetCount();
	if (!numT) return;
	CRGLTile *tP0 = tiles.GetAt(0);
	D2D1_RECT_F ddr = tP0->getPlotRectScaled();
	*_left = ddr.left;
	*_right = ddr.right;
}

void CGraphButton::runXAxisWizard(FLOAT _left, FLOAT _right)
{
	int numLayers = layers.GetCount();
	for (int i = 0; i < numLayers; i++) {
		CRGLLayer *lP = layers.GetAt(i);
		int type = lP->getLayerType();
		if (RGLLayerTypeAxes == type) {
			((CRGLAxes *)lP)->xAxisWizard(_left, _right);
		}
	}
}

void CGraphButton::runYAxisWizard(FLOAT _bottom, FLOAT _top)
{
	int numLayers = layers.GetCount();
	for (int i = 0; i < numLayers; i++) {
		CRGLLayer *lP = layers.GetAt(i);
		int type = lP->getLayerType();
		if (RGLLayerTypeAxes == type) {
			((CRGLAxes *)lP)->yAxisWizard(_bottom, _top);
		}
	}
}

void CGraphButton::changeAnEvent(CEvnt *_evnt, int _command)
{
	// Nothing happens here
}

void CGraphButton::moveEventEdge(CEvnt *_evnt, FLOAT _moveSecs, int _object)
{
	// Nothing happens here
}


void CGraphButton::moveLeftCursor(int _move)
{
	// Nothing happens here
}

void CGraphButton::moveRightCursor(int _move)
{
	// Nothing happens here
}

void CGraphButton::OnMouseMove(UINT nFlags, CPoint point)
{
	// TODO: Add your message handler code here and/or call default

	//---Empty the queue
	MSG msg;
	while (PeekMessage(&msg, this->m_hWnd, WM_MOUSEMOVE, WM_MOUSEMOVE, PM_REMOVE));
	
	bool left = (nFlags & MK_LBUTTON) > 0;
	bool right = (nFlags & MK_RBUTTON) > 0;
	bool middle = (nFlags & MK_MBUTTON) > 0;
	bool ctrl = (FCONTROL & nFlags) > 0;

	if ((!left && !right && middle) || (left && !right && ctrl)) {

		FLOAT time = .0f;
		bool ok = getTimeValAt(point.x, point.y, &time);
		statisticsWindowEdge1 = time;

		float left = (statisticsWindowEdge0 <= statisticsWindowEdge1) ? statisticsWindowEdge0 : statisticsWindowEdge1;
		float right = (statisticsWindowEdge0 >= statisticsWindowEdge1) ? statisticsWindowEdge0 : statisticsWindowEdge1;
		setCursorWindow(left, right);
		render();

		RelayEventAndSetCursor(WM_MOUSEMOVE, (WPARAM)nFlags,
			MAKELPARAM(LOWORD(point.x), LOWORD(point.y)), _T("The text"));
		return CXTPButton::OnMouseMove(nFlags, point);
	}

	////////////////////////////////////
	if (!left && right) {
		switch (cursorControl) {
		case cursorControlX:
			scaleTimeAboutCentre(point);
			break;
		case cursorControlY:
			if (YscaleIsAllowed && (CURS_SIZENS == currentCursor)) {
				if (scaleYAxis(point))
					::PostMessage(GetParent()->m_hWnd, messScaleY, GetDlgCtrlID(), 0L);
			}
			break;
		default:
			break;
		}
		return;
	}
	//////////////////////////////////
	int move;
	if (left && !right) {
		if ((eventsP) && (!eventsP->getIsVisualEventEditingActive())) {
				move = point.x - leftPoint.x;
				shiftTime(move);
				leftPoint.x += move;
				::PostMessage(GetParent()->m_hWnd, messMoveX, GetDlgCtrlID(), 0L);

				if (YshiftIsAllowed) {
					move = -point.y + leftPoint.y;
					shiftYAxis(move);
					::PostMessage(GetParent()->m_hWnd, messMoveY, GetDlgCtrlID(), 0L);
					leftPoint.y -= move;
				}
		}
		else { // Visual editing
			if (visualWholeEventMoving) {
				FLOAT theTime = .0f;
				if (getTimeValAt(point.x, 0, &theTime)) {
					eventEditWindow.begin = theTime - visualGrabDistToLeft;
					eventEditWindow.end = theTime + visualGrabDistToRight;
					if (eventsP->getConflict(visualEventMovingType,eventEditWindow.begin, eventEditWindow.end)) {
						currentCursor = CURS_NO;
						::SetCursor(AfxGetApp()->LoadStandardCursor(IDC_NO));
					}
					else {
						currentCursor = CURS_MOVE;
						::SetCursor(AfxGetApp()->LoadStandardCursor(IDC_HAND));
					}
					render();
				}
			}
			else if (visualLeftEdgeMoving) {
				FLOAT theTime = .0f;
				if (getTimeValAt(point.x, 0, &theTime)) {
					if (theTime > eventEditWindow.end) {
						visualLeftEdgeMoving = false;
						visualRightEdgeMoving = true;
					}
					else eventEditWindow.begin = theTime; 
					if (eventsP->getConflict(visualEventMovingType,eventEditWindow.begin, eventEditWindow.end)) {
						currentCursor = CURS_NO;
						::SetCursor(AfxGetApp()->LoadStandardCursor(IDC_NO));
					}
					else {
						currentCursor = CURS_SIZEWE;
						::SetCursor(AfxGetApp()->LoadStandardCursor(IDC_SIZEWE));
					}
					render();
				}
			}
			else if (visualRightEdgeMoving) {
				FLOAT theTime = .0f;
				if (getTimeValAt(point.x, 0, &theTime)) {
					if (theTime < eventEditWindow.begin) {
						visualLeftEdgeMoving = true;
						visualRightEdgeMoving = false;
					}
					else eventEditWindow.end = theTime;
					if (eventsP->getConflict(visualEventMovingType,eventEditWindow.begin, eventEditWindow.end)) {
						currentCursor = CURS_NO;
						::SetCursor(AfxGetApp()->LoadStandardCursor(IDC_NO));
					}
					else {
						currentCursor = CURS_SIZEWE;
						::SetCursor(AfxGetApp()->LoadStandardCursor(IDC_SIZEWE));
					}
					render();
				}
			}
			else {
				move = point.x - leftPoint.x;
				shiftTime(move);
				leftPoint.x += move;
				::PostMessage(GetParent()->m_hWnd, messMoveX, GetDlgCtrlID(), 0L);
			}
		}
	}
	////////////////////////////////////
	if (left && right) {
		if (YshiftIsAllowed) {
			move = -point.y + leftPoint.y;
			shiftYAxis(move);
			::PostMessage(GetParent()->m_hWnd, messMoveY, GetDlgCtrlID(), 0L);
			leftPoint.y -= move;
		}
	}
	/////////////////////////////////////
	if (!left && !right) {
		if ((eventsP) && (eventsP->getIsVisualEventEditingActive())) {
			
			RelayEventAndSetCursorDuringVisualEventEditing(WM_MOUSEMOVE, (WPARAM)nFlags,				  
												MAKELPARAM(LOWORD(point.x), LOWORD(point.y)), _T("The text"));	  

			UINT whatHit = OVER_NOTHING;
			UINT whereHit = OVER_NOTHING;
			FLOAT theTime0,theTime1;
			bool ok = getTimeValAt(point.x - deltaX, point.y, &theTime0);
			ok = getTimeValAt(point.x + deltaX, point.y, &theTime1);
			UINT level;
			eventsP->getIsOnEventVisualEditing_Range(theTime0,theTime1, &whatHit, &whereHit,
				&level,&eventEditWindow.begin, &eventEditWindow.end);
			if (OVER_LEFT_EDGE & whereHit) {
				currentCursor = CURS_SIZEWE;
				hoverIndication = true;
				::SendMessage(GetParent()->m_hWnd, messDisableEventHoverIndication, 0L, 0L);
				render(); 
				hoverIndication = false;
			}
			else if (OVER_RIGHT_EDGE & whereHit) {
				currentCursor = CURS_SIZEWE;
				hoverIndication = true;
				::SendMessage(GetParent()->m_hWnd, messDisableEventHoverIndication, 0L, 0L);
				render();
				hoverIndication = false;
			}
			else if ((OVER_EVNT & whatHit) && (OVER_EVNT & whereHit)) {
				currentCursor = CURS_MOVE;
				hoverIndication = true;
				::SendMessage(GetParent()->m_hWnd, messDisableEventHoverIndication, 0L, 0L);
				render();
				hoverIndication = false;
			}
			else {
				visualRightEdgeMoving = false;
				visualLeftEdgeMoving = false;
				visualWholeEventMoving = false;
				currentCursor = CURS_ARROW;
				hoverIndication = false;
				::SendMessage(GetParent()->m_hWnd, messDisableEventHoverIndication, 0L, 0L);
				render();
			}
		}
		else {
			ReleaseCapture();
			RelayEventAndSetCursor(WM_MOUSEMOVE, (WPARAM)nFlags,
				MAKELPARAM(LOWORD(point.x), LOWORD(point.y)), _T("The text"));
		}
	}
	CXTPButton::OnMouseMove(nFlags, point);
}

void CGraphButton::OnMouseLeave()
{
	// Totally unreliable !!
}

void CGraphButton::OnLButtonDown(UINT nFlags, CPoint point)
{
	// TODO: Add your message handler code here and/or call default
	if (!m_target) return;

	if (FCONTROL & nFlags) {
		FLOAT time = .0f;
		bool ok = getTimeValAt(point.x, point.y, &time);
		statisticsWindowEdge0 = time;
		statisticsWindowEdge1 = time;
		setCursorWindow(statisticsWindowEdge0, statisticsWindowEdge1);
		eventsToShow |= evntTypeCursorWnd;
		getGraph()->setEventsToShow(eventsToShow);
		render();
		currentCursor = CURS_SIZEWE;
		::SetCursor(AfxGetApp()->LoadStandardCursor(IDC_SIZEWE));
		SetCapture();
		return CXTPButton::OnLButtonDown(nFlags, point);
	}

	if (enableEditableWindowMouseControl) return CXTPButton::OnLButtonDown(nFlags, point);

	CRect r;
	GetClientRect(&r);

	int whatLGrabbed = OVER_NOTHING;
	int whereLGrabbed = OVER_NOTHING;
	FLOAT theTime;
	bool ok = getTimeValAt(point.x, point.y, &theTime);
	if (!ok) return;

	if (eventsP && eventsP->getIsVisualEventEditingActive()) {
		VISUAL_EVENT vE = eventsP->getVisualEditingEventByMouse();
		if (evntTypeNone != vE.type) {
			UINT whatGrabbed = OVER_NOTHING;
			UINT whereGrabbed = OVER_NOTHING;
			FLOAT theTime0, theTime1;
			bool ok = getTimeValAt(point.x - deltaX, point.y, &theTime0);
			ok = getTimeValAt(point.x + deltaX, point.y, &theTime1);
			UINT level = levelTypeUndef;
			UINT eventHit = eventsP->getIsOnEventVisualEditing_Range(theTime0, theTime1, &whatGrabbed, &whereGrabbed,
				&level, &eventEditWindow.begin, &eventEditWindow.end);

			float visualEditLimitToLeft = .0f;
			float visualEditLimimitToRight = eventsP->getEffectiveStop();
			if (OVER_LEFT_EDGE & whereGrabbed) {
				currentCursor = CURS_SIZEWE;
				::SetCursor(AfxGetApp()->LoadStandardCursor(IDC_SIZEWE));
				leftPoint = point;
				visualLeftEdgeMoving = true;
				visualRightEdgeMoving = false;
				visualWholeEventMoving = false;
				visualEventMovingType = vE.type;
				visualEventMovingHasThisLevel = vE.level;
				eventsP->setVisualEditingLimits(eventEditWindow.begin, eventEditWindow.end);
				SetCapture();
				render();

				return;
			}
			else if (OVER_RIGHT_EDGE & whereGrabbed) {
				currentCursor = CURS_SIZEWE;
				::SetCursor(AfxGetApp()->LoadStandardCursor(IDC_SIZEWE));
				leftPoint = point;
				visualRightEdgeMoving = true;
				visualLeftEdgeMoving = false;
				visualWholeEventMoving = false;
				visualEventMovingType = vE.type;
				visualEventMovingHasThisLevel = vE.level;
				eventsP->setVisualEditingLimits(eventEditWindow.begin, eventEditWindow.end);
				SetCapture();
				render();

				return;
			}
			else if ((OVER_EVNT == whatGrabbed) && (OVER_EVNT == whereGrabbed)) {

				currentCursor = CURS_MOVE;
				::SetCursor(AfxGetApp()->LoadStandardCursor(IDC_HAND));
				leftPoint = point;
				visualWholeEventMoving = true;
				visualRightEdgeMoving = false;
				visualLeftEdgeMoving = false;
				visualEventMovingType = vE.type;
				visualEventMovingHasThisLevel = vE.level;
				eventsP->setVisualEditingLimits(eventEditWindow.begin, eventEditWindow.end);
				SetCapture();
				visualGrabDistToLeft = theTime - eventEditWindow.begin;
				visualGrabDistToRight = eventEditWindow.end - theTime;
				render();

				return;
			}
		}
	}

	//CEvnt *evP = eventsP->getIsOnEvent(eventsToShow, theTime, EDGE_DETECT_TOLERANCE, &whatLGrabbed, &whereLGrabbed);
	//if (evP) {
	//	if (whereLGrabbed & OVER_EDGE) {
	//		currentCursor = CURS_ARROW;
	//		::SetCursor(AfxGetApp()->LoadStandardCursor(IDC_SIZEWE));
	//		if (whereLGrabbed & OVER_LEFT_EDGE) {
	//			//pickWhat = whatLGrabbed;
	//			//pickWhere = whereLGrabbed;
	//			if (whatLGrabbed & OVER_CURSOR) r.right = getRightCursorX();
	//		}
	//		else if (whereLGrabbed & OVER_RIGHT_EDGE) {
	//			//pickWhat = whatLGrabbed;
	//			//pickWhere = whereLGrabbed;
	//			if (whatLGrabbed & OVER_CURSOR) r.left = getLeftCursorX();
	//		}
	//		else if (whereLGrabbed & OVER_EDGE) {
	//			//pickWhat = whatLGrabbed;
	//			//pickWhere = whereLGrabbed;
	//			bool ok;
	//			int xx;
	//			if ((eventPointedTo) && (whatLGrabbed & OVER_EVNT)) {
	//				switch (edgePointedTo) {
	//				case closestIsLeft:
	//					ok = getScreenXfromTime(eventPointedTo->getTo(),&xx);
	//					r.right = ok ? xx : 0;
	//					break;
	//				case closestIsRight:
	//					ok = getScreenXfromTime(eventPointedTo->getFrom(),&xx);
	//					r.left = ok ? xx : 0;
	//					break;
	//				}
	//			}
	//		}
	//	}
	//}
	//else {
		//pickWhere = OVER_ENTIRE_GRAPH;
		currentCursor = CURS_MOVE;
		::SetCursor(AfxGetApp()->LoadStandardCursor(IDC_HAND));
	//}
	leftPoint = point;
	SetCapture();

	CXTPButton::OnLButtonDown(nFlags, point);
}

void CGraphButton::OnRButtonDown(UINT nFlags, CPoint point)
{
	// TODO: Add your message handler code here and/or call default
	if (!m_target) return;

	if (enableEditableWindowMouseControl) return;
	//if (eventsP) if (eventsP->getIsVisualEventEditingActive()) return;

	if (cursorControlX == cursorControl) {
		currentCursor = CURS_SIZEWE;
		::SetCursor(AfxGetApp()->LoadStandardCursor(IDC_SIZEWE));
	}
	else if (cursorControlY == cursorControl) {
		if (YscaleIsAllowed) {
			currentCursor = CURS_SIZENS;
			::SetCursor(AfxGetApp()->LoadStandardCursor(IDC_SIZENS));
		}
	}

	rightPoint = point;
	SetCapture();

	CXTPButton::OnRButtonDown(nFlags, point);
}


void CGraphButton::OnRButtonUp(UINT nFlags, CPoint point)
{
	// TODO: Add your message handler code here and/or call default

	if (enableEditableWindowMouseControl) {
		FLOAT time = .0f;
		bool ok = getTimeValAt(point.x, point.y, &time);
		::PostMessage(GetParent()->m_hWnd, messSetRightEditWindow, GetDlgCtrlID(), (LONG)(time * 10.0f));
		ReleaseCapture();
		return CXTPButton::OnLButtonUp(nFlags, point);
	}

	//if (OVER_CURSOR & pickWhere) {
	//	::PostMessage(GetParent()->m_hWnd, messMoveCursorWindow, GetDlgCtrlID(), 0L);
	//}
	//else {
		switch (cursorControl) {
		case cursorControlX:
			::PostMessage(GetParent()->m_hWnd, messScaleX, GetDlgCtrlID(), 0L);
			break;
		case cursorControlY:
			if (YscaleIsAllowed)
				::PostMessage(GetParent()->m_hWnd, messScaleY, GetDlgCtrlID(), 0L);
			break;
		default:
			// Context menu
			break;
		}
	//}

	currentCursor = defaultCursor;
	ReleaseCapture();
	//ClipCursor(NULL);

	CXTPButton::OnRButtonUp(nFlags, point);
}

void CGraphButton::OnMButtonDown(UINT nFlags, CPoint point)
{
	FLOAT time = .0f;
	bool ok = getTimeValAt(point.x, point.y, &time);
	statisticsWindowEdge0 = time;
	statisticsWindowEdge1 = time;
	setCursorWindow(statisticsWindowEdge0, statisticsWindowEdge1);
	eventsToShow |= evntTypeCursorWnd;
	getGraph()->setEventsToShow(eventsToShow);
	render();
	currentCursor = CURS_SIZEWE;
	::SetCursor(AfxGetApp()->LoadStandardCursor(IDC_SIZEWE));
	SetCapture();
}

void CGraphButton::OnMButtonUp(UINT nFlags, CPoint point)
{
	float left = (statisticsWindowEdge0 <= statisticsWindowEdge1) ? statisticsWindowEdge0 : statisticsWindowEdge1;
	float right = (statisticsWindowEdge0 >= statisticsWindowEdge1) ? statisticsWindowEdge0 : statisticsWindowEdge1;
	setCursorWindow(left, right);
	ReleaseCapture();
	currentCursor = CURS_ARROW;
	::SetCursor(AfxGetApp()->LoadStandardCursor(IDC_ARROW));

	::SendMessage(GetParent()->m_hWnd, messSetLeftEditWindow, GetDlgCtrlID(), (LONG)(left * 10.0f));
	::SendMessage(GetParent()->m_hWnd, messSetRightEditWindow, GetDlgCtrlID(), (LONG)(right * 10.0f));
}

void CGraphButton::OnLButtonUp(UINT nFlags, CPoint point)
{
	// TODO: Add your message handler code here and/or call default

	if (FCONTROL & nFlags) {
		float left = (statisticsWindowEdge0 <= statisticsWindowEdge1) ? statisticsWindowEdge0 : statisticsWindowEdge1;
		float right = (statisticsWindowEdge0 >= statisticsWindowEdge1) ? statisticsWindowEdge0 : statisticsWindowEdge1;
		setCursorWindow(left, right);
		ReleaseCapture();
		currentCursor = CURS_ARROW;
		::SetCursor(AfxGetApp()->LoadStandardCursor(IDC_ARROW));

		::SendMessage(GetParent()->m_hWnd, messSetLeftEditWindow, GetDlgCtrlID(), (LONG)(left * 10.0f));
		::SendMessage(GetParent()->m_hWnd, messSetRightEditWindow, GetDlgCtrlID(), (LONG)(right * 10.0f));

		return CXTPButton::OnLButtonUp(nFlags, point);
	}
	if (eventsP) {
		if (eventsP->getIsVisualEventEditingActive()) {
			UINT newLevel;
			bool findFromGradient = false;
			bool keyDown = getLevelFromKey(&newLevel,&findFromGradient);
			if (keyDown || (leftPoint != point)) {
				visualEventMovingHasThisLevel = keyDown ? newLevel : visualEventMovingHasThisLevel;
				if (visualWholeEventMoving || visualLeftEdgeMoving || visualRightEdgeMoving) {
					if (currentCursor != CURS_NO) {
						float leftTimeLimit, rightTimeLimit;
						getMinMaxX(&leftTimeLimit, &rightTimeLimit);
						UINT level = visualEventMovingHasThisLevel;
						if (findFromGradient)
							level = eventsP->findLevelInTimeWindow(eventEditWindow.begin, eventEditWindow.end);
						if ((evntTypeSnoring == visualEventMovingType) && (findFromGradient))
							eventsP->doneVisualEditingSnoringMoves(visualEventMovingType, eventEditWindow.begin, eventEditWindow.end,
								leftTimeLimit, rightTimeLimit);
						else eventsP->doneVisualEditingMoves(visualEventMovingType, eventEditWindow.begin, eventEditWindow.end,
							leftTimeLimit, rightTimeLimit, level);

						::SendMessage(GetParent()->m_hWnd, WM_POST_EVENT_VISUAL_EDIT_CHANGE, (WPARAM)FALSE, 0L);
					}
					else if ((eventsP) && (CURS_NO == currentCursor)) {
						MessageBeep(MB_ICONWARNING);
					}
					visualWholeEventMoving = false;
					visualLeftEdgeMoving = false;
					visualRightEdgeMoving = false;
					visualEventMovingHasThisLevel = levelTypeUndef;
				}
			}
			//else {
			//	if (keyDown) {
			//		int whatRGrabbed = OVER_NOTHING;
			//		int whereRGrabbed = OVER_NOTHING;
			//		FLOAT theTime;
			//		if (getTimeValAt(point.x, point.y, &theTime)) {
			//			UINT oldLevel = levelTypeUndef;
			//			UINT evtType = eventsP->getIsOnEventVisualEditing(theTime, EDGE_DETECT_TOLERANCE, &whatRGrabbed, &whereRGrabbed,&oldLevel);
			//			if (OVER_EVNT & whatRGrabbed)
			//				if (eventsP->visualEditSetLevel(evtType, theTime, visualEventMovingHasThisLevel))
			//					::SendMessage(GetParent()->m_hWnd, WM_POST_EVENT_VISUAL_EDIT_CHANGE, (WPARAM)FALSE, 0L);
			//		}
			//	}
			//}
		}
	}

	//if (enableEditableWindowMouseControl) {
	//	FLOAT time = .0f;
	//	bool ok = getTimeValAt(point.x, point.y,&time);
	//	::PostMessage(GetParent()->m_hWnd, messSetLeftEditWindow, GetDlgCtrlID(), (LONG)(time * 10.0f));
	//	//return CXTPButton::OnLButtonUp(nFlags, point);
	//}

	//if ((eventPointedTo) && (OVER_EVNT & pickWhere)) {
	//	FLOAT pt1 = .0f;
	//	FLOAT pt2 = .0f;
	//	bool ok = getTimeValAt(leftPoint.x, leftPoint.y, &pt1);
	//	ok = getTimeValAt(point.x, point.y,&pt2);
	//	int move = point.x - leftPoint.x;
	//	leftPoint.x += move;
	//	moveEventEdge(eventPointedTo, pt2 - pt1, edgePointedTo);
	//	::PostMessage(GetParent()->m_hWnd, WM_NEW_EVENT_LENGTH, GetDlgCtrlID(), 0L);
	//}

	////---Add or delete events
	//int addRera = GetKeyState('R') | GetKeyState('r');
	//int addCent = GetKeyState('C') | GetKeyState('c');
	//int addMixe = GetKeyState('M') | GetKeyState('m');
	//int addObst = GetKeyState('O') | GetKeyState('o');
	//int addHypo = GetKeyState('H') | GetKeyState('h');
	//int addSnor = GetKeyState('S') | GetKeyState('s');
	//int addAwak = GetKeyState('A') | GetKeyState('a');
	//int addDesa = GetKeyState('D') | GetKeyState('d');
	//int addEvent =	addRera | addCent | addMixe |
	//				addObst | addHypo | addSnor | 
	//				addAwak | addDesa;
	//
	//if (eventPointedTo) {
	//	if (nFlags & MK_CONTROL) {  // Delete an event
	//		eventPointedTo->setEventFlags(eventPointedTo->getEventFlags() | EVNT_DEF_MANUAL_DELETE);
	//		if (eventsP) {
	//			eventsP->actOnManualDelete();
	//			::PostMessage(GetParent()->m_hWnd, messPointAndClickDeleteEvent, (WPARAM) eventPointedTo,0L);
	//		}
	//		TRACE(_T("Delete event\n"));
	//	}
	//	else if (addEvent) {
	//		MessageBeep(MB_ICONWARNING); // Cannot add event here
	//		TRACE(_T("Cannot add event\n"));
	//	}
	//}
	//else {
	//	if (nFlags & MK_CONTROL) {
	//		MessageBeep(MB_ICONWARNING); // Cannot delete here
	//		TRACE(_T("Cannot delete event\n"));
	//	}
	//	else if (addEvent) {
	//		TRACE(_T("Hit event, state of addEvent is %d\n"), (int)addEvent);
	//	}
	//}

	ReleaseCapture();
	currentCursor = defaultCursor;
	ClipCursor(NULL);

	CXTPButton::OnLButtonUp(nFlags, point);
}

/*
Description: Empty function. To be filled in by inheriting classes
*/
void CGraphButton::renderXYPlots(bool _mouseIsOver)
{
}

BOOL CGraphButton::OnSetCursor(CWnd* pWnd, UINT nHitTest, UINT message)
{
	if (enableEditableWindowMouseControl && vArrow) {
		::SetCursor(vArrow);
		return TRUE;
	}

	switch (currentCursor) {
	case CURS_SIZENS:
		::SetCursor(AfxGetApp()->LoadStandardCursor(IDC_SIZENS));
		break;
	case CURS_SIZEWE:
		::SetCursor(AfxGetApp()->LoadStandardCursor(IDC_SIZEWE));
		break;
	case CURS_STUDY:
		::SetCursor(AfxGetApp()->LoadStandardCursor(IDC_CROSS));
		break;
	case CURS_ARROW:
		::SetCursor(AfxGetApp()->LoadStandardCursor(IDC_ARROW));
		break;
	case CURS_SCALE:
		::SetCursor(AfxGetApp()->LoadStandardCursor(IDC_SIZEALL));
		break;
	case CURS_MOVE:
		::SetCursor(AfxGetApp()->LoadStandardCursor(IDC_HAND));
		break;
	case CURS_NO:
		::SetCursor(AfxGetApp()->LoadStandardCursor(IDC_NO));
		break;
	}
	return TRUE;
}


BOOL CGraphButton::OnCommand(WPARAM wParam, LPARAM lParam)
{
	// TODO: Add your specialized code here and/or call the base class
	/*if (ID_GRAPH_CONTEXT_MENU) {
	if (!layerToEdit) CXTPButton::OnCommand(wParam, lParam);

	int num = (int) layers.GetCount();

	int id = ID_MENUPOPUP_L1;
	for (int i = 0 ; i < num ; i++) {
	CRGLLayer *lP = layers.GetAt(i);
	if (i == ((int) wParam - id)) lP->launchParameterDialog();
	}
	}*/

	return CXTPButton::OnCommand(wParam, lParam);
}

int CGraphButton::OnCreate(LPCREATESTRUCT lpCreateStruct)
{
	if (CXTPButton::OnCreate(lpCreateStruct) == -1)
		return -1;

	// TODO:  Add your specialized creation code here
	GetClientRect(&clientRect);
	if (!doneInitDependentResources) {
		HRESULT res = CreateDeviceResources();
		if (SUCCEEDED(res)) doneInitDependentResources = true;
		else ASSERT((int) res);
	}

	CString txt;
	int num = txt.LoadString(IDS_TOOLTIP_TEXT);

	if (!m_ttip.Create(this)) {
		TRACE0("Unable to create tip window.");
	}
	else {
		m_ttip.SetFont(&ttFont);
		if (!m_ttip.AddTool(this, LPCTSTR(txt))) {
			TRACE0("Unable to add tip for the control window.");
		}
		else {
			m_ttip.SetTipBkColor(RGB(255, 255, 100));
			m_ttip.Activate(TRUE);
		}
	}
	return 0;
}
//
//bool CGraphButton::getIsOver(CPoint _point,int *_what,int *_where)
//{
//	return false;  // Nothing goes on here
//}

int CGraphButton::getRightCursorX(void)
{
	return 10;  // Nothing happens here
}

int CGraphButton::getLeftCursorX(void)
{
	return 10;  // Nothing happens here
}

BOOL CGraphButton::OnGestureZoom(CPoint ptCenter, long lDelta)
{
	static long initial = 1L;

	//if (0L == lDelta)
	//	return FALSE;

#ifdef _DEBUG
	if (lDelta > 100) {
		beginZoom = true;
	}
#endif

	if (beginZoom) {
		initial = lDelta;
		beginZoom = false;
		return FALSE;
	}

	int numT = (int)tiles.GetCount();
	if (!numT) return FALSE;

	//if (cursorControlX == cursorControl) {
		CRect r;
		GetClientRect(&r);
		float left = plotRectLeftPercent * (r.right - r.left) / 100.0f;
		float right = plotRectRightPercent * (r.right - r.left) / 100.0f;
		float cPos = (float)(ptCenter.x - left) / (right - left);

		float ratio = (float)lDelta / initial;

		CRGLTile *tP0 = tiles.GetAt(0);
		if (!tP0->zoomTime(cPos, ratio)) return FALSE;  // If the first cannot move, then the rest should not be touched either

		for (int i = 1; i < numT; i++) {
			CRGLTile *tP = tiles.GetAt(i);
			tP->zoomTime(cPos, ratio);
		}

		D2D1_RECT_F ddr = tP0->getPlotRectScaled();
		runXAxisWizard(ddr.left, ddr.right);

		NMHDR nmhdr;
		nmhdr.code = scaleX;
		nmhdr.idFrom = GetDlgCtrlID();
		nmhdr.hwndFrom = this->m_hWnd;
		::SendMessage(GetParent()->m_hWnd, messScaleX, GetDlgCtrlID(), (LPARAM)&nmhdr);
		//render();
	//}
	//if (cursorControlY == cursorControl) {
	//
	//	float ratio = (float)lDelta / initial;
	//
	//	CRGLTile *tP0 = tiles.GetAt(0);
	//	if (!tP0->scaleVert(ratio, dualPolarity)) return FALSE;  // If the first cannot scale, then the rest should not be touched either
	//
	//	for (int i = 1; i < numT; i++) {
	//		CRGLTile *tP = tiles.GetAt(i);
	//		tP->scaleVert(ratio, dualPolarity);
	//	}
	//
	//	D2D1_RECT_F ddr = tP0->getPlotRectScaled();
	//	runYAxisWizard(ddr.bottom, ddr.top);
	//
	//	NMHDR nmhdr;
	//	nmhdr.code = scaleY;
	//	nmhdr.idFrom = GetDlgCtrlID();
	//	nmhdr.hwndFrom = this->m_hWnd;
	//	::SendMessage(GetParent()->m_hWnd, messScaleY, GetDlgCtrlID(), (LPARAM)&nmhdr);
	//	render();
	//}

	return TRUE;
}

BOOL CGraphButton::OnGesturePan(CPoint ptFrom, CPoint ptTo)
{
	ScreenToClient(&ptTo);
	ScreenToClient(&ptFrom);

	shiftTime(ptTo.x - ptFrom.x);

	return TRUE;
}

BOOL CGraphButton::OnGestureTwoFingerTap(CPoint ptCenter)
{
	autoRange();
	render();
	::PostMessage(GetParent()->m_hWnd, messScaleX, GetDlgCtrlID(), 0L);
	::PostMessage(GetParent()->m_hWnd, messScaleY, GetDlgCtrlID(), 0L);
	return TRUE;
}

BOOL CGraphButton::OnGesturePressAndTap(CPoint ptPress, long lDelta)
{
	return TRUE;
}

float CGraphButton::getYAutoMax(void)
{
	return 10.0f; // This is a virtual function. This is for the compiler
}

float CGraphButton::getYAutoMin(void)
{
	return .0f; // This is a virtual function. This is for the compiler
}

/*
Description: Do a render first to make sure geometries are computed, the  autorange
Check if data are present and if m_target is valid
*/
void CGraphButton::renderAndAutoRange(void)
{
	if (!getHasData()) return;

	if (!m_target) return;
	render();  // Must do this to prep the graphics system
	autoRange();
}

void CGraphButton::autoRange(void)
{
	if (!m_target) return;

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
			((CRGLAxes *)lP)->yAxisWizard(dualPolarity ? -yNewMax : yNewMin, yNewMax);
		}
		if ((RGLLayerTypeGraphSparse == type) || (RGLLayerTypeGraph == type)) {
			UINT id, type, family;
			((CRGLGraph *)lP)->getIDTypeFamily(&id, &type, &family);

			int numT = (int)tiles.GetCount();
			if (numT) {

				CRGLTile *tP = tiles.GetAt(0);
				tP->setPlotRectXScaled(start, stop);
				tP->setPlotRectYScaled(dualPolarity ? -yNewMax : yNewMin, yNewMax);
				for (int i = 1; i < numT; i++) {
					CRGLTile *tP = tiles.GetAt(i);
					tP->setPlotRectXScaled(start, stop);
					tP->setPlotRectYScaled(dualPolarity ? -yNewMax : yNewMin, yNewMax);
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
				tP->setPlotRectYScaled(dualPolarity ? -yNewMax : yNewMin, yNewMax);
				for (int i = 1; i < numT; i++) {
					CRGLTile *tP = tiles.GetAt(i);
					tP->setPlotRectXScaled(start, stop);
					tP->setPlotRectYScaled(dualPolarity ? -yNewMax : yNewMin, yNewMax);
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
				tP->setPlotRectYScaled(dualPolarity ? -yNewMax : yNewMin, yNewMax);
				for (int i = 1; i < numT; i++) {
					CRGLTile *tP = tiles.GetAt(i);
					tP->setPlotRectXScaled(start, stop);
					tP->setPlotRectYScaled(dualPolarity ? -yNewMax : yNewMin, yNewMax);
				}
			}
		}
	}
	render();
}
////////////////////////////

CRGLBar::CRGLBar(CString _regSection /* = _T("Bar") */) :
	regSection(_regSection)
{
	drb = 1.0f;
	dgb = dbb = .0f;
	dtransparencyb = 1.0f;
	type = RGLLayerTypeBar;

	midPoint = 50.0f;	//!< %
	width = 10.0f;		//!< %
	amplitude = .0f;	//!< Eng unit

	dGeometry = NULL;
	dTransformedGeometry = NULL;

	m_brush = NULL;

	if (!regSection.IsEmpty()) {
		CXTRegistryManager *reg = new CXTRegistryManager;

		//---Test registry
		double dtest;
		int OKt = reg->GetProfileDouble(regSection, _T("Red"), &dtest);
		if (!OKt) {
			delete reg;
			reg = new CXTRegistryManager(HKEY_LOCAL_MACHINE);
		}

		reg->GetProfileDouble(regSection, _T("Red"), &drb);
		reg->GetProfileDouble(regSection, _T("Green"), &dgb);
		reg->GetProfileDouble(regSection, _T("Blue"), &dbb);
		reg->GetProfileDouble(regSection, _T("Transparency"), &dtransparencyb);
		delete reg;
	}
}

CRGLBar::~CRGLBar()
{
	DiscardDeviceResources();

	if (!regSection.IsEmpty()) {
		CXTRegistryManager *reg = new CXTRegistryManager;
		reg->WriteProfileDouble(regSection, _T("Red"), &drb);
		reg->WriteProfileDouble(regSection, _T("Green"), &dgb);
		reg->WriteProfileDouble(regSection, _T("Blue"), &dbb);
		reg->WriteProfileDouble(regSection, _T("Transparency"), &dtransparencyb);
		delete reg;
	}
}

void CRGLBar::render(void)
{
	if (!show) return;

	CRGLLayer::render();

	if (!mp_target) return;
	if (!m_brush) return;
	if (!myTile) return;

	D2D1_RECT_F plotRectScaled = myTile->getPlotRectScaled();

	D2D1_POINT_2F p0 = D2D1::Point2F(plotRectScaled.left, plotRectScaled.top);
	D2D1_POINT_2F p1 = D2D1::Point2F(plotRectScaled.right, plotRectScaled.bottom);
	p0 = p0 * matPlotRect;
	p1 = p1 * matPlotRect;
	clipRect = D2D1::RectF(p0.x, p0.y, p1.x, p1.y);
	mp_target->PushAxisAlignedClip(clipRect, D2D1_ANTIALIAS_MODE_PER_PRIMITIVE);

	mp_target->FillGeometry(dTransformedGeometry, m_brush);

	mp_target->PopAxisAlignedClip();
}

void CRGLBar::CreateDeviceResources(ID2D1HwndRenderTarget *hwndRenderTarget, ID2D1Factory *_factory)
{
	CRGLLayer::CreateDeviceResources(hwndRenderTarget, _factory);

	if (m_brush) m_brush.Release();
	m_brush = NULL;
	HRESULT hr = mp_target->CreateSolidColorBrush(D2D1::ColorF((float)drb, (float)dgb, (float)dbb,
		(float)dtransparencyb), &m_brush);

	makeSourceGeometry();
	makeTransformedGeometry();
}


void CRGLBar::setBarGeometry(float _mid, float _width)
{
	midPoint = _mid;	//!< %
	width = _width;		//!< %
}

void CRGLBar::setData(FLOAT _amplitude)
{
	amplitude = _amplitude;

	makeSourceGeometry();
	makeTransformedGeometry();
}


void CRGLBar::doGeometry(void)
{
	makeSourceGeometry();
	makeTransformedGeometry();
}

void CRGLBar::DiscardDeviceResources(void)
{
	if (m_brush) m_brush.Release();
	m_brush = NULL;

	if (dGeometry) dGeometry->Release();
	dGeometry = NULL;
	if (dTransformedGeometry) dTransformedGeometry->Release();
	dTransformedGeometry = NULL;
}

bool CRGLBar::isOver(CPoint _point)
{
	D2D1_POINT_2F p = D2D1::Point2F((float)_point.x, (float)_point.y);

	if ((p.x < clipRect.left) || (p.x > clipRect.right)) return false;
	if ((p.y < clipRect.top) || (p.y > clipRect.bottom)) return false;

	int ret = FALSE;
	D2D1::Matrix3x2F mat = D2D1::Matrix3x2F::Identity();

	if (dTransformedGeometry) dTransformedGeometry->StrokeContainsPoint(p, 3.0f, NULL, mat, &ret);

	return ret ? true : false;
}

void CRGLBar::launchParameterDialog(void)
{
}

void CRGLBar::setColour(double _r, double _g, double _b)
{
	drb = _r;
	dgb = _g;
	dbb = _b;

	if (m_brush) m_brush.Release();
	m_brush = NULL;
	HRESULT hr = mp_target->CreateSolidColorBrush(D2D1::ColorF((float)drb, (float)dgb, (float)dbb,
		(float)dtransparencyb), &m_brush);
}


void CRGLBar::setTransparency(float _transp)
{
	dtransparencyb = _transp;

	if (m_brush) m_brush.Release();
	m_brush = NULL;
	HRESULT hr = mp_target->CreateSolidColorBrush(D2D1::ColorF((float)drb, (float)dgb, (float)dbb,
		(float)dtransparencyb), &m_brush);
}

void CRGLBar::resize(void)
{
	CRGLLayer::resize();
	if (!mp_factory) return;
	if (!dGeometry) return;

	makeTransformedGeometry();
}

void CRGLBar::processFullRect(void)
{
	CRGLLayer::processFullRect();
}

void CRGLBar::processPlotRect(void)
{
	CRGLLayer::processPlotRect();
	if (myTile) {
		int changeFlag = myTile->getChangeFlag();
		if (changeFlag & RGLTileChangeXScaled) makeSourceGeometry(); // Refill source Geometry
	}
	makeTransformedGeometry();
}

void CRGLBar::clearData(void)
{
	if (dGeometry) dGeometry->Release();
	dGeometry = NULL;
	if (dTransformedGeometry) dTransformedGeometry->Release();
	dTransformedGeometry = NULL;
}

void CRGLBar::makeTransformedGeometry(void)
{
	if (!mp_factory) return;

	if (dGeometry) {
		if (dTransformedGeometry) dTransformedGeometry->Release();
		dTransformedGeometry = NULL;

		HRESULT hr = mp_factory->CreateTransformedGeometry(dGeometry, matPlotRect, &dTransformedGeometry);
	}
}

void CRGLBar::makeSourceGeometry(void)
{
	if (!myTile) return;

	//---Check if we have to reload the source geometry
	D2D1_RECT_F plotRectScaled = myTile->getPlotRectScaled();

	if (!mp_factory) return;

	FLOAT dist = plotRectScaled.right - plotRectScaled.left;
	FLOAT xm = plotRectScaled.left + dist * midPoint / 100.0f;
	FLOAT x1 = xm - dist * width / 200.0f;
	FLOAT x2 = xm + dist * width / 200.0f;

	if (dGeometry) dGeometry->Release();
	dGeometry = NULL;

	ID2D1GeometrySink *sink = NULL;

	mp_factory->CreatePathGeometry(&dGeometry);

	if (dGeometry) {
		dGeometry->Open(&sink);
		if (sink) {

			D2D1_POINT_2F d2d1p;
			d2d1p.x = x1;
			d2d1p.y = .0f;
			sink->BeginFigure(d2d1p, D2D1_FIGURE_BEGIN_FILLED);

			d2d1p.x = x2;
			d2d1p.y = .0f;
			sink->AddLine(d2d1p);

			d2d1p.x = x2;
			d2d1p.y = amplitude;
			sink->AddLine(d2d1p);

			d2d1p.x = x1;
			d2d1p.y = amplitude;
			sink->AddLine(d2d1p);

			d2d1p.x = x1;
			d2d1p.y = .0f;
			sink->AddLine(d2d1p);
			sink->EndFigure(D2D1_FIGURE_END_CLOSED);

			sink->Close();
			sink->Release();
			sink = NULL;
		}
	}
}

LRESULT CGraphButton::OnNcHitTest(CPoint point)
{
	// TODO: Add your message handler code here and/or call default
	return CXTPButton::OnNcHitTest(point);
}


void CGraphButton::OnContextMenu(CWnd* pWnd, CPoint point)
{
	// TODO: Add your message handler code here
	return; // No longer using this

#ifdef IN_SPIRO_ANALYSIS

	if (enableEditableWindowMouseControl) return;

	if (!eventPointedTo) return;  // Add condition for cursor window

	CMenu menu;
	menu.CreatePopupMenu();
	CString s;

	CString su, sl, sm, sun;
	int nums;
	nums = su.LoadString(IDS_UPPER2);
	nums = sl.LoadString(IDS_LOWER2);
	nums = sm.LoadString(IDS_MULTILEVEL2);
	nums = sun.LoadString(IDS_UNDEF2);

	nums = s.LoadString(IDS_CLEAR_EVENT);
	menu.AppendMenu(MF_STRING, ID_DEF_CLEAR_EVENT, s);

	//---Mixed
	CMenu mixMenu;
	mixMenu.CreatePopupMenu();
	mixMenu.AppendMenu(MF_STRING, ID_DEF_UPPER_MIX, su);
	mixMenu.AppendMenu(MF_STRING, ID_DEF_MULTI_MIX, sm);
	mixMenu.AppendMenu(MF_STRING, ID_DEF_LOWER_MIX, sl);
	mixMenu.AppendMenu(MF_STRING, ID_DEF_UNDEF_MIX, sun);

	nums = s.LoadString(IDS_MIXED2);
	menu.AppendMenu(MF_POPUP, (UINT_PTR)mixMenu.m_hMenu, s);

	CString sc;
	nums = sc.LoadString(IDS_CENTRAL2);
	menu.AppendMenu(MF_STRING, ID_DEF_CENTRAL, sc);

	//---Obstructive
	CMenu obstrMenu;
	obstrMenu.CreatePopupMenu();
	obstrMenu.AppendMenu(MF_STRING, ID_DEF_UPPER_OBSTR, su);
	obstrMenu.AppendMenu(MF_STRING, ID_DEF_MULTI_OBSTR, sm);
	obstrMenu.AppendMenu(MF_STRING, ID_DEF_LOWER_OBSTR, sl);
	obstrMenu.AppendMenu(MF_STRING, ID_DEF_UNDEF_OBSTR, sun);
	CString so;
	nums = so.LoadString(IDS_OBSTRUCTIVE3);
	menu.AppendMenu(MF_POPUP, (UINT_PTR)obstrMenu.m_hMenu, so);

	//---Hypopnea
	CMenu obstrHypoMenu;
	obstrHypoMenu.CreateMenu();
	obstrHypoMenu.AppendMenu(MF_STRING, ID_DEF_UPPER_OBSTRHYPO, su);
	obstrHypoMenu.AppendMenu(MF_STRING, ID_DEF_MULTI_OBSTRHYPO, sm);
	obstrHypoMenu.AppendMenu(MF_STRING, ID_DEF_LOWER_OBSTRHYPO, sl);
	obstrHypoMenu.AppendMenu(MF_STRING, ID_DEF_UNDEF_OBSTRHYPO, sun);
	nums = s.LoadString(IDS_OBSTRHYPOPNEA2);
	menu.AppendMenu(MF_POPUP, (UINT_PTR)obstrHypoMenu.m_hMenu, s);
	CString chy;
	nums = chy.LoadString(IDS_CENTRALHYPOPNEA2);
	menu.AppendMenu(MF_STRING, ID_DEF_CENTRALHYPOPNEA, chy);

	//---Oxygen Desaturations pos 6
	nums = s.LoadString(IDS_SpO2DROP2);
	menu.AppendMenu(MF_STRING, ID_DEF_SPO2DROP, s);

	//---Arousal pos 7
	nums = s.LoadString(IDS_AROUSAL2);
	menu.AppendMenu(MF_STRING, ID_DEF_AROUSAL, s);

	//---RERA pos 8
	CMenu reraMenu;
	reraMenu.CreatePopupMenu();
	reraMenu.AppendMenu(MF_STRING, ID_DEF_UPPER_RERA, su);
	reraMenu.AppendMenu(MF_STRING, ID_DEF_MULTI_RERA, sm);
	reraMenu.AppendMenu(MF_STRING, ID_DEF_LOWER_RERA, sl);
	reraMenu.AppendMenu(MF_STRING, ID_DEF_UNDEF_RERA, sun);
	nums = s.LoadString(IDS_RERA2);
	menu.AppendMenu(MF_POPUP, (UINT_PTR)reraMenu.m_hMenu, s);

	//---Snoring pos 9
	CMenu snoreMenu;
	snoreMenu.CreatePopupMenu();
	snoreMenu.AppendMenu(MF_STRING, ID_DEF_UPPER_SNORING, su);
	snoreMenu.AppendMenu(MF_STRING, ID_DEF_MULTI_SNORING, sm);
	snoreMenu.AppendMenu(MF_STRING, ID_DEF_LOWER_SNORING, sl);
	snoreMenu.AppendMenu(MF_STRING, ID_DEF_UNDEF_SNORING, sun);
	nums = s.LoadString(IDS_SNORING2);
	menu.AppendMenu(MF_POPUP, (UINT_PTR)snoreMenu.m_hMenu, s);

	//---Awake pos 10
	nums = s.LoadString(IDS_AWAKE2);
	menu.AppendMenu(MF_STRING, ID_DEF_AWAKE, s);

	//---Excluded pos 11
	nums = s.LoadString(IDS_EXCLUDED2);
	menu.AppendMenu(MF_STRING, ID_DEF_EXCLUDED, s);

	if (eventPointedTo) {
		UINT type = eventPointedTo->getEventType();
		switch (type) {
		case evntTypeCentral:
			menu.CheckMenuItem(ID_DEF_CENTRAL, MF_BYCOMMAND | MF_CHECKED);
			break;
		case evntTypeArousal:
			menu.CheckMenuItem(ID_DEF_AROUSAL, MF_BYCOMMAND | MF_CHECKED);
			break;
		case evntTypeRERA:
			menu.CheckMenuItem(8, MF_BYPOSITION | MF_CHECKED);
			{
				UINT level = eventPointedTo->getLevelType();
				switch (level) {
				case levelTypeUpper:
					menu.CheckMenuItem(ID_DEF_UPPER_RERA, MF_BYCOMMAND | MF_CHECKED);
					break;
				case levelTypeLower:
					menu.CheckMenuItem(ID_DEF_LOWER_RERA, MF_BYCOMMAND | MF_CHECKED);
					break;
				case levelTypeMulti:
					menu.CheckMenuItem(ID_DEF_MULTI_RERA, MF_BYCOMMAND | MF_CHECKED);
					break;
				case levelTypeUndef:
					menu.CheckMenuItem(ID_DEF_UNDEF_RERA, MF_BYCOMMAND | MF_CHECKED);
					break;
				}
			}
			break;
		case evntTypeAwake:
			menu.CheckMenuItem(ID_DEF_AWAKE, MF_BYCOMMAND | MF_CHECKED);
			break;
		case evntTypeSnoring:
			menu.CheckMenuItem(9, MF_BYPOSITION | MF_CHECKED);
			{
				UINT level = eventPointedTo->getLevelType();
				switch (level) {
				case levelTypeUpper:
					menu.CheckMenuItem(ID_DEF_UPPER_SNORING, MF_BYCOMMAND | MF_CHECKED);
					break;
				case levelTypeLower:
					menu.CheckMenuItem(ID_DEF_LOWER_SNORING, MF_BYCOMMAND | MF_CHECKED);
					break;
				case levelTypeMulti:
					menu.CheckMenuItem(ID_DEF_MULTI_SNORING, MF_BYCOMMAND | MF_CHECKED);
					break;
				case levelTypeUndef:
					menu.CheckMenuItem(ID_DEF_UNDEF_SNORING, MF_BYCOMMAND | MF_CHECKED);
					break;
				}
			}
			break;
		case evntTypeExcluded:
			menu.CheckMenuItem(ID_DEF_EXCLUDED, MF_BYCOMMAND | MF_CHECKED);
			break;
		case evntTypeObstr:
			menu.CheckMenuItem(3, MF_BYPOSITION | MF_CHECKED);
			{
				UINT level = eventPointedTo->getLevelType();
				switch (level) {
				case levelTypeUpper:
					menu.CheckMenuItem(ID_DEF_UPPER_OBSTR, MF_BYCOMMAND | MF_CHECKED);
					break;
				case levelTypeLower:
					menu.CheckMenuItem(ID_DEF_LOWER_OBSTR, MF_BYCOMMAND | MF_CHECKED);
					break;
				case levelTypeMulti:
					menu.CheckMenuItem(ID_DEF_MULTI_OBSTR, MF_BYCOMMAND | MF_CHECKED);
					break;
				case levelTypeUndef:
					menu.CheckMenuItem(ID_DEF_UNDEF_OBSTR, MF_BYCOMMAND | MF_CHECKED);
					break;
				}
			}
			break;
		case evntTypeHypoObstr:
		case evntTypeHypoCentral:		{
			UINT subtype = ((CHypoEvnt *)eventPointedTo)->getHypoType();
			if (hypoTypeCentral == subtype) {
				menu.CheckMenuItem(5, MF_BYPOSITION | MF_CHECKED);
			}
			if (hypoTypeObstr == subtype) {
				menu.CheckMenuItem(4, MF_BYPOSITION | MF_CHECKED);
				UINT level = eventPointedTo->getLevelType();
				switch (level) {
				case levelTypeUpper:
					menu.CheckMenuItem(ID_DEF_UPPER_OBSTRHYPO, MF_BYCOMMAND | MF_CHECKED);
					break;
				case levelTypeLower:
					menu.CheckMenuItem(ID_DEF_LOWER_OBSTRHYPO, MF_BYCOMMAND | MF_CHECKED);
					break;
				case levelTypeMulti:
					menu.CheckMenuItem(ID_DEF_MULTI_OBSTRHYPO, MF_BYCOMMAND | MF_CHECKED);
					break;
				case levelTypeUndef:
					menu.CheckMenuItem(ID_DEF_UNDEF_OBSTRHYPO, MF_BYCOMMAND | MF_CHECKED);
					break;
				}
			}
		}
		break;
		case evntTypeMixed:
			menu.CheckMenuItem(1, MF_BYPOSITION | MF_CHECKED);
			{
				UINT level = eventPointedTo->getLevelType();
				switch (level) {
				case levelTypeUpper:
					menu.CheckMenuItem(ID_DEF_UPPER_MIX, MF_BYCOMMAND | MF_CHECKED);
					break;
				case levelTypeLower:
					menu.CheckMenuItem(ID_DEF_LOWER_MIX, MF_BYCOMMAND | MF_CHECKED);
					break;
				case levelTypeMulti:
					menu.CheckMenuItem(ID_DEF_MULTI_MIX, MF_BYCOMMAND | MF_CHECKED);
					break;
				case levelTypeUndef:
					menu.CheckMenuItem(ID_DEF_UNDEF_MIX, MF_BYCOMMAND | MF_CHECKED);
					break;
				}
			}
			break;
		case evntTypeSpO2Drop:
			menu.CheckMenuItem(ID_DEF_SPO2DROP, MF_BYCOMMAND | MF_CHECKED);
			break;
		}
	}

	int mm = GetSystemMetrics(SM_MENUDROPALIGNMENT);
	int retVal = menu.TrackPopupMenu((mm ? TPM_RIGHTALIGN : TPM_LEFTALIGN) | TPM_RIGHTBUTTON, point.x, point.y, this);

#endif 	
}


BOOL CGraphButton::OnMouseWheel(UINT nFlags, short zDelta, CPoint pt)
{
	// TODO: Add your message handler code here and/or call default
	if (0 == timerID)
		timerID = SetTimer(MWHEEL_TIMER, MWHEEL_TIMER, NULL);

	wheelRunning = true;
	wheelTimerCounter = 0;
	scaleTimeAboutCentre(-zDelta);
	::PostMessage(GetParent()->m_hWnd, messScaleX, GetDlgCtrlID(), 0L);

	return CXTPButton::OnMouseWheel(nFlags, zDelta, pt);
}

void CGraphButton::OnTimer(UINT_PTR nIDEvent)
{
	// TODO: Add your message handler code here and/or call default
	if (wheelRunning) {
		if (wheelTimerCounter++ > 5) {
			wheelTimerCounter = 0;
			wheelRunning = false;
			::PostMessage(GetParent()->m_hWnd, messMoveX, GetDlgCtrlID(), 0L);
		}
	}
	CXTPButton::OnTimer(nIDEvent);
}

/////////////////////////////

CRGLGraphEventMarkers::CRGLGraphEventMarkers(CString _regSection /* = _T("Graph Points") */) :
	yVal(1 - .0f), eventEnum(evExcluded), withLevel(false),
	CRGLGraph(_regSection)
{
	int num = text.LoadString(IDS_EVENT2);
	type = RGLLayerTypeGraphPoints;
	dtransparency = 1.0f;
}

CRGLGraphEventMarkers::~CRGLGraphEventMarkers()
{
}

void CRGLGraphEventMarkers::setEventType(UINT _type, bool _withLevel)
{
	int num = text.LoadString(_type);
	eventEnum = _type;
	withLevel = _withLevel;
}

void CRGLGraphEventMarkers::makeTransformedGeometry(void)
{
	if (!mp_factory) return;

	if (dGeometry) {
		if (dTransformedGeometry) dTransformedGeometry->Release();
		dTransformedGeometry = NULL;

		HRESULT hrPos = mp_factory->CreateTransformedGeometry(dGeometry, matPlotRect, &dTransformedGeometry);
	}
}

void CRGLGraphEventMarkers::makeSourceGeometry(bool _reload)
{
	if (!myTile) return;

	if (!mp_factory) return;

	if (!eventsP) return;

	int majorVersion = 0;
	int minorVersion = 0;
	eventsP->getVersion(&majorVersion, &minorVersion);

	vector <float> *ssP1, *ssP2, *ssP3;
	if ((20 < majorVersion) && (evSnoringAllLevels == eventEnum) && (withLevel)) {
		ssP1 = eventsP->getBeginsEnds(eventEnum,0);
		ssP2 = NULL;
		ssP3 = NULL;
	}
	else {
		if (withLevel) {
			if (!eventsP->getAreEventsRespBeltBased()) {
				ssP1 = eventsP->getBeginsEnds(eventEnum, bgLevelTypeUpper);
				ssP2 = eventsP->getBeginsEnds(eventEnum, bgLevelTypeMulti);
				ssP3 = eventsP->getBeginsEnds(eventEnum, bgLevelTypeLower);
			}
			else {
				ssP1 = eventsP->getBeginsEnds(eventEnum, bgLevelTypeUndef);
				ssP2 = NULL;
				ssP3 = NULL;
			}
		}
		else {
			ssP1 = eventsP->getBeginsEnds(eventEnum);
			ssP2 = NULL;
			ssP3 = NULL;
		}
	}

	//---Check if we have to reload the source geometry
	D2D1_RECT_F plotRectScaled = myTile->getPlotRectScaled();
	if ((plotRectScaled.left > oldSourceGeometryLeft) && (plotRectScaled.right < oldSourceGeometryRight)) {
		return;
	}

	FLOAT dist = plotRectScaled.right - plotRectScaled.left;

	//---Envelopes
	if (dGeometry) dGeometry->Release();
	dGeometry = NULL;

	ID2D1GeometrySink *sink = NULL;

	mp_factory->CreatePathGeometry(&dGeometry);
	if (dGeometry) {
		dGeometry->Open(&sink);
		if (sink) {
			D2D1_POINT_2F d2d1p_0, d2d1p_1;
			d2d1p_0.y = yVal;
			d2d1p_1.y = yVal;
			if ((ssP1) && (ssP1->size() >= 2)) {
				vector <float>::iterator i1 = ssP1->begin();
				vector <float>::iterator i2 = ssP1->end();
				i2--;
				do {
					d2d1p_0.x = *i1;
					i1++;
					d2d1p_1.x = *i1;
					i1++;
					sink->BeginFigure(d2d1p_0, D2D1_FIGURE_BEGIN_HOLLOW);
					sink->AddLine(d2d1p_1);
					sink->EndFigure(D2D1_FIGURE_END_OPEN);
				} while (i1 < i2);
			}
			if ((ssP2) && (ssP2->size() >= 2)) {
				vector <float>::iterator i1 = ssP2->begin();
				vector <float>::iterator i2 = ssP2->end();
				i2--;
				do {
					d2d1p_0.x = *i1;
					i1++;
					d2d1p_1.x = *i1;
					i1++;
					sink->BeginFigure(d2d1p_0, D2D1_FIGURE_BEGIN_HOLLOW);
					sink->AddLine(d2d1p_1);
					sink->EndFigure(D2D1_FIGURE_END_OPEN);
				} while (i1 < i2);
			}
			if ((ssP3) && (ssP3->size() >= 2)) {
				vector <float>::iterator i1 = ssP3->begin();
				vector <float>::iterator i2 = ssP3->end();
				i2--;
				do {
					d2d1p_0.x = *i1;
					i1++;
					d2d1p_1.x = *i1;
					i1++;
					sink->BeginFigure(d2d1p_0, D2D1_FIGURE_BEGIN_HOLLOW);
					sink->AddLine(d2d1p_1);
					sink->EndFigure(D2D1_FIGURE_END_OPEN);
				} while (i1 < i2);
			}
			sink->Close();
			sink->Release();
			sink = NULL;
		}
	}
}

void CRGLGraphEventMarkers::render(void)
{
	if (!show) return;

	CRGLLayer::render();

	if (!mp_target) return;

	if (!m_brush) {
		HRESULT hr = mp_target->CreateSolidColorBrush(D2D1::ColorF((float)dr, (float)dg, (float)db,
			(float)dtransparency), &m_brush);
	}
	if (!myTile) return;

	if (!dTransformedGeometry) {
		if (!dGeometry) makeSourceGeometry();
		makeTransformedGeometry();
	}
	
	D2D1_RECT_F plotRectScaled = myTile->getPlotRectScaled();

	D2D1_POINT_2F p0 = D2D1::Point2F(plotRectScaled.left, plotRectScaled.top);
	D2D1_POINT_2F p1 = D2D1::Point2F(plotRectScaled.right, plotRectScaled.bottom);
	p0 = p0 * matPlotRect;
	p1 = p1 * matPlotRect;
	clipRect = D2D1::RectF(p0.x, p0.y, p1.x, p1.y);
	mp_target->PushAxisAlignedClip(clipRect, D2D1_ANTIALIAS_MODE_PER_PRIMITIVE);

	FLOAT dist = plotRectScaled.right - plotRectScaled.left;

	bool doPlot = true;
	if ((upperTimeSpanLimit >= .0f) && (dist > upperTimeSpanLimit)) doPlot = false;
	if ((lowerTimeSpanLimit >= .0f) && (dist < lowerTimeSpanLimit)) doPlot = false;

	if (doPlot && dTransformedGeometry) {
		mp_target->DrawGeometry(dTransformedGeometry, m_brush,(float)dthickness);
	}

	mp_target->PopAxisAlignedClip();
	mp_target->Flush();
}

void CRGLGraphEventMarkers::setYVal(const float _yVal)
{
	yVal = _yVal;
}

bool CRGLGraphEventMarkers::isOver(CPoint _point)
{
	D2D1_POINT_2F p = D2D1::Point2F((float)_point.x, (float)_point.y);

	if ((p.x < clipRect.left) || (p.x > clipRect.right)) return false;
	if ((p.y < clipRect.top) || (p.y > clipRect.bottom)) return false;

	int ret = FALSE;
	D2D1::Matrix3x2F mat = D2D1::Matrix3x2F::Identity();

	if (dTransformedGeometry) dTransformedGeometry->StrokeContainsPoint(p, 2.0f * SPACE_FOR_EVENT_MARKERS, NULL, mat, &ret);

	return ret ? true : false;
}

//////////////////////////////////////////

CRGLGraphExcludedEventMarker::CRGLGraphExcludedEventMarker(CString _regSection)
{
	dthickness = 20.0f;
	dtransparency = .5f;
}

CRGLGraphExcludedEventMarker::~CRGLGraphExcludedEventMarker()
{
}

void CRGLGraphExcludedEventMarker::makeSourceGeometry(bool _reload)
{
	if (!myTile) return;

	if (!mp_factory) return;

	if (!eventsP) return;

	vector <float> *ssP1;
	ssP1 = eventsP->getBeginsEnds(eventEnum);

	//---Check if we have to reload the source geometry
	D2D1_RECT_F plotRectScaled = myTile->getPlotRectScaled();
	if ((plotRectScaled.left > oldSourceGeometryLeft) && (plotRectScaled.right < oldSourceGeometryRight)) {
		return;
	}

	FLOAT dist = plotRectScaled.right - plotRectScaled.left;

	//---Envelopes
	if (dGeometry) dGeometry->Release();
	dGeometry = NULL;

	ID2D1GeometrySink *sink = NULL;

	mp_factory->CreatePathGeometry(&dGeometry);
	if (dGeometry) {
		dGeometry->Open(&sink);
		if (sink) {
			D2D1_POINT_2F d2d1p_0, d2d1p_1;
			d2d1p_0.y = yVal;
			d2d1p_1.y = yVal;
			if ((ssP1) && (ssP1->size() >= 2)) {
				vector <float>::iterator i1 = ssP1->begin();
				vector <float>::iterator i2 = ssP1->end();
				i2--;
				do {
					d2d1p_0.x = *i1;
					i1++;
					d2d1p_1.x = *i1;
					i1++;
					sink->BeginFigure(d2d1p_0, D2D1_FIGURE_BEGIN_HOLLOW);
					sink->AddLine(d2d1p_1);
					sink->EndFigure(D2D1_FIGURE_END_OPEN);
				} while (i1 < i2);
			}
			sink->Close();
			sink->Release();
			sink = NULL;
		}
	}
}
