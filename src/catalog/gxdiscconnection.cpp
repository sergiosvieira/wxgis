/******************************************************************************
 * Project:  wxGIS (GIS Catalog)
 * Purpose:  wxGxDiscConnection class.
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
#include "wxgis/catalog/gxdiscconnection.h"
#include "../../art/folder_conn_16.xpm"
#include "../../art/folder_conn_48.xpm"
#include "../../art/folder_conn_disbl_16.xpm"
#include "../../art/folder_conn_disbl_48.xpm"

#include <wx/filename.h>

wxGxDiscConnection::wxGxDiscConnection(wxString Path, wxString Name) : wxGxFolder(Path, Name)
{
}

wxGxDiscConnection::~wxGxDiscConnection(void)
{
}

wxIcon wxGxDiscConnection::GetLargeImage(void)
{
	bool bIsOk = wxFileName::IsDirReadable(m_sPath);
	if(bIsOk)
		return wxIcon(folder_conn_48_xpm);
	else
		return wxIcon(folder_conn_disbl_48_xpm);
}

wxIcon wxGxDiscConnection::GetSmallImage(void)
{
	bool bIsOk = wxFileName::IsDirReadable(m_sPath);
	if(bIsOk)
		return wxIcon(folder_conn_16_xpm);
	else
		return wxIcon(folder_conn_disbl_16_xpm);
}

bool wxGxDiscConnection::Delete(void)
{
	return true;
}

bool wxGxDiscConnection::Rename(wxString NewName)
{
	m_sName = NewName; 
	return true;
}

void wxGxDiscConnection::EditProperties(wxWindow *parent)
{
}
