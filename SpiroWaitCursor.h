#ifndef SPIRO_WAIT_CURSOR_INCLUDED
#define SPIRO_WAIT_CURSOR_INCLUDED

/*!
* @author		RKH
* @date		06.06.2017
* @copyright (C)Spiro Medical AS 2013 - 2017
*/

class CMyRegMan : public CXTRegistryManager
{
public:
	CMyRegMan();
	~CMyRegMan();
};

class CSpiroWaitCursor 
{
public:
	CSpiroWaitCursor();
	~CSpiroWaitCursor();
protected:
	CWaitCursor *cwP;
};
//
//class CSpiroRightCursor 
//{
//public:
//	CSpiroRightCursor();
//	~CSpiroRightCursor();
//protected:
//	HCURSOR hCursor;
//};
//
//class CSpiroLeftCursor 
//{
//public:
//	CSpiroLeftCursor();
//	~CSpiroLeftCursor();
//protected:
//	HCURSOR hCursor;
//};

#endif