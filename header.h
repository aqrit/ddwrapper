#pragma once
#pragma warning(disable: 4996)  // strcat is unsafe

#define INITGUID
#define CINTERFACE 
#include <windows.h>
#include <Mmsystem.h>
#include <guiddef.h>
#include <ddraw.h>
#include <d3d.h>
#include <dvp.h>

void __cdecl Log( char* fmt, ...);
#define PROLOGUE
#define EPILOGUE(x) return(x)
#define WARN(x) Log( "%s\n", x );

// classfactory.cpp
namespace classfactory
{
	struct XVTBL; 
	extern const XVTBL xVtbl;
};

// dd.cpp
namespace dd
{
	struct XVTBL; 
	extern const XVTBL xVtbl1;
	extern const XVTBL xVtbl2;
	extern const XVTBL xVtbl4;
	extern const XVTBL xVtbl7;
};

// dds.cpp
namespace dds
{
	struct XVTBL; 
	extern const XVTBL xVtbl1;
	extern const XVTBL xVtbl2;
	extern const XVTBL xVtbl3;
	extern const XVTBL xVtbl4;
	extern const XVTBL xVtbl7;
};

// clipper.cpp
namespace clipper
{
	struct XVTBL;
	extern const XVTBL xVtbl;
};

// palette.cpp
namespace palette
{
	struct XVTBL;
	extern const XVTBL xVtbl;
};

// color.cpp
namespace color
{
	struct XVTBL;
	extern const XVTBL xVtbl;
};

// gamma.cpp
namespace gamma
{
	struct XVTBL;
	extern const XVTBL xVtbl;
};

// unkwn.cpp
namespace unknwn
{
	struct XVTBL;
	extern const XVTBL xVtbl;
};

#include "wrap.h"