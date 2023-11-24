
#include "pch.h"
#include <math.h>
#include "framework.h"
#include <afxwin.h>
#define _USE_MATH_DEFINES
#ifndef SHARED_HANDLERS
#include "Lab1.h"
#endif

#include "Lab1Doc.h"
#include "Lab1View.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#endif


IMPLEMENT_DYNCREATE(CLab1View, CView)

BEGIN_MESSAGE_MAP(CLab1View, CView)
	ON_COMMAND(ID_FILE_PRINT, &CView::OnFilePrint)
	ON_COMMAND(ID_FILE_PRINT_DIRECT, &CView::OnFilePrint)
	ON_COMMAND(ID_FILE_PRINT_PREVIEW, &CView::OnFilePrintPreview)
	ON_WM_KEYDOWN()
END_MESSAGE_MAP()


CLab1View::CLab1View() noexcept
{
}

CLab1View::~CLab1View()
{
}

BOOL CLab1View::PreCreateWindow(CREATESTRUCT& cs)
{
	// TODO: Modify the Window class or styles here by modifying
	//  the CREATESTRUCT cs
	return CView::PreCreateWindow(cs);
}


void CLab1View::OnDraw(CDC* pDC)
{
	CLab1Doc* pDoc = GetDocument();
	ASSERT_VALID(pDoc);
	if (!pDoc)
		return;

	int clientWidth = 500;
	int clientHeight = 500;
	int numOfLines = 19;

	int cellWidth = clientWidth / (numOfLines + 1);

	this->DrawBackground(pDC, clientWidth, clientHeight);
	//this->DrawRegularPolygon(pDC, 250, 250, 200, 4, M_PI / 4, true);
	int cx, cy;
	this->DrawPolygon(pDC, new POINT[3]
		{ 
			{1 * cellWidth, 1 * cellWidth}, 
			{5 * cellWidth, 5 * cellWidth},
			{1 * cellWidth, 10 * cellWidth} 
		},
		3, true, NULL);

	this->FindCenterOfTriangle(new POINT[3]
		{
			{ 1 * cellWidth, 1 * cellWidth },
			{ 5 * cellWidth, 5 * cellWidth },
			{ 1 * cellWidth, 10 * cellWidth }
		}, 3, cx, cy);
	this->DrawRegularPolygon(pDC, cx, cy, 20, 7, 0, false, NULL);

	this->DrawPolygon(pDC, new POINT[3]
		{
			{ 1 * cellWidth, 1 * cellWidth },
			{ 10 * cellWidth, 10 * cellWidth },
			{ 19 * cellWidth, 1 * cellWidth }
		},
		3, false, RGB(255, 248, 56));

	this->FindCenterOfTriangle(new POINT[3]
		{
			{ 1 * cellWidth, 1 * cellWidth },
			{ 10 * cellWidth, 10 * cellWidth },
			{ 19 * cellWidth, 1 * cellWidth }
		}, 3, cx, cy);
	this->DrawRegularPolygon(pDC, cx, cy, 50, 5, 0, false, NULL);

	this->DrawPolygon(pDC, new POINT[3]
		{
			{ 19 * cellWidth, 1 * cellWidth },
			{ 10 * cellWidth, 10 * cellWidth },
			{ 19 * cellWidth, 19 * cellWidth }
		},
		3, false, RGB(245, 21, 5));

	this->FindCenterOfTriangle(new POINT[3]
		{
			{ 19 * cellWidth, 1 * cellWidth },
			{ 10 * cellWidth, 10 * cellWidth },
			{ 19 * cellWidth, 19 * cellWidth }
		}, 3, cx, cy);
	this->DrawRegularPolygon(pDC, cx, cy, 50, 4, 0, false, NULL);

	this->DrawPolygon(pDC, new POINT[3]
		{
			{ 10 * cellWidth, 10 * cellWidth },
			{ 5 * cellWidth, 15 * cellWidth },
			{ 15 * cellWidth, 15 * cellWidth }
		},
		3, false, RGB(147, 34, 181));

	this->FindCenterOfTriangle(new POINT[3]
		{
			{ 10 * cellWidth, 10 * cellWidth },
			{ 5 * cellWidth, 15 * cellWidth },
			{ 15 * cellWidth, 15 * cellWidth }
		}, 3, cx, cy);
	this->DrawRegularPolygon(pDC, cx, cy, 20, 8, 0, false, NULL);

	this->DrawPolygon(pDC, new POINT[3]
		{
			{ 1 * cellWidth, 10 * cellWidth },
			{ 1 * cellWidth, 19 * cellWidth },
			{ 10 * cellWidth, 19 * cellWidth }
		},
		3, false, RGB(242, 191, 227));

	this->FindCenterOfTriangle(new POINT[3]
		{
			{ 1 * cellWidth, 10 * cellWidth },
			{ 1 * cellWidth, 19 * cellWidth },
			{ 10 * cellWidth, 19 * cellWidth }
		}, 3, cx, cy);
	this->DrawRegularPolygon(pDC, cx, cy, 40, 6, 0, false, NULL);

	this->DrawPolygon(pDC, new POINT[4]
		{
			{ 1 * cellWidth, 10 * cellWidth },
			{ 5 * cellWidth, 5 * cellWidth },
			{ 10 * cellWidth, 10 * cellWidth },
			{ 6 * cellWidth, 15 * cellWidth }
		},
		4, false, RGB(240, 135, 60));
	this->DrawPolygon(pDC, new POINT[4]
		{
			{ 6 * cellWidth, 15 * cellWidth },
			{ 15 * cellWidth, 15 * cellWidth },
			{ 19 * cellWidth, 19 * cellWidth },
			{ 10 * cellWidth, 19 * cellWidth }
		},
		4, false, RGB(50, 232, 44));

	this->DrawGrid(pDC, clientWidth, clientHeight, numOfLines);
}

#pragma region PRINTING

// CLab1View printing

BOOL CLab1View::OnPreparePrinting(CPrintInfo* pInfo)
{
	// default preparation
	return DoPreparePrinting(pInfo);
}

void CLab1View::OnBeginPrinting(CDC* /*pDC*/, CPrintInfo* /*pInfo*/)
{
	// TODO: add extra initialization before printing
}

void CLab1View::OnEndPrinting(CDC* /*pDC*/, CPrintInfo* /*pInfo*/)
{
	// TODO: add cleanup after printing
}

#pragma endregion



#ifdef _DEBUG
void CLab1View::AssertValid() const
{
	CView::AssertValid();
}

void CLab1View::Dump(CDumpContext& dc) const
{
	CView::Dump(dc);
}

void CLab1View::DrawBackground(CDC* pDC, int clientWidth, int clientHeight)
{
	CPen pen(PS_NULL, 1, RGB(255, 255, 255));
	CPen* oldPen = pDC->SelectObject(&pen);
	CBrush brush(RGB(222, 222, 222));
	CBrush* pOldBrush = pDC->SelectObject(&brush);
	pDC->SetBrushOrg(0, 0);
	pDC->Rectangle(0, 0, clientWidth, clientHeight);
	pDC->SelectObject(pOldBrush);
	pDC->SelectObject(oldPen);
}

void CLab1View::DrawRegularPolygon(CDC* pDC, int cx, int cy, int r, int n, float rotAngle, bool fill, COLORREF color)
{
	double alpha = (360 / n) * (M_PI / 180);
	LPPOINT points = new POINT[n];
	for (int i = 0; i < n; i++)
	{
		points[i].x = cx + r * cos(alpha * i + rotAngle);
		points[i].y = cy - r * sin(alpha * i + rotAngle);
	}
	CPen pen(PS_SOLID, 5, this->penColor);
	CPen* oldPen = pDC->SelectObject(&pen);

	CBrush* pOldBrush;
	LOGBRUSH logBrush;
	CBrush brush;
	if (fill)
	{
		logBrush.lbColor = color;
		logBrush.lbStyle = BS_SOLID;
	}
	else
	{
		logBrush.lbStyle = BS_NULL;
	}
	brush.CreateBrushIndirect(&logBrush);
	pOldBrush = pDC->SelectObject(&brush);

	pDC->Polygon(points, n);
	pDC->SelectObject(pOldBrush);
	pDC->SelectObject(oldPen);
}

void CLab1View::DrawPolygon(CDC* pDC, LPPOINT points, int n, bool pattern, COLORREF color)
{
	CPen pen(PS_SOLID, 5, this->penColor);
	CPen* oldPen = pDC->SelectObject(&pen);

	CBrush* pOldBrush;
	LOGBRUSH logBrush;
	CBrush brush;
	if (pattern)
	{
		logBrush.lbColor = RGB(144, 160, 232);
		logBrush.lbStyle = BS_HATCHED;
		logBrush.lbHatch = HS_VERTICAL;
	}
	else
	{
		logBrush.lbColor = color;
		logBrush.lbStyle = BS_SOLID;
	}

	brush.CreateBrushIndirect(&logBrush);
	pOldBrush = pDC->SelectObject(&brush);

	pDC->Polygon(points, n);
	pDC->SelectObject(pOldBrush);
	pDC->SelectObject(oldPen);
}

void CLab1View::FindCenterOfTriangle(LPPOINT points, int n, int& cx, int& cy)
{
	cx = (points[0].x + points[1].x + points[2].x) / 3;
	cy = (points[0].y + points[1].y + points[2].y) / 3;
}

void CLab1View::DrawGrid(CDC* pDC, int clientWidth, int clientHeight, int numOfLines)
{
	if (this->isGridVisible == false)
	{
		return;
	}
	else 
	{
		CPen pen(PS_SOLID, 1, RGB(255, 255, 255));
		CPen* oldPen = pDC->SelectObject(&pen);
		int lineSpacing = clientWidth / (numOfLines + 1);
		pDC->MoveTo(0, 0);
		for (int i = 1; i <= numOfLines; i++)
		{
			pDC->MoveTo(i * lineSpacing, 0);
			pDC->LineTo(i * lineSpacing, clientHeight);
		}
		for (int i = 1; i <= numOfLines; i++)
		{
			pDC->MoveTo(0, i * lineSpacing);
			pDC->LineTo(clientWidth, i * lineSpacing);
		}
		pDC->SelectObject(oldPen);
	}
}

CLab1Doc* CLab1View::GetDocument() const // non-debug version is inline
{
	ASSERT(m_pDocument->IsKindOf(RUNTIME_CLASS(CLab1Doc)));
	return (CLab1Doc*)m_pDocument;
}
#endif 


// CLab1View message handlers


void CLab1View::OnKeyDown(UINT nChar, UINT nRepCnt, UINT nFlags)
{
	if (nChar && nChar == VK_SPACE)
	{
		this->isGridVisible = !this->isGridVisible;
		this->Invalidate();
	}

	CView::OnKeyDown(nChar, nRepCnt, nFlags);
}
