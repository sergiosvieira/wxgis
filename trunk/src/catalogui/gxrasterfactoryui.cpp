/******************************************************************************
 * Project:  wxGIS (GIS Catalog)
 * Purpose:  wxGxRasterFactoryUI class.
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
#include "wxgis/catalogui/gxrasterfactoryui.h"
#include "wxgis/catalogui/gxdatasetui.h"

#include "../../art/raster_16.xpm"
#include "../../art/raster_48.xpm"


IMPLEMENT_DYNAMIC_CLASS(wxGxRasterFactoryUI, wxGxRasterFactory)

wxGxRasterFactoryUI::wxGxRasterFactoryUI(void) : wxGxRasterFactory()
{
    m_SmallIcon = wxIcon(raster_16_xpm);
    m_LargeIcon = wxIcon(raster_48_xpm);
}

wxGxRasterFactoryUI::~wxGxRasterFactoryUI(void)
{
}

IGxObject* wxGxRasterFactoryUI::GetGxDataset(wxString path, wxString name, wxGISEnumRasterDatasetType type)
{
    wxGxRasterDatasetUI* pDataset = new wxGxRasterDatasetUI(path, name, type, m_LargeIcon, m_SmallIcon);
    return static_cast<IGxObject*>(pDataset);
}

