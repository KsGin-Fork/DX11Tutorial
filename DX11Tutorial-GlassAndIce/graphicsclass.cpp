////////////////////////////////////////////////////////////////////////////////
// Filename: graphicsclass.cpp
////////////////////////////////////////////////////////////////////////////////
#include "graphicsclass.h"


GraphicsClass::GraphicsClass()
{
	m_D3D = 0;
	m_Camera = 0;
	m_TextureShader = 0;
	m_WindowModel = 0;
	m_GlassShader = 0;
	m_RenderTexture = 0;
}


GraphicsClass::GraphicsClass(const GraphicsClass& other)
{
}


GraphicsClass::~GraphicsClass()
{
}


bool GraphicsClass::Initialize(int screenWidth, int screenHeight, HWND hwnd)
{
	bool result;

		
	// Create the Direct3D object.
	m_D3D = new D3DClass;
	if(!m_D3D)
	{
		return false;
	}

	// Initialize the Direct3D object.
	result = m_D3D->Initialize(screenWidth, screenHeight, VSYNC_ENABLED, hwnd, FULL_SCREEN, SCREEN_DEPTH, SCREEN_NEAR);
	if(!result)
	{
		MessageBox(hwnd, "Could not initialize Direct3D.", "Error", MB_OK);
		return false;
	}

	// Create the camera object.
	m_Camera = new CameraClass;
	if(!m_Camera)
	{
		return false;
	}
	
	m_Model = new ModelClass;
	if (!m_Model) {
		return false;
	}
	char stone01_dds[] = "./data/seafloor.dds";
	char plane01_txt[] = "./data/cube.txt";
	char bump03_dds[] = "./data/icebump01.dds";
	result = m_Model->Initialize(m_D3D->GetDevice(), stone01_dds , plane01_txt , bump03_dds);
	if(!result)
	{
		MessageBox(hwnd, "Could not initialize the model object.", "Error", MB_OK);
		return false;
	}

	m_WindowModel = new ModelClass;
	if (!m_WindowModel) {
		return false;
	}
	char square_txt[] = "./data/square.txt";
	char glass01_dds[] = "./data/ice01.dds";
	result = m_WindowModel->Initialize(m_D3D->GetDevice(), glass01_dds ,square_txt , bump03_dds);
	if(!result)
	{
		MessageBox(hwnd, "Could not initialize the window model object.", "Error", MB_OK);
		return false;
	}

	m_RenderTexture = new RenderTextureClass;
	if (!m_RenderTexture) {
		return false;
	}
	result = m_RenderTexture->Initialize(m_D3D->GetDevice(), screenWidth, screenHeight);
	if (!result) {
		MessageBox(hwnd, "Could not initialize the render texture object.", "Error", MB_OK);
		return false;
	}

	m_TextureShader = new TextureShaderClass;
	if (!m_TextureShader) {
		return false;
	}

	result = m_TextureShader->Initialize(m_D3D->GetDevice(), hwnd);
	if(!result)
	{
		MessageBox(hwnd, "Could not initialize the texture Shader object.", "Error", MB_OK);
		return false;
	}

	m_GlassShader = new GlassShaderClass;
	if (!m_GlassShader) {
		return false;
	}

	result = m_GlassShader->Initialize(m_D3D->GetDevice(), hwnd);
	if (!result) {
		MessageBox(hwnd, "Could not initialize the glass Shader object.", "Error", MB_OK);
		return false;
	}
	return true;
}


void GraphicsClass::Shutdown()
{
	// Release the camera object.
	if(m_Camera)
	{
		delete m_Camera;
		m_Camera = 0;
	}

	// Release the D3D object.
	if(m_D3D)
	{
		m_D3D->Shutdown();
		delete m_D3D;
		m_D3D = 0;
	}

	if (m_Model) {
		m_Model->Shutdown();
		delete m_Model;
		m_Model = 0;
	}

	if (m_TextureShader) {
		m_TextureShader->Shutdown();
		delete m_TextureShader;
		m_TextureShader = 0;
	}

	if (m_GlassShader) {
		m_GlassShader->Shutdown();
		delete m_GlassShader;
		m_GlassShader = 0;
	}

	if (m_RenderTexture) {
		m_RenderTexture->Shutdown();
		delete m_RenderTexture;
		m_RenderTexture = 0;
	}

	if (m_WindowModel) {
		m_WindowModel->Shutdown();
		delete m_WindowModel;
		m_WindowModel = 0;
	}

	return;
}


bool GraphicsClass::Frame()
{
	static float rotation = 0.0f;
	bool result;

	rotation += (float)XM_PI * 0.005f;
	if (rotation > 360.0f) {
		rotation -= 360.0f;
	}
	// Set the position and rotation of the camera.
	m_Camera->SetPosition(0.0f, 0.0f, -10.0f);

	result = RenderToTexture(rotation);
	if (!result) {
		return false;
	}

	result = Render(rotation);
	if (!result) {
		return false;
	}
	return true;
}

bool GraphicsClass::RenderToTexture(float rotation) {
	bool result;
	XMMATRIX worldMatrix, viewMatrix, projectionMatrix;
	m_RenderTexture->SetRenderTarget(m_D3D->GetDeviceContext(), m_D3D->GetDepthStencilView());
	m_RenderTexture->ClearRenderTarget(m_D3D->GetDeviceContext(), m_D3D->GetDepthStencilView(),
		0.0f, 0.0f, 0.0f, 1.0f);
	m_Camera->Render();
	m_D3D->GetWorldMatrix(worldMatrix);
	m_D3D->GetProjectionMatrix(projectionMatrix);
	m_Camera->GetViewMatrix(viewMatrix);

	worldMatrix *= XMMatrixRotationY(rotation);

	m_Model->Render(m_D3D->GetDeviceContext());

	result = m_TextureShader->Render(m_D3D->GetDeviceContext(), 
		m_Model->GetIndexCount(), 
		worldMatrix, viewMatrix, projectionMatrix, 
		m_Model->GetTexture());
	if (!result) {
		return false;
	}

	m_D3D->SetBackBufferRenderTarget();
	return true;
}

bool GraphicsClass::Render(float rotation)
{
	bool result;
	XMMATRIX worldMatrix, viewMatrix, projectionMatrix;
	float refracionScale = 0.1f;

	m_D3D->BeginScene(0.0f, 0.0f, 0.0f, 1.0f);
	m_Camera->Render();

	m_D3D->GetWorldMatrix(worldMatrix);
	m_D3D->GetProjectionMatrix(projectionMatrix);
	m_Camera->GetViewMatrix(viewMatrix);

	worldMatrix *= XMMatrixRotationY(rotation);

	m_Model->Render(m_D3D->GetDeviceContext());
	result = m_TextureShader->Render(
		m_D3D->GetDeviceContext(), 
		m_Model->GetIndexCount(), 
		worldMatrix, 
		viewMatrix, 
		projectionMatrix, 
		m_Model->GetTexture());
	if (!result) {
		return false;
	}

	m_D3D->GetWorldMatrix(worldMatrix);
	worldMatrix *= XMMatrixTranslation(0.0f, 0.0f, -1.5f);

	m_WindowModel->Render(m_D3D->GetDeviceContext());
	result = m_GlassShader->Render(
		m_D3D->GetDeviceContext(),
		m_WindowModel->GetIndexCount(),
		worldMatrix,
		viewMatrix,
		projectionMatrix,
		m_WindowModel->GetTexture() , 
		m_WindowModel->GetNormalTexture(),
		m_RenderTexture->GetShaderResourceView(),
		refracionScale
	);
	if (!result) {
		return false;
	}


	m_D3D->EndScene();

	return true;
}
