#pragma once
#include <ostream>
#include <string>
#include <sstream>

#include "PrimitiveBatch.h"
#include "VertexTypes.h"
#include "BufferHelpers.h"
#include "CommonStates.h"
#include "DDSTextureLoader.h"
#include "DirectXHelpers.h"
#include "Effects.h"
#include "GeometricPrimitive.h"
#include "GraphicsMemory.h"
#include "Keyboard.h"
#include "Model.h"
#include "Mouse.h"
#include "PostProcess.h"
#include "PrimitiveBatch.h"
#include "ScreenGrab.h"
#include "SimpleMath.h"
#include "SpriteBatch.h"
#include "SpriteFont.h"
#include "VertexTypes.h"
#include "WICTextureLoader.h"

namespace RMath
{
	DirectX::XMMATRIX getRotationMatrixFromWorldMatrix(DirectX::XMMATRIX m_world);
	DirectX::XMMATRIX getRotationFrom2Vectors(DirectX::SimpleMath::Vector3 pointA, DirectX::SimpleMath::Vector3 pointB);
	DirectX::SimpleMath::Vector3 getYawPitchRollFromMatrix(DirectX::XMMATRIX m_world);
	DirectX::SimpleMath::Vector3 getLookAtVectorFromMatrix(DirectX::XMMATRIX m_world);
	DirectX::SimpleMath::Vector3 convertQuatToEulerAngles(const DirectX::SimpleMath::Quaternion& quaternion);

	float degToRad(float angle);
	float radToDeg(float angle);

	DirectX::SimpleMath::Vector4 getFloatColor(BYTE r, BYTE g, BYTE b, BYTE a);
	DirectX::SimpleMath::Vector4 getFloatColor(UINT color);

	UINT getHexColor(DirectX::SimpleMath::Vector4 color);

	float frameToTime(int frame, int frameRate = 60.f);
	int timeToFrame(float time, int frameRate = 60.f);
}