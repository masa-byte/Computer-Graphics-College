#include "StdAfx.h"
#include "GLRenderer.h"
#include "GL\gl.h"
#include "GL\glu.h"
#include "GL\glaux.h"
#include "GL\glut.h"
#include "DImage.h"
//#pragma comment(lib, "GL\\glut32.lib")

#define PI 3.14159265358979323

CGLRenderer::CGLRenderer(void)
{
	spider, front, left, right, back, top, bot = 0;
	xRot = 18;
	yRot = zRot = 0;
	zTrans = -50.f;
}

CGLRenderer::~CGLRenderer(void)
{
}

bool CGLRenderer::CreateGLContext(CDC* pDC)
{
	PIXELFORMATDESCRIPTOR pfd ;
   	memset(&pfd, 0, sizeof(PIXELFORMATDESCRIPTOR));
   	pfd.nSize  = sizeof(PIXELFORMATDESCRIPTOR);
   	pfd.nVersion   = 1; 
   	pfd.dwFlags    = PFD_DOUBLEBUFFER | PFD_SUPPORT_OPENGL | PFD_DRAW_TO_WINDOW;   
   	pfd.iPixelType = PFD_TYPE_RGBA; 
   	pfd.cColorBits = 32;
   	pfd.cDepthBits = 24; 
   	pfd.iLayerType = PFD_MAIN_PLANE;
	
	int nPixelFormat = ChoosePixelFormat(pDC->m_hDC, &pfd);
	
	if (nPixelFormat == 0) return false; 

	BOOL bResult = SetPixelFormat (pDC->m_hDC, nPixelFormat, &pfd);
  	
	if (!bResult) return false; 

   	m_hrc = wglCreateContext(pDC->m_hDC); 

	if (!m_hrc) return false; 

	return true;	
}

void CGLRenderer::PrepareScene(CDC *pDC)
{
	wglMakeCurrent(pDC->m_hDC, m_hrc);
	//---------------------------------
	float red = 200 / 255.;
	float green = 200 / 255.;
	float blue = 200 / 255.;

	glClearColor(red, green, blue, 1.0f);
	glEnable(GL_DEPTH_TEST);
	glEnable(GL_TEXTURE_2D);
	glEnable(GL_NORMALIZE);
	//glEnable(GL_CULL_FACE);
	glCullFace(GL_BACK);
	glPixelStorei(GL_UNPACK_ALIGNMENT, 4);
	glTexEnvf(GL_TEXTURE_ENV, GL_TEXTURE_ENV_MODE, GL_REPLACE);

	spider = LoadTexture(CString("res/spider.png"));
	front = LoadTexture(CString("res/front.jpg"));
	left = LoadTexture(CString("res/left.jpg"));
	right = LoadTexture(CString("res/right.jpg"));
	back = LoadTexture(CString("res/back.jpg"));
	top = LoadTexture(CString("res/top.jpg"));
	bot = LoadTexture(CString("res/bot.jpg"));
	
	//---------------------------------
	wglMakeCurrent(NULL, NULL);
}

void CGLRenderer::DrawScene(CDC *pDC)
{
	wglMakeCurrent(pDC->m_hDC, m_hrc);
	//---------------------------------

	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	glLoadIdentity();

	glTranslatef(0, 0, zTrans);
	glRotatef(xRot, 1, 0, 0);
	glRotatef(yRot, 0, 1, 0);
	glRotatef(zRot, 0, 0, 1);

	DrawAxis();
	DrawEnvCube(100);

	glTranslatef(0, 2, 0);
	DrawSpider();

	glFlush();

	SwapBuffers(pDC->m_hDC);
	//---------------------------------
	wglMakeCurrent(NULL, NULL);
}

void CGLRenderer::Reshape(CDC *pDC, int w, int h)
{
	wglMakeCurrent(pDC->m_hDC, m_hrc);
	//---------------------------------

	glViewport(0, 0, (GLsizei)w, (GLsizei)h);

	glMatrixMode(GL_PROJECTION);
	glLoadIdentity();

	gluPerspective(50, (float)w / (float)h, 0.1, 1000);

	glMatrixMode(GL_MODELVIEW);
	
	//---------------------------------
	wglMakeCurrent(NULL, NULL);
}

void CGLRenderer::DestroyScene(CDC *pDC)
{
	wglMakeCurrent(pDC->m_hDC, m_hrc);
	// ... 
	glDeleteTextures(1, &spider);
	glDeleteTextures(1, &front);
	glDeleteTextures(1, &left);
	glDeleteTextures(1, &right);
	glDeleteTextures(1, &back);
	glDeleteTextures(1, &top);
	glDeleteTextures(1, &bot);
	wglMakeCurrent(NULL,NULL); 
	if(m_hrc) 
	{
		wglDeleteContext(m_hrc);
		m_hrc = NULL;
	}
}

void CGLRenderer::DrawAxis()
{
	glDisable(GL_TEXTURE_2D);
	glBegin(GL_LINES);
	glColor3f(1, 0, 0);
	glVertex3f(0, 0, 0);
	glVertex3f(50, 0, 0);
	glColor3f(0, 1, 0);
	glVertex3f(0, 0, 0);
	glVertex3f(0, 50, 0);
	glColor3f(0, 0, 1);
	glVertex3f(0, 0, 0);
	glVertex3f(0, 0, 50);
	glEnd();
	glEnable(GL_TEXTURE_2D);
}

UINT CGLRenderer::LoadTexture(CString fileName)
{
	unsigned int id = 0;
	glGenTextures(1, &id);
	glBindTexture(GL_TEXTURE_2D, id);

	DImage img;
	img.Load(fileName);

	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);;
	gluBuild2DMipmaps(GL_TEXTURE_2D, GL_RGBA, img.Width(), img.Height(), GL_BGRA_EXT, GL_UNSIGNED_BYTE, img.GetDIBBits());

	return id;
}

void CGLRenderer::DrawEnvCube(double a)
{
	glPushMatrix();
	glColor3f(1, 0, 0);
	// front side
	glBindTexture(GL_TEXTURE_2D, front);
	glBegin(GL_QUADS);
	glNormal3f(0, 0, -1);
	glTexCoord2f(0, 1);
	glVertex3f(-a / 2, 0, a / 2);
	glTexCoord2f(0, 0);
	glVertex3f(-a / 2, a, a / 2);
	glTexCoord2f(1, 0);
	glVertex3f(a / 2, a, a / 2);
	glTexCoord2f(1, 1);
	glVertex3f(a / 2, 0, a / 2);
	glEnd();
	// right side 
	glBindTexture(GL_TEXTURE_2D, right);
	glRotatef(90, 0, 1, 0);
	glBegin(GL_QUADS);
	glNormal3f(0, 0, -1);
	glTexCoord2f(0, 1);
	glVertex3f(-a / 2, 0, a / 2);
	glTexCoord2f(0, 0);
	glVertex3f(-a / 2, a, a / 2);
	glTexCoord2f(1, 0);
	glVertex3f(a / 2, a, a / 2);
	glTexCoord2f(1, 1);
	glVertex3f(a / 2, 0, a / 2);
	glEnd();
	// back side
	glBindTexture(GL_TEXTURE_2D, back);
	glRotatef(90, 0, 1, 0);
	glBegin(GL_QUADS);
	glNormal3f(0, 0, -1);
	glTexCoord2f(0, 1);
	glVertex3f(-a / 2, 0, a / 2);
	glTexCoord2f(0, 0);
	glVertex3f(-a / 2, a, a / 2);
	glTexCoord2f(1, 0);
	glVertex3f(a / 2, a, a / 2);
	glTexCoord2f(1, 1);
	glVertex3f(a / 2, 0, a / 2);
	glEnd();
	// left side
	glBindTexture(GL_TEXTURE_2D, left);
	glRotatef(90, 0, 1, 0);
	glBegin(GL_QUADS);
	glNormal3f(0, 0, -1);
	glTexCoord2f(0, 1);
	glVertex3f(-a / 2, 0, a / 2);
	glTexCoord2f(0, 0);
	glVertex3f(-a / 2, a, a / 2);
	glTexCoord2f(1, 0);
	glVertex3f(a / 2, a, a / 2);
	glTexCoord2f(1, 1);
	glVertex3f(a / 2, 0, a / 2);
	glEnd();
	// bottom side
	glBindTexture(GL_TEXTURE_2D, bot);
	glBegin(GL_QUADS);
	glNormal3f(0, 1, 0);
	glTexCoord2f(0, 1);
	glVertex3f(-a / 2, 0, -a / 2);
	glTexCoord2f(0, 0);
	glVertex3f(-a / 2, 0, a / 2);
	glTexCoord2f(1, 0);
	glVertex3f(a / 2, 0, a / 2);
	glTexCoord2f(1, 1);
	glVertex3f(a / 2, 0, -a / 2);
	glEnd();
	// top side
	glBindTexture(GL_TEXTURE_2D, top);
	glBegin(GL_QUADS);
	glNormal3f(0, -1, 0);
	glTexCoord2f(0, 1);
	glVertex3f(-a / 2, a, a / 2);
	glTexCoord2f(0, 0);
	glVertex3f(-a / 2, a, -a / 2);
	glTexCoord2f(1, 0);
	glVertex3f(a / 2, a, -a / 2);
	glTexCoord2f(1, 1);
	glVertex3f(a / 2, a, a / 2);
	glEnd();
	glPopMatrix();
}

void CGLRenderer::DrawSphere(double r, int nSeg, double texU, double texV, double texR)
{
	int alpha = 180 / nSeg;
	int beta = 360 / (2 * nSeg);

	glBindTexture(GL_TEXTURE_2D, spider);

	for (int i = -90; i < 90; i += alpha)
	{
		glBegin(GL_QUAD_STRIP);
		for (int j = 0; j <= 360; j += beta)
		{
			double x1 = r * cos(i * PI / 180) * cos(j * PI / 180);
			double y1 = r * sin(i * PI / 180);
			double z1 = r * cos(i * PI / 180) * sin(j * PI / 180);
			double tx1 = texR * cos(i * PI / 180) * cos(j * PI / 180) + texU;
			double ty1 = texR * cos(i * PI / 180) * sin(j * PI / 180) + texV;

			double x2 = r * cos((i + alpha) * PI / 180) * cos(j * PI / 180);
			double y2 = r * sin((i + alpha) * PI / 180);
			double z2 = r * cos((i + alpha) * PI / 180) * sin(j * PI / 180);
			double tx2 = texR * cos((i + alpha) * PI / 180) * cos(j * PI / 180) + texU;
			double ty2 = texR * cos((i + alpha) * PI / 180) * sin(j * PI / 180) + texV;

			glTexCoord2f(tx2, ty2);
			glVertex3f(x2, y2, z2);
			glTexCoord2f(tx1, ty1);
			glVertex3f(x1, y1, z1);
		}
		glEnd();
	}
}

void CGLRenderer::DrawCone(double r, double h, int nSeg, double texU, double texV, double texR)
{
	glBindTexture(GL_TEXTURE_2D, spider);

	int beta = 360 / nSeg;
	glBegin(GL_TRIANGLE_FAN);
	for (int i = 0; i <= 360; i += beta)
	{
		double x = r * cos(i * PI / 180);
		double z = r * sin(i * PI / 180);
		glVertex3f(x, 0, z);
	}
	glEnd();

	glBegin(GL_TRIANGLE_FAN);
	glTexCoord2f(texU, texV);
	glVertex3f(0, h, 0);
	for (int j = 0; j <= 360; j += beta)
	{
		double x1 = r * cos(j * PI / 180);
		double z1 = r * sin(j * PI / 180);
		double tx1 = texR * x1 / r + texU;
		double ty1 = texR * z1 / r + texV;

		glTexCoord2f(tx1, ty1);
		glVertex3f(x1, 0, z1);
	}
	glEnd();
}

void CGLRenderer::DrawLegSegment(double r, double h, int nSeg)
{
	glPushMatrix();
	glTranslatef(0, r, 0);
	DrawSphere(r, 2 * nSeg, 0.25, 0.25, 0.24);
	DrawCone(r, h, nSeg, 0.75, 0.75, 0.24);
	glPopMatrix();
}

void CGLRenderer::DrawLeg()
{
	glPushMatrix();
	DrawLegSegment(1, 10, 5);
	glTranslatef(0, 11, 0);
	glRotatef(85, 1, 0, 0);
	DrawLegSegment(1, 15, 5);
	glPopMatrix();
}

void CGLRenderer::DrawSpiderBody()
{
	glPushMatrix();
	glScalef(1, 0.5, 1);
	DrawSphere(2, 10, 0.75, 0.25, 0.24);
	glTranslatef(4, 0, 0);
	DrawSphere(3, 10, 0.25, 0.25, 0.24);
	glTranslatef(6, 0, 0);
	glScalef(1, 1.5, 1);
	glScalef(1, 0.8, 1);
	DrawSphere(5, 10, 0.25, 0.25, 0.24);
	glPopMatrix();
}

void CGLRenderer::DrawSpider()
{
	glPushMatrix();
	DrawSpiderBody();
	glTranslatef(3.5, 0, 0);
	for (int i = 0; i < 4; i++) 
	{
		glPushMatrix();
		glRotatef(i * 30 - 45, 0.0, 1.0, 0.0);
		glRotatef(45, 1.0, 0.0, 0.0);
		DrawLeg();
		glPopMatrix();
	}
	glRotatef(180, 0, 1, 0);
	for (int i = 0; i < 4; i++)
	{
		glPushMatrix();
		glRotatef(i * 30 - 45, 0.0, 1.0, 0.0);
		glRotatef(45, 1.0, 0.0, 0.0);
		DrawLeg();
		glPopMatrix();
	}
	glPopMatrix();
}
