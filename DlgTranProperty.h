#pragma once


// CDlgTranProperty dialog

class CDlgTranProperty : public CPropertyPage
{
	DECLARE_DYNAMIC(CDlgTranProperty)

public:
	CDlgTranProperty();   // standard constructor
	virtual ~CDlgTranProperty();

// Dialog Data
	enum { IDD = IDD_DlgTranProperty };

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support

	DECLARE_MESSAGE_MAP()
	
	int m_iMaxHandInRecord;
	int m_iMaxHandDownRecord;

public:
	virtual void OnOK();
};
