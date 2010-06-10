///////////////////////////////////////////////////////////////////////////////
// Name:        Resource.h
// Purpose:     Load the application resources and made them available
//              through getters
// Author:      Philippe Chevrier
// Modified by:
// Created:     2009.06.01
// Copyright:   (c) 2009 Philippe Chevrier <pch@laposte.net>
// License:     GNU Lesser General Public License (LGPLv3)
///////////////////////////////////////////////////////////////////////////////

#ifndef RESOURCE_H_INCLUDED
#define RESOURCE_H_INCLUDED

#include <wx/icon.h>
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

class TResource
{
public:
#define ICON_DEF(name) static const wxIcon *Get ## name ## Icon (void) { return the ## name ## Icon; };
#include "Icon.inc"
#undef ICON_DEF

    static void Load   (void);
    static void Unload (void);

    enum
    {
        HELPID_MAIN_INDEX,
        HELPID_ID_LOCATIONS,
        HELPID_FAQ_LISTS,
        HELPID_LPT_INTERFACES,
    };

private:
#define ICON_DEF(name) static wxIcon *the ## name ## Icon;
#include "Icon.inc"
#undef ICON_DEF
};

#endif // RESOURCE_H_INCLUDED
