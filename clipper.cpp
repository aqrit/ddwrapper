#include "header.h"

namespace clipper
{
	struct XVTBL
	{
		HRESULT (__stdcall * QueryInterface)( WRAP* This, const IID& riid, void** ppvObject ); 
		ULONG   (__stdcall * AddRef)( WRAP* This ); 
		ULONG   (__stdcall * Release)( WRAP* This ); 
		HRESULT (__stdcall * GetClipList)( WRAP* This, LPRECT lpRect, LPRGNDATA lpClipList, LPDWORD lpdwSize );
		HRESULT (__stdcall * GetHWnd)( WRAP* This, HWND* lphWnd );
		HRESULT (__stdcall * Initialize)( WRAP* This, LPDIRECTDRAW lpDD, DWORD dwFlags );
		HRESULT (__stdcall * IsClipListChanged)( WRAP* This, BOOL* lpbChanged );
		HRESULT (__stdcall * SetClipList)( WRAP* This, LPRGNDATA lpClipList, DWORD dwFlags );
		HRESULT (__stdcall * SetHWnd)( WRAP* This, DWORD dwFlags, HWND hWnd );
	};

	HRESULT __stdcall QueryInterface( WRAP* This, const IID& riid, void** ppvObject ) 
	{ 
		PROLOGUE;
		HRESULT hResult = This->clip->lpVtbl->QueryInterface( This->clip, riid, ppvObject );
		if( SUCCEEDED( hResult ) ) Wrap( This->dd_parent, iid_to_vtbl( riid ), ppvObject );
		EPILOGUE( hResult );
	}

	ULONG __stdcall AddRef( WRAP* This ) 
	{ 
		PROLOGUE;
		ULONG dwCount = This->clip->lpVtbl->AddRef( This->clip );
		EPILOGUE( dwCount );
	}

	ULONG __stdcall Release( WRAP* This ) 
	{ 	
		PROLOGUE;
		ULONG dwCount = WrapRelease( This );
		EPILOGUE( dwCount );
	}

    HRESULT __stdcall GetClipList( WRAP* This, LPRECT lpRect, LPRGNDATA lpClipList, LPDWORD lpdwSize )
	{
		PROLOGUE;
		HRESULT hResult = This->clip->lpVtbl->GetClipList( This->clip, lpRect, lpClipList, lpdwSize );
		EPILOGUE( hResult );
	}

    HRESULT __stdcall GetHWnd( WRAP* This, HWND* lphWnd )
	{
		PROLOGUE;
		HRESULT hResult = This->clip->lpVtbl->GetHWnd( This->clip, lphWnd );
		EPILOGUE( hResult );
	}

    HRESULT __stdcall Initialize( WRAP* This, LPDIRECTDRAW lpDD, DWORD dwFlags )
	{
		PROLOGUE;
		HRESULT hResult = This->clip->lpVtbl->Initialize( This->clip, GetInnerInterface( lpDD ), dwFlags );
		EPILOGUE( hResult );
	}

    HRESULT __stdcall IsClipListChanged( WRAP* This, BOOL* lpbChanged )
	{
		PROLOGUE;
		HRESULT hResult = This->clip->lpVtbl->IsClipListChanged( This->clip, lpbChanged );
		EPILOGUE( hResult );
	}

    HRESULT __stdcall SetClipList( WRAP* This, LPRGNDATA lpClipList, DWORD dwFlags )
	{
		PROLOGUE;
		HRESULT hResult = This->clip->lpVtbl->SetClipList( This->clip, lpClipList, dwFlags );
		EPILOGUE( hResult );
	}

    HRESULT __stdcall SetHWnd( WRAP* This, DWORD dwFlags, HWND hWnd )
	{
		PROLOGUE;
		HRESULT hResult = This->clip->lpVtbl->SetHWnd( This->clip, dwFlags, hWnd );
		EPILOGUE( hResult );
	}

	const XVTBL xVtbl = 
	{ 
		QueryInterface,    // 0x00
		AddRef,            // 0x04
		Release,           // 0x08
		GetClipList,       // 0x0C
		GetHWnd,           // 0x10
		Initialize,        // 0x14
		IsClipListChanged, // 0x18
		SetClipList,       // 0x1C
		SetHWnd            // 0x20
	};
};
