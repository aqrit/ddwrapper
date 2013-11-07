#pragma once

// when a dd object is destroyed all children are also destroyed ( surfaces, etc. )
// we've got no good way to known if an dd obj is destroyed
// a dd obj is destroyed when it has no active interfaces... 
// a dd object can have many interfaces at once... IDirectDraw, IDirectDraw2, IDirectDraw4, IDirectDraw7 and IUnknown
struct DD_LIFETIME
{
	IUnknown* obj;
	unsigned long iface_cnt; 
	DD_LIFETIME* next;
};

struct WRAP
{
	const void* xVtbl; // new vtbl
	union // original interface
	{
		void*                    inner_iface;
		IUnknown*                unknwn;
		IClassFactory*           cf;
		IDirectDraw*             dd1;
		IDirectDraw2*            dd2;
		IDirectDraw4*            dd4;
		IDirectDraw7*            dd7;
		IDirectDrawSurface*      dds1;
		IDirectDrawSurface2*     dds2;
		IDirectDrawSurface3*     dds3;
		IDirectDrawSurface4*     dds4;
		IDirectDrawSurface7*     dds7;
		IDirectDrawClipper*      clip;
		IDirectDrawPalette*      pal;
		IDirectDrawColorControl* color;
		IDirectDrawGammaControl* gamma;
	};
	DD_LIFETIME* dd_parent;
	WRAP* next; // for use in unordered hash map
};


struct EnumStruct
{
	const void* xVtbl;
	DD_LIFETIME* dd_parent;
	bool must_exist;
	void* callback;
	void* context;
};

extern CRITICAL_SECTION cs;
extern SYSTEM_INFO sSysInfo;

bool Wrap( DD_LIFETIME* dd_parent, const void* xVtbl, void** ppvInterface );
const void* dd_to_dds_vtbl( WRAP* This );
const void* dds_to_dd_vtbl( WRAP* This );
const void* iid_to_vtbl( const GUID& riid );
ULONG WrapRelease( WRAP* This );
HRESULT __stdcall WrapEnumSurfacesCallback( LPDIRECTDRAWSURFACE lpDDSurface, LPDDSURFACEDESC lpDDSurfaceDesc, LPVOID lpContext );
//
IDirectDraw*        GetInnerInterface( IDirectDraw* iface );
IDirectDrawSurface* GetInnerInterface( IDirectDrawSurface* iface );
IDirectDrawClipper* GetInnerInterface( IDirectDrawClipper* iface );
IDirectDrawPalette* GetInnerInterface( IDirectDrawPalette* iface );


