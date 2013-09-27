#include "header.h"

// darkens two 565 colors by 25%
DWORD inline darken( DWORD x )
{
	DWORD a = (x >> 1) & 0x7BEF7BEF;
	DWORD b = (a >> 1) & 0x7BEF7BEF;
	return a + b;
}

void DrawFog( LPDIRECTDRAWSURFACE lpDDS, DWORD flags )
{
	DDSURFACEDESC ddsd;

	ddsd.dwSize = sizeof(ddsd);
	if( SUCCEEDED( lpDDS->lpVtbl->Lock( lpDDS, NULL, &ddsd, DDLOCK_WAIT, NULL ) ) )
	{
		for( int row = 0; row < 2; row++ )
		{
			for( int col = 0; col < 2; col++ )
			{
				DWORD* p = (DWORD*)( ( (uintptr_t)ddsd.lpSurface ) + ( row * ddsd.lPitch * 32 ) + ( col * 64 ) );
				switch( flags & 0x000000FF )
				{
					case 0: // no fog
					{	
						break;
					}
					case 0x0000001E: // top left triangle
					{
						for( int y = 32; y != 0; y-- )
						{
							for( int x = 0; x < ( y >> 1 ); x++ )
							{  
								p[x] = darken( p[x] );
							}
							p = (DWORD*)(((uintptr_t)p) + ddsd.lPitch);	
						}
						break;
					}
					case 0x00000078: // bottom left triangle
					{
						for( int y = 0; y < 32; y++ )
						{
							for( int x = 0; x <= ( y >> 1 ); x++ )
							{
								p[x] = darken( p[x] );
							}
							p = (DWORD*)(((uintptr_t)p) + ddsd.lPitch);
						}
						break;
					}
					case 0x00000087: // top right triangle
					{
						for( int y = 0; y < 32; y++ )
						{
							for( int x = (y >> 1); x < 16; x++ )
							{  
								p[x] = darken( p[x] );
							}
							p = (DWORD*)(((uintptr_t)p) + ddsd.lPitch);	
						}
						break;
					}
					case 0x000000E1: // bottom right triangle
					{
						for( int y = 0; y < 32; y++ )
						{
							for( int x = ( ( 32 - y ) >> 1 ); x < 16; x++ )
							{
								p[x] = darken( p[x] );
							}
							p = (DWORD*)(((uintptr_t)p) + ddsd.lPitch);
						}
						break;
					}

					default:
//					case 0x0000009F: // overlapped top triangles
//					case 0x0000007E: // overlapped left triangles
//					case 0x000000E7: // overlapped right triangles
//					case 0x000000F9: // overlapped bottom triangles
//					case 0x000000FF: // entire square
					{
						for( int y = 0; y < 32; y++ )
						{
							for( int x = 0; x < 16; x++ )
							{
								p[x] = darken( p[x] );
							}
							p = (DWORD*)(((uintptr_t)p) + ddsd.lPitch);
						}
						break;
					}
				} // switch
				flags >>= 8;
			} // col
		} // row
		lpDDS->lpVtbl->Unlock( lpDDS, NULL );
	} // if locked
}
