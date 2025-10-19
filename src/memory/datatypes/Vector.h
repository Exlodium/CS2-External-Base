#pragma once
// forward
struct Matrix3x4_t;
class QAngle;

class Vector2D
{
public:
	Vector2D() = default;
	Vector2D(float x, float y)
	{
		this->x = x; this->y = y;
	}

	Vector2D(const Vector2D& vecBase)
	{
		this->x = vecBase.x; this->y = vecBase.y;
	}

	bool IsValid() const
	{
		return std::isfinite(this->x) && std::isfinite(this->y);
	}

	constexpr void Invalidate()
	{
		this->x = this->y = std::numeric_limits<float>::infinity();
	}

	constexpr float operator[](std::size_t nIndex) const
	{
		return ((float*)this)[nIndex];
	}

	constexpr float& operator[](std::size_t nIndex)
	{
		return ((float*)this)[nIndex];
	}

	bool operator==(const Vector2D& vecBase) const
	{
		return this->IsEqual(vecBase);
	}

	bool operator!=(const Vector2D& vecBase) const
	{
		return !this->IsEqual(vecBase);
	}

	constexpr Vector2D& operator=(const Vector2D& vecBase)
	{
		this->x = vecBase.x; this->y = vecBase.y;
		return *this;
	}

	constexpr Vector2D& operator+=(const Vector2D& vecBase)
	{
		this->x += vecBase.x; this->y += vecBase.y;
		return *this;
	}

	constexpr Vector2D& operator-=(const Vector2D& vecBase)
	{
		this->x -= vecBase.x; this->y -= vecBase.y;
		return *this;
	}

	constexpr Vector2D& operator*=(const Vector2D& vecBase)
	{
		this->x *= vecBase.x; this->y *= vecBase.y;
		return *this;
	}

	constexpr Vector2D& operator/=(const Vector2D& vecBase)
	{
		this->x /= vecBase.x; this->y /= vecBase.y;
		return *this;
	}

	constexpr Vector2D& operator+=(float flAdd)
	{
		this->x += flAdd; this->y += flAdd;
		return *this;
	}

	constexpr Vector2D& operator-=(float flSubtract)
	{
		this->x -= flSubtract; this->y -= flSubtract;
		return *this;
	}

	constexpr Vector2D& operator*=(float flMultiply)
	{
		this->x *= flMultiply; this->y *= flMultiply;
		return *this;
	}

	constexpr Vector2D& operator/=(float flDivide)
	{
		this->x /= flDivide; this->y /= flDivide;
		return *this;
	}

	constexpr bool operator<(const Vector2D& vecOther) const
	{
		return (x < vecOther.x && y < vecOther.y);
	}

	constexpr bool operator>(const Vector2D& vecOther) const
	{
		return (x > vecOther.x && y > vecOther.y);
	}

	Vector2D operator+(const Vector2D& vecAdd) const
	{
		return Vector2D(this->x + vecAdd.x, this->y + vecAdd.y);
	}

	Vector2D operator-(const Vector2D& vecSubtract) const
	{
		return Vector2D(this->x - vecSubtract.x, this->y - vecSubtract.y);
	}

	Vector2D operator*(const Vector2D& vecMultiply) const
	{
		return Vector2D(this->x * vecMultiply.x, this->y * vecMultiply.y);
	}

	Vector2D operator/(const Vector2D& vecDivide) const {
		return Vector2D(this->x / vecDivide.x, this->y / vecDivide.y);
	}

	Vector2D operator+(float flAdd) const
	{
		return Vector2D(this->x + flAdd, this->y + flAdd);
	}

	Vector2D operator-(float flSubtract) const
	{
		return Vector2D(this->x - flSubtract, this->y - flSubtract);
	}

	Vector2D operator*(float flMultiply) const
	{
		return Vector2D(this->x * flMultiply, this->y * flMultiply);
	}

	Vector2D operator/(float flDivide) const
	{
		return Vector2D(this->x / flDivide, this->y / flDivide);
	}

	bool IsEqual(const Vector2D& vecEqual) const
	{
		return (std::fabsf(this->x - vecEqual.x) < std::numeric_limits<float>::epsilon() &&
			std::fabsf(this->y - vecEqual.y) < std::numeric_limits<float>::epsilon());
	}

	bool IsZero() const
	{
		return (std::fpclassify(this->x) == FP_ZERO &&
			std::fpclassify(this->y) == FP_ZERO);
	}

	float Length() const
	{
		return std::sqrtf(this->x * this->x + this->y * this->y);
	}

	constexpr float LengthSqr() const
	{
		return (this->x * this->x + this->y * this->y);
	}

	float DistTo(const Vector2D& vecEnd) const
	{
		return (*this - vecEnd).Length();
	}

	float DistToSqr(const Vector2D& vecEnd) const
	{
		return (*this - vecEnd).LengthSqr();
	}

	void Normalize()
	{
		float flLength = this->Length();

		if (flLength != 0.0f)
			*this /= flLength;
		else
			this->x = this->y = 0.0f;
	}

public:
	float x, y;
};

class Vector
{
public:
	constexpr Vector(float x = 0.f, float y = 0.f, float z = 0.f) :
		x(x), y(y), z(z) {
	}

	constexpr Vector(const float* arrVector) :
		x(arrVector[0]), y(arrVector[1]), z(arrVector[2]) {
	}

	constexpr Vector(const Vector2D& vecBase2D) :
		x(vecBase2D.x), y(vecBase2D.y), z(0.0f) {
	}

	[[nodiscard]] bool IsValid() const
	{
		return std::isfinite(this->x) && std::isfinite(this->y) && std::isfinite(this->z);
	}

	constexpr void Invalidate()
	{
		this->x = this->y = this->z = std::numeric_limits<float>::infinity();
	}

	[[nodiscard]] float* data()
	{
		return reinterpret_cast<float*>(this);
	}

	[[nodiscard]] const float* data() const
	{
		return reinterpret_cast<float*>(const_cast<Vector*>(this));
	}

	float& operator[](const std::size_t i)
	{
		return this->data()[i];
	}

	const float& operator[](const std::size_t i) const
	{
		return this->data()[i];
	}

	bool operator==(const Vector& vecBase) const
	{
		return this->IsEqual(vecBase);
	}

	bool operator!=(const Vector& vecBase) const
	{
		return !this->IsEqual(vecBase);
	}

	constexpr Vector& operator=(const Vector& vecBase)
	{
		this->x = vecBase.x; this->y = vecBase.y; this->z = vecBase.z;
		return *this;
	}

	constexpr Vector& operator=(const Vector2D& vecBase2D)
	{
		this->x = vecBase2D.x; this->y = vecBase2D.y; this->z = 0.0f;
		return *this;
	}

	constexpr Vector& operator+=(const Vector& vecBase)
	{
		this->x += vecBase.x; this->y += vecBase.y; this->z += vecBase.z;
		return *this;
	}

	constexpr Vector& operator-=(const Vector& vecBase)
	{
		this->x -= vecBase.x; this->y -= vecBase.y; this->z -= vecBase.z;
		return *this;
	}

	constexpr Vector& operator*=(const Vector& vecBase)
	{
		this->x *= vecBase.x; this->y *= vecBase.y; this->z *= vecBase.z;
		return *this;
	}

	constexpr Vector& operator/=(const Vector& vecBase)
	{
		this->x /= vecBase.x; this->y /= vecBase.y; this->z /= vecBase.z;
		return *this;
	}

	constexpr Vector& operator+=(const float flAdd)
	{
		this->x += flAdd; this->y += flAdd; this->z += flAdd;
		return *this;
	}

	constexpr Vector& operator-=(const float flSubtract)
	{
		this->x -= flSubtract; this->y -= flSubtract; this->z -= flSubtract;
		return *this;
	}

	constexpr Vector& operator*=(const float flMultiply)
	{
		this->x *= flMultiply; this->y *= flMultiply; this->z *= flMultiply;
		return *this;
	}

	constexpr Vector& operator/=(const float flDivide)
	{
		this->x /= flDivide; this->y /= flDivide; this->z /= flDivide;
		return *this;
	}

	Vector operator+(const Vector& vecAdd) const
	{
		return Vector(this->x + vecAdd.x, this->y + vecAdd.y, this->z + vecAdd.z);
	}

	Vector operator-(const Vector& vecSubtract) const
	{
		return Vector(this->x - vecSubtract.x, this->y - vecSubtract.y, this->z - vecSubtract.z);
	}

	Vector operator*(const Vector& vecMultiply) const
	{
		return Vector(this->x * vecMultiply.x, this->y * vecMultiply.y, this->z * vecMultiply.z);
	}

	Vector operator/(const Vector& vecDivide) const
	{
		return Vector(this->x / vecDivide.x, this->y / vecDivide.y, this->z / vecDivide.z);
	}

	Vector operator+(const float flAdd) const
	{
		return Vector(this->x + flAdd, this->y + flAdd, this->z + flAdd);
	}

	Vector operator-(const float flSubtract) const
	{
		return Vector(this->x - flSubtract, this->y - flSubtract, this->z - flSubtract);
	}

	Vector operator*(const float flMultiply) const
	{
		return Vector(this->x * flMultiply, this->y * flMultiply, this->z * flMultiply);
	}

	Vector operator/(const float flDivide) const
	{
		return Vector(this->x / flDivide, this->y / flDivide, this->z / flDivide);
	}

	// negation-operator.
	Vector operator-() const
	{
		return Vector{ -this->x, -this->y, -this->z };
	}

	[[nodiscard]] bool IsEqual(const Vector& vecEqual, const float flErrorMargin = std::numeric_limits<float>::epsilon()) const
	{
		return (std::fabsf(this->x - vecEqual.x) < flErrorMargin &&
			std::fabsf(this->y - vecEqual.y) < flErrorMargin &&
			std::fabsf(this->z - vecEqual.z) < flErrorMargin);
	}

	[[nodiscard]] bool IsZero() const
	{
		return (std::fpclassify(this->x) == FP_ZERO &&
			std::fpclassify(this->y) == FP_ZERO &&
			std::fpclassify(this->z) == FP_ZERO);
	}

	void Zero()
	{
		this->x = 0.0f;
		this->y = 0.0f;
		this->z = 0.0f;
	}

	[[nodiscard]] Vector2D ToVector2D() const
	{
		return Vector2D(this->x, this->y);
	}

	[[nodiscard]] float Length() const
	{
		return std::sqrtf(this->LengthSqr());
	}

	[[nodiscard]] constexpr float LengthSqr() const
	{
		return DotProduct(*this);
	}

	[[nodiscard]] float Length2D() const
	{
		return std::sqrtf(this->Length2DSqr());
	}

	[[nodiscard]] constexpr float Length2DSqr() const
	{
		return (this->x * this->x + this->y * this->y);
	}

	[[nodiscard]] float DistTo(const Vector& vecEnd) const
	{
		return (*this - vecEnd).Length();
	}

	[[nodiscard]] constexpr float DistToSqr(const Vector& vecEnd) const
	{
		return (*this - vecEnd).LengthSqr();
	}

	Vector Normalize()
	{
		this->x = std::isfinite(this->x) ? std::remainder(this->x, 360.f) : 0.f;
		this->y = std::isfinite(this->y) ? std::remainder(this->y, 360.f) : 0.f;
		this->z = 0.f;
		return *this;
	}

	[[nodiscard]] Vector Normalized() const
	{
		Vector vecOut = *this;
		vecOut.NormalizeInPlace();
		return vecOut;
	}

	float NormalizeInPlace()
	{
		const float flLength = this->Length();
		const float flRadius = 1.0f / (flLength + std::numeric_limits<float>::epsilon());

		this->x *= flRadius;
		this->y *= flRadius;
		this->z *= flRadius;

		return flLength;
	}

	Vector NormalizeGarbage() 
	{
		Vector vecOut{};
		float flLength = this->Length();

		if (flLength != 0)
		{
			vecOut.x = x / flLength;
			vecOut.y = y / flLength;
			vecOut.z = z / flLength;
		}
		else 
		{
			vecOut.x = vecOut.y = 0.0f;
			vecOut.z = 1.0f;
		}

		return vecOut;
	}

	[[nodiscard]] constexpr float DotProduct(const Vector& vecDot) const
	{
		return (this->x * vecDot.x + this->y * vecDot.y + this->z * vecDot.z);
	}

	[[nodiscard]] constexpr Vector CrossProduct(const Vector& vecCross) const
	{
		return Vector(this->y * vecCross.z - this->z * vecCross.y, this->z * vecCross.x - this->x * vecCross.z, this->x * vecCross.y - this->y * vecCross.x);
	}

	[[nodiscard]] float Delta(const Vector& vecOther)
	{
		Vector vecDelta = Vector(0, 0, 0);

		vecDelta.x = x - vecOther.x;
		vecDelta.y = y - vecOther.y;
		vecDelta.z = z - vecOther.z;

		return vecDelta.Length2D();
	}

	[[nodiscard]] QAngle ToAngles() const;

	/// @returns: transformed vector by given transformation matrix
	[[nodiscard]] Vector Transform(const Matrix3x4_t& matTransform) const;
	// @returns: rotated vector by given transformation matrix
	[[nodiscard]] Vector Rotate(const Matrix3x4_t& matTransform) const;

public:
	float x, y, z;
};

class Vector4D
{
public:
	constexpr Vector4D(float x = 0.f, float y = 0.f, float z = 0.f, float w = 0.f) :
		x(x), y(y), z(z), w(w) {
	}

public:
	float x, y, z, w;
};

class __declspec(align(16)) VectorAligned : public Vector
{
public:
	VectorAligned() = default;

	explicit VectorAligned(const Vector& vecBase)
	{
		this->x = vecBase.x; this->y = vecBase.y; this->z = vecBase.z; this->w = 0.f;
	}

	constexpr VectorAligned& operator=(const Vector& vecBase)
	{
		this->x = vecBase.x; this->y = vecBase.y; this->z = vecBase.z; this->w = 0.f;
		return *this;
	}

public:
	float w;
};