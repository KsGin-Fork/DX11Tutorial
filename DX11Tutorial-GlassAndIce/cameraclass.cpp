////////////////////////////////////////////////////////////////////////////////
// Filename: cameraclass.cpp
////////////////////////////////////////////////////////////////////////////////
#include "cameraclass.h"


CameraClass::CameraClass()
{
	m_positionX = 0.0f;
	m_positionY = 0.0f;
	m_positionZ = 0.0f;

	m_rotationX = 0.0f;
	m_rotationY = 0.0f;
	m_rotationZ = 0.0f;
}


CameraClass::CameraClass(const CameraClass& other)
{
}


CameraClass::~CameraClass()
{
}


void CameraClass::SetPosition(float x, float y, float z)
{
	m_positionX = x;
	m_positionY = y;
	m_positionZ = z;
	return;
}


void CameraClass::SetRotation(float x, float y, float z)
{
	m_rotationX = x;
	m_rotationY = y;
	m_rotationZ = z;
	return;
}


void CameraClass::Render()
{
	XMFLOAT3 up;
	XMFLOAT3 position;
	XMFLOAT3 lookAt;
	float radians;


	// Setup the vector that points upwards.
	up.x = 0.0f;
	up.y = 1.0f;
	up.z = 0.0f;

	// Setup the position of the camera in the world.
	position.x = m_positionX;
	position.y = m_positionY;
	position.z = m_positionZ;

	// Calculate the rotation in radians.
	radians = m_rotationY * 0.0174532925f;

	// Setup where the camera is looking.
	lookAt.x = sinf(radians) + m_positionX;
	lookAt.y = m_positionY;
	lookAt.z = cosf(radians) + m_positionZ;

	// Create the view matrix from the three vectors.
	m_viewMatrix = XMMatrixLookAtLH(
		XMVectorSet(position.x , position.y , position.z , 0.0f), 
		XMVectorSet(lookAt.x , lookAt.y , lookAt.z , 0.0f), 
		XMVectorSet(up.x , up.y , up.z , 0.0f));

	return;
}


void CameraClass::GetViewMatrix(XMMATRIX& viewMatrix)
{
	viewMatrix = m_viewMatrix;
}


void CameraClass::RenderReflection(float height)
{
	XMFLOAT3 up, position, lookAt;
	float radians;


	// Setup the vector that points upwards.
	up.x = 0.0f;
	up.y = 1.0f;
	up.z = 0.0f;

	// Setup the position of the camera in the world.
	// For planar reflection invert the Y position of the camera.
	position.x = m_positionX;
	position.y = -m_positionY + (height * 2.0f);
	position.z = m_positionZ;

	// Calculate the rotation in radians.
	radians = m_rotationY * 0.0174532925f;

	// Setup where the camera is looking.
	lookAt.x = sinf(radians) + m_positionX;
	lookAt.y = position.y;
	lookAt.z = cosf(radians) + m_positionZ;

	// Create the view matrix from the three vectors.
	m_reflectionViewMatrix = XMMatrixLookAtLH(
		XMVectorSet(position.x, position.y, position.z, 0.0f),
		XMVectorSet(lookAt.x, lookAt.y, lookAt.z, 0.0f),
		XMVectorSet(up.x, up.y, up.z, 0.0f));
}


XMMATRIX CameraClass::GetReflectionViewMatrix()
{
	return m_reflectionViewMatrix;
}