#ifndef TCODEMEMPANEL_H
#define TCODEMEMPANEL_H

//(*Headers(TCodeMemPanel)
#include <wx/sizer.h>
#include <wx/panel.h>
#include <wx/grid.h>
//*)
#include "HexGrid.h"
#include "MemAddrGetter.h"

class TCodeMemPanel: public wxPanel
{
	public:

		TCodeMemPanel(wxWindow* parent,wxWindowID id,const wxPoint& pos,const wxSize& size, long style, const wxString& name);
		virtual ~TCodeMemPanel();

		//(*Declarations(TCodeMemPanel)
		THexGrid* aCodeMemGrid;
		//*)

        TMemAddrGetter      aCodeMemAddrGetter1;
        TMemAddrGetter      aCodeMemAddrGetter2;

        void UpdateCodeMemDisplay(void);
        void MakeWeakCodeColour(void);

	protected:

		//(*Identifiers(TCodeMemPanel)
		static const long ID_CODE_MEM_GRID;
		//*)

	private:

		//(*Handlers(TCodeMemPanel)
		//*)

//        wxColour            aWeakCodeColour;

		DECLARE_EVENT_TABLE()
};

#endif
