
// Lab4View.h : interface of the CLab4View class
//

#pragma once
#include "CGLRenderer.h"


class CLab4View : public CView
{
protected: // create from serialization only
	CLab4View() noexcept;
	DECLARE_DYNCREATE(CLab4View)

// Attributes
public:
	CLab4Doc* GetDocument() const;
	CGLRenderer m_glRenderer;

// Operations
public:

// Overrides
public:
	virtual void OnDraw(CDC* pDC);  // overridden to draw this view
	virtual BOOL PreCreateWindow(CREATESTRUCT& cs);
protected:
	virtual BOOL OnPreparePrinting(CPrintInfo* pInfo);
	virtual void OnBeginPrinting(CDC* pDC, CPrintInfo* pInfo);
	virtual void OnEndPrinting(CDC* pDC, CPrintInfo* pInfo);

// Implementation
public:
	virtual ~CLab4View();
#ifdef _DEBUG
	virtual void AssertValid() const;
	virtual void Dump(CDumpContext& dc) const;
#endif

protected:

// Generated message map functions
protected:
	DECLARE_MESSAGE_MAP()
public:
	afx_msg int OnCreate(LPCREATESTRUCT lpCreateStruct);
	afx_msg void OnSize(UINT nType, int cx, int cy);
	afx_msg void OnDestroy();
	virtual void OnInitialUpdate();
	afx_msg BOOL OnEraseBkgnd(CDC* pDC);
	afx_msg void OnKeyDown(UINT nChar, UINT nRepCnt, UINT nFlags);
};

#ifndef _DEBUG  // debug version in Lab4View.cpp
inline CLab4Doc* CLab4View::GetDocument() const
   { return reinterpret_cast<CLab4Doc*>(m_pDocument); }
#endif

