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


void LightClass::SetDiffuseColor(float red, float green, float blue, float alpha)
{
	m_diffuseColor = DirectX::XMFLOAT4(red, green, blue, alpha);
	return;
}



void LightClass::SetPosition(float x, float y, float z)
{
	m_position = DirectX::XMFLOAT4(x, y, z , 1.0f);
	return;
}


DirectX::XMFLOAT4 LightClass::GetDiffuseColor()
{
	return m_diffuseColor;
}





DirectX::XMFLOAT4 LightClass::GetPosition()
{
	return m_position;
}