#pragma once

class CGLRenderer
{
public:
	CGLRenderer(void);
	virtual ~CGLRenderer(void);
		
	bool CreateGLContext(CDC* pDC);			// kreira OpenGL Rendering Context
	void PrepareScene(CDC* pDC);			// inicijalizuje scenu,
	void Reshape(CDC* pDC, int w, int h);	// kod koji treba da se izvrsi svaki put kada se promeni velicina prozora ili pogleda i
	void DrawScene(CDC* pDC);				// iscrtava scenu
	void DestroyScene(CDC* pDC);			// dealocira resurse alocirane u drugim funkcijama ove klase

	void DrawAxis();
	UINT LoadTexture(CString fileName);
	void DrawTriangle(float d1, float d2, float rep, bool mirroed);
	void DrawShip();
	void DrawSpaceCube(double a);
	void DrawCubeSide(double a);
	void SetLight();

protected:
	HGLRC	 m_hrc; //OpenGL Rendering Context 
	unsigned int ship, front, left, right, back, top, bot;

public:
	double xRot, yRot, zRot;
	double zTrans;
	bool lightOn;
};
