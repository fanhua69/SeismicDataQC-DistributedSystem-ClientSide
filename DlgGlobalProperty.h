#pragma once
#include "afxwin.h"


// CDlgGlobalProperty dialog

class CDlgGlobalProperty : public CPropertyPage
{
	DECLARE_DYNAMIC(CDlgGlobalProperty)

public:
	CDlgGlobalProperty();   // standard constructor
	virtual ~CDlgGlobalProperty();

// Dialog Data
	enum { IDD = IDD_DlgGlobalProperty };
	virtual BOOL	OnInitDialog();

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support

	CStringArray m_arrayEntryNames;

	DECLARE_MESSAGE_MAP()
public:
	afx_msg void OnBnClickedbuttondialentry();
	CString m_sDialEntry;
protected:
	virtual void OnOK();
public:
	CComboBox m_comboDialEntry;
	afx_msg void OnBnClickedbuttondial();
	virtual BOOL OnApply();
};
