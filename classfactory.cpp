#include "header.h"

namespace classfactory
{
	struct XVTBL
	{
		HRESULT (__stdcall * QueryInterface)( WRAP* This, const IID& riid, void** ppvObject ); 
		ULONG   (__stdcall * AddRef)( WRAP* This ); 
		ULONG   (__stdcall * Release)( WRAP* This ); 
		HRESULT (__stdcall * CreateInstance)( WRAP* This,  IUnknown *pUnkOuter, REFIID riid, void **ppvObject );
		HRESULT (__stdcall * LockServer)( WRAP* This, BOOL fLock );
	};
	
	HRESULT __stdcall QueryInterface( WRAP* This, const IID& riid, void** ppvObject ) 
	{ 
		PROLOGUE;
		HRESULT hResult = This->cf->lpVtbl->QueryInterface( This->cf, riid, ppvObject );
		if( SUCCEEDED( hResult ) ) Wrap( This->dd_parent, iid_to_vtbl( riid ), ppvObject );
		EPILOGUE( hResult );
	}

	ULONG __stdcall AddRef( WRAP* This ) 
	{ 
		PROLOGUE;
		ULONG dwCount = This->cf->lpVtbl->AddRef( This->cf );
		EPILOGUE( dwCount );
	}

	ULONG __stdcall Release( WRAP* This ) 
	{ 	
		PROLOGUE;
		ULONG dwCount = WrapRelease( This );
		EPILOGUE( dwCount );
	}

	HRESULT __stdcall CreateInstance( WRAP* This, IUnknown *pUnkOuter, REFIID riid, void **ppvObject )
	{
		PROLOGUE;
		HRESULT hResult = This->cf->lpVtbl->CreateInstance( This->cf, pUnkOuter, riid, ppvObject );
		if( SUCCEEDED( hResult ) ) Wrap( This->dd_parent, iid_to_vtbl( riid ), ppvObject );
		EPILOGUE( hResult );
	}

	HRESULT __stdcall LockServer( WRAP* This, BOOL fLock )
	{
		PROLOGUE;
		HRESULT hResult = This->cf->lpVtbl->LockServer( This->cf, fLock );
		EPILOGUE( hResult );
	} 

	const XVTBL xVtbl = 
	{ 
		QueryInterface, // 0x00
		AddRef,         // 0x04
		Release,        // 0x08
		CreateInstance, // 0x0C
		LockServer      // 0x10
	};
};