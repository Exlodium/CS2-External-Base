#include "../Includes.h"

bool SchemaSystem::Setup()
{
    std::uintptr_t uSchemaInterfaceAddress = g_Memory.PatterScan(SCHEMASYSTEM_DLL, X("48 8D 05 ? ? ? ? C3 CC CC CC CC CC CC CC CC 48 89 5C 24 ? 48 89 74 24 ? 48 89 7C 24 ? 4C 89 74 24 ?"), EPatternScanFlags::SCAN_RESOLVE_RIP, 0x3, 0x7);
    std::uintptr_t uSchemaSystemScopeArrayPtr = 0U;
    if (!g_Memory.ReadMemoryRaw(uSchemaInterfaceAddress + CS_OFFSETOF(CSchemaSystem, m_pScopeArray), &uSchemaSystemScopeArrayPtr, sizeof(std::uintptr_t)))
    {
        std::cout << X("Failed to read scope array ptr") << std::endl;
        return false;
    }
  
    int nScopeSize = g_Memory.ReadMemory<int>(uSchemaInterfaceAddress + CS_OFFSETOF(CSchemaSystem, m_uScopeSize));
    void** ppScopeArray = new void* [nScopeSize];

    if (!g_Memory.ReadMemoryRaw(uSchemaSystemScopeArrayPtr, ppScopeArray, (nScopeSize * sizeof(void*))))
    {
        std::cout << X("Failed to read scope array") << std::endl;
        return false;
    }

    for (std::uint16_t i = 0U; i <= nScopeSize; ++i)
    {
        CSchemaSystemTypeScope schemaScope{};
        if (!g_Memory.ReadMemoryRaw(ppScopeArray[i], &schemaScope, sizeof(CSchemaSystemTypeScope)) || !schemaScope.m_pDeclaredClasses)
            continue;

        CSchemaDeclaredClassEntry* pDeclaredClassEntries = new CSchemaDeclaredClassEntry[schemaScope.m_uNumDeclaredClasses + 1U];
        if (!g_Memory.ReadMemoryRaw(schemaScope.m_pDeclaredClasses, pDeclaredClassEntries, (schemaScope.m_uNumDeclaredClasses + 1U) * sizeof(CSchemaDeclaredClassEntry)))
            continue;
     
        if (strcmp(schemaScope.m_szName, X("client.dll")) == 0)
        {     
            for (std::uint16_t j = 0U; j < schemaScope.m_uNumDeclaredClasses; ++j)
            {
                CSchemaDeclaredClass declaredClass{ };
                if (!g_Memory.ReadMemoryRaw(pDeclaredClassEntries[j].m_pDeclaredClass, &declaredClass, sizeof(CSchemaDeclaredClass)))
                    continue;

                CSchemaClass schemaClass{ };
                if (!g_Memory.ReadMemoryRaw(declaredClass.m_Class, &schemaClass, sizeof(CSchemaClass)))
                    continue;

                char szClassName[128]{};
                if (!g_Memory.ReadMemoryRaw((void*)(declaredClass.m_szName), szClassName, sizeof(szClassName)))
                    continue;

                std::uintptr_t uClassFieldsPtr = reinterpret_cast<uintptr_t>(schemaClass.m_pFields);
                if (uClassFieldsPtr)
                {
                    for (std::uint16_t k = 0; k < schemaClass.m_uNumFields; ++k)
                    {
                        CSchemaField schemaField = g_Memory.ReadMemory<CSchemaField>(uClassFieldsPtr + (sizeof(CSchemaField) * k));
                        if (!schemaField.m_pType)
                            continue;

                        char szFieldName[128] = { 0 };
                        if (!g_Memory.ReadMemoryRaw((void*)(schemaField.m_szName), szFieldName, sizeof(szFieldName)))
                            continue;

                        const std::string strSchemaField = std::vformat(X("{}->{}"), std::make_format_args(szClassName, szFieldName));
                        m_mapSchemaOffsets[FNV1A::Hash(strSchemaField.c_str())] = schemaField.m_uOffset;
                    }
                }
            }
        }    
    }

    delete[] ppScopeArray;

	return m_mapSchemaOffsets.size() > 0;
}