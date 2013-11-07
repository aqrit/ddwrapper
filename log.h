/*
	all hookprocs start with PROLOGUE and end with EPILOGUE(x) macros
	those macros can be redefined for logging...

	internal functions are sprinkled with TRACE macros

	lets say we want to log only failed IDirectDrawClipper calls then
	we'd paste the "log light" section (below) in clipper.cpp after the #include(s)

	if instead we want to log failed calls for all interfaces then
	just uncomment the "log light" section in this header file

	LogDDSD and dds32_to_bmp are currently not used anywhere
	they exist in case someone wants them for debugging
*/

extern "C" void * _ReturnAddress( void );
#pragma intrinsic( _ReturnAddress )

void __cdecl Log( char* fmt, ... );
void LogDDSD( LPDDSURFACEDESC lpDDSurfaceDesc );
void LogGUID( const GUID& riid );
void dds32_to_bmp( IDirectDrawSurface* pDDSurface, char* szFileName );

// log nothing
#define EPILOGUE(x) return(x)	
#define LOG_GUID(x)
#define PROLOGUE
#define TRACE(x)
#define WARN(x)

/*
// log light ( warnings, failed iface calls, some iface ids )
#undef EPILOGUE
#undef LOG_GUID
#undef WARN
#define EPILOGUE(x) if(FAILED(x)){Log( "%08X %08X %s\r\n", x, _ReturnAddress(), __FUNCTION__ );} return(x);
#define LOG_GUID(x) LogGUID( x );
#define WARN(x)     Log( "WARNING: %s\r\n", x ); 
*/

/*
// log heavy
#undef EPILOGUE
#undef LOG_GUID
#undef PROLOGUE
#undef TRACE
#undef WARN
#define EPILOGUE(x) Log( "%08X %s\r\n", x, __FUNCTION__ ); return(x);	
#define LOG_GUID(x) LogGUID( x );
#define PROLOGUE    Log( "> %08X %s\r\n", _ReturnAddress(), __FUNCTION__ );
#define TRACE(x)	Log( "TRACE: %s %s\r\n", __FUNCTION__, x );
#define WARN(x)     Log( "WARNING: %s %s\r\n", __FUNCTION__, x ); 
*/


