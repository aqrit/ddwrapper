#pragma comment( lib, "user32.lib" )

#include "header.h"


#define LOG Log


HANDLE file = INVALID_HANDLE_VALUE; 

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
	bmi.bmiHeader.biHeight = -(ddsd.dwHeight); // origin is the upper left corner
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

	if( lpDDSurfaceDesc->dwSize >= sizeof( DDSURFACEDESC2 ) ) LOG( "DDSD2:\r\n");
	else LOG( "DDSD:\r\n");

	DWORD dwKnownFlags = DDSD_CAPS | DDSD_HEIGHT | DDSD_WIDTH | DDSD_PITCH | DDSD_BACKBUFFERCOUNT | DDSD_ZBUFFERBITDEPTH |
		DDSD_ALPHABITDEPTH | DDSD_LPSURFACE | DDSD_PIXELFORMAT | DDSD_CKDESTOVERLAY | DDSD_CKDESTBLT | DDSD_CKSRCOVERLAY | DDSD_CKSRCBLT | 
		DDSD_MIPMAPCOUNT | DDSD_REFRESHRATE | DDSD_LINEARSIZE | DDSD_TEXTURESTAGE | DDSD_FVF | DDSD_SRCVBHANDLE | DDSD_DEPTH;
	LOG( "    Flags{ 0x%08X", lpDDSurfaceDesc->dwFlags );
	if( ( lpDDSurfaceDesc->dwFlags & DDSD_ALL ) == DDSD_ALL ) LOG( ", DDSD_ALL" ); 
	else{
		if( lpDDSurfaceDesc->dwFlags & DDSD_CAPS )				LOG( ", DDSD_CAPS" ); 
		if( lpDDSurfaceDesc->dwFlags & DDSD_HEIGHT )			LOG( ", DDSD_HEIGHT" ); 
		if( lpDDSurfaceDesc->dwFlags & DDSD_WIDTH )				LOG( ", DDSD_WIDTH" ); 
		if( lpDDSurfaceDesc->dwFlags & DDSD_PITCH )				LOG( ", DDSD_PITCH" ); 
		if( lpDDSurfaceDesc->dwFlags & DDSD_BACKBUFFERCOUNT )	LOG( ", DDSD_BACKBUFFERCOUNT" ); 
		if( lpDDSurfaceDesc->dwFlags & DDSD_ZBUFFERBITDEPTH )	LOG( ", DDSD_ZBUFFERBITDEPTH" ); 
		if( lpDDSurfaceDesc->dwFlags & DDSD_ALPHABITDEPTH )		LOG( ", DDSD_ALPHABITDEPTH" ); 
		if( lpDDSurfaceDesc->dwFlags & DDSD_LPSURFACE )			LOG( ", DDSD_LPSURFACE" ); 
		if( lpDDSurfaceDesc->dwFlags & DDSD_PIXELFORMAT )		LOG( ", DDSD_PIXELFORMAT" ); 
		if( lpDDSurfaceDesc->dwFlags & DDSD_CKDESTOVERLAY )		LOG( ", DDSD_CKDESTOVERLAY" ); 
		if( lpDDSurfaceDesc->dwFlags & DDSD_CKDESTBLT )			LOG( ", DDSD_CKDESTBLT" ); 
		if( lpDDSurfaceDesc->dwFlags & DDSD_CKSRCOVERLAY )		LOG( ", DDSD_CKSRCOVERLAY" ); 
		if( lpDDSurfaceDesc->dwFlags & DDSD_CKSRCBLT )			LOG( ", DDSD_CKSRCBLT" ); 
		if( lpDDSurfaceDesc->dwFlags & DDSD_MIPMAPCOUNT )		LOG( ", DDSD_MIPMAPCOUNT" ); 
		if( lpDDSurfaceDesc->dwFlags & DDSD_REFRESHRATE )		LOG( ", DDSD_REFRESHRATE" ); 
		if( lpDDSurfaceDesc->dwFlags & DDSD_LINEARSIZE )		LOG( ", DDSD_LINEARSIZE" ); 
		if( lpDDSurfaceDesc->dwFlags & DDSD_TEXTURESTAGE )		LOG( ", DDSD_TEXTURESTAGE" ); 
		if( lpDDSurfaceDesc->dwFlags & DDSD_FVF )				LOG( ", DDSD_FVF" ); 
		if( lpDDSurfaceDesc->dwFlags & DDSD_SRCVBHANDLE )		LOG( ", DDSD_SRCVBHANDLE" ); 
		if( lpDDSurfaceDesc->dwFlags & DDSD_DEPTH )				LOG( ", DDSD_DEPTH" ); 
		if( lpDDSurfaceDesc->dwFlags & ~( dwKnownFlags ) )		LOG( ", UNKNOWN!! 0x%08X", (lpDDSurfaceDesc->dwFlags & ~( dwKnownFlags )) );       
	}
	LOG( " }\r\n" );

	if( ( lpDDSurfaceDesc->dwFlags & DDSD_CAPS ) || ( lpDDSurfaceDesc->dwFlags == DDSD_ALL ) )
	{
		LOG( "    Caps1{ 0x%08X", lpDDSurfaceDesc->ddsCaps.dwCaps );
		if( lpDDSurfaceDesc->ddsCaps.dwCaps & DDSCAPS_RESERVED1 )		LOG( ", DDSCAPS_RESERVED1" ); 
		if( lpDDSurfaceDesc->ddsCaps.dwCaps & DDSCAPS_ALPHA )			LOG( ", DDSCAPS_ALPHA" ); 
		if( lpDDSurfaceDesc->ddsCaps.dwCaps & DDSCAPS_BACKBUFFER )		LOG( ", DDSCAPS_BACKBUFFER" ); 
		if( lpDDSurfaceDesc->ddsCaps.dwCaps & DDSCAPS_COMPLEX )			LOG( ", DDSCAPS_COMPLEX" ); 
		if( lpDDSurfaceDesc->ddsCaps.dwCaps & DDSCAPS_FLIP )			LOG( ", DDSCAPS_FLIP" ); 
		if( lpDDSurfaceDesc->ddsCaps.dwCaps & DDSCAPS_FRONTBUFFER )		LOG( ", DDSCAPS_FRONTBUFFER" ); 
		if( lpDDSurfaceDesc->ddsCaps.dwCaps & DDSCAPS_OFFSCREENPLAIN )	LOG( ", DDSCAPS_OFFSCREENPLAIN" ); 
		if( lpDDSurfaceDesc->ddsCaps.dwCaps & DDSCAPS_OVERLAY )			LOG( ", DDSCAPS_OVERLAY" ); 
		if( lpDDSurfaceDesc->ddsCaps.dwCaps & DDSCAPS_PALETTE )			LOG( ", DDSCAPS_PALETTE" ); 
		if( lpDDSurfaceDesc->ddsCaps.dwCaps & DDSCAPS_PRIMARYSURFACE )	LOG( ", DDSCAPS_PRIMARYSURFACE" ); 
		if( lpDDSurfaceDesc->ddsCaps.dwCaps & DDSCAPS_RESERVED3 )		LOG( ", DDSCAPS_RESERVED3" ); 
		if( lpDDSurfaceDesc->ddsCaps.dwCaps & DDSCAPS_SYSTEMMEMORY )	LOG( ", DDSCAPS_SYSTEMMEMORY" ); 
		if( lpDDSurfaceDesc->ddsCaps.dwCaps & DDSCAPS_TEXTURE )			LOG( ", DDSCAPS_TEXTURE" ); 
		if( lpDDSurfaceDesc->ddsCaps.dwCaps & DDSCAPS_3DDEVICE )		LOG( ", DDSCAPS_3DDEVICE" ); 
		if( lpDDSurfaceDesc->ddsCaps.dwCaps & DDSCAPS_VIDEOMEMORY )		LOG( ", DDSCAPS_VIDEOMEMORY" ); 
		if( lpDDSurfaceDesc->ddsCaps.dwCaps & DDSCAPS_VISIBLE )			LOG( ", DDSCAPS_VISIBLE" ); 
		if( lpDDSurfaceDesc->ddsCaps.dwCaps & DDSCAPS_WRITEONLY )		LOG( ", DDSCAPS_WRITEONLY" ); 
		if( lpDDSurfaceDesc->ddsCaps.dwCaps & DDSCAPS_ZBUFFER )			LOG( ", DDSCAPS_ZBUFFER" ); 
		if( lpDDSurfaceDesc->ddsCaps.dwCaps & DDSCAPS_OWNDC )			LOG( ", DDSCAPS_OWNDC" ); 
		if( lpDDSurfaceDesc->ddsCaps.dwCaps & DDSCAPS_LIVEVIDEO )		LOG( ", DDSCAPS_LIVEVIDEO" ); 
		if( lpDDSurfaceDesc->ddsCaps.dwCaps & DDSCAPS_HWCODEC )			LOG( ", DDSCAPS_HWCODEC" ); 
		if( lpDDSurfaceDesc->ddsCaps.dwCaps & DDSCAPS_MODEX )			LOG( ", DDSCAPS_MODEX" ); 
		if( lpDDSurfaceDesc->ddsCaps.dwCaps & DDSCAPS_MIPMAP )			LOG( ", DDSCAPS_MIPMAP" ); 
		if( lpDDSurfaceDesc->ddsCaps.dwCaps & DDSCAPS_RESERVED2 )		LOG( ", DDSCAPS_RESERVED2" ); 
		if( lpDDSurfaceDesc->ddsCaps.dwCaps & DDSCAPS_ALLOCONLOAD )		LOG( ", DDSCAPS_ALLOCONLOAD" ); 
		if( lpDDSurfaceDesc->ddsCaps.dwCaps & DDSCAPS_VIDEOPORT )		LOG( ", DDSCAPS_VIDEOPORT" ); 
		if( lpDDSurfaceDesc->ddsCaps.dwCaps & DDSCAPS_LOCALVIDMEM )		LOG( ", DDSCAPS_LOCALVIDMEM" ); 
		if( lpDDSurfaceDesc->ddsCaps.dwCaps & DDSCAPS_NONLOCALVIDMEM )	LOG( ", DDSCAPS_NONLOCALVIDMEM" ); 
		if( lpDDSurfaceDesc->ddsCaps.dwCaps & DDSCAPS_STANDARDVGAMODE )	LOG( ", DDSCAPS_STANDARDVGAMODE" ); 
		if( lpDDSurfaceDesc->ddsCaps.dwCaps & DDSCAPS_OPTIMIZED )		LOG( ", DDSCAPS_OPTIMIZED" ); 
		if( lpDDSurfaceDesc->ddsCaps.dwCaps & 0x03000000 )				LOG( ", UNKNOWN!! 0x%08X", ( lpDDSurfaceDesc->ddsCaps.dwCaps & 0x03000000 ) );
		LOG( " }\r\n" );
	}

	LOG( "    " );
	if( ( lpDDSurfaceDesc->dwFlags & DDSD_WIDTH ) || ( lpDDSurfaceDesc->dwFlags == DDSD_ALL ) ) 
		LOG( "width: %d ", lpDDSurfaceDesc->dwWidth );

	if( ( lpDDSurfaceDesc->dwFlags & DDSD_HEIGHT ) || ( lpDDSurfaceDesc->dwFlags == DDSD_ALL ) ) 
		LOG( "height: %d ", lpDDSurfaceDesc->dwHeight );

	if( ( lpDDSurfaceDesc->dwFlags & DDSD_BACKBUFFERCOUNT ) || ( lpDDSurfaceDesc->dwFlags == DDSD_ALL ) )
		LOG( "backbuffers: %d ", lpDDSurfaceDesc->dwBackBufferCount );
	LOG( "\r\n" );

	if( ( lpDDSurfaceDesc->dwFlags & DDSD_PIXELFORMAT ) || ( lpDDSurfaceDesc->dwFlags == DDSD_ALL ) )
	{
		LOG( "    DDPF:\r\n" );
		LOG( "        Flags{ 0x%08X", lpDDSurfaceDesc->ddpfPixelFormat.dwFlags ); 
		if( lpDDSurfaceDesc->ddpfPixelFormat.dwFlags & DDPF_ALPHAPIXELS )		LOG( ", DDPF_ALPHAPIXELS" ); 
		if( lpDDSurfaceDesc->ddpfPixelFormat.dwFlags & DDPF_ALPHA )				LOG( ", DDPF_ALPHA" ); 
		if( lpDDSurfaceDesc->ddpfPixelFormat.dwFlags & DDPF_FOURCC )			LOG( ", DDPF_FOURCC" ); 
		if( lpDDSurfaceDesc->ddpfPixelFormat.dwFlags & DDPF_PALETTEINDEXED4 )	LOG( ", DDPF_PALETTEINDEXED4" ); 
		if( lpDDSurfaceDesc->ddpfPixelFormat.dwFlags & DDPF_PALETTEINDEXEDTO8 )	LOG( ", DDPF_PALETTEINDEXEDTO8" ); 
		if( lpDDSurfaceDesc->ddpfPixelFormat.dwFlags & DDPF_PALETTEINDEXED8 )	LOG( ", DDPF_PALETTEINDEXED8" ); 
		if( lpDDSurfaceDesc->ddpfPixelFormat.dwFlags & DDPF_RGB )				LOG( ", DDPF_RGB" ); 
		if( lpDDSurfaceDesc->ddpfPixelFormat.dwFlags & DDPF_COMPRESSED )		LOG( ", DDPF_COMPRESSED" ); 
		if( lpDDSurfaceDesc->ddpfPixelFormat.dwFlags & DDPF_RGBTOYUV )			LOG( ", DDPF_RGBTOYUV" ); 
		if( lpDDSurfaceDesc->ddpfPixelFormat.dwFlags & DDPF_YUV )				LOG( ", DDPF_YUV" ); 
		if( lpDDSurfaceDesc->ddpfPixelFormat.dwFlags & DDPF_ZBUFFER )			LOG( ", DDPF_ZBUFFER" ); 
		if( lpDDSurfaceDesc->ddpfPixelFormat.dwFlags & DDPF_PALETTEINDEXED1 )	LOG( ", DDPF_PALETTEINDEXED1" ); 
		if( lpDDSurfaceDesc->ddpfPixelFormat.dwFlags & DDPF_PALETTEINDEXED2 )	LOG( ", DDPF_PALETTEINDEXED2" ); 
		if( lpDDSurfaceDesc->ddpfPixelFormat.dwFlags & DDPF_ZPIXELS )			LOG( ", DDPF_ZPIXELS" ); 
		if( lpDDSurfaceDesc->ddpfPixelFormat.dwFlags & DDPF_STENCILBUFFER )		LOG( ", DDPF_STENCILBUFFER" ); 
		if( lpDDSurfaceDesc->ddpfPixelFormat.dwFlags & DDPF_ALPHAPREMULT )		LOG( ", DDPF_ALPHAPREMULT" ); 
		if( lpDDSurfaceDesc->ddpfPixelFormat.dwFlags & DDPF_LUMINANCE )			LOG( ", DDPF_LUMINANCE" ); 
		if( lpDDSurfaceDesc->ddpfPixelFormat.dwFlags & DDPF_BUMPLUMINANCE )		LOG( ", DDPF_BUMPLUMINANCE" );
		if( lpDDSurfaceDesc->ddpfPixelFormat.dwFlags & DDPF_BUMPDUDV )			LOG( ", DDPF_BUMPDUDV" ); 
		if( lpDDSurfaceDesc->ddpfPixelFormat.dwFlags & 0xFFF10000 )				LOG( ", UNKNOWN!! 0x%08X", ( lpDDSurfaceDesc->ddpfPixelFormat.dwFlags & 0xFFF10000 ) );
		LOG( " }\r\n" );
	}
}
 

void LogGUID(const GUID& riid)
{
	if      ( riid == IID_IUnknown                   ) LOG( "IID_IUnknown\r\n"                   );
	else if ( riid == IID_IClassFactory              ) LOG( "IID_IClassFactory\r\n"              );
	else if ( riid == IID_IDirectDraw                ) LOG( "IID_IDirectDraw\r\n"                );
	else if	( riid == IID_IDirectDraw2               ) LOG( "IID_IDirectDraw2\r\n"               );
	else if	( riid == IID_IDirectDraw4               ) LOG( "IID_IDirectDraw4\r\n"               );
	else if	( riid == IID_IDirectDraw7               ) LOG( "IID_IDirectDraw7\r\n"               );
	else if ( riid == IID_IDirectDrawSurface         ) LOG( "IID_IDirectDrawSurface\r\n"         );
	else if ( riid == IID_IDirectDrawSurface2        ) LOG( "IID_IDirectDrawSurface2\r\n"        ); 
	else if ( riid == IID_IDirectDrawSurface3        ) LOG( "IID_IDirectDrawSurface3\r\n"        );
	else if ( riid == IID_IDirectDrawSurface4        ) LOG( "IID_IDirectDrawSurface4\r\n"        );
	else if ( riid == IID_IDirectDrawSurface7        ) LOG( "IID_IDirectDrawSurface7\r\n"        );
	else if ( riid == IID_IDirectDrawClipper         ) LOG( "IID_IDirectDrawClipper\r\n"         );
		// unsupported //
	else if ( riid == IID_IDDVideoPortContainer      ) LOG( "IID_IDDVideoPortContainer\r\n"      );
	else if ( riid == IID_IDirectDrawVideoPort       ) LOG( "IID_IDirectDrawVideoPort\r\n"       );
	else if ( riid == IID_IDirectDrawVideoPortNotify ) LOG( "IID_IDirectDrawVideoPortNotify\r\n" );
	else if ( riid == IID_IDirect3D                  ) LOG( "IID_IDirect3D\r\n" );
	else if ( riid == IID_IDirect3D2                 ) LOG( "IID_IDirect3D2\r\n" );
	else if ( riid == IID_IDirect3D3                 ) LOG( "IID_IDirect3D3\r\n" );
	else if ( riid == IID_IDirect3D7                 ) LOG( "IID_IDirect3D7\r\n" );
	else if ( riid == IID_IDirect3DTexture           ) LOG( "IID_IDirect3DTexture\r\n" );
	else if ( riid == IID_IDirect3DTexture2          ) LOG( "IID_IDirect3DTexture2\r\n" );
	else if ( riid == IID_IDirect3DRampDevice        ) LOG( "IID_IDirect3DRampDevice\r\n" );
	else if ( riid == IID_IDirect3DRGBDevice         ) LOG( "IID_IDirect3DRGBDevice\r\n" );
	else if ( riid == IID_IDirect3DHALDevice         ) LOG( "IID_IDirect3DHALDevice\r\n" );
	else if ( riid == IID_IDirect3DMMXDevice         ) LOG( "IID_IDirect3DMMXDevice\r\n" );
	else if ( riid == IID_IDirect3DRefDevice         ) LOG( "IID_IDirect3DRefDevice\r\n" );
	else if ( riid == IID_IDirect3DNullDevice        ) LOG( "IID_IDirect3DNullDevice\r\n" );
	else if ( riid == IID_IDirect3DTnLHalDevice      ) LOG( "IID_IDirect3DTnLHalDevice\r\n" );
	else if ( riid == IID_IDirect3DDevice            ) LOG( "IID_IDirect3DDevice\r\n" );
	else if ( riid == IID_IDirect3DDevice2           ) LOG( "IID_IDirect3DDevice2\r\n" );
	else if ( riid == IID_IDirect3DDevice3           ) LOG( "IID_IDirect3DDevice3\r\n" );
	else if ( riid == IID_IDirect3DDevice7           ) LOG( "IID_IDirect3DDevice7\r\n" );
	else if ( riid == IID_IDirect3DLight             ) LOG( "IID_IDirect3DLight\r\n" );
	else if ( riid == IID_IDirect3DMaterial          ) LOG( "IID_IDirect3DMaterial\r\n" );
	else if ( riid == IID_IDirect3DMaterial2         ) LOG( "IID_IDirect3DMaterial2\r\n" );
	else if ( riid == IID_IDirect3DMaterial3         ) LOG( "IID_IDirect3DMaterial3\r\n" );
	else if ( riid == IID_IDirect3DExecuteBuffer     ) LOG( "IID_IDirect3DExecuteBuffer\r\n" );
	else if ( riid == IID_IDirect3DViewport          ) LOG( "IID_IDirect3DViewport\r\n" );
	else if ( riid == IID_IDirect3DViewport2         ) LOG( "IID_IDirect3DViewport2\r\n" );
	else if ( riid == IID_IDirect3DViewport3         ) LOG( "IID_IDirect3DViewport3\r\n" );
	else if ( riid == IID_IDirect3DVertexBuffer      ) LOG( "IID_IDirect3DVertexBuffer\r\n" );
	else if ( riid == IID_IDirect3DVertexBuffer7     ) LOG( "IID_IDirect3DVertexBuffer7\r\n" );
	else 
	{
		LOG( ">>>> Unknown GUID: 0x%08X,0x%04X,0x%04X,0x%02X,0x%02X,0x%02X,0x%02X,0x%02X,0x%02X,0x%02X,0x%02X <<<<\r\n",
			riid.Data1, riid.Data2, riid.Data3, riid.Data4[0], riid.Data4[1], riid.Data4[2], riid.Data4[3], riid.Data4[4],
			riid.Data4[5], riid.Data4[6], riid.Data4[7]
		);
	}
}



// todo: make thread safe
void __cdecl Log( char* fmt, ...){
	static char buf[1024];
	static size_t pos = 0; 
	OVERLAPPED ol;
	int len;
	DWORD bytes_written;

	if( file == INVALID_HANDLE_VALUE )
	{
		file = CreateFile("ddwrapper.log", GENERIC_WRITE, 
			FILE_SHARE_READ | FILE_SHARE_WRITE, NULL, CREATE_ALWAYS, FILE_ATTRIBUTE_NORMAL, 0
		);
		if( file == INVALID_HANDLE_VALUE ) return;
	}

	va_list args;
    va_start(args,fmt);
    len = wvsprintf( buf, fmt, args );
	va_end(args);

	//todo: if( len < strlen( fmt ) ) return; 

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
}

void CloseLog()
{
	CloseHandle( file );
}

