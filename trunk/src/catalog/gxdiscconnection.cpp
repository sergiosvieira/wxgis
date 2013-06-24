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
END_EVENT_TABLE()


wxGxDiscConnection::wxGxDiscConnection(void) : wxGxFolder()
{
    m_pWatcher = new wxFileSystemWatcher();
    m_pWatcher->SetOwner(this);
}

wxGxDiscConnection::wxGxDiscConnection(wxGxObject *oParent, const wxString &soXmlConfPath, int nXmlId, const wxString &soName, const CPLString &soPath) : wxGxFolder(oParent, soName, soPath)
{
    m_nXmlId = nXmlId;
    m_soXmlConfPath = soXmlConfPath;
    m_pWatcher = new wxFileSystemWatcher();
    m_pWatcher->SetOwner(this);
}

wxGxDiscConnection::~wxGxDiscConnection(void)
{
}

bool wxGxDiscConnection::Destroy(void)
{
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
    if(!m_pWatcher->Add(oFileName, wxFSW_EVENT_ALL))
    {
        wxLogError(_("Add File system watcher failed"));
    }
    
    //add children
    wxGxObjectList::const_iterator iter;
    for(iter = GetChildren().begin(); iter != GetChildren().end(); ++iter)
    {
        wxGxObject *current = *iter;
        if(current)
        {
            oFileName = wxFileName::DirName(wxString(current->GetPath(), wxConvUTF8));
            if(!m_pWatcher->Add(oFileName, wxFSW_EVENT_ALL))
            {
                wxLogError(_("Add File system watcher failed"));
            }
        }
    }

    wxArrayString saPaths;
    m_pWatcher->GetWatchedPaths(&saPaths);
    for(size_t i = 0; i < saPaths.GetCount(); ++i)
        wxLogDebug(wxT("path = '%s'"), saPaths[i]);
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
            wxGxCatalogBase* pCatalog = GetGxCatalog();
	        if(pCatalog)
            {
                wxArrayLong ChildrenIds;
                pCatalog->GetChildren(parent, papszFileList, ChildrenIds);
                for(size_t i = 0; i < ChildrenIds.GetCount(); ++i)
                    pCatalog->ObjectAdded(ChildrenIds[i]);
                if(ChildrenIds.GetCount() > 0)
                    m_pWatcher->Add(event.GetPath());
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
                m_pWatcher->Remove(event.GetPath());

                current->SetName(event.GetNewPath().GetFullName());
                current->SetPath( CPLString( event.GetNewPath().GetFullPath().mb_str(wxConvUTF8) ) );
                wxGIS_GXCATALOG_EVENT_ID(ObjectChanged, current->GetId());
                m_pWatcher->Add(event.GetNewPath());

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

    m_pWatcher->RemoveAll();
    wxGxFolder::LoadChildren();
    StartWatcher();
}
