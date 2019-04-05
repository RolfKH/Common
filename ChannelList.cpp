/*
Copyright 2011-2012. Spiro Medical AS

	Control of 
		Graph displays, 
		Channel selection, 
		Colours, 
		Transparency,
		Line thickness
		Engineering units

Rev	By	Description
0	RKH	Initial version
*/

#include "stdafx.h"
#include "ChannelList.h"


CChStorage::CChStorage() :
	configVersion(channelConfigVersion)
{
	resetAll();

#ifdef _DEBUG
	int six = sizeof(page);
	if (six > MM_MMFILE_MAX_SIZE) AfxMessageBox(_T("Please increase MM_MMFILE_MAX_SIZE!!"));
#endif

	/*---See if mem mapped file is present
		If not, read from xml default file
	*/
	mmFile = new CESGenericMMFile(MM_MMFILE_MAX_SIZE,apnMMFileName);
}

CChStorage::~CChStorage() 
{
	if (mmFile) delete mmFile;
}

BOOL CChStorage::save(const CString _file) { return FALSE;}

BOOL CChStorage::read(const CString _file) { return FALSE;}

int CChStorage::getNumPages(void) { return numberOfGraphPages; }

int CChStorage::getSectionsPerPage(void) { return numberOfGraphSectionsPerPage; }

int CChStorage::getChannelsPerSection(void) { return numDataSets; }

int CChStorage::getNumEnabledPages(void)
{
	int cnt = 0;
	int numP = getNumPages();
	for (int i = 0 ; i < numP ; i++) cnt = page[i].enabled ? cnt + 1 : cnt;
	return cnt;
}

int CChStorage::getNumDataTypes(UINT _val)
{
	int numBits = sizeof(_val) * 8;
	int bit = 0x01;
	int cnt = 0;
	UINT testInt = _val;
	for (int i = 0 ; i < numBits ; i++) {
		if (testInt & 0x01) cnt++;
		testInt = testInt >> 1;
	}
	return cnt;
}

int CChStorage::getNumEnabledChannels(const int _page,const int _section)
{
	int cnt = 0;
	int numC = getChannelsPerSection();
	for (int i = 0 ; i < numC ; i++) 
		cnt += getNumDataTypes(page[_page].section[_section].chSpec[i].showDataTypes);
	return cnt;
}

int CChStorage::getNumEnabledSections(const int _page)
{
	int cnt = 0;
	int numSct = getSectionsPerPage();
	for (int i = 0 ; i < numSct ; i++) cnt = page[_page].section[i].enabled ? cnt + 1 : cnt;
	return cnt;
}

CString CChStorage::getSectionText(const int _pageNum,const int _sectionNum)
{
	int numP = getNumPages();
	if (_pageNum >= numP) return _T("");
	if (_pageNum < 0) return _T("");
	int numSct = getSectionsPerPage();
	if (_sectionNum >= numSct) return _T("");
	if (_sectionNum < 0) return _T("");

	CString s(page[_pageNum].section[_sectionNum].sectionTxt);
	return s;
}

CString CChStorage::getPageText(const int _num)
{
	int numP = getNumPages();
	if (_num >= numP) return _T("");
	if (_num < 0) return _T("");
	CString s(page[_num].pageTxt);
	return s;
}

void CChStorage::setSectionText(const int _pnum,const int _snum,const CString _st)
{
	int numP = getNumPages();
	if (_pnum >= numP) return;
	if (_pnum < 0) return;
	
	int numSct = getSectionsPerPage();
	if (_snum >= numSct) return;
	if (_snum < 0) return;

	cstringToChar(_st,page[_pnum].section[_snum].sectionTxt,sectionTxtLen);
}

void CChStorage::setPageText(const int _num,const CString _pt)
{
	int numP = getNumPages();
	if (_num >= numP) return;
	if (_num < 0) return;
	cstringToChar(_pt,page[_num].pageTxt,pageTxtLen);
}

BOOL CChStorage::getSectionEnable(const int _page,const int _section)
{
	int numP = getNumPages();
	if (_page >= numP) return FALSE;
	if (_page < 0) return FALSE;
	int numSct = getSectionsPerPage();
	if (_section >= numSct) return FALSE;
	if (_section < 0) return FALSE;
	return page[_page].section[_section].enabled;
}

BOOL *CChStorage::getSectionEnableAddr(const int _page,const int _section)
{
	int numP = getNumPages();
	if (_page >= numP) return NULL;
	if (_page < 0) return NULL;
	int numSct = getSectionsPerPage();
	if (_section >= numSct) return NULL;
	if (_section < 0) return NULL;
	return &page[_page].section[_section].enabled;
}

void CChStorage::setPageEnable(const int _num,BOOL _on)
{
	int numP = getNumPages();
	if (_num >= numP) return ;
	if (_num < 0) return ;
	page[_num].enabled = _on;
}

BOOL CChStorage::getPageEnable(const int _num)
{
	int numP = getNumPages();
	if (_num >= numP) return FALSE;
	if (_num < 0) return FALSE;
	return page[_num].enabled;
}


CString CChStorage::getExtendedSectionText(const int _pageNum,const int _sectNum)
{
	int numP = getNumPages();
	if (_pageNum >= numP) return _T("");
	if (_pageNum < 0) return _T("");
	int numSct = getSectionsPerPage();
	if (_sectNum >= numSct) return _T("");
	if (_sectNum < 0) return _T("");

	CString s(page[_pageNum].section[_sectNum].sectionTxt);
	int numChannels = getChannelsPerSection();
	CString chList;
	for (int i = 0 ; i < numChannels ; i++) {
		CString s2;
		if (page[_pageNum].section[_sectNum].chSpec[i].showDataTypes) {
			BOOL ok = s2.LoadString(page[_pageNum].section[_sectNum].chSpec[i].userNameStringID);
			if (ok) chList += s2;
			chList += _T(",");
		}
	}
	chList = chList.TrimRight(_T(","));
	if (chList != _T(""))  {
		s += _T(" (");
		s += chList;
		s += _T(")");
	}
	return s;
}


CString CChStorage::getUnit(const int _pageNum,const int _sectNum)
{
	int numP = getNumPages();
	if (_pageNum >= numP) return _T("");
	if (_pageNum < 0) return _T("");
	int numSct = getSectionsPerPage();
	if (_sectNum >= numSct) return _T("");
	if (_sectNum < 0) return _T("");

	//---Find all units
	vector <int> idv,idv2;;
	int numChannels = getChannelsPerSection();
	for (int i = 0 ; i < numChannels ; i++) {
		if (page[_pageNum].section[_sectNum].chSpec[i].showDataTypes) {
			idv.push_back((int) page[_pageNum].section[_sectNum].chSpec[i].unitStrID0);
		}
	}

	//--First sort, then remove multiples
	sort(idv.begin(),idv.end());
	if (idv.size() > 0) idv2.push_back(idv.at(0));
	if (idv.size() > 1) {
		for (unsigned int i = 1 ; i < idv.size() ; i++) {
			if (idv.at(i) != idv.at(i - 1)) 
				idv2.push_back(idv.at(i));
		}
	}
	else {
		CString s;
		BOOL ok = FALSE;
		if (idv.size() > 0) ok = s.LoadString(idv.at(0));
		return ok ? s : _T("") ;
	}
	CString s,s2;
	for (unsigned int i = 0 ; i < idv2.size() ; i++) {
		BOOL ok = s.LoadString(idv2.at(i));
		if (ok) s2 += s;
		s2 += _T("\n");
	}
	s2.TrimRight(_T("\n"));

	return s2;
}

BOOL CChStorage::getUseGrid(const int _pageNum,const int _sectNum)
{
	int numP = getNumPages();
	if (_pageNum >= numP) return FALSE;
	if (_pageNum < 0) return FALSE;
	int numSct = getSectionsPerPage();
	if (_sectNum >= numSct) return FALSE;
	if (_sectNum < 0) return FALSE;

	return page[_pageNum].section[_sectNum].useGrid;
}

BOOL *CChStorage::getUseGridAddr(const int _pageNum,const int _sectNum)
{
	int numP = getNumPages();
	if (_pageNum >= numP) return NULL;
	if (_pageNum < 0) return NULL;
	int numSct = getSectionsPerPage();
	if (_sectNum >= numSct) return NULL;
	if (_sectNum < 0) return NULL;

	return &page[_pageNum].section[_sectNum].useGrid;
}

	
CString CChStorage::getTimeStartStr(const int _pageNum)
{
	return secToString(page[_pageNum].secStart);
}

CString CChStorage::getTimeStopStr(const int _pageNum)
{
	return secToString(page[_pageNum].secStop);
}

/*
Description: Converts time sttimg to seconds
*/
double CChStorage::stringToTime(const CString _s)
{
	CString s = _s;
	s.Trim();
	int negSign = s.Find(_T("-"));
	bool isNeg = 0 == negSign;
	s.Remove(_T('-'));
	
	wchar_t *context = NULL;
	CString hourS = wcstok_s(s.GetBuffer(),_T(":"),&context);
	CString minS = wcstok_s(NULL,_T(":"),&context);
	CString secS = wcstok_s(NULL,_T(":"),&context);
	double ret = _ttoi(secS) + _ttoi(minS) * 60 + _ttoi(hourS) * 3600;
	return isNeg ? - ret : ret ;
}


double CChStorage::getTimeStart(const int _pageNum)
{
	int numP = getNumPages();
	if (_pageNum >= numP) return .0f;
	if (_pageNum < 0) return .0f;
	
	return page[_pageNum].secStart;
}

double CChStorage::getTimeStop(const int _pageNum)
{
	int numP = getNumPages();
	if (_pageNum >= numP) return 1.0f;
	if (_pageNum < 0) return 1.0f;
	
	return page[_pageNum].secStop;
}


void CChStorage::setMinY(const int _page,const int _section,const int _channelID,const double _val)
{
	int numP = getNumPages();
	if (_page >= numP) return;
	if (_page < 0) return;
	int numSct = getSectionsPerPage();
	if (_section >= numSct) return;
	if (_section < 0) return;
	
	int chIndex = channelIndexFromChannelID(_page,_section,_channelID);
	if (chIndex >= 0) 
		page[_page].section[_section].chSpec[chIndex].minY = _val;
}

void CChStorage::setMaxY(const int _page,const int _section,const int _channelID,const double _val)
{
	int numP = getNumPages();
	if (_page >= numP) return;
	if (_page < 0) return;
	int numSct = getSectionsPerPage();
	if (_section >= numSct) return;
	if (_section < 0) return;
	
	int chIndex = channelIndexFromChannelID(_page,_section,_channelID);
	if (chIndex >= 0) 
		page[_page].section[_section].chSpec[chIndex].maxY = _val;
}

int CChStorage::channelIndexFromChannelID(const int _page,const int _section,const int _id)
{
	int numCh = getChannelsPerSection();
	for (int i = 0 ; i < numCh ; i++) {
		if (_id == page[_page].section[_section].chSpec[i].channelID) return i;
	}
	return -1;
}

void CChStorage::setTransparency(const int _page,const int _section,const int _channelID,const int _transparency)
{
	int numP = getNumPages();
	if (_page >= numP) return;
	if (_page < 0) return;
	int numSct = getSectionsPerPage();
	if (_section >= numSct) return;
	if (_section < 0) return;
	
	int chIndex = channelIndexFromChannelID(_page,_section,_channelID);
	if (chIndex >= 0) 
		page[_page].section[_section].chSpec[chIndex].transparency = _transparency;
}


void CChStorage::setDataType(const int _page,const int _section,const int _channelID,const UINT _typeID,const bool _on)
{
	int numP = getNumPages();
	if (_page >= numP) return;
	if (_page < 0) return;
	int numSct = getSectionsPerPage();
	if (_section >= numSct) return;
	if (_section < 0) return;
	
	int chIndex = channelIndexFromChannelID(_page,_section,_channelID);
	if (chIndex < 0) return;

	UINT dataTypes = page[_page].section[_section].chSpec[chIndex].dataTypes;

	int type = 0;
	if (IDS_RAW_XY			== _typeID) {
		if ((pressType == dataTypes) || (tempType == dataTypes)) type = dataTypeXYRaw | dataTypeEnvelope;
		else type = dataTypeXYRaw;
	}
	if (IDS_FILTERED		== _typeID)  {
		if ((pressType == dataTypes) || (tempType == dataTypes)) type = dataTypeXYRaw | dataTypeEnvelope;
		else type = dataTypeXYRaw;
	}
	if (IDS_PEAK_TO_PEAK	== _typeID) type = dataTypePeakToPeak	;	
	if (IDS_BASELINE_AVG	== _typeID) type = dataTypeBaselineAvg	;	
	if (IDS_BASELINE_MAX	== _typeID) type = dataTypeBaselineMax	;	
		
	page[_page].section[_section].chSpec[chIndex].showDataTypes = _on ? 
			page[_page].section[_section].chSpec[chIndex].showDataTypes | type :
			page[_page].section[_section].chSpec[chIndex].showDataTypes & ~type;

}

void CChStorage::setThickness(const int _page,const int _section,const int _channelID,const int _thickness)
{
	int numP = getNumPages();
	if (_page >= numP) return;
	if (_page < 0) return;
	int numSct = getSectionsPerPage();
	if (_section >= numSct) return;
	if (_section < 0) return;
	
	int chIndex = channelIndexFromChannelID(_page,_section,_channelID);
	if (chIndex >= 0) 
		page[_page].section[_section].chSpec[chIndex].thickness = _thickness;
}

void CChStorage::setTimeStart(const int _pageNum,const double _time)
{
	int numP = getNumPages();
	if (_pageNum >= numP) return;
	if (_pageNum < 0) return;
	
	page[_pageNum].secStart = _time;
}

void CChStorage::setTimeStop(const int _pageNum,const double _time)
{
	int numP = getNumPages();
	if (_pageNum >= numP) return;
	if (_pageNum < 0) return;

	page[_pageNum].secStop = _time;
}


CString CChStorage::secToString(const double _secs)
{
	bool neg = _secs < .0f;
	double secs = neg ? - _secs : _secs;
	int hours = (int) (secs / 3600);
	int mins = (int) (secs / 60) - hours * 60;
	double rest = secs - hours * 3600 - mins * 60;
	CString s;
	if (neg) s.Format(_T("-%02d:%02d:%02.0f"),hours,mins,(float) rest);
	else s.Format(_T("+%02d:%02d:%02.0f"),hours,mins,(float) rest);
	return s;
}

CString CChStorage::getPressUnitStr(const int _pageNum,const int _sectNum)
{
	int numP = getNumPages();
	if (_pageNum >= numP) return _T("");
	if (_pageNum < 0) return _T("");
	int numSct = getSectionsPerPage();
	if (_sectNum >= numSct) return _T("");
	if (_sectNum < 0) return _T("");
	CString s;
	BOOL ok = s.LoadString(page[_pageNum].section[_sectNum].pressUnit);
	return ok ? s : _T("") ;
}

int CChStorage::getTempUnit(const int _pageNum,const int _sectNum)
{
	int numP = getNumPages();
	if (_pageNum >= numP) return -1;
	if (_pageNum < 0) return -1;
	int numSct = getSectionsPerPage();
	if (_sectNum >= numSct) return -1;
	if (_sectNum < 0) return -1;
	
	return page[_pageNum].section[_sectNum].tempUnit;
}

int CChStorage::getPressUnit(const int _pageNum,const int _sectNum)
{
	int numP = getNumPages();
	if (_pageNum >= numP) return -1;
	if (_pageNum < 0) return -1;
	int numSct = getSectionsPerPage();
	if (_sectNum >= numSct) return -1;
	if (_sectNum < 0) return -1;
	
	return page[_pageNum].section[_sectNum].pressUnit;
}

CString CChStorage::getTempUnitStr(const int _pageNum,const int _sectNum)
{
	int numP = getNumPages();
	if (_pageNum >= numP) return _T("");
	if (_pageNum < 0) return _T("");
	int numSct = getSectionsPerPage();
	if (_sectNum >= numSct) return _T("");
	if (_sectNum < 0) return _T("");
	
	CString s;
	BOOL ok = s.LoadString(page[_pageNum].section[_sectNum].tempUnit);
	return ok ? s : _T("") ;
}

CH_SPEC *CChStorage::getChSpecP(const int _pageNum,const int _sectNum,const int _chID)
{
	
	int numP = getNumPages();
	if (_pageNum >= numP) return NULL;
	if (_pageNum < 0) return NULL;
	int numSct = getSectionsPerPage();
	if (_sectNum >= numSct) return NULL;
	if (_sectNum < 0) return NULL;

	for (int i = 0 ; i < getChannelsPerSection() ; i++) {
		if (_chID == page[_pageNum].section[_sectNum].chSpec[i].channelID) return &page[_pageNum].section[_sectNum].chSpec[i];
	}
	return NULL;
	return page[_pageNum].section[_sectNum].chSpec;
}

CH_SPEC *CChStorage::getChSpecP(const int _pageNum,const int _sectNum)
{
	int numP = getNumPages();
	if (_pageNum >= numP) return FALSE;
	if (_pageNum < 0) return FALSE;
	int numSct = getSectionsPerPage();
	if (_sectNum >= numSct) return FALSE;
	if (_sectNum < 0) return FALSE;
	return page[_pageNum].section[_sectNum].chSpec;
}

void CChStorage::resetAll(void)
{
	//---Reset all
	CString s0,s1;
	
	int numP = getNumPages();
	for (int i = 0 ; i < numP ; i++) {
		_stprintf_s(page[i].regName,regNameLen,_T("Page %d"),i);
		CString st,sn;
		BOOL ok = st.LoadString(IDS_PAGE);
		sn.Format(_T(" %d"),i);
		st += sn;
		cstringToChar(st,page[i].pageTxt,pageTxtLen);

		page[i].enabled = TRUE;
		page[i].secStart = .0f;
		page[i].secStop = SECS_IN_8_HOURS;
		int numSct = getSectionsPerPage();
		for (int j = 0 ; j < numSct ; j++) {
			s1.Format(_T("Section %d"),j);
			cstringToChar(s1,page[i].section[j].regName,regNameLen);
			cstringToChar(s1,page[i].section[j].sectionTxt,sectionTxtLen);
			page[i].section[j].useGrid = DEF_USE_GRID;
			page[i].section[j].enabled = DEF_SECTION_ENABLE;
			page[i].section[j].pressUnit = DEF_PRESS_UNIT;
			page[i].section[j].tempUnit = DEF_TEMP_UNIT;
			int numChannels = getChannelsPerSection();
			for (int k = 0 ; k < numChannels ; k++) {
				page[i].section[j].chSpec[k] = defaultChSpec[k];
			}
		}
	}
}
	
void CChStorage::saveToFile(void)
{
	CXTRegistryManager reg;
	CString ps;
	CString path;
	ps.Format(_T("%s\\"),profileKey);
	path = reg.GetProfileString(ps,regsExportImportPath,path);

	DWORD flags = OFN_OVERWRITEPROMPT;
	CString filter = _T("XML Files (*.xml)|*.xml||");
	CFileDialog fd(FALSE,_T("XML"),path,flags,filter);
	int ret = fd.DoModal();
	if (IDOK == ret) {
		path = fd.GetPathName();
		CWaitCursor cw;
		BOOL saveToFile = save(path);
		if (saveToFile) BOOL ok2 = reg.WriteProfileString(ps,regsExportImportPath,path);
	}
}

void CChStorage::readFromFile(void)
{
	CXTRegistryManager reg;
	CString ps;
	CString path;
	ps.Format(_T("%s\\"),profileKey);
	path = reg.GetProfileString(ps,regsExportImportPath,path);

	DWORD flags = OFN_FILEMUSTEXIST;
	CString filter = _T("XML Files (*.xml)|*.xml||");
	CFileDialog fd(TRUE,_T("XML"),path,flags,filter);
	int ret = fd.DoModal();
	if (IDOK == ret) {
		path = fd.GetPathName();
		CWaitCursor cw;
		BOOL readFromIni = read(path);
		if (readFromIni) BOOL ok = reg.WriteProfileString(ps,regsExportImportPath,path);
	}
}


/*
Description: Return the cahnnel number fro the Id as it appears in the XML file
*/
int CChStorage::getChannelNumberFromIdString(const CString _str)
{
	int num = getChannelsPerSection();
	for (int i = 0 ; i < num ; i++) {
		CString s;
		BOOL ok = s.LoadString(page[0].section[0].chSpec[i].userNameStringID);
		if (_str == s) return i;
	}
	return -1;
}

int CChStorage::getTempUnitFromString(const CString _newUnitStr)
{
	CString s0,s1;
	BOOL ok = s0.LoadString(IDS_UNIT_T_DEGC_PER_SEC);
	ok = s1.LoadString(IDS_UNIT_T_DEGF_PER_SEC);
	if (s1 == _newUnitStr) return IDS_UNIT_T_DEGF_PER_SEC;
	if (s0 == _newUnitStr) return IDS_UNIT_T_DEGC_PER_SEC;
	return 0;
}

int CChStorage::getPressUnitFromString(const CString _newUnitStr)
{
	CString s0,s1,s2,s3;
	BOOL ok;
	ok = s0.LoadString(IDS_UNIT_P_CMH2O);
	ok = s3.LoadString(IDS_UNIT_P_MMHG);
	ok = s1.LoadString(IDS_UNIT_P_kPA);
	ok = s2.LoadString(IDS_UNIT_P_BAR);
	if (s1 == _newUnitStr) return IDS_UNIT_P_kPA;
	if (s0 == _newUnitStr) return IDS_UNIT_P_CMH2O;
	if (s3 == _newUnitStr) return IDS_UNIT_P_MMHG;
	if (s2 == _newUnitStr) return IDS_UNIT_P_BAR;
	return 0;
}

void CChStorage::forcePressUnit(const int _page,const int _section,const CString _newUnitStr)
{
	int oldUnit = page[_page].section[_section].pressUnit;
	int newUnit = getPressUnitFromString(_newUnitStr);
	
	int numChannels = getChannelsPerSection();
	page[_page].section[_section].pressUnit = newUnit;
	for (int i = 0 ; i < numChannels ; i++) {
		int family = page[_page].section[_section].chSpec[i].family;
		if (FAMILY_PRESS == family) {
			changeChannelMaxMin(oldUnit,newUnit,0,0,_page,_section,i);
			page[_page].section[_section].chSpec[i].unitStrID0 = newUnit;
		}
		if (FAMILY_TEMP_PRESS == family) {
			int tUnit = page[_page].section[_section].tempUnit;
			changeChannelMaxMin(tUnit,tUnit,oldUnit,newUnit,_page,_section,i); 
			page[_page].section[_section].chSpec[i].unitStrID1 = newUnit;
		}
		if (FAMILY_PRESS_TEMP == family) {
			int tUnit = page[_page].section[_section].tempUnit;
			changeChannelMaxMin(oldUnit,newUnit,tUnit,tUnit,_page,_section,i);
			page[_page].section[_section].chSpec[i].unitStrID0 = newUnit;
		}
	}
}

double CChStorage::fromDegCtoDegF(double _val)
{
	double val = _val * (9.0f / 5.0f);
	val += 32.0f;
	return val;
}

double CChStorage::fromDegFtoDegC(double _val)
{
	double val = _val - 32.0f;
	val /= 9.0f;
	val *= 5.0f;
	return val;
}

void CChStorage::changeChannelMaxMin(const int _oldUnit0,const int _newUnit0,const int _oldUnit1,const int _newUnit1,
										const int _page,const int _section,const int _ch)
{

	int family = page[_page].section[_section].chSpec[_ch].family;

	if ((FAMILY_TEMP_PRESS == family) || (FAMILY_PRESS_TEMP == family)) {

		if ((_oldUnit0 == _newUnit0) && (_oldUnit1 == _newUnit1)) return;

		if (FAMILY_TEMP_PRESS == family) {
			if (_oldUnit1 != _newUnit1) {  // If press unit is new

				double factor1 = getFactorToSIUnit(_oldUnit1);
				double factor2 = getFactorToNewUnit(_newUnit1);
				double factor = factor1 * factor2;
				page[_page].section[_section].chSpec[_ch].maxY /= factor;
				page[_page].section[_section].chSpec[_ch].minY /= factor;
				page[_page].section[_section].chSpec[_ch].unitStrID1 = _newUnit1;

				return;
			}
		}
		if (FAMILY_PRESS_TEMP == family) {
			if (_oldUnit0 != _newUnit0) {  // If press unit is new
				double factor1 = getFactorToSIUnit(_oldUnit1);
				double factor2 = getFactorToNewUnit(_newUnit1);
				double factor = factor1 * factor2;
				page[_page].section[_section].chSpec[_ch].maxY *= factor;
				page[_page].section[_section].chSpec[_ch].minY *= factor;
				page[_page].section[_section].chSpec[_ch].unitStrID0 = _newUnit0;

				return;
			}
		}
	}
	
	if (_oldUnit0 == _newUnit0) return;

	if (IDS_UNIT_T_DEGC_PER_SEC == _oldUnit0) {
		
		page[_page].section[_section].chSpec[_ch].maxY = fromDegCtoDegF(page[_page].section[_section].chSpec[_ch].maxY);
		page[_page].section[_section].chSpec[_ch].minY = fromDegCtoDegF(page[_page].section[_section].chSpec[_ch].minY);
		page[_page].section[_section].chSpec[_ch].unitStrID0 = _newUnit0;
		return;
	}
	if (IDS_UNIT_T_DEGF_PER_SEC == _oldUnit0) {
		page[_page].section[_section].chSpec[_ch].maxY = fromDegFtoDegC(page[_page].section[_section].chSpec[_ch].maxY);
		page[_page].section[_section].chSpec[_ch].minY = fromDegFtoDegC(page[_page].section[_section].chSpec[_ch].minY);
		page[_page].section[_section].chSpec[_ch].unitStrID0 = _newUnit0;
		return;
	}

	double factor1 = getFactorToSIUnit(_oldUnit0);
	double factor2 = getFactorToNewUnit(_newUnit0);
	double factor = factor1 * factor2;
	page[_page].section[_section].chSpec[_ch].maxY *= factor;
	page[_page].section[_section].chSpec[_ch].minY *= factor;
	page[_page].section[_section].chSpec[_ch].unitStrID0 = _newUnit0;
}

double CChStorage::getFactorToNewUnit(const int _newUnit) 
{	
	switch (_newUnit) {
	case IDS_UNIT_P_CMH2O : 
		{
			return FROM_kPA_TO_CMH2O;
		}
		break;
	case IDS_UNIT_P_kPA :
		{
			return 1.0f;
		}
		break;			
	case IDS_UNIT_P_BAR	:
		{
			return FROM_kPA_TO_BAR;
		}
		break;		
	case IDS_UNIT_P_MMHG :
		{
			return FROM_kPA_TO_MMHG;
		}
		break;			
	}
	return 1.0f;
}

double CChStorage::getFactorToSIUnit(const int _oldUnit) 
{	
	switch (_oldUnit) {
	case IDS_UNIT_P_CMH2O : 
		{
			return FROM_CMH2O_TO_kPA;
		}
		break;
	case IDS_UNIT_P_kPA :
		{
			return 1.0f;
		}
		break;			
	case IDS_UNIT_P_BAR	:
		{
			return FROM_BAR_TO_kPA;
		}
		break;		
	case IDS_UNIT_P_MMHG :
		{
			return FROM_MMHG_TO_kPA;
		}
		break;		
	}
	return 1.0f;
}

void CChStorage::forceTempUnit(const int _page,const int _section,const CString _newUnitStr)
{
	int oldUnit = page[_page].section[_section].tempUnit;
	int newUnit = getTempUnitFromString(_newUnitStr);
	
	int numChannels = getChannelsPerSection();
	page[_page].section[_section].tempUnit = newUnit;
	for (int i = 0 ; i < numChannels ; i++) {
		int family = page[_page].section[_section].chSpec[i].family;
		if (FAMILY_TEMP == family) {
			changeChannelMaxMin(oldUnit,newUnit,0,0,_page,_section,i);
			page[_page].section[_section].chSpec[i].unitStrID0 = newUnit;
		}
		// No action for families FAMILY_TEMP_PRESS and FAMILY_PRESS_TEMP
	}
}

/*
Description: In order to control the sequence of units that go into the unit selector combo box and in the xml file
Returns: The string constant
*/
int CChStorage::getIndexFromPressUnitStringConstant(const int _sc)
{
	switch (_sc) {
	case IDS_UNIT_P_CMH2O	: return 0;
	case IDS_UNIT_P_MMHG	: return 1;
	case IDS_UNIT_P_kPA		: return 2;
	case IDS_UNIT_P_BAR		: return 3;
	default: break;
	}
	return -1;
}

/*
Description: In order to control the sequence of units that go into the unit selector combo box and in the xml file
Returns: The string constant
*/
int CChStorage::getIndexFromTempUnitStringConstant(const int _sc)
{
	switch (_sc) {
	case IDS_UNIT_T_DEGC_PER_SEC	: return 0;
	case IDS_UNIT_T_DEGF_PER_SEC	: return 1;
	default: break;
	}
	return -1;
}

/*
Description: Converts CString to a char[] string
_s: The CString
_ch: Pointer to char buffer
_maxLen: Max number of characters
*/
void CChStorage::cstringToChar(CString _s,_TCHAR _ch[],int _maxLen)
{
	int len = _s.GetLength();
	int ml = _maxLen - 1;
	int useLen = len > ml ? ml : len;
	int i = 0;
	for (i = 0 ; i < len ; i++) {
		_ch[i] = (char) _s.GetAt(i);
	}
	_ch[i] = 0;
}

bool CChStorage::updateToMemMap(void)
{
	if (!mmFile) return false;

	int size = sizeof(page);
	int didWrite = mmFile->write((void *) page,size,MM_FILES_STD_INNER_WRITE_TIMEOUT,MM_FILES_STD_OUTER_WRITE_TIMEOUT);
	if (didWrite == size) return true;
	else return false;
}

bool CChStorage::updateFromMemMap(void)
{
	if (!mmFile) return false;

	int size = sizeof(page);
	int didRead = mmFile->read((void *) page,size,MM_FILES_STD_READ_TIMEOUT);
	if (didRead == size) return true;
	else return false;
}

/*
Description: In order to control the sequence of units that go into the unit selector combo box and in the xml file
Returns: The string constant
*/
int CChStorage::getPressUnitStringConstantFromIndex(const int _seqNum)
{
	switch (_seqNum) {
	case 0 : return IDS_UNIT_P_CMH2O; break;
	case 1: return IDS_UNIT_P_MMHG	; break;
	case 2: return IDS_UNIT_P_kPA	; break;
	case 3: return IDS_UNIT_P_BAR	; break;
	default: break;
	}
	return -1;
}

/*
Description: In order to control the sequence of units that go into the unit selector combo box and in the xml file
Returns: The string constant
*/
int CChStorage::getTempUnitStringConstantFromIndex(const int _seqNum)
{
	switch (_seqNum) {
	case 0 : return IDS_UNIT_T_DEGC_PER_SEC	; break;
	case 1 : return IDS_UNIT_T_DEGF_PER_SEC ; break;
	default: break;
	}
	return -1;
}

void CChStorage::forceMinMaxY(const int _page,const int _section,const int _channelID)
{
	int chIndex = channelIndexFromChannelID(_page,_section,_channelID);
	if (chIndex < 0 ) return;

	double max = page[_page].section[_section].chSpec[chIndex].maxY;
	double min = page[_page].section[_section].chSpec[chIndex].minY;

	int thisFamily = page[_page].section[_section].chSpec[chIndex].family;

	for (int i = 0 ; i < getChannelsPerSection() ; i++) {
		if (thisFamily == page[_page].section[_section].chSpec[i].family) {
			page[_page].section[_section].chSpec[i].maxY = max;
			page[_page].section[_section].chSpec[i].minY = min;
		}
	}
}		

long CChStorage::getAcceptedTransparency(const long _tryThis)
{
	long ret = _tryThis;
	ret = ret < MIN_TRANSPARENCY ? MIN_TRANSPARENCY : ret;
	ret = ret > MAX_TRANSPARENCY ? MAX_TRANSPARENCY : ret;
	return ret;
}

int CChStorage::getAcceptedThickness(const int _tryThis)
{
	if (_tryThis < minThickness) return minThickness;
	if (_tryThis > maxThickness) return maxThickness;
	return _tryThis;
}

COLORREF CChStorage::colorrefFromString(const CString _str)
{
	wchar_t *context = NULL;
	CString s = _str;
	int red		= _ttoi(wcstok_s(s.GetBuffer(),_T(","),&context));
	int green	= _ttoi(wcstok_s(NULL,_T(","),&context));
	int blue	= _ttoi(wcstok_s(NULL,_T(","),&context));
	
	red = red >= 0 ? red :0;
	red = red <= 255 ? red : 255;
	
	blue = blue >= 0 ? blue :0;
	blue = blue <= 255 ? blue : 255;
	
	green = green >= 0 ? green :0;
	green = green <= 255 ? green : 255;

	COLORREF ref = RGB((BYTE) red,(BYTE) green,(BYTE) blue);
	return ref;
}

bool CChStorage::checkAndCorrectUnits(void)
{
	int np = getNumPages();
	int ns = getSectionsPerPage();
	int nc = getChannelsPerSection();
	CString sp,st;
	for (int i0 = 0 ; i0 < np ; i0++) {
		for (int i1 = 0 ; i1 < ns ; i1++) {
			BOOL ok = sp.LoadString(page[i0].section[i1].pressUnit);
			ok = st.LoadString(page[i0].section[i1].tempUnit);
			forcePressUnit(i0,i1,sp);
			forceTempUnit(i0,i1,st);
		}
	}
	return true;
}

bool CChStorage::checkAndCorrectMinMaxYValues(void)
{
	bool ret = true;
	int numP = getNumPages(); 
	int numSct = getSectionsPerPage();
	int numCh = getChannelsPerSection();
	for (int ip = 0 ; ip < numP ; ip++) {
		for (int iS = 0 ; iS < numSct ; iS++) {
			for (int iC = 0 ; iC < numCh ; iC++) {
				if (page[ip].section[iS].chSpec[iC].maxY <= page[ip].section[iS].chSpec[iC].minY) {
					ret = false;
					page[ip].section[iS].chSpec[iC].maxY = page[ip].section[iS].chSpec[iC].minY + 100.0f;
				}
			}
		}
	}
	return ret;
}

///////////////////////

CChRegInterface::CChRegInterface() 
{
	read();
}

CChRegInterface::~CChRegInterface() 
{
	save(_T(""),true);
}

BOOL CChRegInterface::save(const CString _iniFile,const bool _exit) 
{
	CXTRegistryManager reg;
	if (_iniFile != _T("")) reg.SetINIFileName(_iniFile);
	else reg.SetRegistryKey(_T("Spiro Medical"),profileKey);
	CString ps,pn;
	
	CString t0,t1;
	BOOL ok;
	ok = t0.LoadString(IDS_UNIT_T_DEGC_PER_SEC);
	ok = t1.LoadString(IDS_UNIT_T_DEGF_PER_SEC);
	CString s0,s1,s2,s3;
	ok = s0.LoadString(IDS_UNIT_P_CMH2O);
	ok = s3.LoadString(IDS_UNIT_P_MMHG);
	ok = s1.LoadString(IDS_UNIT_P_kPA);
	ok = s2.LoadString(IDS_UNIT_P_BAR);
	
	PAGE *pageP = page;
	int numP = getNumPages();
	for (int i = 0 ; i < numP ; i++) {
		
		ps = pageP->regName;
		
		BOOL ok = reg.WriteProfileInt(ps,regsVersion,channelConfigVersion);
		CString pt(pageP->pageTxt);
		ok = reg.WriteProfileString(ps,regsPageName,pt);
		ok = reg.WriteProfileInt(ps,regsEnabled,pageP->enabled);

		ok = reg.WriteProfileDouble(ps,regsStartTime,&pageP->secStart);
		ok = reg.WriteProfileDouble(ps,regsStopTime,&pageP->secStop);

		CString sects;
		SECTION *sectionP = pageP->section;
		int numSct = getSectionsPerPage();
		for (int j = 0 ; j < numSct ; j++) {
			sects = ps;
			sects += _T("\\");
			sects += sectionP->regName;
	
			ok = reg.WriteProfileInt(sects,regsEnabled,sectionP->enabled);
			ok = reg.WriteProfileInt(sects,regsUseGrid,sectionP->useGrid);

			ok = reg.WriteProfileInt(sects,regsPressUnit,sectionP->pressUnit);
			ok = reg.WriteProfileInt(sects,regsTempUnit,sectionP->tempUnit);
			
			CString chnls;
			int numChannels = getChannelsPerSection();
			CH_SPEC *chP = sectionP->chSpec;
			for (int k = 0 ; k < numChannels ; k++) {
				chnls = sects;
				chnls += _T("\\");
				chnls += chP->regName;
				
				BOOL ok= (UINT) reg.WriteProfileInt(chnls,regsChannelID,	chP->channelID);
				ok = reg.WriteProfileInt(chnls,regsChannelFamily,			chP->family);
				ok = reg.WriteProfileColor(chnls,regsChannelColour,			&chP->colour);
				ok = reg.WriteProfileInt(chnls,regsChannelDescriptionID,	chP->descriptionStringID);
				ok = reg.WriteProfileInt(chnls,regsChannelTooltipID,		chP->tooltipStringID);
				ok = reg.WriteProfileInt(chnls,regsChannelTransparency,		chP->transparency);
				ok = reg.WriteProfileInt(chnls,regsChannelThickness,		chP->thickness);
				ok = reg.WriteProfileInt(chnls,regsChannelUserNameID,		chP->userNameStringID);
				ok = reg.WriteProfileInt(chnls,regsChannelType,				chP->dataTypes);
				ok = reg.WriteProfileInt(chnls,regsChannelShowType,			chP->showDataTypes);
				ok = reg.WriteProfileInt(chnls,regsExportUnitString0,		chP->unitStrID0);
				ok = reg.WriteProfileInt(chnls,regsExportUnitString1,		chP->unitStrID1);
				ok = reg.WriteProfileDouble(chnls,regsMaxYString,			&chP->maxY);
				ok = reg.WriteProfileDouble(chnls,regsMinYString,			&chP->minY);
				chP++;
			}
			sectionP++;
		}
		pageP++;
	}
	return TRUE;
}

BOOL CChRegInterface::read(const CString _iniFile) 
{
	CXTRegistryManager reg;
	if (_iniFile != _T("")) reg.SetINIFileName(_iniFile);
	else reg.SetRegistryKey(_T("Spiro Medical"),profileKey);
	PAGE *pageP = page;
	int numP = getNumPages();
	for (int i = 0 ; i < numP ; i++) {
		CString ps(pageP->regName);
		CString pt(pageP->pageTxt);
		pt = reg.GetProfileString(ps,regsPageName,pt);
		cstringToChar(pt,pageP->pageTxt,pageTxtLen);
		page->enabled = (BOOL) reg.GetProfileInt(ps,regsEnabled,pageP->enabled);

		BOOL ok = reg.GetProfileDouble(ps,regsStartTime,&pageP->secStart);
		ok = reg.GetProfileDouble(ps,regsStopTime,&pageP->secStop);
		
		CString sects;
		SECTION *sectionP = pageP->section;
		int numSct = getSectionsPerPage();
		for (int j = 0 ; j < numSct ; j++) {
			sects = ps;
			sects += _T("\\");
			CString s3(sectionP->regName);
			sects += s3;
	
		    sectionP->enabled = (BOOL) reg.GetProfileInt(sects,regsEnabled,	sectionP->enabled);
			sectionP->useGrid = (BOOL) reg.GetProfileInt(sects,regsUseGrid,	sectionP->useGrid);
			sectionP->pressUnit =  reg.GetProfileInt(sects,regsPressUnit,	sectionP->pressUnit);
			sectionP->tempUnit =  reg.GetProfileInt(sects,regsTempUnit,		sectionP->tempUnit);

			CString chnls;
			CH_SPEC *chP = sectionP->chSpec;
			int numChannels = getChannelsPerSection();
			for (int k = 0 ; k < numChannels ; k++) {
				chnls = sects;
				chnls += _T("\\");
				CString s4(chP->regName);
				chnls += s4;
				
				chP->channelID = (UINT) reg.GetProfileInt(chnls,regsChannelID,					chP->channelID);
				chP->family = reg.GetProfileInt(chnls,regsChannelFamily,						chP->family);
				BOOL ok = reg.GetProfileColor(chnls,regsChannelColour,							&chP->colour);
				chP->descriptionStringID = reg.GetProfileInt(chnls,regsChannelDescriptionID,	chP->descriptionStringID);
				chP->tooltipStringID = reg.GetProfileInt(chnls,regsChannelTooltipID,			chP->tooltipStringID);
				chP->transparency = (UINT) reg.GetProfileInt(chnls,regsChannelTransparency,		chP->transparency);
				chP->thickness = reg.GetProfileInt(chnls,regsChannelThickness,					chP->thickness);
				chP->userNameStringID = (UINT) reg.GetProfileInt(chnls,regsChannelUserNameID,	chP->userNameStringID);
				chP->dataTypes = (BOOL) reg.GetProfileInt(chnls,regsChannelType,					chP->dataTypes);
				chP->showDataTypes = (BOOL) reg.GetProfileInt(chnls,regsChannelShowType,				chP->showDataTypes);
				chP->unitStrID0 = reg.GetProfileInt(chnls,regsExportUnitString0,				chP->unitStrID0);
				chP->unitStrID1 = reg.GetProfileInt(chnls,regsExportUnitString1,				chP->unitStrID1);
				
				ok = reg.GetProfileDouble(chnls,regsMaxYString,&chP->maxY);
				ok = reg.GetProfileDouble(chnls,regsMinYString,&chP->minY);
				chP++;
			}
			sectionP++;
		}
		pageP++;
	}
	return TRUE;
}

///////////////////////////////////////

CChXMLInterface::CChXMLInterface() 
{
	CXTRegistryManager reg;
	defaultFile = reg.GetProfileString(regFilesStr,regDefaultGraphXML,_T("C:\\ProgramData\\Spiro Medical\\AGS Analysis\\defaultGraphView.xml"));
	lastUsedFile = reg.GetProfileString(regFilesStr,regLastUsedGraphXML,_T("C:\\ProgramData\\Spiro Medical\\AGS Analysis\\lastUsedGraphView.xml"));
	
	bool isMemMappedPresent = mmFile->getFileExists();

	if (!isMemMappedPresent) {
		BOOL ok = read(lastUsedFile);
		if (!ok) read(); // Default
	}
	else updateFromMemMap();
}

CChXMLInterface::~CChXMLInterface() 
{
	save(lastUsedFile);
}

void CChXMLInterface::readDefaults(void)
{
	read();  
}

BOOL CChXMLInterface::read(const CString _file) 
{
	CString fileName;
	if (_T("") == _file) 
		fileName = defaultFile;
	else fileName = _file;

	//---Read the XML file
	HRESULT hr = S_OK;
	IStream *pFileStream = NULL;
	if (FAILED(hr = SHCreateStreamOnFile(fileName, STGM_READ, &pFileStream))) 
    { 
		CString warn;
		BOOL ok = warn.LoadString(IDS_DEFAULT_XML_WARNING);
		warn += fileName;
		AfxMessageBox(warn);
		return FALSE;
    } 

	IXmlReader *pReader = NULL; 
	if (FAILED(hr = CreateXmlReader(__uuidof(IXmlReader), (void**) &pReader, NULL))) 
    { 
		CString errfmt,err;
		BOOL ok = errfmt.LoadString(IDS_ERROR_XML_CREATE_READER);
		err.Format(errfmt,hr);
		AfxMessageBox(err);
		return FALSE;
    }  
	if (FAILED(hr = pReader->SetProperty(XmlReaderProperty_DtdProcessing, DtdProcessing_Prohibit))) 
    { 
		CString errfmt,err;
		BOOL ok = errfmt.LoadString(IDS_ERROR_XML_SETDTDPROC);
		err.Format(errfmt,hr);
		AfxMessageBox(err);
		return FALSE;
    } 
 
    if (FAILED(hr = pReader->SetInput(pFileStream))) 
    { 
		CString errfmt,err;
		BOOL ok = errfmt.LoadString(IDS_ERROR_XML_SETTING_INPUT_FOR_READER);
		err.Format(errfmt,hr);
		AfxMessageBox(err);
		return FALSE;
    } 
	
	//---Read until there are no more nodes 
	XmlNodeType nodeType;
    const WCHAR* pwszPrefix; 
    const WCHAR* pwszLocalName; 
    const WCHAR* pwszValue; 
    UINT cwchPrefix; 
	bool fileOK = true;
	foundInXML(true,NULL,0);  // Reset flags
	CArray <CString,CString> attributes,attrValues;
    while (S_OK == (hr = pReader->Read(&nodeType))) 
    { 
        switch (nodeType) 
        { 
        case XmlNodeType_XmlDeclaration: 
            if (FAILED(hr = GetAttributes(pReader,&attributes,&attrValues)))
            { 
				CString errfmt,err;
				BOOL ok = errfmt.LoadString(IDS_ERROR_XML_WRITING_ATTRIBUTES);
				err.Format(errfmt,hr);
				AfxMessageBox(err);
				fileOK = false;
            } 
			attributes.RemoveAll();
			attrValues.RemoveAll();
            break; 
        case XmlNodeType_Element: 
            if (FAILED(hr = pReader->GetPrefix(&pwszPrefix, &cwchPrefix))) 
            { 
				CString errfmt,err;
				BOOL ok = errfmt.LoadString(IDS_ERROR_XML_ERROR_GETTING_PREFIX);
				err.Format(errfmt,hr);
				AfxMessageBox(err);
				fileOK = false;
            } 
            if (FAILED(hr = pReader->GetLocalName(&pwszLocalName, NULL))) 
            { 
				CString errfmt,err;
				BOOL ok = errfmt.LoadString(IDS_ERROR_XML_ERROR_GETTING_LOCAL_NAME);
				err.Format(errfmt,hr);
				AfxMessageBox(err);
				fileOK = false;
            } 
            if (FAILED(hr = GetAttributes(pReader,&attributes,&attrValues)))
            { 
				CString errfmt,err;
				BOOL ok = errfmt.LoadString(IDS_ERROR_XML_ERROR_WRITING_ATTRIBUTES);
				err.Format(errfmt,hr);
				AfxMessageBox(err);
				fileOK = false;
            } 
            if (cwchPrefix > 0) 
				;
            else {
				if (attributes.GetCount()) {
					fileOK = foundInXML(false,pwszLocalName,XML_TYPE_START_ELEMENT,&attributes,&attrValues);
					attributes.RemoveAll();
					attrValues.RemoveAll();
				}
				else fileOK = foundInXML(false,pwszLocalName,XML_TYPE_START_ELEMENT);
			}
 
 
            //if (pReader->IsEmptyElement() ) ;
            break; 
        case XmlNodeType_EndElement: 
            if (FAILED(hr = pReader->GetPrefix(&pwszPrefix, &cwchPrefix))) 
            { 
				CString errfmt,err;
				BOOL ok = errfmt.LoadString(IDS_ERROR_XML_ERROR_GETTING_PREFIX);
				err.Format(errfmt,hr);
				AfxMessageBox(err);
				fileOK = false;
            } 
            if (FAILED(hr = pReader->GetLocalName(&pwszLocalName, NULL))) 
            { 
				CString errfmt,err;
				BOOL ok = errfmt.LoadString(IDS_ERROR_XML_ERROR_GETTING_LOCAL_NAME);
				err.Format(errfmt,hr);
				AfxMessageBox(err);
				fileOK = false;
            } 
            if (cwchPrefix > 0) ;
            else {
				fileOK = foundInXML(false,pwszLocalName,XML_TYPE_END_ELEMENT);
			}
            break; 
        case XmlNodeType_Text: 
        case XmlNodeType_Whitespace: 
            if (FAILED(hr = pReader->GetValue(&pwszValue, NULL))) 
            { 
				CString errfmt,err;
				BOOL ok = errfmt.LoadString(IDS_ERROR_XML_ERROR_GETTING_VALUE);
				err.Format(errfmt,hr);
				AfxMessageBox(err);
				fileOK = false;
            } 
            //TRACE(L"Text: >%s<\n", pwszValue); // Value here
			/*fileOK =*/ 
			foundInXML(false,pwszValue,XML_TYPE_WHITESPACE);
            break; 
        case XmlNodeType_CDATA: 
            if (FAILED(hr = pReader->GetValue(&pwszValue, NULL))) 
            { 
				CString errfmt,err;
				BOOL ok = errfmt.LoadString(IDS_ERROR_XML_ERROR_GETTING_VALUE);
				err.Format(errfmt,hr);
				AfxMessageBox(err);
				fileOK = false;
            } 
            //TRACE(L"CDATA: %s\n", pwszValue); // CData here
            break; 
        case XmlNodeType_ProcessingInstruction: 
            if (FAILED(hr = pReader->GetLocalName(&pwszLocalName, NULL))) 
            { 
				CString errfmt,err;
				BOOL ok = errfmt.LoadString(IDS_ERROR_XML_ERROR_GETTING_LOCAL_NAME);
				err.Format(errfmt,hr);
				AfxMessageBox(err);
				fileOK = false;
            } 
            if (FAILED(hr = pReader->GetValue(&pwszValue, NULL))) 
            { 
				CString errfmt,err;
				BOOL ok = errfmt.LoadString(IDS_ERROR_XML_ERROR_GETTING_VALUE);
				err.Format(errfmt,hr);
				AfxMessageBox(err);
				fileOK = false;
            } 
            //TRACE(L"Processing Instruction name:%s value:%s\n", pwszLocalName, pwszValue); 
            break; 
        case XmlNodeType_Comment: 
            if (FAILED(hr = pReader->GetValue(&pwszValue, NULL))) 
            { 
				CString errfmt,err;
				BOOL ok = errfmt.LoadString(IDS_ERROR_XML_ERROR_GETTING_VALUE);
				err.Format(errfmt,hr);
				AfxMessageBox(err);
				fileOK = false;
            } 
            //TRACE(L"Comment: %s\n", pwszValue); 
            break; 
        case XmlNodeType_DocumentType: 
            //TRACE(L"DOCTYPE is not printed\n"); 
            break; 
        } 
		if (!fileOK) break;
    } 

	SAFE_RELEASE(pFileStream); 
	SAFE_RELEASE(pReader); 	

	if (!fileOK) {
		CString s;
		BOOL ok = s.LoadString(IDS_ERROR_APN_XML_FILE);
		s += _T(" (");
		s += fileName;
		s += _T(")");
		AfxMessageBox(s);
	}
	checkAndCorrectUnits();
	bool OK = checkAndCorrectMinMaxYValues();
	//if (!OK) TRACE(_T("Found min/max Y values that were not OK!\n"));

	bool ok = updateToMemMap();
	//if (!ok) TRACE(_T("Could not update to mem mapped file\n"));
	return TRUE;
}

HRESULT CChXMLInterface::GetAttributes(IXmlReader* pReader,CArray <CString,CString> *_attributes,CArray <CString,CString> *_values) 
{ 
    const WCHAR* pwszPrefix; 
    const WCHAR* pwszLocalName; 
    const WCHAR* pwszValue; 
    HRESULT hr = pReader->MoveToFirstAttribute(); 
 
    if (S_FALSE == hr) 
        return hr; 
    if (S_OK != hr) 
    { 
        //TRACE(L"Error moving to first attribute, error is %08.8lx\n", hr); 
        return hr; 
    } 
    else 
    { 
        while (TRUE) 
        { 
            if (!pReader->IsDefault()) 
            { 
                UINT cwchPrefix; 
                if (FAILED(hr = pReader->GetPrefix(&pwszPrefix, &cwchPrefix))) 
                { 
                    TRACE(L"Error getting prefix, error is %08.8lx\n", hr); 
                    return hr; 
                } 
                if (FAILED(hr = pReader->GetLocalName(&pwszLocalName, NULL))) 
                { 
                    TRACE(L"Error getting local name, error is %08.8lx\n", hr); 
                    return hr; 
                } 
                if (FAILED(hr = pReader->GetValue(&pwszValue, NULL))) 
                { 
                    TRACE(L"Error getting value, error is %08.8lx\n", hr); 
                    return hr; 
                } 
                if (cwchPrefix > 0) {
                    TRACE(L"Attr: %s:%s=\"%s\" \n", pwszPrefix, pwszLocalName, pwszValue); 
					_attributes->Add(pwszLocalName);
					_values->Add(pwszValue);
				}
                else {
                    //TRACE(L"Attr: %s=\"%s\" \n", pwszLocalName, pwszValue);
					_attributes->Add(pwszLocalName);
					_values->Add(pwszValue); 
				}
            } 
 
            if (S_OK != pReader->MoveToNextAttribute()) 
                break; 
        } 
    } 
    return hr; 
}

/*
Description: Generates a bit string from an UINT
*/
CString CChXMLInterface::bitStringFromUint(const UINT _int)
{
	CString s;
	int myInt = _int;
	int len = sizeof (_int) * 8;
	for (int i = 0 ; i < len ; i++) {
		s += myInt & 0x01 ? _T("1") : _T("0");
		myInt = myInt >> 1;
	}
	s.MakeReverse();
	return s;
}

/*
Description: Generates an UINT from bit string
*/
UINT CChXMLInterface::uintFromBitString(const CString _s)
{
	CString s = _s;
	int len1 = sizeof (UINT) * 8;
	int len2 = s.GetLength();
	int len = len1 < len2 ? len1 : len2;
	UINT ret = 0;
	for (int i = 0 ; i < len ; i++) {
		ret = ret << 1;
		if (_T('1') == s.GetAt(i)) ret += 1 ;
	}
	return ret;
}


/*
Description: Decodes an xml element of a specific type.
Returns: Status of the flag fileOK. fileOK is static and set to true when the tagApneaGraphView is found.
		If this flag is false after all calls to this methiod have been done - the file has not been accepted
*/
bool CChXMLInterface::foundInXML(bool _resetFlags,CString _string,int _type,
								CArray <CString,CString> *_attributes,CArray <CString,CString> *_values)
{
	static bool inPage = false;
	static bool inSection = false;
	static bool inTimeAxis = false;
	static bool inStart = false;
	static bool inStop = false;
	static bool inUseGrid = false;
	static bool inEngUnits = false;
	static bool inTempUnit = false;
	static bool inPressUnit = false;
	static bool inChannel = false;
	static bool inColor = false;
	static bool inThickness = false;
	static bool inTransparency = false;
	static bool inMinimumY = false;
	static bool inMaximumY = false;
	static bool fileOK = false;

	static int  pageNumber = 0;
	static int  sectionNumber = 0;
	static int  currentChannelNumber = -1;

	if (_resetFlags) {
		inPage = false;
		inSection = false;
		inTimeAxis = false;
		inStart = false;
		inStop = false;
		inUseGrid = false;
		inEngUnits = false;
		inTempUnit = false;
		inPressUnit = false;
		inChannel = false;
		inColor = false;
		inThickness = false;
		inTransparency = false;
		inMinimumY = false;
		inMaximumY = false;
		fileOK = false;
		
		pageNumber = 0;
		sectionNumber = 0;
		currentChannelNumber = -1;
		return false;
	}
	CString currentChannelEnabledFlags;

	int minPageNumber = 0;
	int maxPageNumber = getNumPages() - 1;
	maxPageNumber = maxPageNumber < minPageNumber ? 0 : maxPageNumber;
	
	int minSectionNumber = 0;
	int maxSectionNumber = getSectionsPerPage() - 1;
	maxSectionNumber = maxSectionNumber < minSectionNumber ? 0 : maxSectionNumber;

	int minChannelNumber = 0;
	int maxChannelNumber = getChannelsPerSection() - 1;
	maxChannelNumber = maxChannelNumber < minChannelNumber ? 0 : maxChannelNumber;

	switch (_type) {
	case XML_TYPE_START_ELEMENT:
		if (tagApneaGraphView == _string) {
			fileOK = true;
			if (_attributes && _values) {
				int num = _attributes->GetCount();
				if (num == _values->GetCount()) {
					for (int i = 0 ; i < num ; i++) {
						CString nm = _attributes->GetAt(i);
						CString val = _values->GetAt(i);
						if (attrVersion == nm) xmlVersion = val;
					}
				}
			}
		}
		if (fileOK && (tagPage == _string)) {
			inPage = true;
			sectionNumber = 0;
			if (_attributes && _values) {
				int num = _attributes->GetCount();
				if (num == _values->GetCount()) {
					if ((pageNumber >= minPageNumber) && (pageNumber <= maxPageNumber)) {
						page[pageNumber].enabled = true; // If attribute is missing, assume enabled
						for (int i = 0 ; i < num ; i++) {
							CString nm = _attributes->GetAt(i);
							CString val = _values->GetAt(i);
							if (attrName == nm) 
								cstringToChar(val,page[pageNumber].pageTxt,pageTxtLen);
							if (attrEnabled == nm) 
								page[pageNumber].enabled = val == _T("0") ? false : true;
						}
					}
				}
			}
		}
		if (inPage && (tagSection == _string)) {
			inSection = true;
			currentChannelNumber = 0;
			if (_attributes && _values) {
				int num = _attributes->GetCount();
				if (num == _values->GetCount()) {
					if ((pageNumber >= minPageNumber) && (pageNumber <= maxPageNumber) && (sectionNumber >= minSectionNumber) && (sectionNumber <= maxSectionNumber)) {
						page[pageNumber].section[sectionNumber].enabled = true; // If attribute is missing, assume enabled
						for (int i = 0 ; i < num ; i++) {
							CString nm = _attributes->GetAt(i);
							CString val = _values->GetAt(i);
							if ((sectionNumber >= minSectionNumber) && (sectionNumber <= maxSectionNumber)) {
								if (attrName == nm) 
									cstringToChar(val,page[pageNumber].section[sectionNumber].sectionTxt,sectionTxtLen);
								if (attrEnabled == nm) 
									page[pageNumber].section[sectionNumber].enabled = val == _T("0") ? false : true;
							}
						}
					}
				}
			}
		}
				
		if (inSection && (tagChannel == _string)) {
			inChannel = true;
			if (_attributes && _values) {
				int num = _attributes->GetCount();
				bool okChannelID = true;
				if (num == _values->GetCount()) {
					currentChannelEnabledFlags = _T("0000");
					for (int i = 0 ; i < num ; i++) {
						CString nm = _attributes->GetAt(i);
						CString val = _values->GetAt(i);
						if (attrId == nm) {
							currentChannelNumber = getChannelNumberFromIdString(val);
							if (ERROR_CH_ID == currentChannelNumber) {
								okChannelID = false;
								CString s;
								BOOL ok = s.LoadString(IDS_ERROR_XML_CHANNELID_ERROR);
								s += val;
								AfxMessageBox(s);
								fileOK = false;
								return fileOK;
							}
						}
						if (attrEnabled == nm) 
							currentChannelEnabledFlags = val;
					}
				}
				if (okChannelID && 
					(pageNumber >= minPageNumber) && 
					(pageNumber <= maxPageNumber) && 
					(sectionNumber >= minSectionNumber) && 
					(sectionNumber <= maxSectionNumber) &&
					(currentChannelNumber >= minChannelNumber) &&
					(currentChannelNumber <= maxChannelNumber)) 
					page[pageNumber].section[sectionNumber].chSpec[currentChannelNumber].showDataTypes = uintFromBitString(currentChannelEnabledFlags);
			}
		}

		if (inPage && (tagTimeAxis == _string)) inTimeAxis = true;
		if (inTimeAxis && (tagStart == _string)) inStart = true;
		if (inTimeAxis && (tagStop == _string)) inStop = true;
		if (inSection && (tagUseGrid == _string)) inUseGrid = true;
		if (inSection && (tagEngineeringUnits == _string)) inEngUnits = true;
		if (inEngUnits && (tagPressUnit == _string)) inPressUnit = true;
		if (inEngUnits && (tagTempUnit == _string)) inTempUnit = true;

		if (inChannel && (tagColor == _string)) inColor = true;
		if (inChannel && (tagThickness == _string)) inThickness = true;
		if (inChannel && (tagTransparency == _string)) inTransparency = true;
		if (inChannel && (tagMinimumY == _string)) inMinimumY = true;
		if (inChannel && (tagMaximumY == _string)) inMaximumY = true;
		break;
	case XML_TYPE_VALUE:
		break;
	case XML_TYPE_END_ELEMENT:
		if (tagPage == _string) {
			inPage = false;
			pageNumber++;
		}
		if (inPage && (tagSection == _string)) {
			inSection = false;
			sectionNumber++;
		}
		if (inPage && (tagTimeAxis == _string)) inTimeAxis = false;
		if (inTimeAxis && (tagStart == _string)) inStart = false;
		if (inTimeAxis && (tagStop == _string)) inStop = false;
		if (inSection && (tagUseGrid == _string)) inUseGrid = false;
		if (inSection && (tagEngineeringUnits == _string)) inEngUnits = false;
		if (inEngUnits && (tagPressUnit == _string)) inPressUnit = false;
		if (inEngUnits && (tagTempUnit == _string)) inTempUnit = false;
		
		if (inSection && (tagChannel == _string)) inChannel = false;
		if (inChannel && (tagColor == _string)) inColor = false;
		if (inChannel && (tagThickness == _string)) inThickness = false;
		if (inChannel && (tagTransparency == _string)) inTransparency = false;
		if (inChannel && (tagMinimumY == _string)) inMinimumY = false;
		if (inChannel && (tagMaximumY == _string)) inMaximumY = false;
		break;
	case XML_TYPE_XML_DECLARATION:
		break;
	case XML_TYPE_WHITESPACE:
		if ((pageNumber >= minPageNumber) && (pageNumber <= maxPageNumber)) {
			if (inStop) page[pageNumber].secStop = (int) stringToTime(_string);
			if (inStart) page[pageNumber].secStart = (int) stringToTime(_string);
			if ((sectionNumber >= minSectionNumber) && (sectionNumber <= maxSectionNumber)) {
				if (inUseGrid) page[pageNumber].section[sectionNumber].useGrid = _string == _T("0") ? false : true;
				if (inTempUnit) page[pageNumber].section[sectionNumber].tempUnit = getTempUnitStringConstantFromIndex(_ttoi(_string));
				if (inPressUnit) page[pageNumber].section[sectionNumber].pressUnit = getPressUnitStringConstantFromIndex(_ttoi(_string));
				if ((ERROR_CH_ID != currentChannelNumber) && (currentChannelNumber >= minChannelNumber) && (currentChannelNumber <= maxChannelNumber)) {
					if (inColor) 
						page[pageNumber].section[sectionNumber].chSpec[currentChannelNumber].colour =  colorrefFromString(_string);
					if (inThickness)
						page[pageNumber].section[sectionNumber].chSpec[currentChannelNumber].thickness =  getAcceptedThickness(_ttoi(_string));
					if (inTransparency)
						page[pageNumber].section[sectionNumber].chSpec[currentChannelNumber].transparency =  getAcceptedTransparency((long) _ttoi(_string));
					if (inMinimumY) {
						double newVal = (double) _ttof(_string);
						page[pageNumber].section[sectionNumber].chSpec[currentChannelNumber].minY =  newVal;
					}
					if (inMaximumY) {
						double newVal = (double) _ttof(_string);
						page[pageNumber].section[sectionNumber].chSpec[currentChannelNumber].maxY =  newVal;
					}
				}
			}
		}
		break;
	default:
		break;
	}
	return fileOK;
}


BOOL CChXMLInterface::save(const CString _file) 
{
	CString fileName = _file;
	
	//---Write to the XML file
	HRESULT hr = S_OK;
	IStream *pFileStream = NULL;
	if (FAILED(hr = SHCreateStreamOnFile(fileName, STGM_WRITE | STGM_CREATE, &pFileStream))) 
    { 
		CString warn;
		BOOL ok = warn.LoadString(IDS_DEFAULT_XML_WARNING);
		warn += fileName;
		TRACE(_T("%s\n"),warn);
		return FALSE;
    } 
	IXmlWriter *pWriter = NULL; 
	if (FAILED(hr = CreateXmlWriter(__uuidof(IXmlWriter), (void**) &pWriter, NULL))) 
    { 
		CString errfmt,err;
		BOOL ok = errfmt.LoadString(IDS_ERROR_XML_CREATE_WRITER);
		err.Format(errfmt,hr);
		TRACE(_T("%s\n"),err);
		return FALSE;
    }  
	if (FAILED(hr = pWriter->SetOutput(pFileStream))) 
    { 
		CString errfmt,err;
		BOOL ok = errfmt.LoadString(IDS_ERROR_XML_SETTING_OUTPUT_FOR_WRITER);
		err.Format(errfmt,hr);
		TRACE(_T("%s\n"),err);
		return FALSE;
    } 
	pWriter->SetProperty(XmlWriterProperty_Indent, TRUE);

    // Write XML data.
    hr = pWriter->WriteStartDocument(XmlStandalone_Omit); 

	CString chNames,s;
	chNames = _T("\t\t");
	int numC = getChannelsPerSection();
	for (int i = 0 ; i < numC ; i++) {
		s = _T("--");
		BOOL ok = s.LoadString(page[0].section[0].chSpec[i].userNameStringID);
		chNames += s;
		chNames += _T("\n\t\t");
	}
	CString xmlComment = xmlComment11;
	xmlComment += xmlComment12;
	xmlComment += xmlComment13;
	xmlComment += xmlComment14;
	xmlComment += chNames;
	xmlComment += xmlComment21;
	xmlComment += xmlComment22;
	xmlComment += xmlComment23;
	xmlComment += xmlComment24;
	xmlComment += xmlComment25;
	xmlComment += xmlComment26;
	xmlComment += xmlComment30;
	xmlComment += xmlComment32;
	xmlComment += xmlComment33;
    if (FAILED(hr = pWriter->WriteComment(xmlComment))) {
		CString errfmt,err;
		BOOL ok = errfmt.LoadString(IDS_ERROR_XML_CREATE_WRITER);
		err.Format(errfmt,hr);
		TRACE(_T("%s\n"),err);
		return FALSE;
	}
	
	hr = pWriter->WriteStartElement(NULL,tagApneaGraphView,NULL);
	hr = pWriter->WriteAttributeString(NULL,attrVersion,NULL,xmlWriteVersion);
	
	int numP = getNumPages();
	for (int ip = 0 ; ip < numP; ip++) {
		hr = pWriter->WriteStartElement(NULL,tagPage,NULL);
		hr = pWriter->WriteAttributeString(NULL,attrEnabled,NULL,page[ip].enabled ? _T("1") : _T("0"));
		CString st(page[ip].pageTxt);
		hr = pWriter->WriteAttributeString(NULL,attrName,NULL,st);

		hr = pWriter->WriteStartElement(NULL,tagTimeAxis,NULL);
		hr = pWriter->WriteElementString(NULL,tagStart,NULL,getTimeStartStr(ip));
		hr = pWriter->WriteElementString(NULL,tagStop,NULL,getTimeStopStr(ip));
		hr = pWriter->WriteFullEndElement();

		int numS = getSectionsPerPage();
		for (int iS = 0 ; iS < numS ; iS++) {
			hr = pWriter->WriteStartElement(NULL,tagSection,NULL);
			hr = pWriter->WriteAttributeString(NULL,attrEnabled,NULL,page[ip].section[iS].enabled ? _T("1") : _T("0"));
			CString sct(page[ip].section[iS].sectionTxt);
			hr = pWriter->WriteAttributeString(NULL,attrName,NULL,sct);
			hr = pWriter->WriteElementString(NULL,tagUseGrid,NULL,page[ip].section[iS].useGrid ? _T("1") : _T("0"));
			
			// Eng unit
			hr = pWriter->WriteStartElement(NULL,tagEngineeringUnits,NULL);
			CString s;
			s.Format(_T("%d"),getIndexFromPressUnitStringConstant(page[ip].section[iS].pressUnit));
			hr = pWriter->WriteElementString(NULL,tagPressUnit,NULL,s);
			s.Format(_T("%d"),getIndexFromTempUnitStringConstant(page[ip].section[iS].tempUnit));
			hr = pWriter->WriteElementString(NULL,tagTempUnit,NULL,s);
			hr = pWriter->WriteFullEndElement();

			// Channels
			int numC = getChannelsPerSection();
			for (int iC = 0 ; iC < numC ; iC++) {
				hr = pWriter->WriteStartElement(NULL,tagChannel,NULL);
				CString s;
				BOOL ok = s.LoadString(page[ip].section[iS].chSpec[iC].userNameStringID);
				hr = pWriter->WriteAttributeString(NULL,attrId,NULL,s);
				hr = pWriter->WriteAttributeString(NULL,attrEnabled,NULL,bitStringFromUint((UINT) page[ip].section[iS].chSpec[iC].showDataTypes));
				s.Format(_T("%d,%d,%d"),
					GetRValue(page[ip].section[iS].chSpec[iC].colour),
					GetGValue(page[ip].section[iS].chSpec[iC].colour),
					GetBValue(page[ip].section[iS].chSpec[iC].colour));
				hr = pWriter->WriteElementString(NULL,tagColor,NULL,s);
				CString thx;
				thx.Format(_T("%d"),page[ip].section[iS].chSpec[iC].thickness);
				hr = pWriter->WriteElementString(NULL,tagThickness,NULL,thx);
				s.Format(_T("%ld"),page[ip].section[iS].chSpec[iC].transparency);
				hr = pWriter->WriteElementString(NULL,tagTransparency,NULL,s);
				s.Format(_T("%.3f"),page[ip].section[iS].chSpec[iC].maxY);
				hr = pWriter->WriteElementString(NULL,tagMaximumY,NULL,s);
				s.Format(_T("%.3f"),page[ip].section[iS].chSpec[iC].minY);
				hr = pWriter->WriteElementString(NULL,tagMinimumY,NULL,s);
				hr = pWriter->WriteFullEndElement();
			}
			hr = pWriter->WriteFullEndElement();
		}
		hr = pWriter->WriteFullEndElement();
	}
	hr = pWriter->WriteFullEndElement();
    pWriter->Flush();

	SAFE_RELEASE(pFileStream); 
	SAFE_RELEASE(pWriter); 	
	return FALSE;
}
