/******************************************************************************
 * Project:  wxGIS (GIS Catalog)
 * Purpose:  wxGxSpatialReferencesFolderUI classes.
 * Author:   Bishop (aka Barishnikov Dmitriy), polimax@mail.ru
 ******************************************************************************
*   Copyright (C) 2009-2010  Bishop
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

#include "wxgis/catalogui/gxspatreffolderui.h"
#include "wxgis/catalogui/gxcatalogui.h"

#include "../../art/folder_prj_16.xpm"
#include "../../art/folder_prj_48.xpm"

/////////////////////////////////////////////////////////////////////////
// wxGxSpatialReferencesFolder
/////////////////////////////////////////////////////////////////////////
IMPLEMENT_DYNAMIC_CLASS(wxGxSpatialReferencesFolderUI, wxGxSpatialReferencesFolder)

wxGxSpatialReferencesFolderUI::wxGxSpatialReferencesFolderUI(void) : wxGxSpatialReferencesFolder()
{
    m_LargeIcon = wxIcon(folder_prj_48_xpm);
    m_SmallIcon = wxIcon(folder_prj_16_xpm);
}

wxGxSpatialReferencesFolderUI::~wxGxSpatialReferencesFolderUI(void)
{
}

wxIcon wxGxSpatialReferencesFolderUI::GetLargeImage(void)
{
	return m_LargeIcon;
}

wxIcon wxGxSpatialReferencesFolderUI::GetSmallImage(void)
{
	return m_SmallIcon;
}

void wxGxSpatialReferencesFolderUI::EmptyChildren(void)
{
	for(size_t i = 0; i < m_Children.size(); i++)
	{
        wxGxCatalogUI* pCatalog = dynamic_cast<wxGxCatalogUI*>(m_pCatalog);
        if(pCatalog)
        {
            IGxSelection* pSel = pCatalog->GetSelection();
            if(pSel)
                pSel->Unselect(m_Children[i], IGxSelection::INIT_ALL);
        }
		m_Children[i]->Detach();
		wxDELETE(m_Children[i]);
	}
	m_Children.clear();
	m_bIsChildrenLoaded = false;
}

void wxGxSpatialReferencesFolderUI::LoadChildren(void)
{
	if(m_bIsChildrenLoaded)
		return;	

    wxBusyCursor wait;
    //VSIFilesystemHandler *poFSHandler = VSIFileManager::GetHandler( wgWX2MB(m_sPath) );
    //char **res = poFSHandler->ReadDir(wgWX2MB(m_sPath));

    char **papszFileList = VSIReadDir(wgWX2MB(m_sPath));

    if( CSLCount(papszFileList) == 0 )
    {
        wxLogMessage(wxT( "wxGxSpatialReferencesFolder: no files or directories" ));
    }
    else
    {
        //wxLogDebug(wxT("Files: %s"), wgMB2WX(papszFileList[0]) );
       	//wxArrayString FileNames;
        for(int i = 0; papszFileList[i] != NULL; i++ )
		{
			wxString sFileName = wgMB2WX(papszFileList[i]);
            //if(i > 0)
            //    wxLogDebug( wxT("       %s"), sFileName.c_str() );
            VSIStatBufL BufL;
			wxString sFolderPath = m_sPath + wxT("/") + sFileName;
            int ret = VSIStatL(wgWX2MB(sFolderPath), &BufL);
            if(ret == 0)
            {
                //int x = 0;
                if(VSI_ISDIR(BufL.st_mode))
                {
					wxGxPrjFolderUI* pFolder = new wxGxPrjFolderUI(sFolderPath, wxGetTranslation(sFileName), m_LargeIcon, m_SmallIcon);
					IGxObject* pGxObj = static_cast<IGxObject*>(pFolder);
					bool ret_code = AddChild(pGxObj);
                }
                else
                {
                    m_FileNames.Add(sFolderPath);
                }
            }
		}
    }
    CSLDestroy( papszFileList );

	//load names
	GxObjectArray Array;	
	if(m_pCatalog->GetChildren(m_sPath, &m_FileNames, &Array))
	{
		for(size_t i = 0; i < Array.size(); i++)
		{
			bool ret_code = AddChild(Array[i]);
			if(!ret_code)
				wxDELETE(Array[i]);
		}
	}
	m_bIsChildrenLoaded = true;
}

/////////////////////////////////////////////////////////////////////////
// wxGxPrjFolderUI
/////////////////////////////////////////////////////////////////////////

wxGxPrjFolderUI::wxGxPrjFolderUI(wxString Path, wxString Name, wxIcon LargeIcon, wxIcon SmallIcon) : wxGxArchiveFolderUI(Path, Name)
{
    m_LargeIcon = LargeIcon;
    m_SmallIcon = SmallIcon;
}

wxGxPrjFolderUI::~wxGxPrjFolderUI(void)
{
}

wxIcon wxGxPrjFolderUI::GetLargeImage(void)
{
	return m_LargeIcon;
}

wxIcon wxGxPrjFolderUI::GetSmallImage(void)
{
	return m_SmallIcon;
}

void wxGxPrjFolderUI::LoadChildren(void)
{
	if(m_bIsChildrenLoaded)
		return;

    wxBusyCursor wait;
    //VSIFilesystemHandler *poFSHandler = VSIFileManager::GetHandler( wgWX2MB(m_sPath) );
    //char **res = poFSHandler->ReadDir(wgWX2MB(m_sPath));

    wxString sArchPath = m_sPath;
    char **papszFileList = VSIReadDir(wgWX2MB(sArchPath));

    if( CSLCount(papszFileList) == 0 )
    {
        wxLogMessage(wxT( "wxGxPrjFolder: no files or directories" ));
    }
    else
    {
        //wxLogDebug(wxT("Files: %s"), wgMB2WX(papszFileList[0]) );
       	//wxArrayString FileNames;
        for(int i = 0; papszFileList[i] != NULL; i++ )
		{
			wxString sFileName = wgMB2WX(papszFileList[i]);
            //if(i > 0)
            //    wxLogDebug( wxT("       %s"), sFileName.c_str() );
            VSIStatBufL BufL;
			wxString sFolderPath = sArchPath + wxT("/") + sFileName;
            int ret = VSIStatL(wgWX2MB(sFolderPath), &BufL);
            if(ret == 0)
            {
                //int x = 0;
                if(VSI_ISDIR(BufL.st_mode))
                {
					wxGxPrjFolderUI* pFolder = new wxGxPrjFolderUI(sFolderPath, sFileName, m_LargeIcon, m_SmallIcon);
					IGxObject* pGxObj = static_cast<IGxObject*>(pFolder);
					bool ret_code = AddChild(pGxObj);
                }
                else
                {
                    m_FileNames.Add(sFolderPath);
                }
            }
		}
    }
    CSLDestroy( papszFileList );

	//load names
	GxObjectArray Array;	
	if(m_pCatalog->GetChildren(sArchPath, &m_FileNames, &Array))
	{
		for(size_t i = 0; i < Array.size(); i++)
		{
			bool ret_code = AddChild(Array[i]);
			if(!ret_code)
				wxDELETE(Array[i]);
		}
	}
	m_bIsChildrenLoaded = true;
}