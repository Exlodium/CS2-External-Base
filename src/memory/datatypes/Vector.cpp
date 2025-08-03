#include "../../Precompiled.h"

[[nodiscard]] Vector Vector::Transform(const Matrix3x4_t& matTransform) const
{
	return
	{
		this->DotProduct(matTransform[0]) + matTransform[0][3],
		this->DotProduct(matTransform[1]) + matTransform[1][3],
		this->DotProduct(matTransform[2]) + matTransform[2][3]
	};
}