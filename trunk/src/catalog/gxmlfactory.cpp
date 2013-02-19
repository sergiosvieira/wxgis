/******************************************************************************
 * Project:  wxGIS (GIS Catalog)
 * Purpose:  wxGxMLFactory class.
 * Author:   Baryshnikov Dmitriy (aka Bishop), polimax@mail.ru
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

#include "wxgis/catalog/gxmlfactory.h"
#include "wxgis/catalog/gxdataset.h"
#include "wxgis/catalog/gxkmldataset.h"
/*
IMPLEMENT_DYNAMIC_CLASS(wxGxMLFactory, wxObject)

wxGxMLFactory::wxGxMLFactory(void)
{
}

wxGxMLFactory::~wxGxMLFactory(void)
{
}

bool wxGxMLFactory::GetChildren(CPLString sParentDir, char** &pFileNames, GxObjectArray &ObjArray)
{
    for(int i = CSLCount(pFileNames) - 1; i >= 0; i-- )
    {
        IGxObject* pGxObj = NULL;
        CPLString szExt = CPLGetExtension(pFileNames[i]);
        if(wxGISEQUAL(szExt, "kml"))
        {
			pGxObj = GetGxDataset(pFileNames[i], GetConvName(pFileNames[i]), enumVecKML);
            pFileNames = CSLRemoveStrings( pFileNames, i, 1, NULL );
        }
        else if(wxGISEQUAL(szExt, "kmz"))
        {
			pGxObj = GetGxDataset(pFileNames[i], GetConvName(pFileNames[i]), enumVecKMZ);
            pFileNames = CSLRemoveStrings( pFileNames, i, 1, NULL );
        }
        else if(wxGISEQUAL(szExt, "dxf"))
        {
			pGxObj = GetGxDataset(pFileNames[i], GetConvName(pFileNames[i]), enumVecDXF);
            pFileNames = CSLRemoveStrings( pFileNames, i, 1, NULL );
        }
        else if(wxGISEQUAL(szExt, "gml"))
        {
			pGxObj = GetGxDataset(pFileNames[i], GetConvName(pFileNames[i]), enumVecGML);
            pFileNames = CSLRemoveStrings( pFileNames, i, 1, NULL );
        }

		if(pGxObj != NULL)
			ObjArray.push_back(pGxObj);
    }

		//if(ext == wxString(wxT("gml")))
		//{
		//	//if(m_pCatalog->GetShowExt())
		//	//	name += wxT(".") + ext;
		//	//wxGxPrjFile* pFile = new wxGxPrjFile(path, name, enumESRIPrjFile);
		//	//pGxObj = dynamic_cast<IGxObject*>(pFile);
		//	goto REMOVE;
		//}

	return true;
}

 
void wxGxMLFactory::Serialize(wxXmlNode* const pConfig, bool bStore)
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

IGxObject* wxGxMLFactory::GetGxDataset(CPLString path, wxString name, wxGISEnumVectorDatasetType type)
{
    if(type == enumVecKML)
    {
        wxGxKMLDataset* pDataset = new wxGxKMLDataset(path, name, type);
        pDataset->SetEncoding(wxFONTENCODING_UTF8);
        return static_cast<IGxObject*>(pDataset);
    }
    else if(type == enumVecKMZ)
    {
        wxGxKMLDataset* pDataset = new wxGxKMLDataset(path, name, type);
        pDataset->SetEncoding(wxFONTENCODING_UTF8);
        return static_cast<IGxObject*>(pDataset);
    }
    else if(type == enumVecGML)
    {
        wxGxKMLDataset* pDataset = new wxGxKMLDataset(path, name, type);
        //pDataset->SetEncoding(wxFONTENCODING_UTF8);
        return static_cast<IGxObject*>(pDataset);
    }
    else
    {
	    wxGxFeatureDataset* pDataset = new wxGxFeatureDataset(path, name, type);
        return static_cast<IGxObject*>(pDataset);
    }
    return NULL;
}
*/