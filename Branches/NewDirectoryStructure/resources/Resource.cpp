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
#include "Res/Icon.autogen"
#else
#include "Res/WxPic.xpm"
#include "Res/ChipRead.xpm"
#include "Res/ChipVerify.xpm"
#include "Res/ChipWrite.xpm"
#include "Res/ChipReload.xpm"
#include "Res/LedGreen.xpm"
#include "Res/LedRed.xpm"
#include "Res/LedOff.xpm"
#include "Res/Open.xpm"
#include "Res/Save.xpm"
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
