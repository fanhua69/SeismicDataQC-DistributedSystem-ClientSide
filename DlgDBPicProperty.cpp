// DlgDBPicProperty.cpp : implementation file
//

#include "stdafx.h"
#include "QCDB.h"
#include "DlgDBPicProperty.h"


// CDlgDBPicProperty dialog

IMPLEMENT_DYNAMIC(CDlgDBPicProperty, CPropertyPage)
CDlgDBPicProperty::CDlgDBPicProperty()
	: CPropertyPage(CDlgDBPicProperty::IDD)
	, m_iDBPicWidth(100)
	, m_iDBPicHeight(80)
	, m_bDBPicKeepRatio(TRUE)
{
}

CDlgDBPicProperty::~CDlgDBPicProperty()
{
}

void CDlgDBPicProperty::DoDataExchange(CDataExchange* pDX)
{
	CPropertyPage::DoDataExchange(pDX);
	DDX_Text(pDX, IDC_editDBPicWidth, m_iDBPicWidth);
	DDX_Text(pDX, IDC_editDBPicHeight, m_iDBPicHeight);
	DDX_Check(pDX, IDC_checkDBPicKeepRatio, m_bDBPicKeepRatio);
}


BEGIN_MESSAGE_MAP(CDlgDBPicProperty, CPropertyPage)
END_MESSAGE_MAP()


// CDlgDBPicProperty message handlers
