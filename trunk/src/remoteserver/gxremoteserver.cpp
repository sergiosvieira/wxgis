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

wxGxRemoteServer::wxGxRemoteServer(wxXmlNode* pConf) : m_bIsChildrenLoaded(false), m_pProperties(NULL), m_bIsConnected(false)
{
    m_pProperties = new wxXmlNode(*pConf);
    m_sName = m_pProperties->GetPropVal(wxT("name"), NONAME);
    m_sClassName = m_pProperties->GetPropVal(wxT("class"), NONAME);
    //create connection object ?
}

wxGxRemoteServer::~wxGxRemoteServer(void)
{
    wxDELETE(m_pProperties);
}

void wxGxRemoteServer::Detach(void)
{
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
    return m_pProperties;
}

bool wxGxRemoteServer::Connect(void)
{
    //1. create plugin
    //2. set m_pProperties
    //3. run
    return false;
}

bool wxGxRemoteServer::Disconnect(void)
{
    return false;
}


