// DlgTranProperty.cpp : implementation file
//

#include "stdafx.h"
#include "QCDB.h"
#include "DlgTranProperty.h"
#include ".\dlgtranproperty.h"


// CDlgTranProperty dialog

IMPLEMENT_DYNAMIC(CDlgTranProperty, CPropertyPage)
CDlgTranProperty::CDlgTranProperty()
	: CPropertyPage(CDlgTranProperty::IDD)
{
    m_iMaxHandInRecord=1000;
    m_iMaxHandDownRecord=1000;

	GetPar("QCDB_GLOBAL_MAX_HANDDOWN_RECORD",m_iMaxHandDownRecord);
	GetPar("QCDB_GLOBAL_MAX_HANDIN_RECORD",m_iMaxHandInRecord);
}

CDlgTranProperty::~CDlgTranProperty()
{
}

void CDlgTranProperty::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	DDX_Text(pDX, IDC_editMaxHandInRecord, m_iMaxHandInRecord);
	DDX_Text(pDX, IDC_editMaxHandDownRecord, m_iMaxHandDownRecord);
	DDV_MinMaxInt(pDX, m_iMaxHandInRecord, 1, 10000);
}


BEGIN_MESSAGE_MAP(CDlgTranProperty, CPropertyPage)
END_MESSAGE_MAP()


// CDlgTranProperty message handlers

void CDlgTranProperty::OnOK()
{
	AddPar("QCDB_GLOBAL_MAX_HANDDOWN_RECORD",m_iMaxHandDownRecord);
	AddPar("QCDB_GLOBAL_MAX_HANDIN_RECORD",m_iMaxHandInRecord);

	CPropertyPage::OnOK();
}