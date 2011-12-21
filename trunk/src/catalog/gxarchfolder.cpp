/******************************************************************************
 * Project:  wxGIS (GIS Catalog)
 * Purpose:  wxGxArchive classes.
 * Author:   Bishop (aka Baryshnikov Dmitriy), polimax@mail.ru
 ******************************************************************************
*   Copyright (C) 2009,2011  Bishop
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
#include "wxgis/catalog/gxarchfolder.h"
#include "wxgis/datasource/sysop.h"

#include "cpl_vsi_virtual.h"

/////////////////////////////////////////////////////////////////////////
// wxGxArchive
/////////////////////////////////////////////////////////////////////////

wxGxArchive::wxGxArchive(CPLString Path, wxString Name) : wxGxArchiveFolder(Path, Name)
{
}

wxGxArchive::~wxGxArchive(void)
{
}

wxString wxGxArchive::GetBaseName(void)
{
    wxFileName FileName(m_sName);
    FileName.SetEmptyExt();
    return FileName.GetName();
}

bool wxGxArchive::Delete(void)
{
	wxCriticalSectionLocker locker(m_DestructCritSect);
	int nCount = 0;
    for(size_t i = 1; i < m_sPath.length(); ++i)
    {
        nCount++;
        if(m_sPath[i] == '/')
            break;
    }

    m_sPath.erase(0, nCount + 1);

	EmptyChildren();
    if(DeleteFile(m_sPath))
	{
		IGxObjectContainer* pGxObjectContainer = dynamic_cast<IGxObjectContainer*>(m_pParent);
		if(pGxObjectContainer == NULL)
			return false;
		return pGxObjectContainer->DeleteChild(this);
	}
	else
    {
        const char* err = CPLGetLastErrorMsg();
		wxLogError(_("Delete failed! GDAL error: %s, file '%s'"), wxString(err, wxConvUTF8).c_str(), wxString(m_sPath, wxConvUTF8).c_str());
		return false;
    }
}

bool wxGxArchive::Rename(wxString NewName)
{
    CPLString szType("/");
    int nCount = 0;
    for(size_t i = 1; i < m_sPath.length(); ++i)
    {
        nCount++;
        szType += m_sPath[i];
        if(m_sPath[i] == '/')
            break;
    }

    m_sPath.erase(0, nCount + 1);

    NewName = ClearExt(NewName);
	wxFileName PathName(wxString(m_sPath, wxConvUTF8));
	PathName.SetName(NewName);

	wxString sNewPath = PathName.GetFullPath();

	EmptyChildren();
    CPLString szNewPath = sNewPath.mb_str(wxConvUTF8);
    if(RenameFile(m_sPath, szNewPath))
	{
		m_sPath = szType;
        m_sPath += szNewPath;
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

/////////////////////////////////////////////////////////////////////////
// wxGxArchiveFolder
/////////////////////////////////////////////////////////////////////////

wxGxArchiveFolder::wxGxArchiveFolder(CPLString Path, wxString Name) : wxGxFolder(Path, Name)
{
}

wxGxArchiveFolder::~wxGxArchiveFolder(void)
{
}

void wxGxArchiveFolder::LoadChildren(void)
{
	if(m_bIsChildrenLoaded)
		return;
    char **papszItems = CPLReadDir(m_sPath);
    if(papszItems == NULL)
        return;

    char **papszFileList = NULL;
    //remove unused items
    for(int i = CSLCount(papszItems) - 1; i >= 0; i-- )
    {
        if( EQUAL(papszItems[i],".") || EQUAL(papszItems[i],"..") )
            continue;
        CPLString szFileName = m_sPath;
        szFileName += "/";
        szFileName += papszItems[i];

        VSIStatBufL BufL;
        int ret = VSIStatL(szFileName, &BufL);
        if(ret == 0)
        {
            if(VSI_ISDIR(BufL.st_mode))
            {
                wxString sFileName(papszItems[i], wxCSConv(wxT("cp-866")));
				IGxObject* pGxObj = GetArchiveFolder(szFileName, sFileName);
				AddChild(pGxObj);
            }
            else
            {
                papszFileList = CSLAddString( papszFileList, szFileName );
            }
        }
    }
    CSLDestroy( papszItems );

    //load names
	GxObjectArray Array;	
	if(m_pCatalog && m_pCatalog->GetChildren(m_sPath, papszFileList, Array))
	{
		for(size_t i = 0; i < Array.size(); ++i)
		{
			bool ret_code = AddChild(Array[i]);
			if(!ret_code)
				wxDELETE(Array[i]);
		}
	}
    CSLDestroy( papszFileList );
    
	m_bIsChildrenLoaded = true;
}

IGxObject* wxGxArchiveFolder::GetArchiveFolder(CPLString szPath, wxString soName)
{
	wxGxArchiveFolder* pFolder = new wxGxArchiveFolder(szPath, soName);
	return static_cast<IGxObject*>(pFolder);
}

