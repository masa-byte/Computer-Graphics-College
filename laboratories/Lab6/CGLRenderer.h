
#pragma once

class CGLRenderer
{
private:
	HGLRC m_hRC;
	unsigned int grassId = 0;
	unsigned int truckId = 0;
	double square = 0.5;
public:
	// added
	double xRot = 0, yRot = 0, zRot = 0;

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
	void DrawBottom(double side, int nSeg);
	void DrawTruck();
	void DrawTruckUpperBody();
	void DrawTruckLowerBody();
	void DrawTruckWheels();
	void SetColor(double r, double g, double b);

	// textures
	void LoadTextures();
	void SelectTexture(unsigned int id);
	void ReleaseTexture(unsigned int id);
	void PrepareTexturing(bool enableLighting);

	// lighting
	void SetMaterial(float ambient[], float diffuse[], float specular[], float shininess, float emission[], bool emmision);
	void SetLight0(float ambient[], float diffuse[], float specular[], float position[]);
	void CreateLight();
};

