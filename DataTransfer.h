#pragma once

#include "transocket.h"
#include "DataBaseManager.h"
#include "Connection.h"
#include "DataBrowseDoc.h"
#include "MyDate.h"

class CDataTransfer 
{
friend class CConnection;

public:
	CDataTransfer(void);
	~CDataTransfer(void);

	bool ProcessPendingAccept();

	void RemoveAllServers();
	bool GetServers();
	bool AddServer(CServer &server);
	bool AddServer(	CString sServerName,
					CString sAddr,
					int iPort,
					CString arrayUsers,
					CString arrayPasswords);

public:
	bool ListenClients();
	bool OtherServerConnected();
	
	bool ConnectServers(bool bServer); 
	bool DisConnectServers();
	bool ConnectChild();
	bool DisconnectChild(CConnection *pConnTag);
	bool DisConnectServer(CConnection *pConnTag);
	bool DisconnectChildren();
	bool Initiate(bool bServer);	
	bool Initiated()	{ return m_bInitiated;	};
	bool SendDataToApplier(CConnection *pConn);
	bool SendDataToServers(CString sDBFile,CConnection *pConnTag=NULL);
	bool SendDataToServers(CConnection *pConnTag=NULL);
	
/*	bool GetDataFromServer(					CString			sServer, 
											UINT			iPort,
											CString			sTableName,
											CString			sSQL,
											CStringArray	*pArrayFields,
											CDataBrowseDoc	*pDoc,
											int				iMaxHandDownRecord);
*/

	bool StartReceiveData(		CDataBrowseDoc	*pTarBrowseDoc	);

	bool ReceiveRecord(			CConnection		*pConn);
	bool TransferData(			CConnection		*pConn);

	bool GetData(				CString			sServer,
								UINT			iPort,
								CString			sTable,
								CString			sSQL,
								CTableDate		dateStart,
								CStringArray	*pArrayFields,
								CDataBrowseDoc  *pDoc,
								int				iMaxTransferRecord,
								bool			bMarkHandedIn);
	
	bool DeleteRecord(			CString			sDoc);

	bool UpdateRecord(			CString			sDBFile);

	bool	AddRecord(			CString			sDoc);

	void	RecordHistory(		CStringArray	&array);

	void	CloseConnection (	CConnection		*pConn);

	void	SetLocalIP		(	CString			sIP);

	void	SetLocalPort	(	int				iPort);

	bool	IsLocalServer	(	CString			sServer,int iPort);



	CPtrList			m_ChildrenList;			//客户端套节字链表
	CPtrList			m_FathersList;			//其他服务器套节字链表

protected:
	bool				m_bServer;
	CServer				m_Servers[128];
	int					m_iServerQty;
	bool				m_bInitiated;

	CServer				m_LocalServer;

	CTranSocket			*m_pListeningSocket;
	bool				m_bListening;
	bool				m_bOtherServerConnected;

	CDataBaseManager	m_db;

	CStringArray		m_arrayHisFieldNames;
	CStringArray		m_arrayHisFieldTypes;

	CString				m_sLocalIP;
	UINT                m_iLocalPort;


	CConnection *		GetFatherConnection(CString sServer,	int iPort);
	CConnection *		GetChildConnection(CString sServer,		int iPort);

	bool				AddLinker		(	CConnection		*pConn,	bool bServer);
	bool				EndLinker		(	CConnection		*pConn);
	bool				ModifyInFlow	(	CConnection		*pConn, double  lfKB);
	bool				ModifyOutFlow	(	CConnection		*pConn, double  lfKB);
	bool				RemoveLinker	(	CConnection		*pConn);

	bool				CheckOutPicFiles(CString sDBFile,CStringArray &arrayPicFiles);

protected:

//	virtual bool SendDataToClient(CConnection *pConn,char *pInfoAndData);

	


protected:
	bool SetClientName(CTranSocket *pSocket,CString sUser);	
	virtual void UpdateView();

};
