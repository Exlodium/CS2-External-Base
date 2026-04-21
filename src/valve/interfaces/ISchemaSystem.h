#pragma once

class CSchemaField
{
public:
	const char* m_szName;
	void* m_pType;
	std::uint32_t m_uOffset;
	std::uint32_t m_uMetadataSize;
	void* m_pMetadata;
};

struct SchemaClassInfoData_t
{
	MEM_PAD(0x8);
	const char* m_szName;
	const char* m_szProjectName;
	const char* m_szCPPName;
	int m_nSize;									
	std::int16_t m_nFieldCount;						
	std::int16_t m_nStaticMetadataCount;			
	std::uint8_t m_uAlignment;						
	std::uint8_t m_uBaseClassCount;					
	std::int16_t m_nMultipleInheritanceDepth;		
	std::int16_t m_nSingleInheritanceDepth;
	MEM_PAD(0x2);
	CSchemaField* m_pFields;
	MEM_PAD(0x38);
};

class CSchemaDeclaredClass
{
public:
	MEM_PAD(0x8);
	const char* m_szName;
	const char* m_szModuleName;
	const char* m_szUnknownStr;
	SchemaClassInfoData_t* m_pClassInfoData;
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
	MEM_PAD(0x368);
	std::uint16_t m_uNumDeclaredClasses;
	MEM_PAD(0x6);
	CSchemaDeclaredClassEntry* m_pDeclaredClasses;
};

class CSchemaSystem
{
public:
	MEM_PAD(0x190);
	int m_nScopeSize;
	MEM_PAD(0x4);
	CSchemaSystemTypeScope** m_pScopeArray;
};