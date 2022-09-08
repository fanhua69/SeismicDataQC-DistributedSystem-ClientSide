#pragma once


#include "FHEditView.h"
#include "StringExtractor.h"
class CFieldData;

class CDataBrowseView : public CFHEditView
{
	DECLARE_DYNCREATE(CDataBrowseView)

protected:
	CDataBrowseView();           // protected constructor used by dynamic creation
	virtual ~CDataBrowseView();

	virtual void GridModified(int iRow, int iCol);

	void UpdateFlag();


public:
	virtual void OnDraw(CDC* pDC);      // overridden to draw this view
#ifdef _DEBUG
	virtual void AssertValid() const;
	virtual void Dump(CDumpContext& dc) const;
#endif
	
	int m_iRecordStart;
	int m_iRecordPerPage;
	int m_iTotalRecord;

	bool m_bVerticalShow;

    virtual CString GetDataTypeName(CFieldData *pField);

	void UpdateGrid();
	void UpdateGridHor();
	void UpdateGridVer();

	int GetCurrentPage();
	int GetTotalPage();

	void ScanPics();

    bool IsVerticalView();


    virtual void SetComboxContents( CString         sTableName,
                                    CFieldData      *pField,
                                    int             iFieldQty,
                                    int             iFixedRowOrColQty, 
                                    bool            bSetRowToCol);


protected:
	DECLARE_MESSAGE_MAP()
public:
	afx_msg void OnNextpage();
	afx_msg void OnPrevpage();
	afx_msg void OnUpdateNextpage(CCmdUI *pCmdUI);
	afx_msg void OnUpdatePrevpage(CCmdUI *pCmdUI);
protected:
	virtual void OnUpdate(CView* /*pSender*/, LPARAM /*lHint*/, CObject* /*pHint*/);
public:
	afx_msg void OnChoosepage();
	afx_msg void OnDeleterecord();
	afx_msg void OnAddrecord();
    afx_msg void OnCloneLastRecord();
	afx_msg void OnRecoverrecord();
	afx_msg void OnUpdatedatafromdb();
	afx_msg void OnUpdatedb();
	afx_msg void OnUpdateUpdatedb(CCmdUI *pCmdUI);
	afx_msg void OnUpdateSenddatatoserverall(CCmdUI *pCmdUI);
	afx_msg void OnUpdateSenddatatoservernohanded(CCmdUI *pCmdUI);
	afx_msg void OnLoadpic();

	afx_msg LRESULT OnPicPropertyChanged(WPARAM wParam, LPARAM lParam);

	afx_msg void OnLButtonUp(UINT nFlags, CPoint point);
	afx_msg void OnShowhor();
	afx_msg void OnUpdateShowhor(CCmdUI *pCmdUI);
};


