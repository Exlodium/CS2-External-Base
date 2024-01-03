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
        ImGui::SameLine();
        if (ImGui::Button(X("Configs"), ImVec2(100, 0))) Tabs::m_iCurrentTab = 3;

        switch (Tabs::m_iCurrentTab)
        {
        case 0:
            ImGui::Checkbox( X( "Enable" ), &Config::Get<bool>(g_Variables.m_bEnableAimBot));
            if (Config::Get<bool>(g_Variables.m_bEnableAimBot))
            {
                ImGui::Text( X( "Aimbot key" ) );
                Keybind( X( "Aimbot key" ), &Config::Get<KeyBind_t>(g_Variables.m_AimBotKey) );
         
                ImGui::SliderFloat( X( "Aimbot FOV" ), &Config::Get<float>(g_Variables.m_flAimbotFOV), 0.0f, 180.0f, X( "%.f" ) );
                ImGui::SliderFloat( X( "Aimbot smooth" ), &Config::Get<float>(g_Variables.m_flAimbotSmooth), 0.0f, 100.0f, X( "%.f" ) );
            }
            break;
        case 1:
            ImGui::Checkbox( X( "Enable" ), &Config::Get<bool>(g_Variables.m_bEnableTriggerbot));
            if (Config::Get<bool>(g_Variables.m_bEnableTriggerbot))
            {
                ImGui::Text( X( "Trigger-bot key" ) );
                Keybind( X( "Trigger-bot key" ), &Config::Get<KeyBind_t>(g_Variables.m_TriggerBotKey));

                ImGui::SliderInt( X( "Trigger-bot delay" ), &Config::Get<int>(g_Variables.m_iTriggerBotDelay), 0, 1000, X( "%dms" ) );
            }
            break;
        case 2:
            ImGui::Checkbox( X( "Enable" ), &Config::Get<bool>(g_Variables.m_bEnableVisuals));
            if (Config::Get<bool>(g_Variables.m_bEnableVisuals))
            {       
                ImGui::Checkbox( X( "Ignore teammates" ), &Config::Get<bool>(g_Variables.m_bVisualsIgnoreTeammates));
                ImGui::Checkbox( X( "Only when spotted" ), &Config::Get<bool>(g_Variables.m_bVisualsOnlyWhenSpotted));
                
                ImGui::Checkbox( X( "Box ESP" ), &Config::Get<bool>(g_Variables.m_bBox));
                if (Config::Get<bool>(g_Variables.m_bBox))
                    SimpleColorPicker( X( "Box color" ), &Config::Get<Color>(g_Variables.m_colBox));

                ImGui::Checkbox( X( "Health bar" ), &Config::Get<bool>(g_Variables.m_bHealthBar));

                ImGui::Checkbox( X( "Armor bar" ), &Config::Get<bool>(g_Variables.m_bArmorBar));
                if (Config::Get<bool>(g_Variables.m_bArmorBar))
                    SimpleColorPicker( X( "Armor bar color" ), &Config::Get<Color>(g_Variables.m_colArmorBar));

                ImGui::Checkbox( X( "Name ESP" ), &Config::Get<bool>(g_Variables.m_bName));

                ImGui::Checkbox( X( "Distance ESP" ), &Config::Get<bool>(g_Variables.m_bDistance));

                ImGui::Checkbox( X( "Snap line" ), &Config::Get<bool>(g_Variables.m_bSnapLines));
                if (Config::Get<bool>(g_Variables.m_bSnapLines))
                    SimpleColorPicker( X( "Snap line color" ), &Config::Get<Color>(g_Variables.m_colSnapLines));

                MultiComboBox( X( "Flags" ), arrFlagsNames, Config::Get<std::vector<bool>>(g_Variables.m_vecFlags), IM_ARRAYSIZE(arrFlagsNames));

                SimpleColorPicker( X( "ESP outline color" ), &Config::Get<Color>(g_Variables.m_colOutline));
            }
            break;
        case 3:
            ImGui::BeginColumns( X( "##Config-Columns" ), 2, ImGuiColumnsFlags_NoResize );
            {
                static int nSelected = -1;
                if (ImGui::BeginListBox(X("##Config-List"), ImVec2(-1, -1)))
                {
                    for (size_t uIndex = 0U; uIndex < Config::vecFileNames.size(); uIndex++)
                    {
                        if (ImGui::Selectable(Config::vecFileNames.at(uIndex).c_str(), uIndex == nSelected, ImGuiSelectableFlags_DontClosePopups, ImVec2(0, 0)))
                            nSelected = uIndex;
                    }

                    ImGui::EndListBox();
                }

                ImGui::NextColumn();

                static std::string strConfigFile;
                ImGui::InputTextWithHint(X("##config.file"), X("Enter filename..."), &strConfigFile);
                if (ImGui::Button(X("Create config"), ImVec2(0, 0)))
                {
                    Config::Save(strConfigFile);
                    strConfigFile.clear();
                    Config::Refresh();
                }

                if (ImGui::Button(X("Refresh configs"), ImVec2(0, 0)))
                    Config::Refresh();

                if (nSelected != -1)
                {
                    if (ImGui::Button(X("Save config"), ImVec2(0, 0)))
                    {
                        Config::Save(Config::vecFileNames.at(nSelected));
                    }

                    if (ImGui::Button(X("Load config"), ImVec2(0, 0)))
                    {
                        Config::Load(Config::vecFileNames.at(nSelected));
                    }

                    if (ImGui::Button(X("Remove config"), ImVec2(0, 0)))
                    {
                        Config::Remove(Config::vecFileNames.at(nSelected));
                        Config::Refresh();

                        nSelected = -1;
                    }
                }
				
			}
            ImGui::EndColumns();
            break;
        default:
            ImGui::Text( X( "Something went wrong" ) );
            break;

        }  
    }
    ImGui::End( );
}