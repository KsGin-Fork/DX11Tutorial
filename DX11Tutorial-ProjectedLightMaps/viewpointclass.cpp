#include "viewpointclass.h"

ViewPointClass::ViewPointClass()
{
}


ViewPointClass::ViewPointClass(const ViewPointClass& other)
{
}


ViewPointClass::~ViewPointClass()
{
}


void ViewPointClass::SetPosition(float x, float y, float z)
{
	m_position = DirectX::XMVectorSet(x, y, z , 0.0f);
	return;
}


void ViewPointClass::SetLookAt(float x, float y, float z)
{
	m_lookAt = DirectX::XMVectorSet(x, y, z , 0.0f);
	return;
}


void ViewPointClass::SetProjectionParameters(float fieldOfView, float aspectRatio, float nearPlane, float farPlane)
{
	m_fieldOfView = fieldOfView;
	m_aspectRatio = aspectRatio;
	m_nearPlane = nearPlane;
	m_farPlane = farPlane;
	return;
}


void ViewPointClass::GenerateViewMatrix()
{
	DirectX::XMVECTOR up;

	up = DirectX::XMVectorSet(0.0f, 1.0f, 0.0f, 0.0f);

	// Create the view matrix from the three vectors.
	m_viewMatrix = DirectX::XMMatrixLookAtLH( m_position , m_lookAt , up);
	
	return;
}


void ViewPointClass::GenerateProjectionMatrix()
{
	// Create the projection matrix for the view point.
	m_projectionMatrix = DirectX::XMMatrixPerspectiveFovLH(m_fieldOfView, m_aspectRatio, m_nearPlane, m_farPlane);
	return;
}


void ViewPointClass::GetViewMatrix(DirectX::XMMATRIX& viewMatrix)
{
	viewMatrix = m_viewMatrix;
	return;
}


void ViewPointClass::GetProjectionMatrix(DirectX::XMMATRIX& projectionMatrix)
{
	projectionMatrix = m_projectionMatrix;
	return;
}
