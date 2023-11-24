
// Kolokvijum2019View.h : interface of the CKolokvijum2019View class
//

#pragma once
#include "DImage.h"


class CKolokvijum2019View : public CView
{
protected: // create from serialization only
	CKolokvijum2019View() noexcept;
	DECLARE_DYNCREATE(CKolokvijum2019View)

// Attributes
public:
	CKolokvijum2019Doc* GetDocument() const;
	DImage* body, * arm1, * arm2, * leg1, * leg2, * background;
	float degArm1, degArm2, degLeg1, degLeg2, degBody;
	float right;
	CRect clientRect;

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
	void DrawBackground(CDC* pDC, CRect rc);
	void DrawImgTransparent(CDC* pDC, DImage* pImage);
	void Translate(CDC* pDC, float dX, float dY, bool rightMultiply);
	void Rotate(CDC* pDC, float angle, bool rightMultiply);
	void DrawArm1(CDC* pDC);
	void DrawArm2(CDC* pDC);
	void DrawLeg1(CDC* pDC);
	void DrawLeg2(CDC* pDC);
	void DrawBody(CDC* pDC);
	void DrawTransformer(CDC* pDC);
	void Coordinates(CDC* pDC);

// Implementation
public:
	virtual ~CKolokvijum2019View();
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

#ifndef _DEBUG  // debug version in Kolokvijum2019View.cpp
inline CKolokvijum2019Doc* CKolokvijum2019View::GetDocument() const
   { return reinterpret_cast<CKolokvijum2019Doc*>(m_pDocument); }
#endif

