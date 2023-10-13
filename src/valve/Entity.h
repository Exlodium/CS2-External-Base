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
    [[maybe_unused]] uint8_t __pad0009[ 0x8 ];
public:
    bool m_bSpotted; //0x8
    std::uint32_t m_bSpottedByMask[ 2 ]; // 0xC
};

class CBaseEntity
{
public:
    static CBaseEntity* GetBaseEntity( int nIdx )
    {
        static std::uintptr_t uEntityList = Globals::m_Memory.Read<std::uintptr_t>( Modules::m_pClient + Offsets::dwEntityList );
        std::uintptr_t uListEntry = Globals::m_Memory.Read<std::uint64_t>( uEntityList + ( 0x8 * ( nIdx & 0x7FFF ) >> 9 ) + 16 );
        if( !uListEntry )
            return 0;

        CBaseEntity* pEntity = Globals::m_Memory.Read<CBaseEntity*>( uListEntry + 120 * ( nIdx & 0x1FF ) );
        return pEntity;
    }
};

class CCSPlayerPawn
{
public:
    static CCSPlayerPawn* GetPlayerPawn( std::uint32_t uHandle )
    {
        static std::uintptr_t uEntityList = Globals::m_Memory.Read<std::uintptr_t>( Modules::m_pClient + Offsets::dwEntityList );
        std::uintptr_t uListEntry = Globals::m_Memory.Read<std::uintptr_t>( uEntityList + 0x8 * ( ( uHandle & 0x7FFF ) >> 9 ) + 16 );
        if (!uListEntry)
            return 0;

        CCSPlayerPawn* pCSPlayerPawn = Globals::m_Memory.Read<CCSPlayerPawn*>( uListEntry + 120 * ( uHandle & 0x1FF ) );

        return pCSPlayerPawn;
    }

    int m_ArmorValue( )
    {
        return Globals::m_Memory.Read<int>( reinterpret_cast< DWORD64 >( this ) + Offsets::m_ArmorValue );
    }

    int m_iHealth( )
    {
        return Globals::m_Memory.Read<int>( reinterpret_cast< DWORD64 >( this ) + Offsets::m_iHealth );
    }

    float m_flFlashAlpha( )
    {
        return Globals::m_Memory.Read<float>( reinterpret_cast< DWORD64 >( this ) + ( Offsets::m_flFlashMaxAlpha - 0x8 ) );
    }

    float m_flFlashMaxAlpha( )
    {
        return Globals::m_Memory.Read<float>( reinterpret_cast< DWORD64 >( this ) + Offsets::m_flFlashMaxAlpha );
    }

    float m_flFlashDuration( )
    {
        return Globals::m_Memory.Read<float>( reinterpret_cast< DWORD64 >( this ) + Offsets::m_flFlashDuration );
    }

    Vector m_vecOrigin( )
    {
        return Globals::m_Memory.Read<Vector>( reinterpret_cast< DWORD64 >( this ) + Offsets::m_vecOrigin );
    }

    Vector m_vecCameraPosition( )
    {
        return Globals::m_Memory.Read<Vector>( reinterpret_cast< DWORD64 >( this ) + Offsets::m_vecLastClipCameraPos );
    }

    QAngle m_angEyeAngles( )
    {
        return Globals::m_Memory.Read<QAngle>( reinterpret_cast< DWORD64 >( this ) + Offsets::m_angEyeAngles );
    }

    EntitySpottedState_t m_entitySpottedState( )
    {
        return Globals::m_Memory.Read<EntitySpottedState_t>( reinterpret_cast< DWORD64 >( this ) + Offsets::m_entitySpottedState );
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

    Vector GetBonePosition( int index )
    {
        const std::uint64_t uBoneArray = Globals::m_Memory.Read<std::uint64_t>( this->GetGameSceneNode( ) + Offsets::dwModelState + Offsets::dwBoneMatrix );
        return Globals::m_Memory.Read<BoneJointData>( uBoneArray + index * sizeof( BoneJointData ) ).m_vecPosition;
    }

    std::string m_strSanitizedPlayerName( )
    {
        std::string sBuffer = { };
        sBuffer.resize( 32 );

        DWORD64 SanitizedPlayerName = Globals::m_Memory.Read<DWORD64>( ( reinterpret_cast< DWORD64 >( this ) + Offsets::m_sSanitizedPlayerName ) );
        if (!SanitizedPlayerName)
            return { };

        sBuffer = Globals::m_Memory.ReadString( SanitizedPlayerName );
        return sBuffer;
    }

    bool m_bPawnIsAlive( )
    {
        return Globals::m_Memory.Read<bool>( reinterpret_cast< DWORD64 >( this ) + Offsets::m_bPawnIsAlive );
    }

    int m_iTeamNum( )
    {
        return Globals::m_Memory.Read<int>( reinterpret_cast< DWORD64 >( this ) + Offsets::m_iTeamNum );
    }

    bool m_bIsLocalPlayerController( )
    {
        return Globals::m_Memory.Read<bool>( reinterpret_cast< DWORD64 >( this ) + Offsets::m_bIsLocalPlayerController );
    }

    CCSPlayerPawn* m_hPlayerPawn( )
    {
        std::uint32_t uPlayerPawn = Globals::m_Memory.Read<std::uint32_t>( reinterpret_cast< DWORD64 >( this ) + Offsets::m_hPlayerPawn );
        return CCSPlayerPawn::GetPlayerPawn( uPlayerPawn );
    }
};
