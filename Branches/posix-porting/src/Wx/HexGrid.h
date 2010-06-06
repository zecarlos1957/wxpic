///////////////////////////////////////////////////////////////////////////////
// Name:        Language.cpp
// Purpose:     Manage the Language selection for an application
// Author:      Philippe Chevrier
// Modified by:
// Created:     2009.03.01
// Copyright:   (c) 2009 Philippe Chevrier <pch@laposte.net>
// License:     GNU Lesser General Public License (LGPLv3)
///////////////////////////////////////////////////////////////////////////////

#ifndef HEXGRID_H_INCLUDED
#define HEXGRID_H_INCLUDED

#include <wx/grid.h>
#include <stdint.h>

class THexGrid : public wxGrid
{
public:
    enum EColour
    {
        colourNORMAL,
        colourDIMMED,
        colourERROR,
        colourSPECIAL,
        //-- Next values are private and should not be returned by the TAddressGetter
        colourTITLE,
        colourOVERLAP,
        colourEMPTY,
    };
    struct TColouredAddress
    {
        short   address;
        EColour colour;
    };
    class TAddressGetter
    {
    public:
        //-- Provide the address and the color associated to a data index
        virtual TColouredAddress operator()(int pIndex) const = 0;
        virtual void           SetModified (int pIndex)       = 0;
    };
    void LoadHexData (uint32_t *pHexData, int pLength, TAddressGetter &pAddressGetter, wxString pTitle=wxEmptyString, unsigned pSecond = 0);
    void UnloadData  (uint32_t pSecond) { LoadHexData(NULL, 0, *(TAddressGetter*)NULL, wxEmptyString, pSecond); }
    void SetHexFormat(int pWordBitCount);
// TODO (Admin#1#): Add ASCI column

    //-- Constructor compatible with standard control prototype for wxSmith
    THexGrid (wxWindow *parent, wxWindowID id, const wxPoint& pos = wxDefaultPosition, const wxSize& size = wxDefaultSize, long style = 0, const wxString& name = wxT("HexGrid"));

    ~THexGrid();

    //-- This overloads the base class CreateGrid to act on the specific table and not on the default grid
    //-- The number of Rows must be 0, The number of Cols must be a Power of 2
    bool CreateGrid (int numRows, int numCols, wxGrid::wxGridSelectionModes selmode = wxGrid::wxGridSelectCells);

    //-- Overload the default colors setting
    void SetDefaultCellBackgroundColour (wxColour pBgColour);
    void SetDefaultCellTextColour       (wxColour pFgColour);

private:
    friend class THexGridTable;
    class THexGridTable          *aGridTable;
    wxGridCellTextEditor         *aEditor;

    void computeColours (void);
};

#endif // HEXGRID_H_INCLUDED
