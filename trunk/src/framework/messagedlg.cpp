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

#include "wxgis/framework/messagedlg.h"
#include <wx/valgen.h>

BEGIN_EVENT_TABLE( wxGISMessageDlg, wxDialog )
	EVT_BUTTON( wxID_YES, wxGISMessageDlg::OnQuit )
	EVT_BUTTON( wxID_NO, wxGISMessageDlg::OnQuit )
END_EVENT_TABLE()

wxGISMessageDlg::wxGISMessageDlg( wxWindow* parent, wxWindowID id, const wxString& sTitle, const wxString& sMainText,const wxString& sDescriptText, const wxPoint& pos, const wxSize& size, long style ) : wxDialog( parent, id, sTitle, pos, size, style )
{
    m_bNotShowInFuture = false;
	this->SetSizeHints( wxDefaultSize, wxDefaultSize );
	//this->SetSizeHints( size, size );

	wxBoxSizer* bSizer1;
	bSizer1 = new wxBoxSizer( wxVERTICAL );

    if(!sMainText.IsEmpty())
    {
	    m_MainText = new wxStaticText( this, wxID_ANY, sMainText, wxDefaultPosition, wxDefaultSize, 0 );
	    m_MainText->Wrap( -1 );
	    bSizer1->Add( m_MainText, 1, wxALL|wxEXPAND, 5 );
    }

    if(!sDescriptText.IsEmpty())
    {
        m_DecriptText = new wxStaticText( this, wxID_ANY, sDescriptText, wxDefaultPosition, wxDefaultSize, wxALIGN_CENTRE );
        m_DecriptText->Wrap( -1 );
        bSizer1->Add( m_DecriptText, 1, wxALL|wxEXPAND, 5 );
    }

	m_sdbSizer1 = new wxStdDialogButtonSizer();
	m_sdbSizer1Yes = new wxButton( this, wxID_YES, _("Yes") );
	m_sdbSizer1->AddButton( m_sdbSizer1Yes );
	m_sdbSizer1No = new wxButton( this, wxID_NO, _("No") );
	m_sdbSizer1->AddButton( m_sdbSizer1No );
	m_sdbSizer1->Realize();
	bSizer1->Add( m_sdbSizer1, 0, wxEXPAND|wxALL, 5 );

	m_sdbSizer1Yes->SetFocus();

	m_checkBox = new wxCheckBox( this, wxID_ANY, _("Use my choice and do not show this dialog in future."), wxDefaultPosition, wxDefaultSize, 0, wxGenericValidator(&m_bNotShowInFuture) );

	bSizer1->Add( m_checkBox, 0, wxALL|wxEXPAND, 5 );

	this->SetSizer( bSizer1 );
	this->Layout();
}

wxGISMessageDlg::~wxGISMessageDlg()
{
}


void wxGISMessageDlg::OnQuit( wxCommandEvent& event )
{
    event.Skip();
    Validate();
    TransferDataFromWindow();
    EndModal(event.GetId());
}
