// GraphSettings.cpp : implementation file
//


/*!
* @author		RKH
* @date		06.06.2017
* @copyright (C)Spiro Medical AS 2013 - 2017
*/

#include "stdafx.h"
#include "GraphSettings.h"
#include "afxdialogex.h"
#include "RegDefs.h"
#include "CommonResource.h"
#include "AGSMessages.h"


// CGraphSettings dialog

IMPLEMENT_DYNAMIC(CGraphSettings, CXTResizeDialog)

CGraphSettings::CGraphSettings(CWnd* pParent /*=NULL*/)
	: myCreator(pParent),CXTResizeDialog(CGraphSettings::IDD, pParent)
{

}

CGraphSettings::~CGraphSettings()
{
}

void CGraphSettings::DoDataExchange(CDataExchange* pDX)
{
	CXTResizeDialog::DoDataExchange(pDX);
}


BEGIN_MESSAGE_MAP(CGraphSettings, CXTResizeDialog)
	ON_MESSAGE(XTPWM_PROPERTYGRID_NOTIFY, &CGraphSettings::OnGridNotify)
END_MESSAGE_MAP()


// CGraphSettings message handlers



LRESULT CGraphSettings::OnGridNotify(WPARAM wParam, LPARAM lParam)
{
	return 0L;
}

bool CGraphSettings::getUseThis(CString const _s)
{
	//if (BATTERY_VOLTAGE					 == _s) return true;
	//if (BATTERY_CAPACITY				 == _s) return true;
	//if (BATTERY_REMAINING_CAPACITY		 == _s) return true;
	//if (BATTERY_FULL_CHARGE_CAPACITY	 == _s) return true;
	//if (BATTERY_TEMPERATURE				 == _s) return true;
	if (TISSUE_VIBRATION				 == _s) return true; //OK
	//if (BODY_POSITION_XYZ				 == _s) return true;
	if (BODY_POSITION					 == _s) return true; //OK
	//if (BODY_POSITION_X					 == _s) return true;
	//if (BODY_POSITION_Y					 == _s) return true;
	//if (BODY_POSITION_Z					 == _s) return true;
	if (ACTIMETER						 == _s) return true; //OK
	if (KEY_1000HZ						== _s) return true;
	if (KEY_2500HZ						== _s) return true;
	if (KEY_400HZ						== _s) return true;
	if (KEY_160HZ						== _s) return true;
	if (TORSO_ACTIMETER					 == _s) return true; //OK
	if (AIRBORNE_SOUND					 == _s) return true; //---
	if (AIRBORNE_SOUND_F0				 == _s) return true; //---
	if (AIRBORNE_SOUND_F1				 == _s) return true; //---
	if (AIRBORNE_SOUND_F2				 == _s) return true; //---
	if (AIRBORNE_SOUND_F3				 == _s) return true; //---
	if (PULSE_RATE						 == _s) return true; //OK
	if (SpO2							 == _s) return true; //----
	if (PRESSURE_OESOPH					 == _s) return true; //----
	if (PRESSURE_PPHAR					 == _s) return true; //----
	//if (PRESSURE_UPPER_GRADIENT			 == _s) return true;
	//if (PRESSURE_LOWER_GRADIENT			 == _s) return true;
	if (PRESSURE_GRADIENT_RATIO			 == _s) return true; //---
	if (TEMPERATURE_T0					 == _s) return true;
	if (TEMPERATURE_T1					 == _s) return true;
	if (TEMPERATURE_T1_RAW				 == _s) return true;
	if (TEMPERATURE_T0_RAW				 == _s) return true;
	//if (AG200MIC						 == _s) return true;
	if (RESP_FRQ						 == _s) return true;
	if (ADMITTANCE						 == _s) return true;
	if (POES_ENV						 == _s) return true;
	//if (POES_ENV_BASELINE				 == _s) return true;
	if (PPH_ENV							 == _s) return true;
	//if (PPH_ENV_BASELINE				 == _s) return true;
	if (T0_ENV							 == _s) return true; //OK
	if (T1_ENV							 == _s) return true; //OK
	//if (T1_ENV_BASELINE					 == _s) return true;
	if (T0_PLUS_T1						 == _s) return true; //OK
	if (BREATHING_EFFICIENCY			 == _s) return true;
	if (FLOW							 == _s) return true;
	if (FLOW_BASELINE					 == _s) return true;
	//if (GRAPH_EVENTS					 == _s) return true;
	//if (ABDOM_BELT						 == _s) return true;
	//if (CHEST_BELT						 == _s) return true;
	//if (CANNULA							 == _s) return true;
	return false;
}

BOOL CGraphSettings::OnInitDialog()
{
	CXTResizeDialog::OnInitDialog();

	// TODO:  Add extra initialization here
	RECT rect;
	rect.bottom = 30;
	rect.top = 20;
	rect.left = 0;
	rect.right = 10;
	
	DWORD style = 0;
	GetClientRect(&rect);	
	
	if (!m_gProps.Create(rect,this,ID_GRAPH_PROPS_GRID)) {
		AfxMessageBox(_T("Cannot create Graph Properties Dialog"));
	}
	else {
		m_gProps.ShowWindow(TRUE);
	}
	
	m_gProps.SetTheme(xtpGridThemeCool); //NativeWinXP);
	m_gProps.SetCustomColors( RGB(134, 207, 242),	// Help Back
		             RGB(  0,   0,   0),	// Help Fore
				     RGB(134, 207, 242),	// View Line
					 RGB(255, 255, 255),	// View back
					 RGB(  0,   0,   0));	// Category Fore

	CXTPRegistryManager *reg = new CXTPRegistryManager;
	reg->SetRegistryKey(COMPANY_NAME,KEY_GRAPHICS);
	CStringArray keyArray;

	/*
	CXTPRegistryManager *reg = new CXTPRegistryManager;
	reg->SetRegistryKey(COMPANY_NAME,KEY_GRAPHICS);

	//---Test registry
	double dblTest;
	BOOL OKTest = reg->GetProfileDouble(key,THICKNESS,&dblTest);
	if (!OKTest) {
	delete reg;
	reg = new CXTPRegistryManager(HKEY_LOCAL_MACHINE);
	reg->SetRegistryKey(COMPANY_NAME,KEY_GRAPHICS);
	}

	bool ret = true;
	BOOL OK = reg->GetProfileColor(key,CURVE_COLOUR,&col);
	ret = FALSE == OK ? false : ret;
	transparencyPerc = (long) reg->GetProfileInt(key,TRANSPARENCY,100);
	double dbl;
	OK = reg->GetProfileDouble(key,THICKNESS,&dbl);
	ret = FALSE == OK ? false : ret;
	thickness = (float) dbl;
	*/
	//---Test registry
	int num = reg->EnumKeys(GRAPH_BUTTONS,keyArray);
	if (0 == num) {
		delete reg;
		reg = new CXTPRegistryManager(HKEY_LOCAL_MACHINE);
		reg->SetRegistryKey(COMPANY_NAME, KEY_GRAPHICS);
		num = reg->EnumKeys(GRAPH_BUTTONS,keyArray);
	}
	delete reg;
	
	for (int i = 0 ; i < num ; i++) {
		CString s = keyArray.GetAt(i);
		if (getUseThis(s)) {
			CButtonPlotSettings *cbp = new CButtonPlotSettings(false, s);
			settingsArray.Add(cbp);

			CXTPPropertyGridItem *cat = m_gProps.AddCategory(cbp->getTitle());

			//CXTPPropertyGridItemColor *item = new CXTPPropertyGridItemColor(IDS_BACKGROUND_COLOUR,cbp->getBackgrColour(),
			//cbp->getBackgrColourP());
			//cat->AddChildItem(item);
			CString curveColourText;
			int nums = curveColourText.LoadString(IDS_CURVE_COLOUR);
			CArray <CCurvePlotSettings *, CCurvePlotSettings *> *curves = cbp->getCurves();
			int numc = curves->GetCount();
			for (int j = 0; j < numc; j++) {
				CString s;

				CCurvePlotSettings *ps = curves->GetAt(j);
				//CXTPPropertyGridItem *cats = cat->AddChildItem(new CXTPPropertyGridItem(ps->getTitle()));
				CString label = ps->getTitle();
				if (!label.IsEmpty()) {
					label += _T(" ");
					label += curveColourText;
					CXTPPropertyGridItemColor *ic = new CXTPPropertyGridItemColor(label, ps->getColour(),ps->getColourP());
					int nums = s.LoadString(IDS_COLOUR_TOOLTIP);
					ic->SetTooltip(s);
					nums = s.LoadString(IDS_COLOUR_DESCRIPTION);
					ic->SetDescription(s);
					//cats->AddChildItem(ic);
					cat->AddChildItem(ic);

					//CXTPPropertyGridItemNumber *in = new CXTPPropertyGridItemNumber(IDS_CURVE_TRANSPARENCY,
					//	ps->getTransparencyPerc(),ps->getTransparencyPercP());
					//cats->AddChildItem(in);
					//in->AddSliderControl();
					//in->AddSpinButton();
					//s.LoadString(IDS_TRANSPARENCY_DESCRIPTION);
					//in->SetDescription(s);
					//s.LoadString(IDS_TRANSPARENCY_TOOLTIP);
					//in->SetTooltip(s);
					//
					//CXTPPropertyGridItemDouble *thickn = new CXTPPropertyGridItemDouble(IDS_CURVE_THICKNESS,
					//	ps->getThickness(),_T("%.0f"),ps->getThicknessP());
					//cats->AddChildItem(thickn);
					//
					//int currentThick = 0;
					//for (int ii = 0 ; ii < MAX_THICKNESS ; ii++) {
					//	CString ts;
					//	ts.Format(_T("%d"),ii + 1);
					//	thickn->GetConstraints()->AddConstraint(ts);
					//	if ((ii + 1) == ps->getThickness()) currentThick = ii;
					//}	
					//thickn->SetFlags(xtpGridItemHasComboButton);
					//thickn->GetConstraints()->SetCurrent(currentThick);
					//s.LoadString(IDS_THICKNESS_DESCRIPTION);
					//thickn->SetDescription(s);
					//s.LoadString(IDS_THICKNESS_TOOLTIP);
					//thickn->SetTooltip(s);
				}
			}
		}
	}

	SetResize(ID_GRAPH_PROPS_GRID,	SZ_TOP_LEFT,		SZ_BOTTOM_RIGHT);
	SetResize(IDOK,					SZ_BOTTOM_RIGHT,	SZ_BOTTOM_RIGHT);

	return TRUE;  // return TRUE unless you set the focus to a control
	// EXCEPTION: OCX Property Pages should return FALSE
}


void CGraphSettings::OnOK()
{
	// TODO: Add your specialized code here and/or call the base class

	//---Save to registry
	if (myCreator) {
		UINT mess = RegisterWindowMessage(AGS_MESSAGE_PLOT_PARAMETERS_CHANGED);
		myCreator->PostMessage(mess,0L,0L);
	}
	CXTResizeDialog::OnOK();
}


BOOL CGraphSettings::DestroyWindow()
{
	// TODO: Add your specialized code here and/or call the base class
	int num = settingsArray.GetCount();
	for (int i = num - 1 ; i >= 0 ; i--) {
		CButtonPlotSettings *cbp = settingsArray.GetAt(i);
		delete cbp;
	}

	return CXTResizeDialog::DestroyWindow();
}
