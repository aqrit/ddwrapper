#include "header.h"

// defines from <dvp.h>
DEFINE_GUID( _IID_IDDVideoPortContainer, 0x6C142760,0xA733,0x11CE,0xA5,0x21,0x00,0x20,0xAF,0x0B,0xE5,0x60 );
DEFINE_GUID( _IID_IDirectDrawVideoPort, 0xB36D93E0,0x2B43,0x11CF,0xA2,0xDE,0x00,0xAA,0x00,0xB9,0x33,0x56 );
DEFINE_GUID( _IID_IDirectDrawVideoPortNotify, 0xA655FB94,0x0589,0x4E57,0xB3,0x33,0x56,0x7A,0x89,0x46,0x8C,0x88);
 

HANDLE file = INVALID_HANDLE_VALUE; 
CRITICAL_SECTION log_lock;

// todo: this function needs to be replaced
void __cdecl Log( char* fmt, ...)
{
	static char buf[1024];
	static size_t pos = 0; 
	OVERLAPPED ol;
	int len;
	DWORD bytes_written;

	if( file == INVALID_HANDLE_VALUE )
	{
		InitializeCriticalSection(&log_lock);
		EnterCriticalSection(&log_lock); 
		if( file == INVALID_HANDLE_VALUE )
		{
			file = CreateFile("ddwrapper.log", GENERIC_WRITE, 
				FILE_SHARE_READ | FILE_SHARE_WRITE, NULL, CREATE_ALWAYS, FILE_ATTRIBUTE_NORMAL, 0
			);
			LeaveCriticalSection(&log_lock);
			if( file == INVALID_HANDLE_VALUE ){ return; }
		}
	}

	EnterCriticalSection(&log_lock);

	va_list args;
    va_start(args,fmt);
    len = wvsprintf( buf, fmt, args );
	va_end(args);

	ol.Offset = pos;
	ol.OffsetHigh = 0; 
	ol.hEvent = NULL;
	
	if( WriteFile(file, buf, len, &bytes_written,  &ol) )
	{
		pos += bytes_written;
	} 
	else
	{
		// GetLastError();
	}
	LeaveCriticalSection(&log_lock);
}


void dds32_to_bmp( IDirectDrawSurface* pDDSurface, char* szFileName )
{
	BITMAPFILEHEADER bmfh;
	BITMAPINFO bmi;
	HANDLE hFile;
	OVERLAPPED ol;

	DDSURFACEDESC ddsd;
    ddsd.dwSize = sizeof( ddsd );
    if( FAILED( pDDSurface->lpVtbl->Lock( pDDSurface, NULL, &ddsd, DDLOCK_WAIT, NULL) ) ) return;

	bmfh.bfType =  0x4d42; // "BM"  
	bmfh.bfSize = sizeof(BITMAPFILEHEADER) + sizeof(BITMAPINFO) + ( ddsd.dwWidth * ddsd.dwHeight * 4 ); // 4 bytes per pixel
	bmfh.bfReserved1 = 0; 
	bmfh.bfReserved2 = 0; 
	bmfh.bfOffBits = sizeof(BITMAPFILEHEADER) + sizeof(BITMAPINFO);

	RtlSecureZeroMemory( &bmi, sizeof( bmi ) );

	bmi.bmiHeader.biSize = sizeof(BITMAPINFOHEADER);
	bmi.bmiHeader.biWidth = ddsd.dwWidth;
	bmi.bmiHeader.biHeight = -((signed long)ddsd.dwHeight); // origin is the upper left corner
	bmi.bmiHeader.biPlanes = 1;
	bmi.bmiHeader.biBitCount = 32;
	bmi.bmiHeader.biCompression = BI_RGB;
	
	hFile = CreateFile( szFileName, GENERIC_WRITE, 0, NULL, CREATE_NEW, FILE_ATTRIBUTE_NORMAL, 0);
	if( hFile != NULL )
	{
		ol.Offset = 0;
		ol.OffsetHigh = 0; 
		ol.hEvent = NULL;
		WriteFile( hFile, &bmfh, sizeof( bmfh ), NULL,  &ol);
		ol.Offset = sizeof( bmfh );
		WriteFile( hFile, &bmi, sizeof( bmi ), NULL,  &ol);
		ol.Offset += sizeof( bmi );
		for( DWORD y = 0; y < ddsd.dwHeight; y++ )
		{
			WriteFile( hFile, (void*)(((uintptr_t)ddsd.lpSurface) + ( ddsd.lPitch * y )), ddsd.dwWidth * 4, NULL, &ol );
			ol.Offset += ( ddsd.dwWidth * 4 ); // 4 bytes per pixel
		}
		CloseHandle(hFile);
	}
	
	pDDSurface->lpVtbl->Unlock( pDDSurface, NULL );
}


void LogDDSD( LPDDSURFACEDESC lpDDSurfaceDesc )
{
	if( lpDDSurfaceDesc->dwSize >= sizeof( DDSURFACEDESC2 ) ) Log( "DDSD2:\r\n");
	else Log( "DDSD:\r\n");

	DWORD dwKnownFlags = DDSD_CAPS | DDSD_HEIGHT | DDSD_WIDTH | DDSD_PITCH | DDSD_BACKBUFFERCOUNT | DDSD_ZBUFFERBITDEPTH |
		DDSD_ALPHABITDEPTH | DDSD_LPSURFACE | DDSD_PIXELFORMAT | DDSD_CKDESTOVERLAY | DDSD_CKDESTBLT | DDSD_CKSRCOVERLAY | DDSD_CKSRCBLT | 
		DDSD_MIPMAPCOUNT | DDSD_REFRESHRATE | DDSD_LINEARSIZE | DDSD_TEXTURESTAGE | DDSD_FVF | DDSD_SRCVBHANDLE | DDSD_DEPTH;
	Log( "    Flags{ 0x%08X", lpDDSurfaceDesc->dwFlags );
	if( ( lpDDSurfaceDesc->dwFlags & DDSD_ALL ) == DDSD_ALL ) Log( ", DDSD_ALL" ); 
	else{
		if( lpDDSurfaceDesc->dwFlags & DDSD_CAPS )				Log( ", DDSD_CAPS" ); 
		if( lpDDSurfaceDesc->dwFlags & DDSD_HEIGHT )			Log( ", DDSD_HEIGHT" ); 
		if( lpDDSurfaceDesc->dwFlags & DDSD_WIDTH )				Log( ", DDSD_WIDTH" ); 
		if( lpDDSurfaceDesc->dwFlags & DDSD_PITCH )				Log( ", DDSD_PITCH" ); 
		if( lpDDSurfaceDesc->dwFlags & DDSD_BACKBUFFERCOUNT )	Log( ", DDSD_BACKBUFFERCOUNT" ); 
		if( lpDDSurfaceDesc->dwFlags & DDSD_ZBUFFERBITDEPTH )	Log( ", DDSD_ZBUFFERBITDEPTH" ); 
		if( lpDDSurfaceDesc->dwFlags & DDSD_ALPHABITDEPTH )		Log( ", DDSD_ALPHABITDEPTH" ); 
		if( lpDDSurfaceDesc->dwFlags & DDSD_LPSURFACE )			Log( ", DDSD_LPSURFACE" ); 
		if( lpDDSurfaceDesc->dwFlags & DDSD_PIXELFORMAT )		Log( ", DDSD_PIXELFORMAT" ); 
		if( lpDDSurfaceDesc->dwFlags & DDSD_CKDESTOVERLAY )		Log( ", DDSD_CKDESTOVERLAY" ); 
		if( lpDDSurfaceDesc->dwFlags & DDSD_CKDESTBLT )			Log( ", DDSD_CKDESTBLT" ); 
		if( lpDDSurfaceDesc->dwFlags & DDSD_CKSRCOVERLAY )		Log( ", DDSD_CKSRCOVERLAY" ); 
		if( lpDDSurfaceDesc->dwFlags & DDSD_CKSRCBLT )			Log( ", DDSD_CKSRCBLT" ); 
		if( lpDDSurfaceDesc->dwFlags & DDSD_MIPMAPCOUNT )		Log( ", DDSD_MIPMAPCOUNT" ); 
		if( lpDDSurfaceDesc->dwFlags & DDSD_REFRESHRATE )		Log( ", DDSD_REFRESHRATE" ); 
		if( lpDDSurfaceDesc->dwFlags & DDSD_LINEARSIZE )		Log( ", DDSD_LINEARSIZE" ); 
		if( lpDDSurfaceDesc->dwFlags & DDSD_TEXTURESTAGE )		Log( ", DDSD_TEXTURESTAGE" ); 
		if( lpDDSurfaceDesc->dwFlags & DDSD_FVF )				Log( ", DDSD_FVF" ); 
		if( lpDDSurfaceDesc->dwFlags & DDSD_SRCVBHANDLE )		Log( ", DDSD_SRCVBHANDLE" ); 
		if( lpDDSurfaceDesc->dwFlags & DDSD_DEPTH )				Log( ", DDSD_DEPTH" ); 
		if( lpDDSurfaceDesc->dwFlags & ~( dwKnownFlags ) )		Log( ", UNKNOWN!! 0x%08X", (lpDDSurfaceDesc->dwFlags & ~( dwKnownFlags )) );       
	}
	Log( " }\r\n" );

	if( ( lpDDSurfaceDesc->dwFlags & DDSD_CAPS ) || ( lpDDSurfaceDesc->dwFlags == DDSD_ALL ) )
	{
		Log( "    Caps1{ 0x%08X", lpDDSurfaceDesc->ddsCaps.dwCaps );
		if( lpDDSurfaceDesc->ddsCaps.dwCaps & DDSCAPS_RESERVED1 )		Log( ", DDSCAPS_RESERVED1" ); 
		if( lpDDSurfaceDesc->ddsCaps.dwCaps & DDSCAPS_ALPHA )			Log( ", DDSCAPS_ALPHA" ); 
		if( lpDDSurfaceDesc->ddsCaps.dwCaps & DDSCAPS_BACKBUFFER )		Log( ", DDSCAPS_BACKBUFFER" ); 
		if( lpDDSurfaceDesc->ddsCaps.dwCaps & DDSCAPS_COMPLEX )			Log( ", DDSCAPS_COMPLEX" ); 
		if( lpDDSurfaceDesc->ddsCaps.dwCaps & DDSCAPS_FLIP )			Log( ", DDSCAPS_FLIP" ); 
		if( lpDDSurfaceDesc->ddsCaps.dwCaps & DDSCAPS_FRONTBUFFER )		Log( ", DDSCAPS_FRONTBUFFER" ); 
		if( lpDDSurfaceDesc->ddsCaps.dwCaps & DDSCAPS_OFFSCREENPLAIN )	Log( ", DDSCAPS_OFFSCREENPLAIN" ); 
		if( lpDDSurfaceDesc->ddsCaps.dwCaps & DDSCAPS_OVERLAY )			Log( ", DDSCAPS_OVERLAY" ); 
		if( lpDDSurfaceDesc->ddsCaps.dwCaps & DDSCAPS_PALETTE )			Log( ", DDSCAPS_PALETTE" ); 
		if( lpDDSurfaceDesc->ddsCaps.dwCaps & DDSCAPS_PRIMARYSURFACE )	Log( ", DDSCAPS_PRIMARYSURFACE" ); 
		if( lpDDSurfaceDesc->ddsCaps.dwCaps & DDSCAPS_RESERVED3 )		Log( ", DDSCAPS_RESERVED3" ); 
		if( lpDDSurfaceDesc->ddsCaps.dwCaps & DDSCAPS_SYSTEMMEMORY )	Log( ", DDSCAPS_SYSTEMMEMORY" ); 
		if( lpDDSurfaceDesc->ddsCaps.dwCaps & DDSCAPS_TEXTURE )			Log( ", DDSCAPS_TEXTURE" ); 
		if( lpDDSurfaceDesc->ddsCaps.dwCaps & DDSCAPS_3DDEVICE )		Log( ", DDSCAPS_3DDEVICE" ); 
		if( lpDDSurfaceDesc->ddsCaps.dwCaps & DDSCAPS_VIDEOMEMORY )		Log( ", DDSCAPS_VIDEOMEMORY" ); 
		if( lpDDSurfaceDesc->ddsCaps.dwCaps & DDSCAPS_VISIBLE )			Log( ", DDSCAPS_VISIBLE" ); 
		if( lpDDSurfaceDesc->ddsCaps.dwCaps & DDSCAPS_WRITEONLY )		Log( ", DDSCAPS_WRITEONLY" ); 
		if( lpDDSurfaceDesc->ddsCaps.dwCaps & DDSCAPS_ZBUFFER )			Log( ", DDSCAPS_ZBUFFER" ); 
		if( lpDDSurfaceDesc->ddsCaps.dwCaps & DDSCAPS_OWNDC )			Log( ", DDSCAPS_OWNDC" ); 
		if( lpDDSurfaceDesc->ddsCaps.dwCaps & DDSCAPS_LIVEVIDEO )		Log( ", DDSCAPS_LIVEVIDEO" ); 
		if( lpDDSurfaceDesc->ddsCaps.dwCaps & DDSCAPS_HWCODEC )			Log( ", DDSCAPS_HWCODEC" ); 
		if( lpDDSurfaceDesc->ddsCaps.dwCaps & DDSCAPS_MODEX )			Log( ", DDSCAPS_MODEX" ); 
		if( lpDDSurfaceDesc->ddsCaps.dwCaps & DDSCAPS_MIPMAP )			Log( ", DDSCAPS_MIPMAP" ); 
		if( lpDDSurfaceDesc->ddsCaps.dwCaps & DDSCAPS_RESERVED2 )		Log( ", DDSCAPS_RESERVED2" ); 
		if( lpDDSurfaceDesc->ddsCaps.dwCaps & DDSCAPS_ALLOCONLOAD )		Log( ", DDSCAPS_ALLOCONLOAD" ); 
		if( lpDDSurfaceDesc->ddsCaps.dwCaps & DDSCAPS_VIDEOPORT )		Log( ", DDSCAPS_VIDEOPORT" ); 
		if( lpDDSurfaceDesc->ddsCaps.dwCaps & DDSCAPS_LOCALVIDMEM )		Log( ", DDSCAPS_LOCALVIDMEM" ); 
		if( lpDDSurfaceDesc->ddsCaps.dwCaps & DDSCAPS_NONLOCALVIDMEM )	Log( ", DDSCAPS_NONLOCALVIDMEM" ); 
		if( lpDDSurfaceDesc->ddsCaps.dwCaps & DDSCAPS_STANDARDVGAMODE )	Log( ", DDSCAPS_STANDARDVGAMODE" ); 
		if( lpDDSurfaceDesc->ddsCaps.dwCaps & DDSCAPS_OPTIMIZED )		Log( ", DDSCAPS_OPTIMIZED" ); 
		if( lpDDSurfaceDesc->ddsCaps.dwCaps & 0x03000000 )				Log( ", UNKNOWN!! 0x%08X", ( lpDDSurfaceDesc->ddsCaps.dwCaps & 0x03000000 ) );
		Log( " }\r\n" );
	}

	Log( "    " );
	if( ( lpDDSurfaceDesc->dwFlags & DDSD_WIDTH ) || ( lpDDSurfaceDesc->dwFlags == DDSD_ALL ) ) 
		Log( "width: %d ", lpDDSurfaceDesc->dwWidth );

	if( ( lpDDSurfaceDesc->dwFlags & DDSD_HEIGHT ) || ( lpDDSurfaceDesc->dwFlags == DDSD_ALL ) ) 
		Log( "height: %d ", lpDDSurfaceDesc->dwHeight );

	if( ( lpDDSurfaceDesc->dwFlags & DDSD_BACKBUFFERCOUNT ) || ( lpDDSurfaceDesc->dwFlags == DDSD_ALL ) )
		Log( "backbuffers: %d ", lpDDSurfaceDesc->dwBackBufferCount );
	Log( "\r\n" );

	if( ( lpDDSurfaceDesc->dwFlags & DDSD_PIXELFORMAT ) || ( lpDDSurfaceDesc->dwFlags == DDSD_ALL ) )
	{
		Log( "    DDPF:\r\n" );
		Log( "        Flags{ 0x%08X", lpDDSurfaceDesc->ddpfPixelFormat.dwFlags ); 
		if( lpDDSurfaceDesc->ddpfPixelFormat.dwFlags & DDPF_ALPHAPIXELS )		Log( ", DDPF_ALPHAPIXELS" ); 
		if( lpDDSurfaceDesc->ddpfPixelFormat.dwFlags & DDPF_ALPHA )				Log( ", DDPF_ALPHA" ); 
		if( lpDDSurfaceDesc->ddpfPixelFormat.dwFlags & DDPF_FOURCC )			Log( ", DDPF_FOURCC" ); 
		if( lpDDSurfaceDesc->ddpfPixelFormat.dwFlags & DDPF_PALETTEINDEXED4 )	Log( ", DDPF_PALETTEINDEXED4" ); 
		if( lpDDSurfaceDesc->ddpfPixelFormat.dwFlags & DDPF_PALETTEINDEXEDTO8 )	Log( ", DDPF_PALETTEINDEXEDTO8" ); 
		if( lpDDSurfaceDesc->ddpfPixelFormat.dwFlags & DDPF_PALETTEINDEXED8 )	Log( ", DDPF_PALETTEINDEXED8" ); 
		if( lpDDSurfaceDesc->ddpfPixelFormat.dwFlags & DDPF_RGB )				Log( ", DDPF_RGB" ); 
		if( lpDDSurfaceDesc->ddpfPixelFormat.dwFlags & DDPF_COMPRESSED )		Log( ", DDPF_COMPRESSED" ); 
		if( lpDDSurfaceDesc->ddpfPixelFormat.dwFlags & DDPF_RGBTOYUV )			Log( ", DDPF_RGBTOYUV" ); 
		if( lpDDSurfaceDesc->ddpfPixelFormat.dwFlags & DDPF_YUV )				Log( ", DDPF_YUV" ); 
		if( lpDDSurfaceDesc->ddpfPixelFormat.dwFlags & DDPF_ZBUFFER )			Log( ", DDPF_ZBUFFER" ); 
		if( lpDDSurfaceDesc->ddpfPixelFormat.dwFlags & DDPF_PALETTEINDEXED1 )	Log( ", DDPF_PALETTEINDEXED1" ); 
		if( lpDDSurfaceDesc->ddpfPixelFormat.dwFlags & DDPF_PALETTEINDEXED2 )	Log( ", DDPF_PALETTEINDEXED2" ); 
		if( lpDDSurfaceDesc->ddpfPixelFormat.dwFlags & DDPF_ZPIXELS )			Log( ", DDPF_ZPIXELS" ); 
		if( lpDDSurfaceDesc->ddpfPixelFormat.dwFlags & DDPF_STENCILBUFFER )		Log( ", DDPF_STENCILBUFFER" ); 
		if( lpDDSurfaceDesc->ddpfPixelFormat.dwFlags & DDPF_ALPHAPREMULT )		Log( ", DDPF_ALPHAPREMULT" ); 
		if( lpDDSurfaceDesc->ddpfPixelFormat.dwFlags & DDPF_LUMINANCE )			Log( ", DDPF_LUMINANCE" ); 
		if( lpDDSurfaceDesc->ddpfPixelFormat.dwFlags & DDPF_BUMPLUMINANCE )		Log( ", DDPF_BUMPLUMINANCE" );
		if( lpDDSurfaceDesc->ddpfPixelFormat.dwFlags & DDPF_BUMPDUDV )			Log( ", DDPF_BUMPDUDV" ); 
		if( lpDDSurfaceDesc->ddpfPixelFormat.dwFlags & 0xFFF10000 )				Log( ", UNKNOWN!! 0x%08X", ( lpDDSurfaceDesc->ddpfPixelFormat.dwFlags & 0xFFF10000 ) );
		Log( " }\r\n" );
	}
}
 

void LogGUID(const GUID& riid)
{
	if      ( riid == IID_IUnknown                   ) Log( "IID_IUnknown\r\n"                   );
	else if ( riid == IID_IClassFactory              ) Log( "IID_IClassFactory\r\n"              );
	else if ( riid == IID_IDirectDraw                ) Log( "IID_IDirectDraw\r\n"                );
	else if	( riid == IID_IDirectDraw2               ) Log( "IID_IDirectDraw2\r\n"               );
	else if	( riid == IID_IDirectDraw4               ) Log( "IID_IDirectDraw4\r\n"               );
	else if	( riid == IID_IDirectDraw7               ) Log( "IID_IDirectDraw7\r\n"               );
	else if ( riid == IID_IDirectDrawSurface         ) Log( "IID_IDirectDrawSurface\r\n"         );
	else if ( riid == IID_IDirectDrawSurface2        ) Log( "IID_IDirectDrawSurface2\r\n"        ); 
	else if ( riid == IID_IDirectDrawSurface3        ) Log( "IID_IDirectDrawSurface3\r\n"        );
	else if ( riid == IID_IDirectDrawSurface4        ) Log( "IID_IDirectDrawSurface4\r\n"        );
	else if ( riid == IID_IDirectDrawSurface7        ) Log( "IID_IDirectDrawSurface7\r\n"        );
	else if ( riid == IID_IDirectDrawClipper         ) Log( "IID_IDirectDrawClipper\r\n"         );
	else if ( riid == IID_IDirectDrawPalette         ) Log( "IID_IDirectDrawPalette\r\n"         );
	
	// unsupported //
	else if ( riid == _IID_IDDVideoPortContainer      ) Log( "IID_IDDVideoPortContainer\r\n"      );
	else if ( riid == _IID_IDirectDrawVideoPort       ) Log( "IID_IDirectDrawVideoPort\r\n"       );
	else if ( riid == _IID_IDirectDrawVideoPortNotify ) Log( "IID_IDirectDrawVideoPortNotify\r\n" );
	else if ( riid == IID_IDirect3D                  ) Log( "IID_IDirect3D\r\n" );
	else if ( riid == IID_IDirect3D2                 ) Log( "IID_IDirect3D2\r\n" );
	else if ( riid == IID_IDirect3D3                 ) Log( "IID_IDirect3D3\r\n" );
	else if ( riid == IID_IDirect3D7                 ) Log( "IID_IDirect3D7\r\n" );
	else if ( riid == IID_IDirect3DTexture           ) Log( "IID_IDirect3DTexture\r\n" );
	else if ( riid == IID_IDirect3DTexture2          ) Log( "IID_IDirect3DTexture2\r\n" );
	else if ( riid == IID_IDirect3DRampDevice        ) Log( "IID_IDirect3DRampDevice\r\n" );
	else if ( riid == IID_IDirect3DRGBDevice         ) Log( "IID_IDirect3DRGBDevice\r\n" );
	else if ( riid == IID_IDirect3DHALDevice         ) Log( "IID_IDirect3DHALDevice\r\n" );
	else if ( riid == IID_IDirect3DMMXDevice         ) Log( "IID_IDirect3DMMXDevice\r\n" );
	else if ( riid == IID_IDirect3DRefDevice         ) Log( "IID_IDirect3DRefDevice\r\n" );
	else if ( riid == IID_IDirect3DNullDevice        ) Log( "IID_IDirect3DNullDevice\r\n" );
	else if ( riid == IID_IDirect3DTnLHalDevice      ) Log( "IID_IDirect3DTnLHalDevice\r\n" );
	else if ( riid == IID_IDirect3DDevice            ) Log( "IID_IDirect3DDevice\r\n" );
	else if ( riid == IID_IDirect3DDevice2           ) Log( "IID_IDirect3DDevice2\r\n" );
	else if ( riid == IID_IDirect3DDevice3           ) Log( "IID_IDirect3DDevice3\r\n" );
	else if ( riid == IID_IDirect3DDevice7           ) Log( "IID_IDirect3DDevice7\r\n" );
	else if ( riid == IID_IDirect3DLight             ) Log( "IID_IDirect3DLight\r\n" );
	else if ( riid == IID_IDirect3DMaterial          ) Log( "IID_IDirect3DMaterial\r\n" );
	else if ( riid == IID_IDirect3DMaterial2         ) Log( "IID_IDirect3DMaterial2\r\n" );
	else if ( riid == IID_IDirect3DMaterial3         ) Log( "IID_IDirect3DMaterial3\r\n" );
	else if ( riid == IID_IDirect3DExecuteBuffer     ) Log( "IID_IDirect3DExecuteBuffer\r\n" );
	else if ( riid == IID_IDirect3DViewport          ) Log( "IID_IDirect3DViewport\r\n" );
	else if ( riid == IID_IDirect3DViewport2         ) Log( "IID_IDirect3DViewport2\r\n" );
	else if ( riid == IID_IDirect3DViewport3         ) Log( "IID_IDirect3DViewport3\r\n" );
	else if ( riid == IID_IDirect3DVertexBuffer      ) Log( "IID_IDirect3DVertexBuffer\r\n" );
	else if ( riid == IID_IDirect3DVertexBuffer7     ) Log( "IID_IDirect3DVertexBuffer7\r\n" );
	else 
	{
		Log( ">>>> Unknown GUID: 0x%08X,0x%04X,0x%04X,0x%02X,0x%02X,0x%02X,0x%02X,0x%02X,0x%02X,0x%02X,0x%02X <<<<\r\n",
			riid.Data1, riid.Data2, riid.Data3, riid.Data4[0], riid.Data4[1], riid.Data4[2], riid.Data4[3], riid.Data4[4],
			riid.Data4[5], riid.Data4[6], riid.Data4[7]
		);
	}
}


