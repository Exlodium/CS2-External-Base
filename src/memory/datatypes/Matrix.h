#pragma pack(push, 4)
using Matrix3x3_t = float[3][3];

struct Matrix3x4_t
{
	Matrix3x4_t() = default;

	constexpr Matrix3x4_t(
		const float m00, const float m01, const float m02, const float m03,
		const float m10, const float m11, const float m12, const float m13,
		const float m20, const float m21, const float m22, const float m23)
	{
		arrData[0][0] = m00;
		arrData[0][1] = m01;
		arrData[0][2] = m02;
		arrData[0][3] = m03;
		arrData[1][0] = m10;
		arrData[1][1] = m11;
		arrData[1][2] = m12;
		arrData[1][3] = m13;
		arrData[2][0] = m20;
		arrData[2][1] = m21;
		arrData[2][2] = m22;
		arrData[2][3] = m23;
	}

	constexpr Matrix3x4_t(const Vector& vecForward, const Vector& vecLeft, const Vector& vecUp, const Vector& vecOrigin)
	{
		SetForward(vecForward);
		SetLeft(vecLeft);
		SetUp(vecUp);
		SetOrigin(vecOrigin);
	}

	[[nodiscard]] float* operator[](const int nIndex)
	{
		return arrData[nIndex];
	}

	[[nodiscard]] const float* operator[](const int nIndex) const
	{
		return arrData[nIndex];
	}

	constexpr void SetForward(const Vector& vecForward)
	{
		arrData[0][0] = vecForward.x;
		arrData[1][0] = vecForward.y;
		arrData[2][0] = vecForward.z;
	}

	constexpr void SetLeft(const Vector& vecLeft)
	{
		arrData[0][1] = vecLeft.x;
		arrData[1][1] = vecLeft.y;
		arrData[2][1] = vecLeft.z;
	}

	constexpr void SetUp(const Vector& vecUp)
	{
		arrData[0][2] = vecUp.x;
		arrData[1][2] = vecUp.y;
		arrData[2][2] = vecUp.z;
	}

	constexpr void SetOrigin(const Vector& vecOrigin)
	{
		arrData[0][3] = vecOrigin.x;
		arrData[1][3] = vecOrigin.y;
		arrData[2][3] = vecOrigin.z;
	}

	[[nodiscard]] constexpr Vector GetForward() const
	{
		return { arrData[0][0], arrData[1][0], arrData[2][0] };
	}

	[[nodiscard]] constexpr Vector GetLeft() const
	{
		return { arrData[0][1], arrData[1][1], arrData[2][1] };
	}

	[[nodiscard]] constexpr Vector GetUp() const
	{
		return { arrData[0][2], arrData[1][2], arrData[2][2] };
	}

	[[nodiscard]] constexpr Vector GetOrigin() const
	{
		return { arrData[0][3], arrData[1][3], arrData[2][3] };
	}

	constexpr void Invalidate()
	{
		for (auto& arrSubData : arrData)
		{
			for (auto& flData : arrSubData)
				flData = std::numeric_limits<float>::infinity();
		}
	}

	/// concatenate transformations of two matrices into one
	/// @returns: matrix with concatenated transformations
	[[nodiscard]] constexpr Matrix3x4_t ConcatTransforms(const Matrix3x4_t& matOther) const
	{
		return {
			arrData[0][0] * matOther.arrData[0][0] + arrData[0][1] * matOther.arrData[1][0] + arrData[0][2] * matOther.arrData[2][0],
			arrData[0][0] * matOther.arrData[0][1] + arrData[0][1] * matOther.arrData[1][1] + arrData[0][2] * matOther.arrData[2][1],
			arrData[0][0] * matOther.arrData[0][2] + arrData[0][1] * matOther.arrData[1][2] + arrData[0][2] * matOther.arrData[2][2],
			arrData[0][0] * matOther.arrData[0][3] + arrData[0][1] * matOther.arrData[1][3] + arrData[0][2] * matOther.arrData[2][3] + arrData[0][3],

			arrData[1][0] * matOther.arrData[0][0] + arrData[1][1] * matOther.arrData[1][0] + arrData[1][2] * matOther.arrData[2][0],
			arrData[1][0] * matOther.arrData[0][1] + arrData[1][1] * matOther.arrData[1][1] + arrData[1][2] * matOther.arrData[2][1],
			arrData[1][0] * matOther.arrData[0][2] + arrData[1][1] * matOther.arrData[1][2] + arrData[1][2] * matOther.arrData[2][2],
			arrData[1][0] * matOther.arrData[0][3] + arrData[1][1] * matOther.arrData[1][3] + arrData[1][2] * matOther.arrData[2][3] + arrData[1][3],

			arrData[2][0] * matOther.arrData[0][0] + arrData[2][1] * matOther.arrData[1][0] + arrData[2][2] * matOther.arrData[2][0],
			arrData[2][0] * matOther.arrData[0][1] + arrData[2][1] * matOther.arrData[1][1] + arrData[2][2] * matOther.arrData[2][1],
			arrData[2][0] * matOther.arrData[0][2] + arrData[2][1] * matOther.arrData[1][2] + arrData[2][2] * matOther.arrData[2][2],
			arrData[2][0] * matOther.arrData[0][3] + arrData[2][1] * matOther.arrData[1][3] + arrData[2][2] * matOther.arrData[2][3] + arrData[2][3]
		};
	}

	float arrData[3][4] = {};
};

#pragma pack(pop)

class alignas(16) Matrix3x4a_t : public Matrix3x4_t
{
public:
	Matrix3x4a_t() = default;

	constexpr Matrix3x4a_t(
		const float m00, const float m01, const float m02, const float m03,
		const float m10, const float m11, const float m12, const float m13,
		const float m20, const float m21, const float m22, const float m23)
	{
		arrData[0][0] = m00;
		arrData[0][1] = m01;
		arrData[0][2] = m02;
		arrData[0][3] = m03;
		arrData[1][0] = m10;
		arrData[1][1] = m11;
		arrData[1][2] = m12;
		arrData[1][3] = m13;
		arrData[2][0] = m20;
		arrData[2][1] = m21;
		arrData[2][2] = m22;
		arrData[2][3] = m23;
	}

	constexpr Matrix3x4a_t(const Matrix3x4_t& matSource)
	{
		*this = matSource;
	}

	constexpr Matrix3x4a_t& operator=(const Matrix3x4_t& matSource)
	{
		arrData[0][0] = matSource.arrData[0][0];
		arrData[0][1] = matSource.arrData[0][1];
		arrData[0][2] = matSource.arrData[0][2];
		arrData[0][3] = matSource.arrData[0][3];
		arrData[1][0] = matSource.arrData[1][0];
		arrData[1][1] = matSource.arrData[1][1];
		arrData[1][2] = matSource.arrData[1][2];
		arrData[1][3] = matSource.arrData[1][3];
		arrData[2][0] = matSource.arrData[2][0];
		arrData[2][1] = matSource.arrData[2][1];
		arrData[2][2] = matSource.arrData[2][2];
		arrData[2][3] = matSource.arrData[2][3];
		return *this;
	}

	/// concatenate transformations of two aligned matrices into one
	/// @returns: aligned matrix with concatenated transformations
	[[nodiscard]] Matrix3x4a_t ConcatTransforms(const Matrix3x4a_t& matOther) const
	{
		Matrix3x4a_t matOutput;
		CS_ASSERT((reinterpret_cast<std::uintptr_t>(this) & 15U) == 0 && (reinterpret_cast<std::uintptr_t>(&matOther) & 15U) == 0 && (reinterpret_cast<std::uintptr_t>(&matOutput) & 15U) == 0); // matrices aren't aligned

		__m128 thisRow0 = _mm_load_ps(this->arrData[0]);
		__m128 thisRow1 = _mm_load_ps(this->arrData[1]);
		__m128 thisRow2 = _mm_load_ps(this->arrData[2]);

		__m128 otherRow0 = _mm_load_ps(matOther.arrData[0]);
		__m128 otherRow1 = _mm_load_ps(matOther.arrData[1]);
		__m128 otherRow2 = _mm_load_ps(matOther.arrData[2]);

		__m128 outRow0 = _mm_add_ps(_mm_mul_ps(_mm_shuffle_ps(thisRow0, thisRow0, _MM_SHUFFLE(0, 0, 0, 0)), otherRow0), _mm_add_ps(_mm_mul_ps(_mm_shuffle_ps(thisRow0, thisRow0, _MM_SHUFFLE(1, 1, 1, 1)), otherRow1), _mm_mul_ps(_mm_shuffle_ps(thisRow0, thisRow0, _MM_SHUFFLE(2, 2, 2, 2)), otherRow2)));
		__m128 outRow1 = _mm_add_ps(_mm_mul_ps(_mm_shuffle_ps(thisRow1, thisRow1, _MM_SHUFFLE(0, 0, 0, 0)), otherRow0), _mm_add_ps(_mm_mul_ps(_mm_shuffle_ps(thisRow1, thisRow1, _MM_SHUFFLE(1, 1, 1, 1)), otherRow1), _mm_mul_ps(_mm_shuffle_ps(thisRow1, thisRow1, _MM_SHUFFLE(2, 2, 2, 2)), otherRow2)));
		__m128 outRow2 = _mm_add_ps(_mm_mul_ps(_mm_shuffle_ps(thisRow2, thisRow2, _MM_SHUFFLE(0, 0, 0, 0)), otherRow0), _mm_add_ps(_mm_mul_ps(_mm_shuffle_ps(thisRow2, thisRow2, _MM_SHUFFLE(1, 1, 1, 1)), otherRow1), _mm_mul_ps(_mm_shuffle_ps(thisRow2, thisRow2, _MM_SHUFFLE(2, 2, 2, 2)), otherRow2)));

		// add in translation vector
		constexpr std::uint32_t arrComponentMask[4] = { 0x0, 0x0, 0x0, 0xFFFFFFFF };
		outRow0 = _mm_add_ps(outRow0, _mm_and_ps(thisRow0, std::bit_cast<__m128>(arrComponentMask)));
		outRow1 = _mm_add_ps(outRow1, _mm_and_ps(thisRow1, std::bit_cast<__m128>(arrComponentMask)));
		outRow2 = _mm_add_ps(outRow2, _mm_and_ps(thisRow2, std::bit_cast<__m128>(arrComponentMask)));

		_mm_store_ps(matOutput.arrData[0], outRow0);
		_mm_store_ps(matOutput.arrData[1], outRow1);
		_mm_store_ps(matOutput.arrData[2], outRow2);
		return matOutput;
	}
};

static_assert(alignof(Matrix3x4a_t) == 16);

#pragma pack(push, 4)

struct ViewMatrix_t
{
	ViewMatrix_t() = default;

	constexpr ViewMatrix_t(
		const float m00, const float m01, const float m02, const float m03,
		const float m10, const float m11, const float m12, const float m13,
		const float m20, const float m21, const float m22, const float m23,
		const float m30, const float m31, const float m32, const float m33)
	{
		arrData[0][0] = m00;
		arrData[0][1] = m01;
		arrData[0][2] = m02;
		arrData[0][3] = m03;
		arrData[1][0] = m10;
		arrData[1][1] = m11;
		arrData[1][2] = m12;
		arrData[1][3] = m13;
		arrData[2][0] = m20;
		arrData[2][1] = m21;
		arrData[2][2] = m22;
		arrData[2][3] = m23;
		arrData[3][0] = m30;
		arrData[3][1] = m31;
		arrData[3][2] = m32;
		arrData[3][3] = m33;
	}

	constexpr ViewMatrix_t(const Matrix3x4_t& matFrom, const Vector4D& vecAdditionalRow = {})
	{
		arrData[0][0] = matFrom.arrData[0][0];
		arrData[0][1] = matFrom.arrData[0][1];
		arrData[0][2] = matFrom.arrData[0][2];
		arrData[0][3] = matFrom.arrData[0][3];
		arrData[1][0] = matFrom.arrData[1][0];
		arrData[1][1] = matFrom.arrData[1][1];
		arrData[1][2] = matFrom.arrData[1][2];
		arrData[1][3] = matFrom.arrData[1][3];
		arrData[2][0] = matFrom.arrData[2][0];
		arrData[2][1] = matFrom.arrData[2][1];
		arrData[2][2] = matFrom.arrData[2][2];
		arrData[2][3] = matFrom.arrData[2][3];
		arrData[3][0] = vecAdditionalRow.x;
		arrData[3][1] = vecAdditionalRow.y;
		arrData[3][2] = vecAdditionalRow.z;
		arrData[3][3] = vecAdditionalRow.w;
	}

	[[nodiscard]] float* operator[](const int nIndex)
	{
		return arrData[nIndex];
	}

	[[nodiscard]] const float* operator[](const int nIndex) const
	{
		return arrData[nIndex];
	}

	[[nodiscard]] const bool operator==(const ViewMatrix_t& viewOther) const
	{
		return (
			arrData[0][0] == viewOther.arrData[0][0] && arrData[0][1] == viewOther.arrData[0][1] && arrData[0][2] == viewOther.arrData[0][2] && arrData[0][3] == viewOther.arrData[0][3] &&
			arrData[1][0] == viewOther.arrData[1][0] && arrData[1][1] == viewOther.arrData[1][1] && arrData[1][2] == viewOther.arrData[1][2] && arrData[1][3] == viewOther.arrData[1][3] &&
			arrData[2][0] == viewOther.arrData[2][0] && arrData[2][1] == viewOther.arrData[2][1] && arrData[2][2] == viewOther.arrData[2][2] && arrData[2][3] == viewOther.arrData[2][3] &&
			arrData[3][0] == viewOther.arrData[3][0] && arrData[3][1] == viewOther.arrData[3][1] && arrData[3][2] == viewOther.arrData[3][2] && arrData[3][3] == viewOther.arrData[3][3]
			);
	}

	[[nodiscard]] const Matrix3x4_t& As3x4() const
	{
		return *reinterpret_cast<const Matrix3x4_t*>(this);
	}

	[[nodiscard]] Matrix3x4_t& As3x4()
	{
		return *reinterpret_cast<Matrix3x4_t*>(this);
	}

	constexpr ViewMatrix_t& operator+=(const ViewMatrix_t& matAdd)
	{
		for (std::uint8_t c = 0U; c < 4U; c++)
		{
			for (std::uint8_t r = 0U; r < 4U; r++)
				arrData[c][r] += matAdd[c][r];
		}

		return *this;
	}

	constexpr ViewMatrix_t& operator-=(const ViewMatrix_t& matSubtract)
	{
		for (std::uint8_t c = 0U; c < 4U; c++)
		{
			for (std::uint8_t r = 0U; r < 4U; r++)
				arrData[c][r] -= matSubtract[c][r];
		}

		return *this;
	}

	[[nodiscard]] constexpr Vector4D GetRow(const int nIndex) const
	{
		return { arrData[nIndex][0], arrData[nIndex][1], arrData[nIndex][2], arrData[nIndex][3] };
	}

	[[nodiscard]] constexpr Vector4D GetColumn(const int nIndex) const
	{
		return { arrData[0][nIndex], arrData[1][nIndex], arrData[2][nIndex], arrData[3][nIndex] };
	}

	constexpr void Identity()
	{
		for (std::uint8_t c = 0U; c < 4U; c++)
		{
			for (std::uint8_t r = 0U; r < 4U; r++)
				arrData[c][r] = (c == r) ? 1.0f : 0.0f;
		}
	}

	/// concatenate transformations of two matrices into one
	/// @returns: matrix with concatenated transformations
	[[nodiscard]] constexpr ViewMatrix_t ConcatTransforms(const ViewMatrix_t& matOther) const
	{
		return {
			arrData[0][0] * matOther.arrData[0][0] + arrData[0][1] * matOther.arrData[1][0] + arrData[0][2] * matOther.arrData[2][0] + arrData[0][3] * matOther.arrData[3][0],
			arrData[0][0] * matOther.arrData[0][1] + arrData[0][1] * matOther.arrData[1][1] + arrData[0][2] * matOther.arrData[2][1] + arrData[0][3] * matOther.arrData[3][1],
			arrData[0][0] * matOther.arrData[0][2] + arrData[0][1] * matOther.arrData[1][2] + arrData[0][2] * matOther.arrData[2][2] + arrData[0][3] * matOther.arrData[3][2],
			arrData[0][0] * matOther.arrData[0][3] + arrData[0][1] * matOther.arrData[1][3] + arrData[0][2] * matOther.arrData[2][3] + arrData[0][3] * matOther.arrData[3][3],

			arrData[1][0] * matOther.arrData[0][0] + arrData[1][1] * matOther.arrData[1][0] + arrData[1][2] * matOther.arrData[2][0] + arrData[1][3] * matOther.arrData[3][0],
			arrData[1][0] * matOther.arrData[0][1] + arrData[1][1] * matOther.arrData[1][1] + arrData[1][2] * matOther.arrData[2][1] + arrData[1][3] * matOther.arrData[3][1],
			arrData[1][0] * matOther.arrData[0][2] + arrData[1][1] * matOther.arrData[1][2] + arrData[1][2] * matOther.arrData[2][2] + arrData[1][3] * matOther.arrData[3][2],
			arrData[1][0] * matOther.arrData[0][3] + arrData[1][1] * matOther.arrData[1][3] + arrData[1][2] * matOther.arrData[2][3] + arrData[1][3] * matOther.arrData[3][3],

			arrData[2][0] * matOther.arrData[0][0] + arrData[2][1] * matOther.arrData[1][0] + arrData[2][2] * matOther.arrData[2][0] + arrData[2][3] * matOther.arrData[3][0],
			arrData[2][0] * matOther.arrData[0][1] + arrData[2][1] * matOther.arrData[1][1] + arrData[2][2] * matOther.arrData[2][1] + arrData[2][3] * matOther.arrData[3][1],
			arrData[2][0] * matOther.arrData[0][2] + arrData[2][1] * matOther.arrData[1][2] + arrData[2][2] * matOther.arrData[2][2] + arrData[2][3] * matOther.arrData[3][2],
			arrData[2][0] * matOther.arrData[0][3] + arrData[2][1] * matOther.arrData[1][3] + arrData[2][2] * matOther.arrData[2][3] + arrData[2][3] * matOther.arrData[3][3],

			arrData[3][0] * matOther.arrData[0][0] + arrData[3][1] * matOther.arrData[1][0] + arrData[3][2] * matOther.arrData[2][0] + arrData[3][3] * matOther.arrData[3][0],
			arrData[3][0] * matOther.arrData[0][1] + arrData[3][1] * matOther.arrData[1][1] + arrData[3][2] * matOther.arrData[2][1] + arrData[3][3] * matOther.arrData[3][1],
			arrData[3][0] * matOther.arrData[0][2] + arrData[3][1] * matOther.arrData[1][2] + arrData[3][2] * matOther.arrData[2][2] + arrData[3][3] * matOther.arrData[3][2],
			arrData[3][0] * matOther.arrData[0][3] + arrData[3][1] * matOther.arrData[1][3] + arrData[3][2] * matOther.arrData[2][3] + arrData[3][3] * matOther.arrData[3][3]
		};
	}

	float arrData[4][4] = {};
};

#pragma pack(pop)