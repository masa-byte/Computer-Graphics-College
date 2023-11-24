
// SurikenView.h : interface of the CSurikenView class
//

#pragma once
#include "DImage.h"


class CSurikenView : public CView
{
protected: // create from serialization only
	CSurikenView() noexcept;
	DECLARE_DYNCREATE(CSurikenView)

// Attributes
public:
	CSurikenDoc* GetDocument() const;
	DImage sky, bark;
	float deg, translate;
	float cW, cH;

// Operations
public:

// Overrides
public:
	virtual void OnDraw(CDC* pDC);  // overridden to draw this view
	virtual BOOL PreCreateWindow(CREATESTRUCT& cs);
	// mine
	void DrawBlade(CDC* pDC, int size);
	void Translate(CDC* pDC, double x, double y);
	void Rotate(CDC* pDC, float angle);
	void DrawStar(CDC* pDC, int size);
	void Coordinates(CDC* pDC);
	void DrawTexxxt(CDC* pDC, int size, COLORREF color);

protected:
	virtual BOOL OnPreparePrinting(CPrintInfo* pInfo);
	virtual void OnBeginPrinting(CDC* pDC, CPrintInfo* pInfo);
	virtual void OnEndPrinting(CDC* pDC, CPrintInfo* pInfo);

// Implementation
public:
	virtual ~CSurikenView();
#ifdef _DEBUG
	virtual void AssertValid() const;
	virtual void Dump(CDumpContext& dc) const;
#endif

protected:

// Generated message map functions
protected:
	DECLARE_MESSAGE_MAP()
public:
	afx_msg BOOL OnEraseBkgnd(CDC* pDC);
	afx_msg void OnKeyDown(UINT nChar, UINT nRepCnt, UINT nFlags);
};

#ifndef _DEBUG  // debug version in SurikenView.cpp
inline CSurikenDoc* CSurikenView::GetDocument() const
   { return reinterpret_cast<CSurikenDoc*>(m_pDocument); }
#endif

