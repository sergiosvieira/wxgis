/******************************************************************************
 * Project:  wxGIS (GIS Catalog)
 * Purpose:  wxGxShapeFactory class.
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
#include "wxgis/catalog/gxshapefactory.h"
#include "wxgis/catalog/gxdataset.h"
#include <wx/ffile.h>

IMPLEMENT_DYNAMIC_CLASS(wxGxShapeFactory, wxObject)

wxGxShapeFactory::wxGxShapeFactory(void)
{
}

wxGxShapeFactory::~wxGxShapeFactory(void)
{
}

bool wxGxShapeFactory::GetChildren(CPLString sParentDir, char** &pFileNames, GxObjectArray &ObjArray)
{
    for(int i = CSLCount(pFileNames) - 1; i >= 0; i-- )
    {
        IGxObject* pGxObj = NULL;
        CPLString szExt = CPLGetExtension(pFileNames[i]);
        CPLString szPath;

        if(EQUAL(szExt, "shp"))
        {
            bool bHasDbf(false), bHasPrj(false);
            szPath = (char*)CPLResetExtension(pFileNames[i], "dbf");
            if(CPLCheckForFile((char*)szPath.c_str(), NULL))
                bHasDbf = true;
            szPath = (char*)CPLResetExtension(pFileNames[i], "prj");
            if(CPLCheckForFile((char*)szPath.c_str(), NULL))
                bHasPrj = true;
            if(bHasDbf && bHasPrj)
                pGxObj = GetGxDataset(pFileNames[i], GetConvName(pFileNames[i]), enumGISFeatureDataset);
            pFileNames = CSLRemoveStrings( pFileNames, i, 1, NULL );
        }
        else if(EQUAL(szExt, "dbf"))
        {
            bool bHasShp(false);
            szPath = (char*)CPLResetExtension(pFileNames[i], "shp");
            if(CPLCheckForFile((char*)szPath.c_str(), NULL))
                bHasShp = true;
            if(!bHasShp)
                pGxObj = GetGxDataset(pFileNames[i], GetConvName(pFileNames[i]), enumGISTableDataset);
            pFileNames = CSLRemoveStrings( pFileNames, i, 1, NULL );
        }
        else if(EQUAL(szExt, "prj"))
        {
            bool bHasShp(false);
            szPath = (char*)CPLResetExtension(pFileNames[i], "shp");
            if(CPLCheckForFile((char*)szPath.c_str(), NULL))
                bHasShp = true;
            if(bHasShp)
                pFileNames = CSLRemoveStrings( pFileNames, i, 1, NULL );
        }
        else if(EQUAL(szExt, "sbn"))
            pFileNames = CSLRemoveStrings( pFileNames, i, 1, NULL );
        else if(EQUAL(szExt, "sbx"))
            pFileNames = CSLRemoveStrings( pFileNames, i, 1, NULL );
        else if(EQUAL(szExt, "shx"))
            pFileNames = CSLRemoveStrings( pFileNames, i, 1, NULL );
        else if(EQUAL(szExt, "cpg"))
            pFileNames = CSLRemoveStrings( pFileNames, i, 1, NULL );
        else if(EQUAL(szExt, "qix"))
            pFileNames = CSLRemoveStrings( pFileNames, i, 1, NULL );
        else if(EQUAL(szExt, "osf"))
            pFileNames = CSLRemoveStrings( pFileNames, i, 1, NULL );

		if(pGxObj != NULL)
			ObjArray.push_back(pGxObj);
    }
	return true;
}

void wxGxShapeFactory::Serialize(wxXmlNode* const pConfig, bool bStore)
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

IGxObject* wxGxShapeFactory::GetGxDataset(CPLString path, wxString name, wxGISEnumDatasetType type)
{
    switch(type)
    {
    case enumGISFeatureDataset:
        {
	    wxGxFeatureDataset* pDataset = new wxGxFeatureDataset(path, name, enumVecESRIShapefile);
        return static_cast<IGxObject*>(pDataset);
        }
    case enumGISTableDataset:
        {
        wxGxTableDataset* pDataset = new wxGxTableDataset(path, name, enumTableDBF);
        return static_cast<IGxObject*>(pDataset);
        }
    }
    return NULL;
}

