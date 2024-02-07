#pragma once

class CSchemaType
{
public:
	std::string GetName()
	{
		std::uintptr_t uAddress = g_Memory.Read<std::uintptr_t>(std::uintptr_t(this) + 0x8);
		return g_Memory.ReadString(uAddress);
	}
};

class CSchemaField
{
public:
	std::string GetName()
	{
		std::uintptr_t uAddress = g_Memory.Read<std::uintptr_t>(std::uintptr_t(this));
		return g_Memory.ReadString(uAddress);
	}

	CSchemaType* GetSchemaType()
	{
		return g_Memory.Read< CSchemaType* >(std::uintptr_t(this) + 0x8);
	}

	std::uint32_t GetOffset()
	{
		return g_Memory.Read< std::uint32_t >(std::uintptr_t(this) + 0x10);
	}
};

class CSchemaClassBindingBase
{
public:
	std::string GetName()
	{
		std::uintptr_t uAddress = g_Memory.Read<std::uintptr_t>(std::uintptr_t(this) + 0x8);
		return g_Memory.ReadString(uAddress);
	}

	std::string GetProjectName()
	{
		return g_Memory.ReadString(reinterpret_cast<std::uintptr_t>(this) + 0x10);
	}

	std::uint32_t GetSize()
	{
		return g_Memory.Read< std::uint32_t >(std::uintptr_t(this) + 0x18);
	}

	std::uint16_t GetNumFields()
	{
		return g_Memory.Read< std::uint16_t >(std::uintptr_t(this) + 0x1C);
	}

	CSchemaField* GetFields()
	{
		return g_Memory.Read< CSchemaField* >(std::uintptr_t(this) + 0x28);
	}
};

class CSchemaSystemTypeScope
{
public:
	CUtlTSHash< CSchemaClassBindingBase*, 256, unsigned int > GetBindingsTable()
	{
		return g_Memory.Read< CUtlTSHash< CSchemaClassBindingBase*, 256, unsigned int > >(std::uintptr_t(this) + 0x5B8);
	};
};

namespace Schema
{
	bool Setup(const wchar_t* wszFileName);

	/* @section: get */
	// get offset of the field in the class
	// @note: only client.dll class & fields
	[[nodiscard]] std::uintptr_t GetOffset(const FNV1A_t uHashedFieldName);
}

#define ADD_SCHEMA(TYPE, NAME, VARIABLE, ADDITIONAL) \
[[nodiscard]] __forceinline TYPE NAME() noexcept { \
    static std::uintptr_t uOffset = Schema::GetOffset(FNV1A::Hash(VARIABLE)); \
    return g_Memory.Read<TYPE>(reinterpret_cast<std::uintptr_t>(this) + uOffset + ADDITIONAL); \
}

#define ADD_OFFSET(TYPE, NAME, OFFSET) \
[[nodiscard]] __forceinline TYPE NAME() noexcept { \
	static std::uintptr_t uOffset = OFFSET; \
	return g_Memory.Read<TYPE>(reinterpret_cast<std::uintptr_t>(this) + uOffset); \
}

#define SCHEMA_OFFSET(TYPE, NAME, FIELD, ADDITIONAL) ADD_SCHEMA(TYPE, NAME, FIELD, ADDITIONAL)
#define SCHEMA(TYPE, NAME, FIELD) ADD_SCHEMA(TYPE, NAME, FIELD, 0U)
#define OFFSET(TYPE, NAME, OFFSET) ADD_OFFSET(TYPE, NAME, OFFSET)
