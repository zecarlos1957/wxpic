///////////////////////////////////////////////////////////////////////////////
// Name:        Resource.cpp
// Purpose:     Load the application resources and made them available
//              through getters
// Author:      Philippe Chevrier
// Modified by:
// Created:     2009.06.01
// Copyright:   (c) 2009 Philippe Chevrier <pch@laposte.net>
// License:     GNU Lesser General Public License (LGPLv3)
///////////////////////////////////////////////////////////////////////////////

#include "Resource.h"
#ifdef USE_AUTOGEN_ICON_INCLUDE
#include "Icon.autogen"
#else
#include "WxPic.xpm"
#include "ChipRead.xpm"
#include "ChipVerify.xpm"
#include "ChipWrite.xpm"
#include "ChipReload.xpm"
#include "LedGreen.xpm"
#include "LedRed.xpm"
#include "LedOff.xpm"
#include "Open.xpm"
#include "Save.xpm"
#include "WxPic32.xpm"
#endif


#define ICON_DEF(name)  wxIcon  *TResource::the ## name ## Icon = NULL;
#include "Icon.inc"
#undef ICON_DEF

/*static*/ void TResource::Load (void)
{
#define ICON_DEF(name) the ## name ## Icon = new wxIcon(name ## _xpm);
#include "Icon.inc"
#undef ICON_DEF
}

/*static*/ void TResource::Unload (void)
{
#define ICON_DEF(name) delete the ## name ## Icon; the ## name ## Icon = NULL;
#include "Icon.inc"
#undef ICON_DEF
}
