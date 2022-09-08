#pragma once
#include "afxwin.h"


// CDlgDataSelectorInd dialog

class CDlgDataSelectorInd : public CDialog
{
	DECLARE_DYNAMIC(CDlgDataSelectorInd)

public:
	CDlgDataSelectorInd(CWnd *pParent=NULL); 
	virtual ~CDlgDataSelectorInd();

// Dialog Data
	enum { IDD = IDD_ChooseRecordsInd };

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support

	DECLARE_MESSAGE_MAP()

public:
	CString GetSQL();
	void SetTableName(CString sTableName);


protected:
	
	CString GetSingleSQL(CString sField, 
					   CString sConditionCC, 
					   CString sData);

	CString GetSQLCondition(CString sCondition);

	void SetFieldNames(CString sDBName,CString sTableName);
	bool IsCharField(CString sField);

	CString m_sTableName;
	CStringArray m_arrayFields;
	CStringArray m_arrayTypes;


protected:

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

	CString m_conditionBetween12;
	CString m_conditionBetween23;
};
