
#include "stdafx.h"
#include "FHApp.h"
#include "Connection.h"
#include "StringExtractor.h"
#include "FHMainFrm.h"
#include "DataTransfer.h"
#include "qcglobal.h"
#include "fhmainfrm.h"
#include "DataFile.h"
#include "Compressor.h"

CConnection::CConnection()
{
	m_pSocket=NULL;
	memset(m_sName,0,100);
	memset(m_sIP,0,20);
	m_timeStart.GetCurrentTime ();
	m_timeEnd.GetCurrentTime ();
	m_iPort=0;
	m_fpInComing=NULL;
	m_iBufferSize=1000;
	m_pBuffer=new char [m_iBufferSize+sizeof(CDataInfo)+100];
	m_iBufferReadyPos=0;

	m_cItemSplitter	='\n';
	m_cSerSplitter	='\r';
	m_pDataTransfer = NULL;

	m_iHandInOrder=0;
	m_iHandDownOrder=0;

	m_iTranRecordLimit=1000;

	m_bUserVerified=false;
	m_bOtherServersConnected=false;

	m_lfInDataFlow=0;
	m_lfOutDataFlow=0;

    m_fpOutput=NULL;

	m_db.Initiate (GetDBName());

};

CConnection::~CConnection()
{
	if(m_pSocket!=NULL)
	{
		m_pSocket->Close ();
		delete m_pSocket;
		m_pSocket=NULL;
	}

	if(m_fpInComing)
	{
		fclose(m_fpInComing);
		m_fpInComing=NULL;
	}

	if(m_pBuffer)
	{
		delete []m_pBuffer;
		m_pBuffer=NULL;
	}
}

void CConnection::SetDataTransfer(CDataTransfer *pDataTransferTag)
{
	m_pDataTransfer=pDataTransferTag;
}

void CConnection::SetLinker(CString sNameTag,SOCKADDR *pAddr)
{
	SOCKADDR_IN *pInAddr=(SOCKADDR_IN *)pAddr;

	unsigned char b1=pInAddr->sin_addr .S_un .S_un_b .s_b1;
	unsigned char b2=pInAddr->sin_addr .S_un .S_un_b .s_b2;
	unsigned char b3=pInAddr->sin_addr .S_un .S_un_b .s_b3;
	unsigned char b4=pInAddr->sin_addr .S_un .S_un_b .s_b4;

	sprintf(m_sIP,"%d.%d.%d.%d",b1,b2,b3,b4);
	m_iPort=pInAddr->sin_port ;
	strcpy(m_sName,sNameTag);
};

void CConnection::SetLinker (char sName[],char sIP[],int iPort)
{
	strcpy(m_sIP,sIP );
	m_iPort=iPort ;
	strcpy(m_sName,sName);
}


void CConnection::EmptyDataBuffer()
{
	memset(&m_pBuffer[sizeof(CDataInfo)],0,m_iBufferSize);
}

void CConnection::EmptyBuffer()
{
	memset(m_pBuffer,0,sizeof(CDataInfo)+m_iBufferSize);
}

CDataInfo* CConnection::GetInfoBuffer()
{
	return (CDataInfo*)m_pBuffer;
}

char * CConnection::GetDataBuffer()
{
	return (char*)(&m_pBuffer[sizeof(CDataInfo)]);
}

char * CConnection::GetSerItem(char *pData,CStringArray	&array)
{
	int i=0;
	while(pData[i]!=m_cSerSplitter&&i<m_iBufferSize)
	{
		i++;
	}
	int iNextStart=i+1;

	char *ps=new char[i+10];
	memset(ps,0,i+10);
	memcpy(ps,pData,i-1);
	
	CStringExtractor sE;
	sE.SetSepChar (CString(m_cItemSplitter));
	sE.SetString (ps);

	int iQty=sE.GetSubStringQty ();
	array.RemoveAll ();
	for(i=0;i<iQty;i++)
	{
		array.Add (sE.GetSubString (i));
	}

	delete []ps;

	return (char*)(&pData[iNextStart]);
}
void CConnection::ClearString(char *pData,int iLen)
{
	static int i=0;
	for(i=0;i<iLen;i++)
	{
		if(pData[i]==m_cItemSplitter||pData[i]==m_cSerSplitter)
		{
			pData[i]=' ';
		}
	}
}

char * CConnection::SetSerItem(char	*pData, CStringArray &array)
{
	EmptyDataBuffer();

	CString s;
	int iStart=0;
	for(int i=0;i<array.GetCount ();i++)
	{
		s=array.GetAt (i);
		s.Trim ();
		sprintf(&pData[iStart],"%s%c",(LPCTSTR)s,m_cItemSplitter);
		iStart+=s.GetLength ()+1;
		if(iStart>m_iBufferSize)
		{
			ASSERT(false);
			TRACEERROR("传输数据量过大，超过限定缓冲长度:"+vtos(m_iBufferSize));
			return NULL;
		}
	}
	pData[iStart]=m_cSerSplitter;
	iStart++;

	return (char*)(&pData[iStart]);
}

void CConnection::ProcessPendingRead (CTranSocket *pSocket)
{
	if(pSocket!=m_pSocket)
	{
        CString sPeerIP,sPeerIPLocal;
        UINT iPeerPort, iPeerPortLocal;
        pSocket->GetPeerName(sPeerIP,iPeerPort);
        m_pSocket->GetPeerName(sPeerIPLocal,iPeerPortLocal);
        TRACEERROR(CString("接收到的Socket不同于本地Socket:"));
        TRACEERROR(CString("接收到的Socket IP:")+sPeerIP+","+vtos((int)iPeerPort));
        TRACEERROR(CString("本地Socket IP:")+sPeerIPLocal+","+vtos((int)iPeerPortLocal));
		ASSERT(false);
		return;
	}

	if(m_pSocket==NULL)
	{
		TRACEERROR("本连接的套接字为空!");
		return;
	}

	int iReceiveLimit=m_iBufferSize +sizeof(CDataInfo)-m_iBufferReadyPos;
	if(iReceiveLimit<1)
	{
		TRACEERROR("缓冲区已满，无法继续接受数据!");
		return;
	}
	int nReceived=m_pSocket->Receive (
				&m_pBuffer[m_iBufferReadyPos],
				iReceiveLimit,
				0);

	// 纪录进入流量：
	m_lfInDataFlow+=nReceived/(1000.0);
	if(m_pDataTransfer)
	{
		m_pDataTransfer->ModifyInFlow (this,m_lfInDataFlow);
	}

	// 分析数据：
	m_iBufferReadyPos+=nReceived;

	switch(nReceived)
	{
	case 0:  // closed		
		TRACEERROR("连接已经关闭!");
		return ;

	case SOCKET_ERROR :
		TRACEERROR("连接错误!");
		return ;

	default:
		break;

	}

	///////////////////////////////////////
	//  Process the data:
	//  May be received many msgs in one time:
	///////////////////////////////////////
	int iDataOneBlock=0;
	int i=0;
	CString sLocalIP="";
	UINT  iLocalPort=0;
	
    GetLocalIP(sLocalIP,iLocalPort);

    TRACEPROGRESS("将要处理到本机的信息,本机IP:"+sLocalIP +",端口:"+vtos((int)iLocalPort));

	while(m_iBufferReadyPos>0)
	{
		CDataInfo *pInfo=GetInfoBuffer();
		iDataOneBlock=sizeof(CDataInfo)+pInfo->iDataLen;
		if(m_iBufferReadyPos<iDataOneBlock)
		{
			break;
		}

		//
		RecordInfo();

        TRACEPROGRESS(CString("客户信息目标IP:")+pInfo->sTargetIP+",本服务器IP:"+sLocalIP);

		// 如果此信息是发送给本机的:
		if(   strcmp(pInfo->sTargetIP,(LPCTSTR)sLocalIP)==0)
			// &&pInfo->iTargetPort == iLocalPort): does not consider the port. 
		{
			if(pInfo->iInfoClass == INFO_MSG)
			{
				ProcessNormalMsg();
			}
			else if(pInfo->iInfoClass ==INFO_DATA)
			{
				ProcessReceivedData();
			}
			else if(pInfo->iInfoClass ==INFO_ERROR)
			{
				ProcessErrorMsg();
			}
			else 
			{
				ASSERT(false);
				TRACEERROR("程序未处理的信息!");
			}
		}

		// 此信息不是发送给本机的,则继续传输下去:
		else
		{
			if(m_pDataTransfer)
			{
				m_pDataTransfer->TransferData(this);
                TRACEPROGRESS("已经中转数据!");
			}
			else
			{
				TRACEERROR("未定义数据传输器!");
			}
		}

		for(i=iDataOneBlock;i<m_iBufferReadyPos+10;i++)
		{
			m_pBuffer[i-iDataOneBlock]=m_pBuffer[i];
		}
		m_iBufferReadyPos-= iDataOneBlock;
	}

}

void CConnection::ProcessWantData()
{
	CDataInfo *pInfo=GetInfoBuffer();
	if(pInfo->iInfoDetail !=INFO_WANTDATA)
	{
		ASSERT(false);
	}

	CStringArray array;
	GetSerItem(GetDataBuffer(),array);
	int nCount=array.GetCount ();
	if(nCount<9)
	{
		TRACEERROR("错误:客户端所要数据表述错误!");
		return;
	}
	CStringArray arrayFields;
	for(int i=8;i<nCount;i++)
	{
		arrayFields.Add(array.GetAt (i));
	}

	if(pInfo->iMaxTransRecord ==0)
	{
		pInfo->iMaxTransRecord =1000;
	}

	CTableDate dateStart;
	dateStart.sYearField =array.GetAt (2);
	dateStart.sMonthField =array.GetAt (3);
	dateStart.sDayField =array.GetAt (4);
	dateStart.sYear		=array.GetAt (5);
	dateStart.sMonth	=array.GetAt (6);
	dateStart.sDay		=array.GetAt (7);


	CDataBrowseDoc doc;
	if(!m_pDataTransfer->GetData("LOCAL",	// Set the server as LOCAL
								0,			// Port
								array[0],
								array[1],
								dateStart,
								&arrayFields,
								&doc,
								pInfo->iMaxTransRecord,
								false))
	{
		TRACEERROR("无法获得数据:"+array[0]+",SQL:"+array[1]);
		return;
	}

	CString sIP;
	UINT iPort;
	GetLocalIP(sIP,iPort);
	doc.SetSourceIP (sIP);
	doc.SetSourcePort (iPort);
	doc.Close ();

	SendFile(doc.GetFileName(),
				pInfo->sTargetIP ,
				pInfo->iTargetPort ,
				INFO_HAND_DOWN);
}

void CConnection::ProcessNormalMsg()
{
	CDataInfo *pInfo=GetInfoBuffer();
	if(pInfo->iInfoClass != INFO_MSG)
	{
		return ;
	}

	CString sLocalIP="";
	UINT iLocalPort=0;
	GetLocalIP(sLocalIP,iLocalPort);

	pInfo->iTargetPort = pInfo->iSourcePort ;
	strcpy(pInfo->sTargetIP	, pInfo->sSourceIP );
	strcpy(pInfo->sSourceIP , sLocalIP);
	pInfo->iSourcePort = iLocalPort;


	//////////////////////////////////////////////////////////
	// 从服务器发来的信息:
	//////////////////////////////////////////////////////////
	if(pInfo->iInfoDetail == INFO_OTHER_SERVER_CONNECTED)
	{
		m_bOtherServersConnected=true;
		TRACEPROGRESS("数据库服务器通知其他服务器已经连接上!");
	}

	else if(pInfo->iInfoDetail == INFO_CHILD_REMOVED)
	{
		TRACEPROGRESS("各客户端已经断开连接!");
	}

	else if(pInfo->iInfoDetail == INFO_OTHER_SERVER_ENDED)
	{
		TRACEPROGRESS("数据库服务器通知其他服务器已经正常断开!");
	}

	else if (pInfo->iInfoDetail ==INFO_VERIFIED)
	{
		TRACEPROGRESS("合法用户,可以上传数据!");
		m_bUserVerified=true;
	}  

	//////////////////////////////////////////////////////////
	// 从客户端发来的信息:
	//////////////////////////////////////////////////////////
	else if(pInfo->iInfoDetail ==INFO_CHILD_START)
	{	
		if( m_pDataTransfer->OtherServerConnected())
		{
			pInfo->iInfoDetail	= INFO_OTHER_SERVER_CONNECTED;
			pInfo->iInfoClass	= INFO_MSG;
		}
		else
		{
			pInfo->iInfoDetail  = ERROR_OTHER_SERVER_UNCONNECTED;
			pInfo->iInfoClass	= INFO_ERROR;
		}
		pInfo->iInfoDir			= INFO_HAND_DOWN;

		SendInfo();
	}

	else if(pInfo->iInfoDetail ==INFO_CHILD_END)
	{
		m_pDataTransfer->DisconnectChild(this);

		pInfo->iInfoDir		= INFO_HAND_DOWN;
		pInfo->iInfoClass	= INFO_MSG;
		pInfo->iInfoDetail	= INFO_CHILD_REMOVED;

		SendInfo();

	}
	
	else if(pInfo->iInfoDetail ==INFO_PASSWORD)
	{	
        TRACEPROGRESS(CString("用户试图连接:")+pInfo->sUser+",口令:"+pInfo->sPassword);
		if(m_pDataTransfer&&UserValid(pInfo->sUser ,pInfo->sPassword ))
		{
            TRACEPROGRESS("用户合法!");

			strcpy(m_sName , pInfo->sUser);

			pInfo->iInfoDir		= INFO_HAND_DOWN;
			pInfo->iInfoDetail	= INFO_VERIFIED;			
			pInfo->iInfoClass	= INFO_MSG;		

			SendInfo();

			if(m_pDataTransfer->m_bOtherServerConnected)
			{
				pInfo->iInfoDetail	= INFO_OTHER_SERVER_CONNECTED;			
			}
			else
			{
				pInfo->iInfoDetail	= ERROR_OTHER_SERVER_UNCONNECTED;			
			}
			SendInfo();			
		}
		else
		{
            TRACEPROGRESS("用户非法!");

			pInfo->iInfoDir		= INFO_HAND_DOWN;
			pInfo->iInfoDetail	= ERROR_INVALIDUSER;			
			pInfo->iInfoClass	= INFO_ERROR;

			SendInfo();
		}
	}

	else if(pInfo->iInfoDetail ==INFO_WANTDATA)
	{
		enumUserLevel eLevel=GetUserLevel(pInfo->sUser ,pInfo->sPassword);
		if(eLevel==USER_ADMINISTRATOR||eLevel==USER_SUPERUSER)
		{
			ProcessWantData();
		}
		else
		{
			pInfo->iInfoDir		= INFO_HAND_DOWN;
			pInfo->iInfoDetail	= ERROR_USER_LEVEL_TOO_LOW;			
			pInfo->iInfoClass	= INFO_ERROR;
			SendInfo();
		}
	}

    else if(pInfo->iInfoDetail == INFO_FILE_WANTMOREDATA)
    {
        ProcessFileWantMoreData();
    }
	
	else
	{
		ASSERT(false);
		TRACEERROR("未处理的信息!");
	}


	return ;
}

bool CConnection::ProcessFileWantMoreData()
{
    CDataInfo *pInfo=GetInfoBuffer();
    return SendFileContinue(pInfo->iBatchOrder);
}

bool CConnection::ProcessReceivedData()
{
	CDataInfo *pInfo=GetInfoBuffer();

    CString sLocalIP="";
    UINT iLocalPort=0;
    GetLocalIP(sLocalIP,iLocalPort);

    strcpy(pInfo->sTargetIP	, pInfo->sSourceIP );
    pInfo->iTargetPort = pInfo->iSourcePort ;
    strcpy(pInfo->sSourceIP , sLocalIP);
    pInfo->iSourcePort = iLocalPort;


    TRACEPROGRESS("接收数据...");
	if(pInfo->iInfoDetail ==INFO_FILE_START)
	{
        TRACEPROGRESS("接收建立文件信息...");
		if(!StartFile())
		{
			pInfo->iInfoDir		= INFO_HAND_DOWN;
			pInfo->iInfoClass	= INFO_ERROR;
			pInfo->iInfoDetail	= ERROR_CREATEFILE;
			return false;
		}
        else
        {
            pInfo->iInfoDir		= INFO_HAND_DOWN;
            pInfo->iInfoClass	= INFO_MSG;
            pInfo->iInfoDetail	= INFO_FILE_WANTMOREDATA;
            pInfo->iBatchOrder  = 0;
            SendInfo();
            return true;
        };

	}

	else if(pInfo->iInfoDetail ==INFO_FILE_APPEND)
	{
        TRACEPROGRESS("接收文件内容...");
		if(!AppendFileData())
		{
			pInfo->iInfoDir		= INFO_HAND_DOWN;
			pInfo->iInfoClass	= INFO_ERROR;
			pInfo->iInfoDetail	= ERROR_APPENDDATA;
			return false;
		}
        else
        {
            pInfo->iInfoDir		= INFO_HAND_DOWN;
            pInfo->iInfoClass	= INFO_MSG;
            pInfo->iInfoDetail	= INFO_FILE_WANTMOREDATA;
            pInfo->iBatchOrder  ++;
            SendInfo();
        }
	}

	else if(pInfo->iInfoDetail ==INFO_FILE_END)
	{
        TRACEPROGRESS("接收文件结束信息...");
		if(!CloseFile())
		{
			pInfo->iInfoDir		= INFO_HAND_DOWN;
			pInfo->iInfoClass	= INFO_ERROR;
			pInfo->iInfoDetail = ERROR_CLOSEFILE;
			return false;
		};
	}

	/*
	else if(pInfo->iInfoDetail ==INFO_OPENTABLE)
	{
		if(!OpenTable())
		{
			pInfo->iInfoDir		= INFO_HAND_DOWN;
			pInfo->iInfoClass	= INFO_ERROR;
			pInfo->iInfoDetail  = ERROR_OPENTABLE;
			return false;
		}
	}

	else if(pInfo->iInfoDetail ==INFO_APPEND_RECORD)
	{
		if(!AppendRecord())
		{
			pInfo->iInfoDir		= INFO_HAND_DOWN;
			pInfo->iInfoClass	= INFO_ERROR;
			pInfo->iInfoDetail	= ERROR_APPENDRECORD;
			return false;
		}
	}

	else if(pInfo->iInfoDetail ==INFO_CLOSETABLE)
	{
		if(!CloseTable())
		{
			pInfo->iInfoDir		= INFO_HAND_DOWN;
			pInfo->iInfoClass	= INFO_ERROR;
			pInfo->iInfoDetail	= ERROR_CLOSETABLE;
			return false;
		}
	}

	else if(pInfo->iInfoDetail ==INFO_ADD_RECORD)
	{
		CStringArray array,arrayFields,arrayValues;

		char *pBuffer = GetDataBuffer();

		pBuffer = GetSerItem(pBuffer,array);
		pBuffer = GetSerItem(pBuffer,arrayFields);
		pBuffer = GetSerItem(pBuffer,arrayValues);

		LocalAddRecord(array[0],arrayFields,arrayValues);
	}

	else if(pInfo->iInfoDetail ==INFO_DELETE_RECORD)
	{
		CStringArray array;
		GetSerItem(GetDataBuffer(), array);
		int iPort;
		sscanf((LPCTSTR)array[1],"%d",&iPort);
		LocalDeleteRecord(array[0],iPort);
	}

	else if(pInfo->iInfoDetail ==INFO_MODIFY_RECORD)
	{
		CStringArray array,arrayFields,arrayValues;

		char *pBuffer=GetDataBuffer();
		pBuffer=GetSerItem(pBuffer,array);
		pBuffer=GetSerItem(pBuffer,arrayFields);
		GetSerItem(pBuffer,arrayValues);
		int iRecordID;
		sscanf((LPCTSTR)array[1],"%d",&iRecordID);
		LocalUpdateRecord(array[0],iRecordID,arrayFields,arrayValues);
	}
	*/
	
	else
	{
		ASSERT(false);
		TRACEERROR("Not processed information!");
		return false;
	}

	return true;

}


/*
bool CConnection::OpenTable()
{
	CDataInfo *pInfo=GetInfoBuffer();
	int iGlobalOrder=pInfo->iGlobalOrder ;
	CStringArray array;
	GetSerItem(GetDataBuffer(),array);
	CStringArray arrayFields;
	for(int i=0;i<array.GetCount();i++)
	{
		arrayFields.Add (array.GetAt (i));
	}

	if(pInfo->pBrowseDoc== NULL)
	{
		if(arrayFields.GetCount ()==0)
		{
			CStringArray arrayTypes;
			GetFieldNames (	array[0],
							arrayFields,
							arrayTypes);
		}

		m_db.SetSelectField (arrayFields);
		m_db.Select (array[0],array[1]);
	}
	else
	{
		if(m_pDataTransfer)
		{
			m_pDataTransfer->StartReceiveData(pInfo->pBrowseDoc);
		}
		else
		{
			ASSERT(false);
			TRACEERROR("没有数据传输管理器存在!");
			return false;
		}
	}

	return true;
}

bool CConnection::AppendRecord()
{
	CDataInfo *pInfo=GetInfoBuffer();

	CStringArray arrayRecord;
	GetSerItem(GetDataBuffer(),arrayRecord);

	// 服务器把数据直接送入数据库:
	if(IsServer())
	{
		return m_db .AddRecord (arrayRecord);
	}

	// 客户端显示数据:
	else
	{
		if(m_pDataTransfer)
		{
			m_pDataTransfer->ReceiveRecord(this);
		}
		else
		{
			ASSERT(false);
			TRACEERROR("没有数据传输管理器存在!");
			return false;
		}
	}
	
	return true;
}

bool CConnection::CloseTable()
{
	try
	{
		m_db .m_pRecords->Close();
		m_db .m_pRecords=NULL;
	}
	catch(_com_error e)
	{
		CString sError;
		sError.Format("关闭数据库失败!\r\n错误信息:%s",e.ErrorMessage());
		TRACEERROR(sError);
		return false;
	}

	return true;
}
*/
bool CConnection::StartFile()
{
	CDataInfo *pInfo=GetInfoBuffer();
	if(pInfo->iInfoDetail !=INFO_FILE_START)
	{
		ASSERT(false);
		return false;
	}

	CStringArray array;
	GetSerItem(GetDataBuffer(),array);

	CString sPath=GetMyTempPath();	
	sPath+=SeperateName(array[0]);

	if(m_fpInComing )
	{
		fclose(m_fpInComing);
		m_fpInComing=NULL;
	}

	m_fpInComing=fopen(sPath,"wb");
	if(!m_fpInComing)
	{
		TRACEERROR("无法建立文件:"+sPath);
		return false;
	}
	m_sFileReceived=sPath;

	return true;
}

bool CConnection::AppendFileData()
{
	if(!m_fpInComing )
	{
		TRACEERROR("文件句柄尚未打开. 用户名:"+CString(m_sName ));
		return false;
	}

	CDataInfo* pInfo=GetInfoBuffer();
    TRACEPROGRESS("收到文件包序号:"+vtos((int)pInfo->iBatchOrder+1)+",总包数:"+vtos((int)pInfo->iBatchTotalQty));
	
    char *pData=GetDataBuffer();
	fwrite(pData, pInfo->iDataLen ,1,m_fpInComing);

	return true;
}

bool CConnection::CloseFile()
{
	if(!m_fpInComing)
	{
		TRACEERROR("文件句柄尚未打开. 用户名:"+CString(m_sName ));
		return false;
	}

	fclose(m_fpInComing);
	m_fpInComing=NULL;

	////////////////////////////////
	// UnCompress the file:
	////////////////////////////////
	/*CCompressor compr;
	CString sTempFile=GetTempFile();
	compr.UnCompress (m_sFileReceived,sTempFile);
	*/
	////////////////////////////////
	// Process the file:
	////////////////////////////////
	ProcessReceivedFile(m_sFileReceived);

	return true;
}

void CConnection::ProcessReceivedFile(CString sFile)
{
	if(IsPicFile(sFile))return;
	if(!m_pDataTransfer)return;

	CDataBrowseDoc doc;
	if(!doc.OpenDBFile (sFile))return;
	enumDBFileType eFT=doc.GetFileType ();
	doc.Close ();

	if(eFT==DBFT_COMMON)
	{
		CMultiDocTemplate *pTemplate=GetTemplate("DBBrowse");
		CDataInfo *pInfo=GetInfoBuffer();
		if(pInfo->pBrowseDoc !=NULL)
		{
			if(pTemplate)
			{
				POSITION pos=pTemplate->GetFirstDocPosition ();
				while(pos)
				{
					CDataBrowseDoc *pDocHere=(CDataBrowseDoc *)pTemplate->GetNextDoc (pos);
					if(pDocHere==pInfo->pBrowseDoc)
					{
						pDocHere->OpenDBFile (sFile);
						pDocHere->UpdateAllViews (NULL);
					}
				}
			}
		}
		else
		{
			pTemplate->OpenDocumentFile (sFile);
		}
		return;
	}

	else if(eFT==DBFT_ADD)
	{
		m_pDataTransfer->AddRecord (sFile);
	}

	else if(eFT==DBFT_DELETE)
	{
		m_pDataTransfer->DeleteRecord (sFile);
	}

	else if(eFT==DBFT_UPDATE)
	{
		m_pDataTransfer->UpdateRecord (sFile);
	}
	else
	{
		return;
	}
}

CString CConnection::GetFileName(CDataInfo *pInfo,CString sFileName)
{
	CString sUser=pInfo->sUser ;
	sUser.Trim().MakeUpper();

	sFileName.Trim ().MakeUpper ();

	CString sPath=sUser+"_"+sFileName;

	return sPath;
}

void CConnection::ProcessErrorMsg()
{
	CDataInfo *pInfo=GetInfoBuffer();
	if(pInfo->iInfoClass != INFO_ERROR)
	{
		return ;
	}
	
	if(pInfo->iInfoDetail == ERROR_DBINIT)
	{
		TRACEERROR("数据库服务器初始化错误!");
	}

	else if(pInfo->iInfoDetail == ERROR_DBDISCONNECT)
	{
		TRACEERROR("数据库服务器无法断开连接!");
	}

	else if(pInfo->iInfoDetail == ERROR_OPENTABLE)
	{
		TRACEERROR("数据库服务器无法打开数据库表!");
	}
	
	else if(pInfo->iInfoDetail == ERROR_APPENDRECORD)
	{
		TRACEERROR("数据库服务器无法增加记录!");
	}
	
	else if(pInfo->iInfoDetail == ERROR_CLOSETABLE)
	{
		TRACEERROR("数据库服务器无法关闭数据表!");
	}
	
	else if(pInfo->iInfoDetail == ERROR_GETRECORD)
	{
		TRACEERROR("数据库服务器无法获得记录!");
	}
	
	else if(pInfo->iInfoDetail == ERROR_DELETERECORD)
	{
		TRACEERROR("数据库服务器无法删除记录!");
	}

	else if(pInfo->iInfoDetail == ERROR_UPDATERECORD)
	{
		TRACEERROR("数据库服务器无法更新记录!");
	}

	else if(pInfo->iInfoDetail == ERROR_CREATEFILE)
	{
		TRACEERROR("数据库服务器无法建立文件!");
	}

	else if(pInfo->iInfoDetail == ERROR_APPENDDATA)
	{
		TRACEERROR("数据库服务器无法向文件添加数据!");
	}

	else if(pInfo->iInfoDetail == ERROR_CLOSEFILE)
	{
		TRACEERROR("数据库服务器无法关闭数据文件!");
	}

	else if(pInfo->iInfoDetail == ERROR_OTHER_SERVER_UNCONNECTED)
	{
		TRACEERROR("数据库服务器通知其他服务器已经断开!");
	}

	else if(pInfo->iInfoDetail == ERROR_OTHER_SERVER_FAILEDEND)
	{
		TRACEERROR("数据库服务器通知其他服务器无法断开!");
	}

	else if(pInfo->iInfoDetail == ERROR_INVALIDUSER)
	{
		TRACEERROR("数据库服务器通知用户名或口令错误!");
		m_bUserVerified=false;
	}

	else if(pInfo->iInfoDetail == ERROR_USER_LEVEL_TOO_LOW)
	{
		TRACEERROR("数据库服务器通知用户级别不够，无法完成操作!");
	}

	else
	{
		ASSERT(false);
		TRACEERROR("未处理的信息!");
	}
}




bool CConnection::SendInfo()
{
	CDataInfo *pInfo=GetInfoBuffer();
	pInfo->iDataLen = -1;
	return SendData();
}

bool CConnection::SendData()
{
	static int iTotalLen=0;

	CDataInfo *pInfo=GetInfoBuffer();
	if(	   pInfo->iInfoDetail !=INFO_CHILD_START 
		&& pInfo->iInfoDetail !=INFO_PASSWORD
		&& !m_bUserVerified)
	{
		TRACEERROR("用户尚未确认有效,无法上传数据!");
		return false;
	}

	if(	   pInfo->iInfoDetail !=INFO_CHILD_START 
		&& pInfo->iInfoDetail !=INFO_PASSWORD)
	{
		if(IsServer())
		{
			if(!m_pDataTransfer ->m_bOtherServerConnected )
			{
				TRACEERROR("服务器端尚未连接好其他服务器,无法上传数据!");
				return false;
			}
		}
		else
		{
			if(!m_bOtherServersConnected)
			{
				TRACEERROR("服务器端尚未连接好其他服务器,无法上传数据!");
				return false;
			}
		}
	}

	if(pInfo->iDataLen==0)
	{
		char * pData=GetDataBuffer();
		for(int i=0;i<m_iBufferSize-1;i+=2)
		{
			if(pData[i]==0&&pData[i+1]==0)
			{
				pInfo->iDataLen=i+2;
				break;
			}
		}
	}
	else if(pInfo->iDataLen==-1)
	{
		pInfo->iDataLen=0;
	}

	iTotalLen = pInfo->iDataLen + sizeof(CDataInfo) ;
	int iError=m_pSocket->Send(m_pBuffer,iTotalLen ,0);

    if(iError==SOCKET_ERROR)
    {
        iError=GetLastError();

        TRACEERROR("发送数据返回错误信息:"+(vtos(iError)));

        switch(iError)
        {
        case WSANOTINITIALISED   :
            TRACEERROR("A successful AfxSocketInit must occur before using this API. !");
            break;
        case WSAENETDOWN:
            TRACEERROR("The Windows Sockets implementation detected that the network subsystem failed. ");
            break;
        case WSAEACCES:
            TRACEERROR("requested address is a broadcast address, but the appropriate flag was not set. ");
            break;
        case WSAEINPROGRESS   :
            TRACEERROR("A blocking Windows Sockets operation is in progress. ");
            break;
        case WSAEFAULT:
            TRACEERROR("lpBuf argument is not in a valid part of the user address space. ");
            break;
        case WSAENETRESET  :
            TRACEERROR("connection must be reset because the Windows Sockets implementation dropped it. ");
            break;
        case WSAENOBUFS   :
            TRACEERROR("Windows Sockets implementation reports a buffer deadlock. !");
            break;
        case WSAENOTCONN   :
            TRACEERROR("socket is not connected. ");
            break;
        case WSAENOTSOCK:
            TRACEERROR("The descriptor is not a socket. ");
            break;
        case WSAEOPNOTSUPP:
            TRACEERROR("MSG_OOB was specified, but the socket is not of type SOCK_STREAM. ");
            break;
        case WSAESHUTDOWN :
            TRACEERROR("The socket has been shut down; it is not possible to call Send on a socket after ShutDown has been invoked with nHow set to 1 or 2. ");
            break;
        case WSAEWOULDBLOCK:
            TRACEERROR("The socket is marked as nonblocking and the requested operation would block. ");
            break;
        case WSAEMSGSIZE:
            TRACEERROR("The socket is of type SOCK_DGRAM, and the datagram is larger than the maximum supported by the Windows Sockets implementation. ");
            break;
        case WSAEINVAL:
            TRACEERROR("The socket has not been bound with Bind. ");
            break;
        case WSAECONNABORTED :
            TRACEERROR("The virtual circuit was aborted due to timeout or other failure. ");
            break;
        case WSAECONNRESET:
            TRACEERROR("The virtual circuit was reset by the remote side. ");
            break;
        }
    }

	//
	m_lfOutDataFlow+=iTotalLen/(1000.0);
	if(m_pDataTransfer)
	{
		m_pDataTransfer->ModifyOutFlow (this,m_lfOutDataFlow);
	}

	return true;
}

bool CConnection::SendFile(    CString               sFileName,
							   CString               sTargetIP,
							   int                   iTargetPort,
							   enumInfoDirection     enumDir)
{
	if(sFileName=="")return false;
	// 压缩文件:
/*
    CCompressor comp;
	CString sTempFile=GetTempFile();
	comp.Compress ((LPCTSTR)sFileName,(LPCTSTR)sTempFile);
*/
    //
    if(m_fpOutput)
    {
        int iCode=AfxMessageBox("上个文件尚未发送完毕,是否发送新文件?",MB_YESNO);
        if(iCode==IDNO)
        {
            return false;
        }
        else
        {
            fclose(m_fpOutput);
        }
    }

    m_eOutFileDirection=enumDir;

    //

	//发送压缩过的文件:	
	m_fpOutput=fopen(sFileName,"rb");
	if(!m_fpOutput)
	{
		TRACEERROR("无法打开文件:"+sFileName);
		return false;
	}

    m_sOutputingFileName=sFileName;

	long iFileLen=GetFileLen(m_fpOutput);
	fseek(m_fpOutput,0,SEEK_SET);

    //
    m_iOutputFileSectionQty= iFileLen/ m_iBufferSize;
	if(m_iOutputFileSectionQty*m_iBufferSize!=iFileLen)
	{
		m_iOutputFileSectionQty++;
	}
	
	CDataInfo *pInfo=GetInfoBuffer();	

	// Start send file:
	if(enumDir==INFO_HAND_IN)
	{
		m_iHandInOrder++;
		pInfo->iGlobalOrder		= m_iHandInOrder;
	}
	else
	{
		m_iHandDownOrder++;
		pInfo->iGlobalOrder		= m_iHandDownOrder;
	}

	CString sLocalIP="";
	UINT iLocalPort=0;
	GetLocalIP(sLocalIP,iLocalPort);

	SetBaseInfo();

	pInfo->iInfoDir			= enumDir;	
	pInfo->iInfoClass		= INFO_DATA;
	pInfo->iInfoDetail		= INFO_FILE_START;
	pInfo->iDataLen			= 0;
	pInfo->iBatchTotalQty	= m_iOutputFileSectionQty;
	
	strcpy(pInfo->sSourceIP		,(LPCTSTR) sLocalIP);
	pInfo->iSourcePort		= iLocalPort;
	strcpy(pInfo->sTargetIP	,	(LPCTSTR) sTargetIP);
	pInfo->iTargetPort		= iTargetPort;

    m_sTargetIPForOutputingFile     =sTargetIP;
    m_iTargetPortForOutputingFile   =iTargetPort;


	CStringArray array;
	array.Add (SeperateFileName(sFileName)); // 如果全名，则SendData时文件被清空
	if(!SetSerItem(GetDataBuffer(),array))
    {
        fclose(m_fpOutput);
        m_fpOutput=NULL;
        return false;
    }
    TRACEPROGRESS("开始传送二进制文件:"+sFileName+",目标地址:"+sTargetIP+",目标端口:"+vtos((int)iTargetPort));
	if(!SendData())
	{
		fclose(m_fpOutput);
        m_fpOutput=NULL;
		return false;
	}

    m_iOutputingFileSection=0;
    
    return true;
}

bool CConnection::SendFileContinue(int iSectionOrder)
{
    if(iSectionOrder!=m_iOutputingFileSection)
    {
        TRACEERROR("所要的文件段号不同于本地当前文件段号,错误，返回!");
        return false;
    }

    if(!m_fpOutput)
    {
        TRACEERROR("需要传送的文件没有打开,错误，返回!");
        return false;
    }

    if(iSectionOrder>=m_iOutputFileSectionQty)
    {
        return SendFileEnd();
    }

    //
    bool bSuccess=true;
	char *pDataBuffer=GetDataBuffer();
	int iRead=0;

    TRACEPROGRESS(CString("正在传送二进制文件:")+m_sOutputingFileName+"当前传递段序号:"+vtos(iSectionOrder)+",传送需要总段数:"+vtos(m_iOutputFileSectionQty));

    //
    CDataInfo *pInfo=GetInfoBuffer();	

    // Start send file:
    CString sLocalIP="";
    UINT iLocalPort=0;
    GetLocalIP(sLocalIP,iLocalPort);

    SetBaseInfo();

    pInfo->iInfoDir			= m_eOutFileDirection;	
    pInfo->iInfoClass		= INFO_DATA;
    pInfo->iInfoDetail		= INFO_FILE_APPEND;
    pInfo->iDataLen			= 0;
    pInfo->iBatchOrder      = iSectionOrder;
    pInfo->iBatchTotalQty	= m_iOutputFileSectionQty;

    strcpy(pInfo->sSourceIP		,(LPCTSTR) sLocalIP);
    pInfo->iSourcePort		= iLocalPort;
    strcpy(pInfo->sTargetIP	,	(LPCTSTR) m_sTargetIPForOutputingFile);
    pInfo->iTargetPort		= m_iTargetPortForOutputingFile;

    //
	EmptyDataBuffer();
	iRead=fread(pDataBuffer,1,m_iBufferSize,m_fpOutput);

	if(iRead>0)
	{
		pInfo->iInfoDetail = INFO_FILE_APPEND;
		pInfo->iDataLen    = iRead;
        if(!SendData())
		{
			bSuccess=false;
		}
	}
	else
	{
		TRACEERROR("信息文件错误:"+m_sOutputingFileName);
		bSuccess=false;
	}

    m_iOutputingFileSection++;

    return bSuccess;
}

bool CConnection::SendFileEnd()
{
    if(!m_fpOutput)
    {
        return false;
    }
    fclose(m_fpOutput);
    m_fpOutput=NULL;

    // End of sending file:
    CDataInfo *pInfo=GetInfoBuffer();	
    EmptyDataBuffer();

    SetBaseInfo();

    pInfo->iInfoDir			= m_eOutFileDirection;	
    pInfo->iInfoClass		= INFO_DATA;
	pInfo->iInfoDetail      = INFO_FILE_END;
	pInfo->iDataLen         = 0;
	
    SendInfo();

    TRACEPROGRESS(CString("传送二进制文件完毕!"));

	return true;
}

char * CConnection::GetBuffer()
{
	return m_pBuffer;
}


int CConnection::GetBufferSize()
{
	return m_iBufferSize;
}

void CConnection::CopyDataToBuffer(char *pData, int iLen)
{
	if(iLen>m_iBufferSize)
	{
		delete []m_pBuffer;
		m_pBuffer=new char [iLen+100];
		m_iBufferSize=iLen;
	}

	memcpy(GetDataBuffer(),pData,iLen);

	return;
}

/*
/////////////////////////////////
// The client wanted data,
// Take the data from server,
// send the data to the client:
//////////////////////////////////
bool CConnection::SendRecords(enumInfoDirection enumDir,
							  CString			sTable,
							  CString			sSQL,
							  CTableDate		dateStart,
							  CStringArray		&arrayFields,
							  int				iMaxTransferRecord,
							  bool				bMarkHandedIn)
{
	/////////////////////////////////////
	// Send data needed to a browse doc:
	/////////////////////////////////////
	CDataBrowseDoc doc;
	m_pDataTransfer->SendTableToBrowseDoc (sTable,
										sSQL,
										dateStart,
										&arrayFields,
										&doc,
										iMaxTransferRecord,
										bMarkHandedIn);

	SendBinFile(doc.m_sDBFileName ,
	

	///////////////////////////////////
	// Send the records to client:
	///////////////////////////////////
	EmptyBuffer();
	CDataInfo *pInfo=GetInfoBuffer();

	pInfo->iInfoDir = enumDir;
	pInfo->iGlobalOrder ++;
	pInfo->iBatchTotalQty =doc.GetRecordQty ();
	if(pInfo->iBatchTotalQty>m_iTranRecordLimit)
	{
		pInfo->iBatchTotalQty=m_iTranRecordLimit;
	}
	

	pInfo->iInfoClass	=   INFO_DATA;
	pInfo->iInfoDetail	=	INFO_OPENTABLE;
	CStringArray array;
	array.Add (sTable);
	array.Add (sSQL);
	SetSerItem(GetDataBuffer(),array);
	if(!SendData())
	{
		TRACEERROR("无法发送数据,传输中断!");
		return false;
	}

	CStringArray *pArray=NULL;
	int iOrder=0;
	char *pDataStart=GetDataBuffer();
	char *pDataEnd=NULL;
	CStringArray arrayPics;
	CString sPicFile="";
	int i=0,j=0;

	pInfo->iInfoClass	=   INFO_DATA;
	pInfo->iInfoDetail	=	INFO_APPEND_RECORD;

	for(i=0;i<pInfo->iBatchTotalQty;i++)
	{
		pArray=doc.GetRecord (i);
		if(!pArray)
		{
			TRACEERROR("无法从数据表中得到记录");
			continue;
		}

		// Send Pics in the record:
		GetPics(pArray,&arrayPics);
		for(j=0;j<arrayPics.GetCount ();j++)
		{
			sPicFile=arrayPics[j];
			SendBinFile(sPicFile,enumDir);
		}

		// Send The record itself:
		EmptyDataBuffer();
		pDataEnd=SetSerItem(pDataStart,*pArray);

		pInfo->iBatchOrder			=	i;
		pInfo->iBatchOrderSecond	=	0;
		pInfo->iInfoDetail			=	INFO_APPEND_RECORD;

		if(!SendData())
		{
			TRACEERROR("无法发送数据,传输中断!");
			break;
		}
	}

	EmptyDataBuffer();
	pInfo->iInfoClass				=   INFO_DATA;
	pInfo->iInfoDetail				=	INFO_CLOSETABLE;
	pInfo->iBatchOrder				=	iOrder;
	pInfo->iBatchOrderSecond		=	0;
	SendInfo();

	return true;

}
*/
bool CConnection::Accept(CTranSocket *pListeningSocket)
{
	if(m_pSocket)
	{
		m_pSocket->Close ();
		delete m_pSocket;
	}
	m_pSocket=new CTranSocket;

	// Create a new socket corresponding to the
	// coming socket, and attach it to pSocket.
	// and m_pListeningSocket keeps listening.
	SOCKADDR addr;
	int iAddrLen=sizeof(SOCKADDR);
    TRACEPROGRESS("准备接受新连接...");

	if(pListeningSocket->Accept(*m_pSocket,&addr,&iAddrLen))
	{
		SetLinker("",&addr);
		m_pSocket->SetConnection (this);
		CString sPeerIP="";
		UINT iPeerPort=0;
		GetPeerIP(sPeerIP,iPeerPort);
		TRACEPROGRESS("已经接受连接:"+sPeerIP+":"+vtos((int)iPeerPort));
		return true;
	}
	else
	{
		TRACEERROR("无法接受新连接!");
		delete m_pSocket;
		return false;
	}

}

bool CConnection::ConnectServer(CServer &server)
{
	if(m_pSocket)
	{
		m_pSocket->Close ();
		delete m_pSocket;
	}

	m_pSocket=new CTranSocket;
	if(!m_pSocket->Create ())
	{
		AfxMessageBox("无法创建套接字!");
		delete m_pSocket;
		return false;
	}

	TRACEPROGRESS("请等待，正在连接服务器:"+CString(server.sIP)+":"+vtos(server.iPort));

	while(!m_pSocket->Connect (server.sIP ,server.iPort))
	{
		if(AfxMessageBox("无法连接到服务器"+CString(server.sIP)+":"+vtos(server.iPort)+",是否重试?",MB_YESNO)==IDNO)
		{
			delete m_pSocket;
			m_pSocket=NULL;
			return false;
		}
	};
	TRACEPROGRESS("服务器已经连接好:"+CString(server.sIP)+":"+vtos(server.iPort));

	SetLinker (server.sName ,server.sIP,server.iPort);
	m_pSocket->SetConnection (this);

	SetServer(server);

	VerifyUser();

	return true;

}

void CConnection::GetLocalIP(CString &sIP, UINT &iPort)
{
    if(IsServer())
    {
        GetPar("GLOBAL_LOCAL_IP",sIP);
        GetPar("GLOBAL_LOCAL_PORT",iPort );
    }
    else
    {	
        m_pSocket->GetSockName (sIP,iPort);
    }
}

void CConnection::GetPeerIP(CString &sIP, UINT &iPort)
{
    m_pSocket->GetPeerName (sIP,iPort);
}

void CConnection::VerifyUser()
{
	m_bUserVerified=false;

	CDataInfo *pInfo	= GetInfoBuffer();
	pInfo->iInfoDir		= INFO_HAND_IN;
	pInfo->iInfoClass	= INFO_MSG;
	pInfo->iInfoDetail	= INFO_PASSWORD;
	pInfo->iDataLen		= 0;
	pInfo->iTargetPort  = m_server .iPort;
	strcpy(pInfo->sTargetIP,m_server .sIP );
	
	CString sIP="";
	UINT iPort=0;
	GetLocalIP (sIP,iPort);
	strcpy(pInfo->sSourceIP	,(LPCTSTR)sIP);
	pInfo->iSourcePort = iPort;

	strcpy(pInfo->sUser,m_server.sUserName );
	strcpy(pInfo->sPassword ,m_server.sPassword );

	SendInfo();
}

void CConnection::GetPics(CStringArray *pArrayFields,CStringArray *pArrayPics)
{
	CString sFileName,sExt;
	pArrayPics->RemoveAll ();
	for(int i=0;i<pArrayFields->GetCount ();i++)
	{
		sFileName=pArrayFields->GetAt (i);
		if(IsPicFile(sFileName))
		{
			pArrayPics->Add (sFileName);
		}
	}

	return;
}

void CConnection::CopyBuffer(CConnection *pOtherConn)
{
	CopyDataToBuffer(pOtherConn->GetBuffer(),pOtherConn->GetBufferSize());
}

void CConnection::WantData(CString			sServer, 
						   UINT				iPort,
						   CString			sTable,
						   CString			sSQL,
						   CTableDate		dateStart,
						   CStringArray		*pArrayFields,
						   CDataBrowseDoc	*pDoc,
						   int				iMaxTransferRecord)

{


	CDataInfo *pInfo			= GetInfoBuffer ();
	pInfo->iInfoDir				= INFO_HAND_IN;
	pInfo->iInfoClass			= INFO_MSG;
	pInfo->iInfoDetail			= INFO_WANTDATA;
	pInfo->iBatchTotalQty		= 1;
	pInfo->iBatchOrderSecond	= 0;
	pInfo->iDataLen				= 0;
	pInfo->pBrowseDoc			= pDoc;
	pInfo->iMaxTransRecord		= iMaxTransferRecord;

	strcpy(pInfo->sTargetIP		, (LPCTSTR)sServer);
	pInfo->iTargetPort			= iPort;
	
	CString sLocalIP="";
	UINT iLocalPort=0;
	GetLocalIP(sLocalIP,iLocalPort);
	strcpy(pInfo->sSourceIP		, sLocalIP.GetBuffer ());
	pInfo->iSourcePort			= iLocalPort;

	strcpy(pInfo->sUser			, m_server.sUserName );
	strcpy(pInfo->sPassword		, m_server.sPassword );

	CStringArray array;
	array.Add  (sTable);
	array.Add  (sSQL);
	array.Add  (dateStart.sYearField );
	array.Add  (dateStart.sMonthField );
	array.Add  (dateStart.sDayField );
	array.Add  (dateStart.sYear );
	array.Add  (dateStart.sMonth );
	array.Add  (dateStart.sDay );

	for(int i=0;i<pArrayFields->GetCount ();i++)
	{
		array.Add  (pArrayFields->GetAt (i));
	}

	if(!SetSerItem (GetDataBuffer(),array))
	{
		return ;
	};

	SendData ();

	return;
}

void CConnection::SetUserVerified()
{
	m_bUserVerified=true;
}
/*
void CConnection::LocalUpdateRecord(	CString		sTable,
										int 		iRecordID,
										CStringArray &arrayFields,
										CStringArray &arrayValues)
{
	CString sIDField=GetIDFieldName();
	char sCondition[1000];
	sprintf(sCondition," %s = %d ",sIDField, iRecordID);

	m_db.Update (sTable,sCondition,arrayFields,arrayValues);
}

void CConnection::LocalAddRecord(	CString		 sTable,
									CStringArray &arrayFields,
									CStringArray &arrayValues)
{
	m_db.AddRecord (sTable,arrayFields,arrayValues);
}

void CConnection::LocalDeleteRecord(CString sTable,
									int iRecordID)
{
	CString sIDField=GetIDFieldName();
	char sCondition[1000];
	sprintf(sCondition," %s = %d ",sIDField, iRecordID);
	m_db.DeleteRecord (sTable,sCondition);
}
*/
void CConnection::SetBaseInfo()
{
	CDataInfo *pInfo			= GetInfoBuffer();
	pInfo->iInfoDir				= INFO_HAND_IN;
	pInfo->iInfoClass			= INFO_DATA;
	pInfo->iInfoDetail			= INFO_PASSWORD;  // Modify this after called SetBaseInfo
	pInfo->iBatchTotalQty		= 1;
	pInfo->iBatchOrder			= 0;
	pInfo->iBatchOrderSecond	= 0;
	pInfo->iGlobalOrder ++ ;

	CString sLocalIP="";
	UINT iLocalPort=0;
	GetLocalIP(sLocalIP,iLocalPort);

	strcpy(pInfo->sSourceIP ,(LPCTSTR) sLocalIP);
	pInfo->iSourcePort			= iLocalPort;

	pInfo->iMaxTransRecord		= GetMaxHandInRecord();
	
	strcpy(pInfo->sUser ,			m_server.sUserName );
	strcpy(pInfo->sPassword ,		m_server.sPassword );
	return;

}

CString CDataInfo::GetInfoString()
{
	if(iInfoDetail==INFO_PASSWORD)
	{
		return "INFO_PASSWORD";
	}
	else if(iInfoDetail==INFO_WANTDATA)
	{
		return "INFO_WANTDATA";
	}
	else if(iInfoDetail==INFO_CHILD_START)
	{
		return "INFO_CHILD_START";
	}

	else if(iInfoDetail==INFO_CHILD_END)
	{
		return "INFO_CHILD_END";
	}

	else if(iInfoDetail==INFO_VERIFIED)
	{
		return "INFO_VERIFIED";
	}

	else if(iInfoDetail==INFO_FILE_START)
	{
		return "INFO_FILE_START";
	}

	else if(iInfoDetail==INFO_FILE_APPEND)
	{
		return "INFO_FILE_APPEND";
	}

	else if(iInfoDetail==INFO_FILE_END)
	{
		return "INFO_FILE_END";
	}

	else if(iInfoDetail==INFO_WANTDATA)
	{
		return "INFO_WANTDATA";
	}

	else if(iInfoDetail==INFO_WANTDATA)
	{
		return "INFO_WANTDATA";
	}

	else if(iInfoDetail==INFO_SERVER_CONNECTED)
	{
		return "INFO_SERVER_CONNECTED";
	}

	else if(iInfoDetail==INFO_OTHER_SERVER_CONNECTED)
	{
		return "INFO_OTHER_SERVER_CONNECTED";
	}

	else if(iInfoDetail==INFO_CHILD_REMOVED)
	{
		return "INFO_CHILD_REMOVED";
	}

	else if(iInfoDetail==INFO_OTHER_SERVER_ENDED)
	{
		return "INFO_OTHER_SERVER_ENDED";
	}

	else if(iInfoDetail==ERROR_DBINIT)
	{
		return "ERROR_DBINIT";
	}

	else if(iInfoDetail==ERROR_DBDISCONNECT)
	{
		return "ERROR_DBDISCONNECT";
	}

	else if(iInfoDetail==ERROR_OPENTABLE)
	{
		return "ERROR_OPENTABLE";
	}

	else if(iInfoDetail==ERROR_APPENDRECORD)
	{
		return "ERROR_APPENDRECORD";
	}

	else if(iInfoDetail==ERROR_CLOSETABLE)
	{
		return "ERROR_CLOSETABLE";
	}

	else if(iInfoDetail==ERROR_GETRECORD)
	{
		return "ERROR_GETRECORD";
	}

	else if(iInfoDetail==INFO_SERVER_CONNECTED)
	{
		return "INFO_SERVER_CONNECTED";
	}

	else if(iInfoDetail==ERROR_DELETERECORD)
	{
		return "ERROR_DELETERECORD";
	}

	else if(iInfoDetail==ERROR_UPDATERECORD)
	{
		return "ERROR_UPDATERECORD";
	}

	else if(iInfoDetail==ERROR_CREATEFILE)
	{
		return "ERROR_CREATEFILE";
	}

	else if(iInfoDetail==ERROR_APPENDDATA)
	{
		return "ERROR_APPENDDATA";
	}

	else if(iInfoDetail==ERROR_CLOSEFILE)
	{
		return "ERROR_CLOSEFILE";
	}

	else if(iInfoDetail==ERROR_OTHER_SERVER_UNCONNECTED)
	{
		return "ERROR_OTHER_SERVER_UNCONNECTED";
	}

	else if(iInfoDetail==ERROR_OTHER_SERVER_FAILEDEND)
	{
		return "ERROR_OTHER_SERVER_FAILEDEND";
	}

	else if(iInfoDetail==ERROR_INVALIDUSER)
	{
		return "ERROR_INVALIDUSER";
	}

	else
	{
		return "未知信息";
	}

};


void CConnection::RecordInfo()
{
	static CStringArray arrayData;
	static CTime time;
	static CString sTime;
	static char sData[500];
	static int iLen=0;
	static int iLenLimit=490;
	static CDataInfo *pInfo=NULL;
	pInfo=GetInfoBuffer();


	time=CTime::GetCurrentTime ();
	sTime=time.Format ("%Y年%m月%d日%H时%M分%秒");

	arrayData.RemoveAll ();
	arrayData.SetSize (8);

	arrayData.SetAt(0,GetUniID());
	arrayData.SetAt(1,sTime);
	arrayData.SetAt(2,CombineIP(pInfo->sSourceIP,pInfo->iSourcePort ));
	arrayData.SetAt(3,pInfo->sUser );
	arrayData.SetAt(4,CombineIP(pInfo->sTargetIP,pInfo->iTargetPort ));
	CString sAction=pInfo->GetInfoString();
	if(sAction.GetLength ()>18)
	{
		sAction=sAction.Left (18);
	}
	arrayData.SetAt(5,sAction);

	if(pInfo->iDataLen>iLenLimit)
	{
		iLen=iLenLimit;
	}
	else
	{
		iLen=pInfo->iDataLen;
	}
	memset(sData,0,iLenLimit+4);
	memcpy(sData,GetDataBuffer(),iLen);
	ClearString(sData,500);
	if(strlen(sData)>45)
	{
		sData[45]=0;
	}
	arrayData.SetAt (6,sData);

	// 记录是不是父连接:
	if(this->m_pDataTransfer )
	{
		POSITION pos=m_pDataTransfer->m_FathersList.GetHeadPosition ();
		CString sIP="";
		UINT iPort=0;
		arrayData.SetAt (7,"N");
		while(pos)
		{
			CConnection *pConn=(CConnection *)(m_pDataTransfer->m_FathersList.GetNext (pos));
			pConn->GetPeerIP (sIP,iPort);
			if(sIP==pInfo->sSourceIP)
			{
				arrayData.SetAt (7,"Y");
				break;
			}
		}
	}

	//
	if(this->m_pDataTransfer )
	{
		m_pDataTransfer->RecordHistory (arrayData);
	}

}

void CConnection::CloseConnection()
{
	if(m_pDataTransfer)
	{
		m_pDataTransfer->CloseConnection(this);
	}
}

void CConnection::SetServer(CServer &server)
{
	m_server=server;
}