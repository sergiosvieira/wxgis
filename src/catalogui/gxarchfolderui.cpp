/******************************************************************************
 * Project:  wxGIS (GIS Catalog)
 * Purpose:  wxGxArchive classes.
 * Author:   Bishop (aka Barishnikov Dmitriy), polimax@mail.ru
 ******************************************************************************
*   Copyright (C) 2010  Bishop
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
#include "wxgis/catalogui/gxarchfolderui.h"
#include "wxgis/datasource/sysop.h"

#include "../../art/folder_arch_48.xpm"
#include "../../art/folder_arch_16.xpm"

#include "cpl_vsi_virtual.h"

/////////////////////////////////////////////////////////////////////////
// wxGxArchiveUI
/////////////////////////////////////////////////////////////////////////

wxGxArchiveUI::wxGxArchiveUI(wxString Path, wxString Name, wxString sType, wxIcon LargeIcon, wxIcon SmallIcon) : wxGxArchive(Path, Name, sType)
{
    m_oLargeIcon = LargeIcon;
    m_oSmallIcon = SmallIcon;
}

wxGxArchiveUI::~wxGxArchiveUI(void)
{
}

wxIcon wxGxArchiveUI::GetLargeImage(void)
{
	return wxIcon(folder_arch_48_xpm);
}

wxIcon wxGxArchiveUI::GetSmallImage(void)
{
	return wxIcon(folder_arch_16_xpm);
}

void wxGxArchiveUI::LoadChildren(void)
{
	if(m_bIsChildrenLoaded)
		return;

    wxBusyCursor wait;

    wxString sArchPath = m_sType + m_sPath;//wxT("/vsizip/") + wxT("/");
    CPLString soArchPath(sArchPath.mb_str(wxConvUTF8));
    char **papszFileList = VSIReadDir(soArchPath);

    if( CSLCount(papszFileList) == 0 )
    {
        wxLogMessage(wxT( "wxGxArchive: no files or directories" ));
    }
    else
    {
       	//wxArrayString FileNames;
        for(int i = 0; papszFileList[i] != NULL; i++ )
		{
            wxString sFileName(papszFileList[i], wxCSConv(wxT("cp-866")));
            VSIStatBufL BufL;

            CPLString soArchPathF = soArchPath;
            soArchPathF += "/";
            soArchPathF += papszFileList[i];

            int ret = VSIStatL(soArchPathF, &BufL);
            if(ret == 0)
            {
				wxString sFolderPath(soArchPathF, wxConvLocal);// = wgMB2WXsArchPath + wxT("/") + sFileName;

                if(VSI_ISDIR(BufL.st_mode))
                {
					wxGxArchiveFolderUI* pFolder = new wxGxArchiveFolderUI(sFolderPath, sFileName);
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

void wxGxArchiveUI::EditProperties(wxWindow *parent)
{
}

/////////////////////////////////////////////////////////////////////////
// wxGxArchiveFolderUI
/////////////////////////////////////////////////////////////////////////

wxGxArchiveFolderUI::wxGxArchiveFolderUI(wxString Path, wxString Name, wxIcon LargeIcon, wxIcon SmallIcon) : wxGxArchiveFolder(Path, Name)
{
    m_oLargeIcon = LargeIcon;
    m_oSmallIcon = SmallIcon;
}

wxGxArchiveFolderUI::~wxGxArchiveFolderUI(void)
{
}

wxIcon wxGxArchiveFolderUI::GetLargeImage(void)
{
	return wxIcon(folder_arch_48_xpm);
}

wxIcon wxGxArchiveFolderUI::GetSmallImage(void)
{
	return wxIcon(folder_arch_16_xpm);
}

void wxGxArchiveFolderUI::LoadChildren(void)
{
	if(m_bIsChildrenLoaded)
		return;

    wxBusyCursor wait;

    wxString sArchPath = m_sPath;
    CPLString soArchPath(sArchPath.mb_str(wxConvUTF8));//wgWX2MB(sArchPath));
    char **papszFileList = VSIReadDir(soArchPath);

    if( CSLCount(papszFileList) == 0 )
    {
        wxLogMessage(wxT( "wxGxArchive: no files or directories" ));
    }
    else
    {
        for(int i = 0; papszFileList[i] != NULL; i++ )
		{
            wxString sFileName(papszFileList[i], wxCSConv(wxT("cp-866")));

            VSIStatBufL BufL;
            CPLString soArchPathF = soArchPath;
            soArchPathF += "/";
            soArchPathF += papszFileList[i];


            int ret = VSIStatL(soArchPathF, &BufL);
            if(ret == 0)
            {
				wxString sFolderPath(soArchPathF, wxConvLocal);

                if(VSI_ISDIR(BufL.st_mode))
                {
					wxGxArchiveFolderUI* pFolder = new wxGxArchiveFolderUI(sFolderPath, sFileName);
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

