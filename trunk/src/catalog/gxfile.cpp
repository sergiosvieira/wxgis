/******************************************************************************
 * Project:  wxGIS (GIS Catalog)
 * Purpose:  wxGxFile classes.
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
#include "wxgis/catalog/gxfile.h"
#include "../../art/sr_16(2).xpm"
#include "../../art/sr_48.xpm"

//--------------------------------------------------------------
//class wxGxFile
//--------------------------------------------------------------

wxGxFile::wxGxFile(wxString Path, wxString Name)
{
	m_sName = Name;
	m_sPath = Path;
}

wxGxFile::~wxGxFile(void)
{
}


//--------------------------------------------------------------
//class wxGxPrjFile
//--------------------------------------------------------------

wxGxPrjFile::wxGxPrjFile(wxString Path, wxString Name, wxGISEnumPrjFileType Type) : wxGxFile(Path, Name)
{
    m_Type = Type;
}

wxGxPrjFile::~wxGxPrjFile(void)
{
}

wxIcon wxGxPrjFile::GetLargeImage(void)
{
	return wxIcon(sr_48_xpm);
}

wxIcon wxGxPrjFile::GetSmallImage(void)
{
	return wxIcon(sr_16_2_xpm);
}

