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

class CBaseEntity
{
public:
    static CBaseEntity* GetBaseEntity( int nIdx )
    {
        std::uintptr_t uListEntry = g_Memory.Read<std::uint64_t>( Globals::m_uEntityList + ( 0x8 * ( nIdx & 0x7FFF ) >> 9 ) + 16 );
        if( !uListEntry )
            return 0;

        CBaseEntity* pEntity = g_Memory.Read<CBaseEntity*>( uListEntry + 120 * ( nIdx & 0x1FF ) );
        return pEntity;
    }
};

class CCSPlayer_ItemServices
{
public:

    const bool m_bHasDefuser( )
    {
        return g_Memory.Read<bool>( reinterpret_cast< DWORD64 >( this ) + Offsets::m_bHasDefuser );
    }

    const bool m_bHasHelmet( )
    {
        return g_Memory.Read<bool>( reinterpret_cast< DWORD64 >( this ) + Offsets::m_bHasHelmet );
    }

    const bool m_bHasHeavyArmor( )
    {
        return g_Memory.Read<bool>( reinterpret_cast< DWORD64 >( this ) + Offsets::m_bHasHeavyArmor );
    }
};

class CCSPlayerPawn
{
public:
    static CCSPlayerPawn* GetPlayerPawn( std::uint32_t uHandle )
    {
        
        std::uintptr_t uListEntry = g_Memory.Read<std::uintptr_t>( Globals::m_uEntityList + 0x8 * ( ( uHandle & 0x7FFF ) >> 9 ) + 16 );
        if (!uListEntry)
            return 0;

        CCSPlayerPawn* pCSPlayerPawn = g_Memory.Read<CCSPlayerPawn*>( uListEntry + 120 * ( uHandle & 0x1FF ) );

        return pCSPlayerPawn;
    }

    CCSPlayer_ItemServices* m_pItemServices( )
    {
        return reinterpret_cast< CCSPlayer_ItemServices* >( g_Memory.Read<CCSPlayer_ItemServices*>( reinterpret_cast< DWORD64 >( this ) + Offsets::m_pItemServices ) );
    }

    const bool m_bIsScoped( )
    {
        return g_Memory.Read<int>( reinterpret_cast< DWORD64 >( this ) + Offsets::m_bIsScoped );
    }

    const bool m_bIsDefusing( )
    {
        return g_Memory.Read<int>( reinterpret_cast< DWORD64 >( this ) + Offsets::m_bIsDefusing );
    }

    const int m_ArmorValue( )
    {
        return g_Memory.Read<int>( reinterpret_cast< DWORD64 >( this ) + Offsets::m_ArmorValue );
    }

    const int m_iHealth( )
    {
        return g_Memory.Read<int>( reinterpret_cast< DWORD64 >( this ) + Offsets::m_iHealth );
    }

    const float m_flFlashAlpha( )
    {
        return g_Memory.Read<float>( reinterpret_cast< DWORD64 >( this ) + ( Offsets::m_flFlashMaxAlpha - 0x8 ) );
    }

    const float m_flFlashMaxAlpha( )
    {
        return g_Memory.Read<float>( reinterpret_cast< DWORD64 >( this ) + Offsets::m_flFlashMaxAlpha );
    }

    const float m_flFlashDuration( )
    {
        return g_Memory.Read<float>( reinterpret_cast< DWORD64 >( this ) + Offsets::m_flFlashDuration );
    }

    const Vector m_vOldOrigin( )
    {
        return g_Memory.Read<Vector>( reinterpret_cast< DWORD64 >( this ) + Offsets::m_vOldOrigin );
    }

    const Vector m_vecCameraPosition( )
    {
        return g_Memory.Read<Vector>( reinterpret_cast< DWORD64 >( this ) + Offsets::m_vecLastClipCameraPos );
    }

    const QAngle m_angEyeAngles( )
    {
        return g_Memory.Read<QAngle>( reinterpret_cast< DWORD64 >( this ) + Offsets::m_angEyeAngles );
    }

    const EntitySpottedState_t m_entitySpottedState( )
    {
        return g_Memory.Read<EntitySpottedState_t>( reinterpret_cast< DWORD64 >( this ) + Offsets::m_entitySpottedState );
    }
};

class CCSPlayerController_InGameMoneyServices
{
public:

    const int m_iAccount( )
    {
        return g_Memory.Read<int>( reinterpret_cast< DWORD64 >( this ) + Offsets::m_iAccount );
    }

    const int m_iCashSpentThisRound( )
    {
        return g_Memory.Read<int>( reinterpret_cast< DWORD64 >( this ) + Offsets::m_iCashSpentThisRound );
    }

    const int m_iStartAccount( )
    {
        return g_Memory.Read<int>( reinterpret_cast< DWORD64 >( this ) + Offsets::m_iStartAccount );
    }

    const int m_iTotalCashSpent( )
    {
        return g_Memory.Read<int>( reinterpret_cast< DWORD64 >( this ) + Offsets::m_iTotalCashSpent );
    }

    const int m_nPreviousAccount( )
    {
        return g_Memory.Read<int>( reinterpret_cast< DWORD64 >( this ) + Offsets::m_nPreviousAccount );
    }
};

class CCSPlayerController : public CBaseEntity
{
public:
    // get
    const std::uint64_t GetGameSceneNode( )
    {
        return g_Memory.Read<std::uint64_t>( reinterpret_cast< std::uint64_t >( this->m_hPlayerPawn( ) ) + Offsets::dwGameSceneNode );
    }

    const Vector GetBonePosition( int index )
    {
        const std::uint64_t uBoneArray = g_Memory.Read<std::uint64_t>( this->GetGameSceneNode( ) + Offsets::dwModelState + Offsets::dwBoneMatrix );
        return g_Memory.Read<BoneJointData>( uBoneArray + index * sizeof( BoneJointData ) ).m_vecPosition;
    }

    CCSPlayerController_InGameMoneyServices* m_pInGameMoneyServices( )
    {
        return g_Memory.Read<CCSPlayerController_InGameMoneyServices*>( reinterpret_cast< DWORD64 >( this ) + Offsets::m_pInGameMoneyServices );
    }

    const std::string m_strSanitizedPlayerName( )
    {
        std::string sBuffer = { };
        sBuffer.resize( 32 );

        DWORD64 SanitizedPlayerName = g_Memory.Read<DWORD64>( ( reinterpret_cast< DWORD64 >( this ) + Offsets::m_sSanitizedPlayerName ) );
        if (!SanitizedPlayerName)
            return { };

        sBuffer = g_Memory.ReadString( SanitizedPlayerName );
        return sBuffer;
    }

    const bool m_bPawnIsAlive( )
    {
        return g_Memory.Read<bool>( reinterpret_cast< DWORD64 >( this ) + Offsets::m_bPawnIsAlive );
    }

    const int m_iTeamNum( )
    {
        return g_Memory.Read<int>( reinterpret_cast< DWORD64 >( this ) + Offsets::m_iTeamNum );
    }

    const  bool m_bIsLocalPlayerController( )
    {
        return g_Memory.Read<bool>( reinterpret_cast< DWORD64 >( this ) + Offsets::m_bIsLocalPlayerController );
    }

    CCSPlayerPawn* m_hPlayerPawn( )
    {
        std::uint32_t uPlayerPawn = g_Memory.Read<std::uint32_t>( reinterpret_cast< DWORD64 >( this ) + Offsets::m_hPlayerPawn );
        return CCSPlayerPawn::GetPlayerPawn( uPlayerPawn );
    }
};
