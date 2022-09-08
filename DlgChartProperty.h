#pragma once
#include "afxwin.h"


// CDlgChartProperty dialog
#include "GraphGroupProperty.h"

class CDlgChartProperty : public CDialog
{
	DECLARE_DYNAMIC(CDlgChartProperty)

public:
	CDlgChartProperty(CWnd* pParent = NULL);   // standard constructor
	virtual ~CDlgChartProperty();

// Dialog Data
	enum { IDD = IDD_DlgGraphProperty };
protected:

public:
	CPtrArray m_arraySerProperty;


	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	void SetChartStyle();
	CGraphGroupPropertySet * SearchSer(CString sSerName);
	void DrawColor(COLORREF cColor);

public:	
	void AddGroupProperty(CGraphGroupPropertySet *pProperty);
	int GetChartStyle();
	void SetChartStyle(int iStyle);

	bool GetPieDrawLabel();
	void SetPieDrawLabel(bool bDrawLabel);

	DECLARE_MESSAGE_MAP()
	
protected:
	int m_iTypeSel;
	CComboBox m_comboChartType;
	BOOL m_bPieMarkRatio;
	CComboBox m_comboSerNames;
	CComboBox m_comboMarkType;
	CString m_sSerName;
	CString m_sChartType;
	CString m_sMarkType;
	UINT m_iMarkSize;
	UINT m_iLineWidth;
	virtual BOOL OnInitDialog();
	void SetCombos();

protected:
	virtual void OnOK();
public:
	afx_msg void OnBnClickedbuttoncolor();
	afx_msg void OnCbnSelchangecombosername();
	afx_msg void OnEnChangeeditmarksize();
	afx_msg void OnEnChangeeditlinewidth();
	afx_msg void OnPaint();
	afx_msg void OnCbnSelchangecombomarktype();
};
