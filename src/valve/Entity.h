#pragma once

struct BoneJointData
{
    Vector m_vecPosition;
    char pad[ 0x14 ];
};

struct EntitySpottedState_t
{
private:
    [[maybe_unused]] std::uint8_t __pad0000[ 0x8 ]; // 0x0
public:
    // MNetworkEnable
    // MNetworkChangeCallback "OnIsSpottedChanged"
    bool m_bSpotted; // 0x8	
private:
    [[maybe_unused]] std::uint8_t __pad0009[ 0x3 ]; // 0x9
public:
    // MNetworkEnable
    // MNetworkChangeCallback "OnIsSpottedChanged"
    std::uint32_t m_bSpottedByMask[ 2 ]; // 0xc	
};

class CTransform
{
public:
    VectorAligned m_vecPosition;
    QuaternionAligned m_quatOrientation;
};
static_assert(alignof(CTransform) == 16);

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

class CEntityIdentity
{
public:
    std::string m_strDesignerName() noexcept
    {
        std::string sBuffer = { };
        sBuffer.resize(32);

        DWORD64 pDesignerName = g_Memory.Read<DWORD64>(reinterpret_cast<DWORD64>(this) + 0x20);
        if (!pDesignerName)
            return { };

        sBuffer = g_Memory.ReadString(pDesignerName);
        return sBuffer;
    }

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
        std::uintptr_t uEntityIdentity = g_Memory.Read<std::uintptr_t>(reinterpret_cast<DWORD64>(this) + 0x10);
        if (!uEntityIdentity)
            return X("invalid");

        std::uintptr_t uEntityClassInfo = g_Memory.Read<std::uintptr_t>(uEntityIdentity + 0x08);
        std::uintptr_t uPointer1 = g_Memory.Read<std::uintptr_t>(uEntityClassInfo + 0x28);
        std::uintptr_t uPointer2 = g_Memory.Read<std::uintptr_t>(uPointer1 + 0x08);

        std::string strSchemaName = g_Memory.ReadString(uPointer2);
        if (strSchemaName.empty())
            return X("invalid");

        return strSchemaName;
    }

    OFFSET(CEntityIdentity*, m_pEntity, 0x10)
};

class C_BaseEntity : public CEntityInstance
{
public:
    static C_BaseEntity* GetBaseEntity(int nIdx) noexcept
    {
        std::uintptr_t uListEntry = g_Memory.Read<std::uint64_t>(Globals::m_uEntityList + (0x8 * ((nIdx & 0x7FFF) >> 0x9)) + 0x10);
        if (!uListEntry)
            return 0;

        C_BaseEntity* pEntity = g_Memory.Read<C_BaseEntity*>(uListEntry + 0x78 * (nIdx & 0x1FF));
        return pEntity;
    }

    SCHEMA(std::uint8_t, m_iTeamNum, "C_BaseEntity->m_iTeamNum");

    SCHEMA(std::int32_t, m_iHealth, "C_BaseEntity->m_iHealth");
    SCHEMA(std::int32_t, m_iMaxHealth, "C_BaseEntity->m_iMaxHealth");

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

class CCSPlayer_ItemServices
{
public:
    SCHEMA(bool, m_bHasDefuser, "CCSPlayer_ItemServices->m_bHasDefuser");
    SCHEMA(bool, m_bHasHelmet, "CCSPlayer_ItemServices->m_bHasHelmet");
    SCHEMA(bool, m_bHasHeavyArmor, "CCSPlayer_ItemServices->m_bHasHeavyArmor");
};

class C_BasePlayerPawn : public C_BaseModelEntity
{
public:
    SCHEMA(Vector, m_vOldOrigin, "C_BasePlayerPawn->m_vOldOrigin");
    
    SCHEMA(CCSPlayer_ItemServices*, m_pItemServices, "C_BasePlayerPawn->m_pItemServices");
};

class C_CSPlayerPawnBase : public C_BasePlayerPawn
{
public:
    SCHEMA_OFFSET(float, m_flFlashAlpha, "C_CSPlayerPawnBase->m_flFlashMaxAlpha", -0x8);
    SCHEMA(float, m_flFlashMaxAlpha, "C_CSPlayerPawnBase->m_flFlashMaxAlpha");
    SCHEMA(float, m_flFlashDuration, "C_CSPlayerPawnBase->m_flFlashDuration");
    
    SCHEMA(Vector, m_vecLastClipCameraPos, "C_CSPlayerPawnBase->m_vecLastClipCameraPos");
    
    SCHEMA(QAngle, m_angEyeAngles, "C_CSPlayerPawnBase->m_angEyeAngles");
};

class C_CSPlayerPawn : public C_CSPlayerPawnBase
{
public:
    static C_CSPlayerPawn* GetPlayerPawn( std::uint32_t uHandle )
    {
        std::uintptr_t uListEntry = g_Memory.Read<std::uintptr_t>( Globals::m_uEntityList + 0x8 * ( ( uHandle & 0x7FFF ) >> 9 ) + 16 );
        if (!uListEntry)
            return 0;

        C_CSPlayerPawn* pCSPlayerPawn = g_Memory.Read<C_CSPlayerPawn*>( uListEntry + 120 * ( uHandle & 0x1FF ) );

        return pCSPlayerPawn;
    }

    const Vector GetEyePosition()
    {
        return this->m_pGameSceneNode()->m_vecAbsOrigin() + this->m_vecViewOffset();
    }

    const Vector GetBonePosition(int nIndex) noexcept
    {
        static std::uint32_t uOffset = Schema::GetOffset(FNV1A::Hash("C_BaseEntity->m_pGameSceneNode"));
        static std::uint32_t uOffset2 = Schema::GetOffset(FNV1A::Hash("CSkeletonInstance->m_modelState"));
        const std::uint64_t uGameNode = g_Memory.Read<std::uint64_t>(reinterpret_cast<std::uint64_t>(this) + uOffset);
        const std::uint64_t uBoneArray = g_Memory.Read<std::uint64_t>(uGameNode + uOffset2 + Offsets::Client::dwBoneMatrix);
        return g_Memory.Read<BoneJointData>(uBoneArray + nIndex * sizeof(BoneJointData)).m_vecPosition;
    }

    SCHEMA(bool, m_bIsScoped, "C_CSPlayerPawn->m_bIsScoped");
    SCHEMA(bool, m_bIsWalking, "C_CSPlayerPawn->m_bIsWalking");
    SCHEMA(bool, m_bIsDefusing, "C_CSPlayerPawn->m_bIsDefusing");

    SCHEMA(std::int32_t, m_iShotsFired, "C_CSPlayerPawn->m_iShotsFired");
    SCHEMA(std::int32_t, m_ArmorValue, "C_CSPlayerPawn->m_ArmorValue");

    SCHEMA(EntitySpottedState_t, m_entitySpottedState, "C_CSPlayerPawn->m_entitySpottedState");
};

class CCSPlayerController_InGameMoneyServices
{
public:
    SCHEMA(std::int32_t, m_iAccount, "CCSPlayerController_InGameMoneyServices->m_iAccount");
    SCHEMA(std::int32_t, m_iCashSpentThisRound, "CCSPlayerController_InGameMoneyServices->m_iCashSpentThisRound");
    SCHEMA(std::int32_t, m_iStartAccount, "CCSPlayerController_InGameMoneyServices->m_iStartAccount");
    SCHEMA(std::int32_t, m_iTotalCashSpent, "CCSPlayerController_InGameMoneyServices->m_iTotalCashSpent");
    SCHEMA(std::int32_t, m_nPreviousAccount, "CCSPlayerController_InGameMoneyServices->m_nPreviousAccount");
};

class CBasePlayerController : public C_BaseModelEntity
{
public:
    SCHEMA(bool, m_bIsLocalPlayerController, "CBasePlayerController->m_bIsLocalPlayerController");

    SCHEMA(std::int32_t, m_nTickBase, "CBasePlayerController->m_nTickBase");
};

class CCSPlayerController : public CBasePlayerController
{
public:
    const std::string m_strSanitizedPlayerName() noexcept
    {
        std::string sBuffer = { };
        sBuffer.resize(32);

        static std::uint32_t uOffset = Schema::GetOffset(FNV1A::Hash("CCSPlayerController->m_sSanitizedPlayerName"));
        DWORD64 SanitizedPlayerName = g_Memory.Read<DWORD64>((reinterpret_cast<DWORD64>(this) + uOffset));
        if (!SanitizedPlayerName)
            return { };

        sBuffer = g_Memory.ReadString(SanitizedPlayerName);
        return sBuffer;
    }
public:
    SCHEMA(bool, m_bPawnIsAlive, "CCSPlayerController->m_bPawnIsAlive");
    
    SCHEMA(CCSPlayerController_InGameMoneyServices*, m_pInGameMoneyServices, "CCSPlayerController->m_pInGameMoneyServices");

    SCHEMA(CHandle<C_CSPlayerPawn>, m_hPlayerPawn, "CCSPlayerController->m_hPlayerPawn");
};
