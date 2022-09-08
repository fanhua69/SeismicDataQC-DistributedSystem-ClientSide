#pragma once


// CDlgInterfaceProperty dialog

class CDlgInterfaceProperty : public CPropertyPage
{
	DECLARE_DYNAMIC(CDlgInterfaceProperty)

public:
	CDlgInterfaceProperty();   // standard constructor
	virtual ~CDlgInterfaceProperty();

// Dialog Data
	enum { IDD = IDD_InterfaceProperty };

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support

	DECLARE_MESSAGE_MAP()
public:
	BOOL m_bShowInfoExclusively;
	virtual void OnOK();
	BOOL m_bLinkServerAtBegin;
    virtual BOOL OnInitDialog();
};
