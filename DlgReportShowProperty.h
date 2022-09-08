#pragma once


// CDlgReportShowProperty dialog

class CDlgReportShowProperty : public CDialog
{
	DECLARE_DYNAMIC(CDlgReportShowProperty)

public:
	CDlgReportShowProperty(CWnd* pParent = NULL);   // standard constructor
	virtual ~CDlgReportShowProperty();

// Dialog Data
	enum { IDD = IDD_DlgReportShowProperty };

public:
	void	GetTitleLogFont(LOGFONT &lf);
	void	GetViceTitleLogFont(LOGFONT &lf);
	void	GetFootLogFont(LOGFONT &lf);
	void	GetContentLogFont(LOGFONT &lf);
	int 	GetLineWidth();
	int		GetEdgeLineWidth();
	int		GetMarkPicWidth();
	CString GetMarkPicName();
	int		GetLineCharLimit();


protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support

	DECLARE_MESSAGE_MAP()
public:
	afx_msg void OnBnClickedbuttontitlefont();
	afx_msg void OnBnClickedbuttonvicetitlefont();
	afx_msg void OnBnClickedbuttonfootfont();
	afx_msg void OnBnClickedbuttoncontentfont();
	
	CString m_sTitleFont;
	int m_iTitleFontHeight;
	int m_iTitleFontUnderLine;
	int m_iTitleFontWeight;

	CString m_sViceTitleFont;
	int m_iViceTitleFontHeight;
	int m_iViceTitleFontUnderLine;
	int m_iViceTitleFontWeight;

	CString m_sFootFont;
	int m_iFootFontHeight;
	int m_iFootFontUnderLine;
	int m_iFootFontWeight;

	CString m_sContentFont;
	int m_iContentFontHeight;
	int m_iContentFontUnderLine;
	int m_iContentFontWeight;

	int m_iLineWidth;
	int m_iEdgeLineWidth;

	int m_iMarkPicWidth;
	int m_iNoteCharLineLimit;

protected:
	virtual void OnOK();
public:
	afx_msg void OnBnClickedbuttonpicname();
	CString m_sPicName;
};
