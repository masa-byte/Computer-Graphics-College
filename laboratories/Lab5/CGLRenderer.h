
#pragma once
class CGLRenderer
{
private:
	HGLRC m_hRC;

public:

	// added
	double xRot = 0, yRot = 0, zRot = 0;
	double normalsWidth = 0.2;
	int quadsPerWall = 21;
	double quadsSide = 0.5;
	bool normalsShown = false;
	bool blueLightOn = false;
	bool redLightOn = false;
	bool yellowLightOn = false;

	bool CreateGLContext(CDC* pDC);
	void PrepareScene(CDC* pDC);
	void DestroyScene(CDC* pDC);
	void Reshape(CDC* pDC, int w, int h);
	void DrawScene(CDC* pDC);

	// added
	void DrawHalfSphere(double r, int nSegAlpha, int nSegBeta, int* color);
	void DrawCylinder(double h, double lowerR, double upperR, int nSeg, int* color);
	void DrawBaseCylinder(double h, double r1, double r2, int nSeg, int* color);
	void DrawCube(double a, double b, double c, int* color);
	void DrawAxis(double width);
	void DrawDisplay();
	void DrawVase();
	void DrawRoom();
	void SetColor(double r, double g, double b);
	void SetMaterial(float ambient[], float diffuse[], float specular[], float shininess, float emission[], bool emmision);
	void SetLight0(float ambient[], float diffuse[], float specular[], float position[]);
	void SetLight1(float ambient[], float diffuse[], float specular[], float position[]);
	void SetLight2(float ambient[], float diffuse[], float specular[], float position[]);
	void SetLight3(float ambient[], float diffuse[], float specular[], float position[]);
	void CreateLights();
};

