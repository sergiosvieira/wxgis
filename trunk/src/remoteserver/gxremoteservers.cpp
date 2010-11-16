/******************************************************************************
 * Project:  wxGIS (GIS Catalog)
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
//#include "wxgis/catalog/gxdiscconnection.h"


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
}

void wxGxRemoteServers::Refresh(void)
{
	EmptyChildren();
	LoadChildren();
    m_pCatalog->ObjectRefreshed(this);
}
 
void wxGxRemoteServers::Init(wxXmlNode* pConfigNode)
{
//	short bScanOnce = wxAtoi(pConfigNode->GetPropVal(wxT("scan_once"), wxT("0")));
//	if(bScanOnce == 0)
//	{
//        wxLogMessage(_("wxGxDiscConnections: Start scan folder connections"));
//        wxArrayString arr;
//#ifdef __WXMSW__
//		arr = wxFSVolumeBase::GetVolumes(wxFS_VOL_MOUNTED, wxFS_VOL_REMOVABLE | wxFS_VOL_REMOTE);
//#else
//        //linux paths
//        wxStandardPaths stp;
//        arr.Add(wxT("/"));
//        arr.Add(stp.GetUserConfigDir());
////      arr.Add(stp.GetDataDir());
//#endif
//        for(size_t i = 0; i < arr.size(); i++)
//		{
//            CONN_DATA data = {arr[i], arr[i]};
//            m_aConnections.push_back(data);
//		}
//	}
//	else
//	{
//		wxXmlNode* pDiscConn = pConfigNode->GetChildren();
//		while(pDiscConn)
//		{
//			wxString sName = pDiscConn->GetPropVal(wxT("name"), NONAME);
//			wxString sPath = pDiscConn->GetPropVal(wxT("path"), ERR);
//			if(sPath != ERR)
//			{
//                CONN_DATA data = {sName, sPath};
//                m_aConnections.push_back(data);
//			}
//			pDiscConn = pDiscConn->GetNext();
//		}
//	}
}

wxXmlNode* wxGxRemoteServers::GetProperties(void)
{
    wxXmlNode* pNode = new wxXmlNode(wxXML_ELEMENT_NODE, wxT("rootitem"));
    wxClassInfo* pInfo = GetClassInfo();
    if(pInfo)
        pNode->AddProperty(wxT("name"), pInfo->GetClassName());
    pNode->AddProperty(wxT("is_enabled"), m_bEnabled == true ? wxT("1") : wxT("0"));    
//#ifndef WXGISPORTABLE
//	for(size_t i = 0; i < m_Children.size(); i++)
//	{
//        wxGxDiscConnection* pwxGxDiscConnection = dynamic_cast<wxGxDiscConnection*>(m_Children[i]);
//        if(pwxGxDiscConnection)
//        {
//		    wxXmlNode* pDiscConn = new wxXmlNode(pNode, wxXML_ELEMENT_NODE, wxT("DiscConnection"));
//		    pDiscConn->AddProperty(wxT("name"), pwxGxDiscConnection->GetName());
//		    pDiscConn->AddProperty(wxT("path"), pwxGxDiscConnection->GetPath());
//        }
//    }
//    if(m_Children.empty() && !m_aConnections.empty())
//    {
//        for(size_t i = 0; i < m_aConnections.size(); i++)
//        {
//		    wxXmlNode* pDiscConn = new wxXmlNode(pNode, wxXML_ELEMENT_NODE, wxT("DiscConnection"));
//		    pDiscConn->AddProperty(wxT("name"), m_aConnections[i].sName);
//		    pDiscConn->AddProperty(wxT("path"), m_aConnections[i].sPath);
//        }
//    }
//#endif  
    return pNode;
}

void wxGxRemoteServers::EmptyChildren(void)
{
 //   m_aConnections.clear();
	//for(size_t i = 0; i < m_Children.size(); i++)
	//{
 //       wxGxDiscConnection* pwxGxDiscConnection = dynamic_cast<wxGxDiscConnection*>(m_Children[i]);
 //       if(pwxGxDiscConnection)
 //       {
 //           CONN_DATA data = {pwxGxDiscConnection->GetName(), pwxGxDiscConnection->GetPath()};
 //           m_aConnections.push_back(data);
 //       }
	//	m_Children[i]->Detach();
	//	wxDELETE(m_Children[i]);
	//}
	//m_Children.clear();
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

void wxGxRemoteServers::LoadChildren(void)
{
	if(m_bIsChildrenLoaded)
		return;	

    //for(size_t i = 0; i < m_aConnections.size(); i++)
    //{
    //    wxGxDiscConnection* pwxGxDiscConnection = new wxGxDiscConnection(m_aConnections[i].sPath, m_aConnections[i].sName);
    //    IGxObject* pGxObject = static_cast<IGxObject*>(pwxGxDiscConnection);
    //    if(AddChild(pGxObject))
    //        wxLogMessage(_("wxGxDiscConnections: Add folder connection [%s]"), m_aConnections[i].sName.c_str());
    //}

	m_bIsChildrenLoaded = true;
}

