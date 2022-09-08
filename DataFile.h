#pragma once

class CFieldProperty
{
public:
	char		name[16];
	VARENUM		type;

	CFieldProperty()
	{
		memset(this,0,sizeof(CFieldProperty));
	};
};

class CDataFileHead
{
public:
	char sMark[16];
	char sDataBaseName[256];
	char sTableName[256];
	long lFieldQty;
	long lRecordQty;
	
	CDataFileHead()
	{
		memset(this,0,sizeof(CDataFileHead));
		strcpy(sMark,"DATAFILEHEAD");
	};

	bool IsValid()
	{
		return (strcmp(sMark,"DATAFILEHEAD")==0);
	};
};

class CDataFile
{
protected:
	CDataFileHead	m_head;
	CFieldProperty	*m_pFieldProperty;
	long			*m_pRecordStart;
	long			m_lBodyStartPos;

	FILE			*m_fpDataFile;
	bool			m_bWriting;
	long			m_iWritingOrder;

	COleVariant		*m_pRecord;
	char			*m_pRecordData;

	bool ReadVariant(FILE *fp,COleVariant& varSrc);
	bool ReadVariant(char *psString,COleVariant& varSrc);
	bool WriteVariant(FILE *fp,COleVariant& varSrc);

public:
	CDataFile(void);
	~CDataFile(void);

	bool SetInfo(	char sDataBase[], 
					char sTableName[], 
					long lFieldQty,
					long lRecordQty);

	bool CreateDataFile(CString sFileName);
	bool WriteRecord(COleVariant *pVariant, int iFieldQty);
	COleVariant * ReadRecord(int iRecordOrder);
	char * ReadRecordData(int iOrder,long &lDataLen);


	
	bool OpenDataFile(CString sFileName);
	long GetRecordQty();
	long GetFieldQty();


	bool Close();

};
