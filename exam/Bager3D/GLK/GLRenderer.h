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
	void DrawPolygon(POINTF* points, POINTF* texCoords, int n);
	void DrawExtrudedPolygon(POINTF* points,POINTF* texCoords, int n, float zh, float r, float g, float b);
	void DrawBase();
	void DrawBody();
	void DrawArm(double zh);
	void DrawFork();
	void DrawExcavator();
	void DrawEnvCube(double a);

protected:
	HGLRC	 m_hrc; //OpenGL Rendering Context 
	double squares = 16;
	double scale = 1.5;
	unsigned int excavator, front, left, right, back, top, bot;

public:
	double xRot, yRot, zRot;
	double zTrans;
	double cabin, arm1, arm2, fork;
};
