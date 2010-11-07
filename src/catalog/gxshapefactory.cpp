/******************************************************************************
 * Project:  wxGIS (GIS Catalog)
 * Purpose:  wxGxShapeFactory class.
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

bool wxGxShapeFactory::GetChildren(wxString sParentDir, wxArrayString* pFileNames, GxObjectArray* pObjArray)
{
	std::map<wxString, DATA> data_map;

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


		if(data_map[name].bHasShp != 1)
			data_map[name].bHasShp = (ext == wxT("shp")) ? 1 : 0;
		if(data_map[name].bHasDbf != 1)
			data_map[name].bHasDbf = (ext == wxT("dbf")) ? 1 : 0;
		if(data_map[name].bHasPrj != 1)
			data_map[name].bHasPrj = (ext == wxT("prj")) ? 1 : 0;
		if(data_map[name].path.IsEmpty() && (data_map[name].bHasShp || data_map[name].bHasDbf || data_map[name].bHasPrj))
			data_map[name].path = sParentDir+ wxFileName::GetPathSeparator() + FName.GetName();
		if(data_map[name].path.IsEmpty())
			data_map.erase(name);

		if(path.Find(wxT(".shp")) != wxNOT_FOUND)
			goto REMOVE;
		if(path.Find(wxT(".dbf")) != wxNOT_FOUND)
			goto REMOVE;
		if(path.Find(wxT(".prj")) != wxNOT_FOUND)
			goto REMOVE;
		if(path.Find(wxT(".sbn")) != wxNOT_FOUND)
			goto REMOVE;
		if(path.Find(wxT(".sbx")) != wxNOT_FOUND)
			goto REMOVE;
		if(path.Find(wxT(".shx")) != wxNOT_FOUND)
			goto REMOVE;
		if(path.Find(wxT(".cpg")) != wxNOT_FOUND)
			goto REMOVE;
		continue;
REMOVE:
		pFileNames->RemoveAt(i);
		i--;
	}

	for(std::map<wxString, DATA>::const_iterator CI = data_map.begin(); CI != data_map.end(); ++CI)
	{
		IGxObject* pGxObj = NULL;

		if(CI->second.bHasShp)
		{
			wxString name = CI->first + wxT(".shp");
			wxString path = CI->second.path + wxT(".shp");
			//create shp
			pGxObj = GetGxDataset(path, name, enumGISFeatureDataset);
		}
		if(CI->second.bHasDbf && !CI->second.bHasShp)
		{
			wxString name = CI->first + wxT(".dbf");
			wxString path = CI->second.path + wxT(".dbf");

			//create dbf
			pGxObj = GetGxDataset(path, name, enumGISTableDataset);
		}
		if(CI->second.bHasPrj && !CI->second.bHasShp)
		{
			//CI->first + wxT(".prj");
            pFileNames->push_back(CI->second.path + wxT(".prj"));
			//create prj
		}
		if(pGxObj != NULL)
			pObjArray->push_back(pGxObj);
	}
	return true;
}

void wxGxShapeFactory::Serialize(wxXmlNode* pConfig, bool bStore)
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

IGxObject* wxGxShapeFactory::GetGxDataset(wxString path, wxString name, wxGISEnumDatasetType type)
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

