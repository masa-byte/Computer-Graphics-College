
// BilijarView.h : interface of the CBilijarView class
//

#pragma once
#include "DImage.h"


class CBilijarView : public CView
{
protected: // create from serialization only
	CBilijarView() noexcept;
	DECLARE_DYNCREATE(CBilijarView)

// Attributes
public:
	CBilijarDoc* GetDocument() const;
	DImage wood, felt;
	float cw, ch;
// Operations
public:

// Overrides
public:
	virtual void OnDraw(CDC* pDC);  // overridden to draw this view
	virtual BOOL PreCreateWindow(CREATESTRUCT& cs);
	void DrawStick(CDC* pDC, int w);
	void DrawBall(CDC* pDC, int w);
	void Translate(CDC* pDC, float dX, float dY, bool rightMultiply);
	void Rotate(CDC* pDC, float angle, bool rightMultiply);
	void DrawTable(CDC* pDC, CRect rect);
	void DrawBorder(CDC* pDC, CRect rect, int w);
	void DrawHoles(CDC* pDC, CRect rect, int size);
	void DrawPool(CDC* pDC);

protected:
	virtual BOOL OnPreparePrinting(CPrintInfo* pInfo);
	virtual void OnBeginPrinting(CDC* pDC, CPrintInfo* pInfo);
	virtual void OnEndPrinting(CDC* pDC, CPrintInfo* pInfo);

// Implementation
public:
	virtual ~CBilijarView();
#ifdef _DEBUG
	virtual void AssertValid() const;
	virtual void Dump(CDumpContext& dc) const;
#endif

protected:

// Generated message map functions
protected:
	DECLARE_MESSAGE_MAP()
};

#ifndef _DEBUG  // debug version in BilijarView.cpp
inline CBilijarDoc* CBilijarView::GetDocument() const
   { return reinterpret_cast<CBilijarDoc*>(m_pDocument); }
#endif

