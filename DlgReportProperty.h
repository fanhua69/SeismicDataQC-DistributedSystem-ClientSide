#pragma once
#include "afxwin.h"


// CDlgReportProperty dialog
#include "resource.h"
#include "ListBoxTip.h"

class CDlgReportProperty : public CDialog
{
	DECLARE_DYNAMIC(CDlgReportProperty)

public:
	CDlgReportProperty(CWnd* pParent = NULL);   // standard constructor
	virtual ~CDlgReportProperty();

// Dialog Data
	enum { IDD = IDD_DlgReportProperty };

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support

	CStringArray m_arrayReports;
	CStringArray m_arrayProjects;

	void SetProjectNames(CString sSQL);
	void SetReportNames();


	DECLARE_MESSAGE_MAP()
public:

	CComboBox m_comboReportNames;
	CString m_sReportName;
	CComboBox m_comboProjects;
	CString m_sProject;
	int m_iStartYear;
	int m_iStartMonth;
	int m_iStartDay;
	int m_iEndYear;
	int m_iEndMonth;
	int m_iEndDay;
	CString m_sReportCreator;

	void SetLineNames();




protected:
	virtual void OnOK();
public:
	afx_msg void OnBnClickedbuttonchooseprojects();
	virtual BOOL OnInitDialog();
	int m_iStatYear;
	int m_iStatMonth;
	int m_iStatDay;
	CString m_sReportAuditor;
	CComboBox m_comboLines;
	CString m_sLineName;
	afx_msg void OnCbnSelchangecomboprojects();
	CString m_sReportNo;
	int m_iMonthType;
	int m_iPrintPage;
};
