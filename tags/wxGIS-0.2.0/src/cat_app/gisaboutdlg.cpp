/******************************************************************************
 * Project:  wxGIS (GIS Catalog)
 * Purpose:  About Dialog class.
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

#include "wxgis/cat_app/gisaboutdlg.h"
#include "wxgis/catalog/catalog.h"

#include "../../art/logo.xpm"

#include "wx/version.h"
#include "geos_c.h"
#include "proj_api.h"

///////////////////////////////////////////////////////////////////////////////
/// Class wxGISSimpleTextPanel
///////////////////////////////////////////////////////////////////////////////

wxGISSimpleTextPanel::wxGISSimpleTextPanel( wxString soText, wxWindow* parent, wxWindowID id, const wxPoint& pos, const wxSize& size, long style ) : wxPanel( parent, id, pos, size, style )
{
	wxBoxSizer* bSizer = new wxBoxSizer( wxVERTICAL );

    m_pStaticText = new wxStaticText( this, wxID_ANY, soText, wxDefaultPosition, wxDefaultSize, 0 );
	m_pStaticText->Wrap( -1 );
	bSizer->Add( m_pStaticText, 1, wxALL|wxEXPAND, 10 );

	this->SetSizer( bSizer );
	this->Layout();
}

wxGISSimpleTextPanel::~wxGISSimpleTextPanel()
{
}

///////////////////////////////////////////////////////////////////////////////
/// Class wxGISAboutDialog
///////////////////////////////////////////////////////////////////////////////

wxGISAboutDialog::wxGISAboutDialog( wxWindow* parent, wxWindowID id, const wxString& title, const wxPoint& pos, const wxSize& size, long style ) : wxDialog( parent, id, title, pos, size, style )
{
	this->SetSizeHints( wxDefaultSize, wxDefaultSize );

    wxColour BackColor( 255, 255, 255 );
	
	wxBoxSizer* bMainSizer = new wxBoxSizer( wxVERTICAL );	
	wxBoxSizer* bHeadSizer = new wxBoxSizer( wxHORIZONTAL );
	
	m_bitmap = new wxStaticBitmap( this, wxID_ANY, wxBitmap(logo_xpm), wxPoint( -1,-1 ), wxDefaultSize, wxNO_BORDER );
	m_bitmap->SetForegroundColour( BackColor );
	m_bitmap->SetBackgroundColour( BackColor );
	
	bHeadSizer->Add( m_bitmap, 0, 0, 5 );
	
	m_title = new wxStaticText( this, wxID_ANY, wxString::Format(_("\nwxGIS [%s]\nVersion: %s"), APP_NAME, APP_VER), wxDefaultPosition, wxDefaultSize, 0 );
    wxFont titleFont = this->GetFont();
    titleFont.SetWeight(wxFONTWEIGHT_BOLD);
    m_title->SetFont(titleFont);
	m_title->Wrap( -1 );
	m_title->SetBackgroundColour( wxColour( 255, 255, 255 ) );
 //   m_staticText1->
	
	bHeadSizer->Add( m_title, 1, wxEXPAND, 5 );
	
	bMainSizer->Add( bHeadSizer, 0, wxEXPAND, 5 );
	
	m_staticline1 = new wxStaticLine( this, wxID_ANY, wxDefaultPosition, wxDefaultSize, wxLI_HORIZONTAL );
	bMainSizer->Add( m_staticline1, 0, wxEXPAND|wxALL, 5 );

    wxString sGEOSStr = wgMB2WX(GEOSversion());
    wxString sPrjStr = wgMB2WX(pj_get_release());
    wxString sGDALStr = wgMB2WX(GDAL_RELEASE_NAME);
    wxString sWXStr = wxVERSION_STRING;

	m_AuiNotebook = new wxAuiNotebook( this, wxID_ANY, wxDefaultPosition, wxDefaultSize, wxAUI_NB_TOP | wxNO_BORDER | wxAUI_NB_TAB_MOVE );

    wxString sAboutApp = wxString::Format(_("wxGIS [%s]\n\nVersion: %s\n\n(c) 2009-2010 Dmitry Barishnikov (Bishop)"), APP_NAME, APP_VER);
	m_AuiNotebook->AddPage(new wxGISSimpleTextPanel(sAboutApp, m_AuiNotebook), _("About application"));

    long dFreeMem =  (long)(wxGetFreeMemory().ToLong() / 1048576);
    wxString sAboutSys = wxString::Format(_("HOST '%s'\n\nOS desc - %s\n\nFree memory - %u Mb\n\nLibs:\n\nGDAL %s\nGEOS %s\nPROJ %s\n%s"), wxGetFullHostName().c_str(), wxGetOsDescription().c_str(), dFreeMem, sGDALStr.c_str(), sGEOSStr.c_str(), sPrjStr.c_str(), sWXStr.c_str() );
    m_AuiNotebook->AddPage(new wxGISSimpleTextPanel(sAboutSys, m_AuiNotebook), _("About system"));

	m_AuiNotebook->AddPage(new wxGISSimpleTextPanel(_("Thanks GIS-LAB team for help in translating the interface, testing and searching for errors.\nSpecial thanks to Maxim Dubinin"), m_AuiNotebook), _("Thanks"));
	m_AuiNotebook->AddPage(new wxGISSimpleTextPanel(_("This program is free software: you can redistribute it and/or modify it under the terms of the GNU General Public License as published by the Free Software Foundation, either version 3 of the License, or (at your option) any later version.\nThis program is distributed in the hope that it will be useful, but WITHOUT ANY WARRANTY; without even the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.\n\nSee the GNU General Public License for more details.\nYou should have received a copy of the GNU General Public License along with this program. If not, see <http://www.gnu.org/licenses/>"), m_AuiNotebook), _("License"));//TODO: add here full localised license 

	bMainSizer->Add( m_AuiNotebook, 1, wxEXPAND | wxALL, 5 );

	m_sdbSizer = new wxStdDialogButtonSizer();
	m_sdbSizerOK = new wxButton( this, wxID_OK, wxString(_("Close")) );
	m_sdbSizer->AddButton( m_sdbSizerOK );
	m_sdbSizer->Realize();
	bMainSizer->Add( m_sdbSizer, 0, wxALL|wxEXPAND, 5 );
	
	this->SetSizer( bMainSizer );
	this->Layout();
	this->Centre( wxBOTH );
}

wxGISAboutDialog::~wxGISAboutDialog()
{
}

