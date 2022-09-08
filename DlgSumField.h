#pragma once
#include "afxwin.h"


// CDlgSumField dialog

class CDlgSumField : public CDialog
{
	DECLARE_DYNAMIC(CDlgSumField)

public:
	CDlgSumField(CWnd* pParent = NULL);   // standard constructor
	virtual ~CDlgSumField();

// Dialog Data
	enum { IDD = IDD_DBSumField };

	void SetExistFields(CStringArray &arrayFields);
	void SetAddedFields(CStringArray &arrayFields);
	void GetSummedFields(CString &sSummedName, CStringArray &arraySummedFields);
	void SetText(CString sTitle, CString sResultName, CString sResultFields);
	
protected:
	CStringArray m_arrayExistFields;
	CStringArray m_arraySummedFields;

	CString m_sTitle;
	CString m_sResultName;
	CString m_ResultFields;

	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support


	DECLARE_MESSAGE_MAP()
public:
	CString m_sSummedName;
	CListBox m_listExistFields;
	CListBox m_listSummedFields;
	afx_msg void OnBnClickedbuttonsumcurrent();
	afx_msg void OnBnClickedbuttonsumall();
	afx_msg void OnBnClickedbuttonremovecurrent();
	afx_msg void OnBnClickedbuttonremoveall();
	virtual BOOL OnInitDialog();
protected:
	virtual void OnOK();
};
