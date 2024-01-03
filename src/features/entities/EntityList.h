#pragma once

enum EEntityType : unsigned int
{
	ENTITY_INVALID = 0,
	ENTITY_PLAYER,
};

struct EntityObject_t
{
	EntityObject_t(CBaseEntity* pEntity, int nIndex, EEntityType eEntityType) :
		m_pEntity(pEntity), m_nIndex(nIndex), m_eType(eEntityType) { }

	CBaseEntity* m_pEntity = nullptr;
	int				        m_nIndex = 0;
	EEntityType		        m_eType = EEntityType::ENTITY_INVALID;
};

namespace EntityList
{
	inline std::vector<EntityObject_t> m_vecEntities;

	inline void UpdateEntities()
	{
		// clear list from previous tick
		// @NOTE: You can optimize this by checking if entity is still valid
		m_vecEntities.clear();

		// skip first entity, it's always world
		for (int nIndex = 1; nIndex < Interfaces::m_GlobalVariables.m_nMaxClients; nIndex++)
		{
			CBaseEntity* pBaseEntity = CBaseEntity::GetBaseEntity(nIndex);
			if (!pBaseEntity)
				continue;

			/*if (pBaseEntity->GetSchemaName() == X("CCSPlayerController"))
			{
				m_vecEntities.emplace_back(EntityObject_t(pBaseEntity, nIndex, EEntityType::ENTITY_PLAYER));
				continue;
			}
			else
				continue;*/ // undesired entity, just skip them
		}
	}
}