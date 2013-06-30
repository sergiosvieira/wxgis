/******************************************************************************
 * Project:  wxGIS (GIS Catalog)
 * Purpose:  wxGxDiscConnections class.
 * Author:   Baryshnikov Dmitriy (aka Bishop), polimax@mail.ru
 ******************************************************************************
*   Copyright (C) 2010-2012 Bishop
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
#include "wxgis/catalog/gxdiscconnections.h"
#include "wxgis/catalog/gxcatalog.h"
#include "wxgis/core/config.h"
#include "wxgis/core/format.h"
#include "wxgis/catalog/gxdiscconnection.h"

#include <wx/volume.h>
#include <wx/dir.h>


IMPLEMENT_DYNAMIC_CLASS(wxGxDiscConnections, wxGxXMLConnectionStorage)

BEGIN_EVENT_TABLE(wxGxDiscConnections, wxGxXMLConnectionStorage)
    EVT_FSWATCHER(wxID_ANY, wxGxXMLConnectionStorage::OnFileSystemEvent)
END_EVENT_TABLE()

wxGxDiscConnections::wxGxDiscConnections(void) : wxGxXMLConnectionStorage(), m_bIsChildrenLoaded(false)
{
    m_sName = wxString(_("Folder connections"));
}

bool wxGxDiscConnections::Create(wxGxObject *oParent, const wxString &soName, const CPLString &soPath)
{
    if( !wxGxObjectContainer::Create(oParent, _("Folder connections"), soPath) )
    {
        wxLogError(_("wxGxDiscConnections::Create failed. GxObject %s"), wxString(_("Folder connections")).c_str());
        return false;
    }
    //get config path
    m_sXmlStorageName = wxString(CONNCONF);
	wxGISAppConfig oConfig = GetConfig();
	if(oConfig.IsOk())
	{
		m_sUserConfigDir = oConfig.GetLocalConfigDirNonPortable() + wxFileName::GetPathSeparator() + wxString(CONNDIR);
		m_sXmlStoragePath = m_sUserConfigDir + wxFileName::GetPathSeparator() + m_sXmlStorageName;
	}
    m_pWatcher = new wxFileSystemWatcher();
    m_pWatcher->SetOwner(this);

    wxFileName oFileName = wxFileName::DirName(m_sUserConfigDir);
    //if dir is not exist create it
    if(!wxDirExists(m_sUserConfigDir))
        wxFileName::Mkdir(m_sUserConfigDir, 0755, wxPATH_MKDIR_FULL);

    wxLogDebug(wxT("monitoring dir is: %s"), oFileName.GetFullPath().c_str());
    if(!m_pWatcher->Add(oFileName, wxFSW_EVENT_MODIFY))//bool bAdd = |wxFSW_EVENT_CREATE
    {
        wxLogError(_("Add File system watcher failed"));
        return false;
    }

    return true;
}

wxGxDiscConnections::~wxGxDiscConnections(void)
{
}

bool wxGxDiscConnections::Destroy(void)
{
    wxDELETE(m_pWatcher);
    return wxGxXMLConnectionStorage::Destroy();
}

void wxGxDiscConnections::Refresh(void)
{
    if(DestroyChildren())
        LoadConnectionsStorage();
    else
        wxGxXMLConnectionStorage::Refresh();
}

void wxGxDiscConnections::Init(wxXmlNode* const pConfigNode)
{
    LoadConnectionsStorage();
}

void wxGxDiscConnections::Serialize(wxXmlNode* const pConfigNode)
{
    //Nothing to do. We edit connections xml from other functions
    //StoreConnections();
}

bool wxGxDiscConnections::IsObjectExist(wxGxObject* const pObj, const wxXmlNode* pNode)
{
    int nXmlId = GetDecimalValue(pNode, wxT("id"), wxNOT_FOUND);
    wxGxDiscConnection* pConn = wxDynamicCast(pObj, wxGxDiscConnection);
    if(!pConn)
        return false;

    if(pConn->GetXmlId() != nXmlId)
        return false;

    //if exist control name and path

    wxString sName = pNode->GetAttribute(wxT("name"), NONAME);
	wxString sPath = pNode->GetAttribute(wxT("path"), NONAME);
    CPLString szPath(sPath.mb_str(wxConvUTF8));

    if(pObj->GetPath() != szPath)
    {
        pObj->SetPath(szPath);
        //ObjectChanged event
        wxGIS_GXCATALOG_EVENT_ID(ObjectRefreshed, pObj->GetId());
    }

    if(!pObj->GetName().IsSameAs( sName, false))
    {
        pObj->SetName(sName);
        //ObjectChanged event
        wxGIS_GXCATALOG_EVENT_ID(ObjectChanged, pObj->GetId());
    }
    return true;
}

void wxGxDiscConnections::CreateConnectionsStorage(void)
{
    wxLogMessage(_("wxGxDiscConnections: Start scan folder connections"));
    wxArrayString arr;
#ifdef __WXMSW__
	arr = wxFSVolumeBase::GetVolumes(wxFS_VOL_MOUNTED, wxFS_VOL_REMOVABLE);//| wxFS_VOL_REMOTE
#else
    //linux paths
    wxStandardPaths stp;
    arr.Add(wxT("/"));
    arr.Add(stp.GetUserConfigDir());
//      arr.Add(stp.GetDataDir());
#endif
    //create
    wxXmlNode* pRootNode = new wxXmlNode(wxXML_ELEMENT_NODE, wxT("DiscConnections"));
    SetDecimalValue(pRootNode, wxT("ver"), 2);
    wxXmlDocument doc;
    doc.SetRoot(pRootNode);
    for(size_t i = 0; i < arr.size(); ++i)
	{
 	    wxXmlNode* pDiscConn = new wxXmlNode(pRootNode, wxXML_ELEMENT_NODE, wxT("DiscConnection"));
	    pDiscConn->AddAttribute(wxT("name"), arr[i]);
	    pDiscConn->AddAttribute(wxT("path"), arr[i]);
        SetDecimalValue(pDiscConn, wxT("id"), (int)i);
	}
    doc.Save(m_sXmlStoragePath);
}

wxGxObject* wxGxDiscConnections::CreateChildGxObject(const wxXmlNode* pNode)
{
    wxString soName = pNode->GetAttribute(wxT("name"), NONAME);
    wxString sPath = pNode->GetAttribute(wxT("path"), NONAME);
    CPLString soPath(sPath.mb_str(wxConvUTF8));
    int nXmlId = GetDecimalValue(pNode, wxT("id"), wxNOT_FOUND);
    if(sPath == NONAME || nXmlId == wxNOT_FOUND)
        return NULL;
    return new wxGxDiscConnection(this, m_sXmlStoragePath, nXmlId, soName, soPath);
}

/*
void wxGxDiscConnections::EmptyChildren(void)
{
    m_aConnections.clear();
	for(size_t i = 0; i < m_Children.size(); ++i)
	{
        wxGxDiscConnection* pwxGxDiscConnection = dynamic_cast<wxGxDiscConnection*>(m_Children[i]);
        if(pwxGxDiscConnection)
        {
            CONN_DATA data = {pwxGxDiscConnection->GetName(), pwxGxDiscConnection->GetInternalName()};
            m_aConnections.push_back(data);
        }
		m_Children[i]->Detach();
		wxDELETE(m_Children[i]);
	}
	m_Children.clear();
	m_bIsChildrenLoaded = false;
}

bool wxGxDiscConnections::DeleteChild(IGxObject* pChild)
{
	bool bHasChildren = m_Children.size() > 0 ? true : false;
    long nChildID = pChild->GetID();
	if(!IGxObjectContainer::DeleteChild(pChild))
		return false;
    m_pCatalog->ObjectDeleted(nChildID);
	if(bHasChildren != m_Children.size() > 0 ? true : false)
		m_pCatalog->ObjectChanged(GetID());
	return true;
}

void wxGxDiscConnections::LoadChildren(void)
{
	if(m_bIsChildrenLoaded)
		return;

    for(size_t i = 0; i < m_aConnections.size(); ++i)
    {
        wxGxDiscConnection* pwxGxDiscConnection = new wxGxDiscConnection(m_aConnections[i].sPath, m_aConnections[i].sName);
        IGxObject* pGxObject = static_cast<IGxObject*>(pwxGxDiscConnection);
        if(AddChild(pGxObject))
            wxLogVerbose(_("wxGxDiscConnections: Add folder connection [%s]"), m_aConnections[i].sName.c_str());
    }

	m_bIsChildrenLoaded = true;
}
*/
bool wxGxDiscConnections::ConnectFolder(const wxString &sPath)
{
    wxCriticalSectionLocker locker(m_oCritSect);
    if(!wxDir::Exists(sPath))
        return false;
    //find max id
    int nMaxId(0);
	wxGxObjectList ObjectList = GetChildren();
    wxGxObjectList::iterator iter;
    for (iter = ObjectList.begin(); iter != ObjectList.end(); ++iter)
    {
        wxGxObject *current = *iter;
		wxGxDiscConnection* pConn = wxDynamicCast(current, wxGxDiscConnection);
        if(!pConn)
            continue;
        if(nMaxId < pConn->GetXmlId())
            nMaxId = pConn->GetXmlId();
    }
    //add
    wxXmlDocument doc;
    //try to load connections xml file
    if(doc.Load(m_sXmlStoragePath))
    {
        wxXmlNode* pConnectionsNode = doc.GetRoot();
		wxXmlNode* pDiscConn = new wxXmlNode(pConnectionsNode, wxXML_ELEMENT_NODE, wxT("DiscConnection"));

	    pDiscConn->AddAttribute(wxT("name"), sPath);
	    pDiscConn->AddAttribute(wxT("path"), sPath);
        SetDecimalValue(pDiscConn, wxT("id"), nMaxId + 1);

        return doc.Save(m_sXmlStoragePath);
    }
    return false;
}

bool wxGxDiscConnections::DisconnectFolder(int nXmlId)
{
    wxCriticalSectionLocker locker(m_oCritSect);

    wxXmlDocument doc;
    //try to load connections xml file
    if(doc.Load(m_sXmlStoragePath))
    {
        wxXmlNode* pConnectionsNode = doc.GetRoot();
 		wxXmlNode* pConnectionNode = pConnectionsNode->GetChildren();

		while(pConnectionNode)
		{
            wxXmlNode* pCurConnectionNode = pConnectionNode;
            int nNodeXmlId = GetDecimalValue(pCurConnectionNode, wxT("id"), wxNOT_FOUND);
            pConnectionNode = pConnectionNode->GetNext();
            if(nNodeXmlId == nXmlId)
            {
                if(pConnectionsNode->RemoveChild(pCurConnectionNode))
                {
                    wxDELETE(pCurConnectionNode);
                    break;
                }
            }
		}
        return doc.Save(m_sXmlStoragePath);
    }
    return false;
}

/*
void wxGxDiscConnections::StoreConnections(void)
{
//#ifndef WXGISPORTABLE
    if(!wxDirExists(m_sUserConfigDir))
        wxFileName::Mkdir(m_sUserConfigDir, 0750, wxPATH_MKDIR_FULL);

    wxXmlDocument doc;
    wxXmlNode* pRootNode = new wxXmlNode(wxXML_ELEMENT_NODE, wxT("DiscConnections"));
    doc.SetRoot(pRootNode);
    for(size_t i = 0; i < m_Children.size(); ++i)
    {
        wxGxDiscConnection* pwxGxDiscConnection = dynamic_cast<wxGxDiscConnection*>(m_Children[i]);
        if(pwxGxDiscConnection)
        {
	        wxXmlNode* pDiscConn = new wxXmlNode(pRootNode, wxXML_ELEMENT_NODE, wxT("DiscConnection"));
	        pDiscConn->AddAttribute(wxT("name"), pwxGxDiscConnection->GetName());
	        pDiscConn->AddAttribute(wxT("path"), wxString(pwxGxDiscConnection->GetInternalName(), wxConvUTF8));
        }
    }
    if(m_Children.empty() && !m_aConnections.empty())
    {
        for(size_t i = 0; i < m_aConnections.size(); ++i)
        {
	        wxXmlNode* pDiscConn = new wxXmlNode(pRootNode, wxXML_ELEMENT_NODE, wxT("DiscConnection"));
	        pDiscConn->AddAttribute(wxT("name"), m_aConnections[i].sName);
	        pDiscConn->AddAttribute(wxT("path"), wxString(m_aConnections[i].sPath, wxConvUTF8));
        }
    }
    doc.Save(m_sUserConfig);
//#endif
}
*/

