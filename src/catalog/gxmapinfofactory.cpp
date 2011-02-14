/******************************************************************************
 * Project:  wxGIS (GIS Catalog)
 * Purpose:  wxGxMapInfoFactory class.
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
#include "wxgis/catalog/gxmapinfofactory.h"
#include "wxgis/catalog/gxdataset.h"
#include <wx/ffile.h>

IMPLEMENT_DYNAMIC_CLASS(wxGxMapInfoFactory, wxObject)

wxGxMapInfoFactory::wxGxMapInfoFactory(void)
{
}

wxGxMapInfoFactory::~wxGxMapInfoFactory(void)
{
}

bool wxGxMapInfoFactory::GetChildren(CPLString sParentDir, char** &pFileNames, GxObjectArray &ObjArray)
{
    for(int i = CSLCount(pFileNames) - 1; i >= 0; i-- )
    {
        IGxObject* pGxObj = NULL;
        CPLString szExt = CPLGetExtension(pFileNames[i]);
        CPLString szPath;

        if(EQUAL(szExt, "tab"))
        {
            bool bHasMap(false), bHasInd(false), bHasID(false), bHasDat(false);
            szPath = (char*)CPLResetExtension(pFileNames[i], "dat");
            if(CPLCheckForFile((char*)szPath.c_str(), NULL))
                bHasDat = true;
            szPath = (char*)CPLResetExtension(pFileNames[i], "id");
            if(CPLCheckForFile((char*)szPath.c_str(), NULL))
                bHasID = true;
            szPath = (char*)CPLResetExtension(pFileNames[i], "ind");
            if(CPLCheckForFile((char*)szPath.c_str(), NULL))
                bHasInd = true;
            szPath = (char*)CPLResetExtension(pFileNames[i], "map");
            if(CPLCheckForFile((char*)szPath.c_str(), NULL))
                bHasMap = true;
            if(bHasMap && bHasMap && bHasID && bHasDat)
                pGxObj = GetGxDataset(pFileNames[i], GetConvName(pFileNames[i]), enumVecMapinfoTab);
            pFileNames = CSLRemoveStrings( pFileNames, i, 1, NULL );
        }
        else if(EQUAL(szExt, "mif"))
        {
            bool bHasMid(false);
            szPath = (char*)CPLResetExtension(pFileNames[i], "mid");
            if(CPLCheckForFile((char*)szPath.c_str(), NULL))
                bHasMid = true;
            if(bHasMid)
                pGxObj = GetGxDataset(pFileNames[i], GetConvName(pFileNames[i]), enumVecMapinfoMif);
            pFileNames = CSLRemoveStrings( pFileNames, i, 1, NULL );
        }
        else if(EQUAL(szExt, "map"))
            pFileNames = CSLRemoveStrings( pFileNames, i, 1, NULL );
        else if(EQUAL(szExt, "ind"))
            pFileNames = CSLRemoveStrings( pFileNames, i, 1, NULL );
        else if(EQUAL(szExt, "id"))
            pFileNames = CSLRemoveStrings( pFileNames, i, 1, NULL );
        else if(EQUAL(szExt, "dat"))
            pFileNames = CSLRemoveStrings( pFileNames, i, 1, NULL );
        else if(EQUAL(szExt, "mid"))
            pFileNames = CSLRemoveStrings( pFileNames, i, 1, NULL );

		if(pGxObj != NULL)
			ObjArray.push_back(pGxObj);
    }
	return true;
}

void wxGxMapInfoFactory::Serialize(wxXmlNode* pConfig, bool bStore)
{
    if(bStore)
    {
        if(pConfig->HasProp(wxT("factory_name")))
            pConfig->DeleteProperty(wxT("factory_name"));
        pConfig->AddProperty(wxT("factory_name"), GetClassName());
        if(pConfig->HasProp(wxT("is_enabled")))
            pConfig->DeleteProperty(wxT("is_enabled"));
        pConfig->AddProperty(wxT("is_enabled"), m_bIsEnabled == true ? wxT("1") : wxT("0"));
    }
    else
    {
        m_bIsEnabled = wxAtoi(pConfig->GetPropVal(wxT("is_enabled"), wxT("1")));
    }
}

IGxObject* wxGxMapInfoFactory::GetGxDataset(CPLString path, wxString name, wxGISEnumVectorDatasetType type)
{
	wxGxFeatureDataset* pDataset = new wxGxFeatureDataset(path, name, type);
    return static_cast<IGxObject*>(pDataset);
}

