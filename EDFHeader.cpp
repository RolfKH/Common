// EDFHeader.cpp : implementation file
//

#include "stdafx.h"
#include "EDFHeader.h"
#include "afxdialogex.h"

#include "..\common\agMessages.h"

// CEDFHeader dialog

IMPLEMENT_DYNAMIC(CEDFHeader, CDialog)

CEDFHeader::CEDFHeader(CString _fname,EDF_BINARY_HEADER _binHeader,EDF_BINARY_RECORD *_binRecords,CWnd* pParent /*=NULL*/)
	: fileName(_fname),binHeader(_binHeader),binRecords(_binRecords),
	  CDialog(CEDFHeader::IDD, pParent)
{

}

CEDFHeader::~CEDFHeader()
{
}

void CEDFHeader::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
}


BEGIN_MESSAGE_MAP(CEDFHeader, CDialog)
	ON_WM_CREATE()
	ON_WM_SIZE()
	ON_NOTIFY(NM_CLICK, XTP_ID_REPORT_CONTROL, OnReportItemClick)
END_MESSAGE_MAP()


int CEDFHeader::OnCreate(LPCREATESTRUCT lpCreateStruct)
{
	CRect rectDummy;
	rectDummy.SetRectEmpty();
	// Create view:
	const DWORD dwStyle = WS_CHILD | WS_VISIBLE ;

	if (!table.Create(dwStyle, rectDummy, this, 4)) {
		TRACE0("Failed to create \n");
		return -1;      // fail to create
	}
	fillView();
	adjustLayout();

	CString title;
	GetWindowText(title);
	title += _T(" - ");
	title += fileName;
	SetWindowText(title);

	return 0;
}

// CEDFHeader message handlers



void CEDFHeader::adjustLayout()
{
	if (GetSafeHwnd() == NULL)
	{
		return;
	}

	CRect rectClient;
	GetClientRect(rectClient);

	table.SetWindowPos(NULL, rectClient.left + 1, rectClient.top + 1, rectClient.Width() - 2, rectClient.Height() - 2, SWP_NOACTIVATE | SWP_NOZORDER);
}

void CEDFHeader::fillView()
{
	CXTPReportColumn *c0 = table.AddColumn(new CXTPReportColumn(0,_T("Parameter"),10));
	c0->SetTreeColumn(TRUE);
	CXTPReportColumn *c1 = table.AddColumn(new CXTPReportColumn(1,_T("Value"),10));
	c0->SetTreeColumn(TRUE);

	//---Add records
	CString s0,s1;
	CXTPReportRecord *rec1 = new CXTPReportRecord();
	BOOL ok = s0.LoadString(IDS_EDF_VERSION);
	CXTPReportRecordItemText *i0 = new CXTPReportRecordItemText(s0);
	rec1->AddItem(i0);
	rec1->AddItem(new CXTPReportRecordItemText(binHeader.version));
	table.AddRecord(rec1);

	CXTPReportRecord *rec2 = new CXTPReportRecord();
	ok = s0.LoadString(IDS_EDF_PATIENTID);
	CXTPReportRecordItemText *i1 = new CXTPReportRecordItemText(s0);
	rec2->AddItem(i1);
	rec2->AddItem(new CXTPReportRecordItemText(binHeader.patient));
	table.AddRecord(rec2);

	CXTPReportRecord *rec3 = new CXTPReportRecord();
	ok = s0.LoadString(IDS_EDF_RECID);
	CXTPReportRecordItemText *i2 = new CXTPReportRecordItemText(s0);
	rec3->AddItem(i2);
	rec3->AddItem(new CXTPReportRecordItemText(binHeader.recordingID));
	table.AddRecord(rec3);

	CXTPReportRecord *rec4 = new CXTPReportRecord();
	ok = s0.LoadString(IDS_EDF_STARTTIME);
	CXTPReportRecordItemText *i3 = new CXTPReportRecordItemText(s0);
	rec4->AddItem(i3);
	CString sd,st;
	st.Format(_T("%02d:%02d:%02d "),
		            binHeader.startDateAndTime.GetHour(),
					binHeader.startDateAndTime.GetMinute(),
					binHeader.startDateAndTime.GetSecond());
	sd.Format(_T("%02d.%02d.%02d "),
					binHeader.startDateAndTime.GetDay(),
					binHeader.startDateAndTime.GetMonth(),
					binHeader.startDateAndTime.GetYear());
	st += sd;
	rec4->AddItem(new CXTPReportRecordItemText(st));
	table.AddRecord(rec4);

	CXTPReportRecord *rec5 = new CXTPReportRecord();
	ok = s0.LoadString(IDS_EDF_BYTES_IN_HEADER_RECORD);
	CXTPReportRecordItemText *i4 = new CXTPReportRecordItemText(s0);
	rec5->AddItem(i4);
	rec5->AddItem(new CXTPReportRecordItemNumber(binHeader.numBytesInHeaderRecord));
	table.AddRecord(rec5);

	CXTPReportRecord *rec6 = new CXTPReportRecord();
	ok = s0.LoadString(IDS_EDF_NUM_DATA_RECORDS);
	CXTPReportRecordItemText *i5 = new CXTPReportRecordItemText(s0);
	rec6->AddItem(i5);
	rec6->AddItem(new CXTPReportRecordItemNumber(binHeader.numDataRecords));
	table.AddRecord(rec6);

	CXTPReportRecord *rec7 = new CXTPReportRecord();
	ok = s0.LoadString(IDS_EDF_DURATION_OF_DATA_RECORD);
	CXTPReportRecordItemText *i6 = new CXTPReportRecordItemText(s0);
	rec7->AddItem(i6);
	CString sdur;
	CString sfmt;
	
	if (binHeader.secRecordLength == 1) ok = sfmt.LoadString(IDS_SEC_FORMAT_STRING);
	else ok = sfmt.LoadString(IDS_SECS_FORMAT_STRING);
	sdur.Format(sfmt,binHeader.secRecordLength);
	CXTPReportRecordItemText *in = new CXTPReportRecordItemText(sdur);
	rec7->AddItem(in);
	table.AddRecord(rec7);

	CXTPReportRecord *rec8 = new CXTPReportRecord();
	ok = s0.LoadString(IDS_EDF_NUM_SIGNALS_IN_DATA_RECORD);
	CXTPReportRecordItemText *i7 = new CXTPReportRecordItemText(s0);
	rec8->AddItem(i7);
	rec8->AddItem(new CXTPReportRecordItemNumber(binHeader.numSignalsInRecord));
	table.AddRecord(rec8);

	CXTPReportRecord *signals = new CXTPReportRecord();
	CString sigs;
	ok = sigs.LoadString(IDS_SIGNALS);
	signals->AddItem(new CXTPReportRecordItemText(sigs));
	signals->AddItem(new CXTPReportRecordItemNumber(binHeader.numSignalsInRecord));
	for (int i = 0 ; i < binHeader.numSignalsInRecord ; i++) {
		CXTPReportRecord *srec = new CXTPReportRecord();
		srec->AddItem(new CXTPReportRecordItemText(binRecords[i].label));
		CXTPReportRecordItem *rit = new CXTPReportRecordItem();
		rit->HasCheckbox(TRUE);
		rit->SetChecked(TRUE);
		rit->SetItemData(i);
		srec->AddItem(rit);
		
		CString s;
		CXTPReportRecord *r0;

		r0 = new CXTPReportRecord();
		ok = s.LoadString(IDS_TRANSDUCER_TYPE);
		r0->AddItem(new CXTPReportRecordItemText(s));
		r0->AddItem(new CXTPReportRecordItemText(binRecords[i].transducerType));
		srec->GetChilds()->Add(r0);

		r0 = new CXTPReportRecord();
		ok = s.LoadString(IDS_PHYSICAL_DIMENSION);
		r0->AddItem(new CXTPReportRecordItemText(s));
		r0->AddItem(new CXTPReportRecordItemText(binRecords[i].physicalDimension));
		srec->GetChilds()->Add(r0);

		r0 = new CXTPReportRecord();
		ok = s.LoadString(IDS_PHYSICAL_MINIMUM);
		r0->AddItem(new CXTPReportRecordItemText(s));
		r0->AddItem(new CXTPReportRecordItemNumber(binRecords[i].physicalMinimum));
		srec->GetChilds()->Add(r0);

		r0 = new CXTPReportRecord();
		ok = s.LoadString(IDS_PHYSICAL_MAXIMUM);
		r0->AddItem(new CXTPReportRecordItemText(s));
		r0->AddItem(new CXTPReportRecordItemNumber(binRecords[i].physicalMaximum));
		srec->GetChilds()->Add(r0);

		r0 = new CXTPReportRecord();
		ok = s.LoadString(IDS_DIGITAL_MINIMUM);
		r0->AddItem(new CXTPReportRecordItemText(s));
		r0->AddItem(new CXTPReportRecordItemNumber(binRecords[i].digitalMinimum));
		srec->GetChilds()->Add(r0);

		r0 = new CXTPReportRecord();
		ok = s.LoadString(IDS_DIGITAL_MAXIMUM);
		r0->AddItem(new CXTPReportRecordItemText(s));
		r0->AddItem(new CXTPReportRecordItemNumber(binRecords[i].digitalMaximum));
		srec->GetChilds()->Add(r0);

		r0 = new CXTPReportRecord();
		ok = s.LoadString(IDS_PREFILTERING);
		r0->AddItem(new CXTPReportRecordItemText(s));
		r0->AddItem(new CXTPReportRecordItemText(binRecords[i].prefiltering));
		srec->GetChilds()->Add(r0);

		r0 = new CXTPReportRecord();
		ok = s.LoadString(IDS_NUM_SAMPLES_IN_EACH_DATA_RECORD);
		r0->AddItem(new CXTPReportRecordItemText(s));
		r0->AddItem(new CXTPReportRecordItemNumber(binRecords[i].numSamples));
		srec->GetChilds()->Add(r0);

		r0 = new CXTPReportRecord();
		ok = s.LoadString(IDS_SAMPLING_FREQUENCY);
		r0->AddItem(new CXTPReportRecordItemText(s));
		s.Format(_T("%.1f Hz"),binRecords[i].sampFreq);
		r0->AddItem(new CXTPReportRecordItemText(s));
		srec->GetChilds()->Add(r0);

		CXTPReportRecord *r = (signals->GetChilds())->Add(srec);
	}

	table.AddRecord(signals);
	table.Populate();
}


void CEDFHeader::OnSize(UINT nType, int cx, int cy)
{
	CDialog::OnSize(nType, cx, cy);

	// TODO: Add your message handler code here
	adjustLayout();
}

// Handles clicking ReportControl row.
// If "Checked" item is clicked, change "Checked" attribute for current record
void CEDFHeader::OnReportItemClick(NMHDR * pNotifyStruct, LRESULT * /*result*/)
{
	XTP_NM_REPORTRECORDITEM* pItemNotify = (XTP_NM_REPORTRECORDITEM*) pNotifyStruct;

	if (!pItemNotify->pRow || !pItemNotify->pColumn)
		return;

//
////MD - another way to fix wrong reaction on column checkbox click - comment line GetReportCtrl().Populate();
////	instead of 	adding condition if (!bCheckBoxClicked)in function CXTPReportRecordItem::OnClick
//	if (pItemNotify->pColumn->GetItemIndex() == COLUMN_CHECK)
//	{
////		GetReportCtrl().Populate();
////Here we can have more reasonable reaction on Checked / Unchecked field state changed
//	}
}

///////////////////////////////////////////////////////////////////////////


// CEDFHeader2 dialog

IMPLEMENT_DYNAMIC(CEDFHeader2, CDialog)

CEDFHeader2::CEDFHeader2(CString _fname,EDF_BINARY_HEADER _binHeader,EDF_BINARY_RECORD *_binRecords,CWnd* pParent /*=NULL*/)
	: fileName(_fname),binHeader(_binHeader),binRecords(_binRecords),
	  CDialog(CEDFHeader::IDD, pParent)
{
	parChngRedrawMessage = RegisterWindowMessage(AG_MESSAGE_PAR_CHANGED_REDRAW);
}

CEDFHeader2::~CEDFHeader2()
{
}

void CEDFHeader2::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
}


BEGIN_MESSAGE_MAP(CEDFHeader2, CDialog)
	ON_WM_CREATE()
	ON_WM_SIZE()
	ON_MESSAGE(XTPWM_PROPERTYGRID_NOTIFY, OnGridNotify)
END_MESSAGE_MAP()


int CEDFHeader2::OnCreate(LPCREATESTRUCT lpCreateStruct)
{
	CRect rectDummy;
	rectDummy.SetRectEmpty();
	// Create view:

	if (!table.Create(rectDummy, this, ID_EDF_PROPERTIES)) {
		TRACE0("Failed to create \n");
		return -1;      // fail to create
	}
	fillView();
	adjustLayout();

	CString title;
	GetWindowText(title);
	title += _T(" - ");
	title += fileName;
	SetWindowText(title);

	return 0;
}

// CEDFHeader2 message handlers



void CEDFHeader2::adjustLayout()
{
	if (GetSafeHwnd() == NULL)
	{
		return;
	}

	CRect rectClient;
	GetClientRect(rectClient);

	table.SetWindowPos(NULL, rectClient.left + 1, rectClient.top + 1, rectClient.Width() - 2, rectClient.Height() - 2, SWP_NOACTIVATE | SWP_NOZORDER);
}

void CEDFHeader2::fillView()
{
	
	table.SetTheme(xtpGridThemeNativeWinXP);
	table.SetCustomColors( RGB(200, 200, 200), 0, RGB(182, 210, 189), RGB(247, 243, 233), 0);

	CString s;
	BOOL ok = s.LoadString(IDS_EDF_HEADER);
	CXTPPropertyGridItem *h = table.AddCategory(s);
	ok = s.LoadString(IDS_EDF_HEADER_DESCRIPTION);
	h->SetDescription(s);
	ok = s.LoadString(IDS_EDF_HEADER_TIP);
	h->SetTooltip(s);

	ok = s.LoadStringW(IDS_EDF_CHANNELS);
	CXTPPropertyGridItem *c = table.AddCategory(s);
	ok = s.LoadString(IDS_EDF_CHANNELS_DESCRIPTION);
	c->SetDescription(s);
	ok = s.LoadString(IDS_EDF_CHANNELS_TIP);
	c->SetTooltip(s);

	ok = s.LoadString(IDS_EDF_VERSION);
	CXTPPropertyGridItem *it = new CXTPPropertyGridItem(s,binHeader.version);
	it->SetReadOnly(TRUE);
	h->AddChildItem(it);

	ok = s.LoadString(IDS_EDF_PATIENTID);
	CXTPPropertyGridItem *it2 = new CXTPPropertyGridItem(s,binHeader.patient);
	it2->SetReadOnly(TRUE);
	h->AddChildItem(it2);

	ok = s.LoadString(IDS_EDF_RECID);
	CXTPPropertyGridItem *it3 = new CXTPPropertyGridItem(s,binHeader.recordingID);
	it3->SetReadOnly(TRUE);
	h->AddChildItem(it3);

	ok = s.LoadString(IDS_EDF_STARTTIME);
	CString sd,st;
	st.Format(_T("%02d:%02d:%02d "),
		            binHeader.startDateAndTime.GetHour(),
					binHeader.startDateAndTime.GetMinute(),
					binHeader.startDateAndTime.GetSecond());
	sd.Format(_T("%02d.%02d.%02d "),
					binHeader.startDateAndTime.GetDay(),
					binHeader.startDateAndTime.GetMonth(),
					binHeader.startDateAndTime.GetYear());
	st += sd;
	CXTPPropertyGridItem *it4 = new CXTPPropertyGridItem(s,st);
	it4->SetReadOnly(TRUE);
	h->AddChildItem(it4);

	ok = s.LoadString(IDS_EDF_BYTES_IN_HEADER_RECORD);
	CXTPPropertyGridItemNumber *it5 = new CXTPPropertyGridItemNumber(s,(LONG) binHeader.numBytesInHeaderRecord);
	it5->SetReadOnly(TRUE);
	h->AddChildItem(it5);

	ok = s.LoadString(IDS_EDF_NUM_DATA_RECORDS);
	CXTPPropertyGridItemNumber *it6 = new CXTPPropertyGridItemNumber(s,binHeader.numDataRecords);
	it6->SetReadOnly(TRUE);
	h->AddChildItem(it6);

	ok = s.LoadString(IDS_EDF_DURATION_OF_DATA_RECORD);
	CString sdur;
	CString sfmt;
	if (binHeader.secRecordLength == 1) ok = sfmt.LoadString(IDS_SEC_FORMAT_STRING);
	else ok = sfmt.LoadString(IDS_SECS_FORMAT_STRING);
	sdur.Format(sfmt,binHeader.secRecordLength);
	CXTPPropertyGridItem *it7 = new CXTPPropertyGridItem(s,sdur);
	it7->SetReadOnly(TRUE);
	h->AddChildItem(it7);

	ok = s.LoadString(IDS_EDF_NUM_SIGNALS_IN_DATA_RECORD);
	CXTPPropertyGridItemNumber *it8 = new CXTPPropertyGridItemNumber(s,binHeader.numSignalsInRecord);
	it8->SetReadOnly(TRUE);
	h->AddChildItem(it8);



	for (int i = 0 ; i < binHeader.numSignalsInRecord ; i++) {
		CXTPPropertyGridItemBool *iit0 = new CXTPPropertyGridItemBool(binRecords[i].label,binRecords[i].show,&binRecords[i].show);
		iit0->SetCheckBoxStyle(TRUE);
		CXTPPropertyGridItem *ci = c->AddChildItem(iit0);

		ok = s.LoadString(IDS_TRANSDUCER_TYPE);
		CXTPPropertyGridItem *gid0 = new CXTPPropertyGridItem(s,binRecords[i].transducerType);
		gid0->SetReadOnly(TRUE);
		iit0->AddChildItem(gid0);

		ok = s.LoadString(IDS_PHYSICAL_DIMENSION);
		CXTPPropertyGridItem *gid1 = new CXTPPropertyGridItem(s,binRecords[i].physicalDimension);
		gid1->SetReadOnly(TRUE);
		iit0->AddChildItem(gid1);

		ok = s.LoadString(IDS_PHYSICAL_MINIMUM);
		CXTPPropertyGridItemDouble *gid2 = new CXTPPropertyGridItemDouble(s,binRecords[i].physicalMinimum,_T("%.3f"),&binRecords[i].physicalMinimum);
		gid2->SetReadOnly(FALSE); /* NB!*/
		iit0->AddChildItem(gid2);

		ok = s.LoadString(IDS_PHYSICAL_MAXIMUM);
		CXTPPropertyGridItemDouble *gid3 = new CXTPPropertyGridItemDouble(s,binRecords[i].physicalMaximum,_T("%.3f"),&binRecords[i].physicalMaximum);
		gid3->SetReadOnly(FALSE); /*NB!*/
		iit0->AddChildItem(gid3);
		
		ok = s.LoadString(IDS_DIGITAL_MINIMUM);
		CXTPPropertyGridItemDouble *gid4 = new CXTPPropertyGridItemDouble(s,binRecords[i].digitalMinimum);
		gid4->SetReadOnly(TRUE);
		iit0->AddChildItem(gid4);

		ok = s.LoadString(IDS_DIGITAL_MAXIMUM);
		CXTPPropertyGridItemDouble *gid5 = new CXTPPropertyGridItemDouble(s,binRecords[i].digitalMaximum);
		gid5->SetReadOnly(TRUE);
		iit0->AddChildItem(gid5);

		ok = s.LoadString(IDS_PREFILTERING);
		CXTPPropertyGridItem *gid6 = new CXTPPropertyGridItem(s,binRecords[i].prefiltering);
		gid6->SetReadOnly(TRUE);
		iit0->AddChildItem(gid6);

		ok = s.LoadString(IDS_NUM_SAMPLES_IN_EACH_DATA_RECORD);
		CXTPPropertyGridItemNumber *gid7 = new CXTPPropertyGridItemNumber(s,binRecords[i].numSamples);
		gid7->SetReadOnly(TRUE);
		iit0->AddChildItem(gid7);

		ok = s.LoadString(IDS_SAMPLING_FREQUENCY);
		CString sf;
		sf.Format(_T("%.1f Hz"),binRecords[i].sampFreq);
		CXTPPropertyGridItem *gid8 = new CXTPPropertyGridItem(s,sf);
		gid8->SetReadOnly(TRUE);
		iit0->AddChildItem(gid8);

		ok = s.LoadString(IDS_MS_SAMP_INTERVAL);
		sf.Format(_T("%.1f ms"),binRecords[i].msSampInt);
		CXTPPropertyGridItem *gid9 = new CXTPPropertyGridItem(s,sf);
		gid9->SetReadOnly(TRUE);
		iit0->AddChildItem(gid9);
		
		ok = s.LoadString(IDS_GAIN);
		sf.Format(_T("%.3f"),binRecords[i].gain);
		CXTPPropertyGridItem *gid10 = new CXTPPropertyGridItem(s,sf);
		gid10->SetReadOnly(TRUE);
		iit0->AddChildItem(gid10);
		
		ok = s.LoadString(IDS_OFFSET);
		sf.Format(_T("%.3f"),binRecords[i].msSampInt);
		CXTPPropertyGridItem *gid11 = new CXTPPropertyGridItem(s,sf);
		gid11->SetReadOnly(TRUE);
		iit0->AddChildItem(gid11);


		ok = s.LoadString(IDS_VECTOR_TOTAL_LENGTH);
		CXTPPropertyGridItemNumber *gid12 = new CXTPPropertyGridItemNumber(s,binRecords[i].data.size());
		gid12->SetReadOnly(TRUE);
		iit0->AddChildItem(gid12);
	}
}


void CEDFHeader2::OnSize(UINT nType, int cx, int cy)
{
	CDialog::OnSize(nType, cx, cy);

	// TODO: Add your message handler code here
	adjustLayout();
}

LRESULT CEDFHeader2::OnGridNotify(WPARAM wParam, LPARAM lParam)
{
	CXTPPropertyGridItem* pItem = (CXTPPropertyGridItem*)lParam;

	if (XTP_PGN_ITEMVALUE_CHANGED == wParam) 
		::PostMessage(HWND_BROADCAST,parChngRedrawMessage,0L,0L);

	return 0;
}