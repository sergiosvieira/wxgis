/******************************************************************************
 * Project:  wxGIS (GIS Catalog)
 * Purpose:  wxGxDBConnectionsUI class.
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

#include "wxgis/catalogui/gxdbconnectionsui.h"

#include "../../art/db_connections_16.xpm"
#include "../../art/db_connections_48.xpm"

/////////////////////////////////////////////////////////////////////////
// wxGxSpatialReferencesFolder
/////////////////////////////////////////////////////////////////////////
IMPLEMENT_DYNAMIC_CLASS(wxGxDBConnectionsUI, wxGxDBConnections)

wxGxDBConnectionsUI::wxGxDBConnectionsUI(void) : wxGxDBConnections()
{
    m_LargeIcon = wxIcon(db_connections_48_xpm);
    m_SmallIcon = wxIcon(db_connections_16_xpm);
}

wxGxDBConnectionsUI::~wxGxDBConnectionsUI(void)
{
}

wxIcon wxGxDBConnectionsUI::GetLargeImage(void)
{
	return m_LargeIcon;
}

wxIcon wxGxDBConnectionsUI::GetSmallImage(void)
{
	return m_SmallIcon;
}


