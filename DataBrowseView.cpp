// DataBrowseView.cpp : implementation file
//

#include "stdafx.h"
#include "QCDB.h"
#include "DataBrowseDoc.h"
#include "DataBrowseView.h"
#include "FHCGlobal.h"
#include "FHMainFrm.h"
#include "DataBrowseDoc.h"
#include "DlgChoosePage.h"
#include "DlgDBShowProperty.h"
#include "DlgDataSelector.h"
#include "qcglobal.h"
#include ".\databrowseview.h"

// CDataBrowseView

IMPLEMENT_DYNCREATE(CDataBrowseView, CFHEditView)

CDataBrowseView::CDataBrowseView()
{
	m_iRecordStart=0;
	m_iRecordPerPage=10;
	m_iTotalRecord=0;

	m_bVerticalShow=true;
	GetPar("DATABROWSE_VIEW_RecordPerPage",m_iRecordPerPage);
	GetPar("DATABROWSE_VIEW_VerticalShow",m_bVerticalShow);



	m_menuManager.SetPopupMenu (IDR_Menu_DataBrowseView);
}

CDataBrowseView::~CDataBrowseView()
{
}

BEGIN_MESSAGE_MAP(CDataBrowseView, CFHEditView)
	ON_MESSAGE(WM_PICPROPERTY,OnPicPropertyChanged)

	ON_COMMAND(ID_NextPage, OnNextpage)
	ON_COMMAND(ID_PrevPage, OnPrevpage)
	ON_UPDATE_COMMAND_UI(ID_NextPage, OnUpdateNextpage)
	ON_UPDATE_COMMAND_UI(ID_PrevPage, OnUpdatePrevpage)
	ON_COMMAND(ID_ChoosePage, OnChoosepage)
	ON_COMMAND(ID_DeleteRecord, OnDeleterecord)
	ON_COMMAND(ID_AddRecord, OnAddrecord)
    ON_COMMAND(ID_CloneLastRecord, OnCloneLastRecord)    
	ON_COMMAND(ID_RecoverRecord, OnRecoverrecord)
	ON_COMMAND(ID_UpdateDataFromDB, OnUpdatedatafromdb)
	ON_COMMAND(ID_UpdateDB, OnUpdatedb)
	ON_UPDATE_COMMAND_UI(ID_UpdateDB, OnUpdateUpdatedb)
	ON_COMMAND(ID_LoadPic, OnLoadpic)
	ON_WM_LBUTTONUP()
	ON_COMMAND(ID_ShowHor, OnShowhor)
	ON_UPDATE_COMMAND_UI(ID_ShowHor, OnUpdateShowhor)
END_MESSAGE_MAP()


// CDataBrowseView drawing
#include  "io.h"
void CDataBrowseView::OnDraw(CDC* pDC)
{
}


// CDataBrowseView diagnostics

#ifdef _DEBUG
void CDataBrowseView::AssertValid() const
{
	CFHEditView::AssertValid();
}

void CDataBrowseView::Dump(CDumpContext& dc) const
{
	CFHEditView::Dump(dc);
}
#endif //_DEBUG

// CDataBrowseView message handlers
void CDataBrowseView::UpdateGrid()
{
	if(m_bVerticalShow)
	{
		UpdateGridVer();
	}
	else
	{
		UpdateGridHor();
	}
}

void CDataBrowseView::UpdateGridHor()
{
	CDataBrowseDoc *pDoc=(CDataBrowseDoc *)GetDocument();
	m_iTotalRecord=pDoc->GetRecordQty ();
	int iItemQty=pDoc->GetFieldQty();
	
	if(m_iTotalRecord<m_iRecordStart)
	{
		m_iRecordStart=0;
	}


	int iShowRow=m_iRecordPerPage;
	if(m_iTotalRecord-m_iRecordStart<iShowRow)
	{
		iShowRow=m_iTotalRecord-m_iRecordStart;
	}

	SetSize(iShowRow+2,iItemQty+2);
	m_Grid.SetFixedColumnCount (3);
	m_Grid.SetFixedRowCount (2);

	///////////////////////////////////
	// 加入文件中当前页所属行：
	///////////////////////////////////
	long lLineOrder=0;
	int i,j;
	
	CFieldData *pFields=pDoc->GetFields();

	char s[100];
	sprintf(s,"页%d/%d",GetCurrentPage(),GetTotalPage());
	SetItem(0,0,s);
	SetItem(1,1,"状态");

	for(i=0;i<iItemQty;i++)
	{
		SetItem(0,i+2,pFields[i].sFieldName);
		SetItem(1,i+2,GetDataTypeName(&pFields[i]));
	}
	

	CStringArray *pRecord=NULL;
	int iRecord=0;
	CString sItem;
	for(i=0;i<m_iRecordPerPage;i++)
	{
		iRecord=m_iRecordStart+i;
		if(iRecord>=m_iTotalRecord)break;

		SetItem (i+2,0,iRecord+1);
		pRecord =  pDoc->GetRecord (iRecord);
		if(!pRecord)continue;
		if(pRecord->GetCount ()!=iItemQty)
		{
			continue;
		}
		for(j=0;j<iItemQty;j++)
		{
			sItem=pRecord->GetAt (j);
			sItem.Trim();
            if(     pFields[j].enumType ==  DT_BOOL
                ||  pFields[j].enumType ==  DT_CHAR &&  pFields[j].iLen==2)
            {
                if(sItem!="0")
                {
                    sItem="Y";
                }
                else
                {
                    sItem="N";
                }
            }
			SetItem (i+2,j+2,sItem);
		}
	}

	OnPicPropertyChanged(0,0);

    //
    int iFixedRowOrColQty=2;
    bool bSetRowToCol=false;
    SetComboxContents(  pDoc->GetTableName(),
                        pDoc->GetFields(),
                        pDoc->GetFieldQty(),
                        iFixedRowOrColQty, 
                        bSetRowToCol);


	UpdateFlag();

	m_Grid.AutoSize ();
}

CString  CDataBrowseView::GetDataTypeName(CFieldData *pField)
{
    enumDataType eType=pField->enumType;
    int iLen=pField->iLen;
    CString sType;
    if(eType==DT_CHAR)
    {
        if(iLen==2)
        {
            sType="Y/N";
        }
        else if(iLen==3)
        {
            sType="A,B,C,D,E";
        }
        else if(iLen==4)
        {
            sType="A,B,C";
        }
        else if(iLen==5)
        {
            sType="男/女";
        }
        else if(iLen==6)
        {
            sType="12:59";
        }
        else if(iLen%11==0)
        {
            sType="姓名";
        }

        else
        {
            sType.Format("字符(%d)",iLen);
        }
    }
    else if(eType==DT_TEXT)
    {
        sType.Format("文本(%d)",iLen);
    }
    else if(eType==DT_INTEGER)
    {
        sType.Format("整形数");
    }
    else if(eType==DT_LONG)
    {
        sType.Format("长整形数");
    }
    else if(eType==DT_FLOAT)
    {
        sType.Format("浮点型数");
    }
    else if(eType==DT_BOOL)
    {
        sType.Format("布尔型");
    }
    else if(eType==DT_DOUBLE)
    {
        sType.Format("双精度数");
    }
    else if(eType==DT_DATE)
    {
        sType.Format("日期(XXXX年XX月XX日");
    }
    else if(eType==DT_NOTE)
    {
        sType.Format("注释(%d)",iLen);
    }
    else if(eType==DT_BLOG)
    {
        sType.Format("文件名");
    }

    return sType;
}


void CDataBrowseView::UpdateGridVer()
{
	CDataBrowseDoc *pDoc=(CDataBrowseDoc *)GetDocument();
	m_iTotalRecord=pDoc->GetRecordQty ();
	int iItemQty=pDoc->GetFieldQty();
	
	if(m_iTotalRecord<m_iRecordStart)
	{
		m_iRecordStart=0;
	}


	int iRecordRow=m_iRecordPerPage;
	if(m_iTotalRecord-m_iRecordStart<iRecordRow)
	{
		iRecordRow=m_iTotalRecord-m_iRecordStart;
	}

	SetSize(iItemQty+2,iRecordRow+2);
	m_Grid.SetFixedRowCount (3);
	m_Grid.SetFixedColumnCount (2);

	///////////////////////////////////
	// 加入文件中当前页所属行：
	///////////////////////////////////
	long lLineOrder=0;
	int i,j;
	
	CFieldData *pFields=pDoc->GetFields();

	char s[100];
	sprintf(s,"页%d/%d",GetCurrentPage(),GetTotalPage());
	SetItem(0,0,s);
	SetItem(1,1,"状态");

    CString sType;
    int iLen=0;
	for(i=0;i<iItemQty;i++)
	{
		SetItem(i+2,0,pFields[i].sFieldName);
		SetItem(i+2,1,GetDataTypeName(&pFields[i]));
	}
	

	CStringArray *pRecord=NULL;
	int iRecord=0;
	CString sItem;
	for(i=0;i<m_iRecordPerPage;i++)
	{
		iRecord=m_iRecordStart+i;
		if(iRecord>=m_iTotalRecord)break;

		SetItem (0,i+2,iRecord+1);
		pRecord =  pDoc->GetRecord (iRecord);
		if(!pRecord)continue;
		if(pRecord->GetCount ()!=iItemQty)
		{
			continue;
		}
		for(j=0;j<iItemQty;j++)
		{
			sItem=pRecord->GetAt (j);
			sItem.Trim();
            sItem.MakeUpper();
			if(     pFields[j].enumType ==  DT_BOOL
                ||  pFields[j].enumType ==  DT_CHAR &&  pFields[j].iLen==2)
			{
                sItem=sItem.Left(1);
				if(sItem!="0"&&sItem!="N"&&sItem!="F")
				{
					sItem="Y";
				}
				else
				{
					sItem="N";
				}
			}
			SetItem (j+2,i+2,sItem);
		}
	}
   
    //
    int iFixedRowOrColQty=2;
    bool bSetRowToCol=true;
    SetComboxContents(  pDoc->GetTableName(),
                        pDoc->GetFields(),
                        pDoc->GetFieldQty(),
                        iFixedRowOrColQty, 
                        bSetRowToCol);

	OnPicPropertyChanged(0,0);

	UpdateFlag();

	m_Grid.AutoSize ();
}

void  CDataBrowseView::SetComboxContents(   CString         sTableName,
                                            CFieldData      *pFields,
                                            int             iFieldQty,
                                            int             iFixedRowOrColQty, 
                                            bool            bSetRowToCol)
{
}


void CDataBrowseView::OnNextpage()
{
	if(m_iRecordStart+m_iRecordPerPage>=m_iTotalRecord)
	{
		return;
	}
	m_iRecordStart+=m_iRecordPerPage;
	UpdateGrid();
}

void CDataBrowseView::OnPrevpage()
{
	if(m_iRecordStart-m_iRecordPerPage<0)
	{
		return;
	}
	m_iRecordStart-=m_iRecordPerPage;
	UpdateGrid();

}

void CDataBrowseView::OnUpdateNextpage(CCmdUI *pCmdUI)
{
	pCmdUI->Enable (m_iRecordStart+m_iRecordPerPage<m_iTotalRecord);
}

void CDataBrowseView::OnUpdatePrevpage(CCmdUI *pCmdUI)
{
	pCmdUI->Enable (m_iRecordStart-m_iRecordPerPage>=0);
}

void CDataBrowseView::OnUpdate(CView* /*pSender*/, LPARAM /*lHint*/, CObject* /*pHint*/)
{
	UpdateGrid();
}

int CDataBrowseView::GetCurrentPage()
{
	return m_iRecordStart/m_iRecordPerPage+1;
}

int CDataBrowseView::GetTotalPage()
{
	return (m_iTotalRecord-1)/m_iRecordPerPage+1;
}


void CDataBrowseView::OnChoosepage()
{
	// TODO: Add your command handler code here
	CDlgChoosePage dlg;
	dlg.m_iTotalRecord		=   m_iTotalRecord;
	dlg.m_iTotalPage		=	GetTotalPage();
	dlg.m_iCurrentPage		=	GetCurrentPage();
	dlg.m_iRecordTotalQty	=   m_iTotalRecord;
	dlg.m_iRecordQtyPerPage =	m_iRecordPerPage;

	if(dlg.DoModal ()==IDOK)
	{
		m_iRecordPerPage=dlg.m_iRecordQtyPerPage;
		
		AddPar("DATABROWSE_VIEW_RecordPerPage",m_iRecordPerPage);


		m_iRecordStart=(dlg.m_iChosenPage -1)*m_iRecordPerPage;
		if(m_iRecordStart<0)
		{
			m_iRecordStart=0;
		}
		if(m_iRecordStart>=m_iTotalRecord)
		{
			m_iRecordStart=0;
		}
	}

	UpdateGrid();
}

void CDataBrowseView::OnDeleterecord()
{
	CDataBrowseDoc *pDoc=(CDataBrowseDoc *)GetDocument();
	
	int iRecord=-1;
	int iStart=0,iEnd=0;
	CCellRange range=m_Grid.GetSelectedCellRange ();
	if(m_bVerticalShow)
	{
		iRecord=GetCurrentColPos()+m_iRecordStart;

		iStart=range.GetMinCol ()	-2+m_iRecordStart;
		iEnd=range.GetMaxCol ()		-2+m_iRecordStart;
	}
	else
	{
		iRecord=GetCurrentRowPos()+m_iRecordStart;
		
		iStart=range.GetMinRow ()	-2+m_iRecordStart;
		iEnd=range.GetMaxRow ()		-2+m_iRecordStart;
	}

	for(int i=iStart;i<=iEnd;i++)
	{
		pDoc->SetDeletedFlag (i);	
	}

	UpdateFlag();
	m_Grid.AutoSize ();
}

void CDataBrowseView::OnAddrecord()
{
	CDataBrowseDoc *pDoc=(CDataBrowseDoc *)GetDocument();
	pDoc->AddEmptyRecord();

	UpdateGrid();
}

void CDataBrowseView::OnCloneLastRecord()
{
    CDataBrowseDoc *pDoc=(CDataBrowseDoc *)GetDocument();
    pDoc->CloneLastRecord();

    UpdateGrid();
}



void CDataBrowseView::OnRecoverrecord()
{
	CDataBrowseDoc *pDoc=(CDataBrowseDoc *)GetDocument();
	
	int iRecord=-1;
	int iStart=0,iEnd=0;
	CCellRange range=m_Grid.GetSelectedCellRange ();
	if(m_bVerticalShow)
	{
		iRecord=GetCurrentColPos()+m_iRecordStart;

		iStart=range.GetMinCol ()	-2+m_iRecordStart;
		iEnd=range.GetMaxCol ()		-2+m_iRecordStart;
	}
	else
	{
		iRecord=GetCurrentRowPos()+m_iRecordStart;
		
		iStart=range.GetMinRow ()	-2+m_iRecordStart;
		iEnd=range.GetMaxRow ()		-2+m_iRecordStart;
	}

	for(int i=iStart;i<=iEnd;i++)
	{
		pDoc->ResetDeletedFlag (i);	
	}

	UpdateFlag();
	m_Grid.AutoSize ();
}

void CDataBrowseView::OnUpdatedatafromdb()
{
	CDataBrowseDoc *pDoc=(CDataBrowseDoc *)GetDocument();
	CStringArray arrayCurrentFields;
	pDoc->GetFields (arrayCurrentFields);

	CTableDate dateStart;
	CQCDBApp *pApp=(CQCDBApp *)AfxGetApp();
	pApp->m_dt.GetData (pDoc->GetSourceIP(),
						pDoc->GetSourcePort(),
						pDoc->GetTableName (),
						pDoc->GetSQL (),
						dateStart,
						&arrayCurrentFields,
						pDoc,
						GetMaxHandDownRecord(),
						false);

	UpdateGrid();

}

void CDataBrowseView::GridModified(int iRow, int iCol)
{
	CDataBrowseDoc *pDoc=(CDataBrowseDoc *)GetDocument();
	int iRecord	=0;
	int iField	=0;
	if(m_bVerticalShow)
	{
		iRecord	=	m_iRecordStart+iCol-m_Grid.GetFixedColumnCount ();
		iField	=	iRow - 2;
	}
	else
	{
		iRecord	=	m_iRecordStart+iRow-m_Grid.GetFixedRowCount ();
		iField	=	iCol - 2;
	}
	CString sItem;
	GetItem(iRow,iCol,sItem);
	pDoc->SetUpdatedFlag (iRecord);
	pDoc->UpdateField (iRecord,iField,sItem);
	UpdateFlag();
	m_Grid.AutoSize ();
}

void CDataBrowseView::UpdateFlag()
{
	static char sStatus[4],cStatus;
	static int i=0,iRecord=0;

	CDataBrowseDoc *pDoc=(CDataBrowseDoc *)GetDocument();
	m_iTotalRecord=pDoc->GetRecordQty ();

	for(i=0;i<m_iRecordPerPage;i++)
	{
		iRecord=m_iRecordStart+i;
		if(iRecord>=m_iTotalRecord)break;

		memset(sStatus,' ',4);
		sStatus[3]=0;

		cStatus=pDoc->GetRecordStatus(iRecord);
		if(cStatus&RS_DELETED)
		{
			sStatus[0]='X';
		}

		if(cStatus&RS_ADDED)
		{
			sStatus[1]='+';
		}

		if(cStatus&RS_MODIFIED)
		{
			sStatus[2]='*';
		}

		if(m_bVerticalShow)
		{
			SetItem(1,i+2,sStatus);
		}
		else
		{
			SetItem(i+2,1,sStatus);
		}
	}

	if(pDoc->RecordModified ())
	{
		pDoc->SetModifiedFlag (true);
	}
	else
	{
		pDoc->SetModifiedFlag (false);
	}
}

void CDataBrowseView::OnUpdatedb()
{
	CDataBrowseDoc *pDoc=(CDataBrowseDoc *)GetDocument();
	pDoc->UpdateDB();
	pDoc->UpdateFromDB ();
}

void CDataBrowseView::OnUpdateUpdatedb(CCmdUI *pCmdUI)
{
	// TODO: Add your command update UI handler code here
}

void CDataBrowseView::OnUpdateSenddatatoserverall(CCmdUI *pCmdUI)
{
	// TODO: Add your command update UI handler code here
}

void CDataBrowseView::OnUpdateSenddatatoservernohanded(CCmdUI *pCmdUI)
{
	// TODO: Add your command update UI handler code here
}

void CDataBrowseView::OnLoadpic()
{
	if(     m_iCurrRow  <m_Grid.GetFixedRowCount()
        ||  m_iCurrCol  <m_Grid.GetFixedColumnCount())
	{
		TRACEERROR("当前鼠标所指的位置错误，不能加载图像，请选择有效网格!");
		return;
	}

	CString sPicFile=GetFileForOpen("选择欲载入数据库的图片(*.jpg)","jpg");
	if(sPicFile=="")return;

	CDataBrowseDoc *pDoc=(CDataBrowseDoc *)GetDocument();

	CString sDBPicName=GetUserNameShort()+"_"+SeperateFileName(sPicFile);

	CString sDBPicPathName=GetPicFullPath(sDBPicName);
		

	CFHImage image;
	HRESULT hr=image.Load (sPicFile);
	if(hr==E_FAIL)
	{
		AfxMessageBox("无法导入图片:"+sPicFile);
		return;
	}

	CString sPrePic;
	int iRow=m_iCurrRow;
	int iCol=m_iCurrCol;
	GetItem(iRow ,iCol,sPrePic);

	if(IsPicFile(sPrePic))
	{
        sPrePic=GetPicFullPath(sPrePic);
        RemoveFile(sPrePic);
	}

	bool bResult=image.Save (	sDBPicPathName,
								pDoc->m_iDBPicWidth,
								pDoc->m_iDBPicHeight,
								pDoc->m_bDBPicKeepRatio);
	if(bResult==true)
	{	
		SetItem(iRow ,iCol,sDBPicName);
        if(!m_bVerticalShow)
        {
		    pDoc->UpdateField(iRow-2,iCol-2,sDBPicName);
        }
        else
        {
            pDoc->UpdateField(iCol-2,iRow-2,sDBPicName);  
        }

		ScanPics();
		Invalidate();
		GridModified(iRow,iCol);
	}
	else
	{
		AfxMessageBox("无法存储文件:"+sDBPicPathName);
	}
	return;
}

LRESULT CDataBrowseView::OnPicPropertyChanged(WPARAM wParam, LPARAM lParam)
{

	CDataBrowseDoc *pDoc=(CDataBrowseDoc *)GetDocument();
	
	m_Grid.SetCellPicProperty(pDoc->m_iPicWidth ,
								pDoc->m_iPicHeight ,
								pDoc->m_bPicKeepRatio ,
								pDoc->m_bDrawCellPic );

	ScanPics();

	Invalidate();
	pDoc->UpdateAllViews (this);

	return 0;
}

void CDataBrowseView::ScanPics()
{
	int i,j;

	CDataBrowseDoc *pDoc=(CDataBrowseDoc *)GetDocument();
	int iFieldQty=pDoc->GetFieldQty ();
	

	int *pPicField=new int [ iFieldQty+2];
	CFieldData *pFields=pDoc->GetFields ();
	int iPicFieldQty=0;
	for(i=0;i<iFieldQty;i++)
	{
		if(IsPicField(pFields[i].sFieldName ))
		{
			pPicField[iPicFieldQty]=i;
			iPicFieldQty++;
		}
	}

	int iFixedRow=m_Grid.GetFixedRowCount ();
	CString sPic="";
	int iRow=0,iCol=0;

	CStringArray *pRecord=NULL;
	m_Grid.RemoveAllPics ();
	for(i=m_iRecordStart ;i<m_iRecordPerPage ;i++)
	{
		if(i>=pDoc->GetRecordQty ())break;

		pRecord=pDoc->GetRecord (i);

		for(j=0;j<iPicFieldQty;j++)
		{
			sPic=pRecord->GetAt (pPicField[j]);
			if(!IsPicFile(sPic))continue;

			if(m_bVerticalShow)
			{
				iRow=2+pPicField[j];
				iCol=2+i;
			}
			else
			{
				iCol=2+pPicField[j];
				iRow=2+i;
			}

			if(pDoc->m_bDrawCellPic)
			{
				sPic=GetPicFullPath(sPic);
				m_Grid.DrawPic(sPic,iRow,iCol);
			}

			if(pDoc->m_sBigPicName=="")
			{
				pDoc->m_sBigPicName=sPic;
			}
		}
	}

	delete []pPicField;
}

void CDataBrowseView::OnLButtonUp(UINT nFlags, CPoint point)
{
	CFHEditView::OnLButtonUp(nFlags, point);

	CCellID id=m_Grid.GetCellFromPt (point);

	CString sData;
	GetItem(id.row ,id.col ,sData);
	sData.Trim ();

	CDataBrowseDoc *pDoc=(CDataBrowseDoc *)GetDocument();

    
	if(IsPicFile(sData))
	{
        pDoc->m_sBigPicName=GetPicFullPath(sData);
/*
		m_Grid.SetBigPicProperty (m_sBigPicName,
					m_iBigPicOffsetX,
					m_iBigPicOffsetY,
					m_iBigPicWidth,
					m_iBigPicHeight,
					m_bBigPicKeepRatio,
					m_bDrawBigPic);
*/
		pDoc->UpdateAllViews (this);
	}

}

void CDataBrowseView::OnShowhor()
{
	m_bVerticalShow=!m_bVerticalShow;

	AddPar("DATABROWSE_VIEW_VerticalShow",m_bVerticalShow);

	UpdateGrid();
}

void CDataBrowseView::OnUpdateShowhor(CCmdUI *pCmdUI)
{
	pCmdUI->SetCheck (!m_bVerticalShow);
}

bool CDataBrowseView::IsVerticalView()
{
    return m_bVerticalShow;
}
