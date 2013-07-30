#pragma once


// *approximate* when an dd object is destroyed by tracking interface creation and destruction
struct DD_LIFETIME
{
	IUnknown* obj;
	unsigned long iface_cnt; 
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


// SLOT must be smaller than sSysInfo.dwPageSize
struct SLOT
{
	union
	{
		SLOT* next;
		WRAP wrap;
		DD_LIFETIME dd_parent;
	};
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
