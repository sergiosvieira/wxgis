/******************************************************************************
 * Project:  wxGIS (GIS Catalog)
 * Purpose:  wxGxShapeFactoryUI class.
 * Author:   Bishop (aka Barishnikov Dmitriy), polimax@mail.ru
 ******************************************************************************
*   Copyright (C) 2010  Bishop
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
#include "wxgis/catalogui/gxshapefactoryui.h"
#include "wxgis/catalogui/gxdatasetui.h"

#include "../../art/shp_dset_16.xpm"
#include "../../art/shp_dset_48.xpm"
#include "../../art/dbf_dset_16.xpm"
#include "../../art/dbf_dset_48.xpm"


IMPLEMENT_DYNAMIC_CLASS(wxGxShapeFactoryUI, wxGxShapeFactory)

wxGxShapeFactoryUI::wxGxShapeFactoryUI(void) : wxGxShapeFactory()
{
    m_SmallSHPIcon = wxIcon(shp_dset_16_xpm);
    m_LargeSHPIcon = wxIcon(shp_dset_48_xpm);
    m_SmallDXFIcon = wxIcon(dbf_dset_16_xpm);
    m_LargeDXFIcon = wxIcon(dbf_dset_48_xpm);
}

wxGxShapeFactoryUI::~wxGxShapeFactoryUI(void)
{
}

IGxObject* wxGxShapeFactoryUI::GetGxDataset(CPLString path, wxString name, wxGISEnumDatasetType type)
{
    switch(type)
    {
    case enumGISFeatureDataset:
        {
	    wxGxFeatureDatasetUI* pDataset = new wxGxFeatureDatasetUI(path, name, enumVecESRIShapefile, m_LargeSHPIcon, m_SmallSHPIcon);
        return static_cast<IGxObject*>(pDataset);
        }
    case enumGISTableDataset:
        {
        wxGxTableDatasetUI* pDataset = new wxGxTableDatasetUI(path, name, enumTableDBF, m_LargeDXFIcon, m_SmallDXFIcon);
        return static_cast<IGxObject*>(pDataset);
        }
    }
    return NULL;
}