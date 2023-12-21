

#include "pch.h"
#include "framework.h"
#ifndef SHARED_HANDLERS
#include "Lab5.h"
#endif

#include "Lab5Doc.h"
#include "Lab5View.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#endif


IMPLEMENT_DYNCREATE(CLab5View, CView)

BEGIN_MESSAGE_MAP(CLab5View, CView)
	ON_COMMAND(ID_FILE_PRINT, &CView::OnFilePrint)
	ON_COMMAND(ID_FILE_PRINT_DIRECT, &CView::OnFilePrint)
	ON_COMMAND(ID_FILE_PRINT_PREVIEW, &CView::OnFilePrintPreview)
	ON_WM_CREATE()
	ON_WM_SIZE()
	ON_WM_DESTROY()
	ON_WM_ERASEBKGND()
	ON_WM_KEYDOWN()
END_MESSAGE_MAP()

CLab5View::CLab5View() noexcept
{

}

CLab5View::~CLab5View()
{
}

BOOL CLab5View::PreCreateWindow(CREATESTRUCT& cs)
{
	return CView::PreCreateWindow(cs);
}


void CLab5View::OnDraw(CDC* pDC)
{
	CLab5Doc* pDoc = GetDocument();
	ASSERT_VALID(pDoc);
	if (!pDoc)
		return;

	// TODO: add draw code for native data here
	m_glRenderer.DrawScene(pDC);
}


BOOL CLab5View::OnPreparePrinting(CPrintInfo* pInfo)
{
	// default preparation
	return DoPreparePrinting(pInfo);
}

void CLab5View::OnBeginPrinting(CDC* /*pDC*/, CPrintInfo* /*pInfo*/)
{
	// TODO: add extra initialization before printing
}

void CLab5View::OnEndPrinting(CDC* /*pDC*/, CPrintInfo* /*pInfo*/)
{
	// TODO: add cleanup after printing
}


#ifdef _DEBUG
void CLab5View::AssertValid() const
{
	CView::AssertValid();
}

void CLab5View::Dump(CDumpContext& dc) const
{
	CView::Dump(dc);
}

CLab5Doc* CLab5View::GetDocument() const // non-debug version is inline
{
	ASSERT(m_pDocument->IsKindOf(RUNTIME_CLASS(CLab5Doc)));
	return (CLab5Doc*)m_pDocument;
}
#endif //_DEBUG



int CLab5View::OnCreate(LPCREATESTRUCT lpCreateStruct)
{
	if (CView::OnCreate(lpCreateStruct) == -1)
		return -1;

	// TODO:  Add your specialized creation code here
	CDC* pDC = GetDC();
	m_glRenderer.CreateGLContext(pDC);
	ReleaseDC(pDC);

	return 0;
}


void CLab5View::OnSize(UINT nType, int cx, int cy)
{
	CView::OnSize(nType, cx, cy);

	// TODO: Add your message handler code here
	CDC* pDC = GetDC();
	m_glRenderer.Reshape(pDC, cx, cy);
	ReleaseDC(pDC);
}


void CLab5View::OnDestroy()
{
	CView::OnDestroy();

	// TODO: Add your message handler code here
	CDC* pDC = GetDC();
	m_glRenderer.DestroyScene(pDC);
	ReleaseDC(pDC);
}


void CLab5View::OnInitialUpdate()
{
	CView::OnInitialUpdate();

	// TODO: Add your specialized code here and/or call the base class
	CDC* pDC = GetDC();
	m_glRenderer.PrepareScene(pDC);
	ReleaseDC(pDC);
}


BOOL CLab5View::OnEraseBkgnd(CDC* pDC)
{
	return true;
}


void CLab5View::OnKeyDown(UINT nChar, UINT nRepCnt, UINT nFlags)
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
	else if (nChar == VK_SPACE)
	{
		m_glRenderer.normalsShown = !m_glRenderer.normalsShown;
	}
	else if (nChar == '1')
	{
		m_glRenderer.blueLightOn = !m_glRenderer.blueLightOn;
	}
	else if (nChar == '2')
	{
		m_glRenderer.redLightOn = !m_glRenderer.redLightOn;
	}
	else if (nChar == '3')
	{
		m_glRenderer.yellowLightOn = !m_glRenderer.yellowLightOn;
	}

	Invalidate();

	CView::OnKeyDown(nChar, nRepCnt, nFlags);
}
