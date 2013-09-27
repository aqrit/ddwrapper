#pragma once
#pragma warning(disable: 4996)  // strcat is unsafe


#define INITGUID
#define CINTERFACE 
#include <windows.h>
#include <Mmsystem.h>
#include <guiddef.h>
#include <ddraw.h>
#include <d3d.h>
#include <dvp.h>

#include <intrin.h>
void __cdecl Log( char* fmt, ...);
extern "C" void * _ReturnAddress(void);
#pragma intrinsic(_ReturnAddress)
void dds32_to_bmp( IDirectDrawSurface* pDDSurface, char* szFileName );
void LogDDSD( LPDDSURFACEDESC lpDDSurfaceDesc );

#define PROLOGUE
#define EPILOGUE(x) return(x)
#define WARN(x) 

// classfactory.cpp
namespace classfactory
{
	struct XVTBL; 
	extern const XVTBL xVtbl;
};

// dd.cpp
namespace dd
{
	struct XVTBL; 
	extern const XVTBL xVtbl1;
	extern const XVTBL xVtbl2;
	extern const XVTBL xVtbl4;
	extern const XVTBL xVtbl7;
};

// dds.cpp
namespace dds
{
	struct XVTBL; 
	extern const XVTBL xVtbl1;
	extern const XVTBL xVtbl2;
	extern const XVTBL xVtbl3;
	extern const XVTBL xVtbl4;
	extern const XVTBL xVtbl7;
};

// clipper.cpp
namespace clipper
{
	struct XVTBL;
	extern const XVTBL xVtbl;
};

// palette.cpp
namespace palette
{
	struct XVTBL;
	extern const XVTBL xVtbl;
};

// color.cpp
namespace color
{
	struct XVTBL;
	extern const XVTBL xVtbl;
};

// gamma.cpp
namespace gamma
{
	struct XVTBL;
	extern const XVTBL xVtbl;
};

// unkwn.cpp
namespace unknwn
{
	struct XVTBL;
	extern const XVTBL xVtbl;
};

#include "wrap.h"

GUID* DirectDrawCreateDriver_pst( GUID* lpGUID );
HRESULT SetCooperativeLevel_pst( LPDIRECTDRAW lpDD, HWND hWnd, DWORD dwFlags );
HRESULT SetDisplayMode_pst( LPDIRECTDRAW lpDD, DWORD dwWidth, DWORD dwHeight, DWORD dwBPP );
HRESULT GetDisplayMode_pst( LPDIRECTDRAW lpDD, LPDDSURFACEDESC lpDDSurfaceDesc );
HRESULT CreateSurface_pst( WRAP* This, LPDDSURFACEDESC lpDDSurfaceDesc, LPDIRECTDRAWSURFACE *lplpDDSurface, IUnknown *pUnkOuter );
HRESULT Flip_pst( LPDIRECTDRAWSURFACE lpDDS, LPDIRECTDRAWSURFACE lpDDSurfaceTargetOverride, DWORD dwFlags );
HRESULT BltFast_pst( WRAP* This, DWORD dwX, DWORD dwY, WRAP* src, LPRECT lpSrcRect, DWORD dwTrans );
HRESULT Blt_pst( WRAP* This, LPRECT lpDestRect, LPDIRECTDRAWSURFACE lpDDSrcSurface, LPRECT lpSrcRect, DWORD dwFlags, LPDDBLTFX lpDDBltFx );
ULONG Release_dds_pst( WRAP* This );
void DrawFog( LPDIRECTDRAWSURFACE lpDDS, DWORD flags );