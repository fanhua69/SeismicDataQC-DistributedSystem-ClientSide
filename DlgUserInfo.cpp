// DlgUserInfo.cpp : implementation file
//

#include "stdafx.h"
#include "QCDB.h"
#include "DlgUserInfo.h"


// CDlgUserInfo dialog

IMPLEMENT_DYNAMIC(CDlgUserInfo, CPropertyPage)
CDlgUserInfo::CDlgUserInfo()
	: CPropertyPage(CDlgUserInfo::IDD)
{
	m_sUserNameShort=GetUserNameShort();
}

CDlgUserInfo::~CDlgUserInfo()
{
}

void CDlgUserInfo::DoDataExchange(CDataExchange* pDX)
{
	CPropertyPage::DoDataExchange(pDX);
	DDX_Text(pDX, IDC_editUserNameShort, m_sUserNameShort);
}


BEGIN_MESSAGE_MAP(CDlgUserInfo, CPropertyPage)
END_MESSAGE_MAP()


// CDlgUserInfo message handlers

void CDlgUserInfo::OnOK()
{
    AddUserNameShort(m_sUserNameShort);
	CPropertyPage::OnOK();
}

BOOL CDlgUserInfo::OnApply()
{
	AddUserNameShort(m_sUserNameShort);
	return CPropertyPage::OnApply();
}
