#pragma once

enum enumDataType
{
	DT_CHAR,
	DT_TEXT,
	DT_INTEGER,
	DT_LONG,
	DT_FLOAT,
	DT_BOOL,
	DT_DOUBLE,
	DT_DATE,
	DT_NOTE,
	DT_BLOG
};

class CFieldData
{
public:
	char			sFieldName[100];
	enumDataType	enumType;
	char			sFieldType[16];
	int				iStartPos;
	int				iLen;

	CFieldData()
	{
		memset(sFieldName,0,100);
		enumType=DT_INTEGER;
		iStartPos=0;
		iLen=0;
	};

	~CFieldData()
	{
	};

	void Set(CString sName, enumDataType type, int iLenTag=0)
	{
		strcpy(sFieldName,(LPCTSTR)sName);
		enumType=type;

		if(enumType==DT_CHAR)
		{
			iLen=iLenTag;
			sprintf(sFieldType,"CHAR(%d)",iLen);
		}
		else if(enumType==DT_TEXT)
		{
			iLen=iLenTag;
			sprintf(sFieldType,"TEXT(%d)",iLen);
		}
		else if(enumType==DT_INTEGER)
		{
			iLen=sizeof(int);
			sprintf(sFieldType,"INTEGER");
		}
		else if(enumType==DT_LONG)
		{
			iLen=sizeof(long);
		}
		else if(enumType==DT_FLOAT)
		{
			iLen=sizeof(FLOAT);
			sprintf(sFieldType,"FLOAT");
		}
		else if(enumType==DT_DOUBLE)
		{
			iLen=sizeof(DOUBLE);
			sprintf(sFieldType,"DOUBLE");
		}
		else if(enumType==DT_DATE)
		{
			iLen=14;  //  (XXXXÄêXXÔÂXXÈÕ)
			sprintf(sFieldType,"DATE(14)");
		}
		else if(enumType==DT_NOTE)
		{
			iLen=1024;
			sprintf(sFieldType,"NOTE(1024)");
		}
		else if(enumType==DT_BLOG)
		{
			iLen=256; // save the file name.
			sprintf(sFieldType,"BLOG(256)");
		}
		else if(enumType==DT_BOOL)
		{
			iLen=4; 
			sprintf(sFieldType,"BOOL(4)");
		}

		else
		{
			ASSERT(false);
			iLen=sizeof(long);
		};
	}

	CString GetType()
	{
		return sFieldType;
	};
};

enum enumDBFileType
{
	DBFT_COMMON,
	DBFT_ADD,
	DBFT_UPDATE,
	DBFT_DELETE
};

class CDBFileHead
{
public:
	char sMark[32];
	char sSourceIP[128];
	int  iSourcePort;
	char sTargetIP[128];
	int  iTargetPort;
	char sTableName[128];
	int  iRecordStartPos;
	int  iFieldQty;
	int  iRecordLen;
	int  iRecordQty;	
	char sSQL[2000];
	enumDBFileType eDBFT;

	CDBFileHead()
	{
		memset(this,0,sizeof(CDBFileHead));
		strcpy(sMark,"DB_FILE_MODIFIABLE");
		eDBFT=DBFT_COMMON;
	};

	bool IsValid()
	{
		return (strcmp(sMark,"DB_FILE_MODIFIABLE")==0);
	}
};

#define RS_NORMAL		0
#define RS_MODIFIED		1
#define RS_DELETED		2
#define RS_ADDED		4

#define WM_PICPROPERTY WM_USER+1002

#include "ReportReader.h"


class CDataBrowseDoc : public CDocument
{
	DECLARE_DYNCREATE(CDataBrowseDoc)
	friend class CDataBrowseView;
	friend class CPicShowView;

public:
	CDataBrowseDoc();
	virtual ~CDataBrowseDoc();
	virtual void Serialize(CArchive& ar);   // overridden for document i/o
#ifdef _DEBUG
	virtual void AssertValid() const;
	virtual void Dump(CDumpContext& dc) const;
#endif

	void OnFileSave();


public:
	bool			SetTableName(CString sTable);
	bool			SetField(CStringArray &arrayName, CStringArray &arrayType);
	bool			SetFileType(enumDBFileType eFT);
	enumDBFileType	GetFileType();

	void			SetSQL(CString sSQL);
	CString			GetSQL();

	CString			Initiate(CDataBrowseDoc *pDoc);


	// DBFile Creation:
	bool			CreateDBFile(CString sFileName="");
	bool			UpdateRecord(int iOrder, CStringArray *pArrayRecord);
	int				AppendRecord(CStringArray *pArrayRecord);
	bool			UpdateField(int iRecord, int iField, CString sField);

	// DBFile Reading:
	bool			OpenDBFile(CString sFileName);	
	void			Close();

	CString			GetFileName();
	CString			GetTableName();
	int				GetRecordLen();
	int				GetRecordQty();
	int				GetFieldQty();
	CStringArray	*GetRecord(int iRecordOrder);
	CFieldData		*GetFields();
	void			GetFields(CStringArray &array);
	int				CalcRecordQty();	


	bool			SetDeletedFlag(int iRecord);
	bool			ResetDeletedFlag(int iRecord);
	bool			SetUpdatedFlag(int iRecord);
	bool			SetAddedFlag(int iRecord);
	void			UpdateRecordStatus(int iRecord,int iAdded,int iRemoved);
	void			GetIndexFields(CStringArray &array);


	bool			AddEmptyRecord();
    bool            CloneLastRecord();

	char			GetRecordStatus(int iRecord);
	CString			GetRecordID(int iRecord);

	CString			GetSourceIP();
	int				GetSourcePort();
	void			SetSourceIP(char sServerIP[]);
	void			SetSourceIP(CString  sServerIP);
	void			SetSourcePort(int iPort);

	void			SetTargetIP(CString sIP);
	void			SetTargetPort(int iPort);
	CString			GetTargetIP();
	int				GetTargetPort();

	/////////////////////////////
	//
	/////////////////////////////
	void			SendDataToServer(bool bOnlyNotHandedIn);

    virtual void    UpdateDB();
	virtual BOOL    UpdateFromDB();

protected:

	
	int		m_iPicHeight ;
	int		m_iPicWidth ;
	bool	m_bPicKeepRatio;
	bool	m_bDrawCellPic		;

	CString m_sBigPicName;
	int		m_iBigPicWidth  ;
	int		m_iBigPicHeight ;
	bool	m_bBigPicKeepRatio;
	bool	m_bDrawBigPic		;
	int		m_iBigPicOffsetX	;
	int		m_iBigPicOffsetY	;

	bool	m_bDBPicKeepRatio;
	int		m_iDBPicWidth;
	int		m_iDBPicHeight;

protected:
	FILE			*m_fp;
	CDBFileHead		m_head;
	CFieldData		*m_pFieldsData;
	CString			m_sDBFileName;
	CStringArray	m_arrayRecord;
	int				m_iMarkFieldOrder;

	char			*m_pTempBuffer;
	int				m_iTempBufferLen;

	int				m_iRecordStatusQty;
	char			*m_pRecordStatus;

	bool			WriteFileHead();
	bool			ReadFileHead();

	static int		GetNumberInBracket(CString sData);
	bool			CalcFieldPosition();

	int				WriteField(	char		*psBuffer,
								CString		sField,
								CFieldData	*pFieldData);


	CString			ReadField(	char		*psBuffer,
								CFieldData	*pFieldData);

	bool			SetFieldQty(int iFieldQty);
	bool			SetField(int iOrder,CString sName,CString type);
	int				CalcMarkFieldOrder();

	void			ClearRecordStatus();
	void			ClearRecordStatus(int iRecord);
	bool			RecordModified();
	void			SetModifiedFlag(BOOL bModified);
    void            SavePar();




	virtual BOOL OnNewDocument();

	DECLARE_MESSAGE_MAP()
public:
	virtual BOOL OnOpenDocument(LPCTSTR lpszPathName);
	afx_msg void OnSetPicProperty();
	virtual BOOL OnSaveDocument(LPCTSTR lpszPathName);
protected:
	virtual BOOL SaveModified();
public:
	afx_msg void OnModifyData();
	afx_msg void OnSenddatatoservernohanded();
	afx_msg void OnSenddatatoserverall();
};

