#pragma once
#include "afxwin.h"


// CDlgProjectProperty dialog

class CDlgProjectProperty : public CPropertyPage
{
	DECLARE_DYNAMIC(CDlgProjectProperty)

public:
	CDlgProjectProperty();   // standard constructor
	virtual ~CDlgProjectProperty();

// Dialog Data
	enum { IDD = IDD_DlgPeojectProperty };

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	
public:

	DECLARE_MESSAGE_MAP()
public:
	virtual BOOL OnInitDialog();
	virtual void OnOK();

	CString m_sWorkArea;
	CString m_sWorkCrew;
	CString m_sWorkYear;
	CString m_sWorkMonth;
	CString m_sTempFilePath;
    CString m_sDataBasePath;
	afx_msg void OnBnClickedbuttontempfilepath();
    afx_msg void OnBnClickedbuttondatabasefilepath();
};
