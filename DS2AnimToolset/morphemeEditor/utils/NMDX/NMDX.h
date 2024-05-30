#pragma once
#include "MorphemeSystem/MorphemeSystem.h"
#include <SimpleMath.h>

using namespace DirectX::SimpleMath;

namespace NMDX
{
	Vector3 CreateFrom(const NMP::Vector3 in);
	Quaternion CreateFrom(const NMP::Quat in);
	Matrix GetWorldMatrix(NMP::Quat quat, NMP::Vector3 pos);
}