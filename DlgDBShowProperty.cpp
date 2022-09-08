// DBShowProperty.cpp : implementation file
//

#include "stdafx.h"
#include "QCDB.h"
#include "DlgDBShowProperty.h"
#include "DataBaseManager.h"
#include "QCGlobal.h"
#include "DlgSumField.h"

// CDlgDBShowProperty dialog

IMPLEMENT_DYNAMIC(CDlgDBShowProperty, CPropertyPage)
CDlgDBShowProperty::CDlgDBShowProperty()
	: CPropertyPage(CDlgDBShowProperty::IDD)
	, m_sTableName(_T(""))
	, m_sServer(_T(""))
{
	m_nShowQtyMin=1;
	m_nShowQtyMax=2000;
	m_lShowQtyMaxPermit=1000;
	m_sServer=GetLocalServerName();
}

CDlgDBShowProperty::~CDlgDBShowProperty()
{
}

void CDlgDBShowProperty::DoDataExchange(CDataExchange* pDX)
{
	CPropertyPage::DoDataExchange(pDX);
	DDX_Control(pDX, IDC_ComboTableName, m_comboTableName);
	DDX_CBString(pDX, IDC_ComboTableName, m_sTableName);
	DDX_Control(pDX, IDC_EditShowQtyPermit, m_editShowQtyMax);
	DDX_Control(pDX, IDC_ListExistFields, m_listExistFields);
	DDX_Control(pDX, IDC_ListShowFields, m_listShowFields);
	DDX_Text(pDX, IDC_EditShowQtyPermit, m_lShowQtyMaxPermit);
	DDX_Control(pDX, IDC_SpinShowQty, m_spinShowQty);
	DDX_Control(pDX, IDC_comboServer, m_comboServer);
	DDX_CBString(pDX, IDC_comboServer, m_sServer);
}


BEGIN_MESSAGE_MAP(CDlgDBShowProperty, CPropertyPage)
	ON_BN_CLICKED(IDC_buttonShowCurrent, OnBnClickedbuttonshowcurrent)
	ON_BN_CLICKED(IDC_buttonShowAll, OnBnClickedbuttonshowall)
	ON_BN_CLICKED(IDC_buttonRemoveCurrent, OnBnClickedbuttonremovecurrent)
	ON_BN_CLICKED(IDC_buttonRemoveAll, OnBnClickedbuttonremoveall)
	ON_NOTIFY(UDN_DELTAPOS, IDC_SpinShowQty, OnDeltaposSpinshowqty)
	ON_EN_CHANGE(IDC_EditShowQtyPermit, OnEnChangeEditShowQtyPermit)
	ON_CBN_SELCHANGE(IDC_ComboTableName, OnCbnSelchangeCombotablename)
	ON_BN_CLICKED(IDC_buttonSum, OnBnClickedbuttonsum)
	ON_BN_CLICKED(IDC_buttonTotalUnit, OnBnClickedbuttontotalunit)
	ON_BN_CLICKED(IDC_buttonTotalProject, OnBnClickedbuttontotalproject)
END_MESSAGE_MAP()


// CDlgDBShowProperty message handlers

void CDlgDBShowProperty::OnBnClickedbuttonshowcurrent()
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
		m_listShowFields.AddString (arraySel.GetAt (i));
	}

	delete []pSel;
}

void CDlgDBShowProperty::OnBnClickedbuttonshowall()
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
		m_listShowFields.AddString (arraySel.GetAt (i));
	}
}

void CDlgDBShowProperty::OnBnClickedbuttonremovecurrent()
{
	// Get All Selected Strings:
	int nCount = m_listShowFields.GetSelCount();
	
	int *pSel=new int[nCount+2];

	m_listShowFields.GetSelItems(nCount, pSel);

	CStringArray arraySel;
	arraySel.SetSize (nCount);

	CString s;
	for(int i=0;i<nCount;i++)
	{
		m_listShowFields.GetText (pSel[i],s);
		arraySel.SetAt (i,s);
	}

	// Delete All strings int the exist list:
	int iSel=0;
	for(i=0;i<nCount;i++)
	{
		iSel=m_listShowFields.FindString(0,arraySel.GetAt (i));
		if(iSel!=LB_ERR)
		{
			m_listShowFields.DeleteString (iSel);
		}
	}

	// Add All strings to  the show list:
	for(i=0;i<nCount;i++)
	{
		m_listExistFields.AddString (arraySel.GetAt (i));
	}

	delete []pSel;
}

void CDlgDBShowProperty::OnBnClickedbuttonremoveall()
{
	int nCount = m_listShowFields.GetCount ();
	
	// Get All strings in the Exist List:
	CStringArray arraySel;
	arraySel.SetSize (nCount);

	CString s;
	for(int i=0;i<nCount;i++)
	{
		m_listShowFields.GetText (i,s);
		arraySel.SetAt (i,s);
	}

	// Remove all contents in the exist list:
	m_listShowFields.ResetContent ();

	// Add all contents to  the show list:
	for(i=0;i<nCount;i++)
	{
		m_listExistFields.AddString (arraySel.GetAt (i));
	}
}

void CDlgDBShowProperty::OnDeltaposSpinshowqty(NMHDR *pNMHDR, LRESULT *pResult)
{
	LPNMUPDOWN pNMUpDown = reinterpret_cast<LPNMUPDOWN>(pNMHDR);
	// TODO: Add your control notification handler code here

	m_lShowQtyMaxPermit=pNMUpDown->iPos ;

	UpdateData(false);

	*pResult = 0;
}

BOOL CDlgDBShowProperty::OnInitDialog()
{
	CPropertyPage::OnInitDialog();

	int i=0,j=0,iQty=0;
	CServer servers[200];

	CStringArray arrayNames,arrayIPs, arrayPorts,arrayUsers,arrayPasswords;

	m_comboServer.AddString (GetLocalServerName());
	CString sIP,sPort;
	
	iQty=GetServers(servers,200);
	for(i=0;i<iQty;i++)
	{
		sIP=servers[i].sIP;
		sPort=vtos(servers[i].iPort);
		sIP.Trim ();
		sPort.Trim ();
		sIP=sIP+":"+sPort;
		m_comboServer.AddString (sIP);
	}

	/////////////////////////////////////////////
	// 设定最多显示的记录数目：
	/////////////////////////////////////////////
	m_spinShowQty.SetRange (m_nShowQtyMin,m_nShowQtyMax);
	m_spinShowQty.SetPos (m_lShowQtyMaxPermit);

	/////////////////////////////////////////////
	// 加入数据库中存在的表：
	/////////////////////////////////////////////
	for(i=0;i<m_arrayTable.GetCount ();i++)
	{
		m_comboTableName.AddString (m_arrayTable.GetAt (i));
	}

	/////////////////////////////////////////////
	// 加入数据表中存在的字段，
	// 如果该字段为显示字段，则不加入：
	/////////////////////////////////////////////
	CString sExistField,sShowField;
	int iShowFields=m_listShowFields.GetCount ();
	bool bShow=false;
	for(i=0;i<m_arrayExistFields.GetCount ();i++)
	{
		bShow=false;
		sExistField=m_arrayExistFields.GetAt(i);
		for(j=0;j<iShowFields;j++)
		{
			if(sExistField==m_arrayShowFields.GetAt (j))
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
	for(i=0;i<m_arrayShowFields.GetCount ();i++)
	{
		m_listShowFields.AddString (m_arrayShowFields.GetAt(i));
	}

	/////////////////////////////////////////////
	// 加入显示数据表:
	/////////////////////////////////////////////
	if(m_sTableName==""&&m_arrayTable.GetCount ()>0)
	{
		m_sTableName=m_arrayTable.GetAt (0);
	}

	return TRUE;  
}

void CDlgDBShowProperty::SetTableNames(CStringArray &arrayTableNames)
{
	m_arrayTable.Copy (arrayTableNames);
}

void  CDlgDBShowProperty::SetFieldNames(CStringArray &arrayFieldNames)
{
	m_arrayExistFields.Copy (arrayFieldNames);
}

void  CDlgDBShowProperty::SetShowFieldNames(CStringArray &arrayShowFieldNames)
{
	m_arrayShowFields.Copy (arrayShowFieldNames);
}

void CDlgDBShowProperty::SetShowTable(CString sShowTable)
{
	m_sTableName=sShowTable;
}


void CDlgDBShowProperty::OnEnChangeEditShowQtyPermit()
{
	UpdateData(true);
	m_spinShowQty.SetPos (m_lShowQtyMaxPermit);
}

void CDlgDBShowProperty::OnCbnSelchangeCombotablename()
{	
	int iSel=m_comboTableName.GetCurSel();
	if(iSel!=CB_ERR)
	{
		m_comboTableName.GetLBText (iSel,m_sTableName);
	}

	CStringArray arrayFields,arrayTypes;
	GetFieldNames(					m_sTableName,
									arrayFields,
									arrayTypes);

	m_listExistFields.ResetContent ();
	m_listShowFields.ResetContent();

	int iQty=arrayFields.GetCount ();

	for(int i=0;i<iQty;i++)
	{
		m_listExistFields.AddString (arrayFields.GetAt (i));
	}
}
void CDlgDBShowProperty::SetShowQtyMaxPermit(int iQty)
{
	m_lShowQtyMaxPermit=iQty;
}

LRESULT CDlgDBShowProperty::OnWizardNext()
{
	CString sField;
	m_arrayShowFields.SetSize (m_listShowFields.GetCount ());

	for(int i=0;i<m_listShowFields.GetCount ();i++)
	{
		m_listShowFields.GetText (i,sField);
		m_arrayShowFields.SetAt (i,sField);
	}

	return CPropertyPage::OnWizardNext();
}

BOOL CDlgDBShowProperty::OnSetActive()
{
	// TODO: Add your specialized code here and/or call the base class
	CPropertySheet* psheet = (CPropertySheet*) GetParent();   
	psheet->SetWizardButtons(PSWIZB_NEXT);


	return CPropertyPage::OnSetActive();
}

void CDlgDBShowProperty::SetUser(CString sUserID, CString sPassword)
{
	m_sUserID=sUserID;
	m_sPassword=sPassword;
}

void CDlgDBShowProperty::OnBnClickedbuttonsum()
{
	CDlgSumField dlg;
	CStringArray array;
    array.Copy(m_arrayShowFields );
	array.Append (m_arrayExistFields);
	dlg.SetExistFields (array);
	if(dlg.DoModal ()==IDCANCEL)return;

	CString sSummedString;
	CStringArray arraySummedFields;
	dlg.GetSummedFields (sSummedString, arraySummedFields);

	CString s=sSummedString+"=SUM(";
	int iCount=arraySummedFields.GetCount ();
	for(int i=0;i<iCount;i++)
	{
		s+=arraySummedFields.GetAt (i);
		if(i!=iCount-1)
		{
			s+=",";
		}
	}
	s+=")";

	m_listShowFields.AddString (s);
}

void CDlgDBShowProperty::AddTotal(bool bProjectSum)
{
	CDlgSumField dlg;
	CStringArray array,arrayTypes;
	CString sField,sFieldExist;
	bool bExist=false;
	int i,j;

	
	GetFieldNames(	m_sTableName,
					array,
					arrayTypes);

	if(bProjectSum)
	{
		dlg.SetText("项目累计","项目累计名称","项目累计字段");
	}
	else
	{
		dlg.SetText("单元累计","单元累计名称","单元累计字段");
	}

	// Do Modal
	dlg.SetExistFields (array);
	if(dlg.DoModal ()==IDCANCEL)return;

	// Return Value:
	CString sResultString;
	CStringArray arrayValidField;
	dlg.GetSummedFields (sResultString, arrayValidField);
	if(arrayValidField.GetCount ()==0)return;

	CString s=sResultString;
	
	if(bProjectSum)
	{
		s+="=TOTAL_PROJECT(";
	}
	else
	{
		s+="=TOTAL_UNIT(";
	}

	CString sIn="";
	CString sValidField="";
	int n=0;

	for(i=0;i<arrayValidField.GetCount ();i++)
	{
		sValidField=arrayValidField.GetAt (i);
		sValidField.MakeUpper ();
		n=sValidField.Find ("=SUM");
		if(n!=-1)
		{
			sIn=sValidField.Left (n);
		}
		else
		{
			n=sValidField.Find ("=TOTAL");
			if(n!=-1)
			{
				sIn="";
			}
			else
			{
				sIn=sValidField;
			}
		}

		if(sIn!="")
		{
            if(s.Right (1)!="(")
			{
				s+=",";
			}
			s+=sIn;
		}
	}

	s+=")";

	m_listShowFields.AddString (s);
}

void CDlgDBShowProperty::OnBnClickedbuttontotalunit()
{
	AddTotal(false);
}

void CDlgDBShowProperty::OnBnClickedbuttontotalproject()
{
	AddTotal(true);
}
