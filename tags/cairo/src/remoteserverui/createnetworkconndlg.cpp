/******************************************************************************
 * Project:  wxGIS (GIS Remote)
 * Purpose:  Create network connection dialog
 * Author:   Bishop (aka Barishnikov Dmitriy), polimax@mail.ru
 ******************************************************************************
*   Copyright (C) 2008-2010 Bishop
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

#include "wxgis/remoteserverui/createnetworkconndlg.h"
#include "../../art/remoteserver_16.xpm"

///////////////////////////////////////////////////////////////////////////
BEGIN_EVENT_TABLE( wxGISCreateNetworkConnDlg, wxDialog )
	EVT_BUTTON( wxID_SAVE, wxGISCreateNetworkConnDlg::OnSaveBtnClick )
END_EVENT_TABLE()

wxGISCreateNetworkConnDlg::wxGISCreateNetworkConnDlg( NETCONNFACTORYARRAY& apNetConn, wxWindow* parent, wxWindowID id, const wxString& title, const wxPoint& pos, const wxSize& size, long style ) : wxDialog( parent, id, title, pos, size, style )
{
	this->SetSizeHints( wxDefaultSize, wxDefaultSize );
	
	bSizer = new wxBoxSizer( wxVERTICAL );
	
	m_choicebook = new wxChoicebook( this, wxID_ANY, wxDefaultPosition, wxDefaultSize, wxCHB_DEFAULT|wxCLIP_CHILDREN );
	m_choicebook->SetMinSize( wxSize( 300,300 ) );
	for(size_t i = 0; i < apNetConn.size(); ++i)
	{
		wxString sName = apNetConn[i]->GetName();
		INetConnFactoryUI* pConnFactUI = dynamic_cast<INetConnFactoryUI*>(apNetConn[i]);
        if(pConnFactUI)
            m_choicebook->AddPage(pConnFactUI->GetPropertyPage(m_choicebook), sName);
	}
	
	bSizer->Add( m_choicebook, 1, wxEXPAND|wxALL|wxALIGN_CENTER_HORIZONTAL, 5 );
	
	m_staticline1 = new wxStaticLine( this, wxID_ANY, wxDefaultPosition, wxDefaultSize, wxLI_HORIZONTAL );
	bSizer->Add( m_staticline1, 0, wxEXPAND | wxALL, 5 );

	m_sdbSizer = new wxStdDialogButtonSizer();
	m_sdbSizerSave = new wxButton( this, wxID_SAVE );
	m_sdbSizer->AddButton( m_sdbSizerSave );
	m_sdbSizerCancel = new wxButton( this, wxID_CANCEL );
	m_sdbSizer->AddButton( m_sdbSizerCancel );
	m_sdbSizer->Realize();
	bSizer->Add( m_sdbSizer, 0, wxALIGN_CENTER_HORIZONTAL|wxEXPAND|wxALL, 5 );
	
	this->SetSizer( bSizer );
	this->Layout();
	
	this->Centre( wxBOTH );

    // set icon
	SetIcon(remoteserver_16_xpm);
}

wxGISCreateNetworkConnDlg::~wxGISCreateNetworkConnDlg()
{
}

void wxGISCreateNetworkConnDlg::OnSaveBtnClick( wxCommandEvent& evt )
{
	wxNetPropertyPage* pPPage = static_cast<wxNetPropertyPage*>(m_choicebook->GetCurrentPage());
	if(pPPage != NULL)
	{
		m_pConnObj = pPPage->OnSave();
		if(m_pConnObj)
			EndModal(wxID_SAVE);
		else
		{
			
			wxMessageBox(wxString::Format(_("%s\nCreate network connection failed!"), pPPage->GetLastError().c_str()), _("Error"), wxICON_ERROR | wxOK );
			wxLogError(_("wxGISCreateNetworkConnDlg: Create network connection failed!"));
		}
	}
//	Close();
}
