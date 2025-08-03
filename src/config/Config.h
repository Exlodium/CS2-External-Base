#pragma once

#define C_ADD_VARIABLE( Type, szName, pDefault ) const std::uint32_t szName = Config::AddVariable<Type>(FNV1A::HashConst(#szName), FNV1A::HashConst(#Type), pDefault);
#define C_ADD_VARIABLE_VECTOR( Type, uSize, szName, pDefault ) const std::uint32_t szName = Config::AddVariable<std::vector<Type>>(FNV1A::HashConst(#szName), FNV1A::HashConst("std::vector<" #Type ">"), Config::GetFilledVector<Type, uSize>(pDefault));
#define C_INVALID_VARIABLE (std::size_t)(-1)

struct KeyBind_t
{
	KeyBind_t() = default;

	explicit KeyBind_t(const std::uint32_t uKey, const std::uint32_t uMode)
		: m_uKey(uKey), m_uMode(uMode) {
	}

	std::uint32_t m_uKey = 0;
	std::uint32_t m_uMode = 0;
};

struct VariableObject_t
{
	VariableObject_t() = default;

	explicit VariableObject_t(const FNV1A_t uNameHash, const FNV1A_t uTypeHash, const std::any pDefault)
		: uNameHash(uNameHash), uTypeHash(uTypeHash), pValue(pDefault) {
	}

	~VariableObject_t() = default;

	template<typename T>
	T& Get()
	{
		return *(T*)(std::any_cast<T>(&pValue));
	}

	template<typename T>
	void Set(T value)
	{
		pValue.emplace<T>(value);
	}

	FNV1A_t		uNameHash;
	FNV1A_t		uTypeHash;
	std::any	pValue;
};

// @credits: ducarii
// @credits: qo0 (https://github.com/rollraw/qo0-csgo/blob/v1/base/core/config.h)	
namespace Config
{
	bool Setup(std::string_view szDefaultFileName);
	bool Save(std::string_view szFileName);
	bool Load(std::string_view szFileName);
	void Remove(std::string_view szFileName);
	void Refresh();

	std::size_t GetVariableIndex(const FNV1A_t uNameHash);
	std::filesystem::path GetWorkingPath();

	const std::filesystem::path m_fsConfigPath = GetWorkingPath() / X("Configs");
	const std::filesystem::path m_fsMapPath = GetWorkingPath() / X("Maps");

	inline std::deque<std::string> m_vecFileNames = { };
	inline std::vector<VariableObject_t> m_vecVariables = { };

	template <typename T>
	T& Get(const std::uint32_t nIndex)
	{
		return m_vecVariables.at(nIndex).Get<T>();
	}

	template <typename T>
	std::uint32_t AddVariable(const FNV1A_t uNameHash, const FNV1A_t uTypeHash, T pDefault)
	{
		m_vecVariables.emplace_back(uNameHash, uTypeHash, std::make_any<T>(pDefault));
		return m_vecVariables.size() - 1U;
	}

	template <typename T, std::size_t S>
	std::vector<T> GetFilledVector(const T& fill)
	{
		std::vector<T> vecTemp(S);
		std::fill(vecTemp.begin(), vecTemp.begin() + S, fill);
		return vecTemp;
	}
}