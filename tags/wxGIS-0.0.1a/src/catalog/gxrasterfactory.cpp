/******************************************************************************
 * Project:  wxGIS (GIS Catalog)
 * Purpose:  wxGxRasterFactory class.
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
#include "wxgis/catalog/gxrasterfactory.h"
#include "wxgis/catalog/gxdataset.h"

IMPLEMENT_DYNAMIC_CLASS(wxGxRasterFactory, wxObject)

wxGxRasterFactory::wxGxRasterFactory(void)
{
}

wxGxRasterFactory::~wxGxRasterFactory(void)
{
}

bool wxGxRasterFactory::GetChildren(wxString sParentDir, wxArrayString* pFileNames, GxObjectArray* pObjArray)
{
    std::vector<wxString> remove_candidates;
	for(size_t i = 0; i < pFileNames->GetCount(); i++)
	{
		wxString path = pFileNames->Item(i);
		//test is dir
		if(wxFileName::DirExists(path))
			continue;

		wxString vol, shortpath, name, ext;
		wxFileName::SplitPath(path, &vol, &shortpath, &name, &ext);
		ext.MakeLower();
		

		IGxObject* pGxObj = NULL;
		//prj files
		if(ext == wxString(wxT("bmp")) || ext == wxString(wxT("jpg")) || ext == wxString(wxT("img")))
		{
            wxString sRemCand;
            if(vol.IsEmpty())
                sRemCand = shortpath + wxFileName::GetPathSeparator() + name + wxT(".prj");
            else
                sRemCand = vol + wxT(":") + shortpath + wxFileName::GetPathSeparator() + name + wxT(".prj");
            remove_candidates.push_back(sRemCand);

			if(m_pCatalog->GetShowExt())
				name += wxT(".") + ext;
			wxGxRasterDataset* pDataset = new wxGxRasterDataset(path, name, enumRasterUnknown);
			pGxObj = dynamic_cast<IGxObject*>(pDataset);
			goto REMOVE;
		}
		if(ext == wxString(wxT("tif")) || ext == wxString(wxT("tiff")) || ext == wxString(wxT("png")))
		{
            wxString sRemCand;
            if(vol.IsEmpty())
                sRemCand = shortpath + wxFileName::GetPathSeparator() + name + wxT(".prj");
            else
                sRemCand = vol + wxT(":") + shortpath + wxFileName::GetPathSeparator() + name + wxT(".prj");
            remove_candidates.push_back(sRemCand);

			if(m_pCatalog->GetShowExt())
				name += wxT(".") + ext;
			wxGxRasterDataset* pDataset = new wxGxRasterDataset(path, name, enumRasterUnknown);
			pGxObj = dynamic_cast<IGxObject*>(pDataset);
			goto REMOVE;
		}
		if(ext == wxString(wxT("til")) || ext == wxString(wxT("jpeg")) || ext == wxString(wxT("jp2")))//TODO: add other raster file extensions
		{
            wxString sRemCand;
            if(vol.IsEmpty())
                sRemCand = shortpath + wxFileName::GetPathSeparator() + name + wxT(".prj");
            else
                sRemCand = vol + wxT(":") + shortpath + wxFileName::GetPathSeparator() + name + wxT(".prj");
            remove_candidates.push_back(sRemCand);

			if(m_pCatalog->GetShowExt())
				name += wxT(".") + ext;
			wxGxRasterDataset* pDataset = new wxGxRasterDataset(path, name, enumRasterUnknown);
			pGxObj = dynamic_cast<IGxObject*>(pDataset);
			goto REMOVE;
		}		
		path.MakeLower();
		if(path.Find(wxT(".aux")) != wxNOT_FOUND)
			goto REMOVE;
		if(path.Find(wxT(".rrd")) != wxNOT_FOUND)
			goto REMOVE;
		if(path.Find(wxT(".ovr")) != wxNOT_FOUND)
			goto REMOVE;
		if(path.Find(wxT(".w")) != wxNOT_FOUND)//TODO: add other world file extensions
			goto REMOVE;
		if(path.Find(wxT(".wld")) != wxNOT_FOUND)
			goto REMOVE;
		if(path.Find(wxT(".bpw")) != wxNOT_FOUND)
			goto REMOVE;
		if(path.Find(wxT(".bmpw")) != wxNOT_FOUND)
			goto REMOVE;
		continue;
REMOVE:
		pFileNames->RemoveAt(i);
		i--;
		if(pGxObj != NULL)
			pObjArray->push_back(pGxObj);
	}

	for(size_t i = 0; i < pFileNames->GetCount(); i++)
	{
        wxString path = pFileNames->Item(i);
        for(size_t j = 0; j < remove_candidates.size(); j++)
        {
            if(remove_candidates[j].CmpNoCase(path) == 0)
            {
                pFileNames->RemoveAt(i);
                i--;
                break;
            }
        }
    }

	return true;
}


void wxGxRasterFactory::Serialize(wxXmlNode* pConfig, bool bStore)
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
