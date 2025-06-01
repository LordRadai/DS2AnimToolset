#pragma once
#include "MorphemeSystem/MorphemeSystem.h"
#include <SimpleMath.h>

using namespace DirectX::SimpleMath;

namespace utils
{
	namespace NMDX
	{
		Vector3 getDxVector(const NMP::Vector3 in);
		Quaternion getDxQuat(const NMP::Quat in);
		Matrix getTransformMatrix(NMP::Quat quat, NMP::Vector3 pos);
	}
}