#pragma once

class CSchemaType
{
public:
	std::string GetName()
	{
		std::uintptr_t uAddress = g_Memory.ReadMemory<std::uintptr_t>(std::uintptr_t(this) + 0x8);
		return g_Memory.ReadMemoryString(uAddress);
	}
};

class CSchemaField
{
public:
	std::string GetName()
	{
		std::uintptr_t uAddress = g_Memory.ReadMemory<std::uintptr_t>(std::uintptr_t(this));
		return g_Memory.ReadMemoryString(uAddress);
	}

	CSchemaType* GetSchemaType()
	{
		return g_Memory.ReadMemory<CSchemaType*>(std::uintptr_t(this) + 0x8);
	}

	std::uint32_t GetOffset()
	{
		return g_Memory.ReadMemory<std::uint32_t>(std::uintptr_t(this) + 0x10);
	}
};

class CSchemaClassBindingBase
{
public:
	std::string GetName()
	{
		std::uintptr_t uAddress = g_Memory.ReadMemory<std::uintptr_t>(std::uintptr_t(this) + 0x8);
		return g_Memory.ReadMemoryString(uAddress);
	}

	std::string GetProjectName()
	{
		return g_Memory.ReadMemoryString(reinterpret_cast<std::uintptr_t>(this) + 0x10);
	}

	std::uint32_t GetSize()
	{
		return g_Memory.ReadMemory<std::uint32_t>(std::uintptr_t(this) + 0x18);
	}

	std::uint16_t GetNumFields()
	{
		return g_Memory.ReadMemory<std::uint16_t>(std::uintptr_t(this) + 0x1C);
	}

	CSchemaField* GetFields()
	{
		return g_Memory.ReadMemory<CSchemaField*>(std::uintptr_t(this) + 0x28);
	}
};

class CSchemaSystemTypeScope
{
public:
	CUtlTSHash<CSchemaClassBindingBase*, 256, unsigned int> GetBindingsTable()
	{
		return g_Memory.ReadMemory<CUtlTSHash<CSchemaClassBindingBase*, 256, unsigned int>>(std::uintptr_t(this) + 0x500);
	};
};