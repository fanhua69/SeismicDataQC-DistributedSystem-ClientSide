// DlgDataSelector.cpp : implementation file
//

#include "stdafx.h"
#include "QCDB.h"
#include "DlgDataSelector.h"
#include "databasemanager.h"
#include "StringExtractor.h"
#include ".\dlgdataselector.h"
#include "DlgDBShowProperty.h"
#include "qcglobal.h"
#include "TableProcessor.h"

// CDlgDataSelector dialog

IMPLEMENT_DYNAMIC(CDlgDataSelector, CPropertyPage)
CDlgDataSelector::CDlgDataSelector()
	: CPropertyPage(CDlgDataSelector::IDD)
	, m_sField1(_T(""))
	, m_sCondition1(_T(""))
	, m_sData1(_T(""))
	, m_sYearField(_T(""))
	, m_sMonthField(_T(""))
	, m_sDayField(_T(""))
	, m_sConditionBetweenTimeAnd1(_T(""))
	, m_bFieldOnlyHavingYMD(FALSE)
{
}


CDlgDataSelector::~CDlgDataSelector()
{
}

void CDlgDataSelector::LoadPar()
{
	GetPar("DLG_DATASELECTOR_"+m_sTableName+"_StartYear",m_sYear1);
	GetPar("DLG_DATASELECTOR_"+m_sTableName+"_StartMonth",m_sMonth1);
	GetPar("DLG_DATASELECTOR_"+m_sTableName+"_StartDay",m_sDay1);
	GetPar("DLG_DATASELECTOR_"+m_sTableName+"_EndYear",m_sYear2);
	GetPar("DLG_DATASELECTOR_"+m_sTableName+"_EndMonth",m_sMonth2);
	GetPar("DLG_DATASELECTOR_"+m_sTableName+"_EndDay",m_sDay2);
	
	GetPar("DLG_DATASELECTOR_"+m_sTableName+"_YearField",m_sYearField);
	GetPar("DLG_DATASELECTOR_"+m_sTableName+"_MonthField",m_sMonthField);
	GetPar("DLG_DATASELECTOR_"+m_sTableName+"_DayField",m_sDayField);

	GetPar("DLG_DATASELECTOR_"+m_sTableName+"_Field1",m_sField1);
	GetPar("DLG_DATASELECTOR_"+m_sTableName+"_Condition1",m_sCondition1);
	GetPar("DLG_DATASELECTOR_"+m_sTableName+"_Data1",m_sData1);
	GetPar("DLG_DATASELECTOR_"+m_sTableName+"_ConditionBetweenTimeAnd1",m_sConditionBetweenTimeAnd1);

	GetPar("DLG_DATASELECTOR_"+m_sTableName+"_Field2",m_sField2);
	GetPar("DLG_DATASELECTOR_"+m_sTableName+"_Condition2",m_sCondition2);
	GetPar("DLG_DATASELECTOR_"+m_sTableName+"_Data2",m_sData2);
	GetPar("DLG_DATASELECTOR_"+m_sTableName+"_ConditionBetween1And2",m_sConditionBetween12);

	GetPar("DLG_DATASELECTOR_"+m_sTableName+"_Field3",m_sField3);
	GetPar("DLG_DATASELECTOR_"+m_sTableName+"_Condition3",m_sCondition3);
	GetPar("DLG_DATASELECTOR_"+m_sTableName+"_Data3",m_sData3);
	GetPar("DLG_DATASELECTOR_"+m_sTableName+"_ConditionBetween2And3",m_sConditionBetween23);

	GetPar("DLG_DATASELECTOR_"+m_sTableName+"_OnlyYMD",m_bFieldOnlyHavingYMD);
	
	UpdateData(false);
}

void CDlgDataSelector::DoDataExchange(CDataExchange* pDX)
{
	CPropertyPage::DoDataExchange(pDX);

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
	DDX_CBString(pDX, IDC_comboConditionBetween1And2, m_sConditionBetween12);
	DDX_CBString(pDX, IDC_comboConditionBetween2And3, m_sConditionBetween23);
	DDX_Text(pDX, IDC_editYear1, m_sYear1);
	DDX_Text(pDX, IDC_editMonth1, m_sMonth1);
	DDX_Text(pDX, IDC_editDay1, m_sDay1);
	DDX_Text(pDX, IDC_editYear2, m_sYear2);
	DDX_Text(pDX, IDC_editMonth2, m_sMonth2);
	DDX_Text(pDX, IDC_editDay2, m_sDay2);
	DDX_CBString(pDX, IDC_comboYearField, m_sYearField);
	DDX_CBString(pDX, IDC_comboMonthField, m_sMonthField);
	DDX_CBString(pDX, IDC_comboDayField, m_sDayField);
	DDX_Control(pDX, IDC_comboYearField, m_comboYearField);
	DDX_Control(pDX, IDC_comboMonthField, m_comboMonthField);
	DDX_Control(pDX, IDC_comboDayField, m_comboDayField);
	DDX_CBString(pDX, IDC_comboConditionBetweenTimeAnd1, m_sConditionBetweenTimeAnd1);
	DDX_Control(pDX, IDC_comboConditionBetweenTimeAnd1, m_comboBetweenTimeAnd1);
	DDX_Check(pDX, IDC_checkFieldOnlyHaveYMD, m_bFieldOnlyHavingYMD);
}


BEGIN_MESSAGE_MAP(CDlgDataSelector, CPropertyPage)
	ON_BN_CLICKED(IDC_checkFieldOnlyHaveYMD, OnBnClickedcheckfieldonlyhaveymd)
END_MESSAGE_MAP()


BOOL CDlgDataSelector::OnInitDialog()
{
	CPropertyPage::OnInitDialog();

	m_comboCondition1.AddString ("����");
	m_comboCondition1.AddString ("������");
	m_comboCondition1.AddString ("С��");
	m_comboCondition1.AddString ("����");
	m_comboCondition1.AddString ("С�ڵ���");
	m_comboCondition1.AddString ("���ڵ���");
	m_comboCondition1.AddString ("���б���");
	m_comboCondition1.AddString ("�����б���");
	m_comboCondition1.AddString ("����֮��");
	m_comboCondition1.AddString ("������֮��");
	m_comboCondition1.AddString ("ģʽƥ��");
	m_comboCondition1.AddString ("ģʽ��ƥ��");
	m_comboCondition1.AddString ("Ϊ��");
	m_comboCondition1.AddString ("��Ϊ��");

	m_comboCondition2.AddString ("����");
	m_comboCondition2.AddString ("������");
	m_comboCondition2.AddString ("С��");
	m_comboCondition2.AddString ("����");
	m_comboCondition2.AddString ("С�ڵ���");
	m_comboCondition2.AddString ("���ڵ���");
	m_comboCondition2.AddString ("���б���");
	m_comboCondition2.AddString ("�����б���");
	m_comboCondition2.AddString ("����֮��");
	m_comboCondition2.AddString ("������֮��");
	m_comboCondition2.AddString ("ģʽƥ��");
	m_comboCondition2.AddString ("ģʽ��ƥ��");
	m_comboCondition2.AddString ("Ϊ��");
	m_comboCondition2.AddString ("��Ϊ��");


	m_comboCondition3.AddString ("����");
	m_comboCondition3.AddString ("������");
	m_comboCondition3.AddString ("С��");
	m_comboCondition3.AddString ("����");
	m_comboCondition3.AddString ("С�ڵ���");
	m_comboCondition3.AddString ("���ڵ���");
	m_comboCondition3.AddString ("���б���");
	m_comboCondition3.AddString ("�����б���");
	m_comboCondition3.AddString ("����֮��");
	m_comboCondition3.AddString ("������֮��");
	m_comboCondition3.AddString ("ģʽƥ��");
	m_comboCondition3.AddString ("ģʽ��ƥ��");
	m_comboCondition3.AddString ("Ϊ��");
	m_comboCondition3.AddString ("��Ϊ��");

	m_comboBetweenTimeAnd1.AddString ("����");
	m_comboBetweenTimeAnd1.AddString ("����");
	m_comboBetweenTimeAnd1.AddString ("�Ҳ�");

	m_comboBetween12.AddString ("����");
	m_comboBetween12.AddString ("����");
	m_comboBetween12.AddString ("�Ҳ�");

	m_comboBetween23.AddString ("����");
	m_comboBetween23.AddString ("����");
	m_comboBetween23.AddString ("�Ҳ�");

	// TODO:  Add extra initialization here

	return TRUE;  // return TRUE unless you set the focus to a control
	// EXCEPTION: OCX Property Pages should return FALSE
}

void CDlgDataSelector::SetFieldNames()
{
	m_arrayFields.RemoveAll ();
	m_arrayTypes.RemoveAll ();

	CStringArray arrayFields,arrayTypes;
	if(!GetFieldNames(					m_sTableName,
										m_arrayFields,
										m_arrayTypes))
	{
		return ;
	};

	m_comboField1.ResetContent ();
	m_comboField2.ResetContent ();
	m_comboField3.ResetContent();
	m_comboYearField.ResetContent ();
	m_comboMonthField.ResetContent ();
	m_comboDayField.ResetContent ();

	int iQty=m_arrayFields.GetCount ();

	CString sField;

	for(int i=0;i<iQty;i++)
	{
		sField=m_arrayFields.GetAt (i);
		m_comboField1.AddString (sField);
		m_comboField2.AddString (sField);
		m_comboField3.AddString (sField);
		
		if(!m_bFieldOnlyHavingYMD)
		{
			m_comboYearField.AddString (sField);
			m_comboMonthField.AddString (sField);
			m_comboDayField.AddString (sField);
		}
		else
		{
			if(sField.Find ("��")!=-1)
			{
				m_comboYearField.AddString (sField);
			}
			if(sField.Find ("��")!=-1)
			{
				m_comboMonthField.AddString (sField);
			}
			if(sField.Find ("��")!=-1)
			{
				m_comboDayField.AddString (sField);
			}
		}
	}

	LoadPar();

}

CTableDate CDlgDataSelector::GetStartProperty()
{
	CTableDate dateStart;

	dateStart.sYearField	=m_sYearField;
	dateStart.sMonthField	=m_sMonthField;
	dateStart.sDayField		=m_sDayField;
	dateStart.sYear			=m_sYear1;
	dateStart.sMonth	    =m_sMonth1;
	dateStart.sDay			=m_sDay1;

	return dateStart;
}

CString CDlgDataSelector::GetSQL()
{
	CString sTime1=GetTimeSQL("���ڵ���",m_sYear1,m_sMonth1,m_sDay1);
	CString sTime2=GetTimeSQL("С�ڵ���",m_sYear2,m_sMonth2,m_sDay2);
	CString sSQL1=GetSingleSQL(m_sField1,m_sCondition1,m_sData1);
	CString sSQL2=GetSingleSQL(m_sField2,m_sCondition2,m_sData2);
	CString sSQL3=GetSingleSQL(m_sField3,m_sCondition3,m_sData3);

	CString sSQL;
	sSQL.Format ("select * from %s ",m_sTableName);

	if(sTime1!=""||sTime2!=""||sSQL1!=""||sSQL2!=""||sSQL3!="")
	{
		sSQL+=" where ";
	}

	CString sCondition;

    //if(sTime1!="")
    //{
    //    if(sCondition!="")
    //    {
    //        sCondition+=" and ";
    //    }
    //    sCondition+=" "+sTime1;
    //} Do not add here, because will add later.

	if(sTime2!="")
	{
		if(sCondition!="")
		{
			sCondition+=" and ";
		}
		sCondition+=" "+sTime2;
	}


	if(sSQL1!="")
	{
		if(sCondition!="")
		{
			sCondition+=GetLogicString(m_sConditionBetweenTimeAnd1);
		}
		sCondition+=" "+sSQL1;
	}

	if(sSQL2!="")
	{
		if(sCondition!="")
		{
			sCondition+=GetLogicString(m_sConditionBetween12);
		}
		sCondition+=" "+sSQL2;
	}

	if(sSQL3!="")
	{
		if(sCondition!="")
		{
			sCondition+=GetLogicString(m_sConditionBetween23);
		}
		sCondition+=" "+sSQL3;
	}

	sSQL+=sCondition;

	return sSQL;
}

CString CDlgDataSelector::GetLogicString(CString sLogic)
{
	if(sLogic=="")
	{
		return CString (" ");
	}

	if(sLogic=="����"||sLogic=="����"||sLogic=="��"||sLogic=="��")
	{
		return CString(" and ");
	}
	else if(sLogic=="����"||sLogic=="��")
	{
		return CString(" or ");
	}
	else
	{
		return CString(" not ");
	}
}

CString CDlgDataSelector::GetTimeSQL(	CString sConditionCC, 
										CString sYear,
										CString sMonth,
										CString sDay)
{
	CString sCondition=GetSQLCondition(sConditionCC);
	CTableProcessor tp;
	tp.SetTable (m_sTableName );

	sYear=tp.FormatFieldValue (m_sYearField,sYear);
	sMonth=tp.FormatFieldValue (m_sMonthField,sMonth);
	sDay=tp.FormatFieldValue (m_sDayField,sDay);
	CString s=MakeDateCmpString(m_sYearField,
								m_sMonthField,
								m_sDayField,
								sCondition,
								sYear ,
								sMonth ,
								sDay);


	return s;
}

CString CDlgDataSelector::GetSingleSQL(CString sField, 
									   CString sConditionCC, 
									   CString sDataInput)
{
	char sSQL[2000];
	char sTemp[200];
	CString s="",s1="",s2="";

	if(sField==""||sConditionCC==""||sDataInput=="")
	{
		return CString("");
	}

	// Get the tpe of the field:
	int i,n,iFieldQty=m_arrayFields.GetCount ();
	CString sType="";
	for(i=0;i<iFieldQty;i++)
	{
		s=m_arrayFields.GetAt (i);
		if(s==sField)
		{
			sType=m_arrayTypes.GetAt (i);
			break;
		}
	}
	if(sType=="")return CString ("");
	sType=sType.Left (4);

	//
	memset(sSQL,0,2000);
	CString sCondition;
	char sData[200];
	strcpy(sData,sDataInput);
	CStringExtractor ex;
	ex.SetString (sData);
	int nSubString=ex.GetSubStringQty ();

	sprintf(sSQL," %s ",sField);
	sCondition=GetSQLCondition(sConditionCC);
	if(sConditionCC=="")return CString ("");

	if(sCondition=="between"||sCondition=="not between")
	{
		if(nSubString!=2)
		{
			AfxMessageBox("��������ӦΪ����!");
			return CString("");
		}
		else
		{
			s1=ex.GetSubString (0);
			s2=ex.GetSubString (1);
			if(sType=="TEXT"||sType=="CHAR")
			{
				s1="'"+s1+"'";
				s2="'"+s2+"'";
			}

			sprintf(sTemp," %s %s and %s",sCondition,s1,s2);
			strcat(sSQL,sTemp);			
		}
		return sSQL;
	}
	else if(sCondition=="in"||sCondition=="not in")
	{
		if(nSubString<1)
		{
			AfxMessageBox("�����ַ���Ϊ��!");
			return CString ("");
		}

		sprintf(sTemp," %s (",sCondition);
		for(i=0;i<nSubString;i++)
		{
			s=ex.GetSubString (i);
			s.Trim ();
			if(sType=="TEXT"||sType=="CHAR")
			{
				s ="'"+s +"'";
			}
			if(i!=0)
			{
				strcat(sTemp,",");
			}
			strcat(sTemp,LPCTSTR(s));
		}
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

		if(sType=="TEXT"||sType=="CHAR")
		{
			s ="'"+sDataInput +"'";
		}

		strcat(sSQL,(LPCTSTR)s);
		return sSQL;
	}
}

CString CDlgDataSelector::GetSQLCondition(CString sCondition)
{
	if(sCondition=="����")
	{
		return CString("=");
	}
	else if (sCondition=="������")
	{
		return CString("!=");
	}
	else if (sCondition=="С��")
	{
		return CString("<");
	}
	else if (sCondition=="����")
	{
		return CString(">");
	}
	else if (sCondition=="С�ڵ���")
	{
		return CString("<=");
	}
	else if (sCondition=="���ڵ���")
	{
		return CString(">=");
	}
	else if (sCondition=="���б���")
	{
		return CString("in");
	}
	else if (sCondition=="�����б���")
	{
		return CString("not in");
	}
	else if (sCondition=="����֮��")
	{
		return CString("between");
	}
	else if (sCondition=="������֮��")
	{
		return CString("not between");
	}
	else if (sCondition=="ģʽƥ��")
	{
		return CString("like");
	}
	else if (sCondition=="ģʽ��ƥ��")
	{
		return CString("not like");
	}
	else if (sCondition=="Ϊ��")
	{
		return CString("is null");
	}
	else if (sCondition=="��Ϊ��")
	{
		return CString("is not null");
	}
	else
	{
		return CString("");
	}
}


BOOL CDlgDataSelector::OnSetActive()
{
	CPropertySheet *pSheet=(CPropertySheet *)GetParent();
	pSheet->SetWizardButtons(PSWIZB_BACK|PSWIZB_FINISH);

	CDialog *pDialog=pSheet->GetPage (0);
	m_sTableName=((CDlgDBShowProperty*)pDialog)->m_sTableName ;

	SetFieldNames ();

	return CPropertyPage::OnSetActive();
}

LRESULT CDlgDataSelector::OnWizardNext()
{
	UpdateData(true);
	SavePar();
	return CPropertyPage::OnWizardNext();
}

BOOL CDlgDataSelector::OnWizardFinish()
{
	UpdateData(true);
	SavePar();
	return CPropertyPage::OnWizardFinish();
}

void CDlgDataSelector::SavePar()
{
	AddPar("DLG_DATASELECTOR_"+m_sTableName+"_StartYear",m_sYear1);
	AddPar("DLG_DATASELECTOR_"+m_sTableName+"_StartMonth",m_sMonth1);
	AddPar("DLG_DATASELECTOR_"+m_sTableName+"_StartDay",m_sDay1);
	AddPar("DLG_DATASELECTOR_"+m_sTableName+"_EndYear",m_sYear2);
	AddPar("DLG_DATASELECTOR_"+m_sTableName+"_EndMonth",m_sMonth2);
	AddPar("DLG_DATASELECTOR_"+m_sTableName+"_EndDay",m_sDay2);
	
	AddPar("DLG_DATASELECTOR_"+m_sTableName+"_YearField",m_sYearField);
	AddPar("DLG_DATASELECTOR_"+m_sTableName+"_MonthField",m_sMonthField);
	AddPar("DLG_DATASELECTOR_"+m_sTableName+"_DayField",m_sDayField);

	AddPar("DLG_DATASELECTOR_"+m_sTableName+"_Field1",m_sField1);
	AddPar("DLG_DATASELECTOR_"+m_sTableName+"_Condition1",m_sCondition1);
	AddPar("DLG_DATASELECTOR_"+m_sTableName+"_Data1",m_sData1);
	AddPar("DLG_DATASELECTOR_"+m_sTableName+"_ConditionBetweenTimeAnd1",m_sConditionBetweenTimeAnd1);

	AddPar("DLG_DATASELECTOR_"+m_sTableName+"_Field2",m_sField2);
	AddPar("DLG_DATASELECTOR_"+m_sTableName+"_Condition2",m_sCondition2);
	AddPar("DLG_DATASELECTOR_"+m_sTableName+"_Data2",m_sData2);
	AddPar("DLG_DATASELECTOR_"+m_sTableName+"_ConditionBetween1And2",m_sConditionBetween12);

	AddPar("DLG_DATASELECTOR_"+m_sTableName+"_Field3",m_sField3);
	AddPar("DLG_DATASELECTOR_"+m_sTableName+"_Condition3",m_sCondition3);
	AddPar("DLG_DATASELECTOR_"+m_sTableName+"_Data3",m_sData3);
	AddPar("DLG_DATASELECTOR_"+m_sTableName+"_ConditionBetween2And3",m_sConditionBetween23);
}

void CDlgDataSelector::OnBnClickedcheckfieldonlyhaveymd()
{
	UpdateData(true);
	AddPar("DLG_DATASELECTOR_"+m_sTableName+"_OnlyYMD",m_bFieldOnlyHavingYMD);
	SetFieldNames ();
}
