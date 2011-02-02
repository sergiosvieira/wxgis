/******************************************************************************
 * Project:  wxGIS (GIS Catalog)
 * Purpose:  wxGxDiscConnectionUI class.
 * Author:   Bishop (aka Barishnikov Dmitriy), polimax@mail.ru
 ******************************************************************************
*   Copyright (C) 2010 Bishop
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
#include "wxgis/catalogui/gxdiscconnectionui.h"

wxGxDiscConnectionUI::wxGxDiscConnectionUI(CPLString Path, wxString Name, wxIcon SmallIco, wxIcon LargeIco, wxIcon SmallIcoDsbl, wxIcon LargeIcoDsbl) : wxGxDiscConnection(Path, Name)
{
	m_Conn16 = SmallIco;
	m_Conn48 = LargeIco;
	m_ConnDsbld16 = SmallIcoDsbl;
	m_ConnDsbld48 = LargeIcoDsbl;
}

wxGxDiscConnectionUI::~wxGxDiscConnectionUI(void)
{
}

wxIcon wxGxDiscConnectionUI::GetLargeImage(void)
{
    bool bIsOk = wxIsReadable(wxString(m_sPath, wxConvUTF8));
	if(bIsOk)
		return m_Conn48;
	else
		return m_ConnDsbld48;
}

wxIcon wxGxDiscConnectionUI::GetSmallImage(void)
{
    bool bIsOk = wxIsReadable(wxString(m_sPath, wxConvUTF8));
	if(bIsOk)
		return m_Conn16;
	else
		return m_ConnDsbld16;
}

void wxGxDiscConnectionUI::EditProperties(wxWindow *parent)
{
}