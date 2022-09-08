#include "StdAfx.h"
#include "FHApp.h"
#include "databasemanager.h"
#include "StringExtractor.h"
#include "odbcinst.h"
#include "fhcglobal.h"
#include "FHMainFrm.h"
#include "FHTable.h"
#include "QCGlobal.h"
#include "TableProcessor.h"
#include "defines.h"

CDataBaseManager::CDataBaseManager(void)
{
	::CoInitialize (NULL);

	m_sTable="";

	m_bSuccess=false;

	m_iTempBufferLen=1000;
	m_pTempBuffer=new char[m_iTempBufferLen];
}

CDataBaseManager::~CDataBaseManager(void)
{
	CloseDataBase();

	if(m_pTempBuffer)
	{
		delete []m_pTempBuffer;
		m_pTempBuffer=NULL;
	}
}

bool CDataBaseManager::Initiate(CString sDataBase)
{
	CloseDataBase();

	// ��ʼ������
	::CoInitialize (NULL);

	//�����������ݿ����Ӷ���
	HRESULT hr;
	try
	{
		// ����Connection����
		hr=m_pConnection.CreateInstance (__uuidof(Connection));
		if(SUCCEEDED(hr))
		{
			//hr=m_pConnection->Open (_bstr_t(LPCTSTR(sDataBase)),"","",NULL);
			char sDSN[100];
			sprintf(sDSN,"DSN=%s",sDataBase);
            CString sUserID,sPassword;
            sUserID=::GetDataBaseUserName();

#ifndef QCDB_ORACLE
            sUserID="admin";
#endif
            sPassword=::GetDataBasePassword();

			hr=m_pConnection->Open (sDSN,(LPCTSTR)sUserID,(LPCTSTR)sPassword,adModeUnknown);
			if(SUCCEEDED(hr))
			{
				m_sDBName=sDataBase;
				TRACEPROGRESS("���ݿ�������:"+sDataBase);
				return true;
			}
			else
			{
				TRACEERROR("���ݿ��޷�����:"+sDataBase);
				return false;
			}
		}
		else
		{
			TRACEERROR("���ݿ��޷���ʼ��:"+sDataBase);
			return false;
		}
	}
	catch(_com_error e)
	{
		CString sError;
		sError.Format("�������ݿ�ʧ��!\r\n������Ϣ:%s",e.ErrorMessage());
		TRACEERROR(sError);
		return false;
	}

	// 
	return true;
}

bool CDataBaseManager::CloseDataBase()
{
	// �رռ�¼������
	try
	{
		if(m_pConnection!=NULL)
		{
			m_pConnection->Close ();
			m_pConnection=NULL;
		}

		// �ͷŻ���
		::CoUninitialize ();
	}
	catch(_com_error e)
	{
		CString sError;
		sError.Format ("�ر�����ʧ��!\r\n������Ϣ��%s",e.ErrorMessage ());
		TRACEERROR(sError);
		return false;
	}	

	return true;
}

bool CDataBaseManager::Execute(char sSQL[])
{
	if(!Initiate (GetDBName()))
	{
		TRACEERROR("���ݿⲻ�ܳ�ʼ�����޷������ݱ�!");
		return false;
	}

	try
	{
		m_pConnection->Execute (sSQL,NULL,adOptionUnspecified);
	}
	catch(_com_error e)
	{
		CString sError;

		sError.Format ("ִ��SQL���ʧ��!\r\nSQL���:%s\r\n������Ϣ:%s",
			sSQL,
			e.ErrorMessage ());

		TRACEERROR(sError);
		return false;
	}

	return true;
}

////////////////////////////////////////////////////////////////
//
// delete records whose ID are same with those in sDBFile:
//
////////////////////////////////////////////////////////////////
bool CDataBaseManager::DeleteRecord(CString	sDBFile)
{
	if(!Initiate (GetDBName()))return false;

	_variant_t vFieldValue,vFieldName;

	CDataBrowseDoc doc;
	if(!doc.OpenDBFile (sDBFile))
	{
		TRACEERROR("�޷����ļ�:"+sDBFile);
		return false;
	}

	CString sTable=doc.GetTableName ();
	if(sTable=="")
	{
		TRACEERROR("����Ϊ��!");
		return false;
	}

	CString sMarkField=GetIDFieldName();
	_RecordsetPtr pRecord;
	CString sSQL,sID;

	int iRecordQty=doc.GetRecordQty ();
	try
	{
		if(FAILED(pRecord.CreateInstance(__uuidof(Recordset))))
		{
			TRACEERROR("������¼��(RecordSet)����!");
			return false;
		}

		for(int i=0;i<iRecordQty;i++)
		{
			sID=doc.GetRecordID (i);
			sSQL.Format("select * from %s where %s = '%s' ",
					(LPCTSTR)sTable,
					(LPCTSTR)sMarkField,
					(LPCTSTR)sID);

			pRecord->Open (	(LPCTSTR)sSQL,
							m_pConnection.GetInterfacePtr (),
							adOpenDynamic,
							adLockOptimistic,
							adCmdText);

			while(VARIANT_FALSE==pRecord->EndOfFile)
			{
				pRecord->Delete(adAffectCurrent); //adAffectAll);
				pRecord->MoveNext ();
			}

			pRecord->Close ();
		}
	}

	catch( _com_error e)
	{
		TRACEERROR("ɾ����¼��ʧ��!\r\n������Ϣ:"+CString(e.ErrorMessage()));
		return false;
	}

	CloseDataBase();

	return true;
}

bool  CDataBaseManager::ClearDB()
{
    CStringArray arrayTableNames;
    ::GetTableNames(arrayTableNames);

    _RecordsetPtr pRecord;
    CString sSQL,sID;
    int i;
    CString sTable;

    try
    {
        if(FAILED(pRecord.CreateInstance(__uuidof(Recordset))))
        {
            TRACEERROR("������¼��(RecordSet)����!");
            return false;
        }

        for(i=0;i<arrayTableNames.GetCount();i++)
        {
            sTable=arrayTableNames.GetAt(i);
            sSQL.Format("select * from %s ",(LPCTSTR)sTable);

            pRecord->Open (	(LPCTSTR)sSQL,
                m_pConnection.GetInterfacePtr (),
                adOpenDynamic,
                adLockOptimistic,
                adCmdText);

            while(VARIANT_FALSE==pRecord->EndOfFile)
            {
                pRecord->Delete(adAffectCurrent); //adAffectAll);
                pRecord->MoveNext ();
            }

            pRecord->Close ();
        }
    }

    catch( _com_error e)
    {
        TRACEERROR("������ݿ�ʧ��!\r\n������Ϣ:"+CString(e.ErrorMessage()));
        return false;
    }

    CloseDataBase();

    return true;

}



bool CDataBaseManager::AddRecord(CString sDBDoc)
{
	if(!Initiate (GetDBName()))return false;
	CDataBrowseDoc doc;
	if(!doc.OpenDBFile (sDBDoc))
	{
		return false;
	}
	CString sTable=doc.GetTableName ();
	if(sTable=="")
	{
		TRACEERROR("����Ϊ��!");
		return false;
	}
    TRACEPROGRESS("���Կͻ��˵����ݸ������ݱ�:"+sTable);

	bool bError=false;
	long i,j;
	CString sFieldName,sFieldValue;
	_RecordsetPtr	pRecords;
	int iRecordQty=doc.GetRecordQty();

	CStringArray arrayFields, *pArrayRecord=NULL;
	doc.GetFields (arrayFields);

	char sSQL[1000];
	sprintf(sSQL," select * from %s ",(LPCTSTR)sTable);
	HRESULT hr=0;
	_variant_t vFieldValue,vFieldName;

	try
	{
		if(FAILED(pRecords.CreateInstance(__uuidof(Recordset))))
		{
			TRACEERROR("������¼��(RecordSet)����!");
			return false;
		}
	
		hr=pRecords->Open (		sSQL,
								m_pConnection.GetInterfacePtr (),
								adOpenDynamic,
								adLockOptimistic,
								adCmdText);
	}
	
	catch( _com_error e)
	{
		TRACEERROR("�����ݱ�ʧ��:��:"+sTable+",SQL���:"+sSQL+". ������Ϣ:"+CString(e.ErrorMessage()));
		bError=true;
	}


	for(i=0;i<iRecordQty;i++)
	{
		try
		{
			pRecords->AddNew ();
		}
		catch( _com_error e)
		{
			TRACEERROR("�����¼�¼ʧ��!");
			bError=true;
		}


		pArrayRecord=doc.GetRecord (i);
		
		try
		{
			for( j=0;j<arrayFields.GetCount ();j++)
			{
				sFieldName=arrayFields.GetAt (j);
				vFieldName=sFieldName;

				sFieldValue=pArrayRecord->GetAt (j);
				if(IsEmpty(sFieldValue))
				{
					sFieldValue="0";
				}
				vFieldValue=sFieldValue;

				if(IsPicField(sFieldName))
				{
					#ifdef QCDB_ORACLE
						PutPicToRecord(pRecord,sFieldName,sFieldValue);
					#else 
						pRecords->PutCollect(vFieldName, vFieldValue);
					#endif
				}
				else
				{
					pRecords->PutCollect(vFieldName, vFieldValue);
				}
			}
		}
		catch( _com_error e)
		{
			TRACEERROR("PutCollectʧ��:��:"+sTable+"�ֶ�:"+sFieldName+",ֵ:"+sFieldValue+". ������Ϣ:"+CString(e.ErrorMessage()));
			bError=true;
		}

		// Only update those that are updatable:
		try
		{
			hr=pRecords->Update ();
			if(hr!=S_OK)
			{
				TRACEERROR("���Ӽ�¼��ʧ��!");
				continue;  // Because maybe there are some records with same ID, so pass them.
			}
		}
		catch( _com_error e)
		{
			TRACEERROR("���Ӽ�¼��ʧ��!");
			continue; // Because maybe there are some records with same ID, so pass them.
		}

	}

	// �ر����ݿ�:
	try
	{
		pRecords->Close();
		pRecords=NULL;
		CloseDataBase();
        TRACEPROGRESS("���Կͻ��˵����ݸ������ݱ����:"+sTable);
	}
	catch( _com_error e)
	{
		TRACEERROR("���Կͻ��˵����ݸ������ݱ�ʧ��! ������Ϣ:"+CString(e.ErrorMessage()));
		bError=true;
	}

	return !bError;
}

void CDataBaseManager::PutPicToRecord(_RecordsetPtr pRecord,
									  CString sFieldName,
									  CString sFile)
{
	CString sPath=GetMyTempPath();
	if(sFile[1]!=':')  // ���Ǳ��ظ��£��Ǵӿͻ��˷������ļ���
	{
		sFile=sPath+sFile;
	}

	long j=0,iLen=0;
	char *pTempBuf=NULL;
	_variant_t varBLOB;
	SAFEARRAYBOUND rgsabound[1];
	SAFEARRAY *psa=NULL;

	FILE *fpPic=fopen(sFile,"rb");
	if(fpPic)
	{
		fseek(fpPic,0,SEEK_END);
		iLen=ftell(fpPic);
		if(iLen>10000000)
		{
			CString s;
			s.Format ("�ļ����̫��:%d,���ܼ������ݿ�:",iLen);
			TRACEERROR(s);
			strcpy(m_pTempBuffer,(LPCTSTR)sFile);
			iLen=sFile.GetLength ();
		}
		else
		{
			fseek(fpPic,0,SEEK_SET);
			if(iLen>m_iTempBufferLen)
			{
				delete []m_pTempBuffer;
				m_pTempBuffer=new char [iLen+100];
				m_iTempBufferLen=iLen;
			}		
			fread(m_pTempBuffer,iLen,1,fpPic);
			fclose(fpPic);
		}
	}
	else
	{
		TRACEERROR("�޷����ļ�:"+sFile+",���Ѹü�¼��Ϊ���ļ���!");

		strcpy(m_pTempBuffer,(LPCTSTR)sFile);
		iLen=sFile.GetLength ();
	}

	rgsabound[0].lLbound = 0;
	rgsabound[0].cElements = iLen;
	psa=SafeArrayCreate(VT_UI1,1,rgsabound);

	pTempBuf=m_pTempBuffer;
	for(j=0;j<iLen;j++)
	{
		SafeArrayPutElement(psa,&j,pTempBuf++);
	}

	varBLOB.vt = VT_ARRAY|VT_UI1;
	varBLOB.parray = psa;

	pRecord->GetFields ()->GetItem ((LPCTSTR)sFieldName)->AppendChunk (varBLOB);

	return;
}

bool CDataBaseManager::UpdateRecord (CString sDBFile)
{	
	if(!Initiate (GetDBName()))return false;
	CDataBrowseDoc doc;
	if(!doc.OpenDBFile (sDBFile))
	{
		return false;
	}
	CString sTable=doc.GetTableName ();
	if(sTable=="")
	{
		TRACEERROR("����Ϊ��!");
		return false;
	}

	bool bError=false;
	long i,j;
	CString sFieldName,sFieldValue,sID,sSQL;
	_variant_t vFieldValue,vFieldName;

	_RecordsetPtr	pRecords;
	int iRecordQty=doc.GetRecordQty();

	CStringArray arrayFields, *pArrayRecord=NULL;
	doc.GetFields (arrayFields);
	int iFieldQty=arrayFields.GetCount();
	CString sMarkField=GetIDFieldName();

	try
	{
		if(FAILED(pRecords.CreateInstance(__uuidof(Recordset))))
		{
			TRACEERROR("������¼��(RecordSet)����!");
			return false;
		}

		for(i=0;i<iRecordQty;i++)
		{
			pArrayRecord=doc.GetRecord(i);
			sID=doc.GetRecordID (i);

			sSQL.Format("select * from %s where %s = '%s' ",
					(LPCTSTR)sTable,
					(LPCTSTR)sMarkField,
					(LPCTSTR)sID);

			pRecords->Open ((LPCTSTR)sSQL,
							m_pConnection.GetInterfacePtr (),
							adOpenDynamic,
							adLockOptimistic,
							adCmdText);

			while(VARIANT_FALSE==pRecords->EndOfFile)
			{

				for( j=0;j<iFieldQty;j++)
				{
					sFieldName=arrayFields.GetAt (j);			
					vFieldName=sFieldName;

					sFieldValue=pArrayRecord->GetAt (j);
					vFieldValue=sFieldValue;

					if(IsPicField(sFieldName))
					{
						#ifdef QCDB_ORACLE
							PutPicToRecord(pRecords,sFieldName,sFieldValue);
						#else 
							pRecords->PutCollect(vFieldName, vFieldValue);
						#endif
					}
					else
					{
						pRecords->PutCollect(vFieldName, vFieldValue);
					}
				}
				pRecords->Update ();
				pRecords->MoveNext ();
			}
			pRecords->Close ();
		}
	}
	catch(_com_error e)
	{		
		TRACEERROR("���¼�¼��ʧ��:�ֶΣ�"+sFieldName+",ֵ:"+sFieldValue+"!\r\n������Ϣ:"+CString(e.ErrorMessage()));
		return false;
	}

	CloseDataBase();


	return true;
}


CString CDataBaseManager::GetValueFromRecord(	_RecordsetPtr	pRecord,
												CString			sFieldName)
{
	static _variant_t vFieldValue;

	vFieldValue.Clear ();
	vFieldValue=pRecord->GetCollect((LPCTSTR)(sFieldName));
	if(V_VT(&vFieldValue) == VT_NULL)
	{
		return CString("NULL");
	}
	else
	{
		return CString((char*)_bstr_t(vFieldValue));
	}
}
bool CDataBaseManager::ExtractFieldNames(CString sSummedFields,CStringArray &arrayFields)
{
	int iStart=sSummedFields.Find ("(");
	int iEnd=sSummedFields.Find (")");

	if(iStart==-1||iEnd==-1)return false;
	
	CString sValidSum=sSummedFields.Mid (iStart+1,iEnd-iStart-1);
	CStringExtractor ex;
	ex.SetSepChar (",");
	ex.SetString (sValidSum);

	int n=ex.GetSubStringQty ();

	arrayFields.SetSize (n);

	for(int i=0;i<n;i++)
	{
		arrayFields.SetAt (i,ex.GetSubString (i));
	}

	return true;
}

////////////////////////////////////////////////////////////////
// Used in Oracle system.
// In Oracle, picture is saved in database directly.
// while in Access, picture is saved as a name, the data 
////////////////////////////////////////////////////////////////
CString CDataBaseManager::GetPicFromRecord(	_RecordsetPtr	pRecord,
                                            CString         sFieldName)
{
	static int iDataSize;
	static _variant_t varBLOB;
	static char *pBuffTemp=NULL;
	static CString sPicPathName;
	static CFHImage image;
	static FILE *fpPic=NULL;

	iDataSize=pRecord->GetFields ()->GetItem ((LPCTSTR)sFieldName)->ActualSize;
	if(iDataSize>0)
	{
		varBLOB=pRecord->GetFields ()->GetItem ((LPCTSTR)sFieldName)->GetChunk(iDataSize);
		if(varBLOB.vt==(VT_ARRAY|VT_UI1))
		{
			if(iDataSize>m_iTempBufferLen)
			{
				delete []m_pTempBuffer;
				m_pTempBuffer=new char[iDataSize+100];
				m_iTempBufferLen=iDataSize;
			}							

			pBuffTemp=NULL;
			SafeArrayAccessData(varBLOB.parray,(void**)&pBuffTemp);
			memcpy(m_pTempBuffer,pBuffTemp,iDataSize);
			SafeArrayUnaccessData(varBLOB.parray);

			sPicPathName=GetTempFile()+".jpg";
			fpPic=fopen(sPicPathName,"wb");
			if(!fpPic)
			{
				TRACEERROR("�޷�������ʱͼƬ�ļ�:"+sPicPathName);
				return CString("�洢ͼƬ����");
			}
			fwrite(m_pTempBuffer,iDataSize,1,fpPic);
			fclose(fpPic);

			HRESULT hr = image.Load (sPicPathName);
			if(hr==S_OK)
			{
				return sPicPathName;
			}
			else
			{
				return CString("�޷�����ͼƬ");
			}
		}
	}
	else
	{
		return CString("ͼƬ����Ϊ��");
	}

	return CString("");
}


bool CDataBaseManager::RegisterDBSrc(CString sSrcAccess, 
									 CString sODBCName,
									 CString sDiscription,
									 bool bAdd)
{
	char s[1000];
	memset(s,0,1000);
	char *ps=s;

	char sPart[1000];
	
	strcpy(sPart,"DSN=");
	strcat(sPart,sODBCName);
	strcpy(ps,sPart);
	ps+=strlen(sPart)+1;

	strcpy(sPart,"DBQ=");
	strcat(sPart,sSrcAccess);
	strcpy(ps,sPart);
	ps+=strlen(sPart)+1;

	strcpy(sPart,"Description=");
	strcat(sPart,sDiscription);
	strcpy(ps,sPart);
	ps+=strlen(sPart)+1;

	strcpy(sPart,"FileType=Access");
	strcpy(ps,sPart);
	ps+=strlen(sPart)+1;

	strcpy(sPart,"MaxScanRows=20");
	strcpy(ps,sPart);
	ps+=strlen(sPart)+1;

	bool b=true;
	if(bAdd)
	{
		b=SQLConfigDataSource(NULL,ODBC_ADD_DSN, "Driver do Microsoft Access (*.mdb)",s);
	}
	else
	{
		b=SQLConfigDataSource(NULL,ODBC_REMOVE_DSN, "Driver do Microsoft Access (*.mdb)",s);
	}

	/* This is the original code, save for comprison:
	bool b=SQLConfigDataSource(NULL,ODBC_ADD_DSN, "Driver do Microsoft Access (*.mdb)",
		"DSN=test 444\0" 
		"DBQ=d:\\db2.mdb\0"
		"Description=fanhua test New Access Data Source\0" 
		"FileType=Access\0" 
		"DataDirectory=d:\\"
		
		"MaxScanRows=20\0");
		*/


	return b;
}

bool CDataBaseManager::CreateTablesFromBlueDB(  CString sTarDBName,
							                    CString sBlueDBName)
{
	// ��ʼ������
	::CoInitialize (NULL);

	//�����������ݿ����Ӷ���
	_ConnectionPtr	pConnection;

	HRESULT hr;
	try
	{
		// ����Connection����
		hr=pConnection.CreateInstance (__uuidof(Connection));
		if(SUCCEEDED(hr))
		{
			CString sUser=GetDataBaseUserName();
#ifndef QCDB_ORACLE
            sUser="admin";
#endif
            CString sPassword=::GetDataBasePassword();
			hr=pConnection->Open (	_bstr_t(LPCTSTR(sTarDBName)),
									_bstr_t(LPCTSTR(sUser)),
									_bstr_t(LPCTSTR(sPassword)),
									NULL);
		}
	}
	catch(_com_error e)
	{
		CString sError;
		sError.Format("�������ݿ�ʧ��!\r\n������Ϣ:%s",e.ErrorMessage());
		TRACEERROR(sError);
		return false;
	}

	/////////////////////////////////
	// �����ͼ���ݿ��еı���:
	/////////////////////////////////
	CFHTable t;
	if(!t.Open (sBlueDBName))
	{
		TRACEERROR("�޷������ݿ�:"+sBlueDBName);
		return false;
	}

	CStringArray arrayTableNames;
	t.GetTableNames(arrayTableNames);

	/////////////////////////////////////
	// �������ݿ⣬ͬʱ�������б���ֶ�
	/////////////////////////////////////
	char sTableCreateSQL[5000];
	try
	{
		CDatabase database;
		if(!database.Open (sBlueDBName))
		{
			TRACEERROR("�޷������ݿ�:"+sBlueDBName);
			return false;
		}

		CRecordset record (&database);
		int i,j,iFieldQty=0;
		CODBCFieldInfo info;
		int iLen=0;
		char sSQL[200];
		CString sTableName;
		char sType[200];
	
		for(i=0;i<arrayTableNames.GetCount ();i++)
		{
			// 
			sTableName=arrayTableNames.GetAt(i);
			memset(sTableCreateSQL,0,5000);

			if(IsServer())
			{
				sprintf(sTableCreateSQL,"create table SYSTEM.%s (",sTableName);
			}
			else
			{
				sprintf(sTableCreateSQL,"create table %s (",sTableName);
			}

			//
			memset(sSQL,0,200);
			sprintf(sSQL,"select * from %s",arrayTableNames.GetAt (i));
			if(!record.Open (CRecordset::forwardOnly,sSQL))
			{
				TRACEERROR("�޷������ݱ�:"+arrayTableNames.GetAt (i));
				continue;
			}

			iFieldQty=record.GetODBCFieldCount ();
			for(j=0;j<iFieldQty;j++)
			{
				record.GetODBCFieldInfo (j,info);
				GetFieldType(sType,info);		
				strcat(sTableCreateSQL,info.m_strName);
				strcat(sTableCreateSQL,sType);
				strcat(sTableCreateSQL,",");
			}

			record.Close ();

			// ���SQL��䣬��β�����пո�
			iLen=strlen(sTableCreateSQL);
			while(sTableCreateSQL[iLen-1]==' ')
			{
				sTableCreateSQL[iLen-1]=0;
				iLen--;
			}

			if(sTableCreateSQL[iLen-1]==',')
			{
				sTableCreateSQL[iLen-1]=0;
			}

			// ִ��SQL���:
			strcat(sTableCreateSQL," )");

			try
			{
				pConnection->Execute (sTableCreateSQL,NULL,adOptionUnspecified);
			}
			catch(_com_error e)
			{
				CString sError;

				sError.Format ("ִ��SQL���ʧ��! SQL���:%s  ������Ϣ:%s",
					sTableCreateSQL,
					e.ErrorMessage ());

				TRACEERROR(sError);
				continue;
			}
		}
	}

	catch(CDBException* pException)
	{
		TRACEERROR("�޷������������ݿ�Ĵ���! ������Ϣ:"+pException->m_strError);
		delete pException;
	}

	return true;
}

bool CDataBaseManager::SendTableToBrowseDoc(CString			sTable,
											CString			sSQL,
											CTableDate		dateStart,
											CStringArray	*pArrayFields,
											CDataBrowseDoc	*pDoc,
											int				iMaxTransferRecord,
											bool			bMarkHandedIn)
{
	int i=0;

	Initiate (GetDBName());

	// Create DataBrowseDoc:
	pDoc->CreateDBFile ();
	if(!pDoc->SetTableName(sTable))return false;
	pDoc->SetSQL(sSQL);


	// Check if the field names are empty:
	int iFieldQty=0;
	if(pArrayFields)
	{
		iFieldQty=pArrayFields->GetCount ();
	}

	CStringArray arrayNames,arrayTypes;
	CStringArray *pArrayTypes=NULL;
	if(pArrayFields==NULL||iFieldQty==0)
	{
		if(!GetFieldNames (					sTable,
										arrayNames,
										arrayTypes))
		{
			return false;
		}
		pArrayFields=&arrayNames;
	}


	// Add Date Fields:
	int iYearFieldOrder=-1,iMonthFieldOrder=-1,iDayFieldOrder=-1;
	AddDateField(	sTable,
					pArrayFields,
					dateStart,
					iYearFieldOrder,
					iMonthFieldOrder,
					iDayFieldOrder);

    // Get the types of the fields:
    if(!GetFieldTypes(sTable,*pArrayFields,arrayTypes))
    {
        return false;
    }

    // ��ȡ�ֶ��������������գ����Ա������������
	// �����ĵ����ֶ���Ŀ��
	if(!pDoc->SetField (*pArrayFields,arrayTypes))
	{
		return false;
	}



	// Add the condition of Start Year,Month, Day, if No total_project:
	CString sField,sValue;
	bool bExistProjectTotal=false;
	iFieldQty=pArrayFields->GetCount ();

	for(i=0;i<iFieldQty;i++)
	{
		sField=pArrayFields->GetAt (i);
		if(sField.Find ("TOTAL_PROJECT")!=-1)
		{
			bExistProjectTotal=true;
			break;
		}
	}

	CString sStartDay;
	if(!bExistProjectTotal)
	{
		CTableProcessor tp;
		tp.SetTable (sTable);
		CString sYear=tp.FormatFieldValue (dateStart.sYearField,dateStart.sYear);
		CString sMonth=tp.FormatFieldValue (dateStart.sMonthField,dateStart.sMonth);
		CString sDay=tp.FormatFieldValue (dateStart.sDayField,dateStart.sDay);

		sStartDay=MakeDateCmpString(dateStart.sYearField ,
									dateStart.sMonthField,
									dateStart.sDayField ,
									">=",
									sYear,
									sMonth,
									sDay);

		if(sStartDay!="")
		{
			if(sSQL.Find ("where")!=-1)
			{
				sSQL+=" and "+sStartDay;
			}
			else
			{
				sSQL+=" where "+sStartDay;
			}
		}
	}

	// Order by date, in fact the 3 Fields can be any field when sorting:
	CStringArray arrayOrderField;
	if(dateStart.sYearField!="")arrayOrderField.Add (dateStart.sYearField);
	if(dateStart.sMonthField!="")arrayOrderField.Add (dateStart.sMonthField);
	if(dateStart.sDayField!="")arrayOrderField.Add (dateStart.sDayField);

	// Add order fields :
	int iOrderFieldQty=arrayOrderField.GetCount ();

	int iValidOrderField=0;
    for(i=0;i<iOrderFieldQty;i++)
	{
		sField=arrayOrderField.GetAt (i);
		if(sField!="")
		{
			iValidOrderField++;
		}
	}
	int n=0;
	if(iValidOrderField>0)
	{
		sSQL+=" order by ";
		n=0;
        for(i=0;i<iOrderFieldQty;i++)
		{
			sField=arrayOrderField.GetAt (i);
			if(sField!="")
			{
				sSQL+=sField+" asc ";

				if(n!=iValidOrderField-1)
				{
					sSQL+=" , ";
				}
				n++;
			}
		}
	}

	// Select data from database:
	_RecordsetPtr	pRecords;
	
	// Send data to BrowseDoc:
	CStringArray arrayRecord;
	int iRecordQty=0;
	CMyDate date;
	double *pProjectSum=new double [iFieldQty+2];
	double *pUnitSum=new double [iFieldQty+2];
	memset(pProjectSum,0,sizeof(double)*iFieldQty);
	memset(pUnitSum,0,sizeof(double)*iFieldQty);
	bool bFailed=false;

	CString sHandedInFieldName,sHandedInFieldValue;
	GetHandedInField(sHandedInFieldName, sHandedInFieldValue);
	_variant_t vFieldValue,vFieldName;


	try
	{
		if(FAILED(pRecords.CreateInstance(__uuidof(Recordset))))
		{
			TRACEERROR("������¼��(RecordSet)����!");
			return false;
		}
	
		HRESULT hr=0;
		hr=pRecords->Open (		(LPCTSTR)sSQL,
								m_pConnection.GetInterfacePtr (),
								adOpenDynamic,
								adLockOptimistic,
								adCmdText);


		while(!pRecords ->EndOfFile)
		{
			
			GetRecord(pRecords,*pArrayFields,arrayRecord,true);
			if(bMarkHandedIn)
			{
				vFieldName=sHandedInFieldName;
				vFieldValue=sHandedInFieldValue;
				pRecords->PutCollect (vFieldName, vFieldValue);
				pRecords->Update ();
			}


			// Project Sum:
			for(i=0;i<iFieldQty;i++)
			{
				sField=pArrayFields->GetAt (i);
				if(sField.Find ("TOTAL_PROJECT")!=-1)
				{
					sValue=arrayRecord.GetAt (i);
					pProjectSum[i]+=stof(sValue);
				}
			}

			// Check if bigger than start date:
			if(dateStart.sYear !=""||dateStart.sMonth !=""||dateStart.sDay !="")
			{
				if(iYearFieldOrder!=-1)
				{
					date.SetYear (stof(arrayRecord.GetAt (iYearFieldOrder)));
				}

				if(iMonthFieldOrder!=-1)
				{
					date.SetMonth (stof(arrayRecord.GetAt (iMonthFieldOrder)));
				}

				if(iDayFieldOrder!=-1)
				{
					date.SetDay (stof(arrayRecord.GetAt (iDayFieldOrder)));
				}

				if(date<CMyDate(dateStart.sYear ,dateStart.sMonth ,dateStart.sDay ))
				{
					pRecords->MoveNext ();
					continue;
				}
			}

			// Unit Sum:
			for(i=0;i<iFieldQty;i++)
			{
				sField=pArrayFields->GetAt (i);
				if(sField.Find ("TOTAL_UNIT")!=-1)
				{
					pUnitSum[i]+=stof(arrayRecord.GetAt (i));
				}
			}

			// Set the Sum back to pRecord:
			for(i=0;i<iFieldQty;i++)
			{
				sField=pArrayFields->GetAt (i);
				if(sField.Find ("TOTAL_PROJECT")!=-1)
				{
					arrayRecord.SetAt (i,vtos(pProjectSum[i]));
				}
				if(sField.Find ("TOTAL_UNIT")!=-1)
				{
					arrayRecord.SetAt (i,vtos(pUnitSum[i]));
				}
			}

			// Append the record to the Browse Doc:
			pDoc->AppendRecord (&arrayRecord);
			iRecordQty++;
			if(iRecordQty>=iMaxTransferRecord)
			{
				break;
			}
			pRecords->MoveNext ();
		}
	}

	catch(_com_error e)
	{		
		TRACEERROR("��ȡ��¼��ʧ��!\r\n������Ϣ:"+CString(e.ErrorMessage()));
		bFailed=true;
	}
	

	delete []pProjectSum;
	delete []pUnitSum;

	return !bFailed;
}

void CDataBaseManager::AddDateField(	CString			sTable,
										CStringArray	*pArrayFields,
										CTableDate		dateStart,
										int				&iYearFieldOrder,
										int				&iMonthFieldOrder,
										int				&iDayFieldOrder)
{
	if(!pArrayFields)return;

	int i=0;
	int iFieldQty=0;
	iFieldQty=pArrayFields->GetCount ();

	bool bExistYear		=false;
	bool bExistMonth	=false;
	bool bExistDay		=false;

	CString sField="";
	for(i=0;i<iFieldQty;i++)
	{
		sField=pArrayFields->GetAt (i);
		if(sField==dateStart.sYearField )
		{
			bExistYear=true;
		}

		else if(sField==dateStart.sMonthField )
		{
			bExistMonth=true;
		}

		else if(sField==dateStart.sDayField )
		{
			bExistDay=true;
		}
	}

	bool bExistYearInTable=false;
	bool bExistMonthInTable=false;
	bool bExistDayInTable=false;

	CStringArray arrayNames,arrayTypes;
	::GetFieldNames (sTable,arrayNames,arrayTypes);
	for(i=0;i<arrayNames.GetCount ();i++)
	{
		sField=arrayNames.GetAt (i);
		if(sField==dateStart.sYearField)
		{
			bExistYearInTable=true;
		}

		if(sField==dateStart.sMonthField)
		{
			bExistMonthInTable=true;
		}

		if(sField==dateStart.sDayField)
		{
			bExistDayInTable=true;
		}
	}


	if(!bExistYear&&dateStart.sYearField!=""&&bExistYearInTable)
	{
		pArrayFields->Add (dateStart.sYearField);
	}

	if(!bExistMonth&&dateStart.sMonthField!=""&&bExistMonthInTable)
	{
		pArrayFields->Add (dateStart.sMonthField);
	}

	if(!bExistDay&&dateStart.sDayField!=""&&bExistDayInTable)
	{
		pArrayFields->Add (dateStart.sDayField);
	}

	iFieldQty=pArrayFields->GetCount ();
	for(i=0;i<iFieldQty;i++)
	{
		sField=pArrayFields->GetAt (i);
		if(sField==dateStart.sYearField )
		{
			iYearFieldOrder=i;
		}

		else if(sField==dateStart.sMonthField )
		{
			iMonthFieldOrder=i;
		}

		else if(sField==dateStart.sDayField )
		{
			iDayFieldOrder=i;
		}
	}
}


bool CDataBaseManager::GetRecord(	_RecordsetPtr	pRecords,
									CStringArray	&arrayFields,
									CStringArray	&arrayRecord,
									bool			bWithPic)
{
	if(arrayFields.GetCount ()==0)
	{
		return false;
	}
	arrayRecord.RemoveAll ();
	
	static CString sFieldName;
	static int iFieldQty=0;
	static int i=0,j=0;
	static CStringArray arrayFieldsSum;
	static double dSum=0.0;
	static CString s;

	try
	{		
		iFieldQty=arrayFields.GetCount ();
		arrayRecord.SetSize (iFieldQty);
		for(i=0;i<iFieldQty;i++)
		{
			arrayRecord.SetAt (i,"0");
		}

		for(i=0;i<iFieldQty;i++)
		{
			sFieldName=arrayFields.GetAt (i);

			// Picture field:
			if(IsPicField(sFieldName))
			{
				if(bWithPic)
				{
					#ifdef QCDB_ORACLE
						arrayRecord.SetAt (i,GetPicFromRecord(pRecords,sFieldName));
					#else
						arrayRecord.SetAt (i,GetValueFromRecord(pRecords,sFieldName));
					#endif
				}
				else
				{
					arrayRecord.SetAt(i,"ͼƬ");
				}
			}

			// Sum some fields :
			else if(sFieldName.Find ("=SUM(")!=-1||sFieldName.Find ("=TOTAL")!=-1)
			{
				ExtractFieldNames(sFieldName,arrayFieldsSum);
				dSum=0.0;
				for(j=0;j<arrayFieldsSum.GetCount ();j++)
				{
					s=GetValueFromRecord(pRecords,arrayFieldsSum.GetAt (j));
					dSum+=stof(s);
				}
				arrayRecord.SetAt (i,vtos(dSum));
			}

			// single field:
			else
			{
				arrayRecord.SetAt (i,GetValueFromRecord(pRecords,sFieldName));
			}
		}
	}
	catch(_com_error e)
	{		
		TRACEERROR("��ȡ��¼��ʧ��!\r\n������Ϣ:"+CString(e.ErrorMessage()));
	}

	return true;
}

/////////////////////////////////////////////////////
// This function is needed by History Table:
/////////////////////////////////////////////////////
bool CDataBaseManager::AddRecord(CString		sTable,
								 CStringArray	&arrayFields,
								 CStringArray	&arrayValues)
{
	if(!Initiate (GetDBName()))return false;

	if(sTable=="")
	{
		TRACEERROR("����Ϊ��!");
		return false;
	}

	bool bError=false;
	long i;
	char *pTempBuf=NULL;
	CString sFieldName,sFieldValue;
	CString sPath,sFile;

	char sSQL[1000];
	sprintf(sSQL," select * from %s ",(LPCTSTR)sTable);
	try
	{
		_RecordsetPtr pRecords;

		//sSQL="select * from ��Ŀ where ��ĿID > 0";
		if(FAILED(pRecords.CreateInstance(__uuidof(Recordset))))
		{
			TRACEERROR("������¼��(RecordSet)����!");
			return false;
		}
	
		HRESULT hr=0;
		hr=pRecords->Open (	(LPCTSTR)sSQL,
								m_pConnection.GetInterfacePtr (),
								adOpenDynamic,
								adLockOptimistic,
								adCmdText);
		pRecords->AddNew ();
		
		char *pBuffer=NULL;
		long iLen=0;
		_variant_t vFieldValue,vFieldName;

		FILE *fpPic=NULL;

		for( i=0;i<arrayFields.GetCount ();i++)
		{
			sFieldName=arrayFields.GetAt (i);
			vFieldName=sFieldName;

			sFieldValue=arrayValues.GetAt (i);
			sFieldValue.MakeUpper ();
			if(IsEmpty(sFieldValue))
			{
				sFieldValue="0";
			}

			if(sFieldName.Right (4)=="����"||sFieldName.Right (4)=="DATE")
			{
				vFieldValue=sFieldValue;
			}
			else
			{
				vFieldValue=sFieldValue;
			}

			if(IsPicField(sFieldName))
			{
				#ifdef QCDB_ORACLE
					PutPicToRecord(pRecords,sFieldName,sFieldValue);
				#else 
					pRecords->PutCollect(vFieldName, vFieldValue);
				#endif
			}
			else
			{
				pRecords->PutCollect(vFieldName, vFieldValue);
			}
		}

		hr=pRecords->Update ();
		pRecords->Close();
	}

	catch( _com_error e)
	{
		TRACEERROR("���Ӽ�¼��ʧ��:��:"+sTable+",�ֶΣ�"+sFieldName+",ֵ:"+sFieldValue+".������Ϣ:"+CString(e.ErrorMessage()));
		bError=true;
	}


	return !bError;
}