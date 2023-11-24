
// Lab2View.h : interface of the CLab2View class
//

#pragma once
#include <math.h>

class CLab2View : public CView
{
protected: // create from serialization only
	CLab2View() noexcept;
	DECLARE_DYNCREATE(CLab2View)

// Attributes
public:
	CLab2Doc* GetDocument() const;
	bool isGridVisible = true;
	int clientWidth = 500;
	int clientHeight = 500;
	double cellWidth = 0;
	double degreeSmall = 0;
	double degreeBig = 0;

// Operations
public:

// Overrides
public:
	virtual void OnDraw(CDC* pDC);  // overridden to draw this view

	// mine
	void DrawBackground(CDC* pDC, int clientWidth, int clientHeight);
	void DrawGrid(CDC* pDC, int clientWidth, int clientHeight, int numOfLines);
	void DrawPolygon(CDC* pDC, LPPOINT points, int n, COLORREF brushColor, COLORREF penColor);
	void DrawCoordinateSystem(CDC* pDC);
	void WriteText(CDC* pDC, double offset, COLORREF color);
	void DrawPot(CDC* pDC);
	void DrawElipse(CDC* pDC, int x1, int y1, int x2, int y2);
	void Translate(CDC* pDC, float dX, float dY, bool rightMultiply);
	void Scale(CDC* pDC, float sX, float sY, bool rightMultiply);
	void Rotate(CDC* pDC, float angle, bool rightMultiply);
	void DrawFigure(CDC* pDC);

	virtual BOOL PreCreateWindow(CREATESTRUCT& cs);
protected:
	virtual BOOL OnPreparePrinting(CPrintInfo* pInfo);
	virtual void OnBeginPrinting(CDC* pDC, CPrintInfo* pInfo);
	virtual void OnEndPrinting(CDC* pDC, CPrintInfo* pInfo);

// Implementation
public:
	virtual ~CLab2View();
#ifdef _DEBUG
	virtual void AssertValid() const;
	virtual void Dump(CDumpContext& dc) const;
#endif

protected:

// Generated message map functions
protected:
	DECLARE_MESSAGE_MAP()
public:
	afx_msg void OnKeyDown(UINT nChar, UINT nRepCnt, UINT nFlags);
};

#ifndef _DEBUG  // debug version in Lab2View.cpp
inline CLab2Doc* CLab2View::GetDocument() const
   { return reinterpret_cast<CLab2Doc*>(m_pDocument); }
#endif

