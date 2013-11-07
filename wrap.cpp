#include "header.h"

SYSTEM_INFO sSysInfo;
WRAP* wrap_list;
WRAP* wrap_freelist;
DD_LIFETIME* ddlt_list;
DD_LIFETIME* ddlt_freelist;
CRITICAL_SECTION cs;


const void* iid_to_vtbl( const GUID& riid )
{
	TRACE( ">" );
	LOG_GUID( riid );
	const void* xVtbl;
	if      ( riid == IID_IUnknown                ) xVtbl = &unknwn::xVtbl;
	else if ( riid == IID_IClassFactory           ) xVtbl = &classfactory::xVtbl;
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

// simple way to get small amounts of memory on demand...
// will not span page boundries
// will not consider alignment
// allocated memory can not be freed
// not thread safe
void* micro_alloc( int size )
{
	TRACE( ">" );
	static BYTE* end_region = 0; 
	static BYTE* end_page = 0; 
	static BYTE* alloc_ptr = 0;

	if( ( alloc_ptr + size ) >= end_page ) 
	{
		alloc_ptr = end_page;

		if( alloc_ptr == end_region )
		{	
			TRACE( "reserve a new memory region" );
			alloc_ptr = (BYTE*) VirtualAlloc( NULL, sSysInfo.dwAllocationGranularity, MEM_RESERVE, PAGE_NOACCESS );
			end_region = alloc_ptr + sSysInfo.dwAllocationGranularity;
		}

		if( alloc_ptr != NULL )
		{ 
			TRACE( "commit a page of the reserved region" );
			alloc_ptr = (BYTE*) VirtualAlloc( alloc_ptr, sSysInfo.dwPageSize, MEM_COMMIT, PAGE_READWRITE );
			end_page = alloc_ptr + sSysInfo.dwPageSize;
		}

		if( alloc_ptr == NULL )
		{
			WARN( "Allocation FAILED" );
			end_region = 0; 
			end_page = 0; 
		}
	}

	// "bump the pointer"
	void* p = alloc_ptr;
	alloc_ptr += size;
	return p;
}


// find existing or add a new wrapper for *ppvInterface
// returns true if a new wrapper is created
bool Wrap( DD_LIFETIME* dd_parent, const void* xVtbl, void** ppvInterface )
{
	TRACE( ">" );
	WRAP* w;
	bool bNew = false;

	// validate args
	if( ( xVtbl != NULL ) && ( ppvInterface != NULL ) && ( *ppvInterface != NULL ) )
	{
		EnterCriticalSection( &cs );

		// search list
		for( w = wrap_list; ( w != NULL ) && ( w->inner_iface != *ppvInterface ); w = w->next );	
		if( w != NULL )
		{
			TRACE( "found existing wrapper" );
			if( w->xVtbl != xVtbl ){ WARN("existing wrapper is for a different type"); }
			*((WRAP**)ppvInterface) = w;
		}
		else 
		{
			TRACE( "creating new wrapper" );
			if( wrap_freelist != NULL )
			{
				// pop
				w = wrap_freelist; 
				wrap_freelist = w->next;
			}
			else
			{
				w = (WRAP*) micro_alloc( sizeof( WRAP ) );
			}
			if( w != NULL )
			{

				// dd obj lifetime tracker //
				if( ( dd_parent != NULL ))
				{
					if( ( ( xVtbl == &unknwn::xVtbl ) && ( dd_parent->obj == ((IUnknown*)(*ppvInterface)) ) ) || 
					( ( xVtbl == &dd::xVtbl1 ) || ( xVtbl == &dd::xVtbl2 ) || ( xVtbl == &dd::xVtbl4 ) || ( xVtbl == &dd::xVtbl7 ) ) ) 
					{
						TRACE( "new interface of existing dd obj" );
						dd_parent->iface_cnt++;
					}
				}
				else
				{	
					if( ( xVtbl == &dd::xVtbl1 ) || ( xVtbl == &dd::xVtbl2 ) || ( xVtbl == &dd::xVtbl4 ) || ( xVtbl == &dd::xVtbl7 ) )
					{	
						TRACE( "new dd obj" );
						if( ddlt_freelist != NULL )
						{
							// pop
							dd_parent = ddlt_freelist; 
							ddlt_freelist = dd_parent->next;
						}
						else
						{
							dd_parent = (DD_LIFETIME*) micro_alloc( sizeof( DD_LIFETIME ) );
						}
						if( dd_parent != NULL )
						{
							((IUnknown*)(*ppvInterface))->lpVtbl->QueryInterface( ((IUnknown*)(*ppvInterface)), IID_IUnknown, (void**)&dd_parent->obj );
							dd_parent->obj->lpVtbl->Release( dd_parent->obj );
							dd_parent->iface_cnt = 1;
							dd_parent->next = ddlt_list;
							ddlt_list = dd_parent; // push
						}
					}
				}

				// construct
				w->xVtbl = xVtbl;
				w->inner_iface = *ppvInterface;
				w->dd_parent = dd_parent;		
				w->next = wrap_list; 
				wrap_list = w; // push
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
	TRACE( ">" );
	EnterCriticalSection( &cs );
	ULONG dwCount = This->unknwn->lpVtbl->Release( This->unknwn );
	if( dwCount == 0 )
	{
		TRACE( "interface destroyed" );
		WRAP* w = wrap_list;
		WRAP* prev = NULL;
		DD_LIFETIME* dd_parent = This->dd_parent;

		// if dd interface was destroyed
		if( ( dd_parent != NULL ) &&
			( (( This->xVtbl == &unknwn::xVtbl ) && ( This->unknwn == dd_parent->obj )) ||
			( This->xVtbl == &dd::xVtbl1 ) || ( This->xVtbl == &dd::xVtbl2 ) ||
			( This->xVtbl == &dd::xVtbl4 ) || ( This->xVtbl == &dd::xVtbl7 ) ) )
		{
			dd_parent->iface_cnt--;
			if( dd_parent->iface_cnt == 0 ) 
			{
				TRACE( "dd obj destroyed" );
				// destroy ALL wraps that have a matching dd_parent
				while( w != NULL )
				{
					if( w->dd_parent == dd_parent ) // if match
					{
						TRACE( "destroyed child\n" );
						// move to freelist
						if( prev != NULL )
						{
							prev->next = w->next;
							w->next = wrap_freelist;
							wrap_freelist = w; // push
							w = prev->next; // continue
						}
						else 
						{
							wrap_list = w->next;
							w->next = wrap_freelist;
							wrap_freelist = w; // push
							w = wrap_list; // continue
						}
					}
					else // else continue search
					{ 
						prev = w;
						w = w->next;
					}
				}
	
				// move dd_parent to the freelist
				DD_LIFETIME* d_prev = NULL;
				DD_LIFETIME* d;
				for( d = ddlt_list; ( d != NULL ) && ( d != dd_parent ); d = d->next ) d_prev = d; // find previons link
				if( d == NULL ){ WARN( "wrap not in list" ); }
				else
				{
					if( d_prev != NULL ) d_prev->next = d->next;
					else ddlt_list = d->next;
					d->next = ddlt_freelist;
					ddlt_freelist = d;
				}
			}
		}
		else
		{
			// move This to the freelist
			for( w = wrap_list; ( w != NULL ) && ( w != This ); w = w->next ) prev = w; // find previons link
			if( w == NULL ){ WARN( "wrap not in list" ); }
			else
			{
				if( prev != NULL ) prev->next = w->next;
				else wrap_list = w->next;
				w->next = wrap_freelist;
				wrap_freelist = w;
			}
		}
	}
	LeaveCriticalSection( &cs );
	return dwCount;
}


HRESULT __stdcall WrapEnumSurfacesCallback( LPDIRECTDRAWSURFACE lpDDSurface, LPDDSURFACEDESC lpDDSurfaceDesc, LPVOID lpContext )
{
	PROLOGUE;
	EnumStruct* e = (EnumStruct*)lpContext;
	bool bNew = Wrap( e->dd_parent, e->xVtbl, (void**)&lpDDSurface );
	if( ( bNew != false ) && ( e->must_exist != false ) ){ WARN( "interface was not wrapped" ); }
	HRESULT hResult = ((LPDDENUMSURFACESCALLBACK) e->callback)( lpDDSurface, lpDDSurfaceDesc, e->context );
	EPILOGUE( hResult );
}


// all versions of dd interfaces share the same functions ... ( for ease-of-use ) //
// all versions of dds interfaces share the same functions ... ( for ease-of-use ) //
// ... so a little extra work is needed to distinguish between different versions //
const void* dd_to_dds_vtbl( WRAP* This )
{
	TRACE( ">" );
	if( This->xVtbl == &dd::xVtbl7 ) return &dds::xVtbl7;
	else if( This->xVtbl == &dd::xVtbl4 ) return &dds::xVtbl4;
	return &dds::xVtbl1;
}

const void* dds_to_dd_vtbl( WRAP* This )
{
	TRACE( ">" );
	if( This->xVtbl == &dds::xVtbl7 ) return &dd::xVtbl7;
	else if( This->xVtbl == &dds::xVtbl4 ) return &dd::xVtbl4;
	return &dd::xVtbl2;
}


// There are no guarantees that an interface passed as argument is wrapped or not NULL //
IDirectDraw* GetInnerInterface( IDirectDraw* iface )
{
	TRACE( "dd" );
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
	TRACE( "dds" );
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
	TRACE( "clip" );
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
	TRACE( "pal" );
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


