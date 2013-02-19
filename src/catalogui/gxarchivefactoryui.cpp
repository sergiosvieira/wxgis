/******************************************************************************
 * Project:  wxGIS (GIS Catalog)
 * Purpose:  wxGxArchiveFactoryUI class. Create new GxFolderUI objects
 * Author:   Baryshnikov Dmitriy (aka Bishop), polimax@mail.ru
 ******************************************************************************
*   Copyright (C) 2011 Bishop
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
#include "wxgis/catalogui/gxarchivefactoryui.h"
#include "wxgis/catalogui/gxarchfolderui.h"
/*
#include "wx/filename.h"
#include "wx/dir.h"

#include "../../art/folder_arch_16.xpm"
#include "../../art/folder_arch_48.xpm"


IMPLEMENT_DYNAMIC_CLASS(wxGxArchiveFactoryUI, wxGxArchiveFactory)

wxGxArchiveFactoryUI::wxGxArchiveFactoryUI(void)
{
    m_oLargeAFolderIcon = wxIcon(folder_arch_48_xpm);
    m_oSmallAFolderIcon = wxIcon(folder_arch_16_xpm);
}

wxGxArchiveFactoryUI::~wxGxArchiveFactoryUI(void)
{
}

IGxObject* wxGxArchiveFactoryUI::GetGxObject(CPLString szPath, wxString soName)
{
	wxGxArchiveUI* pFolder = new wxGxArchiveUI(szPath, soName, m_oLargeAFolderIcon, m_oSmallAFolderIcon);
	return static_cast<IGxObject*>(pFolder);
}
*/