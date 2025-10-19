#include "../Includes.h"


void Gui::Initialize(unsigned int uFontFlags)
{
    // create fonts
    ImGuiIO& io = ImGui::GetIO();

    ImGui::StyleColorsDark();

    ImFontConfig imVerdanaConfig = { };
    imVerdanaConfig.FontBuilderFlags = ImGuiFreeTypeBuilderFlags::ImGuiFreeTypeBuilderFlags_LightHinting | ImGuiFreeTypeBuilderFlags::ImGuiFreeTypeBuilderFlags_Bold;

    Fonts::Default = io.Fonts->AddFontFromFileTTF(X("C:\\Windows\\Fonts\\Verdana.ttf"), 20, &imVerdanaConfig, io.Fonts->GetGlyphRangesCyrillic());
    Fonts::ESP = io.Fonts->AddFontFromFileTTF(X("C:\\Windows\\Fonts\\Verdana.ttf"), 10, &imVerdanaConfig, io.Fonts->GetGlyphRangesCyrillic());

    m_bInitialized = ImGuiFreeType::BuildFontAtlas(io.Fonts, uFontFlags);
}

void Gui::Update(ImGuiIO& io)
{

    io.MouseDrawCursor = m_bOpen;
    if (m_bOpen)
    {
        POINT p;
        if (GetCursorPos(&p))
        {
            // set imgui mouse position
            io.MousePos = ImVec2(static_cast<float>(p.x), static_cast<float>(p.y));
        }
    }

    // update any menu variables here
}

void Gui::Render()
{
    if (!m_bInitialized)
        return;

    ImGuiIO& io = ImGui::GetIO();
    ImDrawList* pForeGroundDrawList = ImGui::GetForegroundDrawList();

    // update our stuff first
    Gui::Update(io);

    ImGui::SetNextWindowSize(ImVec2(830, 500));
    ImGui::Begin(X("External base"), nullptr, ImGuiWindowFlags_NoResize | ImGuiWindowFlags_NoSavedSettings | ImGuiWindowFlags_NoCollapse);
    {
        if (ImGui::Button(X("Aimbot"), ImVec2(100, 0))) Tabs::m_iCurrentTab = 0;
        ImGui::SameLine();
        if (ImGui::Button(X("Trigger"), ImVec2(100, 0))) Tabs::m_iCurrentTab = 1;
        ImGui::SameLine();
        if (ImGui::Button(X("Visuals"), ImVec2(100, 0))) Tabs::m_iCurrentTab = 2;
        ImGui::SameLine();
        if (ImGui::Button(X("Configs"), ImVec2(100, 0))) Tabs::m_iCurrentTab = 3;

        switch (Tabs::m_iCurrentTab)
        {
        case 0:
            ImGui::Checkbox(X("Enable"), &CONFIG_GET(bool, g_Variables.m_AimBot.m_bEnableAimbot));
            break;
        case 1:
            ImGui::Checkbox(X("Enable"), &CONFIG_GET(bool, g_Variables.m_TriggerBot.m_bEnableTriggerbot));
            break;
        case 2:
            ImGui::Checkbox(X("Enable"), &CONFIG_GET(bool, g_Variables.m_PlayerVisuals.m_bEnableVisuals));
            break;
        case 3:
            ImGui::BeginColumns(X("##Config-Columns"), 2, ImGuiColumnsFlags_NoResize);
            {
                static int nSelected = -1;
                if (ImGui::BeginListBox(X("##Config-List"), ImVec2(-1, -1)))
                {
                    for (size_t uIndex = 0U; uIndex < Config::m_vecFileNames.size(); uIndex++)
                    {
                        if (ImGui::Selectable(Config::m_vecFileNames.at(uIndex).c_str(), uIndex == nSelected, ImGuiSelectableFlags_DontClosePopups, ImVec2(0, 0)))
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
                        Config::Save(Config::m_vecFileNames.at(nSelected));
                    }

                    if (ImGui::Button(X("Load config"), ImVec2(0, 0)))
                    {
                        Config::Load(Config::m_vecFileNames.at(nSelected));
                    }

                    if (ImGui::Button(X("Remove config"), ImVec2(0, 0)))
                    {
                        Config::Remove(Config::m_vecFileNames.at(nSelected));
                        Config::Refresh();

                        nSelected = -1;
                    }
                }

            }
            ImGui::EndColumns();
            break;
        default:
            ImGui::Text(X("Something went wrong"));
            break;

        }
    }
    ImGui::End();
}