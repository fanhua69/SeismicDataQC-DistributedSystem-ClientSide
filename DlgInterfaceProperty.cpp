// DlgInterfaceProperty.cpp : implementation file
//

#include "stdafx.h"
#include "QCDB.h"
#include "DlgInterfaceProperty.h"
#include "FHApp.h"
#include ".\dlginterfaceproperty.h"
#include "defines.h"

// CDlgInterfaceProperty dialog

IMPLEMENT_DYNAMIC(CDlgInterfaceProperty, CPropertyPage)
CDlgInterfaceProperty::CDlgInterfaceProperty()
	: CPropertyPage(CDlgInterfaceProperty::IDD)
	, m_bShowInfoExclusively(FALSE)
	, m_bLinkServerAtBegin(FALSE)
{
	GetPar("QCDB_SHOW_ONLY_ONE_INFOWINDOW",m_bShowInfoExclusively);
	GetPar("QCDB_LINK_SERVER_AT_BEGIN",m_bLinkServerAtBegin);
}

CDlgInterfaceProperty::~CDlgInterfaceProperty()
{
}

void CDlgInterfaceProperty::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	DDX_Check(pDX, IDC_checkShowInfoExclusively, m_bShowInfoExclusively);
	DDX_Check(pDX, IDC_checkLinkServerAtBegin, m_bLinkServerAtBegin);
}


BEGIN_MESSAGE_MAP(CDlgInterfaceProperty, CPropertyPage)
END_MESSAGE_MAP()


void CDlgInterfaceProperty::OnOK()
{
	AddPar("QCDB_SHOW_ONLY_ONE_INFOWINDOW",m_bShowInfoExclusively);
	AddPar("QCDB_LINK_SERVER_AT_BEGIN",m_bLinkServerAtBegin);
	CPropertyPage::OnOK();
}

BOOL CDlgInterfaceProperty::OnInitDialog()
{
    CPropertyPage::OnInitDialog();

    return TRUE;  // return TRUE unless you set the focus to a control
    // EXCEPTION: OCX Property Pages should return FALSE
}
