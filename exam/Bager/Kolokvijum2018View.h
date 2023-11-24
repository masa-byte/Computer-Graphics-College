
// Kolokvijum2018View.h : interface of the CKolokvijum2018View class
//

#pragma once
#include "DImage.h"


class CKolokvijum2018View : public CView
{
protected: // create from serialization only
	CKolokvijum2018View() noexcept;
	DECLARE_DYNCREATE(CKolokvijum2018View)

// Attributes
public:
	CKolokvijum2018Doc* GetDocument() const;
	DImage bager, arm1, arm2, pozadina;
	HENHMETAFILE viljuska;
	float viljuskaH, viljuskaW;
	float degArm1, degArm2, degFork;
	float right;

// Operations
public:

// Overrides
public:
	virtual void OnDraw(CDC* pDC);  // overridden to draw this view
	virtual BOOL PreCreateWindow(CREATESTRUCT& cs);
	void Translate(CDC* pDC, double dX, double dY, bool rightMultiply);
	void Rotate(CDC* pDC, float angle, bool rightMultiply);
	void Scale(CDC* pDC, float sX, float sY, bool rightMultiply);
	void DrawBackground(CDC* pDC);
	void Coordinates(CDC* pDC);
	void DrawImgTransparent(CDC* pDC, DImage* pImage, float offsetX, float offsetY);
	void DrawBody(CDC* pDC);
	void DrawArm1(CDC* pDC);
	void DrawArm2(CDC* pDC);
	void DrawFork(CDC* pDC);
	void DrawExcavator(CDC* pDC);

protected:
	virtual BOOL OnPreparePrinting(CPrintInfo* pInfo);
	virtual void OnBeginPrinting(CDC* pDC, CPrintInfo* pInfo);
	virtual void OnEndPrinting(CDC* pDC, CPrintInfo* pInfo);

// Implementation
public:
	virtual ~CKolokvijum2018View();
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
	afx_msg BOOL OnEraseBkgnd(CDC* pDC);
};

#ifndef _DEBUG  // debug version in Kolokvijum2018View.cpp
inline CKolokvijum2018Doc* CKolokvijum2018View::GetDocument() const
   { return reinterpret_cast<CKolokvijum2018Doc*>(m_pDocument); }
#endif

