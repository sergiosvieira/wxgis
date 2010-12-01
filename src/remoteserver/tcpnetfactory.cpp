/******************************************************************************
 * Project:  wxGIS (GIS Remote)
 * Purpose:  TCP network classes.
 * Author:   Bishop (aka Barishnikov Dmitriy), polimax@mail.ru
 ******************************************************************************
*   Copyright (C) 2010 Bishop
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

#include "wxgis/remoteserver/tcpnetfactory.h"
#include "wxgis/networking/message.h"

//////////////////////////////////////////////////////////////////////
//// wxClientUDPNotifier
//////////////////////////////////////////////////////////////////////

wxClientUDPNotifier::wxClientUDPNotifier(wxClientTCPNetFactory* pFactory) : wxThread()
{
	m_pFactory = pFactory;
	if(!pFactory)
		return;

	wxIPV4address LocalAddress; // For the listening 
	LocalAddress.Service(m_pFactory->GetAdvPort() + 2); // port on which we listen for the answers 

	bool bIsAddrSet = LocalAddress.AnyAddress(); 
	if(!bIsAddrSet)
	{
		wxLogError(_("wxClientUDPNotifier: Invalid address"));
		return;
	}

	m_socket = new wxDatagramSocket(LocalAddress, wxSOCKET_REUSEADDR | wxSOCKET_NOWAIT); 
	const int optval = 1; 
	m_socket->SetOption(SOL_SOCKET, SO_BROADCAST, &optval, sizeof(optval)); 
	//m_socket->SetOption(SOL_SOCKET, SO_KEEPALIVE, &optval, sizeof(optval)); 
	//m_socket->SetOption(SOL_SOCKET, SO_REUSEADDR, &optval, sizeof(optval)); 
}

void *wxClientUDPNotifier::Entry()
{
	unsigned char buff[BUFF] = {0};

	//start loop
	while(!TestDestroy())
	{
		wxIPV4address BroadCastAddress; // For broadcast sending 
		BroadCastAddress.Service(m_pFactory->GetAdvPort() + 2);
		//WaitForRead
		if(m_socket->WaitForRead(0, 100))
		{
			m_socket->RecvFrom(BroadCastAddress, &buff, BUFF); 
			if(m_socket->Error())
			{
				//wxYieldIfNeeded();
				wxThread::Sleep(10);
				continue;
			}
			size_t nSize = m_socket->LastCount();
			wxNetMessage msg(buff, nSize);
			if(!msg.IsOk())
			{
				//wxYieldIfNeeded();
				wxThread::Sleep(10);
				continue;
			}
			if(msg.GetState() == enumGISMsgStOk)
			{
				const wxXmlNode* pRootNode = msg.GetRoot();
				wxXmlNode* pChild = pRootNode->GetChildren();
				while(pChild)
				{
					if(pChild->GetName().CmpNoCase(wxT("srv_info")) == 0)
					{
						INetSearchCallback* pCallback = m_pFactory->GetCallback();
						if(pCallback)
						{
							wxString sPort = pChild->GetPropVal(wxT("port"), wxT("1976"));
							wxString sName = pChild->GetPropVal(wxT("name"), NONAME);
							wxString sBanner = pChild->GetPropVal(wxT("banner"), wxEmptyString);
							wxString sHost = BroadCastAddress.Hostname();
							wxString sIP = BroadCastAddress.IPAddress();
							//Add to list
							wxXmlNode* pOutNode = new wxXmlNode(wxXML_ELEMENT_NODE, wxT("conn_info"));
							pOutNode->AddProperty(wxT("port"), sPort);
							pOutNode->AddProperty(wxT("name"), sName);
							pOutNode->AddProperty(wxT("banner"), sBanner);
							pOutNode->AddProperty(wxT("host"), sHost);
							pOutNode->AddProperty(wxT("ip"), sIP);
							//?? factory id
							//pOutNode->AddProperty(wxT("factory_id"), wxString::Format(wxT("%d"), m_pFactory->GetID()));
							pOutNode->AddProperty(wxT("factory_name"), m_pFactory->GetName());
							pCallback->AddServer(pOutNode);//AddServer mast delete pOutNode
						}
					}
					pChild = pChild->GetNext();
				}
			}
		}
		//wxYieldIfNeeded();
	}
	return NULL;
}

void wxClientUDPNotifier::OnExit()
{
	if(m_socket)
		m_socket->Destroy();
}

void wxClientUDPNotifier::SendBroadcastMsg(void)
{
	wxIPV4address BroadCastAddress; // For broadcast sending 
	BroadCastAddress.Hostname(_("255.255.255.255")); 
	BroadCastAddress.Service(m_pFactory->GetAdvPort()); // port on which we listen for the answers 

	// Create the socket 
	wxString sMsg = wxString::Format(WXNETMESSAGE2, WXNETVER, enumGISMsgStHello, enumGISPriorityHightest);
	wxNetMessage msg(sMsg, wxID_ANY);
    if(msg.IsOk())
    {
		m_socket->Discard();
		if(m_socket->WaitForWrite(0, 100))
		{
			m_socket->SendTo(BroadCastAddress, msg.GetData(), msg.GetDataLen() ); 
		}
    }
}

////////////////////////////////////////////////////////////////////
// wxClientTCPNetFactory
////////////////////////////////////////////////////////////////////

IMPLEMENT_DYNAMIC_CLASS(wxClientTCPNetFactory, wxObject)

wxClientTCPNetFactory::wxClientTCPNetFactory(void) : m_pCallback(NULL), m_pClientUDPNotifier(NULL)
{
}

wxClientTCPNetFactory::~wxClientTCPNetFactory(void)
{
	//delete UDPNotifier thread
	if(m_pClientUDPNotifier)
		m_pClientUDPNotifier->Delete();
}

bool wxClientTCPNetFactory::StartServerSearch()
{
	if(m_pClientUDPNotifier)
	{
		m_pClientUDPNotifier->SendBroadcastMsg();
		return true;
	}
	//create & strart notify thread
	m_pClientUDPNotifier = new wxClientUDPNotifier(this);
	if(!CreateAndRunThread(m_pClientUDPNotifier, wxT("wxClientTCPNetFactory"), wxT("Notifier")))
		return false;
    wxLogMessage(_("%s: Wait %s Thread 0x%lx started (priority = %u)"), wxT("wxClientTCPNetFactory"), wxT("Notifier"), m_pClientUDPNotifier->GetId(), m_pClientUDPNotifier->GetPriority());

	//send broadcast
	m_pClientUDPNotifier->SendBroadcastMsg();
	return true;
}

wxXmlNode* wxClientTCPNetFactory::GetProperties(void)
{
	wxXmlNode* pOutNode = new wxXmlNode(wxXML_ELEMENT_NODE, wxT("factory"));
	pOutNode->AddProperty(wxT("port"), wxString::Format(wxT("%d"), m_nPort));
	pOutNode->AddProperty(wxT("adv_port"), wxString::Format(wxT("%d"), m_nAdvPort));
	pOutNode->AddProperty(wxT("addr"), m_sAddr);
    wxClassInfo* pInfo = GetClassInfo();
    if(pInfo)
        pOutNode->AddProperty(wxT("name"), pInfo->GetClassName());
	return pOutNode;
}

void wxClientTCPNetFactory::SetProperties(const wxXmlNode* pProp)
{
	m_nPort = wxAtoi(pProp->GetPropVal(wxT("port"), wxT("1976")));
	m_nAdvPort = wxAtoi(pProp->GetPropVal(wxT("adv_port"), wxT("1977")));
	m_sAddr = pProp->GetPropVal(wxT("addr"), wxT(""));
}

INetClientConnection* wxClientTCPNetFactory::GetConnection(wxXmlNode* pProp)
{
	INetClientConnection* pConn = new wxClientTCPNetConnection();
	if(pConn->SetProperties(pProp))
	{
		wxDELETE(pProp);
		return pConn;
	}
	return NULL;
}


////////////////////////////////////////////////////////////////////
// wxClientTCPNetPlugin
////////////////////////////////////////////////////////////////////

IMPLEMENT_DYNAMIC_CLASS(wxClientTCPNetConnection, wxObject)

wxClientTCPNetConnection::wxClientTCPNetConnection(void)
{
	m_bIsConnected = false;
	m_nUserID = -1;
	m_pSock = NULL;
}

wxClientTCPNetConnection::~wxClientTCPNetConnection(void)
{
}

wxXmlNode* wxClientTCPNetConnection::GetProperties(void)
{
    return NULL;
}

bool wxClientTCPNetConnection::SetProperties(const wxXmlNode* pProp)
{
	if(!pProp)
		return false;
	m_sConnName = pProp->GetPropVal(wxT("name"), wxT("New connection"));
	m_sUserName = pProp->GetPropVal(wxT("user"), wxT("user"));
	m_sCryptPass = pProp->GetPropVal(wxT("pass"), wxT("passsword"));
	m_sIP = pProp->GetPropVal(wxT("ip"), wxT("0.0.0.0"));
	m_sPort = pProp->GetPropVal(wxT("port"), wxT("1976"));
	return true;
}

bool wxClientTCPNetConnection::Connect(void)
{
	if(m_bIsConnected)
		return true;

	//start conn
	wxIPV4address addr;
	addr.Hostname(m_sIP);
	addr.Service(wxAtoi(m_sPort));
	
	// Create the socket
	m_pSock = new wxSocketClient(wxSOCKET_WAITALL);

	//
	wxLogMessage(_("wxClientTCPNetFactory: Start connection..."));
	//start reader thread
	m_pClientTCPReader = new wxClientTCPReader(this, m_pSock);
	if(!CreateAndRunThread(m_pClientTCPReader, wxT("wxClientTCPNetFactory"), wxT("TCPReader")))
		return false; 
    
	//start writer thread
	m_pClientTCPWriter = new wxClientTCPWriter(this, m_pSock);
	if(!CreateAndRunThread(m_pClientTCPWriter, wxT("wxClientTCPNetFactory"), wxT("TCPWriter")))
		return false; 

	wxLogMessage(_("wxClientTCPNetConnection: Trying to connect (timeout = 5 sec) ..."));
	m_pSock->Connect(addr, false);
	m_pSock->WaitOnConnect(5);
	if(m_pSock->IsConnected())
	{
		//start waitlost thread
		m_pClientTCPWaitlost = new wxClientTCPWaitlost(this, m_pSock);
		if(!CreateAndRunThread(m_pClientTCPWaitlost, wxT("wxClientTCPNetFactory"), wxT("TCPWaitlost")))
			return false; 
        wxLogMessage(_("wxClientTCPNetFactory: Connected"));
        //send auth?
	}

	return true;
}

bool wxClientTCPNetConnection::Disconnect(void)
{
	if(!m_bIsConnected)
		return true;

    m_pClientTCPReader->Delete();
    m_pClientTCPReader = NULL;
    m_pClientTCPWriter->Delete();
    m_pClientTCPWriter = NULL;
    m_pClientTCPWaitlost->Delete();
    m_pClientTCPWaitlost = NULL;

    CleanMsgQueueres();

	m_bIsConnected = false;
	return true;
}



