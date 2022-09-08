// DataTransDoc.cpp : implementation file
//

#include "stdafx.h"
#include "QCDB.h"
#include "DataTransDoc.h"
#include "QCGlobal.h"
#include ".\datatransdoc.h"

// CDataTransDoc

IMPLEMENT_DYNCREATE(CDataTransDoc, CDocument)

CDataTransDoc::CDataTransDoc()
{
	m_db.Initiate (GetDBName());
	m_bShowFathers=false;
	m_sUserShow="";
}

BOOL CDataTransDoc::OnNewDocument()
{
	if (!CDocument::OnNewDocument())
		return FALSE;
	return TRUE;
}

CDataTransDoc::~CDataTransDoc()
{
}


BEGIN_MESSAGE_MAP(CDataTransDoc, CDocument)
END_MESSAGE_MAP()


// CDataTransDoc diagnostics

#ifdef _DEBUG
void CDataTransDoc::AssertValid() const
{
	CDocument::AssertValid();
}

void CDataTransDoc::Dump(CDumpContext& dc) const
{
	CDocument::Dump(dc);
}
#endif //_DEBUG


// CDataTransDoc serialization

void CDataTransDoc::Serialize(CArchive& ar)
{
	if (ar.IsStoring())
	{
		// TODO: add storing code here
	}
	else
	{
		// TODO: add loading code here
	}
}

