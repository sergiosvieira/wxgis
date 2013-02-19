/******************************************************************************
 * Project:  wxGIS (GIS Catalog)
 * Purpose:  wxGxFolder class.
 * Author:   Baryshnikov Dmitriy (aka Bishop), polimax@mail.ru
 ******************************************************************************
*   Copyright (C) 2009,2011,2012 Bishop
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
#include "wxgis/catalog/gxfolder.h"
#include "wxgis/catalog/gxcatalog.h"
#include "wxgis/datasource/sysop.h"

//---------------------------------------------------------------------------
// wxGxFolder
//---------------------------------------------------------------------------
IMPLEMENT_CLASS(wxGxFolder, wxGxObjectContainer)

BEGIN_EVENT_TABLE(wxGxFolder, wxGxObjectContainer)
    EVT_FSWATCHER(wxID_ANY, wxGxFolder::OnFileSystemEvent)
END_EVENT_TABLE()

wxGxFolder::wxGxFolder(void) : wxGxObjectContainer(), m_bIsChildrenLoaded(false), m_pWatcher(NULL)
{
}

wxGxFolder::wxGxFolder(wxGxObject *oParent, const wxString &soName, const CPLString &soPath) : wxGxObjectContainer(oParent, soName, soPath), m_bIsChildrenLoaded(false), m_pWatcher(NULL)
{
}

wxGxFolder::~wxGxFolder(void)
{
}

bool wxGxFolder::Destroy(void)
{
    wxDELETE(m_pWatcher);
    return wxGxObjectContainer::Destroy();
}

wxString wxGxFolder::GetBaseName(void) const 
{
    return GetName();
}

void wxGxFolder::Refresh(void)
{
	DestroyChildren();
    m_bIsChildrenLoaded = false;
    wxDELETE(m_pWatcher);
    //delete watcher
	LoadChildren();
    wxGIS_GXCATALOG_EVENT(ObjectRefreshed);
}

void wxGxFolder::LoadChildren(void)
{
	if(m_bIsChildrenLoaded)
		return;

    char **papszItems = CPLReadDir(m_sPath);
    if(papszItems == NULL)
        return;

    char **papszFileList = NULL;    
    wxGxCatalog* pCatalog = wxDynamicCast(GetGxCatalog(), wxGxCatalog);
    
    //remove unused items
    for(int i = CSLCount(papszItems) - 1; i >= 0; i-- )
    {
        if( wxGISEQUAL(papszItems[i], ".") || wxGISEQUAL(papszItems[i], "..") )
            continue;
        CPLString szFileName = CPLFormFilename(m_sPath, papszItems[i], NULL);
        if(pCatalog && !pCatalog->GetShowHidden() && IsFileHidden(szFileName))
            continue;

        if(CSLFindString(papszFileList, szFileName) == -1)
            papszFileList = CSLAddString( papszFileList, szFileName );

    }
    CSLDestroy( papszItems );

    //create children from path and load them

	if(pCatalog)
    {
        wxArrayLong ChildrenIds;
        pCatalog->GetChildren(this, papszFileList, ChildrenIds);
	}
    CSLDestroy( papszFileList );

    //start watcher
    m_pWatcher = new wxFileSystemWatcher();
    m_pWatcher->SetOwner(this);

    wxFileName oFileName = wxFileName::DirName(wxString(m_sPath, wxConvUTF8));
    if(!m_pWatcher->Add(oFileName, wxFSW_EVENT_ALL))
    {
        wxLogError(_("Add File system watcher failed"));
    }

	m_bIsChildrenLoaded = true;
}

bool wxGxFolder::Delete(void)
{
	if(DeleteDir(m_sPath))
	{
        return true;
	}
	else
    {
        const char* err = CPLGetLastErrorMsg();
		wxLogError(_("Delete failed! GDAL error: %s, file '%s'"), wxString(err, wxConvUTF8).c_str(), wxString(m_sPath, wxConvUTF8).c_str());
        return false;
    }
	return false;
}

bool wxGxFolder::Rename(wxString NewName)
{
	wxFileName PathName(wxString(m_sPath, wxConvUTF8));
	PathName.SetName(NewName);

	wxString sNewPath = PathName.GetFullPath();

	//EmptyChildren();
    CPLString szNewPath(sNewPath.mb_str(wxConvUTF8));
    if(RenameFile(m_sPath, szNewPath))
	{
		//m_sPath = szNewPath;
		//m_sName = NewName;
		//m_pCatalog->ObjectChanged(GetID());
		//Refresh();
		return true;
	}
	else
    {
        const char* err = CPLGetLastErrorMsg();
		wxLogError(_("Operation '%s' failed! GDAL error: %s, file '%s'"), _("Rename"), wxString(err, wxConvUTF8).c_str(), wxString(m_sPath, wxConvUTF8).c_str());
		return false;
    }
	return false;
}
/*
bool wxGxFolder::DeleteChild(IGxObject* pChild)
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
*/
bool wxGxFolder::CanCreate(long nDataType, long DataSubtype)
{
	return wxIsWritable(wxString(m_sPath, wxConvUTF8));
}

bool wxGxFolder::HasChildren(void)
{
    LoadChildren();     
    return wxGxObjectContainer::HasChildren();
}

void wxGxFolder::OnFileSystemEvent(wxFileSystemWatcherEvent& event)
{
    //reread conn.xml file
    wxLogDebug(wxT("*** %s ***"), event.ToString().c_str());
    switch(event.GetChangeType())
    {
    case wxFSW_EVENT_CREATE:
        {
            CPLString szPath(event.GetPath().GetFullPath().mb_str(wxConvUTF8));
            char **papszFileList = NULL;  
            papszFileList = CSLAddString( papszFileList, szPath );
            wxGxCatalogBase* pCatalog = GetGxCatalog();
	        if(pCatalog)
            {
                wxArrayLong ChildrenIds;
                pCatalog->GetChildren(this, papszFileList, ChildrenIds);
                for(size_t i = 0; i < ChildrenIds.GetCount(); ++i)
                    pCatalog->ObjectAdded(ChildrenIds[i]);
	        }
            CSLDestroy( papszFileList );
        }
        break;
    case wxFSW_EVENT_DELETE:
        {
            //search gxobject
            wxGxObjectList::const_iterator iter;
            for(iter = GetChildren().begin(); iter != GetChildren().end(); ++iter)
            {
                wxGxObject *current = *iter;
                if(current)
                {
                    if(current->GetName() == event.GetPath().GetFullName())
                    {
                        current->Destroy();
                        break;
                    }
                }
            }
        }
        break;
    case wxFSW_EVENT_RENAME:
        {
            wxGxObjectList::const_iterator iter;
            for(iter = GetChildren().begin(); iter != GetChildren().end(); ++iter)
            {
                wxGxObject *current = *iter;
                if(current)
                {
                    if(current->GetName() == event.GetPath().GetFullName())
                    {
                        current->SetName(event.GetNewPath().GetFullName());
                        current->SetPath( CPLString( event.GetNewPath().GetFullPath().mb_str(wxConvUTF8) ) );
                        wxGIS_GXCATALOG_EVENT_ID(ObjectChanged, current->GetId());
                        break;
                    }
                }
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