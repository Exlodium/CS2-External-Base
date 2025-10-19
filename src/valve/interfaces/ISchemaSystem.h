#pragma once

class CSchemaField
{
public:
	const char* m_szName;
	void* m_pType;
	std::uint32_t m_uOffset;
	std::uint32_t m_uMetadataSize;
	void* m_nMetadata;
};

class CSchemaClass
{
public:
	MEM_PAD(0x8);
	const char* m_szName;
	const char* m_szModuleName;
	std::uint32_t m_uSize;
	std::uint16_t m_uNumFields;
	MEM_PAD(0x2);
	std::uint16_t m_uStaticSize;
	std::uint16_t m_uMetadataSize;
	MEM_PAD(0x4);
	CSchemaField* m_pFields;
};

class CSchemaDeclaredClass
{
public:
	MEM_PAD(0x8);
	const char* m_szName;
	const char* m_szModuleName;
	const char* m_szUnknownStr;
	CSchemaClass* m_Class;
};

class CSchemaDeclaredClassEntry
{
public:
	std::uint64_t m_nHash[2];
	CSchemaDeclaredClass* m_pDeclaredClass;
};

class CSchemaSystemTypeScope
{
public:
	MEM_PAD(0x8);
	char m_szName[256];
	MEM_PAD(0x338);
	std::uint16_t m_uNumDeclaredClasses;
	MEM_PAD(0x6);
	CSchemaDeclaredClassEntry* m_pDeclaredClasses;
};

class CSchemaSystem
{
public:
	MEM_PAD(0x188);
	std::uint64_t m_uScopeSize;
	CSchemaSystemTypeScope** m_pScopeArray;
};