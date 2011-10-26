/******************************************************************************
 * Project:  wxGIS (GIS Catalog)
 * Purpose:  wxGxDBConnectionFactoryUI class.
 * Author:   Bishop (aka Barishnikov Dmitriy), polimax@mail.ru
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
#include "wxgis/catalogui/gxdbconnfactoryui.h"

#include "../../art/rdb_conn_16.xpm"
#include "../../art/rdb_conn_48.xpm"
#include "../../art/rdb_disconn_16.xpm"
#include "../../art/rdb_disconn_48.xpm"

IMPLEMENT_DYNAMIC_CLASS(wxGxDBConnectionFactoryUI, wxGxDBConnectionFactory)

wxGxDBConnectionFactoryUI::wxGxDBConnectionFactoryUI(void) : wxGxDBConnectionFactory()
{
    m_LargeIconConn = wxIcon(rdb_conn_48_xpm);
    m_SmallIconConn = wxIcon(rdb_conn_16_xpm);
    m_LargeIconDisconn = wxIcon(rdb_disconn_48_xpm);
    m_SmallIconDisconn = wxIcon(rdb_disconn_16_xpm);
}

wxGxDBConnectionFactoryUI::~wxGxDBConnectionFactoryUI(void)
{
}

IGxObject* wxGxDBConnectionFactoryUI::GetGxDataset(CPLString path, wxString name)
{
    //wxGxKMLDatasetUI* pDataset = new wxGxKMLDatasetUI(path, name, enumVecKML, m_LargeKMLIcon, m_SmallKMLIcon, m_LargeIcon, m_SmallIcon);
    //pDataset->SetEncoding(wxFONTENCODING_UTF8);
    //return static_cast<IGxObject*>(pDataset);
 
    return NULL;
}
