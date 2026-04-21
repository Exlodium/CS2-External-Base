#include "../../Includes.h"

void EntityList::UpdateEntities()
{
	// clear list from previous tick
	// @NOTE: You can optimize this by checking if entity is still valid
	m_vecEntities.clear();

	for(CEntityIdentity* pEntity = g_Interfaces.m_GameEntitySystem.m_pLast; pEntity != nullptr; pEntity = pEntity->m_pNext())
	{
		C_BaseEntity* pBaseEntity = reinterpret_cast<C_BaseEntity*>(pEntity->m_pInstance());
		if (!pBaseEntity)
			continue;

		const FNV1A_t uSchemaNameHash = FNV1A::Hash(pBaseEntity->GetSchemaName().c_str());
		switch (uSchemaNameHash)
		{
			case FNV1A::HashConst("CCSPlayerController"):
			{
				m_vecEntities.emplace_back(EntityObject_t(pBaseEntity, pBaseEntity->GetRefEHandle().GetEntryIndex(), EEntityType::ENTITY_PLAYER));
				break;
			}
		}	
	}
}