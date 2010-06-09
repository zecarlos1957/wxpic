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

class TResource
{
public:
#define ICON_DEF_GEN(name,size) static const wxIcon *Get ## name ## Icon ## size (void) { return the ## name ## Icon ## size; };
#include "Icon.inc"
#undef ICON_DEF_GEN

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
#define ICON_DEF_GEN(name,size) static wxIcon *the ## name ## Icon ## size;
#include "Icon.inc"
#undef ICON_DEF_GEN
};

#endif // RESOURCE_H_INCLUDED
