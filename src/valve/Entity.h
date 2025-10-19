#pragma once

using CFiringModeFloat = std::array<std::float_t, 2>;

class CTransform
{
public:
    VectorAligned m_vecPosition;
    QuaternionAligned m_quatOrientation;
};

struct BoneData_t
{
    Vector m_vecPosition;
    float m_flScale;
    Quaternion m_qRotation;

    Matrix3x4_t TranslateToMatrix3x4()
    {
        Matrix3x4_t matrix = Matrix3x4_t();
        Vector4D vecRotation = Vector4D();
        Vector vecPosition = this->m_vecPosition;

        vecRotation.x = this->m_qRotation.x; //rot.x
        vecRotation.y = this->m_qRotation.y; //rot.y
        vecRotation.z = this->m_qRotation.z; //rot.z
        vecRotation.w = this->m_qRotation.w; //rot.w

        matrix[0][0] = 1.0f - 2.0f * vecRotation.y * vecRotation.y - 2.0f * vecRotation.z * vecRotation.z;
        matrix[1][0] = 2.0f * vecRotation.x * vecRotation.y + 2.0f * vecRotation.w * vecRotation.z;
        matrix[2][0] = 2.0f * vecRotation.x * vecRotation.z - 2.0f * vecRotation.w * vecRotation.y;

        matrix[0][1] = 2.0f * vecRotation.x * vecRotation.y - 2.0f * vecRotation.w * vecRotation.z;
        matrix[1][1] = 1.0f - 2.0f * vecRotation.x * vecRotation.x - 2.0f * vecRotation.z * vecRotation.z;
        matrix[2][1] = 2.0f * vecRotation.y * vecRotation.z + 2.0f * vecRotation.w * vecRotation.x;

        matrix[0][2] = 2.0f * vecRotation.x * vecRotation.z + 2.0f * vecRotation.w * vecRotation.y;
        matrix[1][2] = 2.0f * vecRotation.y * vecRotation.z - 2.0f * vecRotation.w * vecRotation.x;
        matrix[2][2] = 1.0f - 2.0f * vecRotation.x * vecRotation.x - 2.0f * vecRotation.y * vecRotation.y;

        matrix[0][3] = vecPosition.x;
        matrix[1][3] = vecPosition.y;
        matrix[2][3] = vecPosition.z;

        return matrix;
    }
};

struct EntitySpottedState_t
{
public:
    MEM_PAD(0x8); // 0x0|
    bool m_bSpotted; // 0x8	
    MEM_PAD(0x3); // 0x9
    std::uint32_t m_bSpottedByMask[2]; // 0xc	
};

class CEntitySubClassVDataBase
{
public:
    template <typename T> requires std::derived_from<T, CEntitySubClassVDataBase>
    inline T* As()
    {
        return (T*)this;
    }
};

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

    std::string m_strDesignerName() noexcept
    {
        std::string strBuffer = { };
        strBuffer.resize(32);

        std::uintptr_t uDesignerName = g_Memory.ReadMemory<std::uintptr_t>(reinterpret_cast<std::uintptr_t>(this) + 0x20);
        if (!uDesignerName)
            return { };

        strBuffer = g_Memory.ReadMemoryString(uDesignerName);
        return strBuffer;
    }

    OFFSET(std::uint32_t, nIndex, 0x10);
    SCHEMA(std::uint32_t, m_flags, "CEntityIdentity->m_flags");

    SCHEMA(CEntityIdentity*, m_pPrev, "CEntityIdentity->m_pPrev");
    SCHEMA(CEntityIdentity*, m_pNext, "CEntityIdentity->m_pNext");
    SCHEMA(CEntityIdentity*, m_pPrevByClass, "CEntityIdentity->m_pPrevByClass");
    SCHEMA(CEntityIdentity*, m_pNextByClass, "CEntityIdentity->m_pNextByClass");
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
        std::uintptr_t uSchemaNameAddress = g_Memory.ReadMemory(reinterpret_cast<std::uintptr_t>(this) + 0x10, { 0x8, 0x30, 0x8 });
        if (uSchemaNameAddress == 0U)
            return {};

        std::string strSchemaName = g_Memory.ReadMemoryString(uSchemaNameAddress);
        if (strSchemaName.empty())
            return {};

        return strSchemaName;
    }

    OFFSET(CEntityIdentity*, m_pEntity, 0x10)
};

class CGameSceneNode
{
public:
   
    SCHEMA(bool, m_bDormant, "CGameSceneNode->m_bDormant");
    SCHEMA(Vector, m_vecAbsOrigin, "CGameSceneNode->m_vecAbsOrigin");
    SCHEMA(CTransform, m_nodeToWorld, "CGameSceneNode->m_nodeToWorld");

    OFFSET(BoneData_t*, m_pBoneCache, 0x1F0)
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

        C_BaseEntity* pEntity = g_Memory.ReadMemory<C_BaseEntity*>(uListEntry + 0x70 * (nIdx & 0x1FF));
        return pEntity;
    }

    bool IsAlive()
    {
        /*
         * @note: if u have problems with lifestate check (some servers has plugin for health restore and entity lifestate doesnt switch back to the alive)
         * required to check for health and/or check lifestate where needed
         * GetHealth() > 0
         */

        return (this->m_lifeState() == 0U || this->m_iHealth() > 0);
    }

    SCHEMA(std::uint8_t, m_iTeamNum, "C_BaseEntity->m_iTeamNum");
    SCHEMA(std::uint8_t, m_lifeState, "C_BaseEntity->m_lifeState");

    SCHEMA(int, m_iHealth, "C_BaseEntity->m_iHealth");
    SCHEMA(int, m_iMaxHealth, "C_BaseEntity->m_iMaxHealth");

    SCHEMA(std::uint32_t, m_fFlags, "C_BaseEntity->m_fFlags");

    SCHEMA(Vector, m_vecVelocity, "C_BaseEntity->m_vecVelocity");
    SCHEMA(Vector, m_vecAbsVelocity, "C_BaseEntity->m_vecAbsVelocity");

    SCHEMA(CCollisionProperty*, m_pCollision, "C_BaseEntity->m_pCollision");
    SCHEMA(CGameSceneNode*, m_pGameSceneNode, "C_BaseEntity->m_pGameSceneNode");

    SCHEMA_OFFSET(CEntitySubClassVDataBase*, m_pVDataBase, "C_BaseEntity->m_nSubclassID", 0x8);
};

class C_BaseModelEntity : public C_BaseEntity
{
public:
    SCHEMA(Vector, m_vecViewOffset, "C_BaseModelEntity->m_vecViewOffset");
};

class CBaseAnimGraph : public C_BaseModelEntity
{
public:

};

class C_BaseFlex : public CBaseAnimGraph
{
public:
};

class C_EconEntity : public C_BaseFlex
{
public:
};

class CBasePlayerWeaponVData : public CEntitySubClassVDataBase
{
public:
    SCHEMA(int, m_iMaxClip1, "CBasePlayerWeaponVData->m_iMaxClip1");
};

class CCSWeaponBaseVData : public CBasePlayerWeaponVData
{
public:
    const std::string m_strName() noexcept
    {
        std::string strBuffer = { };
        strBuffer.resize(32);

        static std::uint32_t uOffset = SchemaSystem::m_mapSchemaOffsets[FNV1A::Hash("CCSWeaponBaseVData->m_szName")];
        std::uintptr_t uName = g_Memory.ReadMemory<std::uintptr_t>((reinterpret_cast<std::uintptr_t>(this) + uOffset));
        if (!uName)
            return { };

        strBuffer = g_Memory.ReadMemoryString(uName);
        return strBuffer;
    }

    const bool IsGun() noexcept
    {
        switch (this->m_WeaponType())
        {
        case CSWeaponType::WEAPONTYPE_PISTOL:
        case CSWeaponType::WEAPONTYPE_SUBMACHINEGUN:
        case CSWeaponType::WEAPONTYPE_RIFLE:
        case CSWeaponType::WEAPONTYPE_SHOTGUN:
        case CSWeaponType::WEAPONTYPE_SNIPER_RIFLE:
        case CSWeaponType::WEAPONTYPE_MACHINEGUN:
            return true;
        default:
            return false;
        }
    }

    SCHEMA(CSWeaponType, m_WeaponType, "CCSWeaponBaseVData->m_WeaponType");

    SCHEMA(CFiringModeFloat, m_flMaxSpeed, "CCSWeaponBaseVData->m_flMaxSpeed");
};

class C_BasePlayerWeapon : public C_EconEntity
{
public:
    SCHEMA(int, m_iClip1, "C_BasePlayerWeapon->m_iClip1");
    SCHEMA(int, m_pReserveAmmo, "C_BasePlayerWeapon->m_pReserveAmmo");

    CCSWeaponBaseVData* GetWeaponBaseVData() noexcept
    {
        return this->m_pVDataBase()->As<CCSWeaponBaseVData>();
    }

    std::uint16_t GetItemDefinitionIndex()
    {
        static std::uintptr_t uAttributeManager = SchemaSystem::m_mapSchemaOffsets[FNV1A::HashConst("C_EconEntity->m_AttributeManager")];
        static std::uintptr_t uItem = SchemaSystem::m_mapSchemaOffsets[FNV1A::HashConst("C_AttributeContainer->m_Item")];
        static std::uintptr_t uItemDefinition = SchemaSystem::m_mapSchemaOffsets[FNV1A::HashConst("C_EconItemView->m_iItemDefinitionIndex")];

        return g_Memory.ReadMemory<std::uint16_t>(reinterpret_cast<std::uintptr_t>(this) + uAttributeManager + uItem + uItemDefinition);
    }
};

class C_CSWeaponBase : public C_BasePlayerWeapon
{
public:
    [[nodiscard]] float GetInaccuracyMoveRebuilt(int nWeaponMode);
    [[nodiscard]] float GetMaxSpeedRebuilt();
    [[nodiscard]] float GetSpreadRebuilt();

    [[nodiscard]] float GetInaccuracyRebuilt(float& flMoveInaccuracy, float& flAirInaccuracy);

    SCHEMA(float, m_flTurningInaccuracy, "C_CSWeaponBase->m_flTurningInaccuracy");
    SCHEMA(float, m_fAccuracyPenalty, "C_CSWeaponBase->m_fAccuracyPenalty");

    SCHEMA(CSWeaponMode, m_weaponMode, "C_CSWeaponBase->m_weaponMode");
};

class C_CSWeaponBaseGun : public C_CSWeaponBase
{
public:
};

class CPlayer_WeaponServices
{
public:
    SCHEMA(CHandle<C_BasePlayerWeapon>, m_hActiveWeapon, "CPlayer_WeaponServices->m_hActiveWeapon");
    SCHEMA(C_NetworkUtlVectorBaseSimple, m_hMyWeapons, "CPlayer_WeaponServices->m_hMyWeapons");
};

class CCSPlayer_WeaponServices : public CPlayer_WeaponServices
{
public:
};

class C_BasePlayerPawn : public C_BaseModelEntity
{
public:
    SCHEMA(CCSPlayer_WeaponServices*, m_pWeaponServices, "C_BasePlayerPawn->m_pWeaponServices");

    SCHEMA(float, m_flFOVSensitivityAdjust, "C_BasePlayerPawn->m_flFOVSensitivityAdjust");
};

class C_CSPlayerPawnBase : public C_BasePlayerPawn
{
public:
    SCHEMA(float, m_flFlashAlpha, "C_CSPlayerPawnBase->m_flFlashMaxAlpha", -0x8);
    SCHEMA(float, m_flFlashMaxAlpha, "C_CSPlayerPawnBase->m_flFlashMaxAlpha");
    SCHEMA(float, m_flFlashDuration, "C_CSPlayerPawnBase->m_flFlashDuration");

    const std::string m_strActiveWeaponName() noexcept
    {
        static std::uint32_t uOffset = SchemaSystem::m_mapSchemaOffsets[FNV1A::HashConst("C_CSPlayerPawn->m_pClippingWeapon")];

        std::uintptr_t uWeaponNameAddress = 0U;
        uWeaponNameAddress = g_Memory.ReadMemory(reinterpret_cast<std::uintptr_t>(this) + uOffset, { 0x10, 0x20 });
        if (uWeaponNameAddress == 0U)
            return {};

        std::string strWeaponName = g_Memory.ReadMemoryString(uWeaponNameAddress);

        std::size_t uIndex = strWeaponName.find(X("_"));
        if (uIndex == std::string::npos || strWeaponName.empty())
            strWeaponName = {};
        else
            strWeaponName = strWeaponName.substr(uIndex + 1, strWeaponName.size() - uIndex - 1);

        return strWeaponName;
    }
};

class C_CSPlayerPawn : public C_CSPlayerPawnBase
{
public:
    const Vector GetEyePosition()
    {
        return this->m_pGameSceneNode()->m_vecAbsOrigin() + this->m_vecViewOffset();
    }

    SCHEMA(bool, m_bIsScoped, "C_CSPlayerPawn->m_bIsScoped");
    SCHEMA(bool, m_bIsWalking, "C_CSPlayerPawn->m_bIsWalking");
    SCHEMA(bool, m_bIsDefusing, "C_CSPlayerPawn->m_bIsDefusing");
    SCHEMA(int, m_ArmorValue, "C_CSPlayerPawn->m_ArmorValue");
    SCHEMA(int, m_iShotsFired, "C_CSPlayerPawn->m_iShotsFired");

    SCHEMA(float, m_flEmitSoundTime, "C_CSPlayerPawn->m_flEmitSoundTime");

    SCHEMA(EntitySpottedState_t, m_entitySpottedState, "C_CSPlayerPawn->m_entitySpottedState");

    SCHEMA(CUtlVectorSimple, m_aimPunchCache, "C_CSPlayerPawn->m_aimPunchCache");
};

class C_CSObserverPawn : public C_CSPlayerPawnBase
{
};

class CBasePlayerController : public C_BaseModelEntity
{
public:
    SCHEMA(bool, m_bIsLocalPlayerController, "CBasePlayerController->m_bIsLocalPlayerController");

    SCHEMA(int, m_nTickBase, "CBasePlayerController->m_nTickBase");

    SCHEMA(CHandle<C_BasePlayerPawn>, m_hPawn, "CBasePlayerController->m_hPawn");
};

class CCSPlayerController : public CBasePlayerController
{
public:
    const std::string m_strSanitizedPlayerName() noexcept
    {
        std::string sBuffer = { };
        sBuffer.resize(32);

        static std::uint32_t uOffset = SchemaSystem::m_mapSchemaOffsets[FNV1A::HashConst("CCSPlayerController->m_sSanitizedPlayerName")];
        DWORD64 SanitizedPlayerName = g_Memory.ReadMemory<DWORD64>((reinterpret_cast<DWORD64>(this) + uOffset));
        if (!SanitizedPlayerName)
            return { };

        sBuffer = g_Memory.ReadMemoryString(SanitizedPlayerName);
        return sBuffer;
    }

    SCHEMA(CHandle<C_CSPlayerPawn>, m_hPlayerPawn, "CCSPlayerController->m_hPlayerPawn");
    SCHEMA(CHandle<C_CSObserverPawn>, m_hObserverPawn, "CCSPlayerController->m_hObserverPawn");
};