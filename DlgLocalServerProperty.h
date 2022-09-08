#pragma once


// CDlgLocalServerProperty dialog

class CDlgLocalServerProperty : public CPropertyPage
{
	DECLARE_DYNAMIC(CDlgLocalServerProperty)

public:
	CDlgLocalServerProperty();
	virtual ~CDlgLocalServerProperty();

// Dialog Data
	enum { IDD = IDD_LocalPort };

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support

	DECLARE_MESSAGE_MAP()
public:
	int m_iLocalPort;
	virtual void OnOK();
    CString m_sPasswordFile;
    afx_msg void OnBnClickedbuttonpasswordfile();

    CString m_sLocalIP;
};
