// DlgDataSelector.cpp : implementation file
//

#include "stdafx.h"
#include "QCDB.h"
#include "DlgDataSelectorInd.h"
#include "databasemanager.h"
#include "StringExtractor.h"
#include "DlgDBShowProperty.h"
#include "qcglobal.h"

// CDlgDataSelectorInd dialog

IMPLEMENT_DYNAMIC(CDlgDataSelectorInd, CDialog)
CDlgDataSelectorInd::CDlgDataSelectorInd(CWnd *pParent)
	: CDialog(CDlgDataSelectorInd::IDD,pParent)
	, m_sField1(_T(""))
	, m_sCondition1(_T(""))
	, m_sData1(_T(""))
	, m_conditionBetween12(_T(""))
	, m_conditionBetween23(_T(""))
{
	m_sTableName="";
}

CDlgDataSelectorInd::~CDlgDataSelectorInd()
{
}

void CDlgDataSelectorInd::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);

	DDX_CBString(pDX, IDC_comboField1, m_sField1);
	DDX_CBString(pDX, IDC_comboCondition1, m_sCondition1);
	DDX_Text(pDX, IDC_editData1, m_sData1);
	DDX_Control(pDX, IDC_comboField1, m_comboField1);
	DDX_Control(pDX, IDC_comboCondition1, m_comboCondition1);

	DDX_CBString(pDX, IDC_comboField2, m_sField2);
	DDX_CBString(pDX, IDC_comboCondition2, m_sCondition2);
	DDX_Text(pDX, IDC_editData2, m_sData2);
	DDX_Control(pDX, IDC_comboField2, m_comboField2);
	DDX_Control(pDX, IDC_comboCondition2, m_comboCondition2);

	DDX_CBString(pDX, IDC_comboField3, m_sField3);
	DDX_CBString(pDX, IDC_comboCondition3, m_sCondition3);
	DDX_Text(pDX, IDC_editData3, m_sData3);
	DDX_Control(pDX, IDC_comboField3, m_comboField3);
	DDX_Control(pDX, IDC_comboCondition3, m_comboCondition3);

	DDX_Control(pDX, IDC_comboConditionBetween1And2, m_comboBetween12);
	DDX_Control(pDX, IDC_comboConditionBetween2And3, m_comboBetween23);
	DDX_CBString(pDX, IDC_comboConditionBetween1And2, m_conditionBetween12);
	DDX_CBString(pDX, IDC_comboConditionBetween2And3, m_conditionBetween23);
}


BEGIN_MESSAGE_MAP(CDlgDataSelectorInd, CDialog)
END_MESSAGE_MAP()


BOOL CDlgDataSelectorInd::OnInitDialog()
{
	CDialog::OnInitDialog();

	m_comboCondition1.AddString ("等于");
	m_comboCondition1.AddString ("不等于");
	m_comboCondition1.AddString ("小于");
	m_comboCondition1.AddString ("大于");
	m_comboCondition1.AddString ("小于等于");
	m_comboCondition1.AddString ("大于等于");
	m_comboCondition1.AddString ("在列表中");
	m_comboCondition1.AddString ("不在列表中");
	m_comboCondition1.AddString ("介于之间");
	m_comboCondition1.AddString ("不介于之间");
	m_comboCondition1.AddString ("模式匹配");
	m_comboCondition1.AddString ("模式不匹配");
	m_comboCondition1.AddString ("为空");
	m_comboCondition1.AddString ("不为空");

	m_comboCondition2.AddString ("等于");
	m_comboCondition2.AddString ("不等于");
	m_comboCondition2.AddString ("小于");
	m_comboCondition2.AddString ("大于");
	m_comboCondition2.AddString ("小于等于");
	m_comboCondition2.AddString ("大于等于");
	m_comboCondition2.AddString ("在列表中");
	m_comboCondition2.AddString ("不在列表中");
	m_comboCondition2.AddString ("介于之间");
	m_comboCondition2.AddString ("不介于之间");
	m_comboCondition2.AddString ("模式匹配");
	m_comboCondition2.AddString ("模式不匹配");
	m_comboCondition2.AddString ("为空");
	m_comboCondition2.AddString ("不为空");


	m_comboCondition3.AddString ("等于");
	m_comboCondition3.AddString ("不等于");
	m_comboCondition3.AddString ("小于");
	m_comboCondition3.AddString ("大于");
	m_comboCondition3.AddString ("小于等于");
	m_comboCondition3.AddString ("大于等于");
	m_comboCondition3.AddString ("在列表中");
	m_comboCondition3.AddString ("不在列表中");
	m_comboCondition3.AddString ("介于之间");
	m_comboCondition3.AddString ("不介于之间");
	m_comboCondition3.AddString ("模式匹配");
	m_comboCondition3.AddString ("模式不匹配");
	m_comboCondition3.AddString ("为空");
	m_comboCondition3.AddString ("不为空");

	m_comboBetween12.AddString ("并且");
	m_comboBetween12.AddString ("或者");
	m_comboBetween12.AddString ("且不");

	m_comboBetween23.AddString ("并且");
	m_comboBetween23.AddString ("或者");
	m_comboBetween23.AddString ("且不");

	if(m_sTableName!="")
	{
		SetFieldNames (GetDBName(),m_sTableName);
	}


	return TRUE;  // return TRUE unless you set the focus to a control
	// EXCEPTION: OCX Property Pages should return FALSE
}

void CDlgDataSelectorInd::SetTableName(CString sTableName)
{
	m_sTableName=sTableName;
}

void CDlgDataSelectorInd::SetFieldNames(CString sDBName,CString sTableName)
{
	if(sTableName==""||sDBName=="")
	{
		return;
	}

	m_sTableName=sTableName;

	if(!GetFieldNames(					sTableName,
										m_arrayFields,
										m_arrayTypes))
	{
		return ;
	};

	m_comboField1.ResetContent ();
	m_comboField2.ResetContent ();
	m_comboField3.ResetContent();

	int iQty=m_arrayFields.GetCount ();

	CString sField;

	for(int i=0;i<iQty;i++)
	{
		sField=m_arrayFields.GetAt (i);
		m_comboField1.AddString (sField);
		m_comboField2.AddString (sField);
		m_comboField3.AddString (sField);
	}
}

CString CDlgDataSelectorInd::GetSQL()
{
	CString sSQL1=GetSingleSQL(m_sField1,m_sCondition1,m_sData1);
	CString sSQL2=GetSingleSQL(m_sField2,m_sCondition2,m_sData2);
	CString sSQL3=GetSingleSQL(m_sField3,m_sCondition3,m_sData3);

	char sSQL[2000];
	sprintf(sSQL,"select * from %s ",m_sTableName);
	if(sSQL1!="")
	{
		strcat(sSQL," where ");
		strcat(sSQL,sSQL1);
	}

	if(sSQL2!="")
	{
		if(m_conditionBetween12=="并且")
		{
			strcat(sSQL," and ");
		}
		else if(m_conditionBetween12=="或者")
		{
			strcat(sSQL," or ");
		}
		else
		{
			strcat(sSQL," not ");
		}
		strcat(sSQL,sSQL2);

	}

	if(sSQL3!="")
	{
		if(m_conditionBetween23=="并且")
		{
			strcat(sSQL," and ");
		}
		else if(m_conditionBetween23=="或者")
		{
			strcat(sSQL," or ");
		}
		else
		{
			strcat(sSQL," not ");
		}
		strcat(sSQL,sSQL3);
	}

	return sSQL;
}

bool CDlgDataSelectorInd::IsCharField(CString sField)
{
	int iOrder=-1;
	for(int i=0;i<m_arrayFields.GetCount ();i++)
	{
		if(sField==m_arrayFields.GetAt (i))
		{
			iOrder=i;
			break;
		}
	}
	if(iOrder==-1)
	{
		CString s="不存在字段名："+sField;
        TRACEERROR(s);
		return false;
	}

	CString sType=m_arrayTypes.GetAt (iOrder);
	sType=sType.Left (4);
	if( sType=="TEXT"||
		sType=="CHAR"||
		sType=="NOTE"||
		sType=="DATE")
	{
		return true;
	}
	else
	{
		return false;
	}
}

CString CDlgDataSelectorInd::GetSingleSQL(CString sField, 
									   CString sConditionCC, 
									   CString sDataInput)
{
	char sSQL[2000];
	char sTemp[200],sTemp2[200],sTemp3[200];

	if(sField==""||sConditionCC==""||sDataInput=="")
	{
		return CString("");
	}

	memset(sSQL,0,2000);
	CString sCondition;
	CStringExtractor ex;
	int i,n;
	char sData[200];
	strcpy(sData,sDataInput);

	sprintf(sSQL," %s ",sField);
	sCondition=GetSQLCondition(sConditionCC);
	if(sConditionCC=="")return CString ("");

	bool bIsChar=IsCharField(sField);


	if(sCondition=="between"||sCondition=="not between")
	{
		ex.SetString (sData);
		if(ex.GetSubStringQty ()!=2)
		{
			AfxMessageBox("错误：条件数据应为两个!");
			return CString("");
		}
		else
		{
			CString s1=ex.GetSubString (0);
			CString s2=ex.GetSubString (1);

			if(bIsChar)
			{
				sprintf(sTemp," %s '%s' and '%s'",sCondition,s1,s2);
			}
			else
			{
				sprintf(sTemp," %s %s and %s",sCondition,s1,s2);
			}
			strcat(sSQL,sTemp);			
		}
		return sSQL;
	}
	else if(sCondition=="in"||sCondition=="not in")
	{
		ex.SetString (sData);
		n=ex.GetSubStringQty ();
		if(n<1)
		{
			AfxMessageBox("错误：条件字符串为空!");
			return CString ("");
		}

		sprintf(sTemp," %s (",sCondition);
		sprintf(sTemp2,"%s",ex.GetSubString (0));
		for(i=1;i<ex.GetSubStringQty ();i++)
		{
			if(bIsChar)
			{
				sprintf(sTemp3,",'%s'",ex.GetSubString (i));
			}
			else
			{
				sprintf(sTemp3,",%s",ex.GetSubString (i));
			}
			strcat(sTemp2,sTemp3);
		}
		strcat(sTemp,sTemp2);
		strcat(sTemp,")");
		strcat(sSQL,sTemp);	
		return sSQL;
	}

	else if(sCondition=="is null"||sCondition=="is not null")
	{
		strcat(sSQL,sCondition);
		return sSQL;
	}
	else
	{
		strcat(sSQL,sCondition);
		if(bIsChar)
		{
			sprintf(sTemp," '%s' ",sData);
		}
		else
		{
			sprintf(sTemp," %s ",sData);
		}
		strcat(sSQL,sTemp);
		return sSQL;
	}
}

CString CDlgDataSelectorInd::GetSQLCondition(CString sCondition)
{
	if(sCondition=="等于")
	{
		return CString("=");
	}
	else if (sCondition=="不等于")
	{
		return CString("!=");
	}
	else if (sCondition=="小于")
	{
		return CString("<");
	}
	else if (sCondition=="大于")
	{
		return CString(">");
	}
	else if (sCondition=="小于等于")
	{
		return CString("<=");
	}
	else if (sCondition=="大于等于")
	{
		return CString(">=");
	}
	else if (sCondition=="在列表中")
	{
		return CString("in");
	}
	else if (sCondition=="不在列表中")
	{
		return CString("not in");
	}
	else if (sCondition=="介于之间")
	{
		return CString("between");
	}
	else if (sCondition=="不介于之间")
	{
		return CString("not between");
	}
	else if (sCondition=="模式匹配")
	{
		return CString("like");
	}
	else if (sCondition=="模式不匹配")
	{
		return CString("not like");
	}
	else if (sCondition=="为空")
	{
		return CString("is null");
	}
	else if (sCondition=="不为空")
	{
		return CString("is not null");
	}
	else
	{
		return CString("");
	}
}

