#pragma comment(linker, "/ENTRY:\"DllEntryPoint\"")

#include "header.h"

#define D3DERR_COMMAND_UNPARSED 0x88760BB8

typedef HRESULT (__stdcall* DllGetClassObject_t       )( REFCLSID, REFIID, LPVOID* );
typedef HRESULT (__stdcall* DirectDrawCreate_t        )( GUID*, LPDIRECTDRAW*, IUnknown* );
typedef HRESULT (__stdcall* DirectDrawCreateEx_t      )( GUID*, LPVOID*, REFIID, IUnknown* );
typedef HRESULT (__stdcall* DirectDrawCreateClipper_t )( DWORD dwFlags, LPDIRECTDRAWCLIPPER FAR *lplpDDClipper,  IUnknown FAR *pUnkOuter );
typedef HRESULT (__stdcall* DirectDrawEnumerateA_t    )( LPDDENUMCALLBACK lpCallback, LPVOID lpContext );
typedef HRESULT (__stdcall* DirectDrawEnumerateExA_t  )( LPDDENUMCALLBACK lpCallback, LPVOID lpContext, DWORD dwFlags );
typedef HRESULT (__stdcall* DirectDrawEnumerateExW_t  )( LPDDENUMCALLBACK lpCallback, LPVOID lpContext, DWORD dwFlags );
typedef HRESULT (__stdcall* DirectDrawEnumerateW_t    )( LPDDENUMCALLBACK lpCallback, LPVOID lpContext );
typedef HRESULT (__stdcall* D3DParseUnknownCommand_t  )( LPVOID lpCmd, LPVOID *lpRetCmd );
typedef HRESULT (__stdcall* DllCanUnloadNow_t)();
//
// undocumented
typedef DWORD    (__stdcall* AcquireDDThreadLock_t         )();
typedef DWORD    (__stdcall* ReleaseDDThreadLock_t         )();
typedef DWORD   (__stdcall* GetOLEThunkData_t             )( DWORD index );
typedef DWORD   (__stdcall* SetAppCompatData_t            )( DWORD index, DWORD data );
typedef DWORD   (__stdcall* CompleteCreateSysmemSurface_t )( DWORD );
typedef HRESULT (__stdcall* DDGetAttachedSurfaceLcl_t     )( DWORD, DWORD, DWORD );
typedef DWORD   (__stdcall* DDInternalLock_t              )( DWORD, DWORD );
typedef DWORD   (__stdcall* DDInternalUnlock_t            )( DWORD );
typedef HRESULT (__stdcall* DSoundHelp_t                  )( DWORD, DWORD, DWORD );
typedef HRESULT (__stdcall* GetDDSurfaceLocal_t           )( DWORD, DWORD, DWORD );
typedef HRESULT (__stdcall* GetSurfaceFromDC_t            )( DWORD, DWORD, DWORD );
typedef HRESULT (__stdcall* RegisterSpecialCase_t         )( DWORD, DWORD, DWORD, DWORD );

HMODULE							hRealDDraw;
DirectDrawCreate_t				pDirectDrawCreate;
DirectDrawCreateEx_t			pDirectDrawCreateEx;
DirectDrawCreateClipper_t		pDirectDrawCreateClipper;
DirectDrawEnumerateA_t			pDirectDrawEnumerateA;
DirectDrawEnumerateExA_t		pDirectDrawEnumerateExA;
DirectDrawEnumerateExW_t		pDirectDrawEnumerateExW;
DirectDrawEnumerateW_t			pDirectDrawEnumerateW;
D3DParseUnknownCommand_t		pD3DParseUnknownCommand;
DllGetClassObject_t				pDllGetClassObject;
DllCanUnloadNow_t				pDllCanUnloadNow;
AcquireDDThreadLock_t			pAcquireDDThreadLock;
ReleaseDDThreadLock_t			pReleaseDDThreadLock;
GetOLEThunkData_t				pGetOLEThunkData;
SetAppCompatData_t				pSetAppCompatData;
CompleteCreateSysmemSurface_t	pCompleteCreateSysmemSurface;
DDGetAttachedSurfaceLcl_t		pDDGetAttachedSurfaceLcl;
DDInternalLock_t				pDDInternalLock;
DDInternalUnlock_t				pDDInternalUnlock;
DSoundHelp_t                    pDSoundHelp;
GetDDSurfaceLocal_t				pGetDDSurfaceLocal;
GetSurfaceFromDC_t				pGetSurfaceFromDC;
RegisterSpecialCase_t			pRegisterSpecialCase;

#pragma warning( disable: 4996 )  // strcat is unsafe
#pragma intrinsic( strcat )
BOOL __stdcall DllEntryPoint( HINSTANCE hDll, DWORD dwReason, LPVOID lpvReserved )
{
	PROLOGUE;
	UNREFERENCED_PARAMETER( lpvReserved );
	switch(dwReason)
	{
	case DLL_PROCESS_ATTACH: 
		{
			InitializeCriticalSection(&cs);
			GetSystemInfo(&sSysInfo);	
			// load ddraw.dll from system32 dir
			char szPath[ MAX_PATH ];
			if( GetSystemDirectory( szPath, MAX_PATH - 10 ))
			{
				strcat( szPath, "\\ddraw.dll" );
				TRACE( szPath );
				hRealDDraw = LoadLibrary( szPath );
				if( hRealDDraw == hDll ) // this dll is NOT the real dll...
				{
					FreeLibrary( hRealDDraw );
				}
				else
				{
					pDirectDrawCreate				= (DirectDrawCreate_t)				GetProcAddress( hRealDDraw, "DirectDrawCreate");
					pDirectDrawCreateEx				= (DirectDrawCreateEx_t)			GetProcAddress( hRealDDraw, "DirectDrawCreateEx");
					pDirectDrawCreateClipper		= (DirectDrawCreateClipper_t)		GetProcAddress( hRealDDraw, "DirectDrawCreateClipper");
					pDirectDrawEnumerateA			= (DirectDrawEnumerateA_t)			GetProcAddress( hRealDDraw, "DirectDrawEnumerateA");
					pDirectDrawEnumerateExA			= (DirectDrawEnumerateExA_t)		GetProcAddress( hRealDDraw, "DirectDrawEnumerateExA");
					pDirectDrawEnumerateExW			= (DirectDrawEnumerateExW_t)		GetProcAddress( hRealDDraw, "DirectDrawEnumerateExW");
					pDirectDrawEnumerateW			= (DirectDrawEnumerateW_t)			GetProcAddress( hRealDDraw, "DirectDrawEnumerateW");
					pD3DParseUnknownCommand			= (D3DParseUnknownCommand_t)		GetProcAddress( hRealDDraw, "D3DParseUnknownCommand");
					pDllGetClassObject				= (DllGetClassObject_t)				GetProcAddress( hRealDDraw, "DllGetClassObject");
					pDllCanUnloadNow				= (DllCanUnloadNow_t)				GetProcAddress( hRealDDraw, "DllCanUnloadNow");
					pAcquireDDThreadLock			= (AcquireDDThreadLock_t)			GetProcAddress( hRealDDraw, "AcquireDDThreadLock");
					pReleaseDDThreadLock			= (ReleaseDDThreadLock_t)			GetProcAddress( hRealDDraw, "ReleaseDDThreadLock");
					pGetOLEThunkData				= (GetOLEThunkData_t)				GetProcAddress( hRealDDraw, "GetOLEThunkData");
					pSetAppCompatData				= (SetAppCompatData_t)				GetProcAddress( hRealDDraw, "SetAppCompatData");
					pCompleteCreateSysmemSurface	= (CompleteCreateSysmemSurface_t)	GetProcAddress( hRealDDraw, "CompleteCreateSysmemSurface");
					pDDGetAttachedSurfaceLcl		= (DDGetAttachedSurfaceLcl_t)		GetProcAddress( hRealDDraw, "DDGetAttachedSurfaceLcl");
					pDDInternalLock					= (DDInternalLock_t)				GetProcAddress( hRealDDraw, "DDInternalLock");
					pDDInternalUnlock				= (DDInternalUnlock_t)				GetProcAddress( hRealDDraw, "DDInternalUnlock");
					pDSoundHelp						= (DSoundHelp_t)					GetProcAddress( hRealDDraw, "DSoundHelp" );
					pGetDDSurfaceLocal				= (GetDDSurfaceLocal_t)				GetProcAddress( hRealDDraw, "GetDDSurfaceLocal" );
					pGetSurfaceFromDC				= (GetSurfaceFromDC_t)				GetProcAddress( hRealDDraw, "GetSurfaceFromDC" );
					pRegisterSpecialCase			= (RegisterSpecialCase_t)			GetProcAddress( hRealDDraw, "RegisterSpecialCase" );
				}
			}
			break;
		}
	case DLL_THREAD_ATTACH:
	case DLL_THREAD_DETACH:
		break;
	case DLL_PROCESS_DETACH:
		TRACE( "DLL_PROCESS_DETACH" );
		FreeLibrary( hRealDDraw );
		break;
	}
	EPILOGUE( TRUE );
}
#pragma warning( default: 4996 )

typedef HRESULT (__stdcall* DirectDrawCreate_t)( GUID*, LPDIRECTDRAW*, IUnknown* );
HRESULT __stdcall DirectDrawCreate( GUID* lpGUID, LPDIRECTDRAW* lplpDD, IUnknown* pUnkOuter )
{
	PROLOGUE;
	HRESULT hResult = E_NOTIMPL;
	if( pDirectDrawCreate != NULL )
	{
		hResult = pDirectDrawCreate( lpGUID, lplpDD, pUnkOuter );
		if( SUCCEEDED( hResult) )
		{
			Wrap( NULL, iid_to_vtbl( IID_IDirectDraw ), (void**)lplpDD );
		}
	}
	EPILOGUE( hResult );
}


HRESULT __stdcall DirectDrawCreateEx( GUID* lpGUID, LPVOID* lplpDD, REFIID iid, IUnknown* pUnkOuter )
{	
	PROLOGUE;
	HRESULT hResult = E_NOTIMPL;
	if( pDirectDrawCreateEx != NULL )
	{
		hResult = pDirectDrawCreateEx( lpGUID, lplpDD, iid, pUnkOuter );
		if( SUCCEEDED( hResult) )
		{
			Wrap( NULL, iid_to_vtbl( iid ), (void**)lplpDD );
		}
	}
	EPILOGUE( hResult );
}


HRESULT __stdcall DirectDrawCreateClipper( DWORD dwFlags, LPDIRECTDRAWCLIPPER FAR *lplpDDClipper,  IUnknown FAR *pUnkOuter )
{	
	PROLOGUE;
	HRESULT hResult = E_NOTIMPL;
	if( pDirectDrawCreateClipper != NULL )
	{
		hResult = pDirectDrawCreateClipper( dwFlags, lplpDDClipper, pUnkOuter );
		if( SUCCEEDED( hResult) )
		{ 
			Wrap( NULL, iid_to_vtbl( IID_IDirectDrawClipper ), (void**)lplpDDClipper );
		}
	}
	EPILOGUE( hResult );
}


// do we care what the CLSID is ?? or do we just look at riid...
// CLSID_DirectDraw, CLSID_DirectDraw7, CLSID_DirectDrawClipper
HRESULT __stdcall DllGetClassObject( REFCLSID rclsid, REFIID riid, LPVOID* ppvObject )
{
	PROLOGUE;
	HRESULT hResult = E_NOTIMPL;
	if( pDllGetClassObject != NULL )
	{
		hResult = pDllGetClassObject( rclsid, riid, ppvObject );
		if( SUCCEEDED( hResult) )
		{ 
			Wrap( NULL, iid_to_vtbl( riid ), ppvObject );
		}
	}
	EPILOGUE( hResult );
}


////////////////////////////////////////////
//
// Every thing below here is just pass-thru
//
////////////////////////////////////////////

// returns void / no return value
DWORD __stdcall AcquireDDThreadLock()
{ 
	PROLOGUE;
	HRESULT hResult = E_NOTIMPL;
	if( pAcquireDDThreadLock != NULL )
	{
		pAcquireDDThreadLock();
		hResult = DD_OK;
	}
	EPILOGUE( hResult );
}

// returns void / no return value
DWORD __stdcall ReleaseDDThreadLock()
{	
	PROLOGUE;
	HRESULT hResult = E_NOTIMPL;
	if( pReleaseDDThreadLock != NULL )
	{
		pReleaseDDThreadLock();
		hResult = DD_OK;
	}
	EPILOGUE( hResult );
}


HRESULT __stdcall DirectDrawEnumerateA( LPDDENUMCALLBACK lpCallback, LPVOID lpContext )
{ 
	PROLOGUE;
	HRESULT hResult = E_NOTIMPL;
	if( pDirectDrawEnumerateA != NULL )
	{
		hResult = pDirectDrawEnumerateA( lpCallback, lpContext );
	}
	EPILOGUE( hResult );
}


HRESULT __stdcall DirectDrawEnumerateExA( LPDDENUMCALLBACK lpCallback, LPVOID lpContext, DWORD dwFlags )
{ 
	PROLOGUE;
	HRESULT hResult = E_NOTIMPL;
	if( pDirectDrawEnumerateExA != NULL ) 
	{
		hResult = pDirectDrawEnumerateExA( lpCallback, lpContext, dwFlags );
	}
	EPILOGUE( hResult );
}


HRESULT __stdcall DirectDrawEnumerateExW( LPDDENUMCALLBACK lpCallback, LPVOID lpContext, DWORD dwFlags )
{ 
	PROLOGUE;
	HRESULT hResult = E_NOTIMPL;
	if( pDirectDrawEnumerateExW != NULL ) 
	{
		hResult = pDirectDrawEnumerateExW( lpCallback, lpContext, dwFlags );
	}
	EPILOGUE( hResult );
}


HRESULT __stdcall DirectDrawEnumerateW( LPDDENUMCALLBACK lpCallback, LPVOID lpContext )
{ 
	PROLOGUE;
	HRESULT hResult = E_NOTIMPL;
	if( pDirectDrawEnumerateW != NULL )
	{
		hResult = pDirectDrawEnumerateW( lpCallback, lpContext );
	}
	EPILOGUE( hResult );
}


HRESULT __stdcall D3DParseUnknownCommand( LPVOID lpCmd, LPVOID *lpRetCmd )
{ 
	PROLOGUE;
	HRESULT hResult = D3DERR_COMMAND_UNPARSED;
	if( pD3DParseUnknownCommand != NULL )
	{
		hResult = pD3DParseUnknownCommand( lpCmd, lpRetCmd );
	}
	EPILOGUE( hResult );
}


HRESULT __stdcall DllCanUnloadNow()
{ 
	PROLOGUE;
	HRESULT hResult = S_FALSE;
	if( pDllCanUnloadNow != NULL )
	{
		hResult = pDllCanUnloadNow();
	}
	EPILOGUE( hResult );
}


DWORD __stdcall GetOLEThunkData( DWORD index )
{ 
	//	switch( index ) 
	//	{
	//		case 1: return _dwLastFrameRate;
	//		case 2: return _lpDriverObjectList;
	//		case 3: return _lpAttachedProcesses;
	//		case 4: return 0; // does nothing?
	//		case 5: return _CheckExclusiveMode;
	//		case 6: return 0; // ReleaseExclusiveModeMutex
	//	}
	PROLOGUE;
	DWORD dwResult = 0;
	if( pGetOLEThunkData != NULL )
	{
		dwResult = pGetOLEThunkData( index );
	}
	EPILOGUE( dwResult );
}


DWORD __stdcall SetAppCompatData( DWORD index, DWORD data )
{
	//	switch( index ) 
	//	{
	//		case 1: _g_bDWMOffForPrimaryLock	= 0;	return 0;
	//		case 2: _g_bDWMOffForPrimaryBlt		= 0;	return 0;
	//		case 3: _g_bForceFullscreenSprite	= 1;	return 0;
	//		case 4: _g_bForceBltToPrimary		= 1;	return 0;
	//		case 5: _g_crShadowBuf				= data;	return 0;
	//		case 6: _g_bDWMOffForFullscreen		= 0;	return 0;
	//	}
	PROLOGUE;
	DWORD dwResult = 0;
	if( pSetAppCompatData != NULL ) 
	{
		dwResult = pSetAppCompatData( index, data );
	}
	EPILOGUE( dwResult );
}


DWORD __stdcall CompleteCreateSysmemSurface( DWORD arg1 )
{ 
	PROLOGUE;
	DWORD dwResult = 0;
	if( pCompleteCreateSysmemSurface != NULL )
	{
		dwResult = pCompleteCreateSysmemSurface( arg1 );
	}
	EPILOGUE( dwResult );
}


HRESULT __stdcall DDGetAttachedSurfaceLcl( DWORD arg1, DWORD arg2, DWORD arg3 )
{ 
	PROLOGUE;
	HRESULT hResult = E_NOTIMPL;
	if( pDDGetAttachedSurfaceLcl != NULL )
	{
		hResult = pDDGetAttachedSurfaceLcl( arg1, arg2, arg3 );
	}
	EPILOGUE( hResult );
}


DWORD __stdcall DDInternalLock( DWORD arg1, DWORD arg2 )
{ 
	PROLOGUE;
	DWORD dwResult = 0xFFFFFFFF;
	if( pDDInternalLock != NULL )
	{
		dwResult = pDDInternalLock( arg1, arg2 );
	}
	EPILOGUE( dwResult );
}


DWORD __stdcall DDInternalUnlock( DWORD arg1 )
{
	PROLOGUE;
	DWORD dwResult = 0xFFFFFFFF;
	if( pDDInternalUnlock != NULL )
	{
		dwResult = pDDInternalUnlock( arg1 );
	}
	EPILOGUE( dwResult );
} 

/*
If SetCooperativeLevel is called once in a process, a binding is established between the process and the window. 
If it is called again in the same process with a different non-null window handle, it returns the DDERR_HWNDALREADYSET 
error value. Some applications may receive this error value when DirectSound(R) specifies a different window handle than
DirectDraw(R)—they should specify the same, top-level application window handle.
*/
HRESULT __stdcall DSoundHelp( DWORD arg1, DWORD arg2, DWORD arg3 )
{ 
	// _internalSetAppHWnd( 0, arg1, 0, 0, arg2, arg3 );
	PROLOGUE;
	HRESULT hResult = E_NOTIMPL;
	if( pDSoundHelp != NULL )
	{
		hResult = pDSoundHelp( arg1, arg2, arg3 );
	}
	EPILOGUE( hResult );
} 


// assume HRESULT...
HRESULT __stdcall GetDDSurfaceLocal(  DWORD arg1, DWORD arg2, DWORD arg3 )
{ 
	PROLOGUE;
	HRESULT hResult = E_NOTIMPL;
	if( pGetDDSurfaceLocal != NULL )
	{
		hResult = pGetDDSurfaceLocal( arg1, arg2, arg3 );
	}
	EPILOGUE( hResult );
}

// assume HRESULT...
HRESULT __stdcall GetSurfaceFromDC_export(  DWORD arg1, DWORD arg2, DWORD arg3 )
{ 
	PROLOGUE;
	HRESULT hResult = E_NOTIMPL;
	if( pGetSurfaceFromDC != NULL )
	{
		hResult = pGetSurfaceFromDC( arg1, arg2, arg3 );
	}
	EPILOGUE( hResult );
} 


// setup for HEL BLT ???
HRESULT __stdcall RegisterSpecialCase( DWORD arg1, DWORD arg2, DWORD arg3, DWORD arg4 )
{ 
	PROLOGUE;
	HRESULT hResult = E_NOTIMPL;
	if( pRegisterSpecialCase != NULL )
	{
		hResult = pRegisterSpecialCase( arg1, arg2, arg3, arg4 );
	}
	EPILOGUE( hResult );
}


