
#pragma once

#include "DataBaseManager.h"
#include "TranSocket.h"
#include "PasswordManager.h"
#include "MyDate.h"

class CServer
{
public:
	char sName[100];
	char sIP[20];
	int  iPort;
	char sUserName[100];
	char sPassword[100];
	CStringArray arrayAreas;
	bool bVerified;

	CServer()
	{
		memset(sName,0,100);
		memset(sIP,0,20);
		iPort=0;
		memset(sUserName,0,100);
		memset(sPassword,0,100);		
		arrayAreas.RemoveAll ();
		bVerified=false;
	};

	CServer(CServer &server)
	{
		memcpy(this,&server,sizeof(CServer));
	};

	void operator =(CServer &other)
	{
		memcpy(this,&other,sizeof(CServer));
	};

	bool operator ==(CServer &other)
	{
		return(strcmp(sIP,other.sIP )==0
			&&iPort==other.iPort );
	};

	bool Set(	char	sNameTag[],
				char	sIPTag[], 
				int		iPortTag,
				CStringArray &arrayAreasTag)
	{
		strcpy(sName,sNameTag);
		strcpy(sIP,sIPTag);
		iPort=iPortTag;
		arrayAreas.Copy (arrayAreasTag);

		bVerified=false;
		return true;
	};

	bool Set(	CString sNameTag,
				CString sIPTag, 
				int		iPortTag,
				CString sUserNameTag,
				CString sPasswordTag)
	{
		strcpy(sName,(LPCTSTR)sNameTag);
		strcpy(sIP,(LPCTSTR)sIPTag);
		iPort=iPortTag;
		strcpy(sUserName,(LPCTSTR)sUserNameTag);
		strcpy(sPassword,(LPCTSTR)sPasswordTag);


		bVerified=false;
		return true;
	};


	bool ExistArea(CString sArea)
	{
		for(int i=0;i<arrayAreas.GetCount ();i++)
		{
			if(arrayAreas[i]==sArea)return true;
		}
		return false;
	};
};

enum enumInfoDirection
{
	INFO_HAND_DOWN,
	INFO_HAND_IN
};

enum enumInfoClass
{
	// From Client:
	INFO_MSG,
	INFO_DATA,
	INFO_ERROR
};

enum enumInfoDetail
{
	// Connection Information:
	// from client:
	INFO_PASSWORD,
	INFO_WANTDATA,
	INFO_CHILD_START,
	INFO_CHILD_END,
	INFO_VERIFIED,

	// Below are belong to INFO_HANDIN, and INFO_HANDIN:
	INFO_FILE_START,
	INFO_FILE_APPEND,
	INFO_FILE_END,

    INFO_FILE_WANTMOREDATA,

	// information of correct reply:
	INFO_SERVER_CONNECTED,
	INFO_OTHER_SERVER_CONNECTED,
	INFO_CHILD_REMOVED,
	INFO_OTHER_SERVER_ENDED,

	// Error information :
	ERROR_DBINIT,
	ERROR_DBDISCONNECT,

	ERROR_OPENTABLE,
	ERROR_APPENDRECORD,
	ERROR_CLOSETABLE,

	ERROR_GETRECORD,
	ERROR_DELETERECORD,
	ERROR_UPDATERECORD,
	

	ERROR_CREATEFILE,
	ERROR_APPENDDATA,
	ERROR_CLOSEFILE,

	ERROR_OTHER_SERVER_UNCONNECTED,
	ERROR_OTHER_SERVER_FAILEDEND,

	ERROR_USER_LEVEL_TOO_LOW,

	ERROR_INVALIDUSER
};

class CDataBrowseDoc;

class CDataInfo
{
public:
	char				sUser[20];
	char				sPassword[20];
	enumInfoDirection	iInfoDir;
	enumInfoClass		iInfoClass;
	enumInfoDetail		iInfoDetail;
	UINT				iGlobalOrder;
	UINT				iBatchOrder;
	UINT				iBatchOrderSecond;
	UINT				iBatchTotalQty;
	char				sSourceIP[20]; // 数据的发送端IP。
	UINT				iSourcePort;
	char				sTargetIP[20]; // 数据的接收端IP。
	UINT				iTargetPort;
	CDataBrowseDoc		*pBrowseDoc;
	UINT				iMaxTransRecord;
	UINT				iDataLen;

	CDataInfo()
	{
		memset(this,0,sizeof(CDataInfo));
		iMaxTransRecord=500;
	};

	void operator =(CDataInfo &other)
	{
		memcpy(this,&other,sizeof(CDataInfo));
	};

	CString GetInfoString();

};

class CConnection
{
public:
	CTranSocket			*m_pSocket;
	char				m_sName[100];
	char				m_sIP[20];
	int					m_iPort;
	CTime				m_timeStart;
	CTime				m_timeEnd;
	int					m_iHandInOrder;
	int					m_iHandDownOrder;
	bool				m_bUserVerified;
	int					m_iTranRecordLimit;
	char				m_cItemSplitter	;
	char				m_cSerSplitter	;
	bool				m_bOtherServersConnected;  // indicate my remote server has connected other servers.
	 
	double				m_lfInDataFlow;
	double				m_lfOutDataFlow;


	CDataBaseManager	m_db;
	FILE				*m_fpInComing;
    int                 m_iInComingFileSection;

    FILE                *m_fpOutput;
    CString             m_sOutputingFileName;
    int                 m_iOutputingFileSection;
    int                 m_iOutputFileSectionQty;
    CString             m_sTargetIPForOutputingFile;
    int                 m_iTargetPortForOutputingFile;
    enumInfoDirection   m_eOutFileDirection;


	CString				m_sFileReceived;

	char				*m_pBuffer;
	int					m_iBufferSize;
	int					m_iBufferReadyPos;

	CDataTransfer		*m_pDataTransfer;



	CConnection();
	~CConnection();
	bool ConnectServer(CServer &server);
	void VerifyUser();
	void GetLocalIP	(CString &sIP, UINT &iPort);
	void GetPeerIP	(CString &sIP, UINT &iPort);

	bool Accept(CTranSocket *pListeningSocket);

	void SetDataTransfer(CDataTransfer *pDataTransferTag);

	void SetLinker (CString sNameTag,SOCKADDR *pAddr);
	void SetLinker (char sName[],char sIP[],int iPort);
	void EmptyBuffer();
	void EmptyDataBuffer();

	void ProcessPendingRead (CTranSocket *pSocket);

	char *		GetBuffer			();
	int			GetBufferSize		();
	CDataInfo*	GetInfoBuffer		();
	char *		GetDataBuffer		();
	void		CopyDataToBuffer	(	char *pData, int iLen);
	void		CopyBuffer			(	CConnection *pOtherConn);
	
	char *		GetSerItem			(	char *pData, CStringArray	&array);
	char *		SetSerItem			(	char *pData, CStringArray	&array);
	void		ClearString			(	char *pData, int iLen);
	void		RecordInfo			();


	bool ProcessReceivedData();
	void ProcessReceivedFile(CString sFile);



	void ProcessNormalMsg();
	void ProcessErrorMsg();
	void ProcessWantData();

    bool ProcessFileWantMoreData();



	bool SendInfo		();
	bool SendData		();   // the msg is stored in m_pBuffer

	bool StartFile		();
	bool AppendFileData	();
	bool CloseFile		();

	bool OpenTable		();
	bool AppendRecord	();
	bool CloseTable		();

	void GetPics		(	CStringArray *pArrayFields,CStringArray *pArrayPics);

	bool SendRecords	(	enumInfoDirection	enumDir, 
							CString				sTable,
							CString				sSQL,
							CTableDate			dateStart,
							CStringArray		&arrayFields,
							int					iMaxRecordQty,
							bool				bMarkHandedIn);

	CString GetFileName	(	CDataInfo *pInfo,CString sFileName);
	
    // Send file functions:
    bool SendFile       (   CString sFileName,
                            CString sTargetIP,
                            int iTargetPort,
                            enumInfoDirection enumDir);
    
    bool SendFileContinue(  int iSectionOrder);

    bool SendFileEnd     ();

    //
	void WantData		(	CString				sServer, 
							UINT				iPort,
							CString				sTable,
							CString				sSQL,
							CTableDate			dateStart,
							CStringArray		*pArrayFields,
							CDataBrowseDoc		*pDoc,
							int					iMaxRecord);

	void SetUserVerified();

	void LocalAddRecord(		CString		 sTable,
								CStringArray &arrayFields,
								CStringArray &arrayValues);

	void LocalDeleteRecord(		CString		sTable,
								int			iRecordID);

	void LocalUpdateRecord(		CString		 sTable,
								int 		 iRecordID,
								CStringArray &arrayFields,
								CStringArray &arrayValues);


	void CloseConnection();

	void SetServer		(		CServer &server	);


protected:
	void SetBaseInfo();

	CServer m_server;


};


