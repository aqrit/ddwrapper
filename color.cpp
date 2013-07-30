#include "header.h"

namespace color
{
	struct XVTBL
	{
		HRESULT (__stdcall * QueryInterface)( WRAP* This, const IID& riid, void** ppvObject ); 
		ULONG   (__stdcall * AddRef)( WRAP* This ); 
		ULONG   (__stdcall * Release)( WRAP* This ); 
		HRESULT (__stdcall * GetColorControls)( WRAP* This, LPDDCOLORCONTROL lpColorControl );
		HRESULT (__stdcall * SetColorControls)( WRAP* This, LPDDCOLORCONTROL lpColorControl );
	};

	HRESULT __stdcall QueryInterface( WRAP* This, const IID& riid, void** ppvObject ) 
	{ 
		PROLOGUE;
		HRESULT hResult = This->color->lpVtbl->QueryInterface( This->color, riid, ppvObject );
		if( SUCCEEDED( hResult ) ) Wrap( This->dd_parent, iid_to_vtbl( riid ), ppvObject );
		EPILOGUE( hResult );
	}

	ULONG __stdcall AddRef( WRAP* This ) 
	{ 
		PROLOGUE;
		ULONG dwCount = This->color->lpVtbl->AddRef( This->color );
		EPILOGUE( dwCount );
	}

	ULONG __stdcall Release( WRAP* This ) 
	{ 	
		PROLOGUE;
		ULONG dwCount = WrapRelease( This );
		EPILOGUE( dwCount );
	}

	HRESULT __stdcall GetColorControls( WRAP* This, LPDDCOLORCONTROL lpColorControl )
	{ 
		PROLOGUE;
		HRESULT hResult = This->color->lpVtbl->GetColorControls( This->color, lpColorControl );
		EPILOGUE( hResult );
	}
	
	HRESULT __stdcall SetColorControls( WRAP* This, LPDDCOLORCONTROL lpColorControl )
	{ 
		PROLOGUE;
		HRESULT hResult = This->color->lpVtbl->SetColorControls( This->color, lpColorControl );
		EPILOGUE( hResult );
	}

	const XVTBL xVtbl = 
	{ 
		QueryInterface,    // 0x00
		AddRef,            // 0x04
		Release,           // 0x08
		GetColorControls,  // 0x0C
		SetColorControls,  // 0x10
	};
};