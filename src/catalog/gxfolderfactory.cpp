/******************************************************************************
 * Project:  wxGIS (GIS Catalog)
 * Purpose:  wxGxFolderFactory class. Create new GxFolder objects
 * Author:   Bishop (aka Barishnikov Dmitriy), polimax@mail.ru
 ******************************************************************************
*   Copyright (C) 2009-2011 Bishop
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

#include "wx/filename.h"
#include "wx/dir.h"

IMPLEMENT_DYNAMIC_CLASS(wxGxFolderFactory, wxObject)

wxGxFolderFactory::wxGxFolderFactory(void)
{
}

wxGxFolderFactory::~wxGxFolderFactory(void)
{
}

bool wxGxFolderFactory::GetChildren(CPLString sParentDir, char** &pFileNames, GxObjectArray &ObjArray)
{
    for(int i = CSLCount(pFileNames) - 1; i >= 0; i-- )
    {
        wxString path(pFileNames[i], wxConvUTF8);
		if(wxFileName::DirExists(path))
		{
            wxFileName FName(path);
            wxString sName = FName.GetFullName();
			IGxObject* pGxObj = GetGxObject(pFileNames[i], sName);
			ObjArray.push_back(pGxObj);
            pFileNames = CSLRemoveStrings( pFileNames, i, 1, NULL );
		}
    }
	return true;
}


void wxGxFolderFactory::Serialize(wxXmlNode* const pConfig, bool bStore)
{
    if(bStore)
    {
        if(pConfig->HasAttribute(wxT("factory_name")))
            pConfig->DeleteAttribute(wxT("factory_name"));
        pConfig->AddAttribute(wxT("factory_name"), GetClassName());  
        if(pConfig->HasAttribute(wxT("is_enabled")))
            pConfig->DeleteAttribute(wxT("is_enabled"));
        pConfig->AddAttribute(wxT("is_enabled"), m_bIsEnabled == true ? wxT("1") : wxT("0"));    
    }
    else
    {
        m_bIsEnabled = wxAtoi(pConfig->GetAttribute(wxT("is_enabled"), wxT("1"))) != 0;
    }
}

IGxObject* wxGxFolderFactory::GetGxObject(CPLString szPath, wxString soName)
{
	wxGxFolder* pFolder = new wxGxFolder(szPath, soName);
	return static_cast<IGxObject*>(pFolder);
}
