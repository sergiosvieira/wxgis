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

bool wxGxMapInfoFactory::GetChildren(wxString sParentDir, wxArrayString* pFileNames, GxObjectArray* pObjArray)
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
        wxString name = GetConvName(path, FName);

		if(data_map[name].bHasTab != 1)
			data_map[name].bHasTab = (ext == wxT("tab")) ? 1 : 0;
		if(data_map[name].bHasMap != 1)
			data_map[name].bHasMap = (ext == wxT("map")) ? 1 : 0;
		if(data_map[name].bHasInd != 1)
			data_map[name].bHasInd = (ext == wxT("ind")) ? 1 : 0;
		if(data_map[name].bHasID != 1)
			data_map[name].bHasID = (ext == wxT("id")) ? 1 : 0;
		if(data_map[name].bHasDat != 1)
			data_map[name].bHasDat = (ext == wxT("dat")) ? 1 : 0;
		if(data_map[name].bHasMid != 1)
			data_map[name].bHasMid = (ext == wxT("mid")) ? 1 : 0;
		if(data_map[name].bHasMif != 1)
			data_map[name].bHasMif = (ext == wxT("mif")) ? 1 : 0;
		if(data_map[name].path.IsEmpty() && (data_map[name].bHasTab || data_map[name].bHasMif || data_map[name].bHasDat || data_map[name].bHasID || data_map[name].bHasMap || data_map[name].bHasInd || data_map[name].bHasMid))
            data_map[name].path = sParentDir+ wxFileName::GetPathSeparator() + FName.GetName();
		if(data_map[name].path.IsEmpty())
			data_map.erase(name);

		if(path.Find(wxT(".tab")) != wxNOT_FOUND)
			goto REMOVE;
		if(path.Find(wxT(".map")) != wxNOT_FOUND)
			goto REMOVE;
		if(path.Find(wxT(".ind")) != wxNOT_FOUND)
			goto REMOVE;
		if(path.Find(wxT(".id")) != wxNOT_FOUND)
			goto REMOVE;
		if(path.Find(wxT(".dat")) != wxNOT_FOUND)
			goto REMOVE;
		if(path.Find(wxT(".mid")) != wxNOT_FOUND)
			goto REMOVE;
		if(path.Find(wxT(".mif")) != wxNOT_FOUND)
			goto REMOVE;
		continue;
REMOVE:
		pFileNames->RemoveAt(i);
		i--;
	}

	for(std::map<wxString, DATA>::const_iterator CI = data_map.begin(); CI != data_map.end(); ++CI)
	{
		IGxObject* pGxObj = NULL;

		if(CI->second.bHasTab && CI->second.bHasMap && CI->second.bHasDat)
		{
			wxString name;
			name = CI->first + wxT(".tab");
			wxString path = CI->second.path + wxT(".tab");
			//create tab
			pGxObj = GetGxDataset(path, name, enumVecMapinfoTab);
		    if(pGxObj != NULL)
			    pObjArray->push_back(pGxObj);
		}

        if(CI->second.bHasMid && CI->second.bHasMif)
		{
			wxString name;
			name = CI->first + wxT(".mif");
			wxString path = CI->second.path + wxT(".mif");
			//create mif
			pGxObj = GetGxDataset(path, name, enumVecMapinfoMif);
		    if(pGxObj != NULL)
			    pObjArray->push_back(pGxObj);
		}
		//if(CI->second.bHasDbf && !CI->second.bHasShp)
		//{
		//	wxString name = CI->first + wxT(".dbf");
		//	wxString path = CI->second.path + wxT(".dbf");
		//	//create dbf
		//	wxGxDataset* pDataset = new wxGxDataset(path, name, enumGISTableDataset);
  //          //code page testing
  //          if(pDataset && CI->second.bHasCpg)
  //          {
  //              pDataset->SetEncoding(GetEncoding(CI->second.path + wxT(".cpg")));
  //          }
		//	pGxObj = dynamic_cast<IGxObject*>(pDataset);
		//}
		//if(CI->second.bHasPrj && !CI->second.bHasShp)
		//{
		//	//CI->first + wxT(".prj");
  //          pFileNames->push_back(CI->second.path + wxT(".prj"));
		//	//create prj
		//}
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

