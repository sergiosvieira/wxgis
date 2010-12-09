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

	m_socket = new wxDatagramSocket(LocalAddress, wxSOCKET_REUSEADDR | wxSOCKET_NOWAIT | wxSOCKET_BLOCK); 
	const int optval = 1; 
	m_socket->SetOption(SOL_SOCKET, SO_BROADCAST, &optval, sizeof(optval)); 
	//m_socket->SetOption(SOL_SOCKET, SO_KEEPALIVE, &optval, sizeof(optval)); 
	//m_socket->SetOption(SOL_SOCKET, SO_REUSEADDR, &optval, sizeof(optval)); 
    m_socket->Notify(false);
}

void *wxClientUDPNotifier::Entry()
{
	unsigned char buff[BUFF] = {0};

	m_socket->SetTimeout(1);
	wxIPV4address BroadCastAddress; // For broadcast sending 
	BroadCastAddress.Service(m_pFactory->GetAdvPort() + 2);

	//start loop
	while(!TestDestroy())
	{
		m_socket->RecvFrom(BroadCastAddress, &buff, BUFF); 
		if(m_socket->Error())
		{
			//wxYieldIfNeeded();
			wxThread::Sleep(50);
			continue;
		}
		size_t nSize = m_socket->LastCount();
		if(nSize > 0)
		{
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
					if(pChild->GetName().CmpNoCase(wxT("info")) == 0)
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
    wxDELETE(m_socket);
}

void wxClientUDPNotifier::SendBroadcastMsg(void)
{
	wxIPV4address BroadCastAddress; // For broadcast sending 
	BroadCastAddress.Hostname(_("255.255.255.255")); 
	BroadCastAddress.Service(m_pFactory->GetAdvPort()); // port on which we listen for the answers 

	// Create the socket 
	wxString sMsg = wxString::Format(WXNETMESSAGE2, WXNETVER, enumGISMsgStHello, enumGISPriorityHighest, wxT("hello"));
	wxNetMessage msg(sMsg);
    if(msg.IsOk())
    {
		while(m_socket->SendTo(BroadCastAddress, msg.GetData(), msg.GetDataLen() ).Error())
		{
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

bool wxClientTCPNetFactory::StopServerSearch()
{
	if(m_pClientUDPNotifier)
		m_pClientUDPNotifier->Delete();
	m_pClientUDPNotifier = NULL;
	return true;
};


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
    m_pClientTCPReader = NULL;
    m_pClientTCPWriter = NULL;
//    m_pClientTCPWaitlost = NULL;
	m_pCallBack = NULL;
}

wxClientTCPNetConnection::~wxClientTCPNetConnection(void)
{
}

wxXmlNode* wxClientTCPNetConnection::GetProperties(void)
{
    wxXmlNode* pNode = new wxXmlNode(wxXML_ELEMENT_NODE, wxT("connection"));
    pNode->AddProperty(wxT("name"), m_sConnName);
    pNode->AddProperty(wxT("user"), m_sUserName);    
    pNode->AddProperty(wxT("pass"), m_sCryptPass);    
    pNode->AddProperty(wxT("ip"), m_sIP);    
    pNode->AddProperty(wxT("port"), m_sPort);    
    wxClassInfo* pInfo = GetClassInfo();
    if(pInfo)
        pNode->AddProperty(wxT("class"), pInfo->GetClassName());
    return pNode;
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
	m_pSock = new wxSocketClient(wxSOCKET_WAITALL | wxSOCKET_BLOCK);
    m_pSock->Notify(false);
	m_pSock->SetTimeout(5);

	wxLogMessage(_("wxClientTCPNetConnection: Trying to connect (timeout = 5 sec) ..."));
	m_pSock->Connect(addr, true);
	if(m_pSock->IsConnected())
	{
		m_pSock->SetTimeout(1);
		m_bIsConnected = true;
		//
		//start reader thread
		m_pClientTCPReader = new wxNetTCPReader(this, m_pSock);
		if(!CreateAndRunThread(m_pClientTCPReader, wxT("wxClientTCPNetFactory"), wxT("TCPReader")))
			return false; 
	    
		//start writer thread
		m_pClientTCPWriter = new wxNetTCPWriter(this, m_pSock);
		if(!CreateAndRunThread(m_pClientTCPWriter, wxT("wxClientTCPNetFactory"), wxT("TCPWriter")))
			return false; 

		//start waitlost thread
		m_pClientTCPWaitlost = new wxNetTCPWaitlost(this, m_pSock);
		if(!CreateAndRunThread(m_pClientTCPWaitlost, wxT("wxClientTCPNetFactory"), wxT("TCPWaitlost")))
			return false; 

        wxLogMessage(_("wxClientTCPNetFactory: Connected"));
		if(m_pCallBack)
			m_pCallBack->OnConnect();
		return true;
	}
	wxDELETE(m_pSock);
	return false;
}

bool wxClientTCPNetConnection::Disconnect(void)
{
	if(!m_bIsConnected)
		return true;

	wxNetMessage MsgOut(wxString::Format(WXNETMESSAGE2, WXNETVER, enumGISMsgStBye, enumGISPriorityHighest, wxT("bye")));
	if(MsgOut.IsOk())
	{
		if(m_pSock->IsConnected())
		{
			m_pSock->SetTimeout(1);
			m_pSock->WriteMsg( MsgOut.GetData(), MsgOut.GetDataLen() ); 
		}
	}

	m_bIsConnected = false;
	m_pSock->Close();

    CleanMsgQueueres();
	if(m_pCallBack)
		m_pCallBack->OnDisconnect();

    if(m_pClientTCPReader)
		m_pClientTCPReader->Delete();
    m_pClientTCPReader = NULL;
    if(m_pClientTCPWriter)
		m_pClientTCPWriter->Delete();
    m_pClientTCPWriter = NULL;
    //if(m_pClientTCPWaitlost)	//auto exit
    //    m_pClientTCPWaitlost->Delete();
    m_pClientTCPWaitlost = NULL;

//	m_pSock->Destroy();

	wxDELETE(m_pSock);

    wxLogMessage(_("wxClientTCPNetFactory: Server %s disconnected"), m_sIP.c_str());

	return true;
}

void wxClientTCPNetConnection::PutInMessage(WXGISMSG msg)
{
	if(m_pCallBack)
		m_pCallBack->PutInMessage(msg);
}

