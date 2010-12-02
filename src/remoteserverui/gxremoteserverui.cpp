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

wxGxRemoteServerUI::wxGxRemoteServerUI(INetClientConnection* pNetConn, wxIcon SmallIcon, wxIcon LargeIcon, wxIcon SmallDsblIcon, wxIcon LargeDsblIcon) : wxGxRemoteServer(pNetConn)
{
    m_SmallIcon = SmallIcon;
    m_LargeIcon = LargeIcon;
    m_SmallDsblIcon = SmallDsblIcon;
    m_LargeDsblIcon = LargeDsblIcon;
}

wxGxRemoteServerUI::~wxGxRemoteServerUI(void)
{
}

wxIcon wxGxRemoteServerUI::GetLargeImage(void)
{
    if(m_pNetConn && m_pNetConn->IsConnected())
        return m_LargeIcon;
    else
        return m_LargeDsblIcon;
}

wxIcon wxGxRemoteServerUI::GetSmallImage(void)
{
    if(m_pNetConn && m_pNetConn->IsConnected())
        return m_SmallIcon;
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
	//any messages goes here
	//translate messages
}
