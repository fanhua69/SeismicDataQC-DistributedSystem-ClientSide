#include "StdAfx.h"
#include "datatransfer.h"
#include "transocket.h"
#include "fhmainfrm.h"
#include "FHApp.h"
#include "QCGlobal.h"
#include "StringExtractor.h"
#include "MainFrm.h"

CDataTransfer::CDataTransfer(void)
{
	m_bServer				=true;

	m_iServerQty			=0;
	m_bListening			=false;
	m_bOtherServerConnected	=false;
	m_pListeningSocket		=NULL;

	m_sLocalIP				="";
	m_iLocalPort			=0;


	if(m_db.Initiate (GetDBName()))
    {
	    GetFieldNames (	GetHistoryTableName (),
					    m_arrayHisFieldNames,
					    m_arrayHisFieldTypes);
    }
}

CDataTransfer::~CDataTransfer(void)
{
	if(m_pListeningSocket)
	{
		delete m_pListeningSocket;
		m_pListeningSocket=NULL;
	}
	DisConnectServers();
	DisconnectChildren();

}

void CDataTransfer::SetLocalIP(CString sIP)
{
	m_sLocalIP=sIP;
}

void CDataTransfer::SetLocalPort(int iPort)
{
	m_iLocalPort=iPort;
}

/////////////////////////////////////////
// Accept the callling client:
// save the socket into list of children:
/////////////////////////////////////////
bool CDataTransfer::ProcessPendingAccept()
{
	return ConnectChild();
}

bool CDataTransfer::ConnectChild()
{
	CConnection *pConn=new CConnection;
	if(!pConn->Accept(m_pListeningSocket))
	{
        TRACEERROR("无法连接客户端!");
		delete pConn;
		return false;
	}
	else
	{
		pConn->SetDataTransfer (this);
		pConn->SetUserVerified();
		m_ChildrenList.AddTail(pConn);

		AddLinker(pConn,false);

        //
		CString sIP;
		UINT iPort;
        pConn->GetLocalIP (sIP,iPort);
        SetLocalIP(sIP);
        SetLocalPort(iPort);

        //
        pConn->GetPeerIP(sIP,iPort);
        TRACEPROGRESS("已经连接上客户端:"+sIP+",端口:"+vtos((int)iPort));

        //
		return true;
	}
}

bool CDataTransfer::AddLinker(CConnection *pConn,bool bServer)
{
	CMainFrame * pMain=(CMainFrame * )AfxGetMainWnd();
	CStringArray array;
	CString sIP="";
	UINT iPort=0;
	pConn->GetPeerIP (sIP,iPort);
	sIP=CombineIP(sIP,iPort);
	array.Add ("连接中...");
	array.Add (sIP);
	if(bServer)
	{
		array.Add ("服务器");
	}
	else
	{
		array.Add ("客户端");
	}
	array.Add (GetCurrTime());
	array.Add (" ");
	array.Add ("0");
	array.Add ("0");


	pMain->m_wndDataLinker .AddRow (array);
	return true;
}

bool CDataTransfer::RemoveLinker(CConnection *pConn)
{
	CMainFrame * pMain=(CMainFrame * )AfxGetMainWnd();
	if(!pMain)return false;

	CStringArray array;
	CString sIP="";
	UINT iPort=0;
	pConn->GetPeerIP (sIP,iPort);
	sIP=CombineIP(sIP,iPort);

	pMain->m_wndDataLinker .DeleteRow (1,sIP);
	return true;
}

bool CDataTransfer::EndLinker(CConnection *pConn)
{
	CMainFrame * pMain=(CMainFrame * )AfxGetMainWnd();
	CStringArray array;
	CString sIP="";
	UINT iPort=0;
	pConn->GetPeerIP (sIP,iPort);
	sIP=CombineIP(sIP,iPort);

	pMain->m_wndDataLinker.ModifyItem (1,sIP,4,GetCurrTime());
	pMain->m_wndDataLinker.ModifyItem (1,sIP,0,"已断开");
	return true;
}

bool CDataTransfer::ModifyInFlow(CConnection *pConn,double lfKiloBytes)
{
	CMainFrame * pMain=(CMainFrame * )AfxGetMainWnd();
	CStringArray array;
	CString sIP="";
	UINT iPort=0;
	pConn->GetPeerIP (sIP,iPort);
	sIP=CombineIP(sIP,iPort);

	pMain->m_wndDataLinker.ModifyItem (1,sIP,6,vtos(lfKiloBytes)+" KB");
	return true;
}

bool CDataTransfer::ModifyOutFlow(CConnection *pConn,double lfKiloBytes)
{
	CMainFrame * pMain=(CMainFrame * )AfxGetMainWnd();
	CStringArray array;
	CString sIP="";
	UINT iPort=0;
	pConn->GetPeerIP (sIP,iPort);
	sIP=CombineIP(sIP,iPort);

	pMain->m_wndDataLinker.ModifyItem (1,sIP,7,vtos(lfKiloBytes)+" KB");
	return true;
}

bool CDataTransfer::OtherServerConnected()
{
	return m_bOtherServerConnected;
}

bool CDataTransfer::ConnectServers(bool bServer)
{
	GetServers();

	if(m_iServerQty==0)
	{
		TRACEERROR("警告:没有指定服务器!");
		m_bOtherServerConnected=true;
		return true;
	}

	// Empty the father lists:
	DisConnectServers();
	m_bOtherServerConnected=true;

	// Connect other servers:
    CString sIP;
    UINT iPort;

	for(int i=0;i<m_iServerQty;i++)
	{
		if(m_Servers[i]==m_LocalServer)continue;
		CConnection *pConn=new CConnection;
		if(pConn->ConnectServer (m_Servers[i]))
		{
            //
            pConn->GetLocalIP (sIP,iPort);
            SetLocalIP(sIP);
            SetLocalPort(iPort);

            //
			pConn->SetDataTransfer (this);
			m_FathersList.AddTail(pConn);
			AddLinker(pConn,true);
		
            TRACEPROGRESS("已经连接到服务器:"
                +CString(m_Servers[i].sName)
                +"IP:"+CString(m_Servers[i].sIP)
                +"Port:"+vtos(m_Servers[i].iPort));
		}
		else
		{
			m_bOtherServerConnected=false;
			delete pConn;
			break;
		}
	}

	UpdateView();

	return m_bOtherServerConnected;
}

bool CDataTransfer::DisConnectServer(CConnection *pConnTag)
{
	POSITION posLast=0;
	POSITION pos=m_FathersList.GetHeadPosition ();
	while(pos)
	{
		posLast=pos;
		CConnection *pConn=(CConnection *)m_FathersList.GetNext (pos);
		if(pConn ==pConnTag)
		{
			m_FathersList.RemoveAt (posLast);
			
			EndLinker(pConn);

			delete pConn;
			return  true;
		}
	}	

	return false;
}

bool CDataTransfer::DisConnectServers()
{
	POSITION pos=m_FathersList.GetHeadPosition ();
	while(pos)
	{
		CConnection *pConn=(CConnection *)m_FathersList.GetNext (pos);
		RemoveLinker(pConn);

		delete pConn;
	}
	
	m_FathersList.RemoveAll ();

	m_bOtherServerConnected=false;

	UpdateView();
	return true;
}

bool CDataTransfer::DisconnectChildren()
{
	POSITION pos=m_ChildrenList.GetHeadPosition ();
	while(pos)
	{
		CConnection *pConn=(CConnection *)m_ChildrenList.GetNext (pos);
		RemoveLinker(pConn);

		delete pConn;
	}
	
	m_ChildrenList.RemoveAll ();

	UpdateView();

	return true;
}


bool CDataTransfer::DisconnectChild(CConnection *pConnTag)
{
	POSITION posLast=0;
	POSITION pos=m_ChildrenList.GetHeadPosition ();
	while(pos)
	{
		posLast=pos;
		CConnection *pConn=(CConnection *)m_ChildrenList.GetNext (pos);
		if(pConn ==pConnTag)
		{
			m_ChildrenList.RemoveAt (posLast);
			
			EndLinker(pConn);

			delete pConn;

			return true;
		}
	}	


	return false;
}

void CDataTransfer::RemoveAllServers()
{
	m_iServerQty=0;
}

bool CDataTransfer::AddServer(CServer &server)
{
	return AddServer(server.sName ,
		server.sIP ,
		server.iPort ,
		server.sUserName ,
		server.sPassword );
}


bool CDataTransfer::AddServer(CString sServerName,
							  CString sIP,
							  int iPort,
							  CString sUserName,
							  CString sPassword)
{
	if(m_iServerQty>=128)
	{
		return false;
	}

	if(m_Servers[m_iServerQty].Set(sServerName,sIP,iPort,sUserName,sPassword))
	{
		m_iServerQty++;
		return true;
	}

	return false;
}

bool CDataTransfer::GetServers()
{
	RemoveAllServers();

	m_iServerQty=::GetServers(m_Servers,128);

    return true;
}

bool CDataTransfer::ListenClients()
{
	m_bListening=false;

	int iPort=0;
	GetPar("GLOBAL_LOCAL_PORT",iPort);
	if(iPort==0)
	{
		iPort=700;
	}

	if(m_pListeningSocket)
	{
		m_pListeningSocket->Close ();
		delete m_pListeningSocket;
		m_pListeningSocket=NULL;
	}

	m_pListeningSocket=new CTranSocket;
	m_pListeningSocket->SetTransfer (this);
	if(!m_pListeningSocket->Create(iPort))
	{
        TRACEERROR("无法创建监听Socket,在端口:"+vtos(iPort));
		return false;
	}

	if(!m_pListeningSocket->Listen())
	{
        TRACEERROR("Socket无法实现监听Socket!");
		return false;
	}

    TRACEPROGRESS("Socket监听成功,在端口:"+vtos(iPort));
		
	m_bListening=true;
	return true;
}

void CDataTransfer::UpdateView()
{
}

bool CDataTransfer::Initiate(bool bServer)
{
	m_bServer=bServer;
	m_bInitiated=false;

	if(bServer)
	{
		//  判断是否需要连接其他服务器:
		if(m_iServerQty==0)
		{
			m_bOtherServerConnected=true;
		}

		// 监听：
		if(!ListenClients())
		{
			TRACEERROR("网络监听无法初始化!");
			m_bInitiated=false;
		}
		else
		{
			TRACEPROGRESS("网络监听已经初始化!");
			m_bInitiated=true;
		}
	}
	else
	{
		m_bInitiated=true;
	}


	return m_bInitiated;
}

bool CDataTransfer::SendDataToServers(CString sDBFile,
									  CConnection *pConnTag)
{
	// 增加数据到其他服务器数据库:
	POSITION pos=m_FathersList.GetHeadPosition ();
	if(!pos)
	{
		TRACEERROR("错误:尚未连接服务器!");
		return false;
	}

	CStringArray arrayPicFiles;
	CheckOutPicFiles(sDBFile,arrayPicFiles);

	int i=0;
	CString sTargetIP="";
	UINT	iTargetPort=0;

	while(pos)
	{
		CConnection  *pConn=(CConnection  *)m_FathersList.GetNext (pos);
		if(pConn==pConnTag)continue;
		pConn->GetPeerIP (sTargetIP,iTargetPort);
		TRACEPROGRESS("恭喜：正在向服务器发送数据:"+CString(pConn->m_sName ));
		
		// Hand In the Pic files firstly:
		for(i=0;i<arrayPicFiles.GetCount ();i++)
		{
			pConn->SendFile     (   arrayPicFiles.GetAt (i),
								    sTargetIP,
								    iTargetPort,
								    INFO_HAND_IN);
		}

		// Hand In the common records: 
		if(pConn->SendFile (sDBFile,sTargetIP,iTargetPort,INFO_HAND_IN))
		{
			TRACEPROGRESS("恭喜：向服务器发送数据完毕:"+sTargetIP+":"+vtos((int)iTargetPort));
		}
	}

	return true;
}

bool CDataTransfer::SendDataToServers(CConnection *pConnTag)
{
	// 增加数据到其他服务器数据库:
	POSITION pos=m_FathersList.GetHeadPosition ();
	while(pos)
	{
		CConnection  *pConn=(CConnection  *)m_FathersList.GetNext (pos);
		if(pConn==pConnTag)continue;
		pConn->CopyBuffer (pConnTag);
		pConn->SendData();
	}

	return true;
}

bool CDataTransfer::SendDataToApplier(CConnection *pConn)
{
	POSITION pos=m_ChildrenList.GetHeadPosition ();
	while(pos)
	{
		CConnection *pConnChild=(CConnection *)m_ChildrenList.GetNext (pos);
		if(strcmp(pConnChild->m_sIP ,pConn->m_sIP )==0)
		{
			pConnChild->CopyBuffer (pConn);
			pConnChild->SendData ();
			return true;
		}
	}
	return false;
}
/*
bool CDataTransfer::GetDataFromServer(CString			sServer, 
										   UINT				iPort,
										   CString			sTableName,
										   CString			sSQL,
										   CStringArray		*pArrayFields,
										   CDataBrowseDoc	*pDoc,
										   int				iMaxHandDownRecord)
{
	if(sServer=="")return false;

	//
	int n=sServer.Find (':');
	if(n!=-1)
	{
		sServer=sServer.Left (n);
	}

	POSITION pos=m_FathersList.GetHeadPosition ();
	CConnection *pTargetServer=NULL;
	CConnection *pConnFather=NULL;
	CString sPeerIP;
	UINT iPeerPort=0;
	while(pos)
	{
		CConnection  *pConnFather=(CConnection  *)m_FathersList.GetNext (pos);
		pConnFather->GetPeerIP (sPeerIP,iPeerPort);

		if(sPeerIP== sServer&& iPort==iPeerPort)
		{
			pTargetServer=pConnFather;
			break;
		}
	}

	// 直接连任意一个服务器:
	if(pTargetServer==NULL&&pConnFather!=NULL)
	{
		pTargetServer=pConnFather;
	}
	
	if(pTargetServer)
	{
		pTargetServer->WantData(sServer,
								iPort,
								sTableName,
								sSQL,
								pArrayFields,
								pDoc,
								iMaxHandDownRecord);
		return true;
	}

	return false;
}

bool CDataTransfer::GetDataFromServer(CConnection *pConn)
{
	CDataInfo *pInfo=pConn->GetInfoBuffer ();
	CString sServerIP=pInfo->sTargetIP;
	UINT iServerPort = pInfo->iTargetPort ;
	if(sServerIP==""||iServerPort==0)
	{
		ASSERT(false);
		return false;
	}

	//
	POSITION pos=m_FathersList.GetHeadPosition ();
	CString sIPTag;
	UINT	iPortTag=0;
	while(pos)
	{
		CConnection  *pConnFather=(CConnection  *)m_FathersList.GetNext (pos);
		pConnFather->GetPeerIP (sIPTag,iPortTag);
		if(sIPTag == sServerIP && iPortTag== iServerPort)
		{
			pConnFather->CopyBuffer (pConn);
			pConnFather->SendData ();
			return true;
		}
	}

	return false;
}

bool CDataTransfer::StartReceiveData(CDataBrowseDoc	*pTarBrowseDoc)
{
	CMultiDocTemplate *pTemplate=GetTemplate("DBBrowse");
	if(!pTemplate)
	{
		TRACEERROR("错误：没有加载数据模版!");
		return false;
	}

	POSITION pos=pTemplate->GetFirstDocPosition ();
	while(pos)
	{
		CDataBrowseDoc *pDoc=(CDataBrowseDoc *)(pTemplate->GetNextDoc (pos));
		if(pDoc==pTarBrowseDoc)
		{
			pDoc->CreateDBFile ();
			break;
		}
	}
	
	return true;
}


bool CDataTransfer::ReceiveRecord(CConnection *pConn)
{
	CDataInfo *pInfo=pConn->GetInfoBuffer ();
	CDataBrowseDoc *pTarBrowseDoc=pInfo->pBrowseDoc ;

	CMultiDocTemplate *pTemplate=GetTemplate("DBBrowse");
	if(!pTemplate)
	{
		TRACEERROR("错误：没有加载数据模版!");
		return false;
	}

	POSITION pos=pTemplate->GetFirstDocPosition ();
	CDataBrowseDoc *pTmpDoc=NULL;
	bool  bFound=false;
	while(pos)
	{
		pTmpDoc=(CDataBrowseDoc *)(pTemplate->GetNextDoc (pos));
		if(pTmpDoc==pTarBrowseDoc)
		{
			CStringArray array;
			pConn->GetSerItem(pConn->GetDataBuffer(),array);

			pTmpDoc->AppendRecord (&array);
			bFound=true;
			break;
		}
	}

	return bFound;
}

*/
/*
CConnection *  CDataTransfer:: GetConnection(CTranSocket *pSocket)
{
	POSITION pos=m_FatherList->GetHeadPosition();
	CConnection *pConn=NULL;
	while(pos)
	{
		pConn=(CConnection *)m_FatherList->GetNext(pos);
		if(pConn->pSocket == pSocket)
		{
			return pConn;
		}
	}

	pos=m_ChildrenList->GetHeadPosition();
	while(pos)
	{
		pConn=(CConnection *)m_ChildrenList->GetNext(pos);
		if(pConn->pSocket == pSocket)
		{
			return pConn;
		}
	}
	
	return NULL;
}
*/

bool CDataTransfer::GetData(	CString			sServer,
								UINT			iPort,
								CString			sTable,
								CString			sSQL,
								CTableDate		dateStart,
								CStringArray	*pArrayFields,
								CDataBrowseDoc  *pDoc,
								int				iMaxTransferRecord,
								bool			bMarkHandedIn)
{
	sServer.MakeUpper ();
	if(IsLocalServer(sServer,iPort))
	{
		CDataBaseManager db;
		return db.SendTableToBrowseDoc(sTable,sSQL,dateStart,pArrayFields,pDoc,iMaxTransferRecord,bMarkHandedIn);
	}

	CConnection *pConnTar=GetFatherConnection(sServer,iPort);

	if(pConnTar)
	{
		pConnTar->WantData (sServer,
							iPort,
							sTable,
							sSQL,
							dateStart,
							pArrayFields,							
							pDoc,
							iMaxTransferRecord);
	}
	else
	{
		TRACEERROR("没有可用的服务器!");
	}

	return true;
}

bool CDataTransfer::TransferData(CConnection *pConn)
{
	CDataInfo *pInfo=pConn->GetInfoBuffer ();
	
	CConnection *pConnNextTar=NULL;
	CString sIPTmp;
	UINT iPortTmp=0;

	if(pInfo->iInfoDir == INFO_HAND_DOWN)
	{
		pConnNextTar=GetChildConnection(pInfo->sTargetIP,pInfo->iTargetPort);
	}
	else
	{
		pConnNextTar=GetFatherConnection(pInfo->sTargetIP,pInfo->iTargetPort);
	}

	if(pConnNextTar!=NULL)
	{
		pConnNextTar->CopyBuffer (pConn);
		pConnNextTar->SendData ();
	}
	else
	{
		char sInfo[1000];
		sprintf(sInfo,"没有可用的服务器可以上传数据，请检查服务器设置或者用户的参数设置。所要目标:%s:%d",pInfo->sTargetIP,pInfo->iTargetPort);
		TRACEERROR(sInfo);
	}

	return true;
}

bool CDataTransfer::DeleteRecord(CString sDBFile)
{
	CDataBrowseDoc doc;
	if(!doc.OpenDBFile (sDBFile))return false;
	CString sServerIP=doc.GetSourceIP();
	int iServerPort=doc.GetSourcePort ();
	doc.Close ();

	if(IsLocalServer(sServerIP,iServerPort))
	{
		m_db.DeleteRecord (sDBFile);
	}
	
	else
	{
		CConnection *pConn=GetFatherConnection(sServerIP, iServerPort);
		if(pConn!=NULL)
		{
			pConn->SendFile (		sDBFile,
										sServerIP,
										iServerPort,
										INFO_HAND_IN);
		}
	}

	return true;
}

bool CDataTransfer::UpdateRecord(CString sDBFile)
{
	CDataBrowseDoc doc;
	if(!doc.OpenDBFile (sDBFile))return false;
	CString sServerIP=doc.GetSourceIP();
	int iServerPort=doc.GetSourcePort ();
	doc.Close ();

	int i;
	if(IsLocalServer(sServerIP,iServerPort))
	{
		return m_db.UpdateRecord(sDBFile);
	}
	
	else
	{
		CStringArray arrayPicFiles;
		CheckOutPicFiles(sDBFile,arrayPicFiles);

		CConnection *pConn=GetFatherConnection(sServerIP, iServerPort);
		if(pConn!=NULL)
		{
			// Hand In the Pic files firstly:
			for(i=0;i<arrayPicFiles.GetCount ();i++)
			{
				pConn->SendFile (arrayPicFiles.GetAt (i),
									sServerIP,
									iServerPort,
									INFO_HAND_IN);
			}

			// Hand In the Data files Secondly:
			pConn->SendFile (sDBFile,
								sServerIP,
								iServerPort,
								INFO_HAND_IN);
		}
	}
	return true;
}

bool CDataTransfer::IsLocalServer(CString sServerIP,int iPort)
{
    //
	sServerIP.MakeUpper ();
	if(   sServerIP==""
		||sServerIP=="本地服务器"
		||sServerIP.Left(5)=="LOCAL"
		||sServerIP.Left(4)=="本地"
		||(sServerIP==m_sLocalIP&&iPort==m_iLocalPort))
	{
		return true;
	}
	else
	{
		return false;
	}
}

bool CDataTransfer::AddRecord(CString sDBFile)
{

	CDataBrowseDoc doc;
	if(!doc.OpenDBFile (sDBFile))return false;
	CString sServerIP=doc.GetSourceIP();
	int iServerPort=doc.GetSourcePort ();
	doc.Close ();

	int i,j;

	if(IsLocalServer(sServerIP,iServerPort))
	{
		m_db.AddRecord (sDBFile);
	}
	
	else
	{
		CStringArray arrayPicFiles;
		CheckOutPicFiles(sDBFile,arrayPicFiles);

		CConnection *pConn=GetFatherConnection(sServerIP, iServerPort);
		if(pConn!=NULL)
		{
			// Hand In the Pic files firstly:
			for(i=0;i<arrayPicFiles.GetCount ();i++)
			{
				pConn->SendFile (arrayPicFiles.GetAt (i),
									sServerIP,
									iServerPort,
									INFO_HAND_IN);
			}

			//  And then hand in the databrowse doc:
			pConn->SendFile (sDBFile,
								sServerIP,
								iServerPort,
								INFO_HAND_IN);
		}
	}

	return true;
}

CConnection * CDataTransfer::GetFatherConnection(CString sServer, int iPort)
{
	CConnection *pConnTar=NULL;
	CConnection *pConnTmp=NULL;
	POSITION pos=m_FathersList.GetHeadPosition ();
	CString sIPTag="";
	UINT iPortTag=0;
	while(pos)
	{
		pConnTmp=(CConnection*)m_FathersList.GetNext (pos);
		pConnTmp->GetPeerIP (sIPTag,iPortTag);
		if(sIPTag==sServer&&iPortTag==iPort)
		{
			pConnTar=pConnTmp;
			break;
		}
	}
	if(pConnTar==NULL)
	{
		pConnTar=pConnTmp;
	}
	return pConnTar;
}

CConnection * CDataTransfer::GetChildConnection(CString sServer, int iPort)
{
	CConnection *pConnTar=NULL;
	CConnection *pConnTmp=NULL;
	POSITION pos=m_ChildrenList.GetHeadPosition ();
	CString sIPTag="";
	UINT iPortTag=0;
	while(pos)
	{
		pConnTmp=(CConnection*)m_FathersList.GetNext (pos);
		pConnTmp->GetPeerIP (sIPTag,iPortTag);
		if(sIPTag==sServer&&iPortTag==iPort)
		{
			pConnTar=pConnTmp;
			break;
		}
	}
	if(pConnTar==NULL)
	{
		pConnTar=pConnTmp;
	}
	return pConnTar;
}

void CDataTransfer::RecordHistory(CStringArray &array)
{
	CMainFrame *pMain=(CMainFrame *)AfxGetMainWnd();
	pMain->m_wndDataFlow .AddRow (array);

	if(m_arrayHisFieldNames.GetCount ()<1)
	{
		TRACEERROR("数据历史表有误!");
		return;
	}
	m_db.AddRecord(GetHistoryTableName (),m_arrayHisFieldNames,array);
}

void CDataTransfer::CloseConnection(CConnection *pConn)
{
	if(!DisconnectChild(pConn))
	{
		DisConnectServer(pConn);
	}
}


bool CDataTransfer::CheckOutPicFiles(CString sDBFile,CStringArray &arrayPicFiles)
{
	if(sDBFile=="")return false ;

	CDataBrowseDoc doc;
	if(!doc.OpenDBFile (sDBFile))
	{
		return false;
	}

	int i=0,j=0;
	CStringArray *pArrayRecord=NULL;
	CString sData;
	int iFieldQty=doc.GetFieldQty ();

	for(i=0;i<doc.GetRecordQty ();i++)
	{
		pArrayRecord=doc.GetRecord (i);
		if(pArrayRecord->GetCount ()!=iFieldQty)return false;
		for(j=0;j<iFieldQty;j++)
		{
			sData=pArrayRecord->GetAt (j);
			if(IsPicFile(sData))
			{
				sData=GetPicFullPath(sData);
				arrayPicFiles.Add (sData);
			}
		}
	}

	return true;
}
