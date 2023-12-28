

#include "pch.h"
#include "framework.h"
#ifndef SHARED_HANDLERS
#include "Lab6.h"
#endif

#include "Lab6Doc.h"
#include "Lab6View.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#endif


IMPLEMENT_DYNCREATE(CLab6View, CView)

BEGIN_MESSAGE_MAP(CLab6View, CView)
	ON_COMMAND(ID_FILE_PRINT, &CView::OnFilePrint)
	ON_COMMAND(ID_FILE_PRINT_DIRECT, &CView::OnFilePrint)
	ON_COMMAND(ID_FILE_PRINT_PREVIEW, &CView::OnFilePrintPreview)
	ON_WM_CREATE()
	ON_WM_SIZE()
	ON_WM_DESTROY()
	ON_WM_ERASEBKGND()
	ON_WM_KEYDOWN()
END_MESSAGE_MAP()

CLab6View::CLab6View() noexcept
{

}

CLab6View::~CLab6View()
{
}

BOOL CLab6View::PreCreateWindow(CREATESTRUCT& cs)
{
	return CView::PreCreateWindow(cs);
}


void CLab6View::OnDraw(CDC* pDC)
{
	CLab6Doc* pDoc = GetDocument();
	ASSERT_VALID(pDoc);
	if (!pDoc)
		return;

	// TODO: add draw code for native data here
	m_glRenderer.DrawScene(pDC);
}


BOOL CLab6View::OnPreparePrinting(CPrintInfo* pInfo)
{
	// default preparation
	return DoPreparePrinting(pInfo);
}

void CLab6View::OnBeginPrinting(CDC* /*pDC*/, CPrintInfo* /*pInfo*/)
{
	// TODO: add extra initialization before printing
}

void CLab6View::OnEndPrinting(CDC* /*pDC*/, CPrintInfo* /*pInfo*/)
{
	// TODO: add cleanup after printing
}


#ifdef _DEBUG
void CLab6View::AssertValid() const
{
	CView::AssertValid();
}

void CLab6View::Dump(CDumpContext& dc) const
{
	CView::Dump(dc);
}

CLab6Doc* CLab6View::GetDocument() const // non-debug version is inline
{
	ASSERT(m_pDocument->IsKindOf(RUNTIME_CLASS(CLab6Doc)));
	return (CLab6Doc*)m_pDocument;
}
#endif //_DEBUG



int CLab6View::OnCreate(LPCREATESTRUCT lpCreateStruct)
{
	if (CView::OnCreate(lpCreateStruct) == -1)
		return -1;

	// TODO:  Add your specialized creation code here
	CDC* pDC = GetDC();
	m_glRenderer.CreateGLContext(pDC);
	ReleaseDC(pDC);

	return 0;
}


void CLab6View::OnSize(UINT nType, int cx, int cy)
{
	CView::OnSize(nType, cx, cy);

	// TODO: Add your message handler code here
	CDC* pDC = GetDC();
	m_glRenderer.Reshape(pDC, cx, cy);
	ReleaseDC(pDC);
}


void CLab6View::OnDestroy()
{
	CView::OnDestroy();

	// TODO: Add your message handler code here
	CDC* pDC = GetDC();
	m_glRenderer.DestroyScene(pDC);
	ReleaseDC(pDC);
}


void CLab6View::OnInitialUpdate()
{
	CView::OnInitialUpdate();

	// TODO: Add your specialized code here and/or call the base class
	CDC* pDC = GetDC();
	m_glRenderer.PrepareScene(pDC);
	ReleaseDC(pDC);
}


BOOL CLab6View::OnEraseBkgnd(CDC* pDC)
{
	return true;
}


void CLab6View::OnKeyDown(UINT nChar, UINT nRepCnt, UINT nFlags)
{
	// TODO: Add your message handler code here and/or call default
	if (nChar == VK_UP)
	{
		m_glRenderer.xRot += 2.0f;
	}
	else if (nChar == VK_DOWN)
	{
		m_glRenderer.xRot -= 2.0f;
	}
	else if (nChar == VK_RIGHT)
	{
		m_glRenderer.yRot += 2.0f;
	}
	else if (nChar == VK_LEFT)
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

	Invalidate();

	CView::OnKeyDown(nChar, nRepCnt, nFlags);
}
