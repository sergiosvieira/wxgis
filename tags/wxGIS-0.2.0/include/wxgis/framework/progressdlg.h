/******************************************************************************
 * Project:  wxGIS (GIS Catalog)
 * Purpose:  wxGISProgressDlg class.
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
//#include <wx/gauge.h>
#include <wx/sizer.h>
#include <wx/button.h>
#include <wx/dialog.h>

///////////////////////////////////////////////////////////////////////////////
/// Class wxGISProgressDlg
///////////////////////////////////////////////////////////////////////////////

class WXDLLIMPEXP_GIS_FRW wxGISProgressDlg : public wxDialog 
{
private:

protected:
	wxStaticText* m_staticText1;
	//wxGauge* m_gauge1;
    IProgressor* m_pProgressBar1;
	wxStaticText* m_staticText2;
	//wxGauge* m_gauge2;
    IProgressor* m_pProgressBar2;
	wxStdDialogButtonSizer* m_sdbSizer1;
	//wxButton* m_sdbSizer1OK;
	wxButton* m_sdbSizer1Cancel;

public:
	wxGISProgressDlg( wxWindow* parent, wxWindowID id = wxID_ANY, const wxString& title = _("Progress..."), const wxPoint& pos = wxDefaultPosition, const wxSize& size = wxSize( 376,185 ), long style = wxDEFAULT_DIALOG_STYLE );
	~wxGISProgressDlg();
    virtual void SetText1(wxString sNewText);
    virtual void SetText2(wxString sNewText);
    virtual IProgressor* GetProgressor1(void){return m_pProgressBar1;};
    virtual IProgressor* GetProgressor2(void){return m_pProgressBar2;};
    //events
    virtual void OnCancel( wxCommandEvent& event );
protected:
    ITrackCancel* m_pTrackCancel;

    DECLARE_EVENT_TABLE()
};

