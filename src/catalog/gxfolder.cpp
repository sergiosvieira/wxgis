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

wxGxFolder::wxGxFolder(void) : wxGxObjectContainer(), m_bIsChildrenLoaded(false)
{
}

wxGxFolder::wxGxFolder(wxGxObject *oParent, const wxString &soName, const CPLString &soPath) : wxGxObjectContainer(oParent, soName, soPath), m_bIsChildrenLoaded(false)
{
}

wxGxFolder::~wxGxFolder(void)
{
}

wxString wxGxFolder::GetBaseName(void) const 
{
    return GetName();
}

void wxGxFolder::Refresh(void)
{
	DestroyChildren();
    m_bIsChildrenLoaded = false;
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

bool wxGxFolder::Rename(const wxString &sNewName)
{
	wxFileName PathName(wxString(m_sPath, wxConvUTF8));
	PathName.SetName(sNewName);

	wxString sNewPath = PathName.GetFullPath();

    CPLString szNewPath(sNewPath.mb_str(wxConvUTF8));
    if(RenameFile(m_sPath, szNewPath))
	{
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

bool wxGxFolder::CanCreate(long nDataType, long DataSubtype)
{
	return wxIsWritable(wxString(m_sPath, wxConvUTF8));
}

bool wxGxFolder::HasChildren(void)
{
    LoadChildren();     
    return wxGxObjectContainer::HasChildren();
}

