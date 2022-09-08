// ReportShowProperty.cpp : implementation file
//

#include "stdafx.h"
#include "QCDB.h"
#include "DlgReportShowProperty.h"


// CDlgReportShowProperty dialog

IMPLEMENT_DYNAMIC(CDlgReportShowProperty, CDialog)
CDlgReportShowProperty::CDlgReportShowProperty(CWnd* pParent /*=NULL*/)
	: CDialog(CDlgReportShowProperty::IDD, pParent)
	, m_iMarkPicWidth(0)
	, m_iNoteCharLineLimit(0)
	, m_sPicName(_T(""))
{
	m_iLineWidth=1;
	m_iEdgeLineWidth=2;

	m_sTitleFont="宋体";
	m_iTitleFontHeight=24;
	m_iTitleFontWeight=700;
	m_iTitleFontUnderLine=0;

	m_sViceTitleFont="宋体";
	m_iViceTitleFontHeight=16;
	m_iViceTitleFontWeight=400;
	m_iViceTitleFontUnderLine=0;

	m_sFootFont="宋体";
	m_iFootFontHeight=16;
	m_iFootFontWeight=400;
	m_iFootFontUnderLine=0;

	m_sContentFont="宋体";
	m_iContentFontHeight=12;
	m_iContentFontWeight=400;
	m_iContentFontUnderLine=0;

	m_iNoteCharLineLimit=20;
	m_iMarkPicWidth=64;

	m_sPicName="cnpc.jpg";

	GetPar("QCDB_REPORT_SHOW_TITLEFONT_Name",m_sTitleFont);
	GetPar("QCDB_REPORT_SHOW_TITLEFONT_Height",m_iTitleFontHeight);
	GetPar("QCDB_REPORT_SHOW_TITLEFONT_Weight",m_iTitleFontWeight);
	GetPar("QCDB_REPORT_SHOW_TITLEFONT_UnderLine",m_iTitleFontUnderLine);

	GetPar("QCDB_REPORT_SHOW_VICETITLEFONT_Name",m_sViceTitleFont);
	GetPar("QCDB_REPORT_SHOW_VICETITLEFONT_Height",m_iViceTitleFontHeight);
	GetPar("QCDB_REPORT_SHOW_VICETITLEFONT_Weight",m_iViceTitleFontWeight);
	GetPar("QCDB_REPORT_SHOW_VICETITLEFONT_UnderLine",m_iViceTitleFontUnderLine);

	GetPar("QCDB_REPORT_SHOW_FOOTFONT_Name",m_sFootFont);
	GetPar("QCDB_REPORT_SHOW_FOOTFONT_Height",m_iFootFontHeight);
	GetPar("QCDB_REPORT_SHOW_FOOTFONT_Weight",m_iFootFontWeight);
	GetPar("QCDB_REPORT_SHOW_FOOTFONT_UnderLine",m_iFootFontUnderLine);

	GetPar("QCDB_REPORT_SHOW_ContentFONT_Name",m_sContentFont);
	GetPar("QCDB_REPORT_SHOW_ContentFONT_Height",m_iContentFontHeight);
	GetPar("QCDB_REPORT_SHOW_ContentFONT_Weight",m_iContentFontWeight);
	GetPar("QCDB_REPORT_SHOW_ContentFONT_UnderLine",m_iContentFontUnderLine);
	
	GetPar("QCDB_REPORT_SHOW_LineWidth",m_iLineWidth);
	GetPar("QCDB_REPORT_SHOW_EdgeLineWidth",m_iEdgeLineWidth);

	GetPar("QCDB_REPORT_SHOW_MarkPicName",m_sPicName);	
	GetPar("QCDB_REPORT_SHOW_PicWidth",m_iMarkPicWidth);
	GetPar("QCDB_REPORT_SHOW_LineCharLimit",m_iNoteCharLineLimit);
}

CDlgReportShowProperty::~CDlgReportShowProperty()
{
}

void CDlgReportShowProperty::OnOK()
{
	UpdateData(true);

	AddPar("QCDB_REPORT_SHOW_TITLEFONT_Name",m_sTitleFont);
	AddPar("QCDB_REPORT_SHOW_TITLEFONT_Height",m_iTitleFontHeight);
	AddPar("QCDB_REPORT_SHOW_TITLEFONT_Weight",m_iTitleFontWeight);
	AddPar("QCDB_REPORT_SHOW_TITLEFONT_UnderLine",m_iTitleFontUnderLine);

	AddPar("QCDB_REPORT_SHOW_VICETITLEFONT_Name",m_sViceTitleFont);
	AddPar("QCDB_REPORT_SHOW_VICETITLEFONT_Height",m_iViceTitleFontHeight);
	AddPar("QCDB_REPORT_SHOW_VICETITLEFONT_Weight",m_iViceTitleFontWeight);
	AddPar("QCDB_REPORT_SHOW_VICETITLEFONT_UnderLine",m_iViceTitleFontUnderLine);

	AddPar("QCDB_REPORT_SHOW_FOOTFONT_Name",m_sFootFont);
	AddPar("QCDB_REPORT_SHOW_FOOTFONT_Height",m_iFootFontHeight);
	AddPar("QCDB_REPORT_SHOW_FOOTFONT_Weight",m_iFootFontWeight);
	AddPar("QCDB_REPORT_SHOW_FOOTFONT_UnderLine",m_iFootFontUnderLine);

	AddPar("QCDB_REPORT_SHOW_ContentFONT_Name",m_sContentFont);
	AddPar("QCDB_REPORT_SHOW_ContentFONT_Height",m_iContentFontHeight);
	AddPar("QCDB_REPORT_SHOW_ContentFONT_Weight",m_iContentFontWeight);
	AddPar("QCDB_REPORT_SHOW_ContentFONT_UnderLine",m_iContentFontUnderLine);

	AddPar("QCDB_REPORT_SHOW_LineWidth",m_iLineWidth);
	AddPar("QCDB_REPORT_SHOW_EdgeLineWidth",m_iEdgeLineWidth);

	AddPar("QCDB_REPORT_SHOW_MarkPicName",m_sPicName);	
	AddPar("QCDB_REPORT_SHOW_PicWidth",m_iMarkPicWidth);
	AddPar("QCDB_REPORT_SHOW_LineCharLimit",m_iNoteCharLineLimit);

	CDialog::OnOK();
}

void CDlgReportShowProperty::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	DDX_Text(pDX, IDC_editTitleFont, m_sTitleFont);
	DDX_Text(pDX, IDC_editTitleFontHeight, m_iTitleFontHeight);
	DDX_Text(pDX, IDC_editTitleFontUnderLine, m_iTitleFontUnderLine);
	DDX_Text(pDX, IDC_editTitleFontWeight, m_iTitleFontWeight);

	DDX_Text(pDX, IDC_viceTitleFont, m_sViceTitleFont);
	DDX_Text(pDX, IDC_editViceTitleFontHeight, m_iViceTitleFontHeight);
	DDX_Text(pDX, IDC_editViceTitleFontUnderLine, m_iViceTitleFontUnderLine);
	DDX_Text(pDX, IDC_editViceTitleFontWeight, m_iViceTitleFontWeight);

	DDX_Text(pDX, IDC_footFont, m_sFootFont);
	DDX_Text(pDX, IDC_editFootFontHeight, m_iFootFontHeight);
	DDX_Text(pDX, IDC_editFootFontUnderLine, m_iFootFontUnderLine);
	DDX_Text(pDX, IDC_editFootFontWeight, m_iFootFontWeight);

	DDX_Text(pDX, IDC_contentFont, m_sContentFont);
	DDX_Text(pDX, IDC_editContentFontHeight, m_iContentFontHeight);
	DDX_Text(pDX, IDC_editContentFontUnderLine, m_iContentFontUnderLine);
	DDX_Text(pDX, IDC_editContentFontWeight, m_iContentFontWeight);

	DDX_Text(pDX, IDC_lineWidth, m_iLineWidth);
	DDX_Text(pDX, IDC_EdgeLineWidth, m_iEdgeLineWidth);
	DDX_Text(pDX, IDC_PicWidth, m_iMarkPicWidth);
	DDX_Text(pDX, IDC_noteLineCharLimit, m_iNoteCharLineLimit);
	DDX_Text(pDX, IDC_PicName, m_sPicName);
}


BEGIN_MESSAGE_MAP(CDlgReportShowProperty, CDialog)
	ON_BN_CLICKED(IDC_buttonTitleFont, OnBnClickedbuttontitlefont)
	ON_BN_CLICKED(IDC_buttonViceTitleFont, OnBnClickedbuttonvicetitlefont)
	ON_BN_CLICKED(IDC_buttonFootFont, OnBnClickedbuttonfootfont)
	ON_BN_CLICKED(IDC_buttonContentFont, OnBnClickedbuttoncontentfont)
	ON_BN_CLICKED(IDC_buttonPicName, OnBnClickedbuttonpicname)
END_MESSAGE_MAP()


// CDlgReportShowProperty message handlers

void CDlgReportShowProperty::OnBnClickedbuttontitlefont()
{
	UpdateData(true);

	LOGFONT lf;
	memset(&lf,0,sizeof(LOGFONT));
	strcpy(lf.lfFaceName , (LPCTSTR)m_sTitleFont);
	lf.lfHeight		= m_iTitleFontHeight;
	lf.lfWeight		= m_iTitleFontWeight;
	lf.lfUnderline	= m_iTitleFontUnderLine;

	CFontDialog dlg(&lf);
	if(dlg.DoModal ()==IDCANCEL)return;
	
	dlg.GetCurrentFont (&lf);


	m_sTitleFont			=	dlg.m_lf.lfFaceName ;
	m_iTitleFontHeight		=	dlg.m_lf.lfHeight ;
	m_iTitleFontWeight		=	dlg.m_lf.lfWeight  ;
	m_iTitleFontUnderLine	=	dlg.m_lf.lfUnderline;	
	
	UpdateData(false);
}

void CDlgReportShowProperty::OnBnClickedbuttonvicetitlefont()
{
	UpdateData(true);

	LOGFONT lf;
	memset(&lf,0,sizeof(LOGFONT));
	strcpy(lf.lfFaceName , (LPCTSTR)m_sViceTitleFont);
	lf.lfHeight		= m_iViceTitleFontHeight;
	lf.lfWeight		= m_iViceTitleFontWeight;
	lf.lfUnderline	= m_iViceTitleFontUnderLine;

	CFontDialog dlg(&lf);
	if(dlg.DoModal ()==IDCANCEL)return;
	
	dlg.GetCurrentFont (&lf);


	m_sViceTitleFont			=	dlg.m_lf.lfFaceName ;
	m_iViceTitleFontHeight		=	dlg.m_lf.lfHeight ;
	m_iViceTitleFontWeight		=	dlg.m_lf.lfWeight  ;
	m_iViceTitleFontUnderLine	=	dlg.m_lf.lfUnderline;	
	
	UpdateData(false);
}

void CDlgReportShowProperty::OnBnClickedbuttonfootfont()
{
	UpdateData(true);

	LOGFONT lf;
	memset(&lf,0,sizeof(LOGFONT));
	strcpy(lf.lfFaceName , (LPCTSTR)m_sFootFont);
	lf.lfHeight		= m_iFootFontHeight;
	lf.lfWeight		= m_iFootFontWeight;
	lf.lfUnderline	= m_iFootFontUnderLine;

	CFontDialog dlg(&lf);
	if(dlg.DoModal ()==IDCANCEL)return;
	
	dlg.GetCurrentFont (&lf);


	m_sFootFont			=	dlg.m_lf.lfFaceName ;
	m_iFootFontHeight		=	dlg.m_lf.lfHeight ;
	m_iFootFontWeight		=	dlg.m_lf.lfWeight  ;
	m_iFootFontUnderLine	=	dlg.m_lf.lfUnderline;	

	UpdateData(false);

}

void  CDlgReportShowProperty::GetTitleLogFont(LOGFONT &lf)
{
	memset(&lf,0,sizeof(LOGFONT));
	strcpy(lf.lfFaceName ,(LPCTSTR)m_sTitleFont);
	lf.lfHeight = m_iTitleFontHeight;
	lf.lfWeight = m_iTitleFontWeight;
	lf.lfUnderline = m_iTitleFontUnderLine;
	lf.lfCharSet = GB2312_CHARSET;

	return;
}

void  CDlgReportShowProperty::GetViceTitleLogFont(LOGFONT &lf)
{
	memset(&lf,0,sizeof(LOGFONT));
	strcpy(lf.lfFaceName ,(LPCTSTR)m_sViceTitleFont);
	lf.lfHeight = m_iViceTitleFontHeight;
	lf.lfWeight = m_iViceTitleFontWeight;
	lf.lfUnderline = m_iViceTitleFontUnderLine;
	lf.lfCharSet = GB2312_CHARSET;

	return;
}

void  CDlgReportShowProperty::GetFootLogFont(LOGFONT &lf)
{
	memset(&lf,0,sizeof(LOGFONT));
	strcpy(lf.lfFaceName ,(LPCTSTR)m_sFootFont);
	lf.lfHeight = m_iFootFontHeight;
	lf.lfWeight = m_iFootFontWeight;
	lf.lfUnderline = m_iFootFontUnderLine;
	lf.lfCharSet = GB2312_CHARSET;

	return ;
}


void  CDlgReportShowProperty::GetContentLogFont(LOGFONT &lf)
{
	memset(&lf,0,sizeof(LOGFONT));
	strcpy(lf.lfFaceName ,(LPCTSTR)m_sContentFont);
	lf.lfHeight = m_iContentFontHeight;
	lf.lfWeight = m_iContentFontWeight;
	lf.lfUnderline = m_iContentFontUnderLine;
	lf.lfCharSet = GB2312_CHARSET;

	return;
}

void CDlgReportShowProperty::OnBnClickedbuttoncontentfont()
{
	UpdateData(true);

	LOGFONT lf;
	memset(&lf,0,sizeof(LOGFONT));
	strcpy(lf.lfFaceName , (LPCTSTR)m_sContentFont);
	lf.lfHeight		= m_iContentFontHeight;
	lf.lfWeight		= m_iContentFontWeight;
	lf.lfUnderline	= m_iContentFontUnderLine;

	CFontDialog dlg(&lf);
	if(dlg.DoModal ()==IDCANCEL)return;
	
	dlg.GetCurrentFont (&lf);

	m_sContentFont				=	dlg.m_lf.lfFaceName ;
	m_iContentFontHeight		=	dlg.m_lf.lfHeight ;
	m_iContentFontWeight		=	dlg.m_lf.lfWeight  ;
	m_iContentFontUnderLine		=	dlg.m_lf.lfUnderline;	

	UpdateData(false);

}

int CDlgReportShowProperty::GetLineWidth()
{
	return m_iLineWidth;
}

int CDlgReportShowProperty::GetEdgeLineWidth()
{
	return m_iEdgeLineWidth;
}

CString CDlgReportShowProperty::GetMarkPicName()
{
	return m_sPicName;
}

int CDlgReportShowProperty::GetMarkPicWidth()
{
	return m_iMarkPicWidth;
}

int CDlgReportShowProperty::GetLineCharLimit()
{
	return m_iNoteCharLineLimit;
}
void CDlgReportShowProperty::OnBnClickedbuttonpicname()
{
	CString s=GetFileForOpen("表头图片","jpg");
	if(s=="")return;

	m_sPicName=s;

	UpdateData(false);
}
