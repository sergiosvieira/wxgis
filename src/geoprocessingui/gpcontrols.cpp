/******************************************************************************
 * Project:  wxGIS (GIS Catalog)
 * Purpose:  controls classes.
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
#include "wxgis/geoprocessingui/gpcontrols.h"

#include "../../art/state_16.xpm"
#include "../../art/open_16.xpm"
#include "../../art/sql_16.xpm"

///////////////////////////////////////////////////////////////////////////////
/// Class wxGISDTBase
///////////////////////////////////////////////////////////////////////////////
wxGISDTBase::wxGISDTBase( IGPParameter* pParam, wxWindow* parent, wxWindowID id, const wxPoint& pos, const wxSize& size, long style ) : wxPanel( parent, id, pos, size, style )
{
    m_pParam = pParam;

	m_ImageList.Create(16, 16);
	m_ImageList.Add(wxBitmap(state_16_xpm));
}

wxGISDTBase::~wxGISDTBase()
{
}


///////////////////////////////////////////////////////////////////////////////
/// Class wxGISDTPath
///////////////////////////////////////////////////////////////////////////////

BEGIN_EVENT_TABLE(wxGISDTPath, wxPanel)
	EVT_BUTTON(wxID_OPEN, wxGISDTPath::OnOpen)
END_EVENT_TABLE()

wxGISDTPath::wxGISDTPath( IGPParameter* pParam, wxWindow* parent, wxWindowID id, const wxPoint& pos, const wxSize& size, long style ) : wxGISDTBase( pParam, parent, id, pos, size, style )
{
	wxFlexGridSizer* fgSizer1;
	fgSizer1 = new wxFlexGridSizer( 2, 2, 0, 0 );
	fgSizer1->AddGrowableCol( 1 );
	fgSizer1->SetFlexibleDirection( wxBOTH );
	fgSizer1->SetNonFlexibleGrowMode( wxFLEX_GROWMODE_SPECIFIED );
	
    m_StateBitmap = new wxStaticBitmap( this, wxID_ANY, m_pParam->GetParameterType() == enumGISGPParameterTypeRequired ? m_ImageList.GetIcon(4) : wxNullBitmap , wxDefaultPosition, wxDefaultSize, 0 );
    //m_StateBitmap->SetToolTip( wxT("some tip") );
	fgSizer1->Add( m_StateBitmap, 0, wxALL, 5 );
	
    m_sParamDisplayName = new wxStaticText( this, wxID_ANY, m_pParam->GetParameterType() == enumGISGPParameterTypeOptional ? m_pParam->GetDisplayName() + _(" (optional)") : m_pParam->GetDisplayName(), wxDefaultPosition, wxDefaultSize, 0 );
	m_sParamDisplayName->Wrap( -1 );
	fgSizer1->Add( m_sParamDisplayName, 1, wxALL|wxEXPAND, 5 );
	
	m_bitmap = new wxStaticBitmap( this, wxID_ANY, wxNullBitmap, wxDefaultPosition, wxDefaultSize, 0 );
	fgSizer1->Add( m_bitmap, 0, wxALL, 5 );
	
	wxBoxSizer* bPathSizer;
	bPathSizer = new wxBoxSizer( wxHORIZONTAL );
	
	m_PathTextCtrl = new wxTextCtrl( this, wxID_ANY, wxEmptyString, wxDefaultPosition, wxDefaultSize, 0 );
	bPathSizer->Add( m_PathTextCtrl, 1, wxALL|wxEXPAND, 5 );
	
	m_bpButton = new wxBitmapButton( this, wxID_OPEN, wxBitmap(open_16_xpm), wxDefaultPosition, wxDefaultSize, wxBU_AUTODRAW );
	//m_bpButton = new wxBitmapButton( this, wxID_ANY, wxBitmap(sql_16_xpm), wxDefaultPosition, wxDefaultSize, wxBU_AUTODRAW );
	bPathSizer->Add( m_bpButton, 0, wxALL, 5 );
	fgSizer1->Add( bPathSizer, 0, wxALL|wxEXPAND, 5 );

	this->SetSizer( fgSizer1 );
	this->Layout();
}

wxGISDTPath::~wxGISDTPath()
{
}

void wxGISDTPath::OnOpen(wxCommandEvent& event)
{
    //m_pParam
}

///////////////////////////////////////////////////////////////////////////////
/// Class wxGISDTChoice
///////////////////////////////////////////////////////////////////////////////

wxGISDTChoice::wxGISDTChoice( wxWindow* parent, wxWindowID id, const wxPoint& pos, const wxSize& size, long style ) : wxPanel( parent, id, pos, size, style )
{
	wxFlexGridSizer* fgSizer1;
	fgSizer1 = new wxFlexGridSizer( 2, 2, 0, 0 );
	fgSizer1->AddGrowableCol( 1 );
	fgSizer1->SetFlexibleDirection( wxBOTH );
	fgSizer1->SetNonFlexibleGrowMode( wxFLEX_GROWMODE_SPECIFIED );
	
	m_bitmap1 = new wxStaticBitmap( this, wxID_ANY, wxNullBitmap, wxDefaultPosition, wxDefaultSize, 0 );
    m_bitmap1->SetToolTip( wxT("some tip") );
	fgSizer1->Add( m_bitmap1, 0, wxALL, 5 );
	
	m_staticText1 = new wxStaticText( this, wxID_ANY, wxT("MyLabel"), wxDefaultPosition, wxDefaultSize, 0 );
	m_staticText1->Wrap( -1 );
	fgSizer1->Add( m_staticText1, 1, wxALL|wxEXPAND, 5 );
	
	m_bitmap2 = new wxStaticBitmap( this, wxID_ANY, wxNullBitmap, wxDefaultPosition, wxDefaultSize, 0 );
	fgSizer1->Add( m_bitmap2, 0, wxALL, 5 );
	
	wxArrayString m_choice1Choices;
	m_choice1 = new wxChoice( this, wxID_ANY, wxDefaultPosition, wxDefaultSize, m_choice1Choices, 0 );
	m_choice1->SetSelection( 0 );
	fgSizer1->Add( m_choice1, 0, wxALL|wxEXPAND, 5 );
	
	this->SetSizer( fgSizer1 );
	this->Layout();
}

wxGISDTChoice::~wxGISDTChoice()
{
}
