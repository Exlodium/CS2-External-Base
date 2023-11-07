#pragma once

enum EBones : unsigned long
{
    PELVIS = 0,
    SPINE = 4,
    NECK,
    HEAD,
    LEFT_UPPER_ARM = 8,
    LEFT_LOWER_ARM,
    LEFT_HAND,
    RIGHT_UPPER_ARM = 13,
    RIGHT_LOWER_ARM,
    RIGHT_HAND,
    LEFT_UPPER_LEG = 22,
    LEFT_LOWER_LEG,
    LEFT_ANKLE,
    RIGHT_UPPER_LEG,
    RIGHT_LOWER_LEG,
    RIGHT_ANKLE
};

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

class CBaseEntity
{
public:
    static CBaseEntity* GetBaseEntity( int nIdx )
    {
        std::uintptr_t uListEntry = Globals::m_Memory.Read<std::uint64_t>( Globals::m_uEntityList + ( 0x8 * ( nIdx & 0x7FFF ) >> 9 ) + 16 );
        if( !uListEntry )
            return 0;

        CBaseEntity* pEntity = Globals::m_Memory.Read<CBaseEntity*>( uListEntry + 120 * ( nIdx & 0x1FF ) );
        return pEntity;
    }
};

class CCSPlayer_ItemServices
{
public:

    const bool m_bHasDefuser( )
    {
        return Globals::m_Memory.Read<bool>( reinterpret_cast< DWORD64 >( this ) + Offsets::m_bHasDefuser );
    }

    const bool m_bHasHelmet( )
    {
        return Globals::m_Memory.Read<bool>( reinterpret_cast< DWORD64 >( this ) + Offsets::m_bHasHelmet );
    }

    const bool m_bHasHeavyArmor( )
    {
        return Globals::m_Memory.Read<bool>( reinterpret_cast< DWORD64 >( this ) + Offsets::m_bHasHeavyArmor );
    }
};

class CCSPlayerPawn
{
public:
    static CCSPlayerPawn* GetPlayerPawn( std::uint32_t uHandle )
    {
        
        std::uintptr_t uListEntry = Globals::m_Memory.Read<std::uintptr_t>( Globals::m_uEntityList + 0x8 * ( ( uHandle & 0x7FFF ) >> 9 ) + 16 );
        if (!uListEntry)
            return 0;

        CCSPlayerPawn* pCSPlayerPawn = Globals::m_Memory.Read<CCSPlayerPawn*>( uListEntry + 120 * ( uHandle & 0x1FF ) );

        return pCSPlayerPawn;
    }

    CCSPlayer_ItemServices* m_pItemServices( )
    {
        return reinterpret_cast< CCSPlayer_ItemServices* >( Globals::m_Memory.Read<CCSPlayer_ItemServices*>( reinterpret_cast< DWORD64 >( this ) + Offsets::m_pItemServices ) );
    }

    const bool m_bIsScoped( )
    {
        return Globals::m_Memory.Read<int>( reinterpret_cast< DWORD64 >( this ) + Offsets::m_bIsScoped );
    }

    const bool m_bIsDefusing( )
    {
        return Globals::m_Memory.Read<int>( reinterpret_cast< DWORD64 >( this ) + Offsets::m_bIsDefusing );
    }

    const int m_ArmorValue( )
    {
        return Globals::m_Memory.Read<int>( reinterpret_cast< DWORD64 >( this ) + Offsets::m_ArmorValue );
    }

    const int m_iHealth( )
    {
        return Globals::m_Memory.Read<int>( reinterpret_cast< DWORD64 >( this ) + Offsets::m_iHealth );
    }

    const float m_flFlashAlpha( )
    {
        return Globals::m_Memory.Read<float>( reinterpret_cast< DWORD64 >( this ) + ( Offsets::m_flFlashMaxAlpha - 0x8 ) );
    }

    const float m_flFlashMaxAlpha( )
    {
        return Globals::m_Memory.Read<float>( reinterpret_cast< DWORD64 >( this ) + Offsets::m_flFlashMaxAlpha );
    }

    const float m_flFlashDuration( )
    {
        return Globals::m_Memory.Read<float>( reinterpret_cast< DWORD64 >( this ) + Offsets::m_flFlashDuration );
    }

    const Vector m_vOldOrigin( )
    {
        return Globals::m_Memory.Read<Vector>( reinterpret_cast< DWORD64 >( this ) + Offsets::m_vOldOrigin );
    }

    const Vector m_vecCameraPosition( )
    {
        return Globals::m_Memory.Read<Vector>( reinterpret_cast< DWORD64 >( this ) + Offsets::m_vecLastClipCameraPos );
    }

    const QAngle m_angEyeAngles( )
    {
        return Globals::m_Memory.Read<QAngle>( reinterpret_cast< DWORD64 >( this ) + Offsets::m_angEyeAngles );
    }

    const EntitySpottedState_t m_entitySpottedState( )
    {
        return Globals::m_Memory.Read<EntitySpottedState_t>( reinterpret_cast< DWORD64 >( this ) + Offsets::m_entitySpottedState );
    }
};

class CCSPlayerController_InGameMoneyServices
{
public:

    const int m_iAccount( )
    {
        return Globals::m_Memory.Read<int>( reinterpret_cast< DWORD64 >( this ) + Offsets::m_iAccount );
    }

    const int m_iCashSpentThisRound( )
    {
        return Globals::m_Memory.Read<int>( reinterpret_cast< DWORD64 >( this ) + Offsets::m_iCashSpentThisRound );
    }

    const int m_iStartAccount( )
    {
        return Globals::m_Memory.Read<int>( reinterpret_cast< DWORD64 >( this ) + Offsets::m_iStartAccount );
    }

    const int m_iTotalCashSpent( )
    {
        return Globals::m_Memory.Read<int>( reinterpret_cast< DWORD64 >( this ) + Offsets::m_iTotalCashSpent );
    }

    const int m_nPreviousAccount( )
    {
        return Globals::m_Memory.Read<int>( reinterpret_cast< DWORD64 >( this ) + Offsets::m_nPreviousAccount );
    }
};

class CCSPlayerController : public CBaseEntity
{
public:
    // get
    const std::uint64_t GetGameSceneNode( )
    {
        return Globals::m_Memory.Read<std::uint64_t>( reinterpret_cast< std::uint64_t >( this->m_hPlayerPawn( ) ) + Offsets::dwGameSceneNode );
    }

    const Vector GetBonePosition( int index )
    {
        const std::uint64_t uBoneArray = Globals::m_Memory.Read<std::uint64_t>( this->GetGameSceneNode( ) + Offsets::dwModelState + Offsets::dwBoneMatrix );
        return Globals::m_Memory.Read<BoneJointData>( uBoneArray + index * sizeof( BoneJointData ) ).m_vecPosition;
    }

    CCSPlayerController_InGameMoneyServices* m_pInGameMoneyServices( )
    {
        return Globals::m_Memory.Read<CCSPlayerController_InGameMoneyServices*>( reinterpret_cast< DWORD64 >( this ) + Offsets::m_pInGameMoneyServices );
    }

    const std::string m_strSanitizedPlayerName( )
    {
        std::string sBuffer = { };
        sBuffer.resize( 32 );

        DWORD64 SanitizedPlayerName = Globals::m_Memory.Read<DWORD64>( ( reinterpret_cast< DWORD64 >( this ) + Offsets::m_sSanitizedPlayerName ) );
        if (!SanitizedPlayerName)
            return { };

        sBuffer = Globals::m_Memory.ReadString( SanitizedPlayerName );
        return sBuffer;
    }

    const bool m_bPawnIsAlive( )
    {
        return Globals::m_Memory.Read<bool>( reinterpret_cast< DWORD64 >( this ) + Offsets::m_bPawnIsAlive );
    }

    const int m_iTeamNum( )
    {
        return Globals::m_Memory.Read<int>( reinterpret_cast< DWORD64 >( this ) + Offsets::m_iTeamNum );
    }

    const  bool m_bIsLocalPlayerController( )
    {
        return Globals::m_Memory.Read<bool>( reinterpret_cast< DWORD64 >( this ) + Offsets::m_bIsLocalPlayerController );
    }

    CCSPlayerPawn* m_hPlayerPawn( )
    {
        std::uint32_t uPlayerPawn = Globals::m_Memory.Read<std::uint32_t>( reinterpret_cast< DWORD64 >( this ) + Offsets::m_hPlayerPawn );
        return CCSPlayerPawn::GetPlayerPawn( uPlayerPawn );
    }
};
