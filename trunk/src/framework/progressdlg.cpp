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
#include "wxgis/framework/progressdlg.h"
#include "wxgis/framework/progressor.h"

BEGIN_EVENT_TABLE( wxGISProgressDlg, wxDialog )
	EVT_BUTTON( wxID_CANCEL, wxGISProgressDlg::OnCancel )
END_EVENT_TABLE()


wxGISProgressDlg::wxGISProgressDlg( wxWindow* parent, wxWindowID id, const wxString& title, const wxPoint& pos, const wxSize& size, long style ) : wxDialog( parent, id, title, pos, size, style ), m_pProgressBar1(NULL), m_pProgressBar2(NULL)
{
	this->SetSizeHints( wxDefaultSize, wxDefaultSize );
	
	wxBoxSizer* bSizer1;
	bSizer1 = new wxBoxSizer( wxVERTICAL );
	
	m_staticText1 = new wxStaticText( this, wxID_ANY, wxEmptyString, wxDefaultPosition, wxDefaultSize, 0 );
	m_staticText1->Wrap( -1 );
	bSizer1->Add( m_staticText1, 1, wxALL|wxEXPAND, 5 );

    wxGISProgressor* pProgressBar1 = new wxGISProgressor(this, wxID_ANY);
	bSizer1->Add( pProgressBar1, 1, wxALL|wxEXPAND, 5 );
    m_pProgressBar1 = static_cast<IProgressor*>(pProgressBar1);

	//m_gauge1 = new wxGauge( this, wxID_ANY, 100, wxDefaultPosition, wxDefaultSize, wxGA_HORIZONTAL );
	//bSizer1->Add( m_gauge1, 0, wxALL|wxEXPAND, 5 );
	
	m_staticText2 = new wxStaticText( this, wxID_ANY, wxEmptyString, wxDefaultPosition, wxDefaultSize, 0 );
	m_staticText2->Wrap( -1 );
	bSizer1->Add( m_staticText2, 1, wxALL|wxEXPAND, 5 );
	
    wxGISProgressor* pProgressBar2 = new wxGISProgressor(this, wxID_ANY);
	bSizer1->Add( pProgressBar2, 1, wxALL|wxEXPAND, 5 );
    m_pProgressBar2 = static_cast<IProgressor*>(pProgressBar2);

	//m_gauge2 = new wxGauge( this, wxID_ANY, 100, wxDefaultPosition, wxDefaultSize, wxGA_HORIZONTAL );
	//bSizer1->Add( m_gauge2, 0, wxALL|wxEXPAND, 5 );
	
	m_sdbSizer1 = new wxStdDialogButtonSizer();
	//m_sdbSizer1OK = new wxButton( this, wxID_OK );
	//m_sdbSizer1->AddButton( m_sdbSizer1OK );
	m_sdbSizer1Cancel = new wxButton( this, wxID_CANCEL, _("Cancel"));
	m_sdbSizer1->AddButton( m_sdbSizer1Cancel );
	m_sdbSizer1->Realize();
	bSizer1->Add( m_sdbSizer1, 1, wxEXPAND|wxALL, 5 );
	
	this->SetSizer( bSizer1 );
	this->Layout();
	
	this->Centre( wxBOTH );
}

wxGISProgressDlg::~wxGISProgressDlg()
{
}

void wxGISProgressDlg::OnCancel( wxCommandEvent& event )
{ 
    event.Skip(); 
    if(m_pTrackCancel)
        m_pTrackCancel->Cancel();
}

void wxGISProgressDlg::SetText1(wxString sNewText)
{
    if(m_staticText1) m_staticText1->SetLabel(sNewText);
}

void wxGISProgressDlg::SetText2(wxString sNewText)
{
    if(m_staticText2) m_staticText2->SetLabel(sNewText);
}
