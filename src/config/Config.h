#pragma once

#pragma region config_definitions
#define C_ADD_VARIABLE( Type, szName, pDefault ) const std::uint32_t szName = Config::AddVariable<Type>(FNV1A::HashConst(#szName), FNV1A::HashConst(#Type), pDefault);
#define C_ADD_VARIABLE_VECTOR( Type, uSize, szName, pDefault ) const std::uint32_t szName = Config::AddVariable<std::vector<Type>>(FNV1A::HashConst(#szName), FNV1A::HashConst("std::vector<" #Type ">"), Config::GetFilledVector<Type, uSize>(pDefault));
#define C_INVALID_VARIABLE (std::size_t)(-1)
#pragma endregion

struct KeyBind_t
{
	KeyBind_t() = default;
	
	explicit KeyBind_t(const unsigned int iKey, const unsigned int iMode)
		: m_iKey(iKey), m_iMode(iMode) { }

	unsigned int m_iKey = 0;
	unsigned int m_iMode = 0;
};

struct VariableObject_t
{
	VariableObject_t() = default;

	explicit VariableObject_t(const FNV1A_t uNameHash, const FNV1A_t uTypeHash, const std::any pDefault)
		: uNameHash(uNameHash), uTypeHash(uTypeHash), pValue(pDefault) { }

	~VariableObject_t() = default;

	/* get casted variable value */
	template<typename T>
	T& Get()
	{
		return *(T*)(std::any_cast<T>(&pValue));
	}

	/* emplace casted variable value */
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
	// Main
	/* create directories, create and load default config */
	bool Setup(std::string_view szDefaultFileName);
	/* write values in config file */
	bool Save(std::string_view szFileName);
	/* read values from config file */
	bool Load(std::string_view szFileName);
	/* remove config file at given index */
	void Remove(std::string_view szFileName);
	/* loop through directory content and push config filenames to vector */
	void Refresh();

	// Get
	/* return variable index by hashed name */
	std::size_t GetVariableIndex(const FNV1A_t uNameHash);
	/* get path where output files will be saved (default: "%userprofile%\documents\.qo0") */
	std::filesystem::path GetWorkingPath();

	// Values
	/* default file path */
	const std::filesystem::path fsPath = GetWorkingPath() / X("Configs");

	/* all user filenames */
	inline std::deque<std::string> vecFileNames = { };
	/* configuration variables */
	inline std::vector<VariableObject_t> vecVariables = { };

	// Templates
	/* returns casted variable value at given index */
	template <typename T>
	T& Get( const std::uint32_t nIndex )
	{
		return vecVariables.at( nIndex ).Get<T>( );
	}

	/* add new configuration variable to massive */
	template <typename T>
	std::uint32_t AddVariable(const FNV1A_t uNameHash, const FNV1A_t uTypeHash, T pDefault)
	{
		vecVariables.emplace_back(uNameHash, uTypeHash, std::make_any<T>(pDefault));
		return vecVariables.size() - 1U;
	}

	template <typename T, std::size_t S>
	std::vector<T> GetFilledVector(const T& fill)
	{
		std::vector<T> vecTemp(S);
		std::fill(vecTemp.begin(), vecTemp.begin() + S, fill);
		return vecTemp;
	}
}