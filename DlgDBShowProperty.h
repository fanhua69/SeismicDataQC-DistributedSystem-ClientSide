#pragma once
#include "afxwin.h"
#include "afxcmn.h"


// CDlgDBShowProperty dialog

class CDlgDBShowProperty : public CPropertyPage
{
	DECLARE_DYNAMIC(CDlgDBShowProperty)

public:
	CDlgDBShowProperty() ;//
	virtual ~CDlgDBShowProperty();

// Dialog Data
	enum { IDD = IDD_DBShowProperty };

public:
	CStringArray	m_arrayTable;
	CStringArray	m_arrayExistFields;
	CStringArray	m_arrayShowFields;

	CString m_sUserID;
	CString m_sPassword;
		

	long m_nShowQtyMin;
	long m_nShowQtyMax;
	long m_lShowQtyMaxPermit;

public:
	void SetTableNames(CStringArray &arrayTableNames);
	void SetFieldNames(CStringArray &arrayFieldNames);
	void SetShowFieldNames(CStringArray &arrayShowFieldNames);
	void SetShowQtyMaxPermit(int iQty);
	void SetShowTable(CString sShowTable);
	void SetUser(CString sUserID, CString sPassword);

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support

	void AddTotal(bool bProjectSum);

	DECLARE_MESSAGE_MAP()
public:
	CComboBox m_comboTableName;
	CString m_sTableName;
	CEdit m_editShowQtyMax;
	CListBox m_listExistFields;
	CListBox m_listShowFields;
	afx_msg void OnBnClickedbuttonshowcurrent();
	afx_msg void OnBnClickedbuttonshowall();
	afx_msg void OnBnClickedbuttonremovecurrent();
	afx_msg void OnBnClickedbuttonremoveall();
	afx_msg void OnDeltaposSpinshowqty(NMHDR *pNMHDR, LRESULT *pResult);
	CSpinButtonCtrl m_spinShowQty;
	virtual BOOL OnInitDialog();
	afx_msg void OnEnChangeEditShowQtyPermit();
	afx_msg void OnCbnSelchangeCombotablename();
	virtual LRESULT OnWizardNext();
	virtual BOOL OnSetActive();
	CComboBox m_comboServer;
	CString m_sServer;
	afx_msg void OnBnClickedbuttonsum();
	afx_msg void OnBnClickedbuttontotalunit();
	afx_msg void OnBnClickedbuttontotalproject();
};
