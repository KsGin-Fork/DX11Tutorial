////////////////////////////////////////////////////////////////////////////////
// Filename: graphicsclass.cpp
////////////////////////////////////////////////////////////////////////////////
#include "graphicsclass.h"


GraphicsClass::GraphicsClass()
{
	m_D3D = 0;
	m_Camera = 0;
	m_Model = 0;
	m_Floor = 0;
	m_Light = 0;

	m_ProjectionShader = 0;
	m_ProjectionTexture = 0;
	m_ViewPoint = 0;
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
	char seafloor_dds[] = "./data/seafloor.dds";
	char cube_txt[] = "./data/cube.txt";
	result = m_Model->Initialize(m_D3D->GetDevice(), seafloor_dds , cube_txt);
	if(!result)
	{
		MessageBox(hwnd, "Could not initialize the model object.", "Error", MB_OK);
		return false;
	}
	m_Floor = new ModelClass;
	if (!m_Floor) {
		return false;
	}
	
	char blue01_dds[] = "./data/blue01.dds";
	char floor_txt[] = "./data/floor.txt";
	result = m_Floor->Initialize(m_D3D->GetDevice(), blue01_dds, floor_txt);
	if (!result) {
		return false;
	}

	m_Light = new LightClass;
	if (!m_Light) {
		return false;
	}
	// Initialize the light object.
	m_Light->SetAmbientColor(0.15f, 0.15f, 0.15f, 1.0f);
	m_Light->SetDiffuseColor(1.0f, 1.0f, 1.0f, 1.0f);
	m_Light->SetDirection(0.0f, -0.75f, 0.5f);

	m_ProjectionShader = new ProjectionShaderClass;
	if (!m_ProjectionShader) {
		return false;
	}

	result = m_ProjectionShader->Initialize(m_D3D->GetDevice(), hwnd);
	if (!result) {
		MessageBox(hwnd, "Could not initialize the projection shader object.", "Error", MB_OK);
		return false;
	}

	m_ProjectionTexture = new TextureClass;
	if (!m_ProjectionTexture) {
		return false;
	}
	char tex[] = "./data/grate.dds";
	result = m_ProjectionTexture->Initialize(m_D3D->GetDevice(), tex);
	if (!result) {
		MessageBox(hwnd, "Could not initialize the projection texture object.", "Error", MB_OK);
		return false;
	}

	m_ViewPoint = new ViewPointClass;
	if (!m_ViewPoint) {
		return false;
	}

	m_ViewPoint->SetPosition(2.0f, 5.0f, -2.0f);
	m_ViewPoint->SetLookAt(0.0f, 0.0f, 0.0f);
	m_ViewPoint->SetProjectionParameters((float)(XM_PI / 2.0f), 1.0f, 0.1f, 100.0f);
	m_ViewPoint->GenerateViewMatrix();
	m_ViewPoint->GenerateProjectionMatrix();


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

	if (m_Floor) {
		m_Floor->Shutdown();
		delete m_Floor;
		m_Floor = 0;
	}

	if (m_Light) {
		delete m_Light;
		m_Light = 0;
	}

	if (m_ProjectionTexture) {
		m_ProjectionTexture->Shutdown();
		delete m_ProjectionTexture;
		m_ProjectionTexture = 0;
	}

	if (m_ProjectionShader) {
		m_ProjectionShader->Shutdown();
		delete m_ProjectionShader;
		m_ProjectionShader = 0;
	}

	if (m_ViewPoint) {
		delete m_ViewPoint;
		m_ViewPoint = 0;
	}

	return;
}


bool GraphicsClass::Frame()
{

	bool result;

	// Set the position and rotation of the camera.
	m_Camera->SetPosition(0.0f, 10.0f, -20.0f);
	m_Camera->SetRotation(0.0f, 30.0f, 0.0f);

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
	XMMATRIX viewMatrix2, projectionMatrix2;

	m_D3D->BeginScene(0.0f, 0.0f, 0.0f, 1.0f);
	m_Camera->Render();

	m_D3D->GetProjectionMatrix(projectionMatrix);
	m_Camera->GetViewMatrix(viewMatrix);
	m_D3D->GetWorldMatrix(worldMatrix);

	m_ViewPoint->GetViewMatrix(viewMatrix2);
	m_ViewPoint->GetProjectionMatrix(projectionMatrix2);

	m_Floor->Render(m_D3D->GetDeviceContext());
	result = m_ProjectionShader->Render(m_D3D->GetDeviceContext(), m_Floor->GetIndexCount(), worldMatrix, viewMatrix, projectionMatrix, 
					    m_Floor->GetTexture(), m_Light->GetAmbientColor(), m_Light->GetDiffuseColor(), m_Light->GetDirection(), 
					    viewMatrix2, projectionMatrix2, m_ProjectionTexture->GetTexture());
	if(!result)
	{
		return false;
	}

	// Reset the world matrix and setup the translation for the cube model.
	m_D3D->GetWorldMatrix(worldMatrix);
	worldMatrix *= XMMatrixTranslation(0.0f, 2.0f, 0.0f);

	// Render the cube model using the projection shader.
	m_Model->Render(m_D3D->GetDeviceContext());
	result = m_ProjectionShader->Render(m_D3D->GetDeviceContext(), m_Model->GetIndexCount(), worldMatrix, viewMatrix, projectionMatrix, 
					    m_Model->GetTexture(), m_Light->GetAmbientColor(), m_Light->GetDiffuseColor(), m_Light->GetDirection(), 
					    viewMatrix2, projectionMatrix2, m_ProjectionTexture->GetTexture());
	if(!result)
	{
		return false;
	}



	m_D3D->EndScene();

	return true;
}
