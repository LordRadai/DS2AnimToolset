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
	DirectX::XMMATRIX GetRotationMatrixFromWorldMatrix(DirectX::XMMATRIX m_world);
	DirectX::XMMATRIX GetRotationFrom2Vectors(DirectX::SimpleMath::Vector3 pointA, DirectX::SimpleMath::Vector3 pointB);
	DirectX::SimpleMath::Vector3 GetYawPitchRollFromMatrix(DirectX::XMMATRIX m_world);
	DirectX::SimpleMath::Vector3 GetLookAtVectorFromMatrix(DirectX::XMMATRIX m_world);
	DirectX::SimpleMath::Vector3 ConvertQuatToEulerAngles(const DirectX::SimpleMath::Quaternion& quaternion);

	inline float ConvertDegAngleToRad(float angle);
	inline float ConvertRadAngleToDeg(float angle);

	UINT ExtractBits(int value, int startBit, int numBits);

	UINT ConvertFloatColorToInt(DirectX::SimpleMath::Vector4 color);

	float FrameToTime(int frame, int frameRate = 60.f);
	int TimeToFrame(float time, int frameRate = 60.f, bool round = true);
	int AlignValue(int value, int alignment);
}