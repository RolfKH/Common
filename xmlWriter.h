#ifndef XML_WRITE_H_INCLUDED
#define XML_WRITE_H_INCLUDED

/*
Rev	By	Description
1	RKH Each channel n	
*/

/*!
* @author		RKH
* @date		06.06.2017
* @copyright (C)Spiro Medical AS 2013 - 2017
*/

#include <xmllite.h>

#define AGS_XML_FILE_VERSION		_T("11.0")   // Not used in Analysis!

class CXmlWriter
{
public:
	CXmlWriter();
	~CXmlWriter();
	bool saveData(void);
protected:
	void writeResultEntries(IXmlWriter *_pWriter);
};

#endif
