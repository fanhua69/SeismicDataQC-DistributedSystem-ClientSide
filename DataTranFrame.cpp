// DataTranFrame.cpp : implementation file
//

#include "stdafx.h"
#include "QCDB.h"
#include "DataTranFrame.h"
#include "DataTransView.h"

// CDataTranFrame

IMPLEMENT_DYNCREATE(CDataTranFrame, CMDIChildWnd)

CDataTranFrame::CDataTranFrame()
{
}

CDataTranFrame::~CDataTranFrame()
{
}

BOOL CDataTranFrame::OnCreateClient(LPCREATESTRUCT /*lpcs*/, CCreateContext* pContext)
{
	/////////////////////////////////////////////////
	// Create the splitter bars:
	if (!m_wndSplitter.CreateStatic(this, 2, 1))return FALSE;
    	
	/////////////////////////////////////////////
	// 连接者(服务器，客户端)浏览视
	if (!m_wndSplitter.CreateView(0,0,pContext->m_pNewViewClass, CSize(100, 100), pContext))
		return FALSE;

	/////////////////////////////////////////////
	// 传送数据浏览视
	if (!m_wndSplitter.CreateView(1, 0,
		RUNTIME_CLASS(CDataTransView), CSize(130,130), pContext))
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
	
	//return CMDIChildWnd::OnCreateClient(lpcs, pContext);




}

BEGIN_MESSAGE_MAP(CDataTranFrame, CMDIChildWnd)
END_MESSAGE_MAP()


// CDataTranFrame message handlers
