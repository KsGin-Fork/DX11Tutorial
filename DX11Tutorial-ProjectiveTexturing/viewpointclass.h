#pragma once
#include <DirectXMath.h>

class ViewPointClass
{
public:
	ViewPointClass();
	ViewPointClass(const ViewPointClass&);
	~ViewPointClass();

	void SetPosition(float, float, float);
	void SetLookAt(float, float, float);
	void SetProjectionParameters(float, float, float, float);

	void GenerateViewMatrix();
	void GenerateProjectionMatrix();

	void GetViewMatrix(DirectX::XMMATRIX&);
	void GetProjectionMatrix(DirectX::XMMATRIX&);

private:
	DirectX::XMVECTOR m_position, m_lookAt;
	DirectX::XMMATRIX m_viewMatrix, m_projectionMatrix;
	float m_fieldOfView, m_aspectRatio, m_nearPlane, m_farPlane;
};
