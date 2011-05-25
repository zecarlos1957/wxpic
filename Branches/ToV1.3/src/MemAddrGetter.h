///////////////////////////////////////////////////////////////////////////////
// Name:        MemAddrGetter.h
// Purpose:     Fournit les infos sur les mots mémoire
// Author:      Philippe Chevrier
// Modified by:
// Created:     2009.06.01
// Copyright:   (c) 2009 Philippe Chevrier <pch@laposte.net>
// License:     GNU Lesser General Public License (LGPLv3)
///////////////////////////////////////////////////////////////////////////////

#ifndef MEMADDRGETTER_H_INCLUDED
#define MEMADDRGETTER_H_INCLUDED

#include "HexGrid.h"

class TMemAddrGetter : public THexGrid::TAddressGetter
{
public:
    virtual THexGrid::TColouredAddress operator()(int pIndex) const;
    virtual void                     SetModified (int pIndex);

    uint32_t *SetMemory   (int pWhichMemory, unsigned pMemLength);
    void      ApplyChange (void);
    void      Invalidate  (void) { aBufferLength = 0; }
    bool      IsModified  (void);
    uint32_t  AddressToTargetArrayIndex (uint32_t pAddress);

private:
    class T_PicBufferInfo *aBufferInfo;
    int                    aBufferLength;
};

#endif // MEMADDRGETTER_H_INCLUDED
