// DataBrowseDoc.cpp : implementation file
//

#include "stdafx.h"
#include "QCDB.h"
#include "DataBrowseDoc.h"
#include "fhcglobal.h"
#include "FHMainFrm.h"
#include "qcglobal.h"
#include "DlgEditShowProperty.h"
#include "DlgDBPicProperty.h"
#include "DlgDBShowProperty.h"
#include "DlgDataSelector.h"
#include "QCReportReader.h"
#include "DataBrowseView.h"


// CDataBrowseDoc

IMPLEMENT_DYNCREATE(CDataBrowseDoc, CDocument)

CDataBrowseDoc::CDataBrowseDoc()
{
	m_sDBFileName="";

	m_pFieldsData=NULL;

	m_iTempBufferLen=10000;
	m_pTempBuffer=new char[m_iTempBufferLen];
	memset(m_pTempBuffer,0,m_iTempBufferLen);

	m_iRecordStatusQty=10000;
	m_pRecordStatus=new char[m_iRecordStatusQty+16];
	memset(m_pRecordStatus,0,m_iRecordStatusQty);

	m_iMarkFieldOrder=-1;

	m_fp=NULL;


	m_sBigPicName="";
	
	m_iPicHeight		= 100;
	m_iPicWidth			= 50;
	m_bPicKeepRatio		= true;
	m_bDrawCellPic		= true;

	m_bDrawBigPic		= true;
	m_iBigPicWidth		= 200;
	m_iBigPicHeight		= 200;
	m_bBigPicKeepRatio	= true;
	m_iBigPicOffsetX	= 25;
	m_iBigPicOffsetY	= 25;

	m_iDBPicWidth		=200;
	m_iDBPicHeight		=200;

	GetPar("DataBrowseDoc_PicHeight",		m_iPicHeight);
	GetPar("DataBrowseDoc_PicWidth",		m_iPicWidth);
	GetPar("DataBrowseDoc_KeepRatio",		m_bPicKeepRatio);
	GetPar("DataBrowseDoc_DrawCellPic",		m_bDrawCellPic);

	GetPar("DataBrowseDoc_DrawBigPic",		m_bDrawBigPic);
	GetPar("DataBrowseDoc_BigPicOffsetX",	m_iBigPicOffsetX);
	GetPar("DataBrowseDoc_BigPicOffsetY",	m_iBigPicOffsetY);
	GetPar("DataBrowseDoc_BigPicWidth",		m_iBigPicWidth);
	GetPar("DataBrowseDoc_BigPicHeight",	m_iBigPicHeight);
	GetPar("DataBrowseDoc_BigPicKeepRatio",	m_bBigPicKeepRatio);

	GetPar("DataBrowseDoc_DBPicKeepRatio",	m_bDBPicKeepRatio);
	GetPar("DataBrowseDoc_DBPicWidth",		m_iDBPicWidth);
	GetPar("DataBrowseDoc_DBPicHeight",		m_iDBPicHeight);
}

BOOL CDataBrowseDoc::OnNewDocument()
{
	if (!CDocument::OnNewDocument())
		return FALSE;

	CString sFile=GetTempFile();

	if(!CreateDBFile(sFile))
	{
		return false;
	};

	return TRUE;
}

CDataBrowseDoc::~CDataBrowseDoc()
{


	/////////////////////////////////
	// Remove Pars:
	/////////////////////////////////
	if(m_pFieldsData)
	{
		delete []m_pFieldsData;
		m_pFieldsData=NULL;
	}

	if(m_pTempBuffer)
	{
		delete []m_pTempBuffer;
		m_pTempBuffer=NULL;
	}

	if(m_pRecordStatus)
	{
		delete []m_pRecordStatus;
		m_pRecordStatus=NULL;
	}

	if(m_fp)
	{
		fclose(m_fp);
		m_fp=NULL;
	}

}


BEGIN_MESSAGE_MAP(CDataBrowseDoc, CDocument)
	ON_COMMAND(ID_SetPicProperty, OnSetPicProperty)
	ON_COMMAND(ID_ModifyShowProperty, OnModifyData)	
	ON_COMMAND(ID_SendDataToServerNoHanded, OnSenddatatoservernohanded)
	ON_COMMAND(ID_SendDataToServerAll, OnSenddatatoserverall)

	ON_COMMAND(ID_FILE_SAVE, OnFileSave)

END_MESSAGE_MAP()


// CDataBrowseDoc diagnostics

#ifdef _DEBUG
void CDataBrowseDoc::AssertValid() const
{
	CDocument::AssertValid();
}

void CDataBrowseDoc::Dump(CDumpContext& dc) const
{
	CDocument::Dump(dc);
}
#endif //_DEBUG


// CDataBrowseDoc serialization

void CDataBrowseDoc::Serialize(CArchive& ar)
{
	if (ar.IsStoring())
	{
		// TODO: add storing code here
	}
	else
	{
		// TODO: add loading code here
	}
}


// CDataBrowseDoc commands

BOOL CDataBrowseDoc::OnOpenDocument(LPCTSTR lpszPathName)
{
	if (!CDocument::OnOpenDocument(lpszPathName))
		return FALSE;

	return OpenDBFile(lpszPathName);
}


bool CDataBrowseDoc::CreateDBFile(CString sFileName)
{
	if(sFileName=="")
	{
		sFileName=GetTempFile();	
	}

	if(m_fp)
	{
		fclose(m_fp);
		m_fp=NULL;
	}
	m_fp=fopen(sFileName,"wb+");

	if(!m_fp)
	{
		return false;
	}
	
	m_head.iRecordStartPos	= 0;
	m_head.iFieldQty		= 0;
	m_head.iRecordQty		= 0;
	m_head.iRecordLen		= 0;
	memset(m_pRecordStatus,0,m_iRecordStatusQty);

	m_sDBFileName=sFileName;

	return true;
}

bool CDataBrowseDoc::SetFieldQty(int iFieldQty)
{
	if(iFieldQty<1||iFieldQty>1000)return false;

	if(m_pFieldsData)
	{
		delete []m_pFieldsData;
		m_pFieldsData=NULL;
	}
	m_pFieldsData=new CFieldData [ iFieldQty + 2 ];
	m_head.iFieldQty =iFieldQty;

	return true;
}

bool CDataBrowseDoc::SetField(CStringArray &arrayName, CStringArray &arrayType)
{
	if(arrayName.GetCount ()!=arrayType.GetCount ())
	{
		TRACEERROR("字段名称和字段类型数量不一致!");
		return false;
	}

	SetFieldQty(arrayName.GetCount ());

	for(int i=0;i<arrayName.GetCount ();i++)
	{
		SetField(i,arrayName.GetAt (i),arrayType.GetAt (i));
	}

	CalcFieldPosition();
	WriteFileHead();

	return true;
}

bool CDataBrowseDoc::SetField(int iOrder, CString sName, CString sType)
{
	if(!m_pFieldsData||iOrder<0||iOrder>=m_head.iFieldQty)
	{
		return false;
	}

	enumDataType type;
	int iLen=0;
	int n=0;

	sType.MakeUpper ();
	if(sType=="INTEGER")
	{
		iLen=sizeof(int);
		type= DT_INTEGER;
	}
	else if(sType=="LONG")
	{
		iLen=sizeof(long);
		type= DT_LONG;
	}
	else if(sType=="FLOAT")
	{
		iLen=sizeof(float);
		type= DT_FLOAT;
	}
	else if(sType=="DOUBLE")
	{
		iLen=sizeof(double);
		type= DT_DOUBLE;
	}
	else if(sType=="DATE")
	{
		iLen=14;
		type= DT_DATE;
	}
	else if(sType=="BLOG")
	{
		iLen=2048; // save the file name for the blog;
		type= DT_BLOG;
	}
	else if(sType=="BOOL")
	{
		iLen=1; // save the file name for the blog;
		type= DT_BOOL;
	}
	else if(sType.Left (4) =="CHAR")
	{
		type= DT_CHAR;
		iLen=GetNumberInBracket(sType);
		if(iLen<1)
		{
			ASSERT(false);
			return false;
		}
	}
	else if(sType.Left (4) =="TEXT")
	{
		type= DT_TEXT;
		iLen=GetNumberInBracket(sType);
		if(iLen<1)
		{
			ASSERT(false);
			return false;
		}
	}

	else if(sType.Left (4) =="NOTE")
	{
		type= DT_NOTE;  // save the file name for the blog;
		iLen=2048;
	}

	else if(sType=="求和"||sType=="项目累计"||sType=="单元累计")
	{
		iLen=sizeof(double);
		type= DT_DOUBLE;
	}

	else
	{
		ASSERT(false);
		type= DT_NOTE;  // save the file name for the blog;
		iLen=50;
	}

	m_pFieldsData[iOrder].Set(sName,type,iLen);
	
	CalcFieldPosition();

	return true;
}

bool CDataBrowseDoc::CalcFieldPosition()
{
	if(!m_pFieldsData)
	{
		return false;
	}
	
	m_pFieldsData[0].iStartPos =0;
	for(int i=1;i<m_head.iFieldQty;i++)
	{
		m_pFieldsData[i].iStartPos = 
				 m_pFieldsData[i-1].iStartPos  
				+m_pFieldsData[i-1].iLen ;
	}


	m_head.iRecordStartPos =sizeof(CDBFileHead)
							+m_head.iFieldQty*sizeof(CFieldData);

	m_head.iRecordLen=	 m_pFieldsData[m_head.iFieldQty-1].iStartPos
					+m_pFieldsData[m_head.iFieldQty-1].iLen ;

	return true;
}

bool CDataBrowseDoc::WriteFileHead()
{
	if(!m_fp)
	{
		return false;
	}

	if(m_sDBFileName=="")
	{
		return false;
	}

	///////////////////////////////////
	//
	///////////////////////////////////
	fseek(m_fp,0,SEEK_SET);
	fwrite(&m_head,sizeof(CDBFileHead),1,m_fp);

	fwrite(m_pFieldsData,sizeof(CFieldData),m_head.iFieldQty ,m_fp);
	fflush(m_fp);

	return true;
}

int CDataBrowseDoc::GetNumberInBracket(CString sData)
{
	static int iStart=-1;
	static int iEnd=-1;
	static iLen=-1;
	CString sLen;

	iStart=sData.Find ('(');
	iEnd=sData.Find (')');
	if(iStart==-1||iEnd==-1||iEnd<=(iStart+1))
	{
		ASSERT(false);
		return false;
	}
	
	sLen=sData.Mid (iStart+1,iEnd-iStart-1);
	iLen=0;
	sscanf((LPCTSTR)sLen,"%d",&iLen);

	return iLen;
}

int CDataBrowseDoc::AppendRecord(CStringArray *pArrayRecord)
{
	int iOrder=GetRecordQty();
	if(!UpdateRecord(iOrder,pArrayRecord))
	{
		return -1;
	}
	else
	{
		return iOrder;
	}
}

bool CDataBrowseDoc::UpdateField(int iRecord, int iField, CString sField)
{
	if(!m_fp)
	{
		return false;
	}

	if(m_sDBFileName=="")
	{
		return false;
	}

	if(iRecord<0||iRecord>=m_head.iRecordQty )
	{
		return false;
	}

	if(iField<0||iField>=m_head.iFieldQty )
	{
		return false;		
	}

	static long lPos=0;
	lPos=		 m_head.iRecordStartPos 
				+m_head.iRecordLen *iRecord
				+m_pFieldsData[iField].iStartPos ;
	fseek(m_fp,lPos,SEEK_SET);


	static int n;
	memset(m_pTempBuffer,0,m_head.iRecordLen);
	n=WriteField(m_pTempBuffer,sField,&m_pFieldsData[iField]);
	fwrite(&m_pTempBuffer[m_pFieldsData[iField].iStartPos ] ,1,n,m_fp);

	fflush(m_fp);

	
	return true;

}


bool CDataBrowseDoc::UpdateRecord(int iOrder, CStringArray *pArrayRecord)
{
	if(!m_fp)
	{
		return false;
	}

	if(m_sDBFileName=="")
	{
		return false;
	}

	if(pArrayRecord->GetCount ()!=m_head.iFieldQty )
	{
		return false;		
	}

	fseek(m_fp,m_head.iRecordStartPos +m_head.iRecordLen *iOrder,SEEK_SET);

	if(m_head.iRecordLen>m_iTempBufferLen)
	{
		delete []m_pTempBuffer;
		m_iTempBufferLen=m_head.iRecordLen;
		m_pTempBuffer=new char [m_iTempBufferLen+16];
	}

	static int i,n;
	memset(m_pTempBuffer,0,m_head.iRecordLen);
	for(i=0;i<m_head.iFieldQty ;i++)
	{
		n=WriteField(m_pTempBuffer,pArrayRecord->GetAt (i),&m_pFieldsData[i]);
	}
	n=fwrite(m_pTempBuffer,m_head.iRecordLen ,1,m_fp);

	fflush(m_fp);

	////////////////////////////
	//
	if(iOrder>=m_iRecordStatusQty)
	{
		int iOldQty=m_iRecordStatusQty;
		char *pTemp=new char[iOldQty+10];
		memcpy(pTemp,m_pRecordStatus,iOldQty);
		delete []m_pRecordStatus;

		m_iRecordStatusQty=iOrder+1000;
		m_pRecordStatus=new char [m_iRecordStatusQty];
		memset(m_pRecordStatus,0,m_iRecordStatusQty);
		memcpy(m_pRecordStatus,pTemp,iOldQty);

		delete []pTemp;
	}

	if(iOrder>=m_head.iRecordQty )
	{
		m_head.iRecordQty =iOrder+1;
	}
	
	return true;
}

int CDataBrowseDoc::WriteField(	char		*psBuffer,
								CString		sField,
								CFieldData	*pFieldData)
{
	static int iData;
	static float fData;
	static long lData;
	static double lfData;
	static int iLen=0;
	static int n;
	static CString sLeft;
	
	if(sField.GetLength ()==0)
	{
		return 0;
	}

	if(sField=="")
	{
		return 0;
	}

	int ii=sField.GetLength ();

	iData=0;
	fData=0.0;
	lData=0;
	lfData=0;
	n=0;
	iLen=0;

	iLen=pFieldData->iLen ;
	enumDataType type =pFieldData->enumType ;

	char * psTBuffer=&psBuffer[pFieldData->iStartPos ];

	if(type==DT_NOTE||type==DT_BLOG||type==DT_CHAR||type==DT_TEXT)
	{
		memset(psTBuffer,0,iLen);
		n=sField.GetLength ();
		if(n>iLen)
		{
			n=iLen;
		}
		strncpy(psTBuffer,(LPCTSTR)sField,n);
	}
	else if(type==DT_INTEGER)
	{
		sscanf((LPCTSTR)sField,"%d",&iData);
		memcpy(psTBuffer,&iData,iLen);
	}
	else if(type==DT_BOOL)
	{
		sLeft=sField.Left(1);
		if(	sLeft=="N"||sLeft=="n"||
			sLeft=="F"||sLeft=="f"||
			sLeft=="0")
		{
			iData=0;
		}
		else
		{
			iData=1;
		}
		iLen=sizeof(int);
		memcpy(psTBuffer,&iData,iLen);
	}
	else if(type==DT_LONG)
	{
		sscanf((LPCTSTR)sField,"%ld",&lData);
		memcpy(psTBuffer,&iData,iLen);
	}
	else if(type==DT_FLOAT)
	{
		sscanf((LPCTSTR)sField,"%f",&fData);
		memcpy(psTBuffer,&fData,iLen);
	}
	else if(type==DT_DOUBLE)
	{
		sscanf((LPCTSTR)sField,"%lf",&lfData);
		memcpy(psTBuffer,&lfData,iLen);
	}
	else if(type==DT_DATE)
	{
		memcpy(psTBuffer,(LPCTSTR)sField,iLen);
	}
	else
	{
		ASSERT(false);
		iLen=sizeof(long);
	};

	return iLen;
}

CString CDataBrowseDoc::ReadField(	char		*psBuffer,
							CFieldData	*pFieldData)
{
	static int		iData;
	static float	fData;
	static long		lData;
	static double	lfData;
	static int		iLen=0;
	static int		n;
	static CString	sData;
	static char		cDataBuffer[16];

	char *psTBuffer=&psBuffer[pFieldData->iStartPos ];


	iLen=pFieldData->iLen ;
	enumDataType type =pFieldData->enumType ;

	if(type==DT_NOTE||type==DT_BLOG||type==DT_CHAR||type==DT_TEXT)
	{
		if(psTBuffer[iLen-1]!=0)
		{
			psTBuffer[iLen-1]=0;
		}
		sData=psTBuffer;
	}
	else if(type==DT_INTEGER)
	{
		memcpy(&iData,psTBuffer,iLen);
		sprintf(cDataBuffer,"%d",iData);
		sData=cDataBuffer;
	}
	else if(type==DT_BOOL)
	{
		memcpy(&iData,psTBuffer,iLen);
		if(iData!=0)
		{
			sData="1";
		}
		else
		{
			sData="0";
		}
	}

	else if(type==DT_LONG)
	{
		memcpy(&lData,psTBuffer,iLen);
		sprintf(cDataBuffer,"%ld",lData);
		sData=cDataBuffer;
	}
	else if(type==DT_FLOAT)
	{
		memcpy(&fData,psTBuffer,iLen);
		sData=vtos(fData);
	}
	else if(type==DT_DOUBLE)
	{
		memcpy(&lfData,psTBuffer,iLen);
		sData=vtos(lfData);
	}
	else if(type==DT_DATE)
	{
		if(psTBuffer[iLen-1]!=0)
		{
			psTBuffer[iLen-1]=0;
		}
		sData=psTBuffer;
	}
	else
	{
		sData="ERROR";
		ASSERT(false);
	};


	return sData;
}

bool CDataBrowseDoc::OpenDBFile(CString sFileName)
{	
	if(sFileName=="")return false;
	if(m_fp)
	{
		fclose(m_fp);
		m_fp=NULL;
	}
	m_fp=fopen(sFileName,"rb+");
	if(!m_fp)
	{
        TRACEERROR("无法打开文件:"+sFileName);
		return false;
	}
	
	m_sDBFileName=sFileName;

	if(!ReadFileHead())
	{
        TRACEERROR(sFileName+"不是数据浏览文件!");
		return false;
	}

	CalcMarkFieldOrder();

	return true;
}

bool CDataBrowseDoc::ReadFileHead()
{
	if(!m_fp)
	{
		return false;
	}

	if(m_sDBFileName=="")
	{
		return false;
	}

	///////////////////////////////////
	//
	///////////////////////////////////
	fseek(m_fp,0,SEEK_SET);
	fread(&m_head,sizeof(CDBFileHead),1,m_fp);
	if(!m_head.IsValid())return false;


	if(!SetFieldQty(m_head.iFieldQty ))
	{
		return false;
	}

	fread(m_pFieldsData,sizeof(CFieldData),m_head.iFieldQty,m_fp);

	CalcFieldPosition();
	m_head.iRecordQty=CalcRecordQty();

	return true;
}


CStringArray * CDataBrowseDoc::GetRecord(int iRecordOrder)
{
	static int i;
	static long lPos=0;
	static int iLen=0;
	if(!m_fp)
	{
		return false;
	}

	lPos=m_head.iRecordStartPos +iRecordOrder*m_head.iRecordLen ;
	fseek(m_fp,lPos,SEEK_SET);


	if(m_head.iRecordLen >m_iTempBufferLen)
	{
		delete []m_pTempBuffer;
		m_iTempBufferLen=m_head.iRecordLen;
		m_pTempBuffer=new char [m_iTempBufferLen+16];
	}

	int nRead=fread(m_pTempBuffer,1,m_head.iRecordLen,m_fp);

	m_arrayRecord.RemoveAll ();
	for(i=0;i<m_head.iFieldQty;i++)
	{
		m_arrayRecord.Add (ReadField(m_pTempBuffer,&m_pFieldsData[i]));
	}
	return &m_arrayRecord;
}

int CDataBrowseDoc::GetRecordQty()
{
	return m_head.iRecordQty ;
}

int CDataBrowseDoc::CalcRecordQty()
{
	if(!m_fp)
	{
		return 0;
	}
	fseek(m_fp,0,SEEK_END);
	int iLen=ftell(m_fp);
	if(iLen==0)
	{
		return 0;
	}

	int iRecordLen=GetRecordLen();
	if(iRecordLen<1)
	{
		return 0;
	}

	m_head.iRecordQty =(iLen-m_head.iRecordStartPos)/GetRecordLen();
	if((m_head.iRecordQty*m_head.iRecordLen+m_head.iRecordStartPos)!=iLen)
	{
		return 0;
	}

	return m_head.iRecordQty;
}

int CDataBrowseDoc::GetRecordLen()
{
	return m_head.iRecordLen;
}

int CDataBrowseDoc::GetFieldQty()
{
	return m_head.iFieldQty ;
}

CFieldData * CDataBrowseDoc::GetFields()
{
	return m_pFieldsData;
}

bool CDataBrowseDoc::SetDeletedFlag(int iRecord)
{
	if(iRecord <0||iRecord>=m_head.iRecordQty)
	{
		return false;
	}
	
	m_pRecordStatus[iRecord]=m_pRecordStatus[iRecord]|RS_DELETED;

	return true;
}

bool CDataBrowseDoc::ResetDeletedFlag(int iRecord)
{
	if(iRecord <0||iRecord>=m_head.iRecordQty)
	{
		return false;
	}
	
	m_pRecordStatus[iRecord]=m_pRecordStatus[iRecord]&(~RS_DELETED);

	return true;
}

bool CDataBrowseDoc::SetUpdatedFlag(int iRecord)
{
	if(iRecord <0||iRecord>=m_head.iRecordQty)
	{
		return false;
	}
	
	m_pRecordStatus[iRecord]=m_pRecordStatus[iRecord]|RS_MODIFIED;

	return true;
}

bool CDataBrowseDoc::SetAddedFlag(int iRecord)
{
	if(iRecord <0||iRecord>=m_head.iRecordQty)
	{
		return false;
	}
	
	m_pRecordStatus[iRecord]=m_pRecordStatus[iRecord]|RS_ADDED;

	return true;
}

bool CDataBrowseDoc::AddEmptyRecord()
{
	int iFieldQty=GetFieldQty();
	CStringArray array;
	int i=0;

	char sID[100];
	memset(sID,0,100);
	CString sIDString=GetIDFieldName();
	memcpy(sID,(LPCTSTR)sIDString,sIDString.GetLength());
	CString sCurrID;

	for(i=0;i<iFieldQty;i++)
	{
		if(strcmp(m_pFieldsData[i].sFieldName ,sID)==0)
		{
			sCurrID=GetUniID();
			array.Add (sCurrID);
		}
		else
		{
			array.Add (CString(""));
		}
	}

	int iOrder=AppendRecord(&array);
    if(iOrder!=-1)
	{
		SetAddedFlag(iOrder);
		return true;
	}
	else
	{
		return true;
	}
}

bool CDataBrowseDoc::CloneLastRecord()
{
    int iFieldQty=GetFieldQty();
    CStringArray array;
    int i=0;

    char sID[100];
    memset(sID,0,100);
    CString sIDString=GetIDFieldName();
    memcpy(sID,(LPCTSTR)sIDString,sIDString.GetLength());
    CString sCurrID;

    CStringArray *pArrayLastRecord=NULL;
    if(m_head.iRecordQty>0)
    {
        pArrayLastRecord=GetRecord(m_head.iRecordQty-1);
    }

    for(i=0;i<iFieldQty;i++)
    {
        if(strcmp(m_pFieldsData[i].sFieldName ,sID)==0)
        {
            sCurrID=GetUniID();
            array.Add (sCurrID);
        }
        else
        {
            if(pArrayLastRecord)
                array.Add (pArrayLastRecord->GetAt(i));
            else
                array.Add ("");
        }
    }

    int iOrder=AppendRecord(&array);
    if(iOrder!=-1)
    {
        SetAddedFlag(iOrder);
        return true;
    }
    else
    {
        return true;
    }
}

CString CDataBrowseDoc::GetTableName()
{
	return m_head .sTableName ;
}

bool CDataBrowseDoc::SetTableName(CString sTable)
{
	if(sTable.GetLength ()>127)
	{
		TRACEERROR("表名字太长,不能大于127个字节:"+sTable);
		return false;
	}
	strcpy(m_head .sTableName ,(LPCTSTR)sTable);

	this->SetTitle (sTable);

	return true;
}

void CDataBrowseDoc::UpdateRecordStatus(int iRecord,int iAdded,int iRemoved)
{
	if(iRecord<0||iRecord>=m_head.iRecordQty)
	{
		return ;
	}
	m_pRecordStatus[iRecord]|=iAdded;
	m_pRecordStatus[iRecord]&=~iRemoved;

	return;
}

char CDataBrowseDoc::GetRecordStatus(int iRecord)
{
	if(iRecord<0||iRecord>=m_head.iRecordQty)
	{
		return 0;
	}
	return m_pRecordStatus[iRecord];
}

CString CDataBrowseDoc::GetFileName()
{
	return m_sDBFileName;
}

void CDataBrowseDoc::GetFields(CStringArray &array)
{
	for(int i=0;i<GetFieldQty();i++)
	{
		array.Add (m_pFieldsData[i].sFieldName );
	}

	return ;
}

void CDataBrowseDoc::SetSQL(CString sSQL)
{
	if(sSQL.GetLength ()>1999)
	{
		sSQL=sSQL.Left (1999);
	}
	strcpy(m_head.sSQL,(LPCTSTR)sSQL);
}

CString CDataBrowseDoc::GetSQL()
{
	return m_head.sSQL;
}

void CDataBrowseDoc::SendDataToServer(bool bOnlyNotHandedIn)
{
	//////////////////////////////////////////////////////
	// Check if the data transfer has been initiated:
	//////////////////////////////////////////////////////
	CDataTransfer *pTr=&(((CQCDBApp*)AfxGetApp())->m_dt) ;
	if(!pTr->Initiated())
	{
		AfxMessageBox("客户端尚未初始化!");
		return;
	}

	CString sSQL=GetSQL();
	if(bOnlyNotHandedIn)
	{
		CString sName,sYesValue;
		GetHandedInField(sName, sYesValue);

		sSQL+=" ";
		if(sSQL.Find ("where")!=-1)
		{
			sSQL+=" and ";
		}
		else
		{
			sSQL+=" where ";

		}
		sSQL+=sName+" <> '"+sYesValue+"' ";
	}

	CDataBrowseDoc doc;
	CDataBaseManager db;
	if(!db.SendTableToBrowseDoc (GetTableName(),sSQL,CTableDate(),NULL,&doc,::GetMaxHandDownRecord (),true))
	{
		return;
	};

	doc.SetFileType (DBFT_ADD);
    CServer server[200];
    int iServQty=::GetServers(server,200);
    if(iServQty>0) // iServQty should be 1, because client can connect only one server.
    {
	    doc.SetSourceIP(server[0].sIP);
	    doc.SetSourcePort(server[0].iPort);
    }
	doc.Close ();

	pTr->SendDataToServers (doc.GetFileName ());
}

void CDataBrowseDoc::UpdateDB()
{
	int i=0;
	char cStatus=0;
	CString sRecordID;
	CDataTransfer *pTr=&(((CQCDBApp*)AfxGetApp())->m_dt) ;
	CStringArray arrayFields;
	GetFields(arrayFields);
	int iOrder=CalcMarkFieldOrder();
	if(iOrder==-1)
	{
		return;
	}

	CDataBrowseDoc docUpdate;
	CDataBrowseDoc docAdd;
	CDataBrowseDoc docDelete;
	CString sDocUpdate	= docUpdate.Initiate(this);
	CString sDocAdd		= docAdd.Initiate(this);
	CString sDocDelete	= docDelete.Initiate(this);
	docUpdate.SetFileType (DBFT_UPDATE);
	docAdd.SetFileType (DBFT_ADD);
	docDelete.SetFileType (DBFT_DELETE);

	if(sDocUpdate==""||sDocAdd==""||sDocDelete=="")return;

	for(i=0;i<GetRecordQty();i++)
	{
		cStatus=GetRecordStatus(i);

		if((cStatus&RS_ADDED)==0&&(cStatus&RS_DELETED)==RS_DELETED)
		{
			docDelete.AppendRecord (GetRecord(i));
			this->ClearRecordStatus (i);
		}

		else if((cStatus&RS_ADDED)==0&&(cStatus&RS_MODIFIED)==RS_MODIFIED)
		{
			docUpdate.AppendRecord (GetRecord(i));
			this->ClearRecordStatus (i);			
		}

		else if((cStatus&RS_ADDED)==RS_ADDED&&(cStatus&RS_DELETED)==0)
		{
			if((cStatus&RS_MODIFIED)!=RS_MODIFIED)
			{
				TRACEERROR("本记录为新增记录,但是未经修改，不予加入数据库:"+vtos(i));
			}
			else
			{
				docAdd.AppendRecord (GetRecord(i));
				this->ClearRecordStatus (i);			
			}
		}
	}

	docDelete.Close ();
	docUpdate.Close ();
	docAdd.Close ();

	if(docDelete.GetRecordQty ()>0)
	{
		pTr->DeleteRecord (sDocDelete);
	}
	if(docUpdate.GetRecordQty ()>0)
	{
		pTr->UpdateRecord (sDocUpdate);
	}
	if(docAdd.GetRecordQty ()>0)
	{
		pTr->AddRecord (sDocAdd);
	}

	SetModifiedFlag(false);

	return;
}

CString CDataBrowseDoc::GetRecordID(int iRecord)
{
	if(iRecord<0||iRecord>=GetRecordQty())
	{
		return CString("");
	}
	if(m_iMarkFieldOrder==-1)return CString("");

	if(m_iMarkFieldOrder<0||m_iMarkFieldOrder>=GetFieldQty())return CString("");

	CStringArray *pRecord=GetRecord(iRecord);
	CString sID=pRecord->GetAt(m_iMarkFieldOrder);
	return sID;
}


int CDataBrowseDoc::CalcMarkFieldOrder()
{
	m_iMarkFieldOrder=-1;

	CString sMarkField=GetIDFieldName();
	CFieldData *pFieldData = GetFields();
	for(int i=0;i<GetFieldQty();i++)
	{
		if(sMarkField==pFieldData[i].sFieldName )
		{
			m_iMarkFieldOrder=i;
			break;
		}
	}
	if(m_iMarkFieldOrder==-1)
	{
		TRACEERROR("无法找到记录标志字段:"+sMarkField);
	}

	return m_iMarkFieldOrder;
}

CString	CDataBrowseDoc::GetSourceIP()
{
	return m_head.sSourceIP ;
}

int	CDataBrowseDoc::GetSourcePort()
{
	return m_head.iSourcePort;
}

CString	CDataBrowseDoc::GetTargetIP()
{
	return m_head.sTargetIP ;
}

int	CDataBrowseDoc::GetTargetPort()
{
	return m_head.iTargetPort;
}

void	CDataBrowseDoc::SetSourceIP(char sSourceIP[])
{
	strcpy( m_head.sSourceIP ,sSourceIP);
}

void	CDataBrowseDoc::SetSourceIP(CString  sSourceIP)
{
	strcpy( m_head.sSourceIP ,(LPCTSTR)sSourceIP);
}

void	CDataBrowseDoc::SetSourcePort(int iPort)
{
	m_head.iSourcePort=iPort;
}

void	CDataBrowseDoc::SetTargetIP(CString  sTargetIP)
{
	strcpy( m_head.sTargetIP ,(LPCTSTR)sTargetIP);
}

void	CDataBrowseDoc::SetTargetPort(int iPort)
{
	m_head.iTargetPort=iPort;
}

void CDataBrowseDoc::OnSetPicProperty()
{
	CPropertySheet sheet;
    sheet.SetTitle("图片显示属性");
	
	CDlgEditShowProperty dlg1;
	dlg1.m_iPicHeight		= m_iPicHeight;
	dlg1.m_iPicWidth		= m_iPicWidth;
	dlg1.m_bKeepRatio		= m_bPicKeepRatio;
	dlg1.m_bDrawCellPic		= m_bDrawCellPic ;

	dlg1.m_iBigPicWidth		= m_iBigPicWidth;
	dlg1.m_iBigPicHeight	= m_iBigPicHeight;
	dlg1.m_bBigPicKeepRatio	= m_bBigPicKeepRatio;
	dlg1.m_bDrawBigPic		= m_bDrawBigPic;
	dlg1.m_iBigPicOffsetX	= m_iBigPicOffsetX;
	dlg1.m_iBigPicOffsetY	= m_iBigPicOffsetY;


	CDlgDBPicProperty    dlg2;
	dlg2.m_bDBPicKeepRatio		= m_bDBPicKeepRatio;
	dlg2.m_iDBPicWidth			= m_iDBPicWidth;
	dlg2.m_iDBPicHeight			= m_iDBPicHeight;

	sheet.AddPage (&dlg1);
	sheet.AddPage (&dlg2);

	int nRet=sheet.DoModal();
	if(nRet==1)  // OK
	{
		m_iPicHeight		= dlg1.m_iPicHeight;
		m_iPicWidth			= dlg1.m_iPicWidth;
		m_bPicKeepRatio		= dlg1.m_bKeepRatio ;
		m_bDrawCellPic		= dlg1.m_bDrawCellPic ;

		m_iBigPicWidth		= dlg1.m_iBigPicWidth;
		m_iBigPicHeight		= dlg1.m_iBigPicHeight;
		m_bBigPicKeepRatio	= dlg1.m_bBigPicKeepRatio;
		m_bDrawBigPic		= dlg1.m_bDrawBigPic;
		m_iBigPicOffsetX	= dlg1.m_iBigPicOffsetX;
		m_iBigPicOffsetY	= dlg1.m_iBigPicOffsetY;

		m_bDBPicKeepRatio	= dlg2.m_bDBPicKeepRatio;
		m_iDBPicWidth		= dlg2.m_iDBPicWidth;
		m_iDBPicHeight		= dlg2.m_iDBPicHeight;

        SavePar();

		POSITION pos=this->GetFirstViewPosition ();
		while(pos)
		{
			CView *pView=this->GetNextView (pos);
			pView->SendMessage (WM_PICPROPERTY);
		}




	/*	m_Grid.SetCellPicProperty(m_iPicWidth,
					m_iPicHeight,
					m_bPicKeepRatio,
					m_bDrawCellPic);
*/
		//ScanPics();
		
		//Invalidate();
	}
}

void CDataBrowseDoc::OnModifyData()
{
	CPropertySheet sheet;
	sheet.SetWizardMode();
	
	CDlgDBShowProperty		page1; 
	CDlgDataSelector		page2;

	CStringArray arrayTables;
	GetTableNames(arrayTables);

	page1.SetTableNames (arrayTables);
	
	CString sField,sFieldCurrent;
	int j;
	bool bExist=false;
	CStringArray arrayFieldsLeft;
	CStringArray arrayFieldsAll,arrayFieldsType;
	GetFieldNames(					GetTableName(),
									arrayFieldsAll,
									arrayFieldsType);

	int iCurFieldsQty=GetFieldQty();
	CFieldData *pFields=GetFields ();
	for(int i=0;i<arrayFieldsAll.GetCount ();i++)
	{
		sField=arrayFieldsAll[i];
		sField.MakeUpper ();

		bExist=false;
		for(j=0;j<iCurFieldsQty;j++)
		{
			sFieldCurrent=pFields[j].sFieldName ;
			sFieldCurrent.MakeUpper ();
			if(sFieldCurrent==sField)
			{
				bExist=true;
				break;
			}
		}

		if(!bExist)
		{
			arrayFieldsLeft.Add (sField);
		}
	}

	CStringArray arrayShowFields;
	for(i=0;i<iCurFieldsQty;i++)
	{
		arrayShowFields.Add (pFields[i].sFieldName);
	}
	

	page1.SetFieldNames(arrayFieldsLeft);		
	page1.SetShowFieldNames (arrayShowFields );
	page1.SetShowTable(GetTableName ());
	page1.SetShowQtyMaxPermit (GetMaxHandDownRecord());
	page1.SetUser (GetDataBaseUserName(),GetDataBasePassword());
	
	// 添加导航页
	sheet.AddPage (&page1);
	sheet.AddPage (&page2);

	
	//  存储参数
	int nRet=sheet.DoModal ();
	if(nRet==ID_WIZFINISH)
	{
		CString sID=GetIDFieldName();
		CStringArray arrayWantedField;
		arrayWantedField.Add (sID);
		CString s;
		for(i=0;i<page1.m_arrayShowFields.GetCount ();i++)
		{
			s=page1.m_arrayShowFields.GetAt (i);
			if(s!=sID)
			{
				arrayWantedField.Add (s);
			}
		}

		SetMaxHandDownRecord(page1.m_lShowQtyMaxPermit);

		CTableDate dateStart=page2.GetStartProperty ();
		RemovePar("DataBrowseDoc_"+page1.m_sTableName+"_INDEXFIELD");
		if(dateStart.sYearField !="")
		{
			AddMorePar("DataBrowseDoc_"+page1.m_sTableName+"_INDEXFIELD",dateStart.sYearField);
		}
		if(dateStart.sMonthField !="")
		{
			AddMorePar("DataBrowseDoc_"+page1.m_sTableName+"_INDEXFIELD",dateStart.sMonthField);
		}
		if(dateStart.sDayField !="")
		{
			AddMorePar("DataBrowseDoc_"+page1.m_sTableName+"_INDEXFIELD",dateStart.sDayField);
		}

		CQCDBApp *pApp=(CQCDBApp *)AfxGetApp();

		CServer server;
		if(!GetServer(page1.m_sServer,server))
		{
			TRACEERROR("没有找到需要的服务器:",page1.m_sServer);
			return;
		};

		SetSourceIP(server.sIP );
		SetSourcePort(server.iPort);

		pApp->m_dt.GetData (server.sIP ,
							server.iPort ,
							page1.m_sTableName,
							page2.GetSQL (),
							dateStart,
							&arrayWantedField,
							this,
							page1.m_lShowQtyMaxPermit,
							false);

		UpdateAllViews(NULL);
	}

	else if(nRet==IDCANCEL)
	{
	}
}

BOOL CDataBrowseDoc::UpdateFromDB()
{
	CQCDBApp *pApp=(CQCDBApp *)AfxGetApp();

	CStringArray arrayFields;
	GetFields(arrayFields);

	CTableDate dateStart;
	pApp->m_dt.GetData (GetSourceIP(),
						GetSourcePort(),
						GetTableName(),
						GetSQL(),
						dateStart,
						&arrayFields,
						this,
						GetMaxHandDownRecord(),
						false);

	UpdateAllViews(NULL);

	return true;
}
void CDataBrowseDoc::OnFileSave()
{
	SaveModified();
	return ;
}

BOOL CDataBrowseDoc::OnSaveDocument(LPCTSTR lpszPathName)
{
	// TODO: Add your specialized code here and/or call the base class
	return true;
}

BOOL CDataBrowseDoc::SaveModified()
{
	// TODO: Add your specialized code here and/or call the base class
	if(RecordModified())
	{
		if(AfxMessageBox("是否更新数据库:"+GetTableName()+"?",MB_YESNO)==IDYES)
		{
			UpdateDB();
			UpdateFromDB ();
		}
	}

	return true;
}

void CDataBrowseDoc::ClearRecordStatus()
{
	memset(m_pRecordStatus,0,m_iRecordStatusQty);
}

void CDataBrowseDoc::ClearRecordStatus(int iRecord)
{
	memset(&m_pRecordStatus[iRecord],0,1);
}

void CDataBrowseDoc::OnSenddatatoservernohanded()
{
	SendDataToServer(true);
}

void CDataBrowseDoc::OnSenddatatoserverall()
{
	SendDataToServer(false);
}

void CDataBrowseDoc::GetIndexFields(CStringArray &array)
{
	GetMorePar("DataBrowseDoc_"+GetTableName()+"_INDEXFIELD",array);

	if(array.IsEmpty ())
	{
		array.Add (::GetColNameField(this->GetTableName ()));
	}

}

bool CDataBrowseDoc::SetFileType(enumDBFileType eFT)
{
	m_head .eDBFT =eFT;
	return true;
}

enumDBFileType CDataBrowseDoc::GetFileType()
{
	return m_head .eDBFT ;
}

CString CDataBrowseDoc::Initiate(CDataBrowseDoc *pDoc)
{
	CString sFile=GetTempFile();

	if(!CreateDBFile(sFile))
	{
		return CString("");
	};

	SetFileType	(pDoc->GetFileType ());
	SetTableName(pDoc->GetTableName());

	SetSourceIP(pDoc->GetSourceIP ());
	SetSourcePort(pDoc->GetSourcePort ());

	int iFieldQty=pDoc->GetFieldQty	();
	CFieldData *pFields=pDoc->GetFields ();

	SetFieldQty	(iFieldQty);
	for(int i=0;i<iFieldQty;i++)
	{
		SetField (i,pFields[i].sFieldName ,pFields[i].sFieldType );
	}
	
	WriteFileHead();

	CalcMarkFieldOrder();

	return sFile;
}

void CDataBrowseDoc::Close()
{
	if(!m_fp)return;

	fseek(m_fp,0,SEEK_SET);
	fwrite(&m_head,sizeof(CDBFileHead),1,m_fp);

	fclose(m_fp);
	m_fp=NULL;

}

bool CDataBrowseDoc::RecordModified()
{
	char cStatus;
	for(int i=0;i<GetRecordQty();i++)
	{
		cStatus=GetRecordStatus(i);

		if((cStatus&RS_ADDED)==0&&(cStatus&RS_DELETED)==RS_DELETED)
		{
			return true;
		}

		else if((cStatus&RS_ADDED)==0&&(cStatus&RS_MODIFIED)==RS_MODIFIED)
		{
			return true;
		}

		else if((cStatus&RS_ADDED)==RS_ADDED&&(cStatus&RS_DELETED)==0)
		{
			if((cStatus&RS_MODIFIED)==RS_MODIFIED)
			{
				return true;
			}
		}
	}

	return false;
}
void CDataBrowseDoc::SetModifiedFlag(BOOL bModified)
{
	CString strTitle=GetTitle();
	CString strDirtyFlag="*"; // note space before the  '*'
	
	//  So we do not break save as dialog:
	if(!IsModified()&&bModified)
	{
		SetTitle(strTitle+strDirtyFlag);
	}
	else if(IsModified()&&!bModified)
	{
		int nTitleLength=strTitle.GetLength();
		int nDirtyLength=strDirtyFlag.GetLength();
		SetTitle(strTitle.Left(nTitleLength-nDirtyLength));
	}

	UpdateFrameCounts();

	CDocument::SetModifiedFlag(bModified);
}

void CDataBrowseDoc::SavePar()
{
    AddPar("DataBrowseDoc_PicHeight",       m_iPicHeight);
    AddPar("DataBrowseDoc_PicWidth",        m_iPicWidth);
    AddPar("DataBrowseDoc_KeepRatio",       m_bPicKeepRatio);
    AddPar("DataBrowseDoc_DrawCellPic",     m_bDrawCellPic);

    AddPar("DataBrowseDoc_DrawBigPic",      m_bDrawBigPic);
    AddPar("DataBrowseDoc_BigPicWidth",     m_iBigPicWidth);
    AddPar("DataBrowseDoc_BigPicHeight",    m_iBigPicHeight);
    AddPar("DataBrowseDoc_BigPicKeepRatio", m_bBigPicKeepRatio);
    AddPar("DataBrowseDoc_BigPicOffsetX",   m_iBigPicOffsetX);
    AddPar("DataBrowseDoc_BigPicOffsetY",   m_iBigPicOffsetY);

    AddPar("DataBrowseDoc_DBPicKeepRatio",  m_bDBPicKeepRatio);
    AddPar("DataBrowseDoc_DBPicWidth",      m_iDBPicWidth);
    AddPar("DataBrowseDoc_DBPicHeight",     m_iDBPicHeight);	
}
