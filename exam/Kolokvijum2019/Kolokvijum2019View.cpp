
#include "pch.h"
#include "framework.h"
#ifndef SHARED_HANDLERS
#include "Kolokvijum2019.h"
#endif

#include "Kolokvijum2019Doc.h"
#include "Kolokvijum2019View.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#endif

#define PI 3.14159265359


IMPLEMENT_DYNCREATE(CKolokvijum2019View, CView)

BEGIN_MESSAGE_MAP(CKolokvijum2019View, CView)
	ON_COMMAND(ID_FILE_PRINT, &CView::OnFilePrint)
	ON_COMMAND(ID_FILE_PRINT_DIRECT, &CView::OnFilePrint)
	ON_COMMAND(ID_FILE_PRINT_PREVIEW, &CView::OnFilePrintPreview)
	ON_WM_KEYDOWN()
	ON_WM_ERASEBKGND()
END_MESSAGE_MAP()


CKolokvijum2019View::CKolokvijum2019View() noexcept
{
	background = new DImage();
	background->Load(CString("res/background.jpg"));

	arm1 = new DImage();
	arm1->Load(CString("res/arm1.png"));

	arm2 = new DImage();
	arm2->Load(CString("res/arm2.png"));

	leg1 = new DImage();
	leg1->Load(CString("res/leg1.png"));

	leg2 = new DImage();
	leg2->Load(CString("res/leg2.png"));

	body = new DImage();
	body->Load(CString("res/body1.png"));

	degArm1 = degArm2 = degLeg1 = degLeg2 = degBody = 0;
	right = 0;
}

CKolokvijum2019View::~CKolokvijum2019View()
{
	delete background;
	delete arm1;
	delete arm2;
	delete leg1;
	delete leg2;
	delete body;
}

BOOL CKolokvijum2019View::PreCreateWindow(CREATESTRUCT& cs)
{
	return CView::PreCreateWindow(cs);
}


void CKolokvijum2019View::OnDraw(CDC* pDC)
{
	CKolokvijum2019Doc* pDoc = GetDocument();
	ASSERT_VALID(pDoc);
	if (!pDoc)
		return;

	GetClientRect(&clientRect);

	CDC memDC;
	memDC.CreateCompatibleDC(pDC);
	CBitmap bmp, *prevBmp;
	bmp.CreateCompatibleBitmap(pDC, clientRect.Width(), clientRect.Height());
	prevBmp = memDC.SelectObject(&bmp);

	DrawBackground(&memDC, CRect(0, 0, background->Width(), background->Height()));
	DrawTransformer(&memDC);

	pDC->BitBlt(0, 0, clientRect.Width(), clientRect.Height(), &memDC, 0, 0, SRCCOPY);
	memDC.DeleteDC();
}


BOOL CKolokvijum2019View::OnPreparePrinting(CPrintInfo* pInfo)
{
	// default preparation
	return DoPreparePrinting(pInfo);
}

void CKolokvijum2019View::OnBeginPrinting(CDC* /*pDC*/, CPrintInfo* /*pInfo*/)
{
	// TODO: add extra initialization before printing
}

void CKolokvijum2019View::OnEndPrinting(CDC* /*pDC*/, CPrintInfo* /*pInfo*/)
{
	// TODO: add cleanup after printing
}

// mine

void CKolokvijum2019View::DrawBackground(CDC* pDC, CRect rc)
{
	background->Draw(pDC, rc, CRect(0, 0, clientRect.Width(), clientRect.Height()));
}

void CKolokvijum2019View::DrawImgTransparent(CDC* pDC, DImage* pImage)
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

	pDC->BitBlt(0, 0, bm->bmWidth, bm->bmHeight, dstDC, 0, 0, SRCAND);
	pDC->BitBlt(0, 0, bm->bmWidth, bm->bmHeight, srcDC, 0, 0, SRCPAINT);

	dstDC->SetTextColor(clrSaveDstText);
	srcDC->SetBkColor(clrSaveBk);

	srcDC->SelectObject(pOldSrcBmp);
	dstDC->SelectObject(pOldDstBmp);

	delete srcDC;
	delete dstDC;
}

void CKolokvijum2019View::Translate(CDC* pDC, float dX, float dY, bool rightMultiply)
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

void CKolokvijum2019View::Rotate(CDC* pDC, float angle, bool rightMultiply)
{
	angle = angle * PI / 180;

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

void CKolokvijum2019View::DrawArm1(CDC* pDC)
{
	DrawImgTransparent(pDC, arm1);
}

void CKolokvijum2019View::DrawArm2(CDC* pDC)
{
	DrawImgTransparent(pDC, arm2);
}

void CKolokvijum2019View::DrawLeg1(CDC* pDC)
{
	DrawImgTransparent(pDC, leg1);
}

void CKolokvijum2019View::DrawLeg2(CDC* pDC)
{
	DrawImgTransparent(pDC, leg2);
}

void CKolokvijum2019View::DrawBody(CDC* pDC)
{
	DrawImgTransparent(pDC, body);
}

void CKolokvijum2019View::DrawTransformer(CDC* pDC)
{
	int prevMode = pDC->SetGraphicsMode(GM_ADVANCED);
	XFORM oldForm{};
	pDC->GetWorldTransform(&oldForm);

	Translate(pDC, right, 0, false);

	// body
	XFORM bodyForm{};
	Translate(pDC, (1. / 12) * clientRect.Width(), (1. / 2) * clientRect.Height(), false);
	Rotate(pDC, -90 + degBody, false);
	Translate(pDC, -26, -133, false);
	DrawBody(pDC);
	pDC->GetWorldTransform(&bodyForm);

	// arm1
	XFORM arm1Form{};
	Translate(pDC, 210, 87, false);
	Rotate(pDC, 90 + degArm1, false);
	Translate(pDC, -34, -31, false);
	pDC->GetWorldTransform(&arm1Form);
	//DrawArm1(pDC);

	// arm2
	Translate(pDC, 210, 103, false);
	Rotate(pDC, -90 + degArm2, false);
	Translate(pDC, -23, -61, false);
	DrawArm2(pDC);

	// arm1 continue
	pDC->SetWorldTransform(&arm1Form);
	DrawArm1(pDC);

	// leg1
	XFORM leg1Form{};
	pDC->SetWorldTransform(&bodyForm);
	Translate(pDC, 23, 130, false);
	Rotate(pDC, -20 + degLeg1, false);
	Translate(pDC, -206, 0, false);
	Translate(pDC, -30, -125, false);
	pDC->GetWorldTransform(&leg1Form);
	//DrawLeg1(pDC);

	// leg 2
	Translate(pDC, 30, 125, false);
	Rotate(pDC, -140 + degLeg2, false);
	Translate(pDC, -35, -60, false);
	DrawLeg2(pDC);

	// leg 1 continue
	pDC->SetWorldTransform(&leg1Form);
	DrawLeg1(pDC);


	pDC->SetWorldTransform(&oldForm);
	pDC->SetGraphicsMode(prevMode);
}

void CKolokvijum2019View::Coordinates(CDC* pDC)
{
	CPen pen(BS_SOLID, 3, RGB(255, 0, 0));
	CPen* oldPen = pDC->SelectObject(&pen);
	pDC->MoveTo(0, 0);
	pDC->LineTo(300, 0); // x coordinate
	pDC->MoveTo(0, 0);
	pDC->LineTo(0, 200); // y coordinate
	pDC->SelectObject(&oldPen);
}


#ifdef _DEBUG
void CKolokvijum2019View::AssertValid() const
{
	CView::AssertValid();
}

void CKolokvijum2019View::Dump(CDumpContext& dc) const
{
	CView::Dump(dc);
}

CKolokvijum2019Doc* CKolokvijum2019View::GetDocument() const // non-debug version is inline
{
	ASSERT(m_pDocument->IsKindOf(RUNTIME_CLASS(CKolokvijum2019Doc)));
	return (CKolokvijum2019Doc*)m_pDocument;
}
#endif //_DEBUG


void CKolokvijum2019View::OnKeyDown(UINT nChar, UINT nRepCnt, UINT nFlags)
{
	switch (nChar)
	{
		case 'W': degBody == 90 ? degBody : degBody += 10;
			break;
		case 'S': degBody == 0 ? degBody : degBody -= 10;
			break;
		case 'T': degArm2 == 0 ? degArm2 : degArm2 += 10;
			break;
		case 'G': degArm2 == -90 ? degArm2 : degArm2 -= 10;
			break;
		case 'Q': {
			degArm1 == 0 ? degArm1 : degArm1 += 10;
			degLeg1 == 20 ? degLeg1 : degLeg1 += 10;
			break;
		}
		case 'A': {
			degArm1 == -90 ? degArm1 : degArm1 -= 10;
			degLeg1 == 0 ? degLeg1 : degLeg1 -= 10;
			break;
		}
		case 'R': degLeg2 == 140 ? degLeg2 : degLeg2 += 10;
			break;
		case 'F': degLeg2 == 0 ? degLeg2 : degLeg2 -= 10;
			break;


		case VK_RIGHT: {
			degBody == 90 ? degBody : degBody += 10;
			degArm1 == -90 ? degArm1 : degArm1 -= 10;
			degLeg1 == 20 ? degLeg1 : degLeg1 += 10;
			degArm2 == -90 ? degArm2 : degArm2 -= 10;
			degLeg2 == 140 ? degLeg2 : degLeg2 += 10;
			right += 20;
		}
			break;
		case VK_LEFT: {
			degBody == 0 ? degBody : degBody -= 10;
			degArm1 == 0 ? degArm1 : degArm1 += 10;
			degLeg1 == 0 ? degLeg1 : degLeg1 -= 10;
			degArm2 == 0 ? degArm2 : degArm2 += 10;
			degLeg2 == 0 ? degLeg2 : degLeg2 -= 10;
			right -= 10;
		}
			break;
	}
	Invalidate();

	CView::OnKeyDown(nChar, nRepCnt, nFlags);
}


BOOL CKolokvijum2019View::OnEraseBkgnd(CDC* pDC)
{
	return true;
}
