/******************************************************************************
 * Project:  wxGIS (GIS Remote)
 * Purpose:  wxGISSearchServerDlg class.
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
#include "wxgis/remoteserverui/serversearchdlg.h"

#include "../../art/remoteservers_16.xpm"
#include "../../art/remoteserver_16.xpm"

//#include "img/main_icons_small.xpm"
//#include <wx/xml/xml.h>
//#include "network.h"
//#include "netmsg.h"
//
//
///////////////////////////////////////////////////////////////////////////
BEGIN_EVENT_TABLE( wxGISSearchServerDlg, wxDialog )
	EVT_BUTTON(ID_SEARCHBT, wxGISSearchServerDlg::OnSearch )
	EVT_BUTTON(ID_STOPBT, wxGISSearchServerDlg::OnStop )
	EVT_BUTTON(ID_ACCEPT, wxGISSearchServerDlg::OnAccept )
	EVT_UPDATE_UI(ID_SEARCHBT, wxGISSearchServerDlg::OnSearchUI )
	EVT_UPDATE_UI(ID_STOPBT, wxGISSearchServerDlg::OnStopUI )
	EVT_UPDATE_UI(ID_ACCEPT, wxGISSearchServerDlg::OnAcceptUI )
	EVT_CLOSE(wxGISSearchServerDlg::OnClose)
END_EVENT_TABLE()
///////////////////////////////////////////////////////////////////////////

wxGISSearchServerDlg::wxGISSearchServerDlg( wxWindow* parent, size_t port, wxWindowID id, const wxString& title, const wxPoint& pos, const wxSize& size, long style ) : wxDialog( parent, id, title, pos, size, style )/*, m_bContinueSearch(false)*/
{
	m_ImageList.Create(16, 16);
	m_ImageList.Add(wxBitmap(remoteserver_16_xpm));
    // set frame icon
	SetIcon(remoteservers_16_xpm);
	SetSize(size);

	this->SetSizeHints( wxSize( -1,-1 ), wxDefaultSize );
	
	wxBoxSizer* bSizer3;
	bSizer3 = new wxBoxSizer( wxVERTICAL );
	
	wxFlexGridSizer* fgSizer2;
	fgSizer2 = new wxFlexGridSizer( 2, 2, 0, 0 );
	fgSizer2->SetFlexibleDirection( wxBOTH );
	fgSizer2->SetNonFlexibleGrowMode( wxFLEX_GROWMODE_SPECIFIED );
	
	m_listCtrl = new wxListCtrl( this, wxID_ANY, wxDefaultPosition, wxDefaultSize, wxLC_REPORT | wxLC_SINGLE_SEL );
	m_listCtrl->SetMinSize( wxSize( 250,150 ) );
	m_listCtrl->SetImageList(&m_ImageList, wxIMAGE_LIST_SMALL);
	m_listCtrl->InsertColumn(0, _("Address"), wxLIST_FORMAT_LEFT, 120);
	m_listCtrl->InsertColumn(1, _("Name"), wxLIST_FORMAT_LEFT, 120);
	
	fgSizer2->Add( m_listCtrl, 1, wxALL|wxEXPAND, 5 );
	
	wxBoxSizer* bSizer4;
	bSizer4 = new wxBoxSizer( wxVERTICAL );
	
	m_button_search = new wxButton( this, ID_SEARCHBT, _("Search"), wxDefaultPosition, wxDefaultSize, 0 );
	bSizer4->Add( m_button_search, 0, wxALL, 5 );
	
	m_button_stop = new wxButton( this, ID_STOPBT, _("Stop"), wxDefaultPosition, wxDefaultSize, 0 );
	bSizer4->Add( m_button_stop, 0, wxALL, 5 );
	
	m_button_accept = new wxButton( this, ID_ACCEPT, _("Accept"), wxDefaultPosition, wxDefaultSize, 0 );
	bSizer4->Add( m_button_accept, 0, wxALL, 5 );
	
	fgSizer2->Add( bSizer4, 1, wxEXPAND, 5 );
	
	bSizer3->Add( fgSizer2, 1, wxEXPAND, 5 );
	
	m_gauge = new wxGauge( this, wxID_ANY, 254, wxDefaultPosition, wxDefaultSize, wxGA_HORIZONTAL|wxGA_SMOOTH );
	m_gauge->SetMinSize( wxSize( -1,15 ) );
	
	bSizer3->Add( m_gauge, 0, wxALL|wxEXPAND, 5 );
	
	this->SetSizer( bSizer3 );
	this->Layout();
	
	this->Centre( wxBOTH );
	wxUpdateUIEvent::SetUpdateInterval(10);
	m_port = port;
}

wxGISSearchServerDlg::~wxGISSearchServerDlg()
{
	//m_bContinueSearch = false;
}

void wxGISSearchServerDlg::OnClose(wxCloseEvent& event)
{
 //   // avoid if we can
 //   if (event.CanVeto() && m_bContinueSearch)
	//{
	//	wxMessageBox(_("You have to stop searching to close this dialog!"), _("Information"), wxOK | wxICON_EXCLAMATION );
 //       event.Veto();
	//}
 //   else
        Destroy();
}

void wxGISSearchServerDlg::OnStop( wxCommandEvent& event )
{ 
	wxBusyCursor wait;
	//m_bContinueSearch = false;
	event.Skip(); 
}

void wxGISSearchServerDlg::OnStopUI( wxUpdateUIEvent& event )
{ 
	//event.Enable(m_bContinueSearch);
	//event.Skip(); 
}

void wxGISSearchServerDlg::OnSearch( wxCommandEvent& event )
{ 
	//wxBusyCursor wait;
	//m_bContinueSearch = true;
	//UpdateWindowUI(wxUPDATE_UI_RECURSE);

	wxSocketBase::Initialize();
	//wxIPV4address myIp;
	//myIp.Hostname(wxGetHostName());
	//wxString ipAddr = myIp.IPAddress();



	//size_t pos = ipAddr.rfind(wxT("."));
	//if(pos == -1)
	//{
	//	m_bContinueSearch = false;
	//	return;
	//}
	//
	//ipAddr = ipAddr.Left(pos);
	//size_t server_count = 0;

	//wxString sAddr;
	//wxSocketClient* m_socarr[MAX_SOCKETS];
	//for(size_t i = 1; i < 255; i += MAX_SOCKETS)
	//{
	//  size_t count = 0;
	//  for(size_t j = i; j < i + MAX_SOCKETS; j++)
	//  {
	//	  if(j > 254)
	//		  continue;
	//	  sAddr = wxString::Format(wxT("%s.%d"), ipAddr.c_str(), j);
	//	  wxIPV4address addr;
	//	  addr.Hostname(sAddr);
	//	  addr.Service(m_port);
	//	  wxSocketClient *m_sock;
	//	  m_sock = new wxSocketClient();// | wxSOCKET_NOBINDwxSOCKET_BROADCAST
	//	  m_sock->SetTimeout(1);
	//	  m_sock->Connect(addr, false);
	//	  m_socarr[count] = m_sock;
	//	  count++;
	//  }

	//  for(size_t k = 0; k < MAX_SOCKETS; k++)
	//  {
	//	  m_gauge->SetValue(i + k);
	//	  if(m_socarr[k] != NULL)			   
	//	  {
	//		  m_socarr[k]->WaitOnConnect(0, TIMEOUT);
	//		  bool IsConnected = m_socarr[k]->IsConnected();
	//		  if(IsConnected)
	//		  {
	//			  if(m_socarr[k]->WaitForRead(2))
	//			  {
	//					wxUint8 sys_type;
	//					wxString smsg;
	//					wxUint32 size;
	//					wxChar pchar[500];
	//					m_socarr[k]->ReadMsg(&sys_type, sizeof(wxUint8));
	//					if(sys_type == CURROS)
	//					{
	//						m_socarr[k]->ReadMsg(pchar, 500 * sizeof(wxChar));
	//						smsg = wxString(pchar);
	//						size = m_socarr[k]->LastCount();
	//					}
	//					else if(CURROS == WIN)
	//					{
	//						wxUint32 ichar[500];
	//						m_socarr[k]->ReadMsg(ichar, 500 * sizeof(wxUint32));
	//						size = m_socarr[k]->LastCount();
	//						size_t real_count = size / sizeof(wxUint32);
	//						for(int i = 0; i < real_count; i++)
	//							pchar[i] = ichar[i];
	//						pchar[real_count] = 0;
	//						smsg = wxString(pchar);
	//					}
	//					else if(CURROS == LIN)
	//					{
	//						wxUint16 ichar[500];
	//						m_socarr[k]->ReadMsg(ichar, 500 * sizeof(wxUint16));
	//						size = m_socarr[k]->LastCount();
	//						size_t real_count = size / sizeof(wxUint16);
	//						for(int i = 0; i < real_count; i++)
	//							pchar[i] = ichar[i];
	//						pchar[real_count] = 0;
	//						smsg = wxString(pchar);
	//					}


	//				  weNetMsg msg(smsg);
	//				  if(msg.IsOk())
	//				  {
	//					  if(msg.GetState() == NETOK)
	//					  {
	//						  wxXmlNode *pChild = msg.GetRoot()->GetChildren();
	//						  while(pChild)
	//						  {
	//							  if(pChild->GetName() == wxT("server"))
	//							  {
	//								  wxString m_servername = pChild->GetPropVal(wxT("name"), wxT("Nemo"));

	//								  sAddr = wxString::Format(wxT("%s.%d"), ipAddr.c_str(), i + k);

	//								  long pos = m_listCtrl->InsertItem(server_count, sAddr, 3);
	//								  m_listCtrl->SetItem(pos, 1, m_servername);
	//								  server_count++;
	//							  }
	//							  pChild = pChild->GetNext();
	//						  }	
	//					  }
	//					  else
	//					  {
	//						  wxLogReceive(msg.GetMessage());
	//					  }
	//				  }
	//			  }
	//		  }
	//		  wxLogDebug(wxT("%d -- %s"), i + k, IsConnected == true ? wxT("true") : wxT("false"));
	//		  weDELETE(m_socarr[k], Destroy());
	//	  }
	//  }
	//  if(!m_bContinueSearch)
	//	  break;
	//}
	//m_bContinueSearch = false;

}

void wxGISSearchServerDlg::OnSearchUI( wxUpdateUIEvent& event )
{ 
	//event.Enable(!m_bContinueSearch);
	//event.Skip(); 
}

void wxGISSearchServerDlg::OnAccept( wxCommandEvent& event )
{
	long item = m_listCtrl->GetNextItem(-1, wxLIST_NEXT_ALL, wxLIST_STATE_SELECTED);
	wxListItem it;
	it.SetId(item);
	it.SetColumn(0);
	it.SetMask(wxLIST_MASK_TEXT);
	if(m_listCtrl->GetItem(it))
	{
		m_ipaddress = it.GetText();
		EndModal(ID_ACCEPT);
	}
	event.Skip(); 
}

void wxGISSearchServerDlg::OnAcceptUI( wxUpdateUIEvent& event )
{ 
	m_listCtrl->GetSelectedItemCount() == 1 ? event.Enable(true) : event.Enable(false);
}

