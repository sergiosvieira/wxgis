/******************************************************************************
 * Project:  wxGIS (GIS Catalog)
 * Purpose:  wxGISMessageDlg class.
 * Author:   Bishop (aka Barishnikov Dmitriy), polimax@mail.ru
 ******************************************************************************
*   Copyright (C) 2009  Bishop
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
#include "wxgis/framework/framework.h"

#include <wx/intl.h>

#include <wx/string.h>
#include <wx/stattext.h>
#include <wx/gdicmn.h>
#include <wx/font.h>
#include <wx/colour.h>
#include <wx/settings.h>
#include <wx/sizer.h>
#include <wx/button.h>
#include <wx/checkbox.h>
#include <wx/dialog.h>

///////////////////////////////////////////////////////////////////////////////
/// Class wxGISMessageDlg
///////////////////////////////////////////////////////////////////////////////

class WXDLLIMPEXP_GIS_FRW wxGISMessageDlg : public wxDialog 
{
private:
    bool m_bNotShowInFuture;

protected:
	wxStaticText* m_MainText;
	wxStaticText* m_DecriptText;
	wxStdDialogButtonSizer* m_sdbSizer1;
	wxButton* m_sdbSizer1Yes;
	wxButton* m_sdbSizer1No;
	wxCheckBox* m_checkBox;

public:
	wxGISMessageDlg( wxWindow* parent, wxWindowID id = wxID_ANY, const wxString& sTitle = wxEmptyString, const wxString& sMainText = wxEmptyString,const wxString& sDescriptText = wxEmptyString, const wxPoint& pos = wxDefaultPosition, const wxSize& size = wxSize( 400,170 ), long style = wxDEFAULT_DIALOG_STYLE );
	~wxGISMessageDlg();
    bool GetShowInFuture(void){return !m_bNotShowInFuture;};
    //events
    virtual void OnQuit( wxCommandEvent& event );

    DECLARE_EVENT_TABLE()
};

