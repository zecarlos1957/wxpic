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
#include <wx/settings.h>
#include <vector>
#include "Appl.h"

enum
{
    MAX_HEX_STRING_LENGTH = 8,
    MIN_ASCII             = 0x0020,
    MAX_ASCII             = 0x007F,
    NON_ASCII_CHAR        = 0x25A1,
};

static const wxFont &getConstantFont (void)
{
    static wxFont ConstantFont = wxSystemSettings::GetFont(wxSYS_ANSI_FIXED_FONT);
    return ConstantFont;
}


class THexGridCellAttrProvider;

class THexGridTable : public wxGridTableBase
{
public:
    THexGridTable(THexGrid *pOwner)
        : wxGridTableBase()
        , aOwner      (pOwner)
        , aColCount   (0)
        , aColPower   (0)
        , aBuffer     (NULL)
    {
        aHexData   [0] = aHexData   [1] = NULL;
        aLength    [0] = aLength    [1] = 0;
        aGetAddress[0] = aGetAddress[1] = NULL;
        aFirstRow  [0] = aFirstRow  [1] = 0;
        SetAttrProvider(newAttrProvider());
    };

    void SetColCount (int pColCount)
    {
        if (pColCount == aColCount)
            return;

        aColCount = pColCount-1; //-- Remove Ascii column
        aColPower = 0;
        if (pColCount > 0)
        {
            int ColCount = aColCount;
            while ((ColCount & 1) == 0)
            {
                ColCount >>= 1;
                ++aColPower;
            }
            wxASSERT(ColCount == 1); //-- Le nombre de colonnes doit être une puissance de 2
        }
        if (aBuffer != NULL)
            delete[] aBuffer;
        aBuffer = new unsigned[aColCount];
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
            aOwner->SetCellSize(aFirstRow[pSecond]-1, 0, 1, aColCount);
        if (!pSecond && ((aLength[1] != 0) || !aTitle[1].IsEmpty()))
        {
            aFirstRow[1] = getNextRow(0);
            if (!aTitle[1].IsEmpty())
            {
                aOwner->SetCellSize(aFirstRow[1], 0, 1, aColCount);
                ++aFirstRow[1];
            }
        }
    }
    void            SetHexFormat  (int pBitCount);
    const wxString &GetHexModel   (void) const { return aHexModel; }
    const wxString &GetAsciiModel (void) const { return aAsciiModel; }
    const wxString &GetHexFormat  (void) const { return aHexFormat; }

    void                      SetDimmedColour    (wxColor pDimmedColour);
    THexGridCellAttrProvider *GetHexAttrProvider (void);

    THexGrid::EColour GetColour (int pRow, int pCol)
    {
        if (isTitleRow0(pRow) || isTitleRow1(pRow))
            return (pCol==0) ? THexGrid::colourTITLE : THexGrid::colourOVERLAP;
        unsigned Second = isSecond(pRow);
        return (isOut(pRow, Second))
                    ? THexGrid::colourEMPTY
                    : (*(aGetAddress[Second]))(((pRow-aFirstRow[Second])<<aColPower)+pCol).colour;
    }

    virtual int GetNumberRows(void) { return getNextRow(((aFirstRow[1] != 0) || (aLength[1] != 0)) ? 1 : 0);  }
    virtual int GetNumberCols(void) { return aColCount + 1; }
    virtual bool IsEmptyCell ( int row, int col )
    {
        if (isTitleRow0(row) || isTitleRow1(row))
            return (col != 0);
        unsigned Second = isSecond(row);
        return isOut(row, Second);
    }
    virtual wxString GetValue( int row, int col )
    {
        if (isTitleRow0(row))
            return aTitle[0];
        if (isTitleRow1(row))
            return aTitle[1];
        unsigned Second = isSecond(row);
        return isOut(row, Second)
                    ? wxString(wxEmptyString)
                    : ((col < (1 << aColPower))
                       ? wxString::Format(aFormat.c_str(), aHexData[Second][((row-aFirstRow[Second])<<aColPower)+col])
                       : getAsciiText(row, Second));
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
        return wxString::Format(_T("%04X"), (*(aGetAddress[Second]))((row-aFirstRow[Second])<<aColPower).address);
    }
    virtual wxString GetColLabelValue ( int col ) { return (col == aColCount) ? _T("ASCII") : wxString::Format(_T("%X"),col); }
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
        ||     ((typeName==wxGRID_VALUE_NUMBER) && !isTitleRow0(row) && !isTitleRow1(row) && (col < aColCount));
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

        Second  = isSecond(row);

        bool          Error   = false;
        int           MemAddr = (row-aFirstRow[Second])<<aColPower;
        uint32_t     *RowData = aHexData[Second]+MemAddr;
        const wxChar *Src     = value.c_str();

        if (col < aColCount)
        {
            //-- Convert Hexa String to Integer
            wxASSERT(value.Length() <= MAX_HEX_STRING_LENGTH);
            wxChar  Buffer[MAX_HEX_STRING_LENGTH+1];
            wxChar *Dst   = Buffer;
            for (; *Src != _T('\0'); ++Src)
            {
                if (((*Src >= _T('0')) && (*Src <= _T('9')))
                ||  ((*Src >= _T('A')) && (*Src <= _T('F')))
                ||  ((*Src >= _T('a')) && (*Src <= _T('f'))))
                    *(Dst++) = *Src;
                else
                    Error = true;
            }
            *Dst = 0;
            if (Error)
                wxBell();
            uint32_t  MemValue   = (uint32_t)_tcstol(Buffer, &Dst, 16);

            //-- Update the value in the Buffer
            MemAddr += col;
            uint32_t &EditedWord = *(RowData+col);
            if (EditedWord != MemValue)
            {
                EditedWord = MemValue;
                (*(aGetAddress[Second])).SetModified(MemAddr);
            }
        }
        else
        {
            //-- Convert ASCII string to value
            if (value.Len() != (aCharCount<<aColPower))
                wxBell();
            else
            {
                const wxChar *CurChar = value.c_str();
                for (int j = 0; j < aColCount; ++j)
                {
                    unsigned char *Src = (unsigned char*)(RowData+j);
                    uint32_t      *Wrd = aBuffer+j;
                    unsigned char *Dst = (unsigned char*)Wrd;
                    *Wrd = 0;
                    for (unsigned i = 0; i < aCharCount; ++i, ++CurChar, ++Src, ++Dst)
                        if (*CurChar != (wxChar)NON_ASCII_CHAR)
                        {
                            if ((*CurChar >= (wxChar)MIN_ASCII)
                            &&  (*CurChar <= (wxChar)MAX_ASCII))
                                *Dst = (unsigned char)*CurChar;
                            else
                                Error =true;
                        }
                        else if ((*CurChar == (wxChar)NON_ASCII_CHAR)
                        &&  (*Src >= MIN_ASCII)
                        &&  (*Src <= MAX_ASCII))
                            Error = true;
                        else
                            *Dst = *Src;

                    if (*Wrd & ~aBitMask)
                        Error = true;
                }
                if (Error)
                    wxBell();
                else
                {
                    //-- When conversion succeeded copy the values in the buffer
                    bool   Changed = false;
                    for (int i = 0; i < aColCount; ++i)
                        if (aBuffer[i] != RowData[i])
                        {
                            RowData[i] = aBuffer[i];
                            (*(aGetAddress[Second])).SetModified(MemAddr+i);
                            Changed = true;
                        }
                    if (Changed)
                        aOwner->ForceRefresh();
                }
            }
        }
    }

private:
    THexGrid *aOwner;
    THexGrid::TAddressGetter *aGetAddress [2];
    uint32_t *aHexData [2];
    int       aLength  [2];
    int       aFirstRow[2];
    wxString  aTitle   [2];
    //-- Number of words per row and bits per word
    int       aColCount;    //-- Number of columns
    int       aColPower;    //-- Number of shift to multiply by number of columns
    unsigned  aBitMask;     //-- Mask that the data should not overflow
    unsigned  aNibbleCount; //-- in Hex representation
    unsigned  aCharCount;   //-- in ASCII representation
    wxString  aFormat;      //-- Format of Hex representation
    wxString  aHexFormat;   //-- Format of Hex cell text editor
    unsigned *aBuffer;      //-- Buffer for processing ASCII entry
    wxString  aHexModel;    //-- Model of Hex value that can be used to size the corresponding cells
    wxString  aAsciiModel;  //-- Model of Ascii value that can be used to size the corresponding cells


    wxGridCellAttrProvider   *newAttrProvider(void);

    bool     isTitleRow0 (int pRow) { return ((pRow == 0)                && (!aTitle [0].IsEmpty())); }
    bool     isTitleRow1 (int pRow) { return ((pRow == aFirstRow[1] - 1) && (!aTitle [1].IsEmpty())); }
    unsigned isSecond    (int pRow) { return (pRow >= getNextRow(0)) ? 1 : 0; }
    bool     isOut       (int pRow, unsigned pSecond) { return ((pRow-aFirstRow[pSecond])<<aColPower) >= aLength[pSecond]; }
    int      getNextRow  (unsigned pSecond) { return ((aLength[pSecond] + (1 << aColPower) - 1) >> aColPower) + aFirstRow[pSecond];  }

    wxString getAsciiText(int pRow, unsigned pSecond)
    {
        wxString  Result;
        int       BufLen  = aCharCount<<aColPower;
        wxChar   *Buffer  = Result.GetWriteBuf(BufLen);
        uint32_t *RowData = aHexData[pSecond]+((pRow-aFirstRow[pSecond])<<aColPower);
        for (int j = 0; j < aColCount; ++j, ++RowData)
        {
            unsigned char *CurWord = (unsigned char*)RowData;
            for (unsigned i = 0; i < aCharCount; ++i, ++CurWord, ++Buffer)
                *Buffer = ((*CurWord < MIN_ASCII) || (*CurWord > MAX_ASCII)) ? NON_ASCII_CHAR : *CurWord;
        }
        Result.UngetWriteBuf(BufLen);
        return Result;
    }
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
    , aAsciiAttr  (new wxGridCellAttr)
    , aAsciiEditor(new wxGridCellTextEditor)
    {
        aDimmedAttr ->SetTextColour(*wxLIGHT_GREY);
        aErrorAttr  ->SetTextColour(*wxRED);
        aSpecialAttr->SetTextColour(wxColor(128, 0, 255));
        aTitleAttr  ->SetReadOnly();
        aEmptyAttr  ->SetReadOnly();
        aAsciiAttr  ->SetEditor(aAsciiEditor);
        aAsciiAttr  ->SetFont(getConstantFont());
    }

    void SetDimmedColour (wxColor pDimmedColour) { aDimmedAttr->SetTextColour(pDimmedColour); }
    void SetTitleWidth   (int     pColCount)
    {
        aTitleAttr->SetSize(1, pColCount);
        theOverlapAttrBuilder.SetWidth(pColCount);
    }
    void SetAsciiEditor (const wxString &pAsciiFormat)
    {
        aAsciiEditor->SetParameters(pAsciiFormat);
    }

    virtual ~THexGridCellAttrProvider()
    {
        aDimmedAttr ->DecRef();
        aErrorAttr  ->DecRef();
        aSpecialAttr->DecRef();
        aTitleAttr  ->DecRef();
        aEmptyAttr  ->DecRef();
        aAsciiAttr  ->DecRef();
    }

    virtual wxGridCellAttr *GetAttr(int row, int col, wxGridCellAttr::wxAttrKind  kind) const
    {
        wxGridCellAttr *Result = (wxGridCellAttr *)NULL;

        if ((kind == wxGridCellAttr::Any) || (kind == wxGridCellAttr::Cell))
        {
            if (col == aOwner->GetNumberCols()-1)
                Result = aAsciiAttr;
            else
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
    wxGridCellAttr * const aAsciiAttr;
    wxGridCellTextEditor * const aAsciiEditor;
};  //-- End of THexGridCellAttrProvider

wxGridCellAttrProvider   *THexGridTable::newAttrProvider    (void)                   { return new THexGridCellAttrProvider(this); }
void                      THexGridTable::SetDimmedColour    (wxColor pDimmedColour)  { GetHexAttrProvider()->SetDimmedColour(pDimmedColour); }
THexGridCellAttrProvider *THexGridTable::GetHexAttrProvider (void)                   { return dynamic_cast<THexGridCellAttrProvider*>(GetAttrProvider()); }
void THexGridTable::SetHexFormat (int pBitCount)
{
    aBitMask     = (1UL<<pBitCount)-1;
    aCharCount   = (pBitCount+7)/8;
    aNibbleCount = (pBitCount+3)/4;
    aFormat.Printf(_T("%%0%dX"), aNibbleCount);
    aHexFormat.Printf(_T("%d"), aNibbleCount);
    aHexModel.Printf(aFormat.c_str(), 0);
    int TotalChar = (aCharCount<<aColPower);
    int Delta     = TotalChar - aAsciiModel.Len();
    if (Delta > 0)
        aAsciiModel.Pad(Delta, NON_ASCII_CHAR);
    else
        aAsciiModel.Truncate(TotalChar);
    GetHexAttrProvider()->SetAsciiEditor(wxString::Format(_T("%d"), TotalChar));
}


//--------------------------------------------------------------
//--------------------------------------------------------------
//--------------------------------------------------------------

THexGrid::THexGrid (wxWindow *parent, wxWindowID id, const wxPoint& pos, const wxSize& size, long style, const wxString& name)
: wxGrid(parent, id, pos, size, style, name)
{
    aGridTable = new THexGridTable(this);
    aHexEditor = new wxGridCellTextEditor();
    aHexEditor->IncRef();
    SetDefaultEditor(aHexEditor);
    EnableDragColSize(false);
    EnableDragColMove(false);
    EnableDragRowSize(false);
    SetDefaultCellAlignment(wxALIGN_CENTER, wxALIGN_CENTER);

    wxClientDC Dc(this);
    Dc.SetFont(GetLabelFont());
    int LabelWidth, LabelHeight;
    Dc.GetTextExtent(_T("0000"), &LabelWidth, &LabelHeight);
    SetColLabelSize(LabelHeight+4);
    SetRowLabelSize(LabelWidth+8);

    SetTable(aGridTable);
}

THexGrid::~THexGrid()
{
    SetTable(NULL);
    delete aGridTable;
    aHexEditor->DecRef();
}

bool THexGrid::CreateGrid (int numRows, int numCols, wxGrid::wxGridSelectionModes selmode)
{
    //-- It is illegal to ask for some rows with this methods.
    //-- The rows are created automatically when loading the Hex Data
    wxASSERT(numRows==0);
    SetSelectionMode(selmode);
    aGridTable->SetColCount(numCols+1);
    aGridTable->GetHexAttrProvider()->SetTitleWidth(numCols+1);
    SetTable(aGridTable);
    return true;
}

void THexGrid::LoadHexData (uint32_t *pHexData, int pLength, TAddressGetter &pAddressGetter, wxString pTitle, uint32_t pSecond)
{
    BeginBatch();
    aGridTable->LoadHexData(pHexData, pLength, pAddressGetter, pTitle, pSecond);
    SetTable(aGridTable);
    ClearAttrCache(); //-- Fix wxGrid cache anomaly
    setColFormat();
}

void THexGrid::SetHexFormat(int pWordBitCount)
{
    BeginBatch();
    aGridTable->SetHexFormat(pWordBitCount);
    setColFormat();
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

void THexGrid::setColFormat (void)
{
    wxClientDC Dc(this);
    Dc.SetFont(GetDefaultCellFont());
    int ColWidth, ColHeight;
    Dc.GetTextExtent(aGridTable->GetHexModel(), &ColWidth, &ColHeight);
    SetDefaultColSize(ColWidth+7);
    Dc.SetFont(getConstantFont());
    Dc.GetTextExtent(aGridTable->GetAsciiModel(), &ColWidth, &ColHeight);
    SetColSize(GetNumberCols()-1, ColWidth+7);
    aHexEditor->SetParameters(aGridTable->GetHexFormat());
    EndBatch();
}
