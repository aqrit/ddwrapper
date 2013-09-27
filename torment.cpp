#include "header.h"
#include <gl\gl.h>
#define GL_UNSIGNED_SHORT_5_6_5           0x8363

HWND g_hWnd;
DWORD g_dwWidth;
DWORD g_dwHeight;
HGLRC g_hRC = NULL;
HDC g_hDC;
WNDPROC g_fnWwndProc;

WRAP* primary;
LPDIRECTDRAWSURFACE dds_secondary;
WRAP* pointer;
WRAP* pointer2;
WRAP* fx;
WRAP* fog;
WRAP* fog2;
WRAP* tool;
WRAP* tool2;
WRAP* tiles;
WRAP* movie = NULL;
WRAP* movie2 = NULL;

DWORD fog_coverage[4800];
DWORD movie_width;
DWORD movie_height;
/*
Torment determines color depth for windowed mode using GDI32.GetDeviceCaps
*/



typedef BOOL (__stdcall* SetProcessDPIAwareFunc)();
typedef const char* (__stdcall* PFNWGLGETEXTENSIONSSTRINGEXTPROC)();
typedef BOOL (__stdcall* PFNWGLSWAPINTERVALEXTPROC)(int interval);

HRESULT DDFloodFill(IDirectDrawSurface* pdds, PRECT prc, COLORREF cr){
	DDBLTFX effects;
	RtlSecureZeroMemory( &effects, sizeof(effects) );
	effects.dwSize = sizeof(effects);
	effects.dwFillColor = cr;
	return pdds->lpVtbl->Blt( pdds, prc, NULL, NULL, (DDBLT_COLORFILL | DDBLT_WAIT), &effects );
}


char* my_strstr ( const char* str, const char* sub_str ){
	const char* sub_str_end = sub_str;
	while(*sub_str_end) sub_str_end++;
	while(*str){
		if( *str++ == *sub_str ){
			const char* s = str;
			const char* ss = &sub_str[1];
			do{
				if( ss == sub_str_end ) return ((char*)--str); // found
				if( *s != *ss++ ) break;
			}while( *s++ );
		}
	}
	return 0; // not found
}

void SetupOpenGL()
{
	g_hDC = GetDC( g_hWnd );
    PIXELFORMATDESCRIPTOR pfd =
    {
        sizeof(PIXELFORMATDESCRIPTOR),
        1,
        PFD_DRAW_TO_WINDOW | PFD_SUPPORT_OPENGL | PFD_DOUBLEBUFFER,    //Flags
        PFD_TYPE_RGBA,            //The kind of framebuffer. RGBA or palette.
        32,                        //Colordepth of the framebuffer.
        0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
        0,                        //Number of bits for the depthbuffer
        0,                        //Number of bits for the stencilbuffer
        0,                        //Number of Aux buffers in the framebuffer.
        PFD_MAIN_PLANE,
        0, 0, 0, 0
    };
	SetPixelFormat( g_hDC, ChoosePixelFormat( g_hDC, &pfd ), &pfd );
	g_hRC = wglCreateContext( g_hDC );
	wglMakeCurrent( g_hDC, g_hRC );

	__asm 
	{ // no float support... so push by hand
		push 0xBF800000
		push 0x3F800000
		call DWORD PTR DS:[glPixelZoom] // glPixelZoom( 1.0, -1.0 ) // 0,0 is at top-left
		push 0
		push 0
		push 0
		push 0
		call DWORD PTR DS:[glClearColor] // black
	}
	glRasterPos4i( -1, 1, 0, 1 ); // move cursor to top-left

	glClear( GL_COLOR_BUFFER_BIT ); // colorfill with glClearColor
	SwapBuffers( g_hDC );
	glClear( GL_COLOR_BUFFER_BIT );

	// enable vsync
	PFNWGLGETEXTENSIONSSTRINGEXTPROC wglGetExtensionsStringEXT = (PFNWGLGETEXTENSIONSSTRINGEXTPROC) wglGetProcAddress( "wglGetExtensionsStringEXT" );
    if( (long)wglGetExtensionsStringEXT > 4 )
	{
		if( my_strstr( wglGetExtensionsStringEXT(), "WGL_EXT_swap_control" ) != NULL )
		{
			PFNWGLSWAPINTERVALEXTPROC wglSwapIntervalEXT = (PFNWGLSWAPINTERVALEXTPROC) wglGetProcAddress( "wglSwapIntervalEXT" );
			if( (long)wglSwapIntervalEXT > 4 )
			{
				wglSwapIntervalEXT( 1 );
			}
		}
	}
}

void GoFullScreen()
{
	SetWindowPos( g_hWnd, HWND_TOP, 0, 0, g_dwWidth, g_dwHeight, SWP_SHOWWINDOW ); 

	if( g_hRC == NULL ) SetupOpenGL(); // this is here to avoid a double flash on startup :(

	DEVMODE devMode;
	RtlSecureZeroMemory(&devMode, sizeof(devMode));
	devMode.dmSize = sizeof(devMode);
	devMode.dmFields = DM_BITSPERPEL | DM_PELSWIDTH | DM_PELSHEIGHT; 
	devMode.dmBitsPerPel = 32;
    devMode.dmPelsWidth  = g_dwWidth;
    devMode.dmPelsHeight = g_dwHeight;
	ChangeDisplaySettings(&devMode, CDS_FULLSCREEN);
}

LRESULT __stdcall WindowProc( HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam )
{
    switch( uMsg )
    {
		// things to look into...
		case WM_PAINT:
		case WM_ERASEBKGND:
		case WM_SIZE:
		case WM_CREATE:
		case WM_MOVE:
		case WM_CAPTURECHANGED:
		case WM_KILLFOCUS:
		case WM_ACTIVATEAPP:
		case WM_DEVICECHANGE:
		case WM_DWMCOMPOSITIONCHANGED:
        {
			break;
		}
		case WM_ACTIVATE:
		{
			BOOL focused = LOWORD(wParam) != WA_INACTIVE;
			BOOL iconified = HIWORD(wParam) ? TRUE : FALSE;

			if( focused && iconified ) focused = FALSE; // can't have focus if minimized...

			if( !focused ) // lost focus
			{
				if( !iconified ) ShowWindow( hWnd, SW_MINIMIZE );
			}
			else // got focus
			{
				GoFullScreen();
			}
			break;
		}
		case WM_SHOWWINDOW:
		{
			GoFullScreen();
			break;
		}
		case WM_SYSCOMMAND: 
		{
			switch(wParam & 0xFFF0)
			{	
				case SC_CLOSE:
				case SC_SCREENSAVE: 
                case SC_MONITORPOWER:
				case SC_KEYMENU: // Alt + Enter
					break;
				case SC_MINIMIZE:
					ShowWindow( hWnd, SW_MINIMIZE );
					break;
				case SC_DEFAULT:
				case SC_MAXIMIZE:
				case SC_RESTORE:
					GoFullScreen();
					break;
			}
			break;
		}
	}
	return CallWindowProc( g_fnWwndProc, hWnd, uMsg, wParam, lParam );
}

GUID* DirectDrawCreateDriver_pst( GUID* lpGUID )
{ 
	// DpiAware
	HMODULE hUser32 = GetModuleHandle( "user32.dll" );
	if( hUser32 )
	{
		SetProcessDPIAwareFunc setDPIAware = (SetProcessDPIAwareFunc)GetProcAddress(hUser32, "SetProcessDPIAware");
		if (setDPIAware) setDPIAware();
	}

	// Force DirectDrawEmulation
	return ( (GUID*) DDCREATE_EMULATIONONLY ); 
}

ULONG Release_dds_pst( WRAP* This )
{
	ULONG cnt =  WrapRelease( This );
	if( cnt == 0 )
	{
		if( This == movie ) movie = NULL;
		if( This == movie2 ) movie2 = NULL;
	}
	return cnt;
}

HRESULT SetCooperativeLevel_pst( LPDIRECTDRAW lpDD, HWND hWnd, DWORD dwFlags )
{	
	// save
	g_hWnd = hWnd;

	// not fullscreen exclusive
	return lpDD->lpVtbl->SetCooperativeLevel( lpDD, hWnd, DDSCL_NORMAL );
}

HRESULT SetDisplayMode_pst( LPDIRECTDRAW lpDD, DWORD dwWidth, DWORD dwHeight, DWORD dwBPP )
{
	// save 
	g_dwWidth = dwWidth;
	g_dwHeight = dwHeight;

	// init
	GoFullScreen();
	g_fnWwndProc = (WNDPROC) SetWindowLong( g_hWnd, GWL_WNDPROC, (LONG)WindowProc ); // hook window messages

	// not fullscreen exclusive
	return DD_OK; // eat it
}

HRESULT GetDisplayMode_pst( LPDIRECTDRAW lpDD, LPDDSURFACEDESC lpDDSurfaceDesc )
{

	HRESULT hResult = lpDD->lpVtbl->GetDisplayMode( lpDD, lpDDSurfaceDesc );
	if( SUCCEEDED( hResult ) )
	{
		// report 16 bit color
		lpDDSurfaceDesc->ddpfPixelFormat.dwRGBBitCount = 16;
		lpDDSurfaceDesc->ddpfPixelFormat.dwBBitMask = 0x0000001F;
		lpDDSurfaceDesc->ddpfPixelFormat.dwGBitMask = 0x000007E0;
		lpDDSurfaceDesc->ddpfPixelFormat.dwRBitMask = 0x0000F800;
		lpDDSurfaceDesc->ddpfPixelFormat.dwRGBAlphaBitMask = 0;
	}

	return hResult;
}


HRESULT Flip_pst( LPDIRECTDRAWSURFACE lpDDS, LPDIRECTDRAWSURFACE lpDDSurfaceTargetOverride, DWORD dwFlags )
{
	HRESULT hResult;

	if( lpDDS == primary->dds1 )
	{
		DDSURFACEDESC ddsd;
		
		glFinish();
		lpDDS->lpVtbl->Unlock( lpDDS, NULL );
		hResult = lpDDS->lpVtbl->Flip( lpDDS, lpDDSurfaceTargetOverride, dwFlags );
		ddsd.dwSize = sizeof(ddsd);
		if(SUCCEEDED( lpDDS->lpVtbl->Lock( lpDDS, NULL, &ddsd, DDLOCK_WAIT, NULL) ) )
		{
			glDrawPixels( g_dwWidth, g_dwHeight, GL_RGB, GL_UNSIGNED_SHORT_5_6_5, ddsd.lpSurface );
		}
		SwapBuffers( g_hDC );
	}
	else
	{
		hResult = lpDDS->lpVtbl->Flip( lpDDS, lpDDSurfaceTargetOverride, dwFlags );
	}
	return hResult;

}

HRESULT CreateSurface_pst( WRAP* This, LPDDSURFACEDESC lpDDSurfaceDesc, LPDIRECTDRAWSURFACE *lplpDDSurface, IUnknown *pUnkOuter ) 
{
	HRESULT hResult;
	LPDIRECTDRAW lpDD = This->dd1;

	// fake primary
	if(( lpDDSurfaceDesc->dwFlags & DDSD_CAPS ) && ( lpDDSurfaceDesc->ddsCaps.dwCaps & DDSCAPS_PRIMARYSURFACE ))
	{
		RtlSecureZeroMemory( lpDDSurfaceDesc, sizeof( DDSURFACEDESC ) );
		lpDDSurfaceDesc->dwSize = sizeof( DDSURFACEDESC );
		lpDDSurfaceDesc->dwFlags = DDSD_CAPS | DDSD_HEIGHT | DDSD_WIDTH | DDSD_PIXELFORMAT;
		lpDDSurfaceDesc->ddsCaps.dwCaps = DDSCAPS_OFFSCREENPLAIN | DDSCAPS_SYSTEMMEMORY;
		lpDDSurfaceDesc->dwHeight = g_dwHeight;
		lpDDSurfaceDesc->dwWidth = g_dwWidth;
		lpDDSurfaceDesc->ddpfPixelFormat.dwSize  = sizeof(DDPIXELFORMAT);
		lpDDSurfaceDesc->ddpfPixelFormat.dwFlags = DDPF_RGB;
		lpDDSurfaceDesc->ddpfPixelFormat.dwRGBBitCount = 16;
		lpDDSurfaceDesc->ddpfPixelFormat.dwBBitMask = 0x0000001F;
		lpDDSurfaceDesc->ddpfPixelFormat.dwGBitMask = 0x000007E0;
		lpDDSurfaceDesc->ddpfPixelFormat.dwRBitMask = 0x0000F800;

		// double buffered 
		LPDIRECTDRAWSURFACE dds_primary;
		lpDD->lpVtbl->CreateSurface( lpDD, lpDDSurfaceDesc, &dds_primary, NULL );
		lpDD->lpVtbl->CreateSurface( lpDD, lpDDSurfaceDesc, &dds_secondary, NULL );
		dds_primary->lpVtbl->AddAttachedSurface( dds_primary, dds_secondary );
		*lplpDDSurface = dds_primary;

		hResult = DD_OK;
	}
	else
	{
		// no video memory
		lpDDSurfaceDesc->ddsCaps.dwCaps &= ~DDSCAPS_VIDEOMEMORY; 
		lpDDSurfaceDesc->ddsCaps.dwCaps |= DDSCAPS_OFFSCREENPLAIN | DDSCAPS_SYSTEMMEMORY;
		lpDDSurfaceDesc->dwFlags |= DDSD_PIXELFORMAT;

		// display mode is 32-bit but we want surfaces 16-bit surfaces
		RtlSecureZeroMemory( &lpDDSurfaceDesc->ddpfPixelFormat, sizeof( DDPIXELFORMAT ) );
		lpDDSurfaceDesc->ddpfPixelFormat.dwSize  = sizeof(DDPIXELFORMAT);
		lpDDSurfaceDesc->ddpfPixelFormat.dwFlags = DDPF_RGB;
		lpDDSurfaceDesc->ddpfPixelFormat.dwRGBBitCount = 16;
		lpDDSurfaceDesc->ddpfPixelFormat.dwBBitMask = 0x0000001F;
		lpDDSurfaceDesc->ddpfPixelFormat.dwGBitMask = 0x000007E0;
		lpDDSurfaceDesc->ddpfPixelFormat.dwRBitMask = 0x0000F800;

		hResult = lpDD->lpVtbl->CreateSurface( lpDD, lpDDSurfaceDesc, lplpDDSurface, pUnkOuter);
	}

	if( SUCCEEDED( hResult ) )
	{
		Wrap( This->dd_parent, dd_to_dds_vtbl( This ), (void**)lplpDDSurface );

		// save ifaces
		static int i = 0;
		switch( i )
		{
			case 0: // primary [5120][3840] // secondary [5120][3840]
				primary = (WRAP*) *lplpDDSurface;
				break;
			case 1: // pointer[128][64]
				pointer = (WRAP*) *lplpDDSurface;
				break;
			case 2: // pointer2[128][64]
				pointer2 = (WRAP*) *lplpDDSurface;
				break;
			case 3: // fx[160][160]
				fx = (WRAP*) *lplpDDSurface;
				break;
			case 4: // fog[64][64]
				fog = (WRAP*) *lplpDDSurface;
				break;
			case 5: // fog2[64][64]
				fog2 = (WRAP*) *lplpDDSurface;
				break;
			case 6: // tool[16][320]
				tool = (WRAP*) *lplpDDSurface;
				break;
			case 7: // tool2[16][320]
				tool2 = (WRAP*) *lplpDDSurface;
				break;
			case 8: // tiles[80][60][64][64]
				tiles = (WRAP*) *lplpDDSurface;
				break;
			case 4808: // movie[640][320]
				movie = (WRAP*) *lplpDDSurface;
				break;
			case 4809: // movie2[640][320]
				i -= 2;
				movie2 = (WRAP*) *lplpDDSurface;
				break;
		}
		i++;
	}
	
	return hResult;
}


HRESULT BltFast_pst( WRAP* This, DWORD dwX, DWORD dwY, WRAP* src, LPRECT lpSrcRect, DWORD dwTrans ) 
{
	// postpone drawing fog...
	// note: mirrored (bottom/right) items are marked in Blt()
	long tile_index = ( This - tiles ) / sizeof( WRAP ); // the allocator *should* keep all these in order...
	if( ( tile_index >= 0 ) && ( tile_index < 4800 ) ) // 80x60 grid of 64x64 px tiles 
	{
		DWORD subsquare_shift = (dwX >> 2) + (dwY >> 1); // each tile is treated as 4 different 32x32 px squares...
		if( ( src == fog ) && ( lpSrcRect->top == 32 ) && ( lpSrcRect->left == 32 ) )
		{
			fog_coverage[ tile_index ] |= ( 0x000000FF << subsquare_shift ); // darken square
			return DD_OK;
		}
		if( ( src == fog2 ) && ( lpSrcRect != NULL ) )
		{	
			if( lpSrcRect->top != 0 )
			{
				if( lpSrcRect->left == 0 ) fog_coverage[ tile_index ] |= ( 0x0000001E << subsquare_shift ); // darken top left triangle 
				else fog_coverage[ tile_index ] |= ( 0x00000087 << subsquare_shift ); // darken top right triangle
			}
			// else eat 32x16 rects 
			return DD_OK;
		}
	}

	// apply fog now.
	if( This->dds1 == dds_secondary )
	{
		long tile_index = ( src - tiles ) / sizeof( WRAP ); // the allocator *should* keep all these in order...
		if( ( tile_index >= 0 ) && ( tile_index < 4800 ) ) // 80x60 grid of 64x64 px tiles 
		{
			DrawFog( GetInnerInterface( ((LPDIRECTDRAWSURFACE)src) ), fog_coverage[tile_index] );
			fog_coverage[tile_index] = 0;
		}
	}

	return This->dds1->lpVtbl->BltFast( This->dds1, dwX, dwY, GetInnerInterface( ((LPDIRECTDRAWSURFACE)src) ), lpSrcRect, dwTrans );
}

HRESULT Blt_pst( WRAP* This, LPRECT lpDestRect, LPDIRECTDRAWSURFACE lpDDSrcSurface, LPRECT lpSrcRect, DWORD dwFlags, LPDDBLTFX lpDDBltFx )
{
	// postpone drawing fog...
	// note: non-mirrored (top/left) items are marked in BltFast()
	long tile_index = ( This - tiles ) / sizeof( WRAP ); // the allocator *should* keep all these in order...
	if( ( tile_index >= 0 ) && ( tile_index < 4800 ) ) // 80x60 grid of 64x64 px tiles 
	{
		if( ( lpDDSrcSurface == fog2->dds1 ) && ( lpSrcRect != NULL ) )
		{	
			if( lpSrcRect->top != 0 )
			{
				// each tile is treated as 4 different 32x32 px squares...
				DWORD subsquare_shift = ( lpDestRect == NULL ) ? 0 : subsquare_shift = ( lpDestRect->left >> 2 ) + ( lpDestRect->top >> 1 );	
				if( lpSrcRect->left == 0 ) fog_coverage[ tile_index ] |= ( 0x000000E1 << subsquare_shift ); // darken bottom right triangle
				else fog_coverage[ tile_index ] |= ( 0x00000078 << subsquare_shift ); // darken bottom left triangle
			}
			// else eat 32x16 rects 
			return DD_OK;
		}
	}

	// stretch movies
	if((This->dds1 == dds_secondary) && (movie != NULL) && (movie2 != NULL) && ((movie->dds1 == lpDDSrcSurface) || (movie2->dds1 == lpDDSrcSurface)))
	{

		DWORD movie_width = lpSrcRect->right;
		DWORD movie_height = lpSrcRect->bottom;
		DWORD screen_width = g_dwWidth;
		DWORD screen_height = g_dwHeight;
		DWORD new_height;
		DWORD new_width;
		RECT rc_fill;
		
		// for compat with previous stretch movie patch
		RECT rc_dst;
		lpDestRect = &rc_dst;
		//
		
		new_height = (movie_height * screen_width) / movie_width;
		if( new_height > screen_height )
		{
			new_width = (movie_width * screen_height) / movie_height;
			lpDestRect->left = ((screen_width - new_width) >> 1);
			lpDestRect->top = 0;
			lpDestRect->right = lpDestRect->left + new_width;
			lpDestRect->bottom = screen_height;

			rc_fill.left   = 0;
			rc_fill.top    = 0;
			rc_fill.right  = lpDestRect->left;
			rc_fill.bottom = screen_height;
			DDFloodFill( dds_secondary, &rc_fill, 0 );
			rc_fill.left   = lpDestRect->right;
			rc_fill.top    = 0;
			rc_fill.right  = screen_width;
			rc_fill.bottom = screen_height;
			DDFloodFill( dds_secondary, &rc_fill, 0 );
		}
		else 
		{
			lpDestRect->left = 0;
			lpDestRect->top = (( screen_height - new_height ) >> 1);
			lpDestRect->right = screen_width;
			lpDestRect->bottom = lpDestRect->top + new_height;

			rc_fill.left   = 0;
			rc_fill.top    = 0;
			rc_fill.right  = screen_width;
			rc_fill.bottom = lpDestRect->top;
			DDFloodFill( dds_secondary, &rc_fill, 0 );
			rc_fill.left   = 0;
			rc_fill.top    = lpDestRect->bottom;
			rc_fill.right  = screen_width;
			rc_fill.bottom = screen_height;
			DDFloodFill( dds_secondary, &rc_fill, 0 );
		}
	}

	return This->dds1->lpVtbl->Blt( This->dds1, lpDestRect, lpDDSrcSurface, lpSrcRect, dwFlags, lpDDBltFx ); 
}
