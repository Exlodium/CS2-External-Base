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
    const bool m_bDormant() noexcept
    {
		static std::uint32_t uOffset = Schema::GetOffset(FNV1A::Hash("CGameSceneNode->m_bDormant"));
		return g_Memory.Read<bool>(reinterpret_cast<DWORD64>(this) + uOffset);
	}

    const Vector m_vecAbsOrigin() noexcept
    {
        static std::uint32_t uOffset = Schema::GetOffset(FNV1A::Hash("CGameSceneNode->m_vecAbsOrigin"));
        return g_Memory.Read<Vector>(reinterpret_cast<DWORD64>(this) + uOffset);
    }

    const CTransform m_nodeToWorld() noexcept
    {
        static std::uint32_t uOffset = Schema::GetOffset(FNV1A::Hash("CGameSceneNode->m_nodeToWorld"));
        return g_Memory.Read<CTransform>(reinterpret_cast<DWORD64>(this) + uOffset);
    }
};

class CCollisionProperty
{
public:
    const Vector m_vecMins() noexcept
    {
        static std::uint32_t uOffset = Schema::GetOffset(FNV1A::Hash("CCollisionProperty->m_vecMins"));
        return g_Memory.Read<Vector>(reinterpret_cast<DWORD64>(this) + uOffset);
    }

    const Vector m_vecMaxs() noexcept
    {
        static std::uint32_t uOffset = Schema::GetOffset(FNV1A::Hash("CCollisionProperty->m_vecMaxs"));
        return g_Memory.Read<Vector>(reinterpret_cast<DWORD64>(this) + uOffset);
    }
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
};

class CEntityInstance
{
public:
    CEntityIdentity* m_pEntityIdentity() noexcept
    {
        CEntityIdentity* pEntityIdentity = g_Memory.Read<CEntityIdentity*>(reinterpret_cast<DWORD64>(this) + 0x10);
        return pEntityIdentity;
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
};

class C_BaseEntity : public CEntityInstance
{
public:
    static C_BaseEntity* GetBaseEntity(std::int32_t nIdx )
    {
        std::uintptr_t uListEntry = g_Memory.Read<std::uint64_t>( Globals::m_uEntityList + ( 0x8 * ( nIdx & 0x7FFF ) >> 9 ) + 16 );
        if( !uListEntry )
            return 0;

        C_BaseEntity* pEntity = g_Memory.Read<C_BaseEntity*>( uListEntry + 120 * ( nIdx & 0x1FF ) );
        return pEntity;
    }

    const std::uint8_t m_iTeamNum() noexcept
    {
		static std::uint32_t uOffset = Schema::GetOffset(FNV1A::Hash("C_BaseEntity->m_iTeamNum"));
		return g_Memory.Read<std::uint8_t>(reinterpret_cast<DWORD64>(this) + uOffset);
	}

    const std::int32_t m_iHealth() noexcept
    {
        static std::uint32_t uOffset = Schema::GetOffset(FNV1A::Hash("C_BaseEntity->m_iHealth"));
        return g_Memory.Read<std::int32_t>(reinterpret_cast<DWORD64>(this) + uOffset);
    }

    const std::int32_t m_iMaxHealth() noexcept
    {
        static std::uint32_t uOffset = Schema::GetOffset(FNV1A::Hash("C_BaseEntity->m_iMaxHealth"));
        return g_Memory.Read<std::int32_t>(reinterpret_cast<DWORD64>(this) + uOffset);
    }

    const Vector m_vecVelocity() noexcept
    {
        static std::uint32_t uOffset = Schema::GetOffset(FNV1A::Hash("C_BaseEntity->m_vecVelocity"));
        return g_Memory.Read<Vector>(reinterpret_cast<DWORD64>(this) + uOffset);
    }

    CCollisionProperty* m_pCollision() noexcept
    {
        static std::uint32_t uOffset = Schema::GetOffset(FNV1A::Hash("C_BaseEntity->m_pCollision"));
        return g_Memory.Read<CCollisionProperty*>(reinterpret_cast<DWORD64>(this) + uOffset);
    }

    CGameSceneNode* m_pGameSceneNode() noexcept
    {
        static std::uint32_t uOffset = Schema::GetOffset(FNV1A::Hash("C_BaseEntity->m_pGameSceneNode"));
        return g_Memory.Read<CGameSceneNode*>(reinterpret_cast< DWORD64>(this) + uOffset);
	}
};

class C_BaseModelEntity : public C_BaseEntity
{
public:
    const Vector m_vecViewOffset() noexcept
    {
        static std::uint32_t uOffset = Schema::GetOffset(FNV1A::Hash("C_BaseModelEntity->m_vecViewOffset"));
        return g_Memory.Read<Vector>(reinterpret_cast<DWORD64>(this) + uOffset);
    }
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
    const bool m_bHasDefuser( )
    {
        static std::uint32_t uOffset = Schema::GetOffset(FNV1A::Hash("CCSPlayer_ItemServices->m_bHasDefuser"));
        return g_Memory.Read<bool>( reinterpret_cast< DWORD64 >( this ) + uOffset);
    }

    const bool m_bHasHelmet( )
    {
        static std::uint32_t uOffset = Schema::GetOffset(FNV1A::Hash("CCSPlayer_ItemServices->m_bHasHelmet"));
        return g_Memory.Read<bool>( reinterpret_cast< DWORD64 >( this ) + uOffset);
    }

    const bool m_bHasHeavyArmor( )
    {
        static std::uint32_t uOffset = Schema::GetOffset(FNV1A::Hash("CCSPlayer_ItemServices->m_bHasHeavyArmor"));
        return g_Memory.Read<bool>( reinterpret_cast< DWORD64 >( this ) + uOffset);
    }
};

class C_BasePlayerPawn : public C_BaseModelEntity
{
public:
    CCSPlayer_ItemServices* m_pItemServices()
    {
        static std::uint32_t uOffset = Schema::GetOffset(FNV1A::Hash("C_BasePlayerPawn->m_pItemServices"));
        return reinterpret_cast<CCSPlayer_ItemServices*>(g_Memory.Read<CCSPlayer_ItemServices*>(reinterpret_cast<DWORD64>(this) + uOffset));
    }

    const Vector m_vOldOrigin() noexcept
    {
        static std::uint32_t uOffset = Schema::GetOffset(FNV1A::Hash("C_BasePlayerPawn->m_vOldOrigin"));
        return g_Memory.Read<Vector>(reinterpret_cast<DWORD64>(this) + uOffset);
    }
};

class C_CSPlayerPawnBase : public C_BasePlayerPawn
{
public:
    const bool m_bIsScoped()
    {
        static std::uint32_t uOffset = Schema::GetOffset(FNV1A::Hash("C_CSPlayerPawnBase->m_bIsScoped"));
        return g_Memory.Read<bool>(reinterpret_cast<DWORD64>(this) + uOffset);
    }

    const bool m_bIsWalking()
    {
        static std::uint32_t uOffset = Schema::GetOffset(FNV1A::Hash("C_CSPlayerPawnBase->m_bIsWalking"));
        return g_Memory.Read<bool>(reinterpret_cast<DWORD64>(this) + uOffset);
    }

    const bool m_bIsDefusing()
    {
        static std::uint32_t uOffset = Schema::GetOffset(FNV1A::Hash("C_CSPlayerPawnBase->m_bIsDefusing"));
        return g_Memory.Read<bool>(reinterpret_cast<DWORD64>(this) + uOffset);
    }

    const float m_flFlashAlpha()
    {
        static std::uint32_t uOffset = Schema::GetOffset(FNV1A::Hash("C_CSPlayerPawnBase->m_flFlashMaxAlpha"));
        return g_Memory.Read<float>(reinterpret_cast<DWORD64>(this) + (uOffset - 0x8));
    }

    const float m_flFlashMaxAlpha()
    {
        static std::uint32_t uOffset = Schema::GetOffset(FNV1A::Hash("C_CSPlayerPawnBase->m_flFlashMaxAlpha"));
        return g_Memory.Read<float>(reinterpret_cast<DWORD64>(this) + uOffset);
    }

    const float m_flFlashDuration()
    {
        static std::uint32_t uOffset = Schema::GetOffset(FNV1A::Hash("C_CSPlayerPawnBase->m_flFlashDuration"));
        return g_Memory.Read<float>(reinterpret_cast<DWORD64>(this) + uOffset);
    }

    const std::int32_t m_iShotsFired() noexcept
    {
		static std::uint32_t uOffset = Schema::GetOffset(FNV1A::Hash("C_CSPlayerPawnBase->m_iShotsFired"));
		return g_Memory.Read<std::int32_t>(reinterpret_cast<DWORD64>(this) + uOffset);
	}

    const std::int32_t m_ArmorValue() noexcept
    {
        static std::uint32_t uOffset = Schema::GetOffset(FNV1A::Hash("C_CSPlayerPawnBase->m_ArmorValue"));
        return g_Memory.Read<std::int32_t>(reinterpret_cast<DWORD64>(this) + uOffset);
    }

    const Vector m_vecLastClipCameraPos() noexcept
    {
        static std::uint32_t uOffset = Schema::GetOffset(FNV1A::Hash("C_CSPlayerPawnBase->m_vecLastClipCameraPos"));
        return g_Memory.Read<Vector>(reinterpret_cast<DWORD64>(this) + uOffset);
    }

    const QAngle m_angEyeAngles() noexcept
    {
        static std::uint32_t uOffset = Schema::GetOffset(FNV1A::Hash("C_CSPlayerPawnBase->m_angEyeAngles"));
        return g_Memory.Read<QAngle>(reinterpret_cast<DWORD64>(this) + uOffset);
    }

    const EntitySpottedState_t m_entitySpottedState() noexcept
    {
        static std::uint32_t uOffset = Schema::GetOffset(FNV1A::Hash("C_CSPlayerPawnBase->m_entitySpottedState"));
        return g_Memory.Read<EntitySpottedState_t>(reinterpret_cast<DWORD64>(this) + uOffset);
    }
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
};

class CCSPlayerController_InGameMoneyServices
{
public:

    const std::int32_t m_iAccount( )
    {
        static std::uint32_t uOffset = Schema::GetOffset(FNV1A::Hash("CCSPlayerController_InGameMoneyServices->m_iAccount"));
        return g_Memory.Read<std::int32_t>( reinterpret_cast< DWORD64 >( this ) + uOffset);
    }

    const std::int32_t m_iCashSpentThisRound( )
    {
        static std::uint32_t uOffset = Schema::GetOffset(FNV1A::Hash("CCSPlayerController_InGameMoneyServices->m_iCashSpentThisRound"));
        return g_Memory.Read<std::int32_t>( reinterpret_cast< DWORD64 >( this ) + uOffset);
    }

    const std::int32_t m_iStartAccount( )
    {
        static std::uint32_t uOffset = Schema::GetOffset(FNV1A::Hash("CCSPlayerController_InGameMoneyServices->m_iStartAccount"));
        return g_Memory.Read<std::int32_t>( reinterpret_cast< DWORD64 >( this ) + uOffset);
    }

    const std::int32_t m_iTotalCashSpent( )
    {
        static std::uint32_t uOffset = Schema::GetOffset(FNV1A::Hash("CCSPlayerController_InGameMoneyServices->m_iTotalCashSpent"));
        return g_Memory.Read<std::int32_t>( reinterpret_cast< DWORD64 >( this ) + uOffset);
    }

    const std::int32_t m_nPreviousAccount( )
    {
        static std::uint32_t uOffset = Schema::GetOffset(FNV1A::Hash("CCSPlayerController_InGameMoneyServices->m_nPreviousAccount"));
        return g_Memory.Read<std::int32_t>( reinterpret_cast< DWORD64 >( this ) + uOffset);
    }
};

class CBasePlayerController : public C_BaseModelEntity
{
public:
    const std::uint32_t m_nTickBase()
    {
        static std::uint32_t uOffset = Schema::GetOffset(FNV1A::Hash("CBasePlayerController->m_nTickBase"));
        return g_Memory.Read<std::uint32_t>(reinterpret_cast<DWORD64>(this) + uOffset);
    }

    const bool m_bIsLocalPlayerController()
    {
        static std::uint32_t uOffset = Schema::GetOffset(FNV1A::Hash("CBasePlayerController->m_bIsLocalPlayerController"));
        return g_Memory.Read<bool>(reinterpret_cast<DWORD64>(this) + uOffset);
    }
};

class CCSPlayerController : public CBasePlayerController
{
public:
    const bool m_bPawnIsAlive()
    {
        static std::uint32_t uOffset = Schema::GetOffset(FNV1A::Hash("CCSPlayerController->m_bPawnIsAlive"));
        return g_Memory.Read<bool>(reinterpret_cast<DWORD64>(this) + uOffset);
    }

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

    C_CSPlayerPawn* m_hPlayerPawn() noexcept
    {
        static std::uint32_t uOffset = Schema::GetOffset(FNV1A::Hash("CCSPlayerController->m_hPlayerPawn"));
        std::uint32_t uPlayerPawn = g_Memory.Read<std::uint32_t>(reinterpret_cast<DWORD64>(this) + uOffset);
        return C_CSPlayerPawn::GetPlayerPawn(uPlayerPawn);
    }

    CCSPlayerController_InGameMoneyServices* m_pInGameMoneyServices()
    {
        static std::uint32_t uOffset = Schema::GetOffset(FNV1A::Hash("CCSPlayerController->m_pInGameMoneyServices"));
        return g_Memory.Read<CCSPlayerController_InGameMoneyServices*>(reinterpret_cast<DWORD64>(this) + uOffset);
    }
};
