#include "pch.h"
#include "CGLRenderer.h"
#include <Windows.h>
#include <gl/GL.h>
#include <gl/GLU.h>

#define PI 3.14159265358979323846


bool CGLRenderer::CreateGLContext(CDC* pDC)
{
	PIXELFORMATDESCRIPTOR pfd;
	memset(&pfd, 0, sizeof(PIXELFORMATDESCRIPTOR));
	pfd.nSize = sizeof(PIXELFORMATDESCRIPTOR);
	pfd.nVersion = 1;
	pfd.dwFlags = LPD_DOUBLEBUFFER | PFD_SUPPORT_OPENGL | PFD_DRAW_TO_WINDOW;
	pfd.iPixelType = PFD_TYPE_RGBA;
	pfd.cColorBits = 32;
	pfd.cDepthBits = 32;
	pfd.iLayerType = PFD_MAIN_PLANE;

	int nPixelFormat = ChoosePixelFormat(pDC->m_hDC, &pfd);

	if (nPixelFormat == 0) return false;

	BOOL bResult = SetPixelFormat(pDC->m_hDC, nPixelFormat, &pfd);

	if (!bResult) return false;

	m_hRC = wglCreateContext(pDC->m_hDC);

	if (!m_hRC) return false;

	return true;
}

void CGLRenderer::PrepareScene(CDC* pDC)
{
	wglMakeCurrent(pDC->m_hDC, m_hRC);

	float red = 128.0f / 255.0f;
	float green = 192.0f / 255.0f;
	float blue = 255.0f / 255.0f;

	glClearColor(0.f, 0.8f, 0.f, 1.f);
	glEnable(GL_DEPTH_TEST);

	wglMakeCurrent(NULL, NULL);
}

void CGLRenderer::DestroyScene(CDC* pDC)
{
	wglMakeCurrent(pDC->m_hDC, m_hRC);

	wglMakeCurrent(NULL, NULL);
	if (m_hRC) 
	{
		wglDeleteContext(m_hRC);
		m_hRC = NULL;
	}
}

void CGLRenderer::Reshape(CDC* pDC, int w, int h)
{
	wglMakeCurrent(pDC->m_hDC, m_hRC);

	glViewport(0, 0, (GLsizei)w, (GLsizei)h);

	glMatrixMode(GL_PROJECTION);
	glLoadIdentity();

	gluPerspective(40, (float)w / (float)h, 0.1, 100);

	glMatrixMode(GL_MODELVIEW);

	wglMakeCurrent(NULL, NULL);
}

void CGLRenderer::DrawScene(CDC* pDC)
{
	wglMakeCurrent(pDC->m_hDC, m_hRC);

	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	glLoadIdentity();

	glTranslatef(0.0f, -1.5f, -6.0f);
	glRotatef(xRot + 10, 1.0f, 0.0f, 0.0f);
	glRotatef(yRot + 270, 0.0f, 1.0f, 0.0f);
	glRotatef(zRot, 0.0f, 0.0f, 1.0f);

	DrawFigure(0);
	DrawAxis(5);
	DrawGrid(3, 3, 10, 10);

	glFlush();

	SwapBuffers(pDC->m_hDC);
	wglMakeCurrent(NULL, NULL);
}

void CGLRenderer::DrawSphere(double r, int nSegAlpha, int nSegBeta)
{
	int alpha = 180 / nSegAlpha;
	int beta = 360 / nSegBeta;

	for (int i = -180; i < 180; i += alpha)
	{
		glBegin(GL_QUAD_STRIP);
		for (int j = 0; j <= 360; j += beta)
		{
			double x1 = r * cos(i * PI / 180) * cos(j * PI / 180);
			double y1 = r * sin(i * PI / 180);
			double z1 = r * cos(i * PI / 180) * sin(j * PI / 180);
			double x2 = r * cos((i + alpha) * PI / 180) * cos(j * PI / 180);
			double y2 = r * sin((i + alpha) * PI / 180);
			double z2 = r * cos((i + alpha) * PI / 180) * sin(j * PI / 180);
			glVertex3f(x2, y2, z2);
			glVertex3f(x1, y1, z1);
		}
		glEnd();
	}
}

void CGLRenderer::DrawCylinder(double h, double r1, double r2, int nSeg)
{
	int alpha = 360 / nSeg;

	// bottom circle
	glBegin(GL_TRIANGLE_FAN);
	for (int i = 360; i >= 0; i -= alpha)
	{
		double x = r1 * cos(i * PI / 180);
		double z = r1 * sin(i * PI / 180);
		glVertex3f(x, 0, z);
	}
	glEnd();

	// upper circle
	glBegin(GL_TRIANGLE_FAN);
	for (int i = 0; i <= 360; i += alpha)
	{
		double x = r2 * cos(i * PI / 180);
		double z = r2 * sin(i * PI / 180);
		glVertex3f(x, h, z);
	}
	glEnd();

	// side
	glBegin(GL_QUAD_STRIP);
	for (int i = 0; i <= 360; i += alpha)
	{
		double x1 = r1 * cos(i * PI / 180);
		double z1 = r1 * sin(i * PI / 180);
		double x2 = r2 * cos(i * PI / 180);
		double z2 = r2 * sin(i * PI / 180);
		glVertex3f(x2, h, z2);
		glVertex3f(x1, 0, z1);
	}
	glEnd();
}

void CGLRenderer::DrawCone(double h, double r, int nSeg)
{
	int alpha = 360 / nSeg;

	// circle
	glBegin(GL_TRIANGLE_FAN);
	for (int i = 360; i >= 0; i -= alpha)
	{
		double x = r * cos(i * PI / 180);
		double z = r * sin(i * PI / 180);
		glVertex3f(x, 0, z);
	}
	glEnd();

	// side
	glBegin(GL_QUAD_STRIP);
	for (int i = 0; i <= 360; i += alpha)
	{
		double x = r * cos(i * PI / 180);
		double z = r * sin(i * PI / 180);
		glVertex3f(0, h, 0);
		glVertex3f(x, 0, z);
	}
	glEnd();
}

void CGLRenderer::DrawAxis(double width)
{
	glBegin(GL_LINES);
	glColor3f(1.0f, 0.0f, 0.0f);
	glVertex3f(0, 0, 0.0f);
	glVertex3f(width, 0, 0.0f);
	glColor3f(0.0f, 1.0f, 0.0f);
	glVertex3f(0, 0, 0.0f);
	glVertex3f(0, width, 0.0f);
	glColor3f(0.0f, 0.0f, 1.0f);
	glVertex3f(0, 0, 0);
	glVertex3f(0, 0, width);
	glEnd();
}

void CGLRenderer::DrawGrid(double width, double height, int nSegW, int nSegH)
{
	double stepW = width / nSegW;
	double stepH = height / nSegH;
	glTranslatef(-width / 2, 0, -height / 2);

	glBegin(GL_LINES);
	SetColor(255, 255, 255);
	for (int i = 0; i < nSegH; i++) 
	{
		for (int j = 0; j <= nSegW; j++)
		{
			glVertex3f(j * stepW, 0, i * stepH);  //v1
			glVertex3f(j * stepW, 0, i * stepH + stepH);  //v2
		}
	}
	for (int i = 0; i < nSegH; i++)
	{
		for (int j = 0; j <= nSegW; j++)
		{
			glVertex3f(i * stepW, 0, j * stepH);  //v1
			glVertex3f(i * stepW + stepW, 0, j * stepH);  //v2
		}
	}
	glEnd();

	glTranslatef(width / 2, 0, height / 2);
}

void CGLRenderer::DrawFigure(double angle)
{
	glPushMatrix();
	float sphereRadius = 0.1f;
	float cilinderRadius = 0.15f;
	float cilinderHeight = 0.45f;
	int lightGreen = 255;
	int darkGreen = 170;
	int segment = 12;

	// pot
	glColor3f(0.47, 0.00, 0.00);
	DrawCylinder(0.4, 0.3, 0.4, 6);
	glTranslatef(0, 0.4, 0);
	DrawCylinder(0.2, 0.5, 0.5, 6);

	// main body
	glTranslatef(0, 0.2, 0);
	glColor3f(1.00, 0.60, 0.00);
	DrawCylinder(cilinderHeight, cilinderRadius, cilinderRadius, segment);

	glTranslatef(0, cilinderHeight + sphereRadius, 0);
	glColor3f(0.50, 0.75, 1.00);
	DrawSphere(sphereRadius, segment, segment);

	glPushMatrix();

	// right arm
	glColor3f(1.00, 0.60, 0.00);
	glRotatef(-45, 1, 0, 0);
	glTranslatef(0, sphereRadius, 0);
	DrawCone(cilinderHeight, cilinderRadius, segment);

	glTranslatef(0, cilinderHeight + sphereRadius, 0);
	glColor3f(0.50, 0.75, 1.00);
	DrawSphere(sphereRadius, segment, segment);

	glPopMatrix();

	// left arm
	glRotatef(45, 1, 0, 0);
	glTranslatef(0, sphereRadius, 0);
	glColor3f(1.00, 0.60, 0.00);
	DrawCylinder(cilinderHeight, cilinderRadius, cilinderRadius, segment);

	glTranslatef(0, cilinderHeight + sphereRadius, 0);
	glColor3f(0.50, 0.75, 1.00);
	DrawSphere(sphereRadius, segment, segment);

	glPushMatrix();
	glRotatef(45, 1, 0, 0);
	glTranslatef(0, sphereRadius, 0);
	glColor3f(1.00, 0.60, 0.00);
	DrawCone(cilinderHeight, cilinderRadius, segment);

	glTranslatef(0, cilinderHeight + sphereRadius, 0);
	glColor3f(0.50, 0.75, 1.00);
	DrawSphere(sphereRadius, segment, segment);

	glPushMatrix();

	glColor3f(1.00, 0.60, 0.00);
	glRotatef(45, 1, 0, 0);
	glTranslatef(0, sphereRadius, 0);
	DrawCone(cilinderHeight, cilinderRadius, segment);

	glTranslatef(0, cilinderHeight + sphereRadius, 0);
	glColor3f(0.50, 0.75, 1.00);
	DrawSphere(sphereRadius, segment, segment);

	glPopMatrix();

	glColor3f(1.00, 0.60, 0.00);
	glRotatef(-45, 1, 0, 0);
	glTranslatef(0, sphereRadius, 0);
	DrawCone(cilinderHeight, cilinderRadius, segment);

	glTranslatef(0, cilinderHeight + sphereRadius, 0);
	glColor3f(0.50, 0.75, 1.00);
	DrawSphere(sphereRadius, segment, segment);

	glPopMatrix();
	glPushMatrix();
	glColor3f(1.00, 0.60, 0.00);
	glRotatef(-45, 1, 0, 0);
	glTranslatef(0, sphereRadius, 0);
	DrawCone(cilinderHeight, cilinderRadius, segment);

	glTranslatef(0, cilinderHeight + sphereRadius, 0);
	glColor3f(0.50, 0.75, 1.00);
	DrawSphere(sphereRadius, segment, segment);

	glPopMatrix();
	glRotatef(alphaAngle, 1, 0, 0);
	glTranslatef(0, sphereRadius, 0);
	glColor3f(1.00, 1.00, 0.00);
	DrawCone(cilinderHeight, cilinderRadius, segment);

	glTranslatef(0, cilinderHeight + sphereRadius, 0);
	glColor3f(0.50, 0.75, 1.00);
	DrawSphere(sphereRadius, segment, segment);

	glPushMatrix();

	glRotatef(45, 1, 0, 0);
	glTranslatef(0, sphereRadius, 0);
	glColor3f(1.00, 0.60, 0.00);
	DrawCylinder(cilinderHeight, cilinderRadius, cilinderRadius, segment);
	glTranslatef(0, cilinderHeight + sphereRadius, 0);
	glColor3f(0.50, 0.75, 1.00);
	DrawSphere(sphereRadius, segment, segment);

	glPopMatrix();
	glPushMatrix();

	glTranslatef(0, sphereRadius, 0);
	glColor3f(1.00, 0.60, 0.00);
	DrawCylinder(cilinderHeight, cilinderRadius, cilinderRadius, segment);
	glTranslatef(0, cilinderHeight + sphereRadius, 0);
	glColor3f(0.50, 0.75, 1.00);
	DrawSphere(sphereRadius, segment, segment);

	glPopMatrix();
	glRotatef(-45, 1, 0, 0);
	glTranslatef(0, sphereRadius, 0);
	glColor3f(1.00, 0.60, 0.00);
	DrawCylinder(cilinderHeight, cilinderRadius, cilinderRadius, segment);
	glTranslatef(0, cilinderHeight + sphereRadius, 0);
	glColor3f(0.50, 0.75, 1.00);
	DrawSphere(sphereRadius, segment, segment);


	glPopMatrix();
}

void CGLRenderer::SetColor(double r, double g, double b)
{
	float red = r / 255.0f;
	float green = g / 255.0f;
	float blue = b / 255.0f;

	glColor3f(red, green, blue);
}
