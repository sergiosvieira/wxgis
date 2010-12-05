/******************************************************************************
 * Project:  wxGIS (GIS Remote)
 * Purpose:  wxGxRemoteServerUI class.
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

#include "wxgis/remoteserverui/gxremoteserverui.h"

wxGxRemoteServerUI::wxGxRemoteServerUI(INetClientConnection* pNetConn, wxIcon SmallIcon, wxIcon LargeIcon, wxIcon SmallDsblIcon, wxIcon LargeDsblIcon, wxIcon SmallAuthIcon, wxIcon LargeAuthIcon) : wxGxRemoteServer(pNetConn), m_bAuth(false)
{
    m_SmallIcon = SmallIcon;
    m_LargeIcon = LargeIcon;
    m_SmallDsblIcon = SmallDsblIcon;
    m_LargeDsblIcon = LargeDsblIcon;
    m_SmallAuthIcon = SmallAuthIcon;
    m_LargeAuthIcon = LargeAuthIcon;
}

wxGxRemoteServerUI::~wxGxRemoteServerUI(void)
{
}

wxIcon wxGxRemoteServerUI::GetLargeImage(void)
{
    if(m_pNetConn && m_pNetConn->IsConnected())
    {
        if(m_bAuth)
            return m_LargeAuthIcon;
        else
            return m_LargeIcon;
    }
    else
        return m_LargeDsblIcon;
}

wxIcon wxGxRemoteServerUI::GetSmallImage(void)
{
    if(m_pNetConn && m_pNetConn->IsConnected())
    {
        if(m_bAuth)
            return m_SmallAuthIcon;
        else
            return m_SmallIcon;
    }
    else
        return m_SmallDsblIcon;
}

bool wxGxRemoteServerUI::Invoke(wxWindow* pParentWnd)
{
    if(m_pNetConn && !m_pNetConn->IsConnected())
	{
		return m_pNetConn->Connect();
	}
	return false;
}

void wxGxRemoteServerUI::OnConnect(void)
{
	m_pCatalog->ObjectChanged(this);
}

void wxGxRemoteServerUI::OnDisconnect(void)
{
	m_pCatalog->ObjectChanged(this);
}

void wxGxRemoteServerUI::PutInMessage(WXGISMSG msg)
{
    const wxXmlNode* pRoot = msg.pMsg->GetRoot();
    if(!pRoot)
        wxDELETE(msg.pMsg);
    wxXmlNode* pChild = pRoot->GetChildren();
    if(!pChild)
    {
        wxString sMessage = pChild->GetNodeContent();
        //log contents and exit
        wxLogMessage(_("wxGxRemoteServerUI: %s"), sMessage.c_str());
    }
    wxString sName = pChild->GetName();
    if(sName.CmpNoCase(wxT("auth")) == 0)
    {

    }
    else
    {
    }
	//any messages goes here
	//translate messages
}
