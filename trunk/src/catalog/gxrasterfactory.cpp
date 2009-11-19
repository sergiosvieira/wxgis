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
	for(size_t i = 0; i < pFileNames->GetCount(); i++)
	{
		wxString path = pFileNames->Item(i);
		//test is dir
		if(wxFileName::DirExists(path))
			continue;

		path.MakeLower();
		wxString name, ext;
		wxFileName::SplitPath(path, NULL, NULL, &name, &ext);
		

		IGxObject* pGxObj = NULL;
		if(ext == wxString(wxT("bmp")) || ext == wxString(wxT("jpg")) || ext == wxString(wxT("img")))
		{
			if(m_pCatalog->GetShowExt())
				name += wxT(".") + ext;
			wxGxRasterDataset* pDataset = new wxGxRasterDataset(path, name, enumRasterUnknown);
			pGxObj = dynamic_cast<IGxObject*>(pDataset);
			goto REMOVE;
		}
		if(ext == wxString(wxT("tif")) || ext == wxString(wxT("tiff")) || ext == wxString(wxT("png")))//TODO: add other raster file extensions
		{
			if(m_pCatalog->GetShowExt())
				name += wxT(".") + ext;
			wxGxRasterDataset* pDataset = new wxGxRasterDataset(path, name, enumRasterUnknown);
			pGxObj = dynamic_cast<IGxObject*>(pDataset);
			goto REMOVE;
		}
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

	return true;
}