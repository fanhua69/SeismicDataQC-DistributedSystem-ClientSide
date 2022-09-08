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


class CDataBaseStManager
{
public:
	CDataBaseStManager(void);
	~CDataBaseStManager(void);

	_ConnectionPtr	m_pConnection;
	_RecordsetPtr	m_pRecords;

	bool Initiate(CString sDataBase);
	bool CloseDataBase();


	bool Update(				CString			sTable,
								CString			sCondition,
 								CStringArray	&arrayFields,
								CStringArray	&arrayNewValue);

	void MarkHandedIn();

	bool AddRecord(				CString			sTable,
								CStringArray	&arrayFields,
								CStringArray	&arrayValues);

	CStringArray *	GetRecord(	bool bWithPicture = false);
	
	bool Update	(				CStringArray	&arrayRecord);

	bool DeleteRecord(			CString			sTable,
 								CString			sCondition);

	void	SetSelectField(CStringArray &array);

public:

	static bool RegisterDBSrc(	CString			sSrcAccess, 
								CString			sODBCName,
								CString			sDiscription,
								bool			bAdd);

public:
	static bool	CreateTablesFromBlueDB(
								CString			sTarDBName,
								CString			sBlueDBName);

	bool Initiated() { return m_bInitiated; };

	CStringArray	m_arrayFieldsAll;
	CStringArray	m_arrayFieldsAllType;
	CStringArray	m_arrayFieldsCurrent;
	CStringArray	m_arrayRecords;
	CStringArray	m_arrayRecord;

protected:
	CString m_sDBName;
	CString m_sTable;
	char m_sTableCreateSQL[5000];
	char m_sTemp[1000];
	char m_sUserID[100];
	char m_sPassword[100];

	long m_nCurrentRecord;

	bool m_bSuccess;
	int  m_nRecordQty;
	bool m_bInitiated;

	char *m_pTempBuffer;
	int  m_iTempBufferLen;

	bool CheckRecordValid();

	void PutPicToRecord			(		_RecordsetPtr	pRecord,
										CString			sFieldName,
										CString			sFieldValue);

	CString GetValueFromRecord	(		_RecordsetPtr	pRecord,
										CString			sFieldName);

	CString GetPicFromRecord	(		_RecordsetPtr	pRecord,
										CString			sFieldName);

};
