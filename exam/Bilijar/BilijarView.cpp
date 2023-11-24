
// BilijarView.cpp : implementation of the CBilijarView class
//

#include "pch.h"
#include "framework.h"
// SHARED_HANDLERS can be defined in an ATL project implementing preview, thumbnail
// and search filter handlers and allows sharing of document code with that project.
#ifndef SHARED_HANDLERS
#include "Bilijar.h"
#endif

#include "BilijarDoc.h"
#include "BilijarView.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#endif

#include <corecrt_math_defines.h>


IMPLEMENT_DYNCREATE(CBilijarView, CView)

BEGIN_MESSAGE_MAP(CBilijarView, CView)
	ON_COMMAND(ID_FILE_PRINT, &CView::OnFilePrint)
	ON_COMMAND(ID_FILE_PRINT_DIRECT, &CView::OnFilePrint)
	ON_COMMAND(ID_FILE_PRINT_PREVIEW, &CView::OnFilePrintPreview)
END_MESSAGE_MAP()


CBilijarView::CBilijarView() noexcept
{
	wood.Load(CString("res/wood.jpg"));
	felt.Load(CString("res/felt2.jpg"));
	cw = ch = 0;
}

CBilijarView::~CBilijarView()
{
}

BOOL CBilijarView::PreCreateWindow(CREATESTRUCT& cs)
{

	return CView::PreCreateWindow(cs);
}

void CBilijarView::DrawStick(CDC* pDC, int w)
{

}

void CBilijarView::DrawBall(CDC* pDC, int w)
{
	CBrush newBrush(RGB(120, 0, 0));
	CBrush* oldBrush = pDC->SelectObject(&newBrush);
	CPen newPen(BS_SOLID, 1, RGB(30, 0, 0));
	CPen* odlPen = pDC->SelectObject(&newPen);

	pDC->Ellipse(0, 0, w, w);

	Translate(pDC, 12, 6, false);
	CBrush newBrush2(RGB(255, 255, 255));
	pDC->SelectObject(&newBrush2);
	pDC->Ellipse(0, 0, w / 3, w / 3);
	Translate(pDC, -12, -6, false);

	pDC->SelectObject(&oldBrush);
	pDC->SelectObject(&odlPen);
}

void CBilijarView::Translate(CDC* pDC, float dX, float dY, bool rightMultiply)
{
	XFORM form{};
	form.eDx = dX;
	form.eDy = dY;
	form.eM11 = 1;
	form.eM12 = 0;
	form.eM21 = 0;
	form.eM22 = 1;

	if (rightMultiply)
		pDC->ModifyWorldTransform(&form, MWT_RIGHTMULTIPLY);
	else
		pDC->ModifyWorldTransform(&form, MWT_LEFTMULTIPLY);
}

void CBilijarView::Rotate(CDC* pDC, float angle, bool rightMultiply)
{
	angle = angle * M_PI / 180;
	XFORM form{};
	form.eDx = 0;
	form.eDy = 0;
	form.eM11 = cos(angle);
	form.eM12 = sin(angle);
	form.eM21 = -sin(angle);
	form.eM22 = cos(angle);

	if (rightMultiply)
		pDC->ModifyWorldTransform(&form, MWT_RIGHTMULTIPLY);
	else
		pDC->ModifyWorldTransform(&form, MWT_LEFTMULTIPLY);
}

void CBilijarView::DrawTable(CDC* pDC, CRect rect)
{
	int imgWidth = felt.Width();
	int imgHeight = felt.Height();
	int crWidth = rect.Width();
	int crHeight = rect.Height();


	for (int i = 0; i < crWidth / imgWidth; i++) {
		for (int j = 0; j < crHeight / imgHeight; j++) {
			felt.Draw(pDC, CRect(0, 0, imgWidth, imgHeight),
				CRect(
					i * imgWidth + rect.left,
					j * imgHeight + rect.left,
					(i + 1) * imgWidth + rect.left,
					(j + 1) * imgHeight + rect.left));
		}
	}
}

void CBilijarView::DrawBorder(CDC* pDC, CRect rect, int w)
{
	int imgWidth = wood.Width();
	int imgHeight = wood.Height();
	int crWidth = rect.Width();
	int crHeight = rect.Height();

	wood.Draw(pDC, CRect(0, 0, imgWidth, imgHeight), CRect(0, 0, crWidth, imgHeight));

	CRect newRect(w, w, crWidth - w, crHeight - w);

	DrawTable(pDC, newRect);
}

void CBilijarView::DrawHoles(CDC* pDC, CRect rect, int size)
{
	int crWidth = rect.Width();
	int crHeight = rect.Height();
	CBrush newBrush(RGB(0, 0, 0));
	CBrush* oldBrush = pDC->SelectObject(&newBrush);

	pDC->Ellipse(size / 2, crHeight - 1.5 * size, 1.5 * size, crHeight - 0.5 * size);
	pDC->Ellipse(size / 2, 0.5 * size, 1.5 * size, 1.5 * size);
	pDC->Ellipse(crWidth - 1.5 * size, crHeight - 1.5 * size, crWidth - 0.5 * size, crHeight - 0.5 * size);
	pDC->Ellipse(crWidth - 1.5 * size, 0.5 * size, crWidth - 0.5 * size, 1.5 * size);
	pDC->Ellipse((crWidth - size) / 2, crHeight - 1.5 * size, (crWidth + size) / 2, crHeight - 0.5 * size);
	pDC->Ellipse((crWidth - size) / 2, 0.5 * size, (crWidth + size) / 2, 1.5 * size);

	pDC->SelectObject(&oldBrush);
}

void CBilijarView::DrawPool(CDC* pDC)
{
	int prevMode = pDC->SetGraphicsMode(GM_ADVANCED);
	XFORM oldForm{};
	pDC->GetWorldTransform(&oldForm);

	CRect clientRect;
	GetClientRect(&clientRect);
	DrawBorder(pDC, clientRect, 50);
	DrawHoles(pDC, clientRect, 50);

	Translate(pDC, 600, 400, false);
	DrawBall(pDC, 30);

	pDC->SetWorldTransform(&oldForm);
	pDC->SetGraphicsMode(prevMode);
}


void CBilijarView::OnDraw(CDC* pDC)
{
	CBilijarDoc* pDoc = GetDocument();
	ASSERT_VALID(pDoc);
	if (!pDoc)
		return;

	CRect clientRect;
	GetClientRect(&clientRect);
	cw = clientRect.Width();
	ch = clientRect.Height();

	CDC memDC;
	memDC.CreateCompatibleDC(pDC);
	CBitmap bmp;
	bmp.CreateCompatibleBitmap(pDC, clientRect.Width(), clientRect.Height());
	memDC.SelectObject(&bmp);

	DrawPool(&memDC);

	pDC->BitBlt(0, 0, clientRect.Width(), clientRect.Height(), &memDC, 0, 0, SRCCOPY);
	memDC.DeleteDC();
}



BOOL CBilijarView::OnPreparePrinting(CPrintInfo* pInfo)
{
	// default preparation
	return DoPreparePrinting(pInfo);
}

void CBilijarView::OnBeginPrinting(CDC* /*pDC*/, CPrintInfo* /*pInfo*/)
{
	// TODO: add extra initialization before printing
}

void CBilijarView::OnEndPrinting(CDC* /*pDC*/, CPrintInfo* /*pInfo*/)
{
	// TODO: add cleanup after printing
}


#ifdef _DEBUG
void CBilijarView::AssertValid() const
{
	CView::AssertValid();
}

void CBilijarView::Dump(CDumpContext& dc) const
{
	CView::Dump(dc);
}

CBilijarDoc* CBilijarView::GetDocument() const // non-debug version is inline
{
	ASSERT(m_pDocument->IsKindOf(RUNTIME_CLASS(CBilijarDoc)));
	return (CBilijarDoc*)m_pDocument;
}
#endif //_DEBUG

