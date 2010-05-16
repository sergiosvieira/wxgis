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

#include "wxgis/geoprocessingui/gptooldlg.h"

#include "../../art/tool_16.xpm"

#include "wx/icon.h"
///////////////////////////////////////////////////////////////////////////
BEGIN_EVENT_TABLE(wxGISGPToolDlg, wxDialog)
	EVT_BUTTON(wxID_HELP, wxGISGPToolDlg::OnHelp)
	EVT_UPDATE_UI(wxID_HELP, wxGISGPToolDlg::OnHelpUI)
END_EVENT_TABLE()

wxGISGPToolDlg::wxGISGPToolDlg( wxWindow* parent, wxWindowID id, const wxString& title, const wxPoint& pos, const wxSize& size, long style ) : wxDialog( parent, id, title, pos, size, style )
{
	SetIcon( wxIcon(tool_16_xpm) );
	this->SetSizeHints( wxSize( 350,500 ) );

	//this->SetSizeHints( wxDefaultSize, wxDefaultSize );
    m_DataWidth = 350;
    m_HtmlWidth = 150;
	
	wxBoxSizer* bMainSizer = new wxBoxSizer( wxVERTICAL );
	
	m_splitter = new wxSplitterWindow( this, SASHCTRLID, wxDefaultPosition, wxDefaultSize, 0 );
	m_splitter->Connect( wxEVT_IDLE, wxIdleEventHandler( wxGISGPToolDlg::m_splitterOnIdle ), NULL, this );

	m_toolpanel = new wxPanel( m_splitter, wxID_ANY, wxDefaultPosition, wxSize(m_DataWidth, size.y)/*wxDefaultSize*/, wxTAB_TRAVERSAL );
	wxBoxSizer* bSizer2 = new wxBoxSizer( wxVERTICAL );
	
	wxBoxSizer* bSizer4;
	bSizer4 = new wxBoxSizer( wxVERTICAL );
	
	bSizer2->Add( bSizer4, 1, wxEXPAND, 5 );
	
	m_sdbSizer1 = new wxStdDialogButtonSizer();
	m_sdbSizer1OK = new wxButton( m_toolpanel, wxID_OK, wxString(_("OK")) );
	m_sdbSizer1->AddButton( m_sdbSizer1OK );
	m_sdbSizer1Cancel = new wxButton( m_toolpanel, wxID_CANCEL, wxString(_("Cancel")) );
	m_sdbSizer1->AddButton( m_sdbSizer1Cancel );
	m_sdbSizer1Help = new wxButton( m_toolpanel, wxID_HELP, wxString(_("Show Help >>")) );
    m_sdbSizer1Help->Enable(false);
	m_sdbSizer1->AddButton( m_sdbSizer1Help );
	m_sdbSizer1->Realize();
	bSizer2->Add( m_sdbSizer1, 0, wxEXPAND|wxALL, 5 );
	
	m_toolpanel->SetSizer( bSizer2 );
	m_toolpanel->Layout();
	bSizer2->Fit( m_toolpanel );

	//m_helppanel = new wxPanel( m_splitter, wxID_ANY, wxDefaultPosition, wxDefaultSize, wxTAB_TRAVERSAL );
	//wxBoxSizer* bSizer3;
	//bSizer3 = new wxBoxSizer( wxVERTICAL );
	
	m_htmlWin = new wxHtmlWindow( m_splitter/*m_helppanel*/, wxID_ANY, wxDefaultPosition, wxSize(m_HtmlWidth, size.y) /*wxDefaultSize*/, wxHW_SCROLLBAR_AUTO | wxBORDER_THEME  );
	//bSizer3->Add( m_htmlWin2, 1, wxEXPAND, 5 );
	
	//m_helppanel->SetSizer( bSizer3 );
	//m_helppanel->Layout();
	//bSizer3->Fit( m_helppanel );

	m_splitter->SetSashGravity(1.0);
	//m_splitter1->SplitVertically(m_commandbarlist, m_buttonslist, 100);


	m_splitter->SplitVertically( m_toolpanel, m_htmlWin/*m_helppanel*/, m_DataWidth );

	bMainSizer->Add( m_splitter, 1, wxEXPAND, 5 );
	
	this->SetSizer( bMainSizer );
	this->Layout();


    m_splitter->Unsplit(m_htmlWin);
    wxSize DlgSize = size;
    DlgSize.x = m_DataWidth;
    SetSize(DlgSize);
}

wxGISGPToolDlg::~wxGISGPToolDlg()
{
}

void wxGISGPToolDlg::OnHelp(wxCommandEvent& event)
{
    wxSize DlgSize = GetSize();
    wxSize DataSize = m_toolpanel->GetSize();
    if(m_splitter->IsSplit())
    {
        wxSize HtmlSize = m_htmlWin->GetSize();
        m_HtmlWidth = HtmlSize.x;
        DlgSize.x = DataSize.x;
        SetSize(DlgSize);
        m_splitter->Unsplit(m_htmlWin);
    }
    else
    {
        DlgSize.x += m_HtmlWidth;
        SetSize(DlgSize);
        m_splitter->SetSashGravity(1.0);
        m_splitter->SplitVertically( m_toolpanel, m_htmlWin, DataSize.x);
    }
}

void wxGISGPToolDlg::OnHelpUI(wxUpdateUIEvent& event)
{
    event.SetText(m_splitter->IsSplit() == true ? _("Hide Help <<") : _("Show Help >>"));
}


