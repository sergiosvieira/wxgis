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


IMPLEMENT_DYNAMIC_CLASS(wxGxRemoteServer, wxObject)

wxGxRemoteServer::wxGxRemoteServer(void) : m_bIsChildrenLoaded(false)
{
    m_bEnabled = true;
}

wxGxRemoteServer::~wxGxRemoteServer(void)
{
}

void wxGxRemoteServer::Detach(void)
{
	EmptyChildren();
}

void wxGxRemoteServer::Refresh(void)
{
}
 
void wxGxRemoteServer::EmptyChildren(void)
{
	for(size_t i = 0; i < m_Children.size(); i++)
	{
		m_Children[i]->Detach();
		wxDELETE(m_Children[i]);
	}
	m_Children.clear();
	m_bIsChildrenLoaded = false;
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

void wxGxRemoteServer::LoadChildren(void)
{
	if(m_bIsChildrenLoaded)
		return;	
	wxXmlNode* pChild = pConf->GetChildren();
	while(pChild)
	{
		pChild = pChild->GetNext();
	}

    //for(size_t i = 0; i < m_aConnections.size(); i++)
    //{
    //    wxGxDiscConnection* pwxGxDiscConnection = new wxGxDiscConnection(m_aConnections[i].sPath, m_aConnections[i].sName);
    //    IGxObject* pGxObject = static_cast<IGxObject*>(pwxGxDiscConnection);
    //    if(AddChild(pGxObject))
    //        wxLogMessage(_("wxGxDiscConnections: Add folder connection [%s]"), m_aConnections[i].sName.c_str());
    //}

	m_bIsChildrenLoaded = true;
}
