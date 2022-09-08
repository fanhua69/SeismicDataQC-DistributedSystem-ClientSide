#pragma once


// CDataTranFrame frame with splitter

class CDataTranFrame : public CMDIChildWnd
{
	DECLARE_DYNCREATE(CDataTranFrame)
protected:
	CDataTranFrame();           // protected constructor used by dynamic creation
	virtual ~CDataTranFrame();

	CSplitterWnd m_wndSplitter;

protected:
	virtual BOOL OnCreateClient(LPCREATESTRUCT lpcs, CCreateContext* pContext);

	DECLARE_MESSAGE_MAP()
};


