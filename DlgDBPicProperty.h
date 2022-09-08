#pragma once


// CDlgDBPicProperty dialog

class CDlgDBPicProperty : public CPropertyPage
{
	DECLARE_DYNAMIC(CDlgDBPicProperty)

public:
	CDlgDBPicProperty();
	virtual ~CDlgDBPicProperty();

// Dialog Data
	enum { IDD = IDD_DBPicProperty };

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support

	DECLARE_MESSAGE_MAP()
public:
	int m_iDBPicWidth;
	int m_iDBPicHeight;
	BOOL m_bDBPicKeepRatio;
};
