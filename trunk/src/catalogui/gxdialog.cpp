/******************************************************************************
 * Project:  wxGIS (GIS Catalog)
 * Purpose:  wxGxDialog class.
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

#include "wxgis/catalogui/gxdialog.h"


wxGxDialog::wxGxDialog( wxWindow* parent, wxWindowID id, const wxString& title, const wxPoint& pos, const wxSize& size, long style ) : wxDialog( parent, id, title, pos, size, style )
{
	this->SetSizeHints( wxSize( 400,300 ), wxDefaultSize );
	
	bMainSizer = new wxBoxSizer( wxVERTICAL );
	
	bHeaderSizer = new wxBoxSizer( wxHORIZONTAL );
	
	m_staticText1 = new wxStaticText( this, wxID_ANY, _("Look in:"), wxDefaultPosition, wxDefaultSize, wxALIGN_RIGHT );
	m_staticText1->Wrap( -1 );
	bHeaderSizer->Add( m_staticText1, 0, wxALL|wxALIGN_CENTER_VERTICAL, 5 );
	
	m_treecombo = new wxComboBox( this, wxID_ANY, _("Combo!"), wxDefaultPosition, wxDefaultSize, 0, NULL, wxCB_READONLY ); 
	bHeaderSizer->Add( m_treecombo, 1, wxALL|wxALIGN_CENTER_VERTICAL, 5 );
	
	m_toolBar = new wxToolBar( this, wxID_ANY, wxDefaultPosition, wxDefaultSize, wxTB_FLAT|wxTB_HORIZONTAL|wxTB_NODIVIDER );
	m_toolBar->SetToolBitmapSize( wxSize( 16,16 ) );
	//m_toolBar->AddTool( wxID_ANY, _("tool"), wxNullBitmap, wxNullBitmap, wxITEM_NORMAL, wxEmptyString, wxEmptyString );
	//m_toolBar->AddTool( wxID_ANY, _("tool"), wxNullBitmap, wxNullBitmap, wxITEM_NORMAL, wxEmptyString, wxEmptyString );
	//m_toolBar->AddTool( wxID_ANY, _("tool"), wxNullBitmap, wxNullBitmap, wxITEM_NORMAL, wxEmptyString, wxEmptyString );
	//m_toolBar->AddTool( wxID_ANY, _("tool"), wxNullBitmap, wxNullBitmap, wxITEM_NORMAL, wxEmptyString, wxEmptyString );
	//m_toolBar->Realize();
	
	bHeaderSizer->Add( m_toolBar, 0, wxALIGN_CENTER_VERTICAL, 5 );
	
	bMainSizer->Add( bHeaderSizer, 0, wxEXPAND, 5 );
	
	m_listCtrl = new wxListCtrl( this, wxID_ANY, wxDefaultPosition, wxDefaultSize, wxLC_REPORT );
	bMainSizer->Add( m_listCtrl, 1, wxALL|wxEXPAND, 5 );
	
	fgCeilSizer = new wxFlexGridSizer( 2, 3, 0, 0 );
	fgCeilSizer->AddGrowableCol( 1 );
	fgCeilSizer->SetFlexibleDirection( wxBOTH );
	fgCeilSizer->SetNonFlexibleGrowMode( wxFLEX_GROWMODE_SPECIFIED );
	
	m_staticText4 = new wxStaticText( this, wxID_ANY, _("Name:"), wxDefaultPosition, wxDefaultSize, wxALIGN_RIGHT );
	m_staticText4->Wrap( -1 );
	fgCeilSizer->Add( m_staticText4, 0, wxALL|wxALIGN_CENTER_VERTICAL|wxALIGN_RIGHT, 5 );
	
	m_NameTextCtrl = new wxTextCtrl( this, wxID_ANY, wxEmptyString, wxDefaultPosition, wxDefaultSize, wxTE_NO_VSCROLL );
	fgCeilSizer->Add( m_NameTextCtrl, 1, wxALL|wxALIGN_CENTER_VERTICAL|wxEXPAND, 5 );
	
	m_OkButton = new wxButton( this, wxID_ANY, _("Activate"), wxDefaultPosition, wxDefaultSize, 0 );
	m_OkButton->SetDefault(); 
	fgCeilSizer->Add( m_OkButton, 0, wxALL|wxALIGN_CENTER_VERTICAL, 5 );
	
	m_staticText6 = new wxStaticText( this, wxID_ANY, _("Show of type:"), wxDefaultPosition, wxDefaultSize, wxALIGN_RIGHT );
	m_staticText6->Wrap( -1 );
	fgCeilSizer->Add( m_staticText6, 0, wxALL|wxALIGN_RIGHT, 5 );
	
	m_comboBox2 = new wxComboBox( this, wxID_ANY, _("Combo!"), wxDefaultPosition, wxDefaultSize, 0, NULL, wxCB_DROPDOWN|wxCB_READONLY|wxCB_SORT ); 
	fgCeilSizer->Add( m_comboBox2, 0, wxALL|wxEXPAND, 5 );
	
	m_CancelButton = new wxButton( this, wxID_ANY, _("Cancel"), wxDefaultPosition, wxDefaultSize, 0 );
	fgCeilSizer->Add( m_CancelButton, 0, wxALL, 5 );
	
	bMainSizer->Add( fgCeilSizer, 0, wxEXPAND, 5 );
	
	this->SetSizer( bMainSizer );
	this->Layout();
}

wxGxDialog::~wxGxDialog()
{
}
