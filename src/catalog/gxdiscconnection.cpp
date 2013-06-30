/******************************************************************************
 * Project:  wxGIS (GIS Catalog)
 * Purpose:  wxGxDiscConnection class.
 * Author:   Baryshnikov Dmitriy (aka Bishop), polimax@mail.ru
 ******************************************************************************
*   Copyright (C) 2009-2010,2012,2013  Bishop
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
#include "wxgis/catalog/gxdiscconnection.h"
#include "wxgis/catalog/gxcatalog.h"
#include "wxgis/core/format.h"

//---------------------------------------------------------------------------
// wxGxDiscConnection
//---------------------------------------------------------------------------
IMPLEMENT_DYNAMIC_CLASS(wxGxDiscConnection, wxGxFolder)

BEGIN_EVENT_TABLE(wxGxDiscConnection, wxGxFolder)
    EVT_FSWATCHER(wxID_ANY, wxGxDiscConnection::OnFileSystemEvent)
#ifdef __WXGTK__
    EVT_GXOBJECT_ADDED(wxGxDiscConnection::OnObjectAdded)
#endif
END_EVENT_TABLE()


wxGxDiscConnection::wxGxDiscConnection(void) : wxGxFolder()
{
    m_pWatcher = new wxFileSystemWatcher();
    m_pWatcher->SetOwner(this);
    m_pCatalog = wxDynamicCast(GetGxCatalog(), wxGxCatalog);
#ifdef __WXGTK__
    m_ConnectionPointCatalogCookie = wxNOT_FOUND;

    if(m_pCatalog)
    {
		m_ConnectionPointCatalogCookie = m_pCatalog->Advise(this);
    }
#endif
}

wxGxDiscConnection::wxGxDiscConnection(wxGxObject *oParent, const wxString &soXmlConfPath, int nXmlId, const wxString &soName, const CPLString &soPath) : wxGxFolder(oParent, soName, soPath)
{
    m_nXmlId = nXmlId;
    m_soXmlConfPath = soXmlConfPath;
    m_pWatcher = new wxFileSystemWatcher();
    m_pWatcher->SetOwner(this);
    m_pCatalog = wxDynamicCast(GetGxCatalog(), wxGxCatalog);

#ifdef __WXGTK__
    m_ConnectionPointCatalogCookie = wxNOT_FOUND;

    if(m_pCatalog)
    {
		m_ConnectionPointCatalogCookie = m_pCatalog->Advise(this);
    }
#endif
}

wxGxDiscConnection::~wxGxDiscConnection(void)
{
}

bool wxGxDiscConnection::Destroy(void)
{
#ifdef __WXGTK__
	if(m_ConnectionPointCatalogCookie != wxNOT_FOUND)
        m_pCatalog->Unadvise(m_ConnectionPointCatalogCookie);
#endif

    wxDELETE(m_pWatcher);
    return wxGxObjectContainer::Destroy();
}

bool wxGxDiscConnection::Delete(void)
{
    wxXmlDocument doc;
    //try to load connections xml file
    if(doc.Load(m_soXmlConfPath))
    {
        wxXmlNode* pConnectionsNode = doc.GetRoot();
 		wxXmlNode* pConnectionNode = pConnectionsNode->GetChildren();

		while(pConnectionNode)
		{
            int nNodeXmlId = GetDecimalValue(pConnectionNode, wxT("id"), wxNOT_FOUND);
            if(nNodeXmlId == m_nXmlId)
            {
                if(pConnectionsNode->RemoveChild(pConnectionNode))
                {
                    wxDELETE(pConnectionNode);
                    break;
                }
            }
            pConnectionNode = pConnectionNode->GetNext();
		}
        return doc.Save(m_soXmlConfPath);
    }
	return false;
}

bool wxGxDiscConnection::Rename(const wxString& NewName)
{
    wxXmlDocument doc;
    //try to load connections xml file
    if(doc.Load(m_soXmlConfPath))
    {
        wxXmlNode* pConnectionsNode = doc.GetRoot();
 		wxXmlNode* pConnectionNode = pConnectionsNode->GetChildren();

		while(pConnectionNode)
		{
            int nNodeXmlId = GetDecimalValue(pConnectionNode, wxT("id"), wxNOT_FOUND);
            if(nNodeXmlId == m_nXmlId)
            {
                pConnectionNode->DeleteAttribute(wxT("name"));
                pConnectionNode->AddAttribute(wxT("name"), NewName);
            }
            pConnectionNode = pConnectionNode->GetNext();
		}
        return doc.Save(m_soXmlConfPath);
    }
	return false;
}

void wxGxDiscConnection::StartWatcher(void)
{
    //add itself
    wxFileName oFileName = wxFileName::DirName(wxString(m_sPath, wxConvUTF8));
    wxString sPath = oFileName.GetFullPath();
    if(!IsPathWatched(sPath))
    {
#ifdef __WXGTK__
        if(!m_pWatcher->Add(oFileName, wxFSW_EVENT_ALL))
#else ifdefined __WXMSW__
        if(!m_pWatcher->AddTree(oFileName, wxFSW_EVENT_ALL))
#endif
        {
            wxLogError(_("Add File system watcher failed"));
        }
    }
#ifdef __WXGTK__
    //add children
    wxGxObjectList::const_iterator iter;
    for(iter = GetChildren().begin(); iter != GetChildren().end(); ++iter)
    {
        wxGxObject *current = *iter;
        if(current)
        {
            oFileName = wxFileName::DirName(wxString(current->GetPath(), wxConvUTF8));
            if(!IsPathWatched(oFileName.GetFullPath()))
            {
               if(!m_pWatcher->Add(oFileName, wxFSW_EVENT_ALL))
                {
                    wxLogError(_("Add File system watcher failed"));
                }
            }
        }
    }
#endif
} 

void wxGxDiscConnection::OnFileSystemEvent(wxFileSystemWatcherEvent& event)
{
    wxLogDebug(wxT("*** %s ***"), event.ToString().c_str());
    switch(event.GetChangeType())
    {
    case wxFSW_EVENT_CREATE:
        {
            //get object parent
            wxFileName oName = event.GetPath();
            wxGxObjectContainer *parent = wxDynamicCast(FindGxObjectByPath(oName.GetPath()), wxGxObjectContainer);
            if(!parent)
                return;
            //check doubles
            if(parent->IsNameExist(event.GetPath().GetFullName()))
                return;

            CPLString szPath(event.GetPath().GetFullPath().mb_str(wxConvUTF8));
            char **papszFileList = NULL;  
            papszFileList = CSLAddString( papszFileList, szPath );
	        if(m_pCatalog)
            {
                wxArrayLong ChildrenIds;
                m_pCatalog->CreateChildren(parent, papszFileList, ChildrenIds);
                for(size_t i = 0; i < ChildrenIds.GetCount(); ++i)
                    m_pCatalog->ObjectAdded(ChildrenIds[i]);
	        }
            CSLDestroy( papszFileList );
        }
        break;
    case wxFSW_EVENT_DELETE:
        {
            //search gxobject
            wxGxObject *current = FindGxObjectByPath(event.GetPath().GetFullPath());
            if(current)
            {
                current->Destroy();
                return;
            }
        }
        break;
    case wxFSW_EVENT_RENAME:
        {
            wxGxObject *current = FindGxObjectByPath(event.GetPath().GetFullPath());
            if(current)
            {
                current->SetName(event.GetNewPath().GetFullName());
                current->SetPath( CPLString( event.GetNewPath().GetFullPath().mb_str(wxConvUTF8) ) );
                wxGIS_GXCATALOG_EVENT_ID(ObjectChanged, current->GetId());

#ifdef __WXGTK__
                m_pWatcher->Remove(event.GetPath());
                if(!IsPathWatched(event.GetNewPath().GetFullPath()))
                {
                    m_pWatcher->Add(event.GetNewPath());

                }
#endif
                return;
            }
        }
        break;
    case wxFSW_EVENT_MODIFY:
        break;
    default:
    case wxFSW_EVENT_ACCESS:
    case wxFSW_EVENT_WARNING:
    case wxFSW_EVENT_ERROR:
        break;
    };
}

void wxGxDiscConnection::LoadChildren(void)
{
	if(m_bIsChildrenLoaded)
		return;

    wxGxFolder::LoadChildren();
    StartWatcher();
}

#ifdef __WXGTK__
void wxGxDiscConnection::OnObjectAdded(wxGxCatalogEvent& event)
{
    wxGxObject* pGxObject = m_pCatalog->GetRegisterObject(event.GetObjectID());
	if(!pGxObject)
		return;
    wxString sPath(pGxObject->GetPath(), wxConvUTF8);
    wxString sConnPath(GetPath(), wxConvUTF8);
    if(sPath.StartsWith(sConnPath))
    {
        if(!IsPathWatched(sPath) && pGxObject->IsKindOf(wxCLASSINFO(wxGxFolder)))
        {
            wxFileName oFileName = wxFileName::DirName(sPath);
            m_pWatcher->Add(oFileName);
        }
    }
}
#endif

bool wxGxDiscConnection::IsPathWatched(const wxString& sPath)
{
    wxArrayString sPaths;
    m_pWatcher->GetWatchedPaths(&sPaths);

    return sPaths.Index(sPath, false) != wxNOT_FOUND;
}

void wxGxDiscConnection::Refresh(void)
{
#ifdef __WXGTK__
    m_pWatcher->RemoveAll();
#endif
	DestroyChildren();
    m_bIsChildrenLoaded = false;
	LoadChildren();
    wxGIS_GXCATALOG_EVENT(ObjectRefreshed);
}
    