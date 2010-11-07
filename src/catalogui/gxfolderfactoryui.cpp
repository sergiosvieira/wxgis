/******************************************************************************
 * Project:  wxGIS (GIS Catalog)
 * Purpose:  wxGxFolderFactoryUI class. Create new GxFolderUI objects
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
#include "wxgis/catalogui/gxfolderfactoryui.h"
#include "wxgis/catalogui/gxarchfolderui.h"

#include "wx/filename.h"
#include "wx/dir.h"

#include "../../art/folder_16.xpm"
#include "../../art/folder_48.xpm"
#include "../../art/folder_arch_16.xpm"
#include "../../art/folder_arch_48.xpm"


IMPLEMENT_DYNAMIC_CLASS(wxGxFolderFactoryUI, wxGxFolderFactory)

wxGxFolderFactoryUI::wxGxFolderFactoryUI(void)
{
    m_oLargeFolderIcon = wxIcon(folder_48_xpm);
    m_oSmallFolderIcon = wxIcon(folder_16_xpm);
    m_oLargeAFolderIcon = wxIcon(folder_arch_48_xpm);
    m_oSmallAFolderIcon = wxIcon(folder_arch_16_xpm);
}

wxGxFolderFactoryUI::~wxGxFolderFactoryUI(void)
{
}

bool wxGxFolderFactoryUI::GetChildren(wxString sParentDir, wxArrayString* pFileNames, GxObjectArray* pObjArray)
{
	for(size_t i = 0; i < pFileNames->GetCount(); i++)
	{
		wxString path = pFileNames->Item(i);

        wxFileName FName(path);
        wxString ext = FName.GetExt().MakeLower();
        FName.ClearExt();
        wxString name = FName.GetName();

		if(wxFileName::DirExists(path))
		{
			wxString sName;
			if(ext.IsEmpty())
				sName = name;
			else
				sName = name + wxT(".") + ext;
			wxGxFolderUI* pFolder = new wxGxFolderUI(path, sName, m_oLargeFolderIcon, m_oSmallFolderIcon);
			IGxObject* pGxObj = static_cast<IGxObject*>(pFolder);
			pObjArray->push_back(pGxObj);
			pFileNames->RemoveAt(i);
			i--;
		}
        else
        {
            //archive in archive not support according performance issue
            wxString sPref = path.Left(4);
            if(ext == wxString(wxT("zip")) && sPref != wxString(wxT("/vsi")))
            {
			    wxGxArchiveUI* pFolder = new wxGxArchiveUI(path, name, wxString(wxT("/vsizip/")), m_oLargeAFolderIcon, m_oSmallAFolderIcon);
			    IGxObject* pGxObj = static_cast<IGxObject*>(pFolder);
			    pObjArray->push_back(pGxObj);
			    pFileNames->RemoveAt(i);
			    i--;
            }
        }
	}
	return true;
}
