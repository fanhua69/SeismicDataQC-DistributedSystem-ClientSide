// DlgEditShowProperty.cpp : implementation file
//

#include "stdafx.h"
#include "QCDB.h"
#include "DlgEditShowProperty.h"


// CDlgEditShowProperty dialog

IMPLEMENT_DYNAMIC(CDlgEditShowProperty, CPropertyPage)
CDlgEditShowProperty::CDlgEditShowProperty(CWnd* pParent /*=NULL*/)
	: CPropertyPage(CDlgEditShowProperty::IDD)
	, m_iPicWidth(0)
	, m_iPicHeight(0)
	, m_bKeepRatio(FALSE)
	, m_iBigPicWidth(0)
	, m_iBigPicHeight(0)
	, m_bBigPicKeepRatio(FALSE)
	, m_iBigPicOffsetX(0)
	, m_iBigPicOffsetY(0)
	, m_bDrawCellPic(FALSE)
	, m_bDrawBigPic(FALSE)
{
}

CDlgEditShowProperty::~CDlgEditShowProperty()
{
}

void CDlgEditShowProperty::DoDataExchange(CDataExchange* pDX)
{
	CPropertyPage::DoDataExchange(pDX);
	DDX_Text(pDX, IDC_editPicWidth, m_iPicWidth);
	DDX_Text(pDX, IDC_editPicHeight, m_iPicHeight);
	DDX_Check(pDX, IDC_checkKeepRatio, m_bKeepRatio);
	DDX_Text(pDX, IDC_editBigPicWidth, m_iBigPicWidth);
	DDX_Text(pDX, IDC_editBigPicHeight, m_iBigPicHeight);
	DDX_Check(pDX, IDC_checkBigPicKeepRatio, m_bBigPicKeepRatio);
	DDX_Text(pDX, IDC_editBigPicOffsetX, m_iBigPicOffsetX);
	DDX_Text(pDX, IDC_editBigPicOffsetY, m_iBigPicOffsetY);
	DDX_Check(pDX, IDC_checkDrawCellPic, m_bDrawCellPic);
	DDX_Check(pDX, IDC_checkDrawBigPic, m_bDrawBigPic);
}


BEGIN_MESSAGE_MAP(CDlgEditShowProperty, CPropertyPage)
END_MESSAGE_MAP()


// CDlgEditShowProperty message handlers
