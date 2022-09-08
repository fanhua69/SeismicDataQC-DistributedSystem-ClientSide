#pragma once


// CDataTransView view
#include "FHEditView.h"

class CDataTransView : public CFHEditView
{
	DECLARE_DYNCREATE(CDataTransView)

protected:
	CDataTransView();           // protected constructor used by dynamic creation
	virtual ~CDataTransView();
	void UpdateGrid();
	CString m_sUserShow;

public:
	virtual void OnDraw(CDC* pDC);      // overridden to draw this view
#ifdef _DEBUG
	virtual void AssertValid() const;
	virtual void Dump(CDumpContext& dc) const;
#endif

protected:
	DECLARE_MESSAGE_MAP()
};


