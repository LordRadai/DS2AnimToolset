#include "NMDX.h"

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
	return Matrix::CreateScale(1.f) * Matrix::CreateFromQuaternion(CreateFrom(quat)) * Matrix::CreateTranslation(CreateFrom(pos));
}
