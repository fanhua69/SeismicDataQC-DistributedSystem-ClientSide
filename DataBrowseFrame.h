#pragma once


// CDataBrowseFrame frame with splitter

class CDataBrowseFrame : public CMDIChildWnd
{
	DECLARE_DYNCREATE(CDataBrowseFrame)
protected:
	CDataBrowseFrame();           // protected constructor used by dynamic creation
	virtual ~CDataBrowseFrame();

	CSplitterWnd m_wndSplitter;

protected:
	virtual BOOL OnCreateClient(LPCREATESTRUCT lpcs, CCreateContext* pContext);

	DECLARE_MESSAGE_MAP()
};


