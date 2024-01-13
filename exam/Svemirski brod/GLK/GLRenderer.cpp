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
	ship, front, left, right, back, top, bot = 0;
	xRot = 18;
	yRot = zRot = 0;
	zTrans = -10.f;
	lightOn = true;
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
	glEnable(GL_CULL_FACE);
	glCullFace(GL_BACK);
	glPixelStorei(GL_UNPACK_ALIGNMENT, 4);
	glTexEnvf(GL_TEXTURE_ENV, GL_TEXTURE_ENV_MODE, GL_REPLACE);

	ship = LoadTexture(CString("res/ShipT1.png"));
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
	DrawSpaceCube(50);

	SetLight();
	glTranslatef(-2.5, 0.5, 0);
	DrawShip();

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
	glDeleteTextures(1, &ship);
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
	glDisable(GL_LIGHTING);
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
	glEnable(GL_LIGHTING);
	glEnable(GL_TEXTURE_2D);
}

UINT CGLRenderer::LoadTexture(CString fileName)
{
	unsigned int id = 0;
	glGenTextures(1, &id);
	glBindTexture(GL_TEXTURE_2D, id);

	DImage img;
	img.Load(fileName);

	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);;
	gluBuild2DMipmaps(GL_TEXTURE_2D, GL_RGBA, img.Width(), img.Height(), GL_BGRA_EXT, GL_UNSIGNED_BYTE, img.GetDIBBits());

	return id;
}

void CGLRenderer::DrawTriangle(float d1, float d2, float rep, bool mirrored)
{
	double c = sqrt(d1 * d1 + d2 * d2);
	double sinAlpha = d1 / c;
	double cosAlpha = d2 / c;
	int z = 0;
	mirrored == true ? z = -1 : z = 1;
	glBegin(GL_TRIANGLES);

	glTexCoord2f(0.5 * rep, c * rep);
	glNormal3f(0, 0, z);
	glVertex3f(0, 0, 0);

	glTexCoord2f((0.5 + sinAlpha * d2) * rep, cosAlpha * d2 * rep);
	glVertex3f(d1, 0, 0);

	glTexCoord2f(0.5 * rep, 0 * rep);
	glVertex3f(d1, d2, 0);

	glEnd();
}

void CGLRenderer::DrawShip()
{
	glDisable(GL_CULL_FACE);
	glBindTexture(GL_TEXTURE_2D, ship);
	glPushMatrix();
	glRotatef(90, 1, 0, 0);
	glRotatef(-4.75, 0, 1, 0);

	// bottom side
	glPushMatrix();
	glRotatef(-13, 1, 0, 0);
	DrawTriangle(5.8, 2.15, 1, true);
	glPopMatrix();

	glScalef(1, -1, 1);
	glRotatef(-13, 1, 0, 0);
	DrawTriangle(5.8, 2.15, 1, false);
	glPopMatrix();

	// top side
	glPushMatrix();
	glRotatef(90, 1, 0, 0);
	glRotatef(9.4, 0, 1, 0);
	glRotatef(-4.75, 0, 1, 0);

	glPushMatrix();
	glRotatef(13, 1, 0, 0);
	DrawTriangle(5.8, 2.15, 1, true);
	glPopMatrix();

	glScalef(1, -1, 1);
	glRotatef(13, 1, 0, 0);
	DrawTriangle(5.8, 2.15, 1, false);

	glPopMatrix();

	glEnable(GL_CULL_FACE);
}

void CGLRenderer::DrawSpaceCube(double a)
{
	glTexEnvf(GL_TEXTURE_ENV, GL_TEXTURE_ENV_MODE, GL_REPLACE);
	// back side
	glBindTexture(GL_TEXTURE_2D, back);
	glPushMatrix();
	DrawCubeSide(a);
	// left side
	glBindTexture(GL_TEXTURE_2D, left);
	glRotatef(90, 0, 1, 0);
	DrawCubeSide(a);
	// front side
	glBindTexture(GL_TEXTURE_2D, front);
	glRotatef(90, 0, 1, 0);
	DrawCubeSide(a);
	// right side
	glBindTexture(GL_TEXTURE_2D, right);
	glRotatef(90, 0, 1, 0);
	DrawCubeSide(a);
	// top side
	glBindTexture(GL_TEXTURE_2D, top);
	glRotatef(90, 1, 0, 0);
	DrawCubeSide(a);
	// bottom side
	glBindTexture(GL_TEXTURE_2D, bot);
	glRotatef(-180, 1, 0, 0);
	DrawCubeSide(a);
	glPopMatrix();
	glTexEnvf(GL_TEXTURE_ENV, GL_TEXTURE_ENV_MODE, GL_MODULATE);
}

void CGLRenderer::DrawCubeSide(double a)
{
	glBegin(GL_QUADS);
	glTexCoord2f(1, 1);
	glVertex3f(-a / 2, a / 2, -a / 2);
	glTexCoord2f(1, 0);
	glVertex3f(-a / 2, -a / 2, -a / 2);
	glTexCoord2f(0, 0);
	glVertex3f(a / 2, -a / 2, -a / 2);
	glTexCoord2f(0, 1);
	glVertex3f(a / 2, a / 2, -a / 2);
	glEnd();
}

void CGLRenderer::SetLight()
{
	if (lightOn)
	{
		glEnable(GL_LIGHTING);
		glTexEnvf(GL_TEXTURE_ENV, GL_TEXTURE_ENV_MODE, GL_MODULATE);
		float ambient[4] = { 0.2f, 0.2f, 0.2f, 1.0f };
		float diffuse[] = { 0.8f, 0.8f, 0.8f, 0.8f };
		float specular[4] = { 1.0f, 1.0f, 1.0f, 1.0f };
		float position[4] = { 0, 0, 1.0f, 0 };
		//glLightModelfv(GL_LIGHT_MODEL_AMBIENT, ambient);

		glEnable(GL_LIGHT0);
		glLightfv(GL_LIGHT0, GL_AMBIENT, ambient);
		glLightfv(GL_LIGHT0, GL_DIFFUSE, diffuse);
		glLightfv(GL_LIGHT0, GL_POSITION, position);
		glLightfv(GL_LIGHT0, GL_SPECULAR, specular);
	}
	else 
	{
		glDisable(GL_LIGHTING);
		glTexEnvf(GL_TEXTURE_ENV, GL_TEXTURE_ENV_MODE, GL_REPLACE);
	}

}
