/******************************************************************************
 * Project:  wxGIS (GIS Catalog)
 * Purpose:  wxGISIdentifyDlg class - dialog/dock window with the results of identify.
 * Author:   Bishop (aka Barishnikov Dmitriy), polimax@mail.ru
 ******************************************************************************
*   Copyright (C) 2011 Bishop
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
#include "wxgis/cartoui/identifydlg.h"

///////////////////////////////////////////////////////////////////////////
// wxGISIdentifyDlg
///////////////////////////////////////////////////////////////////////////

wxGISIdentifyDlg::wxGISIdentifyDlg( wxWindow* parent, wxWindowID id, const wxPoint& pos, const wxSize& size, long style ) : wxPanel( parent, id, pos, size, style )
{
	m_bMainSizer = new wxBoxSizer( wxVERTICAL );
	
	m_fgTopSizer = new wxFlexGridSizer( 1, 3, 0, 0 );
	m_fgTopSizer->AddGrowableCol( 1 );
	m_fgTopSizer->SetFlexibleDirection( wxBOTH );
	m_fgTopSizer->SetNonFlexibleGrowMode( wxFLEX_GROWMODE_SPECIFIED );
	
	m_staticText1 = new wxStaticText( this, wxID_ANY, _("Identify from:"), wxDefaultPosition, wxDefaultSize, 0 );
	m_staticText1->Wrap( -1 );
	m_fgTopSizer->Add( m_staticText1, 0, wxALL|wxALIGN_RIGHT|wxALIGN_CENTER_VERTICAL, 5 );
	
	wxArrayString m_LayerChoiceChoices;
	m_LayerChoice = new wxChoice( this, wxID_ANY, wxDefaultPosition, wxDefaultSize, m_LayerChoiceChoices, 0 );
	m_LayerChoice->SetSelection( 0 );
	fgTopSizer->Add( m_LayerChoice, 0, wxALL|wxALIGN_CENTER_VERTICAL|wxEXPAND, 5 );
	
	m_bpSplitButton = new wxBitmapButton( this, wxID_ANY, wxNullBitmap, wxDefaultPosition, wxDefaultSize, wxBU_AUTODRAW );
	m_fgTopSizer->Add( m_bpSplitButton, 0, wxALL|wxALIGN_CENTER_VERTICAL, 5 );
	
	m_bMainSizer->Add( m_fgTopSizer, 0, wxEXPAND, 5 );
	
	m_splitter = new wxSplitterWindow( this, wxID_ANY, wxDefaultPosition, wxDefaultSize, wxSP_3D );
	m_splitter->Connect( wxEVT_IDLE, wxIdleEventHandler( wxGISIdentifyDlg::SplitterOnIdle ), NULL, this );
	
	m_bMainSizer->Add( m_splitter, 1, wxEXPAND, 5 );
	
	this->SetSizer( m_bMainSizer );
	this->Layout();
}

wxGISIdentifyDlg::~wxGISIdentifyDlg()
{
}
