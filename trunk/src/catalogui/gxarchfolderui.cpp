/******************************************************************************
 * Project:  wxGIS (GIS Catalog)
 * Purpose:  wxGxArchive classes.
 * Author:   Bishop (aka Baryshnikov Dmitriy), polimax@mail.ru
 ******************************************************************************
*   Copyright (C) 2010-2011 Bishop
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

#include "cpl_vsi_virtual.h"

/////////////////////////////////////////////////////////////////////////
// wxGxArchiveUI
/////////////////////////////////////////////////////////////////////////

wxGxArchiveUI::wxGxArchiveUI(CPLString Path, wxString Name, wxIcon LargeIcon, wxIcon SmallIcon) : wxGxArchive(Path, Name)
{
    m_oLargeIcon = LargeIcon;
    m_oSmallIcon = SmallIcon;
}

wxGxArchiveUI::~wxGxArchiveUI(void)
{
}

wxIcon wxGxArchiveUI::GetLargeImage(void)
{
	return m_oLargeIcon;
}

wxIcon wxGxArchiveUI::GetSmallImage(void)
{
	return m_oSmallIcon;
}

IGxObject* wxGxArchiveUI::GetArchiveFolder(CPLString szPath, wxString soName)
{
	wxGxArchiveFolderUI* pFolder = new wxGxArchiveFolderUI(szPath, soName, m_oLargeIcon, m_oSmallIcon);
	return static_cast<IGxObject*>(pFolder);
}

void wxGxArchiveUI::EditProperties(wxWindow *parent)
{
}

/////////////////////////////////////////////////////////////////////////
// wxGxArchiveFolderUI
/////////////////////////////////////////////////////////////////////////

wxGxArchiveFolderUI::wxGxArchiveFolderUI(CPLString Path, wxString Name, wxIcon LargeIcon, wxIcon SmallIcon) : wxGxArchiveFolder(Path, Name)
{
    m_oLargeIcon = LargeIcon;
    m_oSmallIcon = SmallIcon;
}

wxGxArchiveFolderUI::~wxGxArchiveFolderUI(void)
{
}

wxIcon wxGxArchiveFolderUI::GetLargeImage(void)
{
	return m_oLargeIcon;
}

wxIcon wxGxArchiveFolderUI::GetSmallImage(void)
{
	return m_oSmallIcon;
}

IGxObject* wxGxArchiveFolderUI::GetArchiveFolder(CPLString szPath, wxString soName)
{
	wxGxArchiveFolderUI* pFolder = new wxGxArchiveFolderUI(szPath, soName, m_oLargeIcon, m_oSmallIcon);
	return static_cast<IGxObject*>(pFolder);
}

