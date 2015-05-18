#pragma once

#define INITGUID
#define CINTERFACE 

#include <windows.h>
#include <guiddef.h>
#include <ddraw.h>
#include <d3d.h>

#include "wrap.h"
#include "log.h"

namespace dd { struct XVTBL; extern const XVTBL xVtbl1; extern const XVTBL xVtbl2; extern const XVTBL xVtbl4; extern const XVTBL xVtbl7; };
namespace dds { struct XVTBL; extern const XVTBL xVtbl1; extern const XVTBL xVtbl2; extern const XVTBL xVtbl3; extern const XVTBL xVtbl4; extern const XVTBL xVtbl7; };
namespace classfactory { struct XVTBL; extern const XVTBL xVtbl; };
namespace clipper { struct XVTBL; extern const XVTBL xVtbl; };
namespace palette { struct XVTBL; extern const XVTBL xVtbl; };
namespace color { struct XVTBL; extern const XVTBL xVtbl; };
namespace gamma { struct XVTBL; extern const XVTBL xVtbl; };
namespace unknwn { struct XVTBL; extern const XVTBL xVtbl; };

