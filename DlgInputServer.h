#pragma once
#include "afxwin.h"
#include "StringListCtrl.h"

// CDlgInputServer dialog

class CDlgInputServer : public CPropertyPage
{
	DECLARE_DYNAMIC(CDlgInputServer)

public:
	CDlgInputServer();
	virtual ~CDlgInputServer();
	void GetServerProperty(	CStringArray &arrayNames,
							CStringArray &arrayIPs,
							CStringArray &arrayPorts,
							CStringArray &arrayUserNames,
							CStringArray &arrayPasswords);

// Dialog Data
	enum { IDD = IDD_InputServers };

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	void SaveServerProperty();

	CStringArray m_arrayNames;
	CStringArray m_arrayIPs;
	CStringArray m_arrayPorts;
	CStringArray m_arrayUserNames;
	CStringArray m_arrayPasswords;

	DECLARE_MESSAGE_MAP()
public:
	CStringListCtrl m_listServerProperty;
	CEdit m_editServerIP;
	afx_msg void OnBnClickedbuttonremove();
	afx_msg void OnBnClickedbuttonadd();
protected:
	virtual void OnOK();
public:
	virtual BOOL OnInitDialog();
	virtual BOOL Create(LPCTSTR lpszTemplateName, CWnd* pParentWnd = NULL);
	CString m_sServerName;
	CString m_sPort;
	CString m_sAreas;
	CString m_sServerIP;
	CString m_sUserName;
	CString m_sPassword;
};
