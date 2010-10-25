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
#include "wxgis/catalogui/gxpropertypages.h"

///////////////////////////////////////////////////////////////////////////////
/// Class wxGISCatalogGeneralPropertyPage
///////////////////////////////////////////////////////////////////////////////
IMPLEMENT_DYNAMIC_CLASS(wxGISCatalogGeneralPropertyPage, wxPanel)

wxGISCatalogGeneralPropertyPage::wxGISCatalogGeneralPropertyPage(void)
{
}

wxGISCatalogGeneralPropertyPage::~wxGISCatalogGeneralPropertyPage()
{
}

bool wxGISCatalogGeneralPropertyPage::Create(IApplication* application, wxWindow* parent, wxWindowID id, const wxPoint& pos, const wxSize& size, long style, const wxString& name)
{
    wxPanel::Create(parent, id, pos, size, style, name);

    IGxApplication* pGxApplication = dynamic_cast<IGxApplication*>(application);
    if(!pGxApplication)
        return false;
    IGxCatalog* pCatalog = pGxApplication->GetCatalog();
    if(!pCatalog)
        return false;

	wxBoxSizer* bMainSizer;
	bMainSizer = new wxBoxSizer( wxVERTICAL );
	
	m_staticText = new wxStaticText( this, wxID_ANY, _("What top level entries do you want the Catalog to contain?"), wxDefaultPosition, wxDefaultSize, 0 );
	m_staticText->Wrap( -1 );
	bMainSizer->Add( m_staticText, 0, wxALL, 5 );
	
	wxArrayString m_checkList1Choices;
	m_checkList1 = new wxCheckListBox( this, wxID_ANY, wxDefaultPosition, wxDefaultSize, m_checkList1Choices, 0 );
	bMainSizer->Add( m_checkList1, 1, wxALL|wxEXPAND, 5 );
	
	wxStaticBoxSizer* sbSizer = new wxStaticBoxSizer( new wxStaticBox( this, wxID_ANY, _("Which types of data do you want the Catalog to show?") ), wxVERTICAL );
	
	wxArrayString m_checkList2Choices;
	m_checkList2 = new wxCheckListBox( this, wxID_ANY, wxDefaultPosition, wxDefaultSize, m_checkList2Choices, 0 );
	sbSizer->Add( m_checkList2, 1, wxALL|wxEXPAND, 5 );
	
	wxBoxSizer* bSizer = new wxBoxSizer( wxHORIZONTAL );
	
	m_button2 = new wxButton( this, wxID_ANY, _("MyButton"), wxDefaultPosition, wxDefaultSize, 0 );
	bSizer->Add( m_button2, 0, wxALL|wxALIGN_RIGHT, 5 );
	
	m_button3 = new wxButton( this, wxID_ANY, _("MyButton"), wxDefaultPosition, wxDefaultSize, 0 );
	bSizer->Add( m_button3, 0, wxALL, 5 );
	
	sbSizer->Add( bSizer, 0, wxEXPAND, 5 );
	
	bMainSizer->Add( sbSizer, 1, wxEXPAND, 5 );
	
	m_checkBox1 = new wxCheckBox( this, wxID_ANY, _("Hide file extensions"), wxDefaultPosition, wxDefaultSize, 0 );
	
	bMainSizer->Add( m_checkBox1, 0, wxALL, 5 );
	
	m_checkBox2 = new wxCheckBox( this, wxID_ANY, _("Return to last location when wxGISCatalog start up"), wxDefaultPosition, wxDefaultSize, 0 );
	
	bMainSizer->Add( m_checkBox2, 0, wxALL, 5 );
	
	this->SetSizer( bMainSizer );
	this->Layout();

    return true;
}

void wxGISCatalogGeneralPropertyPage::Apply(void)
{
}
