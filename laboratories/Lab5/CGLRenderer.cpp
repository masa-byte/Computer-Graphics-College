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

	float red = 255.0f / 255.0f;
	float green = 255.0f / 255.0f;
	float blue = 255.0f / 255.0f;

	glClearColor(red, green, blue, 1.f);
	glEnable(GL_DEPTH_TEST);
	glEnable(GL_NORMALIZE);

	glLightModeli(GL_LIGHT_MODEL_LOCAL_VIEWER, GL_TRUE);
	glLightModeli(GL_LIGHT_MODEL_TWO_SIDE, GL_FALSE);

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

	glTranslatef(0.0f, -1.5f, -5.0f);
	glRotatef(xRot, 1.0f, 0.0f, 0.0f);
	glRotatef(yRot , 0.0f, 1.0f, 0.0f);
	glRotatef(zRot, 0.0f, 0.0f, 1.0f);

	DrawAxis(5);

	CreateLights();

	DrawRoom();
	DrawDisplay();

	glFlush();

	SwapBuffers(pDC->m_hDC);
	wglMakeCurrent(NULL, NULL);
}

void CGLRenderer::DrawHalfSphere(double r, int nSegAlpha, int nSegBeta, int* color)
{
	float matAmbient[] = { color[0] / 255., color[1] / 255., color[2] / 255., 1.0f };
	float matDiffuse[] = { 0.7f, 0.7f, 0.7f, 1.0f };
	float matSpecular[] = { color[0] / 255., color[1] / 255., color[2] / 255., 1.0f };
	float matShininess = 8;

	SetMaterial(matAmbient, matDiffuse, matSpecular, matShininess, matAmbient, false);

	SetColor(color[0], color[1], color[2]);
	int alpha = 180 / nSegAlpha;
	int beta = 360 / nSegBeta;
	double factor = PI / 180;

	for (int i = 0; i < 180; i += alpha)
	{
		glBegin(GL_QUAD_STRIP);
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
			glVertex3f(x2, y2, z2);
			glNormal3f(nx1, ny1, nz1);
			glVertex3f(x1, y1, z1);
		}
		glEnd();
	}
}

void CGLRenderer::DrawCylinder(double h, double lowerR, double upperR, int nSeg, int* color)
{
	float matAmbient[] = { color[0] / 255., color[1] / 255., color[2] / 255., 1.0f };
	float matDiffuse[] = { 0.7f, 0.7f, 0.7f, 1.0f };
	float matSpecular[] = { color[0] / 255. - 0.2, color[1] / 255. - 0.2, color[2] / 255. - 0.2, 1.0f };
	float matShininess = 8;

	SetMaterial(matAmbient, matDiffuse, matSpecular, matShininess, matDiffuse, false);

	SetColor(color[0], color[1], color[2]);
	int alpha = 360 / nSeg;
	double factor = PI / 180;
	double angle = 0;
	double x, y, z, ny, nx, nz;

	double cosAngle = 0;
	double sinAngle = 0;
	double L = sqrt(pow(lowerR, 2) + pow(h, 2));
	double nr = lowerR / L;
	// quadsSide
	glBegin(GL_QUAD_STRIP);
	for (int i = 0; i <= 360; i += alpha)
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

	if (normalsShown)
	{
		glDisable(GL_LIGHTING);
		SetColor(0, 255, 0);
		glBegin(GL_LINES);
		for (int i = 0; i <= 360; i += alpha)
		{
			angle = i * factor;
			cosAngle = cos(angle);
			sinAngle = sin(angle);
			double x1 = lowerR * cosAngle;
			double z1 = lowerR * sinAngle;
			double x2 = upperR * cosAngle;
			double z2 = upperR * sinAngle;

			double nx = cosAngle * nr;
			double nz = sinAngle * nr;
			double ny = h / L;
			if (lowerR == upperR)
				ny = 0;

			glVertex3f(x2, h, z2);
			glVertex3f(x2 + nx * normalsWidth, h + ny * normalsWidth, z2 + nz * normalsWidth);
			glVertex3f(x1, 0, z1);
			glVertex3f(x1 + nx * normalsWidth, 0 + ny * normalsWidth, z1 + nz * normalsWidth);
		}
		glEnd();
		SetColor(color[0], color[1], color[2]);
		glEnable(GL_LIGHTING);
	}
}

void CGLRenderer::DrawBaseCylinder(double h, double lowerR, double upperR, int nSeg, int* color)
{
	SetColor(color[0], color[1], color[2]);
	int alpha = 360 / nSeg;
	double factor = PI / 180;
	double angle = 0;
	double x, y, z, ny, nx, nz;

	// bottom circle
	glBegin(GL_TRIANGLE_FAN);
	for (int i = 360; i >= 0; i -= alpha)
	{
		angle = i * factor;
		x = lowerR * cos(angle);
		z = lowerR * sin(angle);
		ny = -1;
		glNormal3f(0, ny, 0);
		glVertex3f(x, 0, z);
	}
	glEnd();

	// upper circle
	if (upperR != 0)
	{
		glBegin(GL_TRIANGLE_FAN);
		for (int i = 0; i <= 360; i += alpha)
		{
			angle = i * factor;
			x = upperR * cos(angle);
			z = upperR * sin(angle);
			ny = 1;
			glNormal3f(0, ny, 0);
			glVertex3f(x, h, z);
		}
		glEnd();
	}	

	double cosAngle = 0;
	double sinAngle = 0;
	double L = sqrt(pow(lowerR, 2) + pow(h, 2));
	double nr = lowerR / L;

	// quadsSide
	glBegin(GL_QUAD_STRIP);
	for (int i = 0; i <= 360; i += alpha)
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
}

void CGLRenderer::DrawCube(double a, double b, double c, int* colors)
{
	SetColor(colors[0], colors[1], colors[2]);

	// front quadsSide
	glBegin(GL_QUADS);
	double nz = 1;
	glNormal3d(0, 0, nz);
	glVertex3f(-a / 2, c, b / 2);
	glVertex3f(-a / 2, 0, b / 2);
	glVertex3f(a / 2, 0, b / 2);
	glVertex3f(a / 2, c, b / 2);
	glEnd();

	glRotated(90, 0, 1, 0);
	// right quadsSide
	glBegin(GL_QUADS);
	glNormal3d(0, 0, nz);
	glVertex3f(-b / 2, c, a / 2);
	glVertex3f(-b / 2, 0, a / 2);
	glVertex3f(b / 2, 0, a / 2);
	glVertex3f(b / 2, c, a / 2);
	glEnd();

	glRotated(90, 0, 1, 0);
	// back quadsSide
	glBegin(GL_QUADS);
	glNormal3d(0, 0, nz);
	glVertex3f(-a / 2, c, b / 2);
	glVertex3f(-a / 2, 0, b / 2);
	glVertex3f(a / 2, 0, b / 2);
	glVertex3f(a / 2, c, b / 2);
	glEnd();

	glRotated(90, 0, 1, 0);
	// left quadsSide
	glBegin(GL_QUADS);
	glNormal3d(0, 0, nz);
	glVertex3f(-b / 2, c, a / 2);
	glVertex3f(-b / 2, 0, a / 2);
	glVertex3f(b / 2, 0, a / 2);
	glVertex3f(b / 2, c, a / 2);
	glEnd();

	// top quadsSide
	glBegin(GL_QUADS);
	double ny = 1;
	glNormal3d(0, ny, 0);
	glVertex3f(-a / 2, c, b / 2);
	glVertex3f(a / 2, c, b / 2);
	glVertex3f(a / 2, c, -b / 2);
	glVertex3f(-a / 2, c, -b / 2);
	glEnd();

	// bottom quadsSide
	glBegin(GL_QUADS);
	ny = -1;
	glNormal3d(0, ny, 0);
	glVertex3f(-a / 2, 0, b / 2);
	glVertex3f(-a / 2, 0, -b / 2);
	glVertex3f(a / 2, 0, -b / 2);
	glVertex3f(a / 2, 0, b / 2);
	glEnd();
}

void CGLRenderer::DrawAxis(double width)
{
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
}

void CGLRenderer::DrawDisplay()
{
	glPushMatrix();
	float matAmbient[] = { 0.9f, 0.9f, 0.9f, 1.0f };
	float matDiffuse[] = { 0.8f, 0.8f, 0.8f, 1.0f };
	float matSpecular[] = { 1.0f, 1.0f, 1.0f, 1.0f };
	float matShininess = 8;

	int colors[3] = { 220, 220, 220 };

	SetMaterial(matAmbient, matDiffuse, matSpecular, matShininess, matDiffuse, false);

	DrawHalfSphere(0.5, 15, 15, colors);
	glTranslatef(0, 0.4, 0);
	DrawBaseCylinder(0.5, 0.25, 0.25, 10, colors);
	glTranslatef(0, 0.5, 0);
	DrawCube(1.2, 1.2, 0.25, colors);
	glTranslatef(0, 0.25, 0);
	DrawVase();
	glPopMatrix();
}

void CGLRenderer::DrawVase()
{
	double ratio = 0.12;
	double height = 0.13;
	double segmentNum = 20;
	int yellowColor[3] = { 255, 255, 0 };
	int redColor[3] = { 255, 0, 0 };
	DrawCylinder(height, 0.3 + ratio, 0.3, segmentNum, yellowColor);
	glTranslatef(0, height, 0);
	DrawCylinder(height, 0.3, 0.3 - ratio, segmentNum, redColor);
	glTranslatef(0, height, 0);
	DrawCylinder(height, 0.3 - ratio, 0.3 - ratio, segmentNum, yellowColor);
	glTranslatef(0, height, 0);
	DrawCylinder(height, 0.3 - ratio, 0.3 - ratio, segmentNum, redColor);
	glTranslatef(0, 2 * height, 0);
	glRotated(180, 1, 0, 0);
	DrawCylinder(height, 0.2 + ratio, 0.3 - ratio, segmentNum, yellowColor);
	glRotated(-180, 1, 0, 0);
	DrawCylinder(height, 0.2 + ratio, 0.3 - ratio, segmentNum, redColor);
	glTranslatef(0, 2 * height, 0);
	glRotated(180, 1, 0, 0);
	DrawCylinder(height, 0.2 + ratio, 0.3 - ratio, segmentNum, yellowColor);
	glRotated(-180, 1, 0, 0);
	DrawCylinder(height, 0.2 + ratio, 0.3 - ratio, segmentNum, redColor);
	glTranslatef(0, 2 * height, 0);
	glRotated(180, 1, 0, 0);
	DrawCylinder(height, 0.2 + ratio, 0.3 - ratio, segmentNum, yellowColor);
	glTranslatef(0, -height, 0);
	DrawCylinder(height, 0.35 + ratio, 0.2 + ratio, segmentNum, redColor);
	glRotated(-180, 1, 0, 0);
	DrawCylinder(height, 0.35 + ratio, 0.2 + ratio, segmentNum, yellowColor);
	glTranslatef(0, height, 0);
	DrawCylinder(height, 0.2 + ratio, 0.3 - ratio, segmentNum, redColor);
	glTranslatef(0, 2 * height, 0);
	glRotated(180, 1, 0, 0);
	DrawCylinder(height, 0.2 + ratio, 0.3 - ratio, segmentNum, yellowColor);
	glRotated(-180, 1, 0, 0);
	DrawCylinder(height, 0.2 + ratio, 0.3 - ratio, segmentNum, redColor);
}

void CGLRenderer::DrawRoom()
{
	float matAmbient[] = { 0.8f, 0.8f, 0.8f, 1.0f };
	float matDiffuse[] = { 0.5f, 0.5f, 0.5f, 1.0f };
	float matSpecular[] = { 1.0f, 1.0f, 1.0f, 1.0f };
	float matShininess = 8;

	SetMaterial(matAmbient, matDiffuse, matSpecular, matShininess, matDiffuse, false);

	glPushMatrix();
	int colors[3] = { 200, 200, 200 };

	SetColor(colors[0], colors[1], colors[2]);
	glPushMatrix();
	// right wall
	glTranslatef((-quadsPerWall / 2) * quadsSide, -quadsSide, (-quadsPerWall / 2) * quadsSide);
	for (int j = 0; j < quadsPerWall - 1; j++)
	{
		glTranslatef(0, quadsSide, 0);
		glBegin(GL_QUAD_STRIP);
		glNormal3f(0, 0, 1);
		for (int i = 0; i < quadsPerWall; i++)
		{
			glVertex3f(i * quadsSide, 0, 0);
			glVertex3f(i * quadsSide, quadsSide, 0);
		}
		glEnd();
	}

	glPopMatrix();
	glPushMatrix();
	// back wall
	glRotatef(90, 0, 1, 0);
	glTranslatef((-quadsPerWall / 2) * quadsSide, -quadsSide, (-quadsPerWall / 2) * quadsSide);
	for (int j = 0; j < quadsPerWall - 1; j++)
	{
		glTranslatef(0, quadsSide, 0);
		glBegin(GL_QUAD_STRIP);
		glNormal3f(0, 0, 1);
		for (int i = 0; i < quadsPerWall; i++)
		{
			glVertex3f(i * quadsSide, 0, 0);
			glVertex3f(i * quadsSide, quadsSide, 0);
		}
		glEnd();
	}

	glPopMatrix();
	glPushMatrix();
	// left wall
	glRotatef(180, 0, 1, 0);
	glTranslatef((-quadsPerWall / 2) * quadsSide, -quadsSide, (-quadsPerWall / 2) * quadsSide);
	for (int j = 0; j < quadsPerWall - 1; j++)
	{
		glTranslatef(0, quadsSide, 0);
		glBegin(GL_QUAD_STRIP);
		glNormal3f(0, 0, 1);
		for (int i = 0; i < quadsPerWall; i++)
		{
			glVertex3f(i * quadsSide, 0, 0);
			glVertex3f(i * quadsSide, quadsSide, 0);
		}
		glEnd();
	}
	
	glPopMatrix();
	glPushMatrix();
	// front wall
	glRotatef(270, 0, 1, 0);
	glTranslatef((-quadsPerWall / 2) * quadsSide, -quadsSide, (-quadsPerWall / 2) * quadsSide);
	for (int j = 0; j < quadsPerWall - 1; j++)
	{
		glTranslatef(0, quadsSide, 0);
		glBegin(GL_QUAD_STRIP);
		glNormal3f(0, 0, 1);
		for (int i = 0; i < quadsPerWall; i++)
		{
			glVertex3f(i * quadsSide, 0, 0);
			glVertex3f(i * quadsSide, quadsSide, 0);
		}
		glEnd();
	}

	glPopMatrix();
	glPushMatrix();
	// top wall
	glRotatef(90, 1, 0, 0);
	glTranslatef((-quadsPerWall / 2) * quadsSide, (-quadsPerWall / 2) * quadsSide - quadsSide, -quadsPerWall * quadsSide);
	for (int j = 0; j < quadsPerWall - 1; j++)
	{
		glTranslatef(0, quadsSide, 0);
		glBegin(GL_QUAD_STRIP);
		glNormal3f(0, 0, 1);
		for (int i = 0; i < quadsPerWall; i++)
		{
			glVertex3f(i * quadsSide, 0, 0);
			glVertex3f(i * quadsSide, quadsSide, 0);
		}
		glEnd();
	}

	glPopMatrix();
	// bottom wall
	glRotatef(270, 1, 0, 0);
	glTranslatef((-quadsPerWall / 2) * quadsSide, (-quadsPerWall / 2) * quadsSide - quadsSide, 0);
	for (int j = 0; j < quadsPerWall - 1; j++)
	{
		glTranslatef(0, quadsSide, 0);
		glBegin(GL_QUAD_STRIP);
		glNormal3f(0, 0, 1);
		for (int i = 0; i < quadsPerWall; i++)
		{
			glVertex3f(i * quadsSide, 0, 0);
			glVertex3f(i * quadsSide, quadsSide, 0);
		}
		glEnd();
	}
	
	glPopMatrix();
}

void CGLRenderer::SetColor(double r, double g, double b)
{
	float red = r / 255.0f;
	float green = g / 255.0f;
	float blue = b / 255.0f;

	glColor3f(red, green, blue);
}

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

void CGLRenderer::SetLight1(float ambient[], float diffuse[], float specular[], float position[])
{
	glLightfv(GL_LIGHT1, GL_AMBIENT, ambient);
	glLightfv(GL_LIGHT1, GL_DIFFUSE, diffuse);
	glLightfv(GL_LIGHT1, GL_POSITION, position);
	glLightfv(GL_LIGHT1, GL_SPECULAR, specular);
}

void CGLRenderer::SetLight2(float ambient[], float diffuse[], float specular[], float position[])
{
	glLightfv(GL_LIGHT2, GL_AMBIENT, ambient);
	glLightfv(GL_LIGHT2, GL_DIFFUSE, diffuse);
	glLightfv(GL_LIGHT2, GL_POSITION, position);
	glLightfv(GL_LIGHT2, GL_SPECULAR, specular);
}

void CGLRenderer::SetLight3(float ambient[], float diffuse[], float specular[], float position[])
{
	glLightfv(GL_LIGHT3, GL_AMBIENT, ambient);
	glLightfv(GL_LIGHT3, GL_DIFFUSE, diffuse);
	glLightfv(GL_LIGHT3, GL_POSITION, position);
	glLightfv(GL_LIGHT3, GL_SPECULAR, specular);
}

void CGLRenderer::CreateLights()
{
	glEnable(GL_LIGHTING);
	glPushMatrix();
	float ambientLight[] = { 0.2f, 0.2f, 0.2f, 1.0f };
	glLightModelfv(GL_LIGHT_MODEL_AMBIENT, ambientLight);

	float lightAmbient[] = { 0.2f, 0.2f, 0.2f, 1.0f };
	float lightDiffuse[] = { 0.5f, 0.5f, 0.5f, 0.5f };
	float lightSpecular[] = { 0.5f, 0.5f, 0.5f, 1.0f };
	float lightPosition[] = { 0.0f, 0.0f, 0.0f, 1.0f };
	float spot_direction[] = { -1.0f, 0.0f, 0.0f };

	// directional light
	lightPosition[1] = 0.5;
	lightPosition[2] = 1;
	lightPosition[3] = 0;
	glEnable(GL_LIGHT0);
	SetLight0(lightAmbient, lightDiffuse, lightSpecular, lightPosition);


	lightAmbient[0] = 0.0f;
	lightAmbient[1] = 0.0f;
	lightAmbient[2] = 0.0f;
	// blue light
	glPushMatrix();
	if (blueLightOn)
	{
		lightPosition[0] = (quadsPerWall / 2) * quadsSide;
		lightPosition[1] = (quadsPerWall / 2) * quadsSide;
		lightPosition[2] = 0;
		lightPosition[3] = 1;
		lightSpecular[0] = 0;
		lightSpecular[1] = 0;
		lightSpecular[2] = 1;
		SetLight1(lightAmbient, lightDiffuse, lightSpecular, lightPosition);
		glLightf(GL_LIGHT1, GL_SPOT_CUTOFF, 25.0);
		glLightfv(GL_LIGHT1, GL_SPOT_DIRECTION, spot_direction);
		glLightf(GL_LIGHT1, GL_SPOT_EXPONENT, 2.0);
		glEnable(GL_LIGHT1);

		glTranslated((quadsPerWall / 2) * quadsSide, (quadsPerWall / 2) * quadsSide, 0);
		glRotated(90, 0, 0, 1);
		DrawHalfSphere(0.5, 10, 10, new int[3] {0, 0, 255});
	}
	else
		glDisable(GL_LIGHT1);

	glPopMatrix();
	glPushMatrix();

	// red light
	if (redLightOn)
	{
		lightPosition[0] = -(quadsPerWall / 2) * quadsSide;
		lightPosition[1] = (quadsPerWall / 2) * quadsSide;
		lightPosition[2] = 0;
		lightPosition[3] = 1;
		lightSpecular[0] = 1;
		lightSpecular[1] = 0;
		lightSpecular[2] = 0;
		spot_direction[0] = 1;
		SetLight2(lightAmbient, lightDiffuse, lightSpecular, lightPosition);
		glLightf(GL_LIGHT2, GL_SPOT_CUTOFF, 25.0);
		glLightfv(GL_LIGHT2, GL_SPOT_DIRECTION, spot_direction);
		glLightf(GL_LIGHT2, GL_SPOT_EXPONENT, 2.0);
		glEnable(GL_LIGHT2);

		glTranslated(-(quadsPerWall / 2) * quadsSide, (quadsPerWall / 2) * quadsSide, 0);
		glRotated(270, 0, 0, 1);
		DrawHalfSphere(0.5, 10, 10, new int[3] {255, 0, 0});
	}
	else
		glDisable(GL_LIGHT2);

	glPopMatrix();

	// yellow light
	if (yellowLightOn)
	{
		lightPosition[0] = 0;
		lightPosition[1] = quadsPerWall * quadsSide;
		lightPosition[2] = 0;
		lightPosition[3] = 1;
		lightSpecular[0] = 1;
		lightSpecular[1] = 1;
		lightSpecular[2] = 0;
		spot_direction[0] = 0;
		spot_direction[1] = -1;
		spot_direction[2] = 0;
		SetLight3(lightAmbient, lightDiffuse, lightSpecular, lightPosition);
		glLightf(GL_LIGHT3, GL_SPOT_CUTOFF, 25.0);
		glLightfv(GL_LIGHT3, GL_SPOT_DIRECTION, spot_direction);
		glLightf(GL_LIGHT3, GL_SPOT_EXPONENT, 2.0);
		glEnable(GL_LIGHT3);

		glTranslated(0, quadsPerWall * quadsSide, 0);
		glRotated(180, 0, 0, 1);
		DrawHalfSphere(0.5, 10, 10, new int[3] {255, 255, 0});
	}
	else
		glDisable(GL_LIGHT3);

	glPopMatrix();
}
