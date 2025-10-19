#include "../../Includes.h"

void EntityList::UpdateEntities()
{
	// clear list from previous tick
	// @NOTE: You can optimize this by checking if entity is still valid
	m_vecEntities.clear();

	// skip first entity, it's always world
	for (int nIndex = 1; nIndex < 1024; nIndex++)
	{
		C_BaseEntity* pBaseEntity = C_BaseEntity::GetBaseEntity(nIndex);
		if (!pBaseEntity)
			continue;

		const FNV1A_t uSchemaNameHash = FNV1A::Hash(pBaseEntity->GetSchemaName().c_str());
		switch (uSchemaNameHash)
		{
			case FNV1A::HashConst("CCSPlayerController"):
			{
				m_vecEntities.emplace_back(EntityObject_t(pBaseEntity, nIndex, EEntityType::ENTITY_PLAYER));
				break;
			}
		}	
	}
}