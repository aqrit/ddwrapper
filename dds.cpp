#include "header.h"

namespace dds
{
	struct XVTBL
	{
		HRESULT (__stdcall * QueryInterface)( WRAP* This, const IID& riid, void** ppvObject ); 
		ULONG   (__stdcall * AddRef)( WRAP* This ); 
		ULONG   (__stdcall * Release)( WRAP* This );
		HRESULT (__stdcall * AddAttachedSurface)( WRAP* This, LPDIRECTDRAWSURFACE lpDDSAttachedSurface );
		HRESULT (__stdcall * AddOverlayDirtyRect)( WRAP* This, LPRECT lpRect );
		HRESULT (__stdcall * Blt)( WRAP* This, LPRECT lpDestRect, LPDIRECTDRAWSURFACE lpDDSrcSurface, LPRECT lpSrcRect, DWORD dwFlags, LPDDBLTFX lpDDBltFx );
		HRESULT (__stdcall * BltBatch)( WRAP* This, LPDDBLTBATCH lpDDBltBatch, DWORD dwCount, DWORD dwFlags );
		HRESULT (__stdcall * BltFast)( WRAP* This, DWORD dwX, DWORD dwY, LPDIRECTDRAWSURFACE lpDDSrcSurface, LPRECT lpSrcRect, DWORD dwTrans );
		HRESULT (__stdcall * DeleteAttachedSurface)( WRAP* This, DWORD dwFlags, LPDIRECTDRAWSURFACE lpDDSAttachedSurface );
		HRESULT (__stdcall * EnumAttachedSurfaces)( WRAP* This, LPVOID lpContext, LPDDENUMSURFACESCALLBACK lpEnumSurfacesCallback );
		HRESULT (__stdcall * EnumOverlayZOrders)( WRAP* This, DWORD dwFlags, LPVOID lpContext, LPDDENUMSURFACESCALLBACK lpfnCallback );
		HRESULT (__stdcall * Flip)( WRAP* This, LPDIRECTDRAWSURFACE lpDDSurfaceTargetOverride, DWORD dwFlags );
		HRESULT (__stdcall * GetAttachedSurface)( WRAP* This, LPDDSCAPS lpDDSCaps, LPDIRECTDRAWSURFACE *lplpDDAttachedSurface ); 
		HRESULT (__stdcall * GetBltStatus)( WRAP* This, DWORD dwFlags );
		HRESULT (__stdcall * GetCaps)( WRAP* This, LPDDSCAPS lpDDSCaps );
		HRESULT (__stdcall * GetClipper)( WRAP* This, LPDIRECTDRAWCLIPPER *lplpDDClipper );
		HRESULT (__stdcall * GetColorKey)( WRAP* This, DWORD dwFlags, LPDDCOLORKEY lpDDColorKey );
		HRESULT (__stdcall * GetDC)( WRAP* This, HDC *lphDC );
		HRESULT (__stdcall * GetFlipStatus)( WRAP* This, DWORD dwFlags );
		HRESULT (__stdcall * GetOverlayPosition)( WRAP* This, LPLONG lplX, LPLONG lplY );
		HRESULT (__stdcall * GetPalette)( WRAP* This, LPDIRECTDRAWPALETTE *lplpDDPalette );
		HRESULT (__stdcall * GetPixelFormat)( WRAP* This, LPDDPIXELFORMAT lpDDPixelFormat );
		HRESULT (__stdcall * GetSurfaceDesc)( WRAP* This, LPDDSURFACEDESC lpDDSurfaceDesc );
		HRESULT (__stdcall * Initialize)( WRAP* This, LPDIRECTDRAW lpDD, LPDDSURFACEDESC lpDDSurfaceDesc );
		HRESULT (__stdcall * IsLost)( WRAP* This );
		HRESULT (__stdcall * Lock)( WRAP* This, LPRECT lpDestRect, LPDDSURFACEDESC lpDDSurfaceDesc, DWORD dwFlags, HANDLE hEvent );
		HRESULT (__stdcall * ReleaseDC)( WRAP* This, HDC hDC );
		HRESULT (__stdcall * Restore)( WRAP* This );
		HRESULT (__stdcall * SetClipper)( WRAP* This, LPDIRECTDRAWCLIPPER lpDDClipper );
		HRESULT (__stdcall * SetColorKey)( WRAP* This, DWORD dwFlags, LPDDCOLORKEY lpDDColorKey );
		HRESULT (__stdcall * SetOverlayPosition)( WRAP* This, LONG lX, LONG lY );
		HRESULT (__stdcall * SetPalette)( WRAP* This, LPDIRECTDRAWPALETTE lpDDPalette );
		HRESULT (__stdcall * Unlock)( WRAP* This, LPVOID lpRect );
		HRESULT (__stdcall * UpdateOverlay)( WRAP* This, LPRECT lpSrcRect, LPDIRECTDRAWSURFACE lpDDDestSurface, LPRECT lpDestRect, DWORD dwFlags, LPDDOVERLAYFX lpDDOverlayFx );
		HRESULT (__stdcall * UpdateOverlayDisplay)( WRAP* This, DWORD dwFlags );
		HRESULT (__stdcall * UpdateOverlayZOrder)( WRAP* This, DWORD dwFlags, LPDIRECTDRAWSURFACE lpDDSReference );
		// added in v2
		HRESULT (__stdcall * GetDDInterface)( WRAP* This, LPVOID *lplpDD ); 
		HRESULT (__stdcall * PageLock)( WRAP* This, DWORD dwFlags );
		HRESULT (__stdcall * PageUnlock)( WRAP* This, DWORD dwFlags );
		// added in v3
		HRESULT (__stdcall * SetSurfaceDesc)( WRAP* This, LPDDSURFACEDESC lpDDSD, DWORD dwFlags );
		// added in v4
		HRESULT (__stdcall * SetPrivateData)( WRAP* This, const GUID & tag, LPVOID pData, DWORD cbSize, DWORD dwFlags );
		HRESULT (__stdcall * GetPrivateData)( WRAP* This, const GUID & tag, LPVOID pBuffer, LPDWORD pcbBufferSize );
		HRESULT (__stdcall * FreePrivateData)( WRAP* This, const GUID & tag );
		HRESULT (__stdcall * GetUniquenessValue)( WRAP* This, LPDWORD pValue );
		HRESULT (__stdcall * ChangeUniquenessValue)( WRAP* This );
		// added in v7
		HRESULT (__stdcall * SetPriority)( WRAP* This, DWORD prio );
		HRESULT (__stdcall * GetPriority)( WRAP* This, LPDWORD prio );
		HRESULT (__stdcall * SetLOD)( WRAP* This, DWORD lod );
		HRESULT (__stdcall * GetLOD)( WRAP* This, LPDWORD lod );
	};

	HRESULT __stdcall QueryInterface( WRAP* This, const IID& riid, void** ppvObject ) 
	{ 
		PROLOGUE;
		HRESULT hResult = This->dds1->lpVtbl->QueryInterface( This->dds1, riid, ppvObject );
		if( SUCCEEDED( hResult ) ) Wrap( This->dd_parent, iid_to_vtbl( riid ), ppvObject );
		EPILOGUE( hResult );
	}

	ULONG __stdcall AddRef( WRAP* This ) 
	{ 
		PROLOGUE;
		ULONG dwCount = This->dds1->lpVtbl->AddRef( This->dds1 );
		EPILOGUE( dwCount ); 
	}

	ULONG __stdcall Release( WRAP* This ) 
	{ 	
		PROLOGUE;
		ULONG dwCount = WrapRelease( This );
		EPILOGUE( dwCount );
	}

    HRESULT __stdcall AddAttachedSurface( WRAP* This, LPDIRECTDRAWSURFACE lpDDSAttachedSurface ) 
	{ 
		PROLOGUE;
		HRESULT hResult = This->dds1->lpVtbl->AddAttachedSurface( This->dds1, GetInnerInterface( lpDDSAttachedSurface ) );
		if( SUCCEEDED( hResult ) )
		{
			lpDDSAttachedSurface->lpVtbl->AddRef( lpDDSAttachedSurface );
		}
		EPILOGUE( hResult );
	}

    HRESULT __stdcall AddOverlayDirtyRect( WRAP* This, LPRECT lpRect )
	{
		PROLOGUE;
		HRESULT hResult = This->dds1->lpVtbl->AddOverlayDirtyRect( This->dds1, lpRect );
		EPILOGUE( hResult );
	}


	HRESULT __stdcall Blt( WRAP* This, LPRECT lpDestRect, LPDIRECTDRAWSURFACE lpDDSrcSurface, LPRECT lpSrcRect, DWORD dwFlags, LPDDBLTFX lpDDBltFx )
	{ 		
		PROLOGUE;

		// note:
		// dwDDROP is ignored as "no such ROPs are currently defined"
		// dwROP is assumed to be a rop3 with redundant low-word ignored

		// unwrap interfaces in the DDBLTFX structure if used //
		DDBLTFX fx;
		bool UsesPattern = ( dwFlags & DDBLT_ROP ) ? ( ((lpDDBltFx->dwROP & 0x00F00000) >> 4) != (lpDDBltFx->dwROP & 0x000F0000) ) : false;		
		if( ( UsesPattern != false ) || ( dwFlags & ( DDBLT_ALPHADESTSURFACEOVERRIDE | DDBLT_ALPHASRCSURFACEOVERRIDE | DDBLT_ZBUFFERDESTOVERRIDE | DDBLT_ZBUFFERSRCOVERRIDE )) )
		{
			if( ( lpDDBltFx != NULL ) && ( sizeof( fx ) <= lpDDBltFx->dwSize ) ) 
			{
				memcpy( &fx, lpDDBltFx, sizeof(fx) );
				if( dwFlags & DDBLT_ALPHADESTSURFACEOVERRIDE ) fx.lpDDSAlphaDest   = GetInnerInterface( fx.lpDDSAlphaDest   );
				if( dwFlags & DDBLT_ALPHASRCSURFACEOVERRIDE  ) fx.lpDDSAlphaSrc    = GetInnerInterface( fx.lpDDSAlphaSrc    );
				if( dwFlags & DDBLT_ZBUFFERDESTOVERRIDE      ) fx.lpDDSZBufferDest = GetInnerInterface( fx.lpDDSZBufferDest );
				if( dwFlags & DDBLT_ZBUFFERSRCOVERRIDE       ) fx.lpDDSZBufferSrc  = GetInnerInterface( fx.lpDDSZBufferSrc  );
				if( UsesPattern != false                     ) fx.lpDDSPattern     = GetInnerInterface( fx.lpDDSPattern     );
				lpDDBltFx = &fx;
			}
		}

		// unwrap lpDDSrcSurface only if used //
		bool UsesSource = ( dwFlags & DDBLT_ROP ) ? ( ((lpDDBltFx->dwROP & 0x00F00000) % 0x00500000) || ((lpDDBltFx->dwROP & 0x000F0000) % 0x00050000) ) : true;
		if( ( UsesSource != false ) || ( ! ( dwFlags & ( DDBLT_COLORFILL | DDBLT_DEPTHFILL ) ) ) )
		{
			lpDDSrcSurface = GetInnerInterface( lpDDSrcSurface );
		}

		//
		HRESULT hResult = This->dds1->lpVtbl->Blt( This->dds1, lpDestRect, lpDDSrcSurface, lpSrcRect, dwFlags, lpDDBltFx ); 
		EPILOGUE( hResult );
	}

	HRESULT __stdcall BltBatch( WRAP* This, LPDDBLTBATCH lpDDBltBatch, DWORD dwCount, DWORD dwFlags ) 
	{	// This method was never implemented by ddraw?
		PROLOGUE;
		UNREFERENCED_PARAMETER( This );
		UNREFERENCED_PARAMETER( lpDDBltBatch );
		UNREFERENCED_PARAMETER( dwCount );
		UNREFERENCED_PARAMETER( dwFlags );
		WARN( "E_NOTIMPL" );
		EPILOGUE( E_NOTIMPL );  
	}

	HRESULT __stdcall BltFast( WRAP* This, DWORD dwX, DWORD dwY, LPDIRECTDRAWSURFACE lpDDSrcSurface, LPRECT lpSrcRect, DWORD dwTrans ) 
	{
		PROLOGUE;
		HRESULT hResult = This->dds1->lpVtbl->BltFast( This->dds1, dwX, dwY, GetInnerInterface( lpDDSrcSurface ), lpSrcRect, dwTrans );
		EPILOGUE( hResult );  
	}

    HRESULT __stdcall DeleteAttachedSurface( WRAP* This, DWORD dwFlags, LPDIRECTDRAWSURFACE lpDDSAttachedSurface)
	{
		PROLOGUE;
		HRESULT hResult = This->dds1->lpVtbl->DeleteAttachedSurface( This->dds1, dwFlags, GetInnerInterface( lpDDSAttachedSurface ) );
		if( SUCCEEDED( hResult ) )
		{
			lpDDSAttachedSurface->lpVtbl->Release( lpDDSAttachedSurface );		
		}
		EPILOGUE( hResult ); 
	}


    HRESULT __stdcall EnumAttachedSurfaces( WRAP* This, LPVOID lpContext, LPDDENUMSURFACESCALLBACK lpEnumSurfacesCallback ) 
	{ 
		PROLOGUE;
		EnumStruct e;
		e.callback = lpEnumSurfacesCallback;
		e.context = lpContext;
		e.dd_parent = This->dd_parent;
		e.xVtbl = This->xVtbl;
		e.must_exist = true;
		HRESULT hResult = This->dds1->lpVtbl->EnumAttachedSurfaces( This->dds1, &e, &WrapEnumSurfacesCallback );
		EPILOGUE( hResult ); 
	}

	HRESULT __stdcall EnumOverlayZOrders( WRAP* This, DWORD dwFlags, LPVOID lpContext, LPDDENUMSURFACESCALLBACK lpfnCallback ) 
	{
		PROLOGUE;
		EnumStruct e;
		e.callback = lpfnCallback;
		e.context = lpContext;
		e.dd_parent = This->dd_parent;
		e.xVtbl = This->xVtbl;
		e.must_exist = true;
		HRESULT hResult = This->dds1->lpVtbl->EnumOverlayZOrders( This->dds1, dwFlags, &e, &WrapEnumSurfacesCallback );
		EPILOGUE( hResult ); 
	}

	HRESULT __stdcall Flip( WRAP* This, LPDIRECTDRAWSURFACE lpDDSurfaceTargetOverride, DWORD dwFlags ) 
	{ 
		PROLOGUE;
		HRESULT hResult = This->dds1->lpVtbl->Flip( This->dds1, GetInnerInterface( lpDDSurfaceTargetOverride ), dwFlags );
		EPILOGUE( hResult ); 
	}

    HRESULT __stdcall GetAttachedSurface( WRAP* This, LPDDSCAPS lpDDSCaps, LPDIRECTDRAWSURFACE* lplpDDAttachedSurface ) 
	{ 
		PROLOGUE;
		HRESULT hResult = This->dds1->lpVtbl->GetAttachedSurface( This->dds1, lpDDSCaps, lplpDDAttachedSurface );
		if( SUCCEEDED( hResult ) ) Wrap( This->dd_parent, This->xVtbl, (void**)lplpDDAttachedSurface );
		EPILOGUE( hResult );
	}

    HRESULT __stdcall GetBltStatus( WRAP* This, DWORD dwFlags ) 
	{ 
		PROLOGUE;
		HRESULT hResult = This->dds1->lpVtbl->GetBltStatus( This->dds1, dwFlags ); 
		EPILOGUE( hResult );
	}

    HRESULT __stdcall GetCaps( WRAP* This, LPDDSCAPS lpDDSCaps ) 
	{ 
		PROLOGUE;
		HRESULT hResult = This->dds1->lpVtbl->GetCaps( This->dds1, lpDDSCaps ); 
		EPILOGUE( hResult );
	}

    HRESULT __stdcall GetClipper( WRAP* This, LPDIRECTDRAWCLIPPER *lplpDDClipper ) 
	{ 
		PROLOGUE;
		HRESULT hResult = This->dds1->lpVtbl->GetClipper( This->dds1, lplpDDClipper ); 
		if( SUCCEEDED( hResult ) ) Wrap( This->dd_parent, iid_to_vtbl( IID_IDirectDrawClipper ), (void**)lplpDDClipper );
		EPILOGUE( hResult );
	}

    HRESULT __stdcall GetColorKey( WRAP* This, DWORD dwFlags, LPDDCOLORKEY lpDDColorKey ) 
	{ 
		PROLOGUE;
		HRESULT hResult = This->dds1->lpVtbl->GetColorKey( This->dds1, dwFlags, lpDDColorKey ); 
		EPILOGUE( hResult );
	}

    HRESULT __stdcall GetDC( WRAP* This, HDC *lphDC )
	{ 
		PROLOGUE;
		HRESULT hResult = This->dds1->lpVtbl->GetDC( This->dds1, lphDC );
		EPILOGUE( hResult );
	}

    HRESULT __stdcall GetFlipStatus( WRAP* This, DWORD dwFlags )
	{ 
		PROLOGUE;
		HRESULT hResult = This->dds1->lpVtbl->GetFlipStatus( This->dds1, dwFlags );
		EPILOGUE( hResult );
	}

    HRESULT __stdcall GetOverlayPosition( WRAP* This, LPLONG lplX, LPLONG lplY )
	{ 
		PROLOGUE;
		HRESULT hResult = This->dds1->lpVtbl->GetOverlayPosition( This->dds1, lplX, lplY );
		EPILOGUE( hResult );
	}

    HRESULT __stdcall GetPalette( WRAP* This, LPDIRECTDRAWPALETTE *lplpDDPalette )
	{ 
		PROLOGUE;
		HRESULT hResult = This->dds1->lpVtbl->GetPalette( This->dds1, lplpDDPalette );
		if( SUCCEEDED( hResult ) ) Wrap( This->dd_parent, iid_to_vtbl( IID_IDirectDrawPalette ), (void**)lplpDDPalette );
		EPILOGUE( hResult );
	}

	HRESULT __stdcall GetPixelFormat( WRAP* This, LPDDPIXELFORMAT lpDDPixelFormat )
	{
		PROLOGUE;
		HRESULT hResult = This->dds1->lpVtbl->GetPixelFormat( This->dds1, lpDDPixelFormat ); 
		EPILOGUE( hResult );
	}

    HRESULT __stdcall GetSurfaceDesc( WRAP* This, LPDDSURFACEDESC lpDDSurfaceDesc ) 
	{ 
		PROLOGUE;
		HRESULT hResult = This->dds1->lpVtbl->GetSurfaceDesc( This->dds1, lpDDSurfaceDesc );
		EPILOGUE( hResult );
	}

	HRESULT __stdcall Initialize( WRAP* This, LPDIRECTDRAW lpDD, LPDDSURFACEDESC lpDDSurfaceDesc )
	{ 
		PROLOGUE;
		HRESULT hResult = This->dds1->lpVtbl->Initialize( This->dds1, GetInnerInterface( lpDD ), lpDDSurfaceDesc );
		EPILOGUE( hResult );
	}

    HRESULT __stdcall IsLost( WRAP* This )
	{ 
		PROLOGUE;
		HRESULT hResult = This->dds1->lpVtbl->IsLost( This->dds1 );
		EPILOGUE( hResult );
	}

    HRESULT __stdcall Lock( WRAP* This, LPRECT lpDestRect, LPDDSURFACEDESC lpDDSurfaceDesc, DWORD dwFlags, HANDLE hEvent )
	{ 
		PROLOGUE;
		HRESULT hResult = This->dds1->lpVtbl->Lock( This->dds1, lpDestRect, lpDDSurfaceDesc, dwFlags, hEvent );
		EPILOGUE( hResult );
	}

    HRESULT __stdcall ReleaseDC( WRAP* This, HDC hDC )
	{ 
		PROLOGUE;
		HRESULT hResult = This->dds1->lpVtbl->ReleaseDC( This->dds1, hDC );
		EPILOGUE( hResult );
	}

    HRESULT __stdcall Restore( WRAP* This )
	{ 
		PROLOGUE;
		HRESULT hResult = This->dds1->lpVtbl->Restore( This->dds1 );
		EPILOGUE( hResult );
	}

    HRESULT __stdcall SetClipper( WRAP* This, LPDIRECTDRAWCLIPPER lpDDClipper ) 
	{ 
		PROLOGUE;
		HRESULT hResult;
		LPDIRECTDRAWCLIPPER old_clipper = NULL;
		hResult = This->dds1->lpVtbl->GetClipper( This->dds1, &old_clipper ); 
		if( SUCCEEDED( hResult ) )
		{
			Wrap( This->dd_parent, iid_to_vtbl( IID_IDirectDrawClipper ), (void**)&old_clipper );
		}
		hResult = This->dds1->lpVtbl->SetClipper( This->dds1, GetInnerInterface( lpDDClipper ) ); 
		if( old_clipper != NULL )
		{
			old_clipper->lpVtbl->Release( old_clipper );
		}
		EPILOGUE( hResult );
	}

    HRESULT __stdcall SetColorKey( WRAP* This, DWORD dwFlags, LPDDCOLORKEY lpDDColorKey )
	{ 
		PROLOGUE;
		HRESULT hResult = This->dds1->lpVtbl->SetColorKey( This->dds1, dwFlags, lpDDColorKey );
		EPILOGUE( hResult );
	}

    HRESULT __stdcall SetOverlayPosition( WRAP* This, LONG lX, LONG lY )
	{
		PROLOGUE;
		HRESULT hResult = This->dds1->lpVtbl->SetOverlayPosition( This->dds1, lX, lY );
		EPILOGUE( hResult );
	}

    HRESULT __stdcall SetPalette( WRAP* This, LPDIRECTDRAWPALETTE lpDDPalette )
	{ 
		PROLOGUE;
		HRESULT hResult;
		LPDIRECTDRAWPALETTE old_palette = NULL;
		hResult = This->dds1->lpVtbl->GetPalette( This->dds1, &old_palette ); 
		if( SUCCEEDED( hResult ) )
		{
			Wrap( This->dd_parent, iid_to_vtbl( IID_IDirectDrawPalette ), (void**)&old_palette );
		}
		hResult = This->dds1->lpVtbl->SetPalette( This->dds1, GetInnerInterface( lpDDPalette ) );
		if( old_palette != NULL )
		{
			old_palette->lpVtbl->Release( old_palette );
		}
		EPILOGUE( hResult );
	}

	// lpData is ddsd.lpSurface in version 1, 2, and 3
	// lpData is lpRect in versions 4 and 7
    HRESULT __stdcall Unlock( WRAP* This, LPVOID lpData ) 
	{ 
		PROLOGUE;
		HRESULT hResult = This->dds1->lpVtbl->Unlock( This->dds1, lpData ); 
		EPILOGUE( hResult );
	}

	HRESULT __stdcall UpdateOverlay( WRAP* This, LPRECT lpSrcRect, LPDIRECTDRAWSURFACE lpDDDestSurface, LPRECT lpDestRect, DWORD dwFlags, LPDDOVERLAYFX lpDDOverlayFx )
	{ 
		PROLOGUE;
		DDOVERLAYFX fx;

		// unwrap interfaces in the DDOVERLAYFX structure if used //
		if( dwFlags & ( DDOVER_ALPHADESTSURFACEOVERRIDE | DDOVER_ALPHASRCSURFACEOVERRIDE ) )
		{
			if( ( lpDDOverlayFx != NULL ) && ( sizeof( fx ) <= lpDDOverlayFx->dwSize ) ) 
			{
				memcpy( &fx, lpDDOverlayFx, sizeof(fx) );
				if( dwFlags & DDOVER_ALPHADESTSURFACEOVERRIDE ) fx.lpDDSAlphaDest   = GetInnerInterface( fx.lpDDSAlphaDest   );
				if( dwFlags & DDOVER_ALPHASRCSURFACEOVERRIDE  ) fx.lpDDSAlphaSrc    = GetInnerInterface( fx.lpDDSAlphaSrc    );
				lpDDOverlayFx = &fx;
			}
		}

		//
		HRESULT hResult = This->dds1->lpVtbl->UpdateOverlay( This->dds1, lpSrcRect, GetInnerInterface( lpDDDestSurface ), lpDestRect, dwFlags, lpDDOverlayFx );
		EPILOGUE( hResult );
	}

    HRESULT __stdcall UpdateOverlayDisplay( WRAP* This, DWORD dwFlags )
	{ 
		PROLOGUE;
		HRESULT hResult = This->dds1->lpVtbl->UpdateOverlayDisplay( This->dds1, dwFlags ); 
		EPILOGUE( hResult );
	}

    HRESULT __stdcall UpdateOverlayZOrder( WRAP* This, DWORD dwFlags, LPDIRECTDRAWSURFACE lpDDSReference )
	{ 
		PROLOGUE;
		if( dwFlags & ( DDOVERZ_INSERTINBACKOF | DDOVERZ_INSERTINFRONTOF ) ) lpDDSReference = GetInnerInterface( lpDDSReference );
		HRESULT hResult = This->dds1->lpVtbl->UpdateOverlayZOrder( This->dds1, dwFlags, lpDDSReference ); 
		EPILOGUE( hResult );
	}

	// v2 //

    HRESULT __stdcall GetDDInterface( WRAP* This, LPVOID* lplpDD ) 
	{ 
		// theoretically returns IUnknown but some programs may cast it to a different interface pointer...
		// instead of querying for the interface.
		PROLOGUE;
		HRESULT hResult = This->dds2->lpVtbl->GetDDInterface( This->dds2, lplpDD );
		if( SUCCEEDED( hResult ) ) Wrap( This->dd_parent, dds_to_dd_vtbl( This ), (void**)lplpDD );
		EPILOGUE( hResult ); 
	}

	HRESULT __stdcall PageLock( WRAP* This, DWORD dwFlags )
	{
		PROLOGUE;
		HRESULT hResult = This->dds2->lpVtbl->PageLock( This->dds2, dwFlags );
		EPILOGUE( hResult );
	}

    HRESULT __stdcall PageUnlock( WRAP* This, DWORD dwFlags )
	{
		PROLOGUE;
		HRESULT hResult = This->dds2->lpVtbl->PageUnlock( This->dds2, dwFlags );
		EPILOGUE( hResult ); 
	}

	// v3 //

    HRESULT __stdcall SetSurfaceDesc( WRAP* This, LPDDSURFACEDESC lpDDSD, DWORD dwFlags )
	{
		PROLOGUE;
		HRESULT hResult = This->dds3->lpVtbl->SetSurfaceDesc( This->dds3, lpDDSD, dwFlags );
		EPILOGUE( hResult );
	}

	// v4 //

    HRESULT __stdcall SetPrivateData( WRAP* This, const GUID & tag, LPVOID pData, DWORD cbSize, DWORD dwFlags )
	{ 
		PROLOGUE;
		HRESULT hResult = This->dds4->lpVtbl->SetPrivateData( This->dds4, tag, pData, cbSize, dwFlags );
		EPILOGUE( hResult );
	}

    HRESULT __stdcall GetPrivateData( WRAP* This, const GUID & tag, LPVOID pBuffer, LPDWORD pcbBufferSize )
	{
		PROLOGUE;
		HRESULT hResult = This->dds4->lpVtbl->GetPrivateData( This->dds4, tag, pBuffer, pcbBufferSize );
		EPILOGUE( hResult );
	}

    HRESULT __stdcall FreePrivateData( WRAP* This, const GUID & tag )
	{
		PROLOGUE;
		HRESULT hResult = This->dds4->lpVtbl->FreePrivateData( This->dds4, tag );
		EPILOGUE( hResult );
	}

    HRESULT __stdcall GetUniquenessValue( WRAP* This, LPDWORD pValue )
	{
		PROLOGUE;
		HRESULT hResult = This->dds4->lpVtbl->GetUniquenessValue( This->dds4, pValue ); 
		EPILOGUE( hResult );
	}

    HRESULT __stdcall ChangeUniquenessValue( WRAP* This )
	{ 
		PROLOGUE;
		HRESULT hResult = This->dds4->lpVtbl->ChangeUniquenessValue( This->dds4 );
		EPILOGUE( hResult );
	}

	// v7 //

    HRESULT __stdcall SetPriority( WRAP* This, DWORD prio )
	{
		PROLOGUE;
		HRESULT hResult = This->dds7->lpVtbl->SetPriority( This->dds7, prio );
		EPILOGUE( hResult );
	}

    HRESULT __stdcall GetPriority( WRAP* This, LPDWORD prio )
	{
		PROLOGUE;
		HRESULT hResult = This->dds7->lpVtbl->GetPriority( This->dds7, prio );
		EPILOGUE( hResult );
	}

    HRESULT __stdcall SetLOD( WRAP* This, DWORD lod )
	{
		PROLOGUE;
		HRESULT hResult = This->dds7->lpVtbl->SetLOD( This->dds7, lod );
		EPILOGUE( hResult );
	}

    HRESULT __stdcall GetLOD( WRAP* This, LPDWORD lod )
	{
		PROLOGUE;
		HRESULT hResult = This->dds7->lpVtbl->GetLOD( This->dds7, lod );
		EPILOGUE( hResult );
	}

	const XVTBL xVtbl1 = {
		QueryInterface,         // 0x00
		AddRef,                 // 0x04
		Release,                // 0x08
		AddAttachedSurface,     // 0x0C
		AddOverlayDirtyRect,    // 0x10
 		Blt,                    // 0x14
		BltBatch,               // 0x18
		BltFast,                // 0x1C
		DeleteAttachedSurface,  // 0x20
		EnumAttachedSurfaces,   // 0x24
		EnumOverlayZOrders,     // 0x28
		Flip,                   // 0x2C
		GetAttachedSurface,     // 0x30
		GetBltStatus,           // 0x34
		GetCaps,                // 0x38
		GetClipper,             // 0x3C
		GetColorKey,            // 0x40
		GetDC,                  // 0x44
		GetFlipStatus,          // 0x48
		GetOverlayPosition,     // 0x4C
		GetPalette,             // 0x50
		GetPixelFormat,         // 0x54
		GetSurfaceDesc,         // 0x58
		Initialize,             // 0x5C
		IsLost,                 // 0x60
		Lock,                   // 0x64
		ReleaseDC,              // 0x68
		Restore,                // 0x6C
		SetClipper,             // 0x70
		SetColorKey,            // 0x74
		SetOverlayPosition,     // 0x78
		SetPalette,             // 0x7C
		Unlock,                 // 0x80
		UpdateOverlay,          // 0x84
		UpdateOverlayDisplay,   // 0x88
		UpdateOverlayZOrder,    // 0x8C
		0, // 0x90
		0, // 0x94
		0, // 0x98
		0, // 0x9C
		0, // 0xA0
		0, // 0xA4
		0, // 0xA8  
		0, // 0xAC 
		0, // 0xB0
		0, // 0xB4
		0, // 0xB8
		0, // 0xBC
		0  // 0xC0
	};

	const XVTBL xVtbl2 = {
		QueryInterface,         // 0x00
		AddRef,                 // 0x04
		Release,                // 0x08
		AddAttachedSurface,     // 0x0C
		AddOverlayDirtyRect,    // 0x10
 		Blt,                    // 0x14
		BltBatch,               // 0x18
		BltFast,                // 0x1C
		DeleteAttachedSurface,  // 0x20
		EnumAttachedSurfaces,   // 0x24
		EnumOverlayZOrders,     // 0x28
		Flip,                   // 0x2C
		GetAttachedSurface,     // 0x30
		GetBltStatus,           // 0x34
		GetCaps,                // 0x38
		GetClipper,             // 0x3C
		GetColorKey,            // 0x40
		GetDC,                  // 0x44
		GetFlipStatus,          // 0x48
		GetOverlayPosition,     // 0x4C
		GetPalette,             // 0x50
		GetPixelFormat,         // 0x54
		GetSurfaceDesc,         // 0x58
		Initialize,             // 0x5C
		IsLost,                 // 0x60
		Lock,                   // 0x64
		ReleaseDC,              // 0x68
		Restore,                // 0x6C
		SetClipper,             // 0x70
		SetColorKey,            // 0x74
		SetOverlayPosition,     // 0x78
		SetPalette,             // 0x7C
		Unlock,                 // 0x80
		UpdateOverlay,          // 0x84
		UpdateOverlayDisplay,   // 0x88
		UpdateOverlayZOrder,    // 0x8C
		GetDDInterface,         // 0x90
		PageLock,               // 0x94
		PageUnlock,             // 0x98
		0, // 0x9C
		0, // 0xA0
		0, // 0xA4
		0, // 0xA8  
		0, // 0xAC 
		0, // 0xB0
		0, // 0xB4
		0, // 0xB8
		0, // 0xBC
		0  // 0xC0
	};

	const XVTBL xVtbl3 = {
		QueryInterface,         // 0x00
		AddRef,                 // 0x04
		Release,                // 0x08
		AddAttachedSurface,     // 0x0C
		AddOverlayDirtyRect,    // 0x10
 		Blt,                    // 0x14
		BltBatch,               // 0x18
		BltFast,                // 0x1C
		DeleteAttachedSurface,  // 0x20
		EnumAttachedSurfaces,   // 0x24
		EnumOverlayZOrders,     // 0x28
		Flip,                   // 0x2C
		GetAttachedSurface,     // 0x30
		GetBltStatus,           // 0x34
		GetCaps,                // 0x38
		GetClipper,             // 0x3C
		GetColorKey,            // 0x40
		GetDC,                  // 0x44
		GetFlipStatus,          // 0x48
		GetOverlayPosition,     // 0x4C
		GetPalette,             // 0x50
		GetPixelFormat,         // 0x54
		GetSurfaceDesc,         // 0x58
		Initialize,             // 0x5C
		IsLost,                 // 0x60
		Lock,                   // 0x64
		ReleaseDC,              // 0x68
		Restore,                // 0x6C
		SetClipper,             // 0x70
		SetColorKey,            // 0x74
		SetOverlayPosition,     // 0x78
		SetPalette,             // 0x7C
		Unlock,                 // 0x80
		UpdateOverlay,          // 0x84
		UpdateOverlayDisplay,   // 0x88
		UpdateOverlayZOrder,    // 0x8C
		GetDDInterface,         // 0x90
		PageLock,               // 0x94
		PageUnlock,             // 0x98
		SetSurfaceDesc,         // 0x9C
		0, // 0xA0
		0, // 0xA4
		0, // 0xA8  
		0, // 0xAC 
		0, // 0xB0
		0, // 0xB4
		0, // 0xB8
		0, // 0xBC
		0  // 0xC0
	};

	const XVTBL xVtbl4 = {
		QueryInterface,         // 0x00
		AddRef,                 // 0x04
		Release,                // 0x08
		AddAttachedSurface,     // 0x0C
		AddOverlayDirtyRect,    // 0x10
 		Blt,                    // 0x14
		BltBatch,               // 0x18
		BltFast,                // 0x1C
		DeleteAttachedSurface,  // 0x20
		EnumAttachedSurfaces,   // 0x24
		EnumOverlayZOrders,     // 0x28
		Flip,                   // 0x2C
		GetAttachedSurface,     // 0x30
		GetBltStatus,           // 0x34
		GetCaps,                // 0x38
		GetClipper,             // 0x3C
		GetColorKey,            // 0x40
		GetDC,                  // 0x44
		GetFlipStatus,          // 0x48
		GetOverlayPosition,     // 0x4C
		GetPalette,             // 0x50
		GetPixelFormat,         // 0x54
		GetSurfaceDesc,         // 0x58
		Initialize,             // 0x5C
		IsLost,                 // 0x60
		Lock,                   // 0x64
		ReleaseDC,              // 0x68
		Restore,                // 0x6C
		SetClipper,             // 0x70
		SetColorKey,            // 0x74
		SetOverlayPosition,     // 0x78
		SetPalette,             // 0x7C
		Unlock,                 // 0x80
		UpdateOverlay,          // 0x84
		UpdateOverlayDisplay,   // 0x88
		UpdateOverlayZOrder,    // 0x8C
		GetDDInterface,         // 0x90
		PageLock,               // 0x94
		PageUnlock,             // 0x98
		SetSurfaceDesc,         // 0x9C
		SetPrivateData,         // 0xA0
		GetPrivateData,         // 0xA4
		FreePrivateData,        // 0xA8  
		GetUniquenessValue,     // 0xAC 
		ChangeUniquenessValue,  // 0xB0
		0, // 0xB4
		0, // 0xB8
		0, // 0xBC
		0  // 0xC0
	};

	const XVTBL xVtbl7 = {
		QueryInterface,         // 0x00
		AddRef,                 // 0x04
		Release,                // 0x08
		AddAttachedSurface,     // 0x0C
		AddOverlayDirtyRect,    // 0x10
 		Blt,                    // 0x14
		BltBatch,               // 0x18
		BltFast,                // 0x1C
		DeleteAttachedSurface,  // 0x20
		EnumAttachedSurfaces,   // 0x24
		EnumOverlayZOrders,     // 0x28
		Flip,                   // 0x2C
		GetAttachedSurface,     // 0x30
		GetBltStatus,           // 0x34
		GetCaps,                // 0x38
		GetClipper,             // 0x3C
		GetColorKey,            // 0x40
		GetDC,                  // 0x44
		GetFlipStatus,          // 0x48
		GetOverlayPosition,     // 0x4C
		GetPalette,             // 0x50
		GetPixelFormat,         // 0x54
		GetSurfaceDesc,         // 0x58
		Initialize,             // 0x5C
		IsLost,                 // 0x60
		Lock,                   // 0x64
		ReleaseDC,              // 0x68
		Restore,                // 0x6C
		SetClipper,             // 0x70
		SetColorKey,            // 0x74
		SetOverlayPosition,     // 0x78
		SetPalette,             // 0x7C
		Unlock,                 // 0x80
		UpdateOverlay,          // 0x84
		UpdateOverlayDisplay,   // 0x88
		UpdateOverlayZOrder,    // 0x8C
		GetDDInterface,         // 0x90
		PageLock,               // 0x94
		PageUnlock,             // 0x98
		SetSurfaceDesc,         // 0x9C
		SetPrivateData,         // 0xA0
		GetPrivateData,         // 0xA4
		FreePrivateData,        // 0xA8  
		GetUniquenessValue,     // 0xAC 
		ChangeUniquenessValue,  // 0xB0
		SetPriority,            // 0xB4
		GetPriority,            // 0xB8
		SetLOD,                 // 0xBC
		GetLOD                  // 0xC0
	};
};
