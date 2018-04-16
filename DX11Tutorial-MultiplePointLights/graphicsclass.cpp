////////////////////////////////////////////////////////////////////////////////
// Filename: graphicsclass.cpp
////////////////////////////////////////////////////////////////////////////////
#include "graphicsclass.h"


GraphicsClass::GraphicsClass()
{
	m_D3D = 0;
	m_Camera = 0;
	m_Light1 = 0;
	m_Light2 = 0; 
	m_Light3 = 0;
	m_Light4 = 0;
	m_LightShader = 0;
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
	char stone01_dds[] = "./data/texture4.gif";
	char plane01_txt[] = "./data/cube.txt";
	result = m_Model->Initialize(m_D3D->GetDevice(), stone01_dds , plane01_txt);
	if(!result)
	{
		MessageBox(hwnd, "Could not initialize the model object.", "Error", MB_OK);
		return false;
	}

	// Create the light shader object.
	m_LightShader = new LightShaderClass;
	if(!m_LightShader)
	{
		return false;
	}

	// Initialize the light shader object.
	result = m_LightShader->Initialize(m_D3D->GetDevice(), hwnd);
	if(!result)
	{
		MessageBox(hwnd, "Could not initialize the light shader object.", "Error", MB_OK);
		return false;
	}

	m_Light1 = new LightClass;
	if (!m_Light1) {
		return false;
	}
	m_Light1->SetDiffuseColor(1.0f, 0.0f, 0.0f, 1.0f);
	m_Light1->SetPosition(-10.0f, 1.0f, 10.0f);

	m_Light2 = new LightClass;
	if (!m_Light2) {
		return false;
	}
	m_Light2->SetDiffuseColor(0.0f, 1.0f, 0.0f, 1.0f);
	m_Light2->SetPosition(10.0f, 1.0f, 10.0f);

	m_Light3 = new LightClass;
	if (!m_Light3) {
		return false;
	}
	m_Light3->SetDiffuseColor(0.0f, 0.0f, 1.0f, 1.0f);
	m_Light3->SetPosition(-10.0f, 1.0f, -10.0f);

	m_Light4 = new LightClass;
	if (!m_Light4) {
		return false;
	}
	m_Light4->SetDiffuseColor(1.0f, 1.0f, 1.0f, 1.0f);
	m_Light4->SetPosition(10.0f, 1.0f, -10.0f);

	return true;
}


void GraphicsClass::Shutdown()
{

	// Release the light shader object.
	if(m_LightShader)
	{
		m_LightShader->Shutdown();
		delete m_LightShader;
		m_LightShader = 0;
	}

	// Release the light object.
	if(m_Light1)
	{
		delete m_Light1;
		m_Light1 = 0;
	}

	// Release the light object.
	if(m_Light2)
	{
		delete m_Light2;
		m_Light2 = 0;
	}

	// Release the light object.
	if(m_Light3)
	{
		delete m_Light3;
		m_Light3 = 0;
	}
	// Release the light object.
	if(m_Light4)
	{
		delete m_Light4;
		m_Light4 = 0;
	}

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

	return;
}


bool GraphicsClass::Frame()
{

	bool result;

	// Set the position and rotation of the camera.
	m_Camera->SetPosition(0.0f, 10.0f, -50.0f);

	result = Render();
	if (!result) {
		return false;
	}
	return true;
}


bool GraphicsClass::Render()
{
	bool result;

	XMMATRIX worldMatrix, viewMatrix, projectionMatrix;
	XMFLOAT4 diffusecolor[4];
	XMFLOAT4 lightPosition[4];

	diffusecolor[0] = m_Light1->GetDiffuseColor();
	diffusecolor[1] = m_Light2->GetDiffuseColor();
	diffusecolor[2] = m_Light3->GetDiffuseColor();
	diffusecolor[3] = m_Light4->GetDiffuseColor();

	lightPosition[0] = m_Light1->GetPosition();
	lightPosition[1] = m_Light2->GetPosition();
	lightPosition[2] = m_Light3->GetPosition();
	lightPosition[3] = m_Light4->GetPosition();


	m_D3D->BeginScene(0.0f, 0.0f, 0.0f, 1.0f);
	m_Camera->Render();

	m_D3D->GetWorldMatrix(worldMatrix);
	m_D3D->GetProjectionMatrix(projectionMatrix);
	m_Camera->GetViewMatrix(viewMatrix);

	m_Model->Render(m_D3D->GetDeviceContext());

	worldMatrix *= XMMatrixScaling(10.0f , 0.1f , 10.0f);

	result = m_LightShader->Render(
		m_D3D->GetDeviceContext(),
		m_Model->GetIndexCount(),
		worldMatrix,
		viewMatrix,
		projectionMatrix,
		m_Model->GetTexture(),
		diffusecolor,
		lightPosition);
	if (!result) {
		return false;
	}

	m_D3D->EndScene();

	return true;
}
