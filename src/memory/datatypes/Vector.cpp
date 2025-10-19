#include "../../Includes.h"

[[nodiscard]] QAngle Vector::ToAngles() const
{
	float flPitch, flYaw;
	if (this->x == 0.0f && this->y == 0.0f)
	{
		flPitch = (this->z > 0.0f) ? 270.f : 90.f;
		flYaw = 0.0f;
	}
	else
	{
		flPitch = M_RAD2DEG(std::atan2f(-this->z, this->Length2D()));

		if (flPitch < 0.f)
			flPitch += 360.f;

		flYaw = M_RAD2DEG(std::atan2f(this->y, this->x));

		if (flYaw < 0.f)
			flYaw += 360.f;
	}

	return { flPitch, flYaw, 0.0f };
}

[[nodiscard]] Vector Vector::Transform(const Matrix3x4_t& matTransform) const
{
	return
	{
		this->DotProduct(matTransform[0]) + matTransform[0][3],
		this->DotProduct(matTransform[1]) + matTransform[1][3],
		this->DotProduct(matTransform[2]) + matTransform[2][3]
	};
}

[[nodiscard]] Vector Vector::Rotate(const Matrix3x4_t& matTransform) const
{
	return
	{
		this->x * matTransform[0][0] + this->y * matTransform[1][0] + this->z * matTransform[2][0],
		this->x * matTransform[0][1] + this->y * matTransform[1][1] + this->z * matTransform[2][1],
		this->x * matTransform[0][2] + this->y * matTransform[1][2] + this->z * matTransform[2][2]
	};
}