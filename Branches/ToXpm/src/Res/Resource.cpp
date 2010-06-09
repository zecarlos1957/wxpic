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


#define ICON_DEF_GEN(name,size)  wxIcon  *TResource::the ## name ## Icon ## size = NULL;
#include "Icon.inc"
#undef ICON_DEF_GEN

/*static*/ void TResource::Load (void)
{
#define ICON_DEF_GEN(name,size) the ## name ## Icon ## size = new wxIcon(_T(#name), wxBITMAP_TYPE_ICO_RESOURCE, size, size);
#include "Icon.inc"
#undef ICON_DEF_GEN
}

/*static*/ void TResource::Unload (void)
{
#define ICON_DEF_GEN(name,size) delete the ## name ## Icon ## size; the ## name ## Icon ## size = NULL;
#include "Icon.inc"
#undef ICON_DEF_GEN
}
