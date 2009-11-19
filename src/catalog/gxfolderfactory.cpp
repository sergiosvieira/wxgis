/******************************************************************************
 * Project:  wxGIS (GIS Catalog)
 * Purpose:  wxGxFolderFactory class. Create new GxFolder objects
 * Author:   Bishop (aka Barishnikov Dmitriy), polimax@mail.ru
 ******************************************************************************
*   Copyright (C) 2009  Bishop
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
#include "wxgis/catalog/gxfolderfactory.h"
#include "wxgis/catalog/gxfolder.h"
#include <wx/filename.h>
#include <wx/dir.h>


IMPLEMENT_DYNAMIC_CLASS(wxGxFolderFactory, wxObject)

wxGxFolderFactory::wxGxFolderFactory(void)
{
}

wxGxFolderFactory::~wxGxFolderFactory(void)
{
}

bool wxGxFolderFactory::GetChildren(wxString sParentDir, wxArrayString* pFileNames, GxObjectArray* pObjArray)
{
	for(size_t i = 0; i < pFileNames->GetCount(); i++)
	{
		wxString path = pFileNames->Item(i);
		if(wxFileName::DirExists(path))
		{
			//wxDir dir(path);
			//wxFileName dir = wxFileName::DirName(path);
			wxString name, ext;
			wxFileName::SplitPath(path, NULL, NULL, &name, &ext);
			//wxString name = dir.GetName();
			wxGxFolder* pFolder = new wxGxFolder(path, name + ext, m_pCatalog->GetShowHidden());
			IGxObject* pGxObj = static_cast<IGxObject*>(pFolder);
			pObjArray->push_back(pGxObj);
			pFileNames->RemoveAt(i);
			i--;
		}
	}
	return true;
}