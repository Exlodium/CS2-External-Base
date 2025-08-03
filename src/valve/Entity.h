#pragma once

class CEntityIdentity
{
public:
    
    [[nodiscard]] bool IsValid()
    {
        return nIndex() != INVALID_EHANDLE_INDEX;
    }

    [[nodiscard]] int GetEntryIndex()
    {
        if (!IsValid())
            return ENT_ENTRY_MASK;

        return nIndex() & ENT_ENTRY_MASK;
    }

    [[nodiscard]] int GetSerialNumber()
    {
        return nIndex() >> NUM_SERIAL_NUM_SHIFT_BITS;
    }

    OFFSET(std::uint32_t, nIndex, 0x10);
    SCHEMA(std::uint32_t, m_flags, "CEntityIdentity->m_flags");
};

class CEntityInstance
{
public:
    [[nodiscard]] CBaseHandle GetRefEHandle()
    {
        CEntityIdentity* pIdentity = m_pEntity();
        if (!pIdentity)
            return CBaseHandle();

        return CBaseHandle(pIdentity->GetEntryIndex(), pIdentity->GetSerialNumber() - (pIdentity->m_flags() & 1));
    }

    std::string GetSchemaName()
    {
        std::uintptr_t uEntityIdentity = g_Memory.ReadMemory<std::uintptr_t>(reinterpret_cast<std::uintptr_t>(this) + 0x10);
        if (!uEntityIdentity)
            return X("-");

        std::uintptr_t uEntityClassInfo = g_Memory.ReadMemory<std::uintptr_t>(uEntityIdentity + 0x08);
        std::uintptr_t uPointer1 = g_Memory.ReadMemory<std::uintptr_t>(uEntityClassInfo + 0x28);
        std::uintptr_t uPointer2 = g_Memory.ReadMemory<std::uintptr_t>(uPointer1 + 0x08);

        std::string strSchemaName = g_Memory.ReadMemoryString(uPointer2);
        if (strSchemaName.empty())
            return X("-");

        return strSchemaName;
    }

    OFFSET(CEntityIdentity*, m_pEntity, 0x10)
};

class CTransform
{
public:
    VectorAligned m_vecPosition;
    QuaternionAligned m_quatOrientation;
};

class CGameSceneNode
{
public:
    SCHEMA(bool, m_bDormant, "CGameSceneNode->m_bDormant");
    SCHEMA(Vector, m_vecAbsOrigin, "CGameSceneNode->m_vecAbsOrigin");
    SCHEMA(CTransform, m_nodeToWorld, "CGameSceneNode->m_nodeToWorld");
};

class CCollisionProperty
{
public:
    SCHEMA(Vector, m_vecMins, "CCollisionProperty->m_vecMins");
    SCHEMA(Vector, m_vecMaxs, "CCollisionProperty->m_vecMaxs");
};

class C_BaseEntity : public CEntityInstance
{
public:
    static C_BaseEntity* GetBaseEntity(int nIdx) noexcept
    {
        std::uintptr_t uListEntry = g_Memory.ReadMemory<std::uintptr_t>(g_Globals.m_uEntityList + (0x8 * ((nIdx & 0x7FFF) >> 0x9)) + 0x10);
        if (!uListEntry)
            return 0;

        C_BaseEntity* pEntity = g_Memory.ReadMemory<C_BaseEntity*>(uListEntry + 0x78 * (nIdx & 0x1FF));
        return pEntity;
    }

    SCHEMA(std::uint8_t, m_iTeamNum, "C_BaseEntity->m_iTeamNum");

    SCHEMA(std::int32_t, m_iHealth, "C_BaseEntity->m_iHealth");
    SCHEMA(std::int32_t, m_iMaxHealth, "C_BaseEntity->m_iMaxHealth");
    
    SCHEMA(std::uint32_t, m_fFlags, "C_BaseEntity->m_fFlags");

    SCHEMA(Vector, m_vecVelocity, "C_BaseEntity->m_vecVelocity");

    SCHEMA(CCollisionProperty*, m_pCollision, "C_BaseEntity->m_pCollision");
    SCHEMA(CGameSceneNode*, m_pGameSceneNode, "C_BaseEntity->m_pGameSceneNode");
};

class C_BaseModelEntity : public C_BaseEntity
{
public:
    SCHEMA(Vector, m_vecViewOffset, "C_BaseModelEntity->m_vecViewOffset");
};

// TODO: Fill this
class CBaseAnimGraph : public C_BaseModelEntity
{
public:

};

// TODO: Fill this
class C_BaseFlex : public CBaseAnimGraph
{
public:
};

// TODO: Fill this
class C_EconEntity : public C_BaseFlex
{
public:
};

class C_BasePlayerPawn : public C_BaseModelEntity
{
public:
};

class C_CSPlayerPawnBase : public C_BasePlayerPawn
{
public:
};

class C_CSPlayerPawn : public C_CSPlayerPawnBase
{
public:
    const Vector GetEyePosition()
    {
        return this->m_pGameSceneNode()->m_vecAbsOrigin() + this->m_vecViewOffset();
    }
};

class CBasePlayerController : public C_BaseModelEntity
{
public:
    SCHEMA(std::uint8_t, m_bIsLocalPlayerController, "CBasePlayerController->m_bIsLocalPlayerController");
};

class CCSPlayerController : public CBasePlayerController
{
public:
    SCHEMA(CHandle<C_CSPlayerPawn>, m_hPlayerPawn, "CCSPlayerController->m_hPlayerPawn");
};