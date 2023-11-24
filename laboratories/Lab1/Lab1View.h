
#pragma once
#include <math.h>

class CLab1View : public CView
{
protected: 
	CLab1View() noexcept;
	DECLARE_DYNCREATE(CLab1View)

// Attributes
public:
	CLab1Doc* GetDocument() const;
	bool isGridVisible = true;
	COLORREF penColor = RGB(252, 109, 248);

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
	virtual ~CLab1View();
#ifdef _DEBUG
	virtual void AssertValid() const;
	virtual void Dump(CDumpContext& dc) const;

	// mine

	void DrawGrid(CDC* pDC, int clientWidth, int clientHeight, int numOfLines);
	void DrawBackground(CDC* pDC, int clientWidth, int clientHeight);
	void DrawRegularPolygon(CDC* pDC, int cx, int cy, int r, int n, float rotAngle, bool fill, COLORREF color);
	void DrawPolygon(CDC* pDC, LPPOINT points, int n, bool pattern, COLORREF color);
	void FindCenterOfTriangle(LPPOINT points, int n, int& cx, int& cy);
#endif

protected:

// Generated message map functions
protected:
	DECLARE_MESSAGE_MAP()
public:
	afx_msg void OnKeyDown(UINT nChar, UINT nRepCnt, UINT nFlags);
};

#ifndef _DEBUG  // debug version in Lab1View.cpp
inline CLab1Doc* CLab1View::GetDocument() const
   { return reinterpret_cast<CLab1Doc*>(m_pDocument); }
#endif

