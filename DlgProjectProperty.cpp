// DlgProjectProperty.cpp : implementation file
//

#include "stdafx.h"
#include "QCDB.h"
#include "DlgProjectProperty.h"
#include "DlgCreateDataBase.h"


// CDlgProjectProperty dialog

IMPLEMENT_DYNAMIC(CDlgProjectProperty, CPropertyPage)
CDlgProjectProperty::CDlgProjectProperty()
	: CPropertyPage(CDlgProjectProperty::IDD)
	, m_sWorkArea(_T(""))
	, m_sWorkCrew(_T(""))
	, m_sWorkYear(_T(""))
	, m_sWorkMonth(_T(""))
	, m_sTempFilePath(_T(""))
{

	GetPar("QCDB_CURR_AREA",m_sWorkArea);
	GetPar("QCDB_CURR_CREW",m_sWorkCrew);
	GetPar("QCDB_CURR_YEAR",m_sWorkYear);
	GetPar("QCDB_CURR_MONTH",m_sWorkMonth);

	m_sTempFilePath=GetMyTempPath();
    m_sDataBasePath=GetDataBasePath();
}

CDlgProjectProperty::~CDlgProjectProperty()
{
}

void CDlgProjectProperty::DoDataExchange(CDataExchange* pDX)
{
	CPropertyPage::DoDataExchange(pDX);
	DDX_Text(pDX, IDC_editCurrArea, m_sWorkArea);
	DDX_Text(pDX, IDC_editCurrCrew, m_sWorkCrew);
	DDX_Text(pDX, IDC_editWorkYear, m_sWorkYear);
	DDX_Text(pDX, IDC_editWorkMonth, m_sWorkMonth);
	DDX_Text(pDX, IDC_editTempFilePath, m_sTempFilePath);
    DDX_Text(pDX, IDC_editDataBasePath, m_sDataBasePath);    
}


BEGIN_MESSAGE_MAP(CDlgProjectProperty, CPropertyPage)
	ON_BN_CLICKED(IDC_buttonTempFilePath, OnBnClickedbuttontempfilepath)
    ON_BN_CLICKED(IDC_buttonDataBaseFilePath, OnBnClickedbuttondatabasefilepath)
END_MESSAGE_MAP()


BOOL CDlgProjectProperty::OnInitDialog()
{
	CPropertyPage::OnInitDialog();

	return TRUE;  // return TRUE unless you set the focus to a control
	// EXCEPTION: OCX Property Pages should return FALSE
}

void CDlgProjectProperty::OnOK()
{
	UpdateData(true);

	AddPar("QCDB_CURR_AREA",m_sWorkArea);
	AddPar("QCDB_CURR_CREW",m_sWorkCrew);
	AddPar("QCDB_CURR_YEAR",m_sWorkYear);
	AddPar("QCDB_CURR_MONTH",m_sWorkMonth);

	AddMyTempPath(m_sTempFilePath);
    AddDataBasePath(m_sDataBasePath);


	CPropertyPage::OnOK();
}

void CDlgProjectProperty::OnBnClickedbuttontempfilepath()
{
	// TODO: Add your control notification handler code here
	CString s=GetChosenFolder("选择临时文件路径",m_hWnd);
	if(s!="")
	{
		m_sTempFilePath=s;
		UpdateData(false);
	}
}

void CDlgProjectProperty::OnBnClickedbuttondatabasefilepath()
{
    CString s=GetChosenFolder("选择数据库文件路径",m_hWnd);
    if(s!="")
    {
        m_sDataBasePath=s;
        UpdateData(false);
    }
}
