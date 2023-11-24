
#include "pch.h"
#include "framework.h"
#ifndef SHARED_HANDLERS
#include "Kolokvijum2018.h"
#endif

#include "Kolokvijum2018Doc.h"
#include "Kolokvijum2018View.h"

#define M_PI 3.14159265358979323846

#ifdef _DEBUG
#define new DEBUG_NEW
#endif


IMPLEMENT_DYNCREATE(CKolokvijum2018View, CView)

BEGIN_MESSAGE_MAP(CKolokvijum2018View, CView)
	ON_COMMAND(ID_FILE_PRINT, &CView::OnFilePrint)
	ON_COMMAND(ID_FILE_PRINT_DIRECT, &CView::OnFilePrint)
	ON_COMMAND(ID_FILE_PRINT_PREVIEW, &CView::OnFilePrintPreview)
	ON_WM_KEYDOWN()
	ON_WM_ERASEBKGND()
END_MESSAGE_MAP()


CKolokvijum2018View::CKolokvijum2018View() noexcept
{
	bager.Load(CString("res/bager.png"));
	arm1.Load(CString("res/arm1.png"));
	arm2.Load(CString("res/arm2.png"));
	pozadina.Load(CString("res/pozadina.png"));
	viljuska = GetEnhMetaFile(L"res/viljuska.emf");

	degArm1 = degArm2 = degFork = 0;

	ENHMETAHEADER header;
	GetEnhMetaFileHeader(viljuska, sizeof(header), &header);

	viljuskaW = header.rclBounds.right - header.rclBounds.left;
	viljuskaH = header.rclBounds.bottom - header.rclBounds.top;

	right = 0;
}

CKolokvijum2018View::~CKolokvijum2018View()
{
}

BOOL CKolokvijum2018View::PreCreateWindow(CREATESTRUCT& cs)
{

	return CView::PreCreateWindow(cs);
}


void CKolokvijum2018View::OnDraw(CDC* pDC)
{
	CKolokvijum2018Doc* pDoc = GetDocument();
	ASSERT_VALID(pDoc);
	if (!pDoc)
		return;

	CRect clientRect;
	GetClientRect(&clientRect);

	CDC memDC;
	memDC.CreateCompatibleDC(pDC);
	CBitmap bmp;
	bmp.CreateCompatibleBitmap(pDC, clientRect.Width(), clientRect.Height());
	memDC.SelectObject(&bmp);

	DrawBackground(&memDC);
	DrawExcavator(&memDC);

	pDC->BitBlt(0, 0, clientRect.Width(), clientRect.Height(), &memDC, 0, 0, SRCCOPY);
	memDC.DeleteDC();
}

void CKolokvijum2018View::Translate(CDC* pDC, double dX, double dY, bool rightMultiply)
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

void CKolokvijum2018View::Rotate(CDC* pDC, float angle, bool rightMultiply)
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

void CKolokvijum2018View::Scale(CDC* pDC, float sX, float sY, bool rightMultiply)
{
	XFORM form{};
	form.eDx = 0;
	form.eDy = 0;
	form.eM11 = sX;
	form.eM12 = 0;
	form.eM21 = 0;
	form.eM22 = sY;

	if (rightMultiply)
		pDC->ModifyWorldTransform(&form, MWT_RIGHTMULTIPLY);
	else
		pDC->ModifyWorldTransform(&form, MWT_LEFTMULTIPLY);
}

void CKolokvijum2018View::DrawBackground(CDC* pDC)
{
	CRect clientRect;
	GetClientRect(&clientRect);

	int imgWidth = pozadina.Width();
	int imgHeight = pozadina.Height();
	int crWidth = clientRect.Width();
	int crHeight = clientRect.Height();

	CRect rectImage(0, 0, imgWidth, imgHeight);
	CRect rectDC((crWidth - imgWidth) / 2, crHeight - imgHeight, (crWidth + imgWidth) / 2, crHeight);
	pozadina.Draw(pDC, rectImage, rectDC);
}

void CKolokvijum2018View::Coordinates(CDC* pDC)
{
	pDC->MoveTo(0, 0);
	pDC->LineTo(400, 0); // x axis
	pDC->MoveTo(0, 0);
	pDC->LineTo(0, 200); // y axis
}

void CKolokvijum2018View::DrawImgTransparent(CDC* pDC, DImage* pImage, float offsetX, float offsetY)
{
	int w = pImage->Width();
	int h = pImage->Height();

	CBitmap* bmpImage = new CBitmap();
	bmpImage->CreateCompatibleBitmap(pDC, w, h);

	CBitmap* bmpMask = new CBitmap();
	bmpMask->CreateBitmap(w, h, 1, 1, NULL);

	BITMAP* bm = new BITMAP();
	bmpImage->GetBitmap(bm);

	CDC* srcDC = new CDC();
	srcDC->CreateCompatibleDC(pDC);

	CDC* dstDC = new CDC();
	dstDC->CreateCompatibleDC(pDC);

	CBitmap* pOldSrcBmp = srcDC->SelectObject(bmpImage);
	CBitmap* pOldDstBmp = dstDC->SelectObject(bmpMask);

	pImage->Draw(srcDC, CRect(0, 0, w, h), CRect(0, 0, w, h));

	COLORREF clrTopLeft = srcDC->GetPixel(0, 0);
	COLORREF clrSaveBk = srcDC->SetBkColor(clrTopLeft);
	COLORREF clrSaveDstText = srcDC->SetTextColor(RGB(255, 255, 255));

	dstDC->BitBlt(0, 0, bm->bmWidth, bm->bmHeight, srcDC, 0, 0, SRCCOPY);
	srcDC->SetBkColor(RGB(0, 0, 0));
	srcDC->BitBlt(0, 0, bm->bmWidth, bm->bmHeight, dstDC, 0, 0, SRCAND);

	pDC->BitBlt(offsetX, offsetY, bm->bmWidth, bm->bmHeight, dstDC, 0, 0, SRCAND);
	pDC->BitBlt(offsetX, offsetY, bm->bmWidth, bm->bmHeight, srcDC, 0, 0, SRCPAINT);

	dstDC->SetTextColor(clrSaveDstText);
	srcDC->SetBkColor(clrSaveBk);

	srcDC->SelectObject(pOldSrcBmp);
	dstDC->SelectObject(pOldDstBmp);

	delete srcDC;
	delete dstDC;
}

void CKolokvijum2018View::DrawBody(CDC* pDC)
{
	DrawImgTransparent(pDC, &bager, 0, 0);
}

void CKolokvijum2018View::DrawArm1(CDC* pDC)
{
	DrawImgTransparent(pDC, &arm1, 0, 0);
}

void CKolokvijum2018View::DrawArm2(CDC* pDC)
{
	DrawImgTransparent(pDC, &arm2, 0, 0);
}

void CKolokvijum2018View::DrawFork(CDC* pDC)
{
	PlayEnhMetaFile(pDC->m_hDC, viljuska, CRect(0, 0, viljuskaW, viljuskaH));
}

void CKolokvijum2018View::DrawExcavator(CDC* pDC)
{
	int prevMode = pDC->SetGraphicsMode(GM_ADVANCED);
	XFORM oldForm{};
	pDC->GetWorldTransform(&oldForm);

	CRect clientRect;
	GetClientRect(&clientRect);

	Translate(pDC, right, 0, false);
	{
		// body
		Translate(pDC, (7. / 10) * clientRect.Width(), (1. / 2) * clientRect.Height(), false);
		DrawBody(pDC);

		// arm1
		Translate(pDC, (1. / 6) * bager.Width(), (3.4 / 6) * bager.Height(), false);
		Rotate(pDC, -90 + degArm1, false);
		Translate(pDC, -58, -61, false);
		DrawArm1(pDC);


		// arm2
		Translate(pDC, 309, 61, false);
		Rotate(pDC, -90 + degArm2, false);
		Translate(pDC, -36, -40, false);
		DrawArm2(pDC);

		// fork
		Translate(pDC, 272, 40, false);
		Scale(pDC, 2.5, 2.5, false);
		Rotate(pDC, -90 + degFork, false);
		Translate(pDC, -14, -20, false);
		DrawFork(pDC);
	}


	pDC->SetWorldTransform(&oldForm);
	pDC->SetGraphicsMode(prevMode);
}


BOOL CKolokvijum2018View::OnPreparePrinting(CPrintInfo* pInfo)
{
	// default preparation
	return DoPreparePrinting(pInfo);
}

void CKolokvijum2018View::OnBeginPrinting(CDC* /*pDC*/, CPrintInfo* /*pInfo*/)
{
	// TODO: add extra initialization before printing
}

void CKolokvijum2018View::OnEndPrinting(CDC* /*pDC*/, CPrintInfo* /*pInfo*/)
{
	// TODO: add cleanup after printing
}



#ifdef _DEBUG
void CKolokvijum2018View::AssertValid() const
{
	CView::AssertValid();
}

void CKolokvijum2018View::Dump(CDumpContext& dc) const
{
	CView::Dump(dc);
}

CKolokvijum2018Doc* CKolokvijum2018View::GetDocument() const // non-debug version is inline
{
	ASSERT(m_pDocument->IsKindOf(RUNTIME_CLASS(CKolokvijum2018Doc)));
	return (CKolokvijum2018Doc*)m_pDocument;
}
#endif //_DEBUG


// CKolokvijum2018View message handlers


void CKolokvijum2018View::OnKeyDown(UINT nChar, UINT nRepCnt, UINT nFlags)
{
	switch (nChar) {
	case '1': degArm1 += 10;
		break;
	case '2': degArm1 -= 10;
		break;
	case '3': degArm2 += 10;
		break;
	case '4': degArm2 -= 10;
		break;
	case '5': degFork += 10;
		break;
	case '6': degFork -= 10;
		break;
	case VK_RIGHT: right += 10;
		break;
	case VK_LEFT: right -= 10;
		break;
	}

	Invalidate();

	CView::OnKeyDown(nChar, nRepCnt, nFlags);
}


BOOL CKolokvijum2018View::OnEraseBkgnd(CDC* pDC)
{
	return true;
}
