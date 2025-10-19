#pragma once

enum EEntityType : unsigned int
{
	ENTITY_INVALID = 0,
	ENTITY_PLAYER,
};

struct EntityObject_t
{
	EntityObject_t(C_BaseEntity* pEntity, int nIndex, EEntityType eEntityType) :
		m_pEntity(pEntity), m_nIndex(nIndex), m_eType(eEntityType) {
	}

	EntityObject_t(C_BaseEntity* pEntity, int nIndex, EEntityType eEntityType, FNV1A_t uHashedName, float flSpawnTime = 0.0f) :
		m_pEntity(pEntity), m_nIndex(nIndex), m_eType(eEntityType), m_uHashedName(uHashedName), m_flSpawnTime(flSpawnTime) {
	}

	C_BaseEntity*	m_pEntity = nullptr;
	int				m_nIndex = 0;
	EEntityType		m_eType = EEntityType::ENTITY_INVALID;
	FNV1A_t			m_uHashedName = FNV1A::HashConst("");
	float			m_flSpawnTime = 0.0f;
};

namespace EntityList
{
	void UpdateEntities();
	
	inline std::shared_mutex m_mtxEntities{ };
	inline std::vector<EntityObject_t> m_vecEntities{ };
}