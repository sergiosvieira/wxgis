/******************************************************************************
 * Project:  wxGIS (GIS Remote)
 * Purpose:  wxGxRemoteServers class.
 * Author:   Bishop (aka Barishnikov Dmitriy), polimax@mail.ru
 ******************************************************************************
*   Copyright (C) 2010-2011 Bishop
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

#include "wx/volume.h"
#include "wx/dir.h"
#include "wx/stdpaths.h"

IMPLEMENT_DYNAMIC_CLASS(wxGxRemoteServers, wxObject)

wxGxRemoteServers::wxGxRemoteServers(void) : m_bIsChildrenLoaded(false)
{
    m_bEnabled = true;
	wxSocketBase::Initialize();
    wxStandardPaths stp;
    m_sUserConfigDir = stp.GetUserConfigDir() + wxFileName::GetPathSeparator() + wxString(CONFIG_DIR) + wxFileName::GetPathSeparator() + wxString(CONNDIR);
    m_sUserConfig = m_sUserConfigDir + wxFileName::GetPathSeparator() + wxString(RCONNCONF);
}

wxGxRemoteServers::~wxGxRemoteServers(void)
{
}

void wxGxRemoteServers::Detach(void)
{
	EmptyChildren();
	UnLoadFactories();
}

void wxGxRemoteServers::Refresh(void)
{
	//nothing to do
}
 
void wxGxRemoteServers::Init(wxXmlNode* pConfigNode)
{
	wxXmlNode *pFactories(NULL);
	wxXmlNode* pChild = pConfigNode->GetChildren();
	while(pChild)
	{
		if(pChild->GetName().CmpNoCase(wxT("factories")) == 0)
			pFactories = pChild;
		pChild = pChild->GetNext();
	}
    LoadFactories(pFactories);
	LoadChildren();
}

wxXmlNode* wxGxRemoteServers::GetProperties(void)
{
    wxXmlNode* pNode = new wxXmlNode(wxXML_ELEMENT_NODE, wxT("rootitem"));
    wxClassInfo* pInfo = GetClassInfo();
    if(pInfo)
        pNode->AddProperty(wxT("name"), pInfo->GetClassName());
    pNode->AddProperty(wxT("is_enabled"), m_bEnabled == true ? wxT("1") : wxT("0"));    
#ifndef WXGISPORTABLE
    wxXmlNode* pFactoriesNode = new wxXmlNode(pNode, wxXML_ELEMENT_NODE, wxT("factories"));
	for(size_t i = 0; i < m_apNetConnFact.size(); i++)
	{
		wxXmlNode* pXmlNode = m_apNetConnFact[i]->GetProperties();
		if(pXmlNode)
			//pXmlNode->SetParent(pFactoriesNode);
			pFactoriesNode->InsertChild(pXmlNode, NULL);
	}
#endif  
        
    StoreConnections();

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

void wxGxRemoteServers::LoadChildren()
{
	if(m_bIsChildrenLoaded)
		return;	

    wxXmlDocument doc;
    if (doc.Load(m_sUserConfig))
    {
        wxXmlNode* pConnectionsNode = doc.GetRoot();
		wxXmlNode* pConnNode = pConnectionsNode->GetChildren();
		while(pConnNode)
		{
		    wxString sClassName = pConnNode->GetPropVal(wxT("class"), ERR);
		    if(!sClassName.IsEmpty())
		    {
			    INetClientConnection *pConn = dynamic_cast<INetClientConnection*>(wxCreateDynamicObject(sClassName));
			    if(pConn && pConn->SetProperties(pConnNode))
			    {
				    wxGxRemoteServer* pServerConn = new wxGxRemoteServer(pConn);
				    IGxObject* pGxObj = static_cast<IGxObject*>(pServerConn);
				    if(!AddChild(pGxObj))
				    {
					    wxDELETE(pGxObj);
				    }
				    else //set callback
				    {
					    pConn->SetCallback(static_cast<INetCallback*>(pServerConn));
				    }
			    }
		    }

			pConnNode = pConnNode->GetNext();
		}
    }

	m_bIsChildrenLoaded = true;
}

void wxGxRemoteServers::LoadFactories(wxXmlNode* pConf)
{
    if(!pConf)
        return;
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
		INetConnFactory *pNetConnFactory = dynamic_cast<INetConnFactory*>(pObj);
	    if(pNetConnFactory)
	    {
			pNetConnFactory->SetProperties(pChild);
			m_apNetConnFact.push_back(pNetConnFactory);
			wxLogMessage(_("wxGxRemoteServers: Network connection factory %s loaded"), pNetConnFactory->GetName().c_str());
        }
		pChild = pChild->GetNext();
	}
}

void wxGxRemoteServers::UnLoadFactories()
{
	for(size_t i = 0; i < m_apNetConnFact.size(); i++)
		wxDELETE(m_apNetConnFact[i]);
}

void wxGxRemoteServers::StoreConnections(void)
{
    if(!wxDirExists(m_sUserConfigDir))
        wxFileName::Mkdir(m_sUserConfigDir, 0750, wxPATH_MKDIR_FULL);


 //   wxXmlNode* pConsNode = new wxXmlNode(pNode, wxXML_ELEMENT_NODE, wxT("connections"));


    wxXmlDocument doc;
    wxXmlNode* pRootNode = new wxXmlNode(wxXML_ELEMENT_NODE, wxT("RemoteConnections"));
    doc.SetRoot(pRootNode);
	for(size_t i = 0; i < m_Children.size(); i++)
	{
        wxGxRemoteServer* pGxRemoteServer = dynamic_cast<wxGxRemoteServer*>(m_Children[i]);
        if(pGxRemoteServer)
        {
		    wxXmlNode* pConn = pGxRemoteServer->GetProperties();
            if(pConn)
				pRootNode->InsertChild(pConn, NULL);
                //pConn->SetParent(pConsNode);
        }
    }
    doc.Save(m_sUserConfig);
}

