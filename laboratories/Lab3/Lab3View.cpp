
#include "pch.h"
#include "framework.h"
#ifndef SHARED_HANDLERS
#include "Lab3.h"
#endif

#include "Lab3Doc.h"
#include "Lab3View.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#endif

#define _USE_MATH_DEFINES
#include <string>
#include <math.h>

using namespace std;

IMPLEMENT_DYNCREATE(CLab3View, CView)

BEGIN_MESSAGE_MAP(CLab3View, CView)
	ON_COMMAND(ID_FILE_PRINT, &CView::OnFilePrint)
	ON_COMMAND(ID_FILE_PRINT_DIRECT, &CView::OnFilePrint)
	ON_COMMAND(ID_FILE_PRINT_PREVIEW, &CView::OnFilePrintPreview)
	ON_WM_ERASEBKGND()
END_MESSAGE_MAP()


CLab3View::CLab3View() noexcept
{
	// TODO: add construction code here
	clientHeight = 500;
	clientWidth = 500;
	int numOfLines = 19;

	cellWidth = clientWidth / (numOfLines + 1);
}

CLab3View::~CLab3View()
{
}

BOOL CLab3View::PreCreateWindow(CREATESTRUCT& cs)
{
	// TODO: Modify the Window class or styles here by modifying
	//  the CREATESTRUCT cs

	return CView::PreCreateWindow(cs);
}


void CLab3View::OnDraw(CDC* pDC)
{
	CLab3Doc* pDoc = GetDocument();
	ASSERT_VALID(pDoc);
	if (!pDoc)
		return;

	for (int i = 0; i < 9; i++)
	{
		string path = "img" + to_string(i) + ".dib";
		this->images[i].Load(CString(path.c_str()));
	}

	CDC* pMemDC = new CDC();
	pMemDC->CreateCompatibleDC(pDC);
	CBitmap bmpMem;
	bmpMem.CreateCompatibleBitmap(pDC, clientWidth, clientHeight);
	pMemDC->SelectObject(&bmpMem);

	this->DrawBackground(pDC, clientHeight, clientWidth);
	this->DrawGrid(pDC, clientHeight, clientWidth, 19);
	this->DrawPuzzlePieces(pDC);

	//pDC->BitBlt(0, 0, clientWidth, clientHeight, pMemDC, 0, 0, SRCCOPY);
	pMemDC->DeleteDC();
	delete pMemDC;
}

void CLab3View::DrawBackground(CDC* pDC, int clientWidth, int clientHeight)
{
	CPen pen(PS_NULL, 1, RGB(128, 128, 0));
	CPen* oldPen = pDC->SelectObject(&pen);
	CBrush* brush = new CBrush(RGB(250, 0, 0));
	//CBrush* oldBrush = pDC->SelectObject(brush);
	pDC->Rectangle(0, 0, clientWidth, clientHeight);
	pDC->SelectObject(oldPen);
	//pDC->SelectObject(oldBrush);
}

void CLab3View::DrawGrid(CDC* pDC, int clientWidth, int clientHeight, int numOfLines)
{
	CPen pen(PS_SOLID, 1, RGB(245, 240, 240));
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

void CLab3View::DrawCoordinateSystem(CDC* pDC)
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

void CLab3View::Translate(CDC* pDC, float dX, float dY, bool rightMultiply)
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

void CLab3View::Scale(CDC* pDC, float sX, float sY, bool rightMultiply)
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

void CLab3View::Rotate(CDC* pDC, float angle, bool rightMultiply)
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

void CLab3View::Mirror(CDC* pDC, bool mx, bool my, bool rightMultiply)
{
	XFORM form{};
	form.eM11 = mx ? -1 : 1;
	form.eM12 = 0;
	form.eM21 = 0;
	form.eM22 = my ? -1 : 1;
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

void CLab3View::DrawPuzzlePieces(CDC* pDC)
{
	int prevMode = pDC->SetGraphicsMode(GM_ADVANCED);
	XFORM form{};
	pDC->SetWorldTransform(&form);
	pDC->GetWorldTransform(&form);

	int w, h;
	int xOffset = 1 * cellWidth, yOffset = 1 * cellWidth;
	CBitmap bmpImage;

	w = this->images[0].Width();
	h = this->images[0].Height();

	// 0
	this->Rotate(pDC, 258 * M_PI / 180, true);
	this->Mirror(pDC, true, false, true);
	this->Translate(pDC, 180 + xOffset, 224 + yOffset, true);
	this->MakeTransparent(pDC, &this->images[6], 0);
	pDC->SetWorldTransform(&form);

	// 1
	this->Rotate(pDC, 58 * M_PI / 180, true);
	this->Mirror(pDC, true, false, true);
	this->Translate(pDC, 174 + xOffset, -104 + yOffset, true);
	this->MakeTransparent(pDC, &this->images[2], 1);
	pDC->SetWorldTransform(&form);

	// 2
	this->Rotate(pDC, 142 * M_PI / 180, true);
	this->Mirror(pDC, true, false, true);
	this->Translate(pDC, 192 + xOffset, 104 + yOffset, true);
	this->MakeTransparent(pDC, &this->images[1], 2);
	pDC->SetWorldTransform(&form);

	// 3
	this->Rotate(pDC, 132 * M_PI / 180, true);
	this->Mirror(pDC, true, false, true);
	this->Translate(pDC, -111 + xOffset, 222 + yOffset, true);
	this->MakeTransparent(pDC, &this->images[5], 3);
	pDC->SetWorldTransform(&form);

	// 4
	this->Rotate(pDC, 127 * M_PI / 180, true);
	this->Mirror(pDC, true, false, true);
	this->Translate(pDC, 40 + xOffset, 206 + yOffset, true);
	this->MakeTransparent(pDC, &this->images[4], 4);
	pDC->SetWorldTransform(&form);

	// 5
	this->Rotate(pDC, 107 * M_PI / 180, true);
	this->Mirror(pDC, true, false, true);
	this->Translate(pDC, 207 + xOffset, 145 + yOffset, true);
	this->MakeTransparent(pDC, &this->images[0], 5);
	pDC->SetWorldTransform(&form);

	// 6
	this->Rotate(pDC, 160 * M_PI / 180, true);
	this->Mirror(pDC, true, false, true);
	this->Translate(pDC, -90 + xOffset, 460 + yOffset, true);
	this->MakeTransparent(pDC, &this->images[7], 6);
	pDC->SetWorldTransform(&form);

	// 7
	this->Rotate(pDC, 82 * M_PI / 180, true);
	this->Mirror(pDC, true, false, true);
	this->Translate(pDC, 107 + xOffset, 232 + yOffset, true);
	this->MakeTransparent(pDC, &this->images[3], 7);
	pDC->SetWorldTransform(&form);

	// 8
	this->Rotate(pDC, -57 * M_PI / 180, true);
	this->Mirror(pDC, true, false, true);
	this->Translate(pDC, 557 + xOffset, 405 + yOffset, true);
	this->MakeTransparent(pDC, &this->images[8], 8);
	pDC->SetWorldTransform(&form);

	pDC->SetGraphicsMode(prevMode);
}

void CLab3View::MakeTransparent(CDC* pDC, DImage* bmpImage, int index)
{
	int w = bmpImage->Width();
	int h = bmpImage->Height();

	CBitmap* bmpFromImage = new CBitmap();
	bmpFromImage->CreateCompatibleBitmap(pDC, w, h);
	bmpFromImage->SetBitmapBits(w * h * bmpImage->BPP(), bmpImage->GetDIBBits());
	BITMAP bm;
	bmpFromImage->GetBitmap(&bm);

	CBitmap bmpMask;
	bmpMask.CreateBitmap(bm.bmWidth, bm.bmHeight, 1, 1, NULL);

	CDC SrcDC, DstDC, MemDC;
	SrcDC.CreateCompatibleDC(pDC); 
	DstDC.CreateCompatibleDC(pDC);  
	MemDC.CreateCompatibleDC(pDC);

	CBitmap* pOldSrcBmp = SrcDC.SelectObject(bmpFromImage);
	CBitmap* pOldDstBmp = DstDC.SelectObject(&bmpMask);

	COLORREF clrTopLeft = SrcDC.GetPixel(0, 0); 
	COLORREF clrSaveBk = SrcDC.SetBkColor(clrTopLeft);
	DstDC.BitBlt(0, 0, bm.bmWidth, bm.bmHeight, &SrcDC, 0, 0, SRCCOPY);

	COLORREF clrSaveDstText = SrcDC.SetTextColor(RGB(255, 255, 255)); 
	SrcDC.SetBkColor(RGB(0, 0, 0)); 
	SrcDC.BitBlt(0, 0, bm.bmWidth, bm.bmHeight, &DstDC, 0, 0, SRCAND);

	DstDC.SetTextColor(clrSaveDstText); 
	DstDC.SelectObject(pOldDstBmp); 
	SrcDC.SetBkColor(clrSaveBk); 
	SrcDC.SelectObject(pOldSrcBmp);
	SrcDC.DeleteDC();
	DstDC.DeleteDC();

	CBitmap* bmpOldT = MemDC.SelectObject(&bmpMask);
	pDC->BitBlt(0, 0, bm.bmWidth, bm.bmHeight, &MemDC, 0, 0, SRCAND);
	this->FilterBitmap(bmpFromImage, index, w * h * bmpImage->BPP());
	MemDC.SelectObject(bmpFromImage);
	pDC->BitBlt(0, 0, bm.bmWidth, bm.bmHeight, &MemDC, 0, 0, SRCPAINT);
	MemDC.SelectObject(bmpOldT); 
}

void CLab3View::FilterBitmap(CBitmap* bmpImage, int index, int size)
{
	unsigned char* buff = new unsigned char[size];
	bmpImage->GetBitmapBits(size, buff);

	if (index == 1)
	{
		for (int i = 0; i < size; i += 4)
		{
			int alpha = buff[i + 3];
			int r = buff[i + 2];
			int g = buff[i + 1];
			int b = buff[i];


			buff[i + 3] = alpha;
			buff[i + 2] = 0;
			buff[i + 1] = 0;
			buff[i] = b + 64;
		}
	}
	else
	{
		for (int i = 0; i < size; i += 4)
		{
			int alpha = buff[i + 3];
			int r = buff[i + 2];
			int g = buff[i + 1];
			int b = buff[i];

			//if (alpha > 0)
			{
				int gr = 64 + (r + g + b) / 3;
				if (gr > 255)
					gr = 255;

				buff[i + 3] = alpha;
				buff[i + 2] = gr;
				buff[i + 1] = gr;
				buff[i] = gr;
			}
		}
	}

	bmpImage->SetBitmapBits(size, buff);
	delete[] buff;
}


BOOL CLab3View::OnPreparePrinting(CPrintInfo* pInfo)
{
	return DoPreparePrinting(pInfo);
}

void CLab3View::OnBeginPrinting(CDC* /*pDC*/, CPrintInfo* /*pInfo*/)
{
}

void CLab3View::OnEndPrinting(CDC* /*pDC*/, CPrintInfo* /*pInfo*/)
{
}


#ifdef _DEBUG
void CLab3View::AssertValid() const
{
	CView::AssertValid();
}

void CLab3View::Dump(CDumpContext& dc) const
{
	CView::Dump(dc);
}

CLab3Doc* CLab3View::GetDocument() const
{
	ASSERT(m_pDocument->IsKindOf(RUNTIME_CLASS(CLab3Doc)));
	return (CLab3Doc*)m_pDocument;
}
#endif



BOOL CLab3View::OnEraseBkgnd(CDC* pDC)
{
	// TODO: Add your message handler code here and/or call default

	return TRUE;
}
