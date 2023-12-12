

#include "pch.h"
#include "framework.h"
#ifndef SHARED_HANDLERS
#include "Lab4.h"
#endif

#include "Lab4Doc.h"
#include "Lab4View.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#endif


IMPLEMENT_DYNCREATE(CLab4View, CView)

BEGIN_MESSAGE_MAP(CLab4View, CView)
	ON_COMMAND(ID_FILE_PRINT, &CView::OnFilePrint)
	ON_COMMAND(ID_FILE_PRINT_DIRECT, &CView::OnFilePrint)
	ON_COMMAND(ID_FILE_PRINT_PREVIEW, &CView::OnFilePrintPreview)
	ON_WM_CREATE()
	ON_WM_SIZE()
	ON_WM_DESTROY()
	ON_WM_ERASEBKGND()
	ON_WM_KEYDOWN()
END_MESSAGE_MAP()

CLab4View::CLab4View() noexcept
{

}

CLab4View::~CLab4View()
{
}

BOOL CLab4View::PreCreateWindow(CREATESTRUCT& cs)
{
	return CView::PreCreateWindow(cs);
}


void CLab4View::OnDraw(CDC* pDC)
{
	CLab4Doc* pDoc = GetDocument();
	ASSERT_VALID(pDoc);
	if (!pDoc)
		return;

	// TODO: add draw code for native data here
	m_glRenderer.DrawScene(pDC);
}


BOOL CLab4View::OnPreparePrinting(CPrintInfo* pInfo)
{
	// default preparation
	return DoPreparePrinting(pInfo);
}

void CLab4View::OnBeginPrinting(CDC* /*pDC*/, CPrintInfo* /*pInfo*/)
{
	// TODO: add extra initialization before printing
}

void CLab4View::OnEndPrinting(CDC* /*pDC*/, CPrintInfo* /*pInfo*/)
{
	// TODO: add cleanup after printing
}


#ifdef _DEBUG
void CLab4View::AssertValid() const
{
	CView::AssertValid();
}

void CLab4View::Dump(CDumpContext& dc) const
{
	CView::Dump(dc);
}

CLab4Doc* CLab4View::GetDocument() const // non-debug version is inline
{
	ASSERT(m_pDocument->IsKindOf(RUNTIME_CLASS(CLab4Doc)));
	return (CLab4Doc*)m_pDocument;
}
#endif //_DEBUG



int CLab4View::OnCreate(LPCREATESTRUCT lpCreateStruct)
{
	if (CView::OnCreate(lpCreateStruct) == -1)
		return -1;

	// TODO:  Add your specialized creation code here
	CDC* pDC = GetDC();
	m_glRenderer.CreateGLContext(pDC);
	ReleaseDC(pDC);

	return 0;
}


void CLab4View::OnSize(UINT nType, int cx, int cy)
{
	CView::OnSize(nType, cx, cy);

	// TODO: Add your message handler code here
	CDC* pDC = GetDC();
	m_glRenderer.Reshape(pDC, cx, cy);
	ReleaseDC(pDC);
}


void CLab4View::OnDestroy()
{
	CView::OnDestroy();

	// TODO: Add your message handler code here
	CDC* pDC = GetDC();
	m_glRenderer.DestroyScene(pDC);
	ReleaseDC(pDC);
}


void CLab4View::OnInitialUpdate()
{
	CView::OnInitialUpdate();

	// TODO: Add your specialized code here and/or call the base class
	CDC* pDC = GetDC();
	m_glRenderer.PrepareScene(pDC);
	ReleaseDC(pDC);
}


BOOL CLab4View::OnEraseBkgnd(CDC* pDC)
{
	return true;
}


void CLab4View::OnKeyDown(UINT nChar, UINT nRepCnt, UINT nFlags)
{
	// TODO: Add your message handler code here and/or call default
	if (nChar == 'Q')
	{
		m_glRenderer.xRot += 2.0f;
	}
	else if (nChar == 'W')
	{
		m_glRenderer.xRot -= 2.0f;
	}
	else if (nChar == 'E')
	{
		m_glRenderer.yRot += 2.0f;
	}
	else if (nChar == 'R')
	{
		m_glRenderer.yRot -= 2.0f;
	}
	else if (nChar == 'T')
	{
		m_glRenderer.zRot += 2.0f;
	}
	else if (nChar == 'Y')
	{
		m_glRenderer.zRot -= 2.0f;
	}
	else if (nChar == VK_LEFT)
	{
		m_glRenderer.alphaAngle += 2.0f;
	}
	else if (nChar == VK_RIGHT)
	{
		m_glRenderer.alphaAngle -= 2.0f;
	}

	Invalidate();

	CView::OnKeyDown(nChar, nRepCnt, nFlags);
}
