#pragma once



// CDataChartView view
#include "DataBrowseDoc.h"
#include "GraphView.h"

class CDataChartView : public CGraphView
{
	DECLARE_DYNCREATE(CDataChartView)

protected:
	CDataChartView();           // protected constructor used by dynamic creation
	virtual ~CDataChartView();

	void SetChart();
	CDataBrowseDoc * GetDocument();

	int m_iChartType;
public:
#ifdef _DEBUG
	virtual void AssertValid() const;
	virtual void Dump(CDumpContext& dc) const;
#endif

protected:
	virtual void OnDraw(CDC* pDC);      // overridden to draw this view
	virtual void OnInitialUpdate();     // first time after construct
	void SetLegend();


	int GetCurrentPage();
	int GetTotalPage();

	DECLARE_MESSAGE_MAP()
public:
	int m_iRecordPerPage;
	int m_iRecordStart;
	int m_iTotalRecord;

	afx_msg int OnCreate(LPCREATESTRUCT lpCreateStruct);
	afx_msg void OnDbshownextpage();
	afx_msg void OnDbshowprevpage();
	afx_msg void OnUpdateDbshownextpage(CCmdUI *pCmdUI);
	afx_msg void OnUpdateDbshowprevpage(CCmdUI *pCmdUI);

	afx_msg void OnSetchartproperty();

	void SavePars();
	void GetPars();



protected:
	virtual void OnUpdate(CView* /*pSender*/, LPARAM /*lHint*/, CObject* /*pHint*/);
public:
	afx_msg void OnUpdatedatafromdb();
	afx_msg void OnChooseshowdata();
	afx_msg void OnChoosepage();
};


