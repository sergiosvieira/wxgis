/******************************************************************************
 * Project:  wxGIS (GIS Catalog)
 * Purpose:  wxGxMLFactory class.
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

#include "wxgis/catalog/gxmlfactory.h"
#include "wxgis/catalog/gxdataset.h"
#include "wxgis/catalog/gxkmldataset.h"
#include <wx/ffile.h>

IMPLEMENT_DYNAMIC_CLASS(wxGxMLFactory, wxObject)

wxGxMLFactory::wxGxMLFactory(void)
{
}

wxGxMLFactory::~wxGxMLFactory(void)
{
}

bool wxGxMLFactory::GetChildren(wxString sParentDir, wxArrayString* pFileNames, GxObjectArray* pObjArray)
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

        //name conv cp866 if zip
        wxString name;
        if(path.Find(wxT("/vsizip/")) != wxNOT_FOUND)
        {
            wxString str(FName.GetName().mb_str(*wxConvCurrent), wxCSConv(wxT("cp-866")));
            name = str;
        }
        else
            name = FName.GetName();


		IGxObject* pGxObj = NULL;
		if(ext == wxString(wxT("kml")))
		{
			name += wxT(".") + ext;
            wxGxKMLDataset* pDataset = new wxGxKMLDataset(path, name, enumVecKML);
            pDataset->SetEncoding(wxFONTENCODING_UTF8);
			pGxObj = dynamic_cast<IGxObject*>(pDataset);
			goto REMOVE;
		}
		if(ext == wxString(wxT("gml")))
		{
			//if(m_pCatalog->GetShowExt())
			//	name += wxT(".") + ext;
			//wxGxPrjFile* pFile = new wxGxPrjFile(path, name, enumESRIPrjFile);
			//pGxObj = dynamic_cast<IGxObject*>(pFile);
			goto REMOVE;
		}
		if(ext == wxString(wxT("dxf")))
		{
			name += wxT(".") + ext;
            wxGxFeatureDataset* pDataset = new wxGxFeatureDataset(path, name, enumVecDXF);
			pGxObj = dynamic_cast<IGxObject*>(pDataset);
			goto REMOVE;
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

 
void wxGxMLFactory::Serialize(wxXmlNode* pConfig, bool bStore)
{
    if(bStore)
    {
        if(pConfig->HasProp(wxT("factory_name")))
            pConfig->DeleteProperty(wxT("factory_name"));
        pConfig->AddProperty(wxT("factory_name"), GetName());  
        if(pConfig->HasProp(wxT("is_enabled")))
            pConfig->DeleteProperty(wxT("is_enabled"));
        pConfig->AddProperty(wxT("is_enabled"), m_bIsEnabled == true ? wxT("1") : wxT("0"));    
    }
    else
    {
        m_bIsEnabled = wxAtoi(pConfig->GetPropVal(wxT("is_enabled"), wxT("1")));
    }
}

