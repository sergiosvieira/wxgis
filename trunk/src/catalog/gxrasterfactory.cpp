/******************************************************************************
 * Project:  wxGIS (GIS Catalog)
 * Purpose:  wxGxRasterFactory class.
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
#include "wxgis/catalog/gxrasterfactory.h"
#include "wxgis/catalog/gxdataset.h"

IMPLEMENT_DYNAMIC_CLASS(wxGxRasterFactory, wxObject)

wxGxRasterFactory::wxGxRasterFactory(void)
{
}

wxGxRasterFactory::~wxGxRasterFactory(void)
{
}

bool wxGxRasterFactory::GetChildren(CPLString sParentDir, char** &pFileNames, GxObjectArray &ObjArray)
{
    for(int i = CSLCount(pFileNames) - 1; i >= 0; i-- )
    {
        IGxObject* pGxObj = NULL;
        CPLString szExt = CPLGetExtension(pFileNames[i]);
        CPLString szPath;


		if(EQUAL(szExt, "bmp"))
		{
			pGxObj = GetGxDataset(pFileNames[i], GetConvName(pFileNames[i]), enumRasterBmp);
            pFileNames = CSLRemoveStrings( pFileNames, i, 1, NULL );
		}
		else if(EQUAL(szExt, "jpg") || EQUAL(szExt, "jpeg"))
		{
			pGxObj = GetGxDataset(pFileNames[i], GetConvName(pFileNames[i]), enumRasterJpeg);
            pFileNames = CSLRemoveStrings( pFileNames, i, 1, NULL );
		}
		else if(EQUAL(szExt, "img"))
		{
			pGxObj = GetGxDataset(pFileNames[i], GetConvName(pFileNames[i]), enumRasterImg);
            pFileNames = CSLRemoveStrings( pFileNames, i, 1, NULL );
		}
		else if(EQUAL(szExt, "gif"))
		{
			pGxObj = GetGxDataset(pFileNames[i], GetConvName(pFileNames[i]), enumRasterGif);
            pFileNames = CSLRemoveStrings( pFileNames, i, 1, NULL );
		}
		else if(EQUAL(szExt, "tif") || EQUAL(szExt, "tiff"))
		{
			pGxObj = GetGxDataset(pFileNames[i], GetConvName(pFileNames[i]), enumRasterTiff);
            pFileNames = CSLRemoveStrings( pFileNames, i, 1, NULL );
		}
		else if(EQUAL(szExt, "png"))
		{
			pGxObj = GetGxDataset(pFileNames[i], GetConvName(pFileNames[i]), enumRasterPng);
            pFileNames = CSLRemoveStrings( pFileNames, i, 1, NULL );
		}

        else if(EQUAL(szExt, "prj"))
        {
            szPath = (char*)CPLResetExtension(pFileNames[i], "bmp");
            if(CPLCheckForFile((char*)szPath.c_str(), NULL))
                pFileNames = CSLRemoveStrings( pFileNames, i, 1, NULL );
            szPath = (char*)CPLResetExtension(pFileNames[i], "jpg");
            if(CPLCheckForFile((char*)szPath.c_str(), NULL))
                pFileNames = CSLRemoveStrings( pFileNames, i, 1, NULL );
            szPath = (char*)CPLResetExtension(pFileNames[i], "jpeg");
            if(CPLCheckForFile((char*)szPath.c_str(), NULL))
                pFileNames = CSLRemoveStrings( pFileNames, i, 1, NULL );
            szPath = (char*)CPLResetExtension(pFileNames[i], "img");
            if(CPLCheckForFile((char*)szPath.c_str(), NULL))
                pFileNames = CSLRemoveStrings( pFileNames, i, 1, NULL );
            szPath = (char*)CPLResetExtension(pFileNames[i], "gif");
            if(CPLCheckForFile((char*)szPath.c_str(), NULL))
                pFileNames = CSLRemoveStrings( pFileNames, i, 1, NULL );
            szPath = (char*)CPLResetExtension(pFileNames[i], "tif");
            if(CPLCheckForFile((char*)szPath.c_str(), NULL))
                pFileNames = CSLRemoveStrings( pFileNames, i, 1, NULL );
            szPath = (char*)CPLResetExtension(pFileNames[i], "tiff");
            if(CPLCheckForFile((char*)szPath.c_str(), NULL))
                pFileNames = CSLRemoveStrings( pFileNames, i, 1, NULL );
            szPath = (char*)CPLResetExtension(pFileNames[i], "png");
            if(CPLCheckForFile((char*)szPath.c_str(), NULL))
                pFileNames = CSLRemoveStrings( pFileNames, i, 1, NULL );
        }
        else if(EQUAL(szExt, "aux"))
            pFileNames = CSLRemoveStrings( pFileNames, i, 1, NULL );
        else if(EQUAL(szExt, "rrd"))
            pFileNames = CSLRemoveStrings( pFileNames, i, 1, NULL );
        else if(EQUAL(szExt, "ovr"))
            pFileNames = CSLRemoveStrings( pFileNames, i, 1, NULL );
        else if(EQUAL(szExt, "w"))
            pFileNames = CSLRemoveStrings( pFileNames, i, 1, NULL );
        else if(EQUAL(szExt, "wld"))
            pFileNames = CSLRemoveStrings( pFileNames, i, 1, NULL );
        else if(EQUAL(szExt, "bpw"))
            pFileNames = CSLRemoveStrings( pFileNames, i, 1, NULL );
        else if(EQUAL(szExt, "bmpw"))
            pFileNames = CSLRemoveStrings( pFileNames, i, 1, NULL );
        else if(EQUAL(szExt, "gifw"))
            pFileNames = CSLRemoveStrings( pFileNames, i, 1, NULL );
        else if(EQUAL(szExt, "jpgw"))
            pFileNames = CSLRemoveStrings( pFileNames, i, 1, NULL );
        else if(EQUAL(szExt, "jpegw"))
            pFileNames = CSLRemoveStrings( pFileNames, i, 1, NULL );
        else if(EQUAL(szExt, "pngw"))
            pFileNames = CSLRemoveStrings( pFileNames, i, 1, NULL );

		if(pGxObj != NULL)
			ObjArray.push_back(pGxObj);
    }
	return true;
}

void wxGxRasterFactory::Serialize(wxXmlNode* pConfig, bool bStore)
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

IGxObject* wxGxRasterFactory::GetGxDataset(CPLString path, wxString name, wxGISEnumRasterDatasetType type)
{
    wxGxRasterDataset* pDataset = new wxGxRasterDataset(path, name, type);
    return static_cast<IGxObject*>(pDataset);
}

