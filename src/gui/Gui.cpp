#include "../Precompiled.h"

const char* arrFlagsNames[ ] =
{
    "Money flag",
    "Armor flag",
    "Kit flag",
    "Defusing flag",
    "Zoom flag"
};

void Gui::Initialize( unsigned int uFontFlags )
{
    // create fonts
    ImGuiIO& io = ImGui::GetIO( );

    ImGui::StyleColorsDark( );
        
    ImFontConfig imVerdanaConfig = { };
    imVerdanaConfig.FontBuilderFlags = ImGuiFreeTypeBuilderFlags::ImGuiFreeTypeBuilderFlags_LightHinting | ImGuiFreeTypeBuilderFlags::ImGuiFreeTypeBuilderFlags_Bold;
    Fonts::Default = io.Fonts->AddFontFromFileTTF( X( "C:\\Windows\\Fonts\\Verdana.ttf" ), 20, &imVerdanaConfig, io.Fonts->GetGlyphRangesCyrillic( ) );
    Fonts::ESP = io.Fonts->AddFontFromFileTTF( X( "C:\\Windows\\Fonts\\Verdana.ttf" ), 10, &imVerdanaConfig, io.Fonts->GetGlyphRangesCyrillic( ) );

    m_bInitialized = ImGuiFreeType::BuildFontAtlas( io.Fonts, uFontFlags );
}

void Gui::Update( ImGuiIO& io )
{
    Gui::m_vecScreenSize = io.DisplaySize;

    io.MouseDrawCursor = m_bOpen;
    if (m_bOpen)
    {
        POINT p;
        if (GetCursorPos( &p ))
        {
            // set imgui mouse position
            io.MousePos = ImVec2( static_cast< float >( p.x ), static_cast< float >( p.y ) );
        }
    }

    // update any menu variables here
}

void Gui::Render( )
{
    if (!m_bInitialized)
		return;

    ImGuiIO& io = ImGui::GetIO( );
    ImDrawList* pForeGroundDrawList = ImGui::GetForegroundDrawList( );

    // update our stuff first
    Gui::Update( io );

    ImGui::SetNextWindowSize( ImVec2( 830, 500 ) );
    ImGui::Begin( X( "External base" ), nullptr, ImGuiWindowFlags_NoResize | ImGuiWindowFlags_NoSavedSettings | ImGuiWindowFlags_NoCollapse );
    {
        if (ImGui::Button( X( "Aimbot" ), ImVec2( 100, 0 ) ) ) Tabs::m_iCurrentTab = 0;
        ImGui::SameLine( );
        if ( ImGui::Button( X( "Trigger" ), ImVec2( 100, 0 ) ) ) Tabs::m_iCurrentTab = 1;
        ImGui::SameLine( );
        if ( ImGui::Button( X( "Visuals" ), ImVec2( 100, 0 ) ) ) Tabs::m_iCurrentTab = 2;

        switch (Tabs::m_iCurrentTab)
        {
        case 0:
            ImGui::Checkbox( X( "Enable" ), &Variables::Aimbot::m_bEnabled );
            if (Variables::Aimbot::m_bEnabled)
            {
                ImGui::Text( X( "Aimbot key" ) );
                Keybind( X( "Aimbot key" ), &Variables::Aimbot::m_iAimbotKey );
         
                ImGui::SliderFloat( X( "Aimbot FOV" ), &Variables::Aimbot::m_flAimbotFOV, 0.0f, 180.0f, X( "%.f" ) );
                ImGui::SliderFloat( X( "Aimbot smooth" ), &Variables::Aimbot::m_flAimbotSmooth, 0.0f, 100.0f, X( "%.f" ) );
            }
            break;
        case 1:
            ImGui::Checkbox( X( "Enable" ), &Variables::TriggerBot::m_bEnabled );
            if (Variables::TriggerBot::m_bEnabled)
            {
                ImGui::Text( X( "Trigger-bot key" ) );
                Keybind( X( "Trigger-bot key" ), &Variables::TriggerBot::m_iTriggerBotkey );

                ImGui::SliderInt( X( "Trigger-bot delay" ), &Variables::TriggerBot::m_iTriggerBotDelay, 0, 1000, X( "%dms" ) );
            }
            break;
        case 2:
            ImGui::Checkbox( X( "Enable" ), &Variables::Visuals::m_bEnabled );
            if (Variables::Visuals::m_bEnabled)
            {       
                ImGui::Checkbox( X( "Ignore teammates" ), &Variables::Visuals::m_bIgnoreTeammates );
                ImGui::Checkbox( X( "Only when spotted" ), &Variables::Visuals::m_bOnlyWhenSpotted );
                
                ImGui::Checkbox( X( "Box ESP" ), &Variables::Visuals::m_bBox );
                if (Variables::Visuals::m_bBox)
                    SimpleColorPicker( X( "Box color" ), &Variables::Visuals::m_colBox );

                ImGui::Checkbox( X( "Health bar" ), &Variables::Visuals::m_bHealthBar );

                ImGui::Checkbox( X( "Armor bar" ), &Variables::Visuals::m_bArmorBar );
                if ( Variables::Visuals::m_bArmorBar)
                    SimpleColorPicker( X( "Armor bar color" ), &Variables::Visuals::m_colArmorBar );

                ImGui::Checkbox( X( "Name ESP" ), &Variables::Visuals::m_bName );

                ImGui::Checkbox( X( "Distance ESP" ), &Variables::Visuals::m_bDistance );

                ImGui::Checkbox( X( "Snap line" ), &Variables::Visuals::m_bSnapLines );
                if (Variables::Visuals::m_bSnapLines)
                    SimpleColorPicker( X( "Snap line color" ), &Variables::Visuals::m_colSnapLines );

                MultiCombo( X( "Flags" ), arrFlagsNames, Variables::Visuals::m_arrFlags, IM_ARRAYSIZE( arrFlagsNames ) );

                // TODO: Make this a multi combo
                /*ImGui::Checkbox( X( "Money flag" ), &Variables::Visuals::m_arrFlags[ 0 ] );
                ImGui::Checkbox( X( "Armor flag" ), &Variables::Visuals::m_arrFlags[ 1 ] );
                ImGui::Checkbox( X( "Kit flag" ), &Variables::Visuals::m_arrFlags[ 2 ] );
                ImGui::Checkbox( X( "Defusing flag" ), &Variables::Visuals::m_arrFlags[ 3 ] );
                ImGui::Checkbox( X( "Zoom flag" ), &Variables::Visuals::m_arrFlags[ 4 ] );*/

                SimpleColorPicker( X( "ESP outline color" ), &Variables::Visuals::m_colOutline );
            }
            break;
        default:
            ImGui::Text( X( "Something went wrong" ) );
            break;

        }  
    }
    ImGui::End( );
}