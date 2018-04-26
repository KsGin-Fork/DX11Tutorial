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


DirectX::XMFLOAT3 CameraClass::GetPosition()
{
	return DirectX::XMFLOAT3(m_positionX, m_positionY, m_positionZ);
}


DirectX::XMFLOAT3 CameraClass::GetRotation()
{
	return DirectX::XMFLOAT3(m_rotationX, m_rotationY, m_rotationZ);
}


void CameraClass::Render()
{
	DirectX::XMVECTOR up, position, lookAt;
	float yaw, pitch, roll;
	DirectX::XMMATRIX rotationMatrix;


	// Setup the vector that points upwards.
	up = DirectX::XMVectorSet(0.0f, 1.0f, 0.0f, 1.0f);

	// Setup the position of the camera in the world.
	position = DirectX::XMVectorSet(m_positionX, m_positionY, m_positionZ, 1.0f);

	// Setup where the camera is looking by default.
	lookAt = DirectX::XMVectorSet(0.0f, 0.0f, 1.0f, 1.0f);

	// Set the yaw (Y axis), pitch (X axis), and roll (Z axis) rotations in radians.
	pitch = m_rotationX * 0.0174532925f;
	yaw   = m_rotationY * 0.0174532925f;
	roll  = m_rotationZ * 0.0174532925f;

	// Create the rotation matrix from the yaw, pitch, and roll values.
	rotationMatrix = DirectX::XMMatrixRotationRollPitchYaw(pitch, yaw, roll);

	// Transform the lookAt and up vector by the rotation matrix so the view is correctly rotated at the origin.
	lookAt = DirectX::XMVector3TransformCoord(lookAt, rotationMatrix);
	up = DirectX::XMVector3TransformCoord(up, rotationMatrix);

	// Translate the rotated camera position to the location of the viewer.
	lookAt = DirectX::XMVectorAdd(position, lookAt); 
	// Finally create the view matrix from the three updated vectors.
	m_viewMatrix = DirectX::XMMatrixLookAtLH(position, lookAt, up);

	return;
}


void CameraClass::GetViewMatrix(DirectX::XMMATRIX& viewMatrix)
{
	viewMatrix = m_viewMatrix;
	return;
}