/******************************************************************************
 * Project:  wxGIS (GIS Catalog)
 * Purpose:  wxGxWebConnectionFactoryUI class.
 * Author:   Baryshnikov Dmitriy (aka Bishop), polimax@mail.ru
 ******************************************************************************
*   Copyright (C) 2013 Bishop
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
#include "wxgis/catalogui/gxwebconnfactoryui.h"
//#include "wxgis/catalogui/gxremoteconnui.h"

//#include "../../art/web_conn_16.xpm"
//#include "../../art/web_conn_48.xpm"
//#include "../../art/web_disconn_16.xpm"
//#include "../../art/web_disconn_48.xpm"

//------------------------------------------------------------------------------
// wxGxWebConnectionFactoryUI
//------------------------------------------------------------------------------

IMPLEMENT_DYNAMIC_CLASS(wxGxWebConnectionFactoryUI, wxGxWebConnectionFactory)

wxGxWebConnectionFactoryUI::wxGxWebConnectionFactoryUI(void) : wxGxWebConnectionFactory()
{
    //m_LargeIconConn = wxIcon(rdb_conn_48_xpm);
    //m_SmallIconConn = wxIcon(rdb_conn_16_xpm);
    //m_LargeIconDisconn = wxIcon(rdb_disconn_48_xpm);
    //m_SmallIconDisconn = wxIcon(rdb_disconn_16_xpm);
}

wxGxWebConnectionFactoryUI::~wxGxWebConnectionFactoryUI(void)
{
}

wxGxObject* wxGxWebConnectionFactoryUI::GetGxObject(wxGxObject* pParent, const wxString &soName, const CPLString &szPath)
{
	//wxGxRemoteConnectionUI* pDataset = new wxGxRemoteConnectionUI(pParent, soName, szPath, m_LargeIconConn, m_SmallIconConn, m_LargeIconDisconn, m_SmallIconDisconn);
	//return static_cast<wxGxObject*>(pDataset);
    return NULL;
}