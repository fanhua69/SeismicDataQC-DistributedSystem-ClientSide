#pragma once
#import "c:\Program Files\Common Files\System\ADO\msado15.dll" no_namespace rename("EOF","EndOfFile") rename ("BOF","FirstOfFile")
#include "ICRSINT.h"
#include "afxdb.h"

enum enumFieldType
{
	FT_INT, //整形
	FT_LONG, //长整形
	FT_FLOAT, // 实形
	FT_DATE,  // 日期
	FT_NOTE,  // 备注
	FT_CHAR   // 字符
};

#include "DataBrowseDoc.h"
#include "MyDate.h"

class CDataBaseManager
{
public:
	CDataBaseManager(void);
	~CDataBaseManager(void);

	_ConnectionPtr	m_pConnection;

	bool Initiate(CString sDataBase);
	bool CloseDataBase();

	bool Execute(char sSQL[]);

	bool AddRecord		(		CString sDBDoc);
	bool DeleteRecord	(		CString	sDBDoc);
	bool UpdateRecord	(		CString sDBDoc);

	bool AddRecord		(		CString			sTable,
								CStringArray	&arrayFields,
								CStringArray	&arrayValues);


	bool SendTableToBrowseDoc(	CString			sTable,
								CString			sSQL,
								CTableDate		dateStart,
								CStringArray	*pArrayFields,
								CDataBrowseDoc	*pDoc,
								int				iMaxTransferRecord,
								bool			bMarkHandedIn);


	bool GetRecord(					_RecordsetPtr	pRecord,
									CStringArray	&arrayFields,
									CStringArray	&arrayRecord,
									bool			bWithPic);
	
    bool ClearDB    ();

public:

	static bool RegisterDBSrc(	CString			sSrcAccess, 
								CString			sODBCName,
								CString			sDiscription,
								bool			bAdd);
	
	static bool ExtractFieldNames(CString sSummedFields,CStringArray &arrayFields);

	// Used by SendTableToBrowseDoc
	void AddDateField(	CString			sTable,
						CStringArray	*pArrayFields,
						CTableDate		dateStart,
						int				&iYearFieldOrder,
						int				&iMonthFieldOrder,
						int				&iDayFieldOrder);



public:
	static bool	CreateTablesFromBlueDB(
								CString			sTarDBName,
								CString			sBlueDBName);

protected:
	CString m_sDBName;
	CString m_sTable;
	char m_sTableCreateSQL[5000];
	char m_sTemp[1000];

	bool m_bSuccess;

	char *m_pTempBuffer;
	int  m_iTempBufferLen;


	void PutPicToRecord			(		_RecordsetPtr	pRecord,
										CString			sFieldName,
										CString			sFieldValue);

	CString GetValueFromRecord	(		_RecordsetPtr	pRecord,
										CString			sFieldName);

	CString GetPicFromRecord	(		_RecordsetPtr	pRecord,
										CString			sFieldName);

};
