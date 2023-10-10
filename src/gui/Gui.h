#pragma once

const char* const KeyNames[ ] = {
	"None",
	"M1",
	"M2",
	"Esc",
	"M3",
	"M4",
	"M5",
	"None",
	"Back",
	"Tab",
	"None",
	"None",
	"BSpace",
	"Enter",
	"None",
	"None",
	"Shift",
	"Ctrl",
	"Alt",
	"PB",
	"CL",
	"VK_KANA",
	"Unknown",
	"VK_JUNJA",
	"VK_FINAL",
	"VK_KANJI",
	"Unknown",
	"Esc",
	"VK_CONVERT",
	"VK_NONCONVERT",
	"VK_ACCEPT",
	"VK_MODECHANGE",
	"Space",
	"Page Up",
	"Page Down",
	"End",
	"Home",
	"Left",
	"Up",
	"Right",
	"Down",
	"VK_SELECT",
	"VK_PRINT",
	"VK_EXECUTE",
	"Print Screen",
	"Ins",
	"Del",
	"VK_HELP",
	"0",
	"1",
	"2",
	"3",
	"4",
	"5",
	"6",
	"7",
	"8",
	"9",
	"Unknown",
	"Unknown",
	"Unknown",
	"Unknown",
	"Unknown",
	"Unknown",
	"Unknown",
	"A",
	"B",
	"C",
	"D",
	"E",
	"F",
	"G",
	"H",
	"I",
	"J",
	"K",
	"L",
	"M",
	"N",
	"O",
	"P",
	"Q",
	"R",
	"S",
	"T",
	"U",
	"V",
	"W",
	"X",
	"Y",
	"Z",
	"Left Windows",
	"Right Windows",
	"VK_APPS",
	"Unknown",
	"VK_SLEEP",
	"NUMPAD0",
	"NUMPAD1",
	"NUMPAD2",
	"NUMPAD3",
	"NUMPAD4",
	"NUMPAD5",
	"NUMPAD6",
	"NUMPAD7",
	"NUMPAD8",
	"NUMPAD9",
	"Multiply",
	"+",
	"Separator",
	"Subtract",
	"-",
	"/",
	"F1",
	"F2",
	"F3",
	"F4",
	"F5",
	"F6",
	"F7",
	"F8",
	"F9",
	"F10",
	"F11",
	"F12",
	"F13",
	"F14",
	"F15",
	"F16",
	"F17",
	"F18",
	"F19",
	"F20",
	"F21",
	"F22",
	"F23",
	"F24",
	"Unknown",
	"Unknown",
	"Unknown",
	"Unknown",
	"Unknown",
	"Unknown",
	"Unknown",
	"Unknown",
	"Num Lock",
	"Scroll lock",
	"VK_OEM_NEC_EQUAL",
	"VK_OEM_FJ_MASSHOU",
	"VK_OEM_FJ_TOUROKU",
	"VK_OEM_FJ_LOYA",
	"VK_OEM_FJ_ROYA",
	"Unknown",
	"Unknown",
	"Unknown",
	"Unknown",
	"Unknown",
	"Unknown",
	"Unknown",
	"Unknown",
	"Unknown",
	"LShift",
	"RShift",
	"LCtrl",
	"RCtrl",
	"LAlt",
	"RAlt"
};


namespace Tabs
{
	inline int m_iCurrentTab = 0;
}

namespace Fonts
{
	inline ImFont* Default;
	inline ImFont* ESP;
}

namespace Gui
{
	inline bool m_bInitialized = false;
	inline bool m_bOpen = false;
	inline ImVec2 m_vecScreenSize = ImVec2( 0.0f, 0.0f );
	
	// initialize window
	void Initialize( unsigned int uFontFlags = 0x0 );
	// update window
	void Update( ImGuiIO& io );
	// render our window
	void Render( );

	inline void SimpleColorPicker( const std::string strName, Color* colColor )
	{
		float col[ 4 ] = { colColor->rBase( ), colColor->gBase( ), colColor->bBase( ), colColor->aBase( ) };
		if ( ImGui::ColorEdit4( strName.c_str( ), col, ImGuiColorEditFlags_NoInputs ) )
			colColor->Set( col[ 0 ], col[ 1 ], col[ 2 ], col[ 3 ] );
	}

	struct KeyBindAnim {

        ImVec2 Maxs;
    };
    inline bool Keybind( const char* str_id, KeyBind_t* keybind ) {

        ImGuiWindow* window = ImGui::GetCurrentWindow( );
        if (window->SkipItems)
            return false;

        ImGui::SameLine( );

        ImGuiContext& g = *GImGui;

        const ImGuiStyle& style = g.Style;
        const ImGuiID id = window->GetID( str_id );
        ImGuiIO* io = &ImGui::GetIO( );

        const ImVec2 label_size = ImGui::CalcTextSize( KeyNames[ keybind->m_iKey ] );
        const ImRect frame_bb( window->DC.CursorPos, window->DC.CursorPos + label_size );
        const ImRect total_bb( window->DC.CursorPos, window->DC.CursorPos + ImVec2( window->Pos.x + window->Size.x - window->DC.CursorPos.x, label_size.y ) );

        ImGui::ItemSize( total_bb, style.FramePadding.y );
        if (!ImGui::ItemAdd( total_bb, id, &frame_bb ))
            return false;

        const bool hovered = ImGui::IsItemHovered( );
        const bool edit_requested = hovered && io->MouseClicked[ 0 ];
        const bool style_requested = hovered && io->MouseClicked[ 1 ];

        if (edit_requested) {
            if (g.ActiveId != id) {
                memset( io->MouseDown, 0, sizeof( io->MouseDown ) );
                memset( io->KeysDown, 0, sizeof( io->KeysDown ) );
                keybind->m_iKey = 0;
            }

            ImGui::SetActiveID( id, window );
            ImGui::FocusWindow( window );
        }
        else if (!hovered && io->MouseClicked[ 0 ] && g.ActiveId == id)
            ImGui::ClearActiveID( );

        bool value_changed = false;
        int key = keybind->m_iKey;

        if (g.ActiveId == id) {
            for (auto i = 0; i < 5; i++) {
                if (ImGui::IsMouseDown( i )) {
                    switch (i) {
                    case 0:
                        key = VK_LBUTTON;
                        break;
                    case 1:
                        key = VK_RBUTTON;
                        break;
                    case 2:
                        key = VK_MBUTTON;
                        break;
                    case 3:
                        key = VK_XBUTTON1;
                        break;
                    case 4:
                        key = VK_XBUTTON2;
                    }
                    value_changed = true;
                    ImGui::ClearActiveID( );
                }
            }

            if (!value_changed) {
                for (auto i = VK_BACK; i <= VK_RMENU; i++) {
                    if (ImGui::IsKeyDown( i )) {
                        key = i;
                        value_changed = true;
                        ImGui::ClearActiveID( );
                    }
                }
            }

            if (ImGui::IsKeyPressedMap( ImGuiKey_Escape )) 
			{
                keybind->m_iKey = 0;
                ImGui::ClearActiveID( );
            }
            else
				keybind->m_iKey = key;
        }

        const bool bLeftClicked = hovered && io->MouseClicked[ 1 ];

        static std::map<ImGuiID, KeyBindAnim> anim;
        auto it_anim = anim.find( id );

        if (it_anim == anim.end( ))
        {
            anim.insert( { id, KeyBindAnim( ) } );
            it_anim = anim.find( id );
            it_anim->second.Maxs = ImVec2( 0, 0 );
        }

        if (bLeftClicked && !( g.ActiveId == id ) && keybind->m_iKey != 0)
        {
            ImGui::OpenPopup( str_id );
        }

        auto buttonLabel1 = std::to_string( *str_id ) + X( "btnHold" );
        auto buttonLabel2 = std::to_string( *str_id ) + X( "btnToggle" );
        auto buttonLabel3 = std::to_string( *str_id ) + X( "btnAlwayson" );
        static int mode = 0;

        ImGui::SetNextWindowSize( it_anim->second.Maxs );
        ImGui::PushStyleVar( ImGuiStyleVar_FrameBorderSize, 0.f );

        // do in cfg ty
        //static int m_iKeyStates[256] = { 0 };
        if (ImGui::BeginPopup( str_id ))
        {
            ImVec2 sizelmao = ImGui::GetWindowSize( );
            it_anim->second.Maxs = ImLerp( it_anim->second.Maxs, ImVec2( 90, 75 ), g.IO.DeltaTime * 12.f );
            if (ImGui::Button( X( "Hold" ), ImVec2( 70, 15 ) )) keybind->m_iMode = 0;
            if (ImGui::Button( X( "Toggle" ), ImVec2( 70, 15 ) )) keybind->m_iMode = 1;
            if (ImGui::Button( X( "Always-on" ), ImVec2( 70, 15 ) )) keybind->m_iMode = 2;

            ImGui::EndPopup( );
        }
        else {
            it_anim->second.Maxs = ImVec2( 0, 0 );
        }
        ImGui::PopStyleVar( );

        char buf_display[ 64 ] = "[-]";
        std::string active = std::vformat( X( "[{}]" ), std::make_format_args( KeyNames[ keybind->m_iKey ] ) );

        if (keybind->m_iKey != 0 && g.ActiveId != id)
            strcpy_s( buf_display, active.c_str( ) );
        else if (g.ActiveId == id)
            strcpy_s( buf_display, X( "[-]" ) );

        window->DrawList->AddText( frame_bb.Min, g.ActiveId == id ? ImColor( 255 / 255.f, 16 / 255.f, 16 / 255.f, g.Style.Alpha ) : ImColor( 90 / 255.f, 90 / 255.f, 90 / 255.f, g.Style.Alpha ), buf_display );

        return value_changed;
    }
}