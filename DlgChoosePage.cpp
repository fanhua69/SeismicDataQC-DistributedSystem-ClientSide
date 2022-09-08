// DlgChoosePage.cpp : implementation file
//

#include "stdafx.h"
#include "QCDB.h"
#include "DlgChoosePage.h"
#include ".\dlgchoosepage.h"


// CDlgChoosePage dialog

IMPLEMENT_DYNAMIC(CDlgChoosePage, CDialog)
CDlgChoosePage::CDlgChoosePage(CWnd* pParent /*=NULL*/)
	: CDialog(CDlgChoosePage::IDD, pParent)
	, m_iTotalPage(0)
	, m_iCurrentPage(0)
	, m_iChosenPage(0)
	, m_iRecordQtyPerPage(0)
	, m_iTotalRecord(0)
{
	m_iRecordTotalQty=1000;
}

CDlgChoosePage::~CDlgChoosePage()
{
}

void CDlgChoosePage::SetTotalRecordQty(int iQty)
{
	m_iRecordTotalQty=iQty;
}


void CDlgChoosePage::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	DDX_Text(pDX, IDC_editTotalPage, m_iTotalPage);
	DDX_Text(pDX, IDC_editCurrentPage, m_iCurrentPage);
	DDX_Text(pDX, IDC_editChoosePage, m_iChosenPage);
	DDX_Control(pDX, IDC_Slider_ChoosePage, m_sliderChoosePage);
	DDX_Text(pDX, IDC_editRecordQtyPerPage, m_iRecordQtyPerPage);
	DDV_MinMaxInt(pDX, m_iRecordQtyPerPage, 1, 2000);
	DDX_Text(pDX, IDC_editTotalRecord, m_iTotalRecord);
}


BEGIN_MESSAGE_MAP(CDlgChoosePage, CDialog)
	ON_NOTIFY(NM_RELEASEDCAPTURE, IDC_Slider_ChoosePage, OnNMReleasedcaptureSliderChoosepage)
	ON_EN_CHANGE(IDC_editChoosePage, OnEnChangeeditchoosepage)
	ON_EN_CHANGE(IDC_editRecordQtyPerPage, OnEnChangeeditrecordqtyperpage)
END_MESSAGE_MAP()


// CDlgChoosePage message handlers


void CDlgChoosePage::OnNMReleasedcaptureSliderChoosepage(NMHDR *pNMHDR, LRESULT *pResult)
{
	// TODO: Add your control notification handler code here
	*pResult = 0;
	m_iChosenPage=m_sliderChoosePage.GetPos ();
	UpdateData(false);
}

BOOL CDlgChoosePage::OnInitDialog()
{
	CDialog::OnInitDialog();

	m_iChosenPage=m_iCurrentPage;
	m_sliderChoosePage.SetRange (1,m_iTotalPage);
	m_sliderChoosePage.SetPos (m_iChosenPage);
	UpdateData(false);

	return TRUE;  // return TRUE unless you set the focus to a control
}

void CDlgChoosePage::OnEnChangeeditchoosepage()
{
	UpdateData(true);
	if(m_iChosenPage>0&&m_iChosenPage<=m_iTotalPage)
	{
		m_sliderChoosePage.SetPos (m_iChosenPage);
	}
}

void CDlgChoosePage::OnEnChangeeditrecordqtyperpage()
{
	int iCurrRecordPos=m_iCurrentPage*m_iRecordQtyPerPage;
	int iChosenRecordPos=m_iChosenPage*m_iRecordQtyPerPage;

	UpdateData(true);

	m_iTotalPage=m_iRecordTotalQty/m_iRecordQtyPerPage;
	if(m_iTotalPage*m_iRecordQtyPerPage!=m_iRecordTotalQty)
	{
		m_iTotalPage++;
	}

	m_iCurrentPage=iCurrRecordPos/m_iRecordQtyPerPage;
	m_iChosenPage =iChosenRecordPos/m_iRecordQtyPerPage;

	m_sliderChoosePage.SetRange (1,m_iTotalPage);
	m_sliderChoosePage.SetPos (m_iChosenPage);

	UpdateData(false);
}
