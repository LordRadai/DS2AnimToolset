#include "MorphemeToDirectX.h"

Vector3 NMDX::CreateFrom(const NMP::Vector3 in)
{
	return Vector3(in.x, in.y, in.z);
}

Quaternion NMDX::CreateFrom(const NMP::Quat in)
{
	return Quaternion(in.x, in.y, in.z, in.w);
}

Matrix NMDX::GetWorldMatrix(NMP::Quat quat, NMP::Vector3 pos)
{	
	return DirectX::XMMatrixScaling(1.f, 1.f, 1.f) * DirectX::XMMatrixRotationQuaternion(CreateFrom(quat)) * DirectX::XMMatrixTranslationFromVector(CreateFrom(pos));
}
