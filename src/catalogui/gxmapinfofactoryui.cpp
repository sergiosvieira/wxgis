/******************************************************************************
 * Project:  wxGIS (GIS Catalog)
 * Purpose:  wxGxMapInfoFactoryUI class.
 * Author:   Bishop (aka Barishnikov Dmitriy), polimax@mail.ru
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
#include "wxgis/catalogui/gxmapinfofactoryui.h"
#include "wxgis/catalogui/gxdatasetui.h"
#include <wx/ffile.h>

#include "../../art/mi_dset_16.xpm"
#include "../../art/mi_dset_48.xpm"
#include "../../art/md_dset_16.xpm"
#include "../../art/md_dset_48.xpm"

IMPLEMENT_DYNAMIC_CLASS(wxGxMapInfoFactoryUI, wxGxMapInfoFactory)

wxGxMapInfoFactoryUI::wxGxMapInfoFactoryUI(void) : wxGxMapInfoFactory()
{
    m_SmallTabIcon = wxIcon(mi_dset_16_xpm);
    m_LargeTabIcon = wxIcon(mi_dset_48_xpm);
    m_SmallMifIcon = wxIcon(md_dset_16_xpm);
    m_LargeMifIcon = wxIcon(md_dset_48_xpm);
}

wxGxMapInfoFactoryUI::~wxGxMapInfoFactoryUI(void)
{
}

IGxObject* wxGxMapInfoFactoryUI::GetGxDataset(CPLString path, wxString name, wxGISEnumVectorDatasetType type)
{
    wxGxFeatureDatasetUI* pDataset = NULL;
    switch(type)
    {
    case enumVecMapinfoTab:
	    pDataset = new wxGxFeatureDatasetUI(path, name, type, m_LargeTabIcon, m_SmallTabIcon);
        break;
    case enumVecMapinfoMif:
	    pDataset = new wxGxFeatureDatasetUI(path, name, type, m_LargeMifIcon, m_SmallMifIcon);
        break;
    default:
        return NULL;
    }
    return static_cast<IGxObject*>(pDataset);
}

