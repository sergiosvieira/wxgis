/******************************************************************************
 * Project:  wxGIS (GIS Server)
 * Purpose:  TCP network server plugin class.
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
#include "wxgissrv/srv_networking/tcpnetworkplugin.h"
#include "wxgis/networking/message.h"

#include "wx/datetime.h"

// ----------------------------------------------------------------------------
// wxServerUDPNotifier
// ----------------------------------------------------------------------------
wxServerUDPNotifier::wxServerUDPNotifier(wxString sServName, wxString sAddr, int nPort, int nAdvPort) : wxThread()
{
	m_sServName = sServName;
	m_sAddr = sAddr;
	m_nPort = nPort;
	m_nAdvPort = nAdvPort;
}

void *wxServerUDPNotifier::Entry()
{
	wxIPV4address LocalAddress; // For the listening 
	LocalAddress.Service(m_nAdvPort); // port on which we listen for the answers 

	bool bIsAddrSet = false;
	if(m_sAddr.IsEmpty())
		bIsAddrSet = LocalAddress.AnyAddress(); 
	else
		bIsAddrSet = LocalAddress.Hostname(m_sAddr);
	if(!bIsAddrSet)
	{
		wxLogError(_("wxServerUDPNotifier: Invalid address - %s"), m_sAddr.c_str());
		return (ExitCode)-1;
	}

	m_socket = new wxDatagramSocket(LocalAddress, wxSOCKET_REUSEADDR | wxSOCKET_NOWAIT | wxSOCKET_BLOCK); 
	const int optval = 1; 
	m_socket->SetOption(SOL_SOCKET, SO_BROADCAST, &optval, sizeof(optval)); 
	//m_socket->SetOption(SOL_SOCKET, SO_KEEPALIVE, &optval, sizeof(optval)); 
	//m_socket->SetOption(SOL_SOCKET, SO_REUSEADDR, &optval, sizeof(optval)); 
    m_socket->Notify(false);
	m_socket->SetTimeout(1);

	unsigned char buff[BUFF] = {0};

	wxIPV4address BroadCastAddress; // For broadcast sending 
	BroadCastAddress.Service(m_nAdvPort);
	//BroadCastAddress.AnyAddress(); 

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
				wxThread::Sleep(10);
				continue;
			}
			if(msg.GetState() == enumGISMsgStHello)
			{
				wxString sServData = wxString::Format(wxT("<info port=\"%d\" name=\"%s\" banner=\"%s\" />"), m_nPort, wxNetMessage::FormatXmlString(m_sServName).c_str(), wxNetMessage::FormatXmlString(wxGetOsDescription()).c_str());
				wxString sMsg = wxString::Format(WXNETMESSAGE1, WXNETVER, enumGISMsgStOk, enumGISPriorityHighest, wxT("info"), sServData.c_str());
				wxNetMessage msgout(sMsg);
				if(msgout.IsOk())
				{
					if(m_socket->WaitForWrite(0, 100))
					{
						BroadCastAddress.Service(m_nAdvPort + 2);
						m_socket->SendTo(BroadCastAddress, msgout.GetData(), msgout.GetDataLen()); 
					}
				}
			}
		}		
	}
	return NULL;
}

void wxServerUDPNotifier::OnExit()
{
	wxDELETE(m_socket);
}

// ----------------------------------------------------------------------------
// wxServerTCPWaitConnect
// ----------------------------------------------------------------------------
wxServerTCPWaitAccept::wxServerTCPWaitAccept(wxString sListenAddr, int nListenPort, wxGISNetworkService* pGISNetworkService)
{
    m_pGISNetworkService = pGISNetworkService;

	// Create the address
	wxIPV4address ip_addr;
	ip_addr.Service(nListenPort);
	bool bIsAddrSet = false;
	if(sListenAddr.Len() > 0)
		bIsAddrSet = ip_addr.Hostname(sListenAddr);
	else
		bIsAddrSet = ip_addr.AnyAddress();
	if(!bIsAddrSet)
	{
		wxLogError(_("wxServerTCPWaitAccept: Invalid address -- %s"), sListenAddr);
		return;
	}

	// Create the socket
	m_pSock = new wxSocketServer(ip_addr, wxSOCKET_WAITALL | wxSOCKET_BLOCK);
    m_pSock->Notify(false);
	m_pSock->SetTimeout(1);
	
	// We use Ok() here to see if the server is really listening
	if (! m_pSock->Ok())
	{
		wxLogError(_("wxServerTCPWaitAccept: Could not listen at the specified port! Port number - %d"), nListenPort);
		wxDELETE(m_pSock);
		return;
	}
	else
		wxLogMessage(_("wxServerTCPWaitAccept: Listening..."));
}

void *wxServerTCPWaitAccept::Entry()
{
	if(m_pSock == NULL || m_pGISNetworkService == NULL)
		return (ExitCode)-1;

	while(!TestDestroy())
	{
		wxSocketBase* pNewSocket = m_pSock->Accept(true);
		if (pNewSocket)
		{
			wxIPV4address cliend_addr;
			pNewSocket->GetPeer(cliend_addr);
            pNewSocket->Notify(false);
			if(m_pGISNetworkService->CanAcceptConnection())
			{
				pNewSocket->SetFlags(wxSOCKET_WAITALL | wxSOCKET_BLOCK);//wxSOCKET_REUSEADDR | 
				pNewSocket->SetTimeout(1);
				wxLogMessage(_("wxServerTCPWaitAccept: New client connection accepted. Address - %s"), cliend_addr.IPAddress().c_str());

				INetServerConnection* pConn = new wxServerTCPNetConnection(pNewSocket, m_pGISNetworkService);
				m_pGISNetworkService->AddConnection(pConn);
			}
			else
			{
				wxLogMessage(_("wxServerTCPWaitAccept: To many connections! Connection to address - %s is not established"), cliend_addr.IPAddress().c_str());
                wxString sErr = wxString::Format(wxT("<info text=\"%s\" />"), _("To many connections!"));
                wxString sMsg = wxString::Format(WXNETMESSAGE1, WXNETVER, enumGISMsgStRefuse, enumGISPriorityHighest, wxT("info"), sErr.c_str());
				pNewSocket->SetTimeout(5);
			    wxNetMessage msgout(sMsg);
			    if(msgout.IsOk())
				    pNewSocket->WriteMsg(msgout.GetData(), msgout.GetDataLen()); 
				wxNetMessage msgbye(wxString::Format(WXNETMESSAGE2, WXNETVER, enumGISMsgStBye, enumGISPriorityHighest, wxT("bye")));
			    if(msgbye.IsOk())
				    pNewSocket->WriteMsg(msgbye.GetData(), msgbye.GetDataLen()); 
                //close connection
				//pNewSocket->Close();
				//pNewSocket = NULL;
                wxDELETE(pNewSocket);
			}
		}
	}
	return NULL;
}

void wxServerTCPWaitAccept::OnExit()
{
	wxDELETE(m_pSock);
}

// ----------------------------------------------------------------------------
// wxServerTCPWaitConnect
// ----------------------------------------------------------------------------
wxServerTCPWaitLost::wxServerTCPWaitLost(wxServerTCPNetConnection* pConnection, wxSocketBase* pSock)
{
	m_pConnection = pConnection;
	m_pSock = pSock;
}

void *wxServerTCPWaitLost::Entry()
{
	if(m_pConnection == NULL)
		return (ExitCode)-1;

	unsigned short nCounter(0);

	while(!TestDestroy())
	{
		//check alive and disconnect if not alive
		wxDateTime dLast = m_pConnection->GetAlive();
		wxTimeSpan span = wxDateTime::Now() - dLast;
		if(span.GetMinutes() > 10)
		{
			wxNetMessage *pMsgOut = new wxNetMessage(wxString::Format(WXNETMESSAGE2, WXNETVER, enumGISMsgStBye, enumGISPriorityHighest, wxT("bye")));
			WXGISMSG msg = {pMsgOut, m_pConnection->GetUserID()};
			m_pConnection->PutInMessage(msg);
			wxThread::Sleep(50);
		}
		else
		{
			if(nCounter > 180)
			{
				//send alive packet
				wxNetMessage MsgOut(wxString::Format(WXNETMESSAGE2, WXNETVER, enumGISMsgStAlive, enumGISPriorityHighest, wxT("bye")));
				if(MsgOut.IsOk())
				{
					if(m_pSock->IsConnected())
					{
						m_pSock->SetTimeout(1);
						m_pSock->WriteMsg( MsgOut.GetData(), MsgOut.GetDataLen() ); 
					}
				}
				nCounter = 0;
			}
			nCounter++;
			wxThread::Sleep(500); //sleep 1,5 min
		}
	}
	return NULL;
}

void wxServerTCPWaitLost::OnExit()
{	
}

// ----------------------------------------------------------------------------
// wxServerTCPNetConnection
// ----------------------------------------------------------------------------
wxServerTCPNetConnection::wxServerTCPNetConnection(wxSocketBase* pSocket, wxGISNetworkService* pGISNetworkService)
{
	m_pSock = pSocket;
	m_pGISNetworkService = pGISNetworkService;
    m_pServerTCPReader = NULL;
    m_pServerTCPWriter = NULL;
//    m_pServerTCPWaitlost = NULL;
    m_bIsConnected = false;
	m_stUserInfo.bIsValid = false;
	m_nUserID = -2;
}

wxServerTCPNetConnection::~wxServerTCPNetConnection()
{
}

bool wxServerTCPNetConnection::Connect(void)
{
	if(m_bIsConnected)
		return true;

    m_bIsConnected = true;

	//start reader thread
	m_pServerTCPReader = new wxNetTCPReader(this, m_pSock);
	if(!CreateAndRunThread(m_pServerTCPReader, wxT("wxServerTCPNetConnection"), wxT("TCPReader")))
		return false; 
    
	//start writer thread
	m_pServerTCPWriter = new wxNetTCPWriter(this, m_pSock);
	if(!CreateAndRunThread(m_pServerTCPWriter, wxT("wxServerTCPNetConnection"), wxT("TCPWriter")))
		return false; 

	////start waitlost thread
	//m_pServerTCPWaitlost = new wxNetTCPWaitlost(this, m_pSock);
	//if(!CreateAndRunThread(m_pServerTCPWaitlost, wxT("wxServerTCPNetConnection"), wxT("TCPWaitlost")))
	//	return false; 
	//start waitlost thread
	m_LastAlive = wxDateTime::Now();
	m_pServerTCPWaitlost = new wxServerTCPWaitLost(this, m_pSock);
	if(!CreateAndRunThread(m_pServerTCPWaitlost, wxT("wxServerTCPNetConnection"), wxT("TCPWaitlost")))
		return false; 

    wxString sServData = wxString::Format(wxT("<info name=\"%s\" banner=\"%s\" />"), wxNetMessage::FormatXmlString(m_pGISNetworkService->GetServerName()).c_str(), wxNetMessage::FormatXmlString(wxGetOsDescription()).c_str());
    wxString sMsg = wxString::Format(WXNETMESSAGE1, WXNETVER, enumGISMsgStOk, enumGISPriorityHighest, wxT("info"), sServData.c_str());
    wxNetMessage *pMsgOut = new wxNetMessage(sMsg);
    WXGISMSG msg = {pMsgOut, m_nUserID};
    PutOutMessage(msg);
	
	return true;
}

bool wxServerTCPNetConnection::Disconnect(void)
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

    if(m_pServerTCPReader)
		m_pServerTCPReader->Delete();
    m_pServerTCPReader = NULL;
    if(m_pServerTCPWriter)
		m_pServerTCPWriter->Delete();
    m_pServerTCPWriter = NULL;

    if(m_pServerTCPWaitlost)
        m_pServerTCPWaitlost->Delete();
    m_pServerTCPWaitlost = NULL;
	m_LastAlive = wxDateTime::Now();

    CleanMsgQueueres();

	//m_pSock->Destroy();
	wxDELETE(m_pSock);
	//m_pSock = NULL;
	if(m_pGISNetworkService)
		m_pGISNetworkService->DelConnection(m_nUserID);

	return true;
}

void wxServerTCPNetConnection::PutInMessage(WXGISMSG msg)
{
	if(m_pGISNetworkService)
		m_pGISNetworkService->PutInMessage(msg);
}

// ----------------------------------------------------------------------------
// wxServerTCPNetworkPlugin
// ----------------------------------------------------------------------------
IMPLEMENT_DYNAMIC_CLASS(wxServerTCPNetworkPlugin, wxObject)

wxServerTCPNetworkPlugin::wxServerTCPNetworkPlugin(void) : m_nPort(1976), m_nAdvPort(1977), m_pServerUDPNotifier(NULL), m_pWaitThread(NULL)
{
}

wxServerTCPNetworkPlugin::~wxServerTCPNetworkPlugin(void)
{
}

bool wxServerTCPNetworkPlugin::Start(wxGISNetworkService* pNetService, wxXmlNode* pConfig)
{
    //m_pConfig = pConfig;
    m_pNetService = pNetService;
	m_sAddr = pConfig->GetPropVal(wxT("addr"), wxT(""));
	m_nPort = wxAtoi(pConfig->GetPropVal(wxT("port"), wxT("1976")));
	m_nAdvPort = wxAtoi(pConfig->GetPropVal(wxT("adv_port"), wxT("1977")));

	//strart notify thread
	m_pServerUDPNotifier = new wxServerUDPNotifier(m_pNetService->GetServerName(), m_sAddr, m_nPort, m_nAdvPort);
	if(!CreateAndRunThread(m_pServerUDPNotifier, wxT("wxServerTCPNetworkPlugin"), wxT("Notifier")))
		return false; 
    wxLogMessage(_("%s: Wait %s Thread 0x%lx started (priority = %u)"), wxT("wxServerTCPNetworkPlugin"), wxT("Notifier"), m_pServerUDPNotifier->GetId(), m_pServerUDPNotifier->GetPriority());


	//Start Connect Thread
	m_pWaitThread = new wxServerTCPWaitAccept(m_sAddr, m_nPort, m_pNetService);//this
	if(!CreateAndRunThread(m_pWaitThread, wxT("wxServerTCPNetworkPlugin"), wxT("Accept")))
		return false; 
    wxLogMessage(_("%s: Wait %s Thread 0x%lx started (priority = %u)"), wxT("wxServerTCPNetworkPlugin"), wxT("Accept"), m_pWaitThread->GetId(), m_pWaitThread->GetPriority());

	wxLogMessage(_("%s: Plugin is started..."), wxT("wxServerTCPNetworkPlugin"));
	return true;

}

bool wxServerTCPNetworkPlugin::Stop(void)
{
	//delete UDPNotifier thread
	if(m_pServerUDPNotifier)
		m_pServerUDPNotifier->Delete();

	//delete WaitAccept thread
	if(m_pWaitThread)
		m_pWaitThread->Delete();

	wxLogMessage(_("%s: Plugin is shutdown..."), wxT("wxServerTCPNetworkPlugin"));
	return true;
}

