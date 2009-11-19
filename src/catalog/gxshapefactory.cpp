/******************************************************************************
 * Project:  wxGIS (GIS Catalog)
 * Purpose:  wxGxShapeFactory class.
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
	typedef struct _data{
		wxString path;
		int bHasShp, bHasDbf, bHasPrj, bHasCpg;
	}DATA;
	std::map<wxString, DATA> data_map;

	for(size_t i = 0; i < pFileNames->GetCount(); i++)
	{
		wxString path = pFileNames->Item(i);
		//test is dir
		if(wxFileName::DirExists(path))
			continue;

		path.MakeLower();
		wxString name, ext;
		wxFileName::SplitPath(path, NULL, NULL, &name, &ext);

		if(data_map[name].bHasShp != 1)
			data_map[name].bHasShp = (ext == wxT("shp")) ? 1 : 0;
		if(data_map[name].bHasDbf != 1)
			data_map[name].bHasDbf = (ext == wxT("dbf")) ? 1 : 0;
		if(data_map[name].bHasPrj != 1)
			data_map[name].bHasPrj = (ext == wxT("prj")) ? 1 : 0;
		if(data_map[name].bHasCpg != 1)
			data_map[name].bHasCpg = (ext == wxT("cpg")) ? 1 : 0;
		if(data_map[name].path.IsEmpty() && (data_map[name].bHasCpg || data_map[name].bHasShp || data_map[name].bHasDbf || data_map[name].bHasPrj))
			data_map[name].path = sParentDir+ wxFileName::GetPathSeparator() + name;
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
			wxString name;
			if(m_pCatalog->GetShowExt())
				name = CI->first + wxT(".shp");
			else
				name = CI->first;
			wxString path = CI->second.path + wxT(".shp");
			//create shp
			wxGxShapefileDataset* pDataset = new wxGxShapefileDataset(path, name, enumESRIShapefile);
            //code page testing
            if(pDataset && CI->second.bHasCpg)
            {
                pDataset->SetEncoding(GetEncoding(CI->second.path + wxT(".cpg")));
            }
			pGxObj = dynamic_cast<IGxObject*>(pDataset);
		}
		if(CI->second.bHasDbf && !CI->second.bHasShp)
		{
			wxString name = CI->first + wxT(".dbf");
			wxString path = CI->second.path + wxT(".dbf");
			//create dbf
			wxGxDataset* pDataset = new wxGxDataset(path, name, enumGISTableDataset);
            //code page testing
            if(pDataset && CI->second.bHasCpg)
            {
                pDataset->SetEncoding(GetEncoding(CI->second.path + wxT(".cpg")));
            }
			pGxObj = dynamic_cast<IGxObject*>(pDataset);
		}
		if(CI->second.bHasPrj && !CI->second.bHasShp)
		{
			CI->first + wxT(".prj");
			//create prj
		}
		if(pGxObj != NULL)
			pObjArray->push_back(pGxObj);
	}
	return true;
}

wxString wxGxShapeFactory::GetEncoding(wxString sPath)
{
    wxFFile file(sPath);
    if(file.IsOpened())
    {
        wxString sCP;
        file.ReadAll(&sCP);
        int pos;
        if((pos = sCP.Find('\n')) != wxNOT_FOUND)
            sCP = sCP.Left(pos);
        sCP.Replace(wxT("LCID/"), wxT(""));
        int nCP = wxAtoi(sCP);
        //check encoding from code http://en.wikipedia.org/wiki/Code_page
        switch(nCP)
        {
        case 65001:
            return wxString(wxT("UTF-8"));
        case 65000:
            return wxString(wxT("UTF-7"));
        case 437:
        case 737:
        case 775:
        case 850:
        case 852:
        case 855:
        case 857:
        case 858:
        case 860:
        case 861:
        case 862:
        case 863:
        case 865:
        case 866:
        case 869:
            return wxString::Format(wxT("CP%d"), nCP);
        case 874:
        case 1250:
        case 1251:
        case 1252:
        case 1253:
        case 1254:
        case 1255:
        case 1256:
        case 1257:
        case 1258:
            return wxString::Format(wxT("Windows-%d"), nCP);
        case 1200:
            return wxString(wxT("UCS-2LE"));
        case 1201:
            return wxString(wxT("UCS-2BE"));
        default:
            break;
        }
        return sCP;
    }
    return wxString();
}

