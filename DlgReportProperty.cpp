// DlgReportProperty.cpp : implementation file
//

#include "stdafx.h"
#include "QCDB.h"
#include "DlgReportProperty.h"
#include "DlgDataSelectorInd.h"
#include "qcglobal.h"
#include "fhmainfrm.h"
#include "MyDate.h"
#include "windows.h"
#include "Winuser.h"
#include ".\dlgreportproperty.h"

// CDlgReportProperty dialog

IMPLEMENT_DYNAMIC(CDlgReportProperty, CDialog)
CDlgReportProperty::CDlgReportProperty(CWnd* pParent /*=NULL*/)
	: CDialog(CDlgReportProperty::IDD, pParent)
	, m_sReportNo(_T(""))
	, m_iPrintPage(0)
{
    m_iStartYear=2005;
    m_iStartMonth=1;
    m_iStartDay=1;
    m_iEndYear=2005;
    m_iEndMonth=6;
    m_iEndDay=8;
    m_iMonthType=0;
    m_iPrintPage=1;

	GetPar("QCDB_CT_TABLE_NAME",m_sReportName);
	GetPar("QCDB_CT_PROJECT_NAME",m_sProject);
	GetPar("QCDB_CT_LINE",m_sLineName);
	GetPar("QCDB_CT_START_YEAR",m_iStartYear);
	GetPar("QCDB_CT_START_MONTH",m_iStartMonth);
	GetPar("QCDB_CT_START_Day",m_iStartDay);
	GetPar("QCDB_CT_END_YEAR",m_iEndYear);
	GetPar("QCDB_CT_END_MONTH",m_iEndMonth);
	GetPar("QCDB_CT_END_Day",m_iEndDay);
	GetPar("QCDB_CT_CREATOR",m_sReportCreator);
	GetPar("QCDB_CT_AUDITOR",m_sReportAuditor);
	GetPar("QCDB_CT_REPORTNUMBER",m_sReportNo);
	GetPar("QCDB_CT_MONTHTYPE",m_iMonthType);
	GetPar("QCDB_CT_PRINTPAGE",m_iPrintPage);

	CMyDate date;
	m_iStatYear=date.GetYear ();
	m_iStatMonth=date.GetMonth ();
	m_iStatDay=date.GetDay ();

}

CDlgReportProperty::~CDlgReportProperty()
{
}

void CDlgReportProperty::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	DDX_Control(pDX, IDC_comboReportNames, m_comboReportNames);
	DDX_CBString(pDX, IDC_comboReportNames, m_sReportName);
	DDX_Control(pDX, IDC_comboProjects, m_comboProjects);
	DDX_CBString(pDX, IDC_comboProjects, m_sProject);
	DDX_Text(pDX, IDC_editStartYear, m_iStartYear);
	DDX_Text(pDX, IDC_editStartMonth, m_iStartMonth);
	DDX_Text(pDX, IDC_editStartDay, m_iStartDay);
	DDX_Text(pDX, IDC_editEndYear, m_iEndYear);
	DDX_Text(pDX, IDC_editEndMonth, m_iEndMonth);
	DDX_Text(pDX, IDC_editEndDay, m_iEndDay);
	DDX_Text(pDX, IDC_editReportCreator, m_sReportCreator);
	DDX_Text(pDX, IDC_editStatYear, m_iStatYear);
	DDX_Text(pDX, IDC_editStatMonth, m_iStatMonth);
	DDX_Text(pDX, IDC_editStatDay, m_iStatDay);
	DDX_Text(pDX, IDC_editReportAuditor, m_sReportAuditor);
	DDX_Control(pDX, IDC_comboLines, m_comboLines);
	DDX_CBString(pDX, IDC_comboLines, m_sLineName);
	DDX_Text(pDX, IDC_editReportNo, m_sReportNo);
	DDX_Radio(pDX, IDC_radioNaturalMonth, m_iMonthType);
	DDX_Text(pDX, IDC_editPrintPage, m_iPrintPage);
}


BEGIN_MESSAGE_MAP(CDlgReportProperty, CDialog)
	ON_BN_CLICKED(IDC_buttonChooseProjects, OnBnClickedbuttonchooseprojects)
	ON_CBN_SELCHANGE(IDC_comboProjects, OnCbnSelchangecomboprojects)
END_MESSAGE_MAP()


// CDlgReportProperty message handlers

void CDlgReportProperty::OnOK()
{
	UpdateData(true);

	AddPar("QCDB_CT_TABLE_NAME",m_sReportName);
	AddPar("QCDB_CT_PROJECT_NAME",m_sProject);
	AddPar("QCDB_CT_LINE",m_sLineName);
	AddPar("QCDB_CT_START_YEAR",m_iStartYear);
	AddPar("QCDB_CT_START_MONTH",m_iStartMonth);
	AddPar("QCDB_CT_START_Day",m_iStartDay);
	AddPar("QCDB_CT_END_YEAR",m_iEndYear);
	AddPar("QCDB_CT_END_MONTH",m_iEndMonth);
	AddPar("QCDB_CT_END_Day",m_iEndDay);
	AddPar("QCDB_CT_CREATOR",m_sReportCreator);
	AddPar("QCDB_CT_AUDITOR",m_sReportAuditor);
	AddPar("QCDB_CT_REPORTNUMBER",m_sReportNo);
	AddPar("QCDB_CT_MONTHTYPE",m_iMonthType);
	AddPar("QCDB_CT_PRINTPAGE",m_iPrintPage);


	CDialog::OnOK();
}

void CDlgReportProperty::OnBnClickedbuttonchooseprojects()
{
	CDlgDataSelectorInd dlg;
	dlg.SetTableName ("表0101项目");
	if(dlg.DoModal ()==IDCANCEL)return;

	SetProjectNames(dlg.GetSQL ());
}

BOOL CDlgReportProperty::OnInitDialog()
{
	CDialog::OnInitDialog();

	CTime time;
	time=CTime::GetCurrentTime ();
	int iYear=time.GetYear ()-1;

	char sSQL[200];

	sprintf(sSQL,"Select * from 表0101项目 where 施工年度 >= %d",iYear);

	SetProjectNames(sSQL);
	SetReportNames();
	SetLineNames();

	return TRUE;  // return TRUE unless you set the focus to a control
	// EXCEPTION: OCX Property Pages should return FALSE
}

void CDlgReportProperty::SetReportNames()
{
	CStringArray ar;
	::GetReportNames (ar);

	for(int i=0;i<ar.GetCount ();i++)
	{
		m_comboReportNames.AddString (ar.GetAt (i));
	}
}

void CDlgReportProperty::SetProjectNames(CString sSQL)
{
	CDataBrowseDoc doc;
	CQCDBApp *pApp= (CQCDBApp *)AfxGetApp();
	CStringArray arrayFields;
	arrayFields.Add ("项目名称");

	CTableDate dateStart;

	if(!pApp->m_dt .GetData ("",0,"表0101项目",
										sSQL,
										dateStart,
										&arrayFields,
										&doc,
										::GetMaxHandDownRecord (),
										false))
	{
		TRACEERROR("无法获得项目名称!");
		return;
	}

	CStringArray *pRecord=NULL;
	for(int i=0;i<doc.GetRecordQty ();i++)
	{
		pRecord=doc.GetRecord (i);
		m_comboProjects.AddString (pRecord->GetAt (0));
	}
}


void CDlgReportProperty::OnCbnSelchangecomboprojects()
{
	UpdateData(true);
	
	SetLineNames();
}

void CDlgReportProperty::SetLineNames()
{
	int n=m_comboProjects.GetCurSel ();
	if(n!=-1)
	{
		m_comboProjects.GetLBText(n,m_sProject);
	}

	CStringArray arrayLines;
	if(!GetLineNames(m_sProject,arrayLines))
	{
		return;
	}

	m_comboLines.ResetContent ();
	
	for(int i=0;i<arrayLines.GetCount ();i++)
	{
		m_comboLines.AddString (arrayLines.GetAt (i));
	}
	
	m_comboLines.SetCurSel (0);
}
