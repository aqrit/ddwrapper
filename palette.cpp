 #include "header.h"

namespace palette
{
	struct XVTBL
	{
		HRESULT (__stdcall * QueryInterface)( WRAP* This, const IID& riid, void** ppvObject ); 
		ULONG   (__stdcall * AddRef)( WRAP* This ); 
		ULONG   (__stdcall * Release)( WRAP* This ); 
		HRESULT (__stdcall * GetCaps)( WRAP* This, LPDWORD lpdwCaps );
		HRESULT (__stdcall * GetEntries)( WRAP* This, DWORD dwFlags, DWORD dwBase, DWORD dwNumEntries, LPPALETTEENTRY lpEntries );
		HRESULT (__stdcall * Initialize)( WRAP* This, LPDIRECTDRAW lpDD, DWORD dwFlags, LPPALETTEENTRY lpDDColorTable );
		HRESULT (__stdcall * SetEntries)( WRAP* This, DWORD dwFlags, DWORD dwStartingEntry, DWORD dwCount, LPPALETTEENTRY lpEntries );
	};

	HRESULT __stdcall QueryInterface( WRAP* This, const IID& riid, void** ppvObject ) 
	{ 
		PROLOGUE;
		HRESULT hResult = This->pal->lpVtbl->QueryInterface( This->pal, riid, ppvObject );
		if( SUCCEEDED( hResult ) ) Wrap( This->dd_parent, iid_to_vtbl( riid ), ppvObject );
		EPILOGUE( hResult );
	}

	ULONG __stdcall AddRef( WRAP* This ) 
	{ 
		PROLOGUE;
		ULONG dwCount = This->pal->lpVtbl->AddRef( This->pal );
		EPILOGUE( dwCount );
	}

	ULONG __stdcall Release( WRAP* This ) 
	{ 	
		PROLOGUE;
		ULONG dwCount = WrapRelease( This );
		EPILOGUE( dwCount );
	}

	HRESULT __stdcall GetCaps( WRAP* This, LPDWORD lpdwCaps )
	{
		PROLOGUE;
		HRESULT hResult = This->pal->lpVtbl->GetCaps( This->pal, lpdwCaps );
		EPILOGUE( hResult );
	}
	
	HRESULT __stdcall GetEntries( WRAP* This, DWORD dwFlags, DWORD dwBase, DWORD dwNumEntries, LPPALETTEENTRY lpEntries )
	{
		PROLOGUE;
		HRESULT hResult = This->pal->lpVtbl->GetEntries( This->pal, dwFlags, dwBase, dwNumEntries, lpEntries );
		EPILOGUE( hResult );
	}
	
	HRESULT __stdcall Initialize( WRAP* This, LPDIRECTDRAW lpDD, DWORD dwFlags, LPPALETTEENTRY lpDDColorTable )
	{
		PROLOGUE;
		HRESULT hResult = This->pal->lpVtbl->Initialize( This->pal, GetInnerInterface( lpDD ), dwFlags, lpDDColorTable );
		EPILOGUE( hResult );
	}

	HRESULT __stdcall SetEntries( WRAP* This, DWORD dwFlags, DWORD dwStartingEntry, DWORD dwCount, LPPALETTEENTRY lpEntries )
	{
		PROLOGUE;
		HRESULT hResult = This->pal->lpVtbl->SetEntries( This->pal, dwFlags, dwStartingEntry, dwCount, lpEntries );
		EPILOGUE( hResult );
	}

	const XVTBL xVtbl = 
	{ 
		QueryInterface, // 0x00
		AddRef,         // 0x04
		Release,        // 0x08
		GetCaps,        // 0x0C
		GetEntries,     // 0x10
		Initialize,     // 0x14
		SetEntries      // 0x18
	};
};
