/******************************************************************************
 * Project:  wxGIS (GIS Catalog)
 * Purpose:  wxGxFileFactoryUI class.
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

#include "wxgis/catalogui/gxfilefactoryui.h"
#include "wxgis/catalogui/gxfileui.h"

#include "../../art/sr_16.xpm"
#include "../../art/sr_48.xpm"

IMPLEMENT_DYNAMIC_CLASS(wxGxFileFactoryUI, wxGxFileFactory)

wxGxFileFactoryUI::wxGxFileFactoryUI(void)
{
    m_LargePRJIcon = wxIcon(sr_48_xpm);
    m_SmallPRJIcon = wxIcon(sr_16_xpm);
}

wxGxFileFactoryUI::~wxGxFileFactoryUI(void)
{
}

bool wxGxFileFactoryUI::GetChildren(wxString sParentDir, wxArrayString* pFileNames, GxObjectArray* pObjArray)
{
	for(size_t i = 0; i < pFileNames->GetCount(); i++)
	{
		wxString path = pFileNames->Item(i);
		//test is dir
		if(wxFileName::DirExists(path))
			continue;

        wxFileName FName(path);
        wxString ext = FName.GetExt().MakeLower();
        FName.ClearExt();
        wxString name = GetConvName(path, FName);//name conv cp866 if zip

		IGxObject* pGxObj = NULL;
		if(ext == wxString(wxT("spr")))
		{ 
            name += wxT(".") + ext;
			wxGxPrjFileUI* pFile = new wxGxPrjFileUI(path, name, enumSPRfile, m_LargePRJIcon, m_SmallPRJIcon);
			pGxObj = dynamic_cast<IGxObject*>(pFile);
			goto REMOVE;
		}
		if(ext == wxString(wxT("prj")))
		{
			name += wxT(".") + ext;
			wxGxPrjFileUI* pFile = new wxGxPrjFileUI(path, name, enumESRIPrjFile, m_LargePRJIcon, m_SmallPRJIcon);
			pGxObj = dynamic_cast<IGxObject*>(pFile);
			goto REMOVE;
		}
        //add extensions from config
        for(size_t j = 0; j < m_ExtArray.size(); j++)
        {
            if(m_ExtArray[j] == ext)
            {
			    name += wxT(".") + ext;
			    wxGxTextFileUI* pFile = new wxGxTextFileUI(path, name);
			    pGxObj = dynamic_cast<IGxObject*>(pFile);
			    goto REMOVE;
            }
        }
		continue;
REMOVE:
		pFileNames->RemoveAt(i);
		i--;
		if(pGxObj != NULL)
			pObjArray->push_back(pGxObj);
	}

	return true;
}
