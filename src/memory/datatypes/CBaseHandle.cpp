#include "../../Precompiled.h"

C_BaseEntity* CBaseHandle::Get() const
{
    if (!IsValid())
        return nullptr;

    C_BaseEntity* pEntity = C_BaseEntity::GetBaseEntity(GetEntryIndex());
    if (!pEntity || pEntity->GetRefEHandle() != *this)
        return nullptr;

    return pEntity;
}