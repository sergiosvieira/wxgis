/******************************************************************************
 * Project:  wxGIS (GIS Catalog)
 * Purpose:  tool dialog class.
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

#include "wxgis/geoprocessingui/geoprocessingui.h"

#include <wx/intl.h>

#include <wx/sizer.h>
#include <wx/gdicmn.h>
#include <wx/button.h>
#include <wx/panel.h>
#include <wx/font.h>
#include <wx/colour.h>
#include <wx/settings.h>
#include <wx/string.h>
#include <wx/html/htmlwin.h>
#include <wx/splitter.h>
#include <wx/dialog.h>

///////////////////////////////////////////////////////////////////////////

///////////////////////////////////////////////////////////////////////////////
/// Class wxGISGPToolDlg
///////////////////////////////////////////////////////////////////////////////
class wxGISGPToolDlg : public wxDialog 
{
private:

protected:
    wxSplitterWindow* m_splitter;
    wxPanel* m_toolpanel;
    wxStdDialogButtonSizer* m_sdbSizer1;
    wxButton* m_sdbSizer1OK;
    wxButton* m_sdbSizer1Cancel;
    wxButton* m_sdbSizer1Help;
    //wxPanel* m_helppanel;
    wxHtmlWindow* m_htmlWin;

public:
    wxGISGPToolDlg( wxWindow* parent, wxWindowID id = wxID_ANY, const wxString& title = _("Tool name"), const wxPoint& pos = wxDefaultPosition, const wxSize& size = wxSize( 480,600 ), long style = wxCLOSE_BOX|wxDEFAULT_DIALOG_STYLE|wxMINIMIZE_BOX|wxRESIZE_BORDER|wxSYSTEM_MENU|wxSTAY_ON_TOP|wxDIALOG_NO_PARENT|wxCLIP_CHILDREN );
    ~wxGISGPToolDlg();
    void m_splitterOnIdle( wxIdleEvent& )
    {
        m_splitter->SetSashPosition( 0 );
        m_splitter->Disconnect( wxEVT_IDLE, wxIdleEventHandler( wxGISGPToolDlg::m_splitterOnIdle ), NULL, this );
    }
//events
    virtual void OnHelp(wxCommandEvent& event);
    virtual void OnHelpUI(wxUpdateUIEvent& event);
protected:
    int m_DataWidth, m_HtmlWidth;

    DECLARE_EVENT_TABLE()
};

