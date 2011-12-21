/******************************************************************************
 * Project:  wxGIS (GIS Catalog)
 * Purpose:  wxGxPrjFactory class.
 * Author:   Bishop (aka Baryshnikov Dmitriy), polimax@mail.ru
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
#include "wxgis/catalog/gxprjfactory.h"
#include "wxgis/catalog/gxfile.h"

#include "wx/filename.h"
#include "wx/dir.h"

IMPLEMENT_DYNAMIC_CLASS(wxGxPrjFactory, wxObject)

wxGxPrjFactory::wxGxPrjFactory(void)
{
}

wxGxPrjFactory::~wxGxPrjFactory(void)
{
}

bool wxGxPrjFactory::GetChildren(CPLString sParentDir, char** &pFileNames, GxObjectArray &ObjArray)
{
    for(int i = CSLCount(pFileNames) - 1; i >= 0; i-- )
    {
        CPLString szExt = CPLGetExtension(pFileNames[i]);
        if(EQUAL(szExt, "prj"))
        {
            int nRes(0);
            nRes += CPLCheckForFile((char*)CPLResetExtension(pFileNames[i], "shp"), NULL);
            nRes += CPLCheckForFile((char*)CPLResetExtension(pFileNames[i], "bmp"), NULL);
            nRes += CPLCheckForFile((char*)CPLResetExtension(pFileNames[i], "gif"), NULL);
            nRes += CPLCheckForFile((char*)CPLResetExtension(pFileNames[i], "jpg"), NULL);
            nRes += CPLCheckForFile((char*)CPLResetExtension(pFileNames[i], "png"), NULL);

            if(nRes == 0)
            {
                IGxObject* pGxObj = GetGxObject(pFileNames[i], GetConvName(pFileNames[i]), enumESRIPrjFile);
                ObjArray.push_back(pGxObj);
            }

            pFileNames = CSLRemoveStrings( pFileNames, i, 1, NULL );
            continue;
        }
        else if(EQUAL(szExt, "spr"))
        {
            IGxObject* pGxObj = GetGxObject(pFileNames[i], GetConvName(pFileNames[i]), enumSPRfile);
            ObjArray.push_back(pGxObj);

            pFileNames = CSLRemoveStrings( pFileNames, i, 1, NULL );
            continue;
        }
    }
	return true;
}


void wxGxPrjFactory::Serialize(wxXmlNode* const pConfig, bool bStore)
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

IGxObject* wxGxPrjFactory::GetGxObject(CPLString szPath, wxString soName, wxGISEnumPrjFileType nType)
{
    wxGxPrjFile* pFile = new wxGxPrjFile(szPath, soName, nType);
	return static_cast<IGxObject*>(pFile);
}
