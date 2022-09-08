// DlgInputServer.cpp : implementation file
//

#include "stdafx.h"
#include "QCDB.h"
#include "DlgInputServer.h"
#include ".\dlginputserver.h"
#include "FHCGlobal.h"
#include "QCGlobal.h"

// CDlgInputServer dialog

IMPLEMENT_DYNAMIC(CDlgInputServer, CPropertyPage)
CDlgInputServer::CDlgInputServer()
	: CPropertyPage(CDlgInputServer::IDD)
	, m_sServerName(_T(""))	
	, m_sPort(_T(""))
	, m_sServerIP(_T(""))
	, m_sUserName(_T(""))
	, m_sPassword(_T(""))
{
	CStringArray array;
	array.Add ("服务器名");
	array.Add ("IP");
	array.Add ("Port");
	array.Add ("用户名");
	array.Add ("口令");

	m_listServerProperty.SetColName (array);
}

CDlgInputServer::~CDlgInputServer()
{
}

void CDlgInputServer::DoDataExchange(CDataExchange* pDX)
{
	CPropertyPage::DoDataExchange(pDX);
	DDX_Text(pDX, IDC_editServerName, m_sServerName);
	DDX_Text(pDX, IDC_editServerPort, m_sPort);
	DDX_Control(pDX, IDC_listServerProperty, m_listServerProperty);
	DDX_Control(pDX, IDC_editServerIP, m_editServerIP);
	DDX_Text(pDX, IDC_editServerIP, m_sServerIP);
	DDX_Text(pDX, IDC_editUserName, m_sUserName);
	DDX_Text(pDX, IDC_editPassword, m_sPassword);
}


BEGIN_MESSAGE_MAP(CDlgInputServer, CPropertyPage)
	ON_BN_CLICKED(IDC_buttonAdd, OnBnClickedbuttonadd)
	ON_BN_CLICKED(IDC_buttonRemove, OnBnClickedbuttonremove)
END_MESSAGE_MAP()


// CDlgInputServer message handlers

void CDlgInputServer::OnBnClickedbuttonadd()
{
	UpdateData(true);
	CStringListItem *pList=new CStringListItem ;

	pList->AppendItem (m_sServerName.GetBuffer ());
	pList->AppendItem (m_sServerIP.GetBuffer ());
	pList->AppendItem (m_sPort.GetBuffer ());
	pList->AppendItem (m_sUserName.GetBuffer ());

	CString sPassword;
	sPassword=m_sPassword;
	for(int i=0;i<sPassword.GetLength ();i++)
	{
		sPassword.SetAt (i,'*');
	}
	
	pList->AppendItem (sPassword.GetBuffer ());
	pList->AppendItem (m_sPassword.GetBuffer ());
    pList->AppendItem ("weqrqwe");


	m_listServerProperty.AppendRowItem (pList);
	m_listServerProperty.AutoSize ();
}

void CDlgInputServer::OnBnClickedbuttonremove()
{
	m_listServerProperty.DeleteSelected ();
}

void CDlgInputServer::GetServerProperty(CStringArray &arrayNames,
										CStringArray &arrayIPs,
										CStringArray &arrayPorts,
										CStringArray &arrayUserName,
										CStringArray &arrayPassword)
{
	arrayNames.Copy( m_arrayNames);
	arrayIPs.Copy (m_arrayIPs);
	arrayPorts.Copy (m_arrayPorts);
	arrayUserName.Copy (arrayUserName);
	arrayPassword.Copy (arrayPassword);
}

void CDlgInputServer::SaveServerProperty()
{
	CStringListItem *pList=NULL;
	char *pItem=NULL;
	m_arrayNames.RemoveAll ();
	m_arrayIPs.RemoveAll ();
	m_arrayPorts.RemoveAll ();
	m_arrayUserNames.RemoveAll ();
	m_arrayPasswords.RemoveAll ();

	for(int i=0;i<m_listServerProperty.GetItemCount ();i++)
	{
		pList = (CStringListItem *)m_listServerProperty.GetItemData (i);

		pItem=pList->GetAt (0);
		m_arrayNames.Add (pItem);
		pItem=pList->GetAt (1);
		m_arrayIPs.Add(pItem);
		pItem=pList->GetAt (2);
		m_arrayPorts.Add(pItem);
		pItem=pList->GetAt (3);
		m_arrayUserNames.Add(pItem);

		// 4 : is *******
		pItem=pList->GetAt (5);
		m_arrayPasswords.Add(pItem);
	}



	RemoveServers();
	for(i=0;i<m_arrayNames.GetCount ();i++)
	{
		AddServer(	m_arrayNames.GetAt (i),
					m_arrayIPs.GetAt (i),
					m_arrayPorts.GetAt (i),
					m_arrayUserNames.GetAt (i),
					m_arrayPasswords.GetAt (i));
	}

	return ;
}




void CDlgInputServer::OnOK()
{
	// TODO: Add your specialized code here and/or call the base class
	SaveServerProperty();
	CPropertyPage::OnOK();
}

BOOL CDlgInputServer::OnInitDialog()
{
	CPropertyPage::OnInitDialog();
	
	m_listServerProperty.SetFullRowSel (true);
	m_listServerProperty.SetGridLines (true);
	m_listServerProperty.SetIncCol(false);

	CServer servers[200];
	int iServerQty=GetServers(servers,200);

	CStringListItem  *pItem=NULL;
	CString sPassword;
	int i,j;
	char s[100];
	for(i=0;i<iServerQty;i++)
	{
		pItem=new CStringListItem;
		pItem->AppendItem (servers[i].sName );
		pItem->AppendItem (servers[i].sIP);
		sprintf(s,"%d",servers[i].iPort);
		pItem->AppendItem (s);
		pItem->AppendItem (servers[i].sUserName );

		sPassword=servers[i].sPassword ;
		for(j=0;j<sPassword.GetLength ();j++)
		{
			sPassword.SetAt (j,'*');
		}

		pItem->AppendItem (sPassword.GetBuffer());

        pItem->AppendItem (servers[i].sPassword);

		m_listServerProperty.AppendRowItem (pItem);
	}

	m_listServerProperty.AutoSize ();

	
	return TRUE;  // return TRUE unless you set the focus to a control
	// EXCEPTION: OCX Property Pages should return FALSE
}

BOOL CDlgInputServer::Create(LPCTSTR lpszTemplateName, CWnd* pParentWnd)
{
	// TODO: Add your specialized code here and/or call the base class

	return CPropertyPage::Create(lpszTemplateName, pParentWnd);
}
