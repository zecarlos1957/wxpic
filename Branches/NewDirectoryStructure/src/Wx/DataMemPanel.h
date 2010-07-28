#ifndef DATAMEMPANEL_H
#define DATAMEMPANEL_H

//(*Headers(TDataMemPanel)
#include <wx/sizer.h>
#include <wx/panel.h>
#include <wx/grid.h>
//*)
#include "HexGrid.h"
#include "MemAddrGetter.h"

class TDataMemPanel: public wxPanel
{
	public:

		TDataMemPanel(wxWindow* parent,wxWindowID id,const wxPoint& pos,const wxSize& size, long style, const wxString& name);
		virtual ~TDataMemPanel();

		//(*Declarations(TDataMemPanel)
		THexGrid* aDataMemGrid;
		//*)

        TMemAddrGetter      aDataMemAddrGetter;

        void UpdateDataMemDisplay(void);

	protected:

		//(*Identifiers(TDataMemPanel)
		static const long ID_DATA_MEM_GRID;
		//*)

	private:

		//(*Handlers(TDataMemPanel)
		//*)

		DECLARE_EVENT_TABLE()
};

#endif
