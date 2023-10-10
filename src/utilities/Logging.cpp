#include "../Precompiled.h"

bool Logging::Attach( const char* szConsoleTitle )
{
	// allocate memory for console
	if (!AllocConsole( ))
		return false;

	// attach console to current process
	AttachConsole( ATTACH_PARENT_PROCESS );

	/*
	 * redirect cout stdin - to read / stdout - to write in console window
	 * cuz we doesn't need read anything from console we use only write stream
	 */
	if (freopen_s( &pStream, X( "CONOUT$" ), X( "w" ), stdout ) != 0)
		return false;

	// set console window title
	if (!SetConsoleTitleA( szConsoleTitle ))
		return false;

	return true;
}

void Logging::Detach( )
{
	// close write console stream
	fclose( pStream );

	// free allocated memory
	FreeConsole( );

	// get console window
	if (const auto hConsoleWnd = GetConsoleWindow( ); hConsoleWnd != nullptr)
		// close console window
		PostMessageW( hConsoleWnd, WM_CLOSE, 0U, 0L );
}