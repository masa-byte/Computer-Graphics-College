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
	excavator, front, left, right, back, top, bot = 0;
	xRot = 18;
	yRot = zRot = 0;
	zTrans = -50.f;
	cabin = arm1 = arm2 = fork = 0;
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
	glPixelStorei(GL_UNPACK_ALIGNMENT, 4);
	glTexEnvf(GL_TEXTURE_ENV, GL_TEXTURE_ENV_MODE, GL_REPLACE);

	excavator = LoadTexture(CString("res/excavator.png"));
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
	DrawExcavator();

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

	gluPerspective(55, (float)w / (float)h, 0.1, 1000);

	glMatrixMode(GL_MODELVIEW);
	
	//---------------------------------
	wglMakeCurrent(NULL, NULL);
}

void CGLRenderer::DestroyScene(CDC *pDC)
{
	wglMakeCurrent(pDC->m_hDC, m_hrc);
	// ... 
	glDeleteTextures(1, &excavator);
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

void CGLRenderer::DrawPolygon(POINTF* points, POINTF* texCoords, int n)
{
	glBegin(GL_POLYGON);
	for (int i = 0; i < n; i++)
	{
		glTexCoord2f(texCoords[i].x, texCoords[i].y);
		glVertex2f(points[i].x, points[i].y);
	}
	glEnd();
}

void CGLRenderer::DrawExtrudedPolygon(POINTF* points, POINTF* texCoords, int n, float zh, float r, float g, float b)
{
	glPushMatrix();
	DrawPolygon(points, texCoords, n);

	glDisable(GL_TEXTURE_2D);
	glBegin(GL_QUAD_STRIP);
	glColor3f(r, g, b);
	for (int i = 0; i < n; i++)
	{
		glVertex3f(points[i].x, points[i].y, 0);
		glVertex3f(points[i].x, points[i].y, zh);
	}
	glEnd();
	glEnable(GL_TEXTURE_2D);

	glTranslatef(0, 0, zh);
	DrawPolygon(points, texCoords, n);
	glPopMatrix();
}

void CGLRenderer::DrawBase()
{
	glBindTexture(GL_TEXTURE_2D, excavator);
	POINTF points[8] = {
		{ 1 * scale, 0 },
		{ 0,  1 * scale},
		{ 0,  3 * scale},
		{ 1 * scale,  4 * scale},
		{ 15 * scale, 4 * scale },
		{ 16 * scale, 3 * scale },
		{ 16 * scale, 1 * scale },
		{ 15 * scale, 0 }
	};
	POINTF texCoords[8] = {
		{ 1 / squares, 16 / squares },
		{ 0, 15 / squares },
		{ 0, 13 / squares },
		{ 1 / squares, 12 / squares },
		{ 15 / squares, 12 / squares },
		{ 16 / squares, 13 / squares},
		{ 16 / squares, 15 / squares},
		{ 15 / squares, 16 / squares }
	};

	DrawExtrudedPolygon(points, texCoords, 8, 5, 0, 0, 0);
}

void CGLRenderer::DrawBody()
{
	glBindTexture(GL_TEXTURE_2D, excavator);
	POINTF points[5] = {
		{ 0, 0 },
		{ 0,  8 * scale},
		{ 4 * scale, 8 * scale },
		{ 8 * scale, 4 * scale },
		{ 8 * scale, 0 }
	};
	POINTF texCoords[5] = {
		{ 8 / squares, 8 / squares },
		{ 8 / squares, 0 },
		{ 12 / squares, 0 },
		{ 16 / squares, 4 / squares },
		{ 16 / squares, 8 / squares }
	};

	DrawExtrudedPolygon(points, texCoords, 5, 4, 0.96, 0.5, 0.12);
}

void CGLRenderer::DrawArm(double zh)
{
	glBindTexture(GL_TEXTURE_2D, excavator);
	POINTF points[6] = {
		{ 1 * scale, 0 },
		{ 0,  1 * scale},
		{ 0,  3 * scale},
		{ 1 * scale,  4 * scale},
		{ 16 * scale, 3 * scale },
		{ 16 * scale, 1 * scale },
	};
	POINTF texCoords[6] = {
		{ 1 / squares, 12 / squares },
		{ 0, 11 / squares },
		{ 0, 9 / squares },
		{ 1 / squares, 8 / squares },
		{ 16 / squares, 9 / squares},
		{ 16 / squares, 11 / squares},
	};

	DrawExtrudedPolygon(points, texCoords, 6, zh, 0.96, 0.5, 0.12);
}

void CGLRenderer::DrawFork()
{
	glBindTexture(GL_TEXTURE_2D, excavator);
	POINTF points[6] = {
		{ 0, 0 },
		{ 0,  5 * scale},
		{ 1 * scale,  6 * scale},
		{ 7 * scale,  6 * scale},
		{ 8 * scale, 5 * scale },
		{ 8 * scale, 0 * scale },
	};
	POINTF texCoords[6] = {
		{ 0, 6 / squares },
		{ 0, 1 / squares },
		{ 1 / squares, 0 },
		{ 7 / squares, 0 },
		{ 8 / squares, 1 / squares},
		{ 8 / squares, 6 / squares},
	};

	DrawExtrudedPolygon(points, texCoords, 6, 1.0, 0.7, 0.7, 0.7);
}

void CGLRenderer::DrawExcavator()
{
	glPushMatrix();
	glTranslatef(-8 * scale, 0, 0);
	DrawBase();

	glTranslatef(8 * scale, 4 * scale, 1.75 * scale);
	glRotatef(cabin, 0, 1, 0);
	glTranslatef(-4 * scale, 0, -1.25 * scale);
	DrawBody();

	glTranslatef(7 * scale, 2 * scale, 1.5);
	glRotatef(arm1, 0, 0, 1);
	glTranslatef(-2 * scale, -2 * scale, 0);
	DrawArm(1.0);

	glTranslatef(15 * scale, 2 * scale, 0);
	glRotatef(arm2, 0, 0, 1);
	glTranslatef(-2 * scale, -2 * scale, 0);
	DrawArm(1.5);

	glTranslatef(15 * scale, 2 * scale, 0);
	glRotatef(fork, 0, 0, 1);
	glTranslatef(-2 * scale, -2 * scale, 0);
	DrawFork();
	glPopMatrix();
}

void CGLRenderer::DrawEnvCube(double a)
{
	glEnable(GL_CULL_FACE);
	glCullFace(GL_BACK);
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
	glDisable(GL_CULL_FACE);
}
