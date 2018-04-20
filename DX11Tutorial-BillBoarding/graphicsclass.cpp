////////////////////////////////////////////////////////////////////////////////
// Filename: graphicsclass.cpp
////////////////////////////////////////////////////////////////////////////////
#include "graphicsclass.h"


GraphicsClass::GraphicsClass()
{
	m_D3D = 0;
	m_Camera = 0;
	m_BillBoardModel = 0;
	m_FloorModel = 0;
	m_TextureShader = 0;
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

	// Create the ground model object.
	m_BillBoardModel = new ModelClass;
	if(!m_BillBoardModel)
	{
		return false;
	}

	char seafloor_dds[] = "./data/seafloor.dds";
	char square_txt[] = "./data/square.txt";
	// Initialize the square model object.
	result = m_BillBoardModel->Initialize(m_D3D->GetDevice(), seafloor_dds, square_txt);
	if(!result)
	{
		MessageBox(hwnd, "Could not initialize the square model object.", "Error", MB_OK);
		return false;
	}

	// Create the wall model object.
	m_FloorModel = new ModelClass;
	if(!m_FloorModel)
	{
		return false;
	}

	char grid01_dds[] = "./data/grid01.dds";
	char floor_txt[] = "./data/floor.txt";
	// Initialize the floor model object.
	result = m_FloorModel->Initialize(m_D3D->GetDevice(), grid01_dds, floor_txt);
	if(!result)
	{
		MessageBox(hwnd, "Could not initialize the floor model object.", "Error", MB_OK);
		return false;
	}

	m_TextureShader = new TextureShaderClass;
	if (!m_TextureShader) {
		return false;
	}

	result = m_TextureShader->Initialize(m_D3D->GetDevice(), hwnd);
	if (!result) {
		MessageBox(hwnd, "Could not initialize the texture shader object.", "Error", MB_OK);
		return false;
	}

	return true;
}


void GraphicsClass::Shutdown()
{
	// Release the Floor Model object.
	if(m_FloorModel)
	{
		m_FloorModel->Shutdown();
		delete m_FloorModel;
		m_FloorModel = 0;
	}

	// Release the BillBoarding Model object.
	if(m_BillBoardModel)
	{
		m_BillBoardModel->Shutdown();
		delete m_BillBoardModel;
		m_BillBoardModel = 0;
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

	if (m_TextureShader) {
		m_TextureShader->Shutdown();
		delete m_TextureShader;
		m_TextureShader = 0;
	}

	return;
}


bool GraphicsClass::Frame()
{
	bool result;

	// Set the position and rotation of the camera.
	m_Camera->SetPosition(20.0f , 2.0f , -10.0f);
	m_Camera->SetRotation(0.0f, -45.0f, 0.0f);
	//m_Camera->SetPosition(0.0f , 2.0f , -20.0f);
	result = Render();
	if (!result) {
		return false;
	}

	return true;
}


bool GraphicsClass::Render()
{	
	DirectX::XMMATRIX worldMatrix, viewMatrix, projectionMatrix;
	bool result;
	XMFLOAT3 cameraPosition, modelPosition;
	double angle;
	float rotation;

	// Clear the buffers to begin the scene.
	m_D3D->BeginScene(0.0f, 0.0f, 0.0f, 1.0f);

	// Generate the view matrix based on the camera's position.
	m_Camera->Render();

	// Get the world, view, and projection matrices from the camera and d3d objects.
	m_Camera->GetViewMatrix(viewMatrix);
	m_D3D->GetProjectionMatrix(projectionMatrix);

	m_D3D->GetWorldMatrix(worldMatrix);
	worldMatrix *= XMMatrixScaling(5.0f, 1.0f, 5.0f);
	m_FloorModel->Render(m_D3D->GetDeviceContext());
	result = m_TextureShader->Render(
		m_D3D->GetDeviceContext(), 
		m_FloorModel->GetIndexCount(), 
		worldMatrix, viewMatrix, projectionMatrix, 
		m_FloorModel->GetTexture());
	if (!result) {
		return false;
	}


	m_Camera->GetPosition(cameraPosition);
	modelPosition = XMFLOAT3(0.0f, 1.5f, 1.0f);

	angle = atan2(modelPosition.x - cameraPosition.x, modelPosition.z - cameraPosition.z) * (180.0f / XM_PI);

	rotation = angle * 0.0174532925f;

	m_D3D->GetWorldMatrix(worldMatrix);
	worldMatrix *= XMMatrixTranslation(modelPosition.x, modelPosition.y, modelPosition.z);
	worldMatrix *= XMMatrixRotationY(rotation);
	m_BillBoardModel->Render(m_D3D->GetDeviceContext());
	result = m_TextureShader->Render(
		m_D3D->GetDeviceContext(), 
		m_BillBoardModel->GetIndexCount(), 
		worldMatrix, viewMatrix, projectionMatrix, 
		m_BillBoardModel->GetTexture());
	if (!result) {
		return false;
	}

	// Present the rendered scene to the screen.
	m_D3D->EndScene();

	return true;
}
