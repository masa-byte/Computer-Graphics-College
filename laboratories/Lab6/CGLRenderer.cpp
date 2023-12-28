#include "pch.h"
#include "CGLRenderer.h"
#include <Windows.h>
#include <gl/GL.h>
#include <gl/GLU.h>
#include "DImage.h"

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
	float red = 230 / 255.0f;
	float green = 230 / 255.0f;
	float blue = 230 / 255.0f;

	glClearColor(red, green, blue, 1.f);
	glEnable(GL_DEPTH_TEST);
	glEnable(GL_TEXTURE_2D);
	glEnable(GL_LIGHTING);
	PrepareTexturing(false);
	LoadTextures();

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

	glTranslatef(0.0f, -1.5f, -8.0f);
	glRotatef(xRot + 10, 1.0f, 0.0f, 0.0f);
	glRotatef(yRot - 10, 0.0f, 1.0f, 0.0f);
	glRotatef(zRot, 0.0f, 0.0f, 1.0f);

	CreateLight();
	DrawAxis(5);
	DrawBottom(5, 2);
	DrawTruck();

	glFlush();

	SwapBuffers(pDC->m_hDC);
	wglMakeCurrent(NULL, NULL);
}

void CGLRenderer::DrawSphere(double r, int nSegAlpha, int nSegBeta)
{
	int color[3] = { 0, 0, 0 };
	float matAmbient[] = { color[0] / 255., color[1] / 255., color[2] / 255., 1.0f };
	float matDiffuse[] = { 0.7f, 0.7f, 0.7f, 1.0f };
	float matSpecular[] = { color[0] / 255., color[1] / 255., color[2] / 255., 1.0f };
	float matShininess = 8;

	SetMaterial(matAmbient, matDiffuse, matSpecular, matShininess, NULL, false);

	int alpha = 180 / nSegAlpha;
	int beta = 360 / nSegBeta;
	double factor = PI / 180;

	SelectTexture(truckId);

	int k1 = 360 / alpha;
	int k2 = 360 / beta;
	double step1 = 0.5 / k1;
	double step2 = 1. / k2;

	int counter1 = 0;
	for (int i = -180; i < 180; i += alpha)
	{
		glBegin(GL_QUAD_STRIP);
		int counter2 = 0;
		for (int j = 0; j <= 360; j += beta)
		{
			double x1 = r * cos(i * factor) * cos(j * factor);
			double y1 = r * sin(i * factor);
			double z1 = r * cos(i * factor) * sin(j * factor);

			double nx1 = r * cos(i * factor) * cos(j * factor);
			double ny1 = r * sin(i * factor);
			double nz1 = r * cos(i * factor) * sin(j * factor);

			double x2 = r * cos((i + alpha) * factor) * cos(j * factor);
			double y2 = r * sin((i + alpha) * factor);
			double z2 = r * cos((i + alpha) * factor) * sin(j * factor);

			double nx2 = r * cos((i + alpha / 2) * factor) * cos(j * factor);
			double ny2 = r * sin((i + alpha / 2) * factor);
			double nz2 = r * cos((i + alpha / 2) * factor) * sin(j * factor);

			glNormal3f(nx2, ny2, nz2);
			glTexCoord2f(1 - counter2 * step2, 1 - (counter1 + 1) * step1);
			glVertex3f(x2, y2, z2);
			glNormal3f(nx1, ny1, nz1);
			glTexCoord2f(1 - counter2 * step2, 1 - counter1 * step1);
			glVertex3f(x1, y1, z1);
			counter2++;
		}
		glEnd();
		counter1++;
	}
}

void CGLRenderer::DrawCylinder(double h, double lowerR, double upperR, int nSeg)
{
	int color[3] = { 220, 220, 200 };	
	float matAmbient[] = { color[0] / 255., color[1] / 255., color[2] / 255., 1.0f };
	float matDiffuse[] = { 0.7f, 0.7f, 0.7f, 1.0f };
	float matSpecular[] = { color[0] / 255. - 0.2, color[1] / 255. - 0.2, color[2] / 255. - 0.2, 1.0f };
	float matShininess = 8;

	SetMaterial(matAmbient, matDiffuse, matSpecular, matShininess, matDiffuse, false);

	int alpha = 360 / nSeg;
	double factor = PI / 180;
	double angle = 0;
	double x, y, z, ny, nx, nz;
	double wheelR = 1.5;

	// bottom circle
	SelectTexture(truckId);
	glBegin(GL_TRIANGLE_FAN);
	glNormal3f(0, -1, 0);
	for (int i = 360; i >= 0; i -= alpha)
	{
		angle = i * factor;
		x = lowerR * cos(angle);
		z = lowerR * sin(angle);
		glTexCoord2f(6. / 16 + wheelR * 1. / 16 * cos(angle), 1 - (14.5 / 16 + wheelR * 1. / 16 * sin(angle)));
		glVertex3f(x, 0, z);
	}
	glEnd();

	// upper circle
	if (upperR != 0)
	{
		glBegin(GL_TRIANGLE_FAN);
		glNormal3f(0, 1, 0);
		for (int i = 0; i <= 360; i += alpha)
		{
			angle = i * factor;
			x = upperR * cos(angle);
			z = upperR * sin(angle);
			glTexCoord2f(6. / 16 + 1.5 * 1. / 16 * cos(angle), 1 - (14.5 / 16 + 1.5 * 1. / 16 * sin(angle)));
			glVertex3f(x, h, z);
		}
		glEnd();
	}

	double cosAngle = 0;
	double sinAngle = 0;
	double L = sqrt(pow(lowerR, 2) + pow(h, 2));
	double nr = lowerR / L;

	// quadsSide
	glDisable(GL_TEXTURE_2D);
	glBegin(GL_QUAD_STRIP);
	for (int i = 0; i <= 360 + alpha; i += alpha)
	{
		angle = i * factor;
		cosAngle = cos(angle);
		sinAngle = sin(angle);
		double x1 = lowerR * cosAngle;
		double z1 = lowerR * sinAngle;
		double x2 = upperR * cosAngle;
		double z2 = upperR * sinAngle;

		double nx = cosAngle;
		double nz = sinAngle;
		double ny = 0;
		if (lowerR != upperR)
		{
			nx *= nr;
			ny = h / L;
			nz *= nr;
		}

		glNormal3f(nx, ny, nz);
		glVertex3f(x2, h, z2);
		glNormal3f(nx, ny, nz);
		glVertex3f(x1, 0, z1);
	}
	glEnd();
	glEnable(GL_TEXTURE_2D);
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
	glDisable(GL_TEXTURE_2D);
	glDisable(GL_LIGHTING);
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
	glEnable(GL_LIGHTING);
	glEnable(GL_TEXTURE_2D);
}

void CGLRenderer::DrawBottom(double side, int nSeg)
{
	glPushMatrix();
	glTranslatef(-nSeg * side / 2, 0, -nSeg * side / 2);
	SelectTexture(grassId);

	for (int i = 0; i < nSeg; i++)
	{
		for (int j = 0; j < nSeg; j++)
		{
			glBegin(GL_QUADS);
			glTexCoord2f(0, 0);
			glVertex3f(i * side, 0, j * side);
			glTexCoord2f(1, 0);
			glVertex3f((i + 1) * side, 0, j * side);
			glTexCoord2f(1, 1);
			glVertex3f((i + 1) * side, 0, (j + 1) * side);
			glTexCoord2f(0, 1);
			glVertex3f(i * side, 0, (j + 1) * side);
			glEnd();
		}
	}

	glPopMatrix();
}

void CGLRenderer::DrawTruck()
{
	SetColor(200, 200, 200);
	glPushMatrix();

	glTranslatef(-6.5 * square, 0, 2.5 * square);

	DrawTruckWheels();
	glTranslatef(0, 0.8 * square, 0);
	DrawTruckLowerBody();
	glTranslatef(0, 3 * square, 0);
	DrawTruckUpperBody();
	glScalef(1.3, 0.7, 1);
	glTranslatef(6.5 * square, 3.2 * square, -2.5 * square);
	DrawSphere(3.2 * square, 10, 10);
	glPopMatrix();
}

void CGLRenderer::DrawTruckUpperBody()
{
	double square = 0.5;
	int a = 2;
	int b = 4;
	int c = 5;
	SelectTexture(truckId);
	glPushMatrix();

	// green front
	glBegin(GL_POLYGON);
	glNormal3f(0, 0, 1);
	glTexCoord2f(0, 1 - 12./16);
	glVertex3f(0, 0, 0);
	glTexCoord2f(4./16, 1 - 12./16);
	glVertex3f(b * square, 0, 0);
	glTexCoord2f(4./16, 1 - 1);
	glVertex3f(b * square, b * square, 0);
	glTexCoord2f(2./16, 1 - 1);
	glVertex3f(a * square, b * square, 0);
	glTexCoord2f(0, 1 - 14./16);
	glVertex3f(0, a * square, 0);
	glEnd();

	// gray top
	glDisable(GL_TEXTURE_2D);
	glBegin(GL_QUADS);
	glNormal3f(0, 1, 0);
	glVertex3f(a * square, b * square, 0);
	glVertex3f(b * square, b * square, 0);
	glVertex3f(b * square, b * square, -c * square);
	glVertex3f(a * square, b * square, -c * square);
	glEnd();

	// gray front	
	glBegin(GL_QUADS);
	glNormal3f(-sqrt(2) / 2, sqrt(2) / 2, 0);
	glVertex3f(0, a * square, 0);
	glVertex3f(a * square, b * square, 0);
	glVertex3f(a * square, b * square, -c * square);
	glVertex3f(0, a * square, -c * square);
	glEnd();

	glBegin(GL_QUADS);
	glNormal3f(0, 0, -1);
	glVertex3f(0, 0, 0);
	glVertex3f(0, a * square, 0);
	glVertex3f(0, a * square, -c * square);
	glVertex3f(0, 0, -c * square);
	glEnd();

	// gray back
	glBegin(GL_QUADS);
	glNormal3f(0, 0, 1);
	glVertex3f(b * square, b * square, 0);
	glVertex3f(b * square, 0, 0);
	glVertex3f(b * square, 0, -c * square);
	glVertex3f(b * square, b * square, -c * square);
	glEnd();
	glEnable(GL_TEXTURE_2D);

	// green back
	glTranslatef(0, 0, -c * square);
	glBegin(GL_POLYGON);
	glNormal3f(0, 0, -1);
	glTexCoord2f(0, 1 - 12. / 16);
	glVertex3f(0, 0, 0);
	glTexCoord2f(4. / 16, 1 - 12. / 16);
	glVertex3f(b * square, 0, 0);
	glTexCoord2f(4. / 16, 1 - 1);
	glVertex3f(b * square, b * square, 0);
	glTexCoord2f(2. / 16, 1 - 1);
	glVertex3f(a * square, b * square, 0);
	glTexCoord2f(0, 1 - 14. / 16);
	glVertex3f(0, a * square, 0);
	glEnd();

	glPopMatrix();
}

void CGLRenderer::DrawTruckLowerBody()
{
	double square = 0.5;
	int a = 3;
	int b = 13;
	int c = 5;

	SelectTexture(truckId);
	glPushMatrix();

	// green front
	glBegin(GL_POLYGON);
	glNormal3f(0, 0, 1);
	glTexCoord2f(0, 1 - 9. / 16);
	glVertex3f(0, 0, 0);
	glTexCoord2f(1. / 16, 1 - 9. / 16);
	glVertex3f(1 * square, 0, 0);
	glTexCoord2f(1. / 16, 1 - 10./16);
	glVertex3f(1 * square, 1 * square, 0);
	glTexCoord2f(2. / 16, 1 - 11. / 16);
	glVertex3f(2 * square, 2 * square, 0);
	glTexCoord2f(2. / 16, 1 - 12. / 16);
	glVertex3f(2 * square, a * square, 0);
	glTexCoord2f(0, 1 - 12. / 16);
	glVertex3f(0, a * square, 0);
	glEnd();
	glBegin(GL_QUADS);
	glTexCoord2f(2. / 16, 1 - 11. / 16);
	glVertex3f(2 * square, 2 * square, 0);
	glTexCoord2f(3. / 16, 1 - 11. / 16);
	glVertex3f(3 * square, 2 * square, 0);
	glTexCoord2f(3. / 16, 1 - 12. / 16);
	glVertex3f(3 * square, a * square, 0);
	glTexCoord2f(2. / 16, 1 - 12. / 16);
	glVertex3f(2 * square, a * square, 0);
	glEnd();
	glPushMatrix();
	glScaled(-1, 1, 1);
	glTranslated(-5 * square, 0, 0);
	glBegin(GL_POLYGON);
	glTexCoord2f(5. / 16, 1 - 9. / 16);
	glVertex3f(0, 0, 0);
	glTexCoord2f(4. / 16, 1 - 9. / 16);
	glVertex3f(1 * square, 0, 0);
	glTexCoord2f(4. / 16, 1 - 10. / 16);
	glVertex3f(1 * square, 1 * square, 0);
	glTexCoord2f(3. / 16, 1 - 11. / 16);
	glVertex3f(2 * square, 2 * square, 0);
	glTexCoord2f(3. / 16, 1 - 12. / 16);
	glVertex3f(2 * square, a * square, 0);
	glTexCoord2f(5. / 16, 1 - 12. / 16);
	glVertex3f(0, a * square, 0);
	glEnd();
	
	glPopMatrix();
	glTranslated(5 * square, 0, 0);
	glBegin(GL_POLYGON);
	glTexCoord2f(5. / 16, 1 - 9. / 16);
	glVertex3f(0, 0, 0);
	glTexCoord2f(6. / 16, 1 - 9. / 16);
	glVertex3f(1 * square, 0, 0);
	glTexCoord2f(6. / 16, 1 - 10. / 16);
	glVertex3f(1 * square, 1 * square, 0);
	glTexCoord2f(7. / 16, 1 - 11. / 16);
	glVertex3f(2 * square, 2 * square, 0);
	glTexCoord2f(7. / 16, 1 - 12. / 16);
	glVertex3f(2 * square, a * square, 0);
	glTexCoord2f(5. / 16, 1 - 12. / 16);
	glVertex3f(0, a * square, 0);
	glEnd();
	glBegin(GL_QUADS);
	glTexCoord2f(7. / 16, 1 - 11. / 16);
	glVertex3f(2 * square, 2 * square, 0);
	glTexCoord2f(8. / 16, 1 - 11. / 16);
	glVertex3f(3 * square, 2 * square, 0);
	glTexCoord2f(8. / 16, 1 - 12. / 16);
	glVertex3f(3 * square, a * square, 0);
	glTexCoord2f(7. / 16, 1 - 12. / 16);
	glVertex3f(2 * square, a * square, 0);
	glEnd();
	glPushMatrix();
	glScaled(-1, 1, 1);
	glTranslated(-5 * square, 0, 0);
	glBegin(GL_POLYGON);
	glTexCoord2f(10. / 16, 1 - 9. / 16);
	glVertex3f(0, 0, 0);
	glTexCoord2f(9. / 16, 1 - 9. / 16);
	glVertex3f(1 * square, 0, 0);
	glTexCoord2f(9. / 16, 1 - 10. / 16);
	glVertex3f(1 * square, 1 * square, 0);
	glTexCoord2f(8. / 16, 1 - 11. / 16);
	glVertex3f(2 * square, 2 * square, 0);
	glTexCoord2f(8. / 16, 1 - 12. / 16);
	glVertex3f(2 * square, a * square, 0);
	glTexCoord2f(10. / 16, 1 - 12. / 16);
	glVertex3f(0, a * square, 0);
	glEnd();
	glPopMatrix();
	glTranslated(5 * square, 0, 0);
	glBegin(GL_QUADS);
	glTexCoord2f(10. / 16, 1 - 9. / 16);
	glVertex3f(0, 0, 0);
	glTexCoord2f(13. / 16, 1 - 9. / 16);
	glVertex3f(a * square, 0, 0);
	glTexCoord2f(13. / 16, 1 - 12. / 16);
	glVertex3f(a * square, a * square, 0);
	glTexCoord2f(10. / 16, 1 - 12. / 16);
	glVertex3f(0, a * square, 0);
	glEnd();

	glPopMatrix();
	glPushMatrix();

	glDisable(GL_TEXTURE_2D);
	// gray top
	glBegin(GL_QUADS);
	glNormal3f(0, 1, 0);
	glVertex3f(4 * square, a * square, 0);
	glVertex3f(b * square, a * square, 0);
	glVertex3f(b * square, a * square, -c * square);
	glVertex3f(4 * square, a * square, -c * square);
	glEnd();

	// gray front
	glBegin(GL_QUADS);
	glNormal3f(-1, 0, 0);
	glVertex3f(0, 0, 0);
	glVertex3f(0, a * square, 0);
	glVertex3f(0, a * square, -c * square);
	glVertex3f(0, 0, -c * square);
	glEnd();

	// gray back
	glBegin(GL_QUADS);
	glNormal3f(1, 0, 0);
	glVertex3f(b * square, a * square, 0);
	glVertex3f(b * square, 0, 0);
	glVertex3f(b * square, 0, -c * square);
	glVertex3f(b * square, a * square, -c * square);
	glEnd();

	// gray underneath wheels
	// wheel 1
	glBegin(GL_QUADS);
	glNormal3f(1, 0, 0);
	glVertex3f(1 * square, 1 * square, 0);
	glVertex3f(1 * square, 0, 0);
	glVertex3f(1 * square, 0, -c * square);
	glVertex3f(1 * square, 1 * square, -c * square);
	glEnd();

	glBegin(GL_QUADS);
	glNormal3f(sqrt(2) / 2, -sqrt(2) / 2, 0);
	glVertex3f(2 * square, 2 * square, 0);
	glVertex3f(1 * square, 1 * square, 0);
	glVertex3f(1 * square, 1 * square, -c * square);
	glVertex3f(2 * square, 2 * square, -c * square);
	glEnd();

	glBegin(GL_QUADS);
	glNormal3f(0, -1, 0);
	glVertex3f(2 * square, 2 * square, 0);
	glVertex3f(3 * square, 2 * square, 0);
	glVertex3f(3 * square, 2 * square, -c * square);
	glVertex3f(2 * square, 2 * square, -c * square);
	glEnd();

	glBegin(GL_QUADS);
	glNormal3f(-sqrt(2) / 2, -sqrt(2) / 2, 0);
	glVertex3f(3 * square, 2 * square, 0);
	glVertex3f(4 * square, 1 * square, 0);
	glVertex3f(4 * square, 1 * square, -c * square);
	glVertex3f(3 * square, 2 * square, -c * square);
	glEnd();

	glBegin(GL_QUADS);
	glNormal3f(-1, 0, 0);
	glVertex3f(4 * square, 1 * square, 0);
	glVertex3f(4 * square, 0, 0);
	glVertex3f(4 * square, 0, -c * square);
	glVertex3f(4 * square, 1 * square, -c * square);
	glEnd();

	// wheel 2
	glPushMatrix();
	glTranslatef(5 * square, 0, 0);
	glBegin(GL_QUADS);
	glNormal3f(1, 0, 0);
	glVertex3f(1 * square, 1 * square, 0);
	glVertex3f(1 * square, 0, 0);
	glVertex3f(1 * square, 0, -c * square);
	glVertex3f(1 * square, 1 * square, -c * square);
	glEnd();

	glBegin(GL_QUADS);
	glNormal3f(sqrt(2) / 2, -sqrt(2) / 2, 0);
	glVertex3f(2 * square, 2 * square, 0);
	glVertex3f(1 * square, 1 * square, 0);
	glVertex3f(1 * square, 1 * square, -c * square);
	glVertex3f(2 * square, 2 * square, -c * square);
	glEnd();

	glBegin(GL_QUADS);
	glNormal3f(0, -1, 0);
	glVertex3f(2 * square, 2 * square, 0);
	glVertex3f(3 * square, 2 * square, 0);
	glVertex3f(3 * square, 2 * square, -c * square);
	glVertex3f(2 * square, 2 * square, -c * square);
	glEnd();

	glBegin(GL_QUADS);
	glNormal3f(-sqrt(2) / 2, -sqrt(2) / 2, 0);
	glVertex3f(3 * square, 2 * square, 0);
	glVertex3f(4 * square, 1 * square, 0);
	glVertex3f(4 * square, 1 * square, -c * square);
	glVertex3f(3 * square, 2 * square, -c * square);
	glEnd();

	glBegin(GL_QUADS);
	glNormal3f(-1, 0, 0);
	glVertex3f(4 * square, 1 * square, 0);
	glVertex3f(4 * square, 0, 0);
	glVertex3f(4 * square, 0, -c * square);
	glVertex3f(4 * square, 1 * square, -c * square);
	glEnd();

	glPopMatrix();
	glEnable(GL_TEXTURE_2D);	

	// green back
	glTranslatef(0, 0, -c * square);
	glBegin(GL_POLYGON);
	glNormal3f(0, 0, -1);
	glTexCoord2f(0, 1 - 9. / 16);
	glVertex3f(0, 0, 0);
	glTexCoord2f(1. / 16, 1 - 9. / 16);
	glVertex3f(1 * square, 0, 0);
	glTexCoord2f(1. / 16, 1 - 10. / 16);
	glVertex3f(1 * square, 1 * square, 0);
	glTexCoord2f(2. / 16, 1 - 11. / 16);
	glVertex3f(2 * square, 2 * square, 0);
	glTexCoord2f(2. / 16, 1 - 12. / 16);
	glVertex3f(2 * square, a* square, 0);
	glTexCoord2f(0, 1 - 12. / 16);
	glVertex3f(0, a* square, 0);
	glEnd();
	glBegin(GL_QUADS);
	glTexCoord2f(2. / 16, 1 - 11. / 16);
	glVertex3f(2 * square, 2 * square, 0);
	glTexCoord2f(3. / 16, 1 - 11. / 16);
	glVertex3f(3 * square, 2 * square, 0);
	glTexCoord2f(3. / 16, 1 - 12. / 16);
	glVertex3f(3 * square, a* square, 0);
	glTexCoord2f(2. / 16, 1 - 12. / 16);
	glVertex3f(2 * square, a* square, 0);
	glEnd();
	glPushMatrix();
	glScaled(-1, 1, 1);
	glTranslated(-5 * square, 0, 0);
	glBegin(GL_POLYGON);
	glTexCoord2f(5. / 16, 1 - 9. / 16);
	glVertex3f(0, 0, 0);
	glTexCoord2f(4. / 16, 1 - 9. / 16);
	glVertex3f(1 * square, 0, 0);
	glTexCoord2f(4. / 16, 1 - 10. / 16);
	glVertex3f(1 * square, 1 * square, 0);
	glTexCoord2f(3. / 16, 1 - 11. / 16);
	glVertex3f(2 * square, 2 * square, 0);
	glTexCoord2f(3. / 16, 1 - 12. / 16);
	glVertex3f(2 * square, a* square, 0);
	glTexCoord2f(5. / 16, 1 - 12. / 16);
	glVertex3f(0, a* square, 0);
	glEnd();

	glPopMatrix();
	glTranslated(5 * square, 0, 0);
	glBegin(GL_POLYGON);
	glTexCoord2f(5. / 16, 1 - 9. / 16);
	glVertex3f(0, 0, 0);
	glTexCoord2f(6. / 16, 1 - 9. / 16);
	glVertex3f(1 * square, 0, 0);
	glTexCoord2f(6. / 16, 1 - 10. / 16);
	glVertex3f(1 * square, 1 * square, 0);
	glTexCoord2f(7. / 16, 1 - 11. / 16);
	glVertex3f(2 * square, 2 * square, 0);
	glTexCoord2f(7. / 16, 1 - 12. / 16);
	glVertex3f(2 * square, a* square, 0);
	glTexCoord2f(5. / 16, 1 - 12. / 16);
	glVertex3f(0, a* square, 0);
	glEnd();
	glBegin(GL_QUADS);
	glTexCoord2f(7. / 16, 1 - 11. / 16);
	glVertex3f(2 * square, 2 * square, 0);
	glTexCoord2f(8. / 16, 1 - 11. / 16);
	glVertex3f(3 * square, 2 * square, 0);
	glTexCoord2f(8. / 16, 1 - 12. / 16);
	glVertex3f(3 * square, a* square, 0);
	glTexCoord2f(7. / 16, 1 - 12. / 16);
	glVertex3f(2 * square, a* square, 0);
	glEnd();
	glPushMatrix();
	glScaled(-1, 1, 1);
	glTranslated(-5 * square, 0, 0);
	glBegin(GL_POLYGON);
	glTexCoord2f(10. / 16, 1 - 9. / 16);
	glVertex3f(0, 0, 0);
	glTexCoord2f(9. / 16, 1 - 9. / 16);
	glVertex3f(1 * square, 0, 0);
	glTexCoord2f(9. / 16, 1 - 10. / 16);
	glVertex3f(1 * square, 1 * square, 0);
	glTexCoord2f(8. / 16, 1 - 11. / 16);
	glVertex3f(2 * square, 2 * square, 0);
	glTexCoord2f(8. / 16, 1 - 12. / 16);
	glVertex3f(2 * square, a* square, 0);
	glTexCoord2f(10. / 16, 1 - 12. / 16);
	glVertex3f(0, a* square, 0);
	glEnd();
	glPopMatrix();
	glTranslated(5 * square, 0, 0);
	glBegin(GL_QUADS);
	glTexCoord2f(10. / 16, 1 - 9. / 16);
	glVertex3f(0, 0, 0);
	glTexCoord2f(13. / 16, 1 - 9. / 16);
	glVertex3f(a* square, 0, 0);
	glTexCoord2f(13. / 16, 1 - 12. / 16);
	glVertex3f(a* square, a* square, 0);
	glTexCoord2f(10. / 16, 1 - 12. / 16);
	glVertex3f(0, a* square, 0);
	glEnd();

	glPopMatrix();
}

void CGLRenderer::DrawTruckWheels()
{
	glPushMatrix();

	glRotatef(90, 1, 0, 0);
	glTranslatef(0, -0.7 * square, -1.3 * square);

	// wheel 1
	glTranslatef(2.5 * square, 0, 0);
	DrawCylinder(0.7 * square, 1.3 * square, 1.3 * square, 14);

	// wheel 3
	glTranslatef(5 * square, 0, 0);
	DrawCylinder(0.7 * square, 1.3 * square, 1.3 * square, 14);

	glRotatef(180, 0, 0, 1);
	// wheel 2
	glTranslatef(0, 3.7 * square, 0);
	DrawCylinder(0.7 * square, 1.3 * square, 1.3 * square, 14);

	// wheel 4
	glTranslatef(5 * square, 0, 0);
	DrawCylinder(0.7 * square, 1.3 * square, 1.3 * square, 14);

	glPopMatrix();
}

void CGLRenderer::SetColor(double r, double g, double b)
{
	float red = r / 255.0f;
	float green = g / 255.0f;
	float blue = b / 255.0f;

	glColor3f(red, green, blue);
}

// TEXTURES

void CGLRenderer::LoadTextures()
{
	if (grassId != 0)
	{
		ReleaseTexture(grassId);
	}

	glGenTextures(1, &grassId);
	glBindTexture(GL_TEXTURE_2D, grassId);

	DImage img;
	img.Load(CString("res/grass.jpg"));

	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);;
	gluBuild2DMipmaps(GL_TEXTURE_2D, GL_RGBA, img.Width(), img.Height(), GL_RGBA, GL_UNSIGNED_BYTE, img.GetDIBBits());

	if (truckId != 0)
	{
		ReleaseTexture(truckId);
	}

	glGenTextures(1, &truckId);
	glBindTexture(GL_TEXTURE_2D, truckId);

	img.Load(CString("res/texture1.png"));
	gluBuild2DMipmaps(GL_TEXTURE_2D, GL_RGBA, img.Width(), img.Height(), GL_RGBA, GL_UNSIGNED_BYTE, img.GetDIBBits());
}

void CGLRenderer::SelectTexture(unsigned int id)
{
	glBindTexture(GL_TEXTURE_2D, id);
}

void CGLRenderer::ReleaseTexture(unsigned int id)
{
	glDeleteTextures(1, &id);
}

void CGLRenderer::PrepareTexturing(bool enableLighting)
{
	glPixelStorei(GL_UNPACK_ALIGNMENT, 4);

	if (enableLighting)
	{
		glTexEnvf(GL_TEXTURE_ENV, GL_TEXTURE_ENV_MODE, GL_MODULATE);
	}
	else
	{
		glTexEnvf(GL_TEXTURE_ENV, GL_TEXTURE_ENV_MODE, GL_REPLACE);
	}
}

// LIGHTING
void CGLRenderer::SetMaterial(float ambient[], float diffuse[], float specular[], float shininess, float emission[], bool emmision)
{
	glMaterialfv(GL_FRONT, GL_AMBIENT, ambient);
	glMaterialfv(GL_FRONT, GL_DIFFUSE, diffuse);
	glMaterialfv(GL_FRONT, GL_SPECULAR, specular);
	glMaterialf(GL_FRONT, GL_SHININESS, shininess);
	if (emmision)
		glMaterialfv(GL_FRONT, GL_EMISSION, emission);
}

void CGLRenderer::SetLight0(float ambient[], float diffuse[], float specular[], float position[])
{
	glLightfv(GL_LIGHT0, GL_AMBIENT, ambient);
	glLightfv(GL_LIGHT0, GL_DIFFUSE, diffuse);
	glLightfv(GL_LIGHT0, GL_POSITION, position);
	glLightfv(GL_LIGHT0, GL_SPECULAR, specular);
}

void CGLRenderer::CreateLight()
{
	glPushMatrix();
	float ambientLight[] = { 0.2f, 0.2f, 0.2f, 1.0f };
	glLightModelfv(GL_LIGHT_MODEL_AMBIENT, ambientLight);

	float lightAmbient[] = { 0.2f, 0.2f, 0.2f, 1.0f };
	float lightDiffuse[] = { 0.5f, 0.5f, 0.5f, 0.5f };
	float lightSpecular[] = { 0.5f, 0.5f, 0.5f, 1.0f };
	float lightPosition[] = { 0.0f, 0.0f, 0.0f, 1.0f };
	float spot_direction[] = { -1.0f, 0.0f, 0.0f };

	// directional light
	lightPosition[1] = cos(60 * PI / 180);
	lightPosition[2] = sin(60 * PI / 180);
	lightPosition[3] = 0;
	glEnable(GL_LIGHT0);
	SetLight0(lightAmbient, lightDiffuse, lightSpecular, lightPosition);

	glPopMatrix();
}
