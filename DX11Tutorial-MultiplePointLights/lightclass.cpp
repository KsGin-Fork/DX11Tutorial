////////////////////////////////////////////////////////////////////////////////
// Filename: lightclass.cpp
////////////////////////////////////////////////////////////////////////////////
#include "lightclass.h"


LightClass::LightClass()
{
}


LightClass::LightClass(const LightClass& other)
{
}


LightClass::~LightClass()
{
}


void LightClass::SetAmbientColor(float red, float green, float blue, float alpha)
{
	m_ambientColor = DirectX::XMFLOAT4(red, green, blue, alpha);
	return;
}


void LightClass::SetDiffuseColor(float red, float green, float blue, float alpha)
{
	m_diffuseColor = DirectX::XMFLOAT4(red, green, blue, alpha);
	return;
}


void LightClass::SetSpecularColor(float red, float green, float blue, float alpha)
{
	m_specularColor = DirectX::XMFLOAT4(red, green, blue, alpha);
	return;
}


void LightClass::SetSpecularPower(float power)
{
	m_specularPower = power;
	return;
}


void LightClass::SetDirection(float x, float y, float z)
{
	m_direction = DirectX::XMFLOAT3(x, y, z);
	return;
}


DirectX::XMFLOAT4 LightClass::GetAmbientColor()
{
	return m_ambientColor;
}


DirectX::XMFLOAT4 LightClass::GetDiffuseColor()
{
	return m_diffuseColor;
}


DirectX::XMFLOAT4 LightClass::GetSpecularColor()
{
	return m_specularColor;
}


float LightClass::GetSpecularPower()
{
	return m_specularPower;
}


DirectX::XMFLOAT3 LightClass::GetDirection()
{
	return m_direction;
}