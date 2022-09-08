// DlgChartProperty.cpp : implementation file
//

#include "stdafx.h"
#include "QCDB.h"
#include "GraphGroupProperty.h"
#include "DlgChartProperty.h"
#include "FHMainFrm.h"
#include "GraphView.h"
#include ".\dlgchartproperty.h"

// CDlgChartProperty dialog

IMPLEMENT_DYNAMIC(CDlgChartProperty, CDialog)
CDlgChartProperty::CDlgChartProperty(CWnd* pParent /*=NULL*/)
	: CDialog(CDlgChartProperty::IDD, pParent)
	, m_bPieMarkRatio(FALSE)
	, m_sSerName(_T(""))
	, m_sChartType(_T(""))
	, m_iMarkSize(0)
	, m_iLineWidth(0)
{
	m_iTypeSel=1;
}

CDlgChartProperty::~CDlgChartProperty()
{
	for(int i=0;i<m_arraySerProperty.GetCount ();i++)
	{
		CGraphGroupPropertySet	*p=(CGraphGroupPropertySet	*)m_arraySerProperty.GetAt (i);
		delete p;
		m_arraySerProperty.RemoveAt (i);
	}
}

void CDlgChartProperty::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	DDX_Control(pDX, IDC_comboChartType, m_comboChartType);
	DDX_Check(pDX, IDC_checkPieMarkRatio, m_bPieMarkRatio);
	DDX_Control(pDX, IDC_comboSerName, m_comboSerNames);
	DDX_Control(pDX, IDC_comboMarkType, m_comboMarkType);
	DDX_CBString(pDX, IDC_comboSerName, m_sSerName);
	DDX_CBString(pDX, IDC_comboChartType, m_sChartType);
	DDX_CBString(pDX, IDC_comboMarkType, m_sMarkType);
	DDX_Text(pDX, IDC_editMarkSize, m_iMarkSize);
	DDX_Text(pDX, IDC_editLineWidth, m_iLineWidth);
}


BEGIN_MESSAGE_MAP(CDlgChartProperty, CDialog)
	ON_BN_CLICKED(IDC_buttonColor, OnBnClickedbuttoncolor)
	ON_CBN_SELCHANGE(IDC_comboSerName, OnCbnSelchangecombosername)
	ON_EN_CHANGE(IDC_editMarkSize, OnEnChangeeditmarksize)
	ON_EN_CHANGE(IDC_editLineWidth, OnEnChangeeditlinewidth)
	ON_WM_PAINT()
	ON_CBN_SELCHANGE(IDC_comboMarkType, OnCbnSelchangecombomarktype)
END_MESSAGE_MAP()


// CDlgChartProperty message handlers

void CDlgChartProperty::OnBnClickedbuttoncolor()
{
	CColorDialog dlg;
	if(dlg.DoModal ()==IDCANCEL)
	{
		return;
	}

	COLORREF color= dlg.GetColor ();

	DrawColor(color);

	CGraphGroupPropertySet *pSet=SearchSer(m_sSerName);
	if(!pSet)return;

	pSet->groupColor = color;
}

BOOL CDlgChartProperty::OnInitDialog()
{
	CDialog::OnInitDialog();
	
	SetCombos();
    OnCbnSelchangecombosername();

	return TRUE;  
}

int CDlgChartProperty::GetChartStyle()
{
	switch(m_iTypeSel)
	{
	case 0:
		return BAR_GRAPH;
	case 1:
		return LINE_GRAPH;
	case 2:
		return XY_LINE_GRAPH;
	case 3:
		return PIE_GRAPH;		
	case 4:
		return SCATTER_GRAPH;
	case 5:
		return STACKED_BAR_GRAPH;
	case 6:
		return BAR_GRAPH_3D;
	case 7:
		return LINE_GRAPH_3D;
	case 8:
		return PIE_GRAPH_3D;
	case 9:
		return STACKED_BAR_GRAPH_3D;
	default:
		return BAR_GRAPH;
	}
}
void CDlgChartProperty::SetChartStyle(int iStyle)
{
	switch(iStyle)
	{
	case BAR_GRAPH:
		m_iTypeSel=0;
		break;
	case LINE_GRAPH:
		m_iTypeSel=1;
		break;
	case XY_LINE_GRAPH:
		m_iTypeSel=2;
		break;
	case PIE_GRAPH:
		m_iTypeSel=3;
		break;
	case SCATTER_GRAPH:
		m_iTypeSel=4;
		break;
	case STACKED_BAR_GRAPH:
		m_iTypeSel=5;
		break;
	case BAR_GRAPH_3D:
		m_iTypeSel=6;
		break;
	case LINE_GRAPH_3D:
		m_iTypeSel=7;
		break;
	case PIE_GRAPH_3D:
		m_iTypeSel=8;
		break;
	case STACKED_BAR_GRAPH_3D:
		m_iTypeSel=9;
		break;
	default:
		m_iTypeSel=10;
		break;
	}
}

void CDlgChartProperty::SetCombos()
{
	////////////////////////////////////////////////
	// 
	////////////////////////////////////////////////
	m_comboChartType.ResetContent ();
	m_comboChartType.AddString ("柱状图");
	m_comboChartType.AddString ("带点线条图");
	m_comboChartType.AddString ("无点线条图");
	m_comboChartType.AddString ("饼图");
	m_comboChartType.AddString ("XY散点图");
	m_comboChartType.AddString ("数据合并图");	
	m_comboChartType.AddString ("三维柱状图");	
	m_comboChartType.AddString ("三维线条图");	
	m_comboChartType.AddString ("三维饼图");
	m_comboChartType.AddString ("三维数据合并图");



	m_comboChartType.SetCurSel (m_iTypeSel);

	////////////////////////////////////////////////
	// 
	////////////////////////////////////////////////
	m_comboMarkType.AddString ("无");
	m_comboMarkType.AddString ("三角形");
	m_comboMarkType.AddString ("正方形");
	m_comboMarkType.AddString ("横线");
	m_comboMarkType.AddString ("菱形");

	////////////////////////////////////////////////
	// 
	////////////////////////////////////////////////
	m_comboSerNames.ResetContent ();
	for(int i=0;i<m_arraySerProperty.GetCount ();i++)
	{
		CGraphGroupPropertySet *pSet=(CGraphGroupPropertySet *)m_arraySerProperty.GetAt (i);
		m_comboSerNames.AddString (pSet->sLegendLabel );
	}
	m_comboSerNames.SetCurSel (0);


	OnCbnSelchangecombosername();

	Invalidate();
}

void CDlgChartProperty::OnOK()
{
	m_iTypeSel=m_comboChartType.GetCurSel();
	UpdateData(true);
    OnCbnSelchangecombosername();

	CDialog::OnOK();
}

void CDlgChartProperty::AddGroupProperty(CGraphGroupPropertySet *pProperty)
{
	CGraphGroupPropertySet *pSet=new CGraphGroupPropertySet;
	pSet->Copy (pProperty);
	m_arraySerProperty.Add (pSet);	
}

void CDlgChartProperty::OnCbnSelchangecombosername()
{	
    int i=m_comboSerNames.GetCurSel ();
    if(i==-1)return;

    CGraphGroupPropertySet *pSet=(CGraphGroupPropertySet *)m_arraySerProperty.GetAt (i);
    if(!pSet)return;

    UpdateData(true);

    DrawColor(pSet->groupColor );
	m_sSerName=pSet->sLegendLabel ;
	m_iMarkSize=pSet->iMarkerSize ;
	m_iLineWidth=pSet->iLineWidth ;
	if(pSet->eMarkerType == MT_NONE)
	{
		m_sMarkType="无";
		m_comboChartType.SetCurSel (0);
	}
	else if(pSet->eMarkerType == MT_TRI)
	{
		m_sMarkType="三角形";
		m_comboChartType.SetCurSel (1);
	}
	else if(pSet->eMarkerType == MT_BOX)
	{
		m_sMarkType="正方形";
		m_comboChartType.SetCurSel (2);
	}
	else if(pSet->eMarkerType == MT_SPH)
	{
		m_sMarkType="圆形";
		m_comboChartType.SetCurSel (3);
	}
	else if(pSet->eMarkerType == MT_DIA)
	{
		m_sMarkType="菱形";
		m_comboChartType.SetCurSel (4);
	}


	UpdateData(false);
}

void CDlgChartProperty::DrawColor(COLORREF cColor)
{
	CBrush brush(cColor);

	CWnd *pWnd=GetDlgItem(IDC_SerColor);
	CDC *pDC= pWnd->GetWindowDC ();
	CBrush *pOldBrush = pDC->SelectObject (&brush);
	CRect rect;
	pWnd->GetWindowRect (&rect);
	pDC->Rectangle(0,0,rect.Width (),rect.Height ());
	pDC->SelectObject (pOldBrush);
			
}

void CDlgChartProperty::OnEnChangeeditmarksize()
{
	UpdateData(true);
	CGraphGroupPropertySet *pSet=SearchSer(m_sSerName);
	if(!pSet)return;

	pSet->iMarkerSize = m_iMarkSize;

}

CGraphGroupPropertySet * CDlgChartProperty::SearchSer(CString sSerName)
{
	bool bFound=false;
	CGraphGroupPropertySet *pSet=NULL;
	for(int i=0;i<m_arraySerProperty.GetCount ();i++)
	{
		pSet=(CGraphGroupPropertySet *)m_arraySerProperty.GetAt (i);
		if(strcmp((LPCTSTR)sSerName	, pSet->sLegendLabel )==0)
		{
			bFound=true;
			break;
		}
	}

	if(!bFound)
	{
		TRACEERROR("没有找到数据组名："+m_sSerName);
		return NULL;
	}
	else
	{
		return pSet;
	}
}


void CDlgChartProperty::OnEnChangeeditlinewidth()
{
	UpdateData(true);
	CGraphGroupPropertySet *pSet=SearchSer(m_sSerName);
	if(!pSet)return;

	pSet->iLineWidth = m_iLineWidth;
}

void CDlgChartProperty::OnPaint()
{
	CPaintDC dc(this); // device context for painting

	CGraphGroupPropertySet *pSet=SearchSer(m_sSerName);
	if(!pSet)return;

	DrawColor(pSet->groupColor );
		// Do not call CDialog::OnPaint() for painting messages
}

bool CDlgChartProperty::GetPieDrawLabel()
{
	return m_bPieMarkRatio;
}

void CDlgChartProperty::SetPieDrawLabel(bool bDrawLabel)
{
	m_bPieMarkRatio=bDrawLabel;
}



void CDlgChartProperty::OnCbnSelchangecombomarktype()
{
	UpdateData(true);
	CGraphGroupPropertySet *pSet=SearchSer(m_sSerName);
	if(!pSet)return;

	int iSel=m_comboMarkType.GetCurSel ();
	switch(iSel)
	{
	case 0:
		pSet->eMarkerType = MT_NONE;
		break;
	case 1:
		pSet->eMarkerType = MT_TRI;
		break;
	case 2:
		pSet->eMarkerType = MT_BOX;
		break;
	case 3:
		pSet->eMarkerType = MT_SPH;
		break;
	case 4:
		pSet->eMarkerType = MT_DIA;
		break;
	default:
		pSet->eMarkerType = MT_TRI;
		break;
	}
}
