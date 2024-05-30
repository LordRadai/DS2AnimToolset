#include "RMath.h"
#include <math.h>

using namespace DirectX;
using namespace DirectX::SimpleMath;

DirectX::XMMATRIX RMath::GetRotationMatrixFromWorldMatrix(DirectX::XMMATRIX m_world)
{
	DirectX::XMMATRIX rotation = m_world;

	rotation.r[3].m128_f32[0] = 0;
	rotation.r[3].m128_f32[1] = 0;
	rotation.r[3].m128_f32[2] = 0;
	rotation.r[3].m128_f32[3] = 1;

	return rotation;
}

DirectX::XMMATRIX RMath::GetRotationFrom2Vectors(DirectX::SimpleMath::Vector3 pointA, DirectX::SimpleMath::Vector3 pointB)
{
	XMMATRIX result = XMMatrixIdentity();

	Vector3 euler = Vector3(0, 0, 0);
	Vector3 diff = pointB - pointA;

	if (diff.x == 0 && diff.z == 0)
	{
		if (diff.y >= 0)
			result = XMMatrixRotationZ(XM_PI);
		else
			result = XMMatrixRotationZ(0);

		return result;
	}

	float distance = diff.Length();

	euler.y = asin(diff.y / distance);

	float distance_xz = distance * cos(euler.y);

	euler.x = acos(diff.x / distance_xz);
	euler.z = acos(diff.z / distance_xz);

	XMMATRIX rot_x = XMMatrixRotationX(-euler.x);
	XMMATRIX rot_y = XMMatrixRotationY(-euler.y);
	XMMATRIX rot_z = XMMatrixRotationZ(-euler.z);

	result = rot_x * result;
	result = rot_y * result;
	result = rot_z * result;

	return result;
}

DirectX::SimpleMath::Vector3 RMath::GetYawPitchRollFromMatrix(DirectX::XMMATRIX m_world)
{
	DirectX::XMVECTOR from(DirectX::XMVectorSet(m_world.r[0].m128_f32[1], m_world.r[2].m128_f32[0], 0.0f, 0.0f));
	DirectX::XMVECTOR to(DirectX::XMVectorSet(m_world.r[1].m128_f32[1], m_world.r[2].m128_f32[2], 0.0f, 0.0f));
	DirectX::XMVECTOR res(DirectX::XMVectorATan2(from, to));

	float yaw = DirectX::XMVectorGetY(res);
	float pitch = DirectX::XMScalarASin(-m_world.r[2].m128_f32[2]);
	float roll = DirectX::XMVectorGetX(res);

	return DirectX::SimpleMath::Vector3(roll, pitch, yaw);
}

DirectX::SimpleMath::Vector3 RMath::GetLookAtVectorFromMatrix(DirectX::XMMATRIX m_world)
{
	DirectX::XMMATRIX rotation = RMath::GetRotationMatrixFromWorldMatrix(m_world);

	DirectX:XMVECTOR transformed_pos = DirectX::SimpleMath::Vector3(0, 0, 1);
	transformed_pos = DirectX::XMVector3Transform(transformed_pos, rotation);

	return DirectX::SimpleMath::Vector3(transformed_pos);
}

//Converts quaternion to euler angles
DirectX::SimpleMath::Vector3 RMath::ConvertQuatToEulerAngles(const DirectX::SimpleMath::Quaternion& quaternion)
{
	DirectX::SimpleMath::Vector3 eulerAngles;
	DirectX::SimpleMath::Matrix rotationMatrix = DirectX::XMMatrixRotationQuaternion(quaternion);

	eulerAngles.x = atan2f(rotationMatrix._32, rotationMatrix._33); // Pitch
	eulerAngles.y = asinf(-rotationMatrix._31); // Yaw
	eulerAngles.z = atan2f(rotationMatrix._21, rotationMatrix._11); // Roll

	return eulerAngles;
}

inline float RMath::ConvertDegAngleToRad(float angle)
{
	return ((angle / 360.f) * XM_2PI);
}

inline float RMath::ConvertRadAngleToDeg(float angle)
{
	return ((angle / XM_2PI) * 360.f);
}

UINT RMath::ExtractBits(int value, int shiftValue, int mask)
{
	UINT extractedBits = (UINT)(value >> shiftValue) & mask;

	return (int)extractedBits;
}

UINT RMath::ConvertFloatColorToInt(DirectX::SimpleMath::Vector4 color)
{
	BYTE red = color.x * 255;
	BYTE green = color.y * 255;
	BYTE blue = color.z * 255;
	BYTE alpha = color.w * 255;

	return (alpha << 24) | (red << 16) | (green << 8) | blue;
}

//Converts floating point time value to discrete frame count
float RMath::FrameToTime(int frame, int frameRate)
{
	return ((float)frame / (float)frameRate);
}

//Converts discrete frame count to a floating point time value
int RMath::TimeToFrame(float time, int frameRate, bool round)
{
	float frame = (time * frameRate);

	if (std::roundf(frame) == 0 && frame > 0)
		return 1;

	if (!round)
		return frame;

	return std::roundf(frame);
}

//Returns aligned value to the specified alignment
int RMath::AlignValue(int value, int alignment)
{
	int remainder = value % alignment;

	if (remainder > 0)
		return value + alignment - remainder;

	return value;
}