/******************************************************************************
 * Project:  wxGIS (GIS Server)
 * Purpose:  TCP network server class.
 * Author:   Bishop (aka Barishnikov Dmitriy), polimax@mail.ru
 ******************************************************************************
*   Copyright (C) 2008-2010  Bishop
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
#include "wxgissrv/framework/tcpnetworkplugin.h"
#include "wxgissrv/framework/message.h"

// ----------------------------------------------------------------------------
// wxTCPConnectThread
// ----------------------------------------------------------------------------

wxTCPConnectThread::wxTCPConnectThread(wxTCPNetworkPlugin* pParent) : wxThread(), m_pServer(NULL), m_pParent(NULL)
{
    m_pParent = pParent;
	// Create the address - defaults to localhost:0 initially
	wxIPV4address ip_addr;
	ip_addr.Service(m_pParent->GetPort());
	bool bIsAddrSet = false;
    wxString sAddr = m_pParent->GetAddres();
    if(sAddr.IsEmpty())
		bIsAddrSet = ip_addr.AnyAddress();
	else
		bIsAddrSet = ip_addr.Hostname(sAddr);
	if(!bIsAddrSet)
	{
		wxLogError(_("wxTCPNetworkPlugin: Invalid address - %s"), sAddr.c_str());
		return;
	}

	// Create the socket
	m_pServer = new wxSocketServer( ip_addr, wxSOCKET_WAITALL | wxSOCKET_BLOCK );
	
	// We use Ok() here to see if the server is really listening
	if (!m_pServer->Ok())
	{
		wxLogError(_("wxTCPNetworkPlugin: Could not listen at the specified port! Port number - %d"), m_pParent->GetPort());
		return;
	}
}

void wxTCPConnectThread::OnExit()
{
	m_pParent = NULL;
	wxDELETE(m_pServer);
}

void *wxTCPConnectThread::Entry()
{
	if(m_pServer == NULL || m_pParent == NULL)
		return (ExitCode)-1;

	while(!TestDestroy())
	{
		if(m_pServer->WaitForAccept(2))
		{
			wxSocketBase* pNewSocket = m_pServer->Accept(true);
			if (pNewSocket)
			{
				if(m_pParent->CanAcceptConnection())
				{
					pNewSocket->SetFlags(wxSOCKET_WAITALL);
					wxIPV4address cliend_addr;
					pNewSocket->GetPeer(cliend_addr);
					wxLogMessage(_("wxTCPNetworkPlugin: New client connection accepted. Address - %s"), cliend_addr.IPAddress().c_str());

                    wxNetMessage Msg(-1, enumGISMsgStOk, enumGISPriorityHightest, _("Connection accepted"));
                    wxXmlNode* pRootNode = Msg.GetRoot();
                    wxXmlNode* pNode = new wxXmlNode(pRootNode, wxXML_ELEMENT_NODE, wxT("server"));
                    pNode->AddProperty(wxT("name"), m_pParent->GetServerName().c_str());
                    wxString sData = Msg.GetData();

					//wxString servername = wxString::Format(wxT("<server name=\"%s\"/>"), m_pParent->m_servername.c_str());
					//wxString data = wxString::Format(WEMESSAGE, NETVER, NETOK, _("Connection accepted"), HIGH_PRIORITY, servername.c_str());
					
					wxUint8 nSysType = CURROS;
					pNewSocket->WriteMsg(&nSysType, sizeof(wxUint8));
					pNewSocket->WriteMsg(sData.c_str(), (sData.Len() + 1) * sizeof(wxChar));

                    if(!m_pParent->AddConnection(pNewSocket))
                        wxDELETE(pNewSocket);
				}
				else
				{
					wxLogMessage(_("wxTCPNetworkPlugin: To many connections! Connection is not established"));
                    wxNetMessage Msg(-1, enumGISMsgStRefuse, enumGISPriorityHightest, _("To many connections! Connection refused"));
                    wxString sData = Msg.GetData();

					wxUint8 nSysType = CURROS;
					pNewSocket->WriteMsg(&nSysType, sizeof(wxUint8));
					pNewSocket->WriteMsg(sData.c_str(), (sData.Len() + 1) * sizeof(wxChar));
                    wxDELETE(pNewSocket);

		//			//<?xml version="1.0" encoding="UTF-8"?><weMsg version="1" state="REFUSE" message="To many connections"></weMsg>
		//			wxString data = wxString::Format(WEMESSAGE, NETVER, NETREFUSE, _("To many connections!"), HIGH_PRIORITY, wxT(""));
		////			sock->SetFlags(wxSOCKET_WAITALL);
		////			sock->WriteMsg(data.GetData(), sizeof(data));
		//			wxUint8 sys_type = CURROS;
		//			pNewSocket->WriteMsg(&sys_type, sizeof(wxUint8));
		//			pNewSocket->WriteMsg(data.c_str(), (data.Len() + 1) * sizeof(wxChar));
		//			pNewSocket->Close();
		//			pNewSocket = NULL;
				}
			}
			else
				wxLogError(_("wxTCPNetworkPlugin: Couldn't accept a new connection"));
		}
		wxThread::Sleep(10);
	}
    return NULL;
}


// ----------------------------------------------------------------------------
// wxTCPNetworkPlugin
// ----------------------------------------------------------------------------

IMPLEMENT_DYNAMIC_CLASS(wxTCPNetworkPlugin, wxObject)

wxTCPNetworkPlugin::wxTCPNetworkPlugin(void) : m_nPort(1976), m_pWaitThread(NULL)
{
}

wxTCPNetworkPlugin::~wxTCPNetworkPlugin(void)
{
}

bool wxTCPNetworkPlugin::Start(IServerApplication* pApp, wxXmlNode* pConfig)
{
    m_pConfig = pConfig;
    m_pApp = pApp;

	m_sAddr = pConfig->GetPropVal(wxT("addr"), wxT(""));
	m_nPort = wxAtoi(pConfig->GetPropVal(wxT("port"), wxT("1976")));

	//Start Connect Thread
	m_pWaitThread = new wxTCPConnectThread(this);
	if(!CreateAndRunThread(m_pWaitThread, wxT("wxTCPNetworkPlugin"), _("Wait Connect")))
		return false;
    wxLogMessage(_("wxTCPNetworkPlugin: Wait Connect Thread 0x%lx started (priority = %u)."), m_pWaitThread->GetId(), m_pWaitThread->GetPriority());

    wxLogMessage(_("wxTCPNetworkPlugin: Plugin is started..."));

	return true;

}

bool wxTCPNetworkPlugin::Stop(void)
{
	m_pWaitThread->Delete();

	wxLogMessage(_("wxTCPNetworkPlugin: Plugin is shutdown..."));
	return true;
}

bool wxTCPNetworkPlugin::AddConnection(wxSocketBase* pNewSocket)
{
    long nUserID = m_pApp->GetUserID();
    //1. Create lost thread

    //2. create write / read thread
					//weTCPWaitLostThread* pNewLostThread = new weTCPWaitLostThread(m_pParent, pNewSocket);
					////Start weTCPWaitLostThread
					//if(pNewLostThread->Create() != wxTHREAD_NO_ERROR)
					//{
					//	wxLogError(_("TCPNetworkServerPlugin: Can't create weTCPWaitLostThread thread!"));
					//	pNewSocket->Close();
					//}
					//if(pNewLostThread->Run() != wxTHREAD_NO_ERROR)
					//{
					//	wxLogError(_("TCPNetworkServerPlugin: Can't run weTCPWaitLostThread thread!"));
					//	pNewSocket->Close();
					//}
    return true;
}

void wxTCPNetworkPlugin::RemoveConnection(long nID)
{
    m_pApp->RemoveUserID(nID);
}
