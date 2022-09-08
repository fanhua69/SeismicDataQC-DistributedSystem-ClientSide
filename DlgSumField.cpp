// DlgSumField.cpp : implementation file
//

#include "stdafx.h"
#include "QCDB.h"
#include "DlgSumField.h"
#include ".\dlgsumfield.h"


// CDlgSumField dialog

IMPLEMENT_DYNAMIC(CDlgSumField, CDialog)
CDlgSumField::CDlgSumField(CWnd* pParent /*=NULL*/)
	: CDialog(CDlgSumField::IDD, pParent)
	, m_sSummedName(_T(""))
{
	m_sTitle="字段求和";
	m_sResultName="求和结果";
	m_ResultFields="求和字段";
}

CDlgSumField::~CDlgSumField()
{
}

void CDlgSumField::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	DDX_Text(pDX, IDC_editSummedName, m_sSummedName);
	DDX_Control(pDX, IDC_ListExistFields, m_listExistFields);
	DDX_Control(pDX, IDC_ListSummedFields, m_listSummedFields);
}


BEGIN_MESSAGE_MAP(CDlgSumField, CDialog)
	ON_BN_CLICKED(IDC_buttonSumCurrent, OnBnClickedbuttonsumcurrent)
	ON_BN_CLICKED(IDC_buttonSumAll, OnBnClickedbuttonsumall)
	ON_BN_CLICKED(IDC_buttonRemoveCurrent, OnBnClickedbuttonremovecurrent)
	ON_BN_CLICKED(IDC_buttonRemoveAll, OnBnClickedbuttonremoveall)
END_MESSAGE_MAP()


// CDlgSumField message handlers

void CDlgSumField::OnBnClickedbuttonsumcurrent()
{
	// Get All Selected Strings:
	int nCount = m_listExistFields.GetSelCount();
	
	int *pSel=new int[nCount+2];

	m_listExistFields.GetSelItems(nCount, pSel);

	CStringArray arraySel;
	arraySel.SetSize (nCount);

	CString s;
	for(int i=0;i<nCount;i++)
	{
		m_listExistFields.GetText (pSel[i],s);
		arraySel.SetAt (i,s);
	}

	// Delete All strings int the exist list:
	int iSel=0;
	for(i=0;i<nCount;i++)
	{
		iSel=m_listExistFields.FindString(0,arraySel.GetAt (i));
		if(iSel!=LB_ERR)
		{
			m_listExistFields.DeleteString (iSel);
		}
	}

	// Add All strings to  the show list:
	for(i=0;i<nCount;i++)
	{
		m_listSummedFields.AddString (arraySel.GetAt (i));
	}

	delete []pSel;
}

void CDlgSumField::OnBnClickedbuttonsumall()
{
	int nCount = m_listExistFields.GetCount ();
	
	// Get All strings in the Exist List:
	CStringArray arraySel;
	arraySel.SetSize (nCount);

	CString s;
	for(int i=0;i<nCount;i++)
	{
		m_listExistFields.GetText (i,s);
		arraySel.SetAt (i,s);
	}

	// Remove all contents in the exist list:
	m_listExistFields.ResetContent ();

	// Add all contents to  the show list:
	for(i=0;i<nCount;i++)
	{
		m_listSummedFields.AddString (arraySel.GetAt (i));
	}
}

void CDlgSumField::OnBnClickedbuttonremovecurrent()
{
	// Get All Selected Strings:
	int nCount = m_listSummedFields.GetSelCount();
	
	int *pSel=new int[nCount+2];

	m_listSummedFields.GetSelItems(nCount, pSel);

	CStringArray arraySel;
	arraySel.SetSize (nCount);

	CString s;
	for(int i=0;i<nCount;i++)
	{
		m_listSummedFields.GetText (pSel[i],s);
		arraySel.SetAt (i,s);
	}

	// Delete All strings int the exist list:
	int iSel=0;
	for(i=0;i<nCount;i++)
	{
		iSel=m_listSummedFields.FindString(0,arraySel.GetAt (i));
		if(iSel!=LB_ERR)
		{
			m_listSummedFields.DeleteString (iSel);
		}
	}

	// Add All strings to  the show list:
	for(i=0;i<nCount;i++)
	{
		m_listExistFields.AddString (arraySel.GetAt (i));
	}

	delete []pSel;
}

void CDlgSumField::OnBnClickedbuttonremoveall()
{
	int nCount = m_listSummedFields.GetCount ();
	
	// Get All strings in the Exist List:
	CStringArray arraySel;
	arraySel.SetSize (nCount);

	CString s;
	for(int i=0;i<nCount;i++)
	{
		m_listSummedFields.GetText (i,s);
		arraySel.SetAt (i,s);
	}

	// Remove all contents in the summed list:
	m_listSummedFields.ResetContent ();

	for(i=0;i<nCount;i++)
	{
		m_listExistFields.AddString (arraySel.GetAt (i));
	}
}

void CDlgSumField::SetExistFields(CStringArray &arrayFields)
{
	m_arrayExistFields.Copy (arrayFields);
}

void CDlgSumField::SetAddedFields(CStringArray &arrayFields)
{
	m_arraySummedFields.Copy (arrayFields);
}

void CDlgSumField::GetSummedFields(CString &sSummedName, CStringArray &arraySummedFields)
{
	sSummedName=m_sSummedName;
	arraySummedFields.Copy(m_arraySummedFields);
	return;
}


BOOL CDlgSumField::OnInitDialog()
{
	CDialog::OnInitDialog();

	int i,j;
	/////////////////////////////////////////////
	// 加入数据表中存在的字段，
	// 如果该字段为显示字段，则不加入：
	/////////////////////////////////////////////
	CString sExistField,sShowField;
	int iShowFields=m_arraySummedFields.GetCount ();
	bool bShow=false;
	for(i=0;i<m_arrayExistFields.GetCount ();i++)
	{
		bShow=false;
		sExistField=m_arrayExistFields.GetAt(i);
		for(j=0;j<iShowFields;j++)
		{
			if(sExistField==m_arraySummedFields.GetAt (j))
			{
				bShow=true;
				break;
			}
		}
		if(!bShow)m_listExistFields.AddString (sExistField);
	}

	/////////////////////////////////////////////
	// 加入显示字段
	/////////////////////////////////////////////
	for(i=0;i<m_arraySummedFields.GetCount ();i++)
	{
		m_listSummedFields.AddString (m_arraySummedFields.GetAt(i));
	}

	/////////////////////////////////
	//
	/////////////////////////////////
	this->SetWindowText (m_sTitle);
	this->SetDlgItemText (IDC_staticResultName,m_sResultName);	
	this->SetDlgItemText (IDC_staticAddedFields,m_ResultFields);

	return TRUE;  // return TRUE unless you set the focus to a control
	// EXCEPTION: OCX Property Pages should return FALSE
}


void CDlgSumField::OnOK()
{
	int nCount = m_listSummedFields.GetCount ();
	
	// Get All strings in the Exist List:
	m_arraySummedFields.SetSize (nCount);

	CString s;
	for(int i=0;i<nCount;i++)
	{
		m_listSummedFields.GetText (i,s);
		m_arraySummedFields.SetAt (i,s);
	}

	UpdateData(true);
	if(m_sSummedName=="")
	{
		AfxMessageBox("错误：必须给出求和结果的名称!");
		return;
	}
	CDialog::OnOK();
}


void CDlgSumField::SetText(CString sTitle, 
						   CString sResultName, 
						   CString sResultFields)
{
	m_sTitle=sTitle;
	m_sResultName=sResultName;
	m_ResultFields=sResultFields;
}
