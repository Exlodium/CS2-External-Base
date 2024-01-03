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
		return g_Memory.Read< CUtlTSHash< CSchemaClassBindingBase*, 256, unsigned int > >(std::uintptr_t(this) + 0x588);
	};
};

namespace Schema
{
	bool Setup(const wchar_t* wszFileName);

	/* @section: get */
	// get offset of the field in the class
	// @note: only client.dll class & fields
	[[nodiscard]] std::uint32_t GetOffset(const FNV1A_t uHashedFieldName);
}