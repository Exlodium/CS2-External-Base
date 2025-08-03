#include "../Includes.h"

bool SchemaSystem::Setup()
{
	std::uintptr_t uSchemaInterface = g_Memory.PatterScan(SCHEMASYSTEM_DLL, X("48 8D 05 ? ? ? ? C3 CC CC CC CC CC CC CC CC 48 89 5C 24 ? 48 89 74 24 ? 48 89 7C 24 ? 4C 89 74 24 ?"), EPatternScanFlags::SCAN_RESOLVE_RIP, 0x3, 0x7);
	std::uintptr_t uListElement = g_Memory.ReadMemory<std::uintptr_t>(uSchemaInterface + 0x190);
	
	CSchemaSystemTypeScope* pTypeScope = g_Memory.ReadMemory<CSchemaSystemTypeScope*>(uListElement + (0x10 * 0x8));
	CUtlTSHash< CSchemaClassBindingBase*, 256, unsigned int > pTable = pTypeScope->GetBindingsTable();

	std::unique_ptr<UtlTSHashHandle_t[]> pElements = std::make_unique_for_overwrite< UtlTSHashHandle_t[] >(pTable.Count());
	const int nElements = pTable.GetElements(0, pTable.Count(), pElements.get());

	for (int nElementIndex = 0; nElementIndex < nElements; nElementIndex++)
	{
		const UtlTSHashHandle_t hElement = pElements[nElementIndex];
		if (!hElement)
			continue;

		CSchemaClassBindingBase* pClassBinding = pTable.Element(hElement);
		if (!pClassBinding)
			continue;

		if (!pClassBinding->GetNumFields())
			continue;

		CSchemaField* pSchemaField = &pClassBinding->GetFields()[0];
		for (std::uint16_t uFieldIndex = 0; uFieldIndex < pClassBinding->GetNumFields(); uFieldIndex++)
		{
			if (!pSchemaField)
				continue;

			const std::string strClassBindingName = pClassBinding->GetName();
			const std::string strSchemaFieldName = pSchemaField->GetName();
			const std::string strSchemaField = std::vformat(X("{}->{}"), std::make_format_args(strClassBindingName, strSchemaFieldName));

			m_mapSchemaOffsets[FNV1A::Hash(strSchemaField.c_str())] = pSchemaField->GetOffset();

			pSchemaField = reinterpret_cast<CSchemaField*>(std::uintptr_t(pSchemaField) + 0x20);
		}
	}

	return m_mapSchemaOffsets.size() > 0;
}