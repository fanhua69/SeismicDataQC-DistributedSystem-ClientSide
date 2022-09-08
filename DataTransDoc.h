#pragma once
#include "DataBaseManager.h"
#include "DataTransfer.h"

// CDataTransDoc document
class CDataHis
{
public:
	char	sUser[50];
	CTime	time;
	char	sAction[256];
};


class CDataTransDoc : public CDocument
{
	DECLARE_DYNCREATE(CDataTransDoc)

public:
	CDataTransDoc();
	virtual ~CDataTransDoc();
	virtual void Serialize(CArchive& ar);   // overridden for document i/o

	CDataBaseManager m_db;
	bool		m_bShowFathers;
	CString		m_sUserShow;

#ifdef _DEBUG
	virtual void AssertValid() const;
	virtual void Dump(CDumpContext& dc) const;
#endif

protected:
	virtual BOOL OnNewDocument();

	DECLARE_MESSAGE_MAP()
public:
	
};
