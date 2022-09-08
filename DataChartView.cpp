// DBChart.cpp : implementation file
//

#include "stdafx.h"
#include "QCDB.h"
#include "DataChartView.h"
#include "DlgDBShowProperty.h"
#include "DlgDataSelector.h"
#include "FHMainFrm.h"
#include "qcglobal.h"
#include ".\datachartview.h"
#include "DlgChoosePage.h"
#include "defines.h"

// CDataChartView

IMPLEMENT_DYNCREATE(CDataChartView, CGraphView)

CDataChartView::CDataChartView()
{
	m_iTotalRecord=100;
	m_iRecordPerPage=10;
	m_iRecordStart=0;

	m_menuManager.SetPopupMenu (IDR_DBBrowser_ChooseData);
	m_menuManager.AppendMenu (ID_SetChartProperty,"设置图表属性.",MF_MENUITEM);
}

CDataChartView::~CDataChartView()
{
	AddPar("DataChartView_Chart_Type",m_iChartType);

	CDataBrowseDoc *pDoc=(CDataBrowseDoc *)GetDocument();

	int i=0,iFieldQty=pDoc->GetFieldQty();
	CFieldData *pFields=pDoc->GetFields();
	CString sField;
	if(pFields)
	{
		CString sParName="DataBrowseDoc_"+CString(pDoc->GetTableName ());
		RemovePar(sParName);
		for(int i=0;i<iFieldQty;i++)
		{
			sField=pFields[i].sFieldName ;
			AddMorePar(sParName,sField);
		}
	}
}


BEGIN_MESSAGE_MAP(CDataChartView, CGraphView)
	ON_COMMAND(ID_DBShowNextPage, OnDbshownextpage)
	ON_COMMAND(ID_DBShowPrevPage, OnDbshowprevpage)
	ON_UPDATE_COMMAND_UI(ID_DBShowNextPage, OnUpdateDbshownextpage)
	ON_UPDATE_COMMAND_UI(ID_DBShowPrevPage, OnUpdateDbshowprevpage)
	ON_COMMAND(ID_UpdateDataFromDB, OnUpdatedatafromdb)
	ON_COMMAND(ID_ChooseShowData, OnChooseshowdata)
	ON_COMMAND(ID_ChoosePage, OnChoosepage)
	ON_COMMAND(ID_SetChartProperty, OnSetchartproperty)
END_MESSAGE_MAP()


// CDataChartView drawing

void CDataChartView::OnInitialUpdate()
{
	CGraphView::OnInitialUpdate();

}

void CDataChartView::OnDraw(CDC* pDC)
{
#ifdef DOG
    CFHApp *pApp=(CFHApp*)AfxGetApp();
    if(!pApp->m_dog .m_bCalcChecked )
    {
        return;
    }
    else
    {
        pApp->m_dog.WriteRand (0);
    }

#endif

	CGraphView::OnDraw(pDC);
}


// CDataChartView diagnostics

#ifdef _DEBUG
void CDataChartView::AssertValid() const
{
	CGraphView::AssertValid();
}

void CDataChartView::Dump(CDumpContext& dc) const
{
	CGraphView::Dump(dc);
}
#endif //_DEBUG


// CDataChartView message handlers

int CDataChartView::OnCreate(LPCREATESTRUCT lpCreateStruct)
{
	if (CGraphView::OnCreate(lpCreateStruct) == -1)
		return -1;
	return 0;
}

void CDataChartView::SetChart()
{
	// TODO:  Add your specialized creation code here
	int i,j,k,n;
	CString sField;

	CDataBrowseDoc *pDoc=(CDataBrowseDoc *)GetDocument();
	SetGraphTitle(pDoc->GetTableName() +"数据监控图");

	SetYAxisLabel("Y坐标");

	// Set Data:
	CString sTable=pDoc->GetTableName();
	if(sTable=="")
	{
		return;
	}
	CStringArray arrayIndexField;
	pDoc->GetIndexFields(arrayIndexField); //::GetColNameField(sTable);
	int iIndexQty=arrayIndexField.GetCount();
	int *piOrderOfIndex=new int [iIndexQty+2];
	for(i=0;i<iIndexQty;i++)
	{
		piOrderOfIndex[i]=-1;
	}

	// Set X Axis label:
	CString sLabel="";
    if(m_iGraphType==SCATTER_GRAPH)
    {
        sLabel="X坐标";
    }
    else
    {
        for(i=0;i<iIndexQty;i++)
        {
            sLabel+=arrayIndexField.GetAt (i);
            if(i!=iIndexQty-1)
            {
                sLabel+=".";
            }
        }
    }

	SetXAxisLabel(sLabel );

	//
	CStringArray *pArrayRecord=NULL;
	int iFieldQty=pDoc->GetFieldQty ();
	if(iFieldQty<1)return;

	// find the name field:
	int iIDField=-1;
	int iFieldQtyExceptName=iFieldQty;
	CString sIDFieldName=GetIDFieldName();
	CFieldData *pFields=pDoc->GetFields ();


	// Calc valid field qty:
	for(i=0;i<iFieldQty;i++)
	{
		// 年月日：
		for(j=0;j<iIndexQty;j++)
		{
			sField=arrayIndexField.GetAt (j);
			if(pFields[i].sFieldName ==sField)
			{
				piOrderOfIndex[j]=i;
				iFieldQtyExceptName--;
			}
		}

		// 编号:
		if(pFields[i].sFieldName==sIDFieldName)
		{
			iIDField=i;
		}
	}

	if(iIDField!=-1)
	{
		iFieldQtyExceptName--;
	}

	// Set Y Index:
	int iValidField=0;
	bool bIndex=false;

	sLabel="";
	for(j=0;j<iFieldQty;j++)
	{
		// Check if this field is of NO.:
		if(j==iIDField)continue;  //编号

		// Check if this field is of index:
		bIndex=false;
		for(k=0;k<iIndexQty;k++)
		{
			if(j==piOrderOfIndex[k])         // 列名：例如：日期
			{
				bIndex=true;
				continue;
			}
		}
		if(bIndex)continue;
		if(iValidField==0)
		{
			sLabel=pFields[j].sFieldName;
			n=sLabel.Find ("=");
			if(n!=-1)
			{
				sLabel=sLabel.Left (n);
			}
		}
		else
		{
			sLabel+=",...";
			break;
		}

		iValidField++;
	}

	SetYAxisLabel(sLabel );

	/////////////////////////////////
	// Add series:

	m_iTotalRecord=pDoc->GetRecordQty ();

	CStringArray arrayFields;
	pDoc->GetFields (arrayFields);
	
	RemoveAllSeries ();


	iValidField=0;
	float fData;

	CStringArray *pRecord=NULL;
	CString sData;
	bIndex=false;

	for(int iRecord=m_iRecordStart;
		iRecord<m_iRecordStart+m_iRecordPerPage;
		iRecord++)		
	{
		if(iRecord>=m_iTotalRecord)break;
		pRecord=pDoc->GetRecord (iRecord);
		if(!pRecord)break;

		CGraphSeries *pSeries=new CGraphSeries;

		// Set the Label:
		sLabel="";
		for(j=0;j<iIndexQty;j++)
		{
			if(piOrderOfIndex[j]!=-1)
			{
				sLabel+=pRecord->GetAt(piOrderOfIndex[j]);
				if(j!=iIndexQty-1)
				{
					sLabel+=".";
				}
			}
		}
		if(sLabel=="")
		{
			sLabel=vtos(iRecord);
		}

		pSeries->SetLabel (sLabel);			


		//
		iValidField=0;
		for(j=0;j<iFieldQty;j++)
		{
			// Check if this field is of NO.:
			if(j==iIDField)continue;  //编号

			// Check if this field is of index:
			bIndex=false;
			for(k=0;k<iIndexQty;k++)
			{
				if(j==piOrderOfIndex[k])         // 列名：例如：日期
				{
					//SetRowName(iRecord-m_iRecordStart,pRecord->GetAt (j));
					bIndex=true;
					continue;
				}
			}
			if(bIndex)continue;

			// Add the data to the series:
			sData=pRecord->GetAt (j);
			fData=0.0;
			sscanf((LPCTSTR)sData,"%f",&fData);
			pSeries->SetData (iValidField,fData);
			if(fData<-999999||fData>=999999)
			{
				fData=0.0;
			}

			iValidField++;
		}
		AddSeries (pSeries);
	}
	
	
	delete []piOrderOfIndex;

	return;
}

void CDataChartView::SetLegend()
{
	CDataBrowseDoc *pDoc=(CDataBrowseDoc *)GetDocument();


	int i,j;
	int iFieldQty=pDoc->GetFieldQty ();
	if(iFieldQty<1)return;

	// find the name field:
	int iIDField=-1;
	CString sIDFieldName=GetIDFieldName();
	CStringArray arrayIndex;
	pDoc->GetIndexFields (arrayIndex);
	int iIndex=arrayIndex.GetCount ();
	int *pIndexOrder=new int[iIndex+2];
	for(i=0;i<iIndex;i++)
	{
		pIndexOrder[i]=-1;
	}
	
	CFieldData *pFields=pDoc->GetFields ();

	for(i=0;i<iFieldQty;i++)
	{
		for(j=0;j<iIndex;j++)
		{
			if(pFields[i].sFieldName ==arrayIndex.GetAt (j))
			{
				pIndexOrder[j]=i;
			}
		}

		if(pFields[i].sFieldName ==sIDFieldName)
		{
			iIDField=i;
		}
	}

	
	CStringArray arrayFields;
	pDoc->GetFields (arrayFields);

	int iValidField=0;
	bool bIndex=false;
	for(i=0;i<iFieldQty;i++)
	{
		if(i==iIDField)continue;  //编号

		bIndex=false;
		for(j=0;j<iIndex;j++)
		{
			if(i ==pIndexOrder[j])
			{
				bIndex=true;
				break;
			}
		}
		if(bIndex)continue;

		CGraphView::SetLegend (iValidField,arrayFields.GetAt (i));
		iValidField++;
	}

	delete []pIndexOrder;

}

void CDataChartView::OnUpdate(CView* pSender, LPARAM lHint, CObject* pHint)
{
	CGraphView::OnUpdate(pSender,lHint,pHint);
	GetPars();

}


void CDataChartView::OnDbshownextpage()
{
	CDataBrowseDoc *pDoc=GetDocument();
	long iTotalRecord=pDoc->GetRecordQty ();
	if(m_iRecordStart+m_iRecordPerPage<iTotalRecord)
	{
		m_iRecordStart+=m_iRecordPerPage;
		OnUpdate(NULL,0, NULL);
		Invalidate();
	}
}

void CDataChartView::OnDbshowprevpage()
{
	if(m_iRecordStart-m_iRecordPerPage>=0)
	{
		m_iRecordStart-=m_iRecordPerPage;
		OnUpdate(NULL,0, NULL);
		Invalidate();
	}
}
void CDataChartView::OnUpdateDbshownextpage(CCmdUI *pCmdUI)
{
	CDataBrowseDoc *pDoc=GetDocument();
	long iTotalRecord=pDoc->GetRecordQty ();
	
	pCmdUI->Enable (m_iRecordStart+m_iRecordPerPage<iTotalRecord);
}

void CDataChartView::OnUpdateDbshowprevpage(CCmdUI *pCmdUI)
{
	pCmdUI->Enable (m_iRecordStart-m_iRecordPerPage>=0);
}

CDataBrowseDoc * CDataChartView::GetDocument()
{
	return (CDataBrowseDoc *)m_pDocument;
}

void CDataChartView::OnUpdatedatafromdb()
{
	CDataBrowseDoc *pDoc=GetDocument();
	pDoc->UpdateFromDB ();
    Invalidate();
}

void CDataChartView::OnChooseshowdata()
{
	CDataBrowseDoc *pDoc=GetDocument();
	pDoc->OnModifyData ();
	Invalidate();
}

void CDataChartView::OnChoosepage()
{
	// TODO: Add your command handler code here
	CDlgChoosePage dlg;
	dlg.m_iTotalRecord		=   m_iTotalRecord;
	dlg.m_iTotalPage		=	GetTotalPage();
	dlg.m_iCurrentPage		=	GetCurrentPage();
	dlg.m_iRecordTotalQty	=   m_iTotalRecord;
	dlg.m_iRecordQtyPerPage =	m_iRecordPerPage;

	if(dlg.DoModal ()==IDOK)
	{
		m_iRecordPerPage=dlg.m_iRecordQtyPerPage;

		m_iRecordStart=(dlg.m_iChosenPage -1)*m_iRecordPerPage;
		if(m_iRecordStart<0)
		{
			m_iRecordStart=0;
		}
		if(m_iRecordStart>=m_iTotalRecord)
		{
			m_iRecordStart=0;
		}
	}

	SetChart();
}

int CDataChartView::GetCurrentPage()
{
	return m_iRecordStart/m_iRecordPerPage+1;
}

int CDataChartView::GetTotalPage()
{
	return (m_iTotalRecord-1)/m_iRecordPerPage+1;
}

void CDataChartView::SavePars()
{
	CDataBrowseDoc *pDoc=(CDataBrowseDoc *)GetDocument();
	CString sTable=pDoc->GetTableName ();
	int n;

	// Save the pars for every table:
	for(int i=0;i<m_graphProperty.GetLegendLength ();i++)
	{
		CGraphGroupPropertySet *p = m_graphProperty.Get (i);
		AddPar(sTable+p->sLegendLabel+"MarkerSize" ,p->iMarkerSize );

        GetPar(sTable+p->sLegendLabel+"MarkerSize" ,n );
		GetPar(sTable+p->sLegendLabel+"MarkerType" ,n);
		switch(p->eMarkerType)
		{
		case MT_NONE:
            n=0;
			break;
		case MT_TRI:
            n=1;
			break;
		case MT_BOX:
            n=2;
			break;
		case MT_SPH:
            n=3;
			break;
		case MT_DIA:
            n=4;
			break;
		default:
            n=1;
			break;
		}

		AddPar(sTable+p->sLegendLabel+"MarkerType" ,n);
		AddPar(sTable+p->sLegendLabel+"LineWidth" ,p->iLineWidth );
	}

	AddPar(sTable+"GraphType" ,m_iGraphType);
	AddPar(sTable+"PieDrawLabels" ,m_bPieDrawLabels);
}


void CDataChartView::GetPars()
{
	CDataBrowseDoc *pDoc=(CDataBrowseDoc *)GetDocument();
	CString sTable=pDoc->GetTableName ();
    CString sParName;
    CGraphGroupPropertySet *p=NULL;
	int n;
	for(int i=0;i<m_graphProperty.GetLegendLength ();i++)
	{
		p = m_graphProperty.Get (i);
        sParName=sTable+p->sLegendLabel+"MarkerSize";
		GetPar(sParName,p->iMarkerSize );

		n=-1;
        sParName=sTable+p->sLegendLabel+"MarkerType";
		GetPar(sParName ,n);
		switch(n)
		{
		case 0:
            p->eMarkerType=MT_NONE;
			break;
		case 1:
            p->eMarkerType=MT_TRI;
			break;
		case 2:
            p->eMarkerType=MT_BOX;
			break;
		case 3:
            p->eMarkerType=MT_SPH;
			break;
		case 4:
            p->eMarkerType=MT_DIA;
			break;
		default:
            p->eMarkerType=MT_TRI;
			break;
		}

		GetPar(sTable+p->sLegendLabel+"LineWidth" ,p->iLineWidth );
	}

	GetPar(sTable+"GraphType" ,m_iGraphType);
	GetPar(sTable+"PieDrawLabels" ,m_bPieDrawLabels);
}
void CDataChartView::OnSetchartproperty()
{
	GetPars();
	CGraphView::OnSetchartproperty();
	SavePars();


}