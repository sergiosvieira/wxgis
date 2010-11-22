/******************************************************************************
 * Project:  wxGIS (GIS Remote)
 * Purpose:  wxGxRemoteServers class.
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
#include "wxgis/remoteserver/gxremoteservers.h"
#include "wxgis/remoteserver/gxremoteserver.h"


IMPLEMENT_DYNAMIC_CLASS(wxGxRemoteServers, wxObject)

wxGxRemoteServers::wxGxRemoteServers(void) : m_bIsChildrenLoaded(false)
{
    m_bEnabled = true;
}

wxGxRemoteServers::~wxGxRemoteServers(void)
{
}

void wxGxRemoteServers::Detach(void)
{
	EmptyChildren();
	UnLoadPlugins();
}

void wxGxRemoteServers::Refresh(void)
{
	//nothing to do
}
 
void wxGxRemoteServers::Init(wxXmlNode* pConfigNode)
{
	wxXmlNode *pPlugins, *pConnections;
	wxXmlNode* pChild = pConfigNode->GetChildren();
	while(pChild)
	{
		if(pChild->GetName().CmpNoCase(wxT("plugins")) == 0)
			pPlugins = pChild;
		else if(pChild->GetName().CmpNoCase(wxT("connections")) == 0)
			pConnections = pChild;
		pChild = pChild->GetNext();
	}
	LoadPlugins(pPlugins);
	LoadChildren(pConnections);
}

wxXmlNode* wxGxRemoteServers::GetProperties(void)
{
    wxXmlNode* pNode = new wxXmlNode(wxXML_ELEMENT_NODE, wxT("rootitem"));
    wxClassInfo* pInfo = GetClassInfo();
    if(pInfo)
        pNode->AddProperty(wxT("name"), pInfo->GetClassName());
    pNode->AddProperty(wxT("is_enabled"), m_bEnabled == true ? wxT("1") : wxT("0"));    
#ifndef WXGISPORTABLE
	for(size_t i = 0; i < m_Children.size(); i++)
	{
		
//        wxGxDiscConnection* pwxGxDiscConnection = dynamic_cast<wxGxDiscConnection*>(m_Children[i]);
//        if(pwxGxDiscConnection)
//        {
//		    wxXmlNode* pDiscConn = new wxXmlNode(pNode, wxXML_ELEMENT_NODE, wxT("DiscConnection"));
//		    pDiscConn->AddProperty(wxT("name"), pwxGxDiscConnection->GetName());
//		    pDiscConn->AddProperty(wxT("path"), pwxGxDiscConnection->GetPath());
//        }
    }
    wxXmlNode* pPluginsNode = new wxXmlNode(pNode, wxXML_ELEMENT_NODE, wxT("plugins"));
	for(size_t i = 0; i < m_NetPluginArray.size(); i++)
	{
		wxXmlNode* pXmlNode = m_NetPluginArray[i]->GetProperties();
		if(pXmlNode)
			pXmlNode->SetParent(pPluginsNode);
			//pPluginsNode->InsertChild(pXmlNode, NULL);
	}

#endif  
    return pNode;
}

void wxGxRemoteServers::EmptyChildren(void)
{
	for(size_t i = 0; i < m_Children.size(); i++)
	{
		m_Children[i]->Detach();
		wxDELETE(m_Children[i]);
	}
	m_Children.clear();
	m_bIsChildrenLoaded = false;
}

bool wxGxRemoteServers::DeleteChild(IGxObject* pChild)
{
	bool bHasChildren = m_Children.size() > 0 ? true : false;
    m_pCatalog->ObjectDeleted(pChild);
	if(!IGxObjectContainer::DeleteChild(pChild))
		return false;
	if(bHasChildren != m_Children.size() > 0 ? true : false)
		m_pCatalog->ObjectChanged(this);
	return true;
}

void wxGxRemoteServers::LoadChildren(wxXmlNode* pConf)
{
	if(m_bIsChildrenLoaded)
		return;	
	wxXmlNode* pChild = pConf->GetChildren();
	while(pChild)
	{
		wxGxRemoteServer* pServerConn = new wxGxRemoteServer(pChild);//??? TODO
		IGxObject* pGxObj = static_cast<IGxObject*>(pServerConn);
		if(!AddObject(pGxObj))
			wxDELETE(pGxObj);
		pChild = pChild->GetNext();
	}

	m_bIsChildrenLoaded = true;
}

void wxGxRemoteServers::LoadPlugins(wxXmlNode* pConf)
{
	wxXmlNode* pChild = pConf->GetChildren();
	while(pChild)
	{
        wxString sName = pChild->GetPropVal(wxT("name"), NONAME);
        if(sName.IsEmpty() || sName.CmpNoCase(NONAME) == 0)
        {
            pChild = pChild->GetNext();
            continue;
        }

		wxObject *pObj = wxCreateDynamicObject(sName);
		INetPlugin *pNetPlugin = dynamic_cast<INetPlugin*>(pObj);
	    if(pNetPlugin)
	    {
			pNetPlugin->SetProperties(pChild);
			m_NetPluginArray.push_back(pNetPlugin);
			wxLogMessage(_("wxGxRemoteServers: Network plugin %s loaded"), pNetPlugin->GetName().c_str());
        }
		pChild = pChild->GetNext();
	}
}

void wxGxRemoteServers::UnLoadPlugins()
{
	for(size_t i = 0; i < m_NetPluginArray.size(); i++)
		wxDELETE(m_NetPluginArray[i]);
}
