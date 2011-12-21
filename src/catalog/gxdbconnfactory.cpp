/******************************************************************************
 * Project:  wxGIS (GIS Catalog)
 * Purpose:  wxGxDBConnectionFactory class.
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
#include "wxgis/catalog/gxdbconnfactory.h"
#include "wxgis/catalog/gxremoteconn.h"

IMPLEMENT_DYNAMIC_CLASS(wxGxDBConnectionFactory, wxObject)

wxGxDBConnectionFactory::wxGxDBConnectionFactory(void)
{
}

wxGxDBConnectionFactory::~wxGxDBConnectionFactory(void)
{
}

bool wxGxDBConnectionFactory::GetChildren(CPLString sParentDir, char** &pFileNames, GxObjectArray &ObjArray)
{
    for(int i = CSLCount(pFileNames) - 1; i >= 0; --i )
    {
        IGxObject* pGxObj = NULL;
        CPLString szExt = CPLGetExtension(pFileNames[i]);
        if(EQUAL(szExt, "xconn"))
        {
			pGxObj = GetGxDataset(pFileNames[i], GetConvName(pFileNames[i]));
            pFileNames = CSLRemoveStrings( pFileNames, i, 1, NULL );
        }
		if(pGxObj != NULL)
			ObjArray.push_back(pGxObj);
    }

	return true;
}

 
void wxGxDBConnectionFactory::Serialize(wxXmlNode* const pConfig, bool bStore)
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

IGxObject* wxGxDBConnectionFactory::GetGxDataset(CPLString path, wxString name)
{
    wxGxRemoteConnection* pDataset = new wxGxRemoteConnection(path, name);
    //pDataset->SetEncoding(wxFONTENCODING_UTF8);
    return static_cast<IGxObject*>(pDataset);
}
