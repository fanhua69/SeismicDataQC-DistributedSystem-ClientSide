#include "StdAfx.h"
#include ".\datafile.h"
#include "afxdisp.h"

CDataFile::CDataFile(void)
{
	m_pFieldProperty	=NULL;
	m_pRecordStart		=NULL;
	m_lBodyStartPos		=NULL;
	m_fpDataFile		=NULL;
	m_pRecord			=NULL;
	m_bWriting			=false;
	m_iWritingOrder		=0;
	m_pRecordData		=NULL;	
}

CDataFile::~CDataFile(void)
{
	if(m_pFieldProperty)
	{
		delete []m_pFieldProperty;
		m_pFieldProperty=NULL;
	}

	if(m_pRecordStart)
	{
		delete []m_pRecordStart;
		m_pRecordStart=NULL;
	}

	if(m_fpDataFile)
	{
		fclose(m_fpDataFile);
		m_fpDataFile=NULL;
	}

	if(m_pRecordData)
	{
		delete []m_pRecordData;
		m_pRecordData=NULL;
	}
}

bool CDataFile::SetInfo(	char sDataBase[], 
							char sTableName[], 
							long lFieldQty,
							long lRecordQty)
{
	if(strlen(sDataBase)>=255)return false;
	if(strlen(sTableName)>=255)return false;
	if(lFieldQty<1||lRecordQty<1)return false;
    
	strcpy(m_head.sDataBaseName,sDataBase);
	strcpy(m_head.sTableName ,sTableName );
	m_head.lFieldQty = lFieldQty;
	m_head.lRecordQty = lRecordQty;

	if(m_pFieldProperty)
	{
		delete []m_pFieldProperty;
		m_pFieldProperty=NULL;
	};
	m_pFieldProperty=new CFieldProperty[lFieldQty+10];
	
	if(m_pRecordStart)
	{
		delete []m_pRecordStart;
		m_pRecordStart=NULL;
	}
	m_pRecordStart=new long [lRecordQty+10];

	return true;
}

bool CDataFile::CreateDataFile(CString sFileName)
{	
	if(sFileName=="")return false;

	if(m_fpDataFile)
	{
		fclose(m_fpDataFile);
		m_fpDataFile=NULL;
	}
	m_fpDataFile=fopen(sFileName,"wb");

	if(!m_fpDataFile)return false;

	m_bWriting=true;
	m_iWritingOrder=0;

	return true;
}

bool CDataFile::OpenDataFile(CString sFileName)
{
	if(sFileName=="")return false;

	if(m_fpDataFile)
	{
		fclose(m_fpDataFile);
		m_fpDataFile=NULL;
	}
	m_fpDataFile=fopen(sFileName,"rb");

	if(!m_fpDataFile)return false;

	m_bWriting=false;
	m_iWritingOrder=0;

	//////////////////////////////////
	//
	//////////////////////////////////
	fread(&m_head,sizeof(CDataFileHead),1,m_fpDataFile);
	if(!m_head.IsValid ())
	{
		fclose(m_fpDataFile);
		return false;
	}

	if(m_pFieldProperty)
	{
		delete []m_pFieldProperty;
		m_pFieldProperty=NULL;
	};
	m_pFieldProperty=new CFieldProperty[m_head.lFieldQty+10];

	if(m_pRecord)
	{
		delete []m_pRecord;
		m_pRecord=NULL;
	}
	m_pRecord=new COleVariant[m_head.lFieldQty+10];
	
	if(m_pRecordStart)
	{
		delete []m_pRecordStart;
		m_pRecordStart=NULL;
	}
	m_pRecordStart=new long [m_head.lRecordQty+10];

	fread(m_pFieldProperty,sizeof(CFieldProperty),m_head.lFieldQty,m_fpDataFile);
	fread(m_pRecordStart,sizeof(long),m_head.lRecordQty,m_fpDataFile);

	////////////////////////////
	// Get the max record len:
	////////////////////////////
	int iMaxRecordLen=0;
	int nTemp=0;
	for(int i=0;i<m_head.lRecordQty;i++)
	{
		nTemp=m_pRecordStart[i+1]-m_pRecordStart[i];
		if(nTemp>iMaxRecordLen)
		{
			iMaxRecordLen=nTemp;
		}
	}
	if(m_pRecordData)
	{
		delete []m_pRecordData;
		m_pRecordData=NULL;
	}
	m_pRecordData=new char[iMaxRecordLen+10];


	return true;
}

bool CDataFile::WriteRecord(COleVariant *pVariant, int iFieldQty)
{	
	if(iFieldQty!=m_head.lFieldQty || !m_fpDataFile||!m_bWriting)
	{
		return false;
	}

	int i;
	for(i=0;i<iFieldQty;i++)
	{
		if(pVariant[i].vt!=m_pFieldProperty[i].type )
		{
			return false;
		}
	}

	// Start Pos in the file:
	long lPos=ftell(m_fpDataFile);
	m_pRecordStart[m_iWritingOrder]=lPos;
	m_iWritingOrder++;

	// write into the file:
	CString sValue;
	int iDataLen=0;
	for(i=0;i<iFieldQty;i++)
	{
		WriteVariant(m_fpDataFile,pVariant[i]);
	}
}

char * CDataFile::ReadRecordData(int iOrder,long &lDataLen)
{
	if(!m_fpDataFile||m_bWriting||!m_pRecord||!m_pRecordStart||!m_pFieldProperty
		||iOrder<0||iOrder>=m_head.lRecordQty)
	{
		return NULL;
	}

	// Start Pos in the file:
	long lPos=m_pRecordStart[iOrder];
	if(lPos<sizeof(CDataFileHead))
	{
		return NULL;
	}

	lDataLen=m_pRecordStart[iOrder+1]-m_pRecordStart[iOrder];
	fseek(m_fpDataFile,lPos,SEEK_SET);
	fread(m_pRecordData,1,lDataLen,m_fpDataFile);

	return m_pRecordData;
}

COleVariant * CDataFile::ReadRecord(int iOrder)
{	
	if(!m_fpDataFile||m_bWriting||!m_pRecord||!m_pRecordStart||!m_pFieldProperty
		||iOrder<0||iOrder>=m_head.lRecordQty)
	{
		return NULL;
	}

	// Start Pos in the file:
	long lPos=m_pRecordStart[iOrder];
	if(lPos<sizeof(CDataFileHead))
	{
		return NULL;
	}

	fseek(m_fpDataFile,lPos,SEEK_SET);

	// write into the file:
	CString sValue;
	int iDataLen=0;
	for(int i=0;i<m_head.lFieldQty ;i++)
	{
		if(!ReadVariant(m_fpDataFile,m_pRecord[i]))return NULL;
	}

	return m_pRecord;
}

bool CDataFile::Close()
{
	if(m_fpDataFile)
	{
		if(m_bWriting)
		{
			m_pRecordStart[m_iWritingOrder]=ftell(m_fpDataFile);

			fseek(m_fpDataFile,sizeof(CDataFileHead),SEEK_SET);
			fwrite(m_pFieldProperty,sizeof(CFieldProperty),m_head.lFieldQty,m_fpDataFile);			
			fwrite(m_pRecordStart,sizeof(long),m_head.lRecordQty,m_fpDataFile);
		}

		fclose(m_fpDataFile);
	}
	return true;
}

bool CDataFile::ReadVariant(FILE *fp,COleVariant& varSrc)
{
	LPVARIANT pSrc = &varSrc;

	// Free up current data if necessary
	if (pSrc->vt != VT_EMPTY)
		VariantClear(pSrc);

	fread(&pSrc->vt,sizeof(short),1,fp);

	// No support for VT_BYREF & VT_ARRAY
	if (pSrc->vt & VT_BYREF || pSrc->vt & VT_ARRAY)
		return false;

	switch (pSrc->vt)
	{
	case VT_BOOL:
		return fread(&pSrc->boolVal ,sizeof(WORD),1,fp);
	case VT_I1:
		return fread(&pSrc->cVal,sizeof(char),1,fp);
		//return ar >> pSrc->cVal;
	case VT_UI1:
		return fread(&pSrc->bVal,sizeof(char),1,fp);
		//return ar >> pSrc->bVal;
	case VT_I2:
		return fread(&pSrc->iVal,2,1,fp);
		//return ar >> pSrc->iVal;
	case VT_UI2:
		return fread(&pSrc->uiVal,2,1,fp);
		//return ar >> pSrc->uiVal;
	case VT_I4:
		return fread(&pSrc->lVal,4,1,fp);
		//return ar >> pSrc->lVal;
	case VT_UI4:
		return fread(&pSrc->ulVal,4,1,fp);
		//return ar >> pSrc->ulVal;
	case VT_I8:
		return fread(&pSrc->llVal,8,1,fp);
		//return ar >> pSrc->llVal;

	case VT_UI8:
		return fread(&pSrc->ullVal,8,1,fp);
		//return ar >> pSrc->ullVal;

	case VT_CY:
			   fread(&pSrc->cyVal.Lo,8,1,fp);
		return fread(&pSrc->cyVal.Hi,8,1,fp);
		//ar >> pSrc->cyVal.Lo;
		//return ar >> pSrc->cyVal.Hi;

	case VT_R4:
		fread(&pSrc->fltVal,4,1,fp);
		//return ar >> pSrc->fltVal;

	case VT_R8:
		fread(&pSrc->dblVal,8,1,fp);
		//return ar >> pSrc->dblVal;

	case VT_DATE:
		fread(&pSrc->date,8,1,fp);
		//return ar >> pSrc->date;

	case VT_BSTR:
		{
			int nLen=0;
			fread(&nLen,sizeof(int),1,fp);

			if (nLen > 0)
			{
				pSrc->bstrVal = SysAllocStringByteLen(NULL, nLen);
				if (pSrc->bstrVal == NULL)
					AfxThrowMemoryException();
				fread(pSrc->bstrVal,sizeof(BYTE),nLen,fp);
				//ar.Read(pSrc->bstrVal, nLen * sizeof(BYTE));
			}
			else
				pSrc->bstrVal = NULL;
		}
		return true;
	case VT_ERROR:
		return fread(&pSrc->scode,sizeof(long),1,fp);
		//return ar >> pSrc->scode;

	case VT_DISPATCH:
	case VT_UNKNOWN:
	case VT_EMPTY:
	case VT_NULL:
	default:
		ASSERT(FALSE);
		return false;
	}
}

bool CDataFile::ReadVariant(char *psString,COleVariant& varSrc)
{
	LPVARIANT pSrc = &varSrc;

	// Free up current data if necessary
	if (pSrc->vt != VT_EMPTY)
		VariantClear(pSrc);

	memcpy(&pSrc->vt,psString,sizeof(short));
	psString=&psString[sizeof(short)];

	// No support for VT_BYREF & VT_ARRAY
	if (pSrc->vt & VT_BYREF || pSrc->vt & VT_ARRAY)
		return false;

	switch (pSrc->vt)
	{
	case VT_BOOL:
		memcpy(&pSrc->boolVal,psString,sizeof(WORD));
		psString=&psString[sizeof(WORD)];
		return true;
		//return fread(&pSrc->boolVal ,sizeof(WORD),1,fp);
	case VT_I1:
		memcpy(&pSrc->cVal,psString,sizeof(char));
		psString=&psString[sizeof(char)];
		return true ;
		//return fread(&pSrc->cVal,sizeof(char),1,fp);
		//return ar >> pSrc->cVal;
	case VT_UI1:
		memcpy(&pSrc->bVal,psString,sizeof(char));
		psString=&psString[sizeof(char)];
		return true;
		//return fread(&pSrc->bVal,sizeof(char),1,fp);
		//return ar >> pSrc->bVal;
	case VT_I2:
		memcpy(&pSrc->iVal,psString,2);
		psString=&psString[2];
		return true;

		//return fread(&pSrc->iVal,2,1,fp);
		//return ar >> pSrc->iVal;
	case VT_UI2:
		memcpy(&pSrc->uiVal,psString,2);
		psString=&psString[2];
		return true;

		//return fread(&pSrc->uiVal,2,1,fp);
		//return ar >> pSrc->uiVal;
	case VT_I4:
		memcpy(&pSrc->lVal,psString,4);
		psString=&psString[4];
		return true;

		//return fread(&pSrc->lVal,4,1,fp);
		//return ar >> pSrc->lVal;
	case VT_UI4:
		memcpy(&pSrc->ulVal,psString,4);
		psString=&psString[4];
		return true;

		//return fread(&pSrc->ulVal,4,1,fp);
		//return ar >> pSrc->ulVal;
	case VT_I8:
		memcpy(&pSrc->llVal,psString,8);
		psString=&psString[8];
		return true;

		//return fread(&pSrc->llVal,8,1,fp);
		//return ar >> pSrc->llVal;

	case VT_UI8:
		memcpy(&pSrc->ullVal,psString,8);
		psString=&psString[8];
		return true;

		//return fread(&pSrc->ullVal,8,1,fp);
		//return ar >> pSrc->ullVal;

	case VT_CY:
		memcpy(&pSrc->cyVal.Lo,psString,8);
		psString=&psString[8];
		memcpy(&pSrc->cyVal.Hi,psString,8);
		psString=&psString[8];
		return true;

		//fread(&pSrc->cyVal.Lo,8,1,fp);
		//return fread(&pSrc->cyVal.Hi,8,1,fp);
		//ar >> pSrc->cyVal.Lo;
		//return ar >> pSrc->cyVal.Hi;

	case VT_R4:
		memcpy(&pSrc->fltVal,psString,4);
		psString=&psString[4];
		return true;

		//fread(&pSrc->fltVal,4,1,fp);
		//return ar >> pSrc->fltVal;

	case VT_R8:
		memcpy(&pSrc->dblVal,psString,8);
		psString=&psString[8];
		return true;

		//fread(&pSrc->dblVal,8,1,fp);
		//return ar >> pSrc->dblVal;

	case VT_DATE:
		memcpy(&pSrc->date,psString,8);
		psString=&psString[8];
		return true;

		//fread(&pSrc->date,8,1,fp);
		//return ar >> pSrc->date;

	case VT_BSTR:
		{
			int nLen=0;
			memcpy(&nLen,psString,sizeof(int));
			psString=&psString[sizeof(int)];
			//return;
			//fread(&nLen,sizeof(int),1,fp);

			if (nLen > 0&&nLen<1000000)
			{
				pSrc->bstrVal = SysAllocStringByteLen(NULL, nLen);
				if (pSrc->bstrVal == NULL)
					AfxThrowMemoryException();
	
				memcpy(pSrc->bstrVal,psString,nLen);
				psString=&psString[nLen];
				//fread(pSrc->bstrVal,sizeof(BYTE),nLen,fp);
				//ar.Read(pSrc->bstrVal, nLen * sizeof(BYTE));
			}
			else
				pSrc->bstrVal = NULL;
		}
		return true;
	case VT_ERROR:
		memcpy(&pSrc->scode,psString,sizeof(long));
		psString=&psString[sizeof(long)];
		return true;

		//return fread(pSrc->scode,sizeof(long),1,fp);
		//return ar >> pSrc->scode;

	case VT_DISPATCH:
	case VT_UNKNOWN:
	case VT_EMPTY:
	case VT_NULL:
	default:
		ASSERT(FALSE);
		return false;
	}
}

bool CDataFile::WriteVariant(FILE *fp,COleVariant& varSrc)
{
	LPVARIANT pSrc = &varSrc;
	int nLen=0;

	// Free up current data if necessary
	if (pSrc->vt != VT_EMPTY)
		VariantClear(pSrc);

	fwrite(&pSrc->vt,sizeof(short),1,fp);

	// No support for VT_BYREF & VT_ARRAY
	if (pSrc->vt & VT_BYREF || pSrc->vt & VT_ARRAY)
	{
		return false;
	}

	switch (pSrc->vt)
	{
	case VT_BOOL:
		return fwrite(&(WORD&)V_BOOL(pSrc),sizeof(WORD),1,fp);
	case VT_I1:
		return fwrite(&pSrc->cVal,sizeof(char),1,fp);
		//return ar >> pSrc->cVal;
	case VT_UI1:
		return fwrite(&pSrc->bVal,sizeof(char),1,fp);
		//return ar >> pSrc->bVal;
	case VT_I2:
		return fwrite(&pSrc->iVal,2,1,fp);
		//return ar >> pSrc->iVal;
	case VT_UI2:
		return fwrite(&pSrc->uiVal,2,1,fp);
		//return ar >> pSrc->uiVal;
	case VT_I4:
		return fwrite(&pSrc->lVal,4,1,fp);
		//return ar >> pSrc->lVal;
	case VT_UI4:
		return fwrite(&pSrc->ulVal,4,1,fp);
		//return ar >> pSrc->ulVal;
	case VT_I8:
		return fwrite(&pSrc->llVal,8,1,fp);
		//return ar >> pSrc->llVal;

	case VT_UI8:
		return fwrite(&pSrc->ullVal,8,1,fp);
		//return ar >> pSrc->ullVal;

	case VT_CY:
			   fwrite(&pSrc->cyVal.Lo,8,1,fp);
		return fwrite(&pSrc->cyVal.Hi,8,1,fp);
		//ar >> pSrc->cyVal.Lo;
		//return ar >> pSrc->cyVal.Hi;

	case VT_R4:
		fwrite(&pSrc->fltVal,4,1,fp);
		//return ar >> pSrc->fltVal;

	case VT_R8:
		fwrite(&pSrc->dblVal,8,1,fp);
		//return ar >> pSrc->dblVal;

	case VT_DATE:
		fwrite(&pSrc->date,8,1,fp);
		//return ar >> pSrc->date;

	case VT_BSTR:
		nLen=strlen((char*)pSrc->bstrVal);
		fwrite(&nLen,sizeof(int),1,fp);
		return fwrite(pSrc->bstrVal,nLen,1,fp);
	case VT_ERROR:
		return fwrite(&pSrc->scode,sizeof(long),1,fp);
		//return ar >> pSrc->scode;

	case VT_DISPATCH:
	case VT_UNKNOWN:
	case VT_EMPTY:
	case VT_NULL:
	default:
		ASSERT(FALSE);
		return false;
	}
}

long CDataFile::GetRecordQty()
{
	return m_head.lRecordQty ;
}

long CDataFile::GetFieldQty()
{
	return m_head.lFieldQty ;
}
