#include "stdafx.h"
#include "xmlWriter.h"

/*!
* @author		RKH
* @date		06.06.2017
* @copyright (C)Spiro Medical AS 2013 - 2017
*/

CXmlWriter::CXmlWriter()
{
}

CXmlWriter::~CXmlWriter()
{
}

bool CXmlWriter::saveData(void)
{
	CString fileName = _T("C:\\Kladd\\XMLtest.xml");
	
	//---Write to the XML file
	HRESULT hr = S_OK;
	IStream *pFileStream = NULL;
	if (FAILED(hr = SHCreateStreamOnFile(fileName, STGM_WRITE | STGM_CREATE, &pFileStream))) 
    { 
	//	CString warn;
		//BOOL ok = warn.LoadString(IDS_DEFAULT_XML_WARNING);
	//	warn += fileName;
	//	TRACE(_T("%s\n"),warn);
		return FALSE;
    } 
	IXmlWriter *pWriter = NULL; 
	if (FAILED(hr = CreateXmlWriter(__uuidof(IXmlWriter), (void**) &pWriter, NULL))) 
    { 
	//	CString errfmt,err;
	//	BOOL ok = errfmt.LoadString(IDS_ERROR_XML_CREATE_WRITER);
	//	err.Format(errfmt,hr);
	//	TRACE(_T("%s\n"),err);
		return FALSE;
    }  
	if (FAILED(hr = pWriter->SetOutput(pFileStream))) 
    { 
	//	CString errfmt,err;
	//	BOOL ok = errfmt.LoadString(IDS_ERROR_XML_SETTING_OUTPUT_FOR_WRITER);
	//	err.Format(errfmt,hr);
	//	TRACE(_T("%s\n"),err);
		return FALSE;
    } 
	pWriter->SetProperty(XmlWriterProperty_Indent, TRUE);

    // Write XML data.
    hr = pWriter->WriteStartDocument(XmlStandalone_Omit); 

	
    if (FAILED(hr = pWriter->WriteComment(_T("This is just a sample")))) {
	//	CString errfmt,err;
	//	BOOL ok = errfmt.LoadString(IDS_ERROR_XML_CREATE_WRITER);
	//	err.Format(errfmt,hr);
	//	TRACE(_T("%s\n"),err);
		return FALSE;
	}

	//---Top node
	LPCWSTR s0 = _T("AGS_XML_file");
	if (FAILED(hr = pWriter->WriteStartElement(NULL,s0,NULL))) {
		TRACE(_T("Err 0\n"));
	}
	if (FAILED(hr = pWriter->WriteAttributeString(NULL,_T("Version"),NULL,AGS_XML_FILE_VERSION))) {
		TRACE(_T("Err 1\n"));
	}
	{

		//---Patient
		LPCWSTR s1 = _T("Patient");
		if (FAILED(hr = pWriter->WriteStartElement(NULL,s1,NULL))) {
			TRACE(_T("Err 0\n"));
		}
		{
			s1 = _T("Name");
			LPCWSTR s2 = _T("Ole Olsen");
			if (FAILED(hr = pWriter->WriteElementString(NULL,s1,NULL,s2))) {
				TRACE(_T("Err 2\n"));
			}
			if (FAILED(hr = pWriter->WriteElementString(NULL,_T("Address"),NULL,_T("Sandviksveien 65, 5050 Bergen, Norway")))) {
				TRACE(_T("Err 3\n"));
			}
			if (FAILED(hr = pWriter->WriteElementString(NULL,_T("ID"),NULL,_T("1234")))) {
				TRACE(_T("Err 3\n"));
			}
			if (FAILED(hr = pWriter->WriteElementString(NULL,_T("Gender"),NULL,_T("Male")))) {
				TRACE(_T("Err 3\n"));
			}
			if (FAILED(hr = pWriter->WriteElementString(NULL,_T("DateOfBirth"),NULL,_T("11.01.1951")))) {
				TRACE(_T("Err 3\n"));
			}
			if (FAILED(hr = pWriter->WriteElementString(NULL,_T("DateOfAGSStudy"),NULL,_T("25.11.2014")))) {
				TRACE(_T("Err 3\n"));
			}
			if (FAILED(hr = pWriter->WriteElementString(NULL,_T("ESS"),NULL,_T("23")))) {
				TRACE(_T("Err 3\n"));
			}
			if (FAILED(hr = pWriter->WriteElementString(NULL,_T("BMI"),NULL,_T("22.0")))) {
				TRACE(_T("Err 3\n"));
			}
			if (FAILED(hr = pWriter->WriteElementString(NULL,_T("NeckCircumference"),NULL,_T("34")))) {
				TRACE(_T("Err 3\n"));
			}
			if (FAILED(hr = pWriter->WriteElementString(NULL,_T("Notes"),NULL,_T("Some notes here....")))) {
				TRACE(_T("Err 3\n"));
			}
		}
		if (FAILED(hr = pWriter->WriteFullEndElement())) {
			TRACE(_T("Err 4\n"));
		}

		//---ref phys	
		LPCWSTR s3 = _T("ReferringPhysician");
		if (FAILED(hr = pWriter->WriteStartElement(NULL,s3,NULL))) {
			TRACE(_T("Err 0\n"));
		}
		{
			LPCWSTR s5 = _T("Name");
			LPCWSTR s4 = _T("Jan Peder Jensen");
			if (FAILED(hr = pWriter->WriteElementString(NULL,s5,NULL,s4))) {
				TRACE(_T("Err 2\n"));
			}
			if (FAILED(hr = pWriter->WriteElementString(NULL,_T("Address"),NULL,_T("Sandviksveien 65, 5050 Bergen, Norway")))) {
				TRACE(_T("Err 3\n"));
			}
			if (FAILED(hr = pWriter->WriteElementString(NULL,_T("ReferralDate"),NULL,_T("12.10.2014")))) {
				TRACE(_T("Err 3\n"));
			}
			if (FAILED(hr = pWriter->WriteElementString(NULL,_T("Email"),NULL,_T("JPJensen@online.no")))) {
				TRACE(_T("Err 3\n"));
			}
			if (FAILED(hr = pWriter->WriteElementString(NULL,_T("Telephone"),NULL,_T("99456324")))) {
				TRACE(_T("Err 3\n"));
			}
			if (FAILED(hr = pWriter->WriteElementString(NULL,_T("Notes"),NULL,_T("Some notes here....")))) {
				TRACE(_T("Err 3\n"));
			}
			if (FAILED(hr = pWriter->WriteFullEndElement())) {
				TRACE(_T("Err 4\n"));
			}
		}
		//---ref phys	
		LPCWSTR s13 = _T("AttendingDoctor");
		if (FAILED(hr = pWriter->WriteStartElement(NULL,s13,NULL))) {
			TRACE(_T("Err 0\n"));
		}
		{
			LPCWSTR s5 = _T("Name");
			LPCWSTR s4 = _T("Oulu Oulo");
			if (FAILED(hr = pWriter->WriteElementString(NULL,s5,NULL,s4))) {
				TRACE(_T("Err 2\n"));
			}
			if (FAILED(hr = pWriter->WriteElementString(NULL,_T("Function"),NULL,_T("Head of section")))) {
				TRACE(_T("Err 3\n"));
			}
			if (FAILED(hr = pWriter->WriteElementString(NULL,_T("Notes"),NULL,_T("Some notes here....")))) {
				TRACE(_T("Err 3\n"));
			}
			if (FAILED(hr = pWriter->WriteFullEndElement())) {
				TRACE(_T("Err 4\n"));
			}
		}
		//---ref phys	
		LPCWSTR s23 = _T("AGSOperator");
		if (FAILED(hr = pWriter->WriteStartElement(NULL,s23,NULL))) {
			TRACE(_T("Err 0\n"));
		}
		{
			LPCWSTR s5 = _T("Name");
			LPCWSTR s4 = _T("Linda Medalen");
			if (FAILED(hr = pWriter->WriteElementString(NULL,s5,NULL,s4))) {
				TRACE(_T("Err 2\n"));
			}
			if (FAILED(hr = pWriter->WriteElementString(NULL,_T("Function"),NULL,_T("Head nurse")))) {
				TRACE(_T("Err 3\n"));
			}
			if (FAILED(hr = pWriter->WriteElementString(NULL,_T("Notes"),NULL,_T("Some notes here....")))) {
				TRACE(_T("Err 3\n"));
			}
			if (FAILED(hr = pWriter->WriteFullEndElement())) {
				TRACE(_T("Err 4\n"));
			}
		}
		//---Units
		LPCWSTR s33 = _T("Units");
		if (FAILED(hr = pWriter->WriteStartElement(NULL,s33,NULL))) {
			TRACE(_T("Err 0\n"));
		}
		{
			if (FAILED(hr = pWriter->WriteElementString(NULL,_T("CatheterUnit"),NULL,_T("00020")))) {
				TRACE(_T("Err 2\n"));
			}
			if (FAILED(hr = pWriter->WriteElementString(NULL,_T("NeckUnit"),NULL,_T("00120")))) {
				TRACE(_T("Err 3\n"));
			}
			if (FAILED(hr = pWriter->WriteElementString(NULL,_T("WristUnit"),NULL,_T("000034")))) {
				TRACE(_T("Err 3\n"));
			}
			if (FAILED(hr = pWriter->WriteElementString(NULL,_T("TorsoUnit"),NULL,_T("000224")))) {
				TRACE(_T("Err 3\n"));
			}
			if (FAILED(hr = pWriter->WriteFullEndElement())) {
				TRACE(_T("Err 4\n"));
			}
		}
		//---Study
		if (FAILED(hr = pWriter->WriteStartElement(NULL,_T("Study"),NULL))) {
			TRACE(_T("Err 0\n"));
		}
		{
			if (FAILED(hr = pWriter->WriteElementString(NULL,_T("StartDate"),NULL,_T("11.10.2014")))) {
				TRACE(_T("Err 2\n"));
			}
			if (FAILED(hr = pWriter->WriteElementString(NULL,_T("StartTime"),NULL,_T("22:00")))) {
				TRACE(_T("Err 3\n"));
			}
			if (FAILED(hr = pWriter->WriteElementString(NULL,_T("RecordingLength"),NULL,_T("6")))) {
				TRACE(_T("Err 3\n"));
			}
		}
		//---Results
		if (FAILED(hr = pWriter->WriteStartElement(NULL,_T("Obstructive"),NULL))) {
			TRACE(_T("Err 0\n"));
		}
		writeResultEntries(pWriter);
		if (FAILED(hr = pWriter->WriteFullEndElement())) {
			TRACE(_T("Err 4\n"));
		}
		
		if (FAILED(hr = pWriter->WriteStartElement(NULL,_T("Mixed"),NULL))) {
			TRACE(_T("Err 0\n"));
		}
		writeResultEntries(pWriter);
		if (FAILED(hr = pWriter->WriteFullEndElement())) {
			TRACE(_T("Err 4\n"));
		}
		if (FAILED(hr = pWriter->WriteStartElement(NULL,_T("ObstrHypopnea"),NULL))) {
			TRACE(_T("Err 0\n"));
		}
		writeResultEntries(pWriter);
		if (FAILED(hr = pWriter->WriteFullEndElement())) {
			TRACE(_T("Err 4\n"));
		}
		if (FAILED(hr = pWriter->WriteStartElement(NULL,_T("SumObstructive"),NULL))) {
			TRACE(_T("Err 0\n"));
		}
		writeResultEntries(pWriter);
		if (FAILED(hr = pWriter->WriteFullEndElement())) {
			TRACE(_T("Err 4\n"));
		}

		if (FAILED(hr = pWriter->WriteStartElement(NULL,_T("Central"),NULL))) {
			TRACE(_T("Err 0\n"));
		}
		writeResultEntries(pWriter);
		if (FAILED(hr = pWriter->WriteFullEndElement())) {
			TRACE(_T("Err 4\n"));
		}
		if (FAILED(hr = pWriter->WriteStartElement(NULL,_T("CentralHypopnea"),NULL))) {
			TRACE(_T("Err 0\n"));
		}
		writeResultEntries(pWriter);
		if (FAILED(hr = pWriter->WriteFullEndElement())) {
			TRACE(_T("Err 4\n"));
		}
		if (FAILED(hr = pWriter->WriteStartElement(NULL,_T("SumCentral"),NULL))) {
			TRACE(_T("Err 0\n"));
		}
		writeResultEntries(pWriter);
		if (FAILED(hr = pWriter->WriteFullEndElement())) {
			TRACE(_T("Err 4\n"));
		}
	}
	
    pWriter->Flush();

	SAFE_RELEASE(pFileStream); 
	SAFE_RELEASE(pWriter); 	
	return true;
}

void CXmlWriter::writeResultEntries(IXmlWriter *_pWriter)
{
	HRESULT hr = S_OK;
	if (FAILED(hr = _pWriter->WriteElementString(NULL,_T("TotalNumber"),NULL,_T("5")))) {
		TRACE(_T("Err 2\n"));
	}
	if (FAILED(hr = _pWriter->WriteElementString(NULL,_T("PerHour"),NULL,_T("0.96")))) {
		TRACE(_T("Err 3\n"));
	}
	if (FAILED(hr = _pWriter->WriteElementString(NULL,_T("TotalDuration"),NULL,_T("01:12")))) {
		TRACE(_T("Err 3\n"));
	}
	if (FAILED(hr = _pWriter->WriteElementString(NULL,_T("MaximumDuration"),NULL,_T("20")))) {
		TRACE(_T("Err 3\n"));
	}
	if (FAILED(hr = _pWriter->WriteElementString(NULL,_T("AverageDuration"),NULL,_T("15")))) {
		TRACE(_T("Err 3\n"));
	}
}