#include "NMDX.h"

namespace utils
{
	namespace NMDX
	{
		Vector3 getDxVector(const NMP::Vector3 in)
		{
			return Vector3(in.x, in.y, in.z);
		}

		Quaternion getDxQuat(const NMP::Quat in)
		{
			return Quaternion(in.x, in.y, in.z, in.w);
		}

		Matrix getTransformMatrix(NMP::Quat quat, NMP::Vector3 pos)
		{
			return Matrix::CreateScale(1.f) * Matrix::CreateFromQuaternion(getDxQuat(quat)) * Matrix::CreateTranslation(getDxVector(pos));
		}
	}
}