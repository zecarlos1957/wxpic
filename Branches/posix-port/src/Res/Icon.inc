///////////////////////////////////////////////////////////////////////////////
// Name:        TAboutU.cpp
// Purpose:     Display the window About WxPic
// Author:      Philippe Chevrier
// Modified by:
// Created:     2009.06.01
// Copyright:   (c) 2009 Philippe Chevrier <pch@laposte.net>
// License:     GNU Lesser General Public License (LGPLv3)
///////////////////////////////////////////////////////////////////////////////

#ifndef ICON_16PX_32PX_DEF
#define ICON_16PX_32PX_DEF(name) ICON_16PX_DEF(name) ICON_32PX_DEF(name)
#endif
#ifdef ICON_DEF_GEN
#define ICON_16PX_DEF(name) ICON_DEF_GEN(name,16)
#define ICON_32PX_DEF(name) ICON_DEF_GEN(name,32)
#endif
#define ICON_DEF(name,size) size ## _DEF(name)

ICON_DEF(aWxPic,ICON_32PX)
ICON_DEF(ChipRead,ICON_16PX)
ICON_DEF(ChipVerify,ICON_16PX)
ICON_DEF(ChipWrite,ICON_16PX)
ICON_DEF(ChipReload,ICON_16PX)
ICON_DEF(LedGreen,ICON_16PX)
ICON_DEF(LedRed,ICON_16PX)
ICON_DEF(LedOff,ICON_16PX)
ICON_DEF(Open,ICON_16PX)
ICON_DEF(Save,ICON_16PX)

#undef ICON_DEF
#undef ICON_16PX_32PX_DEF
#undef ICON_16PX_DEF
#undef ICON_32PX_DEF
