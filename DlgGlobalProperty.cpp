// DlgGlobalProperty.cpp : implementation file
//

#include "stdafx.h"
#include "QCDB.h"
#include "DlgGlobalProperty.h"
#include "QCGlobal.h"
#include ".\dlgglobalproperty.h"

// CDlgGlobalProperty dialog

IMPLEMENT_DYNAMIC(CDlgGlobalProperty, CPropertyPage)
CDlgGlobalProperty::CDlgGlobalProperty()
	: CPropertyPage(CDlgGlobalProperty::IDD)
	, m_sDialEntry(_T(""))
{
	GetPar("GLOBAL_DIAL_ENTRY",m_sDialEntry);

	GetDialEntrys(m_arrayEntryNames);

}

CDlgGlobalProperty::~CDlgGlobalProperty()
{
}

void CDlgGlobalProperty::DoDataExchange(CDataExchange* pDX)
{
	CPropertyPage::DoDataExchange(pDX);
	DDX_Control(pDX, IDC_comboDialEntry, m_comboDialEntry);
	DDX_CBString(pDX, IDC_comboDialEntry, m_sDialEntry);
}


BEGIN_MESSAGE_MAP(CDlgGlobalProperty, CPropertyPage)
	ON_BN_CLICKED(IDC_buttonDial, OnBnClickedbuttondial)
END_MESSAGE_MAP()



void CDlgGlobalProperty::OnOK()
{
	CPropertyPage::OnOK();
}

BOOL CDlgGlobalProperty::OnInitDialog()
{
	CPropertyPage::OnInitDialog();
	for(int i=0;i<m_arrayEntryNames.GetCount ();i++)
	{
        m_comboDialEntry.AddString (m_arrayEntryNames.GetAt (i));
	}

	return TRUE;  // return TRUE unless you set the focus to a control
	// EXCEPTION: OCX Property Pages should return FALSE
}

void CDlgGlobalProperty::OnBnClickedbuttondial()
{
	UpdateData(true);
	Dial(m_sDialEntry);
}

BOOL CDlgGlobalProperty::OnApply()
{
	// TODO: Add your specialized code here and/or call the base class
    return CPropertyPage::OnApply();
}
