#pragma once
#include "afxcmn.h"


// CDlgChoosePage dialog

class CDlgChoosePage : public CDialog
{
	DECLARE_DYNAMIC(CDlgChoosePage)

public:
	CDlgChoosePage(CWnd* pParent = NULL);   // standard constructor
	virtual ~CDlgChoosePage();

// Dialog Data
	enum { IDD = IDD_ChoosePage };

	void SetTotalRecordQty(int iQty);

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support


	DECLARE_MESSAGE_MAP()
public:
	int m_iTotalPage;
	int m_iCurrentPage;
	int m_iChosenPage;
	int m_iRecordTotalQty;

	CSliderCtrl m_sliderChoosePage;
	afx_msg void OnNMReleasedcaptureSliderChoosepage(NMHDR *pNMHDR, LRESULT *pResult);
	virtual BOOL OnInitDialog();
	afx_msg void OnEnChangeeditchoosepage();
	int m_iRecordQtyPerPage;
	afx_msg void OnEnChangeeditrecordqtyperpage();
	int m_iTotalRecord;
};
