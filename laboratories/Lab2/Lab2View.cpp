

#include "pch.h"
#include <cmath>
#include "framework.h"
#define _USE_MATH_DEFINES
#ifndef SHARED_HANDLERS
#include "Lab2.h"
#endif

#include "Lab2Doc.h"
#include "Lab2View.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#endif


IMPLEMENT_DYNCREATE(CLab2View, CView)

BEGIN_MESSAGE_MAP(CLab2View, CView)
	ON_COMMAND(ID_FILE_PRINT, &CView::OnFilePrint)
	ON_COMMAND(ID_FILE_PRINT_DIRECT, &CView::OnFilePrint)
	ON_COMMAND(ID_FILE_PRINT_PREVIEW, &CView::OnFilePrintPreview)
	ON_WM_KEYDOWN()
END_MESSAGE_MAP()


CLab2View::CLab2View() noexcept
{}

CLab2View::~CLab2View()
{
}

BOOL CLab2View::PreCreateWindow(CREATESTRUCT& cs)
{
	// TODO: Modify the Window class or styles here by modifying
	//  the CREATESTRUCT cs

	return CView::PreCreateWindow(cs);
}

void CLab2View::OnDraw(CDC* pDC)
{
	CLab2Doc* pDoc = GetDocument();
	ASSERT_VALID(pDoc);
	if (!pDoc)
		return;

	int numOfLines = 19;

	this->cellWidth = clientWidth / (numOfLines + 1);

	this->DrawBackground(pDC, clientWidth, clientHeight);

	this->DrawFigure(pDC);

	this->DrawGrid(pDC, clientWidth, clientHeight, numOfLines);
	this->WriteText(pDC, 3, RGB(0, 0, 0));
	this->WriteText(pDC, 0, RGB(251, 253, 8));
}

void CLab2View::DrawBackground(CDC* pDC, int clientWidth, int clientHeight)
{
	CPen pen(PS_NULL, 1, RGB(135, 206, 235));
	CPen* oldPen = pDC->SelectObject(&pen);
	CBrush brush(RGB(135, 206, 235));
	CBrush* pOldBrush = pDC->SelectObject(&brush);
	pDC->SetBrushOrg(0, 0);
	pDC->Rectangle(0, 0, clientWidth, clientHeight);
	pDC->SelectObject(pOldBrush);
	pDC->SelectObject(oldPen);
}

void CLab2View::DrawGrid(CDC* pDC, int clientWidth, int clientHeight, int numOfLines)
{
	if (this->isGridVisible == false)
	{
		return;
	}
	else
	{
		CPen pen(PS_SOLID, 1, RGB(210, 236, 247));
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

void CLab2View::DrawPolygon(CDC* pDC, LPPOINT points, int n, COLORREF brushColor, COLORREF penColor)
{
	CPen pen(PS_SOLID, 1, penColor);
	CPen* oldPen = pDC->SelectObject(&pen);

	LOGBRUSH logBrush;
	CBrush brush;
	logBrush.lbColor = brushColor;
	logBrush.lbStyle = BS_SOLID;

	brush.CreateBrushIndirect(&logBrush);

	CBrush* pOldBrush = pDC->SelectObject(&brush);

	pDC->Polygon(points, n);
	pDC->SelectObject(pOldBrush);
	pDC->SelectObject(oldPen);
}

void CLab2View::DrawCoordinateSystem(CDC* pDC)
{
	int x = 0;
	int y = 0;

	CPen pen(PS_SOLID, 5, RGB(0, 0, 0));
	CPen* oldPen = pDC->SelectObject(&pen);
	pDC->MoveTo(x, y);
	pDC->LineTo(x + 50, y);
	pDC->MoveTo(x, y);
	pDC->LineTo(x, y + 50);

	pDC->SelectObject(oldPen);
}

void CLab2View::WriteText(CDC* pDC, double offset, COLORREF color)
{
	COLORREF oldColor = pDC->SetTextColor(color);
	int oldMode = pDC->SetBkMode(TRANSPARENT);
	CString message = L"18463 Masa Cirkovic";
	CFont font;
	LOGFONT lf;
	memset(&lf, 0, sizeof(LOGFONT));
	lf.lfHeight = 1.2 * this->cellWidth;
	lf.lfWidth = 15;
	lf.lfWeight = FW_BOLD;
	_tcscpy_s(lf.lfFaceName, L"Arial");
	lf.lfEscapement = 900;
	font.CreateFontIndirect(&lf);
	CFont* oldFont = pDC->SelectObject(&font);
	pDC->TextOutW(int(1 * this->cellWidth), int(19 * this->cellWidth + offset), message);

	pDC->SelectObject(oldFont);
	pDC->SetTextColor(oldColor);
	pDC->SetBkMode(oldMode);
}

void CLab2View::Translate(CDC* pDC, float dX, float dY, bool rightMultiply)
{
	XFORM form{};
	form.eM11 = 1;
	form.eM12 = 0;
	form.eM21 = 0;
	form.eM22 = 1;
	form.eDx = dX;
	form.eDy = dY;

	if (rightMultiply)
	{
		pDC->ModifyWorldTransform(&form, MWT_RIGHTMULTIPLY);
	}
	else
	{
		pDC->ModifyWorldTransform(&form, MWT_LEFTMULTIPLY);
	}
}

void CLab2View::Scale(CDC* pDC, float sX, float sY, bool rightMultiply)
{
	XFORM form{};
	form.eM11 = sX;
	form.eM12 = 0;
	form.eM21 = 0;
	form.eM22 = sY;
	form.eDx = 0;
	form.eDy = 0;

	if (rightMultiply)
	{
		pDC->ModifyWorldTransform(&form, MWT_RIGHTMULTIPLY);
	}
	else
	{
		pDC->ModifyWorldTransform(&form, MWT_LEFTMULTIPLY);
	}
}

void CLab2View::Rotate(CDC* pDC, float angle, bool rightMultiply)
{
	XFORM form{};
	form.eM11 = cos(angle);
	form.eM12 = sin(angle);
	form.eM21 = -sin(angle);
	form.eM22 = cos(angle);
	form.eDx = 0;
	form.eDy = 0;

	if (rightMultiply)
	{
		pDC->ModifyWorldTransform(&form, MWT_RIGHTMULTIPLY);
	}
	else
	{
		pDC->ModifyWorldTransform(&form, MWT_LEFTMULTIPLY);
	}
}

void CLab2View::DrawPot(CDC* pDC)
{
	this->DrawPolygon(pDC, new POINT[4]{
	{int(cellWidth * -2), int(cellWidth * -1)},
	{int(cellWidth * 2), int(cellWidth * -1)},
	{int(cellWidth * 1.6), int(cellWidth * 0.95)},
	{int(cellWidth * -1.6), int(cellWidth * 0.95)} },
	4, RGB(222, 148, 0), RGB(79, 82, 24));

	this->DrawPolygon(pDC, new POINT[4]{
		{int(cellWidth * -2.5), int(cellWidth * -1.8)},
		{int(cellWidth * 2.5), int(cellWidth * -1.8)},
		{int(cellWidth * 2.5), int(cellWidth * -1)},
		{int(cellWidth * -2.5), int(cellWidth * -1)} },
		4, RGB(222, 148, 0), RGB(79, 82, 24));
}

void CLab2View::DrawElipse(CDC* pDC, int x1, int y1, int x2, int y2)
{
	CPen pen(PS_SOLID, 1, RGB(0, 0, 0));
	CPen* oldPen = pDC->SelectObject(&pen);
	CBrush brush(RGB(0, 204, 0));
	CBrush* pOldBrush = pDC->SelectObject(&brush);
	pDC->Ellipse(x1, y1, x2, y2);
	pDC->SelectObject(pOldBrush);
	pDC->SelectObject(oldPen);
}
void CLab2View::DrawFigure(CDC* pDC)
{
	// rotacija krece kada se dodje u joint iz koga krece struktura koju treba rotirati
	// kada se sve iscrta i vratimo se u joint, ide undo rotacije
	// rotacije ulaze u viticaste zagrade

	int circleHalfRadius = int(0.5 * cellWidth);
	HENHMETAFILE mfDark = GetEnhMetaFile(L"cactus_part.emf");
	HENHMETAFILE mfLight = GetEnhMetaFile(L"cactus_part_light.emf");

	int prevMode = pDC->SetGraphicsMode(GM_ADVANCED);
	XFORM oldForm, initialForm{};
	BOOL b = pDC->GetWorldTransform(&oldForm);
	pDC -> SetWorldTransform(&initialForm);

	// figure
	this->Translate(pDC, 0, clientHeight, false);
	this->Rotate(pDC, -90 * M_PI / 180, false);
	this->DrawCoordinateSystem(pDC);
	{
		// main trunk
		this->Translate(pDC, 10 * cellWidth, 17 * cellWidth, false);
		this->DrawElipse(pDC, -circleHalfRadius, -circleHalfRadius, circleHalfRadius, circleHalfRadius);
		this->Translate(pDC, 0, -1.5 * cellWidth, false);
		PlayEnhMetaFile(pDC->m_hDC, mfDark, CRect(-1.3 * cellWidth, -1.5 * cellWidth, 1.3 * cellWidth, 1.5 * cellWidth));
		this->Translate(pDC, 0, -1.5 * cellWidth, false);
		this->DrawElipse(pDC, -circleHalfRadius, -circleHalfRadius, circleHalfRadius, circleHalfRadius);

		// left arm
		this->Rotate(pDC, -45 * M_PI / 180, false);
		this->Translate(pDC, 0, -1 * M_SQRT2 * cellWidth, false);
		PlayEnhMetaFile(pDC->m_hDC, mfDark, CRect(-0.8 * cellWidth, -1.7 * cellWidth, 0.8 * cellWidth, 1.4 * cellWidth));
		this->Translate(pDC, 0, -1.2 * M_SQRT2 * cellWidth, false);
		this->Translate(pDC, 0, -1.2 * M_SQRT2 * cellWidth, false);
		PlayEnhMetaFile(pDC->m_hDC, mfDark, CRect(-1.4 * cellWidth, -1.5 * cellWidth, 1.4 * cellWidth, 1.6 * cellWidth));
		this->Translate(pDC, 0, 1.2 * M_SQRT2 * cellWidth, false);
		this->DrawElipse(pDC, -circleHalfRadius, -circleHalfRadius, circleHalfRadius, circleHalfRadius);
		this->Translate(pDC, 0, 1.2 * M_SQRT2 * cellWidth, false);
		this->Translate(pDC, 0, 1 * M_SQRT2 * cellWidth, false);
		this->Rotate(pDC, 45 * M_PI / 180, false);

		// right arm
		// rotation
		{
			this->Rotate(pDC, (45 + degreeBig) * M_PI / 180, false);
			this->Translate(pDC, 0, -1 * M_SQRT2 * cellWidth, false);
			PlayEnhMetaFile(pDC->m_hDC, mfLight, CRect(-0.8 * cellWidth, -1.7 * cellWidth, 0.8 * cellWidth, 1.4 * cellWidth));
			this->Translate(pDC, 0, -1.2 * M_SQRT2 * cellWidth, false);
			this->Translate(pDC, 0, -1.5 * cellWidth, false);
			PlayEnhMetaFile(pDC->m_hDC, mfDark, CRect(-0.5 * cellWidth, -1.5 * cellWidth, 0.5 * cellWidth, 1.4 * cellWidth));
			this->Translate(pDC, 0, -1.5 * cellWidth, false);
			this->Translate(pDC, 0, -1.2 * M_SQRT2 * cellWidth, false);
			PlayEnhMetaFile(pDC->m_hDC, mfDark, CRect(-1.4 * cellWidth, -1.5 * cellWidth, 1.4 * cellWidth, 1.6 * cellWidth));
			this->Translate(pDC, 0, 1.2 * M_SQRT2 * cellWidth, false);
			this->DrawElipse(pDC, -circleHalfRadius, -circleHalfRadius, circleHalfRadius, circleHalfRadius);
			this->Translate(pDC, 0, 1.5 * cellWidth, false);
			this->Translate(pDC, 0, 1.5 * cellWidth, false);

			this->Rotate(pDC, 45 * M_PI / 180, false);
			this->Translate(pDC, 0, -1.5 * cellWidth, false);
			PlayEnhMetaFile(pDC->m_hDC, mfDark, CRect(-0.5 * cellWidth, -1.5 * cellWidth, 0.5 * cellWidth, 1.4 * cellWidth));
			this->Translate(pDC, 0, -1.5 * cellWidth, false);
			// rotation
			{
				this->Rotate(pDC, (45 + degreeSmall) * M_PI / 180, false);
				this->Translate(pDC, 0, -1.5 * cellWidth, false);
				PlayEnhMetaFile(pDC->m_hDC, mfLight, CRect(-0.5 * cellWidth, -1.5 * cellWidth, 0.5 * cellWidth, 1.4 * cellWidth));
				this->Translate(pDC, 0, 1.5 * cellWidth, false);
				this->Rotate(pDC, -(45 + degreeSmall) * M_PI / 180, false);
			}

			this->Translate(pDC, 0, -1.5 * cellWidth, false);
			PlayEnhMetaFile(pDC->m_hDC, mfDark, CRect(-0.5 * cellWidth, -1.5 * cellWidth, 0.5 * cellWidth, 1.4 * cellWidth));
			this->Translate(pDC, 0, 1.5 * cellWidth, false);
			this->Rotate(pDC, -45 * M_PI / 180, false);
			this->Translate(pDC, 0, -1.5 * cellWidth, false);
			PlayEnhMetaFile(pDC->m_hDC, mfDark, CRect(-0.5 * cellWidth, -1.5 * cellWidth, 0.5 * cellWidth, 1.4 * cellWidth));
			this->Translate(pDC, 0, 1.5 * cellWidth, false);
			this->Rotate(pDC, 45 * M_PI / 180, false);
			this->DrawElipse(pDC, -circleHalfRadius, -circleHalfRadius, circleHalfRadius, circleHalfRadius);
			this->Translate(pDC, 0, 3 * cellWidth, false);
			this->Rotate(pDC, -45 * M_PI / 180, false);

			this->Rotate(pDC, -45 * M_PI / 180, false);
			this->Translate(pDC, 0, -1.5 * cellWidth, false);
			PlayEnhMetaFile(pDC->m_hDC, mfDark, CRect(-0.5 * cellWidth, -1.5 * cellWidth, 0.5 * cellWidth, 1.4 * cellWidth));
			this->Translate(pDC, 0, 1.5 * cellWidth, false);
			this->Rotate(pDC, 45 * M_PI / 180, false);

			this->DrawElipse(pDC, -circleHalfRadius, -circleHalfRadius, circleHalfRadius, circleHalfRadius);
			this->Translate(pDC, 0, 2.2 * M_SQRT2 * cellWidth, false);
			this->DrawElipse(pDC, -circleHalfRadius, -circleHalfRadius, circleHalfRadius, circleHalfRadius);

			this->Rotate(pDC, -(45 + degreeBig) * M_PI / 180, false);
		}

		// pot
		this->Translate(pDC, 0, 3 * cellWidth, false);

		this->Translate(pDC, 0, 2 * cellWidth, false);
		this->DrawPot(pDC);
	}
	this->Rotate(pDC, -90 * M_PI / 180, false);

	DeleteEnhMetaFile(mfDark);
	DeleteEnhMetaFile(mfLight);

	b = pDC->SetWorldTransform(&oldForm);
	pDC->SetGraphicsMode(prevMode);
}


BOOL CLab2View::OnPreparePrinting(CPrintInfo* pInfo)
{
	return DoPreparePrinting(pInfo);
}

void CLab2View::OnBeginPrinting(CDC* /*pDC*/, CPrintInfo* /*pInfo*/)
{}

void CLab2View::OnEndPrinting(CDC* /*pDC*/, CPrintInfo* /*pInfo*/)
{}

#ifdef _DEBUG
void CLab2View::AssertValid() const
{
	CView::AssertValid();
}

void CLab2View::Dump(CDumpContext& dc) const
{
	CView::Dump(dc);
}

CLab2Doc* CLab2View::GetDocument() const 
{
	ASSERT(m_pDocument->IsKindOf(RUNTIME_CLASS(CLab2Doc)));
	return (CLab2Doc*)m_pDocument;
}
#endif


void CLab2View::OnKeyDown(UINT nChar, UINT nRepCnt, UINT nFlags)
{
	if (nChar)
	{
		switch (nChar)
		{
			case VK_SPACE:
				this->isGridVisible = !this->isGridVisible;
				break;
			case VK_RIGHT:
				this->degreeSmall -= 5;
				break;
			case VK_LEFT:
				this->degreeSmall += 5;
				break;
			case VK_UP:
				this->degreeBig += 5;
				break;
			case VK_DOWN:
				this->degreeBig -= 5;
				break;
		}
		this->Invalidate();
	}

	CView::OnKeyDown(nChar, nRepCnt, nFlags);
}
