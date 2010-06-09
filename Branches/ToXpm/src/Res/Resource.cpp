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
#include "Res/Icon.autogen"


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
