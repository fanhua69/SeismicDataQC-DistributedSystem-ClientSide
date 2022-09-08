#pragma once


// CDlgEditShowProperty dialog

class CDlgEditShowProperty : public CPropertyPage
{
	DECLARE_DYNAMIC(CDlgEditShowProperty)

public:
	CDlgEditShowProperty(CWnd* pParent = NULL);   // standard constructor
	virtual ~CDlgEditShowProperty();

// Dialog Data
	enum { IDD = IDD_RecordShowProperty };

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support

	DECLARE_MESSAGE_MAP()
public:
	int m_iPicWidth;
	int m_iPicHeight;
	BOOL m_bKeepRatio;
	int m_iBigPicWidth;
	int m_iBigPicHeight;
	BOOL m_bBigPicKeepRatio;
	int m_iBigPicOffsetX;
	int m_iBigPicOffsetY;
	BOOL m_bDrawCellPic;
	BOOL m_bDrawBigPic;
};
