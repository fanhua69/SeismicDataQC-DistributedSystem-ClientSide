// DataBrowseFrame.cpp : implementation file
//

#include "stdafx.h"
#include "QCDB.h"
#include "PicShowView.h"
#include "DataBrowseFrame.h"

// CDataBrowseFrame

IMPLEMENT_DYNCREATE(CDataBrowseFrame, CMDIChildWnd)

CDataBrowseFrame::CDataBrowseFrame()
{
}

CDataBrowseFrame::~CDataBrowseFrame()
{
}

BOOL CDataBrowseFrame::OnCreateClient(LPCREATESTRUCT /*lpcs*/, CCreateContext* pContext)
{
	/////////////////////////////////////////////////
	// Create the splitter bars:
	if (!m_wndSplitter.CreateStatic(this,1, 2))return FALSE;
    	
	/////////////////////////////////////////////
	// 数据库浏览视
	if (!m_wndSplitter.CreateView(0,0,pContext->m_pNewViewClass, CSize(400, 200), pContext))
		return FALSE;

	/////////////////////////////////////////////
	// 数据库行编辑视
	if (!m_wndSplitter.CreateView(0, 1,
		RUNTIME_CLASS(CPicShowView), CSize(130,130), pContext))
	{
		TRACE0("Failed to create second pane\n");
		return FALSE;
	}

	///////////////////////////////////////////////////
	// activate the input view
	SetActiveView((CView*)m_wndSplitter.GetPane(0,0));
	//m_wndSplitter.SetRowInfo( 0, 50, 50);
	//m_wndSplitter.SetRowInfo( 0, 50, 50);

	return true;





}

BEGIN_MESSAGE_MAP(CDataBrowseFrame, CMDIChildWnd)
END_MESSAGE_MAP()


// CDataBrowseFrame message handlers
