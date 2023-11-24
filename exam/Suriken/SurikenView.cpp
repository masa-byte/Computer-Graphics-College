

#include "pch.h"
#include "framework.h"
#ifndef SHARED_HANDLERS
#include "Suriken.h"
#endif

#include "SurikenDoc.h"
#include "SurikenView.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#endif
#include <corecrt_math_defines.h>


IMPLEMENT_DYNCREATE(CSurikenView, CView)

BEGIN_MESSAGE_MAP(CSurikenView, CView)
	ON_COMMAND(ID_FILE_PRINT, &CView::OnFilePrint)
	ON_COMMAND(ID_FILE_PRINT_DIRECT, &CView::OnFilePrint)
	ON_COMMAND(ID_FILE_PRINT_PREVIEW, &CView::OnFilePrintPreview)
	ON_WM_ERASEBKGND()
	ON_WM_KEYDOWN()
END_MESSAGE_MAP()


CSurikenView::CSurikenView() noexcept
{
	sky.Load(CString("res/sky.jpg"));
	bark.Load(CString("res/bark.jpg"));
	deg = translate = 0;
	cW = cH = 0;
}

CSurikenView::~CSurikenView()
{
}

BOOL CSurikenView::PreCreateWindow(CREATESTRUCT& cs)
{
	return CView::PreCreateWindow(cs);
}

void CSurikenView::DrawBlade(CDC* pDC, int size)
{
	CPen newPen(BS_SOLID, 1, RGB(224, 224, 224));
	CPen* oldPen = pDC->SelectObject(&newPen);
	CBrush newBrush(RGB(224, 224, 224));
	CBrush* oldBrush = pDC->SelectObject(&newBrush);

	LPPOINT points = new POINT[3];
	points[0].x = 0;
	points[0].y = 0;

	points[1].x = 0;
	points[1].y = 4 * size;

	points[2].x = size;
	points[2].y = size;

	pDC->Polygon(points, 3);

	points[2].x = -size;

	CBrush newBrush2(RGB(192, 192, 192));
	pDC->SelectObject(&newBrush2);

	pDC->Polygon(points, 3);

	Rotate(pDC, 90);
	Translate(pDC, 0.6 * size, -0.4 * size);

	Translate(pDC, 0.05 * size, 0);
	DrawTexxxt(pDC, 0.7 * size, RGB(255, 255, 255));
	Translate(pDC, -0.05 * size, 0);

	DrawTexxxt(pDC, 0.7 * size, RGB(60, 60, 60));
	Translate(pDC, -0.6 * size, 0.4 * size);
	Rotate(pDC, -90);

	pDC->SelectObject(&oldPen);
	pDC->SelectObject(&oldBrush);
}

void CSurikenView::Translate(CDC* pDC, double x, double y)
{
	XFORM form{};
	form.eDx = x;
	form.eDy = y;
	form.eM11 = 1;
	form.eM12 = 0;
	form.eM21 = 0;
	form.eM22 = 1;

	pDC->ModifyWorldTransform(&form, MWT_LEFTMULTIPLY);
}

void CSurikenView::Rotate(CDC* pDC, float angle)
{
	angle = angle * M_PI / 180;
	XFORM form{};
	form.eDx = 0;
	form.eDy = 0;
	form.eM11 = cos(angle);
	form.eM12 = sin(angle);
	form.eM21 = -sin(angle);
	form.eM22 = cos(angle);

	pDC->ModifyWorldTransform(&form, MWT_LEFTMULTIPLY);
}

void CSurikenView::DrawStar(CDC* pDC, int size)
{
	int prevMode = pDC->SetGraphicsMode(GM_ADVANCED);
	XFORM oldForm{};
	pDC->GetWorldTransform(&oldForm);

	Translate(pDC, translate, 0);

	// blade1
	Translate(pDC, 150, 220);
	Rotate(pDC, deg);
	DrawBlade(pDC, 40);

	// blade2
	Rotate(pDC, 90);
	DrawBlade(pDC, 40);

	// blade3
	Rotate(pDC, 90);
	DrawBlade(pDC, 40);

	// blade4
	Rotate(pDC, 90);
	DrawBlade(pDC, 40);


	pDC->SetWorldTransform(&oldForm);
	pDC->SetGraphicsMode(prevMode);
}

void CSurikenView::Coordinates(CDC* pDC)
{
	CPen newPen(BS_SOLID, 1, RGB(255, 0, 0));
	CPen* oldPen = pDC->SelectObject(&newPen);

	pDC->MoveTo(0, 0);
	pDC->LineTo(400, 0); // x
	pDC->MoveTo(0, 0);
	pDC->LineTo(0, 200); // y

	pDC->SelectObject(&oldPen);
}

void CSurikenView::DrawTexxxt(CDC* pDC, int size, COLORREF color)
{
	COLORREF oldColor = pDC->SetTextColor(color);
	int oldMode = pDC->SetBkMode(TRANSPARENT);
	CString message = L"Shuricane";
	CFont font;
	LOGFONT lf;
	memset(&lf, 0, sizeof(LOGFONT));
	lf.lfHeight = size;
	_tcscpy_s(lf.lfFaceName, L"Monotype Corsiva");
	font.CreateFontIndirect(&lf);
	CFont* oldFont = pDC->SelectObject(&font);
	pDC->TextOutW(0, 0, message);

	pDC->SelectObject(oldFont);
	pDC->SetTextColor(oldColor);
	pDC->SetBkMode(oldMode);
}

void CSurikenView::OnDraw(CDC* pDC)
{
	CSurikenDoc* pDoc = GetDocument();
	ASSERT_VALID(pDoc);
	if (!pDoc)
		return;

	CRect clientRect;
	GetClientRect(&clientRect);
	this->cH = clientRect.Height();
	this->cW = clientRect.Width();

	CDC memDC;
	memDC.CreateCompatibleDC(pDC);
	CBitmap bmp;
	bmp.CreateCompatibleBitmap(pDC, cW, cH);
	memDC.SelectObject(&bmp);

	sky.Draw(&memDC, CRect(0, 0, sky.Width(), cH), CRect(0, 0, cW, cH));
	DrawStar(&memDC, 20);
	bark.Draw(&memDC, CRect(0, 0, bark.Width(), cH), CRect(cW - bark.Width(), 0, cW, cH));

	pDC->BitBlt(0, 0, cW, cH, &memDC, 0, 0, SRCCOPY);
	memDC.DeleteDC();
}



BOOL CSurikenView::OnPreparePrinting(CPrintInfo* pInfo)
{
	// default preparation
	return DoPreparePrinting(pInfo);
}

void CSurikenView::OnBeginPrinting(CDC* /*pDC*/, CPrintInfo* /*pInfo*/)
{
	// TODO: add extra initialization before printing
}

void CSurikenView::OnEndPrinting(CDC* /*pDC*/, CPrintInfo* /*pInfo*/)
{
	// TODO: add cleanup after printing
}


#ifdef _DEBUG
void CSurikenView::AssertValid() const
{
	CView::AssertValid();
}

void CSurikenView::Dump(CDumpContext& dc) const
{
	CView::Dump(dc);
}

CSurikenDoc* CSurikenView::GetDocument() const // non-debug version is inline
{
	ASSERT(m_pDocument->IsKindOf(RUNTIME_CLASS(CSurikenDoc)));
	return (CSurikenDoc*)m_pDocument;
}
#endif //_DEBUG



BOOL CSurikenView::OnEraseBkgnd(CDC* pDC)
{
	return true;
}


void CSurikenView::OnKeyDown(UINT nChar, UINT nRepCnt, UINT nFlags)
{
	if (nChar == 'W')
	{
		translate == 880 ? deg : deg += 10;
		translate == 880 ? translate : translate += 20;
		Invalidate();
	}

	CView::OnKeyDown(nChar, nRepCnt, nFlags);
}
