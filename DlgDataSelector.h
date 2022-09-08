#pragma once
#include "afxwin.h"
#include "MyDate.h"


// CDlgDataSelector dialog

class CDlgDataSelector : public CPropertyPage
{
	DECLARE_DYNAMIC(CDlgDataSelector)

public:
	CDlgDataSelector(); 
	virtual ~CDlgDataSelector();

// Dialog Data
	enum { IDD = IDD_ChooseRecords };

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support

	DECLARE_MESSAGE_MAP()

public:
	CString GetSQL();

	CTableDate GetStartProperty();

protected:
	
	CString GetSingleSQL(CString sField, 
					   CString sConditionCC, 
					   CString sData);

	CString GetTimeSQL(	CString sConditionCC, 
						CString sYear,
						CString sMonth,
						CString sDay);

	CString GetLogicString(CString sLogic);



	CString GetSQLCondition(CString sCondition);

	void SetFieldNames();

	CString m_sTableName;

protected:
	CStringArray m_arrayFields;
	CStringArray m_arrayTypes;

	void SavePar();
	void LoadPar();



public:
	CString m_sField1;
	CString m_sCondition1;
	CString m_sData1;
	CComboBox m_comboField1;
	CComboBox m_comboCondition1;
	
	CString m_sField2;
	CString m_sCondition2;
	CString m_sData2;
	CString m_sData2B;
	CComboBox m_comboField2;
	CComboBox m_comboCondition2;

	CString m_sField3;
	CString m_sCondition3;
	CString m_sData3;
	CString m_sData3B;
	CComboBox m_comboField3;
	CComboBox m_comboCondition3;

	CComboBox m_comboBetween12;
	CComboBox m_comboBetween23;

	virtual BOOL OnInitDialog();

	CString m_sConditionBetween12;
	CString m_sConditionBetween23;
	virtual BOOL OnSetActive();
	CString  m_sYear1;
	CString  m_sYear2;
	CString  m_sMonth1;
	CString  m_sMonth2;
	CString  m_sDay1;
	CString  m_sDay2;

	CString m_sYearField;
	CString m_sMonthField;
	CString m_sDayField;
	CComboBox m_comboYearField;
	CComboBox m_comboMonthField;
	CComboBox m_comboDayField;
	CString m_sConditionBetweenTimeAnd1;
	CComboBox m_comboBetweenTimeAnd1;
	virtual LRESULT OnWizardNext();
	virtual BOOL OnWizardFinish();
	BOOL m_bFieldOnlyHavingYMD;
	afx_msg void OnBnClickedcheckfieldonlyhaveymd();
};
