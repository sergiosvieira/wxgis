/******************************************************************************
 * Project:  wxGIS (GIS Catalog)
 * Purpose:  PropertyPages of Catalog.
 * Author:   Bishop (aka Barishnikov Dmitriy), polimax@mail.ru
 ******************************************************************************
*   Copyright (C) 2010  Bishop
*
*    This program is free software: you can redistribute it and/or modify
*    it under the terms of the GNU General Public License as published by
*    the Free Software Foundation, either version 3 of the License, or
*    (at your option) any later version.
*
*    This program is distributed in the hope that it will be useful,
*    but WITHOUT ANY WARRANTY; without even the implied warranty of
*    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
*    GNU General Public License for more details.
*
*    You should have received a copy of the GNU General Public License
*    along with this program.  If not, see <http://www.gnu.org/licenses/>.
 ****************************************************************************/
#pragma once

#include "wx/intl.h"
#include "wx/string.h"
#include "wx/stattext.h"
#include "wx/statline.h"
#include "wx/gdicmn.h"
#include "wx/font.h"
#include "wx/colour.h"
#include "wx/settings.h"
#include "wx/textctrl.h"
#include "wx/bitmap.h"
#include "wx/image.h"
#include "wx/icon.h"
#include "wx/bmpbuttn.h"
#include "wx/button.h"
#include "wx/sizer.h"
#include "wx/choice.h"
#include "wx/panel.h"

#include "wxgis/framework/framework.h"
#include "wxgis/core/config.h"

///////////////////////////////////////////////////////////////////////////////
/// Class wxGISMiscPropertyPage
///////////////////////////////////////////////////////////////////////////////
class wxGISMiscPropertyPage : 
    public IPropertyPage
{
    DECLARE_DYNAMIC_CLASS(wxGISMiscPropertyPage)
public:
    wxGISMiscPropertyPage(void);
	~wxGISMiscPropertyPage();
    virtual bool Create(IFrameApplication* application, wxWindow* parent, wxWindowID id = wxID_ANY, const wxPoint& pos = wxDefaultPosition, const wxSize& size = wxSize( 420,540 ), long style = wxTAB_TRAVERSAL, const wxString& name = wxT("Misc_Panel"));
//  IPropertyPage    
    virtual wxString GetPageName(void){return wxString(_("Miscellaneous"));};
    virtual void Apply(void);
    //
    virtual void FillLangArray(wxString sPath);
    //events
    void OnOpenLocPath(wxCommandEvent& event);
    void OnOpenSysPath(wxCommandEvent& event);
    void OnOpenLogPath(wxCommandEvent& event);
protected:
	enum
	{
		ID_LOCPATH = 1000,
		ID_OPENLOCPATH,
		ID_LANGCHOICE,
		ID_SYSPATH,
		ID_OPENSYSPATH,
		ID_LOGPATH,
		ID_OPENLOGPATH,
	};
	
	wxStaticText* m_staticText1;
	wxTextCtrl* m_LocalePath;
	wxBitmapButton* m_bpOpenLocPath;
	wxStaticText* m_staticText2;
	wxChoice* m_LangChoice;
	wxStaticText* m_staticText3;
	wxTextCtrl* m_SysPath;
	wxBitmapButton* m_bpOpenSysPath;
	wxStaticText* m_staticText4;
	wxTextCtrl* m_LogPath;
	wxBitmapButton* m_bpOpenLogPath;
    wxCheckBox* m_checkDebug;
	wxStaticBitmap* m_bitmapwarn;
	wxStaticText* m_staticTextWarn;
    wxStaticLine* m_staticline;

    IFrameApplication* m_pApp;
    wxArrayString m_aLangs;
    wxImageList m_ImageList;

    DECLARE_EVENT_TABLE()
};