
// Lab3View.h : interface of the CLab3View class
//

#pragma once
#include "DImage.h"
#include <math.h>


class CLab3View : public CView
{
protected: // create from serialization only
	CLab3View() noexcept;
	DECLARE_DYNCREATE(CLab3View)

// Attributes
public:
	CLab3Doc* GetDocument() const;
	DImage images[9];
	int clientWidth;
	int clientHeight;
	int cellWidth;

// Operations
public:

// Overrides
public:
	virtual void OnDraw(CDC* pDC);  // overridden to draw this view
	void DrawBackground(CDC* pDC, int clientWidth, int clientHeight);
	void DrawGrid(CDC* pDC, int clientWidth, int clientHeight, int numOfLines);
	void DrawCoordinateSystem(CDC* pDC);
	void Translate(CDC* pDC, float dX, float dY, bool rightMultiply);
	void Scale(CDC* pDC, float sX, float sY, bool rightMultiply);
	void Rotate(CDC* pDC, float angle, bool rightMultiply);
	void Mirror(CDC* pDC, bool mx, bool my, bool rightMultiply);
	void DrawPuzzlePieces(CDC* pDC);
	void MakeTransparent(CDC* pDC, DImage* bmpImage, int index);
	void FilterBitmap(CBitmap* bmpImage, int index, int size);

	virtual BOOL PreCreateWindow(CREATESTRUCT& cs);
protected:
	virtual BOOL OnPreparePrinting(CPrintInfo* pInfo);
	virtual void OnBeginPrinting(CDC* pDC, CPrintInfo* pInfo);
	virtual void OnEndPrinting(CDC* pDC, CPrintInfo* pInfo);

// Implementation
public:
	virtual ~CLab3View();
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
};

#ifndef _DEBUG  // debug version in Lab3View.cpp
inline CLab3Doc* CLab3View::GetDocument() const
   { return reinterpret_cast<CLab3Doc*>(m_pDocument); }
#endif

