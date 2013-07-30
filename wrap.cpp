#include "header.h"

WRAP* buckets[1024]; // unordered hash map
SLOT* freelist = NULL; // linked list of pre-allocated memory slots 
CRITICAL_SECTION cs;
SYSTEM_INFO sSysInfo;


// determine bucket index ( 1024 buckets ) for this key (16 byte aligned interface address)
#define HASHER( key ) ((((uintptr_t)key ) >> 2 ) & 0x3FF )


const void* iid_to_vtbl( const GUID& riid )
{
	const void* xVtbl;

	if      ( riid == IID_IUnknown                ) xVtbl = &unknwn::xVtbl;
	else if ( riid == IID_IClassFactory           ) xVtbl = &classfactory::xVtbl;
	// IID_IDirectDrawClassFactory
	else if ( riid == IID_IDirectDraw             ) xVtbl = &dd::xVtbl1;
	else if	( riid == IID_IDirectDraw2            ) xVtbl = &dd::xVtbl2;
	else if	( riid == IID_IDirectDraw4            ) xVtbl = &dd::xVtbl4;
	else if	( riid == IID_IDirectDraw7            ) xVtbl = &dd::xVtbl7;
	else if ( riid == IID_IDirectDrawSurface      ) xVtbl = &dds::xVtbl1;
	else if ( riid == IID_IDirectDrawSurface2     ) xVtbl = &dds::xVtbl2; 
	else if ( riid == IID_IDirectDrawSurface3     ) xVtbl = &dds::xVtbl3;
	else if ( riid == IID_IDirectDrawSurface4     ) xVtbl = &dds::xVtbl4;
	else if ( riid == IID_IDirectDrawSurface7     ) xVtbl = &dds::xVtbl7;
	else if ( riid == IID_IDirectDrawClipper      ) xVtbl = &clipper::xVtbl;
	else if ( riid == IID_IDirectDrawPalette      ) xVtbl = &palette::xVtbl;
	else if ( riid == IID_IDirectDrawColorControl ) xVtbl = &color::xVtbl;
	else if ( riid == IID_IDirectDrawGammaControl ) xVtbl = &gamma::xVtbl;
	else xVtbl = NULL;

	return xVtbl;
}

// thread should already have lock
SLOT* AllocSlot()
{
	if( freelist == NULL ) // if out of slots
	{
		freelist = (SLOT*) VirtualAlloc( NULL, sSysInfo.dwAllocationGranularity, MEM_COMMIT | MEM_RESERVE, PAGE_READWRITE );
		if( freelist != NULL )
		{
			// build linked list
			for( SLOT* s = freelist; s < &freelist[((sSysInfo.dwAllocationGranularity / sizeof(SLOT)) - 1)]; s = s->next = &s[1] );
		}
		else
		{
			WARN( "Allocation FAILED" );
		}
	}

	// pop slot
	SLOT* slot = freelist;
	freelist = freelist->next;
	return slot;
}

// thread should already have lock
inline void FreeSlot( SLOT* slot )
{
	// push
	slot->next = freelist;
	freelist = slot;
}


// find existing or add a new wrapper for *ppvInterface
// returns true if a new wrapper is created
bool Wrap( DD_LIFETIME* dd_parent, const void* xVtbl, void** ppvInterface )
{
	WRAP* w;
	WRAP* list;
	bool bNew = false;

	// validate args
	if( ( xVtbl != NULL ) && ( ppvInterface != NULL ) && ( *ppvInterface != NULL ) )
	{
		EnterCriticalSection( &cs );

		// search list
		list = buckets[ HASHER( *ppvInterface ) ];
		for( w = list; (w != NULL ) && (w->inner_iface != *ppvInterface); w = w->next );	

		if( w != NULL ) // if existing wrapper is found 
		{
			if( w->xVtbl != xVtbl ){ WARN("existing wrapper is for a different type"); }
			*((WRAP**)ppvInterface) = w;
		}
		else // no wrapper found so create
		{
			w = (WRAP*) AllocSlot();
			if( w != NULL )
			{
				// dd lifetime tracker //
				if( ( xVtbl == &dd::xVtbl1 ) || ( xVtbl == &dd::xVtbl2 ) || ( xVtbl == &dd::xVtbl4 ) || ( xVtbl == &dd::xVtbl7 ) )
				{
					if( dd_parent != NULL )
					{
						dd_parent->iface_cnt++;
					}
					else
					{
						dd_parent = (DD_LIFETIME*) AllocSlot();
						if( dd_parent != NULL )
						{
							((IUnknown*)(*ppvInterface))->lpVtbl->QueryInterface( ((IUnknown*)(*ppvInterface)), IID_IUnknown, (void**)&dd_parent->obj );
							dd_parent->obj->lpVtbl->Release( dd_parent->obj );
							dd_parent->iface_cnt = 1;
						}
					}
				}
				else
				{
					if( ( dd_parent != NULL ) && ( dd_parent->obj == ((IUnknown*)(*ppvInterface))) )
					{
						dd_parent->iface_cnt++;
					}
				}
	
				// construct
				w->xVtbl = xVtbl;
				w->inner_iface = *ppvInterface;
				w->dd_parent = dd_parent;		
				w->next = list; 
				list = w; // push
				*((WRAP**)ppvInterface) = w; // hook it !
				bNew = true;
			}
		}
		LeaveCriticalSection( &cs );
	}
	return bNew;
}


ULONG WrapRelease( WRAP* This )
{
	EnterCriticalSection( &cs );
	ULONG dwCount = This->unknwn->lpVtbl->Release( This->unknwn );
	if( dwCount == 0 )
	{
		// find proceeding link in unordered_map //
		WRAP* list = buckets[ HASHER( This->inner_iface ) ];
		WRAP* prev = NULL;
		WRAP* wrap;
		for( wrap = list; ( wrap != NULL ) && ( wrap != This ); wrap = wrap->next ) prev = wrap;
		if( wrap != NULL )
		{
			// remove this wrapper from the buckets
			if( prev != NULL ) prev->next = wrap->next;
			else list = wrap->next; 

			// dd lifetime tracker //
			if( ( ( wrap->dd_parent != NULL ) && (wrap->unknwn == wrap->dd_parent->obj ) ) ||
			( ( wrap->xVtbl == &dd::xVtbl1 ) || ( wrap->xVtbl == &dd::xVtbl2 ) || ( wrap->xVtbl == &dd::xVtbl4 ) || ( wrap->xVtbl == &dd::xVtbl7 ) ) )
			{
				if( wrap->dd_parent->iface_cnt-- == 0 )
				{
					// destroy all wraps that have a matching dd_parent
					for( int i = 0; i < _countof( buckets ); i++ )
					{
						WRAP* next;
						prev = NULL;
						for( WRAP* w = buckets[i]; w != NULL; w = next )
						{
							next = w->next;
							if( w->dd_parent == wrap->dd_parent )
							{
								if( prev != NULL ) prev->next = next;
								else buckets[i] = next;
								FreeSlot( (SLOT*) w );
							}
							else
							{
								prev = w; 
							}
						}
					}
					FreeSlot( (SLOT*) wrap->dd_parent );
				}
			}
			// free wrap
			FreeSlot( (SLOT*) wrap );
		}
		else WARN( "wrap not in list" );
	}
	LeaveCriticalSection( &cs );
	return dwCount;
}


HRESULT __stdcall WrapEnumSurfacesCallback( LPDIRECTDRAWSURFACE lpDDSurface, LPDDSURFACEDESC lpDDSurfaceDesc, LPVOID lpContext )
{
	EnumStruct* e = (EnumStruct*)lpContext;
	bool bNew = Wrap( e->dd_parent, e->xVtbl, (void**)&lpDDSurface );
	if( ( bNew != false ) && ( e->must_exist != false ) ){ WARN( "interface was not wrapped" ); }
	return ((LPDDENUMSURFACESCALLBACK) e->callback)( lpDDSurface, lpDDSurfaceDesc, e->context );
}


// all versions of dd interfaces share the same functions ... ( for ease-of-use ) //
// all versions of dds interfaces share the same functions ... ( for ease-of-use ) //
// ... so a little extra work is needed to distinguish between different versions //
const void* dd_to_dds_vtbl( WRAP* This )
{
	if( This->xVtbl == &dd::xVtbl7 ) return &dds::xVtbl7;
	else if( This->xVtbl == &dd::xVtbl4 ) return &dds::xVtbl4;
	return &dds::xVtbl1;
}

const void* dds_to_dd_vtbl( WRAP* This )
{
	if( This->xVtbl == &dds::xVtbl7 ) return &dd::xVtbl7;
	else if( This->xVtbl == &dds::xVtbl4 ) return &dd::xVtbl4;
	return &dd::xVtbl2;
}


// There are no guarantees that an interface passed as argument is wrapped or not NULL //
IDirectDraw* GetInnerInterface( IDirectDraw* iface )
{
	if( iface != NULL )
	{
		if( ( ((WRAP*)iface)->xVtbl == &dd::xVtbl1 ) ||
			( ((WRAP*)iface)->xVtbl == &dd::xVtbl2 ) ||
			( ((WRAP*)iface)->xVtbl == &dd::xVtbl4 ) ||
			( ((WRAP*)iface)->xVtbl == &dd::xVtbl7 ) )
		{
			return ((WRAP*)iface)->dd1;
		}
		else WARN( "interface not wrapped" );
	}
	return iface;
}

IDirectDrawSurface* GetInnerInterface( IDirectDrawSurface* iface )
{
	if( iface != NULL )
	{
		if( ( ((WRAP*)iface)->xVtbl == &dds::xVtbl1 ) ||
			( ((WRAP*)iface)->xVtbl == &dds::xVtbl2 ) ||
			( ((WRAP*)iface)->xVtbl == &dds::xVtbl3 ) ||
			( ((WRAP*)iface)->xVtbl == &dds::xVtbl4 ) ||
			( ((WRAP*)iface)->xVtbl == &dds::xVtbl7 ) )
		{
			return ((WRAP*)iface)->dds1;
		}
		else WARN( "interface not wrapped" );
	}
	return iface;
}

IDirectDrawClipper* GetInnerInterface( IDirectDrawClipper* iface )
{
	if( iface != NULL )
	{
		if( ((WRAP*)iface)->xVtbl == &clipper::xVtbl )
		{
			return ((WRAP*)iface)->clip;
		}
		else WARN( "interface not wrapped" );
	}
	return iface;
}

IDirectDrawPalette* GetInnerInterface( IDirectDrawPalette* iface )
{
	if( iface != NULL )
	{
		if( ((WRAP*)iface)->xVtbl == &palette::xVtbl )
		{
			return ((WRAP*)iface)->pal;
		}
		else WARN( "interface not wrapped" );
	}
	return iface;
}


