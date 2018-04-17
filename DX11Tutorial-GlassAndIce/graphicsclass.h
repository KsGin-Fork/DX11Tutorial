////////////////////////////////////////////////////////////////////////////////
// Filename: graphicsclass.h
////////////////////////////////////////////////////////////////////////////////
#ifndef _GRAPHICSCLASS_H_
#define _GRAPHICSCLASS_H_
#include "../DX11Tutorial-ScreenFades/textureshaderclass.h"
#include "../DX11Tutorial-ScreenFades/rendertextureclass.h"
#include "glassShaderClass.h"


/////////////
// GLOBALS //
/////////////
const bool FULL_SCREEN = false;
const bool VSYNC_ENABLED = true;
const float SCREEN_DEPTH = 1000.0f;
const float SCREEN_NEAR = 0.1f;


///////////////////////
// MY CLASS INCLUDES //
///////////////////////
#include "d3dclass.h"
#include "cameraclass.h"
#include "modelclass.h"


////////////////////////////////////////////////////////////////////////////////
// Class name: GraphicsClass
////////////////////////////////////////////////////////////////////////////////
class GraphicsClass
{
public:
	GraphicsClass();
	GraphicsClass(const GraphicsClass&);
	~GraphicsClass();

	bool Initialize(int, int, HWND);
	void Shutdown();
	bool Frame();
	bool RenderToTexture(float);
	bool Render(float);

private:


private:
	D3DClass* m_D3D;
	CameraClass* m_Camera;

	ModelClass *m_Model;
	ModelClass *m_WindowModel;
	TextureShaderClass *m_TextureShader;
	RenderTextureClass *m_RenderTexture;
	GlassShaderClass *m_GlassShader;
};

#endif