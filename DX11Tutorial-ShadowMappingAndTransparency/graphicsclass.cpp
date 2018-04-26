////////////////////////////////////////////////////////////////////////////////
// Filename: graphicsclass.cpp
////////////////////////////////////////////////////////////////////////////////
#include "graphicsclass.h"


GraphicsClass::GraphicsClass()
{
	m_D3D = 0;
	m_Camera = 0;
	m_TrunkModel = 0;
	m_GroundModel = 0;
	m_LeavesModel = 0;
	m_Light = 0;
	m_RenderTexture = 0;
	m_DepthShader = 0;	
	m_ShadowShader = 0;
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

	// Set the initial position of the camera.
	m_Camera->SetPosition(0.0f, 0.0f, -10.0f);
	
	// Create the cube model object.
	m_TrunkModel = new ModelClass;
	if(!m_TrunkModel)
	{
		return false;
	}

	CHAR trunk001_txt[] = "./data/trunk001.txt";
	CHAR trunk001_dds[] = "./data/trunk001.dds";

	// Initialize the cube model object.
	result = m_TrunkModel->Initialize(m_D3D->GetDevice(), trunk001_txt , trunk001_dds);
	if(!result)
	{
		MessageBox(hwnd, "Could not initialize the trunk model object.", "Error", MB_OK);
		return false;
	}

	// Create the sphere model object.
	m_LeavesModel = new ModelClass;
	if(!m_LeavesModel)
	{
		return false;
	}

	CHAR leaf001_txt[] = "./data/leaf001.txt";
	CHAR leaf001_dds[] = "./data/leaf001.dds";
	// Initialize the sphere model object.
	result = m_LeavesModel->Initialize(m_D3D->GetDevice(),leaf001_txt ,leaf001_dds );
	if(!result)
	{
		MessageBox(hwnd, "Could not initialize the sphere model object.", "Error", MB_OK);
		return false;
	}

	// Create the ground model object.
	m_GroundModel = new ModelClass;
	if(!m_GroundModel)
	{
		return false;
	}

	CHAR plane01[] = "./data/plane01.txt";
	CHAR dirt[] = "./data/dirt.dds";

	// Initialize the ground model object.
	result = m_GroundModel->Initialize(m_D3D->GetDevice(),plane01 ,dirt );
	if(!result)
	{
		MessageBox(hwnd, "Could not initialize the ground model object.", "Error", MB_OK);
		return false;
	}

	// Create the light object.
	m_Light = new LightClass;
	if(!m_Light)
	{
		return false;
	}

	// Initialize the light object.
	m_Light->SetAmbientColor(0.3f, 0.3f, 0.3f, 1.0f);
	m_Light->SetDiffuseColor(1.0f, 1.0f, 1.0f, 1.0f);
	m_Light->GenerateOrthoMatrix(10.0f, SCREEN_NEAR, SCREEN_DEPTH);

	// Create the render to texture object.
	m_RenderTexture = new RenderTextureClass;
	if(!m_RenderTexture)
	{
		return false;
	}

	// Initialize the render to texture object.
	result = m_RenderTexture->Initialize(m_D3D->GetDevice(), SHADOWMAP_WIDTH, SHADOWMAP_HEIGHT, SCREEN_DEPTH, SCREEN_NEAR);
	if(!result)
	{
		MessageBox(hwnd, "Could not initialize the render to texture object.", "Error", MB_OK);
		return false;
	}

	// Create the depth shader object.
	m_DepthShader = new DepthShaderClass;
	if(!m_DepthShader)
	{
		return false;
	}

	// Initialize the depth shader object.
	result = m_DepthShader->Initialize(m_D3D->GetDevice(), hwnd);
	if(!result)
	{
		MessageBox(hwnd, "Could not initialize the depth shader object.", "Error", MB_OK);
		return false;
	}

	// Create the shadow shader object.
	m_ShadowShader = new ShadowShaderClass;
	if(!m_ShadowShader)
	{
		return false;
	}

	// Initialize the shadow shader object.
	result = m_ShadowShader->Initialize(m_D3D->GetDevice(), hwnd);
	if(!result)
	{
		MessageBox(hwnd, "Could not initialize the shadow shader object.", "Error", MB_OK);
		return false;
	}

	return true;
}


void GraphicsClass::Shutdown()
{
	// Release the shadow shader object.
	if(m_ShadowShader)
	{
		m_ShadowShader->Shutdown();
		delete m_ShadowShader;
		m_ShadowShader = 0;
	}

	// Release the depth shader object.
	if(m_DepthShader)
	{
		m_DepthShader->Shutdown();
		delete m_DepthShader;
		m_DepthShader = 0;
	}

	// Release the render to texture object.
	if(m_RenderTexture)
	{
		m_RenderTexture->Shutdown();
		delete m_RenderTexture;
		m_RenderTexture = 0;
	}

	// Release the light object.
	if(m_Light)
	{
		delete m_Light;
		m_Light = 0;
	}

	// Release the ground model object.
	if(m_GroundModel)
	{
		m_GroundModel->Shutdown();
		delete m_GroundModel;
		m_GroundModel = 0;
	}

	// Release the sphere model object.
	if(m_LeavesModel)
	{
		m_LeavesModel->Shutdown();
		delete m_LeavesModel;
		m_LeavesModel = 0;
	}

	// Release the cube model object.
	if(m_TrunkModel)
	{
		m_TrunkModel->Shutdown();
		delete m_TrunkModel;
		m_TrunkModel = 0;
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

	return;
}


bool GraphicsClass::Frame(float posX, float posY, float posZ, float rotX, float rotY, float rotZ)
{
	bool result;
	static float lightAngle = 270.0f;
	float radians;
	static float lightPosX = 9.0f;


	// Set the position of the camera.
	m_Camera->SetPosition(posX, posY, posZ);
	m_Camera->SetRotation(rotX, rotY, rotZ);


	lightPosX -= 0.003f;

	// Update the angle of the light each frame.
	lightAngle -= 0.03f;
	if(lightAngle < 90.0f)
	{
		lightAngle = 270.0f;

		// Reset the light position also.
		lightPosX = 9.0f;
	}
	radians = lightAngle * 0.0174532925f;

	// Update the direction of the light.
	m_Light->SetDirection(sinf(radians), cosf(radians), 0.0f);
	m_Light->SetPosition(lightPosX, 8.0f, -1.0f);
	m_Light->SetLookAt(0.0f, 0.0f, 0.0f);

	// Render the graphics scene.
	result = Render();
	if(!result)
	{
		return false;
	}

	return true;
}


bool GraphicsClass::RenderSceneToTexture()
{
	DirectX::XMMATRIX worldMatrix, lightViewMatrix, lightOrthoMatrix;
	float posX, posY, posZ;
	bool result;

	// Turn on alpha blending
	m_D3D->TurnOnAlphaBlending();

	// Set the render target to be the render to texture.
	m_RenderTexture->SetRenderTarget(m_D3D->GetDeviceContext());

	// Clear the render to texture.
	m_RenderTexture->ClearRenderTarget(m_D3D->GetDeviceContext(), 0.0f, 0.0f, 0.0f, 1.0f);

	// Generate the light view matrix based on the light's position.
	m_Light->GenerateViewMatrix();

	// Get the world matrix from the d3d object.
	m_D3D->GetWorldMatrix(worldMatrix);

	// Get the view and orthographic matrices from the light object.
	m_Light->GetViewMatrix(lightViewMatrix);
	m_Light->GetOrthoMatrix(lightOrthoMatrix);

	// Setup the translation matrix for the cube model.
	worldMatrix *= DirectX::XMMatrixScaling(0.05f, 0.05f, 0.05f);

	// Render the cube model with the depth shader.
	m_TrunkModel->Render(m_D3D->GetDeviceContext());
	result = m_DepthShader->Render(m_D3D->GetDeviceContext(), m_TrunkModel->GetIndexCount(), worldMatrix, lightViewMatrix, lightOrthoMatrix , m_TrunkModel->GetTexture());
	if(!result)
	{
		return false;
	}

	// Reset the world matrix.
	m_D3D->GetWorldMatrix(worldMatrix);

	// Setup the translation matrix for the sphere model.
	worldMatrix *= DirectX::XMMatrixScaling(0.05f, 0.05f, 0.05f);

	// Render the sphere model with the depth shader.
	m_LeavesModel->Render(m_D3D->GetDeviceContext());
	result = m_DepthShader->Render(m_D3D->GetDeviceContext(), m_LeavesModel->GetIndexCount(), worldMatrix, lightViewMatrix, lightOrthoMatrix , m_LeavesModel->GetTexture());
	if(!result)
	{
		return false;
	}

	// Reset the world matrix.
	m_D3D->GetWorldMatrix(worldMatrix);

	// Setup the translation matrix for the ground model.
	m_GroundModel->GetPosition(posX, posY, posZ);
	worldMatrix *= DirectX::XMMatrixTranslation(posX, posY, posZ);

	// Render the ground model with the depth shader.
	m_GroundModel->Render(m_D3D->GetDeviceContext());
	result = m_DepthShader->Render(m_D3D->GetDeviceContext(), m_GroundModel->GetIndexCount(), worldMatrix, lightViewMatrix, lightOrthoMatrix , m_GroundModel->GetTexture());
	if(!result)
	{
		return false;
	}

	
	// Turn off alpha blending
	m_D3D->TurnOffAlphaBlending();

	// Reset the render target back to the original back buffer and not the render to texture anymore.
	m_D3D->SetBackBufferRenderTarget();

	// Reset the viewport back to the original.
	m_D3D->ResetViewport();

	return true;
}


bool GraphicsClass::Render()
{
	DirectX::XMMATRIX worldMatrix, viewMatrix, projectionMatrix;
	DirectX::XMMATRIX lightViewMatrix, lightOrthoMatrix;
	bool result;

	// First render the scene to a texture.
	result = RenderSceneToTexture();
	if(!result)
	{
		return false;
	}

	// Clear the buffers to begin the scene.
	m_D3D->BeginScene(0.0f, 0.0f, 0.0f, 1.0f);

	// Turn on alpha blending
	m_D3D->TurnOnAlphaBlending();

	// Generate the view matrix based on the camera's position.
	m_Camera->Render();

	// Generate the light view matrix based on the light's position.
	m_Light->GenerateViewMatrix();

	// Get the world, view, and projection matrices from the camera and d3d objects.
	m_Camera->GetViewMatrix(viewMatrix);
	m_D3D->GetWorldMatrix(worldMatrix);
	m_D3D->GetProjectionMatrix(projectionMatrix);

	// Get the light's view and projection matrices from the light object.
	m_Light->GetViewMatrix(lightViewMatrix);
	m_Light->GetOrthoMatrix(lightOrthoMatrix);

	worldMatrix *= DirectX::XMMatrixScaling(0.05f, 0.05f, 0.05f);
	// Put the cube model vertex and index buffers on the graphics pipeline to prepare them for drawing.
	m_TrunkModel->Render(m_D3D->GetDeviceContext());

	// Render the model using the shadow shader.
	result = m_ShadowShader->Render(m_D3D->GetDeviceContext(), m_TrunkModel->GetIndexCount(), worldMatrix, viewMatrix, projectionMatrix, lightViewMatrix, 
									lightOrthoMatrix, m_TrunkModel->GetTexture(), m_RenderTexture->GetShaderResourceView(), m_Light->GetDirection(),
									m_Light->GetAmbientColor(), m_Light->GetDiffuseColor());
	if(!result)
	{
		return false;
	}

	// Reset the world matrix.
	m_D3D->GetWorldMatrix(worldMatrix);

	worldMatrix *= DirectX::XMMatrixScaling(0.05f, 0.05f, 0.05f);

	// Put the model vertex and index buffers on the graphics pipeline to prepare them for drawing.
	m_LeavesModel->Render(m_D3D->GetDeviceContext());
	result = m_ShadowShader->Render(m_D3D->GetDeviceContext(), m_LeavesModel->GetIndexCount(), worldMatrix, viewMatrix, projectionMatrix, lightViewMatrix, 
									lightOrthoMatrix, m_LeavesModel->GetTexture(), m_RenderTexture->GetShaderResourceView(), m_Light->GetDirection(), 
									m_Light->GetAmbientColor(), m_Light->GetDiffuseColor());
	if(!result)
	{
		return false;
	}

	// Reset the world matrix.
	m_D3D->GetWorldMatrix(worldMatrix);

	// Render the ground model using the shadow shader.
	m_GroundModel->Render(m_D3D->GetDeviceContext());
	result = m_ShadowShader->Render(m_D3D->GetDeviceContext(), m_GroundModel->GetIndexCount(), worldMatrix, viewMatrix, projectionMatrix, lightViewMatrix,
									lightOrthoMatrix, m_GroundModel->GetTexture(), m_RenderTexture->GetShaderResourceView(), m_Light->GetDirection(), 
									m_Light->GetAmbientColor(), m_Light->GetDiffuseColor());
	if(!result)
	{
		return false;
	}


	// Turn off alpha blending
	m_D3D->TurnOffAlphaBlending();

	// Present the rendered scene to the screen.
	m_D3D->EndScene();

	return true;
}