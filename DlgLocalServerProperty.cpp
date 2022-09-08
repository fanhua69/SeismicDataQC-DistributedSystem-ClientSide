// DlgLocalServerProperty.cpp : implementation file
//

#include "stdafx.h"
#include "QCDB.h"
#include "DlgLocalServerProperty.h"
#include ".\dlglocalserverproperty.h"


// CDlgLocalServerProperty dialog

IMPLEMENT_DYNAMIC(CDlgLocalServerProperty, CPropertyPage)
CDlgLocalServerProperty::CDlgLocalServerProperty()
	: CPropertyPage(CDlgLocalServerProperty::IDD)
	, m_iLocalPort(0)
    , m_sLocalIP(_T(""))
{
    GetPar("GLOBAL_LOCAL_IP",m_sLocalIP );
	GetPar("GLOBAL_LOCAL_PORT",m_iLocalPort );
    GetPar("QCDB_PASSWORD_FILE",m_sPasswordFile);
}

CDlgLocalServerProperty::~CDlgLocalServerProperty()
{
}

void CDlgLocalServerProperty::DoDataExchange(CDataExchange* pDX)
{
    CPropertyPage::DoDataExchange(pDX);
    DDX_Text(pDX, IDC_editLocalPort, m_iLocalPort);
    DDX_Text(pDX, IDC_editPasswordFile, m_sPasswordFile);
    DDX_Text(pDX, IDC_editLocalIP, m_sLocalIP);
}


BEGIN_MESSAGE_MAP(CDlgLocalServerProperty, CPropertyPage)
    ON_BN_CLICKED(IDC_buttonPasswordFile, OnBnClickedbuttonpasswordfile)
END_MESSAGE_MAP()


// CDlgLocalServerProperty message handlers

void CDlgLocalServerProperty::OnOK()
{
	// TODO: Add your specialized code here and/or call the base class
    AddPar("GLOBAL_LOCAL_IP",m_sLocalIP );
	AddPar("GLOBAL_LOCAL_PORT",m_iLocalPort );
    AddPar("QCDB_PASSWORD_FILE",m_sPasswordFile);
   

	CPropertyPage::OnOK();
}

void CDlgLocalServerProperty::OnBnClickedbuttonpasswordfile()
{
    CString sFile=GetFileForOpen("已经存在的口令文件(*.dat)","dat");
    if(sFile!="")
    {
        m_sPasswordFile=sFile;
        UpdateData(false);
    }
}