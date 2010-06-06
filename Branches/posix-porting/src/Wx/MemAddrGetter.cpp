///////////////////////////////////////////////////////////////////////////////
// Name:        MemAddrGetter.cpp
// Purpose:     Fournit les infos sur les mots mémoire
// Author:      Philippe Chevrier
// Modified by:
// Created:     2009.06.01
// Copyright:   (c) 2009 Philippe Chevrier <pch@laposte.net>
// License:     GNU Lesser General Public License (LGPLv3)
///////////////////////////////////////////////////////////////////////////////

#include "MemAddrGetter.h"
#include "../WinPic/WinPicPr/pic_hex.h"
#include "../WinPic/WinPicPr/devices.h"

THexGrid::TColouredAddress TMemAddrGetter::operator()(int pIndex) const
{
    THexGrid::TColouredAddress Result;
    Result.address = aBufferInfo->ArrayIndexToTargetAddress(pIndex);
    if (aBufferInfo->pwFlagCopy[pIndex] & (PIC_BUFFER_FLAG_PRG_ERROR | PIC_BUFFER_FLAG_VFY_ERROR | PIC_BUFFER_FLAG_DEAD))
        Result.colour = THexGrid::colourERROR;
    else if (Result.address == PIC_DeviceInfo.lAddressOscCal)
        Result.colour = THexGrid::colourSPECIAL;
    else if (aBufferInfo->pwFlagCopy[pIndex] & PIC_BUFFER_FLAG_USED)
        Result.colour = THexGrid::colourNORMAL;
    else
        Result.colour = THexGrid::colourDIMMED;
    return Result;
}

void TMemAddrGetter::SetModified (int pIndex)
{
    aBufferInfo->pwFlagCopy[pIndex] |= PIC_BUFFER_FLAG_USED;
    if (aBufferInfo->i32LastUsedIndexCopy < pIndex)
        aBufferInfo->i32LastUsedIndexCopy = pIndex;
}

uint32_t *TMemAddrGetter::SetMemory (int pWhichMemory, unsigned pMemLength)
{
    aBufferInfo   = PicBuf + pWhichMemory;
    aBufferLength = pMemLength*sizeof(uint32_t);
    //-- Effectuer la copie des données dans le buffer d'édition
    memcpy (aBufferInfo->pdwCopy,    aBufferInfo->pdwData, aBufferLength);
    memcpy (aBufferInfo->pwFlagCopy, aBufferInfo->pwFlags, aBufferLength/2);
    aBufferInfo->i32LastUsedIndexCopy = aBufferInfo->i32LastUsedArrayIndex;
    return aBufferInfo->pdwCopy;
}

void TMemAddrGetter::ApplyChange (void)
{
    if (aBufferLength > 0)
    {
        //-- Effectuer la mise à jour des données à partir du buffer d'édition
        memcpy (aBufferInfo->pdwData, aBufferInfo->pdwCopy,    aBufferLength);
        memcpy (aBufferInfo->pwFlags, aBufferInfo->pwFlagCopy, aBufferLength/2);
        aBufferInfo->i32LastUsedArrayIndex = aBufferInfo->i32LastUsedIndexCopy;
    }
}

bool TMemAddrGetter::IsModified (void)
{
    return (aBufferLength > 0) && (memcmp(aBufferInfo->pdwData, aBufferInfo->pdwCopy, aBufferLength) != 0);
}
