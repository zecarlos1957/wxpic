///////////////////////////////////////////////////////////////////////////////
// Name:        Language.cpp
// Purpose:     Manage the Language selection for an application
// Author:      Philippe Chevrier
// Modified by:
// Created:     2009.03.01
// Copyright:   (c) 2009 Philippe Chevrier <pch@laposte.net>
// License:     GNU Lesser General Public License (LGPLv3)
///////////////////////////////////////////////////////////////////////////////

#include "HexGrid.h"
#include <wx/textctrl.h>
#include <wx/dcclient.h>
#include <vector>
#include "Appl.h"

#define MAX_HEX_STRING_LENGTH 8


class THexGridTable : public wxGridTableBase
{
public:
    THexGridTable(THexGrid *pOwner)
        : wxGridTableBase()
        , aOwner      (pOwner)
        , aColPower   (0)
    {
        aHexData   [0] = aHexData   [1] = NULL;
        aLength    [0] = aLength    [1] = 0;
        aGetAddress[0] = aGetAddress[1] = NULL;
        aFirstRow  [0] = aFirstRow  [1] = 0;
        SetAttrProvider(getAttrProvider());
    };

    void SetColCount (int pColCount)
    {
        aColPower = 0;
        if (pColCount > 0)
        {
            int ColCount = pColCount;
            while ((ColCount & 1) == 0)
            {
                ColCount >>= 1;
                ++aColPower;
            }
            wxASSERT(ColCount == 1); //-- Le nombre de colonnes doit être une puissance de 2
        }
    }

    //-- Charge la table
    void LoadHexData (uint32_t *pHexData, int pLength, THexGrid::TAddressGetter &pAddressGetter, wxString pTitle, unsigned pSecond)
    {
        wxASSERT(pSecond<2);
        if (!aTitle[pSecond].IsEmpty())
            aOwner->SetCellSize(aFirstRow[pSecond]-1, 0, 1, 1);
        if ((pSecond==0) && (!aTitle[1].IsEmpty()))
            aOwner->SetCellSize(aFirstRow[1]-1, 0, 1, 1);
        aHexData    [pSecond] = pHexData;
        aLength     [pSecond] = pLength;
        aGetAddress [pSecond] = &pAddressGetter;
        aTitle      [pSecond] = pTitle;
        int First = (pTitle.IsEmpty()) ? 0 : 1;
        if (pSecond)
            First += getNextRow(0);
        aFirstRow[pSecond] = First;
        if (!aTitle[pSecond].IsEmpty())
            aOwner->SetCellSize(aFirstRow[pSecond]-1, 0, 1, 1<<aColPower);
        if (!pSecond && ((aLength[1] != 0) || !aTitle[1].IsEmpty()))
        {
            aFirstRow[1] = getNextRow(0);
            if (!aTitle[1].IsEmpty())
            {
                aOwner->SetCellSize(aFirstRow[1], 0, 1, 1<<aColPower);
                ++aFirstRow[1];
            }
        }
    }
    void SetHexFormat (const wxString &pFormat) { aFormat = pFormat; };

    void SetDimmedColour (wxColor pDimmedColour);

    THexGrid::EColour GetColour (int pRow, int pCol)
    {
        if (isTitleRow0(pRow) || isTitleRow1(pRow))
            return (pCol==0) ? THexGrid::colourTITLE : THexGrid::colourOVERLAP;
        unsigned Second = isSecond(pRow);
        return (isOut(pRow, pCol, Second))
                    ? THexGrid::colourEMPTY
                    : (*(aGetAddress[Second]))(((pRow-aFirstRow[Second])<<aColPower)+pCol).colour;
    }

    virtual int GetNumberRows(void) { return getNextRow(((aFirstRow[1] != 0) || (aLength[1] != 0)) ? 1 : 0);  }
    virtual int GetNumberCols(void) { return 1 << aColPower; }
    virtual bool IsEmptyCell ( int row, int col )
    {
        if (isTitleRow0(row) || isTitleRow1(row))
            return (col != 0);
        unsigned Second = isSecond(row);
        return ((((row-aFirstRow[Second]) << aColPower) + col) >= aLength[Second]);
    }
    virtual wxString GetValue( int row, int col )
    {
        if (isTitleRow0(row))
            return aTitle[0];
        if (isTitleRow1(row))
            return aTitle[1];
        unsigned Second = isSecond(row);
        return isOut(row, col, Second)
                    ? wxString(wxEmptyString)
                    : wxString::Format(aFormat.c_str(), aHexData[Second][((row-aFirstRow[Second])<<aColPower)+col]);
    }
    virtual long GetValueAsLong(int row,int col )
    {
        unsigned Second = isSecond(row);
        return aHexData[Second][((row-aFirstRow[Second])<<aColPower)+col];
    }
    virtual wxString GetRowLabelValue ( int row )
    {
        if (isTitleRow0(row) || isTitleRow1(row))
            return wxEmptyString;
        unsigned Second = isSecond(row);
        return wxString::Format(wxT("%04X"), (*(aGetAddress[Second]))((row-aFirstRow[Second])<<aColPower).address);
    }
    virtual wxString GetColLabelValue ( int col ) { return wxString::Format(wxT("%X"),col); }
    virtual wxString GetTypeName(int row, int col){ return wxGRID_VALUE_STRING; }
    virtual void SetValueAsLong( int row, int col, long value )
    {
        wxASSERT(!isTitleRow0(row) && !isTitleRow1(row));
        unsigned Second = isSecond(row);
        aHexData[Second][((row-aFirstRow[Second])<<aColPower)+col]=value;
    }
    virtual bool CanGetValueAs ( int row, int col, const wxString& typeName )
    {
        return (typeName==wxGRID_VALUE_STRING)
        ||     ((typeName==wxGRID_VALUE_NUMBER) && !isTitleRow0(row) && !isTitleRow1(row));
    }
    virtual bool CanSetValueAs ( int row, int col, const wxString& typeName ) { return CanGetValueAs(row, col, typeName); }

    virtual void SetValue( int row, int col, const wxString& value )
    {
        unsigned Second = (isTitleRow0(row)) ? 0 : ((isTitleRow1(row)) ? 1 : 2);
        if (Second < 2)
        {
            if (col == 0)
                aTitle[Second] = value;
            return;
        }
        wxASSERT(value.Length() <= MAX_HEX_STRING_LENGTH);
        wxChar Buffer[MAX_HEX_STRING_LENGTH+1];
        bool         Error = false;
        const wxChar *Src   = value.c_str();
        wxChar       *Dst   = Buffer;
        for (; *Src != wxT('\0'); ++Src)
        {
            if (((*Src >= wxT('0')) && (*Src <= wxT('9')))
            ||  ((*Src >= wxT('A')) && (*Src <= wxT('F')))
            ||  ((*Src >= wxT('a')) && (*Src <= wxT('f'))))
                *(Dst++) = *Src;
            else
                Error = true;
        }
        *Dst = 0;
        if (Error)
            wxBell();
        Second = isSecond(row);
        int  MemAddr  = ((row-aFirstRow[Second])<<aColPower)+col;
        uint32_t MemValue = (uint32_t)_tcstol(Buffer, &Dst, 16);
        uint32_t &EditedWord = aHexData[Second][MemAddr];
        if (EditedWord != MemValue)
        {
            EditedWord = MemValue;
            (*(aGetAddress[Second])).SetModified(MemAddr);
        }
    }

private:
    THexGrid *aOwner;
    THexGrid::TAddressGetter *aGetAddress [2];
    uint32_t *aHexData [2];
    int       aLength  [2];
    int       aFirstRow[2];
    wxString  aTitle   [2];
    int       aColPower;
    wxString  aFormat;

    wxGridCellAttrProvider *getAttrProvider(void);

    bool     isTitleRow0 (int pRow) { return ((pRow == 0)                && (!aTitle [0].IsEmpty())); }
    bool     isTitleRow1 (int pRow) { return ((pRow == aFirstRow[1] - 1) && (!aTitle [1].IsEmpty())); }
    unsigned isSecond    (int pRow) { return (pRow >= getNextRow(0)) ? 1 : 0; }
    bool     isOut       (int pRow, int pCol, int pSecond) { return ((pRow-aFirstRow[pSecond])<<aColPower)+pCol >= aLength[pSecond]; }
    int      getNextRow  (unsigned pSecond) { return ((aLength[pSecond] + (1 << aColPower) - 1) >> aColPower) + aFirstRow[pSecond];  }
};


//--------------------------------------------------------------
//--------------------------------------------------------------
//--------------------------------------------------------------

class TOverlapAttrBuilder
{
public:
    /**/ TOverlapAttrBuilder(void)
    : aMaxWidth(1)
    {}
    /**/ ~TOverlapAttrBuilder()
    {
        for (int Count = aAttrTab.size()-1; Count >= 0; --Count)
            if (aAttrTab[Count] != NULL)
                aAttrTab[Count]->DecRef();
    }
    wxGridCellAttr *GetAttr(int pCol)
    {
        wxGridCellAttr *Result;
        if (aAttrTab.size() < (unsigned)pCol)
        {
            wxASSERT(pCol < aMaxWidth);
            aAttrTab.resize(aMaxWidth-1, NULL);
        }
        Result = aAttrTab[pCol-1];
        if (Result == NULL)
        {
            Result = new wxGridCellAttr;
            Result->SetSize(0, -pCol);
            aAttrTab[pCol-1] = Result;
        }
        Result->IncRef();
        return Result;
    }
    void SetWidth(int pWidth) { if (pWidth > aMaxWidth) aMaxWidth = pWidth; }

private:
    int                          aMaxWidth;
    std::vector<wxGridCellAttr*> aAttrTab;
};

static TOverlapAttrBuilder theOverlapAttrBuilder;

class THexGridCellAttrProvider : public wxGridCellAttrProvider
{
public:
    THexGridCellAttrProvider(THexGridTable *pOwner)
    : wxGridCellAttrProvider()
    , aOwner      (pOwner)
    , aDimmedAttr (new wxGridCellAttr)
    , aErrorAttr  (new wxGridCellAttr)
    , aSpecialAttr(new wxGridCellAttr)
    , aTitleAttr  (new wxGridCellAttr)
    , aEmptyAttr  (new wxGridCellAttr)
    {
        aDimmedAttr ->SetTextColour(*wxLIGHT_GREY);
        aErrorAttr  ->SetTextColour(*wxRED);
        aSpecialAttr->SetTextColour(wxColor(128, 0, 255));
        aTitleAttr  ->SetReadOnly();
        aEmptyAttr  ->SetReadOnly();
    }

    void SetDimmedColour (wxColor pDimmedColour) { aDimmedAttr->SetTextColour(pDimmedColour); }
    void SetTitleWidth   (int     pColCount)
    {
        aTitleAttr->SetSize(1, pColCount);
        theOverlapAttrBuilder.SetWidth(pColCount);
    }

    virtual ~THexGridCellAttrProvider()
    {
        aDimmedAttr ->DecRef();
        aErrorAttr  ->DecRef();
        aSpecialAttr->DecRef();
        aTitleAttr  ->DecRef();
        aEmptyAttr  ->DecRef();
    }

    virtual wxGridCellAttr *GetAttr(int row, int col, wxGridCellAttr::wxAttrKind  kind) const
    {
        wxGridCellAttr *Result = (wxGridCellAttr *)NULL;

        if ((kind == wxGridCellAttr::Any) || (kind == wxGridCellAttr::Cell))
        {
            THexGrid::EColour Colour = aOwner->GetColour(row, col);
            switch (Colour)
            {
            case THexGrid::colourNORMAL:
                break;
            case THexGrid::colourDIMMED:
                Result = aDimmedAttr;
                break;
            case THexGrid::colourERROR:
                Result = aErrorAttr;
                break;
            case THexGrid::colourSPECIAL:
                Result = aSpecialAttr;
                break;
            case THexGrid::colourTITLE:
                Result = aTitleAttr;
                break;
            case THexGrid::colourOVERLAP:
                Result = theOverlapAttrBuilder.GetAttr(col);
                break;
            case THexGrid::colourEMPTY:
                Result = aEmptyAttr;
                break;
            }
            if (Result!=NULL)
                Result->IncRef();
        }
        return Result;
    }

    virtual void SetAttr(wxGridCellAttr *attr, int row, int col) { wxASSERT(false); };
    virtual void SetRowAttr(wxGridCellAttr *attr, int row)       { wxASSERT(false); };
    virtual void SetColAttr(wxGridCellAttr *attr, int col)       { wxASSERT(false); };

private:
    THexGridTable  * const aOwner;
    wxGridCellAttr * const aDimmedAttr;
    wxGridCellAttr * const aErrorAttr;
    wxGridCellAttr * const aSpecialAttr;
    wxGridCellAttr * const aTitleAttr;
    wxGridCellAttr * const aEmptyAttr;
};  //-- End of THexGridCellAttrProvider

wxGridCellAttrProvider *THexGridTable::getAttrProvider(void) { return new THexGridCellAttrProvider(this); }
void THexGridTable::SetDimmedColour (wxColor pDimmedColour)  { static_cast<THexGridCellAttrProvider*>(GetAttrProvider())->SetDimmedColour(pDimmedColour); }


//--------------------------------------------------------------
//--------------------------------------------------------------
//--------------------------------------------------------------

THexGrid::THexGrid (wxWindow *parent, wxWindowID id, const wxPoint& pos, const wxSize& size, long style, const wxString& name)
: wxGrid(parent, id, pos, size, style, name)
{
    aGridTable = new THexGridTable(this);
    aEditor = new wxGridCellTextEditor();
    aEditor->IncRef();
    SetDefaultEditor(aEditor);
    EnableDragColSize(false);
    EnableDragColMove(false);
    EnableDragRowSize(false);
    SetDefaultCellAlignment(wxALIGN_CENTER, wxALIGN_CENTER);

    wxClientDC Dc(this);
    Dc.SetFont(GetLabelFont());
    int LabelWidth, LabelHeight;
    Dc.GetTextExtent(wxT("0000"), &LabelWidth, &LabelHeight);
    SetColLabelSize(LabelHeight+4);
    SetRowLabelSize(LabelWidth+8);

    SetTable(aGridTable);
}

THexGrid::~THexGrid()
{
    SetTable(NULL);
    delete aGridTable;
    aEditor->DecRef();
}

bool THexGrid::CreateGrid (int numRows, int numCols, wxGrid::wxGridSelectionModes selmode)
{
    //-- It is illegal to ask for some rows with this methods.
    //-- The rows are created automatically when loading the Hex Data
    wxASSERT(numRows==0);
    SetSelectionMode(selmode);
    aGridTable->SetColCount(numCols);
    static_cast<THexGridCellAttrProvider*>(aGridTable->GetAttrProvider())->SetTitleWidth(numCols);
    SetTable(aGridTable);
    return true;
}

void THexGrid::LoadHexData (uint32_t *pHexData, int pLength, TAddressGetter &pAddressGetter, wxString pTitle, uint32_t pSecond)
{
    aGridTable->LoadHexData(pHexData, pLength, pAddressGetter, pTitle, pSecond);
    SetTable(aGridTable);
    ClearAttrCache(); //-- Fix wxGrid cache anomaly
    ForceRefresh();
}

void THexGrid::SetHexFormat(int pWordBitCount)
{
    int NibbleCount = (pWordBitCount+3)/4;
    wxString Format;
    Format.Printf(wxT("%%0%dX"), NibbleCount);
    aGridTable->SetHexFormat(Format);
    wxString Sample;
    Sample.Printf(Format, 0);
    wxClientDC Dc(this);
    Dc.SetFont(GetDefaultCellFont());
    int ColWidth, ColHeight;
    Dc.GetTextExtent(Sample, &ColWidth, &ColHeight);
    SetDefaultColSize(ColWidth+7);
    aEditor->SetParameters(wxString::Format(wxT("%d"), NibbleCount));
}

void THexGrid::SetDefaultCellBackgroundColour (wxColour pBgColour)
{
    wxGrid::SetDefaultCellBackgroundColour(pBgColour);
    computeColours();
}

void THexGrid::SetDefaultCellTextColour (wxColour pFgColour)
{
    wxGrid::SetDefaultCellTextColour(pFgColour);
    computeColours();
}

void THexGrid::computeColours (void)
{
    wxColour ForegroundColour = GetDefaultCellTextColour();
    wxColour BackgroundColour = GetDefaultCellBackgroundColour();
    int Red   = (int)(ForegroundColour.Red())   + BackgroundColour.Red();
    int Green = (int)(ForegroundColour.Green()) + BackgroundColour.Green();
    int Blue  = (int)(ForegroundColour.Blue())  + BackgroundColour.Blue();
    aGridTable->SetDimmedColour(wxColour(Red/2, Green/2, Blue/2));
}

