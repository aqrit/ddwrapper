#include "header.h"
#include <gl\gl.h>
#define GL_UNSIGNED_SHORT_5_6_5           0x8363

extern "C" { int _fltused=0; } // no crt so ... float support is questionable...

void inline scale2x( void* void_dst, unsigned long dst_pitch, void* void_src, unsigned long src_pitch, unsigned long width, unsigned long height );

HWND g_hWnd;
DWORD g_dwWidth;
DWORD g_dwHeight;
HGLRC g_hRC = NULL;
HDC g_hDC;
WNDPROC g_fnWwndProc;
GLuint g_Texture; 

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

struct CONFIG
{
	BOOL scale2x;
	BOOL dd_scale;
	BOOL xfog;
} Config;

typedef BOOL (__stdcall* SetProcessDPIAwareFunc)();
typedef const char* (__stdcall* PFNWGLGETEXTENSIONSSTRINGEXTPROC)();
typedef BOOL (__stdcall* PFNWGLSWAPINTERVALEXTPROC)(int interval);


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

void GetConfig()
{
	char* szCmdLine = GetCommandLine();

	Config.scale2x = my_strstr( szCmdLine, "-scale2x" ) ? TRUE : FALSE;
	Config.dd_scale = my_strstr( szCmdLine, "-ovid" ) ? FALSE : TRUE;
	Config.xfog = my_strstr( szCmdLine, "-ofog" ) ? FALSE : TRUE;
}


HRESULT DDFloodFill(IDirectDrawSurface* pdds, PRECT prc, COLORREF cr){
	DDBLTFX effects;
	RtlSecureZeroMemory( &effects, sizeof(effects) );
	effects.dwSize = sizeof(effects);
	effects.dwFillColor = cr;
	return pdds->lpVtbl->Blt( pdds, prc, NULL, NULL, (DDBLT_COLORFILL | DDBLT_WAIT), &effects );
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
	glClearColor( 0, 0, 0, 0 ); // black
	glClear( GL_COLOR_BUFFER_BIT ); 
	SwapBuffers( g_hDC );
	glViewport( 0, 0, (GLsizei)g_dwWidth, (GLsizei)g_dwHeight );
	glMatrixMode(GL_PROJECTION);
	glLoadIdentity();
	glOrtho( 0, (double)g_dwWidth, (double)g_dwHeight, 0, -1, 1 );
	glMatrixMode( GL_MODELVIEW );
	glLoadIdentity();
	glEnable( GL_TEXTURE_2D );
	glDisable( GL_LIGHTING );
    glDisable( GL_DITHER );
    glDisable( GL_BLEND );
    glDisable( GL_DEPTH_TEST );
	glGenTextures( 1, &g_Texture );
    glBindTexture( GL_TEXTURE_2D, g_Texture );
	glTexImage2D( GL_TEXTURE_2D, 0, GL_RGB, g_dwWidth, g_dwHeight, 0, GL_RGB, GL_UNSIGNED_SHORT_5_6_5, 0 ); // NPOT
	glTexParameteri( GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST );
    glTexParameteri( GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST );
	glTexEnvi( GL_TEXTURE_ENV, GL_TEXTURE_ENV_MODE, GL_REPLACE );
	glPixelStorei( GL_UNPACK_ALIGNMENT, 4 );

	// enable vsync
	// WGL_EXT_swap_control
	PFNWGLSWAPINTERVALEXTPROC wglSwapIntervalEXT = (PFNWGLSWAPINTERVALEXTPROC) wglGetProcAddress( "wglSwapIntervalEXT" );
	if( (long)wglSwapIntervalEXT > 4 )
	{
		wglSwapIntervalEXT( 1 );
	}
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
			glTexSubImage2D( GL_TEXTURE_2D, 0, 0, 0, g_dwWidth, g_dwHeight, GL_RGB, GL_UNSIGNED_SHORT_5_6_5, ddsd.lpSurface );

			glBegin(GL_QUADS);
				glTexCoord2i( 0, 0 ); glVertex2i( 0, 0 );
				glTexCoord2i( 1, 0 ); glVertex2i( g_dwWidth, 0 );
				glTexCoord2i( 1, 1 ); glVertex2i( g_dwWidth, g_dwHeight );
				glTexCoord2i( 0, 1 ); glVertex2i( 0, g_dwHeight );
			glEnd();
			
			SwapBuffers( g_hDC );
		}
	}
	else
	{
		hResult = lpDDS->lpVtbl->Flip( lpDDS, lpDDSurfaceTargetOverride, dwFlags );
	}
	return hResult;

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
	// inti
	GetConfig();

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
	if( Config.xfog != FALSE )
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
	}

	return This->dds1->lpVtbl->BltFast( This->dds1, dwX, dwY, GetInnerInterface( ((LPDIRECTDRAWSURFACE)src) ), lpSrcRect, dwTrans );
}

HRESULT Blt_pst( WRAP* This, LPRECT lpDestRect, LPDIRECTDRAWSURFACE lpDDSrcSurface, LPRECT lpSrcRect, DWORD dwFlags, LPDDBLTFX lpDDBltFx )
{
	// postpone drawing fog...
	if( Config.xfog != FALSE )
	{
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
	}

	//	
	// stretch movies //
	//
	if( Config.scale2x != FALSE )
	{
		if( g_dwWidth >= 1280 )
		{
			if((This->dds1 == dds_secondary) && (movie != NULL) && (movie2 != NULL) && ((movie->dds1 == lpDDSrcSurface) || (movie2->dds1 == lpDDSrcSurface)))
			{
				DDSURFACEDESC ddsd;
				BYTE* dst;
				DWORD dst_pitch;
				BYTE* src;
				DWORD src_pitch;
				DWORD width;
				DWORD height;

				ddsd.dwSize = sizeof(ddsd);
				if( SUCCEEDED( This->dds1->lpVtbl->Lock( This->dds1, NULL, &ddsd, DDLOCK_WAIT, NULL ) ) )
				{
					dst = (BYTE*)ddsd.lpSurface;
					dst_pitch = ddsd.lPitch;
					if( SUCCEEDED( lpDDSrcSurface->lpVtbl->Lock( lpDDSrcSurface, NULL, &ddsd, DDLOCK_WAIT, NULL ) ) )
					{
						src = (BYTE*)ddsd.lpSurface;
						src_pitch = ddsd.lPitch;
						width = lpSrcRect->right;
						height = lpSrcRect->bottom; 
						dst += ( g_dwWidth - 1280 ); // h-center
						if( g_dwHeight >= ( height << 1 ) )
						{ 
							dst += ( dst_pitch * (( g_dwHeight - ( height * 2 )) >> 1 )); // v-center 
						}
						else
						{ // clip src so it fits...
							src += ( src_pitch * ( ( ( height - ( g_dwHeight >> 1 ) ) >> 1 ) ) );
							height = g_dwHeight >> 1; 
						}

						// copy and enlarge
						scale2x( dst, dst_pitch, src, src_pitch, width, height );

						lpDDSrcSurface->lpVtbl->Unlock( lpDDSrcSurface, NULL );
					}
					This->dds1->lpVtbl->Unlock( This->dds1, NULL );
				}
				return DD_OK;
			}
		}
	}
	if( Config.dd_scale != FALSE )
	{
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
	}

	return This->dds1->lpVtbl->Blt( This->dds1, lpDestRect, lpDDSrcSurface, lpSrcRect, dwFlags, lpDDBltFx ); 
}
