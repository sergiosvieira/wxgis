/******************************************************************************
 * Project:  wxGIS (GIS Catalog)
 * Purpose:  wxGxFolder class.
 * Author:   Bishop (aka Barishnikov Dmitriy), polimax@mail.ru
 ******************************************************************************
*   Copyright (C) 2009,2011 Bishop
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
#include "wxgis/datasource/sysop.h"

wxGxFolder::wxGxFolder(CPLString Path, wxString Name) : m_bIsChildrenLoaded(false)
{
	m_sName = Name;
	m_sPath = Path;
}

wxGxFolder::~wxGxFolder(void)
{
}

wxString wxGxFolder::GetBaseName(void)
{
    return GetName();
}

void wxGxFolder::Refresh(void)
{
	EmptyChildren();
	LoadChildren();
    m_pCatalog->ObjectRefreshed(GetID());
}

void wxGxFolder::EmptyChildren(void)
{
	for(size_t i = 0; i < m_Children.size(); ++i)
	{
		m_Children[i]->Detach();
		wxDELETE( m_Children[i] );
	}
    m_Children.clear();
	m_bIsChildrenLoaded = false;
}

void wxGxFolder::LoadChildren(void)
{
	if(m_bIsChildrenLoaded)
		return;

    //EmptyChildren();
    char **papszItems = CPLReadDir(m_sPath);
    if(papszItems == NULL)
        return;

    char **papszFileList = NULL;
    //remove unused items
    for(int i = CSLCount(papszItems) - 1; i >= 0; i-- )
    {
        if( EQUAL(papszItems[i],".") || EQUAL(papszItems[i],"..") )
            continue;
        CPLString szFileName = CPLFormFilename(m_sPath, papszItems[i], NULL);
        if(m_pCatalog && !m_pCatalog->GetShowHidden() && IsFileHidden(szFileName))
            continue;
        papszFileList = CSLAddString( papszFileList, szFileName );

    }
    CSLDestroy( papszItems );

    //load names
	size_t nChildrenCount = m_Children.size();
	GxObjectArray Array;	
	if(m_pCatalog && m_pCatalog->GetChildren(m_sPath, papszFileList, Array))
	{
		for(size_t i = 0; i < Array.size(); ++i)
		{
			bool bAdd(true);
			for(size_t j = 0; j < nChildrenCount; ++j)
			{
				if(EQUAL(m_Children[j]->GetInternalName(), Array[i]->GetInternalName()))
					bAdd = false;
			}

			bool ret_code(false);
			if(bAdd)
				ret_code = AddChild(Array[i]);
			if(!ret_code || !bAdd)
				wxDELETE(Array[i]);
		}
	}
    CSLDestroy( papszFileList );
	m_bIsChildrenLoaded = true;
}

bool wxGxFolder::Delete(void)
{
    EmptyChildren();
	if(DeleteDir(m_sPath))
	{
		IGxObjectContainer* pGxObjectContainer = dynamic_cast<IGxObjectContainer*>(m_pParent);
		if(pGxObjectContainer)
    		return pGxObjectContainer->DeleteChild(this);		
	}
	else
    {
        const char* err = CPLGetLastErrorMsg();
		wxLogError(_("Delete failed! GDAL error: %s, file '%s'"), wxString(err, wxConvUTF8).c_str(), wxString(m_sPath, wxConvUTF8).c_str());
    }
	return false;	
}

bool wxGxFolder::Rename(wxString NewName)
{
	wxFileName PathName(wxString(m_sPath, wxConvUTF8));
	PathName.SetName(NewName);

	wxString sNewPath = PathName.GetFullPath();

	EmptyChildren();
    CPLString szNewPath = sNewPath.mb_str(wxConvUTF8);
    if(RenameFile(m_sPath, szNewPath))
	{
		m_sPath = szNewPath;
		m_sName = NewName;
		m_pCatalog->ObjectChanged(GetID());
		Refresh();
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

bool wxGxFolder::CanCreate(long nDataType, long DataSubtype)
{
	return wxIsWritable(wxString(m_sPath, wxConvUTF8));
}
