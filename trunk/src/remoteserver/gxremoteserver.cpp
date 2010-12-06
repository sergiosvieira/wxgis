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
}

wxGxRemoteServer::~wxGxRemoteServer(void)
{
    wxDELETE(m_pNetConn);
}

void wxGxRemoteServer::Detach(void)
{
	Disconnect();
}

void wxGxRemoteServer::Refresh(void)
{
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
 	if(m_pNetConn)
		return m_pNetConn->Connect();
    return false;
}

bool wxGxRemoteServer::Disconnect(void)
{
 	if(m_pNetConn && m_pNetConn->IsConnected())
		return m_pNetConn->Disconnect();
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
	AddMessageReceiver(wxT("srv_info"), static_cast<INetMessageReceiver*>(this));

	OnStartMessageThread();
	m_pCatalog->ObjectChanged(this);
}

void wxGxRemoteServer::OnDisconnect(void)
{
	OnStopMessageThread();
	ClearMessageReceiver();
	m_pCatalog->ObjectChanged(this);
}

void wxGxRemoteServer::ProcessMessage(WXGISMSG msg, wxXmlNode* pChildNode)
{
    wxString sName = pChildNode->GetName();
    if(sName.CmpNoCase(wxT("auth")) == 0)
    {
		if(msg.pMsg->GetState() == enumGISMsgStOk)
		{
			m_bAuth = true;
			m_pCatalog->ObjectChanged(this);
		}
		return;
    }

	if(sName.CmpNoCase(wxT("srv_info")) == 0)
	{
		wxString sUserName(wxT("test"));
		wxString sCryptPass(wxT("test"));
        //send auth
		wxString sAuth = wxString::Format(wxT("<auth user=\"%s\" pass=\"%s\" />"), wxNetMessage::FormatXmlString(sUserName).c_str(), sCryptPass.c_str());
		wxString sMsg = wxString::Format(WXNETMESSAGE1, WXNETVER, enumGISMsgStCmd, enumGISPriorityHigh, sAuth.c_str());
		wxNetMessage* pMsg = new wxNetMessage(sMsg);
		if(pMsg->IsOk())
		{
			WXGISMSG msg = {pMsg, -1};
			if(m_pNetConn)
				m_pNetConn->PutOutMessage(msg);
		}
		//end send auth
		return;
	}
 //   else
 //   {
	////any messages goes here
	////translate messages
 //   }
}

void wxGxRemoteServer::PutInMessage(WXGISMSG msg)
{
	//proceed message in separate thread
	wxCriticalSectionLocker locker(m_CriticalSection);
    m_MsgQueue.push(msg);
}
