// TransDataView.cpp : implementation file
//

#include "stdafx.h"
#include "QCDB.h"
#include "DataTransView.h"
#include "DataTransDoc.h"

// CDataTransView

IMPLEMENT_DYNCREATE(CDataTransView, CFHEditView)

CDataTransView::CDataTransView()
{
	m_nColNumber=4;
}

CDataTransView::~CDataTransView()
{
}

BEGIN_MESSAGE_MAP(CDataTransView, CFHEditView)
END_MESSAGE_MAP()


// CDataTransView drawing

void CDataTransView::OnDraw(CDC* pDC)
{
	CDocument* pDoc = GetDocument();
	// TODO: add draw code here
}


// CDataTransView diagnostics

#ifdef _DEBUG
void CDataTransView::AssertValid() const
{
	CFHEditView::AssertValid();
}

void CDataTransView::Dump(CDumpContext& dc) const
{
	CFHEditView::Dump(dc);
}
#endif //_DEBUG

void CDataTransView::UpdateGrid()
{
	int i,j;
	CDataTransDoc *pDoc=(CDataTransDoc *)GetDocument();
	CStringArray arrayField;
	arrayField.Add ("�û���");
	arrayField.Add ("����");
	arrayField.Add ("����");
	CString sTable=GetHistoryTableName();

	char  sSQL[2000];
	strcpy(sSQL,"select * from "+sTable);

	if(pDoc->m_bShowFathers )
	{
		strcat(sSQL," where ������ = 'Y' ");
	}
	else
	{
		strcat(sSQL," where ������ = 'N'");
	}

	if(pDoc->m_sUserShow !="")
	{
		strcat(sSQL," and where �û��� ='");
		strcat(sSQL,m_sUserShow);
		strcat(sSQL,"'");
	}

	CDataBrowseDoc doc;
	pDoc->m_db.SendTableToBrowseDoc (sTable,
									sSQL,
									CTableDate(),
									&arrayField,
									&doc,
									::GetMaxHandDownRecord (),
									false);

	int iQty=doc.GetRecordQty ();
	SetSize(iQty+1,4);
	SetItem(0,0,"���");
	SetItem(0,1,"�û���");
	SetItem(0,2,"����");
	SetItem(0,3,"����");


	CStringArray *pRecord=NULL;
	for(i=0;i<iQty;i++)
	{
		pRecord=doc.GetRecord (i);
		SetItem(i+1,0,i);
		for(j=0;j<3;j++)
		{
			SetItem(i+1,j+1,pRecord->GetAt (j));
		}
	}

	m_Grid.AutoSize();
	m_Grid.SetListMode (1);
	m_Grid.SetEditable (false);
	Invalidate();

}

// CDataTransView message handlers
