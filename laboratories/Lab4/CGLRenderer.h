
#pragma once
class CGLRenderer
{
private:
	HGLRC m_hRC;

public:

	// added
	double xRot = 0, yRot = 0, zRot = 0;
	double alphaAngle = 0;

	bool CreateGLContext(CDC* pDC);
	void PrepareScene(CDC* pDC);
	void DestroyScene(CDC* pDC);
	void Reshape(CDC* pDC, int w, int h);
	void DrawScene(CDC* pDC);

	// added
	void DrawSphere(double r, int nSegAlpha, int nSegBeta);
	void DrawCylinder(double h, double r1, double r2, int nSeg);
	void DrawCone(double h, double r, int nSeg);;
	void DrawAxis(double width);
	void DrawGrid(double width, double height, int nSegW, int nSegH);
	void DrawFigure(double angle);
	void SetColor(double r, double g, double b);
};

