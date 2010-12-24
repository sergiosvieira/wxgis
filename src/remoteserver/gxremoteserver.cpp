/******************************************************************************
 * Project:  wxGIS (GIS Remote)
 * Purpose:  wxGxRemoteServer class.
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
#include "wxgis/remoteserver/gxremoteserver.h"
#include "wxgis/networking/message.h"

/////////////////////////////////////////////////////////////////////////////////////////////////////
// wxGxRemoteServer
/////////////////////////////////////////////////////////////////////////////////////////////////////

wxGxRemoteServer::wxGxRemoteServer(INetClientConnection* pNetConn) : m_bIsChildrenLoaded(false), m_bAuth(false)
{
	m_pNetConn = pNetConn;
	m_nChildCount = 0;
}

wxGxRemoteServer::~wxGxRemoteServer(void)
{
    wxDELETE(m_pNetConn);
}

bool wxGxRemoteServer::Attach(IGxObject* pParent, IGxCatalog* pCatalog)
{
	if(!IGxObject::Attach(pParent, pCatalog))
		return false;

	return true;
}


void wxGxRemoteServer::Detach(void)
{
	if(IsConnected())
		Disconnect();
	IGxObject::Detach();
}

void wxGxRemoteServer::Refresh(void)
{
	EmptyChildren();
	LoadChildren();
}
 
bool wxGxRemoteServer::DeleteChild(IGxObject* pChild)
{
	bool bHasChildren = m_Children.size() > 0 ? true : false;
    m_pCatalog->ObjectDeleted(pChild);
	if(!IGxObjectContainer::DeleteChild(pChild))
		return false;
	if(bHasChildren != m_Children.size() > 0 ? true : false)
		m_pCatalog->ObjectChanged(this);
	return true;
}

wxXmlNode* wxGxRemoteServer::GetProperties(void)
{
	if(m_pNetConn)
		return m_pNetConn->GetProperties();
    return NULL;
}

bool wxGxRemoteServer::Connect(void)
{
 	if(m_pNetConn && !m_pNetConn->IsConnected())
		return m_pNetConn->Connect();
    return false;
}

bool wxGxRemoteServer::Disconnect(void)
{
 	if(m_pNetConn && m_pNetConn->IsConnected())
		return m_pNetConn->Disconnect();
    return false;
}

bool wxGxRemoteServer::IsConnected(void)
{
 	if(m_pNetConn)
		return m_pNetConn->IsConnected();
	return false;
}

wxString wxGxRemoteServer::GetName(void)
{
 	if(m_pNetConn)
		return m_pNetConn->GetName();
	return NONAME;
}


void wxGxRemoteServer::OnConnect(void)
{
	AddMessageReceiver(wxT("auth"), static_cast<INetMessageReceiver*>(this));
	AddMessageReceiver(wxT("info"), static_cast<INetMessageReceiver*>(this));
	AddMessageReceiver(wxT("bye"), static_cast<INetMessageReceiver*>(this));

	OnStartMessageThread();
	m_pCatalog->ObjectChanged(this);
}

void wxGxRemoteServer::OnDisconnect(void)
{
	OnStopMessageThread();
	EmptyChildren();
	m_bAuth = false;
	m_nChildCount = 0;
	m_bIsChildrenLoaded = false;
}

void wxGxRemoteServer::ProcessMessage(WXGISMSG msg, wxXmlNode* pChildNode)
{
	if(msg.pMsg->GetState() == enumGISMsgStAlive)
	{
        wxNetMessage* pMsg = new wxNetMessage(wxString::Format(WXNETMESSAGE2, WXNETVER, enumGISMsgStAlive, enumGISPriorityHighest, wxT("bye")));
        if(pMsg->IsOk())
        {
	        WXGISMSG msg = {pMsg, -1};
	        m_pNetConn->PutOutMessage(msg);
        }
	}

    wxString sName = msg.pMsg->GetDestination();
    if(sName.CmpNoCase(wxT("auth")) == 0)
    {
		if(msg.pMsg->GetState() == enumGISMsgStOk)
		{
			m_bAuth = true;
			m_pCatalog->ObjectChanged(this);

			LoadChildren();
		}
		return;
    }

	if(sName.CmpNoCase(wxT("info")) == 0)
	{
        if(msg.pMsg->GetState() == enumGISMsgStOk)
        {
            //log text
			if(pChildNode)
			{
				wxString sServer = pChildNode->GetPropVal(wxT("name"), wxT(""));
				wxString sBanner = pChildNode->GetPropVal(wxT("banner"), wxT(""));
				wxLogMessage(wxT("wxGxRemoteServer: Connected to server - %s; %s"), sServer.c_str(), sBanner.c_str());
			}

            if(m_pNetConn)
            {
                wxString sUserName = m_pNetConn->GetUser();
                wxString sCryptPass = m_pNetConn->GetCryptPasswd();
		        wxString sAuth = wxString::Format(wxT("<auth user=\"%s\" pass=\"%s\" />"), wxNetMessage::FormatXmlString(sUserName).c_str(), sCryptPass.c_str());
		        wxString sMsg = wxString::Format(WXNETMESSAGE1, WXNETVER, enumGISMsgStHello, enumGISPriorityHigh, wxT("auth"), sAuth.c_str());
		        wxNetMessage* pMsg = new wxNetMessage(sMsg);
		        if(pMsg->IsOk())
		        {
			        WXGISMSG msg = {pMsg, -1};
			        m_pNetConn->PutOutMessage(msg);
		        }
            }
		    return;
        }
	}
}

void wxGxRemoteServer::PutInMessage(WXGISMSG msg)
{
	//proceed message in separate thread
	wxCriticalSectionLocker locker(m_CriticalSection);
    m_MsgQueue.push(msg);
}

void wxGxRemoteServer::PutOutMessage(WXGISMSG msg)
{
	if(m_pNetConn)
		m_pNetConn->PutOutMessage(msg);
}

void wxGxRemoteServer::EmptyChildren(void)
{
	for(size_t i = 0; i < m_Children.size(); i++)
	{
		m_Children[i]->Detach();
		delete m_Children[i];
	}
	m_Children.clear();
	m_bIsChildrenLoaded = false;
	m_pCatalog->ObjectChanged(this);
}

void wxGxRemoteServer::LoadChildren()
{
	if(m_bIsChildrenLoaded)
		return;
    Connect();
    //request children
    wxString sMsg = wxString::Format(WXNETMESSAGE1, WXNETVER, enumGISMsgStGet, enumGISPriorityNormal, wxT("root"), wxT("<children/>"));
    wxNetMessage* pMsg = new wxNetMessage(sMsg);
    if(pMsg->IsOk())
    {
        WXGISMSG msg = {pMsg, -1};
        m_pNetConn->PutOutMessage(msg);
    }
}